
/* 
 * File:   LTC_driver.c
 * Author: Elena
 *
 * Created on January 28, 2022, 5:35 AM
 */

#include "LTC_driver.h"
#include "LTC_utilities.h"
#include "LTC_cmds/LTC_cmds.h"
#include "../fault_handler.h"
#include <stdint.h>
#define FCY 40000000UL // Instruction cycle frequency, Hz - required for __delayXXX() to work
#include <libpic30.h>        // __delayXXX() functions macros defined here

static uint8_t cell_voltage_check(uint16_t* cell_voltages);
static uint8_t pack_temperature_check(uint16_t* pack_temperatures);

// initialize PEC table necessary for LTC68xx interface
void LTC_initialize()
{
    init_PEC15_Table();
}

// send commands to get cell voltages
uint8_t read_cell_voltages(uint16_t* cell_voltages)
{
    start_cell_voltage_adc_conversion();
    poll_adc_status();
    __delay_ms(10); //TODO: is this delay necessary?
    rdcv_register(ADCVA, &cell_voltages[0]);
    rdcv_register(ADCVB, &cell_voltages[3*NUM_ICS]);
    rdcv_register(ADCVC, &cell_voltages[6*NUM_ICS]);
    rdcv_register(ADCVD, &cell_voltages[9*NUM_ICS]);
    rdcv_register(ADCVE, &cell_voltages[12*NUM_ICS]);
    rdcv_register(ADCVF, &cell_voltages[15*NUM_ICS]);
    
    return cell_voltage_check(cell_voltages);
}

// send commands to get pack temperatures
uint8_t read_temperatures(uint16_t* pack_temperatures)
{
    start_temperature_adc_conversion();
    poll_adc_status();
    __delay_ms(10); //TODO: is this delay necessary?
    
    // store aux register values in intermediate array since not all data
    // is temperature sensor data. See LTC6813 datasheet pg 62
    uint16_t aux_reg[12*NUM_ICS];
    rdaux_register(AUXA, &aux_reg[0*NUM_ICS]);
    rdaux_register(AUXB, &aux_reg[3*NUM_ICS]);
    rdaux_register(AUXC, &aux_reg[6*NUM_ICS]);
    rdaux_register(AUXD, &aux_reg[9*NUM_ICS]);
    // copy over temperature data to temperature array
    uint8_t i = 0;
    for(i = 0; i < NUM_ICS; ++i)
    {
        pack_temperatures[i*NUM_ICS] = aux_reg[3*i];
        pack_temperatures[i*NUM_ICS + 1] = aux_reg[(3*i) + 1];
        pack_temperatures[i*NUM_ICS + 2] = aux_reg[(3*i) + 2];
        pack_temperatures[i*NUM_ICS + 3] = aux_reg[(3*i) + (3*NUM_ICS)];
        pack_temperatures[i*NUM_ICS + 4] = aux_reg[(3*i) + (3*NUM_ICS) + 1];
        pack_temperatures[i*NUM_ICS + 5] = aux_reg[(3*i) + (6*NUM_ICS)];
        pack_temperatures[i*NUM_ICS + 6] = aux_reg[(3*i) + (6*NUM_ICS) + 1];
        pack_temperatures[i*NUM_ICS + 7] = aux_reg[(3*i) + (6*NUM_ICS) + 2];
        pack_temperatures[i*NUM_ICS + 8] = aux_reg[(3*i) + (9*NUM_ICS)];
    }
    
    return pack_temperature_check(pack_temperatures);
}

