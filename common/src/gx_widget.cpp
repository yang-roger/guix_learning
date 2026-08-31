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
/**   Widget Management (Widget)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_widget.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_display.h"
#include "gx_canvas.h"
#include "gx_font.h"
#include "gx_pixelmap.h"
#include "gx_window_root.h"

static void gx_widget_draw_(GX_WIDGET* widget)
{
    widget->draw_();
}

static UINT gx_widget_event_process_(GX_WIDGET* widget, GX_EVENT* event_ptr)
{
    return widget->event_process_(event_ptr);
}

UINT GX_WIDGET::create_(const GX_CHAR* name, GX_WIDGET* parent,
                        ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    GX_SYSTEM_AUTO_LOCK lock;

    this->type = GX_TYPE_WIDGET;

    this->name = name;
    this->id = Id;
    this->style = style;
    this->size = *size;
    this->clip = this->size;
    this->normal_fill_color = GX_COLOR_ID_WIDGET_FILL;
    this->selected_fill_color = GX_COLOR_ID_SELECTED_FILL;
    this->disabled_fill_color = GX_COLOR_ID_DISABLED_FILL;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    this->alpha = GX_ALPHA_VALUE_OPAQUE;
#endif

    this->draw_function = gx_widget_draw_;
    this->event_process_function = gx_widget_event_process_;

    if (style & GX_STYLE_DYNAMICALLY_ALLOCATED)
    {
        this->status = GX_STATUS_DYNAMICALLY_ALLOCATED;
    }
    else
    {
        this->status = 0;
    }

    if (style & GX_STYLE_ENABLED)
    {
        this->status |= (GX_STATUS_ACCEPTS_FOCUS | GX_STATUS_SELECTABLE);
    }

    if (style & GX_STYLE_TRANSPARENT)
    {
        this->status |= GX_STATUS_TRANSPARENT;
    }

    this->parent = GX_NULL;
    this->first_child = GX_NULL;
    this->last_child = GX_NULL;
    this->previous = GX_NULL;
    this->next = GX_NULL;
    this->nav_previous = GX_NULL;
    this->nav_next = GX_NULL;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_WIDGET::delete_()
{
    GX_SYSTEM_AUTO_LOCK lock;

    GX_WIDGET* child;

    // delete children
    while (first_child)
    {
        child = first_child;
        while (child->first_child)
        {
            child = child->first_child;
        }

        child->delete_helper_();
    }

    // delete this
    this->delete_helper_();
}

void GX_WIDGET::delete_helper_()
{
    GX_EVENT e;
    e.type = GX_EVENT_DELETE;
    e.target = this;
    event_process_function_run_(&e);

    unlink_();

    this->type = 0;

    _gx_system_dirty_list_remove(this);
    _gx_system_timer_stop(this, 0);
    _gx_system_event_remove(this);

    if (_gx_system_focus_owner == this)
    {
        _gx_system_focus_owner = GX_NULL;
    }

    if (this->status & GX_STATUS_DYNAMICALLY_ALLOCATED)
    {
        _gx_widget_free(this);
    }
    else
    {
        this->id = 0;
        this->status = 0;
    }
}

void GX_WIDGET::attach_to_(GX_WIDGET* to_parent)
{
    GX_SYSTEM_AUTO_LOCK lock;

    if (this->parent)
    {
        detach_();
    }

    link_to_(to_parent);

    status_remove_(GX_STATUS_HIDDEN);
}

void GX_WIDGET::back_attach_to_(GX_WIDGET* to_parent)
{
    GX_SYSTEM_AUTO_LOCK lock;

    if (this->parent)
    {
        detach_();
    }

    back_link_to_(to_parent);
}

void GX_WIDGET::detach_()
{
    GX_SYSTEM_AUTO_LOCK lock;

    if (is_visible_())
    {
        hide_();
    }

    unlink_();
}

void GX_WIDGET::link_to_(GX_WIDGET* to_parent)
{
    if (this->parent)
    {
        if (this->parent == to_parent)
        {
            return; // already linked as child of this parent, nothing to do
        }

        unlink_();
    }

    if (to_parent)
    {
        if (to_parent->first_child)
        {
            link_as_last_child_of_(to_parent);
        }
        else
        {
            link_as_only_child_of_(to_parent);
        }

        this->parent = to_parent;

        if (to_parent->is_visible_())
        {
            show_();
        }
    }
}

void GX_WIDGET::back_link_to_(GX_WIDGET* to_parent)
{
    if (to_parent)
    {
        if (to_parent->first_child)
        {
            link_as_first_child_of_(to_parent);
        }
        else
        {
            link_as_only_child_of_(to_parent);
        }

        this->parent = to_parent;

        if (to_parent->is_visible_())
        {
            show_();

            if (this->status & GX_STATUS_ACCEPTS_FOCUS)
            {
                to_parent->nav_order_initialize_();
            }
        }
    }
}

GX_WIDGET* GX_WIDGET::unlink_()
{
    if (parent)
    {
        if (parent->first_child == this)
        {
            parent->first_child = next;
            if (next)
            {
                next->previous = GX_NULL;
            }

            if (parent->last_child == this)
            {
                parent->last_child = next;
            }
        }
        else
        {
            previous->next = next;
            if (next)
            {
                next->previous = previous;
            }

            if (parent->last_child == this)
            {
                parent->last_child = previous;
            }
        }

        if (is_visible_())
        {
            hide_();
        }

        parent = GX_NULL;
        previous = GX_NULL;
        next = GX_NULL;
        nav_previous = GX_NULL;
        nav_next = GX_NULL;
    }

    return this;
}

void GX_WIDGET::link_as_first_child_of_(GX_WIDGET* to_parent)
{
    // assert(to_parent);
    // assert(to_parent->first_child);

    to_parent->first_child->previous = this;
    this->next = to_parent->first_child;
    this->previous = GX_NULL;

    to_parent->first_child = this;
}

void GX_WIDGET::link_as_last_child_of_(GX_WIDGET* to_parent)
{
    // assert(to_parent);
    // assert(to_parent->last_child);

    to_parent->last_child->next = this;
    this->previous = to_parent->last_child;
    this->next = GX_NULL;

    to_parent->last_child = this;
}

void GX_WIDGET::link_as_only_child_of_(GX_WIDGET* to_parent)
{
    // assert(to_parent);
    // assert(!to_parent->first_child);
    // assert(!to_parent->last_child);

    this->previous = GX_NULL;
    this->next = GX_NULL;

    to_parent->first_child = this;
    to_parent->last_child = this;
}

UINT GX_WIDGET::move_to_front_()
{
    if (!parent)
    {
        return GX_PTR_ERROR;
    }

    if (parent->last_child == this) // already in frontmost
    {
        return GX_NO_CHANGE;
    }

    GX_RECTANGLE dirty_area;
    dirty_area.set_(GX_VALUE_MAX, GX_VALUE_MAX, -1, -1);

    GX_RECTANGLE overlap;

    for (GX_WIDGET* sibling = next; sibling; sibling = sibling->next)
    {
        if (gx_rectangle_intersect_(this->size, sibling->size, &overlap))
        {
            if (dirty_area.left > dirty_area.right)
            {
                dirty_area = overlap;
            }
            else
            {
                dirty_area.combine_(overlap);
            }
        }
    }

    if (dirty_area.is_valid_())
    {
        dirty_mark_(&dirty_area);
    }

    // unlink this widget
    if (parent->first_child == this)
    {
        parent->first_child = next;
        if (next)
        {
            next->previous = GX_NULL;
        }
    }
    else
    {
        previous->next = next;
        if (next)
        {
            next->previous = previous;
        }
    }

    link_as_last_child_of_(parent);

    if (is_window_())
    {
        ((GX_WINDOW*)this)->views_update_detect_();

        if ((parent->status & GX_STATUS_HAS_FOCUS) &&
            (this->status & GX_STATUS_ACCEPTS_FOCUS) &&
            !(this->status & GX_STATUS_HAS_FOCUS))
        {
            _gx_system_focus_claim(this);
        }
    }

    return GX_SUCCESS;
}

UINT GX_WIDGET::move_to_back_()
{
    if (!parent)
    {
        return GX_PTR_ERROR;
    }

    if (parent->first_child == this) // already in backmost
    {
        return GX_NO_CHANGE;
    }

    parent->dirty_mark_(&this->size);

    // unlink this widget
    if (parent->last_child == this)
    {
        parent->last_child = previous;
        if (previous)
        {
            previous->next = GX_NULL;
        }
    }
    else
    {
        previous->next = next;
        if (next)
        {
            next->previous = previous;
        }
    }

    link_as_first_child_of_(parent);

    return GX_SUCCESS;
}

bool GX_WIDGET::is_ancestor_of_(const GX_WIDGET* child) const
{
    while (child->parent)
    {
        if (child->parent == this)
        {
            return true;
        }

        child = child->parent;
    }

    return false;
}

GX_WINDOW_ROOT* GX_WIDGET::root_window_() const
{
    GX_WIDGET* widget = const_cast<GX_WIDGET*>(this);

    while (widget->parent)
    {
        widget = widget->parent;
    }

    if (widget->is_root_window_())
    {
        return static_cast<GX_WINDOW_ROOT*>(widget);
    }

    return GX_NULL;
}

GX_WIDGET* GX_WIDGET::child_find_by_id_(USHORT widget_id, INT search_depth) const
{
    GX_WIDGET* child = first_child;
    while (child)
    {
        if (child->id == widget_id)
        {
            return child;
        }

        if (search_depth > 0)
        {
            if (child->first_child)
            {
                --search_depth;
                child = child->first_child;
                continue;
            }
        }

        while (!child->next && child != this)
        {
            ++search_depth;
            child = child->parent;
        }

        if (child == this)
        {
            break;
        }

        child = child->next;
    }

    return GX_NULL;
}

GX_WIDGET* GX_WIDGET::child_find_by_type_(USHORT type) const
{
    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        if (child->type == type)
        {
            return child;
        }
    }

    return GX_NULL;
}

GX_WIDGET* GX_WIDGET::top_visible_child_find_() const
{
    for (GX_WIDGET* child = last_child; child; child = child->previous)
    {
        if (child->is_visible_())
        {
            return child;
        }
    }

    return GX_NULL;
}

GX_WIDGET* GX_WIDGET::top_child_in_point_(GX_POINT test_point, ULONG status_flag) const
{
    GX_WIDGET* winner = GX_NULL;

    GX_WIDGET* child = last_child; // the last (frontmost) child
    while (child)
    {
        if (child->is_visible_() && child->clip.contain_point_(test_point))
        {
            if (status_flag)
            {
                if (child->status & status_flag)
                {
                    winner = child;
                }
            }
            else
            {
                winner = child;
            }

            child = child->last_child; // the last (frontmost) child
            continue;
        }

        child = child->previous;
    }

    return winner;
}

GX_WIDGET* GX_WIDGET::client_child_get_from_(GX_WIDGET* start) const
{
    for (GX_WIDGET* child = start; child; child = child->next)
    {
        if (child->is_client_())
        {
            return child;
        }
    }

    return GX_NULL;
}

GX_WIDGET* GX_WIDGET::last_client_child_get_() const
{
    for (GX_WIDGET* child = last_child; child; child = child->previous)
    {
        if (child->is_client_())
        {
            return child;
        }
    }

    return GX_NULL;
}

GX_WIDGET* GX_WIDGET::visible_client_child_get_from_(GX_WIDGET* start) const
{
    for (GX_WIDGET* child = start; child; child = child->next)
    {
        if (child->is_client_() && child->is_visible_())
        {
            return child;
        }
    }

    return GX_NULL;
}

GX_WIDGET* GX_WIDGET::last_visible_client_child_get_() const
{
    for (GX_WIDGET* child = last_child; child; child = child->previous)
    {
        if (child->is_client_() && child->is_visible_())
        {
            return child;
        }
    }

    return GX_NULL;
}

INT GX_WIDGET::client_child_index_get_(GX_WIDGET* client_child) const
{
    INT index = 0;

    GX_WIDGET* child = first_child;

    while (child && !child->is_client_())
    {
        child = child->next;
    }

    while (child)
    {
        if (child == client_child)
        {
            break;
        }

        if (child->is_client_())
        {
            ++index;
        }

        child = child->next;
    }

    return index;
}

void GX_WIDGET::style_add_(ULONG style)
{
    ULONG old_style = this->style;

    this->style |= style;

    if (style & GX_STYLE_TRANSPARENT)
    {
        status_add_(GX_STATUS_TRANSPARENT);
    }

    if (style & GX_STYLE_ENABLED)
    {
        status_add_(GX_STATUS_SELECTABLE);
    }

    dirty_mark_if_visible_();

    style_changed_(old_style);
}

void GX_WIDGET::style_remove_(ULONG style)
{
    ULONG old_style = this->style;

    this->style &= ~style;

    if (style & GX_STYLE_TRANSPARENT)
    {
        status_remove_(GX_STATUS_TRANSPARENT);
    }

    if (style & GX_STYLE_ENABLED)
    {
        status_remove_(GX_STATUS_SELECTABLE);
    }

    dirty_mark_if_visible_();

    style_changed_(old_style);
}

void GX_WIDGET::style_set_(ULONG style)
{
    ULONG old_style = this->style;

    this->style = style;

    if (style & GX_STYLE_TRANSPARENT)
    {
        if (!(this->status & GX_STATUS_TRANSPARENT))
        {
            if (is_window_() && is_visible_())
            {
                ((GX_WINDOW*)this)->views_update_detect_();
            }

            status_add_(GX_STATUS_TRANSPARENT);
        }
    }
    else
    {
        if (this->status & GX_STATUS_TRANSPARENT)
        {
            if (is_window_() && is_visible_())
            {
                ((GX_WINDOW*)this)->views_update_detect_();
            }

            status_remove_(GX_STATUS_TRANSPARENT);
        }
    }

    if (this->style & GX_STYLE_ENABLED)
    {
        status_add_(GX_STATUS_SELECTABLE);
    }
    else
    {
        status_remove_(GX_STATUS_SELECTABLE);
    }

    dirty_mark_if_visible_();

    style_changed_(old_style);
}

void GX_WIDGET::border_style_set_(ULONG style)
{
    ULONG old_style = this->style;

    this->style &= ~GX_STYLE_BORDER_MASK;
    this->style |= (style & GX_STYLE_BORDER_MASK);

    if (is_window_())
    {
        GX_WINDOW* win = (GX_WINDOW*)this;
        client_area_get_(&win->client);
    }

    style_changed_(old_style);
}

void GX_WIDGET::style_changed_(ULONG old_style) const
{
    GX_EVENT e;
    memset(&e, 0, sizeof(GX_EVENT));
    e.type = GX_EVENT_STYLE_CHANGED;
    e.target = const_cast<GX_WIDGET*>(this);
    e.payload.ulongdata = old_style;
    _gx_system_event_fold(&e);
}

void GX_WIDGET::fill_color_set_(GX_RESOURCE_ID normal_color, GX_RESOURCE_ID selected_color, GX_RESOURCE_ID disabled_color)
{
    this->normal_fill_color = normal_color;
    this->selected_fill_color = selected_color;
    this->disabled_fill_color = disabled_color;

    dirty_mark_if_visible_();
}

void GX_WIDGET::show_()
{
    status_remove_(GX_STATUS_HIDDEN);

    if (!is_visible_())
    {
        GX_EVENT e;
        e.type = GX_EVENT_SHOW;
        e.target = GX_NULL;
        e.sender = 0;
        event_process_function_run_(&e);

        clipping_update_();

        dirty_mark_();

        if (is_window_())
        {
            GX_WINDOW* win = (GX_WINDOW*)this;
            win->views_free_();
            win->views_update_detect_();
        }

        if (this->status & GX_STATUS_ACCEPTS_FOCUS)
        {
            if (parent)
            {
                parent->nav_order_initialize_();
            }
        }
    }
}

void GX_WIDGET::hide_()
{
    status_add_(GX_STATUS_HIDDEN);

    if (is_visible_())
    {
        GX_EVENT e;
        e.type = GX_EVENT_HIDE;
        e.target = GX_NULL;
        e.sender = 0;
        event_process_function_run_(&e);

        if (this->status & GX_STATUS_HAS_FOCUS)
        {
            GX_WIDGET* next_focused;

            if (this->nav_next == this)
            {
                // this widget is the last widget in navigation order, move focus to its parent.
                next_focused = parent;
            }
            else
            {
                // move focus to the next widget in navigation order.
                next_focused = nav_next;
            }

            if (next_focused)
            {
                _gx_system_focus_claim(next_focused);
            }
        }

        if (parent)
        {
            parent->dirty_mark_(&this->size);

            if (this->status & GX_STATUS_ACCEPTS_FOCUS)
            {
                parent->nav_order_initialize_();
            }
        }

        if (is_window_())
        {
            GX_WINDOW* win = (GX_WINDOW*)this;
            win->views_free_();
            win->views_update_detect_();
        }
    }
}

GX_VALUE GX_WIDGET::border_width_() const
{
    switch (style & GX_STYLE_BORDER_MASK)
    {
    case GX_STYLE_BORDER_RAISED:
    case GX_STYLE_BORDER_RECESSED:
        return 2;

    case GX_STYLE_BORDER_THIN:
        return 1;

    case GX_STYLE_BORDER_THICK:
        return 5;

    case GX_STYLE_BORDER_NONE:
    default:
        return 0;
    }
}

void GX_WIDGET::client_area_get_(GX_RECTANGLE* area, GX_VALUE border_width) const
{
    if (border_width < 0)
    {
        border_width = border_width_();
    }

    area->set_((GX_VALUE)(left_() + border_width),
               (GX_VALUE)(top_() + border_width),
               (GX_VALUE)(right_() - border_width),
               (GX_VALUE)(bottom_() - border_width));
}

GX_CANVAS* GX_WIDGET::canvas_get_() const
{
    GX_WINDOW_ROOT* root = root_window_();
    if (root)
    {
        return root->canvas;
    }

    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    if (context)
    {
        return context->canvas;
    }

    return GX_NULL;
}

UINT GX_WIDGET::color_get_(GX_RESOURCE_ID color_id, GX_COLOR* return_color) const
{
    UINT result = GX_INVALID_CANVAS;
    GX_COLOR color = 0;

    if (is_visible_())
    {
        GX_CANVAS* canvas = canvas_get_();
        if (canvas)
        {
            GX_DISPLAY* display = canvas->display;
            if (display)
            {
                result = display->color_get_(color_id, &color);
            }
            else
            {
                result = GX_INVALID_DISPLAY;
            }
        }
    }

    *return_color = color;

    return result;
}

UINT GX_WIDGET::font_get_(GX_RESOURCE_ID font_id, GX_FONT** return_font) const
{
    UINT result = GX_INVALID_CANVAS;
    GX_FONT* font = GX_NULL;

    if (is_visible_())
    {
        GX_CANVAS* canvas = canvas_get_();
        if (canvas)
        {
            GX_DISPLAY* display = canvas->display;
            if (display)
            {
                result = display->font_get_(font_id, &font);
            }
            else
            {
                result = GX_INVALID_DISPLAY;
            }
        }
    }

    *return_font = font;

    return result;
}

UINT GX_WIDGET::pixelmap_get_(GX_RESOURCE_ID pixelmap_id, GX_PIXELMAP** return_map) const
{
    UINT result = GX_INVALID_CANVAS;
    GX_PIXELMAP* pixelmap = GX_NULL;

    if (is_visible_())
    {
        GX_CANVAS* canvas = canvas_get_();
        if (canvas)
        {
            GX_DISPLAY* display = canvas->display;
            if (display)
            {
                result = display->pixelmap_get_(pixelmap_id, &pixelmap);
            }
            else
            {
                result = GX_INVALID_DISPLAY;
            }
        }
    }

    *return_map = pixelmap;

    return result;
}

UINT GX_WIDGET::string_get_ext_(GX_RESOURCE_ID string_id, GX_STRING* return_string) const
{
    UINT result = GX_INVALID_CANVAS;
    GX_STRING string;

    string.ptr = GX_NULL;
    string.length = 0;

    if (is_visible_())
    {
        GX_CANVAS* canvas = canvas_get_();
        if (canvas)
        {
            GX_DISPLAY* display = canvas->display;
            if (display)
            {
                result = display->string_get_ext_(string_id, &string);
            }
            else
            {
                result = GX_INVALID_DISPLAY;
            }
        }
    }

    *return_string = string;

    return result;
}

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)

GX_UBYTE GX_WIDGET::text_direction_get_() const
{
    GX_UBYTE direction = GX_LANGUAGE_DIRECTION_LTR;

    GX_CANVAS* canvas = canvas_get_();
    if (canvas)
    {
        GX_DISPLAY* display = canvas->display;
        if (display)
        {
            display->language_direction_get_(display->active_language, &direction);
        }
    }

    return direction;
}

#endif // GX_DYNAMIC_BIDI_TEXT_SUPPORT

bool GX_WIDGET::pixelmap_is_transparent_(GX_RESOURCE_ID pixelmap_id) const
{
    GX_PIXELMAP* pixelmap = GX_NULL;
    pixelmap_get_(pixelmap_id, &pixelmap);

    return pixelmap && PIXELMAP_IS_TRANSPARENT(pixelmap);
}

void GX_WIDGET::resized_notify_() const
{
    GX_EVENT e;
    memset(&e, 0, sizeof(GX_EVENT));
    e.type = GX_EVENT_RESIZED;
    e.target = const_cast<GX_WIDGET*>(this);
    _gx_system_event_fold(&e);
}

void GX_WIDGET::resize_(const GX_RECTANGLE* new_size)
{
    if (size == *new_size)
    {
        return;
    }

    if (is_visible_())
    {
        if (new_size->contain_rectangle_(size))
        {
            dirty_mark_(new_size);
        }
        else
        {
            GX_RECTANGLE dirty_area = size;
            dirty_area.combine_(*new_size);
            parent->dirty_mark_(&dirty_area);
        }
    }

    // keep track of how far I am shifting
    GX_VALUE left_shift = (GX_VALUE)(new_size->left - size.left);
    GX_VALUE top_shift = (GX_VALUE)(new_size->top - size.top);
    GX_VALUE right_shift = (GX_VALUE)(new_size->right - size.right);
    GX_VALUE bottom_shift = (GX_VALUE)(new_size->bottom - size.bottom);

    this->size = *new_size;

    // If this is a window, update its client area.
    if (is_window_())
    {
        GX_WINDOW* win = (GX_WINDOW*)this;

        win->client.adjust_(left_shift, top_shift, right_shift, bottom_shift);

        win->views_update_detect_();
    }

    // If this window top-left shifted, move children.
    if (left_shift || top_shift)
    {
        for (GX_WIDGET* child = first_child; child; child = child->next)
        {
            child->shift_(left_shift, top_shift, GX_FALSE);
        }
    }

    if (is_window_())
    {
        // tell children that I have been resized
        GX_EVENT e;
        memset(&e, 0, sizeof(GX_EVENT));
        e.type = GX_EVENT_PARENT_SIZED;
        children_event_process_run_(&e);
    }

    if (is_visible_())
    {
        clipping_update_();

        if (this->status & GX_STATUS_RESIZE_NOTIFY)
        {
            resized_notify_();
        }
    }
}

void GX_WIDGET::shift_(GX_VALUE x_shift, GX_VALUE y_shift, GX_BOOL mark_dirty)
{
    if (x_shift == 0 && y_shift == 0)
    {
        return;
    }

    this->shift_helper_(x_shift, y_shift, mark_dirty);

    GX_WIDGET* child = first_child;
    while (child)
    {
        child->shift_helper_(x_shift, y_shift, GX_FALSE);

        if (child->first_child)
        {
            child = child->first_child;
            continue;
        }

        while (!child->next && (child != this))
        {
            child = child->parent;
        }

        if (child == this)
        {
            break;
        }

        child = child->next;
    }

    if (mark_dirty && is_visible_())
    {
        clipping_update_();
    }
}

void GX_WIDGET::shift_helper_(GX_VALUE x_shift, GX_VALUE y_shift, GX_BOOL mark_dirty)
{
    GX_RECTANGLE new_size = size;
    new_size.move_(x_shift, y_shift);

    if (mark_dirty && is_visible_())
    {
        GX_RECTANGLE dirty_area = new_size;
        dirty_area.combine_(size);
        parent->dirty_mark_(&dirty_area);
    }

    this->size = new_size;

    if (is_window_())
    {
        GX_WINDOW* win = (GX_WINDOW*)this;

        win->client.move_(x_shift, y_shift);

        if (win->is_visible_())
        {
            win->views_update_detect_();
        }
    }

    if (is_visible_())
    {
        if (this->status & GX_STATUS_RESIZE_NOTIFY)
        {
            resized_notify_();
        }
    }
}

void GX_WIDGET::scroll_shift_(INT x_shift, INT y_shift, GX_BOOL clip)
{
    if (x_shift == 0 && y_shift == 0)
    {
        return;
    }

    this->scroll_shift_helper_(x_shift, y_shift);

    GX_WIDGET* child = first_child;
    while (child)
    {
        child->scroll_shift_helper_(x_shift, y_shift);

        if (child->first_child)
        {
            child = child->first_child;
            continue;
        }

        while (!child->next && (child != this))
        {
            child = child->parent;
        }

        if (child == this)
        {
            break;
        }

        child = child->next;
    }

    if (clip && is_visible_())
    {
        clipping_update_();
    }
}

void GX_WIDGET::scroll_shift_helper_(INT x_shift, INT y_shift)
{
    size.move_((GX_VALUE)x_shift, (GX_VALUE)y_shift);

    if (is_window_())
    {
        ((GX_WINDOW*)this)->client.move_((GX_VALUE)x_shift, (GX_VALUE)y_shift);
    }

    if (this->status & GX_STATUS_DIRTY)
    {
        _gx_system_dirty_entry_shift(this, x_shift, y_shift);
    }
}

UINT GX_WIDGET::block_move_(GX_RECTANGLE* block, INT x_shift, INT y_shift)
{
    if (this->status & GX_STATUS_TRANSPARENT)
    {
        dirty_mark_(block);

        return GX_SUCCESS;
    }

    GX_CANVAS* canvas = canvas_get_();
    if (!canvas)
    {
        return GX_INVALID_CANVAS;
    }
    else
    {
        GX_SYSTEM_AUTO_LOCK lock;

        if (GX_ABS(x_shift) >= width_() || GX_ABS(y_shift) >= height_())
        {
            dirty_mark_(block);
        }
        else
        {
            GX_DRAWING drawing(canvas, this, *block);
            if (drawing.result_() == GX_SUCCESS)
            {
                GX_RECTANGLE dirty;
                if (_gx_canvas_block_move(block, (GX_VALUE)x_shift, (GX_VALUE)y_shift, &dirty) == GX_SUCCESS)
                {
                    draw_in_area_(dirty, canvas, GX_FALSE);

                    drawing.enable_flush_(GX_TRUE);
                }
                else
                {
                    dirty_mark_(block);
                }
            }
        }
    }

    return GX_SUCCESS;
}

void GX_WIDGET::clipping_update_()
{
    if (parent)
    {
        clipping_update_helper_();
    }
    else
    {
        clip = size;
    }

    child_clipping_update_();
}

void GX_WIDGET::child_clipping_update_()
{
    GX_WIDGET* child = first_child;
    while (child)
    {
        child->clipping_update_helper_();

        if (child->first_child)
        {
            child = child->first_child;
            continue;
        }

        while (!child->next && (child != this))
        {
            child = child->parent;
        }

        if (child == this)
        {
            break;
        }

        child = child->next;
    }
}

void GX_WIDGET::clipping_update_helper_()
{
    // assert(parent);

    gx_rectangle_intersect_(parent->clip, size, &clip);

    if (parent->is_window_() && is_client_())
    {
        gx_rectangle_intersect_(((GX_WINDOW*)parent)->client, clip, &clip);
    }
}

void GX_WIDGET::nav_order_initialize_()
{
    bool assign_default_focus = true;

    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        if (assign_default_focus)
        {
            if (child->status & GX_STATUS_DEFAULT_FOCUS)
            {
                assign_default_focus = false;
            }
        }
        else
        {
            // make sure only one child has default focus
            child->status_remove_(GX_STATUS_DEFAULT_FOCUS);
        }

        // clear the navigation order
        child->nav_previous = GX_NULL;
        child->nav_next = GX_NULL;
    }

    // loop through children looking for the top-left child
    // and assigning the navigation order in top-to-bottom left-to-right order.

    GX_WIDGET* first_stop = GX_NULL;
    GX_WIDGET* last_stop = GX_NULL;

    GX_WIDGET* winner;
    while ((winner = next_nav_find_()) != GX_NULL)
    {
        if (!first_stop)
        {
            first_stop = winner;

            if (assign_default_focus)
            {
                first_stop->status_add_(GX_STATUS_DEFAULT_FOCUS);
            }
        }

        if (last_stop)
        {
            last_stop->nav_next = winner;
            winner->nav_previous = last_stop;
        }
        else
        {
            winner->nav_next = winner;
        }

        last_stop = winner;
    }

    // loop the last in the order back to the first
    if (last_stop)
    {
        last_stop->nav_next = first_stop;
        first_stop->nav_previous = last_stop;
    }
}

GX_WIDGET* GX_WIDGET::next_nav_find_() const
{
    GX_WIDGET* winner = GX_NULL;

    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        if (child->is_visible_() &&
            !child->nav_next &&
            (child->status & GX_STATUS_ACCEPTS_FOCUS) &&
            child->is_client_())
        {
            if (winner)
            {
                if ((child->top_() < winner->top_()) ||
                    (child->top_() == winner->top_() && child->left_() < winner->left_()))
                {
                    winner = child;
                }
            }
            else
            {
                winner = child;
            }
        }
    }

    return winner;
}

UINT GX_WIDGET::focus_next_()
{
    if (nav_next)
    {
        _gx_system_focus_claim(nav_next);

        return GX_SUCCESS;
    }

    return GX_FAILURE;
}

UINT GX_WIDGET::focus_previous_()
{
    if (nav_previous)
    {
        _gx_system_focus_claim(nav_previous);

        return GX_SUCCESS;
    }

    return GX_FAILURE;
}

GX_WIDGET* GX_WIDGET::default_focus_child_find_() const
{
    GX_WIDGET* winner = GX_NULL;

    GX_WIDGET* child = first_child;
    while (child)
    {
        if (child->status & GX_STATUS_DEFAULT_FOCUS)
        {
            winner = child;

            if (winner->status & GX_STATUS_NAV_PARENT)
            {
                break;
            }
            else
            {
                child = child->first_child;
            }
        }
        else
        {
            child = child->next;
        }
    }

    return winner;
}

GX_WIDGET* GX_WIDGET::accepts_focus_child_find_() const
{
    GX_WIDGET* winner = GX_NULL;

    GX_WIDGET* child = first_child;
    while (child)
    {
        if ((child->status & GX_STATUS_ACCEPTS_FOCUS) && child->is_client_())
        {
            winner = child;

            if (winner->status & GX_STATUS_NAV_PARENT)
            {
                break;
            }
            else
            {
                child = child->first_child;
            }
        }
        else
        {
            child = child->next;
        }
    }

    return winner;
}

void GX_WIDGET::child_focus_assign_()
{
    GX_WIDGET* child = default_focus_child_find_();
    if (child)
    {
        if (!(child->status & GX_STATUS_HAS_FOCUS) || child != _gx_system_focus_owner)
        {
            _gx_system_focus_claim(child);
        }

        return;
    }

    // nothing has default focus flag, so just try to
    // give focus to the first child that accepts focus

    child = accepts_focus_child_find_();
    if (child)
    {
        if (!(child->status & GX_STATUS_HAS_FOCUS))
        {
            _gx_system_focus_claim(child);
        }
    }
}

GX_VALUE GX_WIDGET::text_x_pos_(const GX_STRING* string, const GX_FONT* font) const
{
    GX_VALUE text_width;

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        _gx_system_string_width_get_ext(font, string, &text_width);
        return (GX_VALUE)(right_() - border_width_() - text_width);

    case GX_STYLE_TEXT_CENTER:
    default:
        _gx_system_string_width_get_ext(font, string, &text_width);
        return (GX_VALUE)(left_() + (width_() - text_width) / 2);

    case GX_STYLE_TEXT_LEFT:
        return (GX_VALUE)(left_() + border_width_());
    }
}

void GX_WIDGET::text_draw_ext_(UINT color_id, UINT font_id, const GX_STRING* string, INT x_offset, INT y_offset)
{
    if (!string || !string->ptr || !string->length)
    {
        return;
    }

    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

    context->line_color_set_(color_id);
    context->font_set_(font_id);

    GX_FONT* font = context->brush.font;
    if (!font)
    {
        return;
    }

    GX_VALUE x = text_x_pos_(string, font);
    GX_VALUE y = (GX_VALUE)(top_() + (height_() - font->line_height) / 2);

    _gx_canvas_text_draw_ext((GX_VALUE)(x + x_offset), (GX_VALUE)(y + y_offset), string);
}

void GX_WIDGET::text_blend_ext_(UINT color_id, UINT font_id, const GX_STRING* string, INT x_offset, INT y_offset, UCHAR alpha)
{
    _gx_system_current_draw_context->brush.alpha = alpha;

    text_draw_ext_(color_id, font_id, string, x_offset, y_offset);
}

void GX_WIDGET::text_id_draw_(UINT color_id, UINT font_id, UINT text_id, INT x_offset, INT y_offset)
{
    GX_STRING string;
    _gx_system_current_draw_context->string_get_ext_(text_id, &string);

    text_draw_ext_(color_id, font_id, &string, x_offset, y_offset);
}

void GX_WIDGET::pixelmap_draw_(const GX_PIXELMAP* pixelmap, GX_VALUE border_width, INT shift)
{
    _gx_system_current_draw_context->fill_color_set_(enabled_fill_color_());

    INT x;
    INT y;

    switch (style & GX_STYLE_PIXELMAP_HALIGN_MASK)
    {
    case GX_STYLE_PIXELMAP_HALIGN_RIGHT:
        x = right_() - border_width - pixelmap->width + 1;
        break;

    case GX_STYLE_PIXELMAP_HALIGN_CENTER:
        x = left_() + (width_() - pixelmap->width) / 2;
        break;

    case GX_STYLE_PIXELMAP_HALIGN_LEFT:
    default:
        x = left_() + border_width;
        break;
    }

    switch (style & GX_STYLE_PIXELMAP_VALIGN_MASK)
    {
    case GX_STYLE_PIXELMAP_VALIGN_BOTTOM:
        y = bottom_() - border_width - pixelmap->height + 1;
        break;

    case GX_STYLE_PIXELMAP_VALIGN_CENTER:
        y = top_() + (height_() - pixelmap->height) / 2;
        break;

    case GX_STYLE_PIXELMAP_VALIGN_TOP:
    default:
        y = top_() + border_width;
        break;
    }

    _gx_canvas_pixelmap_draw((GX_VALUE)(x + shift), (GX_VALUE)(y + shift), (GX_PIXELMAP*)pixelmap);
}

void GX_WIDGET::border_and_background_draw_()
{
    GX_RESOURCE_ID fill_color = fill_color_();
    border_and_background_draw_(GX_COLOR_ID_DEFAULT_BORDER, fill_color, fill_color, GX_TRUE);
}

void GX_WIDGET::border_and_background_draw_(GX_RESOURCE_ID border_color,
                                            GX_RESOURCE_ID upper_color,
                                            GX_RESOURCE_ID lower_color,
                                            GX_BOOL fill)
{
    background_draw_(upper_color, lower_color, fill);
    border_draw_(border_color);
}

void GX_WIDGET::background_draw_(GX_RESOURCE_ID upper_color,
                                 GX_RESOURCE_ID lower_color,
                                 GX_BOOL fill)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

    if (fill && !(style & GX_STYLE_TRANSPARENT))
    {
        GX_RECTANGLE fillrect;
        client_area_get_(&fillrect);

        context->brush_set_(upper_color, upper_color, GX_BRUSH_SOLID_FILL);
        context->brush.width = 0;

        if (upper_color != lower_color)
        {
            fillrect.bottom = (GX_VALUE)(fillrect.top + width_() / 2 - 1);
            _gx_canvas_rectangle_draw(&fillrect);

            context->fill_color_set_(lower_color);
            fillrect.top = (GX_VALUE)(fillrect.bottom + 1);
            fillrect.bottom = (GX_VALUE)(bottom_() - 1);
        }

        _gx_canvas_rectangle_draw(&fillrect);
    }
}

void GX_WIDGET::border_draw_(GX_RESOURCE_ID border_color)
{
    GX_VALUE border_width = border_width_();
    if (border_width <= 0)
    {
        return;
    }

    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    GX_BRUSH& brush = context->brush;

    bool rounded_corner = false;

    context->line_color_set_(border_color);

    INT left = left_();
    INT right = right_();

    if (parent && (!parent->is_root_window_() || (this->status & GX_STATUS_TRANSPARENT)))
    {
        rounded_corner = true;

        left += 2;
        right -= 2;
    }

    // top border

    INT y = top_();

    if (style & GX_STYLE_BORDER_THICK)
    {
        brush.width = border_width;
        y += border_width / 2;
    }
    else
    {
        brush.width = 1;
    }

    _gx_canvas_line_draw((GX_VALUE)left, (GX_VALUE)y, (GX_VALUE)right, (GX_VALUE)y);

    // bottom border

    y = bottom_();

    if (style & GX_STYLE_BORDER_THICK)
    {
        y = y - border_width + 1;
        y += border_width / 2;
    }

    _gx_canvas_line_draw((GX_VALUE)left, (GX_VALUE)y, (GX_VALUE)right, (GX_VALUE)y);

    // left border

    left = left_();

    if (style & GX_STYLE_BORDER_THICK)
    {
        left += border_width / 2;
    }

    if (rounded_corner)
    {
        _gx_canvas_line_draw((GX_VALUE)left, (GX_VALUE)(top_() + 2),
                             (GX_VALUE)left, (GX_VALUE)(bottom_() - 2));
    }
    else
    {
        _gx_canvas_line_draw((GX_VALUE)left, (GX_VALUE)(top_() + border_width - 1),
                             (GX_VALUE)left, (GX_VALUE)(bottom_() - border_width + 1));
    }

    // right border

    right = right_();

    if (style & GX_STYLE_BORDER_THICK)
    {
        right = right - border_width + 1;
        right += border_width / 2;

        _gx_canvas_line_draw((GX_VALUE)right, (GX_VALUE)(top_() + border_width - 1),
                             (GX_VALUE)right, (GX_VALUE)(bottom_() - border_width + 1));

        brush.width = 1;

        // highlight line
        context->line_color_set_(GX_COLOR_ID_SHINE);

        // draw highlight along top border
        _gx_canvas_line_draw((GX_VALUE)(left_()  + 2), (GX_VALUE)(top_() + 1),
                             (GX_VALUE)(right_() - 2), (GX_VALUE)(top_() + 1));

        // draw highlight along left border
        _gx_canvas_line_draw((GX_VALUE)(left_() + 1), (GX_VALUE)(top_() + 2),
                             (GX_VALUE)(left_() + 1), (GX_VALUE)(bottom_() - 2));

        // shadown line
        context->line_color_set_(GX_COLOR_ID_SHADOW);

        // draw shadow along right border
        _gx_canvas_line_draw((GX_VALUE)(right_() - 1), (GX_VALUE)(top_() + 2),
                             (GX_VALUE)(right_() - 1), (GX_VALUE)(bottom_() - 2));

        // draw shadow along bottom border
        _gx_canvas_line_draw((GX_VALUE)(left_()  + 2), (GX_VALUE)(bottom_() - 1),
                             (GX_VALUE)(right_() - 2), (GX_VALUE)(bottom_() - 1));
    }
    else
    {
        if (rounded_corner)
        {
            _gx_canvas_line_draw((GX_VALUE)right, (GX_VALUE)(top_() + 2),
                                 (GX_VALUE)right, (GX_VALUE)(bottom_() - 2));
        }
        else
        {
            _gx_canvas_line_draw((GX_VALUE)right, (GX_VALUE)(top_() + border_width - 1),
                                 (GX_VALUE)right, (GX_VALUE)(bottom_() - border_width + 1));
        }
    }

    if (rounded_corner)
    {
        GX_POINT corner;

        context->line_color_set_(border_color);

        // top-left corner
        corner.x = (GX_VALUE)(left_() + 2);
        corner.y = (GX_VALUE)(top_() + 1);
        _gx_canvas_pixel_draw(corner);
        corner.x = (GX_VALUE)(left_() + 1);
        _gx_canvas_pixel_draw(corner);
        corner.y = (GX_VALUE)(top_() + 2);
        _gx_canvas_pixel_draw(corner);

        // top-right corner
        corner.x = (GX_VALUE)(right_() - 2);
        corner.y = (GX_VALUE)(top_() + 1);
        _gx_canvas_pixel_draw(corner);
        corner.x = (GX_VALUE)(right_() - 1);
        _gx_canvas_pixel_draw(corner);
        corner.y = (GX_VALUE)(top_() + 2);
        _gx_canvas_pixel_draw(corner);

        // bottom-right corner
        corner.x = (GX_VALUE)(right_() - 2);
        corner.y = (GX_VALUE)(bottom_() - 1);
        _gx_canvas_pixel_draw(corner);
        corner.x = (GX_VALUE)(right_() - 1);
        _gx_canvas_pixel_draw(corner);
        corner.y = (GX_VALUE)(bottom_() - 2);
        _gx_canvas_pixel_draw(corner);

        // bottom-left corner
        corner.x = (GX_VALUE)(left_() + 2);
        corner.y = (GX_VALUE)(bottom_() - 1);
        _gx_canvas_pixel_draw(corner);
        corner.x = (GX_VALUE)(left_() + 1);
        _gx_canvas_pixel_draw(corner);
        corner.y = (GX_VALUE)(bottom_() - 2);
        _gx_canvas_pixel_draw(corner);
    }

    if (border_width != 2)
    {
        return;
    }

    // draw raised or recessed border

    brush.width = 1;

    if (style & GX_STYLE_BORDER_RAISED)
    {
        context->line_color_set_(GX_COLOR_ID_SHINE);
    }
    else
    {
        context->line_color_set_(GX_COLOR_ID_SHADOW);
    }

    if (rounded_corner)
    {
        // line down left edge
        _gx_canvas_line_draw((GX_VALUE)(left_() + 1), (GX_VALUE)(top_() + 3),
                             (GX_VALUE)(left_() + 1), (GX_VALUE)(bottom_() - 3));

        // line across the top
        _gx_canvas_line_draw((GX_VALUE)(left_()  + 3), (GX_VALUE)(top_() + 1),
                             (GX_VALUE)(right_() - 3), (GX_VALUE)(top_() + 1));

        GX_POINT corner;
        corner.y = (GX_VALUE)(top_() + 2);
        corner.x = (GX_VALUE)(left_() + 2);
        _gx_canvas_pixel_draw(corner);
        corner.x = (GX_VALUE)(right_() - 2);
        _gx_canvas_pixel_draw(corner);
    }
    else
    {
        // line down left edge
        _gx_canvas_line_draw((GX_VALUE)(left_() + 1), (GX_VALUE)(top_() + 2),
                             (GX_VALUE)(left_() + 1), (GX_VALUE)(bottom_() - 1));

        // line across the top
        _gx_canvas_line_draw((GX_VALUE)(left_()  + 1), (GX_VALUE)(top_() + 1),
                             (GX_VALUE)(right_() - 1), (GX_VALUE)(top_() + 1));
    }

    if (style & GX_STYLE_BORDER_RAISED)
    {
        context->line_color_set_(GX_COLOR_ID_SHADOW);
    }
    else
    {
        context->line_color_set_(GX_COLOR_ID_SHINE);
    }

    if (rounded_corner)
    {
        // line down right edge
        _gx_canvas_line_draw((GX_VALUE)(right_() - 1), (GX_VALUE)(top_() + 3),
                             (GX_VALUE)(right_() - 1), (GX_VALUE)(bottom_() - 3));

        // line across the bottom
        _gx_canvas_line_draw((GX_VALUE)(left_()  + 3), (GX_VALUE)(bottom_() - 1),
                             (GX_VALUE)(right_() - 3), (GX_VALUE)(bottom_() - 1));

        GX_POINT corner;
        corner.y = (GX_VALUE)(bottom_() - 2);

        corner.x = (GX_VALUE)(left_() + 2);
        _gx_canvas_pixel_draw(corner);

        corner.x = (GX_VALUE)(right_() - 2);
        _gx_canvas_pixel_draw(corner);
    }
    else
    {
        // line down right edge
        _gx_canvas_line_draw((GX_VALUE)(right_() - 1), (GX_VALUE)(top_() + 2),
                             (GX_VALUE)(right_() - 1), (GX_VALUE)(bottom_() - 1));

        // line across the bottom
        _gx_canvas_line_draw((GX_VALUE)(left_()  + 2), (GX_VALUE)(bottom_() - 1),
                             (GX_VALUE)(right_() - 2), (GX_VALUE)(bottom_() - 1));
    }
}

void GX_WIDGET::children_draw_()
{
    if (!first_child)
    {
        return;
    }

    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    GX_CANVAS* canvas = context->canvas;
    const GX_RECTANGLE& dirty_area = context->dirty;
    GX_RECTANGLE overlap;

    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        if (child->is_visible_())
        {
            if (gx_rectangle_intersect_(child->clip, dirty_area, &overlap))
            {
                child->draw_in_area_(overlap, canvas, GX_FALSE);
            }
        }
    }
}

void GX_WIDGET::draw_in_area_(const GX_RECTANGLE& dirty_area, GX_CANVAS* canvas, GX_BOOL flush)
{
    GX_DRAWING drawing(canvas, this, dirty_area, flush);
    if (drawing.result_() == GX_SUCCESS)
    {
        draw_function_run_();
    }
}

void GX_WIDGET::draw_()
{
    border_and_background_draw_();
    children_draw_();
}

bool GX_WIDGET::signal_parent_(USHORT event_type, LONG value) const
{
    if (parent && id)
    {
        GX_EVENT e;
        e.type = (ULONG)GX_SIGNAL(id, event_type);
        e.target = parent;
        e.sender = id;
        e.payload.longdata = value;
        return _gx_system_event_send(&e) == GX_SUCCESS;
    }

    return false;
}

void GX_WIDGET::children_event_process_run_(GX_EVENT* event_ptr)
{
    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        child->event_process_function_run_(event_ptr);
    }
}

void GX_WIDGET::children_show_event_process_run_(GX_EVENT* event_ptr)
{
    GX_WIDGET* first_non_client = GX_NULL;
    GX_WIDGET* last_non_client = GX_NULL;

    // all CLIENT children process event

    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        if (child->is_client_())
        {
            child->event_process_function_run_(event_ptr);
        }
        else
        {
            if (!first_non_client)
            {
                first_non_client = child;
            }

            last_non_client = child;
        }
    }

    if (first_non_client)
    {
        // all NON-CLIENT children process event

        for (GX_WIDGET* child = first_non_client; child != last_non_client; child = child->next)
        {
            if (!child->is_client_())
            {
                child->event_process_function_run_(event_ptr);
            }
        }

        last_non_client->event_process_function_run_(event_ptr);
    }
}

void GX_WIDGET::others_input_release_() const
{
    GX_EVENT input_release_event;
    memset(&input_release_event, 0, sizeof(GX_EVENT));
    input_release_event.type = GX_EVENT_INPUT_RELEASE;

    GX_WIDGET*  widget;
    GX_WIDGET** stackptr = _gx_system_input_capture_stack;
    GX_WIDGET** stacktop = _gx_system_input_capture_stack + _gx_system_input_capture_count;
    while (stackptr < stacktop)
    {
        widget = *stackptr;

        if (widget && widget != this)
        {
            input_release_event.target = widget;
            _gx_system_event_send(&input_release_event);
        }

        ++stackptr;
    }
}

UINT GX_WIDGET::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        if (!(this->status & (GX_STATUS_VISIBLE | GX_STATUS_HIDDEN)))
        {
            status_add_(GX_STATUS_VISIBLE);

            children_show_event_process_run_(event_ptr);
        }
        break;

    case GX_EVENT_HIDE:
        if (is_visible_())
        {
            status_remove_(GX_STATUS_VISIBLE);

            if (this->status & GX_STATUS_OWNS_INPUT)
            {
                GX_EVENT e;
                e.type = GX_EVENT_INPUT_RELEASE;
                e.target = this;
                event_process_function_run_(&e);
            }

            children_event_process_run_(event_ptr);
        }
        break;

    case GX_EVENT_INPUT_RELEASE:
        if (this->status & GX_STATUS_OWNS_INPUT)
        {
            _gx_system_input_release(this);
        }
        break;

    case GX_EVENT_FOCUS_GAINED:
        if (!(this->status & GX_STATUS_HAS_FOCUS) &&
            (this->status & GX_STATUS_ACCEPTS_FOCUS))
        {
            status_add_(GX_STATUS_HAS_FOCUS);

            // make sure my parent has focus as well
            if (parent)
            {
                if (!(parent->status & GX_STATUS_HAS_FOCUS))
                {
                    parent->event_process_function_run_(event_ptr);
                }

                if (this->id && (this->status & GX_STATUS_NOTIFY_ON_GAIN_FOCUS))
                {
                    signal_parent_(GX_EVENT_FOCUS_GAIN_NOTIFY, 0);
                }
            }

            if (style & GX_STYLE_ENABLED)
            {
                style |= GX_STYLE_DRAW_SELECTED;
                dirty_mark_();
            }
        }
        break;

    case GX_EVENT_FOCUS_LOST:
        if (this->status & GX_STATUS_HAS_FOCUS)
        {
            status_remove_(GX_STATUS_HAS_FOCUS);

            if (style & GX_STYLE_ENABLED)
            {
                style &= ~GX_STYLE_DRAW_SELECTED;
                dirty_mark_();
            }
        }

        // make sure my children don't think they have focus
        for (GX_WIDGET* child = first_child; child; child = child->next)
        {
            if (child->status & GX_STATUS_HAS_FOCUS)
            {
                child->event_process_function_run_(event_ptr);
                break;
            }
        }
        break;

    case GX_EVENT_FOCUS_NEXT:
        focus_next_();
        break;

    case GX_EVENT_FOCUS_PREVIOUS:
        focus_previous_();
        break;

    case GX_EVENT_LANGUAGE_CHANGE:
    case GX_EVENT_RESOURCE_CHANGE:
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    case GX_EVENT_DYNAMIC_BIDI_TEXT_DISABLE:
    case GX_EVENT_DYNAMIC_BIDI_TEXT_ENABLE:
#endif
        children_event_process_run_(event_ptr);
        break;

    case GX_EVENT_STYLE_CHANGED:
    case GX_EVENT_CLIENT_UPDATED:
    case GX_EVENT_PARENT_SIZED:
    case GX_EVENT_RESIZED:
    case GX_EVENT_DELETE:
        break;

    case GX_EVENT_KEY_DOWN:
        {
            GX_EVENT e;
            memset(&e, 0, sizeof(GX_EVENT));

            USHORT key_value = event_ptr->payload.ushortdata[0];
            switch (key_value)
            {
            case GX_KEY_SELECT:
                if (style & GX_STYLE_ENABLED)
                {
                    e.type = GX_EVENT_SELECT;
                    event_process_function_run_(&e);
                }
                break;

            case GX_KEY_NEXT:
                e.type = GX_EVENT_FOCUS_NEXT;
                e.sender = this->id;
                event_process_function_run_(&e);
                break;

            case GX_KEY_PREVIOUS:
                e.type = GX_EVENT_FOCUS_PREVIOUS;
                e.sender = this->id;
                event_process_function_run_(&e);
                break;

            default:
                parent_event_process_run_(event_ptr);
                break;
            }
        }
        break;

    case GX_EVENT_KEY_UP:
        if (style & GX_STYLE_ENABLED)
        {
            USHORT key_value = event_ptr->payload.ushortdata[0];
            if (key_value == GX_KEY_SELECT)
            {
                GX_EVENT e;
                memset(&e, 0, sizeof(GX_EVENT));
                e.type = GX_EVENT_DESELECT;
                event_process_function_run_(&e);
            }
            else
            {
                parent_event_process_run_(event_ptr);
            }
        }
        break;

    case GX_EVENT_PEN_DOWN:
        if (this->status & GX_STATUS_SELECTABLE)
        {
            if (this->id)
            {
                signal_parent_(GX_EVENT_CLICKED, 0);
            }
        }

        result = parent_event_process_run_(event_ptr);
        break;

    case GX_EVENT_PEN_UP:
    case GX_EVENT_PEN_DRAG:
    default:
        result = parent_event_process_run_(event_ptr);
        break;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_allocate                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function allocates memory for a widget control block           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Return pointer for widget     */
