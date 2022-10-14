/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#ifndef PWM_H
#define PWM_H

#include "main.h"
#include <inttypes.h>

/**
 * @brief     Initialize the given timer 
 * 
 * @param     htim Timer handle
 * @param     period_ms Period to be setted that will be common for each channel
 * @return    HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef PWM_init(TIM_HandleTypeDef *htim, float period_ms);
/**
 * @brief     Start the specified channel 
 * 
 * @param     htim Timer handle
 * @param     duty_cycle Duty cycle to be setted on the specified channel (expressed as a 0-1 value)
 * @param     channel Channel to be started
 * @return    HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef PWM_start(TIM_HandleTypeDef *htim, float duty_cycle, uint32_t channel);
/**
 * @brief     Stop the specified channel 
 * 
 * @param     htim Timer handle
 * @param     channel Channel to be stopped
 * @return    HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef PWM_stop(TIM_HandleTypeDef *htim, uint32_t channel);
/**
 * @brief     Set the duty cycle on a given channel, even while channel is running 
 * 
 * @param     htim Timer handle
 * @param     duty_cycle Duty cycle to be setted on the specified channel (expressed as a 0-1 value)
 * @param     channel Channel to be started
 * @return    HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef PWM_update_duty_cycle(TIM_HandleTypeDef *htim, float duty_cycle, uint32_t channel);
#endif //PWM_H