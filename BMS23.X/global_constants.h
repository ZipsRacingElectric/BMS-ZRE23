/* 
 * File:   global_constants.h
 * Author: Elena
 *
 * 11/27/23
 */

#ifndef GLOBAL_CONSTANTS_H
#define	GLOBAL_CONSTANTS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    
//---------------- General ----------------------------------------------------
#define FCY                           40000000UL // Instruction cycle frequency, Hz - required for __delayXXX() to work
#include <libpic30.h>        // __delayXXX() functions macros defined here. Must be included after FCY definition.
    
//---------------- CANbus -----------------------------------------------------
#define MAIN_CAN                      1
#define AUX_CAN                       2
    
//---------------- Main CANBus IDs --------------------------------------------

// ID for reporting general AMS status to the dashboard.
#define MAIN_CAN_ID_AMS_STATUS        0x100
    
// ID for reporting current limit data to the inverter. The inverter requires a specific ID that cannot be changed.
#define MAIN_CAN_ID_CURRENT_LIMITS    0x202

// ID for reporting more detailed SoC and current information.
#define MAIN_CAN_ID_DETAILED_STATUS   0x440

//---------------- AUX CANBus IDs ---------------------------------------------

// ID for cell voltage reporting. CAN IDs starting at AUX_CAN_ID_CELL_VOLTAGES to [AUX_CAN_ID_CELL_VOLTAGES + ((NUM_CELLS + 3) / 4) - 1] are used.
#define AUX_CAN_ID_CELL_VOLTAGES      0x401

// ID for reporting cell balancing information. CAN IDs starting at AUX_CAN_ID_CELL_BALANCING to [AUX_CAN_ID_CELL_BALANCING + 2] are used.
#define AUX_CAN_ID_CELL_BALANCING     0x418    
 
// ID for cell temperature reporting. CAN IDs starting at AUX_CAN_ID_PACK_TEMPERATURE to [AUX_CAN_ID_PACK_TEMPERATURE + ((NUM_CELLS + 3) / 4) - 1] are used.
#define AUX_CAN_ID_PACK_TEMPERATURE   0x41B
    
// ID for open sense line reporting. CAN IDs starting at AUX_CAN_ID_SENSE_LINE to [AUX_CAN_ID_SENSE_LINE + 2] are used.
#define AUX_CAN_ID_SENSE_LINE_STATUS  0x43D
    
// ID for reporting more detailed SoC and current information
#define AUX_CAN_ID_STATUS             0x440

//---------------- LTC Utilities -------------------------------------------------
// ??
#define DUMMY                         0xFF
    
// ?? (bms command size in bytes?)
#define CMD_SIZE_BYTES                4
    
// Set the number of BMS chips in the isoSPI bus
#define NUM_ICS                       5

// Number of cells stored in each BMS chip register (can this be made dynamic)
#define CELLS_PER_REGISTER            3

// Number of registers in each BMS chip register (can this be made dynamic)
#define CV_REGISTERS_PER_IC           6
    
// Number of cells read per IC (this should be made dynamic)
#define CELLS_PER_IC                  18
    
// Total number of series battery cells
#define NUM_CELLS                     (NUM_ICS * CELLS_PER_IC)

// Number of temp sensors connected to each BMS chip
#define TEMP_SENSORS_PER_IC           9
 
 // Number of Auxillary registers in each BMS chip (can this be made dynamic)
#define AUX_REGISTERS_PER_IC          4
    
// Total number of temperature sensors in the battery
#define NUM_TEMP_SENSORS              (NUM_ICS * TEMP_SENSORS_PER_IC)

// ?? (fault checking maybe)
#define SUCCESS                       0
#define FAILURE                       1
    
// Cell voltage limitations, set as voltage * 10000
#define CELL_VOLTAGE_MAX              41500 // 4.15V 
#define CELL_VOLTAGE_MIN              28000 // 2.8V 

// The minimum voltage threshold to initiate balancing
#define CELL_BALANCE_THRESHOLD        200 // 0.02 V
    
// GPIO voltage x 10000 comes from LTC6813 chip
#define CELL_TEMPERATURE_MAX          40000 // -5 C
#define CELL_TEMPERATURE_MIN          5825  // 60 C

// TODO maybe this should be an enum
#define ADCVA                         0
#define ADCVB                         1 
#define ADCVC                         2
#define ADCVD                         3
#define ADCVE                         4
#define ADCVF                         5
    
#define AUXA                          0
#define AUXB                          1
#define AUXC                          2
#define AUXD                          3
    
#define CV_SELF_TEST_RESULT           0x9555
#define AUX_SELF_TEST_RESULT          0x9555
    
#ifdef	__cplusplus
}
#endif

#endif	/* GLOBAL_CONSTANTS_H */

