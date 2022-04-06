
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
#include <stdbool.h>
#include "../global_constants.h"

//TODO come up with better way to verify that valid PEC is received from commands'
    
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
uint8_t read_cell_voltages(uint16_t* cell_voltages)
{
    start_cell_voltage_adc_conversion();
    poll_adc_status();
    __delay_ms(10); //TODO: is this delay necessary?
    bool valid_pecA = false;
    bool valid_pecB = false;
    bool valid_pecC = false;
    bool valid_pecD = false;
    bool valid_pecE = false;
    bool valid_pecF = false;
    
    uint8_t i = 0;
    for(i = 0; i < 10; ++i)
    {
        if(valid_pecA == false)
            receive_voltage_register(ADCVA, &cell_voltages[0], &valid_pecA);
        if(valid_pecB == false)
            receive_voltage_register(ADCVB, &cell_voltages[3], &valid_pecB);
        if(valid_pecC == false)
            receive_voltage_register(ADCVC, &cell_voltages[6], &valid_pecC);
        if(valid_pecD == false)
            receive_voltage_register(ADCVD, &cell_voltages[9], &valid_pecD);
        if(valid_pecE == false)
            receive_voltage_register(ADCVE, &cell_voltages[12], &valid_pecE);
        if(valid_pecF == false)
            receive_voltage_register(ADCVF, &cell_voltages[15], &valid_pecF);
    }
    
    return cell_voltage_check(cell_voltages);
}

// send commands to get pack temperatures
uint8_t read_temperatures(uint16_t* pack_temperatures)
{
    start_temperature_adc_conversion();
    poll_adc_status();
    __delay_ms(10); //TODO: is this delay necessary?
    bool valid_pecA = false;
    bool valid_pecB = false;
    bool valid_pecC = false;
    bool valid_pecD = false;

    // store aux register values in intermediate array since not all data
    // is temperature sensor data. See LTC6813 datasheet pg 62
    uint16_t aux_reg[12*NUM_ICS];
    uint8_t i = 0;
    for(i = 0; i < 10; ++i)
    {
        if(valid_pecA == false)
            receive_aux_register(AUXA, &aux_reg[0], &valid_pecA);
        if(valid_pecB == false)
            receive_aux_register(AUXB, &aux_reg[3], &valid_pecB);
        if(valid_pecC == false)
            receive_aux_register(AUXC, &aux_reg[6], &valid_pecC);
        if(valid_pecD == false)
            receive_aux_register(AUXD, &aux_reg[9], &valid_pecD);
    }
    
    // copy over temperature data to temperature array
    i = 0;
    // TODO RHS indices seem wrong
    for(i = 0; i < NUM_ICS; ++i)
    {
        pack_temperatures[i*TEMP_SENSORS_PER_IC] = aux_reg[12*i];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 1] = aux_reg[(12*i) + 1];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 2] = aux_reg[(12*i) + 2];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 3] = aux_reg[(12*i) + 3];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 4] = aux_reg[(12*i) + 4];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 5] = aux_reg[(12*i) + 6];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 6] = aux_reg[(12*i) + 7];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 7] = aux_reg[(12*i) + 8];
        pack_temperatures[i*TEMP_SENSORS_PER_IC + 8] = aux_reg[(12*i) + 9];
    }
    
    return pack_temperature_check(pack_temperatures);
}

