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

void can_initialize(void);
void send_status_msg(uint16_t* cell_voltages); //FIXME used for debugging

#ifdef	__cplusplus
}
#endif

#endif	/* CAN_DRIVER_H */