// check whether sense line overcurrent protection has tripped
void open_sense_line_check(uint32_t* sense_line_status)
{
    //see pg 32 of 6813 datasheet for info
    open_wire_check(1); // param: pull dir 0 for down 1 for up
    __delay_us(10);
    open_wire_check(1); // param: pull dir 0 for down 1 for up
    __delay_us(10);
    open_wire_check(1); // param: pull dir 0 for down 1 for up
    __delay_us(10);
    open_wire_check(1); // param: pull dir 0 for down 1 for up
    uint16_t cell_pu[NUM_CELLS]; //TODO make sure valid PEC is received when getting voltage reg values
    rdcv_register(ADCVA, &cell_pu[0]);
    rdcv_register(ADCVB, &cell_pu[3]);
    rdcv_register(ADCVC, &cell_pu[6]);
    rdcv_register(ADCVD, &cell_pu[9]);
    rdcv_register(ADCVE, &cell_pu[12]);
    rdcv_register(ADCVF, &cell_pu[15]);
    open_wire_check(0); // param: pull dir 0 for down 1 for up
    __delay_us(10);
    open_wire_check(0); // param: pull dir 0 for down 1 for up
    __delay_us(10);
    open_wire_check(0); // param: pull dir 0 for down 1 for up
    __delay_us(10);
    open_wire_check(0); // param: pull dir 0 for down 1 for up
    uint16_t cell_pd[NUM_CELLS];
    rdcv_register(ADCVA, &cell_pd[0]); //TODO make sure valid PEC is received when getting voltage reg values
    rdcv_register(ADCVB, &cell_pd[3]);
    rdcv_register(ADCVC, &cell_pd[6]);
    rdcv_register(ADCVD, &cell_pd[9]);
    rdcv_register(ADCVE, &cell_pd[12]);
    rdcv_register(ADCVF, &cell_pd[15]);
    
    //TODO: finish this
    uint8_t i = 0;
    for(i = 0; i < NUM_CELLS; ++i) // for each ic - 0-5
    {
        if(i % 18 == 0) // sense line 0 in a segment
        {
            if(cell_pu[i] == 0) // TODO: exactly equal to zero or just close to zero?
            {
                sense_line_status[i / 18] |= (1 << i);
                increment_sense_line_fault(i);
            }
            else
            {
                sense_line_status[i / 18] &= (uint32_t)(~(1 << i));
                reset_sense_line_fault(i);
            }
        }
        else if(i % 18 == 17) // cell 18 in a segment
        {
            if(cell_pd[i] == 0) // TODO: exactly equal to zero or just close to zero?
            {
                sense_line_status[i / 18] |= (1 << i);
                increment_sense_line_fault(i);
            }
            else
            {
                sense_line_status[i / 18] &= (uint32_t)(~(1 << i));
                reset_sense_line_fault(i);
            }
        }
        else // cells 2-17 in a segment
        {
            int16_t delta = cell_pu[i+1] - cell_pd[i+1]; // V * 10000
            if(delta < -4000) //TODO magic number
            {
                sense_line_status[i / 18] |= (1 << i);
                increment_sense_line_fault(i);
            }
            else
            {
                sense_line_status[i / 18] &= (uint32_t)(~(1 << i));
                reset_sense_line_fault(i);
            }
        }
    }
    
}

static uint8_t cell_voltage_check(uint16_t* cell_voltages) //TODO: implement timeout, or consecutive count of out-of-range samples
{
    uint8_t i = 0;
    uint8_t ret_val = SUCCESS;
    for(i = 0; i < NUM_CELLS; ++i)
    {
        if((cell_voltages[i] > CELL_VOLTAGE_MAX) | (cell_voltages[i] < CELL_VOLTAGE_MIN))
        {
            increment_cell_voltage_fault(i);
            ret_val = FAILURE;
        }
        else
        {
            reset_cell_voltage_fault(i);
        }
    }
    return ret_val;
}

static uint8_t pack_temperature_check(uint16_t* pack_temperatures)
{
    uint8_t i = 0;
    uint8_t ret_val = SUCCESS;
    for(i = 0; i < NUM_TEMP_SENSORS; ++i)
    {
        if((pack_temperatures[i] > CELL_TEMPERATURE_MAX) | (pack_temperatures[i] < CELL_TEMPERATURE_MIN))
        {
            increment_temperature_fault(i);
            ret_val = FAILURE;
        }
        else
        {
            reset_temperature_fault(i);
        }
    }
    return ret_val;
}