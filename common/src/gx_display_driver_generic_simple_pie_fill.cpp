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
/*    _gx_display_driver_generic_simple_pie_fill                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Display driver to fill a pie chart.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xcenter                               x-coord of center of circle   */
/*    ycenter                               y-coord of center of circle   */
/*    r                                     Radius of circle              */
/*    start_angle                           Starting angle                */
/*    end_angle                             Ending angle                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_arc_clipping_get   Get an arc clipping.          */
/*    _gx_utility_rectangle_overlap_detect  Detects two rectangles being  */
/*                                            overlap                     */
/*    _gx_utility_rectangle_point_detect    Detect whether a pixel is     */
/*                                            inside rectangle            */
/*    _gx_utility_circle_point_get          Get point coord on a circle   */
/*    [gx_display_driver_horizontal_pixelmap_line_draw]                   */
/*                                          Basic display driver          */
/*                                            horizontal pixelmap line    */
/*                                            draw function               */
/*    [gx_display_driver_horizontal_line_draw]                            */
/*                                          Basic display driver          */
/*                                            horizontal line draw routine*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#if defined(GX_ARC_DRAWING_SUPPORT)

void _gx_display_driver_generic_simple_pie_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter,
                                                UINT r, INT start_angle, INT end_angle, GX_BOOL skip_end)
{
/* The function will only fill a pie with both start angle and end angle are
   between 0 and 180 or beween 180 and 360.*/

