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

#define DUMMY           0xFF
#define CMD_SIZE_BYTES  4
#define NUM_ICS         2 //TODO: change to 5
#define CELLS_PER_IC    18
#define NUM_CELLS       (CELLS_PER_IC * NUM_ICS)
#define SUCCESS         0
#define FAILURE         1
    
#define CELL_VOLTAGE_MAX    4.2 //TODO: research what these values should be
#define CELL_VOLTAGE_MIN    2.5
    
#define ADCVA       0
#define ADCVB       1
#define ADCVC       2
#define ADCVD       3
#define ADCVE       4
#define ADCVF       5
    
void wakeup_sleep(void);
uint8_t verify_pec(char* data, uint8_t size, char* transmitted_pec);
void init_PEC15_Table();
uint16_t pec15_calc(char *data , uint8_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* PEC_CALC_H */

