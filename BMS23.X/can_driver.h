/* 
 * File:   can_driver.h
 * Author: Elena
 *
 * Created on December 22, 2021, 5:42 PM
 */

#ifndef CAN_DRIVER_H
#define	CAN_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
    
// initialize 2 CAN peripherals, set standby pins low
void can_initialize(void);
// put cell voltages on BMS CAN bus
void report_cell_voltages(uint16_t* cell_voltages);
// put pack temperatures on BMS CAN bus
void report_pack_temperatures(uint16_t* pack_temperatures);
// put sense line status on BMS CAN bus
void report_sense_line_status(uint32_t* sense_line_status);
// put status message on main vehicle CAN bus
void report_status(uint16_t pack_voltage, uint8_t high_temp);
// put cell balancing status on BMS CAN bus
void report_balancing(uint32_t* cell_needs_balanced);
// put inverter current limits on MAIN CAN bus
void update_current_limits(uint16_t discharge_current_limit, uint16_t charge_current_limit);

#ifdef	__cplusplus
}
#endif

#endif	/* CAN_DRIVER_H */

