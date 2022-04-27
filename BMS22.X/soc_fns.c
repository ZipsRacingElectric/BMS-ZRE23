/*
 * contains functions for calculating state of charge, including:
 * timer interrupt which triggers ADC conversion
 * ADC interrupt to collect conversion result
 * function to update SoC value with latest ADC data
 */

////////////////includes///////////////////////////////////////////////////////
#include "soc_fns.h"
#include "mcc_generated_files/adc1.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/can_types.h"
#include "mcc_generated_files/pin_manager.h"
#include "eeprom.h"

////////////////defines////////////////////////////////////////////////////////
#define TOTAL_CHARGE_AH                 2.5
#define ADC_REF_VOLT                    3.3
#define ADC_MAX_BITS                    4095
#define FIVE_THREE_V_DIV                9.1 / (4.7 + 9.1)
#define G_V_PER_A_HIGH                  0.004
#define G_V_PER_A_LOW                   0.0267
#define SAMPLE_TIME_S_X_THOU            4
#define OFFSET_VOLTAGE                  2.5065 //TODO is this correct? This seems to be what the ADC is reading on the CS Low pin when no current is flowing

const float CS_SAMPLE_HI_COEFF =        ADC_REF_VOLT * SAMPLE_TIME_S_X_THOU / (ADC_MAX_BITS * FIVE_THREE_V_DIV * G_V_PER_A_HIGH * 60 * 60);
const float OFFSET_HI_COEFF =           SAMPLE_TIME_S_X_THOU / (G_V_PER_A_HIGH * 60 * 60);
const float CS_SAMPLE_LO_COEFF =        ADC_REF_VOLT * SAMPLE_TIME_S_X_THOU / (ADC_MAX_BITS * FIVE_THREE_V_DIV * G_V_PER_A_LOW * 60 * 60);
const float OFFSET_LO_COEFF =           SAMPLE_TIME_S_X_THOU / (G_V_PER_A_LOW * 60 * 60);

#define USE_LO_THRESHOLD                3200 //~60 A in ADC bits as measured by CS LO channel

#define CS_HIGH_ADC_BITS_TO_AMPS(x)     ((x * ADC_REF_VOLT * 10 / (ADC_MAX_BITS * FIVE_THREE_V_DIV * G_V_PER_A_HIGH)) - OFFSET_VOLTAGE/G_V_PER_A_HIGH * 10)
#define CS_LOW_ADC_BITS_TO_AMPS(x)      ((x * ADC_REF_VOLT * 10 / (ADC_MAX_BITS * FIVE_THREE_V_DIV * G_V_PER_A_LOW)) - OFFSET_VOLTAGE/G_V_PER_A_LOW * 100)

////////////////globals////////////////////////////////////////////////////////
int16_t cs_lo_to_transmit = 0;
int16_t cs_hi_to_transmit = 0;
volatile uint16_t cs_low_sample = 0;
volatile uint16_t cs_high_sample = 0;
float state_of_charge_float = 1000;
volatile uint32_t sum_cs_lo_samples = 0;
volatile uint32_t sum_cs_hi_samples = 0;
volatile uint16_t sample_lo_count = 0;
volatile uint16_t sample_hi_count = 0;
uint8_t eeprom_write_counter = 0;

////////////////interrupt prototypes///////////////////////////////////////////
void timer1_interrupt(void);
void adc1_cs_lo_interrupt(uint16_t valCS_LO);

////////////////functions//////////////////////////////////////////////////////
// initialize peripherals necessary for SoC calculation
void soc_initialize(void)
{
    TMR1_SetInterruptHandler(timer1_interrupt); //my function to handle timer1 interrupts
    ADC1_SetCS_LOInterruptHandler(adc1_cs_lo_interrupt); //my function to handle ADC interrupts

    uint16_t state_of_charge_int = get_state_of_charge_from_eeprom();
    state_of_charge_float = (float)state_of_charge_int;

    ADC1_Enable();
    TMR1_Start();
}

// calculate SoC based on most recently collected current data
void calc_soc(void)
{
    //contribution from low channel + contribution from high channel
    float spent_Ah_xten = CS_SAMPLE_LO_COEFF * sum_cs_lo_samples - OFFSET_LO_COEFF * OFFSET_VOLTAGE * sample_lo_count;
    spent_Ah_xten += CS_SAMPLE_HI_COEFF * sum_cs_hi_samples - OFFSET_HI_COEFF * OFFSET_VOLTAGE * sample_hi_count;
    //reset running counters since we've handled this data
    sum_cs_lo_samples = 0;
    sample_lo_count = 0;
    sum_cs_hi_samples = 0;
    sample_hi_count = 0;

    state_of_charge_float = state_of_charge_float - spent_Ah_xten / TOTAL_CHARGE_AH;
    //can't go higher than 100% SoC or lower than 0%
    if(state_of_charge_float > 1000)
    {
        state_of_charge_float = 1000;
    }
    else if(state_of_charge_float < 0)
    {
        state_of_charge_float = 0;
    }

    cs_lo_to_transmit = (int16_t)CS_LOW_ADC_BITS_TO_AMPS(ADCBUF2);
    
    /* TODO: why is cs_hi val always junk? 
     * do I need/want the following line? cs_high_sample is often junk value 
     * since we only retrieve the high sample val if current is too big to be 
     * measured using the low channel
     */
    cs_hi_to_transmit = (int16_t)CS_HIGH_ADC_BITS_TO_AMPS(ADCBUF3);
    
    ++eeprom_write_counter;
    if(eeprom_write_counter > 20)
    {
        eeprom_write_counter = 0;
        LED5_EEPROM_Toggle();
        uint16_t soc_int = (uint16_t)state_of_charge_float;
        write_eeprom(soc_int);
    }
}

///////////////set/getters/////////////////////////////////////////////////////
uint16_t get_soc_xten(void)
{
    return (uint16_t)state_of_charge_float;
}

int16_t get_cs_lo_xhundred(void)
{
    return cs_lo_to_transmit;
}

int16_t get_cs_hi_xten(void)
{
    return cs_hi_to_transmit;
}

////////////////interrupts/////////////////////////////////////////////////////
//4 ms timer
void timer1_interrupt(void)
{
    LED3_TMR1_SetHigh();
    
    //start ADC sampling for both channels
    ADC1_SoftwareTriggerEnable();
    
    LED3_TMR1_SetLow();
}

//interrupt is triggered when sampling is complete
void adc1_cs_lo_interrupt(uint16_t valCS_LO)
{
    LED4_ADC_SetHigh();
    
    // if current low enough to be measured by low channel
    if(valCS_LO < USE_LO_THRESHOLD)
    {
        //keep tally of ADC samples
        sum_cs_lo_samples = sum_cs_lo_samples + valCS_LO;
        cs_low_sample = valCS_LO;
        sample_lo_count = sample_lo_count + 1;
    }
    else //else need to use high channel
    {
        uint16_t valCS_HI = ADC1_ConversionResultGet(CS_HI);
        sum_cs_hi_samples = sum_cs_hi_samples + valCS_HI;
        cs_high_sample = valCS_HI;
        sample_hi_count = sample_hi_count + 1;
    }

    LED4_ADC_SetLow();
}