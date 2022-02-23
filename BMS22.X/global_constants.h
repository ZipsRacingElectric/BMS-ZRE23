/* 
 * File:   global_constants.h
 * Author: Elena
 *
 * Created on February 21, 2022, 4:52 PM
 */

#ifndef GLOBAL_CONSTANTS_H
#define	GLOBAL_CONSTANTS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define FCY 40000000UL // Instruction cycle frequency, Hz - required for __delayXXX() to work
#include <libpic30.h>        // __delayXXX() functions macros defined here

#ifdef	__cplusplus
}
#endif

#endif	/* GLOBAL_CONSTANTS_H */

