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
    
void balance_timer_initialize(void);
void update_cell_balance_array(uint16_t* cell_voltages);
void set_cell_needs_balancing(uint8_t cell_id);
void reset_cell_needs_balancing(uint8_t cell_id);


#ifdef	__cplusplus
}
#endif

#endif	/* CELL_BALANCING_H */

