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
    
void fault_handler_initialize(void);
void increment_cell_voltage_fault(uint8_t cell_id);
void reset_cell_voltage_fault(uint8_t cell_id);
uint8_t get_fault_codes(void);
void check_for_fault(void);

#ifdef	__cplusplus
}
#endif

#endif	/* FAULT_HANDLER_H */

