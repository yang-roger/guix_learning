
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
#include "gx_pixelmap.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_16bpp_mouse_capture                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service captures canvas memory under mouse position.           */
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
void _gx_display_driver_16bpp_mouse_capture(GX_DISPLAY  *display)
{
INT            width;
INT            height;
INT            row;
INT            column;
USHORT        *put;
USHORT        *getrow;
USHORT        *get;
GX_RECTANGLE  *mouse_rect;
GX_CANVAS     *canvas;

    if (display->mouse.cursor_info)
    {
        if (display->mouse.capture_memory)
        {
            mouse_rect = _gx_display_driver_mouse_rectangle_set(display, &width, &height);
            if (mouse_rect)
            {
                canvas = display->mouse.canvas;

                getrow = (USHORT *)canvas->memory;
                getrow += canvas->x_resolution * mouse_rect->top;
                getrow += mouse_rect->left;
                put = (USHORT *)display->mouse.capture_memory;

                for (row = 0; row < height; row++)
                {
                    get = getrow;
                    for (column = 0; column < width; column++)
                    {
                        *put++ = *get++;
                    }
                    getrow += canvas->x_resolution;
                }
            }
        }
    }
}
#endif
#endif

