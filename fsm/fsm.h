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

#include <inttypes.h>

#define FSM_MAX_EVENTS 32

struct FSM_ConfigStruct {
    uint8_t state_length;
    FSM_StateTypeDef *state_table;
};

struct FSM_HandleStruct {
    uint32_t current_state;

    uint8_t events_length;
    uint32_t events_sync;
    uint32_t events_async;

    FSM_void_function run_callback;
    FSM_void_function transition_callback;

    FSM_ConfigTypeDef *config;
};

struct FSM_StateStruct {
    FSM_event_handler event_handler;
    FSM_void_function entry;
    FSM_state_function do_work;
    FSM_void_function exit;
};

typedef struct FSM_HandleStruct FSM_HandleTypeDef;
typedef struct FSM_StateStruct FSM_StateTypeDef;
typedef struct FSM_ConfigStruct FSM_ConfigTypeDef;

typedef void (*FSM_void_function)(FSM_HandleTypeDef *handle);
typedef uint32_t (*FSM_state_function)(FSM_HandleTypeDef *handle);
typedef uint32_t (*FSM_event_handler)(FSM_HandleTypeDef *handle, uint8_t event);

HAL_StatusTypeDef FSM_init(FSM_HandleTypeDef *handle, FSM_ConfigTypeDef *config, uint8_t event_count, FSM_void_function run_callback, FSM_void_function transition_callback);
HAL_StatusTypeDef FSM_start(FSM_HandleTypeDef *handle);
uint32_t FSM_get_state(FSM_HandleTypeDef *handle);
HAL_StatusTypeDef FSM_trigger_event(FSM_HandleTypeDef *handle, uint8_t event);
HAL_StatusTypeDef FSM_run(FSM_HandleTypeDef *handle);
#endif //FSM_H