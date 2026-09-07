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
#include "gx_context.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_aliased_filled_circle_draw               */
/*                                                                        */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Display driver to draw filled circals with anti-aliased edge.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xcenter                               x-coord of center of circle   */
/*    ycenter                               y-coord of center of circle   */
/*    r                                     Radius of circle              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_pixel_blend]       Driver-level pixel blend      */
/*    [gx_display_driver_horizontal_line_draw]                            */
/*                                          Driver-level horizontal line  */
/*                                            draw function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_generic_aliased_wide_line_draw                   */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_generic_aliased_filled_circle_draw(GX_DRAW_CONTEXT *context, GX_FIXED_VAL xcenter, GX_FIXED_VAL ycenter, GX_FIXED_VAL r)
{
/* The circle draw function is implemented from midpoint circle algorithm. */

GX_DISPLAY   *display;
GX_RECTANGLE *clip;
GX_BRUSH     *brush;
INT           x;
INT           y;
GX_POINT      point;
GX_BYTE       ysign[2] = {1, -1};
INT           index;
INT           error;
INT           yi;
GX_VALUE      xstart;
GX_VALUE      xend;
GX_UBYTE      alpha1;
GX_FIXED_VAL  xfraction;
GX_FIXED_VAL  yfraction;
INT           rr;
INT           half_shift;
void          (*blend_func)(GX_DRAW_CONTEXT *context,
                            INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

#if defined(GX_BRUSH_ALPHA_SUPPORT)
GX_UBYTE brush_alpha;

    brush_alpha = context->brush.alpha;

    if (brush_alpha == 0)
    {
        /* Nothing to draw here. */
        return;
    }
#endif

    display = context->display;
    clip = context->clip;
    brush = &context->brush;

    blend_func = display->driver_pixel_blend;

    if (blend_func == GX_NULL)
    {
        return;
    }

    xfraction = (xcenter & GX_FIXED_VAL_FRACTION_MASK);
    yfraction = (ycenter & GX_FIXED_VAL_FRACTION_MASK);

    r -= GX_FIXED_VAL_HALF;

    if (xfraction)
    {
        x = GX_FIXED_VAL_ONE - xfraction;
    }
    else
    {
        x = 0;
    }
    y = GX_FIXED_VAL_RND_UP(r + yfraction);
    y = GX_FIXED_VAL_MAKE(y) - yfraction;

    half_shift = (GX_FIXED_VAL_SHIFT >> 1);
    rr = (r >> half_shift) * (r >> half_shift);

    while (x < y)
    {
        yi = rr - ((x >> half_shift) * (x >> half_shift));

        error = (INT)_gx_utility_math_sqrt((UINT)yi);
        error <<= 8;
        error >>= half_shift;
        error = GX_FIXED_VAL_TO_INT((y << 8)) - error;

        while (error >= 255)
        {
            error -= 255;
            y -= GX_FIXED_VAL_ONE;
        }

        if (x > y)
        {
            break;
        }

        alpha1 = (GX_UBYTE)(255 - error);
#if defined(GX_BRUSH_ALPHA_SUPPORT)
        alpha1 = (GX_UBYTE)(alpha1 * brush_alpha / 255);
#endif

        for (index = 0; index < 2; index++)
        {
            point.x = (GX_VALUE)GX_FIXED_VAL_TO_INT(xcenter + x);
            point.y = (GX_VALUE)GX_FIXED_VAL_TO_INT(ycenter + y * ysign[index]);

            xend = point.x;

            /* draw pixel(x, y). */
            if (clip->contain_point_(point))
            {
                blend_func(context, point.x, point.y, brush->line_color, alpha1);
            }

            point.x = (GX_VALUE)GX_FIXED_VAL_TO_INT(xcenter - x);

            xstart = point.x;

            yi = GX_FIXED_VAL_TO_INT(ycenter + (y - GX_FIXED_VAL_ONE) * ysign[index]);

            if (xstart < clip->left)
            {
                xstart = clip->left;
            }

            if (xend > clip->right)
            {
                xend = clip->right;
            }

            if (xstart <= xend &&
                yi >= clip->top &&
                yi <= clip->bottom)
            {
                display->driver_horizontal_line_draw(context, xstart, xend, yi, 1, brush->line_color);
            }

            if (x)
            {
                /* draw pixel(-x, y). */
                if (clip->contain_point_(point))
                {
                    blend_func(context, point.x, point.y, brush->line_color, alpha1);
                }
            }
        }

        x += GX_FIXED_VAL_ONE;
    }

    x = GX_FIXED_VAL_RND_UP(r + xfraction);
    x = GX_FIXED_VAL_MAKE(x) - xfraction;

    if (yfraction)
    {
        y = yfraction;
    }
    else
    {
        y = 0;
    }

    while (x > y)
    {
        yi = rr - ((y >> half_shift) * (y >> half_shift));

        error = (INT)_gx_utility_math_sqrt((UINT)yi);
        error <<= 8;
        error >>= half_shift;
        error = GX_FIXED_VAL_TO_INT((x << 8)) - error;

        while (error >= 255)
        {
            error -= 255;
            x -= GX_FIXED_VAL_ONE;
        }

        if (x < y)
        {
            break;
        }

        alpha1 = (GX_UBYTE)(255 - error);
#if defined(GX_BRUSH_ALPHA_SUPPORT)
        alpha1 = (GX_UBYTE)(alpha1 * brush_alpha / 255);
#endif

        for (index = 0; index < 2; index++)
        {
            point.x = (GX_VALUE)GX_FIXED_VAL_TO_INT(xcenter + x);
            point.y = (GX_VALUE)GX_FIXED_VAL_TO_INT(ycenter + y * ysign[index]);

            xend = (GX_VALUE)(point.x - 1);

            /* draw pixel(x, y). */
            if (clip->contain_point_(point))
            {
                blend_func(context, point.x, point.y, brush->line_color, alpha1);
            }


            point.x = (GX_VALUE)GX_FIXED_VAL_TO_INT(xcenter - x);
            xstart = (GX_VALUE)(point.x + 1);

            if (xstart < clip->left)
            {
                xstart = clip->left;
            }

            if (xend > clip->right)
            {
                xend = clip->right;
            }

            if (xstart <= xend &&
                point.y >= clip->top &&
                point.y <= clip->bottom)
            {
                display->driver_horizontal_line_draw(context, xstart, xend, point.y, 1, brush->line_color);
            }

            /* draw pixel(-x, y). */
            if (clip->contain_point_(point))
            {
                blend_func(context, point.x, point.y, brush->line_color, alpha1);
            }
        }

        y += GX_FIXED_VAL_ONE;
    }
}

