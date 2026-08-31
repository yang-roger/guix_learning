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
/*    _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_raw_write  */
/*                                                                        */
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
/*    xstart                                x-coord of line left          */
/*    xend                                  x-coord of line right         */
/*    y                                     y-coord of line top           */
/*    info                                  GX_FILL_PIXELMAP_INFO struct  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_8bpp_horizontal_pixelmap_line_draw               */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_raw_write(GX_DRAW_CONTEXT *context,
                                                                               INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info)
{
INT                xval;
INT                offset;
INT                pic_width;
const GX_UBYTE *get;
GX_UBYTE          *put;
GX_PIXELMAP       *pixelmap;

    pixelmap = info->pixelmap;

    pic_width = pixelmap->height;

    if ((info->draw) && (xstart <= xend))
    {
        get = info->current_pixel_ptr;
        put = (GX_UBYTE *)context->memory;
        put += y * context->pitch + xstart;

        /* Calculate the map offset in x-axis. */
        offset = (info->x_offset % pic_width);

        for (xval = xstart; xval <= xend; xval++)
        {
            /* get points to the start postion of this row. So we need to calculate its position. */
            *put++ = *(get + offset);
            offset++;
            if (offset >= pic_width)
            {
                offset -= pic_width;
            }
        }
    }

    /* This line is drawn. Update the pointer position for next row. */
    info->current_pixel_ptr += (UINT)pic_width * sizeof(GX_UBYTE);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_           */
/*                                                       compressed_write */
/*                                                                        */
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
/*    xstart                                x-coord of line left          */
/*    xend                                  x-coord of line right         */
/*    y                                     y-coord of line top           */
/*    info                                  GX_FILL_PIXELMAP_INFO struct  */
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
/*    _gx_display_driver_8bpp_horizontal_pixelmap_line_draw               */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_compressed_write(GX_DRAW_CONTEXT *context,
                                                                                      INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info)
{
INT                start_pos;
INT                xval;
GX_UBYTE           count;
const GX_UBYTE *get = GX_NULL;
GX_UBYTE           pixel;
GX_UBYTE          *put;
GX_PIXELMAP       *pixelmap;

    pixelmap = info->pixelmap;

    if ((info->draw) && (xstart <= xend))
    {
        /* Calculate draw start position. */
        start_pos = xstart - (info->x_offset % pixelmap->height);

        put = (GX_UBYTE *)context->memory;
        put += y * context->pitch + start_pos;

        /*Repeat the draw operation to fill the whole dirty area.*/
        while (start_pos <= xend)
        {
            xval = start_pos;

            /*Start from where we need to repeat.*/
            get = (const GX_UBYTE *)info->current_pixel_ptr;

            while (xval < start_pos + pixelmap->width)
            {
                count = *get++;
                if (count & 0x80)
                {
                    count = (GX_UBYTE)((count & 0x7f) + 1);
                    pixel = *get++;
                    while (count--)
                    {
                        if (xval >= xstart && xval <= xend)
                        {
                            *put = pixel;
                        }
                        xval++;
                        put++;
                    }
                }
                else
                {
                    count++;
                    while (count--)
                    {
                        pixel = *get++;
                        if (xval >= xstart && xval <= xend)
                        {
                            *put = pixel;
                        }
                        xval++;
                        put++;
                    }
                }
            }
            start_pos += pixelmap->height;
        }
    }
    else
    {
        xval = 0;
        get = (const GX_UBYTE *)info->current_pixel_ptr;
        while (xval < pixelmap->height)
        {
            count = *get++;
            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1);
                get++;
            }
            else
            {
                count++;
                get += count;
            }
            xval += count;
        }
    }

    /*This line is drawn. cache the pointer for next line draw.*/
    info->current_pixel_ptr = (GX_UBYTE *)get;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_rotated_horizontal_line_pixelmap_           */
