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


#define PIXEL_WRITE(loc, val) (*(loc) = ((GX_UBYTE)val))



#include "gx_display.h"

#include "gx_utility.h"
#include "gx_context.h"
#include "gx_canvas.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_rotated_simple_line_draw                    */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 8bpp color format rotated line draw function.               */
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
/*    GX_ABS                                Compute the absolute value    */
/*    GX_SWAP_VALUE                         Swap two values               */
/*    [PIXEL_WRITE]                         Driver level pixel write      */
/*                                            routine                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_8bpp_rotated_simple_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend)
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

GX_UBYTE     *put;
GX_UBYTE     *next_put;

GX_BOOL       clipped = GX_TRUE;
INT           dx;
INT           dy;

GX_RECTANGLE *clip = context->clip;
GX_COLOR      linecolor = context->brush.line_color;
GX_RECTANGLE  rotated_clip;

    GX_SWAP_VALS(xstart, ystart);
    GX_SWAP_VALS(xend, yend);
    clip = context->clip;

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        ystart = context->canvas->x_resolution - ystart - 1;
        yend = context->canvas->x_resolution - yend - 1;

        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - clip->right - 1);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - clip->left - 1);
    }
    else
    {
        xstart = context->canvas->y_resolution - xstart - 1;
        xend = context->canvas->y_resolution - xend - 1;

        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - clip->bottom - 1);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - clip->top - 1);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
    }

    dx = GX_ABS(xend - xstart);
    dy = GX_ABS(yend - ystart);

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

    put = (GX_UBYTE *)(context->memory) + ystart * context->pitch + xstart;
    next_put = (GX_UBYTE *)(context->memory) + yend * context->pitch + xend;


    end_point.x = (GX_VALUE)xstart;
    end_point.y = (GX_VALUE)ystart;

    if (_gx_utility_rectangle_point_detect(&rotated_clip, end_point))
    {
        end_point.x = (GX_VALUE)xend;
        end_point.y = (GX_VALUE)yend;

        if (_gx_utility_rectangle_point_detect(&rotated_clip, end_point))
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

        /* Judge the &rotated_clip in which side.  */
        if (_gx_utility_rectangle_point_detect(&rotated_clip, mid_point))
        {

            /* the &rotated_clip in two sides.  */
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

                    if (curx >= rotated_clip.left &&
                        cury >= rotated_clip.top &&
                        cury <= rotated_clip.bottom)
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
                    if (nextx <= rotated_clip.right &&
                        nexty >= rotated_clip.top &&
                        nexty <= rotated_clip.bottom)
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
                    if (nextx >= rotated_clip.left &&
                        nextx <= rotated_clip.right &&
                        nexty <= rotated_clip.bottom)
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

                    if (curx >= rotated_clip.left &&
                        curx <= rotated_clip.right &&
                        cury >= rotated_clip.top)
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
            }
        }
        else
        {
            /* The &rotated_clip stay at one side.  */
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

                if (gx_rectangle_intersect_(rotated_clip, half_rectangle, &half_over))
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

                    if (curx >= rotated_clip.left &&
                        curx <= rotated_clip.right &&
                        cury >= rotated_clip.top &&
                        cury <= rotated_clip.bottom)
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

                if (gx_rectangle_intersect_(rotated_clip, half_rectangle, &half_over))
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
                    if (curx >= rotated_clip.left &&
                        curx <= rotated_clip.right &&
                        cury >= rotated_clip.top &&
                        cury <= rotated_clip.bottom)
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
            put = (GX_UBYTE *)(context->memory) + ystart * context->pitch + xstart;
            next_put = (GX_UBYTE *)(context->memory) + yend * context->pitch + xend;

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

