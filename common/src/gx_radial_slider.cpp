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
/**   Radial Slider Management (Slider)                                   */
/**                                                                       */
/**************************************************************************/

#include "gx_radial_slider.h"

#include "gx_fixed_val.h"
#include "gx_utility.h"
#include "gx_system.h"
#include "gx_canvas.h"
#include "gx_pixelmap.h"

static void gx_radial_slider_draw_(GX_RADIAL_SLIDER* slider)
{
    slider->draw_();
}

static UINT gx_radial_slider_event_process_(GX_RADIAL_SLIDER* slider, GX_EVENT* event_ptr)
{
    return slider->event_process_(event_ptr);
}

UINT GX_RADIAL_SLIDER::create_(const GX_CHAR* name, GX_WIDGET* parent,
                               const GX_RADIAL_SLIDER_INFO* info, ULONG style,
                               USHORT slider_id, const GX_RECTANGLE* size)
{
    GX_WIDGET::create_(name, GX_NULL, style, slider_id, size);

    this->type = GX_TYPE_RADIAL_SLIDER;

    this->info = *info;

    this->start_angle = 0;
    this->target_angle = 0;

    this->animation_total_steps = 15;
    this->animation_step = 0;
    this->animation_delay = 2;
    this->animation_style = GX_ANIMATION_CIRC_EASE_IN_OUT;

    this->status |= GX_STATUS_ANIMATION_NONE;

    this->draw_function = (void (*)(GX_WIDGET*))gx_radial_slider_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_radial_slider_event_process_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_RADIAL_SLIDER::info_set_(const GX_RADIAL_SLIDER_INFO* info)
{
    this->info = *info;

    dirty_mark_if_visible_();
}

void GX_RADIAL_SLIDER::angle_set_(GX_VALUE new_angle)
{
    if (info.current_angle != new_angle)
    {
        info.current_angle = new_angle;

        signal_parent_(GX_EVENT_SLIDER_VALUE, new_angle);

        dirty_mark_if_visible_();
    }
}

void GX_RADIAL_SLIDER::anchor_angles_set_(GX_VALUE* anchor_values, USHORT anchor_count)
{
    info.angle_list = anchor_values;
    info.list_count = anchor_count;
}

void GX_RADIAL_SLIDER::pixelmap_set_(GX_RESOURCE_ID background_pixelmap, GX_RESOURCE_ID needle_pixelmap)
{
    info.background_pixelmap = background_pixelmap;
    info.needle_pixelmap = needle_pixelmap;

    dirty_mark_if_visible_();
}

void GX_RADIAL_SLIDER::animation_set_(USHORT steps, USHORT delay, USHORT animation_style,
                                      void (*animation_update_callback)(GX_RADIAL_SLIDER* slider))
{
    this->animation_total_steps = steps;
    this->animation_delay = delay;
    this->animation_style = animation_style;
    this->animation_update_callback = animation_update_callback;
}

void GX_RADIAL_SLIDER::animation_start_(GX_VALUE target_angle)
{
    if (target_angle != info.current_angle)
    {
        if (this->animation_total_steps > 1 && this->animation_delay)
        {
            this->start_angle = info.current_angle;
            this->target_angle = target_angle;

            this->animation_step = this->animation_total_steps;

            if (timer_start_(GX_RADIAL_SLIDER_TIMER, animation_delay, animation_delay) == GX_SUCCESS)
            {
                signal_parent_(GX_EVENT_ANIMATION_START, target_angle);
            }
        }
        else
        {
            angle_set_(target_angle);
        }
    }
}

void GX_RADIAL_SLIDER::animation_update_()
{
    if (animation_step > 0)
    {
        --animation_step;

        INT current_val = info.current_angle;

        _gx_utility_easing_function_calculate(animation_style,
                                              start_angle,
                                              target_angle,
                                              animation_total_steps - animation_step,
                                              animation_total_steps,
                                              &current_val);

        angle_set_((GX_VALUE)current_val);

        if (animation_update_callback)
        {
            animation_update_callback(this);
        }

        if (animation_step == 0)
        {
            timer_stop_(GX_RADIAL_SLIDER_TIMER);

            signal_parent_(GX_EVENT_ANIMATION_COMPLETE, current_val);
        }

        dirty_mark_if_visible_();
    }
}

void GX_RADIAL_SLIDER::needle_rectangle_calculate_(GX_RECTANGLE* rectangle)
{
    GX_PIXELMAP* map;
    pixelmap_get_(info.needle_pixelmap, &map);
    if (map)
    {
        INT xpos = _gx_utility_math_cos(GX_FIXED_VAL_MAKE(info.current_angle));
        INT ypos = _gx_utility_math_sin(GX_FIXED_VAL_MAKE(info.current_angle));

        // calculate sin and cos value of current angle value.
        xpos = GX_FIXED_VAL_RND((info.radius + info.needle_offset) * xpos);
        ypos = GX_FIXED_VAL_RND((info.radius + info.needle_offset) * ypos);

        // calculate needle position.
        xpos += info.xcenter;
        ypos = info.ycenter - ypos;

        xpos -= map->width / 2;
        ypos -= map->height / 2;

        xpos += left_();
        ypos += top_();

        // define needle rectangle.
        rectangle->set_((GX_VALUE)xpos, (GX_VALUE)ypos,
                        (GX_VALUE)(xpos + map->width),
                        (GX_VALUE)(ypos + map->height));
    }
    else
    {
        rectangle->set_(0, 0, 0, 0);
    }
}

void GX_RADIAL_SLIDER::draw_()
{
    border_and_background_draw_();

    GX_DRAW_CONTEXT *context = gx_context_current_();

    GX_PIXELMAP* map;

    context->pixelmap_get_(info.background_pixelmap, &map);
    if (map)
    {
        _gx_canvas_pixelmap_draw(left_(), top_(), map);
    }

    context->pixelmap_get_(info.needle_pixelmap, &map);
    if (map)
    {
        GX_RECTANGLE rect;
        needle_rectangle_calculate_(&rect);

        _gx_canvas_pixelmap_draw(rect.left, rect.top, map);
    }

    children_draw_();
}

UINT GX_RADIAL_SLIDER::angle_calculate_(GX_POINT point, GX_VALUE* return_value)
{
    GX_POINT center;
    center.x = (GX_VALUE)(left_() + info.xcenter);
    center.y = (GX_VALUE)(top_() + info.ycenter);

    INT new_value;
    if (_gx_utility_radial_angle_calculate(point, center, &new_value) != GX_SUCCESS)
    {
        *return_value = 0;
        return GX_FAILURE;
    }

    // Range value inside specified [-90, 270]
    if (new_value > 270)
    {
        new_value -= 360;
    }

    *return_value = (GX_VALUE)new_value;

    return GX_SUCCESS;
}

void GX_RADIAL_SLIDER::adjust_to_nearest_anchor_(GX_VALUE* angle_value)
{
    GX_VALUE nearest_anchor_value = _gx_utility_value_clamp(*angle_value, info.min_angle, info.max_angle);

    if (info.angle_list)
    {
        const GX_VALUE* angle_list = info.angle_list;

        GX_VALUE min_value;
        GX_VALUE max_value;

        for (INT i = 0; i < info.list_count; ++i)
        {
            if (i == 0)
            {
                min_value = angle_list[i];
            }
            else
            {
                min_value = (GX_VALUE)((angle_list[i] + angle_list[i - 1]) / 2);
            }

            if (i == info.list_count - 1)
            {
                max_value = angle_list[i];
            }
            else
            {
                max_value = (GX_VALUE)((angle_list[i] + angle_list[i + 1]) / 2);
            }

            if (min_value > max_value)
            {
                GX_SWAP_VALS(min_value, max_value);
            }

            if (min_value <= nearest_anchor_value && nearest_anchor_value <= max_value)
            {
                nearest_anchor_value = angle_list[i];
                break;
            }
        }
    }

    *angle_value = nearest_anchor_value;
}

UINT GX_RADIAL_SLIDER::pen_down_event_process_(GX_EVENT* event_ptr)
{
    if (is_enabled_() && (this->status & GX_STATUS_ANIMATION_NONE))
    {
        pendown_point = event_ptr->payload.pointdata;

        GX_RECTANGLE rect;
        needle_rectangle_calculate_(&rect);

        if (rect.contain_point_(pendown_point))
        {
            status_add_(GX_STATUS_TRACKING_PEN);

            _gx_system_input_capture(this);
        }
        else
        {
            GX_VALUE xpos = (GX_VALUE)(pendown_point.x - left_());
            GX_VALUE ypos = (GX_VALUE)(pendown_point.y - top_());

            INT dist = gx_point_distance_(xpos, ypos, info.xcenter, info.ycenter);

            if (info.radius - info.track_width / 2 <= dist && dist <= info.radius + info.track_width / 2)
            {
                _gx_system_input_capture(this);
            }
        }
    }
    else
    {
        parent_event_process_run_(event_ptr);
    }

    return GX_SUCCESS;
}

UINT GX_RADIAL_SLIDER::pen_drag_event_process_(GX_EVENT* event_ptr)
{
    if (this->status & GX_STATUS_TRACKING_PEN)
    {
        GX_VALUE new_value;
        angle_calculate_(event_ptr->payload.pointdata, &new_value);
        if ((info.min_angle <= new_value) && (new_value <= info.max_angle))
        {
            GX_VALUE previous_value;
            angle_calculate_(pendown_point, &previous_value);

            bool update = false;

            if ((new_value > previous_value) && (new_value - previous_value < 180))
            {
                // counterclockwise
                if (new_value > info.current_angle)
                {
                    update = true;
                }
            }
            else if (new_value < previous_value)
            {
                // clockwise
                if (new_value < info.current_angle)
                {
                    update = true;
                }
            }

            if (update)
            {
                angle_set_(new_value);
            }
        }

        pendown_point = event_ptr->payload.pointdata;
    }
    else
    {
        parent_event_process_run_(event_ptr);
    }

    return GX_SUCCESS;
}

UINT GX_RADIAL_SLIDER::pen_up_event_process_(GX_EVENT* event_ptr)
{
    if (this->status & GX_STATUS_OWNS_INPUT)
    {
        _gx_system_input_release(this);

        GX_POINT point = event_ptr->payload.pointdata;

        bool update = false;
        GX_VALUE new_value;

        if (this->status & GX_STATUS_TRACKING_PEN)
        {
            new_value = info.current_angle;
            update = true;
        }
        else if ((GX_ABS(point.x - pendown_point.x) < 5) && (GX_ABS(point.y - pendown_point.y) < 5))
        {
            angle_calculate_(point, &new_value);
            update = true;
        }

        if (update)
        {
            adjust_to_nearest_anchor_(&new_value);
            animation_start_(new_value);
        }

        status_add_(GX_STATUS_ANIMATION_NONE);
        status_remove_(GX_STATUS_TRACKING_PEN);
    }
    else
    {
        parent_event_process_run_(event_ptr);
    }

    return GX_SUCCESS;
}

UINT GX_RADIAL_SLIDER::event_process_(GX_EVENT* event_ptr)
{
    switch (event_ptr->type)
    {
    case GX_EVENT_PEN_DOWN:
        pen_down_event_process_(event_ptr);
        break;

    case GX_EVENT_PEN_DRAG:
        pen_drag_event_process_(event_ptr);
        break;

    case GX_EVENT_PEN_UP:
        pen_up_event_process_(event_ptr);
        break;

    case GX_EVENT_TIMER:
        if (event_ptr->payload.timer_id == GX_RADIAL_SLIDER_TIMER)
        {
            animation_update_();
        }
        else
        {
            GX_WIDGET::event_process_(event_ptr);
        }
        break;

    default:
        return GX_WIDGET::event_process_(event_ptr);
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_slider_anchor_angles_set                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets anchor angles for radial slider.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    anchor_angles                         The angle list to set, which  */
/*                                            defines anchor angles for   */
/*                                            radial slider               */
/*    anchor_count                          The count of the anchor angles*/
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
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_slider_anchor_angles_set(GX_RADIAL_SLIDER* slider, GX_VALUE* anchor_values, USHORT anchor_count)
{
    slider->anchor_angles_set_(anchor_values, anchor_count);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_slider_angle_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets new slider angle value.                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    new_angle                             New angle to be set           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_generate             Generate child signal         */
/*    _gx_system_dirty_mark                 Mark widget as dirty          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_slider_angle_set(GX_RADIAL_SLIDER* slider, GX_VALUE new_angle)
{
    slider->angle_set_(new_angle);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_slider_animation_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets animation steps, delay time and animation styles  */
/*    for a radial slider.                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    steps                                 Total steps for one animation */
/*    delay                                 Delay time for every step     */
/*    animation_style                       Easing function type          */
/*    animation_update_callback             Function that to be called    */
/*                                          after each animation step,    */
/*                                          could be NULL                 */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_slider_animation_set(GX_RADIAL_SLIDER* slider, USHORT steps,
                                     USHORT delay, USHORT animation_style,
                                     void (*animation_update_callback)(GX_RADIAL_SLIDER* slider))
{
    slider->animation_set_(steps, delay, animation_style, animation_update_callback);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_slider_animation_start                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service starts an animation to move the slider needle from     */
/*    current postion to the new position.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    target_angle                          Target angle value            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_timer_start                Start a timer                 */
/*    _gx_widget_event_generate             Generate child signal         */
/*    _gx_radial_slider_angle_set           Set new slider value          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_slider_animation_start(GX_RADIAL_SLIDER* slider, GX_VALUE target_angle)
{
    slider->animation_start_(target_angle);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_slider_create                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a radial slider, which is a special type of   */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    name                                  Name of radial slider         */
/*    parent                                Parent widget control block   */
/*    info                                  Pointer to radial slider info */
/*    style                                 Style of prompt               */
/*    slider_id                             Application-defined ID of     */
/*                                            radial slider               */
/*    size                                  Widget size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_create                     Create the underlying widget  */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_slider_create(GX_RADIAL_SLIDER* slider, const GX_CHAR* name,
                              GX_WIDGET* parent, GX_RADIAL_SLIDER_INFO* info,
                              ULONG style, USHORT slider_id, const GX_RECTANGLE* size)
{
    return slider->create_(name, parent, info, style, slider_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_slider_draw                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws a radial slider widget.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_draw                       Default widget draw           */
/*    _gx_context_pixelmap_get              Retrieve pixelmap with        */
/*                                            specified pixelmap id       */
/*    _gx_canvas_pixelmap_draw              Draw a pixelmap to canvas     */
/*    _gx_widget_children_draw              Draw widget children          */
/*    _gx_radial_slider_needle_rectangle_get                              */
/*                                          Retrive needle bounding       */
/*                                            rectangle                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
void _gx_radial_slider_draw(GX_RADIAL_SLIDER* slider)
{
    slider->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_slider_event_process                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service processes a radial slider event.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    event_ptr                             Pointer to event process      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_slider_pen_down_event_process                            */
/*                                          Process pen down event        */
/*    _gx_radial_slider_pen_drag_event_process                            */
/*                                          Process pen drag event        */
/*    _gx_radial_slider_pen_up_event_process                              */
/*                                          Process pen up event          */
/*    _gx_widget_event_process              Default widget event process  */
/*    _gx_radial_slider_animation_update    Update needle position        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_slider_event_process(GX_RADIAL_SLIDER* slider, GX_EVENT* event_ptr)
{
    return slider->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_slider_info_get                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves slider infomation.                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    info                                  Retrieved radial slider info  */
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
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_slider_info_get(GX_RADIAL_SLIDER* slider, GX_RADIAL_SLIDER_INFO** info)
{
    *info = &slider->info;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_slider_info_set                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets radial slider information.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    info                                  Pointer to radial slider info */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark widget as dirty          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_slider_info_set(GX_RADIAL_SLIDER* slider, GX_RADIAL_SLIDER_INFO* info)
{
    slider->info_set_(info);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_slider_pixelmap_set                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets background and needle pixelmap for radial slider.*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    background_pixelmap                   Resource id of background     */
/*                                            pixelmap                    */
/*    needle_pixelmap                       Resource id of needle pixelmap*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark widget as dirty          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_slider_pixelmap_set(GX_RADIAL_SLIDER* slider,
                                    GX_RESOURCE_ID background_pixelmap,
                                    GX_RESOURCE_ID needle_pixelmap)
{
    slider->pixelmap_set_(background_pixelmap, needle_pixelmap);

    return GX_SUCCESS;
}
