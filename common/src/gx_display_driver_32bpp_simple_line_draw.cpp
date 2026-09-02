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
#define PIXEL_WRITE(loc, val) (*(loc) = (val))




#include "gx_display.h"

#include "gx_utility.h"
#include "gx_context.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_simple_line_draw                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 32bpp color format line draw function.                      */
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
/*    _gx_display_driver_simple_line_alpha_draw                           */
/*                                          Basic display driver line     */
/*                                            alpha draw function         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_32bpp_simple_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend)
{
INT           curx;
INT           cury;
INT           x_sign;
INT           y_sign;
INT           decision;
INT           nextx;
INT           nexty;
INT           y_increment;
GX_POINT      end_point;
GX_POINT      mid_point;
GX_RECTANGLE  half_rectangle;
GX_RECTANGLE  half_over;
INT           sign;
INT           steps;

ULONG        *put;
ULONG        *next_put;

GX_BOOL       clipped = GX_TRUE;
INT           dx = GX_ABS(xend - xstart);
INT           dy = GX_ABS(yend - ystart);

GX_RECTANGLE *clip = context->clip;
GX_COLOR      linecolor = context->brush.line_color;
#if defined GX_BRUSH_ALPHA_SUPPORT
GX_UBYTE      alpha;

    alpha = context->brush.alpha;
    if (alpha == 0)
    {
        /* Nothing to drawn. Just return. */
        return;
    }
    if (alpha != 0xff)
    {
        _gx_display_driver_simple_line_alpha_draw(context, xstart, ystart, xend, yend, alpha);
        return;
    }
