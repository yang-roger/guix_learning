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
/**   Line Chart Management (Line Chart)                                  */
/**                                                                       */
/**************************************************************************/

#include "gx_line_chart.h"

#include "gx_fixed_val.h"
#include "gx_context.h"
#include "gx_canvas.h"

static void gx_line_chart_draw_(GX_LINE_CHART* chart)
{
    chart->draw_();
}

UINT GX_LINE_CHART::create_(const GX_CHAR* name, GX_WIDGET* parent,
                            const GX_LINE_CHART_INFO* info,
                            ULONG style, USHORT chart_id, const GX_RECTANGLE* size)
{
    GX_WINDOW::create_(name, GX_NULL, style, chart_id, size);

    this->type = GX_TYPE_LINE_CHART;

    this->info = *info;

    this->draw_function = (void (*)(GX_WIDGET*))gx_line_chart_draw_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_LINE_CHART::update_(INT* data, INT data_count)
{
    info.data = data;
    info.active_data_count = (GX_VALUE)data_count;

    dirty_mark_();
}

INT GX_LINE_CHART::y_scale_calculate_() const
{
    INT y_scale = content_height_() - info.axis_line_width;

    y_scale = GX_FIXED_VAL_MAKE(y_scale);

    if (info.max_val != info.min_val)
    {
        y_scale /= (info.max_val - info.min_val);
    }

    return y_scale;
}

void GX_LINE_CHART::draw_()
{
    GX_WINDOW::draw_();
    axis_draw_();
    data_draw_();
}

void GX_LINE_CHART::axis_draw_()
{
    GX_DRAW_CONTEXT* context = gx_context_current_();

    context->brush_set_(info.axis_color, info.axis_color, GX_BRUSH_OUTLINE);
    context->brush.width = (UINT)info.axis_line_width;

    GX_RECTANGLE chart_bound;
    content_rectangle_(&chart_bound);

    // draw y axis in left side
    GX_VALUE pos = (GX_VALUE)(chart_bound.left + info.axis_line_width / 2);
    _gx_canvas_line_draw(pos, chart_bound.top, pos, (GX_VALUE)(chart_bound.bottom - info.axis_line_width + 1));

    // draw x axis in bottom side
    pos = (GX_VALUE)(chart_bound.bottom - info.axis_line_width / 2);
    _gx_canvas_line_draw(chart_bound.left, pos, chart_bound.right, pos);
}

void GX_LINE_CHART::data_draw_()
{
    if (info.active_data_count <= 0 || !info.data)
    {
        return;
    }

    GX_RECTANGLE chart_bound;
    content_rectangle_(&chart_bound);

    INT x_step = chart_bound.right - chart_bound.left - info.axis_line_width;
    x_step = GX_FIXED_VAL_MAKE(x_step);
    if (info.max_data_count > 0)
    {
        x_step /= info.max_data_count;
    }

    INT y_scale = y_scale_calculate_();

    INT last_x_pos = GX_FIXED_VAL_MAKE(chart_bound.left + info.axis_line_width + 1);
    INT last_y_pos = y_pos_for_value_(info.data[0], y_scale, chart_bound.bottom);

    GX_DRAW_CONTEXT* context = gx_context_current_();

    context->brush_set_(info.line_color, info.line_color, GX_BRUSH_ALIAS | GX_BRUSH_ROUND);
    context->brush.width = (UINT)info.data_line_width;

    INT x_pos;
    INT y_pos;

    for (INT i = 1; i < info.active_data_count; ++i)
    {
        x_pos = last_x_pos + x_step;
        y_pos = y_pos_for_value_(info.data[i], y_scale, chart_bound.bottom);

        _gx_canvas_line_draw((GX_VALUE)(GX_FIXED_VAL_TO_INT(last_x_pos)), (GX_VALUE)last_y_pos,
                             (GX_VALUE)(GX_FIXED_VAL_TO_INT(x_pos)), (GX_VALUE)y_pos);

        last_x_pos = x_pos;
        last_y_pos = y_pos;
    }
}

INT GX_LINE_CHART::y_pos_for_value_(INT value, INT y_scale, GX_VALUE chart_bottom) const
{
    INT dy = (value - info.min_val) * y_scale;

    return (INT)chart_bottom - GX_FIXED_VAL_TO_INT(dy);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_line_chart_axis_draw                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the x,y chart axis of a line chart              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    chart                                 Line chart                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_brush_define                                            */
/*    _gx_context_brush_width_set                                         */
/*    _gx_canvas_line_draw                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
void _gx_line_chart_axis_draw(GX_LINE_CHART* chart)
{
    chart->axis_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_line_chart_create                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a GX_LINE_CHART widget                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    chart                                 GX_LINE_CHART control block   */
/*    name                                  Logical name of icon widget   */
/*    parent                                Pointer to the parent widget  */
/*    info                                  chart drawing parameters      */
/*    style                                 chart style flags             */
/*    chart_id                              chart ID                      */
/*    size                                  chart size                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_create                                                   */
/*    _gx_widget_link                                                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_line_chart_create(GX_LINE_CHART* chart,
                           const GX_CHAR* name,
                           GX_WIDGET* parent,
                           const GX_LINE_CHART_INFO* info,
                           ULONG style,
                           USHORT chart_id,
                           const GX_RECTANGLE* size)
{
    return chart->create_(name, parent, info, style, chart_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_line_chart_data_draw                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the chart data line                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    chart                                 Line chart                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_line_chart_y_scale_calculate                                    */
/*    _gx_context_brush_width_set                                         */
/*    _gx_context_brush_define                                            */
/*    _gx_canvas_line_draw                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
void _gx_line_chart_data_draw(GX_LINE_CHART* chart)
{
    chart->data_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_line_chart_draw                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws a line chart                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    chart                                 Line chart control block      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_draw                       Draw Window                   */
/*    _gx_line_chart_axis_draw              Draw line chart axis          */
/*    _gx_line_chart_data_draw              Draw data in line chart       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
void _gx_line_chart_draw(GX_LINE_CHART* chart)
{
    chart->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_line_chart_update                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function updates the data array associated with a line chart   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    chart                                 GX_LINE_CHART control block   */
/*    data                                  Data array                    */
/*    data_count                            Data array size               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                          Mark widget dirty             */
/*    _gx_system_dirty_mark                                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_line_chart_update(GX_LINE_CHART* chart, INT* data, INT data_count)
{
    chart->update_(data, data_count);

    return GX_SUCCESS;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_line_chart_y_scale_calculate                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function calculates chart y axis scale value                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    chart                                 Line chart                    */
/*    return_value                          location to pass return value */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    none                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_line_chart_y_scale_calculate(GX_LINE_CHART* chart, INT* return_val)
{
    *return_val = chart->y_scale_calculate_();

    return GX_SUCCESS;
}
