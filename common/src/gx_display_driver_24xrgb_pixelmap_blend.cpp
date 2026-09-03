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
/*    _gx_display_driver_24xrgb_pixelmap_raw_blend                        */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of uncompressed      */
/*    pixlemap file without alpha channel.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixel_blend Display driver basic function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_blend                            */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_pixelmap_raw_blend(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT           yval;
INT           xval;
INT           draw_width;
GX_COLOR      color;
ULONG        *get;

GX_RECTANGLE *clip = context->clip;

    get = (GX_COLOR *)pixelmap->data;
    get +=  pixelmap->width * (clip->top - ypos);
    get += (clip->left - xpos);

    draw_width = clip->right - clip->left + 1;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        for (xval = clip->left; xval <= clip->right; xval++)
        {
            color = *get & 0x00ffffff;
            get++;
            _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, color, alpha);
        }
        get += pixelmap->width - draw_width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_alpha_blend                      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of uncompressed      */
/*    pixlemap file with alpha channel.                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixel_blend Display driver basic function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_blend                            */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_pixelmap_alpha_blend(GX_DRAW_CONTEXT *context,
                                                           INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
int           xval;
int           yval;
int           color;
int           width;
ULONG        *get;
UCHAR         alpha_value;
ULONG         combined_alpha;

GX_RECTANGLE *clip = context->clip;

    get = (GX_COLOR *)(pixelmap->data + (INT)sizeof(GX_COLOR) * pixelmap->width * (clip->top - ypos));
    get += (clip->left - xpos);

    width = clip->right - clip->left + 1;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        for (xval = clip->left; xval <= clip->right; xval++)
        {
            alpha_value = (UCHAR)(*get >> 24);

            if (alpha_value)
            {
                combined_alpha = alpha_value;
                combined_alpha *= alpha;
                combined_alpha /= 255;

                if (combined_alpha)
                {
                    color = *get & 0x00ffffff;
                    _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, (GX_COLOR)color, (GX_UBYTE)combined_alpha);
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
/*    _gx_display_driver_24xrgb_palette_pixelmap_blend                    */
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
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixel_blend Display driver basic function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_blend                            */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_palette_pixelmap_blend(GX_DRAW_CONTEXT *context,
                                                             INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT           xval;
INT           yval;
GX_COLOR      color;
INT           width;
GX_UBYTE     *get;
GX_COLOR     *palette;

GX_RECTANGLE *clip = context->clip;

    get = (GX_UBYTE *)(pixelmap->data + pixelmap->width * (clip->top - ypos));
    get += (clip->left - xpos);

    palette = (GX_COLOR *)pixelmap->aux_data;

    width = clip->right - clip->left + 1;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        for (xval = clip->left; xval <= clip->right; xval++)
        {
            color = palette[*get++] & 0x00ffffff;
            _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, color, alpha);
        }

        get += pixelmap->width - width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_palette_pixelmap_transparent_blend        */
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
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixel_blend Display driver basic function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_blend                            */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_palette_pixelmap_transparent_blend(GX_DRAW_CONTEXT *context,
                                                                         INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
 {
INT           xval;
INT           yval;
GX_COLOR      color;
INT           width;
GX_UBYTE     *get;
GX_COLOR     *palette;

GX_RECTANGLE *clip = context->clip;

     get = (GX_UBYTE *)(pixelmap->data + pixelmap->width * (clip->top - ypos));
     get += (clip->left - xpos);

     palette = (GX_COLOR *)pixelmap->aux_data;

     width = clip->right - clip->left + 1;

     for (yval = clip->top; yval <= clip->bottom; yval++)
     {
         for (xval = clip->left; xval <= clip->right; xval++)
         {
             if ((*get) != pixelmap->transparent_color)
             {
                 color = palette[*get] & 0x00ffffff;
                 _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, color, alpha);
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
/*    _gx_display_driver_24xrgb_4444argb_pixelmap_alpha_blend             */
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
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixel_blend Display driver basic function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_blend                            */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_4444argb_pixelmap_alpha_blend(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT           skipcount;
INT           xval;
INT           yval;
USHORT       *getrow;
const USHORT *get;
UCHAR         falpha;
GX_UBYTE      combined_alpha;
ULONG         pixel;

GX_RECTANGLE *clip = context->clip;

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
            falpha = (UCHAR)(((*get) & 0xf000) >> 8);
            if (falpha)
            {
                falpha = (GX_UBYTE)(falpha | (falpha >> 4));
                combined_alpha = (GX_UBYTE)(falpha * alpha / 255);
                if (combined_alpha)
                {
                    pixel = *get;
                    pixel = GX_COLOR_24RGB_FROM_4444ARGB(pixel);
                    _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, combined_alpha);
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
/*    _gx_display_driver_24xrgb_565rgb_pixelmap_alpha_blend               */
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
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixel_blend Display driver basic function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_blend                            */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_565rgb_pixelmap_alpha_blend(GX_DRAW_CONTEXT *context,
                                                                  INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT             skipcount;
INT             xval;
INT             yval;
const GX_UBYTE *getalpha;
const USHORT   *get;
USHORT         *getrow;
GX_UBYTE       *getrowalpha;
GX_COLOR        pixel;
GX_UBYTE        falpha;
GX_UBYTE        combined_alpha;

GX_RECTANGLE   *clip = context->clip;

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
            falpha = *getalpha++;
            if (falpha)
            {
                combined_alpha = (GX_UBYTE)(falpha * alpha / 255);
                if (combined_alpha)
                {
                    pixel = *get;
                    pixel = GX_COLOR_24RGB_FROM_565RGB(pixel);
                    _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, combined_alpha);
                }
            }
            get++;
        }
        getrow += pixelmap->width;
        getrowalpha += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_565rgb_pixelmap_raw_blend                 */
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
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixel_blend Display driver basic function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_blend                            */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_565rgb_pixelmap_raw_blend(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT           xval;
INT           yval;
USHORT       *getrow;
const USHORT *get;
GX_COLOR      pixel;

GX_RECTANGLE *clip = context->clip;

    getrow = (USHORT *)(pixelmap->data);
    getrow += pixelmap->width * (clip->top - ypos);
    getrow += (clip->left - xpos);

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        get = getrow;

        for (xval = clip->left; xval <= clip->right; xval++)
        {
            pixel = *get;
            pixel = GX_COLOR_24RGB_FROM_565RGB(pixel);
            _gx_display_driver_24xrgb_pixel_blend(context, xval, yval, pixel, alpha);
            get++;
        }
        getrow += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_blend                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    32xrgb format screen driver pixelmap blending function that         */
/*    handles compressed or uncompress, with or without alpha channel.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_24xrgb_palette_pixelmap_transparent_blend        */
/*                                          Real pixelmap draw function.  */
/*    _gx_display_driver_24xrgb_palette_pixelmap_blend                    */
/*                                          Real pixelmap draw function.  */
/*    _gx_display_driver_24xrgb_4444argb_pixelmap_alpha_blend             */
/*                                          Real pixelmap draw function.  */
/*    _gx_display_driver_24xrgb_565rgb_pixelmap_alpha_blend               */
/*                                          Real pixelmap draw function.  */
/*    _gx_display_driver_24xrgb_565rgb_pixelmap_raw_blend                 */
/*                                          Real pixelmap draw function.  */
/*    _gx_display_driver_24xrgb_pixelmap_alpha_blend                      */
/*                                          Real pixelmap draw function.  */
/*    _gx_display_driver_24xrgb_pixelmap_raw_blend                        */
/*                                          Real pixelmap draw function.  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_24xrgb_pixelmap_blend(GX_DRAW_CONTEXT *context,
                                              INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
    if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
    {
        return;
    }

    switch (pixelmap->format)
    {
    case GX_COLOR_FORMAT_8BIT_PALETTE:
        if (pixelmap->flags & GX_PIXELMAP_TRANSPARENT)
        {
            _gx_display_driver_24xrgb_palette_pixelmap_transparent_blend(context, xpos, ypos, pixelmap, alpha);
        }
        else
        {
            _gx_display_driver_24xrgb_palette_pixelmap_blend(context, xpos, ypos, pixelmap, alpha);
        }
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        _gx_display_driver_24xrgb_4444argb_pixelmap_alpha_blend(context, xpos, ypos, pixelmap, alpha);
        break;

    case GX_COLOR_FORMAT_565RGB:
        if (pixelmap->flags & GX_PIXELMAP_ALPHA)
        {
            _gx_display_driver_24xrgb_565rgb_pixelmap_alpha_blend(context, xpos, ypos, pixelmap, alpha);
        }
        else
        {
            _gx_display_driver_24xrgb_565rgb_pixelmap_raw_blend(context, xpos, ypos, pixelmap, alpha);
        }
        break;

    case GX_COLOR_FORMAT_24XRGB:
    case GX_COLOR_FORMAT_32ARGB:
        if (pixelmap->flags & GX_PIXELMAP_ALPHA)
        {
            _gx_display_driver_24xrgb_pixelmap_alpha_blend(context, xpos, ypos, pixelmap, alpha);

        }
        else
        {
            _gx_display_driver_24xrgb_pixelmap_raw_blend(context, xpos, ypos, pixelmap, alpha);
        }
        break;
    }

    return;
}
