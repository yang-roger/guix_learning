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
/*    _gx_display_driver_4bpp_glyph_4bit_draw                             */
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
void _gx_display_driver_4bpp_glyph_4bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area, GX_POINT *map_offset, const GX_GLYPH *glyph)
{
UINT                row;
UINT                height;
GX_VALUE            index;
GX_UBYTE           *glyph_getrow;
GX_UBYTE           *glyphdata_get;
GX_UBYTE            getmask;
GX_UBYTE            text_color;
GX_UBYTE            alpha;
GX_UBYTE           *put;
GX_UBYTE            putmask;
GX_UBYTE           *draw_start;
INT                 putstride;
INT                 getstride;

    putstride = (INT)(context->pitch + 1) >> 1;
    draw_start = (GX_UBYTE *)context->memory;
    draw_start += putstride * draw_area->top;
    draw_start += draw_area->left >> 1;

    text_color =  (GX_UBYTE)(context->brush.line_color + 15);

    /* Find the width of the glyph */
    getstride = glyph->width;
    /* Make it byte-aligned. */
    getstride = (getstride + 1) >> 1;

    glyph_getrow = (GX_UBYTE *)glyph->map;

    if (map_offset->y)
    {
        glyph_getrow = glyph_getrow + ((INT)getstride * map_offset->y);
    }

    glyph_getrow += (map_offset->x >> 1);

    height = (UINT)(draw_area->bottom - draw_area->top + 1);

    for (row = 0; row < height; row++)
    {
        glyphdata_get = glyph_getrow;
        if (map_offset->x & 1)
        {
            getmask = 0x0f;
        }
        else
        {
            getmask = 0xf0;
        }

        put = draw_start;
        if (draw_area->left & 1)
        {
            putmask = 0x0f;
        }
        else
        {
            putmask = 0xf0;
        }

        for (index = draw_area->left; index <= draw_area->right; index++)
        {
            alpha = *glyphdata_get & getmask;

            if (getmask == 0xf0)
            {
                alpha >>= 4;
                getmask >>= 4;
            }
            else
            {
                glyphdata_get++;
                getmask = 0xf0;
            }

            alpha = (GX_UBYTE)(text_color - alpha);
            alpha |= (GX_UBYTE)(alpha << 4);

            *put &= (GX_UBYTE)(~putmask);
            *put |= (alpha & putmask);

            putmask >>= 4;
            if (putmask == 0)
            {
                putmask = 0xf0;
                put++;
            }

        }

        glyph_getrow +=  getstride;
        draw_start += putstride;
    }
}

