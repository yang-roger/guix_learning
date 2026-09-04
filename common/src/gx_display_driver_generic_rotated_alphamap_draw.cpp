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

#if defined GX_BRUSH_ALPHA_SUPPORT

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_rotated_alphamap_raw_alpha_draw          */
/*                                                                        */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing  with brush alpha     */
/*    of uncompressed alpha map file.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 alpha value from 0 to 255     */
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
/*    _gx_display_driver_generic_alphamap_draw                            */
/*                                                                        */
/**************************************************************************/
static void  _gx_display_driver_generic_rotated_alphamap_raw_alpha_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pmp, GX_UBYTE alpha)
{
INT                xval;
INT                yval;
GX_UBYTE          *getrowalpha;
const GX_UBYTE    *getalpha;
GX_UBYTE           combined_alpha;
GX_COLOR           fill_color;
GX_RECTANGLE      *clip;
GX_RECTANGLE       rotated_clip;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    /* Pick up clip rectangle.  */
    clip = context->clip;

    GX_SET_BLEND_FUNCTION(blend_func, context->display->color_format)

    /* Pick up context fill color.  */
    fill_color = context->brush.fill_color;

    GX_SWAP_VALS(xpos, ypos);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        ypos = context->canvas->x_resolution - ypos - pmp->width;
        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
    }
    else
    {
        xpos = context->canvas->y_resolution - xpos - pmp->height;
        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
    }

    getrowalpha = (UCHAR *)(pmp->data);
    getrowalpha += (pmp->height) * (rotated_clip.top - ypos);
    getrowalpha += (rotated_clip.left - xpos);

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        getalpha = getrowalpha;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            combined_alpha = (GX_UBYTE)((*getalpha++) * alpha / 255);
            blend_func(context, xval, yval, fill_color, combined_alpha);
        }

        getrowalpha += pmp->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_rotated_alphamap_compressed_alpha_draw   */
/*                                                                        */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing with brush alpha      */
/*    of compressed alpha map file.                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 alpha value from 0 to 255     */
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
/*    _gx_display_driver_generic_alphamap_draw                            */
/*                                                                        */
/**************************************************************************/
static void  _gx_display_driver_generic_rotated_alphamap_compressed_alpha_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pmp, GX_UBYTE alpha)
{
INT                yval;
INT                xval;
const GX_UBYTE    *get;
GX_UBYTE           count;
GX_UBYTE           falpha;
GX_COLOR           fill_color;
GX_RECTANGLE      *clip;
GX_RECTANGLE       rotated_clip;
GX_UBYTE           combined_alpha;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    /* Pick up clip rectangle. */
    clip = context->clip;

    GX_SET_BLEND_FUNCTION(blend_func, context->display->color_format)

    /* Pick up context fill color.  */
    fill_color = context->brush.fill_color;

    get = (const GX_UBYTE *)pmp->data;

    GX_SWAP_VALS(xpos, ypos);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        ypos = context->canvas->x_resolution - ypos - pmp->width;
        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
    }
    else
    {
        xpos = context->canvas->y_resolution - xpos - pmp->height;
        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
    }

    /* compressed with no alpha is a two-byte count and two-byte pixel value */

    /* first, skip to the starting row */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pmp->height)
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

    while (yval <= rotated_clip.bottom)
    {
        xval = xpos;

        while (xval < xpos + pmp->height)
        {
            count = *get++;

            if (count & 0x80)
            {
                /* repeated value */
                count = (GX_UBYTE)((count & 0x7f) + 1);
                falpha = *get++;

                while (count--)
                {
                    if (xval >= rotated_clip.left &&
                        xval <= rotated_clip.right)
                    {
                        combined_alpha = (GX_UBYTE)(falpha * alpha / 255);
                        blend_func(context, xval, yval, fill_color, combined_alpha);
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
                    if (xval >= rotated_clip.left &&
                        xval <= rotated_clip.right)
                    {
                        combined_alpha = (GX_UBYTE)((*get) * alpha / 255);
                        blend_func(context, xval, yval, fill_color, combined_alpha);
                    }
                    get++;
                    xval++;
                }
            }
        }
        yval++;
    }
}
#endif /* GX_BRUSH_ALPHA_SUPPORT*/

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_rotated_alphamap_raw_draw                */
/*                                                                        */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of uncompressed       */
/*    alpha map file.                                                     */
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
/*    _gx_display_driver_generic_alphamap_draw                            */
/*                                                                        */
/**************************************************************************/
static void  _gx_display_driver_generic_rotated_alphamap_raw_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pmp)
{
INT                xval;
INT                yval;
GX_UBYTE          *getrowalpha;
const GX_UBYTE    *getalpha;
GX_COLOR           fill_color;
GX_RECTANGLE      *clip;
GX_RECTANGLE       rotated_clip;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    /* Pick up clip rectangle.  */
    clip = context->clip;

    GX_SET_BLEND_FUNCTION(blend_func, context->display->color_format)

    /* Pick up context fill color.  */
    fill_color = context->brush.fill_color;

    GX_SWAP_VALS(xpos, ypos);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        ypos = context->canvas->x_resolution - ypos - pmp->width;
        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
    }
    else
    {
        xpos = context->canvas->y_resolution - xpos - pmp->height;
        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
    }

    getrowalpha = (UCHAR *)(pmp->data);
    getrowalpha += (pmp->height) * (rotated_clip.top - ypos);
    getrowalpha += (rotated_clip.left - xpos);

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        getalpha = getrowalpha;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            blend_func(context, xval, yval, fill_color, *getalpha++);
        }

        getrowalpha += pmp->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_rotated_alphamap_compressed_draw         */
