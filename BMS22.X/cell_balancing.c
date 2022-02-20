/*
 * cell_balancing.c
 *
 * contains functions for timer-based cell balancing
 */
#include "cell_balancing.h"
#include "LTC/LTC_cmds/LTC_cmds.h"
#include "LTC/LTC_driver.h"
#include "LTC/LTC_utilities.h"
#include "mcc_generated_files/tmr2.h"
#include <stdint.h>
#include <stdbool.h>
#define FCY 40000000UL // Instruction cycle frequency, Hz - required for __delayXXX() to work
#include <libpic30.h>        // __delayXXX() functions macros defined here

volatile uint8_t turn_off_balance_switches = 0;
uint32_t cell_needs_balanced[NUM_ICS]; 

////////////////interrupt prototypes///////////////////////////////////////////
void timer2_interrupt(void);

///////////////functions///////////////////////////////////////////////////////
// initialize peripherals necessary for cell balancing
void balance_timer_initialize(void)
{
    // TODO initialize cell needs balanced values to false
    TMR2_SetInterruptHandler(timer2_interrupt); //my function to handle timer2 interrupts   
    TMR2_Start();
}

void update_cell_balance_array(uint16_t* cell_voltages)
{
    // find minimum cell voltage
    uint8_t i = 0; 
    uint16_t minimum_voltage = 42000;
    for(i = 0; i < NUM_CELLS; ++i)
    {
        if(cell_voltages[i] < minimum_voltage && cell_voltages[i] > CELL_VOLTAGE_MIN) //TODO can't use this > check?
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

uint32_t* get_cell_balance_array(void)
{
    return cell_needs_balanced;
}

////////////////interrupts/////////////////////////////////////////////////////
void timer2_interrupt(void)
{
    if(turn_off_balance_switches == 0)
    {
        turn_off_balance_switches = 1;
        turn_off_all_balancing();
    }
    else
    {
        //TODO balance based on cell voltage
        
        turn_off_balance_switches += 1;
        uint8_t i = 0;
        uint8_t data_to_write[6*NUM_ICS] = {0xE4, 0x52, 0x27, 0xA0, 0x00, 0x50};

        // TODO wrap this in a num ics for loop
        for(i = 0; i < 8; ++i)
        {
            if(cell_needs_balanced[0] >> i == 1)
            {
                data_to_write[4] |= (1 << i);
            }
        } 

        wrcfga(data_to_write);
        //TODO add cfgrb
        //TODO make this work for more cells, for multiple ICs
        
        if(turn_off_balance_switches >= 5)
        {
            turn_off_balance_switches = 0;
        }
    }
}