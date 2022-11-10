/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#ifndef FSM_H
#define FSM_H

#include "main.h"

#include <inttypes.h>

#define FSM_MAX_EVENTS 32

typedef void (*FSM_void_function)(void);
typedef void (*FSM_callback_function)(uint32_t state);
typedef uint32_t (*FSM_state_function)(void);
typedef uint32_t (*FSM_event_handler)(uint8_t event);

struct FSM_StateStruct {
    FSM_event_handler event_handler;
    FSM_void_function entry;
    FSM_state_function do_work;
    FSM_void_function exit;
};
typedef struct FSM_StateStruct FSM_StateTypeDef;

struct FSM_ConfigStruct {
    uint8_t state_length;
    FSM_StateTypeDef *state_table;
};
typedef struct FSM_ConfigStruct FSM_ConfigTypeDef;

struct FSM_HandleStruct {
    uint32_t current_state;

    uint8_t events_length;
    uint32_t events_sync;
    uint32_t events_async;

    FSM_callback_function run_callback;
    FSM_callback_function transition_callback;

    FSM_ConfigTypeDef *config;
};
typedef struct FSM_HandleStruct FSM_HandleTypeDef;

/**
 * @brief     Initialize a FSM_HandleTypeDef structure 
 * 
 * @param     handle Reference to the struct to be initialized
 * @param     config Reference to the FSM_ConfigTypeDef containing all the states
 * @param     event_count Number of events
 * @param     run_callback Function to be executed on every FSM_routine call
 * @param     transition_callback Function to be executen on every transition
 * @return    HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef FSM_init(
    FSM_HandleTypeDef *handle,
    FSM_ConfigTypeDef *config,
    uint8_t event_count,
    FSM_callback_function run_callback,
    FSM_callback_function transition_callback);
/**
 * @brief     Runs the entry function of the FSM_HandleTypeDef structure 
 * 
 * @param     handle Reference to the initialized struct
 * @return    HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef FSM_start(FSM_HandleTypeDef *handle);
/**
 * @brief     Get the current state of the FSM_HandleTypeDef structure 
 * 
 * @param     handle Reference to the initialized struct
 * @return    state of the fsm
 */
uint32_t FSM_get_state(FSM_HandleTypeDef *handle);
/**
 * @brief     Trigger an event on the FSM_HandleTypeDef structure 
 * 
 * @param     handle Reference to the initialized struct
 * @param     event The event to be triggered
 * @return    HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef FSM_trigger_event(FSM_HandleTypeDef *handle, uint8_t event);
/**
 * @brief     Routine to be called in the main loop
 * 
 * @param     handle Reference to the initialized struct
 * @return    HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef FSM_routine(FSM_HandleTypeDef *handle);
#endif  //FSM_H