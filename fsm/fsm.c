/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#include "fsm.h"

#include <string.h>

HAL_StatusTypeDef FSM_init(FSM_HandleTypeDef *handle, FSM_ConfigTypeDef *config, uint8_t event_count, FSM_void_function run_callback, FSM_void_function transition_callback) {
    if(handle == NULL) {
        return HAL_ERROR;
    }

    if(config == NULL) {
        return HAL_ERROR;
    }

    handle->current_state = 0;
    
    handle->events_length = event_count;
    handle->events_sync = 0;
    handle->events_async = 0;

    handle->run_callback = run_callback;
    handle->transition_callback = transition_callback;

    handle->config = config;

    return HAL_OK;
}

HAL_StatusTypeDef FSM_start(FSM_HandleTypeDef *handle) {
    if(handle == NULL) {
        return HAL_ERROR;
    }

    if(handle->config->state_length > 0 && handle->config->state_table[0].entry != NULL) {
        handle->config->state_table[0].entry();
    }

    return HAL_OK;
}

uint32_t FSM_get_state(FSM_HandleTypeDef *handle) {
    return handle->current_state;
}

HAL_StatusTypeDef FSM_trigger_event(FSM_HandleTypeDef *handle, uint8_t event) {
    uint32_t mask = 1 << event;

    if(handle == NULL) {
        return HAL_ERROR;
    }

    if(event >= handle->events_length) {
        return HAL_ERROR;
    }

    if(~(handle->events_async ^ handle->events_sync) & mask) {
        handle->events_async ^= mask;
    }

    return HAL_OK;
}

HAL_StatusTypeDef _FSM_transition(FSM_HandleTypeDef *handle, uint32_t state) {
    if(handle == NULL) {
        return HAL_ERROR;
    }

    if(state >= handle->config->state_length) {
        return HAL_ERROR;
    }

    if(handle->config->state_table[handle->current_state].exit != NULL) {
        handle->config->state_table[handle->current_state].exit();
    }

    handle->current_state = state;

    if(handle->config->state_table[handle->current_state].entry != NULL) {
        handle->config->state_table[handle->current_state].entry();
    }

    return HAL_OK;
}

HAL_StatusTypeDef FSM_routine(FSM_HandleTypeDef *handle) {
    if(handle == NULL) {
        return HAL_ERROR;
    }

    if(handle->config->state_table[handle->current_state].event_handler != NULL) {
        for(uint8_t i=0; i<handle->events_length; ++i) {
            uint32_t mask = 1 << i;
            if((handle->events_async ^ handle->events_sync) & mask) {
                handle->events_async ^= mask;
                uint32_t ret = handle->config->state_table[handle->current_state].event_handler(i);

                if(ret != handle->current_state) {
                    return _FSM_transition(handle, ret);
                }
            }
        }
    }

    if(handle->config->state_table[handle->current_state].do_work != NULL) {
        uint32_t ret = handle->config->state_table[handle->current_state].do_work();

        if(ret != handle->current_state) {
            return _FSM_transition(handle, ret);
        }
    }

    if(handle->run_callback != NULL) {
        handle->run_callback();
    }

    return HAL_OK;
}