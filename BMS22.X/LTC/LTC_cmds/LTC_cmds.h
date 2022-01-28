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
void poll_adc_status(void);
void rdcva_register(void);

#ifdef	__cplusplus
}
#endif

#endif	/* LTC_CMDS_H */

