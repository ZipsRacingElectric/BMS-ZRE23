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

#define FCY 40000000UL // Instruction cycle frequency, Hz - required for __delayXXX() to work
#include <libpic30.h>        // __delayXXX() functions macros defined here

/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    soc_initialize();
    can_initialize();

    while (1)
    {
        send_status_msg();
        calc_soc();
        
//        int16_t cs_lo = get_cs_lo_xten();
//        int16_t cs_hi = get_cs_hi_xten();
//        uint16_t soc = get_soc_xten();
//        uint8_t cs_data[6] = {cs_lo & 0xFF, cs_lo >> 8, cs_hi & 0xFF, cs_hi >> 8, soc & 0xFF, soc >> 8};
//
//        //send CAN msg
//        CAN_MSG_FIELD overhead = {
//            .idType = CAN_FRAME_STD,
//            .frameType = CAN_FRAME_DATA,
//            .dlc = CAN_DLC_6,
//        };
//
//        CAN_MSG_OBJ cs_msg = {
//            .msgId = 0x100,
//            .field = overhead,
//            .data = cs_data,
//        };
//        CAN_TX_MSG_REQUEST_STATUS status = CAN2_Transmit(CAN_PRIORITY_MEDIUM, &cs_msg);
//        if(status == CAN_TX_MSG_REQUEST_SUCCESS)
//        {
//            LED7_SetHigh();
//        }
//        else
//        {
//            LED7_SetLow();
//        }
        
        LED1_HEARTBEAT_SetHigh();
        __delay_ms(200);
        LED1_HEARTBEAT_SetLow();

        __delay_ms(200);
    }
    return 1; 
}
/**
 End of File
*/

