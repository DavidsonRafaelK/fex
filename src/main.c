#include "../include/window.h"
#include "../include/common.h"

#include <unistd.h>

int main(void)
{
    log_message(LOG_LEVEL_DEBUG, __FILE__, __LINE__, "Debugging information");

    window_config_t config = window_get_config();
    window_context_t ctx = {0};

    fe_error_t err = window_init(&ctx, &config);
    if (err != FE_SUCCESS)
    {
        LOG_ERROR("Failed to initialize window: %s", fe_strerror(err));
        return EXIT_FAILURE;
    }

    // Main loop
    while (!window_should_close(&ctx))
    {
        window_event_t event;
        while (window_poll_event(&ctx, &event) == FE_SUCCESS && event.type != WINDOW_EVENT_NONE)
        {
            switch (event.type)
            {
            case WINDOW_EVENT_CLOSE:
                LOG_INFO("Close event received");
                ctx.should_close = true;
                break;
            case WINDOW_EVENT_KEY_PRESS:
                if (event.data.key.key == XK_Escape)
                {
                    LOG_INFO("Escape key pressed, closing window");
                    ctx.should_close = true;
                }
                break;
            case WINDOW_EVENT_RESIZE:
                LOG_INFO("Window resized to: %dx%d", event.data.resize.width, event.data.resize.height);
                break;
            default:
                break;
            }
        }

        usleep(FRAME_TIME * 1000);
    }

    // Cleanup
    window_destroy(&ctx);
    return EXIT_SUCCESS;
}