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

uint16_t cell_one_v_x_tenthou = 0;
uint16_t cell_two_v_x_tenthou = 0;
uint16_t cell_three_v_x_tenthou = 0;

void start_cell_voltage_adc_conversion(void)
{        
    wakeup_sleep(1);
    
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
    dummy_adc = SPI1_Exchange8bit(DUMMY);

    //when ADC conversion is complete, MISO will be pulled high
    while(dummy_adc <= 0)
    {
        LED6_Toggle();
        dummy_adc = SPI1_Exchange8bit(DUMMY);
    }
    CS_6820_SetHigh();  
    
}

void rdcva_register(void)
{
    wakeup_sleep(1);
        
    //RDCVA command
    cmd[0] = 0x00;
    cmd[1] = 0x04;
    cmd_pec = pec15_calc(cmd, 2);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    \
    CS_6820_SetLow();
    for(buffer_iterator = 0; buffer_iterator < 4; ++buffer_iterator)
    {
        dummy_buf[buffer_iterator] = SPI1_Exchange8bit(cmd[buffer_iterator]);
    }

    uint8_t adcva_buf[8];
    for(buffer_iterator = 0; buffer_iterator < 8; ++buffer_iterator)
    {
        adcva_buf[buffer_iterator] = SPI1_Exchange8bit(DUMMY);
    }

    uint8_t pec_success = verify_pec(adcva_buf, 6, &adcva_buf[6]);

    cell_one_v_x_tenthou = (adcva_buf[1] << 8) + adcva_buf[0];
    cell_two_v_x_tenthou = (adcva_buf[3] << 8) + adcva_buf[2];
    cell_three_v_x_tenthou = (adcva_buf[5] << 8) + adcva_buf[4];

    CS_6820_SetHigh();
}