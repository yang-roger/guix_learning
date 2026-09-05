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

#include "gx_utility.h"
#include "gx_canvas.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_canvas_blend                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Canvas blend function for 4444argb color foramt.                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*   canvas                                 The canvas to blend to        */
/*   composite                              The canvas to blend from      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_shift           Adjust the rectangle          */
/*    _gx_utility_recttangle_overlap_detect Detect whether two areas      */
/*                                            overlap                     */
/*    REDVAL                                Extrace Red from canvas       */
/*    GREENVAL                              Extrace Green from canvas     */
/*    BLUEVAL                               Extrace Blue from canvas      */
/*    ASSEMBLECOLOR                         Compose the RGB color         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_4444argb_canvas_blend(GX_CANVAS *canvas, GX_CANVAS *composite)
{
GX_RECTANGLE dirty;
GX_RECTANGLE overlap;
USHORT      *read;
USHORT      *read_start;
USHORT      *write;
USHORT      *write_start;
GX_UBYTE     alpha;
USHORT       fcolor;
GX_UBYTE     falpha;
GX_UBYTE     fred;
GX_UBYTE     fgreen;
GX_UBYTE     fblue;
GX_UBYTE     balpha;
GX_UBYTE     bred;
GX_UBYTE     bgreen;
GX_UBYTE     bblue;
GX_UBYTE     inv_alpha;
GX_UBYTE     combined_alpha;
USHORT       bcolor;
INT          row;
INT          col;

    canvas->display_area_(&dirty);

    if (gx_rectangle_intersect_(dirty, composite->dirty_area, &overlap))
    {
        alpha = canvas->alpha;

        read_start = (USHORT *)canvas->memory;
        read_start += (overlap.top - dirty.top) * canvas->x_resolution;
        read_start += overlap.left - dirty.left;

        write_start = (USHORT *)composite->memory;
        write_start += overlap.top * composite->x_resolution;
        write_start += overlap.left;

        for (row = overlap.top; row <= overlap.bottom; row++)
        {
            read = read_start;
            write = write_start;

            for (col = overlap.left; col <= overlap.right; col++)
            {
                /* read the foreground color */
                fcolor = *read++;

                /* split foreground into red, green, and blue components */
                ALPHAVAL_4444ARGB_EXT(falpha, fcolor);
                fred = REDVAL_4444ARGB(fcolor);
                fgreen = GREENVAL_4444ARGB(fcolor);
                fblue = BLUEVAL_4444ARGB(fcolor);

                combined_alpha = (GX_UBYTE)((falpha * alpha) >> 8);

                /* read background color */
                bcolor = *write;

                /* split background color into alpha, red, green, and blue components */
                ALPHAVAL_4444ARGB_EXT(balpha, bcolor);
                bred = REDVAL_4444ARGB(bcolor);
                bgreen = GREENVAL_4444ARGB(bcolor);
                bblue = BLUEVAL_4444ARGB(bcolor);

                /* background alpha is inverse of foreground alpha */
                inv_alpha = (GX_UBYTE)(256 - combined_alpha);

                /* blend foreground and background, each color channel */
                falpha = (GX_UBYTE)(((balpha * inv_alpha) + (falpha * combined_alpha)) >> 8);
                fred = (GX_UBYTE)(((bred * inv_alpha) + (fred * combined_alpha)) >> 8);
                fgreen = (GX_UBYTE)(((bgreen * inv_alpha) + (fgreen * combined_alpha)) >> 8);
                fblue = (GX_UBYTE)(((bblue * inv_alpha) + (fblue * combined_alpha)) >> 8);

                /* re-assemble into 16-bit color and write it out */
                *write++ = ASSEMBLECOLOR_4444ARGB(falpha, fred, fgreen, fblue);
            }
            write_start += composite->x_resolution;
            read_start += canvas->x_resolution;
        }
    }
}

