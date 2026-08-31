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
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_raw_blend        */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of uncompressed       */
/*    pixlemap file without alpha channel with brush alpha.               */
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
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_draw             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_raw_blend(GX_DRAW_CONTEXT *context,
                                                                         INT xstart, INT xend, INT y,
                                                                         GX_FILL_PIXELMAP_INFO *info, GX_UBYTE alpha)
{
INT              xval;
INT              offset;
INT              pic_width;
const USHORT *get;
USHORT           pixel;
GX_PIXELMAP     *pixelmap;
void             (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE balpha);

    blend_func = context->display->driver_pixel_blend;
    if (blend_func == GX_NULL)
    {
        return;
    }

    pixelmap = info->pixelmap;
    pic_width = pixelmap->width;

    /* Pick the data pointer to the current row. */
    get = (const USHORT *)info->current_pixel_ptr;

    if ((info->draw) && (xstart <= xend))
    {
        /* Calculate the map offset in x-axis. */
        offset = (info->x_offset % pic_width);

        for (xval = xstart; xval <= xend; xval++)
        {
            pixel = *(get + offset);
            blend_func(context, xval, y, pixel, alpha);
            offset++;

            if (offset >= pic_width)
            {
                offset -= pic_width;
            }
        }
    }

    /* Update data pointer for next row.*/
    info->current_pixel_ptr += (UINT)pic_width * sizeof(USHORT);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_alpha_blend      */
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
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_draw             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_alpha_blend(GX_DRAW_CONTEXT *context,
                                                                           INT xstart, INT xend, INT y,
                                                                           GX_FILL_PIXELMAP_INFO *info, GX_UBYTE alpha)
{
INT                xval;
const USHORT   *get;
const GX_UBYTE *getalpha;
USHORT             color;
GX_UBYTE           falpha;
GX_UBYTE           combined_alpha;
GX_PIXELMAP       *pixelmap;
INT                pic_width;
INT                offset;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;
    if (blend_func == GX_NULL)
    {
        return;
    }

    pixelmap = info->pixelmap;
    pic_width = pixelmap->width;

    if ((info->draw) && (xstart <= xend))
    {
        /* Pick the data pointer to the current row. */
        get = (const USHORT *)info->current_pixel_ptr;
        getalpha = (const GX_UBYTE *)info->current_aux_ptr;

        /* Calculate the map offset in x-axis. */
        offset = (info->x_offset % pic_width);

        for (xval = xstart; xval <= xend; xval++)
        {
            color = *(get + offset);
            falpha = *(getalpha + offset);

            if (falpha)
            {
                combined_alpha = (GX_UBYTE)(falpha * alpha / 255);

                blend_func(context, xval, y, color, combined_alpha);
            }

            offset++;
            if (offset >= pic_width)
            {
                offset -= pic_width;
            }
        }
    }

    /* Update data pointers for next row. */
    info->current_pixel_ptr += (UINT)pic_width * sizeof(USHORT);
    info->current_aux_ptr += (UINT)pic_width * sizeof(GX_UBYTE);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_blend */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixlemap file without alpha channel with brush_alpha.               */
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
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_draw             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_blend(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info, GX_UBYTE alpha)
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

    /* Update data pointer for next row. */
    info->current_pixel_ptr = (GX_UBYTE *)get;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_c_a_blend        */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles writing of compressed         */
/*    pixlemap file with alpha channel with brush alpha.                  */
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
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_draw             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_alpha_blend(GX_DRAW_CONTEXT *context,
                                                                                      INT xstart, INT xend, INT y,
                                                                                      GX_FILL_PIXELMAP_INFO *info, GX_UBYTE alpha)
{
INT                xval;
GX_UBYTE           count;
INT                start_pos;
GX_UBYTE           falpha;
GX_UBYTE           combined_alpha;
USHORT             pixel;
const GX_UBYTE *get = GX_NULL;
const USHORT   *getpixel;
GX_PIXELMAP       *pixelmap;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    pixelmap = info->pixelmap;
    blend_func = context->display->driver_pixel_blend;

    if (blend_func == GX_NULL)
    {
        return;
    }

    if ((info->draw) && (xstart <= xend))
    {
        /* Calculate the draw start position. */
        start_pos = xstart - (info->x_offset % pixelmap->width);

        while (start_pos <= xend)
        {
            xval = start_pos;

            /*Start from where we need to repeat.*/
            get = (const GX_UBYTE *)info->current_pixel_ptr;
            while (xval < start_pos + pixelmap->width)
            {
                count = *get;
                if (count & 0x80)
                {
                    count = (GX_UBYTE)((count & 0x7f) + 1u);
                    falpha = *(get + 1);
                    combined_alpha = (GX_UBYTE)(falpha * alpha / 255);
                    if (combined_alpha)
                    {
                        get += 2;
                        getpixel = (const USHORT *)get;
                        pixel = *getpixel;
                        get += 2;

                        while (count--)
                        {
                            if (xval >= xstart && xval <= xend)
                            {
                                blend_func(context, xval, y, pixel, combined_alpha);
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
                    count++;
                    while (count--)
                    {
                        if (xval >= xstart && xval <= xend)
                        {
                            falpha = *(get + 1);
                            combined_alpha = (GX_UBYTE)(falpha * alpha / 255);
                            get += 2;
                            getpixel = (USHORT *)get;
                            pixel = *getpixel;
                            get += 2;
                            blend_func(context, xval, y, pixel, combined_alpha);
                        }
                        else
                        {
                            get += 4;
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
        /* Skip this line. */
        xval = 0;
        get = (const GX_UBYTE *)info->current_pixel_ptr;
        while (xval < pixelmap->width)
        {
            count = *get;
            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1);
                get += 4;
            }
            else
            {
                count++;
                get += count * 4;
            }
            xval += count;
        }
    }

    /* Update data pointer for the next line. */
    info->current_pixel_ptr = (GX_UBYTE *)get;
}

#endif /* GX_BRUSH_ALPHA_SUPPORT */

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_raw_write        */
/*                                                                        */
/*                                                           6.X          */
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
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_draw             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_raw_write(GX_DRAW_CONTEXT *context,
                                                                         INT xstart, INT xend, INT y,
                                                                         GX_FILL_PIXELMAP_INFO *info)
{
INT              xval;
INT              offset;
INT              pic_width;
const USHORT *get = GX_NULL;
USHORT          *put;
GX_PIXELMAP     *pixelmap;

    pixelmap = info->pixelmap;

    pic_width = pixelmap->width;

    /* Pickup data pointer for the current line. */
    get = (const USHORT *)info->current_pixel_ptr;

    if ((info->draw) && (xstart <= xend))
    {
        put = (USHORT *)context->memory;
        GX_CALCULATE_PUTROW(put, xstart, y, context);

        /*calculate the offset.*/
        offset = (info->x_offset % pic_width);

        for (xval = xstart; xval <= xend; xval++)
        {
            *put++ = *(get + offset);
            offset++;

            if (offset >= pic_width)
            {
                offset -= pic_width;
            }
        }
    }

    /* Update data pointer for the next line. */
    info->current_pixel_ptr += (UINT)pic_width * sizeof(USHORT);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_alpha_write      */
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
/*    [gx_display_driver_pixel_write]       Basic display driver pixel    */
/*                                            write function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_draw             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_alpha_write(GX_DRAW_CONTEXT *context,
                                                                           INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info)
{
INT                xval;
const USHORT   *get;
const GX_UBYTE *getalpha;
USHORT             color;
GX_UBYTE           alpha;
GX_PIXELMAP       *pixelmap;
INT                pic_width;
INT                offset;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;
    pixelmap = info->pixelmap;

    if (blend_func == GX_NULL)
    {
        return;
    }

    pic_width = pixelmap->width;
    if ((info->draw) && (xstart <= xend))
    {
        /* Pick up data pointers to the current line. */
        get = (const USHORT *)info->current_pixel_ptr;
        getalpha = (const GX_UBYTE *)info->current_aux_ptr;

        /* calculate map offset in x-axis. */
        offset = (info->x_offset % pic_width);

        for (xval = xstart; xval <= xend; xval++)
        {
            color = *(get + offset);
            alpha = *(getalpha + offset);

            blend_func(context, xval, y, color, alpha);

            offset++;
            if (offset >= pic_width)
            {
                offset -= pic_width;
            }
        }
    }

    /* Update data pointers for the next line. */
    info->current_pixel_ptr += (UINT)pic_width * sizeof(USHORT);
    info->current_aux_ptr += (UINT)pic_width * sizeof(GX_UBYTE);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_write */
/*                                                                        */
/*                                                           6.3.0        */
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
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_draw             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_write(GX_DRAW_CONTEXT *context,
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
        GX_CALCULATE_PUTROW(put, start_pos, y, context);

        /*Repeat the draw operation to fill the whole dirty area.*/
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

    /* Update data pointer for the next line. */
    info->current_pixel_ptr = (GX_UBYTE *)get;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_c_a_write        */
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
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_draw             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_alpha_write(GX_DRAW_CONTEXT *context,
                                                                                      INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO *info)
{
INT                xval;
GX_UBYTE           count;
INT                start_pos;
GX_UBYTE           alpha;
USHORT             pixel;
const GX_UBYTE *get = GX_NULL;
const USHORT   *getpixel;
GX_PIXELMAP       *pixelmap;
void               (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

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

            /* Start from where we need to repeat. */
            get = (const GX_UBYTE *)info->current_pixel_ptr;
            while (xval < start_pos + pixelmap->width)
            {
                count = *get;
                if (count & 0x80)
                {
                    count = (GX_UBYTE)((count & 0x7f) + 1u);
                    alpha = *(get + 1);
                    if (alpha)
                    {
                        get += 2;

                        getpixel = (const USHORT *)get;
                        pixel = *getpixel;
                        get += 2;

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
                        get += 4;
                        xval += count;
                    }
                }
                else
                {
                    count++;
                    while (count--)
                    {
                        if (xval >= xstart && xval <= xend)
                        {
                            alpha = *(get + 1);
                            get += 2;
                            getpixel = (USHORT *)get;
                            pixel = *getpixel;
                            get += 2;
                            blend_func(context, xval, y, pixel, alpha);
                        }
                        else
                        {
                            get += 4;
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
        /* Just do skip operation here. */
        xval = 0;
        get = (const GX_UBYTE *)info->current_pixel_ptr;
        while (xval < pixelmap->width)
        {
            count = *get;
            if (count & 0x80)
            {
                count = (GX_UBYTE)((count & 0x7f) + 1);
                get += 4;
            }
            else
            {
                count++;
                get += count * 4;
            }
            xval += count;
        }
    }

    /* Update data pinter for the next line. */
    info->current_pixel_ptr = (GX_UBYTE *)get;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_horizontal_pixelmap_line_draw             */
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
/*     _gx_display_driver_565rgb_horizontal_pixelmap_line_c_a_blend       */
/*                                          Real pixelmap blend function  */
/*     _gx_display_driver_565rgb_horizontal_pixelmap_line_alpha_blend     */
/*                                          Real pixelmap blend function  */
/*     _gx_display_driver_565rgb_horizontal_pixelmap_line_c_blend         */
/*                                          Real pixelmap blend function  */
/*     _gx_display_driver_565rgb_horizontal_pixelmap_line_raw_blend       */
/*                                          Real pixelmap blend function  */
/*     _gx_display_driver_565rgb_horizontal_pixelmap_line_c_a_write       */
/*                                          Real pixelmap write function  */
/*     _gx_display_driver_565rgb_horizontal_pixelmap_line_alpha_write     */
/*                                          Real pixelmap write function  */
/*     _gx_display_driver_565rgb_horizontal_pixelmap_line_c_write         */
/*                                          Real pixelmap write function  */
/*     _gx_display_driver_565rgb_horizontal_pixelmap_line_raw_write       */
/*                                          Real pixelmap write function  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_565rgb_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT *context,
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
                _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_alpha_blend(context, xstart, xend, y, info, alpha);
            }
            else
            {
                /* alpha, no compression */
                _gx_display_driver_565rgb_horizontal_pixelmap_line_alpha_blend(context, xstart, xend, y, info, alpha);
            }
        }
        else
        {
            if (info->pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                /* compressed with no alpha */
                _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_blend(context, xstart, xend, y, info, alpha);
            }
            else
            {
                /* no compression or alpha */
                _gx_display_driver_565rgb_horizontal_pixelmap_line_raw_blend(context, xstart, xend, y, info, alpha);
            }
        }

        /* Data pointer goes to the end of the fill map, move it to the start again. */
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
            _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_alpha_write(context, xstart, xend, y, info);
        }
        else
        {
            /* alpha, no compression */
            _gx_display_driver_565rgb_horizontal_pixelmap_line_alpha_write(context, xstart, xend, y, info);
        }
    }
    else
    {
        if (info->pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            /* compressed with no alpha */
            _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_write(context, xstart, xend, y, info);
        }
        else
        {
            /* no compression or alpha */
            _gx_display_driver_565rgb_horizontal_pixelmap_line_raw_write(context, xstart, xend, y, info);
        }
    }

    /* Data pointers goes to the end of full map, move it to the start again. */
    if (info->current_pixel_ptr >= info->pixelmap->data + info->pixelmap->data_size)
    {
        info->current_pixel_ptr = (GX_UBYTE *)info->pixelmap->data;
        info->current_aux_ptr = (GX_UBYTE *)info->pixelmap->aux_data;
    }
}
