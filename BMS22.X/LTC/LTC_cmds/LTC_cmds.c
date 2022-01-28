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
#define FCY 40000000UL // Instruction cycle frequency, Hz - required for __delayXXX() to work
#include <libpic30.h>        // __delayXXX() functions macros defined here

uint8_t buffer_iterator = 0;
uint8_t cmd[4];
uint8_t dummy_buf[4];
uint16_t cmd_pec = 0;

void start_cell_voltage_adc_conversion(void)
{        
    wakeup_sleep(NUM_ICS);
    
    //ADCV cmd
    cmd[0] = 0x03;
    cmd[1] = 0x60;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    CS_6820_SetLow(); 
    for(buffer_iterator = 0; buffer_iterator < 4; ++buffer_iterator)
    {
        dummy_buf[buffer_iterator] = SPI1_Exchange8bit(cmd[buffer_iterator]);
    }
    CS_6820_SetHigh();
    __delay_us(2); //TODO is this necessary?
}

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
    for(buffer_iterator = 0; buffer_iterator < 4; ++buffer_iterator)
    {
        dummy_buf[buffer_iterator] = SPI1_Exchange8bit(cmd[buffer_iterator]);
    }
    
    uint8_t dummy_adc = 0;

    //when ADC conversion is complete, MISO will be pulled high
    while(dummy_adc <= 0)
    {
        LED6_Toggle();
        dummy_adc = SPI1_Exchange8bit(DUMMY);
    }
    CS_6820_SetHigh();  
    
}

void rdcv_register(uint8_t which_reg, uint16_t* buf)
{
    wakeup_sleep(NUM_ICS);
        
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
    for(buffer_iterator = 0; buffer_iterator < 4; ++buffer_iterator)
    {
        dummy_buf[buffer_iterator] = SPI1_Exchange8bit(cmd[buffer_iterator]);
    }

    uint8_t adcv_buf[8];
    for(buffer_iterator = 0; buffer_iterator < 8; ++buffer_iterator)
    {
        adcv_buf[buffer_iterator] = SPI1_Exchange8bit(DUMMY);
    }

    if(verify_pec(adcv_buf, 6, &adcv_buf[6]) == SUCCESS)
    {
        buf[0] = (adcv_buf[1] << 8) + adcv_buf[0];
        buf[1] = (adcv_buf[3] << 8) + adcv_buf[2];
        buf[2] = (adcv_buf[5] << 8) + adcv_buf[4];
    }

    CS_6820_SetHigh();
}