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
/**   Scroll Wheel Management (Scroll Wheel)                              */
/**                                                                       */
/**************************************************************************/

#include "gx_scroll_wheel.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_display.h"
#include "gx_canvas.h"

static UINT gx_scroll_wheel_event_process_(GX_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr)
{
    return wheel->event_process_(event_ptr);
}

static UINT gx_scroll_wheel_scroll_(GX_SCROLL_WHEEL* wheel, GX_VALUE shift)
{
    wheel->scroll_(shift);

    return GX_SUCCESS;
}

static GX_BOOL gx_scroll_wheel_wrap_style_check_(GX_SCROLL_WHEEL* wheel)
{
    return wheel->is_wrap_style_();
}

UINT GX_SCROLL_WHEEL::create_(const GX_CHAR* name, GX_WIDGET* parent, INT total_rows,
                              ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    GX_PARAMETER_NOT_USED(parent);

    if ((style & GX_STYLE_TEXT_ALIGNMENT_MASK) == 0)
    {
        style |= GX_STYLE_TEXT_CENTER;
    }

    GX_WINDOW::create_(name, GX_NULL, style, Id, size);

    this->type = GX_TYPE_SCROLL_WHEEL;

    this->total_rows = total_rows;
    this->row_height = 30;

    memset(&this->gradient, 0, sizeof(GX_GRADIENT));

    this->selected_background = GX_NULL;
    this->selected_row = 0;
    this->selected_yshift = 0;
    this->shift_error = 0;

    this->animation_speed = 0;
    this->animation_end_speed = 0;
    this->animation_steps = 0;
    this->animation_max_steps = 10;
    this->animation_delay = 2;
    this->animation_start_speed_rate = GX_FIXED_VAL_MAKE(1);
    this->animation_end_speed_rate = 200;

    this->event_process_function = (UINT (*)(GX_WIDGET *, GX_EVENT *))gx_scroll_wheel_event_process_;
    this->scroll = gx_scroll_wheel_scroll_;
    this->wrap_style_check = gx_scroll_wheel_wrap_style_check_;

    return GX_SUCCESS;
}

void GX_SCROLL_WHEEL::total_rows_set_(INT total_rows)
{
    this->total_rows = total_rows;

    if (this->selected_row > total_rows - 1)
    {
        selected_set_(total_rows - 1);
    }

    dirty_mark_if_visible_();
}

void GX_SCROLL_WHEEL::selected_set_(INT row)
{
    if (row < 0 || total_rows <= 0)
    {
        row = 0;
    }
    else if (row > total_rows - 1)
    {
        row = total_rows - 1;
    }

    if (selected_row == row)
    {
        return;
    }

    if (is_visible_())
    {
        INT diff;

        if (wrap_style_check(this))
        {
            if (GX_ABS(selected_row - row) <
                GX_ABS(selected_row + total_rows - row))
            {
                diff = selected_row - row;
            }
            else
            {
                diff = selected_row + total_rows - row;
            }
        }
        else
        {
            diff = selected_row - row;
        }

        diff *= row_height;

        animation_steps = 10;
        animation_speed = (GX_VALUE)(diff / 10);
        shift_error = (GX_BYTE)(diff % 10);

        timer_start_(GX_ANIMATION_TIMER, 2, 2);
    }
    else
    {
        selected_row = row;
    }
}

void GX_SCROLL_WHEEL::row_height_set_(GX_VALUE row_height)
{
    this->row_height = row_height;

    dirty_mark_if_visible_();
}

void GX_SCROLL_WHEEL::selected_background_set_(GX_RESOURCE_ID selected_bg)
{
    this->selected_background = selected_bg;

    dirty_mark_if_visible_();
}

void GX_SCROLL_WHEEL::speed_set_(GX_FIXED_VAL start_speed_rate, GX_FIXED_VAL end_speed_rate,
                                 GX_VALUE max_steps, GX_VALUE delay)
{
    this->animation_start_speed_rate = start_speed_rate;
    this->animation_end_speed_rate = end_speed_rate;
    this->animation_max_steps = max_steps;
    this->animation_delay = delay;
}

void GX_SCROLL_WHEEL::gradient_alpha_set_(GX_UBYTE start_alpha, GX_UBYTE end_alpha)
{
    gradient.alpha_start = start_alpha;
    gradient.alpha_end = end_alpha;

    if (is_visible_())
    {
        gradient_create_();

        dirty_mark_();
    }
}

