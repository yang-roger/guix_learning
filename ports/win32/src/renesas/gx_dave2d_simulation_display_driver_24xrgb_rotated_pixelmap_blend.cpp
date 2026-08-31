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
/*    _gx_dave2d_simulation_display_driver_24xrgb_pixelmap_blend          */
/*                                                                        */
/*                                                           6.4.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of uncompressed      */
/*    pixlemap file without alpha channel.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*     _gx_display_driver_24xrgb_pixel_blend                              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
VOID _gx_dave2d_simulation_display_driver_24xrgb_rotated_pixelmap_blend(GX_DRAW_CONTEXT *context,
                                                                         INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
GX_RECTANGLE *clip = context->clip;
GX_RECTANGLE  rotated_clip;
GX_PIXELMAP   temp_map = *pixelmap;

    GX_SWAP_VALS(xpos, ypos);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
        ypos = (context->canvas->x_resolution - ypos - pixelmap->width);
    }
    else
    {
        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
        xpos = (context->canvas->y_resolution - xpos - pixelmap->height);
    }

    context->clip = &rotated_clip;
    GX_SWAP_VALS(temp_map.height, temp_map.width);
    _gx_dave2d_simulation_display_driver_24xrgb_pixelmap_blend(context, xpos, ypos, &temp_map, alpha);
    context->clip = clip;
}

