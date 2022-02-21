/* 
 * File:   fault_handler.h
 * Author: Elena
 *
 * Created on February 10, 2022, 6:42 PM
 */

#ifndef FAULT_HANDLER_H
#define	FAULT_HANDLER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
// initialize fault tracking arrays, enable BMS shutdown loop relay
void fault_handler_initialize(void);
uint8_t get_fault_codes(void);
// iterate through fault arrays to determine whether actionable fault has occurred
void check_for_fault(void);

void increment_oor_voltage_fault(uint8_t cell_id);
void reset_oor_voltage_fault(uint8_t cell_id);
void increment_missing_voltage_measurement_fault(uint8_t section_id);
void reset_missing_voltage_measurement_fault(uint8_t section_id);
void increment_temperature_fault(uint8_t temp_sensor_id);
void reset_temperature_fault(uint8_t temp_sensor_id);

#ifdef	__cplusplus
}
#endif

#endif	/* FAULT_HANDLER_H */

