/* 
 * File:   soc_fns.h
 * Author: Elena
 *
 * Created on December 21, 2021, 6:52 PM
 */

#ifndef SOC_FNS_H
#define	SOC_FNS_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "mcc_generated_files/can_types.h"
    
void soc_initialize(void);
void calc_soc(void);

uint16_t get_soc_xten(void);
int16_t get_cs_lo_xten(void);
int16_t get_cs_hi_xten(void);

void adc1_cs_lo_interrupt(uint16_t valCS_LO);
void timer1_interrupt(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SOC_FNS_H */

