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
/*    _gx_display_driver_1bpp_glyph_1bpp_draw                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This draws the specified text using the current context,            */
/*      clipped to one viewport                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Draw context                  */
/*    draw_area                             The rectangle where the glyph */
/*                                            is to be drawn.             */
/*    map_offset                            Offset into the glyph         */
/*    glyph                                 Pointer to glyph structure    */
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

/* Draw area defines where the glyph is drawn.  The draw area is not larger than the glyph.  If it is smaller than
   the glyph, the map_offset defines the how many rows on the top to skip, and how many col on the left to skip. */

void _gx_display_driver_1bpp_glyph_1bpp_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area, GX_POINT *map_offset, const GX_GLYPH *glyph)
{
INT             xval;
INT             yval;
INT             width;
GX_UBYTE       *putrow;
GX_UBYTE       *getrow;
GX_UBYTE       *put;
const GX_UBYTE *get;
GX_UBYTE        putmask;
GX_UBYTE        getmask;
INT             putstride;
INT             getstride;
GX_UBYTE        text_color;

    text_color = (GX_UBYTE)context->brush.line_color;

    putstride = (context->pitch + 7) >> 3;
    getstride = (glyph->width + 7) >> 3;

    putrow = (GX_UBYTE *)context->memory;
    putrow += (draw_area->top * putstride);
    putrow += (draw_area->left >> 3);

    getrow = (GX_UBYTE *)(glyph->map);
    getrow += (map_offset->y * getstride);
    getrow += (map_offset->x >> 3);

    width = draw_area->right - draw_area->left + 1;

    for (yval = draw_area->top; yval <= draw_area->bottom; yval++)
    {
        put = putrow;
        get = getrow;

        putmask = (GX_UBYTE)(0x80 >> (draw_area->left & 0x07));
        getmask = (GX_UBYTE)(0x80 >> (map_offset->x & 0x07));

        for (xval = 0; xval < width; xval++)
        {
            if (text_color == 0x01)
            {
                if ((*get) & getmask)
                {
                    *put |= putmask;
                }
            }
            else if (text_color == 0x00)
            {
                if ((*get) & getmask)
                {
                    *put = (GX_UBYTE)((*put) & (~putmask));
                }
            }

            getmask >>= 1;
            putmask >>= 1;

            if (!getmask)
            {
                getmask = 0x80;
                get++;
            }

            if (!putmask)
            {
                putmask = 0x80;
                put++;
            }
        }

        putrow += putstride;
        getrow += getstride;
    }
}

