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

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_arc_fill                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Display driver to fill circle sector.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xcenter                               x-coord of center of circle   */
/*    ycenter                               y-coord of center of circle   */
/*    r                                     Radius of circle              */
/*    start_angle                           The start angle of circle arc */
/*    end_angle                             The end angle of circle arc   */
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
/*    _gx_display_driver_arc_clipping_get   Get an arc clipping.          */
/*    _gx_utility_rectangle_point_detect    Detect whether a pixel is     */
/*                                            inside rectangle            */
/*    _gx_utility_circle_point_get          Get point coord on a circle   */
/*    [gx_display_driver_horizontal_pixelmap_line_draw]                   */
/*                                          Basic display driver          */
/*                                            horizontal pixelmap line    */
/*                                            draw function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_generic_arc_draw                                 */
/*                                                                        */
/**************************************************************************/
#if defined(GX_ARC_DRAWING_SUPPORT)

void _gx_display_driver_generic_arc_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle)
{

GX_DISPLAY           *display;
GX_RECTANGLE         *clip;
GX_RECTANGLE          arc_clip[4];
GX_BRUSH             *brush;
GX_POINT              point;
INT                   sign[4][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
INT                  *pLineEnds;
INT                   ymin;
INT                   ymax;
INT                   xstart;
INT                   xend;
INT                   ystart;
INT                   yend;
INT                   curx;
INT                   cury;
INT                   nextx;
INT                   nexty;
INT                   dx;
INT                   dy;
INT                   Index;
INT                   Index1;
INT                   height;
INT                   xsign;
INT                   ysign;
INT                   decision;
void                  (*line_draw)(GX_DRAW_CONTEXT *context, INT x1, INT x2, INT ypos, INT width, GX_COLOR color);
GX_PIXELMAP          *pixelmap = GX_NULL;
INT                   xpos;
INT                   ypos;
INT                   inner_offset;
GX_VALUE              format;
INT                   skip_line;
GX_FILL_PIXELMAP_INFO info;

    display = context->display;
    brush = &context->brush;
    inner_offset = brush->width;
    line_draw = display->driver_horizontal_line_draw;
    clip = context->clip;

    if (inner_offset)
    {
        inner_offset -= 1;
        inner_offset >>= 1;
    }

    if (r <= (UINT)inner_offset)
    {
        return;
    }

    if (brush->style & GX_BRUSH_PIXELMAP_FILL)
    {
        /* Pick up pixelmap. */
        pixelmap = brush->pixelmap;

        if (pixelmap == GX_NULL)
        {
            return;
        }
        if (pixelmap->width <= 0 ||
            pixelmap->height <= 0)
        {
            return;
        }
        format = display->color_format;
        memset(&info, 0, sizeof(GX_FILL_PIXELMAP_INFO));

        if (format >= GX_COLOR_FORMAT_24XRGB)
        {
            /*32ARGB FORMAT use 24xrgb driver for now. So this is a special case.*/
            if (pixelmap->format < GX_COLOR_FORMAT_24XRGB)
            {
                /* Display driver only support its native format pixelmap.*/
                /* Nothing should be drawn if pixelmap format isn't support. */
                return;
            }
        }
        else
        {
            if (pixelmap->format != format)
            {
                /* Display driver only support its native format pixelmap.*/
                /* Nothing should be drawn if pixelmap format isn't support. */
                return;
            }
            else if (pixelmap->format == GX_COLOR_FORMAT_4BIT_GRAY)
            {
                info.mask = 0xf0;
            }
        }

        info.pixelmap = brush->pixelmap;
        info.current_pixel_ptr = (GX_UBYTE *)info.pixelmap->data;

        if (pixelmap->aux_data_size)
        {
            info.current_aux_ptr = (GX_UBYTE *)pixelmap->aux_data;
        }
    }

    r = (UINT)(r - (UINT)inner_offset);

    ymax = ycenter + (INT)r;
    ymin = ycenter - (INT)r;

    /* Get two endpoint of the arc. */
    _gx_utility_circle_point_get(xcenter, ycenter, r, start_angle, &point);

    xstart = point.x;
    ystart = point.y;

    _gx_utility_circle_point_get(xcenter, ycenter, r, end_angle, &point);

    xend = point.x;
    yend = point.y;

    xpos = xcenter - (INT)r;
    ypos = ycenter - (INT)r;
    skip_line = 0;

    /* Calculate minimum y line. */
    if (((start_angle < 90) && (end_angle < 90)) ||
        ((start_angle > 90) && (end_angle < 450)))
    {
        if (ystart > yend)
        {
            ymin = yend;
        }
        else
        {
            ymin = ystart;
        }
    }

    if (clip->top > ymin)
    {
        ymin = clip->top;
    }
    skip_line = ymin - ypos;

    /* Calculate maximum y line. */
    if (((start_angle < 270) && (end_angle < 270)) ||
        ((start_angle > 270) && (end_angle < 630)))
    {
        if (ystart > yend)
        {
            ymax = ystart;
        }
        else
        {
            ymax = yend;
        }
    }

    if (clip->bottom < ymax)
    {
        ymax = clip->bottom;
    }

    height = ymax - ymin + 1;

    /* default the point array to being off the screen on both sides: */
    pLineEnds = _gx_system_scratchpad;

    for (Index = 0; Index < height * 2; Index += 2)
    {
        pLineEnds[Index] = 2000;
        pLineEnds[Index + 1] = 0;
    }

    /* Get the clipping rectangles of the circle arc. */
    _gx_display_driver_arc_clipping_get(xcenter, ycenter, r, start_angle, end_angle, &arc_clip[0], &arc_clip[1], &arc_clip[2], &arc_clip[3]);

    curx = 0;
    cury = (INT)r;
    decision = (INT)(5 - 4 * r);

    while (curx <= cury)
    {
        for (Index = 0; Index < 4; Index++)
        {
            point.x = (GX_VALUE)(curx * sign[Index][0] + xcenter);
            point.y = (GX_VALUE)(cury * sign[Index][1] + ycenter);

            if ((point.y >= ymin) &&
                (point.y <= ymax))
            {
                if (arc_clip[0].contain_point_(point) ||
                    arc_clip[1].contain_point_(point) ||
                    arc_clip[2].contain_point_(point) ||
                    arc_clip[3].contain_point_(point))
                {
                    Index1 = (point.y - ymin) << 1;
                    if (point.x < pLineEnds[Index1])
                    {
                        pLineEnds[Index1] = point.x;
                    }

                    if (point.x > pLineEnds[Index1 + 1])
                    {
                        pLineEnds[Index1 + 1] = point.x;
                    }
                }
            }

            point.x = (GX_VALUE)(cury * sign[Index][0] + xcenter);
            point.y = (GX_VALUE)(curx * sign[Index][1] + ycenter);

            if ((point.y >= ymin) &&
                (point.y <= ymax))
            {
                if (arc_clip[0].contain_point_(point) ||
                    arc_clip[1].contain_point_(point) ||
                    arc_clip[2].contain_point_(point) ||
                    arc_clip[3].contain_point_(point))
                {
                    Index1 = (point.y - ymin) << 1;
                    if (point.x < pLineEnds[Index1])
                    {
                        pLineEnds[Index1] = point.x;
                    }

                    if (point.x > pLineEnds[Index1 + 1])
                    {
                        pLineEnds[Index1 + 1] = point.x;
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

    /* Fill in the point array by using Breshenhams line for
       the line that connect two endpoints of the arc. */

    dx = GX_ABS(xend - xstart);
    dy = GX_ABS(yend - ystart);

    /* Horizontal Line. */
    if (ystart != yend)
    {
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
                if ((cury >= ymin) &&
                    (cury <= ymax))
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
        }
        else
        {
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
                        Index1 = (nexty - ymin) << 1;

                        if (nextx < pLineEnds[Index1])
                        {
                            pLineEnds[Index1] = nextx;
                        }

                        if (nextx > pLineEnds[Index1 + 1])
                        {
                            pLineEnds[Index1 + 1] = nextx;
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
                        Index1 = (nexty - ymin) << 1;

                        if (nextx < pLineEnds[Index1])
                        {
                            pLineEnds[Index1] = nextx;
                        }

                        if (nextx > pLineEnds[Index1 + 1])
                        {
                            pLineEnds[Index1 + 1] = nextx;
                        }
                    }
                }
            }
        }
    }

    if (pixelmap)
    {
        /* Skip the un-draw line.*/
        if (skip_line > 0)
        {
            skip_line %= info.pixelmap->height;
            info.draw = GX_FALSE;
            while (skip_line--)
            {
                display->driver_horizontal_pixelmap_line_draw(context, 0, 0, cury, &info);
            }
        }
        info.draw = GX_TRUE;
    }

    Index = 0;
    for (cury = ymin; cury <= ymax; cury++)
    {
        if (pLineEnds[Index] < clip->left)
        {
            pLineEnds[Index] = clip->left;
        }

        if (pLineEnds[Index + 1] > clip->right)
        {
            pLineEnds[Index + 1] = clip->right;
        }

        if (pixelmap)
        {
            /* Filling arc area with pixelmap. */
            info.x_offset = pLineEnds[Index] - xpos;
            display->driver_horizontal_pixelmap_line_draw(context, pLineEnds[Index], pLineEnds[Index + 1], cury, &info);
        }
        else
        {
            /* Fill arc with horizontal lines. */
            line_draw(context, pLineEnds[Index], pLineEnds[Index + 1], cury, 1, brush->fill_color);
        }

        Index += 2;
    }
}

#endif

