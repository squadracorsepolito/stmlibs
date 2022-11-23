/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#ifndef TIMER_UTILS_H
#define TIMER_UTILS_H

#include "main.h"

#include <inttypes.h>

/**
 * @brief     Check whether the timer is placed in either APB1 or APB2 bus
 * 
 * @param     __HANDLE__ TIM Handle
 * @return    True if the timer is on APB1 bus false if it is on APB2 
 */
#define _M_GET_TIM_APB_PLACEMENT(__HANDLE__) (((__HANDLE__)->Instance < (TIM_TypeDef *)APB2PERIPH_BASE) ? 1U : 0U)

/**
 * @brief     Get timer frequency
 * 
 * @param     TIM TIM Handle
 * @return    value of frequency in Hz
 */
#define TIM_GET_FREQ(TIM) ((float)TIM_GetInternalClkFreq((TIM)) / ((TIM)->Instance->PSC + 1))

/**
 * @brief     Convert milliseconds to ticks for a specified timer
 * 
 * @param     TIM TIM Handle
 * @param     MS Value in milliseconds to convert (float)
 * @return    ticks in uint32_t
 */
#define TIM_MS_TO_TICKS(TIM, MS) (uint32_t)((TIM_GET_FREQ((TIM)) * (MS)) / 1000)

/**
 * @brief     Convert ticks to milliseconds for a specified timer
 * 
 * @param     TIM TIM Handle
 * @param     TICKS Value in ticks to convert (uint32)
 * @return    milliseconds in float 
 */
#define TIM_TICKS_TO_MS(TIM, TICKS) (((float)(TICKS)*1000) / TIM_GET_FREQ((TIM)))
/**
 * @brief     Get timer max autoreload value
 * 
 * @param     htim TIM Handle
 * @return    Autoreload
 */
#define TIM_GET_MAX_AUTORELOAD(TIM) (IS_TIM_32B_COUNTER_INSTANCE((TIM)->Instance) ? UINT32_MAX : UINT16_MAX)
/**
 * @brief     Get timer length
 * 
 * @param     htim TIM Handle
 * @return    length
 */
#define TIM_GET_LENGTH(TIM) (IS_TIM_32B_COUNTER_INSTANCE((TIM)->Instance) ? 32U : 16U)

/**
 * @brief     Get timer clock
 * 
 * @param     htim TIM Handle
 * @return    clk frequency in Hz
 */
uint32_t TIM_GetInternalClkFreq(TIM_HandleTypeDef *htim);

#endif