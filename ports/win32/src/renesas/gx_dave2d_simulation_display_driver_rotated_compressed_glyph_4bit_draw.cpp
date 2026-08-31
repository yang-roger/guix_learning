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

#define Extend_alpha_4bit(alpha) alpha = (GX_UBYTE)(alpha | (alpha >> 4));

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_dave2d_simulation_display_driver_glyph_4bit_compress_draw       */
/*                                                                        */
/*                                                           6.4.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified text using the current context,   */
/*    clipped to one viewport                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Draw context                  */
/*    draw_area                             The rectangle where the glyph */
/*                                            is drawn to                 */
/*    map_offset                            Offset from the glyph map     */
/*    glyph                                 The glyph structure           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_pixel_blend]       Call display driver pixel     */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX internal code                                                  */
/*                                                                        */
/**************************************************************************/
VOID _gx_dave2d_simulation_display_driver_rotated_compressed_glyph_4bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
                                                                              GX_POINT *map_offset, GX_CONST GX_GLYPH *glyph)
{
GX_DISPLAY          *display;
GX_UBYTE            *glyph_data;
INT                  row;
INT                  col;
INT                  start_x;
GX_COLOR             text_color;
GX_UBYTE             alpha1;
GX_UBYTE             alpha2;
GX_UBYTE             count;
GX_COMPRESSED_GLYPH *compressed_glyph;
VOID                 (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);
GX_RECTANGLE         rotated_draw_area;
GX_POINT             rotated_map_offset;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
GX_UBYTE brush_alpha = context->brush.alpha;
    if (brush_alpha == 0)
    {
        return;
    }
#endif
    compressed_glyph = (GX_COMPRESSED_GLYPH *)glyph;

    if (compressed_glyph->map_size & 0x8000)
    {
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

        text_color = context->brush.line_color;

        /* pickup pointer to current dispaly driver */
        display = context->display;

        GX_SET_BLEND_FUNCTION(blend_func, display->color_format)

        glyph_data = (GX_UBYTE *)glyph->map;

        /* First, skip to the starting row.  */
        for (row = 0; row < rotated_map_offset.y; row++)
        {
            col = 0;

            while (col < glyph->height)
            {
                count = *glyph_data++;
                if (count & 0x80)
                {
                    count = (GX_UBYTE)((count & 0x7f) + 1);
                    glyph_data++;
                }
                else
                {
                    count++;
                    glyph_data += count;
                }
                col += count * 2;
            }
        }

        row = rotated_draw_area.top;

        /* Now we are on the first visible row, copy pixels until we get
           to the enf of the last visible row.  */
        while (row <= rotated_draw_area.bottom)
        {
            start_x = rotated_draw_area.left - rotated_map_offset.x;
            col = start_x;

            while (col < start_x + glyph->height)
            {
                count = *glyph_data++;

                if (count & 0x80)
                {
                    /* Repeat.  */
                    count = (GX_UBYTE)((count & 0x7f) + 1);

                    alpha1 = (GX_UBYTE)((*glyph_data) << 4);
                    alpha2 = (*glyph_data) & 0xf0;

                    Extend_alpha_4bit(alpha1)
                    Extend_alpha_4bit(alpha2)
#if defined(GX_BRUSH_ALPHA_SUPPORT)
                    alpha1 = (GX_UBYTE)(alpha1 * brush_alpha / 255);
                    alpha2 = (GX_UBYTE)(alpha2 * brush_alpha / 255);
#endif

                    while (count--)
                    {
                        if (alpha1 > 0)
                        {
                            if ((col <= rotated_draw_area.right) &&
                                (col >= rotated_draw_area.left))
                            {
                                blend_func(context,
                                           col,
                                           row,
                                           text_color,
                                           alpha1);
                            }
                        }
                        col++;

                        if (alpha2 > 0)
                        {
                            if ((col <= rotated_draw_area.right) &&
                                (col >= rotated_draw_area.left))
                            {
                                blend_func(context,
                                           col,
                                           row,
                                           text_color,
                                           alpha2);
                            }
                        }
                        col++;
                    }
                    glyph_data++;
                }
                else
                {
                    count++;
                    while (count--)
                    {
                        alpha1 = (GX_UBYTE)((*glyph_data) << 4);
                        alpha2 = (*glyph_data) & 0xf0;

                        if (alpha1 > 0)
                        {
                            if ((col <= rotated_draw_area.right) &&
                                (col >= rotated_draw_area.left))
                            {
                                Extend_alpha_4bit(alpha1)
#if defined(GX_BRUSH_ALPHA_SUPPORT)
                                alpha1 = (GX_UBYTE)(alpha1 * brush_alpha / 255);
#endif
                                blend_func(context,
                                           col,
                                           row,
                                           text_color,
                                           alpha1);
                            }
                        }
                        col++;

                        if (alpha2 > 0)
                        {
                            if ((col <= rotated_draw_area.right) &&
                                (col >= rotated_draw_area.left))
                            {
                                Extend_alpha_4bit(alpha2)
#if defined(GX_BRUSH_ALPHA_SUPPORT)
                                alpha2 = (GX_UBYTE)(alpha2 * brush_alpha / 255);
#endif
                                blend_func(context,
                                           col,
                                           row,
                                           text_color,
                                           alpha2);
                            }
                        }
                        col++;
                        glyph_data++;
                    }
                }
            }
            row++;
        }
    }
    else
    {
        _gx_dave2d_simulation_display_driver_rotated_glyph_4bit_draw(context, draw_area, map_offset, glyph);
    }
}

