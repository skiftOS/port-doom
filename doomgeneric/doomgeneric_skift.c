#include "doomgeneric.h"
#include "doomkeys.h"

#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/System.h>
#include <libsystem/process/Process.h>
#include <libsystem/utils/RingBuffer.h>
#include <libwidget/Application.h>

/* --- Doom widget ---------------------------------------------------------- */

#define KEYQUEUE_SIZE 64

typedef struct
{
    Widget widget;
    Bitmap *framebuffer;
    RingBuffer *keybuffer;
} DoomWidget;

void doom_widget_paint(DoomWidget *widget, Painter *painter, Rectangle rectangle)
{
    painter_blit_bitmap_no_alpha(
        painter,
        widget->framebuffer,
        bitmap_bound(widget->framebuffer),
        widget_get_bound(widget));
}

static int convertToDoomKey(Key key)
{
    int doom_key = 0;

    switch (key)
    {
    case KEYBOARD_KEY_ENTER:
        doom_key = KEY_ENTER;
        break;
    case KEYBOARD_KEY_ESC:
        doom_key = KEY_ESCAPE;
        break;
    case KEYBOARD_KEY_LEFT:
        doom_key = KEY_LEFTARROW;
        break;
    case KEYBOARD_KEY_RIGHT:
        doom_key = KEY_RIGHTARROW;
        break;
    case KEYBOARD_KEY_UP:
        doom_key = KEY_UPARROW;
        break;
    case KEYBOARD_KEY_DOWN:
        doom_key = KEY_DOWNARROW;
        break;
    case KEYBOARD_KEY_LCTRL:
    case KEYBOARD_KEY_RCTRL:
        doom_key = KEY_FIRE;
        break;
    case KEYBOARD_KEY_SPACE:
        doom_key = KEY_USE;
        break;
    case KEYBOARD_KEY_LSHIFT:
    case KEYBOARD_KEY_RSHIFT:
        doom_key = KEY_RSHIFT;
        break;
    default:
        doom_key = 'y';
        break;
    }

    return doom_key;
}

void doom_widget_event(DoomWidget *widget, Event *event)
{
    if (event->type == EVENT_KEYBOARD_KEY_PRESS)
    {
        logger_trace("Press");
        int key_data = convertToDoomKey(event->keyboard.key);
        key_data = (1 << 8) | key_data;
        ringbuffer_write(widget->keybuffer, (const char *)&key_data, sizeof(key_data));
        event->accepted = true;
    }

    if (event->type == EVENT_KEYBOARD_KEY_RELEASE)
    {
        logger_trace("Release");
        int key_data = convertToDoomKey(event->keyboard.key);
        ringbuffer_write(widget->keybuffer, (const char *)&key_data, sizeof(key_data));

        event->accepted = true;
    }
}

void doom_widget_destroy(DoomWidget *widget)
{
    bitmap_destroy(widget->framebuffer);
    ringbuffer_destroy(widget->keybuffer);
}

WidgetClass doom_widget_class = {
    .name = "Doom",

    .destroy = (WidgetDestroyCallback)doom_widget_destroy,
    .paint = (WidgetPaintCallback)doom_widget_paint,
    .event = (WidgetEventCallback)doom_widget_event,
};

DoomWidget *doom_widget_create(Widget *parent)
{
    DoomWidget *widget = __create(DoomWidget);

    widget_initialize(WIDGET(widget), &doom_widget_class, parent);

    widget->framebuffer = bitmap_create(DOOMGENERIC_RESX, DOOMGENERIC_RESY);
    widget->keybuffer = ringbuffer_create(KEYQUEUE_SIZE * sizeof(int));

    return widget;
}

/* --- Doom generic stuff --------------------------------------------------- */

static uint started_tick;
static Window *g_window;
static DoomWidget *g_doom_widget;

void DG_Init()
{
    started_tick = system_get_ticks();

    application_initialize(0, NULL);

    g_window = window_create(
        "application",
        "Doom",
        DOOMGENERIC_RESX,
        DOOMGENERIC_RESY,
        WINDOW_RESIZABLE);

    g_doom_widget = doom_widget_create(window_root(g_window));

    window_set_focused_widget(g_window, g_doom_widget);

    window_show(g_window);
}

void DG_DrawFrame()
{
    for (size_t i = 0; i < DOOMGENERIC_RESX * DOOMGENERIC_RESY; i++)
    {
        Color color;
        color.packed = DG_ScreenBuffer[i];

        g_doom_widget->framebuffer->pixels[i].R = color.B;
        g_doom_widget->framebuffer->pixels[i].G = color.G;
        g_doom_widget->framebuffer->pixels[i].B = color.R;
        g_doom_widget->framebuffer->pixels[i].A = 255;
    }

    widget_update(WIDGET(g_doom_widget));

    application_pump();
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
    if (ringbuffer_is_empty(g_doom_widget->keybuffer))
    {
        return 0;
    }

    int key_data;
    ringbuffer_read(g_doom_widget->keybuffer, (const char *)&key_data, sizeof(key_data));

    *pressed = key_data >> 8;
    *doomKey = key_data & 0xFF;

    return 1;
}

void DG_SetWindowTitle(const char *title)
{
}
