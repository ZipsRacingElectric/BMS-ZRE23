/*
 * contains functions for counting faults and reporting them
 */
////////////////includes///////////////////////////////////////////////////////
#include "fault_handler.h"
#include "LTC/LTC_utilities.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr2.h"
#include <stdint.h>
////////////////defines////////////////////////////////////////////////////////
#define VOLTAGE_MAX_FAULTS                  20 //TODO make this 10 (50 ms measurement period, 500 ms fault period)
#define OPEN_SENSE_LINE_MAX_FAULTS          10
#define TEMP_FAULTS_MAX                     10
#define SELF_TEST_FAULTS_MAX                10

////////////////globals////////////////////////////////////////////////////////
uint8_t oor_voltage_faults[NUM_CELLS];
uint8_t missing_voltage_measurement_fault[NUM_ICS*6];
uint8_t temp_faults[9*NUM_ICS];
uint8_t sense_line_faults[NUM_CELLS];
uint8_t self_test_faults[NUM_ICS];

uint8_t fault_codes = 0;

//////////////// prototypes ///////////////////////////////////////////////////
static void set_voltage_fault_bit(void);
static void set_temperature_fault_bit(void);
static void shutdown_car(void);

//////////////// public functions /////////////////////////////////////////////

// initialize fault tracking arrays, enable BMS shutdown loop relay
void fault_handler_initialize(void)
{
    BMS_RELAY_EN_SetLow();
    
    uint8_t i = 0;
    
    for(i = 0; i < NUM_CELLS; ++i)
    {
        oor_voltage_faults[i] = 0;
        sense_line_faults[i] = 0;
    }
    
    for(i = 0; i < NUM_ICS * 6; ++i)
    {
        missing_voltage_measurement_fault[i] = 0;
    }
    
    for(i = 0; i < NUM_TEMP_SENSORS; ++i)
    {
        temp_faults[i] = 0;
    }
    
    for(i = 0; i < NUM_ICS; ++i)
    {
        self_test_faults[i] = 0;
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
        if(oor_voltage_faults[i] > OOR_VOLTAGE_MAX_FAULTS)
        {
            shutdown_car();
            set_voltage_fault_bit();
        }
   
        if(sense_line_faults[i] > OPEN_SENSE_LINE_MAX_FAULTS)
        {
            shutdown_car();
            //TODO SET_SENSE_LINE_FAULT_BIT(1);
        }
    }
    
    for(i = 0; i < NUM_ICS * 6; ++i)
    {
        if(missing_voltage_measurement_fault[i] > MISSING_VOLTAGE_MEASUREMENT_MAX_FAULTS)
        {
            shutdown_car();
            set_voltage_fault_bit();
        }
    }
    
    for(i = 0; i < 9*NUM_ICS; ++i)
    {
        if(temp_faults[i] > TEMP_FAULTS_MAX)
        {
            shutdown_car();
            set_temperature_fault_bit();
        }
    }
    
    for(i = 0; i < NUM_ICS; ++i)
    {
        if(self_test_faults[i]> SELF_TEST_FAULTS_MAX)
        {
            shutdown_car();
            //TODO set_self_test_fault_bit();)
        }
    }
}

void increment_outofrange_voltage_fault(uint8_t cell_id)
{
    oor_voltage_faults[cell_id] += 1;
}

void reset_outofrange_voltage_fault(uint8_t cell_id)
{
    oor_voltage_faults[cell_id] = 0;
}

void increment_missing_voltage_measurement_fault(uint8_t section_id)
{
    missing_voltage_measurement_fault[section_id] += 1;
}

void reset_missing_voltage_measurement_fault(uint8_t section_id)
{
    missing_voltage_measurement_fault[section_id] = 0;
}

void increment_outofrange_temperature_fault(uint8_t temp_sensor_id)
{
    temp_faults[temp_sensor_id] += 1;
}

void reset_outofrange_temperature_fault(uint8_t temp_sensor_id)
{
    temp_faults[temp_sensor_id] = 0;
}

// TODO cell voltage fault check
// possible faults: over voltage, under voltage, missing measurement

//////////////// private functions ////////////////////////////////////////////

static void set_voltage_fault_bit(void)
{
    fault_codes |= (1 << 1); //TODO magic numbers?
}

static void set_temperature_fault_bit(void)
{
    fault_codes |= (1 << 0); //TODO magic numbers?
}

static void shutdown_car(void)
{
    // turn off all balancing
    TMR2_Stop();
    uint8_t data_to_write[6*NUM_ICS] = {0xE4, 0x52, 0x27, 0xA0, 0x00, 0x50};
    write_config_A(data_to_write);
    
    // open shutdown loop
    BMS_RELAY_EN_SetLow();
}