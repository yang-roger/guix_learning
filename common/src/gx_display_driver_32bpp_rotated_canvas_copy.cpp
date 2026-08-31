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
/*    _gx_display_driver_32bpp_rotated_canvas_copy                        */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic rotated 32bpp canvas copy function.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*   canvas                                 The canvas to copy from       */
/*   composite                              The canvas to copy to         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_shift           Move the rectangle            */
/*    _gx_utility_rectangle_overlap_detect  Detect two rectangles being   */
/*                                            overlap to each other       */
/*    memcpy                                Move canvas data              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_32bpp_rotated_canvas_copy(GX_CANVAS *canvas, GX_CANVAS *composite)
{
GX_RECTANGLE dirty;
GX_RECTANGLE overlap;
ULONG       *read;
ULONG       *write;
INT          width;
INT          row;

    dirty.left = dirty.top = 0;
    dirty.right = (GX_VALUE)(canvas->x_resolution - 1);
    dirty.bottom = (GX_VALUE)(canvas->y_resolution - 1);

    _gx_utility_rectangle_shift(&dirty, canvas->display_offset_x, canvas->display_offset_y);

    if (_gx_utility_rectangle_overlap_detect(&dirty, &composite->dirty_area, &overlap))
    {
        width = overlap.bottom - overlap.top + 1;
        read = (ULONG *)canvas->memory;
        write = (ULONG *)composite->memory;

        if (canvas->display->rotation_angle == GX_SCREEN_ROTATION_CW)
        {
            /* Index into starting row.  */
            read += (dirty.right - overlap.right) * canvas->y_resolution;

            /* Index into pixel.  */
            read += overlap.top - dirty.top;

            /* Calculate the write pointer.  */
            write += (composite->x_resolution - overlap.right - 1) * composite->y_resolution;
            write += overlap.top;
        }
        else
        {
            /* Index into starting row.  */
            read += (overlap.left - dirty.left) * canvas->y_resolution;

            /* Index into pixel.  */
            read += dirty.bottom - overlap.bottom;

            /* Calculate the write pointer.  */
            write += overlap.left * composite->y_resolution;
            write += (composite->y_resolution - overlap.bottom - 1);
        }

        for (row = overlap.left; row <= overlap.right; row++)
        {
            memcpy(write, read, (size_t)width * 4); /* Use case of memcpy is verified. */

            write += composite->y_resolution;
            read += canvas->y_resolution;
        }
    }
}

