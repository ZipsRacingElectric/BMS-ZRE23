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
    
#define CAN_ID_CELL_VOLTAGES     0x401
#define CAN_ID_PACK_TEMPERATURE  0x41B
#define CAN_ID_STATUS            0x440
#define CAN_ID_CELL_BALANCING    0x419
    
// initialize 2 CAN peripherals, set standby pins low
void can_initialize(void);
// put cell voltages on BMS CAN bus
void report_cell_voltages(uint16_t* cell_voltages);
// put pack temperatures on BMS CAN bus
void report_pack_temperatures(uint16_t* pack_temperatures);
// put status message on main vehicle CAN bus
void report_status(void);

#ifdef	__cplusplus
}
#endif

#endif	/* CAN_DRIVER_H */

