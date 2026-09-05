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
/*    _gx_display_driver_565rgb_pixelmap_raw_write                        */
/*                                                           6.X          */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
                                                         INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              xval;
INT              yval;
INT              width;
USHORT          *putrow;
USHORT          *getrow;
USHORT          *put;
const USHORT    *get;

GX_RECTANGLE    *clip = context->clip;

    putrow = (USHORT *)context->memory;

    GX_CALCULATE_PUTROW(putrow, clip->left, clip->top, context);

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
/*    _gx_display_driver_565rgb_pixelmap_alpha_write                      */
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
/*    _gx_display_driver_565rgb_pixel_blend  Display driver basic pixel   */
/*                                             blend function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_pixelmap_alpha_write(GX_DRAW_CONTEXT *context,
                                                           INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                skipcount;
INT                xval;
INT                yval;
USHORT            *getrow;
GX_UBYTE          *getrowalpha;
const USHORT      *get;
const GX_UBYTE    *getalpha;

GX_RECTANGLE      *clip = context->clip;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;
    if (blend_func == GX_NULL)
    {
        return;
    }

    /* calculate how many pixels to skip */
    skipcount = (pixelmap->width) * (clip->top - ypos);
    skipcount += (clip->left - xpos);
    getrow = (USHORT *)(pixelmap->data);
    getrow += skipcount;

    getrowalpha = (GX_UBYTE *)(pixelmap->aux_data);
    getrowalpha += skipcount;

    for (yval  = clip->top; yval <= clip->bottom; yval++)
    {
        get = getrow;
        getalpha = getrowalpha;

        for (xval = clip->left; xval <= clip->right; xval++)
        {
            blend_func(context, xval, yval, *get++, *getalpha++);
        }
        getrow += pixelmap->width;
        getrowalpha += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_pixelmap_compressed_write                 */
/*                                                                        */
/*                                                           6.X          */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                                INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              yval;
INT              xval;
const USHORT    *get;
USHORT          *put;
USHORT          *putrow;
USHORT           count;
USHORT           pixel;

GX_RECTANGLE    *clip = context->clip;

    get = (const USHORT *)pixelmap->data;
    /* compressed with no alpha is a two-byte count and two-byte pixel value */

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
    putrow = (USHORT *)context->memory;

    GX_CALCULATE_PUTROW(putrow, xpos, yval, context);

    while (yval <= clip->bottom)
    {
        put = putrow;
        xval = xpos;

        while (xval < xpos + pixelmap->width)
        {
            count = *get++;

            if (count & 0x8000)
            {
                /* repeated value */
                count = (USHORT)((count & 0x7fff) + 1u);
                pixel = *get++;
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
                /* string of non-repeated values */
                count++;

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
        }
        putrow +=  context->pitch;
        yval++;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_pixelmap_compressed_alpha_write           */
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
/*    _gx_display_driver_565rgb_pixel_blend  Display driver basic pixel   */
/*                                             blend function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_pixelmap_compressed_alpha_write(GX_DRAW_CONTEXT *context,
                                                                      INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                yval;
INT                xval;
const GX_UBYTE    *get;
const USHORT      *getpixel;
USHORT             count;
USHORT             pixel;
GX_UBYTE           falpha;
GX_UBYTE           brush_alpha;
GX_UBYTE           combined_alpha;

GX_RECTANGLE      *clip = context->clip;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;
    if (blend_func == GX_NULL)
    {
        return;
    }

    get = pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* compressed with alpha is byte count, byte alpha, and and two-byte pixel value */

    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get;

            if (count & 0x80)
            {
                count = (USHORT)((count & 0x7f) + 1u);
                get +=  4;      /* skip repeated pixel value */
            }
            else
            {
                count++;
                get += (count * 4);      /* skip string of non-repeated pixels */
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
                count = (USHORT)((count & 0x7f) + 1u);
                falpha = *(get + 1);

                if (falpha)
                {
                    get += 2;

                    getpixel = (USHORT *)get;
                    pixel = *getpixel;
                    get += 2;

                    if (brush_alpha == 0xff)
                    {
                        combined_alpha = falpha;
                    }
                    else
                    {
                        combined_alpha = (GX_UBYTE)(falpha * brush_alpha / 255);
                    }

                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            blend_func(context, xval, yval, pixel, combined_alpha);
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
                            falpha = *(get + 1);
                            get += 2;
                            getpixel = (USHORT *)get;
                            pixel = *getpixel;
                            get += 2;
                            blend_func(context, xval, yval, pixel, falpha);
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
                            falpha = *(get + 1);
                            get += 2;
                            getpixel = (USHORT *)get;
                            pixel = *getpixel;
                            get += 2;
                            combined_alpha = (GX_UBYTE)(falpha * brush_alpha / 255);
                            blend_func(context, xval, yval, pixel, combined_alpha);
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
/*    _gx_display_driver_565rgb_palette_pixelmap_raw_write                */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of raw pixlemap       */
/*    file without transparent for palette pixelmap                       */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_palette_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
                                                                 INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                xval;
INT                yval;
INT                width;
USHORT            *putrow;
GX_UBYTE          *getrow;
USHORT            *put;
const GX_UBYTE    *get;
GX_COLOR          *palette;
GX_COLOR           color;

GX_RECTANGLE      *clip = context->clip;

    putrow = (USHORT *)context->memory;

    GX_CALCULATE_PUTROW(putrow, clip->left, clip->top, context);

    getrow = (GX_UBYTE *)(pixelmap->data);
    getrow += pixelmap->width * (clip->top - ypos);
    getrow += (clip->left - xpos);

    palette = (GX_COLOR *)pixelmap->aux_data;

    width = clip->right - clip->left + 1;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        put = putrow;
        get = getrow;

        for (xval = 0; xval < width; xval++)
        {
            color = palette[*get++];
            *put++ = GX_COLOR_565RGB_FROM_32RGB(color);
        }
        putrow += context->pitch;
        getrow += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_palette_pixelmap_transparent_raw_write    */
/*                                                                        */
/*                                                           6.X          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of raw pixlemap       */
/*    file with transparent for palette pixelmap.                         */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_palette_pixelmap_transparent_raw_write(GX_DRAW_CONTEXT *context,
                                                                             INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                xval;
INT                yval;
INT                width;
USHORT            *putrow;
GX_UBYTE          *getrow;
USHORT            *put;
const GX_UBYTE    *get;
GX_COLOR          *palette;
GX_COLOR           color;

GX_RECTANGLE      *clip = context->clip;

    putrow = (USHORT *)context->memory;
    GX_CALCULATE_PUTROW(putrow, clip->left, clip->top, context);

    getrow = (GX_UBYTE *)(pixelmap->data);
    getrow += pixelmap->width * (clip->top - ypos);
    getrow += (clip->left - xpos);

    palette = (GX_COLOR *)pixelmap->aux_data;

    width = clip->right - clip->left + 1;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        put = putrow;
        get = getrow;

        for (xval = 0; xval < width; xval++)
        {
            if ((*get) != pixelmap->transparent_color)
            {
                color = palette[*get];
                *put = GX_COLOR_565RGB_FROM_32RGB(color);
            }
            get++;
            put++;
        }
        putrow += context->pitch;
        getrow += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_palette_pixelmap_transparent_compressed   */
/*      _write                                                            */
/*                                                                        */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixlemap file with transparent for palette pixelmap                 */
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
/*    _gx_display_driver_565rgb_pixel_blend  Display driver basic pixel   */
/*                                             blend function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_palette_pixelmap_transparent_compressed_write(GX_DRAW_CONTEXT *context,
                                                                                    INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                yval;
INT                xval;
const GX_UBYTE    *get;
USHORT            *put;
USHORT            *putrow;
GX_COLOR          *palette;
GX_UBYTE           brush_alpha;
USHORT             count;
USHORT             pixel;
GX_COLOR           color;
GX_RECTANGLE      *clip = context->clip;

    get = (const GX_UBYTE *)pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* compressed with no alpha is a one-byte count and one-byte index value */
    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (USHORT)((count & 0x7f) + 1u);
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
    putrow = (USHORT *)context->memory;
    GX_CALCULATE_PUTROW(putrow, xpos, yval, context);

    palette = (GX_COLOR *)pixelmap->aux_data;

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
                count = (USHORT)((count & 0x7f) + 1u);

                if ((*get) != pixelmap->transparent_color)
                {
                    color = palette[*get];
                    pixel = GX_COLOR_565RGB_FROM_32RGB(color);

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
                                _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
                            }
                            xval++;
                        }
                    }
                }
                else
                {
                    put += count;
                    xval += count;
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
                                color = palette[*get];
                                *put = GX_COLOR_565RGB_FROM_32RGB(color);
                            }
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
                        if ((*get) != pixelmap->transparent_color)
                        {
                            if (xval >= clip->left &&
                                xval <= clip->right)
                            {
                                color = palette[*get];
                                pixel = GX_COLOR_565RGB_FROM_32RGB(color);
                                _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
                            }
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
/*    _gx_display_driver_565rgb_palette_pixelmap_compressed_write         */
/*                                                                        */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixlemap file without alpha channel for palette pixelmap.           */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_palette_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                                        INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                yval;
INT                xval;
const GX_UBYTE    *get;
USHORT            *put;
USHORT            *putrow;
GX_COLOR          *palette;
USHORT             count;
GX_COLOR           color;
USHORT             pixel;
GX_UBYTE           brush_alpha;
GX_RECTANGLE      *clip = context->clip;

    get = (const GX_UBYTE *)pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* compressed with no alpha is a one-byte count and one-byte index value */
    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (USHORT)((count & 0x7f) + 1u);
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
    putrow = (USHORT *)context->memory;
    GX_CALCULATE_PUTROW(putrow, xpos, yval, context);

    palette = (GX_COLOR *)pixelmap->aux_data;

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
                count = (USHORT)((count & 0x7f) + 1u);

                color = palette[*get++];
                pixel = GX_COLOR_565RGB_FROM_32RGB(color);

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
                            _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
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
                            color = palette[*get];
                            *put = GX_COLOR_565RGB_FROM_32RGB(color);
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
                            color = palette[*get];
                            pixel = GX_COLOR_565RGB_FROM_32RGB(color);
                            _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
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
/*    _gx_display_driver_16bpp_4444argb_pixelmap_raw_write                */
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
/*    _gx_display_driver_565rgb_pixel_blend  Display driver basic pixel   */
/*                                             blend function             */
/*    _gx_display_driver_16bpp_pixel_write   Display driver basic pixel   */
/*                                             write function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_16bpp_4444argb_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
                                                                 INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              skipcount;
INT              xval;
INT              yval;
USHORT          *getrow;
const USHORT    *get;
UCHAR            alpha_value;
USHORT           pixel;

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
            alpha_value = alpha_value | (alpha_value >> 4);
            if (alpha_value)
            {
                pixel = *get;
                pixel = GX_COLOR_565RGB_FROM_4444ARGB(pixel);
                if (alpha_value == 0xff)
                {
                    _gx_display_driver_16bpp_pixel_write(context, xval, yval, pixel);
                }
                else
                {
                    _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, alpha_value);
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
/*    _gx_display_driver_16bpp_4444argb_pixelmap_compressed_write         */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixelmap data of format 4444argb in 16bpp drivers.                  */
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
/*    _gx_display_driver_565rgb_pixel_blend  Display driver basic pixel   */
/*                                             blend function             */
/*    _gx_display_driver_16bpp_pixel_write   Display driver basic pixel   */
/*                                             write function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_16bpp_4444argb_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                                        INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              yval;
INT              xval;
const USHORT    *get;
USHORT           count;
USHORT           pixel;
GX_UBYTE         alpha_value;
GX_UBYTE         combined_alpha;
GX_UBYTE         brush_alpha;

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
                count = (USHORT)((count & 0x7fff) + 1u);
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
                count = (USHORT)((count & 0x7fff) + 1u);
                pixel = *get++;
                alpha_value = (GX_UBYTE)((pixel & 0xf000) >> 8);
                alpha_value = (alpha_value >> 4) | alpha_value;
                if (alpha_value)
                {
                    pixel = GX_COLOR_565RGB_FROM_4444ARGB(pixel);

                    if (brush_alpha == 0xff)
                    {
                        while (count--)
                        {
                            if (xval >= clip->left &&
                                xval <= clip->right)
                            {
                                if (alpha_value == 0xff)
                                {
                                    _gx_display_driver_16bpp_pixel_write(context, xval, yval, pixel);
                                }
                                else
                                {
                                    _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, alpha_value);
                                }
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
                                combined_alpha = (GX_UBYTE)(brush_alpha * alpha_value / 255);
                                _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, combined_alpha);
                            }
                            xval++;
                        }
                    }
                }
                else
                {
                    while (count--)
                    {
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
                            alpha_value = (GX_UBYTE)((pixel & 0xf000) >> 8);
                            alpha_value = (alpha_value >> 4) | alpha_value;
                            pixel = GX_COLOR_565RGB_FROM_4444ARGB(pixel);
                            if (alpha_value)
                            {
                                if (alpha_value == 0xff)
                                {
                                    _gx_display_driver_16bpp_pixel_write(context, xval, yval, pixel);
                                }
                                else
                                {
                                    _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, alpha_value);
                                }
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
                            alpha_value = (GX_UBYTE)((pixel & 0xf000) >> 8);
                            alpha_value = (alpha_value >> 4) | alpha_value;
                            pixel = GX_COLOR_565RGB_FROM_4444ARGB(pixel);
                            combined_alpha = (GX_UBYTE)(brush_alpha * alpha_value / 255);
                            _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, combined_alpha);
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
/*    _gx_display_driver_565rgb_pixelmap_draw                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    565rgb screen driver pixelmap drawing function that handles         */
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
/*     _gx_display_driver_565rgb_pixelmap_compressed_alpha_write          */
/*     _gx_display_driver_565rgb_pixelmap_alpha_write                     */
/*     _gx_display_driver_565rgb_pixelmap_compressed_write                */
/*     _gx_display_driver_565rgb_pixelmap_raw_write                       */
/*     _gx_display_driver_565rgb_palette_pixelmap_compressed_write        */
/*     _gx_display_driver_565rgb_palette_pixelmap_raw_write               */
/*     _gx_display_driver_16bpp_4444argb_pixelmap_raw_write               */
/*     _gx_display_driver_565rgb_palette_pixelmap_transparent_compressed  */
/*       _write                                                           */
/*     _gx_display_driver_565rgb_palette_pixelmap_transparent_raw_write   */
/*     _gx_display_driver_16bpp_4444argb_pixelmap_compressed_write        */
/*     _gx_display_driver_565rgb_pixelmap_blend                           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_565rgb_pixelmap_draw(GX_DRAW_CONTEXT *context,
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
        if (pixelmap->aux_data == GX_NULL)
        {
            break;
        }

        if (pixelmap->flags & GX_PIXELMAP_TRANSPARENT)
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* compressed with */
                _gx_display_driver_565rgb_palette_pixelmap_transparent_compressed_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* no compression */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_565rgb_palette_pixelmap_transparent_raw_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        else
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* compressed with */

                _gx_display_driver_565rgb_palette_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* no compression */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_565rgb_palette_pixelmap_raw_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        break;

    case GX_COLOR_FORMAT_565BGR:
    case GX_COLOR_FORMAT_565RGB:
        if (pixelmap->flags & GX_PIXELMAP_ALPHA)
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* has both compression and alpha */
                _gx_display_driver_565rgb_pixelmap_compressed_alpha_write(context,
                                                                          xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* alpha, no compression */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_565rgb_pixelmap_alpha_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        else
        {
            if (brush_alpha == 0xff)
            {
                if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
                {
                    /* compressed with no alpha */
                    _gx_display_driver_565rgb_pixelmap_compressed_write(context,
                                                                        xpos, ypos, pixelmap);
                }
                else
                {
                    /* no compression or alpha */
                    _gx_display_driver_565rgb_pixelmap_raw_write(context,
                                                                 xpos, ypos, pixelmap);
                }
                drawn = GX_TRUE;
            }
        }
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /*not write yet*/
            _gx_display_driver_16bpp_4444argb_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
            drawn = GX_TRUE;
        }
        else
        {
            if (brush_alpha == 0xff)
            {
                _gx_display_driver_16bpp_4444argb_pixelmap_raw_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
        }
        break;

    default:
        drawn = GX_TRUE;
        break;
    }

    if ((!drawn) && (brush_alpha != 0xff))
    {
        _gx_display_driver_565rgb_pixelmap_blend(context, xpos, ypos, pixelmap, brush_alpha);
    }

    return;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_1555xrgb_pixelmap_draw                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    1555xrgb screen driver pixelmap drawing function that handles       */
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
/*     _gx_display_driver_565rgb_pixelmap_compressed_alpha_write          */
/*     _gx_display_driver_565rgb_pixelmap_alpha_write                     */
/*     _gx_display_driver_565rgb_pixelmap_compressed_write                */
/*     _gx_display_driver_565rgb_pixelmap_raw_write                       */
/*     _gx_display_driver_15555xrgb_pixelmap_blend                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_1555xrgb_pixelmap_draw(GX_DRAW_CONTEXT *context,
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
    case GX_COLOR_FORMAT_1555XRGB:
        if (pixelmap->flags & GX_PIXELMAP_ALPHA)
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* has both compression and alpha */
                _gx_display_driver_565rgb_pixelmap_compressed_alpha_write(context,
                                                                          xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* alpha, no compression */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_565rgb_pixelmap_alpha_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        else
        {
            if (brush_alpha == 0xff)
            {
                if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
                {
                    /* compressed with no alpha */
                    _gx_display_driver_565rgb_pixelmap_compressed_write(context,
                                                                        xpos, ypos, pixelmap);
                }
                else
                {
                    /* no compression or alpha */
                    _gx_display_driver_565rgb_pixelmap_raw_write(context,
                                                                 xpos, ypos, pixelmap);
                }
                drawn = GX_TRUE;
            }
        }
        break;

    default:
        return;
    }

    if (!drawn)
    {
        _gx_display_driver_1555xrgb_pixelmap_blend(context, xpos, ypos, pixelmap, brush_alpha);
    }

    return;
}

