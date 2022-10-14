#include "fsm.h"

#include <string.h>

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

HAL_StatusTypeDef FSM_init(FSM_HandleTypeDef *handle, FSM_ConfigTypeDef *config, uint8_t event_count, FSM_void_function run_callback, FSM_void_function transition_callback) {
    if(handle == NULL) {
        return HAL_ERROR;
    }

    if(config == NULL) {
        return HAL_ERROR;
    }

    handle->current_state = 0;
    
    handle->events_length = 0;
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
        handle->config->state_table[0].entry(handle);
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

    if(!(handle->events_async ^ handle->events_sync) & mask) {
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
        handle->config->state_table[handle->current_state].exit(handle);
    }

    handle->current_state = state;

    if(handle->config->state_table[handle->current_state].entry != NULL) {
        handle->config->state_table[handle->current_state].entry(handle);
    }

    return HAL_OK;
}

HAL_StatusTypeDef FSM_run(FSM_HandleTypeDef *handle) {
    if(handle == NULL) {
        return HAL_ERROR;
    }

    if(handle->config->state_table[handle->current_state].event_handler != NULL) {
        for(uint8_t i=0; i<handle->events_length; ++i) {
            uint32_t mask = 1 << i;
            if((handle->events_async ^ handle->events_sync) & mask) {
                handle->events_async ^= mask;
                uint32_t ret = handle->config->state_table[handle->current_state].event_handler(handle, i);

                if(ret != handle->current_state) {
                    return _FSM_transition(handle, ret);
                }
            }
        }
    }

    if(handle->config->state_table[handle->current_state].do_work != NULL) {
        uint32_t ret = handle->config->state_table[handle->current_state].do_work(handle);

        if(ret != handle->current_state) {
            return _FSM_transition(handle, ret);
        }
    }

    if(handle->run_callback != NULL) {
        handle->run_callback(handle);
    }

    return HAL_OK;
}