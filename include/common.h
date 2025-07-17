#ifndef COMMON_H
#define COMMON_H

#define _GNU_SOURCE
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define APP_NAME "FEX"
#define APP_VERSION "0.1.0"
#define APP_AUTHOR "DavidsonRafaelK"
#define APP_PATH_LENGTH 4096
#define WINDOW_DEFAULT_WIDTH 800
#define WINDOW_DEFAULT_HEIGHT 600
#define WINDOW_MIN_WIDTH 400
#define WINDOW_MIN_HEIGHT 300

#define FPS_TARGET 60                  // Make the target FPS constant
#define FRAME_TIME (1000 / FPS_TARGET) // Frame time in milliseconds

typedef enum
{
    FE_SUCCESS = 0,
    FE_ERROR_INVALID_PARAMETER,
    FE_ERROR_OUT_OF_MEMORY,
    FE_ERROR_FILE_IO,
    FE_ERROR_SYSTEM,
    FE_ERROR_NOT_FOUND,
    FE_ERROR_PERMISSION_DENIED,
    FE_ERROR_X11,
    FE_ERROR_WINDOW_CREATION_FAILED,
    FE_ERROR_UNKNOWN
} fe_error_t;

#endif