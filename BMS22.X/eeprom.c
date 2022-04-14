/* 
 * File: eeprom.c
 * Author: Elena
 *
 * Created on April 10, 2022, 4:29 PM
 */

#include "eeprom.h"
#include "mcc_generated_files/pin_manager.h"
#include <stdbool.h>
#include "mcc_generated_files/spi1.h"
#include "global_constants.h"

#define EEPROM_INITIALIZED_CODE_0     0xDE
#define EEPROM_INITIALIZED_CODE_1     0xAD

uint8_t READ_CMD = 0x03;
uint8_t WRITE_CMD = 0x02;
uint8_t ADDRESS = 0x0A;
uint8_t WRITE_ENABLE = 0x06;
uint8_t READ_STATUS = 0x05;

uint16_t eeprom_soc = 0;
uint8_t eeprom_dummy_buf[4] = {0, 0, 0, 0};

void eeprom_initialize(void)
{
    CS_EEPROM_SetHigh();
    __delay_us(2); // EEPROM chip must see falling edge on CS
    // read from EEPROM
    uint8_t read_bytes[4] = {0, 0, 0, 0};
    CS_EEPROM_SetLow();
    SPI1_Exchange8bitBuffer(&READ_CMD, 1, eeprom_dummy_buf); // send read command
    SPI1_Exchange8bitBuffer(&ADDRESS, 1, eeprom_dummy_buf); // send read address
    SPI1_Exchange8bitBuffer(eeprom_dummy_buf, 4, read_bytes); // read four memory bytes
    CS_EEPROM_SetHigh();
        
    bool is_valid = (read_bytes[2] == EEPROM_INITIALIZED_CODE_0) && (read_bytes[3] == EEPROM_INITIALIZED_CODE_1);
    
    // if not valid, make valid
    if(!is_valid)
    {
        eeprom_soc = 50;
    }
    else
    {
        // if valid, initialize soc var
        eeprom_soc = (read_bytes[1] << 8) | read_bytes[0];
    }

// DBG: uncomment for debugging purposes
//    CS_EEPROM_SetLow();
//    SPI1_Exchange8bit(READ_STATUS);
//    uint8_t status = SPI1_Exchange8bit(eeprom_dummy_buf[0]);
//    CS_EEPROM_SetHigh();
//    __delay_us(1);
}

uint16_t get_state_of_charge_from_eeprom(void)
{
    return eeprom_soc;
}

void write_eeprom(uint16_t write_data)
{
    CS_EEPROM_SetLow();
    SPI1_Exchange8bit(WRITE_ENABLE); // enable writing
    CS_EEPROM_SetHigh();
    __delay_us(1); //TODO is this necessary?
    
    CS_EEPROM_SetLow();
    uint8_t write_bytes[6] = {WRITE_CMD, ADDRESS, (write_data & 0xFF), (write_data >> 8) & 0xFF, EEPROM_INITIALIZED_CODE_0, EEPROM_INITIALIZED_CODE_1};
    SPI1_Exchange8bitBuffer(write_bytes, 6, eeprom_dummy_buf); // write four memory bytes
    CS_EEPROM_SetHigh();
}

