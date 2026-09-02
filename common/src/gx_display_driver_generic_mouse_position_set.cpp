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


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** GUIX Component                                                        */
/**                                                                       */
/**   Dispaly Management (Dispaly)                                        */
/**                                                                       */
/**************************************************************************/

#include "gx_display.h"

#include "gx_utility.h"
#include "gx_canvas.h"

#if defined(GX_MOUSE_SUPPORT)
#if !defined(GX_HARDWARE_MOUSE_SUPPORT)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_mouse_position_set                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the mouse position for software mouse support.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
/*    position                              Position of mouse cursor      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
void  _gx_display_driver_generic_mouse_position_set(GX_DISPLAY *display, GX_POINT *pos)
{
GX_RECTANGLE mouse_rect;
GX_CANVAS   *canvas;

    if (display->mouse.cursor_info)
    {
        canvas = display->mouse.canvas;
        mouse_rect = display->mouse.rect;

        /* First hide the mouse if the area under mouse has been captured */
        if (display->mouse.status & GX_MOUSE_VISIBLE)
        {
            display->mouse_restore(display);
        }

        display->mouse.position = *pos;

        if (display->mouse.status & GX_MOUSE_ENABLED)
        {
            display->mouse_capture(display);
            display->mouse_draw(display);
        }

        if (display->driver_buffer_toggle)
        {
            mouse_rect.combine_(display->mouse.rect);
            canvas->dirty_area = mouse_rect;
            display->driver_buffer_toggle(canvas, &canvas->dirty_area);
        }
    }
}
#endif
#endif

