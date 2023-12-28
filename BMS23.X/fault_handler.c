/*
 * contains functions for counting faults and reporting them
 */
////////////////includes///////////////////////////////////////////////////////
#include "fault_handler.h"
#include "LTC/LTC_utilities.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr2.h"
#include "cell_balancing.h"
#include "global_constants.h"
#include <stdint.h>
////////////////defines////////////////////////////////////////////////////////
#define OUTOFRANGE_VOLTAGE_MAX_FAULTS           30 //TODO make this 10 (50 ms measurement period, 500 ms fault period)
#define MISSING_VOLTAGE_MEASUREMENT_MAX_FAULTS  30
#define OPEN_SENSE_LINE_MAX_FAULTS              30
#define OUTOFRANGE_TEMPERATURE_MAX_FAULTS       10
#define MISSING_TEMP_MEASUREMENT_FAULTS_MAX     30
#define SELF_TEST_FAULTS_MAX                    30

////////////////globals////////////////////////////////////////////////////////
uint8_t outofrange_voltage_fault[NUM_CELLS];
uint8_t missing_voltage_measurement_fault[NUM_ICS * CV_REGISTERS_PER_IC];
uint8_t outofrange_temperature_fault[NUM_TEMP_SENSORS];
uint8_t missing_temperature_measurement_fault[NUM_ICS * AUX_REGISTERS_PER_IC];
uint8_t sense_line_fault[NUM_CELLS];
uint8_t cv_self_test_fault[NUM_ICS * CV_REGISTERS_PER_IC];
uint8_t aux_self_test_fault[NUM_ICS * AUX_REGISTERS_PER_IC];
uint8_t mux_self_test_fault[NUM_ICS];

uint8_t fault_codes = 0;

//////////////// prototypes ///////////////////////////////////////////////////
static void set_voltage_fault_bit(void);
static void set_temperature_fault_bit(void);
static void set_sense_line_fault_bit(void);
static void set_self_test_fault_bit(void);
static void shutdown_car(void);

//////////////// public functions /////////////////////////////////////////////

// initialize fault tracking arrays, enable BMS shutdown loop relay
void fault_handler_initialize(void) //TODO make sure all these for loop ranges are correct
{
    BMS_RELAY_EN_SetLow();
    LED8_SetHigh();
    __delay_ms(500);
    LED8_SetLow();
    
    uint8_t i = 0;
    
    for(i = 0; i < NUM_CELLS; ++i)
    {
        outofrange_voltage_fault[i] = 0;
        sense_line_fault[i] = 0;
    }
    
    for(i = 0; i < NUM_ICS * 6; ++i)
    {
        missing_voltage_measurement_fault[i] = 0;
    }
    
    for(i = 0; i < NUM_TEMP_SENSORS; ++i)
    {
        outofrange_temperature_fault[i] = 0;
    }
    
    for(i = 0; i < 4*NUM_ICS; ++i)
    {
        missing_temperature_measurement_fault[i] = 0;
    }
    
    for(i = 0; i < NUM_ICS * CV_REGISTERS_PER_IC; ++i)
    {
        cv_self_test_fault[i] = 0;
    }
    
    for(i = 0; i < NUM_ICS * AUX_REGISTERS_PER_IC; ++i)
    {
        aux_self_test_fault[i] = 0;
    }
    
    for(i = 0; i < NUM_ICS; ++i)
    {
        mux_self_test_fault[i] = 0;
    }

    BMS_RELAY_EN_SetHigh();
}

uint8_t get_fault_codes(void)
{
    return fault_codes;
}

