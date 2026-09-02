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


#define REDVAL(_c)   (GX_UBYTE)(((_c) >> 10) & 0x1f)
#define GREENVAL(_c) (GX_UBYTE)(((_c) >> 5) & 0x1f)
#define BLUEVAL(_c)  (GX_UBYTE)(((_c)) & 0x1f)


/* Define macros for assembling a 15-bit r:g:b value from 3 components.  */

#define ASSEMBLECOLOR(_r, _g, _b) \
    ((((_r) & 0x1f) << 10) |      \
     (((_g) & 0x1f) << 5) |       \
     (((_b) & 0x1f)))


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_1555xrgb_canvas_blend                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Canvas blend function for 1555xrgb color foramt.                    */
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
/*    _gx_utility_rectangle_shift           Shift rectangle by specified  */
/*                                            value                       */
/*    _gx_utility_rectangle_overlap_detect  Detect overlaps of specified  */
/*                                            rectangles                  */
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
void _gx_display_driver_1555xrgb_canvas_blend(GX_CANVAS *canvas, GX_CANVAS *composite)
{
GX_RECTANGLE dirty;
GX_RECTANGLE overlap;
USHORT      *read;
USHORT      *read_start;
USHORT      *write;
USHORT      *write_start;
USHORT       fcolor;
GX_UBYTE     fred, fgreen, fblue;
GX_UBYTE     bred, bgreen, bblue;
GX_UBYTE     alpha, balpha;

USHORT       bcolor;
INT          row;
INT          col;

    canvas->display_area_(&dirty);

    if (gx_rectangle_intersect_(dirty, composite->dirty_area, &overlap))
    {
        alpha = canvas->alpha;
        balpha = (GX_UBYTE)(256 - alpha);

        read_start = (USHORT *)canvas->memory;

        /* index into starting row */
        read_start += (overlap.top - dirty.top) * canvas->x_resolution;

        /* index into pixel */

        read_start += overlap.left - dirty.left;

        /* calculate the write pointer */
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
                fred = REDVAL(fcolor);
                fgreen = GREENVAL(fcolor);
                fblue = BLUEVAL(fcolor);

                /* read background color */
                bcolor = *write;

                /* split background color into red, green, and blue components */
                bred = REDVAL(bcolor);
                bgreen = GREENVAL(bcolor);
                bblue = BLUEVAL(bcolor);

                /* blend foreground and background, each color channel */
                fred = (GX_UBYTE)(((bred * balpha) + (fred * alpha)) >> 8);
                fgreen = (GX_UBYTE)(((bgreen * balpha) + (fgreen * alpha)) >> 8);
                fblue = (GX_UBYTE)(((bblue * balpha) + (fblue * alpha)) >> 8);

                /* re-assemble into 16-bit color and write it out */
                *write++ = (USHORT)ASSEMBLECOLOR(fred, fgreen, fblue);
            }
            write_start += composite->x_resolution;
            read_start += canvas->x_resolution;
        }
    }
}

