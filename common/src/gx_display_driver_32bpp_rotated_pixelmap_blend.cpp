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
#include "gx_canvas.h"
#include "gx_pixelmap.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_raw_blend                 */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of rotated           */
/*    uncompressed pixlemap data without alpha channel.                   */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32argb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_blend                     */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_pixelmap_raw_blend(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT           yval;
INT           xval;
ULONG        *get;
ULONG        *getrow;
GX_RECTANGLE *clip = context->clip;
GX_RECTANGLE  rotated_clip;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    GX_SET_32BPP_BLEND_FUNCTION(blend_func, context->display->color_format);

    GX_SWAP_VALS(xpos, ypos);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
        ypos = (context->canvas->x_resolution - ypos - pixelmap->width);
    }
    else
    {
        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
        xpos = (context->canvas->y_resolution - xpos - pixelmap->height);
    }

    getrow = (GX_COLOR *)pixelmap->data;
    getrow +=  pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        get = getrow;
        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            blend_func(context, xval, yval, (*get++), alpha);
        }
        getrow += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_alpha_blend               */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of rotated           */
/*    uncompressed pixlemap data with alpha channel.                      */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32argb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_blend                     */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_pixelmap_alpha_blend(GX_DRAW_CONTEXT *context,
                                                                  INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
int           xval;
int           yval;
ULONG        *get;
ULONG        *getrow;
UCHAR         alpha_value;
ULONG         combined_alpha;
ULONG         color;
GX_RECTANGLE *clip = context->clip;
GX_RECTANGLE  rotated_clip;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    GX_SET_32BPP_BLEND_FUNCTION(blend_func, context->display->color_format);

    GX_SWAP_VALS(xpos, ypos);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
        ypos = (context->canvas->x_resolution - ypos - pixelmap->width);
    }
    else
    {
        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
        xpos = (context->canvas->y_resolution - xpos - pixelmap->height);
    }

    getrow = (GX_COLOR *)(pixelmap->data);
    getrow += (pixelmap->height * (rotated_clip.top - ypos));
    getrow += (rotated_clip.left - xpos);

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        get = getrow;
        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            color = (*get);
            alpha_value = ALPHAVAL_32BPP(color);

            if (alpha_value)
            {
                combined_alpha = alpha_value;
                combined_alpha *= alpha;
                combined_alpha /= 255;

                if (combined_alpha)
                {
                    color |= 0xff000000;
                    blend_func(context, xval, yval, color, (GX_UBYTE)combined_alpha);
                }
            }
            get++;
        }
        getrow += pixelmap->height;
    }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_palette_pixelmap_blend             */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated            */
/*    uncompressed palette pixlemap data without transparent.             */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32argb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_blend                     */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_palette_pixelmap_blend(GX_DRAW_CONTEXT *context,
                                                                    INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT           xval;
INT           yval;
GX_UBYTE     *getrow;
GX_UBYTE     *get;
GX_COLOR     *palette;
GX_RECTANGLE *clip = context->clip;
GX_RECTANGLE  rotated_clip;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    GX_SET_32BPP_BLEND_FUNCTION(blend_func, context->display->color_format);

    GX_SWAP_VALS(xpos, ypos);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
        ypos = (context->canvas->x_resolution - ypos - pixelmap->width);
    }
    else
    {
        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
        xpos = (context->canvas->y_resolution - xpos - pixelmap->height);
    }

    getrow = (GX_UBYTE *)(pixelmap->data);
    getrow += pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);


    palette = (GX_COLOR *)pixelmap->aux_data;

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        get = getrow;
        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            blend_func(context, xval, yval, palette[*get++], alpha);
        }

        getrow += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_palette_pixelmap_transparent_blend */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated            */