// iterate through fault arrays to determine whether actionable fault has occurred
void check_for_fault(void)
{
    uint8_t i = 0;
    
    for(i = 0; i < NUM_CELLS; ++i)
    {
        if(outofrange_voltage_fault[i] > OUTOFRANGE_VOLTAGE_MAX_FAULTS)
        {
            shutdown_car();
            set_voltage_fault_bit();
        }
   
        if(sense_line_fault[i] > OPEN_SENSE_LINE_MAX_FAULTS)
        {
            shutdown_car();
            set_sense_line_fault_bit();
        }
    }
    
    for(i = 0; i < NUM_ICS * CV_REGISTERS_PER_IC; ++i)
    {
        if(missing_voltage_measurement_fault[i] > MISSING_VOLTAGE_MEASUREMENT_MAX_FAULTS)
        {
            shutdown_car();
            set_voltage_fault_bit();
        }
    }
    
    for(i = 0; i < NUM_ICS * TEMP_SENSORS_PER_IC; ++i)
    {
        if(outofrange_temperature_fault[i] > OUTOFRANGE_TEMPERATURE_MAX_FAULTS)
        {
            shutdown_car();
            set_temperature_fault_bit();
        }
    }
    
    for(i = 0; i < NUM_ICS * AUX_REGISTERS_PER_IC; ++i)
    {
        if(missing_temperature_measurement_fault[i] > MISSING_TEMP_MEASUREMENT_FAULTS_MAX)
        {
            shutdown_car(); 
            set_temperature_fault_bit();
        }
    }
    
    for(i = 0; i < NUM_ICS * CV_REGISTERS_PER_IC; ++i)
    {
        if(cv_self_test_fault[i] > SELF_TEST_FAULTS_MAX)
        {
            shutdown_car();
            set_self_test_fault_bit();
        }
    }
    
    for(i = 0; i < NUM_ICS * AUX_REGISTERS_PER_IC; ++i)
    {
        if(aux_self_test_fault[i] > SELF_TEST_FAULTS_MAX)
        {
            shutdown_car();
            set_self_test_fault_bit();
        }
    }

    for(i = 0; i < NUM_ICS; ++i)
    {
        if(mux_self_test_fault[i] > SELF_TEST_FAULTS_MAX)
        {
            shutdown_car();
            set_self_test_fault_bit();
        }
    }
}

void increment_outofrange_voltage_fault(uint8_t cell_id)
{
    outofrange_voltage_fault[cell_id] += 1;
}

void reset_outofrange_voltage_fault(uint8_t cell_id)
{
    outofrange_voltage_fault[cell_id] = 0;
}

void increment_missing_voltage_measurement_fault(uint8_t section_id)
{
    missing_voltage_measurement_fault[section_id] += 1;
}

void reset_missing_voltage_measurement_fault(uint8_t section_id)
{
    missing_voltage_measurement_fault[section_id] = 0;
}

uint8_t get_missing_voltage_measurement_fault(uint8_t section_id)
{
    return missing_voltage_measurement_fault[section_id];
}

void increment_outofrange_temperature_fault(uint8_t temp_sensor_id)
{
    outofrange_temperature_fault[temp_sensor_id] += 1;
}

void reset_outofrange_temperature_fault(uint8_t temp_sensor_id)
{
    outofrange_temperature_fault[temp_sensor_id] = 0;
}

void increment_missing_temperature_fault(uint8_t section_id)
{
    missing_temperature_measurement_fault[section_id] += 1;
}

void reset_missing_temperature_fault(uint8_t section_id)
{
    missing_temperature_measurement_fault[section_id] = 0;
}

uint8_t get_missing_temperature_fault(uint8_t section_id)
{
    return missing_temperature_measurement_fault[section_id];
}

void increment_sense_line_fault(uint8_t cell_id)
{
    sense_line_fault[cell_id] += 1;
}

void reset_sense_line_fault(uint8_t cell_id)
{
    sense_line_fault[cell_id] = 0;
}

void increment_cv_self_test_fault(uint8_t section_id)
{
    cv_self_test_fault[section_id] += 1;
}

void reset_cv_self_test_fault(uint8_t section_id)
{
    cv_self_test_fault[section_id] = 0;
}

void increment_aux_self_test_fault(uint8_t section_id)
{
    aux_self_test_fault[section_id] += 1;
}

void reset_aux_self_test_fault(uint8_t section_id)
{
    aux_self_test_fault[section_id] = 0;
}

void increment_mux_self_test_fault(uint8_t chip_id)
{
    mux_self_test_fault[chip_id] += 1;
}

void reset_mux_self_test_fault(uint8_t chip_id)
{
    mux_self_test_fault[chip_id] = 0;
}

//////////////// private functions ////////////////////////////////////////////

static void set_voltage_fault_bit(void)
{
    fault_codes |= (1 << 1);
}

static void set_temperature_fault_bit(void)
{
    fault_codes |= (1 << 0);
}

static void set_sense_line_fault_bit(void)
{
    fault_codes |= (1 << 4);
}

static void set_self_test_fault_bit(void)
{
    fault_codes |= (1 << 2);
}

static void shutdown_car(void)
{
    // turn off all balancing
    disable_cell_balancing();
    // open shutdown loop
    BMS_RELAY_EN_SetLow();
}