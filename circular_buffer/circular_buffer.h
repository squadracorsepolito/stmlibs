/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "main.h"
#include "stmlibs_status.h"

#include <inttypes.h>

struct CIRCULAR_BUFFER_HandleStruct {
    void *buffer;
    uint32_t length;
    uint32_t size;
    uint32_t head;
    uint32_t tail;
};
typedef struct CIRCULAR_BUFFER_HandleStruct CIRCULAR_BUFFER_HandleTypeDef;

STMLIBS_StatusTypeDef CIRCULAR_BUFFER_init(CIRCULAR_BUFFER_HandleTypeDef *handle,
                                           void *buffer,
                                           uint32_t length,
                                           uint32_t el_size);

STMLIBS_StatusTypeDef CIRCULAR_BUFFER_enqueue(CIRCULAR_BUFFER_HandleTypeDef *handle, void *obj);

STMLIBS_StatusTypeDef CIRCULAR_BUFFER_dequeue(CIRCULAR_BUFFER_HandleTypeDef *handle, void *obj);

#endif  //CIRCULAR_BUFFER_H