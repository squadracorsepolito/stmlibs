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
#define GREEN_BG(S)            "\033[0;42m" S NORMAL
#define YELLOW_BG(S)           "\033[0;43m" S NORMAL
#define CYAN_BG(S)             "\033[0;46m" S NORMAL
#define RED_BG_ON_YELLOW_FG(S) "\033[0;31;43m" S NORMAL

#define LOGGER_INFO_STRING    GREEN_BG("[INFO]")
#define LOGGER_DEBUG_STRING   WHITE_BG("[DBG]")
#define LOGGER_WARNING_STRING YELLOW_BG("[WRN]")
#define LOGGER_ERROR_STRING   RED_BG("[ERR]")

STMLIBS_StatusTypeDef LOGGER_init(LOGGER_HandleTypeDef *handle,
                                  char *buffer,
                                  uint32_t buffer_len,
                                  LOGGER_flushTypeDef flush_raw) {
    if (handle == NULL) {
        return STMLIBS_ERROR;
    }

    if (buffer == NULL) {
        return STMLIBS_ERROR;
    }

    if (flush_raw == NULL) {
        return STMLIBS_ERROR;
    }

    handle->buffer     = buffer;
    handle->buffer_len = buffer_len;
    handle->index      = 0;
    handle->flush_raw  = flush_raw;

    return STMLIBS_OK;
}

STMLIBS_StatusTypeDef LOGGER_log(LOGGER_HandleTypeDef *handle, LOGGER_MODE mode, char *template, ...) {
    if (handle == NULL) {
        return STMLIBS_ERROR;
    }

    if (template == NULL) {
        return STMLIBS_ERROR;
    }

    if (handle->index >= handle->buffer_len - 1) {
        return STMLIBS_ERROR;
    }

    CS_ENTER();

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
            return STMLIBS_ERROR;
    }

    if (mode != LOGGER_RAW) {
        handle->buffer[handle->index++] = '\r';
        handle->buffer[handle->index++] = '\n';
        handle->index += snprintf(
            handle->buffer + handle->index, handle->buffer_len - handle->index - 1, "%s[%5ld] ", s, HAL_GetTick());
    }

    va_list args;
    va_start(args, template);
    handle->index += vsnprintf(handle->buffer + handle->index, handle->buffer_len - handle->index - 1, template, args);
    va_end(args);

    CS_EXIT();

    if (handle->index >= handle->buffer_len) {
        return STMLIBS_ERROR;
    }

    return STMLIBS_OK;
}

STMLIBS_StatusTypeDef LOGGER_flush(LOGGER_HandleTypeDef *handle) {
    if (handle == NULL) {
        return STMLIBS_ERROR;
    }

    static STMLIBS_StatusTypeDef errorcode = STMLIBS_OK;

    if (handle->index == 0) {
        return STMLIBS_OK;
    }

    if ((errorcode = handle->flush_raw(handle->buffer, handle->index)) != STMLIBS_OK) {
        return errorcode;
    }
    handle->buffer[0] = '\0';
    handle->index     = 0;

    return errorcode;
}