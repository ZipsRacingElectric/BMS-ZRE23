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
// read configuration register A
uint8_t read_config_reg_a(uint8_t* buffer);
// get latest config buffer values and write config registers
uint8_t update_config_A_and_B(void);
// send commands to get cell voltages
uint8_t read_cell_voltages(uint16_t* cell_voltages);
// send commands to get pack temperatures
uint8_t read_temperatures(uint16_t* pack_temperatures);
// check whether sense line overcurrent protection has tripped
void open_sense_line_check(uint32_t* sense_line_status);
// run BMS self test
void self_test();

#ifdef	__cplusplus
}
#endif

#endif	/* LTC_DRIVER_H */

