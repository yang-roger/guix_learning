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

#define DRAW_PIXEL if (alpha & mask) \
    {                                \
        *put = text_color;           \
    }                                \
    put++;                           \
    mask = (unsigned char)(mask << 1);

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_dave2d_simulation_display_driver_16bpp_compressed_glyph_1bit_   */
/*    draw                                                                */
/*                                                                        */
/*                                                           6.4.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws compressed monochrome font on 16bpp canvas,     */
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
VOID _gx_dave2d_simulation_display_driver_16bpp_compressed_glyph_1bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
                                                                            GX_POINT *map_offset, GX_CONST GX_GLYPH *glyph)
{
GX_UBYTE            *glyph_data;
INT                  row;
INT                  col;
INT                  start_x;
USHORT               text_color;
GX_UBYTE             alpha_value;
GX_UBYTE             alpha;
GX_UBYTE             mask;
GX_UBYTE             count;
GX_UBYTE             index;
GX_COMPRESSED_GLYPH *compressed_glyph;

GX_UBYTE             brush_alpha;
VOID                 (*blend_func)(GX_DRAW_CONTEXT *, INT, INT, GX_COLOR, GX_UBYTE);

    brush_alpha = context->brush.alpha;
    if (brush_alpha == 0)
    {
        return;
    }

    GX_SET_BLEND_FUNCTION(blend_func, context->display->color_format)

    if (blend_func == GX_NULL)
    {
        return;
    }


    compressed_glyph = (GX_COMPRESSED_GLYPH *)glyph;

    if (compressed_glyph->map_size & 0x8000)
    {

        text_color = (USHORT)context->brush.line_color;

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
                col += count * 8;
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
                    count = (GX_UBYTE)((count & 0x7f) + 1);
                    alpha_value = *glyph_data++;
                    while (count--)
                    {
                        mask = 0x01;
                        if (alpha_value != 0)
                        {
                            for (index = 0; index < 8; index++)
                            {
                                alpha = alpha_value & mask;
                                mask = (GX_UBYTE)(mask << 1);
                                if (alpha != 0)
                                {
                                    if ((col <= draw_area->right) &&
                                        (col >= draw_area->left))
                                    {
                                        blend_func(context, col, row, text_color, brush_alpha);
                                    }
                                }
                                col++;
                            }
                        }
                        else
                        {
                            col += 8;
                        }
                    }
                }
                else
                {
                    count++;
                    while (count--)
                    {
                        alpha_value = *glyph_data++;
                        mask = 0x01;
                        if (alpha_value != 0)
                        {
                            for (index = 0; index < 8; index++)
                            {
                                alpha = alpha_value & mask;
                                mask = (GX_UBYTE)(mask << 1);
                                if (alpha != 0)
                                {
                                    if ((col <= draw_area->right) &&
                                        (col >= draw_area->left))
                                    {
                                        blend_func(context, col, row, text_color, brush_alpha);
                                    }
                                }
                                col++;
                            }
                        }
                        else
                        {
                            col += 8;
                        }
                    }
                }
            }
            row++;
        }
    }
    else
    {
        _gx_dave2d_simulation_display_driver_16bpp_glyph_1bit_draw(context, draw_area, map_offset, glyph);
    }
}

