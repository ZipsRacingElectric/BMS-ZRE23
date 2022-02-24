/*
 * cell_balancing.c
 *
 * contains functions for timer-based cell balancing
 */
#include "cell_balancing.h"
#include "LTC/LTC_utilities.h"
#include "mcc_generated_files/tmr2.h"
#include <stdint.h>
#include <stdbool.h>
#include "global_constants.h"

volatile uint8_t cell_balance_duty_cycle_counter = 0;
volatile uint8_t balancing_enabled = 1;
uint32_t cell_needs_balanced[NUM_ICS]; 

////////////////interrupt prototypes///////////////////////////////////////////
void write_balance_switches(void);

///////////////functions///////////////////////////////////////////////////////
// initialize peripherals necessary for cell balancing
void balance_timer_initialize(void)
{
    uint8_t i = 0;
    for(i = 0; i < NUM_ICS; ++i)
    {
        cell_needs_balanced[i] = 0;
    }
    TMR2_SetInterruptHandler(write_balance_switches); //my function to handle timer2 interrupts   
    TMR2_Start();
}

// param: cell voltages
// updates cell_needs_balanced array to reflect latest cell voltages
void update_cell_balance_array(uint16_t* cell_voltages)
{
    // find minimum cell voltage
    uint8_t i = 0; 
    uint16_t minimum_voltage = 42000;
    for(i = 0; i < NUM_CELLS; ++i)
    {
        if(cell_voltages[i] < minimum_voltage && cell_voltages[i] > CELL_VOLTAGE_MIN) //TODO change this to a check that the cell voltage is > 0 -> since 0 is used to indicate invalid PEC or missing packet
        {
            minimum_voltage = cell_voltages[i];
        }
    }
    
     for(i = 0; i < NUM_ICS; ++i)
    {
        uint8_t k = 0;
        for(k = 0; k < CELLS_PER_IC; ++k)
        {
            if(cell_voltages[k + i*CELLS_PER_IC] > (minimum_voltage + CELL_BALANCE_THRESHOLD))
            {
                cell_needs_balanced[i] |= (1 << k);
            }
            else
            {
                cell_needs_balanced[i] &= (uint32_t)(~(1 << k));
            }
        }
    }
}

// returns pointer to cell_needs_balanced array
// used to put cell balancing information on CAN bus
uint32_t* get_cell_balance_array(void)
{
    return cell_needs_balanced;
}

// disable all cell balancing
void disable_cell_balancing(void)
{
    balancing_enabled = 0;
}

////////////////interrupts/////////////////////////////////////////////////////
void write_balance_switches(void)
{
    if(cell_balance_duty_cycle_counter == 0 || balancing_enabled == 0)
    {
        cell_balance_duty_cycle_counter += 1;
        
        uint8_t i = 0;
        for(i = 0; i < NUM_ICS; ++i)
        {
            set_cfgra_dcc8_1(i, 0);
            set_cfgra_dcc12_9(i, 0);
            set_cfgrb_dcc16_13(i, 0);
            set_cfgrb_dcc18_17(i, 0);
        }
    }
    else
    {
        cell_balance_duty_cycle_counter += 1;
        
        uint8_t i = 0;
        for(i = 0; i < NUM_ICS; ++i)
        {
            set_cfgra_dcc8_1(i, cell_needs_balanced[i] & 0xFF);
            set_cfgra_dcc12_9(i, (cell_needs_balanced[i] >> 8) & 0xF);
            set_cfgrb_dcc16_13(i, (cell_needs_balanced[i] >> 12) & 0xF);
            set_cfgrb_dcc18_17(i, (cell_needs_balanced[i] >> 16) & 0x3);
            
        }
        
        if(cell_balance_duty_cycle_counter >= 5)
        {
            cell_balance_duty_cycle_counter = 0;
        }
    }
}