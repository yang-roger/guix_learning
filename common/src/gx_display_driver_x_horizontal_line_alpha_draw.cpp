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

#if defined(GX_BRUSH_ALPHA_SUPPORT)

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_horizontal_line_alpha_draw                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that draws an alpha blended horizontal     */
/*    line.                                                               */
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
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_8bpp_horizontal_line_draw                        */
/*    _gx_display_driver_16bpp_horizontal_line_draw                       */
/*    _gx_display_driver_32bpp_horizontal_line_draw                       */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_horizontal_line_alpha_draw(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos, INT width, GX_COLOR color, GX_UBYTE alpha)
{
INT    xval;
INT    yval;
INT    yend;
void (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;

    if (blend_func == GX_NULL)
    {
        return;
    }
    yend = ypos + width;

    /* draw 1-pixel hi lines to fill width */
    for (yval = ypos; yval < yend; yval++)
    {
        /* draw one line, left to right */
        for (xval = xstart; xval <= xend; xval++)
        {
            blend_func(context, xval, yval, color, alpha);
        }
    }
}

#endif /* GX_BRUSH_ALPHA_SUPPORT */

