#include "window.h"
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

static void set_window_properties(window_context_t *ctx)
{
    XStoreName(ctx->display, ctx->window, ctx->config.title);

    // Set window size hints for min/max size if resizable
    XSizeHints *size_hints = XAllocSizeHints();
    if (!ctx->config.resizable)
    {
        size_hints->flags = PMinSize | PMaxSize;
        size_hints->min_width = ctx->config.width;
        size_hints->min_height = ctx->config.height;
        size_hints->max_width = ctx->config.width;
        size_hints->max_height = ctx->config.height;
    }
    else
    {
        size_hints->flags = PMinSize;
        size_hints->min_width = WINDOW_MIN_WIDTH;
        size_hints->min_height = WINDOW_MIN_HEIGHT;
    }
    XSetWMNormalHints(ctx->display, ctx->window, size_hints);
    XFree(size_hints);

    // Set WM protocols for handling window close
    ctx->wm_delete_window = XInternAtom(ctx->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(ctx->display, ctx->window, &ctx->wm_delete_window, 1);
}

// Initialize window
fe_error_t window_init(window_context_t *ctx, const window_config_t *config)
{
    if (!ctx || !config)
    {
        LOG_ERROR("Invalid parameter: ctx or config is NULL");
        return FE_ERROR_INVALID_PARAMETER;
    }

    // Open connection to X server
    ctx->display = XOpenDisplay(NULL);
    if (!ctx->display)
    {
        LOG_ERROR("Failed to open X display");
        return FE_ERROR_X11;
    }

    ctx->screen = DefaultScreen(ctx->display);
    ctx->config = *config;

    // Create window and set the properties
    ctx->window = XCreateSimpleWindow(
        ctx->display,
        RootWindow(ctx->display, ctx->screen),
        config->x, config->y,
        config->width, config->height,
        1, // Border width
        BlackPixel(ctx->display, ctx->screen),
        BlackPixel(ctx->display, ctx->screen));

    if (!ctx->window)
    {
        LOG_ERROR("Failed to create window");
        XCloseDisplay(ctx->display);
        return FE_ERROR_WINDOW_CREATION_FAILED;
    }

    // Set window properties
    set_window_properties(ctx);

    // Select input events
    XSelectInput(ctx->display, ctx->window,
                 ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask |
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask);

    // Map window to screen
    XMapWindow(ctx->display, ctx->window);
    XFlush(ctx->display);

    ctx->should_close = false;
    ctx->is_focused = true;

    LOG_INFO("Window initialized: %s (%dx%d)", config->title, config->width, config->height);
    return FE_SUCCESS;
}

// Poll events
fe_error_t window_poll_event(window_context_t *ctx, window_event_t *event)
{
    if (!ctx || !event)
    {
        LOG_ERROR("Invalid parameter: ctx or event is NULL");
        return FE_ERROR_INVALID_PARAMETER;
    }

    event->type = WINDOW_EVENT_NONE;

    if (XPending(ctx->display))
    {
        XEvent xev;
        XNextEvent(ctx->display, &xev);

        switch (xev.type)
        {
        case ConfigureNotify:
        {
            XConfigureEvent *cev = &xev.xconfigure;
            event->type = WINDOW_EVENT_RESIZE;
            event->data.resize.width = cev->width;
            event->data.resize.height = cev->height;
            ctx->config.width = cev->width;
            ctx->config.height = cev->height;
            LOG_DEBUG("Window resized: %dx%d", cev->width, cev->height);
            break;
        }
        case KeyPress:
        {
            event->type = WINDOW_EVENT_KEY_PRESS;
            event->data.key.key = XLookupKeysym(&xev.xkey, 0);
            event->data.key.state = xev.xkey.state;
            XLookupString(&xev.xkey, event->data.key.text, sizeof(event->data.key.text), NULL, NULL);
            LOG_DEBUG("Key pressed: %lu", event->data.key.key);
            break;
        }
        case KeyRelease:
        {
            event->type = WINDOW_EVENT_KEY_RELEASE;
            event->data.key.key = XLookupKeysym(&xev.xkey, 0);
            event->data.key.state = xev.xkey.state;
            XLookupString(&xev.xkey, event->data.key.text, sizeof(event->data.key.text), NULL, NULL);
            LOG_DEBUG("Key released: %lu", event->data.key.key);
            break;
        }
        case ButtonPress:
        {
            event->type = WINDOW_EVENT_MOUSE_BUTTON_PRESS;
            event->data.mouse.x = xev.xbutton.x;
            event->data.mouse.y = xev.xbutton.y;
            event->data.mouse.button = xev.xbutton.button;
            LOG_DEBUG("Mouse button pressed: %d at (%d, %d)", event->data.mouse.button, event->data.mouse.x, event->data.mouse.y);
            break;
        }
        case ButtonRelease:
        {
            event->type = WINDOW_EVENT_MOUSE_BUTTON_RELEASE;
            event->data.mouse.x = xev.xbutton.x;
            event->data.mouse.y = xev.xbutton.y;
            event->data.mouse.button = xev.xbutton.button;
            LOG_DEBUG("Mouse button released: %d at (%d, %d)", event->data.mouse.button, event->data.mouse.x, event->data.mouse.y);
            break;
        }
        case MotionNotify:
        {
            event->type = WINDOW_EVENT_MOUSE_MOVE;
            event->data.mouse.x = xev.xmotion.x;
            event->data.mouse.y = xev.xmotion.y;
            LOG_DEBUG("Mouse moved to: (%d, %d)", event->data.mouse.x, event->data.mouse.y);
            break;
        }
        case FocusIn:
        {
            event->type = WINDOW_EVENT_FOCUS_GAINED;
            ctx->is_focused = true;
            LOG_DEBUG("Window gained focus");
            break;
        }
        case FocusOut:
        {
            event->type = WINDOW_EVENT_FOCUS_LOST;
            ctx->is_focused = false;
            LOG_DEBUG("Window lost focus");
            break;
        }
        case ClientMessage:
        {
            if ((Atom)xev.xclient.data.l[0] == ctx->wm_delete_window)
            {
                event->type = WINDOW_EVENT_CLOSE;
                ctx->should_close = true;
                LOG_INFO("Window close requested");
            }
            break;
        }
        default:
            break;
        }
    }

    return FE_SUCCESS;
}

// Resize window
fe_error_t window_resize(window_context_t *ctx, int width, int height)
{
    if (!ctx)
    {
        LOG_ERROR("Invalid parameter: ctx is NULL");
        return FE_ERROR_INVALID_PARAMETER;
    }

    if (width < WINDOW_MIN_WIDTH || height < WINDOW_MIN_HEIGHT)
    {
        LOG_ERROR("Invalid window size: %dx%d (min: %dx%d)", width, height, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
        return FE_ERROR_INVALID_PARAMETER;
    }

    XResizeWindow(ctx->display, ctx->window, width, height);
    ctx->config.width = width;
    ctx->config.height = height;
    XFlush(ctx->display);
    LOG_INFO("Window resized to: %dx%d", width, height);
    return FE_SUCCESS;
}

// Destroy window
void window_destroy(window_context_t *ctx)
{
    if (!ctx)
        return;

    if (ctx->window)
    {
        XDestroyWindow(ctx->display, ctx->window);
        LOG_INFO("Window destroyed");
    }
    if (ctx->display)
    {
        XCloseDisplay(ctx->display);
        LOG_INFO("X11 display closed");
    }
    ctx->window = 0;
    ctx->display = NULL;
}

// Swap buffers (not used in pure X11, placeholder for compatibility)
void window_swap_buffers(window_context_t *ctx)
{
    if (ctx)
    {
        XFlush(ctx->display);
    }
}

// Check if window should close
bool window_should_close(const window_context_t *ctx)
{
    return ctx ? ctx->should_close : true;
}

// Check if window is focused
bool window_is_focused(const window_context_t *ctx)
{
    return ctx ? ctx->is_focused : false;
}

// Get default window config
window_config_t window_get_config(void)
{
    window_config_t config = {
        .width = WINDOW_DEFAULT_WIDTH,
        .height = WINDOW_DEFAULT_HEIGHT,
        .x = 0,
        .y = 0,
        .title = APP_NAME,
        .resizable = true};
    return config;
}