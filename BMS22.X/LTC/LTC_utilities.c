/* 
 * File:   pec_calc.c
 * Author: Elena
 *
 * Created on January 27, 2022, 4:43 PM
 */

#include "LTC_utilities.h"
#include <stdint.h>
#include "../mcc_generated_files/pin_manager.h"
#include "../global_constants.h"

///////////////////////// Config Register A ///////////////////////////////////
// see LTC6813 datasheet page 60 for config register contents
uint8_t cfgra_gpio5_1[NUM_ICS];
uint8_t cfgra_refon[NUM_ICS];
uint8_t cfgra_dten[NUM_ICS];
uint8_t cfgra_adcopt[NUM_ICS];
uint8_t cfgra_vuv7_0[NUM_ICS];
uint8_t cfgra_vuv11_8[NUM_ICS];
uint8_t cfgra_vov3_0[NUM_ICS];
uint8_t cfgra_vov11_4[NUM_ICS];
uint8_t cfgra_dcc8_1[NUM_ICS];
uint8_t cfgra_dcto3_0[NUM_ICS];
uint8_t cfgra_dcc12_9[NUM_ICS];
// to construct config register A from these values:
uint8_t config_reg_A[NUM_ICS][6];

///////////////////////// Config Register B ///////////////////////////////////
// see LTC6813 datasheet page 60 for config register contents
uint8_t cfgrb_dcc16_13[NUM_ICS];
uint8_t cfgrb_gpio9_6[NUM_ICS];
uint8_t cfgrb_mute[NUM_ICS];
uint8_t cfgrb_fdrf[NUM_ICS];
uint8_t cfgrb_ps1_0[NUM_ICS];
uint8_t cfgrb_dtmen[NUM_ICS];
uint8_t cfgrb_dcc0[NUM_ICS];
uint8_t cfgrb_dcc18_17[NUM_ICS];

// to construct config register B from these values:
uint8_t config_reg_B[NUM_ICS][6];

// initialize config register A and B values
void init_cfgr_A_and_B_buffers(void)
{
    uint8_t i = 0;
    for(i = 0; i < NUM_ICS; ++i)
    {
        cfgra_gpio5_1[i] = 0b11111;
        cfgra_refon[i] = 0b0;
        cfgra_dten[i] = 0b1;
        cfgra_adcopt[i] = 0b0;
        cfgra_vuv7_0[i] = 0x27;
        cfgra_vuv11_8[i] = 0x7;
        cfgra_vov3_0[i] = 0x1;
        cfgra_vov11_4[i] = 0xA4;
        cfgra_dcc8_1[i] = 0b00000000;
        cfgra_dcto3_0[i] = 0x5;
        cfgra_dcc12_9[i] = 0b0000;

        cfgrb_dcc16_13[i] = 0b0000;
        cfgrb_gpio9_6[i] = 0b1111;
        cfgrb_mute[i] = 0b0;
        cfgrb_fdrf[i] = 0b0;
        cfgrb_ps1_0[i] = 0b00;
        cfgrb_dtmen[i] = 0b1;
        cfgrb_dcc0[i] = 0b0;
        cfgrb_dcc18_17[i] = 0b00;
    }
}

// Generic wakeup command to wake the LTC681x from sleep state
// wakeup time depends on number of chips in daisy chain
void wakeup_daisychain(void) 
{
    uint8_t i = 0;
	for (i = 0; i < NUM_ICS; ++i)
	{
	   CS_6820_SetLow();
	   __delay_us(300); // Guarantees the LTC681x will be in standby (rather than sleep)
	   CS_6820_SetHigh();
	   __delay_us(10);
	}
}

// verify that received packet error code matches calculated packet error code
uint8_t verify_pec(uint8_t* data, uint8_t size, uint8_t* received_pec)
{
    uint16_t calculated_pec = pec15_calc(data, size);
    uint16_t transmitted_pec = (received_pec[0] << 8) + received_pec[1];
    if(calculated_pec == transmitted_pec)
    {
        LED5_SetHigh();
        return SUCCESS;
    }
    else
    {
        LED5_SetLow();
        return FAILURE;
    }
}

