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
/*    _gx_display_driver_generic_aliased_fixed_point_line_draw            */
/*                                                                        */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    General anti-aliasing line drawing function.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xstart                                x-coord of endpoint           */
/*    ystart                                y-coord of endpoint           */
/*    xend                                  x-coord of endpoint           */
/*    yend                                  y-coord of endpoint           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_pixel_blend]       Display driver blend function */
/*    _gx_utility_rectangle_point_detect    Detect whether a pixel is     */
/*                                            inside rectangle            */
/*    GX_SWAP_VALS                          Swap two values               */
/*    [gx_display_driver_simple_line_draw]  Basic display driver simple   */
/*                                            line draw function          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_generic_aliased_fixed_point_line_draw(GX_DRAW_CONTEXT *context,
                                                              GX_FIXED_VAL xstart, GX_FIXED_VAL ystart,
                                                              GX_FIXED_VAL xend, GX_FIXED_VAL yend)
{
GX_BRUSH     *brush = &context->brush;
GX_COLOR      line_color = brush->line_color;
GX_RECTANGLE *clip = context->clip;

void          (*blend_func)(GX_DRAW_CONTEXT *context,
                            INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

GX_FIXED_VAL dx = GX_ABS(xend - xstart);
GX_FIXED_VAL dy = GX_ABS(yend - ystart);
GX_FIXED_VAL x_fraction;
GX_FIXED_VAL y_fraction;
GX_POINT     point;
GX_POINT     point2;
INT          curx = 0;
INT          cury = 0;
INT          decision;
INT          back_alpha;
INT          fore_alpha;
INT          x_sign;
INT          y_sign;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
GX_UBYTE brush_alpha;

    brush_alpha = context->brush.alpha;

    if (brush_alpha == 0)
    {
        /* Nothing to draw here. */
        return;
    }
