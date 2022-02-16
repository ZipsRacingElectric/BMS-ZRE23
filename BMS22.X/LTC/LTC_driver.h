/* 
 * File:   LTC_driver.h
 * Author: Elena
 *
 * Created on January 28, 2022, 5:35 AM
 */

#ifndef LTC_DRIVER_H
#define	LTC_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

// initialize PEC table necessary for LTC68xx interface
void LTC_initialize(void);
// send commands to get cell voltages
uint8_t read_cell_voltages(uint16_t* cell_voltages);
// send commands to get pack temperatures
uint8_t read_temperatures(uint16_t* pack_temperatures);
// check whether sense line overcurrent protection has tripped
uint8_t open_sense_line_check(void);

#ifdef	__cplusplus
}
#endif

#endif	/* LTC_DRIVER_H */

