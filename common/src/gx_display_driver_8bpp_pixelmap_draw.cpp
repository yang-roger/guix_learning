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
/*    _gx_display_driver_8bpp_pixelmap_raw_write                          */
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
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
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
/*    _gx_display_driver_8bpp_pixelmap_compressed_write                   */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixlemap file.                                                      */
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
static void _gx_display_driver_8bpp_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                              INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT             yval;
INT             xval;
const GX_UBYTE *get;
GX_UBYTE       *put;
GX_UBYTE       *putrow;
GX_UBYTE        count;
INT             length;
INT             repeat;
INT             width;
GX_UBYTE        pixel = 0;
GX_RECTANGLE   *clip = context->clip;

    get = (const GX_UBYTE *)pixelmap->data;

    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1);
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
        width = pixelmap->width;

        while (xval < (xpos + pixelmap->width))
        {
            count = *get++;
            if (count & 0x80)
            {
                /* repeated value */
                count = (GX_UBYTE)((count & 0x7f) + 1);
                pixel = *get++;
                repeat = GX_TRUE;
            }
            else
            {
                /* string of non-repeated values */
                count++;
                repeat = GX_FALSE;
            }

            if (repeat == GX_TRUE)
            {

                if (count < width)
                {
                    length = count;
                }
                else
                {
                    length = width;
                }
                width -= length;
                while (length--)
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
                if (count < pixelmap->width)
                {
                    length = count;
                }
                else
                {
                    length = pixelmap->width;
                }
                width -= length;
                while (length--)
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
/*    _gx_display_driver_8bpp_pixelmap_transparent_write                  */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_pixelmap_transparent_write(GX_DRAW_CONTEXT *context,
                                                               INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT             xval;
INT             yval;
INT             width;
GX_UBYTE       *putrow;
GX_UBYTE       *getrow;
GX_UBYTE       *put;
GX_UBYTE        inval;
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
            inval = *get++;
            if (inval == pixelmap->transparent_color)
            {
                put++;
            }
            else
            {
                *put++ = inval;
            }
        }
        putrow += context->pitch;
        getrow += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_pixelmap_compressed_transparent_write       */
/*                                                                        */
/*                                                           6.1.7        */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_pixelmap_compressed_transparent_write(GX_DRAW_CONTEXT *context,
                                                                          INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT             yval;
INT             xval;
const GX_UBYTE *get;
GX_UBYTE       *put;
GX_UBYTE       *putrow;
GX_UBYTE        count;
INT             length;
INT             repeat;
INT             width;
GX_UBYTE        pixel = 0;
GX_RECTANGLE   *clip = context->clip;

    get = (const GX_UBYTE *)pixelmap->data;

    /* first, skip to the starting row */
    for (yval = ypos; yval < clip->top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->width)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1);
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
        width = pixelmap->width;

        while (xval < (xpos + pixelmap->width))
        {
            count = *get++;
            if (count & 0x80)
            {
                /* repeated value */
                count = (GX_UBYTE)((count & 0x7f) + 1);
                pixel = *get++;
                repeat = GX_TRUE;
            }
            else
            {
                /* string of non-repeated values */
                count++;
                repeat = GX_FALSE;
            }

            if (repeat == GX_TRUE)
            {
                if (count < width)
                {
                    length = count;
                }
                else
                {
                    length = width;
                }
                width -= length;

                if (pixel == pixelmap->transparent_color)
                {
                    put += length;
                    xval += length;
                }
                else
                {
                    while (length--)
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
            }
            else
            {
                if (count < pixelmap->width)
                {
                    length = count;
                }
                else
                {
                    length = pixelmap->width;
                }

                width -= length;

                while (length--)
                {
                    pixel = *get++;

                    if (xval >= clip->left &&
                        xval <= clip->right &&
                        pixel != pixelmap->transparent_color)
                    {
                        *put = pixel;
                    }
                    put++;
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
/*    _gx_display_driver_8bpp_pixelmap_draw                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    8bit screen driver pixelmap drawing function that handles           */
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
/*     _gx_display_driver_8bit_pixelmap_compressed_write                  */
/*     _gx_display_driver_8bit_pixelmap_compressed_transparent_write      */
/*     _gx_display_driver_8bit_pixelmap_transparent_write                 */
/*     _gx_display_driver_8bit_pixelmap_raw_write                         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_8bpp_pixelmap_draw(GX_DRAW_CONTEXT *context,
                                           INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{

    if (pixelmap->format != GX_COLOR_FORMAT_8BIT_PALETTE ||
        (pixelmap->flags & GX_PIXELMAP_ALPHA))
    {
        /* wrong color format for this driver */
        return;
    }

    if (pixelmap->flags & GX_PIXELMAP_TRANSPARENT)
    {
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /* has both compression and transparent */
            _gx_display_driver_8bpp_pixelmap_compressed_transparent_write(context, xpos, ypos, pixelmap);
        }
        else
        {
            /* transparent, no compression */
            _gx_display_driver_8bpp_pixelmap_transparent_write(context, xpos, ypos, pixelmap);
        }
    }
    else
    {
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /* compressed with no transparency */
            _gx_display_driver_8bpp_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
        }
        else
        {
            /* no compression or transaprency */
            _gx_display_driver_8bpp_pixelmap_raw_write(context, xpos, ypos, pixelmap);
        }
    }
}

