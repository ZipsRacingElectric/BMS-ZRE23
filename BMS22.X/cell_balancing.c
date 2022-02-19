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

uint8_t turn_off_balance_switches = 0;
bool cell_needs_balanced[NUM_CELLS]; 

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
        if(cell_voltages[i] < minimum_voltage && cell_voltages[i] > 30000) //TODO can't use this 30000 check?
        {
            minimum_voltage = cell_voltages[i];
        }
    }
    
     for(i = 0; i < NUM_CELLS; ++i)
    {
        if(cell_voltages[i] > (minimum_voltage + CELL_BALANCE_THRESHOLD))
        {
            cell_needs_balanced[i] = true;
        }
        else
        {
            cell_needs_balanced[i] = false;
        }
    }
}

////////////////interrupts/////////////////////////////////////////////////////
void timer2_interrupt(void)
{
    if(turn_off_balance_switches == 1)
    {
        turn_off_balance_switches = 0;
        turn_off_all_balancing();
    }
    else
    {
        //TODO balance based on cell voltage
        
        turn_off_balance_switches = 1;
        uint8_t i = 0;
        uint8_t data_to_write[6*NUM_ICS] = {0xE4, 0x52, 0x27, 0xA0, 0x00, 0x50};
        for(i = 0; i < NUM_CELLS; ++i)
        {
            if(cell_needs_balanced[i])
            {                
                switch(i)
                {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        data_to_write[4] |= (1 << i);
                        break;
                    default:
                        break;   
                }
            }
        }
        wrcfga(data_to_write);
        //TODO add cfgrb
        //TODO make this work for more cells, for multiple ICs
    }
}