/*                                            control block.              */
/*    memsize                               Size of control block.        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           allocate memory               */
/*    memset                                                              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_allocate(GX_WIDGET** widget, ULONG memsize)
{
    void* mem = _gx_system_memory_allocator(memsize);
    if (!mem)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    *widget = (GX_WIDGET*)mem;
    memset(mem, 0, memsize);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_attach                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function attached a widget to it's parent, putting the child   */
/*    in front of all siblings.                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Parent widget control block   */
/*    child                                 child widget control block    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    GX_ENTER_CRITICAL                     Obtain GUIX system lock       */
/*    _gx_widget_detach                     Detach from current parent    */
/*    _gx_widget_link                       Link widget                   */
/*    GX_EXIT_CRITICAL                      Release GUIX system lock      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_attach(GX_WIDGET* parent, GX_WIDGET* child)
{
    child->attach_to_(parent);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_back_attach                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function attaches a widget to the parent, in the back of the   */
/*    Z order.                                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Parent widget control block   */
/*    child                                 child widget control block    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    GX_ENTER_CRITICAL                     Obtain GUIX system lock       */
/*    _gx_widget_detach                     Detach widget                 */
/*    _gx_widget_back_link                  Link widget in back           */
/*    GX_EXIT_CRITICAL                      Release GUIX system lock      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_back_attach(GX_WIDGET* parent, GX_WIDGET* child)
{
    child->back_attach_to_(parent);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_back_move                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function moves a widget to the back in the Z order.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    widget_moved                          Return flag indicate widget   */
