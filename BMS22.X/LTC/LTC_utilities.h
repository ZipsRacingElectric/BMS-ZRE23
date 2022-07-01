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

#define DUMMY                0xFF
    
#define CMD_SIZE_BYTES       4

// system constants
#define NUM_ICS                  5 //@TODO: change to 5
#define CELLS_PER_REGISTER       3
#define CV_REGISTERS_PER_IC      6
#define CELLS_PER_IC             18
#define NUM_CELLS                (NUM_ICS * CELLS_PER_IC)
#define TEMP_SENSORS_PER_IC      9
#define AUX_REGISTERS_PER_IC     4
#define NUM_TEMP_SENSORS         (NUM_ICS * TEMP_SENSORS_PER_IC)
    
#define SUCCESS              0
#define FAILURE              1
    
// cell voltage x 10000 comes from LTC6813 chip
#define CELL_VOLTAGE_MAX        (42000) //TODO: research what these values should be
#define CELL_VOLTAGE_MIN        (27000)
#define CELL_BALANCE_THRESHOLD  300 // 0.03 V threshold for cell balancing
// GPIO voltage x 10000 comes from LTC6813 chip
#define CELL_TEMPERATURE_MAX    40000 // -5 C
#define CELL_TEMPERATURE_MIN    5825  // 60 C

// TODO maybe this should be an enum
#define ADCVA       0
#define ADCVB       1 
#define ADCVC       2
#define ADCVD       3
#define ADCVE       4
#define ADCVF       5
    
#define AUXA        0
#define AUXB        1
#define AUXC        2
#define AUXD        3
    
#define CV_SELF_TEST_RESULT    0x9555
#define AUX_SELF_TEST_RESULT   0x9555
    
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