void GX_SCROLL_WHEEL::gradient_create_()
{
    if (gradient.alpha_start || gradient.alpha_end)
    {
        if (!gradient.pixelmap.data)
        {
            GX_CANVAS* canvas = canvas_get_();
            if (canvas)
            {
                GX_DISPLAY* display = canvas->display;

                GX_UBYTE gradient_type = GX_GRADIENT_TYPE_MIRROR | GX_GRADIENT_TYPE_ALPHA;

                if (display->rotation_angle == GX_SCREEN_ROTATION_NONE ||
                    display->rotation_angle == GX_SCREEN_ROTATION_FLIP)
                {
                    _gx_system_gradient_create(&gradient, 3, height_(),
                                               gradient_type | GX_GRADIENT_TYPE_VERTICAL,
                                               gradient.alpha_start,
                                               gradient.alpha_end);
                }
                else
                {
                    _gx_system_gradient_create(&gradient, height_(), 3,
                                               gradient_type,
                                               gradient.alpha_start,
                                               gradient.alpha_end);

                    GX_SWAP_VALS(gradient.pixelmap.width, gradient.pixelmap.height);
                }
            }
        }
    }
    else
    {
        _gx_system_gradient_delete(&gradient);
    }
}

void GX_SCROLL_WHEEL::scroll_(GX_VALUE shift)
{
    shift_update_(shift);
    selected_row_calculate_();

    dirty_mark_if_visible_();
}

GX_VALUE GX_SCROLL_WHEEL::shift_update_(GX_VALUE shift)
{
    if (!wrap_style_check(this))
    {
        if ((shift > 0 && selected_row == 0) ||
            (shift < 0 && selected_row == total_rows - 1))
        {
            INT y_shift = selected_yshift + shift;

            const INT min_shift = (selected_row - total_rows + 1) * row_height;
            const INT max_shift = (selected_row * row_height);

            if (y_shift < min_shift || y_shift > max_shift)
            {
                shift = (GX_VALUE)(shift + shift_error);
                shift_error = (GX_BYTE)(shift % 4);
                shift /= 4;
            }
            else
            {
                shift_error = 0;
            }
        }
    }

    selected_yshift = (GX_VALUE)(selected_yshift + shift);

    return shift;
}

void GX_SCROLL_WHEEL::selected_row_calculate_()
{
    INT yshift = selected_yshift;
    INT selected = selected_row;

    INT half_row_height = row_height / 2;

    if (wrap_style_check(this))
    {
        while (yshift > half_row_height)
        {
            // scroll down
            yshift -= row_height;
            --selected;
        }

        while (yshift < -half_row_height)
        {
            // scroll up
            yshift += row_height;
            ++selected;
        }

        while (selected > total_rows - 1)
        {
            selected -= total_rows;
        }

        while (selected < 0)
        {
            selected += total_rows;
        }
    }
    else
    {
        while (yshift > half_row_height && selected > 0)
        {
            // scroll down
            yshift -= row_height;
            --selected;
        }

        while (yshift < -half_row_height && selected < total_rows - 1)
        {
            // scroll up
            yshift += row_height;
            ++selected;
        }
    }

    selected_row = selected;
    selected_yshift = (GX_VALUE)yshift;
}

GX_BOOL GX_SCROLL_WHEEL::is_wrap_style_() const
{
    return (style & GX_STYLE_WRAP) ? GX_TRUE : GX_FALSE;
}

void GX_SCROLL_WHEEL::timer_event_process_(UINT timer_id)
{
    INT shift;
    INT increment;
    bool notify_parent = true;

    switch (timer_id)
    {
    case GX_SNAP_TIMER:
        if (GX_ABS(selected_yshift) < GX_ABS(row_height) / 3)
        {
            timer_stop_(GX_SNAP_TIMER);

            notify_parent = true;

            shift = selected_yshift;
        }
        else
        {
            if (selected_yshift > 0)
            {
                shift = row_height / 3;
            }
            else
            {
                shift = (GX_VALUE)(-row_height / 3);
            }
        }

        scroll(this, (GX_VALUE)(-shift));
        break;

    case GX_FLICK_TIMER:
        --animation_steps;
        shift = animation_speed;

        if (shift_error)
        {
            shift += shift_error;
            shift_error = 0;
        }

        scroll(this, (GX_VALUE)shift);

        if (animation_steps > 0)
        {
            increment = (animation_end_speed - animation_speed) / animation_steps;
            animation_speed = (GX_VALUE)(animation_speed + increment);
        }

        if (!wrap_style_check(this))
        {
            if ((selected_row == 0 && selected_yshift > 0) ||
                (selected_row == total_rows - 1 &&
                 selected_yshift < 0))
            {
                animation_steps = 0;
            }
        }

        if (animation_steps == 0)
        {
            timer_stop_(GX_FLICK_TIMER);

            if (selected_yshift)
            {
                timer_start_(GX_SNAP_TIMER, 1, 1);
            }
            else
            {
                notify_parent = true;
            }
        }
        break;

    case GX_ANIMATION_TIMER:
        --animation_steps;

        if (shift_error)
        {
            scroll(this, shift_error);
            shift_error = 0;
        }

        scroll(this, animation_speed);

        if (animation_steps == 0)
        {
            timer_stop_(GX_ANIMATION_TIMER);

            if (selected_yshift)
            {
                timer_start_(GX_SNAP_TIMER, 1, 1);
            }
            else
            {
                notify_parent = true;
            }
        }
        break;

    default:
        break;
    }

    if (notify_parent)
    {
        signal_parent_(GX_EVENT_LIST_SELECT, selected_row);
    }
}

