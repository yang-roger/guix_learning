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
#include "gx_canvas.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_rotated_glyph_3bit_draw                     */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This draws the specified 3bit glyph using the current context,      */
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
void _gx_display_driver_8bpp_rotated_glyph_3bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area, GX_POINT *map_offset, const GX_GLYPH *glyph)
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
GX_VALUE  rotated_map_offset_x;
GX_VALUE  rotated_map_offset_y;
GX_VALUE  rotated_left;
GX_VALUE  rotated_top;

    text_color = (GX_UBYTE)(context->brush.line_color + 7);
    pixel_width = (UINT)(draw_area->bottom - draw_area->top + 1);

    y_height = (UINT)(draw_area->right - draw_area->left + 1);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        rotated_left = draw_area->top;
        rotated_top = (GX_VALUE)(context->canvas->x_resolution - draw_area->right - 1);

        rotated_map_offset_x = map_offset->y;
        rotated_map_offset_y = (GX_VALUE)(glyph->width - map_offset->x - (GX_VALUE)y_height);
    }
    else
    {
        rotated_left = (GX_VALUE)(context->canvas->display_offset_y - draw_area->bottom - 1);
        rotated_top = draw_area->left;

        rotated_map_offset_x = (GX_VALUE)(glyph->height - map_offset->y - (GX_VALUE)pixel_width);
        rotated_map_offset_y = map_offset->x;
    }

    leading_pixel = (rotated_map_offset_x & 1);

    pixel_width -= leading_pixel;

    trailing_pixel = pixel_width & 1;

    pixel_width = pixel_width >> 1;

    /* Find the width of the glyph.  */
    pitch = glyph->height;

    /* Make it byte-aligned.  */
    pitch = (pitch + 1) >> 1;

    glyph_row = (GX_UBYTE *)glyph->map;

    if (rotated_map_offset_y)
    {
        glyph_row = glyph_row + ((INT)pitch * rotated_map_offset_y);
    }

    glyph_row += (rotated_map_offset_x >> 1);

    draw_start = (GX_UBYTE *)context->memory;
    draw_start += context->pitch * rotated_top;
    draw_start += rotated_left;

    for (row = 0; row < y_height; row++)
    {
        glyph_data = glyph_row;

        put = draw_start;

        if (leading_pixel)
        {
            alpha = (*glyph_data) & 0x0f;

            /* use 3bit color gradient instead of 4 bit */
            alpha >>= 1;

            *put = (GX_UBYTE)(text_color - alpha);
            put++;

            glyph_data++;
        }
        for (index = 0; index < pixel_width; index++)
        {
            alpha = (*glyph_data) & 0xf0;

            /* use 3bit color gradient instead of 4 bit */
            alpha >>= 1;

            *put = (GX_UBYTE)(text_color - (alpha >> 4));
            put++;

            alpha = (*glyph_data) & 0x0f;

            /* use 3bit color gradient instead of 4 bit */
            alpha >>= 1;

            *put = (GX_UBYTE)(text_color - alpha);
            put++;
            glyph_data++;
        }

        if (trailing_pixel)
        {
            alpha = (*glyph_data) & 0xf0;

            /* use 3bit color gradient instead of 4 bit */
            alpha >>= 1;

            *put = (GX_UBYTE)(text_color - (alpha >> 4));
            put++;
        }

        glyph_row += pitch;
        draw_start += context->pitch;
    }
}

