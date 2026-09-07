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


#define BLEND_PIXEL if (alpha & mask)                             \
    {                                                             \
        blend_func(context, xval, yval, text_color, brush_alpha); \
    }                                                             \
    xval++;                                                       \
    mask = (unsigned char)(mask << 1);

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_dave2d_simulation_display_driver_32bpp_rotated_glyph_1bit_draw  */
/*                                                                        */
/*                                                                        */
/*                                                           6.4.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This functions draw monochrome font on 32bpp rotated canvas,        */
/*    clipped to one viewport.                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Draw context                  */
/*    draw_area                             The region bound by the       */
/*                                            rectangle where the glyph   */
/*                                            is drawn                    */
/*    map_offset                            X,Y offset into the glyph map */
/*    glyph                                 Pointer to the glyph          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX internal code                                                  */
/*                                                                        */
/**************************************************************************/
VOID _gx_dave2d_simulation_display_driver_32bpp_rotated_glyph_1bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
                                                                         GX_POINT *map_offset, GX_CONST GX_GLYPH *glyph)
{
GX_COMPRESSED_GLYPH rotated_glyph;
GX_RECTANGLE        rotated_draw_area;
GX_POINT            rotated_map_offset;

    rotated_glyph = *((GX_COMPRESSED_GLYPH *)glyph);

    GX_SWAP_VALS(rotated_glyph.height, rotated_glyph.width);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        rotated_draw_area.left = draw_area->top;
        rotated_draw_area.right = draw_area->bottom;
        rotated_draw_area.top = (GX_VALUE)(context->canvas->x_resolution - draw_area->right - 1);
        rotated_draw_area.bottom = (GX_VALUE)(context->canvas->x_resolution - draw_area->left - 1);

        rotated_map_offset.x = map_offset->y;
        rotated_map_offset.y = (GX_VALUE)(glyph->width - map_offset->x -
                                                   (GX_VALUE)(draw_area->right - draw_area->left + 1));
    }
    else
    {
        rotated_draw_area.left = (GX_VALUE)(context->canvas->y_resolution - draw_area->bottom - 1);
        rotated_draw_area.right = (GX_VALUE)(context->canvas->y_resolution - draw_area->top - 1);
        rotated_draw_area.top = draw_area->left;
        rotated_draw_area.bottom = draw_area->right;

        rotated_map_offset.x = (GX_VALUE)(glyph->height - map_offset->y -
                                                   (GX_VALUE)(draw_area->bottom - draw_area->top + 1));
        rotated_map_offset.y = map_offset->x;
    }

    _gx_dave2d_simulation_display_driver_32bpp_glyph_1bit_draw(context, &rotated_draw_area, &rotated_map_offset, (GX_GLYPH *)&rotated_glyph);
}

