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
/*    _gx_display_driver_8bpp_rotated_vertical_line_draw                  */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 8bpp color format rotated vertical line draw function.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    ystart                                y-coord of top endpoint       */
/*    yend                                  y-coord of bottom endpoint    */
/*    xpos                                  x-coord of left edge          */
/*    width                                 width of the line             */
/*    color                                 Color of line to draw         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
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
void _gx_display_driver_8bpp_rotated_vertical_line_draw(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos, INT width, GX_COLOR color)
{
INT       row;
INT       column;
GX_UBYTE *put;
GX_UBYTE *rowstart;
INT       len = yend - ystart + 1;

    /* Pick up starting address of canvas memory.  */
    rowstart = (GX_UBYTE *)context->memory;

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
            *put++ = (GX_UBYTE)color;
        }

        /* Ddvance to the next scaneline.  */
        rowstart -= context->pitch;
    }
}

