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

#include "gx_context.h"
#include "gx_pixelmap.h"
#include "gx_polygon_edge.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_polygon_min_max_get                              */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that calculates the top most and bottom    */
/*    most y coordinate of the specified polygon.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    vertex                            Array of points of the polygon,   */
/*                                        terminated by NULL entry        */
/*    num                               Number of points of polygon       */
/*    xmin                              Retrieved left most x coordinate  */
/*    xmax                              Retrieved right most x coordinate */
/*    ymin                              Retrieved top most y coordinate   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_generic_polygon_fill                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_polygon_min_max_get(GX_POINT *vertex, INT num, INT *xmin, INT *xmax, INT *ymin)
{
GX_VALUE index = (GX_VALUE)num;
INT      x_min;
INT      x_max;
INT      y_min;

    x_min = vertex[0].x;
    x_max = vertex[0].x;
    y_min = vertex[0].y;

    while (index--)
    {
        if (vertex[index].x < x_min)
        {
            x_min = vertex[index].x;
        }
        else if (vertex[index].x > x_max)
        {
            x_max = vertex[index].x;
        }

        if (vertex[index].y < y_min)
        {
            y_min = vertex[index].y;
        }
    }

    *xmin = x_min;
    *xmax = x_max;
    *ymin = y_min;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_polygon_new_edge_table_init_90                   */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that Initiates new edge table, which       */
/*    contain all the edges of the specified polygon.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    vertex                            Array of points of the polygon,   */
/*                                        terminated by NULL entry        */
/*    num                               Number of points of polygon       */
/*    net                               Pointer to new edge table         */
/*    net_size                          New edge table size               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_generic_polygon_fill                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_polygon_new_edge_table_init_90(GX_POINT *vertex, INT num, GX_POLYGON_EDGE *net, INT *net_size)
{
GX_POLYGON_EDGE edge;
INT             index;
GX_POINT        s_point;
GX_POINT        ss_point;
GX_POINT        e_point;
GX_POINT        ee_point;
GX_VALUE        net_index = 0;

    for (index = 0; index < num; index++)
    {
        s_point = vertex[index];
        e_point = vertex[(index + 1) % num];
        ss_point = vertex[(index - 1 + num) % num];
        ee_point = vertex[(index + 2) % num];

        /* Skip vertical line. */
        if (s_point.x != e_point.x)
        {
            /* Add one edge to new edge table, s_point and e_point are two endpoints of the added edge. */

            edge.dx = GX_ABS(e_point.x - s_point.x);
            edge.dy = GX_ABS(e_point.y - s_point.y);
            edge.x_decision = edge.dx >> 1;
            if (((e_point.x > s_point.x) && (e_point.y > s_point.y)) ||
                ((e_point.x < s_point.x) && (e_point.y < s_point.y)))
            {
                edge.sign = -1;
            }
            else
            {
                edge.sign = 1;
            }

            if (e_point.x > s_point.x)
            {
                /* Set xi value, which is the y coordinate value of the edge's rightmost point. */
                edge.xi = e_point.y;

                /* Set min x coordinate of the edge. */
                edge.ymax = s_point.x;

                if (ss_point.x <= s_point.x)
                {
                    /* If e_point isn't the left most point, reset xmin to xmin - 1.
                       To avoid producing 2 intersections when scan line comes to the vertex point. */
                    edge.ymax += 1;
                }

                edge.ymin = e_point.x;
            }
            else
            {
                /* Set xi value. */
                edge.xi = s_point.y;

                /* Set max x coordinate of the edge. */
                edge.ymax = e_point.x;

                if (ee_point.x <= e_point.x)
                {
                    /* If e_point isn't the left most point, reset xmin to xmin - 1. */
                    edge.ymax += 1;
                }

                edge.ymin = s_point.x;
            }

            net[net_index++] = edge;
        }
    }

    *net_size = net_index;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_polygon_new_edge_table_init_270                  */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that Initiates new edge table, which       */
