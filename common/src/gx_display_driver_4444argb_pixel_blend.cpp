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
/*    _gx_display_driver_4444argb_pixel_blend                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Pixel blend function for 565rgb color format.                       */
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
void _gx_display_driver_4444argb_pixel_blend(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha)
{
GX_UBYTE falpha, fred, fgreen, fblue;
GX_UBYTE balpha, bred, bgreen, bblue;
GX_UBYTE inv_alpha;
INT      combined_alpha;
USHORT   bcolor;
USHORT  *put;


    ALPHAVAL_4444ARGB_EXT(falpha, fcolor);

    combined_alpha = falpha * alpha;
    combined_alpha /= 255;

    /* Is the pixel non-transparent? */
    if (combined_alpha > 0)
    {
        /* calculate address of pixel */
        put = (USHORT *)context->memory;
        put += context->pitch * y;
        put += x;

        /* No need to blend if alpha value is 255. */
        if (combined_alpha == 255)
        {
            *put = (USHORT)fcolor;
            return;
        }

        /* split foreground into red, green, and blue components */
        REDVAL_4444ARGB_EXT(fred, fcolor);
        GREENVAL_4444ARGB_EXT(fgreen, fcolor);
        BLUEVAL_4444ARGB_EXT(fblue, fcolor);

        /* read background color */
        bcolor = *put;

        /* split background color into red, green, and blue components */
        ALPHAVAL_4444ARGB_EXT(balpha, bcolor);
        REDVAL_4444ARGB_EXT(bred, bcolor);
        GREENVAL_4444ARGB_EXT(bgreen, bcolor);
        BLUEVAL_4444ARGB_EXT(bblue, bcolor);

        /* background alpha is inverse of foreground alpha */
        inv_alpha = (GX_UBYTE)(256 - combined_alpha);

        /* blend foreground and background, each color channel */
        falpha = (GX_UBYTE)(((balpha * inv_alpha) + (falpha * combined_alpha)) >> 12);
        fred = (GX_UBYTE)(((bred * inv_alpha) + (fred * combined_alpha)) >> 12);
        fgreen = (GX_UBYTE)(((bgreen * inv_alpha) + (fgreen * combined_alpha)) >> 12);
        fblue = (GX_UBYTE)(((bblue * inv_alpha) + (fblue * combined_alpha)) >> 12);

        /* re-assemble into 16-bit color and write it out */
        *put = (USHORT)ASSEMBLECOLOR_4444ARGB(falpha, fred, fgreen, fblue);
    }
}

