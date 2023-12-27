/* 
 * File:   current_limiting.c
 * Author: Brian Glen
 * Date: 12/14/23
 * 
 * Contains functions and lookup tables for BMS current limiting
 */

#include <stdint.h>

////////////////constants///////////////////////////////////////////
static const uint16_t cont_discharge_current_limits[20] =
{
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210,
    210
};

static const uint16_t peak_discharge_current_limits[20]=
{
    245,
    245,
    245,
    245,
    245,
    245,
    245,
    245,
    245,
    245,
    242,
    238,
    235,
    231,
    228,
    225,
    222,
    216,
    216,
    213
};

static const uint16_t charge_current_limits[20]=
{
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35,
    35
};

///////////////functions///////////////////////////////////////////////////////
// Get discharge current limit from lookup table
uint16_t get_discharge_current_limit(uint16_t pack_voltage, uint16_t max_pack_voltage)
{
    // get instantaneous current draw of pack
    uint16_t current = 0;
    uint16_t discharge_limit = 0;
    
    // interpolate voltage to get lookup index
    // every index corresponds to 5V, highest index is the max_pack_voltage
    uint8_t i = 0;
    i = 20 - (int)((max_pack_voltage - pack_voltage) / 50);
    
    // out-of-bound check
    if(i < 20)
    {
        // grab current limit
        discharge_limit = peak_discharge_current_limits[i];
        
        // TODO: insert const vs peak current limiting algorithm
        
    } else{
        // handle fault
    }
    
    return discharge_limit;
}

// Get charge current limit from lookup table
uint16_t get_charge_current_limit(uint16_t pack_voltage, uint16_t max_pack_voltage)
{
    // get instantaneous current draw of pack
    uint16_t current = 0;
    uint16_t charge_limit = 0;
    
    // successive approximation to get lookup index
    uint8_t i = 0;
    i = 20 - (int)((max_pack_voltage - pack_voltage) / 50);
    
    // out-of-bound check
    if(i < 20)
    {
        // grab current limit
        charge_limit = charge_current_limits[i];
        
    } else{
        // handle fault
    }
    
    return charge_limit;
}
