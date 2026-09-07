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
VOID _gx_dave2d_simulation_display_driver_compressed_glyph_4bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
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

        text_color = context->brush.line_color;

        /* pickup pointer to current dispaly driver */
        display = context->display;

        blend_func = display->driver_pixel_blend;

        glyph_data = (GX_UBYTE *)glyph->map;

        /* First, skip to the starting row.  */
        for (row = 0; row < map_offset->y; row++)
        {
            col = 0;

            while (col < glyph->width)
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

        row = draw_area->top;

        /* Now we are on the first visible row, copy pixels until we get
           to the enf of the last visible row.  */
        while (row < draw_area->bottom + 1)
        {
            start_x = draw_area->left - map_offset->x;
            col = start_x;

            while (col < start_x + glyph->width)
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
                            if ((col <= draw_area->right) &&
                                (col >= draw_area->left))
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
                            if ((col <= draw_area->right) &&
                                (col >= draw_area->left))
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
                            if ((col <= draw_area->right) &&
                                (col >= draw_area->left))
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
                            if ((col <= draw_area->right) &&
                                (col >= draw_area->left))
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
        _gx_dave2d_simulation_display_driver_glyph_4bit_draw(context, draw_area, map_offset, glyph);
    }
}