/*    contain all the edges of the specified polygon.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    vertex                            Array of points of the polygon,   */
/*                                        terminated by NULL entry        */
/*    num                               Number of points of polygon       */
/*    net                               Pointer to new edge table         */
/*    net_size                          New edge table size               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_generic_polygon_fill                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_polygon_new_edge_table_init_270(GX_POINT *vertex, INT num, GX_POLYGON_EDGE *net, INT *net_size)
{
GX_POLYGON_EDGE edge;
INT             index;
GX_POINT        s_point;
GX_POINT        ss_point;
GX_POINT        e_point;
GX_POINT        ee_point;
GX_VALUE        net_index = 0;

    for (index = 0; index < num; index++)
    {
        s_point = vertex[index];
        e_point = vertex[(index + 1) % num];
        ss_point = vertex[(index - 1 + num) % num];
        ee_point = vertex[(index + 2) % num];

        /* Skip vertical line. */
        if (s_point.x != e_point.x)
        {
            /* Add one edge to new edge table, s_point and e_point are two endpoints of the added edge. */
            edge.dx = GX_ABS(e_point.x - s_point.x);
            edge.dy = GX_ABS(e_point.y - s_point.y);
            edge.x_decision = edge.dx >> 1;
            if (((e_point.x > s_point.x) && (e_point.y > s_point.y)) ||
                ((e_point.x < s_point.x) && (e_point.y < s_point.y)))
            {
                edge.sign = 1;
            }
            else
            {
                edge.sign = -1;
            }

            if (e_point.x > s_point.x)
            {
                /* Set xi value. */
                edge.xi = s_point.y;

                /* Set max x coordinate of the edge. */
                edge.ymax = e_point.x;

                if (ee_point.x >= e_point.x)
                {
                    /* If e_point isn't the right most point, reset xmax to xmax - 1. */
                    edge.ymax -= 1;
                }

                edge.ymin = s_point.x;
            }
            else
            {
                /* Set xi value, which is the y coordinate value of the edge's leftmost point. */
                edge.xi = e_point.y;

                /* Set max x coordinate of the edge. */
                edge.ymax = s_point.x;

                if (ss_point.x >= s_point.x)
                {
                    /* If e_point isn't the right most point, reset xmax to xmax - 1.
                       To avoid producing 2 intersections when scan line comes to the vertex point. */
                    edge.ymax -= 1;
                }

                edge.ymin = e_point.x;
            }

            net[net_index++] = edge;
        }
    }

    *net_size = net_index;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_polygon_active_edge_insert                       */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that Inserts an edge from new edge table to*/
/*    active edge table.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    aet                               Pointer to active edge table      */
/*    net                               Pointer to new edge table         */
/*    x                                 X coordinate of current scan line */
/*    net                               Pointer to new edge table         */
/*    net_size                          New edge table size               */
/*    aet_size                          Retured active edge table size    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_generic_polygon_fill                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_polygon_active_edge_insert(GX_POLYGON_EDGE *aet, GX_POLYGON_EDGE *net, INT x, INT net_size, INT *aet_size)
{
GX_VALUE ni = (GX_VALUE)(net_size - 1);
GX_VALUE ai;
INT      active_edge_table_size = *aet_size;

    for (ni = 0; ni < net_size; ni++)
    {
        if (net[ni].ymin == x)
        {
            /* If the rightmost x coordinate value of the edge equals to the current scan line x,
               Insert the edge to active edge table, and increase aet size by 1. */

            ai = (GX_VALUE)(active_edge_table_size - 1);
            while ((ai >= 0) && (net[ni].xi < aet[ai].xi))
            {
                aet[ai + 1] = aet[ai];
                ai--;
            }
            aet[ai + 1] = net[ni];
            active_edge_table_size++;
        }
    }

    *aet_size = active_edge_table_size;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_polygon_non_active_edge_remove                   */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that removes an edge from active edge table*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    aet                               Pointer to active edge table      */
