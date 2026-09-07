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
/*    _gx_display_driver_8bpp_pixelmap_raw_blend                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of uncompressed      */
/*    pixlemap file.                                                      */
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
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_8bpp_pixelmap_blend                              */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_pixelmap_raw_blend(GX_DRAW_CONTEXT *context,
                                                       INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT           xval;
INT           yval;
GX_UBYTE     *get;
GX_UBYTE     *getrow;
GX_UBYTE      pixel;

GX_RECTANGLE *clip = context->clip;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;
    if (!blend_func)
    {
        return;
    }

    getrow = (GX_UBYTE *)(pixelmap->data + (INT)sizeof(GX_UBYTE) * pixelmap->width * (clip->top - ypos));
    getrow += (clip->left - xpos);
    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        get = getrow;
        for (xval = clip->left; xval <= clip->right; xval++)
        {
            pixel = *get++;
            blend_func(context, xval, yval, pixel, alpha);
        }
        getrow += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_pixelmap_alpha_blend                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of uncompressed      */
/*    pixelmap file with alpha channel.                                   */
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
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_8bpp_pixelmap_blend                              */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_pixelmap_alpha_blend(GX_DRAW_CONTEXT *context,
                                                         INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT           skipcount;
INT           xval;
INT           yval;
GX_UBYTE     *getrow;
GX_UBYTE     *getrowalpha;
GX_UBYTE     *get;
GX_UBYTE      pixel;
GX_UBYTE     *getalpha;
INT           combined_alpha;
GX_UBYTE      internal_alpha;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
GX_RECTANGLE *clip = context->clip;

    blend_func = context->display->driver_pixel_blend;
    if (GX_NULL == blend_func)
    {
        return;
    }

    /* calculate how many pixels to skip */
    skipcount = (pixelmap->width) * (clip->top - ypos);
    skipcount += (clip->left - xpos);
    getrow = (GX_UBYTE *)(pixelmap->data);
    getrow += skipcount;

    getrowalpha = (GX_UBYTE *)(pixelmap->aux_data);
    getrowalpha += skipcount;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        get = getrow;
        getalpha = getrowalpha;

        for (xval = clip->left; xval <= clip->right; xval++)
        {
            internal_alpha = *getalpha++;
            if (internal_alpha)
            {
                combined_alpha = internal_alpha * alpha;
                combined_alpha /= 255;
                pixel = *get;
                blend_func(context, xval, yval, pixel, (GX_UBYTE)combined_alpha);
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
/*    _gx_display_driver_8bpp_pixelmap_blend                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Driver entry point for pixelmap blending function that handles      */
/*    compressed or uncompress, with or without alpha channel.            */
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
/*    _gx_display_driver_8bpp_pixelmap_alpha_blend                        */
/*                                          Real display driver pixelmap  */
/*                                            blend function for 8bpp     */
/*    _gx_display_driver_8bpp_pixelmap_raw_blend                          */
/*                                          Real display driver pixelmap  */
/*                                            blend function for 8bpp     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_8bpp_pixelmap_blend(GX_DRAW_CONTEXT *context,
                                            INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
    if ((pixelmap->flags & GX_PIXELMAP_TRANSPARENT) || (pixelmap->flags & GX_PIXELMAP_COMPRESSED))
    {
        /* Wrong format. */
        return;
    }

    if (pixelmap->format != GX_COLOR_FORMAT_8BIT_PACKED_PIXEL)
    {
        /* wrong color format for this driver */
        return;
    }

    if (pixelmap->flags & GX_PIXELMAP_ALPHA)
    {
        /* alpha, no compression */
        _gx_display_driver_8bpp_pixelmap_alpha_blend(context,
                                                     xpos, ypos, pixelmap, alpha);
    }
    else
    {
        /* no compression or alpha */
        _gx_display_driver_8bpp_pixelmap_raw_blend(context,
                                                   xpos, ypos, pixelmap, alpha);
    }
}