/*                                            moved                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_partial_add          Add dirty area                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_back_move(GX_WIDGET* widget, GX_BOOL* widget_moved)
{
    UINT result = widget->move_to_back_();

    if (widget_moved)
    {
        *widget_moved = (result == GX_SUCCESS ? GX_TRUE : GX_FALSE);
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_block_move                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service move a rectangular block of pixels.                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    block                                 Rectangle to move             */
/*    x_shift                               Number of pixels to shift on  */
/*                                            the x-axis                  */
/*    y_shift                               Number of pixels to shift on  */
/*                                            the y-axis                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_partial_add          Add area to the dirty list    */
/*    _gx_widget_canvas_get                 Find canvas associated with   */
/*                                            the specified widget        */
/*    _gx_canvas_drawing_initiate           Start canvas draw             */
/*    _gx_canvas_block_move                 Canvas-level block move call  */
/*    _gx_canvas_drawing_complete           Finish canvas draw            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_block_move(GX_WIDGET* widget, GX_RECTANGLE* block, INT x_shift, INT y_shift)
{
    return widget->block_move_(block, x_shift, y_shift);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_background_draw                          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified widget.                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_border_draw                Draw border                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_widget_background_draw(GX_WIDGET* widget)
{
    return widget->border_and_background_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_border_draw                              PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified widget.                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    border_color                          Color of border               */
