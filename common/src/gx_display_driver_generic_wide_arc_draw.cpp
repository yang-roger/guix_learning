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

#if defined(GX_ARC_DRAWING_SUPPORT)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_simple_wide_arc_draw                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Innner help function that draw a wide arc between [90, 180] or      */
/*    [270, 540].                                                         */
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
/*    _gx_display_driver_arc_clipping_get   Get an arc clipping.          */
/*    _gx_utility_rectangle_point_detect    Detect whether a pixel is     */
/*                                            inside rectangle            */
/*    [gx_display_driver_line_draw]         The generic display driver    */
/*                                            line drawing routine        */
/*    _gx_utility_circle_point_get          Get point coord on a circle   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_generic_wide_arc_draw                            */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_generic_simple_wide_arc_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle)
{
/* The function draw a wide arc between 90 and 270 or beween 270 and 450.*/
GX_DISPLAY   *display;
GX_BRUSH     *brush;
GX_RECTANGLE *clip;
GX_RECTANGLE  arc_clip[4];
GX_POINT      point;
GX_POINT      inner_start;
GX_POINT      inner_end;
GX_POINT      outer_start;
GX_POINT      outer_end;
INT           sign[4][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
INT          *pLineEnds;
INT           ymin;
INT           ymax;
INT           xstart;
INT           xend;
INT           ystart;
INT           yend;
INT           curx;
INT           cury;
INT           nextx;
INT           nexty;
INT           dx;
INT           dy;
INT           Index;
INT           Index1;
INT           loop;
INT           height;
INT           xsign;
INT           ysign;
INT           decision;
INT           brush_width;
void          (*line_draw)(GX_DRAW_CONTEXT *context, INT x1, INT x2, INT ypos, INT width, GX_COLOR color);


    display = context->display;
    brush = &context->brush;
    brush_width = brush->width;
    line_draw = display->driver_horizontal_line_draw;

    if (r <= (UINT)((brush_width - 1) >> 1))
    {
        return;
    }

    clip = context->clip;
    pLineEnds = _gx_system_scratchpad;

    ymin = ycenter - (INT)r - (brush_width >> 1);
    ymax = ycenter + (INT)r + (brush_width >> 1);

    /* Get end points. */
    _gx_utility_circle_point_get(xcenter, ycenter, r - (UINT)((brush_width - 1) >> 1), start_angle, &inner_start);
    _gx_utility_circle_point_get(xcenter, ycenter, r - (UINT)((brush_width - 1) >> 1), end_angle, &inner_end);
    _gx_utility_circle_point_get(xcenter, ycenter, r + (UINT)(brush_width >> 1), start_angle, &outer_start);
    _gx_utility_circle_point_get(xcenter, ycenter, r + (UINT)(brush_width >> 1), end_angle, &outer_end);

    if (((start_angle < 90) && (end_angle < 90)) ||
        ((start_angle > 90) && (end_angle < 450)))
    {
        if (outer_start.y < outer_end.y)
        {
            ymin = outer_start.y;
        }
        else
        {
            ymin = outer_end.y;
        }

        if (inner_start.y < ymin)
        {
            ymin = inner_start.y;
        }

        if (inner_end.y < ymin)
        {
            ymin = inner_end.y;
        }
    }

    if (clip->top > ymin)
    {
        ymin = clip->top;
    }

    /* Calculate maximum y line. */
    if (((start_angle < 270) && (end_angle < 270)) || (start_angle > 270))
    {
        if (outer_start.y > outer_end.y)
        {
            ymax = outer_start.y;
        }
        else
        {
            ymax = outer_end.y;
        }

        if (inner_start.y > ymax)
        {
            ymax = inner_start.y;
        }

        if (inner_end.y > ymax)
        {
            ymax = inner_end.y;
        }
    }

    if (clip->bottom < ymax)
    {
        ymax = clip->bottom;
    }

    height = ymax - ymin + 1;

    /* default the point array to being off the screen on both sides: */

    for (loop = 0; loop < height * 2; loop += 2)
    {
        pLineEnds[loop] = 2000;
        pLineEnds[loop + 1] = 0;
    }

    r = (UINT)(r - (UINT)((brush_width - 1) >> 1));

    /* Get point array of inner arc and outer arc. */
    for (Index1 = 0; Index1 < 2; Index1++)
    {
        if (Index1 == 1)
        {
            r += (UINT)(brush_width - 1);
        }

        _gx_display_driver_arc_clipping_get(xcenter, ycenter, r, start_angle, end_angle,
                                            &arc_clip[0], &arc_clip[1], &arc_clip[2], &arc_clip[3]);

        curx = 0;
        cury = (INT)r;
        decision = 5 - (INT)(4 * r);

        while (curx <= cury)
        {
            for (loop = 0; loop < 4; loop++)
            {
                point.x = (GX_VALUE)(curx * sign[loop][0] + xcenter);
                point.y = (GX_VALUE)(cury * sign[loop][1] + ycenter);

                if ((point.y >= ymin) && (point.y <= ymax))
                {
                    if (_gx_utility_rectangle_point_detect(&arc_clip[0], point) ||
                        _gx_utility_rectangle_point_detect(&arc_clip[1], point))
                    {
                        Index = (point.y - ymin) << 1;
                        if (point.x < pLineEnds[Index])
                        {
                            pLineEnds[Index] = point.x;
                        }

                        if (point.x > pLineEnds[Index + 1])
                        {
                            pLineEnds[Index + 1] = point.x;
                        }
                    }
                }

                point.x = (GX_VALUE)(cury * sign[loop][0] + xcenter);
                point.y = (GX_VALUE)(curx * sign[loop][1] + ycenter);

                if ((point.y >= ymin) && (point.y <= ymax))
                {
                    if (_gx_utility_rectangle_point_detect(&arc_clip[0], point) ||
                        _gx_utility_rectangle_point_detect(&arc_clip[1], point))
                    {
                        Index = (point.y - ymin) << 1;
                        if (point.x < pLineEnds[Index])
                        {
                            pLineEnds[Index] = point.x;
                        }

                        if (point.x > pLineEnds[Index + 1])
                        {
                            pLineEnds[Index + 1] = point.x;
                        }
                    }
                }
            }

            if (decision < 0)
            {
                decision += 8 * curx + 12;
            }
            else
            {
                decision += 8 * (curx - cury) + 20;
                cury--;
            }
            curx++;
        }
    }

    /* Fill in the point array by using Breshenhams line for
       2 lines of the arc end.
     */

    for (loop = 0; loop < 2; loop++)
    {
        if (loop == 0)
        {
            xstart = inner_start.x;
            ystart = inner_start.y;
            xend = outer_start.x;
            yend = outer_start.y;
        }
        else
        {
            xstart = inner_end.x;
            ystart = inner_end.y;
            xend = outer_end.x;
            yend = outer_end.y;
        }

        dx = GX_ABS(xend - xstart);
        dy = GX_ABS(yend - ystart);

        /* Horizontal Line. */
        if (ystart == yend)
        {
            continue;
        }

        /* Vertical Line. */
        if (xstart == xend)
        {
            if (ystart > yend)
            {
                GX_SWAP_VALS(xstart, xend);
                GX_SWAP_VALS(ystart, yend);
            }

            for (cury = ystart; cury <= yend; cury++)
            {
                if ((cury >= ymin) && (cury <= ymax))
                {
                    Index = (cury - ymin) << 1;
                    if (xstart <= pLineEnds[Index])
                    {
                        pLineEnds[Index] = xstart;
                    }

                    if (xstart > pLineEnds[Index + 1])
                    {
                        pLineEnds[Index + 1] = xstart;
                    }
                }
            }
            continue;
        }

        /* Simple Line. */

        if (((dx >= dy && (xstart > xend)) ||
             ((dy > dx) && ystart > yend)))
        {
            GX_SWAP_VALS(xend, xstart);
            GX_SWAP_VALS(yend, ystart);
        }

        xsign = (xend - xstart) / dx;
        ysign = (yend - ystart) / dy;

        if (dx >= dy)
        {
            for (curx = xstart, cury = ystart, nextx = xend, nexty = yend,
                 decision = (dx >> 1); curx <= nextx; curx++, nextx--,
                 decision += dy)
            {
                if (decision >= dx)
                {
                    decision -= dx;
                    cury += ysign;
                    nexty -= ysign;
                }

                if ((cury >= ymin) && (cury <= ymax))
                {
                    Index = (cury - ymin) << 1;

                    if (curx < pLineEnds[Index])
                    {
                        pLineEnds[Index] = curx;
                    }

                    if (curx > pLineEnds[Index + 1])
                    {
                        pLineEnds[Index + 1] = curx;
                    }
                }

                if ((nexty >= ymin) && (nexty <= ymax))
                {
                    Index = (nexty - ymin) << 1;

                    if (nextx < pLineEnds[Index])
                    {
                        pLineEnds[Index] = nextx;
                    }

                    if (nextx > pLineEnds[Index + 1])
                    {
                        pLineEnds[Index + 1] = nextx;
                    }
                }
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
                    curx += xsign;
                    nextx -= xsign;
                }

                if ((cury >= ymin) && (cury <= ymax))
                {
                    Index = (cury - ymin) << 1;
                    if (curx < pLineEnds[Index])
                    {
                        pLineEnds[Index] = curx;
                    }

                    if (curx > pLineEnds[Index + 1])
                    {
                        pLineEnds[Index + 1] = curx;
                    }
                }


                if ((nexty >= ymin) && (nexty <= ymax))
                {
                    Index = (nexty - ymin) << 1;

                    if (nextx < pLineEnds[Index])
                    {
                        pLineEnds[Index] = nextx;
                    }

                    if (nextx > pLineEnds[Index + 1])
                    {
                        pLineEnds[Index + 1] = nextx;
                    }
                }
            }
        }
    }

    /* Filling the outline area with horizontal line. */
    Index = 0;
    for (cury = ymin; cury <= ymax; cury++)
    {
        if (pLineEnds[Index] < pLineEnds[Index + 1])
        {
            if (pLineEnds[Index] < clip->left)
            {
                pLineEnds[Index] = clip->left;
            }

            if (pLineEnds[Index + 1] > clip->right)
            {
                pLineEnds[Index + 1] = clip->right;
            }

            line_draw(context, pLineEnds[Index], pLineEnds[Index + 1], cury, 1,
                      brush->line_color);
        }

        Index += 2;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_wide_arc_draw                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Display driver function to draw simple circular arcle with wide     */
/*    outline.                                                            */
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
/*    [_gx_display_driver_generic_simple_wide_arc_draw]                   */
/*                                          Real arc draw function        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_generic_wide_arc_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle)
{
GX_BRUSH *brush;
INT       brush_width;
GX_POINT  startp;
GX_POINT  endp;
GX_COLOR  old_fill;
UINT      old_style;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
GX_UBYTE old_alpha;
    old_alpha = context->brush.alpha;
    context->brush.alpha = GX_ALPHA_VALUE_OPAQUE;
#endif

    if (start_angle < 90)
    {
        if (end_angle <= 90)
        {
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, start_angle, end_angle);
        }
        else if (end_angle <= 270)
        {
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, start_angle, 90);
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, 90, end_angle);
        }
        else
        {
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, start_angle, 90);
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, 90, 270);
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, 270, end_angle);
        }
    }
    else if (start_angle < 270)
    {
        if (end_angle <= 270)
        {
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, start_angle, end_angle);
        }
        else if (end_angle <= 450)
        {
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, start_angle, 270);
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, 270, end_angle);
        }
        else
        {
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, start_angle, 270);
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, 270, 450);
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, 90, end_angle - 360);
        }
    }
    else
    {
        if (end_angle <= 450)
        {
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, start_angle, end_angle);
        }
        else if (end_angle <= 630)
        {
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, start_angle, 450);
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, 90, end_angle - 360);
        }
        else
        {
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, start_angle, 450);
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, 90, 270);
            _gx_display_driver_generic_simple_wide_arc_draw(context, xcenter, ycenter, r, 270, end_angle - 360);
        }
    }

    brush = &context->brush;
    brush_width = brush->width;
    old_fill = brush->fill_color;
    old_style = brush->style;

    brush->width = 1;
    brush->fill_color = brush->line_color;
    brush->style |= GX_BRUSH_SOLID_FILL;

    r = (UINT)(r - (UINT)((brush_width - 1) >> 1));
    _gx_utility_circle_point_get(xcenter, ycenter, r, start_angle, &startp);
    _gx_utility_circle_point_get(xcenter, ycenter, r + (UINT)(brush_width - 1), start_angle, &endp);

    if (brush->style & GX_BRUSH_ROUND)
    {
        brush->style &= (ULONG)(~GX_BRUSH_PIXELMAP_FILL);
        _gx_display_driver_generic_filled_circle_draw(context,
                                                      GX_FIXED_VAL_MAKE(startp.x + endp.x) >> 1,
                                                      GX_FIXED_VAL_MAKE(startp.y + endp.y) >> 1,
                                                      GX_FIXED_VAL_MAKE(brush_width) >> 1);
    }

    _gx_utility_circle_point_get(xcenter, ycenter, r, end_angle, &startp);
    _gx_utility_circle_point_get(xcenter, ycenter, r + (UINT)(brush_width - 1), end_angle, &endp);

    if (brush->style & GX_BRUSH_ROUND)
    {
        brush->style &= (ULONG)(~GX_BRUSH_PIXELMAP_FILL);
        _gx_display_driver_generic_filled_circle_draw(context,
                                                      GX_FIXED_VAL_MAKE(startp.x + endp.x) >> 1,
                                                      GX_FIXED_VAL_MAKE(startp.y + endp.y) >> 1,
                                                      GX_FIXED_VAL_MAKE(brush_width) >> 1);
    }

    brush->width = (GX_VALUE)brush_width;
    brush->fill_color = old_fill;
    brush->style = old_style;
#if defined(GX_BRUSH_ALPHA_SUPPORT)
    context->brush.alpha = old_alpha;
#endif
}
#endif

