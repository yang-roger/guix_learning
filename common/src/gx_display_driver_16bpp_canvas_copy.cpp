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

#include "gx_display_driver.h"

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
void _gx_display_driver_16bpp_canvas_copy(GX_CANVAS* canvas, GX_CANVAS* composite)
{
#ifdef GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
    if (canvas->status & GX_CANVAS_PARTIAL_FRAME_BUFFER)
    {
        return; // Not supported.
    }
#endif

    GX_RECTANGLE dirty;
    canvas->display_area_(&dirty);

    GX_RECTANGLE overlap;
    if (gx_rectangle_intersect_(dirty, composite->dirty_area, &overlap))
    {
        USHORT* read = (USHORT*)canvas->memory;
        read += (overlap.top - dirty.top) * canvas->x_resolution;
        read += overlap.left - dirty.left;

        USHORT* write = (USHORT*)composite->memory;
        write += overlap.top * composite->x_resolution;
        write += overlap.left;

        size_t width_in_bytes = overlap.width_() * sizeof(USHORT);

        for (INT row = overlap.top; row <= overlap.bottom; ++row)
        {
            memcpy(write, read, width_in_bytes); // Use case of memcpy is verified.

            write += composite->x_resolution;
            read += canvas->x_resolution;
        }
    }
}

