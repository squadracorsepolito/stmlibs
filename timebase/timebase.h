/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#ifndef TIMEBASE_H
#define TIMEBASE_H

#include "main.h"
#include "stdint.h"

#define TIMEBASE_MAX_INTERVALS 32

#ifndef TIMEBASE_MAX_CALLBACKS
#define TIMEBASE_MAX_CALLBACKS 16
#endif  //TIMEBASE_MAX_CALLBACKS

typedef HAL_StatusTypeDef (*TIMEBASE_CallbackTypeDef)();

struct TIMEBASE_IntervalStruct {
    uint32_t interval_us;
    TIMEBASE_CallbackTypeDef callbacks[TIMEBASE_MAX_CALLBACKS];
    uint8_t callbacks_length;
};
typedef struct TIMEBASE_IntervalStruct TIMEBASE_IntervalTypeDef;

struct TIMEBASE_HandleStruct {
    TIM_HandleTypeDef *htim;

    uint32_t repetition_counter;

    uint32_t base_interval_us;

    TIMEBASE_IntervalTypeDef intervals[TIMEBASE_MAX_INTERVALS];
    uint32_t intervals_flag;
    uint8_t intervals_length;
};
typedef struct TIMEBASE_HandleStruct TIMEBASE_HandleTypeDef;

/**
 * @brief     Initialize a TIMEBASE_HandleTypeDef structure
 * 
 * @param     handle Reference to the struct to be initialized
 * @param     base_interval_us Base interval tick expressed in us
 * @return    HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef TIMEBASE_init(TIMEBASE_HandleTypeDef *handle, TIM_HandleTypeDef *htim, uint32_t base_interval_us);
/**
 * @brief     Adds an interval to the specified TIMEBASE_HandleTypeDef structure
 * 
 * @param     handle Reference to the handle
 * @param     interval_us Interval to be added expressed in us
 * @param     interval_index Reference to an uint8_t in which the index of the added interval will be stored
 * @return    HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef TIMEBASE_add_interval(TIMEBASE_HandleTypeDef *handle, uint32_t interval_us, uint8_t *interval_index);
/**
 * @brief     Register a callback to the specified interval
 * 
 * @param     handle Reference to the handle
 * @param     interval_index Index of the interval returned by @TIMEBASE_add_interval
 * @param     callback Callback to be registered
 * @return    HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef TIMEBASE_register_callback(
    TIMEBASE_HandleTypeDef *handle,
    uint8_t interval_index,
    TIMEBASE_CallbackTypeDef callback);
/**
 * @brief     Routine to be called in the main loop which executes the callbacks
 * 
 * @param     handle Reference to the handle
 * @return    HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef TIMEBASE_routine(TIMEBASE_HandleTypeDef *handle);
/**
 * @brief     Function to be called in the HAL_TIM_PeriodElapsedCallback function
 * 
 * @param     handle Reference to the handle
 * @param     htim Parameter of the HAL_TIM_PeriodElapsedCallback function
 */
void TIMEBASE_TimerElapsedCallback(TIMEBASE_HandleTypeDef *handle, TIM_HandleTypeDef *htim);

#endif  //TIMEBASE_H