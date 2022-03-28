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
// returns fault code byte which is transmitted on the CAN bus
uint8_t get_fault_codes(void);
// iterate through fault arrays to determine whether actionable fault has occurred
void check_for_fault(void);

// out-of-range voltage fault functions
void increment_outofrange_voltage_fault(uint8_t cell_id);
void reset_outofrange_voltage_fault(uint8_t cell_id);
// missing/invalid PEC when cell voltages are requested
void increment_missing_voltage_measurement_fault(uint8_t section_id);
void reset_missing_voltage_measurement_fault(uint8_t section_id);
uint8_t get_missing_voltage_measurement_fault(uint8_t section_id);
// out-of-range temperature functions
void increment_outofrange_temperature_fault(uint8_t temp_sensor_id);
void reset_outofrange_temperature_fault(uint8_t temp_sensor_id);
// missing temperature measurement functions
void increment_missing_temperature_fault(uint8_t section_id);
void reset_missing_temperature_fault(uint8_t section_id);
uint8_t get_missing_temperature_fault(uint8_t section_id);
// open sense line fault functions
void increment_sense_line_fault(uint8_t cell_id);
void reset_sense_line_fault(uint8_t cell_id);
// self test fault functions
void increment_cv_self_test_fault(uint8_t section_id);
void reset_cv_self_test_fault(uint8_t section_id);
void increment_aux_self_test_fault(uint8_t section_id);
void reset_aux_self_test_fault(uint8_t section_id);


#ifdef	__cplusplus
}
#endif

#endif	/* FAULT_HANDLER_H */

