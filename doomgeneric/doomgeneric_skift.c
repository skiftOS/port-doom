#include <libgraphic/framebuffer.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>
#include <libsystem/process/Process.h>
#include <libsystem/system.h>

#include "doomgeneric.h"

static uint started_tick;
static framebuffer_t *framebuffer;

void DG_Init()
{
    started_tick = system_get_ticks();

    framebuffer = framebuffer_open();
    framebuffer_set_mode(framebuffer, DOOMGENERIC_RESX, DOOMGENERIC_RESY);
}

void DG_DrawFrame()
{
    for (size_t i = 0; i < DOOMGENERIC_RESX * DOOMGENERIC_RESY; i++)
    {
        Color color;
        color.packed = DG_ScreenBuffer[i];

        framebuffer->backbuffer->pixels[i].R = color.B;
        framebuffer->backbuffer->pixels[i].G = color.G;
        framebuffer->backbuffer->pixels[i].B = color.R;
        framebuffer->backbuffer->pixels[i].A = color.A;
    }

    framebuffer_blit(framebuffer);
}

void DG_SleepMs(uint32_t ms)
{
    process_sleep(ms);
}

uint32_t DG_GetTicksMs()
{

    return system_get_ticks() - started_tick;
}

int DG_GetKey(int *pressed, unsigned char *doomKey)
{

    return 0;
}

void DG_SetWindowTitle(const char *title)
{
}
