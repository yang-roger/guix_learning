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
/**   Button Management (Button)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_button.h"

#include "gx_system.h"
#include "gx_display.h"
#include "gx_context.h"
#include "gx_canvas.h"

static void gx_button_draw_(GX_BUTTON *button)
{
    button->draw_();
}

static UINT gx_button_event_process_(GX_BUTTON* button, GX_EVENT* event_ptr)
{
    return button->event_process_(event_ptr);
}

static void gx_button_select_(GX_BUTTON* button)
{
    button->select_();
}

static void gx_button_deselect_(GX_BUTTON* button, GX_BOOL generate_event)
{
    button->deselect_(generate_event);
}

UINT GX_BUTTON::create_(const GX_CHAR* name, GX_WIDGET* parent,
                        ULONG style, USHORT button_id, const GX_RECTANGLE* size)
{
    GX_WIDGET::create_(name, GX_NULL, style, button_id, size);

    this->type = GX_TYPE_BUTTON;

    status_add_(GX_STATUS_BUTTON_DERIVED);

    this->normal_fill_color = GX_COLOR_ID_BUTTON_LOWER;
    this->selected_fill_color = GX_COLOR_ID_BUTTON_UPPER;

    this->draw_function = (void (*)(GX_WIDGET*))gx_button_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_button_event_process_;
    this->select_handler = (void (*)(GX_BUTTON*))gx_button_select_;
    this->deselect_handler = (void (*)(GX_BUTTON*, GX_BOOL))gx_button_deselect_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_BUTTON::select_()
{
    if (style & GX_STYLE_ENABLED)
    {
        if (!(style & GX_STYLE_BUTTON_PUSHED))
        {
            style |= GX_STYLE_BUTTON_PUSHED;

            if (status & GX_STATUS_ACCEPTS_FOCUS)
            {
                move_to_front_();
            }

            if (style & GX_STYLE_BUTTON_RADIO)
            {
                siblings_deselect_();
                signal_parent_(GX_EVENT_RADIO_SELECT, 0);
            }
            else if (style & GX_STYLE_BUTTON_TOGGLE)
            {
                signal_parent_(GX_EVENT_TOGGLE_ON, 0);
            }
            else if (style & GX_STYLE_BUTTON_EVENT_ON_PUSH)
            {
                signal_parent_(GX_EVENT_CLICKED, 0);
            }

            if (style & GX_STYLE_BUTTON_REPEAT)
            {
                timer_start_(GX_BUTTON_TIMER, GX_REPEAT_BUTTON_INITIAL_TICS, GX_REPEAT_BUTTON_REPEAT_TICS);
            }

            dirty_mark_();
        }
        else
        {
            if (style & GX_STYLE_BUTTON_TOGGLE)
            {
                status |= GX_STATUS_TOGGLE_UNLOCK;
            }
        }
    }
}

void GX_BUTTON::deselect_(GX_BOOL generate_event)
{
    if (style & GX_STYLE_BUTTON_PUSHED)
    {
        if (style & GX_STYLE_BUTTON_TOGGLE)
        {
            if (!(status & GX_STATUS_TOGGLE_UNLOCK))
            {
                return;
            }

            status &= ~GX_STATUS_TOGGLE_UNLOCK;
        }

        if (style & GX_STYLE_BUTTON_REPEAT)
        {
            timer_stop_(GX_BUTTON_TIMER);
        }

        style &= ~GX_STYLE_BUTTON_PUSHED;

        if (generate_event)
        {
            if (style & GX_STYLE_BUTTON_RADIO)
            {
                signal_parent_(GX_EVENT_RADIO_DESELECT, 0);
            }
            else if (style & GX_STYLE_BUTTON_TOGGLE)
            {
                signal_parent_(GX_EVENT_TOGGLE_OFF, 0);
            }
            else if (!(style & GX_STYLE_BUTTON_EVENT_ON_PUSH))
            {
                signal_parent_(GX_EVENT_CLICKED, 0);
            }
        }
    }

    dirty_mark_if_visible_();
}

void GX_BUTTON::siblings_deselect_()
{
    for (GX_WIDGET* sibling = parent->first_child; sibling; sibling = sibling->next)
    {
        if (sibling != this)
        {
            if (sibling->status & GX_STATUS_BUTTON_DERIVED)
            {
                GX_BUTTON* button = (GX_BUTTON*)sibling;

                if ((button->style & (GX_STYLE_BUTTON_RADIO | GX_STYLE_BUTTON_PUSHED)) ==
                                     (GX_STYLE_BUTTON_RADIO | GX_STYLE_BUTTON_PUSHED))
                {
                    button->deselect_handler(button, GX_TRUE);
                }
            }
        }
    }
}

void GX_BUTTON::background_draw_()
{
    GX_RESOURCE_ID color1;
    GX_RESOURCE_ID color2;

    if (style & GX_STYLE_ENABLED)
    {
        if (style & GX_STYLE_BUTTON_PUSHED)
        {
            if ((style & GX_STYLE_BORDER_MASK) == GX_STYLE_BORDER_RAISED)
            {
                border_style_set_(GX_STYLE_BORDER_RECESSED);
            }

            color1 = normal_fill_color;
            color2 = selected_fill_color;
        }
        else
        {
            if ((style & GX_STYLE_BORDER_MASK) == GX_STYLE_BORDER_RECESSED)
            {
                border_style_set_(GX_STYLE_BORDER_RAISED);
            }

            color1 = selected_fill_color;
            color2 = normal_fill_color;
        }
    }
    else
    {
        color1 = disabled_fill_color;
        color2 = disabled_fill_color;
    }

    border_and_background_draw_(GX_COLOR_ID_BUTTON_BORDER, color1, color2, GX_TRUE);
}

void GX_BUTTON::draw_()
{
    background_draw_();
    children_draw_();
}

void GX_BUTTON::select_process_()
{
    if (style & GX_STYLE_ENABLED)
    {
        select_handler(this);

        _gx_system_input_capture(this);
    }
}

void GX_BUTTON::deselect_process_(GX_EVENT* event_ptr)
{
    if (status & GX_STATUS_OWNS_INPUT)
    {
        _gx_system_input_release(this);

        if (style & GX_STYLE_BUTTON_PUSHED)
        {
            if (!(style & GX_STYLE_BUTTON_RADIO))
            {
                GX_BOOL generate_event = GX_FALSE;

                if (event_ptr->type == GX_EVENT_DESELECT)
                {
                    generate_event = GX_TRUE;
                }
                else if (event_ptr->type == GX_EVENT_PEN_UP)
                {
                    if (size.contain_point_(event_ptr->payload.pointdata))
                    {
                        generate_event = GX_TRUE;
                    }
                }

                deselect_handler(this, generate_event);
            }
        }
    }
}

UINT GX_BUTTON::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_SELECT:
        select_process_();
        break;

    case GX_EVENT_PEN_DOWN:
        select_process_();
        result = parent_event_process_run_(event_ptr);
        break;

    case GX_EVENT_PEN_UP:
        deselect_process_(event_ptr);
        result = parent_event_process_run_(event_ptr);
        break;

    case GX_EVENT_DESELECT:
    case GX_EVENT_INPUT_RELEASE:
        deselect_process_(event_ptr);
        break;

    case GX_EVENT_TIMER:
        if (event_ptr->payload.timer_id == GX_BUTTON_TIMER)
        {
            if (style & GX_STYLE_BUTTON_REPEAT)
            {
                signal_parent_(GX_EVENT_CLICKED, id);
            }
            else
            {
                timer_stop_(GX_BUTTON_TIMER);
            }
        }
        break;

    default:
        result = GX_WIDGET::event_process_(event_ptr);
        break;
    }

    return result;
}

void GX_BUTTON::monochrome_driver_disabled_line_draw_()
{
    GX_DRAW_CONTEXT* context = gx_context_current_();

    INT color_format = context->canvas->display->color_format;
    if (color_format != GX_COLOR_FORMAT_MONOCHROME)
    {
        return;
    }

    GX_COLOR line_color;
    context->color_get_(normal_fill_color, &line_color);

    line_color = ~line_color; // invert color

    context->raw_line_color_set_(line_color);
    context->brush.width = 2;

    GX_VALUE left = (GX_VALUE)(left_() + 5);
    GX_VALUE right = (GX_VALUE)(right_() - 5);
    GX_VALUE y = (GX_VALUE)((top_() + bottom_()) / 2);

    _gx_canvas_line_draw(left, y, right, y);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_button_background_draw                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified button, which is a special type   */
