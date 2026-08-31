
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

#if defined(GX_MOUSE_SUPPORT)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_drawing_complete                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function provides default drawing complete operatoin for       */
/*    generic display driver instance. Performs mouse cursor drawing      */
/*    if mouse support is enabled.                                        */
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
void _gx_display_driver_generic_drawing_complete(GX_DISPLAY *display, GX_CANVAS *canvas)
{
GX_CANVAS      *mouse_canvas;

    if (display->mouse.cursor_info)
    {
        mouse_canvas = display->mouse.canvas;

        if (canvas == mouse_canvas)
        {
            if (canvas->draw_nesting == 1)
            {
                if ((display->mouse.status & (GX_MOUSE_ENABLED | GX_MOUSE_VISIBLE)) == GX_MOUSE_ENABLED)
                {
                    display->mouse_capture(display);
                    display->mouse_draw(display);
                }
            }
        }
    }
}
#endif
#endif

