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
/*    _gx_display_driver_32xrgb_pixelmap_alpha_write                      */
/*                                                           6.1.7        */
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
/*     _gx_display_driver_32argb_pixel_blend                              */
/*                                          Display driver basic pixel    */
/*                                            blend function              */
/*     _gx_display_driver_32bpp_pixel_write                               */
/*                                          Display driver basic pixel    */
/*                                            write function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32argb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32argb_pixelmap_alpha_write(GX_DRAW_CONTEXT *context,
                                                           INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           xval;
INT           yval;
GX_COLOR      color;
INT           width;
ULONG        *get;
UCHAR         alpha_value;

GX_RECTANGLE *clip = context->clip;

    get = (ULONG *)((ULONG)pixelmap->data + (sizeof(GX_COLOR) * (UINT)(pixelmap->width) * (UINT)(((INT)(clip->top) - ypos))));
    get += (clip->left - xpos);

    width = clip->right - clip->left + 1;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        for (xval = clip->left; xval <= clip->right; xval++)
        {
            color = *get;
            alpha_value = (UCHAR)(color >> 24);
            if (alpha_value == 255)
            {
                _gx_display_driver_32bpp_pixel_write(context, xval, yval, color);
            }
            else
            {
                _gx_display_driver_32argb_pixel_blend(context, xval, yval, color, 0xff);
            }
            get++;
        }
        get += pixelmap->width - width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32xrgb_pixelmap_compressed_alpha_write           */
/*                                                                        */
/*                                                           6.1.12       */
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
/*     _gx_display_driver_32argb_pixel_blend                              */
/*                                          Display driver basic pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32argb_pixelmap_compressed_alpha_write(GX_DRAW_CONTEXT *context,
                                                                      INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                yval;
INT                xval;
const GX_COLOR *get;
GX_UBYTE           count;
GX_COLOR           pixel;
const GX_UBYTE *get_count;
GX_UBYTE           brush_alpha;
GX_UBYTE           alpha;

GX_RECTANGLE      *clip = context->clip;

    brush_alpha = context->brush.alpha;

    get = (const GX_COLOR *)pixelmap->data;
    get_count = (const GX_UBYTE *)pixelmap->aux_data;

    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get_count++;

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
       to the end of the last visible row
     */

    while (yval <= clip->bottom)
    {
        xval = xpos;

        while (xval < xpos + pixelmap->width)
        {
            count = *get_count++;

            if (count & 0x80)
            {
                /* repeated value */
                count = (GX_UBYTE)((count & 0x7f) + 1u);
                alpha = (GX_UBYTE)((*get) >> 24);
                pixel = *get++;

                if (alpha)
                {
                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            _gx_display_driver_32argb_pixel_blend(context, xval, yval, pixel, brush_alpha);
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

                while (count--)
                {
                    if (xval >= clip->left &&
                        xval <= clip->right)
                    {
                        pixel = *get;

                        _gx_display_driver_32argb_pixel_blend(context, xval, yval, pixel, brush_alpha);
                    }
                    get++;
                    xval++;
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
/*    _gx_display_driver_32argb_pixelmap_compressed_write                 */
/*                                                                        */
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
/*     _gx_display_driver_32argb_pixel_blend                              */
/*                                          Display driver basic pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32argb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32argb_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                                INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                yval;
INT                xval;
const GX_COLOR *get;
GX_COLOR          *put;
GX_COLOR          *putrow;
GX_UBYTE           count;
GX_COLOR           pixel;
const GX_UBYTE *get_count;
GX_UBYTE           brush_alpha;

GX_RECTANGLE      *clip = context->clip;

    brush_alpha = context->brush.alpha;

    get = (const GX_COLOR *)pixelmap->data;
    get_count = (const GX_UBYTE *)pixelmap->aux_data;

    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get_count++;

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
    putrow = (GX_COLOR *)context->memory;
    putrow += yval * context->pitch;
    putrow += xpos;

    while (yval <= clip->bottom)
    {
        put = putrow;
        xval = xpos;

        while (xval < xpos + pixelmap->width)
        {
            count = *get_count++;

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
                            _gx_display_driver_32argb_pixel_blend(context, xval, yval, pixel, brush_alpha);
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
                            _gx_display_driver_32argb_pixel_blend(context, xval, yval, *get, brush_alpha);
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
/*    _gx_display_driver_32argb_pixelmap_raw_write                        */
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
/*    _gx_display_driver_32argb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32argb_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
                                                         INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           xval;
INT           yval;
INT           width;
GX_COLOR     *putrow;
GX_COLOR     *getrow;
GX_COLOR     *put;
GX_COLOR     *get;

GX_RECTANGLE *clip = context->clip;

    getrow = (GX_COLOR *)(pixelmap->data);
    getrow += pixelmap->width * (clip->top - ypos);
    getrow += (clip->left - xpos);

    /* brush alpha is 0xff means draw pixelmap to memory directly. */
    putrow = context->memory;
    putrow += clip->top * context->pitch;
    putrow += clip->left;

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
/*    _gx_display_driver_32xrgb_pixelmap_draw                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    32xrgb format screen driver pixelmap drawing function that handles  */
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
/*     _gx_display_driver_32argb_pixelmap_compressed_alpha_write          */
/*                                          Real Pixelmap draw function   */
/*     _gx_display_driver_32argb_pixelmap_alpha_write                     */
/*                                          Real Pixelmap draw function   */
/*     _gx_display_driver_32argb_pixelmap_compressed_write                */
/*                                          Real Pixelmap draw function   */
/*     _gx_display_driver_32argb_pixelmap_raw_write                       */
/*                                          Real Pixelmap draw function   */
/*     _gx_display_driver_32argb_pixelmap_blend                           */
/*                                          Basic display driver function */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_32argb_pixelmap_draw(GX_DRAW_CONTEXT *context,
                                             INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
GX_BOOL  drawn = GX_FALSE;
GX_UBYTE brush_alpha = context->brush.alpha;

    if (brush_alpha == 0)
    {
        /* Draw nothing here. Just return. */
        return;
    }

    if (pixelmap->flags & GX_PIXELMAP_TRANSPARENT)
    {
        /* Not supported yet. */
        return;
    }

    if (pixelmap->format != GX_COLOR_FORMAT_32ARGB)
    {
        /* wrong color format for this driver */
        return;
    }

    if (pixelmap->flags & GX_PIXELMAP_ALPHA)
    {
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /* has both compression and alpha */
            _gx_display_driver_32argb_pixelmap_compressed_alpha_write(context, xpos, ypos, pixelmap);
            drawn = GX_TRUE;
        }
        else
        {
            if (brush_alpha == 0xff)
            {
                /* alpha, no compression */
                _gx_display_driver_32argb_pixelmap_alpha_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
        }
    }
    else
    {
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /* compressed with no alpha */
            _gx_display_driver_32argb_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
            drawn = GX_TRUE;
        }
        else
        {
            /* no compression or alpha */
            if (brush_alpha == 0xff)
            {
                _gx_display_driver_32argb_pixelmap_raw_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
        }
    }

    if (!drawn)
    {
        _gx_display_driver_32argb_pixelmap_blend(context, xpos, ypos, pixelmap, brush_alpha);
    }
}

