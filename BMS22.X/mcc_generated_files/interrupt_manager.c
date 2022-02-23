/**
  System Interrupts Generated Driver File 

  @Company:
    Microchip Technology Inc.

  @File Name:
    interrupt_manager.h

  @Summary:
    This is the generated driver implementation file for setting up the
    interrupts using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description:
    This source file provides implementations for PIC24 / dsPIC33 / PIC32MM MCUs interrupts.
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  dsPIC33EP128GS804
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB             :  MPLAB X v5.45
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
    Section: Includes
*/
#include <xc.h>

/**
    void INTERRUPT_Initialize (void)
*/
void INTERRUPT_Initialize (void)
{
    //    CI: CAN 2 combined sources
    //    Priority: 1
        IPC14bits.C2IP = 1;
    //    CTXI: CAN 2 Tx Data Request
    //    Priority: 1
        IPC17bits.C2TXIP = 1;
    //    CRXI: CAN 2 Rx Data Ready
    //    Priority: 1
        IPC13bits.C2RXIP = 1;
    //    CI: CAN 1 combined sources
    //    Priority: 1
        IPC8bits.C1IP = 1;
    //    CTXI: CAN 1 Tx Data Request
    //    Priority: 1
        IPC17bits.C1TXIP = 1;
    //    CRXI: CAN 1 Rx Data Ready
    //    Priority: 1
        IPC8bits.C1RXIP = 1;
    //    ADCAN2: ADC AN2 Convert Done
    //    Priority: 1
        IPC28bits.ADCAN2IP = 1;
    //    ADCAN3: ADC AN3 Convert Done
    //    Priority: 1
        IPC28bits.ADCAN3IP = 1;
    //    DMA1I: DMA Channel 1
    //    Priority: 1
        IPC3bits.DMA1IP = 1;
    //    DMA0I: DMA Channel 0
    //    Priority: 1
        IPC1bits.DMA0IP = 1;
    //    DMA3I: DMA Channel 3
    //    Priority: 1
        IPC9bits.DMA3IP = 1;
    //    DMA2I: DMA Channel 2
    //    Priority: 1
        IPC6bits.DMA2IP = 1;
    //    TI: Timer 2
    //    Priority: 1
        IPC1bits.T2IP = 1;
    //    TI: Timer 1
    //    Priority: 1
        IPC0bits.T1IP = 1;
}