/*                                                                        */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    alpha map file.                                                     */
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
/*    _gx_display_driver_generic_alphamap_draw                            */
/*                                                                        */
/**************************************************************************/
static void  _gx_display_driver_generic_rotated_alphamap_compressed_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pmp)
{
INT                yval;
INT                xval;
const GX_UBYTE    *get;
GX_UBYTE           count;
GX_UBYTE           pixel;
GX_COLOR           fill_color;
GX_RECTANGLE      *clip;
GX_RECTANGLE       rotated_clip;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    /* Pick up clip rectangle.  */
    clip = context->clip;

    GX_SET_BLEND_FUNCTION(blend_func, context->display->color_format)

    /* Pick up context fill color.  */
    fill_color = context->brush.fill_color;

    get = (const GX_UBYTE *)pmp->data;

    GX_SWAP_VALS(xpos, ypos);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        ypos = context->canvas->x_resolution - ypos - pmp->width;
        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
    }
    else
    {
        xpos = context->canvas->y_resolution - xpos - pmp->height;
        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
    }

    /* compressed with no alpha is a two-byte count and two-byte pixel value */

    /* first, skip to the starting row */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pmp->height)
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

    while (yval <= rotated_clip.bottom)
    {
        xval = xpos;

        while (xval < xpos + pmp->height)
        {
            count = *get++;

            if (count & 0x80)
            {
                /* repeated value */
                count = (GX_UBYTE)((count & 0x7f) + 1);
                pixel = *get++;

                while (count--)
                {
                    if (xval >= rotated_clip.left &&
                        xval <= rotated_clip.right)
                    {
                        blend_func(context, xval, yval, fill_color, pixel);
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
                    if (xval >= rotated_clip.left &&
                        xval <= rotated_clip.right)
                    {
                        blend_func(context, xval, yval, fill_color, *get);
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
/*    _gx_display_driver_generic_rotated_alphamap_draw                    */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function blends the context fill color with the canvas         */
/*      background.                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pmp                                   Pointer to GX_PIXELMAP struct */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_generic_rotated_alphamap_compressed_alpha_draw   */
/*                                          Real display driver alphamap  */
/*                                            draw function               */
/*    _gx_display_driver_generic_rotated_alphamap_raw_alpha_draw          */
/*                                          Real display driver alphamap  */
/*                                            draw function               */
/*    _gx_display_driver_generic_rotated_alphamap_compressed_draw         */
/*                                          Real display driver alphamap  */
/*                                            draw function               */
/*    _gx_display_driver_generic_rotated_alphamap_raw_draw                */
/*                                          Real display driver alphamap  */
/*                                            draw function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default draw funtions                                          */
/*                                                                        */
/**************************************************************************/
void  _gx_display_driver_generic_rotated_alphamap_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pmp)
{
#if defined GX_BRUSH_ALPHA_SUPPORT
GX_UBYTE alpha;

    alpha = context->brush.alpha;
    if (alpha == 0)
    {
        /* Nothing to drawn. Just return. */
        return;
    }

    if (alpha != 0xff)
    {
        if (pmp->flags & GX_PIXELMAP_COMPRESSED)
        {
            _gx_display_driver_generic_rotated_alphamap_compressed_alpha_draw(context, xpos, ypos, pmp, alpha);
        }
        else
        {
            _gx_display_driver_generic_rotated_alphamap_raw_alpha_draw(context, xpos, ypos, pmp, alpha);
        }
        return;
    }
#endif

    if (pmp->flags & GX_PIXELMAP_COMPRESSED)
    {
        _gx_display_driver_generic_rotated_alphamap_compressed_draw(context, xpos, ypos, pmp);
    }
    else
    {
        _gx_display_driver_generic_rotated_alphamap_raw_draw(context, xpos, ypos, pmp);
    }
}