// check whether sense line overcurrent protection has tripped
void open_sense_line_check(uint32_t* sense_line_status)
{
    //see pg 32 of 6813 datasheet for info
    start_open_wire_check(1); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    start_open_wire_check(1); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    start_open_wire_check(1); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    start_open_wire_check(1); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    start_open_wire_check(1); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    start_open_wire_check(1); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    uint16_t cell_pu[NUM_CELLS];
    uint8_t i = 0;
    for(i = 0; i < NUM_CELLS; ++i)
    {
        cell_pu[i] = 0;
    }
    
    bool valid_pecA = false;
    bool valid_pecB = false;
    bool valid_pecC = false;
    bool valid_pecD = false;
    bool valid_pecE = false;
    bool valid_pecF = false;
    
    i = 0;
    for(i = 0; i < 10; ++i)
    {
        if(valid_pecA == false)
            receive_voltage_register(ADCVA, &cell_pu[0], &valid_pecA);
        if(valid_pecB == false)
            receive_voltage_register(ADCVB, &cell_pu[3], &valid_pecB);
        if(valid_pecC == false)
            receive_voltage_register(ADCVC, &cell_pu[6], &valid_pecC);
        if(valid_pecD == false)
            receive_voltage_register(ADCVD, &cell_pu[9], &valid_pecD);
        if(valid_pecE == false)
            receive_voltage_register(ADCVE, &cell_pu[12], &valid_pecE);
        if(valid_pecF == false)
            receive_voltage_register(ADCVF, &cell_pu[15], &valid_pecF);
    }
    
    start_open_wire_check(0); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    start_open_wire_check(0); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    start_open_wire_check(0); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    start_open_wire_check(0); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    start_open_wire_check(0); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    __delay_us(10);
    start_open_wire_check(0); // param: pull dir 0 for down 1 for up
    poll_adc_status();
    uint16_t cell_pd[NUM_CELLS];
    for(i = 0; i < NUM_CELLS; ++i)
    {
        cell_pd[i] = 0;
    }

    valid_pecA = false;
    valid_pecB = false;
    valid_pecC = false;
    valid_pecD = false;
    valid_pecE = false;
    valid_pecF = false;
    
    i = 0;
    for(i = 0; i < 10; ++i)
    {
        if(valid_pecA == false)
            receive_voltage_register(ADCVA, &cell_pd[0], &valid_pecA);
        if(valid_pecB == false)
            receive_voltage_register(ADCVB, &cell_pd[3], &valid_pecB);
        if(valid_pecC == false)
            receive_voltage_register(ADCVC, &cell_pd[6], &valid_pecC);
        if(valid_pecD == false)
            receive_voltage_register(ADCVD, &cell_pd[9], &valid_pecD);
        if(valid_pecE == false)
            receive_voltage_register(ADCVE, &cell_pd[12], &valid_pecE);
        if(valid_pecF == false)
            receive_voltage_register(ADCVF, &cell_pd[15], &valid_pecF);
    }
    
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
    start_cell_voltage_self_test();
    __delay_ms(10); //TODO: is this delay necessary?
    uint16_t self_test_cv[NUM_CELLS];
    bool valid_pecA = false;
    bool valid_pecB = false;
    bool valid_pecC = false;
    bool valid_pecD = false;
    bool valid_pecE = false;
    bool valid_pecF = false;
    
    uint8_t j = 0;
    for(j = 0; j < 10; ++j)
    {
        if(valid_pecA == false)
            receive_voltage_register(ADCVA, &self_test_cv[0], &valid_pecA);
        if(valid_pecB == false)
            receive_voltage_register(ADCVB, &self_test_cv[3], &valid_pecB);
        if(valid_pecC == false)
            receive_voltage_register(ADCVC, &self_test_cv[6], &valid_pecC);
        if(valid_pecD == false)
            receive_voltage_register(ADCVD, &self_test_cv[9], &valid_pecD);
        if(valid_pecE == false)
            receive_voltage_register(ADCVE, &self_test_cv[12], &valid_pecE);
        if(valid_pecF == false)
            receive_voltage_register(ADCVF, &self_test_cv[15], &valid_pecF);
    }
    
    // check whether received values are expected value
    uint8_t i = 0;
    uint8_t k = 0;
    for(i = 0; i < NUM_ICS * CV_REGISTERS_PER_IC; ++i)
    {
        bool pass = true;
        for(k = i * CELLS_PER_REGISTER; k < (i + 1) * CELLS_PER_REGISTER; ++k)
        {
            if(self_test_cv[k] != CV_SELF_TEST_RESULT)
            {
                pass = false;
            }   
        }
        if(pass == false)
        {
            increment_cv_self_test_fault(i);
        }
        else
        {
            reset_cv_self_test_fault(i);
        }
    }
    
    start_aux_reg_self_test();
    __delay_ms(10) //TODO is this necessary?
    uint16_t aux_registers[12 * NUM_ICS];
    j = 0;
    valid_pecA = false;
    valid_pecB = false;
    valid_pecC = false;
    valid_pecD = false;
    for(j = 0; j < 10; ++j)
    {
        if(valid_pecA == false)
            receive_voltage_register(ADCVA, &aux_registers[0], &valid_pecA);
        if(valid_pecB == false)
            receive_voltage_register(ADCVB, &aux_registers[3], &valid_pecB);
        if(valid_pecC == false)
            receive_voltage_register(ADCVC, &aux_registers[6], &valid_pecC);
        if(valid_pecD == false)
            receive_voltage_register(ADCVD, &aux_registers[9], &valid_pecD);
    }
    
    // check whether received values are expected value
    i = 0;
    k = 0;
    for(i = 0; i < NUM_ICS * AUX_REGISTERS_PER_IC; ++i)
    {
        bool pass = true;
        for(k = i * 3; k < (i + 1) * 3; ++k) // TODO magic number VALS_PER_REG = 3
        {
            if(k % 12 == 10 || k % 12 == 11) //skip reserved bit places in AUXD
            {
                break;
            }
            if(aux_registers[k] != AUX_SELF_TEST_RESULT)
            {
                pass = false;
            }
        }
        if(pass == false)
        {
            increment_aux_self_test_fault(i);
        }
        else
        {
            reset_aux_self_test_fault(i);
        }
    }

    start_mux_self_test();
    __delay_ms(10); //TODO: is this delay necessary?
    uint8_t status_b[6*NUM_ICS];
    read_status_B(status_b);
    // check MUXFAIL bits
    i = 0;
    for(i = 0; i < NUM_ICS; ++i)
    {
        uint8_t muxfail_bit = ((status_b[i * 6 + 5]) >> 1) & 0b1;
        if(muxfail_bit == 1)
        {
            increment_mux_self_test_fault(i);
        }
        else
        {
            reset_mux_self_test_fault(i);
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