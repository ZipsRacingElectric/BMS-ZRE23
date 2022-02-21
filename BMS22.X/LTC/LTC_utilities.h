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

#define DUMMY               0xFF
#define CMD_SIZE_BYTES      4
#define NUM_ICS             1 //TODO: change to 5
#define CELLS_PER_IC        18
#define NUM_CELLS           (CELLS_PER_IC * NUM_ICS)
#define NUM_TEMP_SENSORS    (9 * NUM_ICS)
#define SUCCESS             0
#define FAILURE             1
    
// cell voltage x 10000 comes from LTC6813 chip
#define CELL_VOLTAGE_MAX        (42000) //TODO: research what these values should be
#define CELL_VOLTAGE_MIN        (30000)
#define CELL_BALANCE_THRESHOLD  200 // 0.02 V threshold for cell balancing TODO change this to like 10 mV
// GPIO voltage x 10000 comes from LTC6813 chip
#define CELL_TEMPERATURE_MAX    18536 // 15 C
#define CELL_TEMPERATURE_MIN    5825  // 60 C
    
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
    
// Generic wakeup command to wake the LTC681x from sleep state
void wakeup_daisychain(void);
// verify that received packet error code matches calculated packet error code
uint8_t verify_pec(char* data, uint8_t size, char* transmitted_pec);
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
uint16_t pec15_calc(char *data , uint8_t len);
#ifdef	__cplusplus
}
#endif

#endif	/* PEC_CALC_H */