void GX_SCROLL_WHEEL::flick_event_process_(INT flick_speed)
{
    INT shift = flick_speed / GX_FIXED_VAL_HALF;

    if (GX_ABS(shift) > 5 && total_rows)
    {
        timer_stop_(GX_SNAP_TIMER);

        if (GX_ABS(shift) < row_height / 3)
        {
            shift /= GX_ABS(shift);
            shift *= row_height / 3;
        }

        INT start_speed = (animation_start_speed_rate * shift) >> GX_FIXED_VAL_SHIFT;
        INT end_speed = (animation_end_speed * shift) >> GX_FIXED_VAL_SHIFT;

        if (GX_ABS(end_speed) < row_height / 10)
        {
            end_speed = row_height / 10;

            if (shift < 0)
            {
                end_speed = -end_speed;
            }
        }

        INT total_steps = GX_ABS(shift) / 2;

        if (total_steps > animation_max_steps)
        {
            total_steps = animation_max_steps;
        }

        animation_speed = (GX_VALUE)start_speed;
        animation_end_speed = (GX_VALUE)end_speed;
        animation_steps = (GX_VALUE)total_steps;

        INT speed = start_speed;
        shift = start_speed;
        while (total_steps > 1)
        {
            total_steps--;
            speed += (end_speed - speed) / total_steps;

            shift += speed;

            if (GX_ABS(shift) > row_height)
            {
                shift %= row_height;
            }
        }

        if (start_speed > 0)
        {
            shift += selected_yshift;
            shift %= row_height;
            if (shift < 0)
            {
                shift_error = (GX_VALUE)(-shift);
            }
            else if (shift > 0)
            {
                shift_error = (GX_VALUE)(row_height - shift);
            }
        }
        else if (start_speed < 0)
        {
            shift += selected_yshift;
            shift %= row_height;

            if (shift < 0)
            {
                shift_error = (GX_VALUE)(-shift - row_height);
            }
            else if (shift > 0)
            {
                shift_error = (GX_VALUE)(-shift);
            }
        }

        timer_start_(GX_FLICK_TIMER, (UINT)animation_delay, (UINT)animation_delay);
    }
}

void GX_SCROLL_WHEEL::pen_up_event_process_()
{
    status_remove_(GX_STATUS_TRACKING_PEN);

    if (selected_yshift)
    {
        timer_start_(GX_SNAP_TIMER, 1, 1);
    }
    else
    {
        GX_RECTANGLE dirty = client;
        dirty.top = (GX_VALUE)(dirty.top + dirty.height_() / 2);
        dirty.top = (GX_VALUE)(dirty.top - row_height / 2);
        dirty.bottom = (GX_VALUE)(dirty.top + row_height - 1);
        dirty_mark_(&dirty);

        signal_parent_(GX_EVENT_LIST_SELECT, selected_row);
    }
}

