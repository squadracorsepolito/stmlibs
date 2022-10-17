/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#include "pwm.h"
#include "timer_utils.h"

HAL_StatusTypeDef PWM_init(TIM_HandleTypeDef *htim, float period_ms) {
    if(htim == NULL) {
        return HAL_ERROR;
    }

    uint32_t ticks = TIM_MS_TO_TICKS(htim, period_ms);
    if(ticks > __HAL_TIM_GetAutoreload(htim)) {
        return HAL_ERROR;
    }

    __HAL_TIM_SetAutoreload(htim, ticks);

    return HAL_OK;
}

HAL_StatusTypeDef PWM_start(TIM_HandleTypeDef *htim, float duty_cycle, uint32_t channel) {
    if(PWM_update_duty_cycle(htim, duty_cycle, channel) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_TIM_PWM_Start(htim, channel);
}

HAL_StatusTypeDef PWM_stop(TIM_HandleTypeDef *htim, uint32_t channel) {
    if(htim == NULL) {
        return HAL_ERROR;
    }
    
    return HAL_TIM_PWM_Stop(htim, channel);
}

HAL_StatusTypeDef PWM_update_duty_cycle(TIM_HandleTypeDef *htim, float duty_cycle, uint32_t channel) {
    if(htim == NULL) {
        return HAL_ERROR;
    }

    if(duty_cycle < 0 || duty_cycle > 1) {
        return HAL_ERROR;
    }
    
    __HAL_TIM_SetCompare(htim, channel, __HAL_TIM_GetAutoreload(htim) * duty_cycle);

    return HAL_OK;
}