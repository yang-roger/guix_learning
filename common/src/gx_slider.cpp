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
/**   Slider Management (Slider)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_slider.h"

#include "gx_canvas.h"
#include "gx_context.h"
#include "gx_fixed_val.h"

static void gx_slider_draw_(GX_SLIDER* slider)
{
    slider->draw_();
}

static UINT gx_slider_event_process_(GX_SLIDER* slider, GX_EVENT* event_ptr)
{
    return slider->event_process_(event_ptr);
}

UINT GX_SLIDER::create_(const GX_CHAR* name, GX_WIDGET* parent,
                        INT tick_count, const GX_SLIDER_INFO* slider_info,
                        ULONG style, USHORT slider_id, const GX_RECTANGLE* size)
{
    GX_WIDGET::create_(name, GX_NULL, style, slider_id, size);

    this->type = GX_TYPE_SLIDER;

    info = *slider_info;

    const GX_VALUE GX_SLIDER_NEEDLE_WIDTH = 5;

    if (info.needle_width <= 0)
    {
        if (status & GX_STYLE_SLIDER_VERTICAL)
        {
            info.needle_width = (GX_VALUE)(width_() / 2);
        }
        else
        {
            info.needle_width = GX_SLIDER_NEEDLE_WIDTH;
        }
    }

    if (info.needle_height <= 0)
    {
        if (status & GX_STYLE_SLIDER_VERTICAL)
        {
            info.needle_height = GX_SLIDER_NEEDLE_WIDTH;
        }
        else
        {
            info.needle_height = (GX_VALUE)(height_() / 2);
        }
    }

    this->normal_fill_color = GX_COLOR_ID_BUTTON_UPPER;
    this->selected_fill_color = GX_COLOR_ID_BUTTON_UPPER;

    this->tick_color = GX_COLOR_ID_SLIDER_TICK;
    this->tick_count = tick_count;

    this->draw_function = (void (*)(GX_WIDGET*))gx_slider_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_slider_event_process_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_SLIDER::info_set_(const GX_SLIDER_INFO* slider_info)
{
    info = *slider_info;

    dirty_mark_();
}

void GX_SLIDER::value_set_(GX_SLIDER_INFO* slider_info, INT new_value)
{
    if (new_value != slider_info->current_val)
    {
        GX_RECTANGLE old_pos;
        needle_position_get_(slider_info, &old_pos);

        slider_info->current_val = new_value;

        GX_RECTANGLE new_pos;
        needle_position_get_(slider_info, &new_pos);

        new_pos.combine_(old_pos);

        if (type == GX_TYPE_PIXELMAP_SLIDER)
        {
            new_pos.left = left_();
            new_pos.right = right_();
        }

        dirty_mark_(&new_pos);

        signal_parent_(GX_EVENT_SLIDER_VALUE, new_value);
    }
}

void GX_SLIDER::needle_position_get_(const GX_SLIDER_INFO* slider_info, GX_RECTANGLE* return_position) const
{
    INT shift = 0;
    INT hotspot;

    if (slider_info->max_val > slider_info->min_val)
    {
        INT min_travel;
        INT max_travel;
        travel_get_(slider_info, &min_travel, &max_travel);

        shift = slider_info->min_travel + (max_travel - min_travel) *
            (slider_info->current_val - slider_info->min_val) / (slider_info->max_val - slider_info->min_val);
    }

    if (return_position)
    {
        if (style & GX_STYLE_SLIDER_VERTICAL)
        {
            hotspot = bottom_() - shift;

            return_position->left = (GX_VALUE)(left_() + slider_info->needle_inset);
            return_position->top = (GX_VALUE)(hotspot - slider_info->needle_hotspot_offset);

            if (slider_info->needle_width > 0)
            {
                return_position->right = (GX_VALUE)(return_position->left + slider_info->needle_width - 1);
                return_position->bottom = (GX_VALUE)(return_position->top + slider_info->needle_height - 1);
            }
            else
            {
                return_position->right = right_();
                return_position->bottom = return_position->top;
            }
        }
        else
        {
            hotspot = left_() + shift;

            return_position->top = (GX_VALUE)(top_() + slider_info->needle_inset);
            return_position->left = (GX_VALUE)(hotspot - slider_info->needle_hotspot_offset);

            if (slider_info->needle_width > 0)
            {
                return_position->bottom = (GX_VALUE)(return_position->top + slider_info->needle_height - 1);
                return_position->right = (GX_VALUE)(return_position->left + slider_info->needle_width - 1);
            }
            else
            {
                return_position->bottom = bottom_();
                return_position->right = return_position->left;
            }
        }
    }
}

void GX_SLIDER::travel_get_(const GX_SLIDER_INFO* slider_info, INT* return_min_travel, INT* return_max_travel) const
{
    INT min_travel;
    INT max_travel;

    if (style & GX_STYLE_SLIDER_VERTICAL)
    {
        min_travel = top_() + slider_info->max_travel;
        max_travel = bottom_() - slider_info->min_travel;
    }
    else
    {
        min_travel = left_() + slider_info->min_travel;
        max_travel = right_() - slider_info->max_travel;
    }

    *return_min_travel = min_travel;
    *return_max_travel = max_travel;
}

void GX_SLIDER::position_to_value_(GX_SLIDER_INFO* slider_info, INT new_position)
{
    INT min_travel;
    INT max_travel;
    INT new_value;

    travel_get_(slider_info, &min_travel, &max_travel);

    if (new_position < min_travel)
    {
        new_position = min_travel;
    }
    else if (new_position > max_travel)
    {
        new_position = max_travel;
    }

    if (max_travel > min_travel)
    {
        // new_value = slider_info->min_val + (slider_info->max_val - slider_info->min_val) *
        //                                    diff_travel / (max_travel - min_travel)

        new_value = (style & GX_STYLE_SLIDER_VERTICAL) ?
            (max_travel - new_position) : (new_position - min_travel);

        new_value = GX_FIXED_VAL_MAKE(new_value);

        new_value *= (slider_info->max_val - slider_info->min_val);
        new_value /= (max_travel - min_travel);

        new_value = GX_FIXED_VAL_RND(new_value);

        new_value += slider_info->min_val;

        if (new_value != slider_info->current_val)
        {
            value_set_(slider_info, new_value);
        }
    }
}

void GX_SLIDER::tickmarks_draw_()
{
    if (tick_count <= 1)
    {
        return;
    }

    INT mintravel;
    INT maxtravel;
    INT tickstart;
    INT tickend;
    INT space;

    // make a border 1/8 of widget size around tickmarks

    if (style & GX_STYLE_SLIDER_VERTICAL)
    {
        mintravel = left_();
        maxtravel = right_();
        space = (maxtravel - mintravel) / 8;

        tickstart = mintravel + space;
        tickend = (maxtravel + mintravel) / 2 - space;
    }
    else
    {
        mintravel = top_();
        maxtravel = bottom_();
        space = (maxtravel - mintravel) / 8;

        tickstart = (maxtravel + mintravel) / 2 + space;
        tickend = maxtravel - space;
    }

    _gx_context_line_color_set(this->tick_color);
    _gx_context_brush_width_set(1);

    travel_get_(&mintravel, &maxtravel);

    INT tickcount = this->tick_count;
    space = GX_FIXED_VAL_MAKE(maxtravel - mintravel) / (tickcount - 1);

    GX_FIXED_VAL tickpos = GX_FIXED_VAL_MAKE(mintravel);

    while (tickcount > 0)
    {
        mintravel = GX_FIXED_VAL_RND(tickpos);

        if (this->style & GX_STYLE_SLIDER_VERTICAL)
        {
            _gx_canvas_line_draw((GX_VALUE)tickstart, (GX_VALUE)mintravel,
                                 (GX_VALUE)tickend, (GX_VALUE)mintravel);
        }
        else
        {
            _gx_canvas_line_draw((GX_VALUE)mintravel, (GX_VALUE)tickstart,
                                 (GX_VALUE)mintravel, (GX_VALUE)tickend);
        }

        tickpos += space;
        --tickcount;
    }
}

void GX_SLIDER::needle_draw_()
{
    GX_DRAW_CONTEXT* context = gx_context_current_();

    context->brush.width = 1;

    GX_VALUE left;
    GX_VALUE top;
    GX_VALUE right;
    GX_VALUE bottom;

    // draw the line on which needle slides

    if (style & GX_STYLE_SLIDER_VERTICAL)
    {
        top = (GX_VALUE)(top_() + info.needle_height);
        bottom = (GX_VALUE)(bottom_() - info.needle_height);

        left = (GX_VALUE)((left_() + right_()) / 2);

        context->line_color_set_(GX_COLOR_ID_SLIDER_GROOVE_TOP);
        _gx_canvas_line_draw(left, top, left, bottom);

        left = (GX_VALUE)(left + 1);

        context->line_color_set_(GX_COLOR_ID_SLIDER_GROOVE_BOTTOM);
        _gx_canvas_line_draw(left, top, left, bottom);
    }
    else
    {
        left = (GX_VALUE)(left_() + info.needle_width);
        right = (GX_VALUE)(right_() - info.needle_width);

        top = (GX_VALUE)((top_() + bottom_()) / 2);

        context->line_color_set_(GX_COLOR_ID_SLIDER_GROOVE_TOP);
        _gx_canvas_line_draw(left, top, right, top);

        top = (GX_VALUE)(top + 1);

        context->line_color_set_(GX_COLOR_ID_SLIDER_GROOVE_BOTTOM);
        _gx_canvas_line_draw(left, top, right, top);
    }

    // draw the needle itself

    GX_RECTANGLE need_size;
    needle_position_get_(&info, &need_size);

    context->brush_set_(GX_COLOR_ID_SLIDER_NEEDLE_OUTLINE,
                        GX_COLOR_ID_SLIDER_NEEDLE_FILL,
                        GX_BRUSH_SOLID_FILL);
    _gx_canvas_rectangle_draw(&need_size);

    if (this->style & GX_STYLE_SLIDER_VERTICAL)
    {
        left = (GX_VALUE)(need_size.left + 1);
        right = (GX_VALUE)(need_size.right - 1);

        top = (GX_VALUE)(need_size.top + 2);

        context->line_color_set_(GX_COLOR_ID_SLIDER_NEEDLE_LINE1);
        _gx_canvas_line_draw(left, top, right, top);

        top = (GX_VALUE)(top + 1);

        context->line_color_set_(GX_COLOR_ID_SLIDER_NEEDLE_LINE2);
        _gx_canvas_line_draw(left, top, right, top);
    }
    else
    {
        top = (GX_VALUE)(need_size.top + 1);
        bottom = (GX_VALUE)(need_size.bottom - 1);

        left = (GX_VALUE)(need_size.left + 2);

        context->line_color_set_(GX_COLOR_ID_SLIDER_NEEDLE_LINE1);
        _gx_canvas_line_draw(left, top, left, bottom);

        left = (GX_VALUE)(left + 1);

        context->line_color_set_(GX_COLOR_ID_SLIDER_NEEDLE_LINE2);
        _gx_canvas_line_draw(left, top, left, bottom);
    }
}

void GX_SLIDER::draw_()
{
    GX_RESOURCE_ID fill = fill_color_();
    border_and_background_draw_(GX_COLOR_ID_BUTTON_BORDER, fill, fill, GX_TRUE);

    if (style & GX_STYLE_SHOW_TICKMARKS)
    {
        tickmarks_draw_();
    }

    if (style & GX_STYLE_SHOW_NEEDLE)
    {
        needle_draw_();
    }

    children_draw_();
}

void GX_SLIDER::position_to_value_(const GX_POINT& point)
{
    GX_VALUE pen_pos = (style & GX_STYLE_SLIDER_VERTICAL) ? point.y : point.x;
    position_to_value_((INT)pen_pos);
}

UINT GX_SLIDER::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_PEN_DOWN:
        style |= GX_STYLE_BUTTON_PUSHED;
        position_to_value_(event_ptr->payload.pointdata);
        break;

    case GX_EVENT_PEN_DRAG:
        if (style & GX_STYLE_BUTTON_PUSHED)
        {
            position_to_value_(event_ptr->payload.pointdata);
        }
        break;

    case GX_EVENT_PEN_UP:
        style &= ~GX_STYLE_BUTTON_PUSHED;
        break;

    default:
        result = GX_WIDGET::event_process_(event_ptr);
        break;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_slider_create                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service creates a slider.                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider control block          */
