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

#include "gx_context.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_glyph_4bit_draw                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This draws the specified text using the current context,            */
/*    clipped to one viewport                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Draw context                  */
/*    draw_position                         The X and Y coordinate where  */
/*                                            the glyph is drawn to       */
/*    string                                String to draw                */
/*    count                                 Count of string characters    */
/*    view                                  view to clip drawing within   */
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
/*    _gx_canvas_text_draw                                                */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_8bpp_glyph_4bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area, GX_POINT *map_offset, const GX_GLYPH *glyph)
{
GX_UBYTE *glyph_row;
GX_UBYTE *glyph_data;
UINT      row;
UINT      pixel_width = 0;
UINT      leading_pixel;
UINT      trailing_pixel;
GX_UBYTE  text_color;
UINT      y_height;
GX_UBYTE  alpha;
UINT      pitch;
UINT      index;
GX_UBYTE *put;
GX_UBYTE *draw_start;

    text_color =  (GX_UBYTE)(context->brush.line_color + 15);
    pixel_width = (UINT)(draw_area->right - draw_area->left + 1);

    /* Find the width of the glyph */
    pitch = glyph->width;
    /* Make it byte-aligned. */
    pitch = (pitch + 1) >> 1;

    glyph_row = (GX_UBYTE *)glyph->map;

    if (map_offset->y)
    {
        glyph_row = glyph_row + ((INT)pitch * map_offset->y);
    }

    glyph_row += (map_offset->x >> 1);

    y_height = (UINT)(draw_area->bottom - draw_area->top + 1);

    leading_pixel = (map_offset->x & 1);

    pixel_width -= leading_pixel;

    trailing_pixel = pixel_width & 1;

    pixel_width = pixel_width >> 1;


    draw_start = (GX_UBYTE *)context->memory;
    draw_start += context->pitch * draw_area->top;
    draw_start += draw_area->left;

    for (row = 0; row < y_height; row++)
    {
        glyph_data = glyph_row;

        put = draw_start;

        if (leading_pixel)
        {
            alpha = (*glyph_data) & 0x0f;

            *put = (GX_UBYTE)(text_color - alpha);
            put++;

            glyph_data++;
        }
        for (index = 0; index < pixel_width; index++)
        {
            alpha = (*glyph_data) & 0xf0;

            *put = (GX_UBYTE)(text_color - (alpha >> 4));
            put++;

            alpha = (*glyph_data) & 0x0f;
            *put = (GX_UBYTE)(text_color - alpha);
            put++;
            glyph_data++;
        }

        if (trailing_pixel)
        {
            alpha = (*glyph_data) & 0xf0;
            *put = (GX_UBYTE)(text_color - (alpha >> 4));
            put++;
        }

        glyph_row += pitch;
        draw_start += context->pitch;
    }
}

