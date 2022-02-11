/*
 * contains functions for counting faults and reporting them
 */
////////////////includes///////////////////////////////////////////////////////
#include "fault_handler.h"
#include "LTC/LTC_utilities.h"
#include <stdint.h>
////////////////defines////////////////////////////////////////////////////////

////////////////globals////////////////////////////////////////////////////////
uint8_t cell_voltage_faults[NUM_CELLS];
uint8_t temp_faults[9*NUM_ICS];
uint8_t sense_line_faults[NUM_CELLS];
uint8_t self_test_faults[NUM_ICS];

////////////////prototypes/////////////////////////////////////////////////////

////////////////functions//////////////////////////////////////////////////////

void fault_handler_initialize(void)
{
    uint8_t i = 0;
    
    for(i = 0; i < NUM_CELLS; ++i)
    {
        cell_voltage_faults[i] = 0;
        sense_line_faults[i] = 0;
    }
    
    for(i = 0; i < 9*NUM_ICS; ++i)
    {
        temp_faults[i] = 0;
    }
    
    for(i = 0; i < NUM_ICS; ++i)
    {
        self_test_faults[i] = 0;
    }
}

void increment_cell_voltage_fault(uint8_t cell_id)
{
    ++cell_voltage_faults[cell_id - 1]; // -1 since array index starts at 0
}

void reset_cell_voltage_fault(uint8_t cell_id)
{
    cell_voltage_faults[cell_id - 1] = 0;
}