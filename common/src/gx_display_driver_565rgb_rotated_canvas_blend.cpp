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
/*    _gx_display_driver_565rgb_rotated_canvas_blend                      */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Rotated canvas blend function for 565rgb color format.              */
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
void _gx_display_driver_565rgb_rotated_canvas_blend(GX_CANVAS *canvas, GX_CANVAS *composite)
{
GX_RECTANGLE dirty;
GX_RECTANGLE overlap;
USHORT      *read;
USHORT      *read_start;
USHORT      *write;
USHORT      *write_start;
USHORT       fcolor;
USHORT       bcolor;
GX_UBYTE     alpha, balpha;

INT          row;
INT          col;

    canvas->display_area_(&dirty);

    if (gx_rectangle_intersect_(dirty, composite->dirty_area, &overlap))
    {
        alpha = canvas->alpha;
        balpha = (GX_UBYTE)(256 - alpha);

        read_start = (USHORT *)canvas->memory;
        write_start = (USHORT *)composite->memory;

        if (canvas->display->rotation_angle == GX_SCREEN_ROTATION_CW)
        {
            /* index into starting row */
            read_start += (dirty.right - overlap.right) * canvas->y_resolution;

            /* index into pixel */
            read_start += overlap.top - dirty.top;

            /* calculate the write pointer */
            write_start += (composite->x_resolution - overlap.right - 1) * composite->y_resolution;
            write_start += overlap.top;
        }
        else
        {
            /* index into starting row */
            read_start += (overlap.left - dirty.left) * canvas->y_resolution;

            /* index into pixel */
            read_start += dirty.bottom - overlap.bottom;

            /* calculate the write pointer */
            write_start += overlap.left * composite->y_resolution;
            write_start += (composite->y_resolution - overlap.bottom - 1);
        }

        for (row = overlap.left; row <= overlap.right; row++)
        {
            read = read_start;
            write = write_start;

            for (col = overlap.top; col <= overlap.bottom; col++)
            {
                /* read the foreground color */
                fcolor = *read++;

                /* read background color */
                bcolor = *write;

                *write++ = gx_color_565rgb_blend(fcolor, alpha, bcolor, balpha);
            }
            write_start += composite->y_resolution;
            read_start += canvas->y_resolution;
        }
    }
}