/*    uncompressed palette pixlemap data with transparent.                */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32argb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_palette_pixelmap_transparent_blend(GX_DRAW_CONTEXT *context,
                                                                                INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT           xval;
INT           yval;
GX_UBYTE     *getrow;
GX_UBYTE     *get;
GX_COLOR     *palette;
GX_RECTANGLE *clip = context->clip;
GX_RECTANGLE  rotated_clip;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    GX_SET_32BPP_BLEND_FUNCTION(blend_func, context->display->color_format);

    GX_SWAP_VALS(xpos, ypos);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
        ypos = (context->canvas->x_resolution - ypos - pixelmap->width);
    }
    else
    {
        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
        xpos = (context->canvas->y_resolution - xpos - pixelmap->height);
    }

    getrow = (GX_UBYTE *)(pixelmap->data);
    getrow += pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);
    palette = (GX_COLOR *)pixelmap->aux_data;

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        get = getrow;
        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            if ((*get) != pixelmap->transparent_color)
            {
                blend_func(context, xval, yval, palette[*get], alpha);
            }
            get++;
        }

        getrow += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_4444argb_pixelmap_alpha_blend      */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated            */
/*    uncompressed 4444argb format pixlemap data with alpha channel.      */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_blend                     */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_4444argb_pixelmap_alpha_blend(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT              xval;
INT              yval;
USHORT          *getrow;
const USHORT *get;
UCHAR            falpha;
GX_UBYTE         combined_alpha;
ULONG            pixel;

GX_RECTANGLE    *clip = context->clip;
GX_RECTANGLE     rotated_clip;
void             (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    GX_SET_32BPP_BLEND_FUNCTION(blend_func, context->display->color_format);

    GX_SWAP_VALS(xpos, ypos);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
        ypos = (context->canvas->x_resolution - ypos - pixelmap->width);
    }
    else
    {
        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
        xpos = (context->canvas->y_resolution - xpos - pixelmap->height);
    }

    /* Calculate how many pixels to skip.  */
    getrow = (USHORT *)(pixelmap->data);
    getrow += pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        get = getrow;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {

            /* Pick alpha value from 4444argb color.  */
            falpha = (UCHAR)(((*get) & 0xf000) >> 8);

            if (falpha)
            {
                /* Extend alpha value to improve accuracy.  */
                falpha = (GX_UBYTE)(falpha | (falpha >> 4));

                /* Convert 4444argb color to 24xrgb color.  */
                pixel = (GX_COLOR)((((*get) & 0x0f00) << 12) | (((*get) & 0x00f0) << 8) | (((*get) & 0x000f) << 4));
                pixel |= 0xff000000;

                /* Calulate combined alpha.  */
                combined_alpha = (GX_UBYTE)(falpha * alpha / 255);

                /* Blend color to background.  */
                blend_func(context, xval, yval, pixel, combined_alpha);
            }
            get++;
        }
        getrow += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_565rgb_pixelmap_alpha_blend        */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated            */
/*    uncompressed 565rgb format pixelmap data with alpha channel.        */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32argb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_blend                     */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_565rgb_pixelmap_alpha_blend(GX_DRAW_CONTEXT *context,
                                                                         INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT                skipcount;
INT                xval;
INT                yval;
const GX_UBYTE *getalpha;
const USHORT   *get;
USHORT            *getrow;
GX_UBYTE          *getrowalpha;
GX_COLOR           pixel;
GX_UBYTE           falpha;
GX_UBYTE           combined_alpha;
GX_RECTANGLE      *clip = context->clip;
GX_RECTANGLE       rotated_clip;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    GX_SET_32BPP_BLEND_FUNCTION(blend_func, context->display->color_format);

    GX_SWAP_VALS(xpos, ypos);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
        ypos = (context->canvas->x_resolution - ypos - pixelmap->width);
    }
    else
    {
        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
        xpos = (context->canvas->y_resolution - xpos - pixelmap->height);
    }

    skipcount = (pixelmap->height) * (rotated_clip.top - ypos);
    skipcount += (rotated_clip.left - xpos);
    getrow = (USHORT *)(pixelmap->data);
    getrow += skipcount;

    getrowalpha = (GX_UBYTE *)(pixelmap->aux_data);
    getrowalpha += skipcount;

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        get = getrow;
        getalpha = getrowalpha;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            falpha = *getalpha++;
            if (falpha)
            {
                combined_alpha = (GX_UBYTE)(falpha * alpha / 255);
                pixel = *get;

                /* Convert 565rgb color to 24xrgb color.  */
                pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff,
                                                       REDVAL_16BPP(pixel) << 3,
                                                       GREENVAL_16BPP(pixel) << 2,
                                                       BLUEVAL_16BPP(pixel) << 3);

                /* Blend 24xrgb color to background.  */
                blend_func(context, xval, yval, pixel, combined_alpha);
            }
            get++;
        }

        getrow += pixelmap->height;
        getrowalpha += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_565rgb_pixelmap_raw_blend          */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated            */
