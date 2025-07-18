#include "../include/window.h"
#include "../include/common.h"
#include "../include/config.h"

window_config_t config = {
    .width = WINDOW_DEFAULT_WIDTH,
    .height = WINDOW_DEFAULT_HEIGHT,
    .x = WINDOW_POSITION_X,
    .y = WINDOW_POSITION_Y,
    .title = WINDOW_TITLE,
    .resizable = WINDOW_RESIZABLE,
};

int main(void)
{
    window_context_t ctx;

    if (window_init(&ctx, &config) != FE_SUCCESS)
    {
        LOG_ERROR("Failed to initialize window");
        return EXIT_FAILURE;
    }

    // Main Loop
    while (!window_should_close(&ctx))
    {
        window_event_t event;
        if (window_poll_event(&ctx, &event) == FE_SUCCESS)
        {
            switch (event.type)
            {
            case WINDOW_EVENT_CLOSE:
                ctx.should_close = true;
                break;
            case WINDOW_EVENT_RESIZE:
                LOG_INFO("Window resized to: %dx%d", event.data.resize.width, event.data.resize.height);
                break;
            default:
                break;
            }
        }
        window_swap_buffers(&ctx);
    }

    window_destroy(&ctx);
    LOG_INFO("Window closed successfully");
    return EXIT_SUCCESS;
}
