/* 
 * File:   6823_cmds.c
 * Author: Elena
 *
 * Created on January 27, 2022, 4:55 PM
 */

#include "LTC_cmds.h"
#include "../LTC_utilities.h"
#include "../../mcc_generated_files/pin_manager.h"
#include "../../mcc_generated_files/spi1.h"
#include "../../fault_handler.h"
#include <stdint.h>
#include "../../global_constants.h"

// see LTC6813 datasheet page 59 table 37 Command Bit Descriptions
#define MD          0b10    // ADC mode
#define DCP         0b0     // discharge permitted
#define CH          0b000   // cell selection for ADC conversion
#define PUP         0b1     // pull up for open wire conversions
#define PDN         0b0     // pull down for open wire conversions
#define CHG         0b000   // GPIO selection for ADC conversion
#define ST          0b01    // which self-test to run

uint8_t buffer_iterator = 0;
uint8_t cmd[4];
uint8_t dummy_buf[4];
uint16_t cmd_pec = 0;

/* send command to start ADC conversion for cell voltages
 * command: ADCV starts conversion
 */
void start_cell_voltage_adc_conversion(void)
{        
    wakeup_daisychain();
    
    //ADCV cmd
    cmd[0] = 0x02 | ((MD >> 1) & 0b01);
    cmd[1] = ((MD&0b01) << 7) | 0x60 | (DCP << 4) | CH;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    CS_6820_SetLow(); 
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);
    CS_6820_SetHigh();
    __delay_us(2); //TODO is this necessary?
}

/* send command to start ADC conversion for GPIO pins (temp sensors)
 * command: ADAX starts conversion
 */
void start_temperature_adc_conversion(void)
{        
    wakeup_daisychain();
    
    //ADAX cmd
    cmd[0] = 0x04 | ((MD >> 1) & 0b01);
    cmd[1] = ((MD&0b01) << 7) | 0x60 | CHG;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    CS_6820_SetLow(); 
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);
    CS_6820_SetHigh();
    __delay_us(2); //TODO is this necessary?
}

/* send command to poll ADC status
 * command: PLADC polls ADC status until conversion is complete
 */
void poll_adc_status(void)
{
    //PLADC cmd
    cmd[0] = 0x07;
    cmd[1] = 0x14;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);

    CS_6820_SetLow();
    //send PLADC command
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);
    
    uint8_t dummy_adc = 0;

    //when ADC conversion is complete, MISO will be pulled high
    while(dummy_adc <= 0)
    {
        LED6_Toggle();
        dummy_adc = SPI1_Exchange8bit(DUMMY);
    }
    CS_6820_SetHigh();  
}

/* receive voltage register data
 * command: RDCV
 */
void receive_voltage_register(uint8_t which_reg, uint16_t* buf)
{
    wakeup_daisychain();
        
    switch(which_reg)
    {
        case ADCVA:            
            //RDCVA command
            cmd[0] = 0x00;
            cmd[1] = 0x04;
            break;
        case ADCVB:
            //RDCVB command
            cmd[0] = 0x00;
            cmd[1] = 0x06;
            break;
        case ADCVC:
            //RDCVC command
            cmd[0] = 0x00;
            cmd[1] = 0x08;
            break;
        case ADCVD:
            //RDCVD command
            cmd[0] = 0x00;
            cmd[1] = 0x0A;
            break;
        case ADCVE:
            //RDCVE command
            cmd[0] = 0x00;
            cmd[1] = 0x09;
            break;
        case ADCVF:
            //RDCVF command
            cmd[0] = 0x00;
            cmd[1] = 0x0B;
            break;
        default:
            cmd[0] = 0x00;
            cmd[1] = 0x00;
            break;
    }

    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    CS_6820_SetLow();
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);

    // verify PEC for each of the 6 cell voltage messages received from each of the ICs in the daisy chain
    // if PEC is valid, write cell voltages to buf to be shared over CAN bus
    uint8_t i = 0;
    for(i = 0; i < NUM_ICS; ++i)
    {
        uint8_t k = 0;
        bool got_valid_pec = false;
        for(k = 0; k < 10; ++k) //TODO magic number. try 10 times to get valid PEC
        {
            __delay_us(2); //TODO is this necessary? Want to put some time gap between attempts at reading registers
            // 8 data bytes = 2 * 3 cell voltages + 2 PEC bytes
            uint8_t adcv_buf[8 * NUM_ICS];
            SPI1_Exchange8bitBuffer(dummy_buf, 8 * NUM_ICS, adcv_buf);
    
            // if valid PEC: update cell voltages, reset invalid counter
            if(verify_pec(&adcv_buf[8*i], 6, &adcv_buf[8 * i + 6]) == SUCCESS)
            {
                buf[CELLS_PER_IC*i] = (adcv_buf[8*i + 1] << 8) + adcv_buf[8*i];
                buf[CELLS_PER_IC*i + 1] = (adcv_buf[8*i + 3] << 8) + adcv_buf[8*i + 2];
                buf[CELLS_PER_IC*i + 2] = (adcv_buf[8*i + 5] << 8) + adcv_buf[8*i + 4];
                reset_missing_voltage_measurement_fault(which_reg + i*6);
                // adcv_buf 6 and 7 are PEC bytes
                got_valid_pec = true;
                break; //end for loop
            }
        }
        
        if(got_valid_pec == false)
        {
            increment_missing_voltage_measurement_fault(which_reg + i*6);
            // TODO only do this if we get several missing measurement faults in a row?
            if(get_missing_voltage_measurement_fault(which_reg + i*6) > 10) //TODO magic number
            {
                buf[CELLS_PER_IC*i] = 0;
                buf[CELLS_PER_IC*i + 1] = 0;
                buf[CELLS_PER_IC*i + 2] = 0;
            }
        }
    }

    CS_6820_SetHigh();
}