#endif

    if (((dx >= dy && (xstart > xend)) || ((dy > dx) && ystart > yend)))
    {
        GX_SWAP_VALS(xend, xstart);
        GX_SWAP_VALS(yend, ystart);
    }
    x_sign = (xend - xstart) / dx;
    y_sign = (yend - ystart) / dy;

    if (y_sign > 0)
    {
        y_increment = context->pitch;
    }
    else
    {
        y_increment = 0 - context->pitch;
    }

    put = (ULONG *)(context->memory) + ystart * context->pitch + xstart;
    next_put = (ULONG *)(context->memory) + yend * context->pitch + xend;


    end_point.x = (GX_VALUE)xstart;
    end_point.y = (GX_VALUE)ystart;

    if (clip->contain_point_(end_point))
    {
        end_point.x = (GX_VALUE)xend;
        end_point.y = (GX_VALUE)yend;

        if (clip->contain_point_(end_point))
        {
            clipped = GX_FALSE;
        }
    }

    if (clipped)
    {
        /* here if we must do clipping in the inner loop, because one
        or both of the end points are outside clipping rectangle */

        /* Calculate the middle point of the line.  */
        mid_point.x = (GX_VALUE)(xend + xstart) >> 1;
        mid_point.y = (GX_VALUE)(yend + ystart) >> 1;

        /* Judge the clip in which side.  */
        if (clip->contain_point_(mid_point))
        {

            /* the clip in two sides.  */
            if (dx >= dy)
            {
                /* walk out the clipping point.  */
                for (curx = xstart, cury = ystart, decision = (dx >> 1); curx < mid_point.x;
                    curx++, decision += dy)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        cury += y_sign;
                        put += y_increment;
                    }

                    if (curx >= clip->left &&
                        cury >= clip->top &&
                        cury <= clip->bottom)
                    {
                        break;
                    }
                    put++;
                }
                for (; curx <= mid_point.x;
                    curx++, decision += dy)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        cury += y_sign;
                        put += y_increment;
                    }
                    PIXEL_WRITE(put, linecolor);
                    put++;
                }
                for (nextx = xend, nexty = yend, decision = (dx >> 1); nextx > mid_point.x;
                    nextx--, decision += dy)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        nexty -= y_sign;
                        next_put -= y_increment;
                    }
                    if (nextx <= clip->right &&
                        nexty >= clip->top &&
                        nexty <= clip->bottom)
                    {
                        break;
                    }
                    next_put--;
                }

                for (; nextx > mid_point.x;
                    nextx--, decision += dy)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        nexty -= y_sign;
                        next_put -= y_increment;
                    }
                    PIXEL_WRITE(next_put, linecolor);
                    next_put--;
                }
            }
            else
            {
                for (nextx = xend, nexty = yend, decision = (dy >> 1); nexty > mid_point.y;
                    nexty--, decision += dx)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        nextx -= x_sign;
                        next_put -= x_sign;
                    }
                    if (nextx >= clip->left &&
                        nextx <= clip->right &&
                        nexty <= clip->bottom)
                    {
                        break;
                    }
                    next_put -= context->pitch;
                }

                for (; nexty > mid_point.y;
                    nexty--, decision += dx)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        nextx -= x_sign;
                        next_put -= x_sign;
                    }
                    PIXEL_WRITE(next_put, linecolor);
                    next_put -= context->pitch;
                }

                /* walk out the clipping point.  */
                for (curx = xstart, cury = ystart, decision = (dy >> 1); cury < mid_point.y;
                    cury++, decision += dx)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        curx += x_sign;
                        put += x_sign;
                    }

                    if (curx >= clip->left &&
                        curx <= clip->right &&
                        cury >= clip->top)
                    {
                        break;
                    }
                    put += context->pitch;
                }
                for (; cury <= mid_point.y;
                    cury++, decision += dx)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        curx += x_sign;
                        put += x_sign;
                    }
                    PIXEL_WRITE(put, linecolor);
                    put += context->pitch;
                }
            }   /**/
                /*context->display->driver_pixel_write(context, mid_point.x,mid_point.y, GX_COLOR_RED);*/
        }
        else
        {
            /* The clip stay at one side.  */
            if (dx >= dy)
            {
                half_rectangle.left = (GX_VALUE)xstart;
                half_rectangle.right = mid_point.x;
                if (y_sign == 1)
                {
                    half_rectangle.top = (GX_VALUE)ystart;
                    half_rectangle.bottom = mid_point.y;
                }
                else
                {
                    half_rectangle.top = mid_point.y;
                    half_rectangle.bottom = (GX_VALUE)ystart;
                }

                if (gx_rectangle_intersect_(*clip, half_rectangle, &half_over))
                {
                    curx = xstart;
                    cury = ystart;
                    steps = mid_point.x - curx + 1;
                    sign = 1;
                }
                else
                {
                    curx = xend;
                    cury = yend;
                    steps = xend - mid_point.x;
                    sign = -1;
                    y_increment = 0 - y_increment;
                    y_sign = 0 - y_sign;
                    put = next_put;
                }
                for (decision = (dx >> 1); steps > 0; curx += sign, decision += dy, steps--)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        cury += y_sign;
                        put += y_increment;
                    }

                    if (curx >= clip->left &&
                        curx <= clip->right &&
                        cury >= clip->top &&
                        cury <= clip->bottom)
                    {
                        PIXEL_WRITE(put, linecolor);
                    }
                    put += sign;
                }
            }
            else
            {
                half_rectangle.top = (GX_VALUE)ystart;
                half_rectangle.bottom = mid_point.y;
                if (x_sign == 1)
                {
                    half_rectangle.right = mid_point.x;
                    half_rectangle.left = (GX_VALUE)xstart;
                }
                else
                {
                    half_rectangle.right = (GX_VALUE)xstart;
                    half_rectangle.left = mid_point.x;
                }

                if (gx_rectangle_intersect_(*clip, half_rectangle, &half_over))
                {
                    curx = xstart;
                    cury = ystart;
                    steps = mid_point.y - cury + 1;
                    y_increment = context->pitch;
                    sign = 1;
                }
                else
                {
                    curx = xend;
                    cury = yend;
                    steps = yend - mid_point.y;
                    sign = -1;
                    y_increment = 0 - context->pitch;
                    x_sign = 0 - x_sign;
                    put = next_put;
                }

                for (decision = (dy >> 1); steps > 0; cury += sign, decision += dx, steps--)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        curx += x_sign;
                        put += x_sign;
                    }
                    if (curx >= clip->left &&
                        curx <= clip->right &&
                        cury >= clip->top &&
                        cury <= clip->bottom)
                    {
                        PIXEL_WRITE(put, linecolor);
                    }
                    put += y_increment;
                }
            }
        }
    }
    else
    {
        /* here if both line ends lie within clipping rectangle, we can
        run a faster inner loop */
        if (dx >= dy)
        {
            put = (ULONG *)(context->memory) + ystart * context->pitch + xstart;
            next_put = (ULONG *)(context->memory) + yend * context->pitch + xend;

            for (curx = xstart, cury = ystart, nextx = xend, nexty = yend,
                decision = (dx >> 1); curx <= nextx; curx++, nextx--,
                decision += dy)
            {

                if (decision >= dx)
                {
                    decision -= dx;
                    cury += y_sign;
                    nexty -= y_sign;

                    put += y_increment;
                    next_put -= y_increment;
                }
                PIXEL_WRITE(put, linecolor);
                PIXEL_WRITE(next_put, linecolor);

                put++;
                next_put--;
            }
        }
        else
        {

            for (curx = xstart, cury = ystart, nextx = xend, nexty = yend,
                decision = (dy >> 1); cury <= nexty; cury++, nexty--,
                decision += dx)
            {
                if (decision >= dy)
                {
                    decision -= dy;
                    curx += x_sign;
                    nextx -= x_sign;

                    put += x_sign;
                    next_put -= x_sign;
                }
                PIXEL_WRITE(put, linecolor);
                PIXEL_WRITE(next_put, linecolor);

                put += context->pitch;
                next_put -= context->pitch;
            }
        }
    }
}