/*    upper_color                           Color of upper fill           */
/*    lower_color                           Color of lower fill           */
/*    fill                                  flag to request fill          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_border_width_get           Get border width              */
/*    _gx_context_raw_line_color_set        Set line color in context     */
/*    _gx_context_brush_define              Define the brush of the       */
/*                                            context                     */
/*    _gx_context_brush_width_set           Set brush width in context    */
/*    _gx_canvas_line_draw                  Draw a line                   */
/*    _gx_widget_client_get                 Get widget client             */
/*    _gx_canvas_rectangle_draw             Draw rectangle                */

/*    _gx_context_fill_color_set            Set the fill color            */
/*    _gx_canvas_pixel_draw                 Draw a pixel                  */
/*    _gx_context_line_color_set            Set line color                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_widget_border_draw(GX_WIDGET* widget,
                            GX_RESOURCE_ID border_color,
                            GX_RESOURCE_ID upper_color,
                            GX_RESOURCE_ID lower_color,
                            GX_BOOL fill)
{
    widget->border_and_background_draw_(border_color, upper_color, lower_color, fill);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_border_style_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the border style flags of the widget.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    style                                 Border style for widget       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_border_style_set(GX_WIDGET* widget, ULONG style)
{
    widget->border_style_set_(style);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_border_width_get                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the widget border width.                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    return_width                          Pointer to destination for    */
