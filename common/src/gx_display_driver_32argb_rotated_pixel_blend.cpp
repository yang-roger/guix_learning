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
/*    _gx_display_driver_32argb_rotated_pixel_blend                       */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Rotated Pixel blend function for 32argb color format.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    x                                     X coordinate                  */
/*    y                                     Y coordinate                  */
/*    color                                 Color of line to write        */
/*    alpha                                 blending value 0 to 255       */
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
void _gx_display_driver_32argb_rotated_pixel_blend(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha)
{
GX_UBYTE falpha;
ULONG    bcolor;
ULONG   *put;


    falpha = ALPHAVAL_32BPP(fcolor);
    falpha = (GX_UBYTE)((falpha * alpha) / 255);

    /* Is the pixel non-transparent?  */
    if (falpha > 0)
    {
        /* Calculate address of pixel.  */
        put = (ULONG *)context->memory;

        GX_SWAP_VALS(x, y);
        if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
        {
            y = context->canvas->x_resolution - y - 1;
        }
        else
        {
            x = context->canvas->y_resolution - x - 1;
        }

        put += context->pitch * y;
        put += x;

        /* Read background color.  */
        bcolor = *put;

        *put = gx_color_32argb_blend(fcolor, falpha, bcolor);
    }
}

