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
/*    _gx_display_driver_565rgb_rotated_pixelmap_raw_write                */
/*                                                           6.1.3        */
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
static void _gx_display_driver_565rgb_rotated_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
                                                                 INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT           yval;
INT           width;
USHORT       *putrow;
USHORT       *getrow;
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

    putrow = (USHORT *)context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += rotated_clip.left;

    getrow = (USHORT *)(pixelmap->data);
    getrow += pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);

    width = rotated_clip.right - rotated_clip.left + 1;

    for (yval = clip->left; yval <= clip->right; yval++)
    {
        memcpy(putrow, getrow, (size_t)(width * 2)); /* Use case of memcpy is verified. */
        putrow += context->pitch;
        getrow += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_rotated_pixelmap_alpha_write              */
/*                                                           6.1.3        */
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
static void _gx_display_driver_565rgb_rotated_pixelmap_alpha_write(GX_DRAW_CONTEXT *context,
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
GX_RECTANGLE       rotated_clip;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    blend_func = _gx_display_driver_565rgb_pixel_blend;

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

    /* Calculate how many pixels to skip. */
    skipcount = pixelmap->height * (rotated_clip.top - ypos);
    skipcount += (rotated_clip.left - xpos);
    getrow = (USHORT *)(pixelmap->data);
    getrow += skipcount;

    getrowalpha = (GX_UBYTE *)(pixelmap->aux_data);
    getrowalpha += skipcount;

    for (yval  = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        get = getrow;
        getalpha = getrowalpha;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            blend_func(context, xval, yval, *get++, *getalpha++);
        }
        getrow += pixelmap->height;
        getrowalpha += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_rotated_pixelmap_compressed_write         */
/*                                                                        */
/*                                                           6.1.3        */
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
static void _gx_display_driver_565rgb_rotated_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
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
GX_RECTANGLE     rotated_clip;

    /* Compressed with no alpha is a two-byte count and two-byte pixel value.  */

    get = (const USHORT *)pixelmap->data;

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

    /* First, skip to the starting row.  */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get++;

            if (count & 0x8000)
            {
                count = (USHORT)((count & 0x7fff) + 1u);

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
    putrow = (USHORT *)context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += xpos;

    while (yval <= rotated_clip.bottom)
    {
        put = putrow;
        xval = xpos;

        while (xval < xpos + pixelmap->height)
        {
            count = *get++;

            if (count & 0x8000)
            {
                /* Repeated value.  */
                count = (USHORT)((count & 0x7fff) + 1u);
                pixel = *get++;
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
                /* String of non-repeated values.  */
                count++;

                while (count--)
                {
                    if (xval >= rotated_clip.left &&
                        xval <= rotated_clip.right)
                    {
                        *put = *get;
                    }
                    put++;
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
/*    _gx_display_driver_565rgb_rotated_pixelmap_compressed_alpha_write   */
/*                                                                        */
/*                                                           6.1.3        */
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
static void _gx_display_driver_565rgb_rotated_pixelmap_compressed_alpha_write(GX_DRAW_CONTEXT *context,
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
GX_RECTANGLE       rotated_clip;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    blend_func = _gx_display_driver_565rgb_pixel_blend;

    get = pixelmap->data;
    brush_alpha = context->brush.alpha;

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

    /* Compressed with alpha is byte count, byte alpha, and and two-byte pixel value.  */

    /* First, skip to the starting row.  */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get;

            if (count & 0x80)
            {
                count = (USHORT)((count & 0x7f) + 1u);
                get +=  4;      /* Skip repeated pixel value.  */
            }
            else
            {
                count++;
                get += (count * 4);      /* Skip string of non-repeated pixels.  */
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
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
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
/*    _gx_display_driver_565rgb_rotated_palette_pixelmap_raw_write        */
/*                                                                        */
/*                                                           6.1.3        */
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
static void _gx_display_driver_565rgb_rotated_palette_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
                                                                         INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                xval;
INT                yval;
USHORT            *putrow;
GX_UBYTE          *getrow;
USHORT            *put;
const GX_UBYTE    *get;
GX_COLOR          *palette;
GX_COLOR           color;

GX_RECTANGLE      *clip = context->clip;
GX_RECTANGLE       rotated_clip;

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

    putrow = (USHORT *)context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += rotated_clip.left;

    getrow = (GX_UBYTE *)(pixelmap->data);
    getrow += pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);

    palette = (GX_COLOR *)pixelmap->aux_data;

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        put = putrow;
        get = getrow;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            color = palette[*get++];
            *put++ = GX_COLOR_565RGB_FROM_32RGB(color);
        }
        putrow += context->pitch;
        getrow += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_rotated_palette_pixelmap_transparent_raw_ */
/*                                                                  write */
/*                                                                        */
/*                                                           6.1.3        */
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
static void _gx_display_driver_565rgb_rotated_palette_pixelmap_transparent_raw_write(GX_DRAW_CONTEXT *context,
                                                                                     INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                xval;
INT                yval;
USHORT            *putrow;
GX_UBYTE          *getrow;
USHORT            *put;
const GX_UBYTE    *get;
GX_COLOR          *palette;
GX_COLOR           color;

GX_RECTANGLE      *clip = context->clip;
GX_RECTANGLE       rotated_clip;

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

    putrow = (USHORT *)context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += rotated_clip.left;

    getrow = (GX_UBYTE *)(pixelmap->data);
    getrow += pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);

    palette = (GX_COLOR *)pixelmap->aux_data;

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        put = putrow;
        get = getrow;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
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
        getrow += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_rotated_palette_pixelmap_transparent      */
/*                                                      _compressed_write */
/*                                                                        */
/*                                                           6.1.3        */
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
static void _gx_display_driver_565rgb_rotated_palette_pixelmap_transparent_compressed_write(GX_DRAW_CONTEXT *context,
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
GX_RECTANGLE       rotated_clip;

    get = (const GX_UBYTE *)pixelmap->data;
    brush_alpha = context->brush.alpha;

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

    /* Compressed with no alpha is a one-byte count and one-byte index value.  */

    /* First, skip to the starting row.  */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (USHORT)((count & 0x7f) + 1u);
                get++;      /* Skip repeated pixel value.  */
            }
            else
            {
                count++;
                get += count;   /* Skip raw pixel values.  */
            }
            xval += count;
        }
    }

    /* Now we are on the first visible row, copy pixels until we get
       to the end of the last visible row. */
    putrow = (USHORT *)context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += xpos;

    palette = (GX_COLOR *)pixelmap->aux_data;

    while (yval <= rotated_clip.bottom)
    {
        put = putrow;
        xval = xpos;

        while (xval < xpos + pixelmap->height)
        {
            count = *get++;

            if (count & 0x80)
            {
                /* Count the number of repeated value. */
                count = (USHORT)((count & 0x7f) + 1u);

                if ((*get) != pixelmap->transparent_color)
                {
                    color = palette[*get];
                    pixel = GX_COLOR_565RGB_FROM_32RGB(color);

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
                /* String of non-repeated values.  */
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
                            if (xval >= rotated_clip.left &&
                                xval <= rotated_clip.right)
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
/*    _gx_display_driver_565rgb_rotated_palette_pixelmap_compressed_write */
/*                                                                        */
/*                                                           6.1.3        */
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
static void _gx_display_driver_565rgb_rotated_palette_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
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
GX_RECTANGLE       rotated_clip;

    get = (const GX_UBYTE *)pixelmap->data;
    brush_alpha = context->brush.alpha;

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

    /* Compressed with no alpha is a one-byte count and one-byte index value.  */

    /* First, skip to the starting row.  */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (USHORT)((count & 0x7f) + 1u);
                get++;      /* Skip repeated pixel value.  */
            }
            else
            {
                count++;
                get += count;   /* Skip raw pixel values.  */
            }
            xval += count;
        }
    }

    /* Now we are on the first visible row, copy pixels until we get
       to the end of the last visible row. */
    putrow = (USHORT *)context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += xpos;

    palette = (GX_COLOR *)pixelmap->aux_data;

    while (yval <= rotated_clip.bottom)
    {
        put = putrow;
        xval = xpos;

        while (xval < xpos + pixelmap->height)
        {
            count = *get++;

            if (count & 0x80)
            {
                /* Repeated value.  */
                count = (USHORT)((count & 0x7f) + 1u);

                color = palette[*get++];
                pixel = GX_COLOR_565RGB_FROM_32RGB(color);

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
                            _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, brush_alpha);
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
                        if (xval >= rotated_clip.left &&
                            xval <= rotated_clip.right)
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
/*    _gx_display_driver_16bpp_rotated_4444argb_pixelmap_raw_write        */
/*                                                                        */
/*                                                           6.1.3        */
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
static void _gx_display_driver_16bpp_rotated_4444argb_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
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

    /* Calculate how many pixels to skip.  */
    skipcount = pixelmap->height * (rotated_clip.top - ypos);
    skipcount += (rotated_clip.left - xpos);
    getrow = (USHORT *)(pixelmap->data);
    getrow += skipcount;

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        get = getrow;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
        {
            /* 0x000f-->b , 0x00f0-->g , 0x0f00-->r , 0xf000-->a */
            /* 4444bgra -->565rgb */
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
        getrow += pixelmap->height;
    }
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_16bpp_rotated_4444argb_pixelmap_compressed_write */
/*                                                                        */
/*                                                           6.1.3        */
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
static void _gx_display_driver_16bpp_rotated_4444argb_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
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
                count = (USHORT)((count & 0x7fff) + 1u);
                get++;      /* Skip repeated pixel value.  */
            }
            else
            {
                count++;
                get += count;   /* Skip raw pixel values. */
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
                            if (xval >= rotated_clip.left &&
                                xval <= rotated_clip.right)
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
                            if (xval >= rotated_clip.left &&
                                xval <= rotated_clip.right)
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
                            alpha_value = (GX_UBYTE)((pixel & 0xf000) >> 8);
                            alpha_value = (alpha_value >> 4) | alpha_value;
                            if (alpha_value)
                            {
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
/*    _gx_display_driver_565rgb_rotated_pixelmap_draw                     */
/*                                                           6.1.3        */
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
/*     _gx_display_driver_565rgb_rotated_pixelmap_compressed_alpha_write  */
/*     _gx_display_driver_565rgb_rotated_pixelmap_alpha_write             */
/*     _gx_display_driver_565rgb_rotated_pixelmap_compressed_write        */
/*     _gx_display_driver_565rgb_rotated_pixelmap_raw_write               */
/*     _gx_display_driver_565rgb_rotated_palette_pixelmap_compressed_write*/
/*     _gx_display_driver_565rgb_rotated_palette_pixelmap_raw_write       */
/*     _gx_display_driver_16bpp_rotated_4444argb_pixelmap_raw_write       */
/*     _gx_display_driver_565rgb_rotated_palette_pixelmap_transparent_    */
/*       compressed_write                                                 */
/*     _gx_display_driver_565rgb_palette_pixelmap_transparent_raw_write   */
/*     _gx_display_driver_16bpp_4444argb_pixelmap_compressed_write        */
/*     _gx_display_driver_565rgb_pixelmap_blend                           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_565rgb_rotated_pixelmap_draw(GX_DRAW_CONTEXT *context,
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
                /* Compressed with.  */
                _gx_display_driver_565rgb_rotated_palette_pixelmap_transparent_compressed_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* No compression.  */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_565rgb_rotated_palette_pixelmap_transparent_raw_write(context, xpos, ypos, pixelmap);
                    drawn = GX_TRUE;
                }
            }
        }
        else
        {
            if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* Compressed with.  */

                _gx_display_driver_565rgb_rotated_palette_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* No compression.  */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_565rgb_rotated_palette_pixelmap_raw_write(context, xpos, ypos, pixelmap);
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
                /* Has both compression and alpha.  */
                _gx_display_driver_565rgb_rotated_pixelmap_compressed_alpha_write(context,
                                                                                  xpos, ypos, pixelmap);
                drawn = GX_TRUE;
            }
            else
            {
                /* Alpha, no compression.  */
                if (brush_alpha == 0xff)
                {
                    _gx_display_driver_565rgb_rotated_pixelmap_alpha_write(context, xpos, ypos, pixelmap);
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
                    /* Compressed with no alpha.  */
                    _gx_display_driver_565rgb_rotated_pixelmap_compressed_write(context,
                                                                                xpos, ypos, pixelmap);
                }
                else
                {
                    /* No compression or alpha.  */
                    _gx_display_driver_565rgb_rotated_pixelmap_raw_write(context,
                                                                         xpos, ypos, pixelmap);
                }
                drawn = GX_TRUE;
            }
        }
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /* Not write yet. */
            _gx_display_driver_16bpp_rotated_4444argb_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
            drawn = GX_TRUE;
        }
        else
        {
            if (brush_alpha == 0xff)
            {
                _gx_display_driver_16bpp_rotated_4444argb_pixelmap_raw_write(context, xpos, ypos, pixelmap);
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
        _gx_display_driver_565rgb_rotated_pixelmap_blend(context, xpos, ypos, pixelmap, brush_alpha);
    }

    return;
}

