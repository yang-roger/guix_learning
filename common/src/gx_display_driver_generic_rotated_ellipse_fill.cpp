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

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_pixelmap.h"

#if defined(GX_ARC_DRAWING_SUPPORT)

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_rotated_ellipse_fill                     */
/*                                                           6.1.6        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Display driver to draw a filled ellipse.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xcenter                               x-coord of center of ellipse  */
/*    ycenter                               y-coord of center of ellipse  */
/*    a                                     Length of the Semi-major Axis */
/*    b                                     Length of the Semi-minor Axis */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_horizontal_line_draw]                            */
/*                                          Basic display driver          */
/*                                            horizontal line draw routine*/
/*    [gx_display_driver_horizontal_pixelmap_line_draw]                   */
/*                                          Basic display driver          */
/*                                            horizontal pixelmap line    */
/*                                            draw function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_generic_rotated_ellipse_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b)
{
/* The ellipse fill function follows Bresenham ellipse algorithm while
   connecting the two point that symmetric with respect to y-axis. */

GX_DISPLAY           *display;
GX_RECTANGLE         *clip;
GX_BRUSH             *brush;
INT                   x1;
INT                   x;
INT                   y;
INT                   y0;
INT                   y1;
INT                   sign[2] = {1, -1};
INT                  *pLineEnds;
INT                   index;
INT                   width;
INT                   Index;
INT                   aa;
INT                   bb;
INT                   ypos;
INT                   xmin;
INT                   xmax;
INT                   skip_line;
GX_PIXELMAP          *pixelmap = GX_NULL;
GX_COLOR              fill_color;
GX_FILL_PIXELMAP_INFO info;
GX_BYTE               xsign;
INT                   error;
INT                   realval;
GX_BOOL               record;

    display = context->display;
    clip = context->clip;
    brush = &context->brush;

    if (brush->style & GX_BRUSH_PIXELMAP_FILL)
    {
        if (brush->pixelmap == GX_NULL)
        {
            /* Nothing should be drawn if pixelmap isn't set with GX_BRUSH_PIXELMAP_FILL style. */
            return;
        }

        pixelmap = brush->pixelmap;

        if (pixelmap->format != display->color_format)
        {
            /* Display driver only support its native format pixelmap.*/
            /* Nothing should be drawn if pixelmap format isn't support. */
            return;
        }

        memset(&info, 0, sizeof(GX_FILL_PIXELMAP_INFO));

        info.pixelmap = brush->pixelmap;
        info.current_pixel_ptr = (GX_UBYTE *)info.pixelmap->data;

        if (pixelmap->aux_data_size)
        {
            info.current_aux_ptr = (GX_UBYTE *)pixelmap->aux_data;
        }
    }

    xmin = xcenter - a;
    xmax = xcenter + a;

    /* Calculate minimum y line. */
    if (clip->left > xmin)
    {
        xmin = clip->left;
    }

    /* Calculate maximum y line. */
    if (clip->right < xmax)
    {
        xmax = clip->right;
    }

    width = (xmax - xmin + 1);
    pLineEnds = _gx_system_scratchpad;

    /* default the point array to being off the screen on both sides: */
    for (Index = 0; Index < width * 2; Index += 2)
    {
        pLineEnds[Index] = 2000;
        pLineEnds[Index + 1] = 0;
    }

    aa = a * a;
    bb = b * b;
    x = 0;
    y = b;
    error = 0;

    /* Region I of the first quarter of the ellipse.  */
    while (2 * bb * (x + 1) < aa * (2 * y - 1))
    {
        /* calculate error of next pixel. */
        realval = bb - bb * (x + 1) * (x + 1) / aa;
        error = (y << 8) - (INT)(_gx_utility_math_sqrt((UINT)(realval << 10)) << 3);

        if (error >= 510)
        {
            /* The slope in point(x + 1, y) is greater than -1,
               make point(x, y) the delimit pixel, break here. */
            realval = bb - bb * x * x / aa;
            error = (y << 8) - (INT)(_gx_utility_math_sqrt((UINT)(realval << 10)) << 3);
            break;
        }

        y0 = ycenter - (y - 1);
        y1 = ycenter + (y - 1);

        if (y0 < clip->top)
        {
            y0 = clip->top;
        }

        if (y1 > clip->bottom)
        {
            y1 = clip->bottom;
        }

        for (index = 0; index < 2; index++)
        {
            x1 = x * sign[index] + xcenter;

            if ((x1 >= xmin) && (x1 <= xmax))
            {
                Index = (x1 - xmin) << 1;
                pLineEnds[Index] = y0;
                pLineEnds[Index + 1] = y1;
            }
        }

        if (error >= 255)
        {
            y--;
        }

        x++;
    }

    record = GX_TRUE;

    y0 = ycenter - y;
    y1 = ycenter + y;

    if (y0 < clip->top)
    {
        y0 = clip->top;
    }

    if (y1 > clip->bottom)
    {
        y1 = clip->bottom;
    }

    for (index = 0; index < 2; index++)
    {
        x1 = x * sign[index] + xcenter;

        if ((x1 >= xmin) && (x1 <= xmax))
        {
            Index = (x1 - xmin) << 1;
            pLineEnds[Index] = y0;
            pLineEnds[Index + 1] = y1;
        }
    }

    /* Region II of the first quarter of the ellipse.  */
    while (y > 0)
    {
        y--;

        realval = aa - aa * y * y / bb;
        error = (INT)(_gx_utility_math_sqrt((UINT)(realval << 10)) << 3) - (x << 8);

        while (error >= 255)
        {
            error -= 255;
            x++;

            record = GX_TRUE;
        }

        if (record)
        {
            record = GX_FALSE;

            y0 = ycenter - y;
            y1 = ycenter + y;

            if (y0 < clip->top)
            {
                y0 = clip->top;
            }

            if (y1 > clip->bottom)
            {
                y1 = clip->bottom;
            }

            for (index = 0; index < 2; index++)
            {
                x1 = x * sign[index] + xcenter;

                if ((x1 >= xmin) && (x1 <= xmax))
                {
                    Index = (x1 - xmin) << 1;
                    pLineEnds[Index] = y0;
                    pLineEnds[Index + 1] = y1;
                }
            }
        }
    }

    if (pixelmap != GX_NULL)
    {
        if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
        {
            skip_line = (xmax - (xcenter - a) + 1) % pixelmap->width;

            if (skip_line)
            {
                skip_line = pixelmap->width - skip_line;
            }

            x1 = xmax;
            xsign = -1;
            Index = (width - 1) * 2;
        }
        else
        {
            skip_line = (xmin - clip->left);

            x1 = xmin;
            xsign = 1;
            Index = 0;
        }

        if (skip_line)
        {
            info.draw = GX_FALSE;
            while (skip_line--)
            {
                display->driver_horizontal_pixelmap_line_draw(context, 0, 0, 0, &info);
            }
        }

        info.draw = GX_TRUE;
        ypos = ycenter - b;

        for (x = xmin; x <= xmax; x++)
        {
            info.x_offset = pLineEnds[Index] - ypos;
            display->driver_horizontal_pixelmap_line_draw(context, pLineEnds[Index], pLineEnds[Index + 1], x1, &info);

            x1 += xsign;
            Index += xsign;
            Index += xsign;
        }
    }
    else
    {
        Index = 0;
        fill_color = brush->fill_color;

        for (x = xmin; x <= xmax; x++)
        {
            display->driver_vertical_line_draw(context, pLineEnds[Index], pLineEnds[Index + 1], x, 1, fill_color);
            Index += 2;
        }
    }
}
#endif

