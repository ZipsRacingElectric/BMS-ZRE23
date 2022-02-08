/*
 * contains functions for interacting with the CAN peripheral
 */
////////////////includes///////////////////////////////////////////////////////
#include "can_driver.h"
#include "mcc_generated_files/can1.h"
#include "mcc_generated_files/can2.h"
#include "mcc_generated_files/can_types.h"
#include "mcc_generated_files/pin_manager.h"
////////////////defines////////////////////////////////////////////////////////

////////////////globals////////////////////////////////////////////////////////

////////////////functions//////////////////////////////////////////////////////
void can_initialize(void)
{
    CAN1_TransmitEnable();
    CAN1_ReceiveEnable();
    CAN1_OperationModeSet(CAN_NORMAL_OPERATION_MODE);
    
    CAN2_TransmitEnable();
    CAN2_ReceiveEnable();
    CAN2_OperationModeSet(CAN_NORMAL_OPERATION_MODE);
    
    CAN1_STDBY_SetLow();
    CAN2_STDBY_SetLow();
}

void send_status_msg(uint16_t* cell_voltages) //FIXME used for debugging
{
    uint8_t can_data[8] = {(uint8_t)(cell_voltages[0] >> 8), (uint8_t)(cell_voltages[0] & 0xFF), (uint8_t)(cell_voltages[1] >> 8), (uint8_t)(cell_voltages[1] & 0xFF), (uint8_t)(cell_voltages[2] >> 8), (uint8_t)(cell_voltages[2] & 0xFF), (uint8_t)(cell_voltages[3] >> 8), (uint8_t)(cell_voltages[3] & 0xFF)};

    CAN_MSG_FIELD overhead = {
        .idType = CAN_FRAME_STD,
        .frameType = CAN_FRAME_DATA,
        .dlc = CAN_DLC_8,
    };

    CAN_MSG_OBJ msg = {
        .msgId = 0x200,
        .field = overhead,
        .data = can_data,
    };

    CAN_TX_MSG_REQUEST_STATUS status = CAN1_Transmit(CAN_PRIORITY_MEDIUM, &msg);
    if(status == CAN_TX_MSG_REQUEST_SUCCESS)
    {
        LED2_CAN_STATUS_SetHigh();
    }
    else
    {
        LED2_CAN_STATUS_SetLow();
    }
}

//add functions here to interact with CAN peripheral from other tasks
