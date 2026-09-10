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

#include "partial_frame_buffer_16bpp_resources.h"
#include "partial_frame_buffer_16bpp_specifications.h"

extern VOID memory_free(VOID *mem);

#define RATIO_ONE 100

INT x_ratio = RATIO_ONE;
INT y_ratio = RATIO_ONE;

GX_RESOURCE_ID   resize_pixelmap_id = GX_PIXELMAP_ID_ROTATE_FOOT;

VOID resize_ratio_format(GX_NUMERIC_PROMPT *prompt, INT value)
{
    INT index = 0;

    /* Transfer float value to string. */
    prompt->buffer[index++] = '0' + (value / RATIO_ONE);
    prompt->buffer[index++] = '.';
    prompt->buffer[index++] = '0' + (value % RATIO_ONE) / 10;
    prompt->buffer[index++] = '0' + (value % RATIO_ONE) % 10;
    prompt->buffer[index] = '\0';
}

UINT resize_screen_event_handler(GX_WINDOW *window, GX_EVENT *myevent)
{
    switch (myevent->type)
    {
    case GX_SIGNAL(ID_SLIDER_WIDTH, GX_EVENT_SLIDER_VALUE):
        x_ratio = myevent->payload.longdata;
        gx_numeric_prompt_value_set(&resize_screen.resize_screen_width, x_ratio);
        gx_system_dirty_mark((GX_WIDGET *)&resize_screen.resize_screen_resize_window);
        break;

    case GX_SIGNAL(ID_SLIDER_HEIGHT, GX_EVENT_SLIDER_VALUE):
        y_ratio = myevent->payload.longdata;
        gx_numeric_prompt_value_set(&resize_screen.resize_screen_height, y_ratio);
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
        return gx_window_event_process(window, myevent);
    }
    return 0;
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

    width = (pixelmap->width * x_ratio / RATIO_ONE);
    height = (pixelmap->height * y_ratio / RATIO_ONE);

    if (x_ratio == RATIO_ONE && y_ratio == RATIO_ONE)
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
            memory_free((VOID *)destination.data);
        }

        if (destination.aux_data)
        {
            memory_free((VOID *)destination.aux_data);
        }
    }
}
