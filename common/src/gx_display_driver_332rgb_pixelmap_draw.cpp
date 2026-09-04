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
#include "gx_pixelmap.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_332rgb_pixelmap_raw_write                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of uncompressed       */
/*    pixlemap file without alpha channel.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
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
/*    _gx_display_driver_332rgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_332rgb_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
                                                         INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT             xval;
INT             yval;
INT             width;
GX_UBYTE       *putrow;
GX_UBYTE       *getrow;
GX_UBYTE       *put;
const GX_UBYTE *get;

GX_RECTANGLE   *clip = context->clip;

    putrow = (GX_UBYTE *)context->memory;
    putrow += clip->top * context->pitch;
    putrow += clip->left;

    getrow = (GX_UBYTE *)(pixelmap->data);
    getrow += pixelmap->width * (clip->top - ypos);
    getrow += (clip->left - xpos);

    width = clip->right - clip->left + 1;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        put = putrow;
        get = getrow;

        for (xval = 0; xval < width; xval++)
        {
            *put++ = *get++;
        }
        putrow += context->pitch;
        getrow += pixelmap->width;
    }
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_332rgb_pixelmap_compressed_write                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixlemap file without alpha channel.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_332rgb_pixel_blend Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_332rgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_332rgb_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                                INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                yval;
INT                xval;
const GX_UBYTE    *get;
GX_UBYTE          *put;
GX_UBYTE          *putrow;
GX_UBYTE           count;
GX_UBYTE           pixel;
GX_UBYTE           brush_alpha;

GX_RECTANGLE   *clip = context->clip;

    get = (const GX_UBYTE *)pixelmap->data;
    brush_alpha = context->brush.alpha;
    /* compressed with no alpha is a one-byte count and one-byte pixel value */

    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1u);
                get++;      /* skip repeated pixel value */
            }
            else
            {
                count++;
                get += count;   /* skip raw pixel values */
            }
            xval += count;
        }
    }

    /* now we are on the first visible row, copy pixels until we get
       to the enf of the last visible row
     */
    putrow = (GX_UBYTE *)context->memory;
    putrow += yval * context->pitch;
    putrow += xpos;

    while (yval <= clip->bottom)
    {
        put = putrow;
        xval = xpos;

        while (xval < xpos + pixelmap->width)
        {
            count = *get++;

            if (count & 0x80)
            {
                /* repeated value */
                count = (GX_UBYTE)((count & 0x7f) + 1u);
                pixel = *get++;

                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            *put = pixel;
                        }
                        put++;
                        xval++;
                    }
                }
                else
                {
                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            _gx_display_driver_332rgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
                        }
                        xval++;
                    }
                }

            }
            else
            {
                /* string of non-repeated values */
                count++;
                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            *put = *get;
                        }
                        put++;
                        get++;
                        xval++;
                    }
                }
                else
                {
                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            _gx_display_driver_332rgb_pixel_blend(context, xval, yval, *get, brush_alpha);
                        }
                        get++;
                        xval++;
                    }
                }

            }
        }
        putrow += context->pitch;
        yval++;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_332rgb_pixelmap_alpha_write                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of uncompressed       */
