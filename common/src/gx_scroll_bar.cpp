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
/**   Scroll Management (Scroll)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_scroll_bar.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_canvas.h"
#include "gx_pixelmap.h"
#include "gx_window.h"

static void gx_scrollbar_draw_(GX_SCROLLBAR* scrollbar)
{
    scrollbar->draw_();
}

static UINT gx_scrollbar_event_process_(GX_SCROLLBAR* scrollbar, GX_EVENT* event_ptr)
{
    return scrollbar->event_process_(event_ptr);
}

UINT GX_SCROLLBAR::create_(const GX_CHAR* name, GX_WINDOW* parent, const GX_SCROLLBAR_APPEARANCE* appearance, ULONG style)
{
    USHORT widget_type;
    USHORT widget_id;
    ULONG selected_style;

    if (style & GX_SCROLLBAR_VERTICAL)
    {
        widget_type = GX_TYPE_VERTICAL_SCROLL;
        widget_id = GX_ID_VERTICAL_SCROLL;
        selected_style = GX_SCROLLBAR_VERTICAL;
    }
    else
    {
        widget_type = GX_TYPE_HORIZONTAL_SCROLL;
        widget_id = GX_ID_HORIZONTAL_SCROLL;
        selected_style = GX_SCROLLBAR_HORIZONTAL;
    }

    GX_RECTANGLE widget_size;
    widget_size.set_(0, 0, 0, 0);

    GX_WIDGET::create_(name, GX_NULL, style, widget_id, &widget_size);

    this->type = widget_type;

    this->normal_fill_color = GX_COLOR_ID_SCROLL_FILL;
    this->selected_fill_color = GX_COLOR_ID_SCROLL_FILL;

    if (appearance)
    {
        this->appearance = *appearance;
    }
    else
    {
        _gx_system_scroll_appearance_get(selected_style, &this->appearance);
    }

    if (style & GX_SCROLLBAR_END_BUTTONS)
    {
        const ULONG button_style = GX_STYLE_ENABLED | GX_STYLE_BORDER_RAISED;
        GX_RESOURCE_ID button_color = this->appearance.button_color;

        upleft.create_(GX_NULL, this, button_style, GX_ID_SCROLL_UP_LEFT, &widget_size);
        upleft.fill_color_set_(button_color, button_color, button_color);
        upleft.status_remove_(GX_STATUS_ACCEPTS_FOCUS);

        downright.create_(GX_NULL, this, button_style, GX_ID_SCROLL_DOWN_RIGHT, &widget_size);
        downright.fill_color_set_(button_color, button_color, button_color);
        downright.status_remove_(GX_STATUS_ACCEPTS_FOCUS);
    }

    ULONG thumb_style = this->appearance.thumb_border_style | selected_style | GX_STYLE_ENABLED;
    thumb.create_(this, thumb_style);

    // scroll-bars have non-client area status and do not accept focus
    status_add_(GX_STATUS_NONCLIENT);
    status_remove_(GX_STATUS_ACCEPTS_FOCUS);

    draw_function = (void (*)(GX_WIDGET*))gx_scrollbar_draw_;
    event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_scrollbar_event_process_;

    if (parent)
    {
        link_to_(parent);

        if (parent->is_visible_())
        {
            parent->child_clipping_update_();
        }
    }

    return GX_SUCCESS;
}

void GX_SCROLLBAR::reset_(const GX_SCROLL_INFO* info)
{
    if (info)
    {
        this->info = *info;
    }
    else
    {
        // assert(parent->is_window_());

        GX_WINDOW* win = (GX_WINDOW*)parent;
        if (win)
        {
            win->scroll_info_get(win, style, &this->info);
        }
    }

    size_update_();
    value_to_thumb_();

    dirty_mark_if_visible_();
}

void GX_SCROLLBAR::size_update_()
{
    if (!parent)
    {
        return;
    }

    // assert(parent->is_window_());

    const GX_WINDOW* parent_win = (const GX_WINDOW*)parent;
    const GX_RECTANGLE& parent_client = parent_win->client;

    GX_RECTANGLE new_size;

    if (is_vertical_())
    {
        new_size.top = parent_client.top;
        new_size.bottom = parent_client.bottom;
        new_size.right = (GX_VALUE)(parent_win->right_() - parent_win->border_width_());
        new_size.left = (GX_VALUE)(new_size.right - appearance.width + 1);
    }
    else
    {
        new_size.left = parent_win->client.left;
        new_size.right = parent_win->client.right;
        new_size.bottom = (GX_VALUE)(parent_win->bottom_() - parent_win->border_width_());
        new_size.top = (GX_VALUE)(new_size.bottom - appearance.width + 1);
    }

    resize_(&new_size);

    if (style & GX_SCROLLBAR_END_BUTTONS)
    {
        if (is_vertical_()) // up button and down button
        {
            new_size.left = (GX_VALUE)(left_() + 1);
            new_size.right = (GX_VALUE)(right_() - 1);

            new_size.top = (GX_VALUE)(top_() + 1);
            new_size.bottom = (GX_VALUE)(new_size.top + appearance.width - 1);
            upleft.resize_(&new_size);

            new_size.bottom = (GX_VALUE)(bottom_() - 1);
            new_size.top = (GX_VALUE)(new_size.bottom - appearance.width + 1);
            downright.resize_(&new_size);
        }
        else // left button and right button
        {
            new_size.top = (GX_VALUE)(top_() + 1);
            new_size.bottom = (GX_VALUE)(bottom_() - 1);

            new_size.left = (GX_VALUE)(left_() + 1);
            new_size.right = (GX_VALUE)(new_size.left + appearance.width - 1);
            upleft.resize_(&new_size);

            new_size.right = (GX_VALUE)(right_() - 1);
            new_size.left = (GX_VALUE)(new_size.right - appearance.width + 1);
            downright.resize_(&new_size);
        }
    }
}

void GX_SCROLLBAR::value_set_(INT value)
{
    INT old_value = info.value;
    info.value = value_clamp_(value);
    if (info.value != old_value)
    {
        value_to_thumb_();
        value_changed_(old_value);

        dirty_mark_if_visible_();
    }
}

void GX_SCROLLBAR::value_to_thumb_()
{
    INT travel_size = is_vertical_() ? height_() : width_();
    travel_size -= appearance.thumb_travel_max + appearance.thumb_travel_min;

    INT range = info.maximum - info.minimum + 1;

    INT thumb_width = appearance.thumb_width;
    INT thumb_length = 0;

    if (style & GX_SCROLLBAR_RELATIVE_THUMB)
    {
        if (info.maximum != info.minimum)
        {
            thumb_length = travel_size;

            if (range)
            {
                thumb_length = thumb_length * info.visible / range;
            }

            if (thumb_length < thumb_width)
            {
                thumb_length = thumb_width;  // just a square thumb button
            }
        }
    }
    else
    {
        GX_PIXELMAP* thumb_pixelmap = GX_NULL;
        if (appearance.thumb_pixelmap)
        {
            pixelmap_get_(appearance.thumb_pixelmap, &thumb_pixelmap);
        }

        if (thumb_pixelmap)
        {
            if (is_vertical_())
            {
                thumb_length = thumb_pixelmap->height;
                thumb_width = thumb_pixelmap->width;
            }
            else
            {
                thumb_length = thumb_pixelmap->width;
                thumb_width = thumb_pixelmap->height;
            }
        }
        else
        {
            thumb_length = thumb_width; // just a square thumb button
        }
    }

    INT space = travel_size - thumb_length;
    space *= info.value - info.minimum;

    range -= info.visible;
    if (range)
    {
        space = (space + range / 2) / range;
    }

    if (info.value > info.minimum &&
        info.value + info.visible - 1 < info.maximum)
    {
        if (space == 0)
        {
            space = 1;
        }
        else if (space == (travel_size - thumb_length))
        {
            space -= 1;
        }
    }

    GX_RECTANGLE thumb_rect;

    if (is_vertical_())
    {
        thumb_rect.left = (GX_VALUE)(left_() + (width_() - thumb_width) / 2);
        thumb_rect.right = (GX_VALUE)(thumb_rect.left + thumb_width - 1);

        thumb_rect.top = (GX_VALUE)(top_() + appearance.thumb_travel_min + space);
        thumb_rect.bottom = (GX_VALUE)(thumb_rect.top + thumb_length - 1);
    }
    else
    {
        thumb_rect.top = (GX_VALUE)(top_() + (height_() - thumb_width) / 2);
        thumb_rect.bottom = (GX_VALUE)(thumb_rect.top + thumb_width - 1);

        thumb_rect.left = (GX_VALUE)(left_() + appearance.thumb_travel_min + space);
        thumb_rect.right = (GX_VALUE)(thumb_rect.left + thumb_length - 1);
    }

    thumb.resize_(&thumb_rect);
}

void GX_SCROLLBAR::thumb_to_value_(INT thumb_pos, INT thumb_length)
{
    // calculate range of allowed values
    INT range = info.maximum - info.minimum  + 1;
    range -= info.visible;

    INT new_value;
    INT travel_size;

    // scale this by distance thumb has traveled from minimum
    if (is_vertical_())
    {
        new_value = range * (thumb_pos - (top_() + appearance.thumb_travel_min));
        travel_size = height_();
    }
    else
    {
        new_value = range * (thumb_pos - (left_() + appearance.thumb_travel_min));
        travel_size = width_();
    }

    travel_size -= appearance.thumb_travel_max + appearance.thumb_travel_min;
    travel_size -= thumb_length;

    // scale the value using linear interpolation
    if (travel_size)
    {
        new_value = (new_value + travel_size / 2) / travel_size;
    }

    // offset the value based on minimum
    new_value += info.minimum;

    INT old_value = info.value;
    info.value = value_clamp_(new_value);
    if (info.value != old_value)
    {
        value_changed_(old_value);
    }
}

void GX_SCROLLBAR::value_clamp_()
{
    info.value = value_clamp_(info.value);
}

INT GX_SCROLLBAR::value_clamp_(INT value) const
{
    if (value < info.minimum)
    {
        value = info.minimum;
    }

    INT max_value = info.maximum - info.visible + 1;
    if (value > max_value)
    {
        value = max_value;
    }

    return value;
}

void GX_SCROLLBAR::value_changed_(INT old_value) const
{
    GX_EVENT e;

    if (is_vertical_())
    {
        e.type = GX_EVENT_VERTICAL_SCROLL;
    }
    else
    {
        e.type = GX_EVENT_HORIZONTAL_SCROLL;
    }

    e.target = parent;
    e.sender = id;

    e.payload.intdata[0] = info.value;
    e.payload.intdata[1] = old_value;

    _gx_system_event_send(&e);
}

void GX_SCROLLBAR::draw_()
{
    if (!(style & GX_STYLE_TRANSPARENT))
    {
        GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

        GX_RECTANGLE bar_size = size;

        // Draw background

        GX_RESOURCE_ID fill_color = fill_color_();

        if (style & GX_STYLE_BORDER_THIN)
        {
            context->brush_set_(GX_COLOR_ID_SHADOW, fill_color, GX_BRUSH_SOLID_FILL);
            context->brush.width = 1;
        }
        else
        {
            context->brush_set_(fill_color, fill_color, GX_BRUSH_SOLID_FILL);
            context->brush.width = 0;
        }

        _gx_canvas_rectangle_draw(&bar_size);

        // Draw pixelmaps

        GX_VALUE x;
        GX_VALUE y;
        GX_PIXELMAP* map;

        context->pixelmap_get_(appearance.up_pixelmap, &map);
        if (map)
        {
            if (is_vertical_())
            {
                x = (GX_VALUE)(bar_size.left + (width_() - map->width) / 2);
                y = bar_size.top;

                bar_size.top = (GX_VALUE)(bar_size.top + map->height);
            }
            else
            {
                x = bar_size.left;
                y = (GX_VALUE)(bar_size.top + (height_() - map->height) / 2);

                bar_size.left = (GX_VALUE)(bar_size.left + map->width);
            }

            _gx_canvas_pixelmap_draw(x, y, map);
        }

        context->pixelmap_get_(appearance.down_pixelmap, &map);
        if (map)
        {
            if (is_vertical_())
            {
                bar_size.bottom = (GX_VALUE)(bar_size.bottom - map->height);

                x = (GX_VALUE)(bar_size.left + (width_() - map->width) / 2);
                y = bar_size.bottom;
            }
            else
            {
                bar_size.right = (GX_VALUE)(bar_size.right - map->width);

                x = bar_size.right;
                y = (GX_VALUE)(bar_size.top + (height_() - map->height) / 2);
            }

            _gx_canvas_pixelmap_draw(x, y, map);
        }

        context->pixelmap_get_(appearance.fill_pixelmap, &map);
        if (map)
        {
            // Draw background pixelmap.
            if (style & GX_STYLE_TILE_BACKGROUND)
            {
                _gx_canvas_pixelmap_tile(&bar_size, map);
            }
            else
            {
                x = (GX_VALUE)(bar_size.left + (width_() - map->width) / 2);
                y = (GX_VALUE)(bar_size.top + (height_() - map->height) / 2);

                GX_RECTANGLE old_dirty = context->dirty;
                if(gx_rectangle_intersect_(old_dirty, bar_size, &bar_size))
                {
                    context->dirty = bar_size;

                    _gx_canvas_pixelmap_draw(x, y, map);

                    context->dirty = old_dirty;
                }
            }
        }
    }

    children_draw_();
}

void GX_SCROLLBAR::parent_window_client_updated_() const
{
    GX_EVENT e;
    e.type = GX_EVENT_CLIENT_UPDATED;
    e.target = parent;
    _gx_system_event_fold(&e);
}

UINT GX_SCROLLBAR::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;
    GX_WINDOW* parent_win;
    GX_SCROLLBAR* other_scrollbar;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        result = GX_WIDGET::event_process_(event_ptr);

        if (appearance.fill_pixelmap && pixelmap_is_transparent_(appearance.fill_pixelmap))
        {
            status_add_(GX_STATUS_TRANSPARENT);
        }

        parent_win = (GX_WINDOW*)parent;
        if (parent_win)
        {
            GX_RECTANGLE& parent_client = parent_win->client;
            GX_VALUE edge;

            if (is_vertical_())
            {
                if ((style & GX_STYLE_TRANSPARENT) || (this->status & GX_STATUS_HIDDEN))
                {
                    edge = (GX_VALUE)(parent_win->right_() - parent_win->border_width_());
                }
                else
                {
                    edge = (GX_VALUE)(parent_client.right - appearance.width);
                }

                if (parent_client.right != edge)
                {
                    parent_client.right = edge;
                    parent_win->child_clipping_update_();
                }

                other_scrollbar = parent_win->scrollbar_find_(GX_TYPE_HORIZONTAL_SCROLL);
            }
            else
            {
                if ((style & GX_STYLE_TRANSPARENT) || (this->status & GX_STATUS_HIDDEN))
                {
                    edge = (GX_VALUE)(parent_win->bottom_() - parent_win->border_width_());
                }
                else
                {
                    edge = (GX_VALUE)(parent_client.bottom - appearance.width);
                }

                if (parent_client.bottom != edge)
                {
                    parent_client.bottom = edge;
                    parent_win->child_clipping_update_();
                }

                other_scrollbar = parent_win->scrollbar_find_(GX_TYPE_VERTICAL_SCROLL);
            }

            if (other_scrollbar && other_scrollbar->is_visible_())
            {
                other_scrollbar->reset_();
            }

            parent_window_client_updated_();
        }

        reset_();
        break;

    case GX_EVENT_HIDE:
        result = GX_WIDGET::event_process_(event_ptr);

        parent_win = (GX_WINDOW*)parent;
        if (parent_win)
        {
            GX_RECTANGLE& parent_client = parent_win->client;

            if (is_vertical_())
            {
                parent_client.right = (GX_VALUE)(parent_win->right_() - parent_win->border_width_());
                parent_win->child_clipping_update_();

                other_scrollbar = parent_win->scrollbar_find_(GX_TYPE_HORIZONTAL_SCROLL);
            }
            else
            {
                parent_client.bottom = (GX_VALUE)(parent_win->bottom_() - parent_win->border_width_());
                parent_win->child_clipping_update_();

                other_scrollbar = parent_win->scrollbar_find_(GX_TYPE_VERTICAL_SCROLL);
            }

            if (other_scrollbar && other_scrollbar->is_visible_())
            {
                other_scrollbar->size_update_();
            }

            if (parent_win->is_visible_())
            {
                parent_window_client_updated_();
            }
        }

        break;

    case GX_EVENT_PEN_DOWN:
        {
            GX_RECTANGLE rect = size;

            if (upleft.left_() != upleft.right_())
            {
                if (is_vertical_())
                {
                    rect.top = (GX_VALUE)(upleft.bottom_() + 1);
                    rect.bottom = (GX_VALUE)(downright.top_() - 1);
                }
                else
                {
                    rect.left = (GX_VALUE)(upleft.right_() + 1);
                    rect.right = (GX_VALUE)(downright.left_() - 1);
                }
            }

            // Calculate valid rectangle here to avoid situation that click on scrollbar end button,
            // since we delivery PEN_DOWN event to parent, if we click on scrollbar end buttons,
            // the PEN_DOWN event will also be sent to it's parent,
            // which handles scrollbar thumb in another way.

            GX_POINT pen_point = event_ptr->payload.pointdata;

            if (rect.contain_point_(pen_point))
            {
                thumb.size.center_find_(&event_ptr->payload.pointdata);
                thumb.event_process_function_run_(event_ptr);

                GX_EVENT e;
                e.type = GX_EVENT_PEN_DRAG;
                e.payload.pointdata = pen_point;
                thumb.event_process_function_run_(&e);
            }
        }
        break;

    case GX_EVENT_VERTICAL_SCROLL:
    case GX_EVENT_HORIZONTAL_SCROLL:
        thumb_to_value_(event_ptr->payload.intdata[0], event_ptr->payload.intdata[1]);
        break;

    case GX_EVENT_PEN_UP:
        break;

    case GX_SIGNAL(GX_ID_SCROLL_UP_LEFT, GX_EVENT_CLICKED):
        value_set_(info.value - info.increment);
        break;

    case GX_SIGNAL(GX_ID_SCROLL_DOWN_RIGHT, GX_EVENT_CLICKED):
        value_set_(info.value + info.increment);
        break;

    case GX_EVENT_PARENT_SIZED:
        reset_();
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
/*    _gx_horizontal_scrollbar_create                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a horizontal scrollbar.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             scroll control block          */
/*    name                                  name of scroll                */
/*    parent                                parent window                 */
/*    appearance                            appearance of scroll bar      */
/*    style                                 style of scroll bar           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define          Define a rectangle            */
/*    _gx_widget_create                     Create the underlying prompt  */
/*    _gx_system_scroll_appearance_get      Get the scrollbar appearance  */
/*    _gx_button_create                     Button create call            */
/*    [gx_window_scroll_info_get]           Retrieve window scroll info   */
/*    _gx_scroll_thumb_create               Create the thrum button       */
/*    _gx_widget_link                       Link the widget to its parent */
/*    _gx_widget_child_clipping_update      Update the clipping area      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_horizontal_scrollbar_create(GX_SCROLLBAR* scrollbar, const GX_CHAR* name, GX_WINDOW* parent,
                                     GX_SCROLLBAR_APPEARANCE* appearance, ULONG style)
{
    return scrollbar->create_horizontal_(name, parent, appearance, style);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_vertical_scrollbar_create                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service creates a vertical scrollbar.                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             Scrollbar control block       */