/*                                                      transparent_write */
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
/*    pixlemap file with alpha channel.                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xstart                                x-coord of line left          */
/*    xend                                  x-coord of line right         */
/*    y                                     y-coord of line top           */
/*    info                                  GX_FILL_PIXELMAP_INFO struct  */
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
/*    _gx_display_driver_8bpp_horizontal_pixelmap_line_draw               */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_transparent_write(GX_DRAW_CONTEXT *context,
                                                                                       INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info)
{
INT                xval;
INT                offset;
INT                pic_width;
const GX_UBYTE *get;
GX_UBYTE          *put;
GX_PIXELMAP       *pixelmap;
GX_UBYTE           pixel;

    pixelmap = info->pixelmap;
    pic_width = pixelmap->height;

    if ((info->draw) && (xstart <= xend))
    {
        get = info->current_pixel_ptr;
        put = (GX_UBYTE *)context->memory;
        put += y * context->pitch + xstart;

        /* Calculate the map offset in x-axis. */
        offset = (info->x_offset % pic_width);

        for (xval = xstart; xval <= xend; xval++)
        {
            /* get points to the start postion of this row. So we need to calculate its position. */
            pixel = *(get + offset);
            offset++;
            if (offset >= pic_width)
            {
                offset -= pic_width;
            }

            if (pixel != pixelmap->transparent_color)
            {
                *put = pixel;
            }
            put++;
        }
    }

    /* This line is drawn. Update the pointer position for next row. */
    info->current_pixel_ptr += (UINT)pic_width * sizeof(GX_UBYTE);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_           */
/*                                           compressed_transparent_write */
/*                                                                        */
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
/*    xstart                                x-coord of line left          */
/*    xend                                  x-coord of line right         */
/*    y                                     y-coord of line top           */
/*    info                                  GX_FILL_PIXELMAP_INFO struct  */
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
/*    _gx_display_driver_8bpp_horizontal_pixelmap_line_draw               */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_compressed_transparent_write(GX_DRAW_CONTEXT *context,
                                                                                                  INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info)
{
INT                start_pos;
INT                xval;
GX_UBYTE           count;
const GX_UBYTE *get = GX_NULL;
GX_UBYTE           pixel;
GX_UBYTE          *put;
GX_PIXELMAP       *pixelmap;

    pixelmap = info->pixelmap;

    if ((info->draw) && (xstart <= xend))
    {
        /* Calcualte draw start position. */
        start_pos = xstart - (info->x_offset % pixelmap->height);

        put = (GX_UBYTE *)context->memory;
        put += y * context->pitch + start_pos;


        /*Repeat the draw operation to fill the whole dirty area.*/
        while (start_pos <= xend)
        {
            xval = start_pos;

            /*Start from where we need to repeat.*/
            get = (const GX_UBYTE *)info->current_pixel_ptr;

            while (xval < start_pos + pixelmap->height)
            {
                count = *get++;
                if (count & 0x80)
                {
                    count = (GX_UBYTE)((count & 0x7f) + 1);
                    pixel = *get++;
                    if (pixel != pixelmap->transparent_color)
                    {
                        while (count--)
                        {
                            if (xval >= xstart && xval <= xend)
                            {
                                *put = pixel;
                            }
                            xval++;
                            put++;
                        }
                    }
                    else
                    {
                        xval += count;
                        put += count;
                    }
                }
                else
                {
                    count++;
                    while (count--)
                    {
                        pixel = *get++;

                        if (xval >= xstart && xval <= xend)
                        {
                            if (pixel != pixelmap->transparent_color)
                            {
                                *put = pixel;
                            }
                        }
                        xval++;
                        put++;
                    }
                }
            }
            start_pos += pixelmap->height;
        }
    }
    else
    {
        xval = 0;
        get = (const GX_UBYTE *)info->current_pixel_ptr;
        while (xval < pixelmap->height)
        {
            count = *get++;
            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1);
                get++;
            }
            else
            {
                count++;
                get += count;
            }
            xval += count;
        }
    }

    /* This line is drawn. cache the pointer for next line draw.*/
    info->current_pixel_ptr = (GX_UBYTE *)get;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_rotated_pixelmap_draw                       */
/*                                                           6.1.5        */
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
/*    xstart                                x-coord of line left          */
/*    xend                                  x-coord of line end           */
/*    y                                     y-coord of line top           */
/*    info                                  GX_FILL_PIXELMAP_INFO struct  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*     _gx_display_driver_8bit_rotated_horizontal_pixelmap_line_raw_write */
/*     _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_          */
/*                                                       compressed_write */
/*     _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_          */
/*                                                      transparent_write */
/*     _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_          */
/*                                           compressed_transparent_write */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT *context,
                                                                   INT ystart, INT yend, INT x, GX_FILL_PIXELMAP_INFO *info)
{
INT xstart;
INT xend;
INT y;

    if (info->pixelmap == GX_NULL)
    {
        /* Nothing to drawn. Just return. */
        return;
    }

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        xstart = ystart;
        xend = yend;
        y = context->canvas->x_resolution - x - 1;
    }
    else
    {
        xstart = context->canvas->y_resolution - yend - 1;
        xend = context->canvas->y_resolution - ystart - 1;
        y = x;
        info->x_offset = (yend - ystart + 1 + info->x_offset) % info->pixelmap->height;

        if (info->x_offset)
        {
            info->x_offset = info->pixelmap->height - info->x_offset;
        }
    }

    if (info->pixelmap->flags & GX_PIXELMAP_TRANSPARENT)
    {
        if (info->pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /* has both compression and transparent */
            _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_compressed_transparent_write(context, xstart, xend, y, info);
        }
        else
        {
            /* transparent, no compression */
            _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_transparent_write(context, xstart, xend, y, info);
        }
    }
    else
    {
        if (info->pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /* compressed with no transparency */
            _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_compressed_write(context, xstart, xend, y, info);
        }
        else
        {
            /* no compression or transaprency */
            _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_raw_write(context, xstart, xend, y, info);
        }
    }


    /*Current pixelmap has gone over, so the offset pointer should be reset.*/
    if (info->current_pixel_ptr >= info->pixelmap->data + info->pixelmap->data_size)
    {
        info->current_pixel_ptr = (GX_UBYTE *)info->pixelmap->data;
        info->current_aux_ptr = (GX_UBYTE *)info->pixelmap->aux_data;
    }

    return;
}

