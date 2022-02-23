
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
#include "../global_constants.h"

uint16_t aux_reg[12*NUM_ICS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //TODO initialize in for loop
    
static uint8_t cell_voltage_check(uint16_t* cell_voltages);
static uint8_t pack_temperature_check(uint16_t* pack_temperatures);

// initialize PEC table necessary for LTC68xx interface
void LTC_initialize()
{
    init_PEC15_Table();
}

// read configuration register A
uint8_t read_config_reg_a(uint8_t* buffer)
{
    return read_config_A(buffer);
}

// send commands to get cell voltages
uint8_t read_cell_voltages(uint16_t* cell_voltages, uint8_t* cell_voltage_invalid_counter)
{
    start_cell_voltage_adc_conversion();
    poll_adc_status();
    __delay_ms(10); //TODO: is this delay necessary?
    receive_voltage_register(ADCVA, &cell_voltages[0], &cell_voltage_invalid_counter[0]);
    receive_voltage_register(ADCVB, &cell_voltages[3], &cell_voltage_invalid_counter[3]);
    receive_voltage_register(ADCVC, &cell_voltages[6], &cell_voltage_invalid_counter[6]);
    receive_voltage_register(ADCVD, &cell_voltages[9], &cell_voltage_invalid_counter[9]);
    receive_voltage_register(ADCVE, &cell_voltages[12], &cell_voltage_invalid_counter[12]);
    receive_voltage_register(ADCVF, &cell_voltages[15], &cell_voltage_invalid_counter[15]);
    
    return cell_voltage_check(cell_voltages);
}

// send commands to get pack temperatures
uint8_t read_temperatures(uint16_t* pack_temperatures, uint8_t* aux_register_invalid_counter)
{
    start_temperature_adc_conversion();
    poll_adc_status();
    __delay_ms(10); //TODO: is this delay necessary?
    
    // store aux register values in intermediate array since not all data
    // is temperature sensor data. See LTC6813 datasheet pg 62

    receive_aux_register(AUXA, &aux_reg[0*NUM_ICS], &aux_register_invalid_counter[0]);
    receive_aux_register(AUXB, &aux_reg[3*NUM_ICS], &aux_register_invalid_counter[3]);
    receive_aux_register(AUXC, &aux_reg[6*NUM_ICS], &aux_register_invalid_counter[6]);
    receive_aux_register(AUXD, &aux_reg[9*NUM_ICS], &aux_register_invalid_counter[9]);
    // copy over temperature data to temperature array
    uint8_t i = 0;
    // TODO RHS indices seem wrong
    for(i = 0; i < NUM_ICS; ++i)
    {
        pack_temperatures[i*TEMP_SENSORS_PER_IC] = aux_reg[3*i];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 1] = aux_reg[(3*i) + 1];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 2] = aux_reg[(3*i) + 2];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 3] = aux_reg[(3*i) + (3*NUM_ICS)];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 4] = aux_reg[(3*i) + (3*NUM_ICS) + 1];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 5] = aux_reg[(3*i) + (6*NUM_ICS)];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 6] = aux_reg[(3*i) + (6*NUM_ICS) + 1];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 7] = aux_reg[(3*i) + (6*NUM_ICS) + 2];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 8] = aux_reg[(3*i) + (9*NUM_ICS)];
    }
    
    return pack_temperature_check(pack_temperatures);
}

// check whether sense line overcurrent protection has tripped
uint8_t open_sense_line_check(void)
{
//    //see pg 32 of 6813 datasheet for info
//    open_wire_check(1); // param: pull dir 0 for down 1 for up
//    open_wire_check(1); // param: pull dir 0 for down 1 for up
//    uint16_t cell_pu[8*NUM_ICS];
//    rdcv_register(ADCVA, &cell_pu[0]);
//    rdcv_register(ADCVB, &cell_pu[3]);
//    rdcv_register(ADCVC, &cell_pu[6]);
//    rdcv_register(ADCVD, &cell_pu[9]);
//    rdcv_register(ADCVE, &cell_pu[12]);
//    rdcv_register(ADCVF, &cell_pu[15]);
//    open_wire_check(0); // param: pull dir 0 for down 1 for up
//    open_wire_check(0); // param: pull dir 0 for down 1 for up
//    uint16_t cell_pd[8*NUM_ICS];
//    rdcv_register(ADCVA, &cell_pd[0]);
//    rdcv_register(ADCVB, &cell_pd[3]);
//    rdcv_register(ADCVC, &cell_pd[6]);
//    rdcv_register(ADCVD, &cell_pd[9]);
//    rdcv_register(ADCVE, &cell_pd[12]);
//    rdcv_register(ADCVF, &cell_pd[15]);
    
    //TODO: finish this
//    uint8_t i = 0;
//    for(i = 0; i < NUM_CELLS; ++i) // for each ic - 0-35
//    {
//        if()
//    }
//    // edge cases - C0 and C18
//    if()
    
}

static uint8_t cell_voltage_check(uint16_t* cell_voltages) //TODO: implement timeout, or consecutive count of out-of-range samples
{
    uint8_t i = 0;
    uint8_t ret_val = SUCCESS;
    for(i = 0; i < NUM_CELLS; ++i)
    {
        if((cell_voltages[i] > CELL_VOLTAGE_MAX) | (cell_voltages[i] < CELL_VOLTAGE_MIN))
        {
            increment_outofrange_voltage_fault(i);
            ret_val = FAILURE;
        }
        else
        {
            reset_outofrange_voltage_fault(i);
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
            increment_outofrange_temperature_fault(i);
            ret_val = FAILURE;
        }
        else
        {
            reset_outofrange_temperature_fault(i);
        }
    }
    return ret_val;
}