
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

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_mouse_restore                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service restores captured memory under the mouse area.         */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#if defined(GX_MOUSE_SUPPORT)
#if !defined(GX_HARDWARE_MOUSE_SUPPORT)
void _gx_display_driver_24xrgb_mouse_restore(GX_DISPLAY *display)
{
INT        row;
INT        column;
INT        height;
INT        width;
GX_COLOR  *putrow;
GX_COLOR  *put;
GX_COLOR  *get;
GX_CANVAS *canvas;

    if(display->mouse.cursor_info)
    {
        if (display->mouse.capture_memory &&
            (display->mouse.status & GX_MOUSE_VISIBLE))
        {
            canvas = display->mouse.canvas;
            height = display->mouse.rect.bottom - display->mouse.rect.top + 1;
            width = display->mouse.rect.right - display->mouse.rect.left + 1;

            if (width > 0 && height > 0)
            {
                get = (GX_COLOR *)display->mouse.capture_memory;
                putrow = canvas->memory;
                putrow += canvas->x_resolution * display->mouse.rect.top;
                putrow += display->mouse.rect.left;

                for (row = 0; row < height; row++)
                {
                    put = putrow;
                    for (column = 0; column < width; column++)
                    {
                        *put++ = *get++;
                    }
                    putrow += display->width;
                }
            }
        }
        display->mouse.status &= (GX_UBYTE)(~GX_MOUSE_VISIBLE);
    }
}
#endif
#endif