/*    of widget.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_border_style_set           Set the border style          */
/*    _gx_widget_border_draw                Draw the border               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    _gx_button_draw                                                     */
/*    _gx_icon_button_draw                                                */
/*    _gx_text_button_draw                                                */
/*                                                                        */
/**************************************************************************/
void _gx_button_background_draw(GX_BUTTON* button)
{
    button->background_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_button_create                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a vector button, which is a special type of   */
/*    prompt (widget).                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Pointer to button control     */
/*                                            block                       */
/*    name                                  Logical name of button        */
/*    parent                                Pointer to parent widget      */
/*                                            of button                   */
/*    style                                 Button stuyle                 */
/*    button_id                             Application-defined ID of     */
/*                                             the button                 */
/*    size                                  Size of the button            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_create                     Create the underlying prompt  */
/*    _gx_widget_status_add                 Set the widget status         */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    _gx_horizontal_scrollbar_create                                     */
/*    _gx_icon_button_create                                              */
/*    _gx_pixelmap_button_create                                          */
/*    _gx_scroll_thumb_create                                             */
/*    _gx_text_button_create                                              */
/*    _gx_vertical_scrollbar_create                                       */
/*    _gx_scroll_thumb_create                                             */
/*    _gx_text_button_create                                              */
/*                                                                        */
/**************************************************************************/
UINT _gx_button_create(GX_BUTTON* button, const GX_CHAR* name, GX_WIDGET* parent,
                       ULONG style, USHORT button_id, const GX_RECTANGLE* size)
{
    return button->create_(name, parent, style, button_id,size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_button_deselect                                 PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service deselects the specified button.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Pointer to button control     */
/*                                            block                       */
/*    generate_event                        If GX_TRUE, the button will   */
/*                                            generate an event depending */
/*                                            on the button style.        */
/*                                          If GX_FALSE, the button will  */
/*                                            not generate any higher     */
/*                                            level event.                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_timer_stop                 Stop an active GUIX timer     */
/*    _gx_widget_event_generate             Generate event to notify      */
/*                                             parent widget              */
/*    _gx_system_dirty_mark                 Sets the dirty flag           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_button_deselect(GX_BUTTON* button, GX_BOOL generate_event)
{
    button->deselect_(generate_event);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_button_draw                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified button, which is a special type   */
/*    of widget.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Pointer to the button         */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_button_background_draw            Draw button background        */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_button_draw(GX_BUTTON *button)
{
    button->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_button_event_process                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes events for the specified button.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Pointer to button control     */
/*                                            block                       */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [_gx_button_select_handler]           Button select callback        */
/*    _gx_system_input_capture              Assign system input widget    */
/*    _gx_system_input_release              Release system input widget   */
/*    [_gx_button_deselect_handler]         Widget-provided deselect      */
/*                                            handler routine             */
/*    _gx_widget_event_generate             Generate event to notify      */
/*                                             parent widget              */
/*    _gx_system_timer_stop                 Stop an active GUIX timer     */
/*    _gx_widget_event_process              Default widget event process  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_button_event_process(GX_BUTTON* button, GX_EVENT* event_ptr)
{
    return button->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_button_select                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function selects one button, invalidating and eventing as      */
/*    necessary.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_front_move                 Move widget to the front      */
/*    _gx_button_siblings_deselect          Deselect all the siblings     */
/*    _gx_widget_event_generate             Generate events for widget    */
/*    _gx_system_timer_start                Allocate a free timer and     */
/*                                          activates it                  */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_button_select(GX_BUTTON* button)
{
    button->select_();

    return GX_SUCCESS;
}
