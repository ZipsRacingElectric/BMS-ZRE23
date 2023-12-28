/* 
 * File:   LTC_utilities.h
 * Author: Elena
 *
 * Created on January 27, 2022, 4:43 PM
 */

#ifndef PEC_CALC_H
#define	PEC_CALC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
// initialize config register A and B values
void init_cfgr_A_and_B_buffers(void);
// Generic wakeup command to wake the LTC681x from sleep state
void wakeup_daisychain(void);
// verify that received packet error code matches calculated packet error code
uint8_t verify_pec(uint8_t* data, uint8_t size, uint8_t* transmitted_pec);
// function to pull lastest data to be written to cfgra
void refresh_cfgra_write_buffer(void);
// get data to write to cfgra
uint8_t* get_cfgra_write_buffer(uint8_t chip_number);
void set_cfgra_dcc8_1(uint8_t chip_number, uint8_t set_value);
void set_cfgra_dcc12_9(uint8_t chip_number, uint8_t set_value);
// function to pull lastest data to be written to cfgrb
void refresh_cfgrb_write_buffer(void);
// get data to write to cfgrb
uint8_t* get_cfgrb_write_buffer(uint8_t chip_number);
void set_cfgrb_dcc16_13(uint8_t chip_number, uint8_t set_value);
void set_cfgrb_dcc18_17(uint8_t chip_number, uint8_t set_value);

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
void init_PEC15_Table();
// calculated packet error code based on received data and PEC table
uint16_t pec15_calc(uint8_t *data , uint8_t len);
#ifdef	__cplusplus
}
#endif

#endif	/* PEC_CALC_H */

