
/* 
 * File:   LTC_driver.c
 * Author: Elena
 *
 * Created on January 28, 2022, 5:35 AM
 */

#include "LTC_driver.h"
#include "LTC_utilities.h"
#include "LTC_cmds/LTC_cmds.h"
#define FCY 40000000UL // Instruction cycle frequency, Hz - required for __delayXXX() to work
#include <libpic30.h>        // __delayXXX() functions macros defined here

uint8_t cell_voltage_check();

void LTC_initialize()
{
    init_PEC15_Table();
}

uint8_t read_cell_voltages()
{
    start_cell_voltage_adc_conversion();
    poll_adc_status();
    __delay_ms(10); //TODO: is this delay necessary?
    uint16_t cell_voltages[18];
    rdcv_register(ADCVA, &cell_voltages[0]);
    rdcv_register(ADCVB, &cell_voltages[3]);
    rdcv_register(ADCVC, &cell_voltages[6]);
    rdcv_register(ADCVD, &cell_voltages[9]);
    rdcv_register(ADCVE, &cell_voltages[12]);
    rdcv_register(ADCVF, &cell_voltages[15]);
    
    return cell_voltage_check();
}

uint8_t cell_voltage_check()
{
    return SUCCESS;
}