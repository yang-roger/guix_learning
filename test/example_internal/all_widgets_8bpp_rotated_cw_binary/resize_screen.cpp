/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* This is a small demo of the high-performance GUIX graphics framework. */

#include <stdio.h>
#include "gx_api.h"
#include "gx_canvas.h"

#include "all_widgets_8bpp_rotated_cw_binary_resources.h"
#include "all_widgets_8bpp_rotated_cw_binary_specifications.h"

extern UINT gx_prompt_text_set(GX_PROMPT *prompt, const GX_CHAR *text);
extern VOID dynamic_memory_free(VOID *mem);

float            x_ratio = 1;
float            y_ratio = 1;
GX_RESOURCE_ID   resize_pixelmap_id = GX_PIXELMAP_ID_ROTATE_FOOT;

VOID update_prompt_text(GX_RESOURCE_ID id, float value)
{
    GX_PROMPT *pp;
    static GX_CHAR text[10];

    gx_widget_find((GX_WIDGET *)&resize_screen, (USHORT)id, 0, &pp);

    if (pp)
    {
        /* Transfer float value to string. */
        text[0] = '0' + (int)(value);
        text[1] = '.';
        text[2] = '0' + (int)(value * 10) % 10;
        text[3] = '0' + (int)(value * 100) % 10;
        text[4] = '\0';

        /* Reset prompt text. */
        gx_prompt_text_set(pp, text);
    }
}

UINT resize_screen_event_handler(GX_WINDOW *window, GX_EVENT *myevent)
{
    UINT status = 0;
    INT  value;

    switch (myevent->type)
    {
    case GX_SIGNAL(ID_SLIDER_WIDTH, GX_EVENT_SLIDER_VALUE):
        value = myevent->payload.longdata;
        value -= 50;
        x_ratio = 1 + (float)value / 100;
        update_prompt_text(ID_WIDTH_DISPLAY, x_ratio);
        gx_system_dirty_mark((GX_WIDGET *)&resize_screen.resize_screen_resize_window);
        break;

    case GX_SIGNAL(ID_SLIDER_HEIGHT, GX_EVENT_SLIDER_VALUE):
        value = myevent->payload.longdata;
        value -= 50;
        y_ratio = 1 + (float)value / 100;
        update_prompt_text(ID_HEIGHT_DISPLAY, y_ratio);
        gx_system_dirty_mark((GX_WIDGET *)&resize_screen.resize_screen_resize_window);
        break;

    case GX_SIGNAL(ID_RADIO_FOOT, GX_EVENT_RADIO_SELECT):
        resize_pixelmap_id = GX_PIXELMAP_ID_ROTATE_FOOT;
        gx_system_dirty_mark((GX_WIDGET *)&resize_screen.resize_screen_resize_window);
        break;

    case GX_SIGNAL(ID_RADIO_FOOT_ALPHA, GX_EVENT_RADIO_SELECT):
        resize_pixelmap_id = GX_PIXELMAP_ID_ROTATE_APPLE;
        gx_system_dirty_mark((GX_WIDGET *)&resize_screen.resize_screen_resize_window);
        break;

    case GX_SIGNAL(ID_RADIO_FISH, GX_EVENT_RADIO_SELECT):
        resize_pixelmap_id = GX_PIXELMAP_ID_ROTATE_FISH;
        gx_system_dirty_mark((GX_WIDGET *)&resize_screen.resize_screen_resize_window);
        break;

    default:
        status = next_button_handler(window, myevent);
        break;
    }
    return status;
}

VOID resize_window_draw(GX_WINDOW *window)
{
    INT  xpos;
    INT  ypos;
    GX_PIXELMAP *pixelmap;
    GX_RECTANGLE win_size;
    GX_PIXELMAP destination;
    INT width, height;

    gx_window_draw((GX_WINDOW*)window);

    gx_context_pixelmap_get(resize_pixelmap_id, &pixelmap);
    win_size = resize_screen.resize_screen_resize_window.size;

    xpos = (win_size.right + win_size.left -pixelmap->width) >> 1;
    ypos = (win_size.top + win_size.bottom - pixelmap->height) >> 1;

    width = (int)(pixelmap->width * x_ratio);
    height = (int)(pixelmap->height * y_ratio);

    if (x_ratio == 1 && y_ratio == 1)
    {
        /* Draw source pixelmap.  */
        gx_canvas_pixelmap_draw(xpos, ypos, pixelmap);
    }
    else if (gx_utility_pixelmap_resize(pixelmap, &destination, width, height) == GX_SUCCESS)
    {
        xpos += (pixelmap->width - destination.width) >> 1;
        ypos += (pixelmap->height - destination.height) >> 1;

        /* Draw resized pixelmap.  */
        gx_canvas_pixelmap_draw(xpos, ypos, &destination);

        if (destination.data)
        {
            dynamic_memory_free((VOID *)destination.data);
        }

        if (destination.aux_data)
        {
            dynamic_memory_free((VOID *)destination.aux_data);
        }
    }
}
