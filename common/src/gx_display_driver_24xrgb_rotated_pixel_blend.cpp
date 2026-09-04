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
/*    _gx_display_driver_24xrgb_rotated_pixel_blend                       */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Rotated pixel blend function for 24xrgb color format.               */
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
/*    REDVAL_24BPP                          Extract the red component     */
/*    GREENVAL_24BPP                        Extract the green component   */
/*    BLUEVAL_24BPP                         Extract the blue component    */
/*    ASSEMBLECOLOR_24BPP                   Assemble color components     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_24xrgb_rotated_pixel_blend(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha)
{
ULONG    bcolor;
ULONG   *put;


    /* Is the pixel non-transparent?  */
    if (alpha > 0)
    {
        /* Calculate address of pixel.  */
        put = (ULONG *)context->memory;

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

        /* Read background color.  */
        bcolor = *put;

        *put = gx_color_24xrgb_blend(fcolor, alpha, bcolor);
    }
}

