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
/*    _gx_display_driver_32argb_pixelmap_raw_blend                        */
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
/*    _gx_display_driver_32xrgb_pixel_blend Display driver pixel blend    */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32argb_pixelmap_blend                            */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32argb_pixelmap_raw_blend(GX_DRAW_CONTEXT *context,
                                                         INT xpos, INT ypos,
                                                         GX_PIXELMAP *pixelmap,
                                                         GX_UBYTE alpha)
{
INT           yval;
INT           xval;
GX_COLOR      color;
GX_COLOR     *get;
GX_COLOR     *getrow;

GX_RECTANGLE *clip = context->clip;

    getrow = (GX_COLOR *)((UINT)(pixelmap->data) + sizeof(GX_COLOR) * (UINT)(pixelmap->width) * (UINT)((INT)(clip->top) - ypos));
    getrow += (clip->left - xpos);

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        get = getrow;

        for (xval = clip->left; xval <= clip->right; xval++)
        {
            color = *get;
            get++;
            _gx_display_driver_32argb_pixel_blend(context, xval, yval, color, alpha);
        }

        getrow += pixelmap->width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32argb_pixelmap_alpha_blend                      */
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
/*    _gx_display_driver_32xrgb_pixel_blend Display driver pixel blend    */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32argb_pixelmap_blend                            */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32argb_pixelmap_alpha_blend(GX_DRAW_CONTEXT *context,
                                                           INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
int           xval;
int           yval;
int           color;
int           width;
ULONG        *get;

GX_RECTANGLE *clip = context->clip;

    get = (ULONG *)((pixelmap->data) + sizeof(GX_COLOR) * (UINT)pixelmap->width * (UINT)((INT)clip->top - ypos));
    get += (clip->left - xpos);
    width = clip->right - clip->left + 1;

    for (yval = clip->top; yval <= clip->bottom; yval++)
    {
        for (xval = clip->left; xval <= clip->right; xval++)
        {
            color = (int)(*get);
            _gx_display_driver_32argb_pixel_blend(context, xval, yval, (GX_COLOR)color, alpha);
            get++;
        }
        get += pixelmap->width - width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32argb_pixelmap_blend                            */
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
/*     _gx_display_driver_32argb_pixelmap_alpha_blend                     */
/*                                          Real pixelmap blend function  */
/*     _gx_display_driver_32argb_pixelmap_raw_blend                       */
/*                                          Real pixelmap blend function  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_32argb_pixelmap_blend(GX_DRAW_CONTEXT *context,
                                              INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
    if (pixelmap->flags & GX_PIXELMAP_TRANSPARENT)
    {
        /* Not supported yet. */
        return;
    }

    if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
    {
        return;
    }

    if (pixelmap->format != GX_COLOR_FORMAT_32ARGB)
    {
        /* wrong color format for this driver */
        return;
    }

    if (pixelmap->flags & GX_PIXELMAP_ALPHA)
    {
        /* alpha, no compression */
        _gx_display_driver_32argb_pixelmap_alpha_blend(context, xpos, ypos, pixelmap, alpha);

    }
    else
    {
        /* no compression or alpha */
        _gx_display_driver_32argb_pixelmap_raw_blend(context, xpos, ypos, pixelmap, alpha);
    }
}

