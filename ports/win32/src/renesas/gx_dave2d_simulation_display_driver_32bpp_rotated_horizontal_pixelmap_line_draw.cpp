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
/**   Synergy Simulation Display Management (Display)                     */
/**                                                                       */
/**************************************************************************/




/* Include necessary system files.  */

#include "gx_dave2d_simulation_display_driver.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_dave2d_simulation_display_driver_32bpp_rotated_horizontal_      */
/*                                                     pixelmap_line_draw */
/*                                                                        */
/*                                                                        */
/*                                                           6.4.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    32bpp screen driver horizontal pixelmap line drawing function that  */
/*    handles compressed or uncompress, with or without alpha channel.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xstart                                x-coord of line left          */
/*    xend                                  x-coord of line right         */
/*    y                                     y-coord of line top           */
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*     _gx_dave2d_simulation_display_driver_32bpp_horizontal_pixelmap_    */
/*                                                              line_draw */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
VOID _gx_dave2d_simulation_display_driver_32bpp_rotated_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT *context,
                                                                                       INT ystart, INT yend, INT x, GX_FILL_PIXELMAP_INFO *info)
{
INT          xstart;
INT          xend;
INT          y;
GX_PIXELMAP *old_map = info->pixelmap;
GX_PIXELMAP  map = *old_map;

    info->pixelmap = &map;
    GX_SWAP_VALS(map.height, map.width);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        xstart = ystart;
        xend = yend;
        y = context->canvas->x_resolution - x - 1;
    }
    else
    {
        xstart = context->canvas->y_resolution - yend - 1;
        xend = context->canvas->y_resolution - ystart - 1;
        y = x;
        info->x_offset = (yend - ystart + 1 + info->x_offset) % old_map->height;

        if (info->x_offset)
        {
            info->x_offset = old_map->height - info->x_offset;
        }
    }

    _gx_dave2d_simulation_display_driver_32bpp_horizontal_pixelmap_line_draw(context, xstart, xend, y, info);

    info->pixelmap = old_map;
}

