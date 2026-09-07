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
/*    _gx_display_driver_565rgb_rotated_pixel_blend                       */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Rotated pixel blend function for 565rgb color format.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    x                                     X coordinate                  */
/*    y                                     Y coordinate                  */
/*    color                                 Color of line to write        */
/*    alpha                                 Alpha value                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    REDVAL                                Extract the red component     */
/*    GREENVAL                              Extract the green component   */
/*    BLUEVAL                               Extract the blue component    */
/*    ASSEMBLECOLOR                         Assemble color components     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_565rgb_rotated_pixel_blend(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha)
{
USHORT   bcolor;
USHORT  *put;


    /* Is the pixel non-transparent? */
    if (alpha > 0)
    {
        put = (USHORT *)context->memory;

        /* calculate address of pixel */
        if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
        {
            put += context->pitch * (context->canvas->x_resolution - x - 1);
            put += y;
        }
        else
        {
            put += context->pitch * x;
            put += (context->canvas->y_resolution - y - 1);
        }

        /* read background color */
        bcolor = *put;

        *put = gx_color_565rgb_blend((USHORT)fcolor, alpha, bcolor);
    }
}

