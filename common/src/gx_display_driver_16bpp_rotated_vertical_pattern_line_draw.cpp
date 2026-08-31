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
/*    _gx_display_driver_16bpp_rotated_vertical_pattern_line_draw         */
/*                                                                        */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 16bpp color format rotated vertical pattern line draw       */
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
void _gx_display_driver_16bpp_rotated_vertical_pattern_line_draw(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos)
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
    rowstart = (USHORT *)context->memory;

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        /* calculate start of scanline */
        rowstart += (context->canvas->x_resolution - xpos - 1) * context->pitch;

        /* offset into starting pixel */
        rowstart += ystart;
    }
    else
    {
        /* calculate start of scanline */
        rowstart += xpos * context->pitch;

        /* offset into starting pixel */
        rowstart += (context->canvas->y_resolution - yend - 1);
    }

    /* pick up the requested pattern and mask */
    pattern = context->brush.line_pattern;
    mask = context->brush.pattern_mask;
    on_color = (USHORT)context->brush.line_color;
    off_color = (USHORT)context->brush.fill_color;

    put = rowstart;

    /* draw line from left to right */
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

        /* advance to the right */
        put++;
    }

    /* save current masks value back to brush */
    context->brush.pattern_mask = mask;
}