UINT GX_SCROLL_WHEEL::event_process_(GX_EVENT* event_ptr)
{
    GX_VALUE shift;

    switch (event_ptr->type)
    {
    case GX_EVENT_PEN_DOWN:
        if (total_rows)
        {
            _gx_system_input_capture(this);

            move_start = event_ptr->payload.pointdata;
            shift_error = 0;
        }
        break;

    case GX_EVENT_PEN_DRAG:
        if (this->status & GX_STATUS_OWNS_INPUT)
        {
            status_add_(GX_STATUS_TRACKING_PEN);

            shift = (GX_VALUE)(event_ptr->payload.pointdata.y - move_start.y);
            if (shift)
            {
                scroll(this, shift);

                move_start = event_ptr->payload.pointdata;
            }
        }
        else
        {
            parent_event_process_run_(event_ptr);
        }
        break;

    case GX_EVENT_PEN_UP:
        if (this->status & GX_STATUS_OWNS_INPUT)
        {
            _gx_system_input_release(this);

            pen_up_event_process_();
        }
        else
        {
            parent_event_process_run_(event_ptr);
        }
        break;

    case GX_EVENT_VERTICAL_FLICK:
        flick_event_process_(event_ptr->payload.intdata[0]);
        break;

    case GX_EVENT_TIMER:
        timer_event_process_(event_ptr->payload.timer_id);
        break;

    case GX_EVENT_SHOW:
        GX_WINDOW::event_process_(event_ptr);
        gradient_create_();
        break;

    case GX_EVENT_DELETE:
        _gx_system_gradient_delete(&gradient);
        break;

    default:
        return GX_WINDOW::event_process_(event_ptr);
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scroll_wheel_create                                             */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a scroll wheel selector widget.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    name                                  Name of widget                */
/*    parent                                Parent widget control block   */
/*    total_rows                            Total rows of the scroll wheel*/
/*    style                                 Style of widget               */
/*    Id                                    Application-defined ID of the */
/*                                            the widget                  */
/*    size                                  Widget size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_create                     Create a window               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_scroll_wheel_create(GX_SCROLL_WHEEL* wheel, const GX_CHAR* name,
                             GX_WIDGET* parent, INT total_rows,
                             ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    return wheel->create_(name, parent, total_rows, style, Id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scroll_wheel_event_process                                      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes the comming events for a scroll wheel       */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Text scroll wheel control     */
/*                                            block                       */
/*    event_ptr                             Event to be processed         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_input_capture              Temporarily direct all inputs */
/*                                            to specified widget         */
/*    _gx_system_input_release              Release captured input events */
/*    _gx_window_event_process              Default window event process  */
/*    _gx_widget_event_to_parent            Send event to widget's parent */
/*    _gx_scroll_wheel_scroll               Scroll a scroll wheel widget  */
/*    _gx_scroll_wheel_pen_up_event_handler Handle pen up event           */
/*    _gx_scroll_wheel_flick_event_handler  Handle flick event            */
/*    _gx_scroll_wheel_timer_event_handler  Handle timer event            */
/*    _gx_scroll_wheel_gradient_create      Create a gradient pixelmap    */
/*    gx_gradient_delete_                   Delete a gradient             */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_scroll_wheel_event_process(GX_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr)
{
    return wheel->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scroll_wheel_gradient_alpha_set                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the start and end alpha of gradient pixelmap that*/
/*    used to cover the top and bottom of a text scroll wheel widget.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    start_alpha                           Start alpha                   */
/*    end_alpha                             End alpha                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_gradient_create      Create a gradient pixelmap    */
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_scroll_wheel_gradient_alpha_set(GX_SCROLL_WHEEL* wheel, GX_UBYTE start_alpha, GX_UBYTE end_alpha)
{
    wheel->gradient_alpha_set_(start_alpha, end_alpha);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scroll_wheel_row_height_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets row height for scroll wheel widget.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    row_height                            Row height to be set          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_scroll_wheel_row_height_set(GX_SCROLL_WHEEL* wheel, GX_VALUE row_height)
{
    wheel->row_height_set_(row_height);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scroll_wheel_selected_background_set                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets background pixelmap id for the selected row of   */
/*    scroll wheel widget.                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    selected_bg                           Selected background to be set */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_scroll_wheel_selected_background_set(GX_SCROLL_WHEEL* wheel, GX_RESOURCE_ID selected_bg)
{
    wheel->selected_background_set_(selected_bg);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scroll_wheel_selected_get                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function gets the current selected row index of a scroll wheel */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    row                                   Selected row to be retrieved  */
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
UINT _gx_scroll_wheel_selected_get(GX_SCROLL_WHEEL* wheel, INT* row)
{
    *row = wheel->selected_row;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scroll_wheel_selected_set                                       */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the current selected row index for scroll wheel  */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    row                                   Selected row to be set        */
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
UINT _gx_scroll_wheel_selected_set(GX_SCROLL_WHEEL* wheel, INT row)
{
    wheel->selected_set_(row);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scroll_wheel_speed_set                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets scrolling speed for the scroll wheel widget.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    start_speed_rate                      The rate of start speed to    */
/*                                            flick speed.                */
/*    end_speed_rate                        The rate of end speed to      */
/*                                            flick speed.                */
/*    max_steps                             Max steps for scrolling       */
/*    delay                                 Delay time of each step       */
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
UINT _gx_scroll_wheel_speed_set(GX_SCROLL_WHEEL* wheel,
                                GX_FIXED_VAL start_speed_rate, GX_FIXED_VAL end_speed_rate,
                                GX_VALUE max_steps, GX_VALUE delay)
{
    wheel->speed_set_(start_speed_rate, end_speed_rate, max_steps, delay);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scroll_wheel_total_rows_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the total rows for scroll wheel widget.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    total_rows                            Total rows to set             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_scroll_wheel_total_rows_set(GX_SCROLL_WHEEL* wheel, INT total_rows)
{
    wheel->total_rows_set_(total_rows);

    return GX_SUCCESS;
}
