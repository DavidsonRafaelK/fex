#ifndef WINDOW_H
#define WINDOW_H

#include "common.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdbool.h>

typedef struct
{
    int width;
    int height;
    int x;
    int y;
    const char *title;
    bool resizable;
} window_config_t;

typedef struct
{
    Display *display;
    Window window;
    int screen;
    window_config_t config;
    Atom wm_delete_window;
    bool should_close;
    bool is_focused;
} window_context_t;

typedef enum
{
    WINDOW_EVENT_NONE,
    WINDOW_EVENT_CLOSE,
    WINDOW_EVENT_FOCUS_GAINED,
    WINDOW_EVENT_FOCUS_LOST,
    WINDOW_EVENT_RESIZE,
    WINDOW_EVENT_KEY_PRESS,
    WINDOW_EVENT_KEY_RELEASE,
    WINDOW_EVENT_MOUSE_BUTTON_PRESS,
    WINDOW_EVENT_MOUSE_BUTTON_RELEASE,
    WINDOW_EVENT_MOUSE_MOVE,
    WINDOW_EVENT_MOUSE_SCROLL
} window_event_type_t;

typedef struct
{
    window_event_type_t type;
    union
    {
        struct
        {
            int width, height;
        } resize;

        struct
        {
            KeySym key;
            unsigned int state;
            char text[8];
        } key;

        struct
        {
            int x, y;
            int button;
        } mouse;
    } data;
} window_event_t;

fe_error_t window_init(window_context_t *ctx, const window_config_t *config);
fe_error_t window_poll_event(window_context_t *ctx, window_event_t *event);
fe_error_t window_resize(window_context_t *ctx, int width, int height);

void window_destroy(window_context_t *ctx);
void window_swap_buffers(window_context_t *ctx);

bool window_should_close(const window_context_t *ctx);
bool window_is_focused(const window_context_t *ctx);

window_config_t window_get_config(void);

#endif