
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
/*    _gx_display_driver_4bpp_mouse_capture                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service captures memory under mouse position.                  */
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
void _gx_display_driver_4bpp_mouse_capture(GX_DISPLAY  *display)
{
INT            width;
INT            height;
INT            row;
INT            column;
GX_UBYTE      *put;
GX_UBYTE      *putrow;
GX_UBYTE      *getrow;
GX_UBYTE      *get;
GX_RECTANGLE  *mouse_rect;
GX_UBYTE       getmask;
GX_UBYTE       putmask = 0xf0;
GX_UBYTE       pixel;
GX_CANVAS     *canvas;

    if (display->mouse.cursor_info)
    {
        if (display->mouse.capture_memory)
        {
            mouse_rect = _gx_display_driver_mouse_rectangle_set(display, &width, &height);
            if (mouse_rect)
            {
                canvas = display->mouse.canvas;

                getrow = (GX_UBYTE *)canvas->memory;
                getrow += ((canvas->x_resolution + 1) >> 1) * mouse_rect->top;
                getrow += mouse_rect->left >> 1;
                putrow = (GX_UBYTE *)display->mouse.capture_memory;

                for (row = 0; row < height; row++)
                {
                    if (mouse_rect->left & 1)
                    {
                        getmask = 0x0f;
                    }
                    else
                    {
                        getmask = 0xf0;
                    }
                    get = getrow;
                    put = putrow;
                    putmask = 0xf0;

                    for (column = 0; column < width; column++)
                    {
                        pixel = (*get) & getmask;
                        if (getmask == 0x0f)
                        {
                            get++;
                            getmask = 0xf0;
                            pixel |= (GX_UBYTE)(pixel << 4);
                        }
                        else
                        {
                            getmask = 0x0f;
                            pixel |= pixel >> 4;
                        }

                        *put &= (GX_UBYTE)(~putmask);
                        *put |= pixel & putmask;
                        putmask >>= 4;
                        if (putmask == 0)
                        {
                            putmask = 0xf0;
                            put++;
                        }
                    }
                    getrow += (canvas->x_resolution + 1) >> 1;
                    putrow += (width + 1) >> 1;
                }
            }
        }
    }
}
#endif
#endif

