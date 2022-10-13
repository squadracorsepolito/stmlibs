#include "timebase.h"
#include "timer_utils.h"

HAL_StatusTypeDef TIMEBASE_init(TIMEBASE_HandleTypeDef *handle, TIM_HandleTypeDef *htim, uint32_t base_interval_us) {
    handle->htim = htim;
    handle->base_interval_us = base_interval_us;

    uint32_t ticks = TIM_MS_TO_TICKS(handle->htim, base_interval_us/1000.0);
    if(ticks > __HAL_TIM_GetAutoreload(handle->htim)) {
        return HAL_ERROR;
    }

    __HAL_TIM_SetAutoreload(handle->htim, ticks);

    handle->repetition_counter = 0;
    handle->intervals_lenght = 0;
    handle->intervals_flag = 0;

    return HAL_OK;
}

HAL_StatusTypeDef TIMEBASE_add_interval(TIMEBASE_HandleTypeDef *handle, uint32_t interval_us, uint8_t *interval_index) {
    if(interval_us % handle->base_interval_us != 0) {
        return HAL_ERROR;
    }

    if(handle->intervals_lenght == TIMEBASE_MAX_INTERVALS) {
        return HAL_ERROR;
    }

    if(handle->intervals_lenght == 0) {
        __HAL_TIM_CLEAR_IT(handle->htim, TIM_IT_UPDATE);
        HAL_TIM_Base_Start_IT(handle->htim);
    }

    handle->intervals[handle->intervals_lenght].interval_us = interval_us;
    handle->intervals[handle->intervals_lenght].callbacks_lenght = 0;

    if(interval_index != NULL)
        *interval_index = handle->intervals_lenght;

    ++handle->intervals_lenght;

    return HAL_OK;
}

HAL_StatusTypeDef TIMEBASE_register_callback(TIMEBASE_HandleTypeDef *handle, uint8_t interval_index, TIMEBASE_CallbackTypeDef callback) {
    TIMEBASE_IntervalTypeDef *interval = &handle->intervals[interval_index];

    if(interval->callbacks_lenght == TIMEBASE_MAX_CALLBACKS) {
        return HAL_ERROR;
    }

    interval->callbacks[interval->callbacks_lenght] = callback;

    ++interval->callbacks_lenght;

    return HAL_OK;
}

HAL_StatusTypeDef TIMEBASE_routine(TIMEBASE_HandleTypeDef *handle) {
    for(uint8_t i=0; i<handle->intervals_lenght; ++i) {
        if(!(handle->intervals_flag & (1 << i)))
            continue;

        for(uint8_t j=0; j<handle->intervals[i].callbacks_lenght; ++j) {
            if(handle->intervals[i].callbacks[j]() != HAL_OK) {
                // LOG SOMEHOW
            }
        }

        handle->intervals_flag &= ~(1 << i);
    }

    return HAL_OK;
}

void TIMEBASE_AutoreloadCallback(TIMEBASE_HandleTypeDef *handle, TIM_HandleTypeDef *htim) {
    if(handle->htim == htim) {
        ++handle->repetition_counter;
        for(uint8_t i=0; i<handle->intervals_lenght; ++i) {
            if((handle->repetition_counter*handle->base_interval_us) % handle->intervals[i].interval_us == 0) {
                handle->intervals_flag |= (1 << i);
            }
        }
    }
}