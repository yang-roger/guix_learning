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

#include "gx_display_driver.h"

#include "gx_context.h"
#include "gx_pixelmap.h"

static const USHORT* gx_compressed_pixelmap_line_skip(GX_FILL_PIXELMAP_INFO* info)
{
    const USHORT* get = (const USHORT*)info->current_pixel_ptr;

    USHORT count;

    INT pixelmap_width = info->pixelmap->width;
    INT x = 0;
    while (x < pixelmap_width)
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

        x += count;
    }

    return get;
}

static const GX_UBYTE* gx_compressed_alpha_pixelmap_line_skip(GX_FILL_PIXELMAP_INFO* info)
{
    const GX_UBYTE* get = (const GX_UBYTE*)info->current_pixel_ptr;

    GX_UBYTE count;

    INT pixelmap_width = info->pixelmap->width;
    INT x = 0;
    while (x < pixelmap_width)
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

        x += count;
    }

    return get;
}

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
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_raw_blend(
    GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info, GX_UBYTE alpha)
{
    void (*blend_func)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;
    if (blend_func == GX_NULL)
    {
        return;
    }

    INT pixelmap_width = info->pixelmap->width;

    if (info->draw && (xstart <= xend))
    {
        // Pick the data pointer to the current row.
        const USHORT* get = (const USHORT*)info->current_pixel_ptr;

        INT offset = (info->x_offset % pixelmap_width);

        for (INT x = xstart; x <= xend; ++x)
        {
            USHORT color = *(get + offset);

            blend_func(context, x, y, color, alpha);

            ++offset;
            if (offset >= pixelmap_width)
            {
                offset -= pixelmap_width;
            }
        }
    }

    // Update data pointer for next row.
    info->current_pixel_ptr += (UINT)pixelmap_width * sizeof(USHORT);
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
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_alpha_blend(
    GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info, GX_UBYTE alpha)
{
    void (*blend_func)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;
    if (blend_func == GX_NULL)
    {
        return;
    }

    INT pixelmap_width = info->pixelmap->width;

    if (info->draw && (xstart <= xend))
    {
        // Pick the data pointer to the current row.
        const USHORT* get = (const USHORT*)info->current_pixel_ptr;
        const GX_UBYTE* getalpha = (const GX_UBYTE*)info->current_aux_ptr;

        INT offset = (info->x_offset % pixelmap_width);

        for (INT x = xstart; x <= xend; ++x)
        {
            USHORT color = *(get + offset);
            GX_UBYTE falpha = *(getalpha + offset);

            if (falpha)
            {
                GX_UBYTE combined_alpha = (GX_UBYTE)(falpha * alpha / 255);

                blend_func(context, x, y, color, combined_alpha);
            }

            ++offset;
            if (offset >= pixelmap_width)
            {
                offset -= pixelmap_width;
            }
        }
    }

    // Update data pointers for next row.
    info->current_pixel_ptr += (UINT)pixelmap_width * sizeof(USHORT);
    info->current_aux_ptr += (UINT)pixelmap_width * sizeof(GX_UBYTE);
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
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_blend(
    GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info, GX_UBYTE alpha)
{
    void (*blend_func)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;
    if (blend_func == GX_NULL)
    {
        return;
    }

    const USHORT* get = GX_NULL;

    if (info->draw && (xstart <= xend))
    {
        USHORT count;
        USHORT color;

        INT pixelmap_width = info->pixelmap->width;

        INT start_pos = xstart - (info->x_offset % pixelmap_width);

        while (start_pos <= xend)
        {
            // Start from where we need to repeat.
            get = (const USHORT*)info->current_pixel_ptr;

            INT x = start_pos;
            while (x < start_pos + pixelmap_width)
            {
                count = *get++;

                if (count & 0x8000)
                {
                    count = (USHORT)((count & 0x7fff) + 1);
                    color = *get++;

                    while (count--)
                    {
                        if (x >= xstart && x <= xend)
                        {
                            blend_func(context, x, y, color, alpha);
                        }

                        x++;
                    }
                }
                else
                {
                    count++;
                    while (count--)
                    {
                        color = *get++;

                        if (x >= xstart && x <= xend)
                        {
                            blend_func(context, x, y, color, alpha);
                        }

                        x++;
                    }
                }
            }

            start_pos += pixelmap_width;
        }
    }
    else
    {
        get = gx_compressed_pixelmap_line_skip(info);
    }

    // Update data pointer for next row.
    info->current_pixel_ptr = (GX_UBYTE*)get;
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
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_alpha_blend(
    GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info, GX_UBYTE alpha)
{
    void (*blend_func)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;
    if (blend_func == GX_NULL)
    {
        return;
    }

    const GX_UBYTE* get = GX_NULL;

    if (info->draw && (xstart <= xend))
    {
        GX_UBYTE count;
        USHORT color;
        GX_UBYTE falpha;
        GX_UBYTE combined_alpha;

        INT pixelmap_width = info->pixelmap->width;

        INT start_pos = xstart - (info->x_offset % pixelmap_width);

        while (start_pos <= xend)
        {
            // Start from where we need to repeat.
            get = (const GX_UBYTE*)info->current_pixel_ptr;

            INT x = start_pos;
            while (x < start_pos + pixelmap_width)
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
                        color = *((const USHORT*)get);
                        get += 2;

                        while (count--)
                        {
                            if (x >= xstart && x <= xend)
                            {
                                blend_func(context, x, y, color, combined_alpha);
                            }

                            x++;
                        }
                    }
                    else
                    {
                        get += 4;
                        x += count;
                    }
                }
                else
                {
                    count++;
                    while (count--)
                    {
                        if (x >= xstart && x <= xend)
                        {
                            falpha = *(get + 1);
                            combined_alpha = (GX_UBYTE)(falpha * alpha / 255);

                            get += 2;
                            color = *((const USHORT*)get);
                            get += 2;

                            blend_func(context, x, y, color, combined_alpha);
                        }
                        else
                        {
                            get += 4;
                        }

                        x++;
                    }
                }
            }

            start_pos += pixelmap_width;
        }
    }
    else
    {
        get = gx_compressed_alpha_pixelmap_line_skip(info);
    }

    // Update data pointer for the next line.
    info->current_pixel_ptr = (GX_UBYTE*)get;
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
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_raw_write(
    GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info)
{
    INT pixelmap_width = info->pixelmap->width;

    if (info->draw && (xstart <= xend))
    {
        // Pick up data pointers to the current line.
        const USHORT* get = (const USHORT*)info->current_pixel_ptr;

        USHORT* put = (USHORT*)context->memory;
        GX_CALCULATE_PUTROW(put, xstart, y, context);

        INT offset = (info->x_offset % pixelmap_width);

        for (INT x = xstart; x <= xend; x++)
        {
            *put++ = *(get + offset);

            ++offset;
            if (offset >= pixelmap_width)
            {
                offset -= pixelmap_width;
            }
        }
    }

    // Update data pointer for the next line.
    info->current_pixel_ptr += (UINT)pixelmap_width * sizeof(USHORT);
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
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_alpha_write(
    GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info)
{
    void (*blend_func)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;
    if (blend_func == GX_NULL)
    {
        return;
    }

    INT pixelmap_width = info->pixelmap->width;

    if (info->draw && (xstart <= xend))
    {
        // Pick up data pointers to the current line.
        const USHORT* get = (const USHORT*)info->current_pixel_ptr;
        const GX_UBYTE* getalpha = (const GX_UBYTE*)info->current_aux_ptr;

        INT offset = (info->x_offset % pixelmap_width);

        for (INT x = xstart; x <= xend; ++x)
        {
            USHORT color = *(get + offset);
            GX_UBYTE alpha = *(getalpha + offset);

            blend_func(context, x, y, color, alpha);

            ++offset;
            if (offset >= pixelmap_width)
            {
                offset -= pixelmap_width;
            }
        }
    }

    // Update data pointers for the next line.
    info->current_pixel_ptr += (UINT)pixelmap_width * sizeof(USHORT);
    info->current_aux_ptr += (UINT)pixelmap_width * sizeof(GX_UBYTE);
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
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_write(
    GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info)
{
    const USHORT* get = GX_NULL;

    if (info->draw && (xstart <= xend))
    {
        USHORT count;
        USHORT color;

        INT pixelmap_width = info->pixelmap->width;

        INT start_pos = xstart - (info->x_offset % pixelmap_width);

        USHORT* put = (USHORT*)context->memory;
        GX_CALCULATE_PUTROW(put, start_pos, y, context);

        // Repeat the draw operation to fill the whole dirty area.
        while (start_pos <= xend)
        {
            // Start from where we need to repeat.
            get = (const USHORT*)info->current_pixel_ptr;

            INT x = start_pos;
            while (x < start_pos + pixelmap_width)
            {
                count = *get++;

                if (count & 0x8000)
                {
                    count = (USHORT)((count & 0x7fff) + 1);
                    color = *get++;

                    while (count--)
                    {
                        if (x >= xstart && x <= xend)
                        {
                            *put = color;
                        }

                        x++;
                        put++;
                    }
                }
                else
                {
                    count++;
                    while (count--)
                    {
                        color = *get++;

                        if (x >= xstart && x <= xend)
                        {
                            *put = color;
                        }

                        x++;
                        put++;
                    }
                }
            }

            start_pos += pixelmap_width;
        }
    }
    else
    {
        get = gx_compressed_pixelmap_line_skip(info);
    }

    // Update data pointer for the next line.
    info->current_pixel_ptr = (GX_UBYTE*)get;
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
static void _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_alpha_write(
    GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info)
{
    void (*blend_func)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    blend_func = context->display->driver_pixel_blend;
    if (blend_func == GX_NULL)
    {
        return;
    }

    INT pixelmap_width = info->pixelmap->width;

    const GX_UBYTE* get = GX_NULL;

    INT x;
    GX_UBYTE count;

    if (info->draw && (xstart <= xend))
    {
        USHORT   color;
        GX_UBYTE alpha;

        INT start_pos = xstart - (info->x_offset % pixelmap_width);

        // Repeat the draw operation to fill the whole dirty area.
        while (start_pos <= xend)
        {
            // Start from where we need to repeat.
            get = (const GX_UBYTE*)info->current_pixel_ptr;

            x = start_pos;
            while (x < start_pos + pixelmap_width)
            {
                count = *get;
                if (count & 0x80)
                {
                    count = (GX_UBYTE)((count & 0x7f) + 1u);
                    alpha = *(get + 1);
                    if (alpha)
                    {
                        get += 2;
                        color = *((const USHORT*)get);
                        get += 2;

                        while (count--)
                        {
                            if (x >= xstart && x <= xend)
                            {
                                blend_func(context, x, y, color, alpha);
                            }

                            x++;
                        }
                    }
                    else
                    {
                        get += 4;
                        x += count;
                    }
                }
                else
                {
                    count++;
                    while (count--)
                    {
                        if (x >= xstart && x <= xend)
                        {
                            alpha = *(get + 1);

                            get += 2;
                            color = *((const USHORT*)get);
                            get += 2;

                            blend_func(context, x, y, color, alpha);
                        }
                        else
                        {
                            get += 4;
                        }

                        x++;
                    }
                }
            }

            start_pos += pixelmap_width;
        }
    }
    else
    {
        get = gx_compressed_alpha_pixelmap_line_skip(info);
    }

    // Update data pinter for the next line.
    info->current_pixel_ptr = (GX_UBYTE*)get;
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
void _gx_display_driver_565rgb_horizontal_pixelmap_line_draw(
    GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info)
{
#if defined(GX_BRUSH_ALPHA_SUPPORT)

    GX_UBYTE alpha = context->brush.alpha;

    if ((alpha == 0) || (info->pixelmap == GX_NULL))
    {
        return; // Nothing to drawn. Just return.
    }

    if (alpha != 0xff)
    {
        if (info->pixelmap->flags & GX_PIXELMAP_ALPHA)
        {
            if (info->pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                // has both compression and alpha
                _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_alpha_blend(context, xstart, xend, y, info, alpha);
            }
            else
            {
                // alpha, no compression
                _gx_display_driver_565rgb_horizontal_pixelmap_line_alpha_blend(context, xstart, xend, y, info, alpha);
            }
        }
        else
        {
            if (info->pixelmap->flags & GX_PIXELMAP_COMPRESSED)
            {
                // compressed with no alpha
                _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_blend(context, xstart, xend, y, info, alpha);
            }
            else
            {
                // no compression or alpha
                _gx_display_driver_565rgb_horizontal_pixelmap_line_raw_blend(context, xstart, xend, y, info, alpha);
            }
        }

        info->data_ptrs_reset_();

        return;
    }

#endif // GX_BRUSH_ALPHA_SUPPORT

    if (info->pixelmap->flags & GX_PIXELMAP_ALPHA)
    {
        if (info->pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            // has both compression and alpha
            _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_alpha_write(context, xstart, xend, y, info);
        }
        else
        {
            // alpha, no compression
            _gx_display_driver_565rgb_horizontal_pixelmap_line_alpha_write(context, xstart, xend, y, info);
        }
    }
    else
    {
        if (info->pixelmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            // compressed with no alpha
            _gx_display_driver_565rgb_horizontal_pixelmap_line_compressed_write(context, xstart, xend, y, info);
        }
        else
        {
            // no compression or alpha
            _gx_display_driver_565rgb_horizontal_pixelmap_line_raw_write(context, xstart, xend, y, info);
        }
    }

    info->data_ptrs_reset_();
}
