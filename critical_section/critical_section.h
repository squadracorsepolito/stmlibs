/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#ifndef CRITICAL_SECTION_H
#define CRITICAL_SECTION_H

#include "cmsis_compiler.h"

#include <stdint.h>

#define CS_ENTER()                       \
    uint32_t rPriMask = __get_PRIMASK(); \
    __set_PRIMASK(1);

#define CS_EXIT() __set_PRIMASK(rPriMask);

#endif  //CRITICAL_SECTION_H