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
/**   Scroll Wheel Management (Generic Scroll Wheel)                      */
/**                                                                       */
/**************************************************************************/

#include "gx_generic_scroll_wheel.h"

#include "gx_utility.h"
#include "gx_context.h"
#include "gx_canvas.h"

static void gx_generic_scroll_wheel_draw_(GX_GENERIC_SCROLL_WHEEL* wheel)
{
    wheel->draw_();
}

static UINT gx_generic_scroll_wheel_event_process_(GX_GENERIC_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr)
{
    return wheel->event_process_(event_ptr);
}

static UINT gx_generic_scroll_wheel_scroll_(GX_GENERIC_SCROLL_WHEEL* wheel, GX_VALUE shift)
{
    return wheel->scroll_(shift);
}

static GX_BOOL gx_generic_scroll_wheel_wrap_style_check_(GX_GENERIC_SCROLL_WHEEL* wheel)
{
    return wheel->is_wrap_style_();
}

UINT GX_GENERIC_SCROLL_WHEEL::create_(const GX_CHAR* name, GX_WIDGET* parent, INT total_rows,
                                      void (*callback)(GX_GENERIC_SCROLL_WHEEL*, GX_WIDGET*, INT),
                                      ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    GX_SCROLL_WHEEL::create_(name, GX_NULL, total_rows, style, Id, size);

    this->type = GX_TYPE_GENERIC_SCROLL_WHEEL;

    this->top_index = 0;
    this->child_count = 0;
    this->visible_rows = 0;

    this->callback = callback;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_generic_scroll_wheel_event_process_;
    this->draw_function = (void (*)(GX_WIDGET *))gx_generic_scroll_wheel_draw_;
    this->scroll = (UINT(*)(GX_SCROLL_WHEEL*, GX_VALUE))gx_generic_scroll_wheel_scroll_;
    this->wrap_style_check = (GX_BOOL(*)(GX_SCROLL_WHEEL*))gx_generic_scroll_wheel_wrap_style_check_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_GENERIC_SCROLL_WHEEL::children_position_()
{
    GX_WIDGET* child = first_visible_client_child_get_();
    if (!child)
    {
        return;
    }

    if (row_height <= 0)
    {
        return;
    }

    GX_VALUE selected_top = (GX_VALUE)((client.top + client.bottom) / 2);
    selected_top = (GX_VALUE)(selected_top - row_height / 2);
    selected_top = (GX_VALUE)(selected_top + selected_yshift);

    GX_RECTANGLE childsize = client;

    INT top_rows = (selected_top - client.top + row_height) / row_height;
    if (total_rows > 0 && top_rows >= total_rows)
    {
        top_rows = total_rows - 1;
    }

    childsize.top = (GX_VALUE)(selected_top - top_rows * row_height);
    childsize.bottom = (GX_VALUE)(childsize.top + row_height - 1);

    child_count = 0;

    INT old_selected_row = selected_row;

    selected_row = top_index + top_rows;

    while (child)
    {
        ++child_count;

        if (childsize.top == selected_top)
        {
            child->style |= GX_STYLE_DRAW_SELECTED;
        }
        else
        {
            child->style &= ~GX_STYLE_DRAW_SELECTED;
        }

        child->status_remove_(GX_STATUS_ACCEPTS_FOCUS);

        child->resize_(&childsize);

        childsize.move_(0, row_height);

        child = child->next_visible_client_child_get_();
    }

    visible_rows = (GX_VALUE)((client.height_() + row_height - 1) / row_height);

    if (selected_row != old_selected_row)
    {
        scroll_((GX_VALUE)((selected_row - old_selected_row) * row_height));
    }
}

void GX_GENERIC_SCROLL_WHEEL::total_rows_set_(INT count)
{
    total_rows = count;

    if (count <= 0)
    {
        selected_row = 0;
    }
    else if (selected_row >= count)
    {
        selected_row = count - 1;
    }

    child_count = 0;

    GX_WIDGET* child = first_client_child_get_();
    while (child)
    {
        if (!child->is_visible_())
        {
            child->show_();
        }

        ++child_count;

        child = child->next_client_child_get_();
    }

    child = last_child;
    while (child && child_count > count)
    {
        if (child->is_client_())
        {
            child->hide_();

            --child_count;
        }

        child = child->previous;
    }

    top_index = 0;

    if (callback)
    {
        INT row = 0;

        child = first_visible_client_child_get_();
        while (child)
        {
            child->style &= ~GX_STYLE_DRAW_SELECTED;
            callback(this, child, row);
            ++row;

            child = child->next_visible_client_child_get_();
        }
    }

    children_position_();

    dirty_mark_if_visible_();
}

void GX_GENERIC_SCROLL_WHEEL::row_height_set_(GX_VALUE row_height)
{
    this->row_height = row_height;

    if (is_visible_())
    {
        children_position_();
    }
}

void GX_GENERIC_SCROLL_WHEEL::up_wrap_()
{
    GX_WIDGET* first;
    GX_WIDGET* last;
    GX_RECTANGLE newpos;
    INT index;

    while ((top_index + child_count < total_rows) ||
           (style & GX_STYLE_WRAP))
    {
        first = first_visible_client_child_get_();

        if (first && first->bottom_() < top_())
        {
            // top widget is above client area, move it to the bottom

            last = last_visible_client_child_get_();
            if (last)
            {
                first->detach_();

                newpos = first->size;
                newpos.move_(0, (GX_VALUE)(last->bottom_() - first->top_() + 1));
                first->resize_(&newpos);

                index = top_index + child_count;
                if (index >= total_rows)
                {
                    index -= total_rows;
                }

                callback(this, first, index);

                first->attach_to_(this);

                ++top_index;
                if (top_index >= total_rows)
                {
                    top_index -= total_rows;
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
}

void GX_GENERIC_SCROLL_WHEEL::down_wrap_()
{
    GX_WIDGET* first;
    GX_WIDGET* last;
    GX_RECTANGLE newpos;

    while ((top_index > 0) ||
           (style & GX_STYLE_WRAP))
    {
        last = last_visible_client_child_get_();

        if (last && last->top_() > bottom_())
        {
            // bottom widget is below client area, move it to the top

            --top_index;
            if (top_index < 0)
            {
                top_index = total_rows - 1;
            }

            first = first_visible_client_child_get_();
            if (first)
            {
                last->detach_();

                newpos = last->size;
                newpos.move_(0, (GX_VALUE)(-(last->bottom_() - first->top_() + 1)));
                last->resize_(&newpos);

                callback(this, last, top_index);

                last->back_attach_to_(this);
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
}

UINT GX_GENERIC_SCROLL_WHEEL::scroll_(GX_VALUE shift)
{
    if (shift == 0)
    {
        return GX_SUCCESS;
    }

    shift = shift_update_(shift);

    selected_row_calculate_();

    GX_WIDGET* child = first_visible_client_child_get_();
    while (child)
    {
        child->scroll_shift_(0, shift, GX_TRUE);

        if (child->style & GX_STYLE_DRAW_SELECTED)
        {
            child->style &= ~GX_STYLE_DRAW_SELECTED;
        }

        child = child->next_visible_client_child_get_();
    }

    if ((callback != GX_NULL) &&
        (visible_rows < total_rows) &&
        ((child_count < total_rows) || (style & GX_STYLE_WRAP)))
    {
        if (shift < 0)
        {
            up_wrap_();
        }
        else
        {
            down_wrap_();
        }
    }

    dirty_mark_if_visible_();

    return GX_SUCCESS;
}

GX_BOOL GX_GENERIC_SCROLL_WHEEL::is_wrap_style_() const
{
    if ((style & GX_STYLE_WRAP) &&
        (callback) &&
        (visible_rows < total_rows))
    {
        return GX_TRUE;
    }
    else
    {
        return GX_FALSE;
    }
}

void GX_GENERIC_SCROLL_WHEEL::draw_()
{
    GX_WINDOW::background_draw_();

    GX_PIXELMAP* map;
    _gx_context_pixelmap_get(selected_background, &map);

    GX_RECTANGLE selected_area = client;
    selected_area.top = (GX_VALUE)((selected_area.top + selected_area.bottom) / 2);
    selected_area.top = (GX_VALUE)(selected_area.top - row_height / 2);
    selected_area.bottom = (GX_VALUE)(selected_area.top + row_height - 1);

    if (map)
    {
        _gx_canvas_pixelmap_tile(&selected_area, map);
    }

    if (!(this->status & GX_STATUS_TRACKING_PEN) && (animation_steps == 0))
    {
        // The scroll wheel is stop scrolling,
        // set GX_STYLE_DRAW_SELECTED style to the selected widget.

        GX_WIDGET* child = first_visible_client_child_get_();
        while (child)
        {
            if (selected_area.top == child->top_())
            {
                child->style |= GX_STYLE_DRAW_SELECTED;
                break;
            }

            child = child->next_visible_client_child_get_();
        }
    }

    children_draw_();

    if (gradient.pixelmap.data)
    {
        _gx_canvas_pixelmap_tile(&size, &gradient.pixelmap);
    }
}

UINT GX_GENERIC_SCROLL_WHEEL::event_process_(GX_EVENT* event_ptr)
{
    UINT result;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        result = GX_SCROLL_WHEEL::event_process_(event_ptr);

        if (child_count <= 0)
        {
            children_position_();
        }
        break;

    default:
        result = GX_SCROLL_WHEEL::event_process_(event_ptr);
        break;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_generic_scroll_wheel_children_position                          */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function positions the children for the generic scroll wheel.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_shift           Shift rectangle               */
/*    _gx_window_client_height_get          Retrieve the client height    */
/*                                            of the widget               */
/*    _gx_widget_first_visible_client_child_get                           */
/*                                          Get the first visible client  */
/*    _gx_widget_next_visible_client_child_get                            */
/*                                          Get the next visible client   */
/*    _gx_widget_resize                     resizes the widget            */
/*    _gx_generic_scroll_wheel_scroll       Scroll the generic scroll wheel*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_generic_scroll_wheel_children_position(GX_GENERIC_SCROLL_WHEEL* wheel)
{
    wheel->children_position_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_generic_scroll_wheel_create                                     */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a generic scroll wheel selector widget.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    name                                  Name of widget                */
/*    parent                                Parent widget control block   */
/*    total_rows                            Total rows of the scroll wheel*/
/*    callback                              Callback function to create a */
/*                                            widget row                  */
/*    style                                 Style of widget               */
/*    id                                    Application-defined ID of the */
/*                                            the widget                  */
/*    size                                  Widget size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_create               Create a scroll wheel widget  */
/*    _gx_widget_link                       Link a widget to its parent   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_generic_scroll_wheel_create(GX_GENERIC_SCROLL_WHEEL* wheel,
                                     const GX_CHAR* name,
                                     GX_WIDGET* parent,
                                     INT total_rows,
                                     void (*callback)(GX_GENERIC_SCROLL_WHEEL*, GX_WIDGET*, INT),
                                     ULONG style,
                                     USHORT id,
                                     const GX_RECTANGLE* size)
{
    return wheel->create_(name, parent, total_rows, callback, style, id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_generic_scroll_wheel_draw                                       */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the generic scroll wheel widget.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Generic scroll wheel control  */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_background_draw            Draw window background        */
/*    _gx_context_pixelmap_get              Get pixelmap by resource ID   */
/*    _gx_window_client_height_get          Get window client height      */
/*    _gx_window_client_width_get           Get window client width       */
/*    _gx_canvas_pixelmap_tile              Tile a pixelmap               */
/*    _gx_widget_children_draw              Draw children of the widget   */
/*    _gx_widget_first_visible_client_child_get                           */
/*                                          Get the first visible client  */
/*    _gx_widget_next_visible_client_child_get                            */
/*                                          Get the next visible client   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_generic_scroll_wheel_draw(GX_GENERIC_SCROLL_WHEEL* wheel)
{
    wheel->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_generic_scroll_wheel_event_process                              */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service processes an event for the generic scroll wheel.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_generic_scroll_wheel_children_position                          */
/*                                          Position the children for     */
/*                                            the scroll wheel children   */
/*    _gx_scroll_wheel_event_process        Process events for the        */
/*                                            scroll wheel widget         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_generic_scroll_wheel_event_process(GX_GENERIC_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr)
{
    return wheel->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_generic_scroll_wheel_row_height_set                             */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets row height for generic scroll wheel widget.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Widget Scroll wheel control   */
/*                                            block                       */
/*    row_height                            Row height to be set          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_generic_scroll_wheel_children_position                          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_generic_scroll_wheel_row_height_set(GX_GENERIC_SCROLL_WHEEL* wheel, GX_VALUE row_height)
{
    wheel->row_height_set_(row_height);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_generic_scroll_wheel_total_rows_set                             */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns total number of the generic scroll wheel.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll Wheel control block    */
/*    count                                 Number of rows                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_hide                       Hide a widget                 */
/*    _gx_widget_first_visible_client_child_get                           */
/*                                          Get the first visible client  */
/*    _gx_widget_next_visible_client_child_get                            */
/*                                          Get the next visible client   */
/*    _gx_generic_scroll_wheel_children_position                          */
/*                                          Position the children of the  */
/*                                            generic scroll wheel        */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_generic_scroll_wheel_total_rows_set(GX_GENERIC_SCROLL_WHEEL* wheel, INT count)
{
    wheel->total_rows_set_(count);

    return GX_SUCCESS;
}
