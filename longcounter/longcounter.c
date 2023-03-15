/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Not a Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Filippo Volpe [filippovolpe98+dev@gmail.com]
 * 
 * Reviewers
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#include "longcounter.h"

#include "timer_utils.h"

STMLIBS_StatusTypeDef LONGCOUNTER_init(LONGCOUNTER_HandleTypeDef *handle, TIM_HandleTypeDef *htim) {
    if (handle == NULL || htim == NULL) {
        return STMLIBS_ERROR;
    }

    handle->_htim    = htim;
    handle->_counter = 0;

    handle->_htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    handle->_htim->Init.Period      = TIM_GET_MAX_AUTORELOAD(htim);
    TIM_Base_SetConfig(handle->_htim->Instance, &handle->_htim->Init);
    HAL_TIM_Base_Start_IT(handle->_htim);

    return STMLIBS_OK;
}

LONGCOUNTER_Counter_Type LONGCOUNTER_get_counter(LONGCOUNTER_HandleTypeDef *handle) {
    // Compose the counter using the top bits from the long counter and the lower one from TIM
    return ((handle->_counter) & (~TIM_GET_MAX_AUTORELOAD(handle->_htim))) |
           (__HAL_TIM_GetCounter(handle->_htim) & TIM_GET_MAX_AUTORELOAD(handle->_htim));
}

void LONGCOUNTER_TIM_OverflowCallback(LONGCOUNTER_HandleTypeDef *handle, TIM_HandleTypeDef *htim) {
    if (htim == handle->_htim) {
        handle->_counter += (1 << TIM_GET_LENGTH(handle->_htim));
    }
}