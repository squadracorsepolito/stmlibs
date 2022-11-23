/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#ifndef LOCK_H
#define LOCK_H

#include "critical_section.h"
#include "main.h"

typedef uint8_t LOCK_TypeDef;

#define LOCK_LOCK(FLAG)      \
    do {                     \
        CS_ENTER();          \
        if (!(FLAG)) {       \
            (FLAG) = 1;      \
            CS_EXIT();       \
        } else {             \
            CS_EXIT()        \
            return HAL_BUSY; \
        }                    \
    } while (0);

#define LOCK_UNLOCK(FLAG) \
    do {                  \
        CS_ENTER();       \
        FLAG = 0;         \
        CS_EXIT();        \
    } while (0);

#endif  //LOCK_H