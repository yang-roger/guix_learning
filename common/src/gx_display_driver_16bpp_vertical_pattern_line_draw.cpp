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
/*    _gx_display_driver_16bpp_vertical_pattern_line_draw                 */
/*                                                                        */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 16bpp color format vertical pattern line draw function.     */
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
void _gx_display_driver_16bpp_vertical_pattern_line_draw(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos)
{
INT     row;
USHORT *put;
USHORT *rowstart;
ULONG   pattern;
ULONG   mask;
USHORT  on_color;
USHORT  off_color;

INT     len = yend - ystart + 1;

    /* pick up starting address of canvas memory */
    rowstart =  (USHORT *)context->memory;

    GX_CALCULATE_PUTROW(rowstart, xpos, ystart, context);

    /* pick up the requested pattern and mask */
    pattern = context->brush.line_pattern;
    mask = context->brush.pattern_mask;
    on_color = (USHORT)context->brush.line_color;
    off_color = (USHORT)context->brush.fill_color;

    /* draw line from top to bottom */
    for (row = 0; row < len; row++)
    {
        put = rowstart;

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

        /* advance to the next scaneline */
        rowstart +=  context->pitch;
    }
    /* save current masks value back to brush */
    context->brush.pattern_mask = mask;
}