/* receive GPIO register data. Temperature data is in these registers
 * command: RDAUX
 */
void receive_aux_register(uint8_t which_reg, uint16_t* buf)
{
    wakeup_daisychain();
        
    switch(which_reg)
    {
        case AUXA:            
            //RDAUXA command
            cmd[0] = 0x00;
            cmd[1] = 0x0C;
            break;
        case AUXB:
            //RDAUXB command
            cmd[0] = 0x00;
            cmd[1] = 0x0E;
            break;
        case AUXC:
            //RDAUXC command
            cmd[0] = 0x00;
            cmd[1] = 0x0D;
            break;
        case AUXD:
            //RDAUXD command
            cmd[0] = 0x00;
            cmd[1] = 0x0F;
            break;
        default:
            cmd[0] = 0x00;
            cmd[1] = 0x00;
            break;
    }

    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    CS_6820_SetLow();
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);

    // verify PEC for each of the 4 GPIO registers received from each of the ICs in the daisy chain
    // if PEC is valid, write GPIO messages to buf to be shared over CAN bus
    uint8_t i = 0;
    for(i = 0; i < NUM_ICS; ++i)
    {
        uint8_t k = 0;
        bool got_valid_pec = false;
        for(k = 0; k < 10; ++k) //TODO magic number. try 10 times to get valid PEC
        {
            __delay_us(2); //TODO is this necessary? Want to put some time gap between attempts at reading registers
            // 8 data bytes = 2 * 3 GPIO results + 2 PEC bytes
            uint8_t adaux_buf[8 * NUM_ICS];
            SPI1_Exchange8bitBuffer(dummy_buf, 8 * NUM_ICS, adaux_buf);

            // if valid PEC: update cell voltages, reset invalid counter
            if(verify_pec(&adaux_buf[8*i], 6, &adaux_buf[8 * i + 6]) == SUCCESS)
            {
                // for each IC, 12 aux data bytes will be returned
                buf[12*i] = (adaux_buf[8*i + 1] << 8) + adaux_buf[8*i];
                buf[12*i + 1] = (adaux_buf[8*i + 3] << 8) + adaux_buf[8*i + 2];
                buf[12*i + 2] = (adaux_buf[8*i + 5] << 8) + adaux_buf[8*i + 4];
                // adaux_buf 6 and 7 are PEC bytes
                reset_missing_temperature_fault(which_reg + i*4);
                got_valid_pec = true;
                break; //end for loop
            }
        }

        if(got_valid_pec == false)
        {
            increment_missing_temperature_fault(which_reg + i*4);
            // TODO only do this if we get several missing measurement faults in a row?
            if(get_missing_temperature_fault(which_reg + i*4) > 10) //TODO magic number
            {
                buf[12*i] = 0;
                buf[12*i + 1] = 0;
                buf[12*i + 2] = 0;
            }
        }
    }

    CS_6820_SetHigh();
}

/* run open sense line check
 * command: ADOW
 */
void start_open_wire_check(uint8_t pull_dir)
{
    wakeup_daisychain();
    
    //ADOW cmd
    cmd[0] = 0x02 | ((MD >> 1) & 0b01);
    cmd[1] = ((MD&0b01) << 7) | 0x28 | (pull_dir << 6) | (DCP << 4) | CH;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    CS_6820_SetLow(); 
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);

    CS_6820_SetHigh();
}

/*
 * run self test on cell voltage ADCs
 * command: CVST
 */
void start_cell_voltage_self_test()
{
    wakeup_daisychain();
    
    //CVST cmd
    cmd[0] = 0x02 | ((MD >> 1) & 0b01);
    cmd[1] = ((MD & 0b01) << 7) | (ST << 5) | 0x7;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    CS_6820_SetLow(); 
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);
    
    uint8_t dummy_adc = 0;

    //when ADC conversion is complete, MISO will be pulled high
    while(dummy_adc <= 0)
    {
        LED6_Toggle();
        dummy_adc = SPI1_Exchange8bit(DUMMY);
    }

    CS_6820_SetHigh();
}

