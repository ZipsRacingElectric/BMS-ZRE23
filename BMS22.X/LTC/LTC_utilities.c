/* 
 * File:   pec_calc.c
 * Author: Elena
 *
 * Created on January 27, 2022, 4:43 PM
 */

#include "LTC_utilities.h"
#include <stdint.h>
#include "../mcc_generated_files/pin_manager.h"
#define FCY 40000000UL // Instruction cycle frequency, Hz - required for __delayXXX() to work
#include <libpic30.h>        // __delayXXX() functions macros defined here

/* Generic wakeup command to wake the LTC681x from sleep state */
void wakeup_sleep(uint8_t total_ic) //Number of ICs in the system
{
    int i = 0;
	for (i =0; i<total_ic; i++)
	{
	   CS_6820_SetLow();
	   __delay_us(300); // Guarantees the LTC681x will be in standby (rather than sleep)
	   CS_6820_SetHigh();
	   __delay_us(10);
	}
}

uint8_t verify_pec(char* data, uint8_t size, char* received_pec)
{
    uint16_t calculated_pec = pec15_calc(data, size);
    uint16_t transmitted_pec = (received_pec[0] << 8) + received_pec[1];
    if(calculated_pec == transmitted_pec)
    {
        LED5_SetHigh();
    }
    else
    {
        LED5_SetLow();
    }
    return 0;
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