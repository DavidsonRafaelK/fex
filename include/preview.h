#ifndef PREVIEW_H
#define PREVIEW_H

#include "window.h"

typedef struct
{
    int width;
    int height;
    int x;
    int y;
    const char *title;
} preview_config_t;

typedef struct
{
    void *base;
    size_t capacity;
    size_t used;
} memory_pool_t;

typedef struct
{
    window_context_t window;
} preview_context_t;

typedef enum
{
    PREVIEW_EVENT_NONE,
    PREVIEW_EVENT_RESIZE,
} preview_event_type_t;

typedef struct
{
    preview_event_type_t type;
    union
    {
        struct
        {
            int width, height;
        } resize;

        struct
        {
            char text[8];
            KeySym key;
            unsigned int state;
        } key_event;
    } data;
} preview_event_t;

fe_error_t preview_init(preview_context_t *ctx, const preview_config_t *config);
fe_error_t preview_poll_event(preview_context_t *ctx, preview_event_t *event);
void preview_destroy(preview_context_t *ctx);

#endif
