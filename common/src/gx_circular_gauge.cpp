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
/**   Circular Gauge Management (Circular Gauge)                          */
/**                                                                       */
/**************************************************************************/

#include "gx_circular_gauge.h"

#include "gx_utility.h"
#include "gx_display.h"
#include "gx_canvas.h"
#include "gx_context.h"

static void gx_circular_gauge_draw_(GX_CIRCULAR_GAUGE* gauge)
{
    gauge->draw_();
}

static UINT gx_circular_gauge_event_process_(GX_CIRCULAR_GAUGE* gauge, GX_EVENT* event_ptr)
{
    return gauge->event_process_(event_ptr);
}

UINT GX_CIRCULAR_GAUGE::create_(const GX_CHAR* name,
                                GX_WIDGET* parent,
                                GX_CIRCULAR_GAUGE_INFO* circular_gauge_info,
                                GX_RESOURCE_ID background,
                                ULONG style,
                                USHORT circular_gauge_id,
                                GX_VALUE xpos, GX_VALUE ypos)
{
    GX_ICON::create_(name, GX_NULL, background, style, circular_gauge_id, xpos, ypos);

    this->type = GX_TYPE_CIRCULAR_GAUGE;

    this->info = *circular_gauge_info;

    if (this->info.animation_delay <= 0)
    {
        this->info.animation_delay = GX_DEFAULT_CIRCULAR_GAUGE_ANIMATION_DELAY;
    }

    memset(&this->needle_rotated, 0, sizeof(GX_PIXELMAP));
    this->needle_source = GX_NULL;

    this->start_angle = 0;
    this->current_angle = 0;
    this->target_angle = 0;

    this->angle_increment = 0;
    this->animation_step = 0;
    this->current_needle_rectangle.set_(0, 0, 0, 0);

    this->status |= GX_STATUS_RESIZE_NOTIFY;

    this->draw_function = (void (*)(GX_WIDGET*))gx_circular_gauge_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_circular_gauge_event_process_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_CIRCULAR_GAUGE::animation_set_(INT steps, INT delay)
{
    info.animation_steps = steps;

    if (delay <= 0)
    {
        delay = GX_DEFAULT_CIRCULAR_GAUGE_ANIMATION_DELAY;
    }
    info.animation_delay = delay;

    angle_increment_calculate_();
}

void GX_CIRCULAR_GAUGE::angle_set_(INT angle)
{
    this->target_angle = angle;
    angle_increment_calculate_();
}

void GX_CIRCULAR_GAUGE::angle_increment_calculate_()
{
    start_angle = current_angle;
    angle_increment = GX_FIXED_VAL_MAKE(target_angle - current_angle);
    animation_step = 0;

    if (is_visible_() && angle_increment && info.animation_steps > 0)
    {
        angle_increment /= info.animation_steps;

        INT delay = info.animation_delay;
        if (delay <= 0)
        {
            delay = GX_DEFAULT_CIRCULAR_GAUGE_ANIMATION_DELAY;
            info.animation_delay = delay;
        }

        timer_start_(GX_CIRCULAR_GAUGE_TIMER, (UINT)delay, (UINT)delay);
    }
    else
    {
        current_angle = target_angle;

        dirty_mark_if_visible_();
    }
}

bool GX_CIRCULAR_GAUGE::needle_rectangle_calculate_(INT angle, GX_RECTANGLE* rect)
{
    if (!needle_source)
    {
        return false;
    }

    while (angle < 0)
    {
        angle += 360;
    }

    INT rot_cx = (INT)info.needle_xcor;
    INT rot_cy = (INT)info.needle_ycor;

    rect->left   = (GX_VALUE)(left_() + info.needle_xpos - rot_cx);
    rect->top    = (GX_VALUE)(top_()  + info.needle_ypos - rot_cy);
    rect->right  = (GX_VALUE)(rect->left + needle_source->width - 1);
    rect->bottom = (GX_VALUE)(rect->top + needle_source->height - 1);

    _gx_canvas_rotated_pixelmap_bound_calculate(rect, angle, rot_cx, rot_cy);

    return true;
}

void GX_CIRCULAR_GAUGE::needle_dirty_mark_()
{
    GX_RECTANGLE dirty_rect = current_needle_rectangle;

    if (needle_rectangle_calculate_(current_angle, &current_needle_rectangle))
    {
        dirty_rect.combine_(current_needle_rectangle);
    }

    dirty_mark_(&dirty_rect);
}

// rotate the |needle_source| pixelmap to the |needle_rotated| pixelmap
UINT GX_CIRCULAR_GAUGE::needle_rotate_()
{
    UINT result = GX_SUCCESS;

    if (needle_source)
    {
        INT xcor = (INT)info.needle_xcor;
        INT ycor = (INT)info.needle_ycor;

        current_needle_x = info.needle_xpos;
        current_needle_y = info.needle_ypos;

        result = _gx_utility_pixelmap_rotate(needle_source, current_angle, &needle_rotated, &xcor, &ycor);
        if (result == GX_SUCCESS)
        {
            // Calculate start position of needle pixelmap.
            current_needle_x = left_() + info.needle_xpos - xcor;
            current_needle_y = top_() + info.needle_ypos - ycor;
        }
    }

    return result;
}

static UINT gx_needle_rotate_callback_(void* gauge)
{
    return ((GX_CIRCULAR_GAUGE*)gauge)->needle_rotate_();
}

void GX_CIRCULAR_GAUGE::background_draw_()
{
    GX_ICON::background_draw_();

    if (info.needle_pixelmap)
    {
        _gx_context_pixelmap_get(info.needle_pixelmap, &needle_source);
    }

    if (current_angle != 0)
    {
        GX_DISPLAY* display = gx_context_current_()->display;
        if (display && display->driver_callback_assign)
        {
            display->driver_callback_assign(gx_needle_rotate_callback_, this);
        }
        else
        {
            needle_rotate_();
        }

        if (needle_rotated.data)
        {
            _gx_canvas_pixelmap_draw((GX_VALUE)current_needle_x, (GX_VALUE)current_needle_y, &needle_rotated);

            _gx_utility_pixelmap_free(&needle_rotated);
        }
    }
    else
    {
        current_needle_x = left_() + info.needle_xpos - info.needle_xcor;
        current_needle_y = top_() + info.needle_ypos - info.needle_ycor;

        if (needle_source && needle_source->data)
        {
            _gx_canvas_pixelmap_draw((GX_VALUE)current_needle_x, (GX_VALUE)current_needle_y, needle_source);
        }
    }
}

void GX_CIRCULAR_GAUGE::draw_()
{
    background_draw_();
    children_draw_();
}

UINT GX_CIRCULAR_GAUGE::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        result = GX_ICON::event_process_(event_ptr);
        if (!needle_source)
        {
            pixelmap_get_(info.needle_pixelmap, &needle_source);
            needle_rectangle_calculate_(current_angle, &current_needle_rectangle);
        }
        angle_increment_calculate_();
        break;

    case GX_EVENT_HIDE:
        if (info.animation_steps > 0)
        {
            timer_stop_(GX_CIRCULAR_GAUGE_TIMER);
        }
        result = GX_ICON::event_process_(event_ptr);
        break;

    case GX_EVENT_TIMER:
        if (event_ptr->payload.timer_id == GX_CIRCULAR_GAUGE_TIMER)
        {
            if (current_angle != target_angle)
            {
                ++animation_step;
                if (animation_step < info.animation_steps)
                {
                    current_angle = start_angle;
                    current_angle += GX_FIXED_VAL_TO_INT(angle_increment * animation_step);
                }
                else
                {
                    current_angle = target_angle;

                    timer_stop_(GX_CIRCULAR_GAUGE_TIMER);
                    animation_step = 0;

                    signal_parent_(GX_EVENT_ANIMATION_COMPLETE, 0);
                }

                needle_dirty_mark_();
            }
        }
        else
        {
            result = GX_WIDGET::event_process_(event_ptr);
        }
        break;

    case GX_EVENT_RESIZED:
        needle_rectangle_calculate_(current_angle, &current_needle_rectangle);
        break;

    default:
        result = GX_ICON::event_process_(event_ptr);
        break;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_circular_gauge_angle_get                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves the current needle angle of circular gauge. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gauge                                 Pointer to circular gauge     */
/*                                            control block               */
/*    angle                                 Current needle angle to be    */
/*                                            retrieved                   */
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
UINT _gx_circular_gauge_angle_get(GX_CIRCULAR_GAUGE* gauge, INT* angle)
{
    *angle = gauge->current_angle;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_circular_gauge_angle_set                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the target angle of a circular gauge widget.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gauge                                 Pointer to circular gauge     */
/*                                            control block               */
/*    target_angle                          Target needle angle           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_circular_gauge_dirty_mark         Mark the needle area as dirty */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_circular_gauge_angle_set(GX_CIRCULAR_GAUGE* gauge, INT angle)
{
    gauge->angle_set_(angle);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_circular_gauge_animation_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets animation steps delay time for a circular gauge. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gauge                                 Pointer to circular gauge     */
/*                                            control block.              */
/*    steps                                 Total steps for one rotation  */
/*    delay                                 Delay time for every step     */
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
UINT _gx_circular_gauge_animation_set(GX_CIRCULAR_GAUGE* gauge, INT steps, INT delay)
{
    gauge->animation_set_(steps, delay);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_circular_gauge_background_draw                                  */
/*                                                           6.1.9        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws background of the specified circular gauge.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gauge                                 Pointer to circular gauge     */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_icon_background_draw              Draw icon background          */
/*    _gx_context_pixelmap_get              Gets the pixelmap associated  */
/*                                            the supplied resource ID.   */
/*    _gx_circular_gauge_needle_rotate      Needle rotate function        */
/*    _gx_canvas_pixelmap_draw              Draw the specified pixelmap.  */
/*    _gx_system_memory_free                Free memory.                  */
/*    [gx_display_driver_callback_assign]   Assigns widget callback       */
/*                                            function                    */
/*    _gx_circular_gauge_needle_rotate_callback                           */
/*                                          Needle rotate callback        */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_circular_gauge_background_draw(GX_CIRCULAR_GAUGE* gauge)
{
    gauge->background_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_circular_gauge_create                                           */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This functon creates a circular gauge with the specified properties.*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gauge                                 Pointer to circular gauge     */
/*                                            control block               */
/*    name                                  Logical name of circular gauge*/
/*                                            widget                      */
/*    parent                                Pointer to the parent widget  */
/*    info                                  Pointer to                    */
/*                                            GX_CIRCULAR_GAUGE_INFO      */
/*                                            structure                   */
/*    style                                 Style of circular gauge.      */
/*    circular_gauge_id                     Application-defined ID of     */
/*                                            circular gauge              */
/*    xpos                                  Gauge x-coordinate position   */
/*    ypos                                  Gauge y-coordinate position   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_create                     Creates a widget.             */
/*    _gx_widget_link                       Links a child widget to its   */
/*                                            parent.                     */
/*    _gx_circular_gauge_angle_increment_calculate                        */
/*                                          Calculate increment angle for */
/*                                            each step.                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_circular_gauge_create(GX_CIRCULAR_GAUGE* gauge,
                               const GX_CHAR* name,
                               GX_WIDGET* parent,
                               GX_CIRCULAR_GAUGE_INFO* circular_gauge_info,
                               GX_RESOURCE_ID background,
                               ULONG style,
                               USHORT circular_gauge_id,
                               GX_VALUE xpos, GX_VALUE ypos)
{
    return gauge->create_(name, parent, circular_gauge_info, background, style,
                          circular_gauge_id, xpos, ypos);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_circular_gauge_draw                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified circular gauge.                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gauge                                 Pointer to circular gauge     */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_circular_gauge_background_draw    Draw gauge background         */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_circular_gauge_draw(GX_CIRCULAR_GAUGE* gauge)
{
    gauge->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_circular_gauge_event_process                                    */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes events for the specified circular gauge     */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    circular_gauge                        Circular gauge control block  */
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_circular_gauge_info_get           Retrieve the circular gauge   */
/*                                            information structure.      */
/*    _gx_circular_gauge_info_set           Set the circular gauge info.  */
/*    _gx_widget_event_process              Process event for specified   */
/*                                            widget.                     */
/*    _gx_widget_pixelmap_get               Get needle pixelmap.          */
/*    _gx_circular_gauge_needle_rectangle_calculate                       */
/*                                          Calculate current needle      */
/*                                            rectangle.                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_circular_gauge_event_process(GX_CIRCULAR_GAUGE* gauge, GX_EVENT* event_ptr)
{
    return gauge->event_process_(event_ptr);
}
