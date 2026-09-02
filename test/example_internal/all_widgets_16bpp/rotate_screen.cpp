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

/****************************************************************/
/* This file tests gx_utility_4444argb_roate and                */
/*                 _gx_display_driver_16bpp_pixelmap_rotate     */
/*                                                              */
/****************************************************************/
#include <stdio.h>
#include "gx_api.h"
#include "all_widgets_16bpp_resources.h"
#include "all_widgets_16bpp_specifications.h"

extern GX_CONST GX_PIXELMAP *Main_Display_theme_1_pixelmap_table[];
extern VOID                  rotate_memory_free(VOID *mem);

extern GX_WINDOW     *pRotateScreen;
extern GX_WINDOW     *pRotateWin;
INT                   angle = 90;
INT                   rotate_pixelmap_index = 0;
GX_RESOURCE_ID        rotate_pixelmap_id[] = { GX_PIXELMAP_ID_ROTATE_FOOT, GX_PIXELMAP_ID_ROTATE_APPLE, GX_PIXELMAP_ID_ROTATE_FISH, GX_PIXELMAP_ID_BLACK_PAUSE_ALPHAMAP};
GX_BOOL               direct = GX_FALSE;

VOID update_angle_prompt()
{
    GX_PROMPT *pp;
    static GX_CHAR rotate_angle[10];
    GX_STRING string;

    gx_widget_find(pRotateScreen, ID_ANGLE_DISPLAY, 0, &pp);

    if (pp)
    {
        gx_utility_ltoa(angle, rotate_angle, 10);
        string.ptr = rotate_angle;
        string.length = strnlen(rotate_angle, sizeof(rotate_angle));
        gx_prompt_text_set_ext(pp, &string);
    }
}

UINT rotate_event_handler(GX_WINDOW *window, GX_EVENT *myevent)
{
UINT status = 0;

    switch (myevent->type)
    {
    case GX_SIGNAL(ID_SLIDER_ANGLE, GX_EVENT_SLIDER_VALUE):
        angle = myevent->payload.longdata;
        update_angle_prompt();
        gx_system_dirty_mark(pRotateScreen);
        break;

    case GX_SIGNAL(ID_RADIO_FOOT, GX_EVENT_RADIO_SELECT):
        rotate_pixelmap_index = 0;
        gx_system_dirty_mark(pRotateScreen);
        break;

    case GX_SIGNAL(ID_RADIO_APPLE, GX_EVENT_RADIO_SELECT):
        rotate_pixelmap_index = 1;
        gx_system_dirty_mark(pRotateScreen);
        break;

    case GX_SIGNAL(ID_RADIO_FISH, GX_EVENT_RADIO_SELECT):
        rotate_pixelmap_index = 2;
        gx_system_dirty_mark(pRotateScreen);
        break;
        
    case GX_SIGNAL(ID_DIRECT_ROTATE, GX_EVENT_TOGGLE_ON):
        direct = GX_TRUE;
        gx_system_dirty_mark(pRotateScreen);
        break;

    case GX_SIGNAL(ID_DIRECT_ROTATE, GX_EVENT_TOGGLE_OFF):
        direct = GX_FALSE;
        gx_system_dirty_mark(pRotateScreen);
        break;

    default:
        return next_button_handler(window, myevent);
        break;
    }
    return status;
}

VOID pixelmap_draw(GX_WINDOW *window)
{
INT  xpos;
INT  ypos;
GX_PIXELMAP *pixelmap;
GX_RECTANGLE win_size;
GX_PIXELMAP destination;
INT x_axis_old;
INT y_axis_old;
INT x_axis;
INT y_axis;

    gx_window_draw((GX_WINDOW*)window);

    pixelmap = (GX_PIXELMAP *)Main_Display_theme_1_pixelmap_table[rotate_pixelmap_id[rotate_pixelmap_index]];
    win_size = pRotateWin->size;

    xpos = (win_size.right + win_size.left - pixelmap->width) >> 1;
    ypos = (win_size.top + win_size.bottom - pixelmap->height) >> 1;

    if (direct)
    {
        gx_canvas_pixelmap_rotate(xpos, ypos, pixelmap, angle, -1, -1);
    }
    else
    {
        x_axis_old = (pixelmap->width >> 1);
        y_axis_old = (pixelmap->height >> 1);

        x_axis = x_axis_old;
        y_axis = y_axis_old;

        if (gx_utility_pixelmap_rotate(pixelmap, angle, &destination, &x_axis, &y_axis) == GX_SUCCESS)
        {
            xpos += x_axis_old - x_axis;
            ypos += y_axis_old - y_axis;

            /* Draw rotated pixelmap.  */
            gx_canvas_pixelmap_draw(xpos, ypos, &destination);

            /* free memory of the rotated pixelmap */
            if (destination.data)
            {
                rotate_memory_free((VOID *)destination.data);
            }
            if (destination.aux_data)
            {
                rotate_memory_free((VOID *)destination.aux_data);
            }
        }
        else
        {
            gx_canvas_pixelmap_draw(xpos, ypos, pixelmap);
        }
    }
}
