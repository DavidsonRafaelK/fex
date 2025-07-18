#ifndef COMMON_H
#define COMMON_H

#define _GNU_SOURCE
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "config.h"

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

#define SAFE_MALLOC(ptr, size)                                           \
    do                                                                   \
    {                                                                    \
        if (((ptr) = malloc(size)) == NULL)                              \
        {                                                                \
            LOG_ERROR("Memory allocation failed for %zu bytes", (size)); \
            exit(EXIT_FAILURE);                                          \
        }                                                                \
    } while (0)

#define SAFE_REALLOC(ptr, size)                                                 \
    do                                                                          \
    {                                                                           \
        void *temp = realloc(ptr, size);                                        \
        if (temp == NULL && (size) > 0)                                         \
        {                                                                       \
            LOG_ERROR("Memory reallocation failed: %zu bytes", (size_t)(size)); \
            exit(EXIT_FAILURE);                                                 \
        }                                                                       \
        (ptr) = temp;                                                           \
    } while (0)

#define SAFE_CALLOC(ptr, count, size)                                                                \
    do                                                                                               \
    {                                                                                                \
        if (((ptr) = calloc(count, size)) == NULL)                                                   \
        {                                                                                            \
            LOG_ERROR("Memory allocation failed: %zu * %zu bytes", (size_t)(count), (size_t)(size)); \
            exit(EXIT_FAILURE);                                                                      \
        }                                                                                            \
    } while (0)

#define SAFE_FREE(ptr)    \
    do                    \
    {                     \
        if (ptr)          \
        {                 \
            free(ptr);    \
            (ptr) = NULL; \
        }                 \
    } while (0)

#define SAFE_STRNCPY(dest, src, size)   \
    do                                  \
    {                                   \
        strncpy(dest, src, (size) - 1); \
        (dest)[(size) - 1] = '\0';      \
    } while (0)

typedef enum
{
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} log_level_t;

extern log_level_t g_log_level;

#define LOG_DEBUG(...) log_message(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) log_message(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) log_message(LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_message(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) log_message(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(value, min, max) MIN(MAX(value, min), max)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void log_message(log_level_t level, const char *file, int line, const char *fmt, ...);
void log_set_level(log_level_t level);
void log_to_file(const char *filename);
const char *fe_strerror(fe_error_t error);

typedef struct
{
    struct timespec start;
    struct timespec end;
} perf_timer_t;

void perf_timer_start(perf_timer_t *timer);
double perf_timer_end(perf_timer_t *timer);

const char *fe_error_to_string(fe_error_t error);
bool is_debug_build(void);

#endif