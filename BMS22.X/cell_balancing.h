/* 
 * File:   cell_balancing.h
 * Author: Elena
 *
 * Created on February 17, 2022, 3:57 PM
 */

#ifndef CELL_BALANCING_H
#define	CELL_BALANCING_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
// initialize peripherals necessary for cell balancing
void balance_timer_initialize(void);
// updates cell_needs_balanced array to reflect latest cell voltages
void update_cell_balance_array(uint16_t* cell_voltages);
// returns pointer to array indicating which cells need to be balanced
uint32_t* get_cell_balance_array(void);

#ifdef	__cplusplus
}
#endif

#endif	/* CELL_BALANCING_H */

