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
/*    _gx_display_driver_8bpp_rotated_pixelmap_raw_write                  */
/*                                                           6.1.4        */
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
static void _gx_display_driver_8bpp_rotated_pixelmap_raw_write(GX_DRAW_CONTEXT *context,
                                                               INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                xval;
INT                yval;
GX_UBYTE          *putrow;
GX_UBYTE          *getrow;
GX_UBYTE          *put;
const GX_UBYTE *get;

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

    putrow = (GX_UBYTE *)context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += rotated_clip.left;

    getrow = (GX_UBYTE *)(pixelmap->data);
    getrow += pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);

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
/*    _gx_display_driver_8bpp_rotated_pixelmap_compressed_write           */
/*                                                                        */
/*                                                           6.1.4        */
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
static void _gx_display_driver_8bpp_rotated_pixelmap_compressed_write(GX_DRAW_CONTEXT *context,
                                                                      INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                yval;
INT                xval;
const GX_UBYTE *get;
GX_UBYTE          *put;
GX_UBYTE          *putrow;
GX_UBYTE           count;
GX_UBYTE           pixel = 0;
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

    get = (const GX_UBYTE *)pixelmap->data;

    /* First, skip to the starting row.  */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1);

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
       to the end of the last visible row.  */
    putrow = (GX_UBYTE *)context->memory;
    putrow += yval * context->pitch;
    putrow += xpos;

    while (yval <= rotated_clip.bottom)
    {
        put = putrow;
        xval = xpos;

        while (xval < (xpos + pixelmap->height))
        {
            count = *get++;

            if (count & 0x80)
            {

                /* Repeated value.  */
                count = (GX_UBYTE)((count & 0x7f) + 1);
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
        putrow +=  context->pitch;
        yval++;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_rotated_pixelmap_transparent_write          */
/*                                                                        */
/*                                                           6.1.4        */
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
static void _gx_display_driver_8bpp_rotated_pixelmap_transparent_write(GX_DRAW_CONTEXT *context,
                                                                       INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                xval;
INT                yval;
GX_UBYTE          *putrow;
GX_UBYTE          *getrow;
GX_UBYTE          *put;
GX_UBYTE           inval;
const GX_UBYTE *get;

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

    putrow = (GX_UBYTE *)context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += rotated_clip.left;

    getrow = (GX_UBYTE *)(pixelmap->data);
    getrow += pixelmap->height * (rotated_clip.top - ypos);
    getrow += (rotated_clip.left - xpos);

    for (yval = rotated_clip.top; yval <= rotated_clip.bottom; yval++)
    {
        put = putrow;
        get = getrow;

        for (xval = rotated_clip.left; xval <= rotated_clip.right; xval++)
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
        getrow += pixelmap->height;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_rotated_pixelmap_compressed_transparent_    */
/*                                                                  write */
/*                                                                        */
/*                                                           6.1.4        */
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
static void _gx_display_driver_8bpp_rotated_pixelmap_compressed_transparent_write(GX_DRAW_CONTEXT *context,
                                                                                  INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
INT                yval;
INT                xval;
const GX_UBYTE *get;
GX_UBYTE          *put;
GX_UBYTE          *putrow;
GX_UBYTE           count;
GX_UBYTE           pixel = 0;
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

    get = (const GX_UBYTE *)pixelmap->data;

    /* First, skip to the starting row.  */
    for (yval = ypos; yval < rotated_clip.top; yval++)
    {
        xval = 0;
        while (xval < pixelmap->height)
        {
            count = *get++;

            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1);

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
       to the end of the last visible row.  */
    putrow = (GX_UBYTE *)context->memory;
    putrow += yval * context->pitch;
    putrow += xpos;

    while (yval <= rotated_clip.bottom)
    {
        put = putrow;
        xval = xpos;
        while (xval < (xpos + pixelmap->height))
        {
            count = *get++;

            if (count & 0x80)
            {

                /* Repeated value.  */
                count = (GX_UBYTE)((count & 0x7f) + 1);
                pixel = *get++;

                if (pixel == pixelmap->transparent_color)
                {
                    put += count;
                    xval += count;
                }
                else
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
            }
            else
            {

                /* String of non-repeated values.  */
                count++;

                while (count--)
                {
                    pixel = *get++;

                    if (xval >= rotated_clip.left &&
                        xval <= rotated_clip.right &&
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
/*    _gx_display_driver_8bpp_rotated_pixelmap_draw                       */
/*                                                           6.1.4        */
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
/*     _gx_display_driver_8bit_rotated_pixelmap_compressed_write          */
/*     _gx_display_driver_8bit_rotated_pixelmap_compressed_transparent_   */
/*                                                                  write */
/*     _gx_display_driver_8bit_rotated_pixelmap_transparent_write         */
/*     _gx_display_driver_8bit_rotated_pixelmap_raw_write                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_8bpp_rotated_pixelmap_draw(GX_DRAW_CONTEXT *context,
                                                   INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{

    if (pixelmap->format != GX_COLOR_FORMAT_8BIT_PALETTE ||
        (pixelmap->flags & GX_PIXELMAP_ALPHA))
    {

        /* Wrong color format for this driver.  */
        return;
    }

    if (pixelmap->flags & GX_PIXELMAP_TRANSPARENT)
    {
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {

            /* Has both compression and transparent.  */
            _gx_display_driver_8bpp_rotated_pixelmap_compressed_transparent_write(context, xpos, ypos, pixelmap);
        }
        else
        {

            /* Transparent, no compression.  */
            _gx_display_driver_8bpp_rotated_pixelmap_transparent_write(context, xpos, ypos, pixelmap);
        }
    }
    else
    {
        if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {

            /* Compressed with no transparency.  */
            _gx_display_driver_8bpp_rotated_pixelmap_compressed_write(context, xpos, ypos, pixelmap);
        }
        else
        {

            /* No compression or transaprency.  */
            _gx_display_driver_8bpp_rotated_pixelmap_raw_write(context, xpos, ypos, pixelmap);
        }
    }
}

