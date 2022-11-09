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

#include "timer_utils.h"

HAL_StatusTypeDef ERROR_UTILS_init(
    ERROR_UTILS_HandleTypeDef *handle,
    TIM_HandleTypeDef *htim,
    ERROR_UTILS_ConfigTypeDef *config,
    ERROR_UTILS_CallbackTypeDef global_toggle_callback,
    ERROR_UTILS_CallbackTypeDef global_expiry_callback) {
    if (handle == NULL) {
        return HAL_ERROR;
    }

    if (htim == NULL || !IS_TIM_INSTANCE(htim->Instance)) {
        return HAL_ERROR;
    }

    if (config == NULL) {
        return HAL_ERROR;
    }

    for (uint32_t i = 0; i < handle->config->errors_length; ++i) {
        if (TIM_MS_TO_TICKS(htim, handle->config->errors_array[i].expiry_delay_ms) > TIM_GET_MAX_AUTORELOAD(htim)) {
            return HAL_ERROR;
        }
        for (uint32_t j = 0; j < handle->config->errors_array[i].instances_length; ++j) {
            handle->config->errors_array[i].instances[j].is_triggered       = 0;
            handle->config->errors_array[i].instances[j].expected_expiry_ms = 0;
        }
    }

    handle->htim                           = htim;
    handle->first_to_expire_error_index    = 0;
    handle->first_to_expire_instance_index = 0;
    handle->global_toggle_callback         = global_toggle_callback;
    handle->global_expiry_callback         = global_expiry_callback;
    handle->config                         = config;

    return HAL_OK;
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

HAL_StatusTypeDef _ERROR_UTILS_set_timer(ERROR_UTILS_HandleTypeDef *handle, uint32_t expiry) {
    if (handle == NULL) {
        return HAL_ERROR;
    }
    uint32_t ticks = TIM_MS_TO_TICKS(handle->htim, _ERROR_UTILS_get_delta_from_now(expiry));

    if (ticks > TIM_GET_MAX_AUTORELOAD(handle->htim)) {
        return HAL_ERROR;
    }

    __HAL_TIM_SetCounter(handle->htim, 0);
    __HAL_TIM_SetAutoreload(handle->htim, ticks);
    return HAL_TIM_Base_Start_IT(handle->htim);
}

HAL_StatusTypeDef ERROR_UTILS_error_set(
    ERROR_UTILS_HandleTypeDef *handle,
    uint32_t error_index,
    uint32_t instance_index) {
    if (handle == NULL) {
        return HAL_ERROR;
    }

    if (error_index >= handle->config->errors_length) {
        return HAL_ERROR;
    }

    if (instance_index >= handle->config->errors_array[error_index].instances_length) {
        return HAL_ERROR;
    }

    ERROR_UTILS_ErrorInstanceTypeDef *instance = &(handle->config->errors_array[error_index].instances[instance_index]);
    ERROR_UTILS_ErrorInstanceTypeDef *first_to_expire_inst =
        &(handle->config->errors_array[handle->first_to_expire_error_index]
              .instances[handle->first_to_expire_instance_index]);

    if (!instance->is_triggered) {
        instance->is_triggered       = 1;
        instance->expected_expiry_ms = HAL_GetTick() + handle->config->errors_array[error_index].expiry_delay_ms;

        if (!first_to_expire_inst->is_triggered ||
            _ERROR_UTILS_is_before(instance->expected_expiry_ms, first_to_expire_inst->expected_expiry_ms)) {
            _ERROR_UTILS_set_timer(handle, instance->expected_expiry_ms);

            handle->first_to_expire_error_index    = error_index;
            handle->first_to_expire_instance_index = instance_index;
        }

        if (handle->config->errors_array[error_index].toggle_callback != NULL) {
            handle->config->errors_array[error_index].toggle_callback(error_index, instance_index);
        }

        if (handle->global_toggle_callback != NULL) {
            handle->global_toggle_callback(error_index, instance_index);
        }
    }

    return HAL_OK;
}

HAL_StatusTypeDef _ERROR_UTILS_find_first_expiring_and_set_timer(ERROR_UTILS_HandleTypeDef *handle) {
    if (handle == NULL) {
        return HAL_ERROR;
    }

    uint32_t min_error_index    = handle->config->errors_length;
    uint32_t min_instance_index = 0;
    uint32_t min_expiry         = UINT32_MAX;

    for (uint32_t i = 0; i < handle->config->errors_length; ++i) {
        for (uint32_t j = 0; j < handle->config->errors_array[i].instances_length; ++j) {
            ERROR_UTILS_ErrorInstanceTypeDef *test_inst = &(handle->config->errors_array[i].instances[j]);
            if (test_inst->is_triggered && _ERROR_UTILS_is_before(test_inst->expected_expiry_ms, min_expiry)) {
                min_error_index    = i;
                min_instance_index = j;
                min_expiry         = test_inst->expected_expiry_ms;
            }
        }
    }

    if (min_error_index != handle->config->errors_length) {
        _ERROR_UTILS_set_timer(handle, min_expiry);

        handle->first_to_expire_error_index    = min_error_index;
        handle->first_to_expire_instance_index = min_instance_index;
    } else {
        HAL_TIM_Base_Stop_IT(handle->htim);
    }

    return HAL_OK;
}

HAL_StatusTypeDef ERROR_UTILS_error_reset(
    ERROR_UTILS_HandleTypeDef *handle,
    uint32_t error_index,
    uint32_t instance_index) {
    if (handle == NULL) {
        return HAL_ERROR;
    }

    if (error_index >= handle->config->errors_length) {
        return HAL_ERROR;
    }

    if (instance_index >= handle->config->errors_array[error_index].instances_length) {
        return HAL_ERROR;
    }

    ERROR_UTILS_ErrorInstanceTypeDef *instance = &(handle->config->errors_array[error_index].instances[instance_index]);

    if (instance->is_triggered) {
        instance->is_triggered = 0;

        _ERROR_UTILS_find_first_expiring_and_set_timer(handle);

        if (handle->config->errors_array[error_index].toggle_callback != NULL) {
            handle->config->errors_array[error_index].toggle_callback(error_index, instance_index);
        }

        if (handle->global_toggle_callback != NULL) {
            handle->global_toggle_callback(error_index, instance_index);
        }
    }

    return HAL_OK;
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

HAL_StatusTypeDef ERROR_UTILS_TimerElapsedCallback(ERROR_UTILS_HandleTypeDef *handle, TIM_HandleTypeDef *htim) {
    if (handle == NULL) {
        return HAL_ERROR;
    }

    if (htim == NULL) {
        return HAL_ERROR;
    }

    if (handle->htim == htim) {
        uint32_t error_index    = handle->first_to_expire_error_index;
        uint32_t instance_index = handle->first_to_expire_instance_index;

        _ERROR_UTILS_find_first_expiring_and_set_timer(handle);

        if (handle->config->errors_array[error_index].toggle_callback != NULL) {
            handle->config->errors_array[error_index].toggle_callback(error_index, instance_index);
        }

        if (handle->global_toggle_callback != NULL) {
            handle->global_toggle_callback(error_index, instance_index);
        }
    }

    return HAL_OK;
}