/*    pixlemap file with alpha channel.                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_332rgb_pixel_blend Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_332rgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_332rgb_pixelmap_alpha_write(GX_DRAW_CONTEXT *context,
                                                           INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT             skipcount;
INT             xval;
INT             yval;
GX_UBYTE       *getrow;
GX_UBYTE       *getalpha_row;
const GX_UBYTE *getalpha;
const GX_UBYTE *get;

GX_RECTANGLE   *clip = context->clip;

    /* calculate how many pixels to skip */
    skipcount = (pixelmap->width) * (clip->top - ypos);
    skipcount += (clip->left - xpos);
    getrow = (GX_UBYTE *)(pixelmap->data);
    getrow += skipcount;
    getalpha_row = (GX_UBYTE *)(pixelmap->aux_data);
    getalpha_row += skipcount;

    for (yval  = clip->top; yval <= clip->bottom; yval++)
    {
        get = getrow;
        getalpha = getalpha_row;
        for (xval = clip->left; xval <= clip->right; xval++)
        {
            _gx_display_driver_332rgb_pixel_blend(context, xval, yval, *get++, *getalpha++);
        }
        getrow += pixelmap->width;
        getalpha_row += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_332rgb_pixelmap_compressed_alpha_write           */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixlemap file with alpha channel.                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_332rgb_pixel_blend Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_332rgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_332rgb_pixelmap_compressed_alpha_write(GX_DRAW_CONTEXT *context,
                                                                      INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           yval;
INT           xval;
const USHORT *get;
USHORT        count;
USHORT        pixel;
GX_UBYTE      alpha;
GX_UBYTE      brush_alpha;
GX_UBYTE      combined_alpha;
GX_RECTANGLE *clip = context->clip;

    get = (const USHORT *)pixelmap->data;
    brush_alpha = context->brush.alpha;
    /* compressed with alpha is 2-byte count, byte alpha, and and byte pixel value */

    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get++;

            if (count & 0x8000)
            {
                count = (USHORT)((count & 0x7fff) + 1u);
                get++;       /* skip repeated pixel value */
            }
            else
            {
                count++;
                get += count;      /* skip string of non-repeated pixels */
            }
            xval += count;
        }
    }

    /* now we are on the first visible row, copy pixels until we get
       to the end of the last visible row.
     */
    while (yval <= clip->bottom)
    {
        xval = xpos;
        while (xval < xpos + pixelmap->width)
        {
            count = *get++;

            if (count & 0x8000)
            {
                /* repeated value */
                count = (USHORT)((count & 0x7fff) + 1u);
                pixel = *get++;
                alpha = (GX_UBYTE)((pixel & 0xff00) >> 8);

                if (alpha)
                {
                    if (brush_alpha != 0xff)
                    {
                        combined_alpha = (GX_UBYTE)(alpha * brush_alpha / 255);
                        alpha = combined_alpha;
                    }

                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            _gx_display_driver_332rgb_pixel_blend(context, xval, yval, (GX_COLOR)(pixel & 0xff), alpha);
                        }
                        xval++;
                    }
                }
                else
                {
                    xval += count;
                }
            }
            else
            {
                /* string of non-repeated values */
                count++;
                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            pixel = *get;
                            alpha = (GX_UBYTE)((pixel & 0xff00) >> 8);
                            if (alpha)
                            {
                                _gx_display_driver_332rgb_pixel_blend(context, xval, yval, (GX_COLOR)(pixel & 0xff), alpha);
                            }
                        }
                        get++;
                        xval++;
                    }
                }
                else
                {
                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            pixel = *get;
                            alpha = (GX_UBYTE)((pixel & 0xff00) >> 8);
                            combined_alpha = (GX_UBYTE)(alpha * brush_alpha / 255);
                            _gx_display_driver_332rgb_pixel_blend(context, xval, yval, (GX_COLOR)(pixel & 0xff), combined_alpha);
                        }
                        get++;
                        xval++;
                    }
                }

            }
        }
        yval++;
    }
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_332rgb_pixelmap_draw                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    332rgb screen driver pixelmap drawing function that handles         */
/*    compressed or uncompress, with or without alpha channel.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*     _gx_display_driver_332rgb_pixelmap_compressed_alpha_write          */
/*                                          Real pixelmap draw function   */
/*     _gx_display_driver_332rgb_pixelmap_alpha_write                     */
/*                                          Real pixelmap draw function   */
/*     _gx_display_driver_332rgb_pixelmap_compressed_write                */
/*                                          Real pixelmap draw function   */
/*     _gx_display_driver_332rgb_pixelmap_raw_write                       */
/*                                          Real pixelmap draw function   */
/*     _gx_display_driver_8bpp_pixelmap_blend                             */
/*                                          Basic display driver pixelmap */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_332rgb_pixelmap_draw(GX_DRAW_CONTEXT *context,
                                             INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
GX_BOOL  drawn = GX_FALSE;
GX_UBYTE brush_alpha = context->brush.alpha;

    if (brush_alpha == 0)
    {
        /* Nothing to draw here. Just return. */
        return;
    }

    if (pixelmap->format != GX_COLOR_FORMAT_8BIT_PACKED_PIXEL)
    {
        return;
    }

    if (pixelmap->flags & GX_PIXELMAP_ALPHA)
    {
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            _gx_display_driver_332rgb_pixelmap_compressed_alpha_write(context, xpos, ypos, pixelmap);
            drawn = GX_TRUE;
        }
        else
        {
            if (brush_alpha == 0xff)
            {
                _gx_display_driver_332rgb_pixelmap_alpha_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
        }
    }
    else
    {
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            _gx_display_driver_332rgb_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
            drawn = GX_TRUE;
        }
        else
        {
            if (brush_alpha == 0xff)
            {
                _gx_display_driver_332rgb_pixelmap_raw_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
        }
    }

    if (!drawn)
    {
        _gx_display_driver_8bpp_pixelmap_blend(context, xpos, ypos, pixelmap, brush_alpha);
    }

    return;
}

