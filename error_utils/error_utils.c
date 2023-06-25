/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */
#include "error_utils.h"

#include "critical_section.h"
#include "timer_utils.h"

STMLIBS_StatusTypeDef ERROR_UTILS_init(ERROR_UTILS_HandleTypeDef *handle,
                                       TIM_HandleTypeDef *htim,
                                       ERROR_UTILS_ConfigTypeDef *config,
                                       ERROR_UTILS_CallbackTypeDef global_toggle_callback,
                                       ERROR_UTILS_CallbackTypeDef global_expiry_callback) {
    if (handle == NULL) {
        return STMLIBS_ERROR;
    }

    if (htim == NULL || !IS_TIM_INSTANCE(htim->Instance)) {
        return STMLIBS_ERROR;
    }

    if (config == NULL) {
        return STMLIBS_ERROR;
    }

    handle->htim                           = htim;
    handle->first_to_expire_error_index    = config->errors_length;
    handle->first_to_expire_instance_index = 0;
    handle->global_toggle_callback         = global_toggle_callback;
    handle->global_expiry_callback         = global_expiry_callback;
    handle->config                         = config;

    for (uint32_t i = 0; i < handle->config->errors_length; ++i) {
        if (TIM_MS_TO_TICKS(htim, handle->config->errors_array[i].expiry_delay_ms) - 1 > TIM_GET_MAX_AUTORELOAD(htim)) {
            return STMLIBS_ERROR;
        }
        for (uint32_t j = 0; j < handle->config->errors_array[i].instances_length; ++j) {
            handle->config->errors_array[i].instances[j].is_triggered       = 0;
            handle->config->errors_array[i].instances[j].expected_expiry_ms = 0;
        }
    }

    return STMLIBS_OK;
}

uint8_t _ERROR_UTILS_is_before(uint32_t expiry_1, uint32_t expiry_2) {
    uint64_t e1 = expiry_1, e2 = expiry_2;
    if (expiry_1 < HAL_GetTick()) {
        e1 += UINT32_MAX;
    }

    if (expiry_2 < HAL_GetTick()) {
        e2 += UINT32_MAX;
    }

    return e1 < e2;
}

uint32_t _ERROR_UTILS_get_delta_from_now(uint32_t expiry) {
    uint64_t e = expiry;
    if (expiry < HAL_GetTick()) {
        e += UINT32_MAX;
    }

    return e - HAL_GetTick();
}