/*    name                                  Name of scrollbar             */
/*    parent                                Pointer to parent widget      */
/*    appearance                            Appearance of vertical        */
/*                                            scrollbar widget            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define          Define a rectangle            */
/*    _gx_system_scroll_appearance_get      Retrieve scroll bar settings  */
/*    _gx_button_create                     Create a button               */
/*    _gx_widget_status_add                 Set status bit                */
/*    _gx_widget_status_remove              Remove status bit             */
/*    [gx_window_scroll_info_get]           Retrieve widget scroll info   */
/*    _gx_scroll_thumb_create               Create scroll thumb image     */
/*    _gx_pixelmap_transparent_detect       Detect whether or not a       */
/*                                            pixelmap contains           */
/*                                            transparency information    */
/*    _gx_widget_link                       Link a widget to its parent   */
/*    _gx_widget_child_clipping_update      Update child clipping info    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_vertical_scrollbar_create(GX_SCROLLBAR* scrollbar, const GX_CHAR* name, GX_WINDOW* parent,
                                   GX_SCROLLBAR_APPEARANCE* appearance, ULONG style)
{
    return scrollbar->create_vertical_(name, parent, appearance, style);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scrollbar_draw                                                  */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified scroll bar, which is a            */
/*      special type of widget.                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             Scrollbar widget to draw      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_brush_define              Define the brush for the      */
/*                                            context                     */
/*    _gx_context_brush_width_set           Set the width of the brush    */
/*    _gx_canvas_rectangle_draw             Draw rectangle                */
/*    _gx_context_pixelmap_get              Retrieve pixelmap image       */
/*    _gx_canvas_pixelmap_tile              Tile the canvas area with     */
/*                                            pixelmap                    */
/*    _gx_canvas_pixelmap_draw              Draw pixelmap                 */
/*    _gx_widget_width_get                  Retrieve the width of the     */
/*                                            widget                      */
/*    _gx_widget_height_get                 Retrieve the height of the    */
/*                                            widget                      */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_scrollbar_draw(GX_SCROLLBAR* scrollbar)
{
    scrollbar->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scrollbar_event_process                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes events for the specified window.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             Scrollbar control block       */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_center_find     Locate the center of a        */
/*                                             rectangle                  */
/*    [gx_widget_event_process_function]    Parent widget event process   */
/*                                            routine                     */
/*    _gx_scrollbar_thumb_position_calculate                              */
/*                                          Calculate the position of the */
/*                                            thumb                       */
/*    _gx_scrollbar_limit_check             Check for scroll limit        */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*    _gx_scrollbar_value_calcualte         Calculate the value of the    */
/*                                            scrollbar                   */
/*    _gx_scrollbar_reset                   Reset the scrollbar           */
/*    _gx_widget_border_width_get           Retrieve widget border width  */
/*    _gx_widget_child_clipping_update      Update widget child clipping  */
/*                                            area                        */
/*    _gx_widget_type_find                  Find the type of the widget   */
/*    _gx_scrollbar_size_update             Update the size of scrollbar  */
/*    _gx_widget_event_process              Invoke the widget event       */
/*                                            process routine             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_scrollbar_event_process(GX_SCROLLBAR* scrollbar, GX_EVENT* event_ptr)
{
    return scrollbar->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scrollbar_limit_check                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Check the scrollbar assigned value and make sure it is within the   */
/*    range limits.                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             Scrollbar control block       */
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
UINT _gx_scrollbar_limit_check(GX_SCROLLBAR* scrollbar)
{
    scrollbar->value_clamp_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scrollbar_reset                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service resets the scrollbar.                                  */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             Scrollbar control block       */
/*    info                                  Pointer to GX_SCROLL          */
/*                                            structure that defines the  */
/*                                            scrollbar limits, current   */
/*                                            value, and step or          */
/*                                            increment.                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_window_scroll_info_get]           Get the window scroll info    */
/*    _gx_scrollbar_size_update             Update the scroll size        */
/*    _gx_scrollbar_thumb_position_calculate                              */
/*                                          Calculate the scrollbar thumb */
/*                                          position                      */
/*    _gx_system_dirty_mark                 Mark system block dirty       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_scrollbar_reset(GX_SCROLLBAR* scrollbar, GX_SCROLL_INFO* info)
{
    scrollbar->reset_(info);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scrollbar_value_set                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service resets the scrollbar.                                  */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             Scrollbar control block       */
/*    value                                 New scrollbar value           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scrollbar_limit_check             Check value limits            */
/*    _gx_scrollbar_thumb_position_calculate                              */
/*                                          Calculate the scrollbar thumb */
/*                                          position                      */
/*    _gx_system_dirty_mark                 Mark system block dirty       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_scrollbar_value_set(GX_SCROLLBAR* scrollbar, INT value)
{
    scrollbar->value_set_(value);

    return GX_SUCCESS;
}
