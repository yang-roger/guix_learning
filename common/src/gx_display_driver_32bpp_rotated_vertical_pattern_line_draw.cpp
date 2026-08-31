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
/*    _gx_display_driver_32bpp_rotated_vertical_pattern_line_draw         */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic rotated 32bpp color format vertical pattern line draw       */
/*    function.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    ystart                                y-coord of top endpoint       */
/*    yend                                  y-coord of bottom endpoint    */
/*    xpos                                  x-coord of left edge          */
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
void _gx_display_driver_32bpp_rotated_vertical_pattern_line_draw(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos)
{
INT    row;
ULONG *put;
ULONG *rowstart;
ULONG  pattern;
ULONG  mask;
ULONG  on_color;
ULONG  off_color;
INT    len = yend - ystart + 1;

    /* Pick up starting address of canvas memory.  */
    rowstart =  (ULONG *)context->memory;

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        /* Calculate start of scanline.  */
        rowstart += (context->canvas->x_resolution - xpos - 1) * context->pitch;

        /* Offset into starting pixel.  */
        rowstart += ystart;
    }
    else
    {
        /* Calculate start of scanline.  */
        rowstart += xpos * context->pitch;

        /* Offset into starting pixel.  */
        rowstart += (context->canvas->y_resolution - yend - 1);
    }

    /* Pick up the requested pattern and mask.  */
    pattern = context->brush.line_pattern;
    mask = context->brush.pattern_mask;
    on_color = (ULONG)context->brush.line_color;
    off_color = (ULONG)context->brush.fill_color;

    put = rowstart;

    /* Draw line from top to bottom.  */
    for (row = 0; row < len; row++)
    {
        if (pattern & mask)
        {
            *put = on_color;
        }
        else
        {
            *put = off_color;
        }

        mask >>= 1;
        if (!mask)
        {
            mask = 0x80000000;
        }

        /* Advance to the next scaneline.  */
        put++;
    }
    /* Save current masks value back to brush.  */
    context->brush.pattern_mask = mask;
}

