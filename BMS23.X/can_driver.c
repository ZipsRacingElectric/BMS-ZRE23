/*
 * contains functions for interacting with the CAN peripheral
 */
////////////////includes///////////////////////////////////////////////////////
#include "can_driver.h"
#include "LTC/LTC_utilities.h"
#include "soc_fns.h"
#include "fault_handler.h"
#include "mcc_generated_files/can1.h"
#include "mcc_generated_files/can2.h"
#include "mcc_generated_files/can_types.h"
#include "mcc_generated_files/pin_manager.h"
#include "cell_balancing.h"
#include <stdbool.h>
#include "global_constants.h"

////////////////globals////////////////////////////////////////////////////////

////////////////prototypes/////////////////////////////////////////////////////
static void CAN_Msg_Send(uint16_t id, CAN_DLC dlc, uint8_t *tx_data, uint8_t can_port);

////////////////functions//////////////////////////////////////////////////////
// initialize 2 CAN peripherals, set standby pins low
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

// put cell voltages on AUX CAN bus
void report_cell_voltages(uint16_t* cell_voltages)
{
    uint8_t i = 0;
    uint8_t upper_bound = (NUM_CELLS + 3) / 4;
    for(i = 0; i < upper_bound; ++i)
    {
        // Split cell voltage data into LSB and MSB, send 4 values per CAN message.
        // Note: This function seems to go out-of-bounds on the cell_voltages array. For the last message it tries to access past the end of the array.
        uint8_t can_data[8] = {(uint8_t)(cell_voltages[4*i] >> 8), (uint8_t)(cell_voltages[4*i] & 0xFF), 
                               (uint8_t)(cell_voltages[4*i + 1] >> 8), (uint8_t)(cell_voltages[4*i + 1] & 0xFF), 
                               (uint8_t)(cell_voltages[4*i + 2] >> 8), (uint8_t)(cell_voltages[4*i + 2] & 0xFF), 
                               (uint8_t)(cell_voltages[4*i + 3] >> 8), (uint8_t)(cell_voltages[4*i + 3] & 0xFF)};

        CAN_Msg_Send(AUX_CAN_ID_CELL_VOLTAGES + i, CAN_DLC_8, can_data, AUX_CAN);
    }
}

// put sense line status on AUX CAN bus
void report_sense_line_status(uint32_t* sense_line_status)
{
    uint8_t sense_line0[6] = {(sense_line_status[0] & 0xFF), ((sense_line_status[0] >> 8) & 0xFF), ((sense_line_status[0] >> 16) & 0xFF),
                              (sense_line_status[1] & 0xFF), ((sense_line_status[1] >> 8) & 0xFF), ((sense_line_status[1] >> 16) & 0xFF)};
    CAN_Msg_Send(AUX_CAN_ID_SENSE_LINE_STATUS, 6, sense_line0, AUX_CAN);
    
    if(NUM_CELLS > 36)
    {
        uint8_t sense_line1[6] = {(sense_line_status[2] & 0xFF), ((sense_line_status[2] >> 8) & 0xFF), ((sense_line_status[2] >> 16) & 0xFF),
                                  (sense_line_status[3] & 0xFF), ((sense_line_status[3] >> 8) & 0xFF), ((sense_line_status[3] >> 16) & 0xFF)};
        CAN_Msg_Send(AUX_CAN_ID_SENSE_LINE_STATUS + 1, 6, sense_line1, AUX_CAN);
    }
    if(NUM_CELLS > 72)
    {
        uint8_t sense_line2[6] = {(sense_line_status[4] & 0xFF), ((sense_line_status[4] >> 8) & 0xFF), ((sense_line_status[4] >> 16) & 0xFF)};
        CAN_Msg_Send(AUX_CAN_ID_SENSE_LINE_STATUS + 2, 6, sense_line2, AUX_CAN);
    }
}

// put pack temperatures on the AUX CAN bus
void report_pack_temperatures(uint16_t* pack_temperatures)
{
    uint8_t i = 0;
    uint8_t upper_bound = (NUM_TEMP_SENSORS + 3) / 4;
    for(i = 0; i < upper_bound; ++i)
    {
        uint8_t can_data[8] = {(uint8_t)(pack_temperatures[4*i] >> 8), (uint8_t)(pack_temperatures[4*i] & 0xFF), 
                       (uint8_t)(pack_temperatures[4*i + 1] >> 8), (uint8_t)(pack_temperatures[4*i + 1] & 0xFF), 
                       (uint8_t)(pack_temperatures[4*i + 2] >> 8), (uint8_t)(pack_temperatures[4*i + 2] & 0xFF), 
                       (uint8_t)(pack_temperatures[4*i + 3] >> 8), (uint8_t)(pack_temperatures[4*i + 3] & 0xFF)};

        CAN_Msg_Send(AUX_CAN_ID_PACK_TEMPERATURE + i, CAN_DLC_8, can_data, AUX_CAN);
    }
}

