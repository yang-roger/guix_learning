
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
#include "gx_utility.h"

#if defined(GX_MOUSE_SUPPORT)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_drawing_initiate                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initiates drawing on the specified display. If mouse  */
/*    support is enable, this function implements hiding the mouse cursor */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
/*    canvas                                Canvas control block          */
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
#if !defined (GX_HARDWARE_MOUSE_SUPPORT)
void _gx_display_driver_generic_drawing_initiate(GX_DISPLAY *display, GX_CANVAS *canvas)
{
GX_RECTANGLE    mouse_rect;
GX_CANVAS      *mouse_canvas;

    if (display->mouse.cursor_info)
    {
        mouse_canvas = display->mouse.canvas;
        if (canvas == mouse_canvas)
        {
            if (canvas->draw_nesting == 1 && (display->mouse.status & GX_MOUSE_VISIBLE))
            {
                if (gx_rectangle_intersect_(display->mouse.rect, canvas->dirty_area, &mouse_rect))
                {
                    /* restore captured data */
                    display->mouse_restore(display);
                }
            }
        }
    }
}
#endif
#endif
