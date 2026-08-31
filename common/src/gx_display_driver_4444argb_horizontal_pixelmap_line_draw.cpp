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


#if defined(GX_BRUSH_ALPHA_SUPPORT)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_alpha_blend    */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of uncompressed       */
/*    pixlemap file with alpha channel with brush alpha.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xstart                                x-coord of line left          */
/*    xend                                  x-coord of line right         */
/*    y                                     y-coord of line top           */
/*    info                                  GX_FILL_PIXELMAP_INFO struct  */
/*    alpha                                 Alpha value                   */
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
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_draw           */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_4444argb_horizontal_pixelmap_line_alpha_blend(GX_DRAW_CONTEXT *context,
                                                                             INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info, GX_UBYTE alpha)
{
INT              xval;
INT              offset;
INT              pic_width;
USHORT           color;
const USHORT *get;
GX_PIXELMAP     *pixelmap;
void             (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    pixelmap = info->pixelmap;
    blend_func = context->display->driver_pixel_blend;

    if (blend_func == GX_NULL)
    {
        return;
    }
    pic_width = pixelmap->width;

    if ((info->draw) && (xstart <= xend))
    {
        get = (const USHORT *)info->current_pixel_ptr;

        /*calculate the offset.*/
        offset = (info->x_offset % pic_width);

        for (xval = xstart; xval <= xend; xval++)
        {
            /*get points to the start postion of this row. So we need to calculate its position.*/
            color = *(get + offset);
            offset++;
            if (color & 0xf000)
            {
                /* not transparent */
                blend_func(context, xval, y, color, alpha);
            }

            if (offset >= pic_width)
            {
                offset -= pic_width;
            }
        }
    }

    /*This line is drawn. Update the pointer position for next row.*/
    info->current_pixel_ptr += (UINT)pic_width * sizeof(USHORT);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_raw_blend      */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of uncompressed       */
/*    pixlemap file with alpha whose value is always 0xf.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xstart                                x-coord of line left          */
/*    xend                                  x-coord of line end           */
/*    y                                     y-coord of line top           */
/*    info                                  GX_FILL_PIXELMAP_INFO struct  */
/*    alpha                                 Alpha value                   */
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
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_draw           */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_4444argb_horizontal_pixelmap_line_raw_blend(GX_DRAW_CONTEXT *context,
                                                                           INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info, GX_UBYTE alpha)
{
INT              xval;
INT              offset;
INT              pic_width;
const USHORT *get;
GX_PIXELMAP     *pixelmap;
void             (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    pixelmap = info->pixelmap;
    blend_func = context->display->driver_pixel_blend;

    if (blend_func == GX_NULL)
    {
        return;
    }

    pic_width = pixelmap->width;
    get = (const USHORT *)info->current_pixel_ptr;

    if ((info->draw) && (xstart <= xend))
    {
        /* calculate the offset. */
        offset = (info->x_offset % pic_width);

        for (xval = xstart; xval <= xend; xval++)
        {
            /*get points to the start postion of this row. So we need to calculate its position.*/
            blend_func(context, xval, y, *(get + offset), alpha);

            offset++;
            if (offset >= pic_width)
            {
                offset -= pic_width;
            }
        }
    }

    /*This line is drawn. Update the pointer position for next row.*/
    info->current_pixel_ptr += (UINT)pic_width * sizeof(USHORT);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_compressed_    */
/*    alpha_blend                                                         */
/*                                                                        */
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
/*    xstart                                x-coord of line left          */
/*    xend                                  x-coord of line end           */
/*    y                                     y-coord of line top           */
/*    info                                  GX_FILL_PIXELMAP_INFO struct  */
/*    alpha                                 Alpha value                   */
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
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_draw           */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_4444argb_horizontal_pixelmap_line_compressed_alpha_blend(GX_DRAW_CONTEXT *context,
                                                                                        INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info, GX_UBYTE alpha)
{
INT              start_pos;
INT              xval;
USHORT           count;
USHORT           pixel;
const USHORT *get = GX_NULL;
GX_PIXELMAP     *pixelmap;
void             (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    pixelmap = info->pixelmap;
    blend_func = context->display->driver_pixel_blend;

    if (blend_func == GX_NULL)
    {
        return;
    }

    if ((info->draw) && (xstart <= xend))
    {
        /* Calculate draw start position. */
        start_pos = xstart - (info->x_offset % pixelmap->width);

        /* Repeat the draw operation to fill the whole dirty area. */
        while (start_pos <= xend)
        {
            xval = start_pos;
            /*Start from where we need to repeat.*/
            get = (const USHORT *)info->current_pixel_ptr;

            while (xval < start_pos + pixelmap->width)
            {
                count = *get++;
                if (count & 0x8000)
                {
                    count = (USHORT)((count & 0x7fff) + 1);
                    pixel = *get++;
                    if (pixel & 0xf000)
                    {
                        while (count--)
                        {
                            if (xval >= xstart && xval <= xend)
                            {
                                blend_func(context, xval, y, pixel, alpha);
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
                    count++;
                    while (count--)
                    {
                        pixel = *get++;
                        if (xval >= xstart && xval <= xend)
                        {
                            if (pixel & 0xf000)
                            {
                                blend_func(context, xval, y, pixel, alpha);
                            }
                        }
                        xval++;
                    }
                }
            }
            start_pos += pixelmap->width;
        }
    }
    else
    {
        xval = 0;
        get = (const USHORT *)info->current_pixel_ptr;
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

    /*This line is drawn. cache the pointer for next line draw.*/
    info->current_pixel_ptr = (GX_UBYTE *)get;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_c_blend        */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixlemap file with alpha whose value is always 0xf.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xstart                                x-coord of line left          */
/*    xend                                  x-coord of line end           */
/*    y                                     y-coord of line top           */
/*    info                                  GX_FILL_PIXELMAP_INFO struct  */
/*    alpha                                 Alpha value                   */
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
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_draw           */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_4444argb_horizontal_pixelmap_line_compressed_blend(GX_DRAW_CONTEXT *context,
                                                                                  INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info, GX_UBYTE alpha)
{
INT              start_pos;
INT              xval;
USHORT           count;
USHORT           pixel;
const USHORT *get = GX_NULL;
GX_PIXELMAP     *pixelmap;
void             (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;
    pixelmap = info->pixelmap;
    if (blend_func == GX_NULL)
    {
        return;
    }

    if ((info->draw) && (xstart <= xend))
    {
        /* Calculate draw start position. */
        start_pos = xstart - (info->x_offset % pixelmap->width);

        /* Repeat the draw operation to fill the whole dirty area. */
        while (start_pos <= xend)
        {
            xval = start_pos;
            /*Start from where we need to repeat.*/
            get = (const USHORT *)info->current_pixel_ptr;

            while (xval < start_pos + pixelmap->width)
            {
                count = *get++;
                if (count & 0x8000)
                {
                    count = (USHORT)((count & 0x7fff) + 1);
                    pixel = *get++;
                    while (count--)
                    {
                        if (xval >= xstart && xval <= xend)
                        {
                            blend_func(context, xval, y, pixel, alpha);
                        }
                        xval++;
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
                            blend_func(context, xval, y, pixel, alpha);
                        }
                        xval++;
                    }
                }
            }
            start_pos += pixelmap->width;
        }
    }
    else
    {
        xval = 0;
        get = (const USHORT *)info->current_pixel_ptr;
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

    /* This line is drawn. cache the pointer for next line draw. */
    info->current_pixel_ptr = (GX_UBYTE *)get;
}

#endif /* GX_BRUSH_ALPHA_SUPPORT */
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_alpha_write    */
/*                                                                        */
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
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_draw           */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_4444argb_horizontal_pixelmap_line_alpha_write(GX_DRAW_CONTEXT *context,
                                                                             INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info)
{
INT              xval;
INT              offset;
INT              pic_width;
USHORT           color;
const USHORT *get;
GX_PIXELMAP     *pixelmap;
void             (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    pixelmap = info->pixelmap;
    blend_func = context->display->driver_pixel_blend;

    if (blend_func == GX_NULL)
    {
        return;
    }
    pic_width = pixelmap->width;

    if ((info->draw) && (xstart <= xend))
    {
        get = (const USHORT *)info->current_pixel_ptr;

        /* calculate the offset. */
        offset = (info->x_offset % pic_width);

        for (xval = xstart; xval <= xend; xval++)
        {
            /* get points to the start postion of this row. So we need to calculate its position. */
            color = *(get + offset);
            offset++;

            if (color & 0xf000)
            {
                /* not transparent */
                blend_func(context, xval, y, color, 0xff);
            }

            if (offset >= pic_width)
            {
                offset -= pic_width;
            }
        }
    }

    /* This line is drawn. Update the pointer position for next row. */
    info->current_pixel_ptr += (UINT)pic_width * sizeof(USHORT);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_raw_write      */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of uncompressed       */
/*    pixlemap file with alpha whose value is always 0xf.                 */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_draw           */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_4444argb_horizontal_pixelmap_line_raw_write(GX_DRAW_CONTEXT *context,
                                                                           INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info)
{
INT              xval;
INT              offset;
INT              pic_width;
const USHORT *get;
USHORT          *put;
GX_PIXELMAP     *pixelmap;

    pixelmap = info->pixelmap;

    pic_width = pixelmap->width;
    get = (const USHORT *)info->current_pixel_ptr;

    if ((info->draw) && (xstart <= xend))
    {
        put = (USHORT *)context->memory;
        put += y * context->pitch;
        put += xstart;

        /* calculate the offset. */
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
    info->current_pixel_ptr += (UINT)pic_width * sizeof(USHORT);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_c_a_write      */
/*                                                                        */
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
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_draw           */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_4444argb_horizontal_pixelmap_line_compressed_alpha_write(GX_DRAW_CONTEXT *context,
                                                                                        INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info)
{
INT              start_pos;
INT              xval;
USHORT           count;
USHORT           pixel;
const USHORT *get = GX_NULL;
GX_PIXELMAP     *pixelmap;
void             (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    pixelmap = info->pixelmap;
    blend_func = context->display->driver_pixel_blend;

    if (blend_func == GX_NULL)
    {
        return;
    }

    if ((info->draw) && (xstart <= xend))
    {
        /* Calculate draw start position. */
        start_pos = xstart - (info->x_offset % pixelmap->width);

        /* Repeat the draw operation to fill the whole dirty area. */
        while (start_pos <= xend)
        {
            xval = start_pos;
            /*Start from where we need to repeat.*/
            get = (const USHORT *)info->current_pixel_ptr;

            while (xval < start_pos + pixelmap->width)
            {
                count = *get++;
                if (count & 0x8000)
                {
                    count = (USHORT)((count & 0x7fff) + 1);
                    pixel = *get++;
                    if (pixel & 0xf000)
                    {
                        while (count--)
                        {
                            if (xval >= xstart && xval <= xend)
                            {
                                blend_func(context, xval, y, pixel, 0xff);
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
                    count++;
                    while (count--)
                    {
                        pixel = *get++;
                        if (xval >= xstart && xval <= xend)
                        {
                            if (pixel & 0xf000)
                            {
                                blend_func(context, xval, y, pixel, 0xff);
                            }
                        }
                        xval++;
                    }
                }
            }
            start_pos += pixelmap->width;
        }
    }
    else
    {
        xval = 0;
        get = (const USHORT *)info->current_pixel_ptr;
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

    /* This line is drawn. cache the pointer for next line draw. */
    info->current_pixel_ptr = (GX_UBYTE *)get;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_c_write        */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixlemap file with alpha whose value is always 0xf.                 */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_draw           */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_4444argb_horizontal_pixelmap_line_compressed_write(GX_DRAW_CONTEXT *context,
                                                                                  INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info)
{
INT              start_pos;
INT              xval;
USHORT           count;
USHORT           pixel;
const USHORT *get = GX_NULL;
USHORT          *put;
GX_PIXELMAP     *pixelmap;

    pixelmap = info->pixelmap;

    if ((info->draw) && (xstart <= xend))
    {
        /* Calculate draw start position. */
        start_pos = xstart - (info->x_offset % pixelmap->width);

        put = (USHORT *)context->memory;
        put += y * context->pitch + start_pos;

        /* Repeat the draw operation to fill the whole dirty area. */
        while (start_pos <= xend)
        {
            xval = start_pos;

            /* Start from where we need to repeat. */
            get = (const USHORT *)info->current_pixel_ptr;

            while (xval < start_pos + pixelmap->width)
            {
                count = *get++;
                if (count & 0x8000)
                {
                    count = (USHORT)((count & 0x7fff) + 1);
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
            start_pos += pixelmap->width;
        }
    }
    else
    {
        xval = 0;
        get = (const USHORT *)info->current_pixel_ptr;
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

    /* This line is drawn. cache the pointer for next line draw. */
    info->current_pixel_ptr = (GX_UBYTE *)get;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_draw           */
/*                                                                        */
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
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_c_a_blend      */
/*                                          Real display driver pixelmap  */
/*                                            line draw function          */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_alpha_blend    */
/*                                          Real display driver pixelmap  */
/*                                            line draw function          */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_c_blend        */
/*                                          Real display driver pixelmap  */
/*                                            line draw function          */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_raw_blend      */
/*                                          Real display driver pixelmap  */
/*                                            line draw function          */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_c_a_write      */
/*                                          Real display driver pixelmap  */
/*                                            line draw function          */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_alpha_write    */
/*                                          Real display driver pixelmap  */
/*                                            line draw function          */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_c_write        */
/*                                          Real display driver pixelmap  */
/*                                            line draw function          */
/*    _gx_display_driver_4444argb_horizontal_pixelmap_line_raw_write      */
/*                                          Real display driver pixelmap  */
/*                                            line draw function          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_4444argb_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT *context,
                                                               INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info)
{
#if defined GX_BRUSH_ALPHA_SUPPORT
GX_UBYTE alpha;

    alpha = context->brush.alpha;
    if ((alpha == 0) || (info->pixelmap == GX_NULL))
    {
        /* Nothing to drawn. Just return. */
        return;
    }
    if (alpha != 0xff)
    {

        if (info->pixelmap->flags & GX_PIXELMAP_ALPHA)
        {
            if (info->pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* has both compression and alpha */
                _gx_display_driver_4444argb_horizontal_pixelmap_line_compressed_alpha_blend(context, xstart, xend, y, info, alpha);
            }
            else
            {
                /* alpha, no compression */
                _gx_display_driver_4444argb_horizontal_pixelmap_line_alpha_blend(context, xstart, xend, y, info, alpha);
            }
        }
        else
        {
            if (info->pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* compressed with no alpha */
                _gx_display_driver_4444argb_horizontal_pixelmap_line_compressed_blend(context, xstart, xend, y, info, alpha);
            }
            else
            {
                /* no compression or alpha */
                _gx_display_driver_4444argb_horizontal_pixelmap_line_raw_blend(context, xstart, xend, y, info, alpha);
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
#endif

    if (info->pixelmap->flags & GX_PIXELMAP_ALPHA)
    {
        if (info->pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /* has both compression and alpha */
            _gx_display_driver_4444argb_horizontal_pixelmap_line_compressed_alpha_write(context, xstart, xend, y, info);
        }
        else
        {
            /* alpha, no compression */
            _gx_display_driver_4444argb_horizontal_pixelmap_line_alpha_write(context, xstart, xend, y, info);
        }
    }
    else
    {
        if (info->pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /* has both compression */
            _gx_display_driver_4444argb_horizontal_pixelmap_line_compressed_write(context, xstart, xend, y, info);
        }
        else
        {
            /*no alpha, no compression */
            _gx_display_driver_4444argb_horizontal_pixelmap_line_raw_write(context, xstart, xend, y, info);
        }
    }

    /*Current pixelmap has gone over, so the offset pointer should be reset.*/
    if (info->current_pixel_ptr >= info->pixelmap->data + info->pixelmap->data_size)
    {
        info->current_pixel_ptr = (GX_UBYTE *)info->pixelmap->data;
        info->current_aux_ptr = (GX_UBYTE *)info->pixelmap->aux_data;
    }
}
