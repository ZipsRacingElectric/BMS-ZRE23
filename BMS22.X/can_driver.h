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
#define CAN_ID_STATUS            0x440

void can_initialize(void);
void report_cell_voltages(uint16_t* cell_voltages);
void report_status(void);

#ifdef	__cplusplus
}
#endif

#endif	/* CAN_DRIVER_H */

