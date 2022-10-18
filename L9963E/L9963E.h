/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#ifndef L9963E_H
#define L9963E_H

#include "main.h"
#include <inttypes.h>

/*
#include <stdio.h>
#include <inttypes.h>

void init_CRC6_Table()
{
        uint64_t remainder;
        for(uint8_t dividend=0; dividend < 64; ++dividend) {
                uint8_t currByte = dividend;
                for(uint8_t i=0; i<6; ++i){
                        if(currByte & 0x20){
                                currByte <<= 1;
                        } else {
                                currByte <<= 1;
                                currByte ^= 0b0011001;
                        }
                }
                printf("0x%x", currByte & 0x3f);
                if(dividend != 63)
                    printf(", ");
        }
}

void main(void) {
        printf("{");
        init_CRC6_Table();
        printf("}");
        return;
}
*/

typedef enum {
        DEV_GEN_CFG             = 0x01,
        fastch_baluv            = 0x02,
        Bal_1                   = 0x03,
        Bal_2                   = 0x04,
        Bal_3                   = 0x05,
        Bal_4                   = 0x06,
        Bal_5                   = 0x07,
        Bal_6                   = 0x08,
        Bal_7                   = 0x09,
        Bal_8                   = 0x0A,
        VCELL_THRESH_UV_OV      = 0x0B,
        VBATT_SUM_TH            = 0x0C,
        ADCV_CONV               = 0x0D,
        NCYCLE_PROG_1           = 0x0E,
        NCYCLE_PROG_2           = 0x0F,
        BalCell14_7act          = 0x10,
        BalCell6_1act           = 0x11,
        FSM                     = 0x12,
        GPOxOn_and_GPI93        = 0x13,
        GPIO9_3_CONF            = 0x14,
        GPIO3_THR               = 0x15,
        GPIO4_THR               = 0x16,
        GPIO5_THR               = 0x17,
        GPIO6_THR               = 0x18,
        GPIO7_THR               = 0x19,
        GPIO8_THR               = 0x1A,
        GPIO9_THR               = 0x1B,
        VCELLS_EN               = 0x1C,
        Faultmask               = 0x1D,
        Faultmask2              = 0x1E,
        CSA_THRESH_NORM         = 0x1F,
        CSA_GPIO_MSK            = 0x20,
        Vcell1                  = 0x21,
        Vcell2                  = 0x22,
        Vcell3                  = 0x23,
        Vcell4                  = 0x24,
        Vcell5                  = 0x25,
        Vcell6                  = 0x26,
        Vcell7                  = 0x27,
        Vcell8                  = 0x28,
        Vcell9                  = 0x29,
        Vcell10                 = 0x2A,
        Vcell11                 = 0x2B,
        Vcell12                 = 0x2C,
        Vcell13                 = 0x2D,
        Vcell14                 = 0x2E,
        Ibattery_synch          = 0x2F,
        Ibattery_calib          = 0x30,
        CoulCntrTime            = 0x31,
        CoulCntr_msb            = 0x32,
        CoulCntr_lsb            = 0x33,
        GPIO3_MEAS              = 0x34,
        GPIO4_MEAS              = 0x35,
        GPIO5_MEAS              = 0x36,
        GPIO6_MEAS              = 0x37,
        GPIO7_MEAS              = 0x38,
        GPIO8_MEAS              = 0x39,
        GPIO9_MEAS              = 0x3A,
        TempChip                = 0x3B,
        Faults1                 = 0x3C,
        Faults2                 = 0x3D,
        BAL_OPEN                = 0x3E,
        BAL_SHORT               = 0x3F,
        VSUMBATT                = 0x40,
        VBATTDIV                = 0x41,
        CELL_OPEN               = 0x42,
        VCELL_UV                = 0x43,
        VCELL_OV                = 0x44,
        VGPIO_OT_UT             = 0x45,
        VCELL_BAL_UV            = 0x46,
        GPIO_fastchg_OT         = 0x47,
        MUX_BIST_FAIL           = 0x48,
        BIST_COMP               = 0x49,
        OPEN_BIST_FAIL          = 0x4A,
        GPIO_BIST_FAIL          = 0x4B,
        VTREF                   = 0x4C,
        NVM_WR_1                = 0x4D,
        NVM_WR_2                = 0x4E,
        NVM_WR_3                = 0x4F,
        NVM_WR_4                = 0x50,
        NVM_WR_5                = 0x51,
        NVM_WR_6                = 0x52,
        NVM_WR_7                = 0x53,
        NVM_RD_1                = 0x54,
        NVM_RD_2                = 0x55,
        NVM_RD_3                = 0x56,
        NVM_RD_4                = 0x57,
        NVM_RD_5                = 0x58,
        NVM_RD_6                = 0x59,
        NVM_RD_7                = 0x5A,
        NVM_CMD_CNTR            = 0x5B,
        NVM_UNLCK_PRG           = 0x5C
} L9963E_RegistersTypeDef;

HAL_StatusTypeDef L9963E_wakeup(SPI_HandleTypeDef *hspi);
uint8_t L9963E_crc_calc(uint8_t const data[static 5]);

#endif //L9963E_H