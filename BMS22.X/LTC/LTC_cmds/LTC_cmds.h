/* 
 * File:   6813_cmds.h
 * Author: Elena
 *
 * Created on January 27, 2022, 4:55 PM
 */

#ifndef LTC_CMDS_H
#define	LTC_CMDS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
void start_cell_voltage_adc_conversion(void);
void start_temperature_adc_conversion(void);
void poll_adc_status(void);
void rdcv_register(uint8_t which_reg, uint16_t* buf);
void rdaux_register(uint8_t which_reg, uint16_t* buf);
void open_wire_check(uint8_t pull_dir);
void rdcfga(uint8_t* buffer);
void wrcfga(uint8_t* data_to_write);

#ifdef	__cplusplus
}
#endif

#endif	/* LTC_CMDS_H */

