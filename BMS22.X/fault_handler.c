/*
 * contains functions for counting faults and reporting them
 */
////////////////includes///////////////////////////////////////////////////////
#include "fault_handler.h"
#include "LTC/LTC_utilities.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr2.h"
#include "cell_balancing.h"
#include <stdint.h>
////////////////defines////////////////////////////////////////////////////////
#define OUTOFRANGE_VOLTAGE_MAX_FAULTS           30 //TODO make this 10 (50 ms measurement period, 500 ms fault period)
#define MISSING_VOLTAGE_MEASUREMENT_MAX_FAULTS  30
#define OPEN_SENSE_LINE_MAX_FAULTS              30
#define OUTOFRANGE_TEMPERATURE_MAX_FAULTS       30
#define MISSING_TEMP_MEASUREMENT_FAULTS_MAX     30
#define SELF_TEST_FAULTS_MAX                    30

////////////////globals////////////////////////////////////////////////////////
uint8_t outofrange_voltage_fault[NUM_CELLS];
uint8_t missing_voltage_measurement_fault[NUM_ICS*6];
uint8_t outofrange_temperature_fault[9*NUM_ICS];
uint8_t missing_temperature_measurement_fault[NUM_ICS*4];
uint8_t sense_line_fault[NUM_CELLS];
uint8_t self_test_fault[NUM_ICS];

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
    
    for(i = 0; i < NUM_ICS; ++i)
    {
        self_test_fault[i] = 0;
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
    
    for(i = 0; i < 5; ++i)// TODO check all cell voltages NUM_CELLS; ++i)
    {
        if(outofrange_voltage_fault[i] > OUTOFRANGE_VOLTAGE_MAX_FAULTS)
        {
            shutdown_car();
            set_voltage_fault_bit();
        }
   
        if(sense_line_fault[i] > OPEN_SENSE_LINE_MAX_FAULTS)
        {
            shutdown_car();
            //TODO SET_SENSE_LINE_FAULT_BIT(1);
        }
    }
    
    for(i = 0; i < 2; ++i) // TODO check all registers for missing voltage measurement NUM_ICS * 6; ++i)
    {
        if(missing_voltage_measurement_fault[i] > MISSING_VOLTAGE_MEASUREMENT_MAX_FAULTS)
        {
            shutdown_car();
            set_voltage_fault_bit();
        }
    }
    
    for(i = 0; i < 2; ++i) // TODO track faults in all temp sensors 9*NUM_ICS; ++i)
    {
        if(outofrange_temperature_fault[i] > OUTOFRANGE_TEMPERATURE_MAX_FAULTS)
        {
            shutdown_car();
            set_temperature_fault_bit();
        }
    }
    
    for(i = 0; i < 1; ++i) // TODO track temp faults for all regs for all ICs 4*NUM_ICS; ++i)
    {
        if(missing_temperature_measurement_fault[i] > MISSING_TEMP_MEASUREMENT_FAULTS_MAX)
        {
            shutdown_car();
            set_temperature_fault_bit();
        }
    }
    
    for(i = 0; i < NUM_ICS; ++i)
    {
        if(self_test_fault[i]> SELF_TEST_FAULTS_MAX)
        {
            shutdown_car();
            //TODO set_self_test_fault_bit();)
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
    disable_cell_balancing();
    
    // open shutdown loop
    BMS_RELAY_EN_SetLow();
}