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

#include "gx_display_driver.h"

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
void _gx_display_driver_16bpp_horizontal_pattern_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT ypos)
{
    // draw 1-pixel height lines to fill width

    // pick up the requested pattern and mask
    ULONG pattern = context->brush.line_pattern;
    ULONG mask = context->brush.pattern_mask;
    USHORT on_color = (USHORT)context->brush.line_color;
    USHORT off_color = (USHORT)context->brush.fill_color;

    // pick up start address of canvas memory
    USHORT* put = (USHORT*)context->memory;
    GX_CALCULATE_PUTROW(put, xstart, ypos, context);

    INT length = xend - xstart + 1;

    /* draw one line, left to right */
    for (INT column = 0; column < length; ++column)
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

    // save current masks value back to brush
    context->brush.pattern_mask = mask;
}