/*
 * run self test on GPIO ADCs
 * command: AXST
 */
void start_aux_reg_self_test()
{
    wakeup_daisychain();

    //AXST cmd
    cmd[0] = 0x04 | ((MD >> 1) & 0b01);
    cmd[1] = ((MD & 0b01) << 7) | (ST << 5) | 0x7;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    CS_6820_SetLow();
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);

    uint8_t dummy_adc = 0;

    //when ADC conversion is complete, MISO will be pulled high
    while(dummy_adc <= 0)
    {
        LED6_Toggle();
        dummy_adc = SPI1_Exchange8bit(DUMMY);
    }

    CS_6820_SetHigh();
}

/*
 * read config register A
 */
uint8_t read_config_A(uint8_t* buffer)
{
    wakeup_daisychain();
    
    //RDCFGA cmd
    cmd[0] = 0x00;
    cmd[1] = 0x02;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    CS_6820_SetLow(); 
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);
    SPI1_Exchange8bitBuffer(dummy_buf, 6*NUM_ICS, buffer);
    CS_6820_SetHigh();
    
    if(verify_pec(buffer, 6, &buffer[6]) == SUCCESS)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 * write configuration register A
 */
void write_config_A(void)
{
    wakeup_daisychain();
    
    //WRCFGA cmd
    cmd[0] = 0x00;
    cmd[1] = 0x01;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    
    CS_6820_SetLow(); 
    
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);
    
    uint8_t i = 0;
    for(i = 0; i < NUM_ICS; ++i)
    {
        uint8_t* data_to_write = get_cfgra_write_buffer(i);
        uint16_t data_pec = pec15_calc(data_to_write, 6);
        uint8_t data_pec_transmit[2] = {(uint8_t)(data_pec >> 8), (uint8_t)(data_pec & 0xFF)};    

        SPI1_Exchange8bitBuffer(data_to_write, 6*NUM_ICS, dummy_buf);
        SPI1_Exchange8bitBuffer(data_pec_transmit, 2, dummy_buf);
    }
    CS_6820_SetHigh();
}

/*
 * write configuration register B
 */
void write_config_B(void)
{
    wakeup_daisychain();
    
    //WRCFGB cmd
    cmd[0] = 0x00;
    cmd[1] = 0x24;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    
    CS_6820_SetLow(); 
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);
    
    uint8_t i = 0;
    for(i = 0; i < NUM_ICS; ++i)
    {
        uint8_t* data_to_write = get_cfgrb_write_buffer(i);

        uint16_t data_pec = pec15_calc(data_to_write, 6);
        uint8_t data_pec_transmit[2] = {(uint8_t)(data_pec >> 8), (uint8_t)(data_pec & 0xFF)};    

        SPI1_Exchange8bitBuffer(data_to_write, 6*NUM_ICS, dummy_buf);
        SPI1_Exchange8bitBuffer(data_pec_transmit, 2, dummy_buf); 
    }
    CS_6820_SetHigh();
}

/*
 * read status register A
 */
uint8_t read_status_A(uint8_t* buffer)
{
    wakeup_daisychain();

    //RDSTATA cmd
    cmd[0] = 0x00;
    cmd[1] = 0x10;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    CS_6820_SetLow();
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);
    SPI1_Exchange8bitBuffer(dummy_buf, 6*NUM_ICS, buffer);
    CS_6820_SetHigh();

    if(verify_pec(buffer, 6, &buffer[6]) == SUCCESS)
    {
        return SUCCESS;
    }
    return FAILURE;
}

/*
 * read status register B
 */
uint8_t read_status_B(uint8_t* buffer)
{
    wakeup_daisychain();

    //RDSTATB cmd
    cmd[0] = 0x00;
    cmd[1] = 0x12;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);

    uint8_t i = 0;
    for(i = 0; i < 10; ++i) // 10 tries to get valid PEC
    {
        CS_6820_SetLow();
        SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);
        SPI1_Exchange8bitBuffer(dummy_buf, 6*NUM_ICS, buffer);
        CS_6820_SetHigh();

        if(verify_pec(buffer, 6, &buffer[6]) == SUCCESS)
        {
            return SUCCESS;
        }
    }
    return FAILURE;
}

/*
 * run multiplexer self test
 * command: DIAGN
 */
void start_mux_self_test(void)
{
    wakeup_daisychain();

    //DIAGN cmd
    cmd[0] = 0x07;
    cmd[1] = 0x15;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    CS_6820_SetLow();
    SPI1_Exchange8bitBuffer(cmd, CMD_SIZE_BYTES, dummy_buf);

    uint8_t dummy_adc = 0;

    //when ADC conversion is complete, MISO will be pulled high
    while(dummy_adc <= 0)
    {
        LED6_Toggle();
        dummy_adc = SPI1_Exchange8bit(DUMMY);
    }

    CS_6820_SetHigh();
}