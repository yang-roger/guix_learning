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
/*    _gx_display_driver_32bpp_rotated_pixelmap_raw_write                 */
/*                                                                        */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated            */
/*    uncompressed pixlemap data without alpha channel.                   */
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
static void _gx_display_driver_32bpp_rotated_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
                                                                INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           xval;
INT           yval;
GX_COLOR     *putrow;
GX_COLOR     *getrow;
GX_COLOR     *put;
GX_COLOR     *get;

GX_RECTANGLE *clip = context->clip;
GX_RECTANGLE  rotated_clip;

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
    getrow += pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);

    /* Brush alpha is 0xff means draw pixelmap to memory directly.  */
    putrow = context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += rotated_clip.left;

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        put = putrow;
        get = getrow;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            *put++ = *get++;
        }
        putrow += context->pitch;
        getrow += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_alpha_write               */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated            */
/*    uncompressed pixlemap data with alpha channel.                      */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32bpp_pixel_write                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_pixelmap_alpha_write(GX_DRAW_CONTEXT *context,
                                                                  INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           xval;
INT           yval;
GX_COLOR      color;
ULONG        *getrow;
ULONG        *get;
UCHAR         alpha_value;
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

    getrow = (ULONG *)(pixelmap->data);
    getrow += pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        get = getrow;
        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            alpha_value = ALPHAVAL_32BPP(*get);
            if (alpha_value)
            {
                color = *get;
                if (alpha_value == 255)
                {
                    _gx_display_driver_32bpp_pixel_write(context, xval, yval, color);
                }
                else
                {
                    color |= 0xff000000;
                    blend_func(context, xval, yval, color, alpha_value);
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
/*    _gx_display_driver_32bpp_rotated_pixelmap_compressed_write          */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated compressed */
/*    pixlemap data without alpha channel.                                */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32argb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
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
    brush_alpha = context->brush.alpha;

    get = (const GX_COLOR *)pixelmap->data;
    get_count = (const GX_UBYTE *)pixelmap->aux_data;

    /* First, skip to the starting row.  */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get_count++;

            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1u);

                /* Skip repeated pixel value.  */
                get++;
            }
            else
            {
                count++;

                /* Skip raw pixel values.  */
                get += count;
            }
            xval += count;
        }
    }

    /* Now we are on the first visible row, copy pixels until we get
       to the enf of the last visible row.  */
    putrow = (GX_COLOR *)context->memory;
    putrow += yval * context->pitch;
    putrow += xpos;

    while (yval <= rotated_clip.bottom)
    {
        put = putrow;
        xval = xpos;

        while (xval < xpos + pixelmap->height)
        {
            count = *get_count++;

            if (count & 0x80)
            {

                /* Repeated value.  */
                count = (GX_UBYTE)((count & 0x7f) + 1u);
                pixel = (*get++);

                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
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
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            blend_func(context, xval, yval, pixel, brush_alpha);
                        }
                        xval++;
                    }
                }
            }
            else
            {

                /* String of non-repeated values.  */
                count++;

                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            *put = (*get);
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
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            blend_func(context, xval, yval, *get, brush_alpha);
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
/*    _gx_display_driver_32bpp_rotated_pixelmap_compressed_alpha_write    */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32argb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_pixelmap_compressed_alpha_write(GX_DRAW_CONTEXT *context,
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

    brush_alpha = context->brush.alpha;

    get = (const GX_COLOR *)pixelmap->data;
    get_count = (const GX_UBYTE *)pixelmap->aux_data;

    /* First, skip to the starting row.  */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get_count++;

            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1u);

                /* Skip repeated pixel value.  */
                get++;
            }
            else
            {
                count++;

                /* Skip raw pixel values.  */
                get += count;
            }
            xval += count;
        }
    }

    /* Now we are on the first visible row, copy pixels until we get
       to the enf of the last visible row.  */

    while (yval <= rotated_clip.bottom)
    {
        xval = xpos;

        while (xval < xpos + pixelmap->height)
        {
            count = *get_count++;

            if (count & 0x80)
            {

                /* Repeated value.  */
                count = (GX_UBYTE)((count & 0x7f) + 1u);
                alpha = (GX_UBYTE)((*get) >> 24);
                pixel = (*get++) | 0xff000000;

                combined_alpha = (GX_UBYTE)(alpha * brush_alpha / 255);

                if (combined_alpha)
                {
                    while (count--)
                    {
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            blend_func(context, xval, yval, pixel, combined_alpha);
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

                /* String of non-repeated values.  */
                count++;

                while (count--)
                {
                    if (xval >= rotated_clip.left &&
                        xval <= rotated_clip.right)
                    {
                        alpha = (GX_UBYTE)((*get) >> 24);
                        pixel = (*get) | 0xff000000;
                        combined_alpha = (GX_UBYTE)(alpha * brush_alpha / 255);
                        blend_func(context, xval, yval, pixel, combined_alpha);
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
/*    _gx_display_driver_32bpp_rotated_palette_pixelmap_compressed_write  */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated compressed */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_palette_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                                               INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT             yval;
INT             xval;
const UCHAR *get;
UCHAR           count;
GX_COLOR       *put;
GX_COLOR       *putrow;
GX_COLOR        pixel;
GX_COLOR       *palette;
GX_UBYTE        brush_alpha;
GX_RECTANGLE   *clip = context->clip;
GX_RECTANGLE    rotated_clip;
void            (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

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
    get = (const UCHAR *)pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* Compressed with  alpha is a one-byte count and  one-byte pixel index,
       first, skip to the starting row.   */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (UCHAR)((count & 0x7f) + 1);

                /* Skip repeated pixel value.  */
                get++;
            }
            else
            {
                count++;

                /* Skip raw pixel values.  */
                get += count;
            }
            xval += count;
        }
    }

    /* Now we are on the first visible row, copy pixels until we get
       to the enf of the last visible row. */
    putrow = (GX_COLOR *)context->memory;
    putrow += yval * context->pitch;
    putrow += xpos;

    /* Now we are on the first visible row, copy pixels until we get
       to the end of the last visible row.  */
    palette = (GX_COLOR *)pixelmap->aux_data;

    while (yval <= rotated_clip.bottom)
    {
        xval = xpos;
        put = putrow;

        while (xval < xpos + pixelmap->height)
        {
            count = *get++;

            if (count & 0x80)
            {
                /* Repeated value.  */
                count = (UCHAR)((count & 0x7f) + 1);

                pixel = palette[*get++];

                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
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
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            blend_func(context, xval, yval, pixel, brush_alpha);
                        }
                        xval++;
                    }
                }
            }
            else
            {
                /* String of non-repeated values.  */
                count++;
                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            pixel = palette[*get];
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
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            pixel = palette[*get];
                            blend_func(context, xval, yval, pixel, brush_alpha);
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
/*    _gx_display_driver_32bpp_rotated_palette_pixelmap_write             */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated            */
/*    uncompressed pixlemap file without transparent of palette pixelmap. */
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
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_palette_pixelmap_write(GX_DRAW_CONTEXT *context,
                                                                    INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           xval;
INT           yval;
GX_UBYTE     *getrow;
GX_UBYTE     *get;
GX_COLOR     *palette;
GX_COLOR     *put;
GX_COLOR     *putrow;
GX_RECTANGLE *clip = context->clip;
GX_RECTANGLE  rotated_clip;

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
    getrow += (pixelmap->height * (rotated_clip.top - ypos));
    getrow += (rotated_clip.left - xpos);

    palette = (GX_COLOR *)pixelmap->aux_data;

    /* Now we are on the first visible row, copy pixels until we get
       to the enf of the last visible row.  */
    putrow = (GX_COLOR *)context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += rotated_clip.left;

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        put = putrow;
        get = getrow;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            *put++ = palette[*get++];
        }
        putrow += context->pitch;
        getrow += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_palette_pixelmap_transparent_write */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.4        */
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
/*    _gx_display_driver_32bpp_pixel_write                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_palette_pixelmap_transparent_write(GX_DRAW_CONTEXT *context,
                                                                                INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           xval;
INT           yval;
GX_UBYTE     *getrow;
GX_UBYTE     *get;
GX_COLOR     *palette;

GX_RECTANGLE *clip = context->clip;
GX_RECTANGLE  rotated_clip;

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

    palette = (GX_COLOR *)pixelmap->aux_data;
    getrow = (GX_UBYTE *)(pixelmap->data);
    getrow += (pixelmap->height * (rotated_clip.top - ypos));
    getrow += (rotated_clip.left - xpos);

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        get = getrow;
        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            if ((*get) != pixelmap->transparent_color)
            {
                _gx_display_driver_32bpp_pixel_write(context, xval, yval, palette[*get]);
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
/*    _gx_display_driver_32bpp_rotated_palette_pixelmap_transparent_      */
/*                                                       compressed_write */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated compressed */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32argb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_palette_pixelmap_transparent_compressed_write(GX_DRAW_CONTEXT *context,
                                                                                           INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT             yval;
INT             xval;
const UCHAR *get;
UCHAR           count;
GX_COLOR        pixel;
GX_COLOR       *palette;
GX_COLOR       *put;
GX_COLOR       *putrow;
GX_UBYTE        brush_alpha;
GX_RECTANGLE   *clip = context->clip;
GX_RECTANGLE    rotated_clip;
void            (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

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
    get = (const UCHAR *)pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* Compressed with  alpha is a one-byte count and  one-byte pixel index,
       first, skip to the starting row.  */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (UCHAR)((count & 0x7f) + 1);

                /* Skip repeated pixel value.  */
                get++;
            }
            else
            {
                count++;

                /* Skip raw pixel values.  */
                get += count;
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

    while (yval <= rotated_clip.bottom)
    {
        xval = xpos;
        put = putrow;

        while (xval < xpos + pixelmap->height)
        {
            count = *get++;
            if (count & 0x80)
            {
                /* repeated value */
                count = (UCHAR)((count & 0x7f) + 1);
                if ((*get) != pixelmap->transparent_color)
                {
                    pixel = palette[*get];
                    if (brush_alpha == 0xff)
                    {
                        while (count--)
                        {
                            if (xval >= rotated_clip.left &&
                                xval <= rotated_clip.right)
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
                            if (xval >= rotated_clip.left &&
                                xval <= rotated_clip.right)
                            {
                                blend_func(context, xval, yval, pixel, brush_alpha);
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
                            if (xval >= rotated_clip.left &&
                                xval <= rotated_clip.right)
                            {
                                pixel = palette[*get];
                                *put = pixel;
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
                            if (xval >= rotated_clip.left &&
                                xval <= rotated_clip.right)
                            {
                                pixel = palette[*get];
                                blend_func(context, xval, yval, pixel, brush_alpha);
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
/*    _gx_display_driver_32bpp_rotated_565rgb_pixelmap_raw_write          */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.4        */
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
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_565rgb_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
                                                                       INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              xval;
INT              yval;
GX_COLOR        *putrow;
USHORT          *getrow;
GX_COLOR        *put;
const USHORT *get;

GX_RECTANGLE    *clip = context->clip;
GX_RECTANGLE     rotated_clip;

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

    putrow = (GX_COLOR *)context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += rotated_clip.left;

    getrow = (USHORT *)(pixelmap->data);
    getrow += pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        put = putrow;
        get = getrow;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            *put++ = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff,
                                                    REDVAL_16BPP(*get) << 3,
                                                    GREENVAL_16BPP(*get) << 2,
                                                    BLUEVAL_16BPP(*get) << 3);
            get++;
        }
        putrow += context->pitch;
        getrow += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_rotated_565rgb_pixelmap_compressed_write  */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated compressed */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_565rgb_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                                              INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              yval;
INT              xval;
const USHORT *get;
USHORT           count;
GX_COLOR         pixel;
GX_UBYTE         r;
GX_UBYTE         g;
GX_UBYTE         b;
GX_UBYTE         brush_alpha;
GX_COLOR        *put;
GX_COLOR        *putrow;
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
    get = (const USHORT *)pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* First, skip to the starting row.  */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get++;

            if (count & 0x8000)
            {
                count = (USHORT)((count & 0x7fff) + 1);

                /* Skip repeated pixel value.  */
                get++;
            }
            else
            {
                count++;

                /* Skip raw pixel values.  */
                get += count;
            }
            xval += count;
        }
    }

    /* Now we are on the first visible row, copy pixels until we get
       to the enf of the last visible row.  */
    putrow = (GX_COLOR *)context->memory;
    putrow += yval * context->pitch;
    putrow += xpos;

    while (yval <= rotated_clip.bottom)
    {
        xval = xpos;
        put = putrow;

        while (xval < xpos + pixelmap->height)
        {
            count = *get++;

            if (count & 0x8000)
            {

                /* Repeated value.  */
                count = (USHORT)((count & 0x7fff) + 1);
                pixel = *get++;
                r = (GX_UBYTE)(REDVAL_16BPP(pixel) << 3);
                g = (GX_UBYTE)(GREENVAL_16BPP(pixel) << 2);
                b = (GX_UBYTE)(BLUEVAL_16BPP(pixel) << 3);
                pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, r, g, b);

                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
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
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            blend_func(context, xval, yval, pixel, brush_alpha);
                        }
                        xval++;
                    }
                }
            }
            else
            {

                /* String of non-repeated values.  */
                count++;
                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            pixel = *get;
                            r = (GX_UBYTE)(REDVAL_16BPP(pixel) << 3);
                            g = (GX_UBYTE)(GREENVAL_16BPP(pixel) << 2);
                            b = (GX_UBYTE)(BLUEVAL_16BPP(pixel) << 3);
                            pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, r, g, b);
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
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            pixel = *get;
                            r = (GX_UBYTE)(REDVAL_16BPP(pixel) << 3);
                            g = (GX_UBYTE)(GREENVAL_16BPP(pixel) << 2);
                            b = (GX_UBYTE)(BLUEVAL_16BPP(pixel) << 3);
                            pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, r, g, b);
                            blend_func(context, xval, yval, pixel, brush_alpha);
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
/*    _gx_display_driver_24xrgb_rotated_565rgb_pixelmap_compressed_alpha_ */
/*                                                               write    */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32bpp_pixel_write                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_565rgb_pixelmap_compressed_alpha_write(GX_DRAW_CONTEXT *context,
                                                                                    INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                yval;
INT                xval;
const GX_UBYTE *get;
GX_UBYTE           count;
GX_COLOR           pixel;
GX_UBYTE           alpha_value;
GX_UBYTE           r;
GX_UBYTE           g;
GX_UBYTE           b;
GX_UBYTE           brush_alpha;
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
    get = (const GX_UBYTE *)pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* First, skip to the starting row.  */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get;

            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1);

                /* Skip repeated pixel value.  */
                get += 4;
            }
            else
            {
                count++;

                /* Skip raw pixel values.  */
                get += (count * 4);
            }
            xval += count;
        }
    }

    /* Now we are on the first visible row, copy pixels until we get
       to the enf of the last visible row.  */
    while (yval <= rotated_clip.bottom)
    {
        xval = xpos;

        while (xval < xpos + pixelmap->height)
        {
            count = *get;

            if (count & 0x80)
            {

                /* Repeated value.  */
                count = (GX_UBYTE)((count & 0x7f) + 1);
                alpha_value = *(get + 1);

                if (alpha_value)
                {
                    if (brush_alpha == 0xff)
                    {
                        get += 2;
                        pixel = *(USHORT *)get;
                        r = (GX_UBYTE)(REDVAL_16BPP(pixel) << 3);
                        g = (GX_UBYTE)(GREENVAL_16BPP(pixel) << 2);
                        b = (GX_UBYTE)(BLUEVAL_16BPP(pixel) << 3);
                        pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, r, g, b);
                        get += 2;
                        while (count--)
                        {
                            if (xval >= rotated_clip.left &&
                                xval <= rotated_clip.right)
                            {
                                if (alpha_value == 0xff)
                                {
                                    _gx_display_driver_32bpp_pixel_write(context, xval, yval, pixel);
                                }
                                else
                                {
                                    blend_func(context, xval, yval, pixel, alpha_value);
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
                            r = (GX_UBYTE)(REDVAL_16BPP(pixel) << 3);
                            g = (GX_UBYTE)(GREENVAL_16BPP(pixel) << 2);
                            b = (GX_UBYTE)(BLUEVAL_16BPP(pixel) << 3);
                            pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, r, g, b);
                            get += 2;
                            while (count--)
                            {
                                if (xval >= rotated_clip.left &&
                                    xval <= rotated_clip.right)
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
                }
                else
                {
                    xval += count;
                    get += 4;
                }
            }
            else
            {

                /* String of non-repeated values.  */
                count++;
                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            alpha_value = *(get + 1);
                            get += 2;
                            if (alpha_value)
                            {
                                pixel = *(USHORT *)get;
                                r = (GX_UBYTE)(REDVAL_16BPP(pixel) << 3);
                                g = (GX_UBYTE)(GREENVAL_16BPP(pixel) << 2);
                                b = (GX_UBYTE)(BLUEVAL_16BPP(pixel) << 3);
                                pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, r, g, b);
                                if (alpha_value == 0xff)
                                {
                                    _gx_display_driver_32bpp_pixel_write(context, xval, yval, pixel);
                                }
                                else
                                {
                                    blend_func(context, xval, yval, pixel, alpha_value);
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
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            alpha_value = *(get + 1);
                            get += 2;
                            if (alpha_value)
                            {
                                pixel = *(USHORT *)get;
                                r = (GX_UBYTE)(REDVAL_16BPP(pixel) << 3);
                                g = (GX_UBYTE)(GREENVAL_16BPP(pixel) << 2);
                                b = (GX_UBYTE)(BLUEVAL_16BPP(pixel) << 3);
                                pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, r, g, b);
                                combined_alpha = (GX_UBYTE)(brush_alpha * alpha_value / 255);
                                blend_func(context, xval, yval, pixel, combined_alpha);
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
/*    _gx_display_driver_24xrgb_rotated_565rgb_pixelmap_alpha_write       */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
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
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32bpp_pixel_write                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_565rgb_pixelmap_alpha_write(GX_DRAW_CONTEXT *context,
                                                                         INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                skipcount;
INT                xval;
INT                yval;
const GX_UBYTE *getalpha;
const USHORT   *get;
USHORT            *getrow;
GX_UBYTE          *getrowalpha;
GX_UBYTE           r;
GX_UBYTE           g;
GX_UBYTE           b;
GX_COLOR           pixel;
GX_UBYTE           alpha_value;

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
            alpha_value = *getalpha++;
            pixel = *get++;
            if (alpha_value)
            {
                r = (GX_UBYTE)(REDVAL_16BPP(pixel) << 3);
                g = (GX_UBYTE)(GREENVAL_16BPP(pixel) << 2);
                b = (GX_UBYTE)(BLUEVAL_16BPP(pixel) << 3);
                pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, r, g, b);
                if (alpha_value == 0xff)
                {
                    _gx_display_driver_32bpp_pixel_write(context, xval, yval, pixel);
                }
                else
                {
                    blend_func(context, xval, yval, pixel, alpha_value);
                }
            }
        }
        getrow += pixelmap->height;
        getrowalpha += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_4444argb_pixelmap_alpha_write      */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*    _gx_display_driver_32bpp_pixel_write                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_4444argb_pixelmap_alpha_write(GX_DRAW_CONTEXT *context,
                                                                           INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              skipcount;
INT              xval;
INT              yval;
USHORT          *getrow;
const USHORT *get;
UCHAR            alpha_value;
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
    skipcount = (pixelmap->height) * (rotated_clip.top - ypos);
    skipcount += (rotated_clip.left - xpos);
    getrow = (USHORT *)(pixelmap->data);
    getrow += skipcount;

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        get = getrow;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            alpha_value = (UCHAR)(((*get) & 0xf000) >> 8);
            if (alpha_value)
            {

                /* Convert 4444argb pixel to 24xrgb pixel.  */
                pixel = (GX_COLOR)((((*get) & 0x0f00) << 12) | (((*get) & 0x00f0) << 8) | (((*get) & 0x000f) << 4));
                pixel |= 0xff000000;
                if (alpha_value == 0xf0)
                {
                    _gx_display_driver_32bpp_pixel_write(context, xval, yval, pixel);
                }
                else
                {
                    blend_func(context, xval, yval, pixel, alpha_value);
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
/*    _gx_display_driver_24xrgb_4444argb_rotated_pixelmap_compressed_     */
/*                                                            alpha_write */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of rotated compressed */
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
/*    _gx_display_driver_24xrgb_pixel_blend                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_draw                      */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_4444argb_pixelmap_compressed_alpha_write(GX_DRAW_CONTEXT *context,
                                                                                      INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT              yval;
INT              xval;
const USHORT *get;
USHORT           count;
GX_COLOR         pixel;
GX_UBYTE         falpha;
GX_UBYTE         brush_alpha;
GX_UBYTE         combined_alpha;
GX_UBYTE         r;
GX_UBYTE         g;
GX_UBYTE         b;
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
    get = (const USHORT *)pixelmap->data;
    brush_alpha = context->brush.alpha;

    /* First, skip to the starting row.  */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get++;

            if (count & 0x8000)
            {
                count = (USHORT)((count & 0x7fff) + 1);

                /* Skip repeated pixel value.  */
                get++;
            }
            else
            {
                count++;

                /* Skip raw pixel values.  */
                get += count;
            }
            xval += count;
        }
    }

    /* Now we are on the first visible row, copy pixels until we get
       to the enf of the last visible row.  */
    while (yval <= rotated_clip.bottom)
    {
        xval = xpos;

        while (xval < xpos + pixelmap->height)
        {
            count = *get++;

            if (count & 0x8000)
            {
                /* Repeated value.  */
                count = (USHORT)((count & 0x7fff) + 1);
                pixel = *get++;
                falpha = (GX_UBYTE)(((USHORT)pixel & 0xf000) >> 8);
                falpha = (falpha >> 4) | falpha;
                if (falpha)
                {
                    r = (GX_UBYTE)(((USHORT)pixel & 0x0f00) >> 4);
                    r = (GX_UBYTE)((r >> 4) | r);
                    g = (GX_UBYTE)((USHORT)pixel & 0x00f0);
                    g = (GX_UBYTE)((g >> 4) | g);
                    b = (GX_UBYTE)((USHORT)pixel & 0x000f);
                    b = (GX_UBYTE)((b << 4) | b);
                    pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, r, g, b);
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
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            blend_func(context, xval, yval, pixel, combined_alpha);
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

                /* String of non-repeated values.  */
                count++;
                if (brush_alpha == 0xff)
                {
                    while (count--)
                    {
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            pixel = *get;
                            falpha = (GX_UBYTE)(((USHORT)pixel & 0xf000) >> 8);
                            falpha = (falpha >> 4) | falpha;
                            r = (GX_UBYTE)(((USHORT)pixel & 0x0f00) >> 4);
                            r = (GX_UBYTE)((r >> 4) | r);
                            g = (GX_UBYTE)((USHORT)pixel & 0x00f0);
                            g = (GX_UBYTE)((g >> 4) | g);
                            b = (GX_UBYTE)((USHORT)pixel & 0x000f);
                            b = (GX_UBYTE)((b << 4) | b);
                            pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, r, g, b);
                            if (falpha)
                            {
                                blend_func(context, xval, yval, pixel, falpha);
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
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
                        {
                            pixel = *get;
                            falpha = (GX_UBYTE)(((USHORT)pixel & 0xf000) >> 8);
                            falpha = (falpha >> 4) | falpha;
                            combined_alpha = (GX_UBYTE)(falpha * brush_alpha / 255);
                            r = (GX_UBYTE)(((USHORT)pixel & 0x0f00) >> 4);
                            r = (GX_UBYTE)((r >> 4) | r);
                            g = (GX_UBYTE)((USHORT)pixel & 0x00f0);
                            g = (GX_UBYTE)((g >> 4) | g);
                            b = (GX_UBYTE)((USHORT)pixel & 0x000f);
                            b = (GX_UBYTE)((b << 4) | b);
                            pixel = (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, r, g, b);
                            blend_func(context, xval, yval, pixel, combined_alpha);
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
/*    _gx_display_driver_24xrgb_rotated_pixelmap_draw                     */
/*                                                           6.1.5        */
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
/*    _gx_display_driver_24xrgb_rotated_palette_pixelmap_transparent_     */
/*                                                       compressed_write */
/*    _gx_display_driver_24xrgb_rotated_palette_pixelmap_transparent_write*/
/*    _gx_display_driver_24xrgb_rotated_palette_pixelmap_compressed_write */
/*    _gx_display_driver_24xrgb_rotated_palette_pixelmap_write            */
/*    _gx_display_driver_24xrgb_rotated_4444argb_pixelmap_compressed_     */
/*                                                            alpha_write */
/*    _gx_display_driver_24xrgb_rotated_4444argb_pixelmap_alpha_write     */
/*    _gx_display_driver_24xrgb_rotated_565rgb_pixelmap_compressed_alpha_ */
/*                                                                  write */
/*    _gx_display_driver_24xrgb_rotated_565rgb_pixelmap_alpha_write       */
/*    _gx_display_driver_24xrgb_rotated_565rgb_pixelmap_compressed_write  */
/*    _gx_display_driver_24xrgb_rotated_565rgb_pixelmap_raw_write         */
/*    _gx_display_driver_24xrgb_rotated_pixelmap_compressed_alpha_write   */
/*    _gx_display_driver_24xrgb_rotated_pixelmap_alpha_write              */
/*    _gx_display_driver_24xrgb_rotated_pixelmap_compressed_write         */
/*    _gx_display_driver_24xrgb_rotated_pixelmap_raw_write                */
/*    _gx_display_driver_24xrgb_rotated_pixelmap_blend                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_32bpp_rotated_pixelmap_draw(GX_DRAW_CONTEXT *context,
                                                    INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
GX_BOOL  drawn = GX_FALSE;
GX_UBYTE brush_alpha = context->brush.alpha;

    if (brush_alpha == 0)
    {
        /* Draw nothing here. Just return.  */
        return;
    }

    switch (pixelmap->format)
    {
    case GX_COLOR_FORMAT_8BIT_PALETTE:
        if (pixelmap->aux_data == GX_NULL)
        {

            /* Palette data is not availlable.  */
            return;
        }

        if (pixelmap->flags & GX_PIXELMAP_TRANSPARENT)
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* Compressed with transparent.  */
                _gx_display_driver_32bpp_rotated_palette_pixelmap_transparent_compressed_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* No compression with transparent.  */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_32bpp_rotated_palette_pixelmap_transparent_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        else
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* Compressed with no alpha.  */
                _gx_display_driver_32bpp_rotated_palette_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* No compression or alpha.  */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_32bpp_rotated_palette_pixelmap_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /* Compressed.  */
            _gx_display_driver_32bpp_rotated_4444argb_pixelmap_compressed_alpha_write(context, xpos, ypos, pixelmap);
            drawn = GX_TRUE;
        }
        else
        {
            /* No compression.  */
            if (brush_alpha == 0xff)
            {
                _gx_display_driver_32bpp_rotated_4444argb_pixelmap_alpha_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
        }
        break;

    case GX_COLOR_FORMAT_565RGB:
        if (pixelmap->flags & GX_PIXELMAP_ALPHA)
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* Compressed with alpha.  */
                _gx_display_driver_32bpp_rotated_565rgb_pixelmap_compressed_alpha_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* Uncompressed with alpha.  */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_32bpp_rotated_565rgb_pixelmap_alpha_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        else
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {

                /* Compressed without alpha.  */
                _gx_display_driver_32bpp_rotated_565rgb_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {

                /* Uncompressed withou alpha.  */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_32bpp_rotated_565rgb_pixelmap_raw_write(context, xpos, ypos, pixelmap);
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

                /* Has both compression and alpha.  */
                _gx_display_driver_32bpp_rotated_pixelmap_compressed_alpha_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {

                /* Alpha, no compression.  */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_32bpp_rotated_pixelmap_alpha_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        else
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {

                /* Compressed with no alpha.  */
                _gx_display_driver_32bpp_rotated_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {

                /* No compression or alpha.  */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_32bpp_rotated_pixelmap_raw_write(context, xpos, ypos, pixelmap);
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
        _gx_display_driver_32bpp_rotated_pixelmap_blend(context, xpos, ypos, pixelmap, brush_alpha);
    }

    return;
}

