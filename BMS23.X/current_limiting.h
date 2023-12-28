/* 
 * File:   current_limiting.h
 * Author: Brian Glen
 * Date: 12/14/23
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef CURRENT_LIMITING_H
#define	CURRENT_LIMITING_H


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

const uint16_t cont_discharge_current_limits[20];
const uint16_t peak_discharge_current_limits[20];
const uint16_t charge_current_limits[20];

// Get max discharge current limit
uint16_t get_discharge_current_limit(uint16_t pack_voltage, uint16_t max_pack_voltage);
// Get max charging current limit
uint16_t get_charge_current_limit(uint16_t pack_voltage, uint16_t max_pack_voltage);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