#endif

    if (!dx || !dy)
    {
        return;
    }

    blend_func = context->display->driver_pixel_blend;

    if (blend_func == GX_NULL)
    {
        return;
    }

    if (((dx >= dy && (xstart > xend)) || ((dy > dx) && ystart > yend)))
    {
        GX_SWAP_VALS(xend, xstart);
        GX_SWAP_VALS(yend, ystart);
    }

    x_sign = (xend - xstart) / dx;
    y_sign = (yend - ystart) / dy;

    x_fraction = (xstart & GX_FIXED_VAL_FRACTION_MASK);
    y_fraction = (ystart & GX_FIXED_VAL_FRACTION_MASK);

    if (dx >= dy)
    {
        point.x = (GX_VALUE)GX_FIXED_VAL_TO_INT(xstart);
        point2.x = (GX_VALUE)GX_FIXED_VAL_RND_UP(xend);

        if (y_sign < 0)
        {
            point.y = (GX_VALUE)GX_FIXED_VAL_RND_UP(ystart);
            point2.y = (GX_VALUE)GX_FIXED_VAL_TO_INT(yend);

            if (y_fraction)
            {
                y_fraction = GX_FIXED_VAL_ONE - y_fraction;
            }
        }
        else
        {
            point.y = (GX_VALUE)GX_FIXED_VAL_TO_INT(ystart);
            point2.y = (GX_VALUE)GX_FIXED_VAL_RND_UP(yend);
        }

        decision = (y_fraction * dx - x_fraction * dy) / GX_FIXED_VAL_ONE;

        cury = point.y;

        if (decision < 0)
        {
            decision += dx;
            cury -= y_sign;
        }
    }
    else
    {
        point.y = (GX_VALUE)GX_FIXED_VAL_TO_INT(ystart);
        point2.y = (GX_VALUE)GX_FIXED_VAL_RND_UP(yend);

        if (x_sign < 0)
        {
            point.x = (GX_VALUE)GX_FIXED_VAL_RND_UP(xstart);
            point2.x = (GX_VALUE)GX_FIXED_VAL_TO_INT(xend);

            if (x_fraction)
            {
                x_fraction = GX_FIXED_VAL_FRACTION_MASK - x_fraction;
            }
        }
        else
        {
            point.x = (GX_VALUE)GX_FIXED_VAL_TO_INT(xstart);
            point2.x = (GX_VALUE)GX_FIXED_VAL_RND_UP(xend);
        }

        decision = (x_fraction * dy - y_fraction * dx) / GX_FIXED_VAL_ONE;

        curx = point.x;

        if (decision < 0)
        {
            decision += dy;
            curx -= x_sign;
        }
    }

    if (clip->contain_point_(point) &&
        clip->contain_point_(point2))
    {
        /* both endpoints are inside clip rectangle. We don't need to clip
           inside the inner loop */
        if (dx >= dy)
        {
            /* handle end points separately. */

            /* draw anti-aliased line along x axis, no clipping */
            for (curx = point.x; curx <= point2.x; curx++, decision += dy)
            {
                if (decision >= dx)
                {
                    decision -= dx;
                    cury += y_sign;
                }

                back_alpha = ((decision << 8) / dx);

                fore_alpha = 255 - back_alpha;

                if ((curx == point.x || curx == point2.x) && x_fraction)
                {
                    fore_alpha = fore_alpha * (GX_FIXED_VAL_ONE - x_fraction) / GX_FIXED_VAL_ONE;
                    back_alpha = back_alpha * (GX_FIXED_VAL_ONE - x_fraction) / GX_FIXED_VAL_ONE;
                    x_fraction = GX_FIXED_VAL_ONE - (xend & GX_FIXED_VAL_FRACTION_MASK);
                }

#if defined(GX_BRUSH_ALPHA_SUPPORT)
                fore_alpha = (GX_UBYTE)(brush_alpha * fore_alpha / 255);
                back_alpha = (GX_UBYTE)(brush_alpha * back_alpha / 255);
#endif
                blend_func(context, curx, cury, line_color, (GX_UBYTE)fore_alpha);
                blend_func(context, curx, cury + y_sign, line_color, (GX_UBYTE)back_alpha);
            }
        }
        else
        {
            /* draw anti-aliased line along y axis, no clipping */
            for (cury = point.y; cury <= point2.y; cury++, decision += dx)
            {
                if (decision >= dy)
                {
                    decision -= dy;
                    curx += x_sign;
                }

                back_alpha = ((decision << 8) / dy);
                fore_alpha = 255 - back_alpha;

                if ((cury == point.y || cury == point2.y) && y_fraction)
                {
                    fore_alpha = fore_alpha * (GX_FIXED_VAL_ONE - y_fraction) / GX_FIXED_VAL_ONE;
                    back_alpha = back_alpha * (GX_FIXED_VAL_ONE - y_fraction) / GX_FIXED_VAL_ONE;
                    y_fraction = GX_FIXED_VAL_ONE - (yend & GX_FIXED_VAL_FRACTION_MASK);
                }

#if defined(GX_BRUSH_ALPHA_SUPPORT)
                fore_alpha = (GX_UBYTE)(brush_alpha * fore_alpha / 255);
                back_alpha = (GX_UBYTE)(brush_alpha * back_alpha / 255);
#endif
                blend_func(context, curx, cury, line_color, (GX_UBYTE)fore_alpha);
                blend_func(context, curx + x_sign, cury, line_color, (GX_UBYTE)back_alpha);
            }
        }
    }
    else
    {
        if (dx >= dy)
        {
            /* draw anti-aliased line along x axis, clipping in the inner loop */
            for (curx = point.x; curx <= point2.x; curx++, decision += dy)
            {
                if (decision >= dx)
                {
                    decision -= dx;
                    cury += y_sign;
                }

                back_alpha = ((decision << 8) / dx);
                fore_alpha = 255 - back_alpha;

                if ((curx == point.x || curx == point2.x) && x_fraction)
                {
                    fore_alpha = fore_alpha * (GX_FIXED_VAL_ONE - x_fraction) / GX_FIXED_VAL_ONE;
                    back_alpha = back_alpha * (GX_FIXED_VAL_ONE - x_fraction) / GX_FIXED_VAL_ONE;
                    x_fraction = GX_FIXED_VAL_ONE - (xend & GX_FIXED_VAL_FRACTION_MASK);
                }

#if defined(GX_BRUSH_ALPHA_SUPPORT)
                fore_alpha = (GX_UBYTE)(brush_alpha * fore_alpha / 255);
                back_alpha = (GX_UBYTE)(brush_alpha * back_alpha / 255);
#endif
                if (curx >= clip->left &&
                    curx <= clip->right &&
                    cury >= clip->top &&
                    cury <= clip->bottom)
                {
                    blend_func(context, curx, cury, line_color, (GX_UBYTE)fore_alpha);
                }

                if (curx >= clip->left &&
                    curx <= clip->right &&
                    (cury + y_sign) >= clip->top &&
                    (cury + y_sign) <= clip->bottom)
                {
                    blend_func(context, curx, cury + y_sign, line_color, (GX_UBYTE)back_alpha);
                }
            }
        }
        else
        {
            for (cury = point.y; cury <= point2.y; cury++, decision += dx)
            {
                if (decision >= dy)
                {
                    decision -= dy;
                    curx += x_sign;
                }

                back_alpha = ((decision << 8) / dy);
                fore_alpha = 255 - back_alpha;

                if ((cury == point.y || cury == point2.y) && y_fraction)
                {
                    fore_alpha = fore_alpha * (GX_FIXED_VAL_ONE - y_fraction) / GX_FIXED_VAL_ONE;
                    back_alpha = back_alpha * (GX_FIXED_VAL_ONE - y_fraction) / GX_FIXED_VAL_ONE;
                    y_fraction = GX_FIXED_VAL_ONE - (yend & GX_FIXED_VAL_FRACTION_MASK);
                }

#if defined(GX_BRUSH_ALPHA_SUPPORT)
                fore_alpha = (GX_UBYTE)(brush_alpha * fore_alpha / 255);
                back_alpha = (GX_UBYTE)(brush_alpha * back_alpha / 255);
#endif
                if (curx >= clip->left &&
                    curx <= clip->right &&
                    cury >= clip->top &&
                    cury <= clip->bottom)
                {
                    blend_func(context, curx, cury, line_color, (GX_UBYTE)fore_alpha);
                }

                if ((curx + x_sign) >= clip->left &&
                    (curx + x_sign) <= clip->right &&
                    cury >= clip->top &&
                    cury <= clip->bottom)
                {
                    blend_func(context, curx + x_sign, cury, line_color, (GX_UBYTE)back_alpha);
                }
            }
        }
    }
}

