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
/*    _gx_display_driver_32bpp_canvas_copy                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 32bpp canvas copy function.                                 */
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
void _gx_display_driver_32bpp_canvas_copy(GX_CANVAS *canvas, GX_CANVAS *composite)
{
GX_RECTANGLE dirty;
GX_RECTANGLE overlap;
ULONG       *read;
ULONG       *write;
INT          width;
INT          row;

    canvas->display_area_(&dirty);

    if (_gx_utility_rectangle_overlap_detect(&dirty, &composite->dirty_area, &overlap))
    {
        width = overlap.right - overlap.left + 1;
        read = (ULONG *)canvas->memory;

        /* index into starting row */
        read += (overlap.top - dirty.top) * canvas->x_resolution;

        /* index into pixel */

        read += overlap.left - dirty.left;

        /* calculate the write pointer */
        write = (ULONG *)composite->memory;
        write += overlap.top * composite->x_resolution;
        write += overlap.left;

        for (row = overlap.top; row <= overlap.bottom; row++)
        {
            memcpy(write, read, (size_t)width * 4); /* Use case of memcpy is verified. */

            write += composite->x_resolution;
            read += canvas->x_resolution;
        }
    }
}

