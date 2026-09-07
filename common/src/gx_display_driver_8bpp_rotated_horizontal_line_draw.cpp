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
/*    _gx_display_driver_8bpp_rotated_horizontal_line_draw                */
/*                                                                        */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 8bpp color format rotated horizontal line draw function.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xstart                                x-coord of left endpoint      */
/*    xend                                  x-coord of right endpoint     */
/*    ypos                                  y-coord of line top           */
/*    width                                 Width (height) of the line    */
/*    color                                 Color of line to write        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    NOne                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_8bpp_rotated_horizontal_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos, INT width, GX_COLOR color)
{
INT       row;
INT       column;
GX_UBYTE *put;
GX_UBYTE *rowstart;
INT       len = xend - xstart + 1;

    /* Pick up start address of canvas memory.  */
    rowstart = (GX_UBYTE *)context->memory;

    /* Calculate start of row address.  */
    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        rowstart += (context->canvas->x_resolution - xstart - 1) * context->pitch;
        rowstart += ypos;
    }
    else
    {
        rowstart += xend * context->pitch;
        rowstart += (context->canvas->y_resolution - ypos - width);
    }

    /* Draw one line, left to right.  */
    for (column = 0; column < len; column++)
    {
        put = rowstart;

        /* Draw pixel to fill width.  */
        for (row = 0; row < width; row++)
        {
            *put++ = (GX_UBYTE)color;
        }
        rowstart -= context->pitch;
    }
}

