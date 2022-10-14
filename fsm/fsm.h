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

typedef struct FSM_HandleStruct FSM_HandleTypeDef;
typedef struct FSM_StateStruct FSM_StateTypeDef;
typedef struct FSM_ConfigStruct FSM_ConfigTypeDef;

typedef uint8_t (*FSM_state_function)(FSM_HandleTypeDef handle);
typedef uint8_t (*FSM_event_handler)(FSM_HandleTypeDef handle, uint8_t event);

HAL_StatusTypeDef FSM_init(FSM_HandleTypeDef *handle, FSM_ConfigTypeDef *config, uint8_t event_count, FSM_state_function run_callback, FSM_state_function transition_callback);

#endif //FSM_H