GX_DISPLAY           *display;
GX_BRUSH             *brush;
GX_RECTANGLE         *clip;
GX_RECTANGLE          arc_clip[4];
GX_POINT              point;
GX_POINT              points[3];
INT                   sign[4][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
INT                  *pLineEnds;
GX_POINT             *pGet;
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
INT                   loop;
INT                   height;
INT                   xsign;
INT                   ysign;
INT                   decision;
int                   fillingwards;
void                  (*line_draw)(GX_DRAW_CONTEXT *context, INT x1, INT x2, INT ypos, INT width, GX_COLOR color);
INT                   xpos = 0;
GX_PIXELMAP          *pixelmap = GX_NULL;
GX_VALUE              format;
INT                   skip_line;
GX_FILL_PIXELMAP_INFO info;

    display = context->display;
    brush = &context->brush;
    line_draw = display->driver_horizontal_line_draw;

    if (brush->style & GX_BRUSH_PIXELMAP_FILL)
    {
        /* Pick up pixelmap. */
        pixelmap = brush->pixelmap;
        if (pixelmap == GX_NULL)
        {
            return;
        }
        if (pixelmap->height <= 0 ||
            pixelmap->width <= 0)
        {
            return;
        }

        memset(&info, 0, sizeof(GX_FILL_PIXELMAP_INFO));
        format = display->color_format;

        /*Check the pixelmap format is supported in this driver or not.*/
        if (format >= GX_COLOR_FORMAT_24XRGB)
        {
            /*32ARGB FORMAT use 24xrgb driver for now. So this is a special case.*/
            if (pixelmap->format < GX_COLOR_FORMAT_24XRGB)
            {
                /* Display driver only support its native format pixelmap. */
                /* Nothing should be drawn if pixelmap format isn't support. */
                return;
            }
        }
        else
        {
            if (pixelmap->format != format)
            {
                /* Display driver only support its native format pixelmap. */
                /* Nothing should be drawn if pixelmap format isn't support. */
                return;
            }
            else if (pixelmap->format == GX_COLOR_FORMAT_4BIT_GRAY)
            {
                info.mask = 0xf0;
            }
        }

        info.pixelmap = pixelmap;
        info.current_pixel_ptr = (GX_UBYTE *)pixelmap->data;
        if (pixelmap->aux_data_size)
        {
            info.current_aux_ptr = (GX_UBYTE *)pixelmap->aux_data;
        }
    }

    pGet = points;

    _gx_display_driver_arc_clipping_get(xcenter, ycenter, r, start_angle, end_angle,
                                        &arc_clip[0], &arc_clip[1], &arc_clip[2], &arc_clip[3]);

    if ((start_angle <= 180) && (end_angle <= 180))
    {
        /* Pie is in upper side. */
        ymin = ycenter - (INT)r;
        ymax = ycenter;

        if (skip_end)
        {
            ymax--;
        }
    }
    else
    {
        /* Pie is in lower side. */
        ymin = ycenter;
        ymax = ycenter + (INT)r;
    }

    clip = context->clip;

    if (clip->top > ymin)
    {
        ymin = clip->top;
    }
    skip_line = ymin - (ycenter - (INT)r);

    if (clip->bottom < ymax)
    {
        ymax = clip->bottom;
    }

    height = ymax - ymin + 1;

    pLineEnds = _gx_system_scratchpad;

    /* default the point array to being off the screen on both sides: */

    for (loop = 0; loop < height * 2; loop += 2)
    {
        pLineEnds[loop] = 2000;
        pLineEnds[loop + 1] = 0;
    }

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

    /* Fill in the point array by using Breshenhams line for
       2 lines of circle sector
     */

    points[1].x = (GX_VALUE)xcenter;
    points[1].y = (GX_VALUE)ycenter;

    _gx_utility_circle_point_get(xcenter, ycenter, r, start_angle, &points[0]);
    _gx_utility_circle_point_get(xcenter, ycenter, r, end_angle, &points[2]);

    for (loop = 0; loop < 2; loop++)
    {
        xstart = pGet->x;
        ystart = pGet->y;
        pGet++;
        xend = pGet->x;
        yend = pGet->y;
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

        if (start_angle < 180)
        {
            if (loop == 0)
            {
                fillingwards = (1 + ysign) >> 1;
            }
            else
            {
                fillingwards = (1 - ysign) >> 1;
            }
        }
        else
        {
            if (loop == 0)
            {
                fillingwards = (1 - ysign) >> 1;
            }
            else
            {
                fillingwards = (1 + ysign) >> 1;
            }
        }

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

                if ((cury - 1 + fillingwards >= ymin) && (cury - 1 + fillingwards <= ymax))
                {
                    Index = (cury - 1 + fillingwards - ymin) << 1;

                    if (curx < pLineEnds[Index])
                    {
                        pLineEnds[Index] = curx;
                    }

                    if (curx > pLineEnds[Index + 1])
                    {
                        pLineEnds[Index + 1] = curx;
                    }
                }

                if ((nexty - 1 + fillingwards >= ymin) && (nexty - 1 + fillingwards <= ymax))
                {
                    Index = (nexty - 1 + fillingwards - ymin) << 1;

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

                    if (curx - 1 > pLineEnds[Index + 1])
                    {
                        pLineEnds[Index + 1] = curx - 1;
                    }
                }

                if ((nexty >= ymin) && (nexty <= ymax))
                {
                    Index = (nexty - ymin) << 1;

                    if (nextx < pLineEnds[Index])
                    {
                        pLineEnds[Index] = nextx;
                    }

                    if (nextx - 1 > pLineEnds[Index + 1])
                    {
                        pLineEnds[Index + 1] = nextx - 1;
                    }
                }
            }
        }
    }

    if (pixelmap != GX_NULL)
    {
        /* Calculate the draw_start position and skip line to fill the pie area. */
        xpos = xcenter - (INT)r;

        /*Skip the un-draw line.*/
        if (skip_line > 0)
        {
            skip_line %= info.pixelmap->height;
            info.draw = GX_FALSE;
            while (skip_line--)
            {
                display->driver_horizontal_pixelmap_line_draw(context, 0, 0, cury, &info);
            }
        }
    }

    /* Filling circle sector with horizontal line. */
    Index = 0;
    for (cury = ymin; cury <= ymax; cury++)
    {
        if (pLineEnds[Index] <= pLineEnds[Index + 1])
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
                info.draw = GX_TRUE;
                info.x_offset = pLineEnds[Index] - xpos;
                /* Filling pie area with pixelmap. */
                display->driver_horizontal_pixelmap_line_draw(context, pLineEnds[Index], pLineEnds[Index + 1], cury, &info);
            }
            else
            {
                line_draw(context, pLineEnds[Index], pLineEnds[Index + 1], cury, 1,
                          brush->fill_color);
            }
        }
        else
        {
            if (pixelmap != GX_NULL)
            {
                info.draw = GX_FALSE;
                display->driver_horizontal_pixelmap_line_draw(context, 0, 0, cury, &info);
            }
        }

        Index += 2;
    }
}

#endif

