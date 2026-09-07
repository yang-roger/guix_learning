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

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_16bpp_horizontal_pattern_line_draw               */
/*                                                                        */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 16bpp color format horizontal pattern line draw function.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xstart                                x-coord of left endpoint      */
/*    xend                                  x-coord of right endpoint     */
/*    ypos                                  y-coord of line top           */
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
void _gx_display_driver_16bpp_horizontal_pattern_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos)
{
INT     column;
USHORT *put;
USHORT *rowstart;
ULONG   pattern;
ULONG   mask;
USHORT  on_color;
USHORT  off_color;

INT     len = xend - xstart + 1;

    /* pick up start address of canvas memory */
    rowstart = (USHORT *)context->memory;

    GX_CALCULATE_PUTROW(rowstart, xstart, ypos, context);

    /* draw 1-pixel hi lines to fill width */

    /* pick up the requested pattern and mask */
    pattern = context->brush.line_pattern;
    mask = context->brush.pattern_mask;
    on_color = (USHORT)context->brush.line_color;
    off_color = (USHORT)context->brush.fill_color;

    put = rowstart;

    /* draw one line, left to right */
    for (column = 0; column < len; column++)
    {
        if (pattern & mask)
        {
            *put++ = on_color;
        }
        else
        {
            *put++ = off_color;
        }
        mask >>= 1;
        if (!mask)
        {
            mask = 0x80000000;
        }
    }

    /* save current masks value back to brush */
    context->brush.pattern_mask = mask;
}

