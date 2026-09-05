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
/*    _gx_display_driver_generic_glyph_8bit_draw                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified text using the current context,   */
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
/*    [gx_display_driver_pixel_blend]       Call display driver pixel     */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX internal code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_generic_glyph_8bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area, GX_POINT *map_offset, const GX_GLYPH *glyph)
{
GX_DISPLAY *display;
GX_UBYTE   *glyph_row;
GX_UBYTE   *glyph_data;
UINT        row;
UINT        col;
UINT        pixel_width;
GX_COLOR    text_color;
UINT        y_height;
GX_UBYTE    alpha1;
GX_UBYTE    brush_alpha = 0xff;

#if defined (GX_BRUSH_ALPHA_SUPPORT)
    brush_alpha = context->brush.alpha;
    if (brush_alpha == 0)
    {
        return;
    }
#endif

    display = context->display;

    if (display->driver_pixel_blend == GX_NULL)
    {
        return;
    }

    glyph_row = (GX_UBYTE *)glyph->map;

    if (map_offset->y)
    {
        glyph_row = glyph_row + (glyph->width * map_offset->y);
    }

    glyph_row += map_offset->x;

    text_color =  context->brush.line_color;
    pixel_width = (UINT)(draw_area->right - draw_area->left + 1);
    y_height = (UINT)(draw_area->bottom - draw_area->top + 1);

    if (brush_alpha == 0xff)
    {
        for (row = 0; row < y_height; row++)
        {
            glyph_data = glyph_row;

            for (col = 0; col < pixel_width; col++)
            {
                alpha1 = *glyph_data;

                if (alpha1 > 0)
                {
                    display->driver_pixel_blend(context,
                                                draw_area->left + (GX_VALUE)col,
                                                draw_area->top + (GX_VALUE)row,
                                                text_color, alpha1);
                }

                glyph_data++;
            }
            glyph_row += glyph->width;
        }
    }
#if defined (GX_BRUSH_ALPHA_SUPPORT)
    else
    {
        for (row = 0; row < y_height; row++)
        {
            glyph_data = glyph_row;

            for (col = 0; col < pixel_width; col++)
            {
                alpha1 = *glyph_data;
                alpha1 = (GX_UBYTE)(alpha1 * brush_alpha / 255);

                if (alpha1 > 0)
                {
                    display->driver_pixel_blend(context,
                                                draw_area->left + (GX_VALUE)col,
                                                draw_area->top + (GX_VALUE)row,
                                                text_color, alpha1);
                }

                glyph_data++;
            }
            glyph_row += glyph->width;
        }
    }
#endif
}

