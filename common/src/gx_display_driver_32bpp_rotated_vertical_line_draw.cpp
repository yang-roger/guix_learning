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
/**   Display Management (Display)                                        */
/**                                                                       */
/**************************************************************************/

#include "gx_display.h"

#include "gx_context.h"
#include "gx_canvas.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_vertical_line_draw                 */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic rotated vertical line draw function for 16bpp canvas.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    ystart                                y-coord of top endpoint       */
/*    yend                                  y-coord of bottom endpoint    */
/*    xpos                                  x-coord of left edge          */
/*    width                                 width of the line             */
/*    color                                 Color of line to write        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_vertical_line_alpha_draw                         */
/*                                          Basic display driver vertical */
/*                                            line alpha draw function    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_32bpp_rotated_vertical_line_draw(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos, INT width, GX_COLOR color)
{
INT    row;
INT    column;
ULONG *put;
ULONG *rowstart;
INT    len = yend - ystart + 1;

#if defined GX_BRUSH_ALPHA_SUPPORT
GX_UBYTE alpha;

    alpha = context->brush.alpha;
    if (alpha == 0)
    {
        /* Nothing to drawn. Just return.  */
        return;
    }
    if (alpha != 0xff)
    {
        _gx_display_driver_vertical_line_alpha_draw(context, ystart, yend, xpos, width, color, alpha);
        return;
    }
#endif

    /* Pick up starting address of canvas memory.  */
    rowstart = (ULONG *)context->memory;

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {

        /* Calculate start of scanline.  */
        rowstart += (context->canvas->x_resolution - xpos - 1) * context->pitch;

        /* Offset into starting pixel.  */
        rowstart += ystart;
    }
    else
    {
        rowstart += (xpos + width - 1) * context->pitch;
        rowstart += (context->canvas->y_resolution - yend - 1);
    }

    /* Draw line width from left to right.  */
    for (column = 0; column < width; column++)
    {
        put = rowstart;

        /* Draw line from top to bottom.  */
        for (row = 0; row < len; row++)
        {
            *put++ = (ULONG)color;
        }

        /* Advance to the next scaneline.  */
        rowstart -= context->pitch;
    }
}

