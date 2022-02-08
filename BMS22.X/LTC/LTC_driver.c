
/* 
 * File:   LTC_driver.c
 * Author: Elena
 *
 * Created on January 28, 2022, 5:35 AM
 */

#include "LTC_driver.h"
#include "LTC_utilities.h"
#include "LTC_cmds/LTC_cmds.h"
#include <stdint.h>
#define FCY 40000000UL // Instruction cycle frequency, Hz - required for __delayXXX() to work
#include <libpic30.h>        // __delayXXX() functions macros defined here

uint8_t cell_voltage_check();

void LTC_initialize()
{
    init_PEC15_Table();
}

uint8_t read_cell_voltages(uint16_t* cell_voltages)
{
    start_cell_voltage_adc_conversion();
    poll_adc_status();
    __delay_ms(10); //TODO: is this delay necessary?
    rdcv_register(ADCVA, &cell_voltages[0]);
    rdcv_register(ADCVB, &cell_voltages[3]);
    rdcv_register(ADCVC, &cell_voltages[6]);
    rdcv_register(ADCVD, &cell_voltages[9]);
    rdcv_register(ADCVE, &cell_voltages[12]);
    rdcv_register(ADCVF, &cell_voltages[15]);
    
    return cell_voltage_check(cell_voltages);
}

uint8_t cell_voltage_check(uint16_t* cell_voltages) //TODO: implement timeout, or consecutive count of out-of-range samples
{
    uint8_t i = 0;
    for(i = 0; i < 18; ++i)
    {
        if((cell_voltages[i] > CELL_VOLTAGE_MAX) | (cell_voltages[i] < CELL_VOLTAGE_MIN))
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}