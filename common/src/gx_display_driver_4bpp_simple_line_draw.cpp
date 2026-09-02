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
/*    _gx_display_driver_4bpp_simple_line_draw                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Simple line draw function for the 4bpp display driver.              */
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
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_4bpp_simple_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend)
{

/**************************************************************************/
/* Integer non-aliased line draw. This algorithm is taken directly from   */
/* Roger Stevens "The C++ graphics programming handbook" pages 281-282.   */
/* Removed the pattern test and renamed the variables and converted from  */
/* C++ to C. Also modified the algorithm to draw from both ends in to     */
/* the middle, instead of drawing from one end to the other.              */
/**************************************************************************/

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
INT           pos_start;
INT           pos_end;
GX_UBYTE      start_mask;
GX_UBYTE      end_mask;
GX_UBYTE     *start_address;
GX_UBYTE     *put;
GX_UBYTE     *next_put;
GX_BOOL       clipped = GX_TRUE;
INT           dx = GX_ABS(xend - xstart);
INT           dy = GX_ABS(yend - ystart);

GX_RECTANGLE *clip = context->clip;
GX_UBYTE      linecolor = context->brush.line_color & 0x0f;
GX_UBYTE      color = linecolor;
INT           stride;

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

    stride = (context->pitch + 1) >> 1;

    start_address = (GX_UBYTE *)(context->memory);
    pos_start = ystart * (stride << 1) + xstart;
    pos_end = yend * (stride << 1) + xend;

    end_point.x = (GX_VALUE)xstart;
    end_point.y = (GX_VALUE)ystart;

    if (_gx_utility_rectangle_point_detect(clip, end_point))
    {
        end_point.x = (GX_VALUE)xend;
        end_point.y = (GX_VALUE)yend;

        if (_gx_utility_rectangle_point_detect(clip, end_point))
        {
            clipped = GX_FALSE;
        }
    }

    if (clipped)
    {
        /* here if we must do clipping in the inner loop, because one
           or both of the end points are outside clipping rectangle */

        /* Calculate the middle point of the line.  */
        mid_point.x = (GX_VALUE)((xend + xstart) >> 1);
        mid_point.y = (GX_VALUE)((yend + ystart) >> 1);

        /* Judge the clip in which side.  */
        if (_gx_utility_rectangle_point_detect(clip, mid_point))
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

                        pos_start += y_increment;
                    }

                    if (curx >= clip->left &&
                        cury >= clip->top &&
                        cury <= clip->bottom)
                    {
                        break;
                    }
                    pos_start++;
                }
                for (; curx <= mid_point.x;
                     curx++, decision += dy)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        cury += y_sign;
                        pos_start += y_increment;
                    }
                    put = start_address;
                    put += pos_start >> 1;
                    if (pos_start & 0x01)
                    {
                        start_mask = 0x0f;
                        color = linecolor;
                    }
                    else
                    {
                        start_mask = 0xf0;
                        color = (GX_UBYTE)(linecolor << 4);
                    }
                    *put = (GX_UBYTE)((GX_UBYTE)((*put) & (~start_mask)) | color);
                    pos_start++;
                }
                for (nextx = xend, nexty = yend, decision = (dx >> 1); nextx > mid_point.x;
                     nextx--, decision += dy)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        nexty -= y_sign;
                        pos_end -= y_increment;
                    }
                    if (nextx <= clip->right &&
                        nexty >= clip->top &&
                        nexty <= clip->bottom)
                    {
                        break;
                    }
                    pos_end--;
                }

                for (; nextx > mid_point.x;
                     nextx--, decision += dy)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        nexty -= y_sign;
                        pos_end -= y_increment;
                    }
                    next_put = start_address;
                    next_put += pos_end >> 1;
                    if (pos_end & 0x01)
                    {
                        end_mask = 0x0f;
                        color = linecolor;
                    }
                    else
                    {
                        end_mask = 0xf0;
                        color = (GX_UBYTE)(linecolor << 4);
                    }
                    *next_put = (GX_UBYTE)((GX_UBYTE)((*next_put) & (~end_mask)) | color);
                    pos_end--;
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
                        pos_end -= x_sign;
                    }
                    if (nextx >= clip->left &&
                        nextx <= clip->right &&
                        nexty <= clip->bottom)
                    {
                        break;
                    }
                    pos_end -= context->pitch;
                }

                for (; nexty > mid_point.y;
                     nexty--, decision += dx)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        nextx -= x_sign;
                        pos_end -= x_sign;
                    }
                    next_put = start_address;
                    next_put += pos_end >> 1;
                    if (pos_end & 0x01)
                    {
                        end_mask = 0x0f;
                        color = linecolor;
                    }
                    else
                    {
                        end_mask = 0xf0;
                        color = (GX_UBYTE)(linecolor << 4);
                    }
                    *next_put = (GX_UBYTE)((GX_UBYTE)((*next_put) & (~end_mask)) | color);
                    pos_end -= context->pitch;
                }

                /* walk out the clipping point.  */
                for (curx = xstart, cury = ystart, decision = (dy >> 1); cury < mid_point.y;
                     cury++, decision += dx)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        curx += x_sign;
                        pos_start += x_sign;
                    }

                    if (curx >= clip->left &&
                        curx <= clip->right &&
                        cury >= clip->top)
                    {
                        break;
                    }
                    pos_start += context->pitch;
                }
                for (; cury <= mid_point.y;
                     cury++, decision += dx)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        curx += x_sign;
                        pos_start += x_sign;
                    }
                    put = start_address;
                    put += pos_start >> 1;
                    if (pos_start & 0x01)
                    {
                        start_mask = 0x0f;
                        color = linecolor;
                    }
                    else
                    {
                        start_mask = 0xf0;
                        color = (GX_UBYTE)(linecolor << 4);
                    }
                    *put = (GX_UBYTE)((GX_UBYTE)((*put) & (~start_mask)) | color);
                    pos_start += context->pitch;
                }
            }
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
                    pos_start = pos_end;
                }
                for (decision = (dx >> 1); steps > 0; curx += sign, decision += dy, steps--)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        cury += y_sign;
                        pos_start += y_increment;
                    }

                    if (curx >= clip->left &&
                        curx <= clip->right &&
                        cury >= clip->top &&
                        cury <= clip->bottom)
                    {
                        put = start_address;
                        put += pos_start >> 1;
                        if (pos_start & 0x01)
                        {
                            start_mask = 0x0f;
                            color = linecolor;
                        }
                        else
                        {
                            start_mask = 0xf0;
                            color = (GX_UBYTE)(linecolor << 4);
                        }
                        *put = (GX_UBYTE)((GX_UBYTE)((*put) & (~start_mask)) | color);
                    }
                    pos_start += sign;
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
                    pos_start = pos_end;
                }

                for (decision = (dy >> 1); steps > 0; cury += sign, decision += dx, steps--)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        curx += x_sign;
                        pos_start += x_sign;
                    }
                    if (curx >= clip->left &&
                        curx <= clip->right &&
                        cury >= clip->top &&
                        cury <= clip->bottom)
                    {
                        put = start_address;
                        put += pos_start >> 1;
                        if (pos_start & 0x01)
                        {
                            start_mask = 0x0f;
                            color = linecolor;
                        }
                        else
                        {
                            start_mask = 0xf0;
                            color = (GX_UBYTE)(linecolor << 4);
                        }
                        *put = (GX_UBYTE)((GX_UBYTE)((*put) & (~start_mask)) | color);
                    }
                    pos_start += y_increment;
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
            for (curx = xstart, cury = ystart, nextx = xend, nexty = yend,
                 decision = (dx >> 1); curx <= nextx; curx++, nextx--,
                 decision += dy)
            {

                if (decision >= dx)
                {
                    decision -= dx;
                    cury += y_sign;
                    nexty -= y_sign;

                    pos_start += y_increment;
                    pos_end -= y_increment;
                }
                put = start_address;
                put += pos_start >> 1;
                if (pos_start & 0x01)
                {
                    start_mask = 0x0f;
                    color = linecolor;
                }
                else
                {
                    start_mask = 0xf0;
                    color = (GX_UBYTE)(linecolor << 4);
                }
                *put = (GX_UBYTE)((GX_UBYTE)((*put) & (~start_mask)) | color);
                next_put = start_address;
                next_put += pos_end >> 1;
                if (pos_end & 0x01)
                {
                    end_mask = 0x0f;
                    color = linecolor;
                }
                else
                {
                    end_mask = 0xf0;
                    color = (GX_UBYTE)(linecolor << 4);
                }
                *next_put = (GX_UBYTE)((GX_UBYTE)((*next_put) & (~end_mask)) | color);
                pos_start++;
                pos_end--;
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

                    pos_start += x_sign;
                    pos_end -= x_sign;
                }
                put = start_address;
                put += pos_start >> 1;
                if (pos_start & 0x01)
                {
                    start_mask = 0x0f;
                    color = linecolor;
                }
                else
                {
                    start_mask = 0xf0;
                    color = (GX_UBYTE)(linecolor << 4);
                }
                *put = (GX_UBYTE)((GX_UBYTE)((*put) & (~start_mask)) | color);
                next_put = start_address;
                next_put += pos_end >> 1;
                if (pos_end & 0x01)
                {
                    end_mask = 0x0f;
                    color = linecolor;
                }
                else
                {
                    end_mask = 0xf0;
                    color = (GX_UBYTE)(linecolor << 4);
                }
                *next_put = (GX_UBYTE)((GX_UBYTE)((*next_put) & (~end_mask)) | color);

                pos_start += context->pitch;
                pos_end -= context->pitch;
            }
        }
    }
}

