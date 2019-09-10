#include <libsystem/iostream.h>
#include <libsystem/messaging.h>
#include <libsystem/logger.h>
#include <libsystem/system.h>
#include <libsystem/process.h>

#include <libdevice/framebuffer.h>

#include "doomgeneric.h"

static uint started_tick;

static framebuffer_mode_info_t mode_info = {true, DOOMGENERIC_RESX, DOOMGENERIC_RESY};
static iostream_t *framebuffer_device;

void DG_Init()
{
    TRACE_FUNCTION_BEGIN;

    started_tick = system_get_ticks();

    framebuffer_device = iostream_open(FRAMEBUFFER_DEVICE, IOSTREAM_READ);

    if (framebuffer_device == NULL)
    {
        error_print("Failled to open " FRAMEBUFFER_DEVICE);
        return -1;
    }

    if (iostream_call(framebuffer_device, FRAMEBUFFER_CALL_SET_MODE, &mode_info) < 0)
    {
        error_print("Ioctl to " FRAMEBUFFER_DEVICE " failled");
        return -1;
    }

    TRACE_FUNCTION_END;
}

void DG_DrawFrame()
{
    TRACE_FUNCTION_BEGIN;

    iostream_call(framebuffer_device, FRAMEBUFFER_CALL_BLIT, DG_ScreenBuffer);

    TRACE_FUNCTION_END;
}

void DG_SleepMs(uint32_t ms)
{
    TRACE_FUNCTION_BEGIN;

    process_sleep(ms);

    TRACE_FUNCTION_END;
}

uint32_t DG_GetTicksMs()
{
    TRACE_FUNCTION_BEGIN;

    return system_get_ticks() - started_tick;

    TRACE_FUNCTION_END;
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
    TRACE_FUNCTION_BEGIN;
    TRACE_FUNCTION_END;

    return 0;
}

void DG_SetWindowTitle(const char * title)
{
    TRACE_FUNCTION_BEGIN;
    TRACE_FUNCTION_END;
}
