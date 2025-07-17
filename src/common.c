#include "../include/common.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>

static log_level_t current_log_level = LOG_LEVEL_INFO;
static const char *log_level_strings[] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

void set_log_level(log_level_t level)
{
    current_log_level = level;
}

void log_message(log_level_t level, const char *file, int line, const char *fmt, ...)
{
    if (level < current_log_level)
        return;

    time_t now;
    struct tm *tm_info;
    char timestamp[64];

    time(&now);
    tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    const char *filename = strrchr(file, '/');
    filename = filename ? filename + 1 : file;

    FILE *output = (level >= LOG_LEVEL_ERROR) ? stderr : stdout;

    fprintf(output, "[%s] %s %s:%d - ", timestamp, log_level_strings[level], filename, line);

    va_list args;
    va_start(args, fmt);
    vfprintf(output, fmt, args);
    va_end(args);

    fprintf(output, "\n");
    fflush(output);
}

const char *fe_strerror(fe_error_t error)
{
    switch (error)
    {
    case FE_SUCCESS:
        return "Success";
    case FE_ERROR_OUT_OF_MEMORY:
        return "Memory allocation failed";
    case FE_ERROR_FILE_IO:
        return "File I/O error";
    case FE_ERROR_INVALID_PARAMETER:
        return "Invalid parameter";
    case FE_ERROR_SYSTEM:
        return "System error";
    case FE_ERROR_X11:
        return "X11 error";
    case FE_ERROR_WINDOW_CREATION_FAILED:
        return "Window creation failed";
    default:
        return "Unknown error";
    }
}