// put status message on MAIN CAN bus and AUX CAN bus
void report_status(uint16_t pack_voltage, uint8_t high_temp)
{
    int16_t cs_lo = get_cs_lo_xhundred();
    int16_t cs_hi = get_cs_hi_xten();
    uint16_t soc = get_soc_xten();
    uint8_t fault_codes = get_fault_codes();

    uint8_t can_data[7] = {(uint8_t)(soc & 0xFF), (uint8_t)(soc >> 8),
                           (uint8_t)(cs_hi & 0xFF), (uint8_t)(cs_hi >> 8),
                           (uint8_t)(cs_lo & 0xFF), (uint8_t)(cs_lo >> 8),
                           fault_codes};

    uint8_t soc_byte = soc / 10;
    int8_t current_byte = cs_lo / 100;
    if(current_byte < 0)
    {
        current_byte *= -1;
    }
    uint8_t power_kw = (uint8_t)(pack_voltage * current_byte / 1000);
    uint8_t status_data[8] = { (uint8_t) high_temp,
                               (uint8_t) power_kw,
                               (uint8_t) soc_byte,
                               (uint8_t)(pack_voltage & 0xFF), (uint8_t)(pack_voltage >> 8),
                               (uint8_t) fault_codes,
                               (uint8_t)(cs_lo & 0xFF), (uint8_t)(cs_lo >> 8)};
    
    CAN_Msg_Send(0x100, CAN_DLC_8, status_data, MAIN_CAN);
    CAN_Msg_Send(AUX_CAN_ID_STATUS, CAN_DLC_7, can_data, AUX_CAN);
    CAN_Msg_Send(AUX_CAN_ID_STATUS, CAN_DLC_7, can_data, MAIN_CAN);
}

// put cell balance information on the AUX CAN bus
void report_balancing(uint32_t* cell_needs_balanced)
{
    uint8_t balance_data0[6] = {(cell_needs_balanced[0] & 0xFF), ((cell_needs_balanced[0] >> 8) & 0xFF), ((cell_needs_balanced[0] >> 16) & 0xFF),
                                (cell_needs_balanced[1] & 0xFF), ((cell_needs_balanced[1] >> 8) & 0xFF), ((cell_needs_balanced[1] >> 16) & 0xFF)};
    CAN_Msg_Send(AUX_CAN_ID_CELL_BALANCING, 6, balance_data0, AUX_CAN);
    
    if(NUM_CELLS > 36)
    {
        uint8_t balance_data1[6] = {(cell_needs_balanced[2] & 0xFF), ((cell_needs_balanced[2] >> 8) & 0xFF), ((cell_needs_balanced[2] >> 16) & 0xFF),
                                    (cell_needs_balanced[3] & 0xFF), ((cell_needs_balanced[3] >> 8) & 0xFF), ((cell_needs_balanced[3] >> 16) & 0xFF)};
        CAN_Msg_Send(AUX_CAN_ID_CELL_BALANCING + 1, 6, balance_data1, AUX_CAN);
    }
    if(NUM_CELLS > 72)
    {
        uint8_t balance_data2[6] = {(cell_needs_balanced[4] & 0xFF), ((cell_needs_balanced[4] >> 8) & 0xFF), ((cell_needs_balanced[4] >> 16) & 0xFF)};
        CAN_Msg_Send(AUX_CAN_ID_CELL_BALANCING + 2, 6, balance_data2, AUX_CAN);
    }
}

// put inverter current limits on MAIN CAN bus
void update_current_limits(uint16_t discharge_current_limit, uint16_t charge_current_limit)
{
    // arrange data in little endian format in the data array (LSB first)
    uint16_t can_data[4] = {(uint8_t)(discharge_current_limit & 0xFF), (uint8_t)(discharge_current_limit & 0xFF00), (uint8_t)(charge_current_limit & 0xFF), (uint8_t)(charge_current_limit & 0xFF00)};
    
    CAN_Msg_Send(MAIN_CAN_ID_CURRENT_LIMITS, CAN_DLC_4, can_data, MAIN_CAN);
}

static void CAN_Msg_Send(uint16_t id, CAN_DLC dlc, uint8_t *tx_data, uint8_t can_port)
{
    CAN_MSG_FIELD overhead = {
        .idType = CAN_FRAME_STD,
        .frameType = CAN_FRAME_DATA,
        .dlc = dlc,
    };

    CAN_MSG_OBJ msg = {
        .msgId = id,
        .field = overhead,
        .data = tx_data,
    };

    CAN_TX_MSG_REQUEST_STATUS status;
    if(can_port == MAIN_CAN)
    {
        status = CAN1_Transmit(CAN_PRIORITY_MEDIUM, &msg);
    }
    else
    {
        status = CAN2_Transmit(CAN_PRIORITY_MEDIUM, &msg);
    }
    if(status == CAN_TX_MSG_REQUEST_SUCCESS)
    {
        LED2_CAN_STATUS_SetHigh();
    }
    else if(status == CAN_TX_MSG_REQUEST_BUFFER_FULL)
    {
        LED2_CAN_STATUS_SetLow();
    }
    else
    {
        LED2_CAN_STATUS_SetLow();
    }
    __delay_us(50); // can't send so many CAN messages back to back to back, transmit buffers will overflow
}   
