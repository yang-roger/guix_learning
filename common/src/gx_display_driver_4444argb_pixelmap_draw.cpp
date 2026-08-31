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
/*    _gx_display_driver_4444argb_pixelmap_alpha_write                    */
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
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_4444argb_pixelmap_draw                           */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_4444argb_pixelmap_alpha_write(GX_DRAW_CONTEXT *context,
                                                           INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              skipcount;
INT              xval;
INT              yval;
USHORT          *getrow;
const USHORT *get;
USHORT           pixel;
GX_UBYTE         alpha;
GX_UBYTE         brush_alpha;
void           (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);
GX_RECTANGLE    *clip = context->clip;

    brush_alpha = context->brush.alpha;
    blend_func = context->display->driver_pixel_blend;
    if (!blend_func)
    {
        return;
    }

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
            pixel = *get++;
            alpha = (GX_UBYTE)(pixel >> 12);
            if (alpha)
            {
                blend_func(context, xval, yval, pixel, brush_alpha);
            }
        }
        getrow += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_pixelmap_raw_write                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of uncompressed       */
/*    pixlemap file with 0xf alpha channel value.                         */
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
/*    _gx_display_driver_4444argb_pixelmap_draw                           */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_4444argb_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
                                                           INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           skipcount;
INT           xval;
INT           yval;
USHORT       *getrow;
const USHORT *get;
USHORT        pixel;
USHORT       *put;
USHORT       *putrow;
GX_RECTANGLE *clip = context->clip;

    /* calculate how many pixels to skip */
    skipcount = (pixelmap->width) * (clip->top - ypos);
    skipcount += (clip->left - xpos);
    getrow = (USHORT *)(pixelmap->data);
    getrow += skipcount;

    putrow = (USHORT *)context->memory;
    putrow += (clip->top) * context->pitch;
    putrow += clip->left;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        get = getrow;
        put = putrow;

        for (xval = clip->left; xval <= clip->right; xval++)
        {
            /*Or with 0xf000 to make sure it has no alpha.*/
            pixel = (*get++) | 0xf000;
            *put++ = pixel;
        }
        getrow += pixelmap->width;
        putrow += context->pitch;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_pixelmap_c_a_write                      */
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
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_4444argb_pixelmap_draw                           */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_4444argb_pixelmap_compressed_alpha_write(GX_DRAW_CONTEXT *context,
                                                                        INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           yval;
INT           xval;
const USHORT *get;
USHORT        count;
USHORT        pixel;
GX_UBYTE      alpha;
GX_UBYTE      brush_alpha;
void        (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
GX_RECTANGLE *clip = context->clip;

    get = (const USHORT *)pixelmap->data;
    brush_alpha = context->brush.alpha;
    blend_func = context->display->driver_pixel_blend;

    if (blend_func == GX_NULL)
    {
        return;
    }

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
                alpha = (GX_UBYTE)(pixel >> 12);

                if (alpha)
                {
                    while (count--)
                    {
                        if (xval >= clip->left &&
                            xval <= clip->right)
                        {
                            blend_func(context, xval, yval, pixel, brush_alpha);
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
                        blend_func(context, xval, yval, *get, brush_alpha);
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
/*    _gx_display_driver_4444argb_pixelmap_compressed_write               */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_4444argb_pixelmap_draw                           */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_4444argb_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                                  INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT               yval;
INT               xval;
const USHORT  *get;
USHORT            count;
USHORT            pixel;
USHORT           *put;
USHORT           *putrow;
GX_RECTANGLE     *clip = context->clip;

    get = (const USHORT *)pixelmap->data;

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
    putrow = (USHORT *)context->memory;
    putrow += clip->top * context->pitch;
    putrow += clip->left;

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
                /*Or with 0xf000 to make sure it has no alpha.*/
                pixel = (*get++) | 0xf000 ;
                while (count--)
                {
                    if (xval >= clip->left &&
                        xval <= clip->right)
                    {
                        *put++ = pixel;
                    }
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
                        /*Or with 0xf000 to make sure it has no alpha.*/
                        pixel = (*get) | 0xf000;
                        *put++ = pixel;
                    }
                    get++;
                    xval++;
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
/*    _gx_display_driver_4444argb_pixelmap_draw                           */
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
/*    _gx_display_driver_4444argb_pixelmap_compressed_alpha_write         */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_4444argb_pixelmap_compressed_write               */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_4444argb_pixelmap_alpha_write                    */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_4444argb_pixelmap_raw_write                      */
/*                                          Real display driver pixelmap  */
/*                                            draw function               */
/*    _gx_display_driver_4444argb_pixelmap_blend                          */
/*                                          Basic display driver pixelmap */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_4444argb_pixelmap_draw(GX_DRAW_CONTEXT *context,
                                               INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
GX_BOOL   drawn = GX_FALSE;
GX_UBYTE  brush_alpha = context->brush.alpha;

    if (brush_alpha == 0)
    {
        /* Draw nothing here. Just return. */
        return;
    }

    switch (pixelmap->format)
    {
    case GX_COLOR_FORMAT_4444ARGB:
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            if (pixelmap->flags & GX_PIXELMAP_ALPHA)
            {
                _gx_display_driver_4444argb_pixelmap_compressed_alpha_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_4444argb_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        else
        {
            if (pixelmap->flags & GX_PIXELMAP_ALPHA)
            {
                _gx_display_driver_4444argb_pixelmap_alpha_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_4444argb_pixelmap_raw_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        break;

    default:
        drawn = GX_TRUE;
        break;
    }

    if (!drawn)
    {
        _gx_display_driver_4444argb_pixelmap_blend(context, xpos, ypos, pixelmap, brush_alpha);
    }

    return;
}

