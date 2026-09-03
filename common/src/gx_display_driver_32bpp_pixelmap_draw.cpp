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
/*    _gx_display_driver_24xrgb_pixelmap_raw_write                        */
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
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
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
/*    _gx_display_driver_24xrgb_pixelmap_alpha_write                      */
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
/*    _gx_display_driver_24xrgb_pixel_blend Basic display driver pixel    */
/*                                            blend function for 24xrgb   */
/*                                            format                      */
/*    _gx_display_driver_32bpp_pixel_write  Basic display driver pixel    */
/*                                            write function for 32bpp    */
/*                                            color depth                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_pixelmap_alpha_write(GX_DRAW_CONTEXT *context,
                                                           INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           xval;
INT           yval;
GX_COLOR      color;
INT           width;
ULONG        *get;
UCHAR         alpha_value;
GX_RECTANGLE *clip = context->clip;

    get = (ULONG *)((UINT)(pixelmap->data) + (INT)sizeof(GX_COLOR) * (UINT)(pixelmap->width) * (UINT)((INT)(clip->top) - ypos));
    get += (clip->left - xpos);

    width = clip->right - clip->left + 1;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        for (xval = clip->left; xval <= clip->right; xval++)
        {
            alpha_value = (UCHAR)(*get >> 24);
            if (alpha_value)
            {
                color = *get & 0x00ffffff;
                if (alpha_value == 255)
                {
                    _gx_display_driver_32bpp_pixel_write(context, xval, yval, color);
                }
                else
                {
                    _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, color, alpha_value);
                }
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
/*    _gx_display_driver_24xrgb_pixelmap_compressed_write                 */
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
/*    _gx_display_driver_24xrgb_pixel_blend Basic display driver pixel    */
/*                                            blend function for 24xrgb   */
/*                                            format                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                                INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                yval;
INT                xval;
const GX_COLOR    *get;
GX_COLOR          *put;
GX_COLOR          *putrow;
GX_UBYTE           count;
GX_COLOR           pixel;
const GX_UBYTE    *get_count;
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
                pixel = (*get++) & 0x00ffffff;

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
                            _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
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
                            *put = (*get) & 0x00ffffff;
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
                            _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, *get, brush_alpha);
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
/*    _gx_display_driver_24xrgb_pixelmap_compressed_alpha_write           */
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
/*    _gx_display_driver_24xrgb_pixel_blend Basic display driver pixel    */
/*                                            blend function for 24xrgb   */
/*                                            format                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_pixelmap_compressed_alpha_write(GX_DRAW_CONTEXT *context,
                                                                      INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                yval;
INT                xval;
const GX_COLOR    *get;
GX_UBYTE           count;
GX_COLOR           pixel;
const GX_UBYTE    *get_count;
GX_UBYTE           brush_alpha;
GX_UBYTE           alpha;
GX_UBYTE           combined_alpha;

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
                pixel = (*get++) & 0x00ffffff;

                combined_alpha = (GX_UBYTE)(alpha * brush_alpha / 255);

                if (combined_alpha)
                {
                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, combined_alpha);
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
                        alpha = (GX_UBYTE)((*get) >> 24);
                        pixel = (*get) & 0x00ffffff;
                        combined_alpha = (GX_UBYTE)(alpha * brush_alpha / 255);
                        _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, combined_alpha);
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
/*    _gx_display_driver_24xrgb_palette_pixelmap_compressed_write         */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixlemap file without transparent of palette pixelmap.              */
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
/*    _gx_display_driver_24xrgb_pixel_blend Basic display driver pixel    */
/*                                            blend function for 24xrgb   */
/*                                            format                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_palette_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                                        INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT             yval;
INT             xval;
const UCHAR    *get;
UCHAR           count;
GX_COLOR       *put;
GX_COLOR       *putrow;
GX_COLOR        pixel;
GX_COLOR       *palette;
GX_UBYTE        brush_alpha;
GX_RECTANGLE   *clip = context->clip;

    get = (const UCHAR *)pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* compressed with  alpha is a one-byte count and  one-byte pixel index */
    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (UCHAR)((count & 0x7f) + 1);
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

    /* Now we are on the first visible row, copy pixels until we get
       to the end of the last visible row. */

    palette = (GX_COLOR *)pixelmap->aux_data;

    while (yval <= clip->bottom)
    {
        xval = xpos;
        put = putrow;

        while (xval < xpos + pixelmap->width)
        {
            count = *get++;

            if (count & 0x80)
            {
                /* repeated value */
                count = (UCHAR)((count & 0x7f) + 1);

                pixel = palette[*get++] & 0x00ffffff;

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
                            _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
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
                            *put = palette[*get] & 0x00ffffff;
                        }
                        get++;
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
                            pixel = palette[*get] & 0x00ffffff;
                            _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
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
/*    _gx_display_driver_24xrgb_palette_pixelmap_write                    */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of uncompressed       */
/*    pixlemap file without transparent of palette pixelmap.              */
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
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_palette_pixelmap_write(GX_DRAW_CONTEXT *context,
                                                             INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           xval;
INT           yval;
INT           width;
GX_UBYTE     *get;
GX_COLOR     *palette;
GX_COLOR     *put;
GX_COLOR     *putrow;

GX_RECTANGLE *clip = context->clip;

    yval = clip->top;

    get = (GX_UBYTE *)(pixelmap->data + pixelmap->width * (clip->top - ypos));
    get += (clip->left - xpos);

    palette = (GX_COLOR *)pixelmap->aux_data;

    if (!palette)
    {
        return;
    }
    /* now we are on the first visible row, copy pixels until we get
       to the enf of the last visible row
     */
    putrow = (GX_COLOR *)context->memory;
    putrow += yval * context->pitch;
    putrow += xpos;

    width = clip->right - clip->left + 1;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        put = putrow;
        for (xval = clip->left; xval <= clip->right; xval++)
        {
            *put++ = palette[*get++] & 0x00ffffff;
        }
        putrow += context->pitch;
        get += pixelmap->width - width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_palette_pixelmap_transparent_write        */
/*                                                                        */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of uncompressed       */
/*    pixlemap file with transparent of palette pixelmap.                 */
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
/*    _gx_display_driver_32bpp_pixel_write  Basic display driver pixel    */
/*                                            write function for 32bpp    */
/*                                            color depth                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_palette_pixelmap_transparent_write(GX_DRAW_CONTEXT *context,
                                                                         INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           xval;
INT           yval;
GX_COLOR      color;
INT           width;
GX_UBYTE     *get;
GX_COLOR     *palette;

GX_RECTANGLE *clip = context->clip;

    palette = (GX_COLOR *)pixelmap->aux_data;

    if (!palette)
    {
        return;
    }

    get = (GX_UBYTE *)(pixelmap->data + pixelmap->width * (clip->top - ypos));
    get += (clip->left - xpos);

    width = clip->right - clip->left + 1;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        for (xval = clip->left; xval <= clip->right; xval++)
        {
            if ((*get) != pixelmap->transparent_color)
            {
                color = palette[*get] & 0x00ffffff;
                _gx_display_driver_32bpp_pixel_write(context, xval, yval, color);
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
/*    _gx_display_driver_24xrgb_palette_pixelmap_transparent_compressed   */
/*    _write                                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixlemap file with transparent of palette pixelmap.                 */
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
/*    _gx_display_driver_24xrgb_pixel_blend Basic display driver pixel    */
/*                                            blend function for 24xrgb   */
/*                                            format                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_palette_pixelmap_transparent_compressed_write(GX_DRAW_CONTEXT *context,
                                                                                    INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT             yval;
INT             xval;
const UCHAR    *get;
UCHAR           count;
GX_COLOR        pixel;
GX_COLOR       *palette;
GX_COLOR       *put;
GX_COLOR       *putrow;
GX_UBYTE        brush_alpha;
GX_RECTANGLE   *clip = context->clip;

    get = (const UCHAR *)pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* compressed with  alpha is a one-byte count and  one-byte pixel index */
    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (UCHAR)((count & 0x7f) + 1);
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

    /* Now we are on the first visible row, copy pixels until we get
       to the end of the last visible row. */

    palette = (GX_COLOR *)pixelmap->aux_data;
    putrow = (GX_COLOR *)context->memory;
    putrow += yval * context->pitch;
    putrow += xpos;

    while (yval <= clip->bottom)
    {
        xval = xpos;
        put = putrow;

        while (xval < xpos + pixelmap->width)
        {
            count = *get++;
            if (count & 0x80)
            {
                /* repeated value */
                count = (UCHAR)((count & 0x7f) + 1);
                if ((*get) != pixelmap->transparent_color)
                {
                    pixel = palette[*get] & 0x00ffffff;
                    if (brush_alpha == 0xff)
                    {
                        while (count--)
                        {
                            if (xval >= clip->left &&
                                xval <= clip->right)
                            {
                                *put = pixel;
                            }
                            xval++;
                            put++;
                        }
                    }
                    else
                    {
                        while (count--)
                        {
                            if (xval >= clip->left &&
                                xval <= clip->right)
                            {
                                _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
                            }
                            xval++;
                        }
                    }
                }
                else
                {
                    xval += count;
                    put += count;
                }

                get++;
            }
            else
            {
                /* string of non-repeated values */
                count++;
                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if ((*get) != pixelmap->transparent_color)
                        {
                            if (xval >= clip->left &&
                                xval <= clip->right)
                            {
                                *put = palette[*get] & 0x00ffffff;
                            }
                        }
                        get++;
                        xval++;
                        put++;
                    }
                }
                else
                {
                    while (count--)
                    {
                        if ((*get) != pixelmap->transparent_color)
                        {
                            if (xval >= clip->left &&
                                xval <= clip->right)
                            {
                                pixel = palette[*get] & 0x00ffffff;
                                _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
                            }
                        }
                        get++;
                        xval++;
                    }
                }
            }
        }
        yval++;
        putrow += context->pitch;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_565rgb_pixelmap_write                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of 565rgb format      */
/*    uncompressed pixlemap file without alpha channel.                   */
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
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_565rgb_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
                                                                INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              xval;
INT              yval;
INT              width;
GX_COLOR        *putrow;
USHORT          *getrow;
GX_COLOR        *put;
const USHORT    *get;
USHORT           pixel;

GX_RECTANGLE    *clip = context->clip;

    putrow = (GX_COLOR *)context->memory;
    putrow += clip->top * context->pitch;
    putrow += clip->left;

    getrow = (USHORT *)(pixelmap->data);
    getrow += pixelmap->width * (clip->top - ypos);
    getrow += (clip->left - xpos);

    width = clip->right - clip->left + 1;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        put = putrow;
        get = getrow;

        for (xval = 0; xval < width; xval++)
        {
            pixel = *get++;
            *put++ = GX_COLOR_24RGB_FROM_565RGB(pixel);
        }
        putrow += context->pitch;
        getrow += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_565rgb_pixelmap_compressed_write          */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixelmap data of 565rgb format in 32bpp driver.                     */
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
/*    _gx_display_driver_24xrgb_pixel_blend Basic display driver pixel    */
/*                                            blend function for 24xrgb   */
/*                                            format                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_565rgb_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                                       INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              yval;
INT              xval;
const USHORT    *get;
USHORT           count;
GX_COLOR         pixel;
GX_UBYTE         brush_alpha;
GX_COLOR        *put;
GX_COLOR        *putrow;
GX_RECTANGLE    *clip = context->clip;

    get = (const USHORT *)pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get++;

            if (count & 0x8000)
            {
                count = (USHORT)((count & 0x7fff) + 1);
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
        xval = xpos;
        put = putrow;

        while (xval < xpos + pixelmap->width)
        {
            count = *get++;

            if (count & 0x8000)
            {
                /* repeated value */
                count = (USHORT)((count & 0x7fff) + 1);
                pixel = *get++;
                pixel = GX_COLOR_24RGB_FROM_565RGB(pixel);
                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            *put = pixel;
                        }
                        xval++;
                        put++;
                    }
                }
                else
                {
                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
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
                            pixel = *get;
                            pixel = GX_COLOR_24RGB_FROM_565RGB(pixel);
                            *put = pixel;
                        }
                        get++;
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
                            pixel = *get;
                            pixel = GX_COLOR_24RGB_FROM_565RGB(pixel);
                            _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
                        }
                        get++;
                        xval++;
                    }
                }
            }
        }
        yval++;
        putrow += context->pitch;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_565rgb_pixelmap_compressed_alpha_write    */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed-alpha   */
