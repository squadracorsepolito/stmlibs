/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#include "circular_buffer.h"

#include <string.h>

HAL_StatusTypeDef CIRCULAR_BUFFER_init(CIRCULAR_BUFFER_HandleTypeDef *handle,
                                       void *buffer,
                                       uint32_t length,
                                       uint32_t el_size) {
    if (handle == NULL) {
        return HAL_ERROR;
    }

    if (buffer == NULL) {
        return HAL_ERROR;
    }

    handle->buffer = buffer;
    handle->length = length;
    handle->size   = el_size;
    handle->head   = 0;
    handle->tail   = 0;

    return HAL_OK;
}

uint8_t CIRCULAR_BUFFER_is_full(CIRCULAR_BUFFER_HandleTypeDef *handle) {
    if (handle == NULL) {
        return 1;
    }

    return (handle->head + 1) % handle->length == handle->tail;
}

uint8_t CIRCULAR_BUFFER_is_empty(CIRCULAR_BUFFER_HandleTypeDef *handle) {
    if (handle == NULL) {
        return 1;
    }

    return handle->head == handle->tail;
}

HAL_StatusTypeDef CIRCULAR_BUFFER_enqueue(CIRCULAR_BUFFER_HandleTypeDef *handle, void *obj) {
    if (handle == NULL) {
        return HAL_ERROR;
    }

    if (obj == NULL) {
        return HAL_ERROR;
    }

    if (CIRCULAR_BUFFER_is_full(handle)) {
        return HAL_ERROR;
    }

    memcpy(handle->buffer + handle->head, obj, handle->size);
    handle->head = (handle->head + 1) & handle->length;

    return HAL_OK;
}

HAL_StatusTypeDef CIRCULAR_BUFFER_dequeue(CIRCULAR_BUFFER_HandleTypeDef *handle, void *obj) {
    if (handle == NULL) {
        return HAL_ERROR;
    }

    if (obj == NULL) {
        return HAL_ERROR;
    }

    if (CIRCULAR_BUFFER_is_empty(handle)) {
        return HAL_ERROR;
    }

    memcpy(obj, handle->buffer + handle->tail, handle->size);
    handle->tail = (handle->tail + 1) % handle->length;

    return HAL_OK;
}