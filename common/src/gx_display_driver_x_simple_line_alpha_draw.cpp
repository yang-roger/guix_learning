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


#define PIXEL_WRITE(loc, val) (*(loc) = ((USHORT)val))




#include "gx_display.h"

#include "gx_utility.h"
#include "gx_context.h"


#if defined (GX_BRUSH_ALPHA_SUPPORT)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_simple_line_alpha_draw                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic driver function that handles drawing lines with brush       */
/*    alpha.                                                              */
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
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_simple_line_alpha_draw(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend, GX_UBYTE alpha)
{
INT           curx;
INT           cury;
INT           x_sign;
INT           y_sign;
INT           decision;
INT           nextx;
INT           nexty;
GX_POINT      end_point;
GX_POINT      mid_point;
GX_RECTANGLE  half_rectangle;
GX_RECTANGLE  half_over;
INT           sign;
INT           steps;
GX_BOOL       clipped = GX_TRUE;
INT           dx = GX_ABS(xend - xstart);
INT           dy = GX_ABS(yend - ystart);

GX_RECTANGLE *clip = context->clip;
GX_COLOR      linecolor = context->brush.line_color;
void        (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

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
        mid_point.x = (GX_VALUE)((xend + xstart) >> 1);
        mid_point.y = (GX_VALUE)((yend + ystart) >> 1);

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
                    }

                    if (curx >= clip->left &&
                        cury >= clip->top &&
                        cury <= clip->bottom)
                    {
                        break;
                    }
                }
                for (; curx <= mid_point.x;
                    curx++, decision += dy)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        cury += y_sign;
                    }

                    blend_func(context, curx, cury, linecolor, alpha);
                }
                for (nextx = xend, nexty = yend, decision = (dx >> 1); nextx > mid_point.x;
                    nextx--, decision += dy)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        nexty -= y_sign;
                    }
                    if (nextx <= clip->right &&
                        nexty >= clip->top &&
                        nexty <= clip->bottom)
                    {
                        break;
                    }
                }

                for (; nextx > mid_point.x;
                    nextx--, decision += dy)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        nexty -= y_sign;
                    }
                    blend_func(context, nextx, nexty, linecolor, alpha);
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
                    }
                    if (nextx >= clip->left &&
                        nextx <= clip->right &&
                        nexty <= clip->bottom)
                    {
                        break;
                    }
                }

                for (; nexty > mid_point.y;
                    nexty--, decision += dx)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        nextx -= x_sign;
                    }
                    blend_func(context, nextx, nexty, linecolor, alpha);
                }

                /* walk out the clipping point.  */
                for (curx = xstart, cury = ystart, decision = (dy >> 1); cury < mid_point.y;
                    cury++, decision += dx)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        curx += x_sign;
                    }

                    if (curx >= clip->left &&
                        curx <= clip->right &&
                        cury >= clip->top)
                    {
                        break;
                    }
                }
                for (; cury <= mid_point.y;
                    cury++, decision += dx)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        curx += x_sign;
                    }
                    blend_func(context, curx, cury, linecolor, alpha);
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
                    y_sign = 0 - y_sign;
                }
                for (decision = (dx >> 1); steps > 0; curx += sign, decision += dy, steps--)
                {
                    if (decision >= dx)
                    {
                        decision -= dx;
                        cury += y_sign;
                    }

                    if (curx >= clip->left &&
                        curx <= clip->right &&
                        cury >= clip->top &&
                        cury <= clip->bottom)
                    {
                        blend_func(context, curx, cury, linecolor, alpha);
                    }
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
                    sign = 1;
                }
                else
                {
                    curx = xend;
                    cury = yend;
                    steps = yend - mid_point.y;
                    sign = -1;
                    x_sign = 0 - x_sign;
                }

                for (decision = (dy >> 1); steps > 0; cury += sign, decision += dx, steps--)
                {
                    if (decision >= dy)
                    {
                        decision -= dy;
                        curx += x_sign;
                    }
                    if (curx >= clip->left &&
                        curx <= clip->right &&
                        cury >= clip->top &&
                        cury <= clip->bottom)
                    {
                        blend_func(context, curx, cury, linecolor, alpha);
                    }
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

                }
                blend_func(context, curx, cury, linecolor, alpha);
                blend_func(context, nextx, nexty, linecolor, alpha);
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
                }
                blend_func(context, curx, cury, linecolor, alpha);
                blend_func(context, nextx, nexty, linecolor, alpha);
            }
        }
    }
}

#endif /* GX_BRUSH_ALPHA_SUPPORT */
