/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Not a Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Filippo Volpe [filippovolpe98+dev@gmail.com]
 */

#include "longcounter.h"

HAL_StatusTypeDef LONGCOUNTER_init(LONGCOUNTER_HandleTypeDef *handle, TIM_HandleTypeDef *htim) {
    if(handle == NULL || htim == NULL) {
        return HAL_ERROR;
    }
    // TODO: check the timer htim is LONGCOUNTER_TIM_LENGTH bits

    handle->_htim = htim;
    handle->_counter = 0;

    handle->_htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    handle->_htim->Init.Period = (uint32_t)((1ul<<LONGCOUNTER_TIM_LENGTH)-1);
    TIM_Base_SetConfig(handle->_htim->Instance,&handle->_htim->Init);
    HAL_TIM_Base_Start_IT(handle->_htim);

    return HAL_OK;
}

LONGCOUNTER_Counter_Type LONGCOUNTER_get_counter(LONGCOUNTER_HandleTypeDef *handle){
    // Compose the counter using the top bits from the long counter and the lower one from TIM
    return ((handle->_counter)&(~((1ul<<LONGCOUNTER_TIM_LENGTH)-1)))|((handle->_htim->Instance->CNT)&((1ul<<LONGCOUNTER_TIM_LENGTH)-1));
}

void LONGCOUNTER_TIM_OverflowCallback(LONGCOUNTER_HandleTypeDef *handle){
    handle->_counter+=(1<<LONGCOUNTER_TIM_LENGTH);
}