/*    uncompressed 565rgb format pixlemap data without alpha channel.     */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32argb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_blend                     */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_565rgb_pixelmap_raw_blend(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT              xval;
INT              yval;
USHORT          *getrow;
const USHORT *get;
GX_COLOR         pixel;
GX_RECTANGLE    *clip = context->clip;
GX_RECTANGLE     rotated_clip;
void             (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    GX_SET_32BPP_BLEND_FUNCTION(blend_func, context->display->color_format);

    GX_SWAP_VALS(xpos, ypos);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
        ypos = (context->canvas->x_resolution - ypos - pixelmap->width);
    }
    else
    {
        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
        xpos = (context->canvas->y_resolution - xpos - pixelmap->height);
    }

    getrow = (USHORT *)(pixelmap->data);
    getrow += pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        get = getrow;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            /* Convert 565rgb color to 24xrgb color.  */
            pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff,
                                                   REDVAL_16BPP(*get) << 3,
                                                   GREENVAL_16BPP(*get) << 2,
                                                   BLUEVAL_16BPP(*get) << 3);

            blend_func(context, xval, yval, pixel, alpha);
            get++;
        }

        getrow += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_blend                     */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    24xrgb format screen driver pixelmap blending function that         */
/*    handles uncompressed pixelmap blend, with or without alpha channel. */
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
/*    _gx_display_driver_32bpp_rotated_palette_pixelmap_transparent_blend */
/*    _gx_display_driver_32bpp_rotated_palette_pixelmap_blend             */
/*    _gx_display_driver_32bpp_rotated_4444argb_pixelmap_alpha_blend      */
/*    _gx_display_driver_32bpp_rotated_565rgb_pixelmap_alpha_blend        */
/*    _gx_display_driver_32bpp_rotated_565rgb_pixelmap_raw_blend          */
/*    _gx_display_driver_32bpp_rotated_pixelmap_alpha_blend               */
/*    _gx_display_driver_32bpp_rotated_pixelmap_raw_blend                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_32bpp_rotated_pixelmap_blend(GX_DRAW_CONTEXT *context,
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
            _gx_display_driver_32bpp_rotated_palette_pixelmap_transparent_blend(context, xpos, ypos, pixelmap, alpha);
        }
        else
        {
            _gx_display_driver_32bpp_rotated_palette_pixelmap_blend(context, xpos, ypos, pixelmap, alpha);
        }
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        _gx_display_driver_32bpp_rotated_4444argb_pixelmap_alpha_blend(context, xpos, ypos, pixelmap, alpha);
        break;

    case GX_COLOR_FORMAT_565RGB:
        if (pixelmap->flags & GX_PIXELMAP_ALPHA)
        {
            _gx_display_driver_32bpp_rotated_565rgb_pixelmap_alpha_blend(context, xpos, ypos, pixelmap, alpha);
        }
        else
        {
            _gx_display_driver_32bpp_rotated_565rgb_pixelmap_raw_blend(context, xpos, ypos, pixelmap, alpha);
        }
        break;

    case GX_COLOR_FORMAT_24XRGB:
    case GX_COLOR_FORMAT_32ARGB:
        if (pixelmap->flags & GX_PIXELMAP_ALPHA)
        {
            _gx_display_driver_32bpp_rotated_pixelmap_alpha_blend(context, xpos, ypos, pixelmap, alpha);
        }
        else
        {
            _gx_display_driver_32bpp_rotated_pixelmap_raw_blend(context, xpos, ypos, pixelmap, alpha);
        }
        break;
    }

    return;
}

