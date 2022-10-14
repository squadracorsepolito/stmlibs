#include "fsm.h"

#include <string.h>

struct FSM_ConfigStruct {
    uint8_t state_lenght;
    FSM_StateTypeDef *state_table;
};

struct FSM_HandleStruct {
    uint32_t current_state;

    uint8_t events_lenght;
    uint32_t events_sync;
    uint32_t events_async;

    FSM_state_function run_callback;
    FSM_state_function transition_callback;

    FSM_ConfigTypeDef *config;
};

struct FSM_StateStruct {
    FSM_event_handler state_handler;
    FSM_state_function state_entry;
    FSM_state_function state_do;
    FSM_state_function state_exit;
};

HAL_StatusTypeDef FSM_init(FSM_HandleTypeDef *handle, FSM_ConfigTypeDef *config, uint8_t event_count, FSM_state_function run_callback, FSM_state_function transition_callback) {
    if(handle == NULL) {
        return HAL_ERROR;
    }

    if(config == NULL) {
        return HAL_ERROR;
    }

    handle->current_state = 0;
    
    handle->events_lenght = 0;
    handle->events_sync = 0;
    handle->events_async = 0;

    handle->run_callback = run_callback;
    handle->transition_callback = transition_callback;

    handle->config = config;

    return HAL_OK;
}