/*                                          widget border width           */
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
/*    GUIX draw functions                                                 */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_border_width_get(GX_WIDGET* widget, GX_VALUE* return_width)
{
    *return_width = widget->border_width_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_canvas_get                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the widget canvas.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    return_canvas                         Pointer to destination for    */
/*                                          widget's canvas               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_root_find                  Find the root window          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_canvas_get(GX_WIDGET* widget, GX_CANVAS** return_canvas)
{
    *return_canvas = widget->canvas_get_();

    return *return_canvas ? GX_SUCCESS : GX_FAILURE;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_child_detect                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service detects if the widget is a child of the parent widget. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Pointer to parent widget      */
/*    child                                 Pointer to child widget       */
/*    return_detect                         Pointer to destination for    */
/*                                          detection                     */
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
/*    GUIX application code                                               */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_child_detect(GX_WIDGET* parent, GX_WIDGET* child, GX_BOOL* return_detect)
{
    *return_detect = parent->is_ancestor_of_(child) ? GX_TRUE : GX_FALSE;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_children_draw                            PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws all children of widget.                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_drawing_complete           Complete drawing on canvas    */
/*    _gx_canvas_drawing_initiate           Initiate drawing on canvas    */
/*    _gx_utility_rectangle_overlap_detect  Check for overlap             */
/*    [gx_widget_draw_function]             Child widget drawing function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_widget_children_draw(GX_WIDGET* widget)
{
    widget->children_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_client_get                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves client area of a widget                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    border_width                          width of widget border        */
/*    return_size                           Return rectangle pointer      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_border_width_get           Get widget border width       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX draw functions                                                 */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_client_get(GX_WIDGET* widget, GX_VALUE border_width, GX_RECTANGLE* return_size)
{
    widget->client_area_get_(return_size, border_width);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_color_get                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the color associated with the supplied            */
/*      resource ID from the system color table.                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                caller identify               */
/*    color_id                              Resource ID of color          */
/*    return_color                          Pointer to destination for    */
/*                                            color                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_canvas_get                 Get canvas pointer            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default draw funtions                                          */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_color_get(GX_WIDGET* widget, GX_RESOURCE_ID color_id, GX_COLOR* color)
{
    return widget->color_get_(color_id, color);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_create                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a widget.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    name                                  Name of widget                */
/*    parent                                Parent widget control block   */
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
/*    GX_ENTER_CRITICAL                     Lock access to GUIX           */
/*    _gx_widget_link                       Link a widget to parent       */
/*    GX_EXIT_CRITICAL                      Release the protection        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_create(GX_WIDGET* widget, const GX_CHAR* name, GX_WIDGET* parent,
                       ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    return widget->create_(name, parent, style, Id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_created_test                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function tests the status of a widget for a particular flag    */
/*    and returns TRUE or FALSE.                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    return_test                           Pointer to the return value   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    GX_TRUE or GX_FALSE                                                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX application code                                               */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_created_test(GX_WIDGET* widget, GX_BOOL* return_test)
{
    *return_test = widget->is_created_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_delete                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function delete a widget tree.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    GX_ENTER_CRITICAL                     Lock access to GUIX           */
/*    _gx_widget_delete_helper              Safely delete widget instance */
/*    _gx_widget_free                       Free memory owned by widget   */
/*    GX_EXIT_CRITICAL                      Release the protection        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_delete(GX_WIDGET* widget)
{
    widget->delete_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_detach                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function detaches the widget from its parent.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    GX_ENTER_CRITICAL                      Lock access to GUIX          */
/*    _gx_widget_hide                        Hide the widget              */
/*    _gx_widget_unlink                      Unlink the widget            */
/*    GX_EXIT_CRITICAL                       Release the protection       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_detach(GX_WIDGET* child)
{
    child->detach_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_draw                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified widget.                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_background_draw            Draw the widget background    */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_widget_draw(GX_WIDGET* widget)
{
    widget->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_draw_set                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function overrides the default drawing function of the widget. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                              Widget control block            */
/*    draw_func                           pointer to drawing function     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                              Completion status               */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    application                                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_draw_set(GX_WIDGET* widget, void (*draw_func)(GX_WIDGET*))
{
    widget->draw_function = draw_func;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_event_generate                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a new GX_EVENT and send the event to the      */
/*    widget parent.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    event_type                            Type of event                 */
/*    value                                 Additional event information  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_widget_event_process_function]    The parent event process      */
/*                                            routine                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_event_generate(GX_WIDGET* widget, USHORT event_type, LONG value)
{
    return widget->signal_parent_(event_type, value);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_event_process                                            */
/*                                                           6.4.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes events for the specified widget.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_children_event_process     Forward event to children     */
/*    [gx_widget_event_process_function]    Widget-specific event process */
/*                                            routine                     */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*    _gx_widget_event_to_parent            Signal the parent             */
/*    _gx_widget_event_generate             Generate an event             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_event_process(GX_WIDGET* widget, GX_EVENT* event_ptr)
{
    return widget->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_event_process_set                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function updates the pointer to the event processing           */
/*    function for the specified widget.                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    event_processing_function             pointer to event function     */
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
/*    GUIX Application                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_event_process_set(GX_WIDGET* widget, UINT(*event_processing_function)(GX_WIDGET*, GX_EVENT*))
{
    widget->event_process_function = event_processing_function;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_event_to_parent                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sends event to the widget's parent.                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_widget_event_process_function]    Forward event to parent       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_event_to_parent(GX_WIDGET* widget, GX_EVENT* event_ptr)
{
    return widget->parent_event_process_run_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_fill_color_set                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the background color of the widget.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    normal_color                          Normal fill color id          */
/*    selected_color                        Selected fill color id        */
/*    disabled_color                        Disabled fill color id        */
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
UINT _gx_widget_fill_color_set(GX_WIDGET* widget,
                               GX_RESOURCE_ID normal_color,
                               GX_RESOURCE_ID selected_color,
                               GX_RESOURCE_ID disabled_color)
{
    widget->fill_color_set_(normal_color, selected_color, disabled_color);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_find                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service searches for the specified widget.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Pointer to parent widget to   */
/*                                          start search from             */
/*    widget_id                             Widget ID                     */
/*    search_depth                          how many generations to search*/
/*    return_widget                         Pointer to destination for    */
/*                                            found widget                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_find                       Find the specified widget     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_find(GX_WIDGET* parent, USHORT widget_id, INT search_depth, GX_WIDGET** return_widget)
{
    *return_widget = parent->child_find_by_id_(widget_id, search_depth);

    return *return_widget ? GX_SUCCESS : GX_NOT_FOUND;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_first_child_get                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns the first child of the parent.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Pointer to parent widget      */
/*    child_return                          Pointer to destination for    */
/*                                            found widget                */
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
/*    application                                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_first_child_get(GX_WIDGET* parent, GX_WIDGET** child_return)
{
    *child_return = parent->first_child;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_font_get                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the font associated with the specified            */
/*    resource ID.                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block pointer  */
/*    resource_id                           Resource ID of font           */
/*    return_font                           Pointer to destination for    */
/*                                            font pointer                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_canvas_get                 Get widget canvas pointer     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default drawing functions                                      */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_font_get(GX_WIDGET* widget, GX_RESOURCE_ID font_id, GX_FONT** return_font)
{
    return widget->font_get_(font_id, return_font);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_focus_next                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function moves input focus to the next widget in the           */
/*    navigation order.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_focus_claim                                              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_focus_next(GX_WIDGET* widget)
{
    return widget->focus_next_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_focus_previous                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Move focus to previous widget in nav order.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_focus_claim                                              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_focus_previous(GX_WIDGET* widget)
{
    return widget->focus_previous_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_free                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function release the memory associated with a wiget control    */
/*      block.                                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*     _gx_system_memory_free                                             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_free(GX_WIDGET* widget)
{
    _gx_system_memory_free(widget);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_front_move                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function moves the widget to the front.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget to move     */
/*    return_moved                          Pointer to destination for    */
/*                                            indication widget was moved */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_partial_add          Add dirty area                */
/*    _gx_utility_rectangle_combine         Combine rectangles            */
/*    _gx_utility_rectangle_define          Define rectangle              */
/*    _gx_utility_rectangle_overlap_detect  Check for overlap             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_front_move(GX_WIDGET* widget, GX_BOOL* return_moved)
{
    UINT result = widget->move_to_front_();

    if (return_moved)
    {
        *return_moved = (result ? GX_TRUE : GX_FALSE);
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_height_get                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the widget height.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    return_height                         Pointer to destination for    */
/*                                          widget height                 */
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
UINT _gx_widget_height_get(GX_WIDGET* widget, GX_VALUE* return_height)
{
    *return_height = widget->height_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_hide                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function hides the widget.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_widget_event_process_function]    Call widget event processing  */
/*    _gx_system_dirty_partial_add          Mark the parent dirty         */
/*    _gx_window_view_update_detect         Update viewports if needed    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_hide(GX_WIDGET* widget)
{
    widget->hide_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_last_child_get                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns the last child of the parent.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Pointer to parent widget      */
/*    child_return                          Pointer to destination for    */
/*                                            found widget                */
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
/*    application                                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_last_child_get(GX_WIDGET* parent, GX_WIDGET** child_return)
{
    *child_return = parent->last_child;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_next_sibling_get                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns the next sibling of the current widget.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    current                               Pointer to current widget     */
/*    sibling_return                        Pointer to destination for    */
/*                                            found widget                */
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
/*    application                                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_next_sibling_get(GX_WIDGET* current, GX_WIDGET** sibling_return)
{
    *sibling_return = current->next;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_parent_get                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns the parent of the current widget.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    current                               Pointer to current widget     */
/*    parent_return                         Pointer to destination for    */
/*                                            found widget                */
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
/*    application                                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_parent_get(GX_WIDGET* current, GX_WIDGET** parent_return)
{
    *parent_return = current->parent;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_pixelmap_get                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the pixelmap associated with the supplied         */
/*      resource ID.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Called widget control block   */
/*    pixelmap_id                           Pixelmap resource ID          */
/*    return_pixelmap                       Pointer to pixelmap           */
/*                                            destination pointer         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_pixelmap_get               Actual widget pixelmap get    */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    _gx_drop_list_pixelmap_set            Assign pixelmap to drop list  */
/*    _gx_icon_pixelmap_update              Update pixelmap in an icon    */
/*    _gx_pixelmap_slider_pixelmap_update   Update pixelmap in slider     */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_pixelmap_get(GX_WIDGET* widget, GX_RESOURCE_ID pixelmap_id, GX_PIXELMAP** return_map)
{
    return widget->pixelmap_get_(pixelmap_id, return_map);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_previous_sibling_get                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns the previous sibling of the current widget.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    current                               Pointer to current widget     */
/*    sibling_return                        Pointer to destination for    */
/*                                            found widget                */
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
/*    application                                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_previous_sibling_get(GX_WIDGET* current, GX_WIDGET** sibling_return)
{
    *sibling_return = current->previous;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_resize                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function  resizes the widget.                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    new_size                              New widget size               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_compare          Compares two rectangles      */
/*    _gx_utility_rectangle_inside_detect    Detect if a second rectangle */
/*                                             is completely within the   */
/*                                             first one                  */
/*    _gx_system_dirty_partial_add           Add dirty area               */
/*    _gx_utility_rectangle_combine          Combines two rectangles      */
/*    _gx_widget_client_get                  Retrieves client area        */
/*    _gx_widget_shift                       Changes the position of      */
/*                                             a widget                   */
/*    _gx_widget_clipping_update             Update the clipping members  */
/*                                             of each child widget       */
/*    _gx_widget_event_process_function      Indirect call to widget      */
/*                                             event processing function  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_resize(GX_WIDGET* widget, GX_RECTANGLE* new_size)
{
    widget->resize_(new_size);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_shift                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service shifts the widget and optionally marks it as dirty.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    x_shift                               Number of pixels to shift on  */
/*                                            the x-axis                  */
/*    y_shift                               Number of pixels to shift on  */
/*                                            the y-axis                  */
/*    mark_dirty                            GX_TRUE to indicate dirty,    */
/*                                            otherwise GX_FALSE          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_shift_helper               Shift a widget                */
/*    _gx_widget_clipping_update            Update widget clipping area   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_shift(GX_WIDGET* widget, GX_VALUE x_shift, GX_VALUE y_shift, GX_BOOL mark_dirty)
{
    widget->shift_(x_shift, y_shift, mark_dirty);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_show                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function shows the widget.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_widget_event_process_function]    Call widget event processing  */
/*    _gx_widget_clipping_update            Update the clipping area      */
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*    _gx_window_view_update_detect         Detect window view area for   */
/*                                            update                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_show(GX_WIDGET* widget)
{
    widget->show_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_status_add                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service adds status flag(s) to the widget.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    status                                Status to add                 */
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
UINT _gx_widget_status_add(GX_WIDGET* widget, ULONG status)
{
    widget->status_add_(status);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_status_get                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service retrieves status from the widget.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    return_status                         Pointer to destination for    */
/*                                            status of widget            */
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
UINT _gx_widget_status_get(GX_WIDGET* widget, ULONG* return_status)
{
    *return_status = widget->status;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_status_remove                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service removes status flag(s) from the widget.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    status                                Status flag(s) to remove      */
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
UINT _gx_widget_status_remove(GX_WIDGET* widget, ULONG status)
{
    widget->status_remove_(status);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_status_test                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service tests status of the widget.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    status                                Status to test                */
/*    return_test                           Pointer to destination for    */
/*                                          result of test                */
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
/*    GUIX application code                                               */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_status_test(GX_WIDGET* widget, ULONG status, GX_BOOL* return_test)
{
    *return_test = (widget->status & status) ? GX_TRUE : GX_FALSE;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_string_get_ext                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the string associated with the supplied           */
/*      resource ID.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Called widget control block   */
/*    string_id                             Pixelmap resource ID          */
/*    return_string                         Pointer to string             */
/*                                            destination pointer         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_string_get                Retrieve string from display  */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_string_get_ext(GX_WIDGET* widget, GX_RESOURCE_ID string_id, GX_STRING* return_string)
{
    return widget->string_get_ext_(string_id, return_string);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_style_add                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service adds a style to the widget.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    style                                 New style to add              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark     Mark widget dirty if visible              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_style_add(GX_WIDGET* widget, ULONG style)
{
    widget->style_add_(style);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_style_get                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service retrieves style flags from the widget.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    return_style                          Pointer to destination for    */
/*                                            style                       */
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
UINT _gx_widget_style_get(GX_WIDGET* widget, ULONG* return_style)
{
    *return_style = widget->style;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_style_remove                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service removes a style from the widget.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                               Pointer to widget              */
/*    style                                Style to remove                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                               Completion status              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark     Mark widget dirty if visible              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_style_remove(GX_WIDGET* widget, ULONG style)
{
    widget->style_remove_(style);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_style_set                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function updates the style of the specified widget.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    style                                 New style                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_view_update_detect         Detect if views changed       */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_style_set(GX_WIDGET* widget, ULONG style)
{
    widget->style_set_(style);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_text_blend_ext                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function blends the specified text using current brush and     */
/*    text alignment.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    color_id                              Text Color                    */
/*    font_id                               Font Id                       */
/*    string                                Drawing string                */
/*    x_offset                              Drawing position adjustment   */
/*    y_offset                              Drawing position adjustment   */
/*    alpha                                 Blending value 0-255          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_line_color_set            Set the line color            */
/*    _gx_context_font_set                  Set the font in the context   */
/*    _gx_context_brush_get                 Get the context brush         */
/*    _gx_widget_height_get                 Get widget height             */
/*    _gx_widget_width_get                  Get widget width              */
/*    _gx_widget_border_width_get           Get widget border width       */
/*    _gx_system_string_width_get           Get string width              */
/*    _gx_canvas_text_draw                  Draw text on the canvas       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_text_blend_ext(GX_WIDGET* widget, UINT color_id, UINT font_id,
                               const GX_STRING* string, INT x_offset, INT y_offset, UCHAR alpha)
{
    widget->text_blend_ext_(color_id, font_id, string, x_offset, y_offset, alpha);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_text_draw_ext                            PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified text using current brush and      */
/*    text alignment.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    tColor                                Text Color                    */
/*    font_id                               Font Id                       */
/*    string                                Drawing string                */
/*    x_offset                              Drawing position adjustment   */
/*    y_offset                              Drawing position adjustment   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_line_color_set            Set the line color            */
/*    _gx_context_font_set                  Set the font in the context   */
/*    _gx_context_brush_get                 Get the context brush         */
/*    _gx_widget_height_get                 Get widget height             */
/*    _gx_widget_width_get                  Get widget width              */
/*    _gx_system_string_width_get           Get string width              */
/*    _gx_canvas_text_draw                  Draw text on the canvas       */
/*    _gx_utility_string_length_check       Test string length            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_widget_text_draw_ext(GX_WIDGET* widget, UINT tColor, UINT font_id,
                              const GX_STRING* string, INT x_offset, INT y_offset)
{
    widget->text_draw_ext_(tColor, font_id, string, x_offset, y_offset);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_text_id_draw                             PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws text over a widget given a text id.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    tColor                                Text Color                    */
/*    fColor                                Text fill color               */
/*    font_id                               Font Id                       */
/*    text_id                               Text Id                       */
/*    x_offset                              Drawing position adjustment   */
/*    y_offset                              Drawing position adjustment   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_string_get                Retrieve the text string      */
/*    _gx_widget_text_draw                  Draw text on canvas           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_widget_text_id_draw(GX_WIDGET* widget, UINT tColor, UINT font_id,
                             UINT text_id, INT x_offset, INT y_offset)
{
    widget->text_id_draw_(tColor, font_id, text_id, x_offset, y_offset);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_top_visible_child_get                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns the topmost visible child of the parent.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Pointer to parent widget      */
/*    child_return                          Pointer to destination for    */
/*                                            found widget                */
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
/*    application                                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_widget_top_visible_child_find(GX_WIDGET* parent, GX_WIDGET** child_return)
{
    *child_return = parent->top_visible_child_find_();

    return *child_return ? GX_SUCCESS : GX_NOT_FOUND;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_type_find                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service searches for a widget of the requested type.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Pointer to parent widget to   */
/*                                          start search from             */
/*    widget_type                           Widget type                   */
/*    return_widget                         Pointer to destination for    */
/*                                            found widget                */
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
UINT _gx_widget_type_find(GX_WIDGET* parent, USHORT widget_type, GX_WIDGET** return_widget)
{
    *return_widget = parent->child_find_by_type_(widget_type);

    return *return_widget ? GX_SUCCESS : GX_NOT_FOUND;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_widget_width_get                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the width of the widget.                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    return_width                          Pointer to destination for    */
/*                                            widget width                */
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
UINT _gx_widget_width_get(GX_WIDGET* widget, GX_VALUE* return_width)
{
    *return_width = widget->width_();

    return GX_SUCCESS;
}