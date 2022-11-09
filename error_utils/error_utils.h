/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */
#ifndef ERROR_UTILS_H
#define ERROR_UTILS_H

#include "main.h"
#include "stdint.h"

typedef void (*ERROR_UTILS_CallbackTypeDef)(uint8_t error_index, uint8_t instance_index);

struct ERROR_UTILS_ErrorInstanceStruct {
    uint8_t is_triggered;
    uint32_t expected_expiry_ms;
};
typedef struct ERROR_UTILS_ErrorInstanceStruct ERROR_UTILS_ErrorInstanceTypeDef;

struct ERROR_UTILS_ErrorStruct {
    uint32_t expiry_delay_ms;
    uint32_t instances_length;
    ERROR_UTILS_ErrorInstanceTypeDef *instances;
    ERROR_UTILS_CallbackTypeDef toggle_callback;
    ERROR_UTILS_CallbackTypeDef expiry_callback;
};
typedef struct ERROR_UTILS_ErrorStruct ERROR_UTILS_ErrorTypeDef;

struct ERROR_UTILS_ConfigStruct {
    uint32_t errors_length;
    ERROR_UTILS_ErrorTypeDef *errors_array;
};
typedef struct ERROR_UTILS_ConfigStruct ERROR_UTILS_ConfigTypeDef;

struct ERROR_UTILS_HandleStruct {
    TIM_HandleTypeDef *htim;
    uint32_t first_to_expire_error_index;
    uint32_t first_to_expire_instance_index;
    ERROR_UTILS_CallbackTypeDef global_toggle_callback;
    ERROR_UTILS_CallbackTypeDef global_expiry_callback;
    ERROR_UTILS_ConfigTypeDef *config;
};
typedef struct ERROR_UTILS_HandleStruct ERROR_UTILS_HandleTypeDef;

HAL_StatusTypeDef ERROR_UTILS_init(
    ERROR_UTILS_HandleTypeDef *handle,
    TIM_HandleTypeDef *htim,
    ERROR_UTILS_ConfigTypeDef *config,
    ERROR_UTILS_CallbackTypeDef global_toggle_callback,
    ERROR_UTILS_CallbackTypeDef global_expiry_callback);

HAL_StatusTypeDef ERROR_UTILS_error_set(
    ERROR_UTILS_HandleTypeDef *handle,
    uint32_t error_index,
    uint32_t instance_index);

HAL_StatusTypeDef ERROR_UTILS_error_reset(
    ERROR_UTILS_HandleTypeDef *handle,
    uint32_t error_index,
    uint32_t instance_index);

uint8_t ERROR_UTILS_is_set(ERROR_UTILS_HandleTypeDef *handle, uint32_t error_index, uint32_t instance_index);

HAL_StatusTypeDef ERROR_UTILS_TimerElapsedCallback(ERROR_UTILS_HandleTypeDef *handle, TIM_HandleTypeDef *htim);

#endif  //ERROR_UTILS_H
