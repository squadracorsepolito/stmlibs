/*
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * Authors
 * - Federico Carbone [federico.carbone.sc@gmail.com]
 */

#include "logger.h"

#include "critical_section.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define NORMAL                 "\033[0m"
#define WHITE_BG(S)            "\033[0;47m" S NORMAL
#define RED_BG(S)              "\033[0;41m" S NORMAL
#define YELLOW_BG(S)           "\033[0;43m" S NORMAL
#define CYAN_BG(S)             "\033[0;46m" S NORMAL
#define RED_BG_ON_YELLOW_FG(S) "\033[0;31;43m" S NORMAL

#define LOGGER_INFO_STRING    WHITE_BG("[INFO]")
#define LOGGER_DEBUG_STRING   WHITE_BG("[DBG]")
#define LOGGER_WARNING_STRING YELLOW_BG("[WRN]")
#define LOGGER_ERROR_STRING   RED_BG("[ERR]")

HAL_StatusTypeDef LOGGER_init(
    LOGGER_HandleTypeDef *handle,
    char *buffer,
    uint32_t buffer_len,
    LOGGER_flushTypeDef flush_raw) {
    if (handle == NULL) {
        return HAL_ERROR;
    }

    if (buffer == NULL) {
        return HAL_ERROR;
    }

    if (flush_raw == NULL) {
        return HAL_ERROR;
    }

    handle->buffer     = buffer;
    handle->buffer_len = buffer_len;
    handle->index      = 0;
    handle->flush_raw  = flush_raw;

    return HAL_OK;
}

HAL_StatusTypeDef LOGGER_log(LOGGER_HandleTypeDef *handle, LOGGER_MODE mode, char *template, ...) {
    if (handle == NULL) {
        return HAL_ERROR;
    }

    if (template == NULL) {
        return HAL_ERROR;
    }

    if (handle->index >= handle->buffer_len - 1) {
        return HAL_ERROR;
    }

    CS_ENTER();

    handle->buffer[handle->index++] = '\r';
    handle->buffer[handle->index++] = '\n';

    char *s = NULL;

    switch (mode) {
        case LOGGER_INFO:
            s = LOGGER_INFO_STRING;
            break;
        case LOGGER_DEBUG:
            s = LOGGER_DEBUG_STRING;
            break;
        case LOGGER_WARNING:
            s = LOGGER_WARNING_STRING;
            break;
        case LOGGER_ERROR:
            s = LOGGER_ERROR_STRING;
            break;
        default:
            return HAL_ERROR;
    }

    handle->index += snprintf(handle->buffer + handle->index, handle->buffer_len - handle->index - 1, "%s ", s);

    va_list args;
    va_start(args, template);
    handle->index += vsnprintf(handle->buffer + handle->index, handle->buffer_len - handle->index - 1, template, args);
    va_end(args);

    CS_EXIT();

    if (handle->index >= handle->buffer_len) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef LOGGER_flush(LOGGER_HandleTypeDef *handle) {
    if (handle == NULL) {
        return HAL_ERROR;
    }

    static HAL_StatusTypeDef errorcode = HAL_OK;

    if (handle->index == 0) {
        return HAL_OK;
    }

    if ((errorcode = handle->flush_raw(handle->buffer, handle->index)) != HAL_OK) {
        volatile uint8_t a = 0;
        return errorcode;
    }
    handle->buffer[0] = '\0';
    handle->index     = 0;

    return errorcode;
}