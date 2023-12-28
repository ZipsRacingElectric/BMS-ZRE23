/* 
 * File:   eeprom.h
 * Author: Elena
 *
 * Created on April 10, 2022, 4:28 PM
 */

#ifndef EEPROM_H
#define	EEPROM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

void eeprom_initialize(void);
uint16_t get_state_of_charge_from_eeprom(void);
void write_eeprom(uint16_t write_data);

#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

