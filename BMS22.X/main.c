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
#include "LTC/LTC_utilities.h"
#include "LTC/LTC_cmds/LTC_cmds.h"

#define FCY 40000000UL // Instruction cycle frequency, Hz - required for __delayXXX() to work
#include <libpic30.h>        // __delayXXX() functions macros defined here

void wakeup_sleep(uint8_t total_ic);

uint16_t cell_one_v_x_tenthou = 0;
uint16_t cell_two_v_x_tenthou = 0;
uint16_t cell_three_v_x_tenthou = 0;

/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    CS_6820_SetHigh();

    soc_initialize();
    can_initialize();

    init_PEC15_Table();
    
    while (1)
    {
        send_status_msg();
        calc_soc();
        start_cell_voltage_adc_conversion();
        poll_adc_status();
        
        uint8_t buffer_iterator = 0;
        
        uint8_t cmd[4];
        uint8_t dummy_buf[4];
        uint16_t cmd_pec;
      
        __delay_ms(10);
        
        wakeup_sleep(1);
        
        //RDCVA command
        cmd[0] = 0x00;
        cmd[1] = 0x04;
        cmd_pec = pec15_calc(cmd, 2);
        cmd[2] = (uint8_t)(cmd_pec >> 8);
        cmd[3] = (uint8_t)(cmd_pec);
        \
        CS_6820_SetLow();
        for(buffer_iterator = 0; buffer_iterator < 4; ++buffer_iterator)
        {
            dummy_buf[buffer_iterator] = SPI1_Exchange8bit(cmd[buffer_iterator]);
        }

        uint8_t adcva_buf[8];
        for(buffer_iterator = 0; buffer_iterator < 8; ++buffer_iterator)
        {
            adcva_buf[buffer_iterator] = SPI1_Exchange8bit(DUMMY);
        }
        
        uint8_t pec_success = verify_pec(adcva_buf, 6, &adcva_buf[6]);
        
        cell_one_v_x_tenthou = (adcva_buf[1] << 8) + adcva_buf[0];
        cell_two_v_x_tenthou = (adcva_buf[3] << 8) + adcva_buf[2];
        cell_three_v_x_tenthou = (adcva_buf[5] << 8) + adcva_buf[4];

        CS_6820_SetHigh();
        
        LED1_HEARTBEAT_SetHigh();
        __delay_ms(250);
        LED1_HEARTBEAT_SetLow();

        __delay_ms(250);
    }
    return 1; 
}
