
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

#include "gx_canvas.h"
#include "gx_system.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_mouse_draw                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws the mouse image for software mouse.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
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
#if defined(GX_MOUSE_SUPPORT)
#if !defined(GX_HARDWARE_MOUSE_SUPPORT)
void _gx_display_driver_generic_mouse_draw(GX_DISPLAY *display)
{
GX_DRAW_CONTEXT  mouse_context;
GX_DRAW_CONTEXT *old_context;
GX_PIXELMAP     *map;
GX_VALUE         left;
GX_VALUE         top;
GX_RESOURCE_ID   image_id;
GX_CANVAS       *canvas;

    if(display->mouse.cursor_info)
    {
        image_id = display->mouse.cursor_info->image_id;

        if (image_id && image_id < display->pixelmap_table_size)
        {
            canvas = display->mouse.canvas;
            left = display->mouse.position.x;
            top = display->mouse.position.y;
            left = (GX_VALUE)(left - display->mouse.cursor_info->hotspot_x);
            top = (GX_VALUE)(top - display->mouse.cursor_info->hotspot_y);
            map = display->pixelmap_table[image_id];

            old_context = _gx_system_current_draw_context;

            // set up draw context clip area
            mouse_context.dirty = display->mouse.rect;

            // set up draw context view
            mouse_context.simple_view.next = GX_NULL;
            mouse_context.simple_view.rectangle = display->mouse.rect;
            mouse_context.view_head = &mouse_context.simple_view;

            mouse_context.canvas = canvas;
            /* Mouse pixelmap should never been blend. */
            mouse_context.brush.alpha = 0xff;
            mouse_context.display = display;
            mouse_context.memory = canvas->memory;
            mouse_context.pitch = canvas->x_resolution;

            _gx_system_current_draw_context = &mouse_context;
            _gx_canvas_pixelmap_draw(left, top, map);

            _gx_system_current_draw_context = old_context;
            display->mouse.status |= GX_MOUSE_VISIBLE;
        }
    }
}

#endif
#endif

