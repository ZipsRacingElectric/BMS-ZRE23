/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  dsPIC33EP128GS804
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB 	          :  MPLAB X v5.45
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"
#include <stdint.h>

#include "mcc_generated_files/pin_manager.h"
#include "can_driver.h"
#include "soc_fns.h"
#include "mcc_generated_files/spi1.h"

#define FCY 40000000UL // Instruction cycle frequency, Hz - required for __delayXXX() to work
#include <libpic30.h>        // __delayXXX() functions macros defined here

void init_PEC15_Table();
uint16_t pec15_calc(char *data , int len);
void wakeup_sleep(uint8_t total_ic);

uint16_t cell_one_v_x_tenthou = 0;
uint16_t cell_two_v_x_tenthou = 0;
uint16_t cell_three_v_x_tenthou = 0;

/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    CS_6820_SetHigh();

    soc_initialize();
    can_initialize();

    init_PEC15_Table();
    
    while (1)
    {
        send_status_msg();
        calc_soc();
        
        
//        CS_EEPROM_SetLow();
//        
//        CS_EEPROM_SetHigh();
        uint8_t buffer_iterator = 0;
        
        uint8_t cmd[4];
        uint8_t dummy_buf[4];
        uint16_t cmd_pec;
        
        // wakeup sleep
        wakeup_sleep(1);
        uint8_t dummy = 0xFF;
        uint8_t dummy_return = 0;
        CS_6820_SetLow();
        dummy_return = SPI1_Exchange8bit(dummy);
        CS_6820_SetHigh();

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
//        uint16_t dataSent = SPI1_Exchange8bitBuffer(cmd, 4, dummy_buf);
        CS_6820_SetHigh();
        __delay_us(2);
        //PLADC cmd
        cmd[0] = 0x07;
        cmd[1] = 0x14;
        cmd_pec = pec15_calc(cmd, 2);
        cmd[2] = (uint8_t)(cmd_pec >> 8);
        cmd[3] = (uint8_t)(cmd_pec);

        CS_6820_SetLow();
        for(buffer_iterator = 0; buffer_iterator < 4; ++buffer_iterator)
        {
            dummy_buf[buffer_iterator] = SPI1_Exchange8bit(cmd[buffer_iterator]);
        }
//        dataSent = SPI1_Exchange8bitBuffer(cmd, 4, dummy_buf);
//        CS_6820_SetHigh();
        
        uint8_t dummy_adc = 0;
//        CS_6820_SetLow();
        dummy_adc = SPI1_Exchange8bit(dummy);

        while(dummy_adc <= 0)
        {
            LED6_Toggle();
            dummy_adc = SPI1_Exchange8bit(dummy);
        }
        CS_6820_SetHigh();
        __delay_ms(10);
        
        // wakeup sleep
        wakeup_sleep(1);
        dummy = 0xFF;
        dummy_return = 0;
        CS_6820_SetLow();
        dummy_return = SPI1_Exchange8bit(dummy);
        CS_6820_SetHigh();
        
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
            adcva_buf[buffer_iterator] = SPI1_Exchange8bit(dummy);
        }
        
        cell_one_v_x_tenthou= (adcva_buf[1] << 8) + adcva_buf[0];
        cell_two_v_x_tenthou= (adcva_buf[3] << 8) + adcva_buf[2];
        cell_three_v_x_tenthou= (adcva_buf[5] << 8) + adcva_buf[4];

        CS_6820_SetHigh();
        
        LED1_HEARTBEAT_SetHigh();
        __delay_ms(250);
        LED1_HEARTBEAT_SetLow();

        __delay_ms(250);
    }
    return 1; 
}

/************************************
Copyright 2012 Analog Devices, Inc. (ADI)
Permission to freely use, copy, modify, and distribute this software for any purpose with or
without fee is hereby granted, provided that the above copyright notice and this permission
notice appear in all copies: THIS SOFTWARE IS PROVIDED ?AS IS? AND ADI DISCLAIMS ALL WARRANTIES
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL ADI BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM ANY
USE OF SAME, INCLUDING ANY LOSS OF USE OR DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
***************************************/
int16_t pec15Table[256];
int16_t CRC15_POLY = 0x4599;
void init_PEC15_Table()
{
    int16_t remainder;
    int i;
    for (i = 0; i < 256; i++)
    {
        remainder = i << 7;
        int bit;
        for (bit = 8; bit > 0; --bit)
        {
            if (remainder & 0x4000)
            {
                remainder = ((remainder << 1));
                remainder = (remainder ^ CRC15_POLY);
            }
            else
            {
                remainder = ((remainder << 1));
            }
        }
        pec15Table[i] = remainder&0xFFFF;
    }
}

uint16_t pec15_calc(char *data , int len)
{
    int16_t remainder,address;
    remainder = 16;//PEC seed
    int i;
    for (i = 0; i < len; i++)
    {
        address = ((remainder >> 7) ^ data[i]) & 0xff;//calculate PEC table address
        remainder = (remainder << 8 ) ^ pec15Table[address];
    }
    return (remainder*2);//The CRC15 has a 0 in the LSB so the final value must be multiplied by 2
}
/**
 End of File
*/

// but what if i'm using a 6820? Will this wake up the 6820?
/* Generic wakeup command to wake the LTC681x from sleep state */
void wakeup_sleep(uint8_t total_ic) //Number of ICs in the system
{
    int i = 0;
	for (i =0; i<total_ic; i++)
	{
	   CS_6820_SetLow();
	   __delay_us(300); // Guarantees the LTC681x will be in standby
	   CS_6820_SetHigh();
	   __delay_us(10);
	}
}