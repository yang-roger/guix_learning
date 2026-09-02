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
/*    _gx_display_driver_generic_aliased_line_draw                        */
/*                                                           6.1          */
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
void _gx_display_driver_generic_aliased_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend)
{
GX_BRUSH     *brush = &context->brush;
GX_COLOR      line_color = brush->line_color;
GX_RECTANGLE *clip = context->clip;

void          (*blend_func)(GX_DRAW_CONTEXT *context,
                            INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

INT      dx = GX_ABS(xend - xstart);
INT      dy = GX_ABS(yend - ystart);
GX_POINT point;
GX_POINT point2;
INT      curx;
INT      cury;
INT      decision;
INT      nextx;
INT      nexty;
INT      back_alpha;
INT      fore_alpha;
INT      x_sign;
INT      y_sign;
GX_UBYTE alpha;

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

    if (!(context->display->driver_pixel_blend))
    {
        context->display->driver_simple_line_draw(context, xstart, ystart, xend, yend);
        return;
    }
    blend_func = context->display->driver_pixel_blend;

    /* special case of 1x1 pixel line:*/

    if (dx == 1 && dy == 1)
    {
        point.x = (GX_VALUE)xstart;
        point.y = (GX_VALUE)ystart;

        if (clip->contain_point_(point))
        {
            alpha = 0xc0;
#if defined(GX_BRUSH_ALPHA_SUPPORT)
            alpha = (GX_UBYTE)(brush_alpha * alpha / 255);
#endif
            blend_func(context, xstart, ystart, line_color, alpha);
        }
        point.x = (GX_VALUE)xend;
        point.y = (GX_VALUE)yend;

        if (clip->contain_point_(point))
        {
            alpha = 0x40;
#if defined(GX_BRUSH_ALPHA_SUPPORT)
            alpha = (GX_UBYTE)(brush_alpha * alpha / 255);
#endif
            blend_func(context, xend, yend, line_color, alpha);
        }
        point.x = (GX_VALUE)xstart;
        point.y = (GX_VALUE)yend;

        if (clip->contain_point_(point))
        {
            alpha = 0x40;
#if defined(GX_BRUSH_ALPHA_SUPPORT)
            alpha = (GX_UBYTE)(brush_alpha * alpha / 255);
#endif
            blend_func(context, xstart, yend, line_color, alpha);
        }
        point.x = (GX_VALUE)xend;
        point.y = (GX_VALUE)ystart;

        if (clip->contain_point_(point))
        {
            alpha = 0xc0;
#if defined(GX_BRUSH_ALPHA_SUPPORT)
            alpha = (GX_UBYTE)(brush_alpha * alpha / 255);
#endif
            blend_func(context, xend, ystart, line_color, alpha);
        }
        return;
    }

    if (((dx >= dy && (xstart > xend)) || ((dy > dx) && ystart > yend)))
    {
        GX_SWAP_VALS(xend, xstart);
        GX_SWAP_VALS(yend, ystart);
    }

    x_sign = ((INT)xend - (INT)xstart) / dx;
    y_sign = ((INT)yend - (INT)ystart) / dy;

    point.x = (GX_VALUE)xstart;
    point.y = (GX_VALUE)ystart;
    point2.x = (GX_VALUE)xend;
    point2.y = (GX_VALUE)yend;

    if (clip->contain_point_(point) &&
        clip->contain_point_(point2))
    {
        /* both endpoints are inside clip rectangle. We don't need to clip
           inside the inner loop */

        if (dx >= dy)
        {
            /* draw anti-aliased line along x axis, no clipping */
            for (curx = xstart, cury = ystart, nextx = xend, nexty = yend,
                 decision = 0; curx <= nextx; curx++, nextx--,
                 decision += dy)
            {
                if (decision >= dx)
                {
                    decision -= dx;
                    cury += y_sign;
                    nexty -= y_sign;
                }

                back_alpha = ((decision << 8) / dx);
                fore_alpha = 255 - back_alpha;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
                fore_alpha = (GX_UBYTE)(brush_alpha * fore_alpha / 255);
                back_alpha = (GX_UBYTE)(brush_alpha * back_alpha / 255);
#endif
                blend_func(context, curx, cury, line_color, (GX_UBYTE)fore_alpha);
                blend_func(context, curx, cury + y_sign, line_color, (GX_UBYTE)back_alpha);
                if (curx != nextx)
                {
                    blend_func(context, nextx, nexty, line_color, (GX_UBYTE)fore_alpha);
                    blend_func(context, nextx, nexty - y_sign, line_color, (GX_UBYTE)back_alpha);
                }
            }
        }
        else
        {
            /* draw anti-aliased line along y axis, no clipping */
            for (curx = xstart, cury = ystart, nextx = xend, nexty = yend,
                 decision = 0; cury <= nexty; cury++, nexty--,
                 decision += dx)
            {
                if (decision >= dy)
                {
                    decision -= dy;
                    curx += x_sign;
                    nextx -= x_sign;
                }

                back_alpha = ((decision << 8) / dy);
                fore_alpha = 255 - back_alpha;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
                fore_alpha = (GX_UBYTE)(brush_alpha * fore_alpha / 255);
                back_alpha = (GX_UBYTE)(brush_alpha * back_alpha / 255);
#endif
                blend_func(context, curx, cury, line_color, (GX_UBYTE)fore_alpha);
                blend_func(context, curx + x_sign, cury, line_color,  (GX_UBYTE)back_alpha);
                if (cury != nexty)
                {
                    blend_func(context, nextx, nexty, line_color, (GX_UBYTE)fore_alpha);
                    blend_func(context, nextx - x_sign, nexty, line_color,  (GX_UBYTE)back_alpha);
                }
            }
        }
    }
    else
    {

        if (dx >= dy)
        {
            /* draw anti-aliased line along x axis, clipping in the inner loop */
            for (curx = xstart, cury = ystart, nextx = xend, nexty = yend,
                 decision = 0; curx <= nextx; curx++, nextx--,
                 decision += dy)
            {
                if (decision >= dx)
                {
                    decision -= dx;
                    cury += y_sign;
                    nexty -= y_sign;
                }

                back_alpha = ((decision << 8) / dx);
                fore_alpha = 255 - back_alpha;

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
                if (curx == nextx)
                {
                    continue;
                }
                if (nextx >= clip->left &&
                    nextx <= clip->right &&
                    nexty >= clip->top &&
                    nexty <= clip->bottom)
                {
                    blend_func(context, nextx, nexty, line_color, (GX_UBYTE)fore_alpha);
                }
                if (nextx >= clip->left &&
                    nextx <= clip->right &&
                    (nexty - y_sign) >= clip->top &&
                    (nexty - y_sign) <= clip->bottom)
                {
                    blend_func(context, nextx, nexty - y_sign, line_color, (GX_UBYTE)back_alpha);
                }
            }
        }
        else
        {
            /* draw anti-aliased line along y axis, clipping in the inner loop */
            for (curx = xstart, cury = ystart, nextx = xend, nexty = yend,
                 decision = 0; cury <= nexty; cury++, nexty--,
                 decision += dx)
            {
                if (decision >= dy)
                {
                    decision -= dy;
                    curx += x_sign;
                    nextx -= x_sign;
                }

                back_alpha = ((decision << 8) / dy);
                fore_alpha = 255 - back_alpha;

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

                if (cury == nexty)
                {
                    continue;
                }
                if (nextx >= clip->left &&
                    nextx <= clip->right &&
                    nexty >= clip->top &&
                    nexty <= clip->bottom)
                {
                    blend_func(context, nextx, nexty, line_color, (GX_UBYTE)fore_alpha);
                }
                if ((nextx - x_sign) >= clip->left &&
                    (nextx - x_sign) <= clip->right &&
                    nexty >= clip->top &&
                    nexty <= clip->bottom)
                {
                    blend_func(context, nextx - x_sign, nexty, line_color, (GX_UBYTE)back_alpha);
                }
            }
        }
    }
}