/*    pixelmap data of 565rgb format with 32bpp driver.                   */
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
/*    _gx_display_driver_24xrgb_pixel_blend Basic display driver pixel    */
/*                                            blend function for 24xrgb   */
/*                                            format                      */
/*    _gx_display_driver_32bpp_pixel_write  Basic display driver pixel    */
/*                                            write function for 32bpp    */
/*                                            color depth                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_565rgb_pixelmap_compressed_alpha_write(GX_DRAW_CONTEXT *context,
                                                                             INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                yval;
INT                xval;
const GX_UBYTE    *get;
GX_UBYTE           count;
GX_COLOR           pixel;
GX_UBYTE           alpha_value;
GX_UBYTE           brush_alpha;
GX_UBYTE           combined_alpha;

GX_RECTANGLE      *clip = context->clip;

    get = (const GX_UBYTE *)pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get;

            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1);
                get += 4;      /* skip repeated pixel value */
            }
            else
            {
                count++;
                get += (count * 4);   /* skip raw pixel values */
            }
            xval += count;
        }
    }

    /* now we are on the first visible row, copy pixels until we get
       to the enf of the last visible row
     */
    while (yval <= clip->bottom)
    {
        xval = xpos;

        while (xval < xpos + pixelmap->width)
        {
            count = *get;

            if (count & 0x80)
            {
                /* repeated value */
                count = (GX_UBYTE)((count & 0x7f) + 1);
                alpha_value = *(get + 1);

                if (alpha_value)
                {
                    if (brush_alpha == 0xff)
                    {
                        get += 2;
                        pixel = *(USHORT *)get;
                        pixel = GX_COLOR_24RGB_FROM_565RGB(pixel);
                        get += 2;
                        while (count--)
                        {
                            if (xval >= clip->left &&
                                xval <= clip->right)
                            {
                                if (alpha_value == 0xff)
                                {
                                    _gx_display_driver_32bpp_pixel_write(context, xval, yval, pixel);
                                }
                                else
                                {
                                    _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, alpha_value);
                                }
                            }
                            xval++;
                        }
                    }
                    else
                    {
                        combined_alpha = (GX_UBYTE)(alpha_value * brush_alpha / 255);
                        if (combined_alpha)
                        {
                            get += 2;
                            pixel = *(USHORT *)get;
                            pixel = GX_COLOR_24RGB_FROM_565RGB(pixel);
                            get += 2;
                            while (count--)
                            {
                                if (xval >= clip->left &&
                                    xval <= clip->right)
                                {
                                    _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, combined_alpha);
                                }
                                xval++;
                            }
                        }
                        else
                        {
                            get += 4;
                            xval += count;
                        }
                    }
                }
                else
                {
                    xval += count;
                    get += 4;
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
                            alpha_value = *(get + 1);
                            get += 2;
                            if (alpha_value)
                            {
                                pixel = *(USHORT *)get;
                                pixel = GX_COLOR_24RGB_FROM_565RGB(pixel);
                                if (alpha_value == 0xff)
                                {
                                    _gx_display_driver_32bpp_pixel_write(context, xval, yval, pixel);
                                }
                                else
                                {
                                    _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, alpha_value);
                                }
                            }
                            get += 2;
                        }
                        else
                        {
                            get += 4;
                        }
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
                            alpha_value = *(get + 1);
                            get += 2;
                            if (alpha_value)
                            {
                                pixel = *(USHORT *)get;
                                pixel = GX_COLOR_24RGB_FROM_565RGB(pixel);
                                combined_alpha = (GX_UBYTE)(brush_alpha * alpha_value / 255);
                                _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, combined_alpha);
                            }
                            get += 2;
                        }
                        else
                        {
                            get += 4;
                        }
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
/*    _gx_display_driver_24xrgb_565rgb_pixelmap_alpha_write               */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of non_compressed     */
/*    but with alpha channel pixelmap data of 565rgb format with 32bpp    */
/*    driver.                                                             */
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
/*    _gx_display_driver_24xrgb_pixel_blend Basic display driver pixel    */
/*                                            blend function for 24xrgb   */
/*                                            format                      */
/*    _gx_display_driver_32bpp_pixel_write  Basic display driver pixel    */
/*                                            write function for 32bpp    */
/*                                            color depth                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_565rgb_pixelmap_alpha_write(GX_DRAW_CONTEXT *context,
                                                                  INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                skipcount;
INT                xval;
INT                yval;
const GX_UBYTE    *getalpha;
const USHORT      *get;
USHORT            *getrow;
GX_UBYTE          *getrowalpha;
GX_COLOR           pixel;
GX_UBYTE           alpha_value;

GX_RECTANGLE      *clip = context->clip;

    skipcount = (pixelmap->width) * (clip->top - ypos);
    skipcount += (clip->left - xpos);
    getrow = (USHORT *)(pixelmap->data);
    getrow += skipcount;

    getrowalpha = (GX_UBYTE *)(pixelmap->aux_data);
    getrowalpha += skipcount;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        get = getrow;
        getalpha = getrowalpha;

        for (xval = clip->left; xval <= clip->right; xval++)
        {
            alpha_value = *getalpha++;
            pixel = *get++;
            if (alpha_value)
            {
                pixel = GX_COLOR_24RGB_FROM_565RGB(pixel);
                if (alpha_value == 0xff)
                {
                    _gx_display_driver_32bpp_pixel_write(context, xval, yval, pixel);
                }
                else
                {
                    _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, alpha_value);
                }
            }
        }
        getrow += pixelmap->width;
        getrowalpha += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_4444argb_pixelmap_alpha_write             */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of uncompressed       */