/*    name                                  Name of prompt                */
/*    parent                                Parent widget control block   */
/*    tick_count                            Number of slider ticks        */
/*    slider_info                           Pointer to slider info        */
/*    style                                 Style of slider               */
/*    slider_id                             Application-defined ID of     */
/*                                            slider                      */
/*    size                                  Dimensions of slider          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memset                                Set control block to zero     */
/*    _gx_widget_create                     Create the underlying widget  */
/*    _gx_widget_width_get                  Retrieve the width of the     */
/*                                            widget                      */
/*    _gx_widget_height_get                 Retrieve the height of the    */
/*                                            widget                      */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_slider_create(GX_SLIDER* slider, const GX_CHAR* name,
                       GX_WIDGET* parent, INT tick_count,
                       GX_SLIDER_INFO* slider_info, ULONG style,
                       USHORT slider_id, const GX_RECTANGLE* size)
{
    return slider->create_(name, parent, tick_count, slider_info, style, slider_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_slider_draw                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws the specified slider.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Widget control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_color_get                  Get the color by resource ID  */
/*    _gx_widget_border_draw                Draw border                   */
/*    _gx_slider_tickmarks_draw             Draw tickmarks                */
/*    _gx_slider_needle_draw                Draw the slider needle        */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_slider_draw(GX_SLIDER* slider)
{
    slider->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_slider_event_process                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service processes a slider event.                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider widget control block   */
/*    event                                 Pointer to event process      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_value_calculate            Calculate the slider value    */
/*    _gx_widget_event_process              Default widget event process  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_slider_event_process(GX_SLIDER* slider, GX_EVENT* event_ptr)
{
    return slider->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_slider_info_set                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reset the slider information structure of a GX_SLIDER */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Widget control block          */
/*    slider_info                           Slider info block             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_slider_info_set(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info)
{
    slider->info_set_(slider_info);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_slider_needle_draw                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws a slider needle.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider widget control block   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_needle_position_get        Get the position of the       */
/*                                            slider needle               */
/*    _gx_context_line_color_set            Set the line color for the    */
/*                                            context                     */
/*    _gx_context_brush_width_set           Set the brush width of the    */
/*                                            context                     */
/*    _gx_canvas_line_draw                  Draw a line on the canvas     */
/*    _gx_context_brush_define              Define the brush of the       */
/*                                            context                     */
/*    _gx_canvas_rectangle_draw             Draw a rectangle              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_slider_needle_draw(GX_SLIDER* slider)
{
    slider->needle_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_slider_needle_position_get                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the slider needle position.                       */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider widget control block   */
/*    slider_info                           Pointer to slider information */
/*                                            structure defining the      */
/*                                            slider limits, needle size  */
/*                                            and offset, and other       */
/*                                            slider parameters.          */
/*    return_position                       Pointer to destination for    */
/*                                            needle position             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_travel_get                 Gets the slider travel        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_slider_needle_position_get(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info, GX_RECTANGLE* return_position)
{
    slider->needle_position_get_(slider_info, return_position);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_slider_tickmarks_draw                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws the slider tickmarks.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider widget control block   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_travel_get                 Get the slider travel         */
/*    _gx_context_line_color_set            Set the line color for the    */
/*                                            drawing context             */
/*    _gx_context_brush_width_set           Set the brush width for the   */
/*                                            drawing context             */
/*    _gx_canvas_line_draw                  Draw a line                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_slider_tickmarks_draw(GX_SLIDER* slider)
{
    slider->tickmarks_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_slider_travel_get                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the slider travel.                                */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider control block          */
/*    slider_info                           Pointer to slider info        */
/*                                            structure                   */
/*    return_min_travel                     Pointer to destination for    */
/*                                            for minimum travel value    */
/*    return_max_travel                     Pointer to destination for    */
/*                                            for maximum travel value    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_slider_travel_get(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info, INT* return_min_travel, INT* return_max_travel)
{
    slider->travel_get_(slider_info, return_min_travel, return_max_travel);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_slider_value_calculate                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service calculates the slider value.                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider widget control block   */
/*    slider_info                           Pointer to slider info        */
/*    new_position                          New slider position           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_travel_get                 Get the slider travel         */
/*    _gx_slider_value_set                  Set the slider value          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_slider_value_calculate(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info, INT newpos)
{
    slider->position_to_value_(slider_info, newpos);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_slider_value_set                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the slider value.                                 */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider widget control block   */
/*    slider_info                           Pointer to slider info        */
/*    new_value                             New slider value              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_needle_position_get        Get the slider needle         */
/*                                            position                    */
/*    _gx_utility_rectangle_combine         Combine the first and second  */
/*                                            rectangle into the first    */
/*                                            rectangle                   */
/*    _gx_system_dirty_partial_add          Mark the partial area of this */
/*                                            widget as dirty             */
/*    _gx_widget_event_generate             Generate an event and send it */
/*                                            to the parent widget        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_slider_value_set(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info, INT new_value)
{
    slider->value_set_(slider_info, new_value);

    return GX_SUCCESS;
}
