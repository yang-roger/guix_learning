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
/*    _gx_display_driver_4bpp_glyph_1bit_draw                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws monochrome font to the 4bpp canvas, clipped     */
/*    to one viweport.                                                    */
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

#define WRITE_PIXEL if (alpha & mask)                      \
    {                                                      \
        *put = *put & (GX_UBYTE)(~putmask);                \
        *put = *put | (GX_UBYTE)(text_color & putmask);    \
    }                                                      \
    putmask = putmask >> 4;                                \
    if (putmask == 0)                                      \
    {                                                      \
        putmask = 0xf0;                                    \
        put++;                                             \
    }


void _gx_display_driver_4bpp_glyph_1bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area, GX_POINT *map_offset, const GX_GLYPH *glyph)
{
GX_UBYTE *glyph_row;
GX_UBYTE *glyph_data;
UINT      row;
UINT      pixel_per_row;
UINT      pixel_in_first_byte;
UINT      pixel_in_last_byte = 0;
GX_UBYTE  text_color;
UINT      y_height;
GX_UBYTE  alpha;
UINT      glyph_width;
GX_UBYTE *put;
UINT      num_bytes;
UINT      num_bits;
GX_UBYTE *line_start;
GX_UBYTE  mask, init_mask;
UINT      i;
GX_UBYTE  putmask;
INT       putstride;

    text_color =  (GX_UBYTE)context->brush.line_color;
    text_color |= (GX_UBYTE)(text_color << 4);
    pixel_per_row = (UINT)draw_area->right - (UINT)draw_area->left + (UINT)1;
    putstride = (context->pitch + 1) >> 1;
    /* pickup pointer to current dispaly driver */
    /*display = context->display;*/

    /* Find the width of the glyph, in terms of bytes */
    glyph_width = glyph->width;
    /* Make it byte-aligned. */
    glyph_width = (glyph_width + 7) >> 3;

    /* Compute the number of useful bytes from the glyph this routine is going to use.
       Because of map_offset, the first byte may contain pixel bits we don't need to draw;
       And the width of the draw_area may produce part of the last byte in the row to be ignored. */
    num_bytes = ((UINT)map_offset->x + pixel_per_row + 7) >> 3;
    /* Take into account if map_offset specifies the number of bytes to ignore from the beginning of the row. */
    num_bytes -= (UINT)(map_offset->x) >> 3;

    /* Compute the number of pixels to draw from the first byte of the glyph data. */
    pixel_in_first_byte = (UINT)(8 - ((map_offset->x) & 0x7));
    init_mask = (GX_UBYTE)(1 << (pixel_in_first_byte - 1));
    /* Compute the number of pixels to draw from the last byte, if there are more than one byte in a row. */
    if (num_bytes != 1)
    {
        pixel_in_last_byte = (map_offset->x + (INT)pixel_per_row) & 0x7;
        if (pixel_in_last_byte == 0)
        {
            pixel_in_last_byte = 8;
        }
    }
    else
    {
        if ((map_offset->x + (INT)pixel_per_row) < 8)
        {
            pixel_in_first_byte = pixel_per_row;
        }
        else
        {
            pixel_in_last_byte = 0;
        }
    }


    glyph_row = (GX_UBYTE *)glyph->map;

    if (map_offset->y)
    {
        glyph_row = glyph_row + ((INT)glyph_width * map_offset->y);
    }

    glyph_row += (map_offset->x >> 3);

    y_height = (UINT)(draw_area->bottom - draw_area->top + 1);

    line_start = (GX_UBYTE *)context->memory;
    line_start += putstride * (draw_area->top);
    line_start += draw_area->left >> 1;


    for (row = 0; row < y_height; row++)
    {
        if (draw_area->left & 0x01)
        {
            putmask = 0x0f;
        }
        else
        {
            putmask = 0xf0;
        }
        glyph_data = glyph_row;
        alpha = *(glyph_data);
        mask = init_mask;
        num_bits = pixel_in_first_byte;
        put = line_start;
        for (i = 0; i < num_bytes; i++)
        {
            if ((i == (num_bytes - 1)) && (num_bytes > 1))
            {
                num_bits = pixel_in_last_byte;
            }
            switch (num_bits)
            {
            case 8:
                WRITE_PIXEL;
                mask >>= 1;
                /* fallthrough */
            case 7:
                WRITE_PIXEL;
                mask >>= 1;
                /* fallthrough */
            case 6:
                WRITE_PIXEL;
                mask >>= 1;
                /* fallthrough */
            case 5:
                WRITE_PIXEL;
                mask >>= 1;
                /* fallthrough */
            case 4:
                WRITE_PIXEL;
                mask >>= 1;
                /* fallthrough */
            case 3:
                WRITE_PIXEL;
                mask >>= 1;
                /* fallthrough */
            case 2:
                WRITE_PIXEL;
                mask >>= 1;
                /* fallthrough */
            default:
                WRITE_PIXEL;
            }
            glyph_data++;
            alpha = *(glyph_data);
            num_bits = 8;
            mask = 0x80;
        }

        glyph_row +=  glyph_width;
        line_start += putstride;
    }

    return;
}

