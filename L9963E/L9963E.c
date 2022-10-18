#include "L9963E.h"

uint8_t crc_table[] = { 0x14, 0xd, 0x26, 0x3f, 0x29, 0x30, 0x1b, 0x2,
                        0x37, 0x2e, 0x5, 0x1c, 0xa, 0x13, 0x38, 0x21,
                        0xb, 0x12, 0x39, 0x20, 0x36, 0x2f, 0x4, 0x1d,
                        0x28, 0x31, 0x1a, 0x3, 0x15, 0xc, 0x27, 0x3e,
                        0x2a, 0x33, 0x18, 0x1, 0x17, 0xe, 0x25, 0x3c,
                        0x9, 0x10, 0x3b, 0x22, 0x34, 0x2d, 0x6, 0x1f,
                        0x35, 0x2c, 0x7, 0x1e, 0x8, 0x11, 0x3a, 0x23,
                        0x16, 0xf, 0x24, 0x3d, 0x2b, 0x32, 0x19, 0x0 };

HAL_StatusTypeDef L9963E_wakeup(SPI_HandleTypeDef *hspi) {
    uint8_t dummy[5];
    return HAL_SPI_Transmit(hspi, dummy, 5, 10);
}

uint8_t L9963E_crc_calc(uint8_t const data[static 5]) {
    uint8_t crc = 0b111000;
    uint8_t index;

    volatile uint64_t d = 0;

    for(uint8_t i=0; i<5; ++i) {
        ((uint8_t*)(&d))[i] = data[i];
    }
    d >>= 6;

    for(int8_t shift=30; shift>=0; shift-=6) {
        index = ((d >> shift) & 0x3F) ^ crc;
        crc = crc_table[index];
    }

    return crc;

/*
    // first iteration 
    _6bit |= (data[0] & 0b11000000) >> 6;
    _6bit |= (data[1] & 0b00001111) << 2;

    index = (_6bit ^ crc);
    crc = crc_table[index];
    // end first iteration

    // second iteration 
    _6bit = 0;
    _6bit |= (data[1] & 0b11110000) >> 4;
    _6bit |= (data[2] & 0b00000011) << 4;

    index = (_6bit ^ crc);
    crc = crc_table[index];
    // end second iteration

    // third iteration 
    _6bit = 0;
    _6bit |= (data[2] & 0b11111100) >> 2;

    index = (_6bit ^ crc);
    crc = crc_table[index];
    // end third iteration

    // fourth iteration 
    _6bit = 0;
    _6bit |= (data[3] & 0b00111111);

    index = (_6bit ^ crc);
    crc = crc_table[index];
    // end fourth iteration 

    // fifth iteration 
    _6bit = 0;
    _6bit |= (data[3] & 0b11000000) >> 6;
    _6bit |= (data[4] & 0b00001111) << 2;

    index = (_6bit ^ crc);
    crc = crc_table[index];
    // end fifth iteration 

    // sixth iteration 
    _6bit = 0;
    _6bit |= (data[4] & 0b11110000) >> 4;

    index = (_6bit ^ crc);
    crc = crc_table[index];
    // end sixth iteration 

    return crc;
    */
}