void refresh_cfgra_write_buffer(void)
{
    uint8_t i = 0;
    
    for(i = 0; i < NUM_ICS; ++i)
    {
        uint8_t config_reg_A0 = (cfgra_gpio5_1[i] << 3) | (cfgra_refon[i] << 2) | (cfgra_dten[i] << 1) | (cfgra_adcopt[i]);
        uint8_t config_reg_A1 = (cfgra_vuv7_0[i]);
        uint8_t config_reg_A2 = (cfgra_vov3_0[i] << 4) | (cfgra_vuv11_8[i]);
        uint8_t config_reg_A3 = (cfgra_vov11_4[i]);
        uint8_t config_reg_A4 = (cfgra_dcc8_1[i]);
        uint8_t config_reg_A5 = (cfgra_dcto3_0[i] << 4) | (cfgra_dcc12_9[i]);

        config_reg_A[i][0] = config_reg_A0;
        config_reg_A[i][1] = config_reg_A1;
        config_reg_A[i][2] = config_reg_A2;
        config_reg_A[i][3] = config_reg_A3;
        config_reg_A[i][4] = config_reg_A4;
        config_reg_A[i][5] = config_reg_A5;
    }
}

// TODO does this return val work?
uint8_t* get_cfgra_write_buffer(uint8_t chip_number)
{
    return &config_reg_A[chip_number][0];
}

void set_cfgra_dcc8_1(uint8_t chip_number, uint8_t set_value)
{
    cfgra_dcc8_1[chip_number] = set_value;
}

void set_cfgra_dcc12_9(uint8_t chip_number, uint8_t set_value)
{
    cfgra_dcc12_9[chip_number] = set_value;
}

void refresh_cfgrb_write_buffer(void)
{
    uint8_t i = 0;
    for(i = 0; i < NUM_ICS; ++i)
    {
        // B2-B5 are reserved bits
        uint8_t config_reg_B0 = (cfgrb_dcc16_13[i] << 4) | (cfgrb_gpio9_6[i]);
        uint8_t config_reg_B1 = (cfgrb_mute[i] << 7) | (cfgrb_fdrf[i] << 6) | (cfgrb_ps1_0[i] << 4) | (cfgrb_dtmen[i] << 3) | (cfgrb_dcc0[i] << 2) | (cfgrb_dcc18_17[i]);
        uint8_t config_reg_B2 = 0x00;
        uint8_t config_reg_B3 = 0x00;
        uint8_t config_reg_B4 = 0x00;
        uint8_t config_reg_B5 = 0x00;

        config_reg_B[i][0] = config_reg_B0;
        config_reg_B[i][1] = config_reg_B1;
        config_reg_B[i][2] = config_reg_B2;
        config_reg_B[i][3] = config_reg_B3;
        config_reg_B[i][4] = config_reg_B4;
        config_reg_B[i][5] = config_reg_B5;
    }
}

// TODO does this return val work?
uint8_t* get_cfgrb_write_buffer(uint8_t chip_number)
{
    return &config_reg_B[chip_number][0];
}

void set_cfgrb_dcc16_13(uint8_t chip_number, uint8_t set_value)
{
    cfgrb_dcc16_13[chip_number] = set_value;
}

void set_cfgrb_dcc18_17(uint8_t chip_number, uint8_t set_value)
{
    cfgrb_dcc18_17[chip_number] = set_value;
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
    uint16_t i;
    for (i = 0; i < 256; i++)
    {
        remainder = i << 7;
        uint8_t bit;
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

// calculated packet error code based on received data and PEC table
uint16_t pec15_calc(uint8_t *data , uint8_t len)
{
    int16_t remainder,address;
    remainder = 16;//PEC seed
    uint8_t i;
    for (i = 0; i < len; i++)
    {
        address = ((remainder >> 7) ^ data[i]) & 0xff;//calculate PEC table address
        remainder = (remainder << 8 ) ^ pec15Table[address];
    }
    return (remainder*2);//The CRC15 has a 0 in the LSB so the final value must be multiplied by 2
}