/*    pixlemap file with alpha channel of 4444argb format.                */
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
/*    _gx_display_driver_24xrgb_pixel_blend Basic display driver pixel    */
/*                                            blend function for 24xrgb   */
/*                                            format                      */
/*    _gx_display_driver_32bpp_pixel_write  Basic display driver pixel    */
/*                                            write function for 32bpp    */
/*                                            color depth                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_4444argb_pixelmap_alpha_write(GX_DRAW_CONTEXT *context,
                                                                    INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              skipcount;
INT              xval;
INT              yval;
USHORT          *getrow;
const USHORT    *get;
UCHAR            alpha_value;
ULONG            pixel;

GX_RECTANGLE    *clip = context->clip;

    /* calculate how many pixels to skip */
    skipcount = (pixelmap->width) * (clip->top - ypos);
    skipcount += (clip->left - xpos);
    getrow = (USHORT *)(pixelmap->data);
    getrow += skipcount;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        get = getrow;

        for (xval = clip->left; xval <= clip->right; xval++)
        {
            alpha_value = (UCHAR)(((*get) & 0xf000) >> 8);
            if (alpha_value)
            {
                pixel = *get;
                pixel = GX_COLOR_24RGB_FROM_4444ARGB(pixel);
                if (alpha_value == 0xf0)
                {
                    _gx_display_driver_32bpp_pixel_write(context, xval, yval, pixel);
                }
                else
                {
                    _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, alpha_value);
                }
            }
            get++;
        }
        getrow += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_4444argb_pixelmap_compressed_alpha_write  */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixelmap data of 4444argb format.                                   */
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
/*    _gx_display_driver_24xrgb_pixel_blend Basic display driver pixel    */
/*                                            blend function for 24xrgb   */
/*                                            format                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_4444argb_pixelmap_compressed_alpha_write(GX_DRAW_CONTEXT *context,
                                                                               INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              yval;
INT              xval;
const USHORT    *get;
USHORT           count;
GX_COLOR         pixel;
GX_UBYTE         falpha;
GX_UBYTE         brush_alpha;
GX_UBYTE         combined_alpha;

GX_RECTANGLE    *clip = context->clip;

    get = (const USHORT *)pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get++;

            if (count & 0x8000)
            {
                count = (USHORT)((count & 0x7fff) + 1);
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
    while (yval <= clip->bottom)
    {
        xval = xpos;

        while (xval < xpos + pixelmap->width)
        {
            count = *get++;

            if (count & 0x8000)
            {
                /* repeated value */
                count = (USHORT)((count & 0x7fff) + 1);
                pixel = *get++;
                falpha = (GX_UBYTE)(((USHORT)pixel & 0xf000) >> 8);
                falpha = (falpha >> 4) | falpha;
                if (falpha)
                {
                    pixel = gx_color_24rgb_from_4444argb((USHORT)pixel);
                    if (brush_alpha == 0xff)
                    {
                        combined_alpha = falpha;
                    }
                    else
                    {
                        combined_alpha = (GX_UBYTE)(brush_alpha * falpha / 255);
                    }

                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, combined_alpha);
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
                            falpha = (GX_UBYTE)(((USHORT)pixel & 0xf000) >> 8);
                            falpha = (falpha >> 4) | falpha;
                            if (falpha)
                            {
                                pixel = gx_color_24rgb_from_4444argb((USHORT)pixel);
                                _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, falpha);
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
                            falpha = (GX_UBYTE)(((USHORT)pixel & 0xf000) >> 8);
                            falpha = (falpha >> 4) | falpha;
                            combined_alpha = (GX_UBYTE)(falpha * brush_alpha / 255);
                            if (combined_alpha)
                            {
                                pixel = gx_color_24rgb_from_4444argb((USHORT)pixel);
                                _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, combined_alpha);
                            }
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
/*    _gx_display_driver_24xrgb_pixelmap_draw                             */
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
/*    _gx_display_driver_24xrgb_palette_pixelmap_transparent_c_write      */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_palette_pixelmap_transparent_write        */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_palette_pixelmap_compressed_write         */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_palette_pixelmap_write                    */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_4444argb_pixelmap_compressed_alpha_write  */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_4444argb_pixelmap_alpha_write             */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_565rgb_pixelmap_compressed_alpha_write    */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_565rgb_pixelmap_alpha_write               */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_565rgb_pixelmap_compressed_write          */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_565rgb_pixelmap_raw_write                 */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_pixelmap_compressed_alpha_write           */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_pixelmap_alpha_write                      */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_pixelmap_compressed_write                 */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_pixelmap_raw_write                        */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_24xrgb_pixelmap_blend                            */
/*                                          Basic display driver pixelmap */
/*                                            blend function with alpha   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_24xrgb_pixelmap_draw(GX_DRAW_CONTEXT *context,
                                             INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
GX_BOOL  drawn = GX_FALSE;
GX_UBYTE brush_alpha = context->brush.alpha;

    if (brush_alpha == 0)
    {
        /* Draw nothing here. Just return. */
        return;
    }

    switch (pixelmap->format)
    {
    case GX_COLOR_FORMAT_8BIT_PALETTE:
        if (pixelmap->flags & GX_PIXELMAP_TRANSPARENT)
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* compressed with transparent */
                _gx_display_driver_24xrgb_palette_pixelmap_transparent_compressed_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* no compression with transparent */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_24xrgb_palette_pixelmap_transparent_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        else
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* compressed with no alpha */
                _gx_display_driver_24xrgb_palette_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* no compression or alpha */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_24xrgb_palette_pixelmap_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /* compressed */
            _gx_display_driver_24xrgb_4444argb_pixelmap_compressed_alpha_write(context, xpos, ypos, pixelmap);
            drawn = GX_TRUE;
        }
        else
        {
            /* no compression  */
            if (brush_alpha == 0xff)
            {
                _gx_display_driver_24xrgb_4444argb_pixelmap_alpha_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
        }
        break;

    case GX_COLOR_FORMAT_565RGB:
        if (pixelmap->flags & GX_PIXELMAP_ALPHA)
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* compressed with alpha */
                _gx_display_driver_24xrgb_565rgb_pixelmap_compressed_alpha_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* uncompressed with alpha */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_24xrgb_565rgb_pixelmap_alpha_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        else
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* compressed without alpha */
                _gx_display_driver_24xrgb_565rgb_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* uncompressed withou alpha */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_24xrgb_565rgb_pixelmap_raw_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        break;

    case GX_COLOR_FORMAT_24XRGB:
    case GX_COLOR_FORMAT_32ARGB:
        if (pixelmap->flags & GX_PIXELMAP_ALPHA)
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* has both compression and alpha */
                _gx_display_driver_24xrgb_pixelmap_compressed_alpha_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* alpha, no compression */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_24xrgb_pixelmap_alpha_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        else
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* compressed with no alpha */
                _gx_display_driver_24xrgb_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* no compression or alpha */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_24xrgb_pixelmap_raw_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        break;

    default:
        break;
    }

    if ((!drawn) && (brush_alpha != 0xff))
    {
        _gx_display_driver_24xrgb_pixelmap_blend(context, xpos, ypos, pixelmap, brush_alpha);
    }

    return;
}

