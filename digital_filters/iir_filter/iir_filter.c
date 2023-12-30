/**
 * @file      iir_filter.c
 * @prefix    IIR
 * @author    Simone Ruffini [simone.ruffini@squadracorse.com
 *                            simone.ruffini.work@gmail.com]
 * @date      mar 19 dic 2023, 16:54:43, CET
 *
 * @brief     Iinfinite Impulse Response digital-filter source file
 *
 * @note      Refactoring/Overhaul from https://github.com/wooters/miniDSP/blob/master/biquad.c
 * @license   Licensed under "THE BEER-WARE LICENSE", Revision 69 (see LICENSE)
 *
*/

/* Includes ------------------------------------------------------------------*/
#include "iir_filter_gen.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

IIRFILT_DECL_FILTER(uint8_t, u8);
IIRFILT_DECL_FILTER(uint16_t, u16);
IIRFILT_DECL_FILTER(uint32_t, u32);
IIRFILT_DECL_FILTER(float, f);
IIRFILT_DECL_FILTER(double, d);

