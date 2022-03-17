
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

//TODO come up with better way to verify that valid PEC is received from commands'

uint16_t aux_reg[12*NUM_ICS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //TODO initialize in for loop
    
static uint8_t cell_voltage_check(uint16_t* cell_voltages);
static uint8_t pack_temperature_check(uint16_t* pack_temperatures);

// initialize PEC table necessary for LTC68xx interface
void LTC_initialize()
{
    init_PEC15_Table();
    init_cfgr_A_and_B_buffers();
    update_config_A_and_B();
}

// read configuration register A
uint8_t read_config_reg_a(uint8_t* buffer)
{
    return read_config_A(buffer);
}

// get latest config buffer values and write config registers
uint8_t update_config_A_and_B(void)
{
    refresh_cfgra_write_buffer();
    refresh_cfgrb_write_buffer();
    
    write_config_A();
    write_config_B();
    return SUCCESS;
}

// send commands to get cell voltages
uint8_t read_cell_voltages(uint16_t* cell_voltages, uint8_t* cell_voltage_invalid_counter)
{
    start_cell_voltage_adc_conversion();
    poll_adc_status();
    __delay_ms(10); //TODO: is this delay necessary?
    receive_voltage_register(ADCVA, &cell_voltages[0], &cell_voltage_invalid_counter[0]); //TODO is cell_voltage_invalid_counter index consistent w/ ltc_cmds?
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
void open_sense_line_check(uint32_t* sense_line_status)
{
    //see pg 32 of 6813 datasheet for info
    open_wire_check(1); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    open_wire_check(1); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    open_wire_check(1); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    open_wire_check(1); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    open_wire_check(1); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    open_wire_check(1); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    uint16_t cell_pu[NUM_CELLS]; //TODO make sure valid PEC is received when getting voltage reg values
    uint8_t data_not_valid; //TODO make this work for multiple ICs
    uint8_t i = 0;
    for(i = 0; i < NUM_CELLS; ++i)
    {
        cell_pu[i] = 0;
    }
    
    do
    {
        data_not_valid = 0;
        receive_voltage_register(ADCVA, &cell_pu[0], &data_not_valid);
    } while(data_not_valid != 0);
    do
    {
        data_not_valid = 0;
        receive_voltage_register(ADCVB, &cell_pu[3], &data_not_valid);
    } while(data_not_valid != 0);
    do
    {
        data_not_valid = 0;
        receive_voltage_register(ADCVC, &cell_pu[6], &data_not_valid);
    } while(data_not_valid != 0);
    do
    {
        data_not_valid = 0;
        receive_voltage_register(ADCVD, &cell_pu[9], &data_not_valid);
    } while(data_not_valid != 0);
    do
    {
        data_not_valid = 0;
        receive_voltage_register(ADCVE, &cell_pu[12], &data_not_valid);
    } while(data_not_valid != 0);
    do
    {
        data_not_valid = 0;
        receive_voltage_register(ADCVF, &cell_pu[15], &data_not_valid);
    } while(data_not_valid != 0);
    
    open_wire_check(0); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    open_wire_check(0); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    open_wire_check(0); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    open_wire_check(0); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    open_wire_check(0); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    open_wire_check(0); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    uint16_t cell_pd[NUM_CELLS];
    for(i = 0; i < NUM_CELLS; ++i)
    {
        cell_pd[i] = 0;
    }

    do
    {
        data_not_valid = 0;
        receive_voltage_register(ADCVA, &cell_pd[0], &data_not_valid);
    } while(data_not_valid != 0);
    do
    {
        data_not_valid = 0;
        receive_voltage_register(ADCVB, &cell_pd[3], &data_not_valid);
    } while(data_not_valid != 0);
    do
    {
        data_not_valid = 0;
        receive_voltage_register(ADCVC, &cell_pd[6], &data_not_valid);
    } while(data_not_valid != 0);
    do
    {
        data_not_valid = 0;
        receive_voltage_register(ADCVD, &cell_pd[9], &data_not_valid);
    } while(data_not_valid != 0);
    do
    {
        data_not_valid = 0;
        receive_voltage_register(ADCVE, &cell_pd[12], &data_not_valid);
    } while(data_not_valid != 0);
    do
    {
        data_not_valid = 0;
        receive_voltage_register(ADCVF, &cell_pd[15], &data_not_valid);
    } while(data_not_valid != 0);
    
    for(i = 0; i < NUM_CELLS; ++i) // for each ic - 0-5
    {
        if(i % 18 == 0) // sense line 0 in a segment
        {
            if(cell_pu[i] == 0) // TODO: exactly equal to zero or just close to zero?
            {
                sense_line_status[i / 18] |= (1 << i);
                increment_sense_line_fault(i);
            }
            else
            {
                sense_line_status[i / 18] &= (uint32_t)(~(1 << i));
                reset_sense_line_fault(i);
            }
        }
        else if(i % 18 == 17) // cell 18 in a segment
        {
            if(cell_pd[i] == 0) // TODO: exactly equal to zero or just close to zero?
            {
                sense_line_status[i / 18] |= (1 << i);
                increment_sense_line_fault(i);
            }
            else
            {
                sense_line_status[i / 18] &= (uint32_t)(~(1 << i));
                reset_sense_line_fault(i);
            }
        }
        else // cells 2-17 in a segment
        {
            int16_t delta = cell_pu[i+1] - cell_pd[i+1]; // V * 10000
            if(delta < -4000) //TODO magic number
            {
                sense_line_status[i / 18] |= (1 << i);
                increment_sense_line_fault(i);
            }
            else
            {
                sense_line_status[i / 18] &= (uint32_t)(~(1 << i));
                reset_sense_line_fault(i);
            }
        }
    }
    
}

// run self-test commands
void self_test()
{
    cell_voltage_self_test();
    __delay_ms(10); //TODO: is this delay necessary?
    uint16_t cell_voltages[NUM_CELLS];
    uint8_t cell_voltage_invalid_counter[6*NUM_ICS];
    receive_voltage_register(ADCVA, &cell_voltages[0], &cell_voltage_invalid_counter[0]); //TODO is cell_voltage_invalid_counter index consistent w/ ltc_cmds?
    receive_voltage_register(ADCVB, &cell_voltages[3], &cell_voltage_invalid_counter[3]);
    receive_voltage_register(ADCVC, &cell_voltages[6], &cell_voltage_invalid_counter[6]);
    receive_voltage_register(ADCVD, &cell_voltages[9], &cell_voltage_invalid_counter[9]);
    receive_voltage_register(ADCVE, &cell_voltages[12], &cell_voltage_invalid_counter[12]);
    receive_voltage_register(ADCVF, &cell_voltages[15], &cell_voltage_invalid_counter[15]);
    
    // check whether received values are expected value
    uint8_t i = 0;
    uint8_t k = 0;
    for(i = 0; i < NUM_ICS; ++i)
    {
        uint8_t pass = 1;
        for(k = i*CELLS_PER_IC; k < (i + 1) * CELLS_PER_IC; ++k)
        {
            if(cell_voltages[k] != SELF_TEST_RESULT)
            {
                pass = 0;
            }   
        }
        if(pass == 0)
        {
            increment_self_test_fault(i);
        }
        else
        {
            reset_self_test_fault(i);
        }
    }
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