STMLIBS_StatusTypeDef _ERROR_UTILS_set_timer(ERROR_UTILS_HandleTypeDef *handle, uint32_t expiry) {
    if (handle == NULL) {
        return STMLIBS_ERROR;
    }
    uint32_t ticks = TIM_MS_TO_TICKS(handle->htim, _ERROR_UTILS_get_delta_from_now(expiry)) - 1;

    if (ticks > TIM_GET_MAX_AUTORELOAD(handle->htim)) {
        return STMLIBS_ERROR;
    }

    __HAL_TIM_SetCounter(handle->htim, 0);
    __HAL_TIM_SetAutoreload(handle->htim, ticks);
    __HAL_TIM_CLEAR_IT(handle->htim, TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(handle->htim);

    return STMLIBS_OK;
}

STMLIBS_StatusTypeDef ERROR_UTILS_error_set(ERROR_UTILS_HandleTypeDef *handle,
                                            uint32_t error_index,
                                            uint32_t instance_index) {
    STMLIBS_StatusTypeDef errorcode = STMLIBS_OK;
    //enter critical section
    CS_ENTER()

    if (handle == NULL) {
        errorcode = STMLIBS_ERROR;
        goto exit;
    }

    if (error_index >= handle->config->errors_length) {
        errorcode = STMLIBS_ERROR;
        goto exit;
    }

    if (instance_index >= handle->config->errors_array[error_index].instances_length) {
        errorcode = STMLIBS_ERROR;
        goto exit;
    }

    ERROR_UTILS_ErrorInstanceTypeDef *instance = &(handle->config->errors_array[error_index].instances[instance_index]);
    ERROR_UTILS_ErrorInstanceTypeDef *first_to_expire_inst = NULL;

    if (handle->first_to_expire_error_index < handle->config->errors_length) {
        first_to_expire_inst = &(handle->config->errors_array[handle->first_to_expire_error_index]
                                     .instances[handle->first_to_expire_instance_index]);
    }

    if (!instance->is_triggered) {
        instance->expected_expiry_ms = HAL_GetTick() + handle->config->errors_array[error_index].expiry_delay_ms;

        if (first_to_expire_inst == NULL || !first_to_expire_inst->is_triggered ||
            _ERROR_UTILS_is_before(instance->expected_expiry_ms, first_to_expire_inst->expected_expiry_ms)) {
            if (_ERROR_UTILS_set_timer(handle, instance->expected_expiry_ms) != STMLIBS_OK) {
                errorcode = STMLIBS_ERROR;
                goto exit;
            }

            handle->first_to_expire_error_index    = error_index;
            handle->first_to_expire_instance_index = instance_index;
        }

        instance->is_triggered = 1;
        ++handle->count;

        if (handle->config->errors_array[error_index].toggle_callback != NULL) {
            handle->config->errors_array[error_index].toggle_callback(error_index, instance_index);
        }

        if (handle->global_toggle_callback != NULL) {
            handle->global_toggle_callback(error_index, instance_index);
        }
    }

exit:
    CS_EXIT();
    return errorcode;
}

STMLIBS_StatusTypeDef _ERROR_UTILS_find_first_expiring_and_set_timer(ERROR_UTILS_HandleTypeDef *handle) {
    if (handle == NULL) {
        return STMLIBS_ERROR;
    }

    uint32_t min_error_index    = handle->config->errors_length;
    uint32_t min_instance_index = 0;
    uint32_t min_expiry         = UINT32_MAX;
    uint32_t error_count        = handle->count;

    for (uint32_t i = 0; i < handle->config->errors_length && error_count > 0; ++i) {
        for (uint32_t j = 0; j < handle->config->errors_array[i].instances_length && error_count > 0; ++j) {
            ERROR_UTILS_ErrorInstanceTypeDef *test_inst = &(handle->config->errors_array[i].instances[j]);
            if (test_inst->is_triggered) {
                if (_ERROR_UTILS_is_before(test_inst->expected_expiry_ms, min_expiry)) {
                    min_error_index    = i;
                    min_instance_index = j;
                    min_expiry         = test_inst->expected_expiry_ms;
                }
                --error_count;
            }
        }
    }

    if (min_error_index != handle->config->errors_length) {
        if (_ERROR_UTILS_set_timer(handle, min_expiry) != STMLIBS_OK) {
            return STMLIBS_ERROR;
        }

        handle->first_to_expire_error_index    = min_error_index;
        handle->first_to_expire_instance_index = min_instance_index;
    } else {
        handle->first_to_expire_error_index = handle->config->errors_length;
        HAL_TIM_Base_Stop_IT(handle->htim);
    }

    return STMLIBS_OK;
}

STMLIBS_StatusTypeDef ERROR_UTILS_error_reset(ERROR_UTILS_HandleTypeDef *handle,
                                              uint32_t error_index,
                                              uint32_t instance_index) {
    STMLIBS_StatusTypeDef errorcode = STMLIBS_OK;
    //enter critical section
    CS_ENTER();

    if (handle == NULL) {
        errorcode = STMLIBS_ERROR;
        goto exit;
    }

    if (error_index >= handle->config->errors_length) {
        errorcode = STMLIBS_ERROR;
        goto exit;
    }

    if (instance_index >= handle->config->errors_array[error_index].instances_length) {
        errorcode = STMLIBS_ERROR;
        goto exit;
    }

    ERROR_UTILS_ErrorInstanceTypeDef *instance = &(handle->config->errors_array[error_index].instances[instance_index]);

    if (instance->is_triggered) {
        instance->is_triggered = 0;
        --handle->count;

        if (_ERROR_UTILS_find_first_expiring_and_set_timer(handle) != STMLIBS_OK) {
            errorcode = STMLIBS_ERROR;
            goto exit;
        }

        if (handle->config->errors_array[error_index].toggle_callback != NULL) {
            handle->config->errors_array[error_index].toggle_callback(error_index, instance_index);
        }

        if (handle->global_toggle_callback != NULL) {
            handle->global_toggle_callback(error_index, instance_index);
        }
    }

exit:
    CS_EXIT();
    return errorcode;
}
uint8_t ERROR_UTILS_is_set(ERROR_UTILS_HandleTypeDef *handle, uint32_t error_index, uint32_t instance_index) {
    if (handle == NULL) {
        return 0;
    }

    if (error_index >= handle->config->errors_length) {
        return 0;
    }

    if (instance_index >= handle->config->errors_array[error_index].instances_length) {
        return 0;
    }

    return handle->config->errors_array[error_index].instances[instance_index].is_triggered;
}

uint32_t ERROR_UTILS_get_count(ERROR_UTILS_HandleTypeDef *handle) {
    if (handle == NULL) {
        return 0;
    }

    return handle->count;
}

STMLIBS_StatusTypeDef ERROR_UTILS_TimerElapsedCallback(ERROR_UTILS_HandleTypeDef *handle, TIM_HandleTypeDef *htim) {
    if (handle == NULL) {
        return STMLIBS_ERROR;
    }

    if (htim == NULL) {
        return STMLIBS_ERROR;
    }

    if (handle->htim == htim) {
        uint32_t error_index    = handle->first_to_expire_error_index;
        uint32_t instance_index = handle->first_to_expire_instance_index;

        if (error_index < handle->config->errors_length &&
            handle->config->errors_array[error_index].instances[instance_index].is_triggered) {
            handle->config->errors_array[error_index].instances[instance_index].is_triggered = 0;

            if (_ERROR_UTILS_find_first_expiring_and_set_timer(handle) != STMLIBS_OK) {
                return STMLIBS_ERROR;
            }

            if (handle->config->errors_array[error_index].expiry_callback != NULL) {
                handle->config->errors_array[error_index].expiry_callback(error_index, instance_index);
            }

            if (handle->global_expiry_callback != NULL) {
                handle->global_expiry_callback(error_index, instance_index);
            }
        }
    }

    return STMLIBS_OK;
}