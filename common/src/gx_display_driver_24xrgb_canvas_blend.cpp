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
/*    _gx_display_driver_24xrgb_canvas_blend                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Canvas blend function for 24xrgb color format.                      */
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
void _gx_display_driver_24xrgb_canvas_blend(GX_CANVAS *canvas, GX_CANVAS *composite)
{
GX_RECTANGLE dirty;
GX_RECTANGLE overlap;
ULONG       *read;
ULONG       *read_start;
ULONG       *write;
ULONG       *write_start;
ULONG        fcolor;
GX_UBYTE     alpha, balpha;

ULONG        bcolor;
INT          row;
INT          col;

    canvas->display_area_(&dirty);

    if (gx_rectangle_intersect_(dirty, composite->dirty_area, &overlap))
    {
        alpha = canvas->alpha;
        balpha = (GX_UBYTE)(256 - alpha);

        read_start = (ULONG *)canvas->memory;

        /* index into starting row */
        read_start += (overlap.top - dirty.top) * canvas->x_resolution;

        /* index into pixel */

        read_start += overlap.left - dirty.left;

        /* calculate the write pointer */
        write_start = (ULONG *)composite->memory;
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

                /* read background color */
                bcolor = *write;

                *write++ = gx_color_24xrgb_blend(fcolor, alpha, bcolor, balpha);
            }
            write_start += composite->x_resolution;
            read_start += canvas->x_resolution;
        }
    }
}