/*    x                                 X coordinate of current scan line */
/*    aet_size                          Active edge table size            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_generic_polygon_fill                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_polygon_non_active_edge_remove(GX_POLYGON_EDGE *aet, INT x, INT *aet_size)
{
INT i;
INT j;

    for (i = 0; i < *aet_size; i++)
    {
        /* If the bottom most y coordinate value of the edge equals to the current scan line y,
           Remove the edge from active edge table, and decrease aet size by 1. */
        if (aet[i].ymax == x)
        {
            for (j = i; j < *aet_size - 1; j++)
            {
                aet[j] = aet[j + 1];
            }
            (*aet_size)--;
            i--;
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_polygon_scan_line_draw                           */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that filling area between pair             */
/*    intersections.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                           Drawing context                   */
/*    aet                               Pointer to active edge table      */
/*    x                                 X coordinate of current scan line */
/*    aet_size                          Active edge table size            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_horizontal_line_draw]                            */
/*                                      Basic display driver horizontal   */
/*                                        line draw function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_generic_polygon_fill                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_polygon_scan_line_draw(GX_DRAW_CONTEXT *context, GX_POLYGON_EDGE *aet, INT x, INT aet_size)
{
GX_VALUE      index;
INT           y1, y2;
GX_DISPLAY   *display = context->display;
GX_RECTANGLE *clip = context->clip;
GX_COLOR      fill_color;

    fill_color = context->brush.fill_color;

    for (index = 0; index < aet_size - 1; index = (GX_VALUE)(index + (GX_VALUE)2))
    {

        /* Pick up y coordinate value of the pair intersections. */
        y1 = aet[index].xi;
        y2 = aet[index + 1].xi;

        if (context->brush.style & GX_BRUSH_ALIAS)
        {
            if ((aet[index].dy > aet[index].dx) &&
                (aet[index].sign < 0))
            {
                y1 += 1;
            }

            if (aet[index + 1].dy >= aet[index + 1].dx)
            {
                if (aet[index + 1].sign > 0)
                {
                    y2 -= 1;
                }
            }
            else
            {
                y2 -= 1;
            }
        }

        /* Cliping segments y1y2. */
        if (y1 < clip->top)
        {
            y1 = clip->top;
        }

        if (y2 > clip->bottom)
        {
            y2 = clip->bottom;
        }

        if (y1 <= y2)
        {
            /* Filling area between pairs of the intersections. */
            display->driver_vertical_line_draw(context, y1, y2, x, 1, fill_color);
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_polygon_pixelmap_scan_line_draw                  */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that filling area between pair             */
/*    intersections with pixelmap.                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                           Drawing context                   */
/*    aet                               Pointer to active edge table      */
/*    aet_size                          Active edge table size            */
/*    ypos                              Y coord of draw start position    */
/*    x                                 X coordinate of current scan line */
/*    pixelmap                          Pixelmap to fill                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_horizontal_pixelmap_line_draw]                   */
/*                                      Basic display driver horizontal   */
/*                                        pixelmap line draw function     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_generic_polygon_fill                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_polygon_pixelmap_scan_line_draw(GX_DRAW_CONTEXT *context, GX_POLYGON_EDGE *aet, INT aet_size,
                                                               INT ypos, INT x, GX_FILL_PIXELMAP_INFO *info)
{
GX_VALUE              index;
INT                   y1, y2;
GX_DISPLAY           *display = context->display;
GX_RECTANGLE         *clip = context->clip;
GX_FILL_PIXELMAP_INFO old_info;
GX_BOOL               is_drawn = GX_FALSE;

    /* Store the ptr info in case that this line will be drawn several times.*/
    old_info = (*info);

    for (index = 0; index < aet_size - 1; index = (GX_VALUE)(index + (GX_VALUE)2))
    {
        /* if aet_size is bigger than 2, it means there's at least two segment in this line.
           So it must be drawn this line several times and should always be started from the "correct" position. */
        (*info) = old_info;

        /* Pick up y coordinate value of the pair intersections. */
        y1 = aet[index].xi;
        y2 = aet[index + 1].xi;

        if (context->brush.style & GX_BRUSH_ALIAS)
        {
            if ((aet[index].dy > aet[index].dx) &&
                (aet[index].sign < 0))
            {
                y1 += 1;
            }

            if (aet[index + 1].dy >= aet[index + 1].dx)
            {
                if (aet[index + 1].sign > 0)
                {
                    y2 -= 1;
                }
            }
            else
            {
                y2 -= 1;
            }
        }

        /* Cliping segments y1y2. */
        if (y1 < clip->top)
        {
            y1 = clip->top;
        }

        if (y2 > clip->bottom)
        {
            y2 = clip->bottom;
        }

        if (y1 > y2)
        {
            continue;
        }

        /* Filling area between pairs of the intersections. */
        info->x_offset = y1 - ypos;
        display->driver_horizontal_pixelmap_line_draw(context, y1, y2, x, info);

        is_drawn = GX_TRUE;
    }

    if (!is_drawn)
    {
        info->draw = GX_FALSE;
        display->driver_horizontal_pixelmap_line_draw(context, 0, 0, 0, info);
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_polygon_active_edge_table_update_and_restore     */
/*                                                                        */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that updates x coordinate of intersection  */
/*    according to edge gradient, and sort intersections by increase x    */
/*    coordinate value.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    aet                               Pointer to active edge table      */
/*    aet_size                          Active edge table size            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_generic_polygon_fill                             */
/*                                                                        */
/**************************************************************************/
static UINT _gx_display_driver_polygon_active_edge_table_update_and_restore(GX_POLYGON_EDGE *aet, INT aet_size)
{
GX_VALUE        index = (GX_VALUE)aet_size, xi, yi;
GX_POLYGON_EDGE temp;

    /* Update xi value. */
    while (index--)
    {
        if (aet[index].dy == 0)
        {
            continue;
        }

        /* When scan line move one step from right to left,
           we need to update y value of active edges with the delta of the edge,
           the following codes equals to y += delta(x). */
        aet[index].x_decision += aet[index].dy;

        while (aet[index].x_decision > aet[index].dx)
        {
            aet[index].xi += aet[index].sign;
            aet[index].x_decision -= aet[index].dx;
        }
    }

    /* Sort with xi value in ascending order. */
    for (xi = 0; xi < aet_size - 1; xi++)
    {
        for (yi = (GX_VALUE)(xi + 1); yi < aet_size; yi++)
        {
            if (aet[xi].xi > aet[yi].xi)
            {
                temp = aet[yi];
                aet[yi] = aet[xi];
                aet[xi] = temp;
            }
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_rotated_polygon_fill                     */
/*                                                            6.1.3       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This fills a polygon.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    num                               Number of points of polygon       */
/*    vertex                            Array of points of the polygon,   */
/*                                        terminated by NULL entry.       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Completion status                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_line_draw              Draw the specified line           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_generic_rotated_polygon_fill(GX_DRAW_CONTEXT *context, GX_POINT *vertex, INT num)
{
INT                    xmin;
INT                    xmax;
INT                    ypos;
INT                    x;
INT                    xstart;
GX_BYTE                xsign;
static GX_POLYGON_EDGE net[GX_POLYGON_MAX_EDGE_NUM]; /* new edge table: obtain all edges of the polygon. */
static GX_POLYGON_EDGE aet[GX_POLYGON_MAX_EDGE_NUM]; /* active edge table: obtain all edges that touched by the current scan line. */
GX_BRUSH              *brush;
INT                    aet_size = 0;
INT                    net_size = num;
GX_PIXELMAP           *pixelmap = GX_NULL;
GX_FILL_PIXELMAP_INFO  info;
GX_DISPLAY            *display = context->display;
GX_RECTANGLE          *clip = context->clip;
INT                    skip_line;

    if (num <= GX_POLYGON_MAX_EDGE_NUM)
    {
        brush = &context->brush;
        if (brush->style & GX_BRUSH_PIXELMAP_FILL)
        {
            if (brush->pixelmap == GX_NULL)
            {
                return;
            }

            /* Pick up brush pixelmap. */
            pixelmap = brush->pixelmap;

            if (pixelmap->format != display->color_format)
            {
                /* Display driver only support its native format pixelmap.*/
                /* Nothing should be drawn if pixelmap format isn't support. */
                return;
            }

            memset(&info, 0, sizeof(GX_FILL_PIXELMAP_INFO));

            info.pixelmap = brush->pixelmap;
            info.current_pixel_ptr = (GX_UBYTE *)info.pixelmap->data;

            if (pixelmap->aux_data_size)
            {
                info.current_aux_ptr = (GX_UBYTE *)pixelmap->aux_data;
            }
        }

        /* Get the top most and bottom most y coordinate of the specified polygon. */
        _gx_display_driver_polygon_min_max_get(vertex, num, &xmin, &xmax, &ypos);

        if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
        {
            /* Initiate new edge table. */
            _gx_display_driver_polygon_new_edge_table_init_90(vertex, num, net, &net_size);

            if (pixelmap)
            {
                skip_line = (xmax - xmin + 1) % pixelmap->width;

                if (skip_line)
                {
                    skip_line = pixelmap->width - skip_line;

                    info.draw = GX_FALSE;
                    while (skip_line--)
                    {
                        display->driver_horizontal_pixelmap_line_draw(context, 0, 0, 0, &info);
                    }
                }
            }
        }
        else
        {
            _gx_display_driver_polygon_new_edge_table_init_270(vertex, num, net, &net_size);
        }

        if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
        {
            xstart = xmax;
            xsign = -1;
        }
        else
        {
            xstart = xmin;
            xsign = 1;
        }

        /* Moving scan line from polygon top to bottom. */
        for (x = xmin; x <= xmax; x++)
        {
            /* This function will check if the scan line comes to the top most y coordinate of any edges in net table.
               If it is true, add it to the aet table. */
            _gx_display_driver_polygon_active_edge_insert(aet, net, xstart, net_size, &aet_size);

            if ((xstart < clip->left) ||
                (xstart > clip->right))
            {
                if (pixelmap)
                {
                    /* skip the undraw lines. */
                    info.draw = GX_FALSE;
                    display->driver_horizontal_pixelmap_line_draw(context, 0, 0, 0, &info);
                }
            }
            else
            {
                if (pixelmap)
                {
                    info.draw = GX_TRUE;
                    _gx_display_driver_polygon_pixelmap_scan_line_draw(context, aet, aet_size, ypos, xstart, &info);
                }
                else
                {
                    /* Filling the polygon area that touched by scan line. */
                    _gx_display_driver_polygon_scan_line_draw(context, aet, xstart, aet_size);
                }
            }
            /* This function will check if the scan line comes to the bottom most y coordinate of any edges in aet table.
               If it is true, remove it from aet table. */
            _gx_display_driver_polygon_non_active_edge_remove(aet, xstart, &aet_size);

            /* Update x coordinate of intersections between polygon and scan line, and sort them by increase x coordinate value. */
            _gx_display_driver_polygon_active_edge_table_update_and_restore(aet, aet_size);

            xstart += xsign;
        }
    }
}

