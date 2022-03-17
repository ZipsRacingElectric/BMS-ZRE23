/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  dsPIC33EP128GS804
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB 	          :  MPLAB X v5.45
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"
#include <stdint.h>
#include "mcc_generated_files/pin_manager.h"
#include "can_driver.h"
#include "soc_fns.h"
#include "mcc_generated_files/spi1.h"
#include "LTC/LTC_driver.h"
#include "LTC/LTC_utilities.h"
#include "fault_handler.h"
#include "cell_balancing.h"
#include "global_constants.h"

//TODO why are these globals?
uint16_t cell_voltages[NUM_CELLS];
uint8_t cell_voltage_invalid_counter[6*NUM_ICS];

uint16_t pack_temperatures[NUM_TEMP_SENSORS];
uint8_t pack_temperature_invalid_counter[4*NUM_ICS];

/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    CS_6820_SetHigh();
    
    uint8_t i = 0;
    for(i = 0; i < NUM_CELLS; ++i)
    {
        cell_voltages[i] = 0;
    }
    for(i = 0; i < 6*NUM_ICS; ++i)
    {
        cell_voltage_invalid_counter[i] = 0;
    }
    for(i = 0; i < NUM_TEMP_SENSORS; ++i)
    {
        pack_temperatures[i] = 0;
    }
    for(i = 0; i < 4*NUM_ICS; ++i)
    {
        pack_temperature_invalid_counter[i] = 0;
    }

    soc_initialize();
    can_initialize();
    LTC_initialize();
    fault_handler_initialize();
//    balance_timer_initialize(); TODO turn off when using resistor ladder circuit
    
    while (1)
    {
        // WARN: don't put all the CAN output back to back to back here, 
        //       the transmit buffers will overflow
        calc_soc();
        
        //TODO balance for 20 s, check cell voltages, balance for 20 more s...
        
        read_cell_voltages(cell_voltages, cell_voltage_invalid_counter);
        report_cell_voltages(cell_voltages);
        
        update_cell_balance_array(cell_voltages);
        uint32_t* cell_needs_balanced = get_cell_balance_array();
        report_balancing(cell_needs_balanced);
        update_config_A_and_B();

        read_temperatures(pack_temperatures, pack_temperature_invalid_counter);
        report_pack_temperatures(pack_temperatures);
        
        uint32_t sense_line_status[NUM_ICS];
        open_sense_line_check(sense_line_status);
        report_sense_line_status(sense_line_status);
        
        self_test();
        
        check_for_fault();
        report_status();
        
        LED1_HEARTBEAT_SetHigh();
        __delay_ms(150);
        LED1_HEARTBEAT_SetLow();
        __delay_ms(150);
    }
    return 1; 
}
