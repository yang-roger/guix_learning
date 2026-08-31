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
/*    _gx_display_driver_4bpp_horizontal_pattern_line_draw                */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Horizontal pattern line draw function for 4bpp display driver.      */
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
void _gx_display_driver_4bpp_horizontal_pattern_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos)
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
GX_UBYTE  start_mask;

    /* pick up start address of canvas memory */
    rowstart = (GX_UBYTE *)context->memory;

    pos = context->pitch * ypos + xstart;

    /* pick up the requested pattern and mask */
    pattern = context->brush.line_pattern;
    mask = context->brush.pattern_mask;
    on_color = (GX_UBYTE)context->brush.line_color & 0x0f;
    on_color |= (GX_UBYTE)(on_color << 4);
    off_color = (GX_UBYTE)context->brush.fill_color & 0x0f;
    off_color |= (GX_UBYTE)(off_color << 4);

    put = rowstart;
    put +=  pos >> 1;
    if (pos & 0x01)
    {
        start_mask = 0x0f;
    }
    else
    {
        start_mask = 0xf0;
    }

    for (column = 0; column < len; column++)
    {
        /* Set bits to 0 first */
        *put &= (GX_UBYTE)(~start_mask);
        /* Set bits color */
        if (pattern & mask)
        {
            *put |= (on_color & start_mask);
        }
        else
        {
            *put |= (off_color & start_mask);
        }

        start_mask >>= 4;
        if (start_mask == 0)
        {
            put++;
            start_mask = 0xf0;
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

