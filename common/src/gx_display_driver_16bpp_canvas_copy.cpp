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
/*    _gx_display_driver_16bpp_canvas_copy                                */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 16bpp canvas copy function.                                 */
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
void _gx_display_driver_16bpp_canvas_copy(GX_CANVAS *canvas, GX_CANVAS *composite)
{
GX_RECTANGLE dirty;
GX_RECTANGLE overlap;
USHORT      *read;
USHORT      *write;
INT          width;
INT          row;

#ifdef GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
    if (canvas->status & GX_CANVAS_PARTIAL_FRAME_BUFFER)
    {
        /* Not supported. */
        return;
    }
#endif

    dirty.left = dirty.top = 0;
    dirty.right = (GX_VALUE)(canvas->x_resolution - (GX_VALUE)1);
    dirty.bottom = (GX_VALUE)(canvas->y_resolution - (GX_VALUE)1);

    _gx_utility_rectangle_shift(&dirty, canvas->display_offset_x, canvas->display_offset_y);

    if (_gx_utility_rectangle_overlap_detect(&dirty, &composite->dirty_area, &overlap))
    {
        width = overlap.right - overlap.left + 1;
        read = (USHORT *)canvas->memory;

        /* index into starting row */
        read += (overlap.top - dirty.top) * canvas->x_resolution;

        /* index into pixel */

        read += overlap.left - dirty.left;

        /* calculate the write pointer */
        write = (USHORT *)composite->memory;
        write += overlap.top * composite->x_resolution;
        write += overlap.left;

        for (row = overlap.top; row <= overlap.bottom; row++)
        {
            memcpy(write, read, (size_t)(width * 2)); /* Use case of memcpy is verified. */

            write += composite->x_resolution;
            read += canvas->x_resolution;
        }
    }
}

