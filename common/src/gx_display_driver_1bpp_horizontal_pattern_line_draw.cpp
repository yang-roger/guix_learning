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
/*    _gx_display_driver_1bpp_horizontal_pattern_line_draw                */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Horizontal pattern line draw function for 1bpp display driver.      */
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
void _gx_display_driver_1bpp_horizontal_pattern_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos)
{
INT       column;
GX_UBYTE *put;
GX_UBYTE *rowstart;
ULONG     pattern;
ULONG     mask;
GX_UBYTE  on_color;
GX_UBYTE  off_color;
INT       pos;
INT       len = xend - xstart + 1;
INT       start_in_byte = 0;
GX_UBYTE  start_mask;

    /* pick up start address of canvas memory */
    rowstart = (GX_UBYTE *)context->memory;

    pos = context->pitch * ypos + xstart;

    /* pick up the requested pattern and mask */
    pattern = context->brush.line_pattern;
    mask = context->brush.pattern_mask;
    on_color = (GX_UBYTE)context->brush.line_color & 0x01;
    off_color = (GX_UBYTE)context->brush.fill_color & 0x01;

    put = rowstart;
    put +=  pos >> 3;
    start_in_byte = pos & 0x07;
    start_mask = (GX_UBYTE)(((GX_UBYTE)0x80) >> start_in_byte);

    for (column = 0; column < len; column++)
    {
        if (pattern & mask)
        {
            if (on_color == 0x00)
            {
                *put = (GX_UBYTE)((*put) & (~start_mask));
            }
            else
            {
                *put |= start_mask;
            }
        }
        else
        {
            if (off_color == 0x00)
            {
                *put &= (GX_UBYTE)((*put) & (~start_mask));
            }
            else
            {
                *put |= start_mask;
            }
        }
        start_mask >>= 1;
        if (start_mask == 0)
        {
            put++;
            start_mask = 0x80;
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

