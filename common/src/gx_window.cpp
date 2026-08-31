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
/**   Window Management (Window)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_window.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_canvas.h"
#include "gx_pixelmap.h"
#include "gx_scroll_bar.h"
#include "gx_window_root.h"

static void gx_window_draw_(GX_WINDOW* window)
{
    window->draw_();
}

static UINT gx_window_event_process_(GX_WINDOW* window, GX_EVENT* event_ptr)
{
    return window->event_process_(event_ptr);
}

static void gx_window_scroll_info_get_(GX_WINDOW* window, ULONG style, GX_SCROLL_INFO* return_scroll_info)
{
    window->scroll_info_get_(style, return_scroll_info);
}

UINT GX_WINDOW::create_(const GX_CHAR* name, GX_WIDGET* parent,
                        ULONG style, USHORT window_id, const GX_RECTANGLE* size)
{
    GX_WIDGET::create_(name, GX_NULL, style, window_id, size);

    this->type = GX_TYPE_WINDOW;

    this->status |= GX_STATUS_ACCEPTS_FOCUS;

    client_area_get_(&this->client);

    this->views = GX_NULL;
    this->normal_fill_color = GX_COLOR_ID_WINDOW_FILL;
    this->selected_fill_color = GX_COLOR_ID_WINDOW_FILL;
    this->wallpaper = 0;
    this->move_mode = GX_FALSE;
    this->move_start.x = 0;
    this->move_start.y = 0;

    this->draw_function = (void (*)(GX_WIDGET*))gx_window_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_window_event_process_;
    this->scroll_info_get = (void (*)(GX_WINDOW*, ULONG, GX_SCROLL_INFO*))gx_window_scroll_info_get_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

UINT GX_WINDOW::close_()
{
    GX_EVENT e;
    e.type = GX_EVENT_CLOSE;
    e.target = this;
    e.sender = GX_NULL;
    _gx_system_event_send(&e);

    return GX_SUCCESS;
}

void GX_WINDOW::scroll_(GX_VALUE x_scroll, GX_VALUE y_scroll)
{
    if (!is_visible_())
    {
        return;
    }

    bool has_transparent_nonclient_child = false;
    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        if (!child->is_client_() && (child->style & GX_STYLE_TRANSPARENT))
        {
            has_transparent_nonclient_child = true;
            break;
        }
    }

    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        if (child->is_client_())
        {
            child->scroll_shift_(x_scroll, y_scroll, GX_TRUE);
        }
    }

    if (has_transparent_nonclient_child)
    {
        dirty_mark_(&client);
    }
    else
    {
        // we can do faster scrolling using block-move function
        block_move_(&client, x_scroll, y_scroll);
    }
}

void GX_WINDOW::client_scroll_(GX_VALUE x_scroll, GX_VALUE y_scroll)
{
    // This function is designed to be called by the application, i.e. not driven
    // by the user operating a scrollbar. However if the window does have scrollbars,
    // we will satisfy the scrolling request by assigning the scroll bar value.

    if (x_scroll)
    {
        GX_SCROLLBAR* scrollbar = scrollbar_find_(GX_TYPE_HORIZONTAL_SCROLL);
        if (scrollbar)
        {
            scrollbar->value_set_(scrollbar->info.value - x_scroll);

            x_scroll = 0;
        }
    }

    if (y_scroll)
    {
        GX_SCROLLBAR* scrollbar = scrollbar_find_(GX_TYPE_VERTICAL_SCROLL);
        if (scrollbar)
        {
            scrollbar->value_set_(scrollbar->info.value - y_scroll);

            y_scroll = 0;
        }
    }

    // If we get to here with non-zero scroll values,
    // the window is being scrolled with no scrollbars.
    // Manually invoke the _gx_window_scroll functions to accomplish
    // the requested scrolling

    if (x_scroll)
    {
        scroll_(x_scroll, 0);
    }

    if (y_scroll)
    {
        scroll_(0, y_scroll);
    }
}

void GX_WINDOW::scroll_info_get_(ULONG style, GX_SCROLL_INFO* return_scroll_info)
{
    if (style & GX_SCROLLBAR_VERTICAL)
    {
        return_scroll_info->minimum = client.top;
        return_scroll_info->maximum = client.bottom;
        return_scroll_info->visible = (GX_VALUE)(return_scroll_info->maximum - return_scroll_info->minimum + 1);
        return_scroll_info->value = client.top;

        for (GX_WIDGET* child = first_child; child; child = child->next)
        {
            if (child->is_client_())
            {
                if (child->top_() < return_scroll_info->minimum)
                {
                    return_scroll_info->minimum = child->top_();
                }

                if (child->bottom_() > return_scroll_info->maximum)
                {
                    return_scroll_info->maximum = child->bottom_();
                }
            }
        }
    }
    else
    {
        return_scroll_info->minimum = client.left;
        return_scroll_info->maximum = client.right;
        return_scroll_info->visible = (GX_VALUE)(return_scroll_info->maximum - return_scroll_info->minimum + 1);
        return_scroll_info->value = client.left;

        for (GX_WIDGET* child = first_child; child; child = child->next)
        {
            if (child->is_client_())
            {
                if (child->left_() < return_scroll_info->minimum)
                {
                    return_scroll_info->minimum = child->left_();
                }

                if (child->right_() > return_scroll_info->maximum)
                {
                    return_scroll_info->maximum = child->right_();
                }
            }
        }
    }

    return_scroll_info->increment = (GX_VALUE)((return_scroll_info->maximum - return_scroll_info->minimum) / 10);
}

GX_SCROLLBAR* GX_WINDOW::scrollbar_find_(USHORT type) const
{
    if ((type == GX_TYPE_HORIZONTAL_SCROLL) || (type == GX_TYPE_VERTICAL_SCROLL))
    {
        for (GX_WIDGET* child = first_child; child; child = child->next)
        {
            if (child->type == type && !child->is_client_())
            {
                return (GX_SCROLLBAR*)child;
            }
        }
    }

    return GX_NULL;
}

void GX_WINDOW::wallpaper_set_(GX_RESOURCE_ID wallpaper_id, GX_BOOL tile)
{
    wallpaper = wallpaper_id;

    if (tile)
    {
        style |= GX_STYLE_TILE_WALLPAPER;
    }
    else
    {
        style &= ~GX_STYLE_TILE_WALLPAPER;
    }

    dirty_mark_if_visible_();
}

void GX_WINDOW::view_add_(const GX_RECTANGLE& added)
{
    if (view_fold_(added))
    {
        return;
    }

    GX_VIEW* new_view = _gx_system_free_view_get();
    if (new_view)
    {
        new_view->rectangle = added;

        new_view->next = views;
        views = new_view;
    }
}

bool GX_WINDOW::view_fold_(const GX_RECTANGLE& added)
{
    for (GX_VIEW* view = views; view; view = view->next)
    {
        GX_RECTANGLE& existing = view->rectangle;

        if (existing.left == added.left && existing.right == added.right) // two views stacked vertically
        {
            if (existing.bottom == (added.top - 1))
            {
                // new view is right below existing view, expand bottom of existing
                existing.bottom = added.bottom;
                return true;
            }

            if (existing.top == (added.bottom + 1))
            {
                // new view is right above existing view, expand top of existing
                existing.top = added.top;
                return true;
            }
        }
        else if (existing.top == added.top && existing.bottom == added.bottom) // two views stacked horizontally
        {
            if (existing.right == (added.left - 1))
            {
                // new view is to the right of existing view, expand right of existing
                existing.right = added.right;
                return true;
            }

            if (existing.left == (added.right + 1))
            {
                // new view is to the left of existing view, expand left of existing
                existing.left = added.left;
                return true;
            }
        }
        else
        {
            // do nothing
        }
    }

    return false;
}

void GX_WINDOW::views_free_()
{
    if (views)
    {
        _gx_system_views_free(views);
        views = GX_NULL;
    }
}

void GX_WINDOW::views_update_detect_()
{
    GX_WIDGET* win = parent ? parent : this;
    if (win->is_root_window_())
    {
        GX_WINDOW_ROOT* root = static_cast<GX_WINDOW_ROOT*>(win);
        root->views_changed = GX_TRUE;
    }
}

void GX_WINDOW::background_draw_()
{
    background_draw_(fill_color_());
}

void GX_WINDOW::background_draw_(GX_COLOR fill_color)
{
    GX_PIXELMAP* pixelmap = GX_NULL;
    GX_RECTANGLE client_rect;

    GX_BOOL fill = GX_TRUE;

    if (wallpaper)
    {
        _gx_context_pixelmap_get(wallpaper, &pixelmap);
        if (pixelmap)
        {
            client_area_get_(&client_rect);

            if (!PIXELMAP_IS_TRANSPARENT(pixelmap))
            {
                if (style & GX_STYLE_TILE_WALLPAPER)
                {
                    fill = GX_FALSE;
                }
                else
                {
                    if (pixelmap->width > client_rect.width_() &&
                        pixelmap->height > client_rect.height_())
                    {
                        fill = GX_FALSE;
                    }
                }
            }
        }
    }

    border_and_background_draw_(GX_COLOR_ID_WINDOW_BORDER, fill_color, fill_color, fill);

    if (pixelmap)
    {
        GX_DRAW_CONTEXT* context = gx_context_current_();

        context->fill_color_set_(fill_color);

        if (style & GX_STYLE_TILE_WALLPAPER)
        {
            _gx_canvas_pixelmap_tile(&client_rect, pixelmap);
        }
        else
        {
            GX_RECTANGLE old_dirty = context->dirty;

            if (gx_rectangle_intersect_(old_dirty, client_rect, &context->dirty))
            {
                _gx_canvas_pixelmap_draw(client_rect.left, client_rect.top, pixelmap);
            }

            context->dirty = old_dirty;
        }
    }
}

void GX_WINDOW::draw_()
{
    background_draw_();
    children_draw_();
}

UINT GX_WINDOW::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_CLOSE:
        detach_();
        return GX_EVENT_CLOSE;

    case GX_EVENT_SHOW:
        result = GX_WIDGET::event_process_(event_ptr);

        nav_order_initialize_();
        views_update_detect_();

        // if this window is in front, claim focus
        if (!is_root_window_() && !(status & GX_STATUS_HAS_FOCUS))
        {
            GX_WIDGET* widget = this;
            while (!widget->next && widget->parent)
            {
                if (widget->parent->is_root_window_())
                {
                    _gx_system_focus_claim(this);
                    break;
                }

                widget = widget->parent;
            }
        }
        break;

    case GX_EVENT_HIDE:
        result = GX_WIDGET::event_process_(event_ptr);
        views_update_detect_();
        break;

    case GX_EVENT_FOCUS_GAINED:
        if (this->status & GX_STATUS_ACCEPTS_FOCUS)
        {
            GX_WIDGET::event_process_(event_ptr);

            if (!_gx_system_focus_owner || _gx_system_focus_owner == this)
            {
                child_focus_assign_();
            }

            if (!is_root_window_())
            {
                if (move_to_front_() == GX_SUCCESS)
                {
                    views_update_detect_();
                }
            }
        }
        break;

    case GX_EVENT_PEN_DOWN:
        if (this->status & GX_STATUS_MOVABLE)
        {
            move_mode = GX_TRUE;
            move_start = event_ptr->payload.pointdata;

            return GX_SUCCESS;
        }
        else
        {
            result = GX_WIDGET::event_process_(event_ptr);
        }
        break;

    case GX_EVENT_PEN_DRAG:
        if (move_mode)
        {
            GX_VALUE x_shift = (GX_VALUE)(event_ptr->payload.pointdata.x - move_start.x);
            GX_VALUE y_shift = (GX_VALUE)(event_ptr->payload.pointdata.y - move_start.y);

            move_start = event_ptr->payload.pointdata;

            shift_(x_shift, y_shift, GX_TRUE);
        }
        else
        {
            result = GX_WIDGET::event_process_(event_ptr);
        }
        break;

    case GX_EVENT_PEN_UP:
        if (move_mode)
        {
            move_mode = GX_FALSE;
        }
        else
        {
            result = GX_WIDGET::event_process_(event_ptr);
        }
        break;

    case GX_EVENT_VERTICAL_SCROLL:
        scroll_(0, (GX_VALUE)(event_ptr->payload.intdata[1] - event_ptr->payload.intdata[0]));
        break;

    case GX_EVENT_HORIZONTAL_SCROLL:
        scroll_((GX_VALUE)(event_ptr->payload.intdata[1] - event_ptr->payload.intdata[0]), 0);
        break;

    default:
        result = GX_WIDGET::event_process_(event_ptr);
        break;
    }

    return result;
}

UINT GX_WINDOW::execute_(ULONG* return_ptr)
{
    UINT result = GX_SUCCESS;
    UINT return_code = 0;

#ifdef GX_THREADX_BINDING
    ULONG event_memory[GX_EVENT_ULONGS];
#else
    GX_EVENT event_memory;
#endif

    GX_EVENT* event_ptr;

    if (this->status & GX_STATUS_MODAL)
    {
        return GX_NO_CHANGE;
    }

    status_add_(GX_STATUS_MODAL);

    while (!return_code)
    {
#ifdef GX_THREADX_BINDING

        result = tx_queue_receive(&_gx_system_event_queue, &event_memory[0], TX_NO_WAIT);
        if (result == TX_QUEUE_EMPTY)
        {
            _gx_system_canvas_refresh();

            result = tx_queue_receive(&_gx_system_event_queue, &event_memory[0], TX_WAIT_FOREVER);
        }

        if (result == TX_SUCCESS)
        {
            result = GX_SUCCESS;
        }

#else // !GX_THREADX_BINDING

        // here for generic RTOS binding
        result = GX_EVENT_POP(&event_memory, GX_FALSE);
        if (result == GX_FAILURE)
        {
            _gx_system_canvas_refresh();

            result = GX_EVENT_POP(&event_memory, GX_TRUE);
        }

#endif // !GX_THREADX_BINDING

        if (result != GX_SUCCESS)
        {
            status_remove_(GX_STATUS_MODAL);

            if (return_ptr)
            {
                *return_ptr = 0;
            }

            _gx_system_error_process(GX_SYSTEM_EVENT_RECEIVE_ERROR);

            return GX_SYSTEM_EVENT_RECEIVE_ERROR;
        }

        event_ptr = (GX_EVENT*)(&event_memory);

        switch (event_ptr->type)
        {
        case GX_EVENT_REDRAW:
            _gx_system_all_canvas_dirty();
            break;

        case GX_EVENT_TIMER:
            if (!event_ptr->target)
            {
                // the event is from gx_system_timer_expiration
                _gx_system_timer_update(event_ptr->payload.ulongdata);
            }
            else
            {
                return_code = _gx_system_timer_event_dispatch(event_ptr);
                if (return_code == GX_NOT_FOUND)
                {
                    return_code = 0;
                }
            }
            break;

        case GX_EVENT_PEN_DOWN:
        case GX_EVENT_PEN_UP:
        case GX_EVENT_PEN_DRAG:
            if (_gx_system_input_capture_count > 0 ||
                size.contain_point_(event_ptr->payload.pointdata))
            {
                return_code = _gx_system_event_dispatch(event_ptr);
            }
            break;

        case GX_EVENT_TERMINATE:
            _gx_system_event_send(event_ptr);
            return_code = GX_EVENT_TERMINATE;
            break;

        case GX_EVENT_HIDE:
            _gx_system_event_dispatch(event_ptr);
            return_code = GX_EVENT_HIDE;
            break;

        case GX_EVENT_CLOSE:
            _gx_system_event_dispatch(event_ptr);
            return_code = GX_EVENT_CLOSE;
            break;

        case 0:
            // event has been purged
            break;

        default:
            // Dispatch the event to GUIX proper window/widget.
            return_code = _gx_system_event_dispatch(event_ptr);
            break;
        }
    }

    status_remove_(GX_STATUS_MODAL);

    if (return_ptr)
    {
        *return_ptr = return_code;
    }

    detach_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_background_draw                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the background of a window widget.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_border_draw                Draw window background with   */
/*                                            specified fill color        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_window_background_draw(GX_WINDOW* window)
{
    window->background_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_client_height_get                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the client height of the window.                  */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window             */
/*    return_height                         Pointer to destination        */
/*                                            for client height           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
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
UINT _gx_window_client_height_get(GX_WINDOW* window, GX_VALUE* return_height)
{
    *return_height = window->client.height_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_client_scroll                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service scrolls the window client area by the specified amount */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window             */
/*    x_scroll                              Amount to scroll on x-axis    */
/*    y_scroll                              Amount to scroll on y-axis    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_scroll                     main scroll function          */
/*    _gx_scrollbar_value_set               reset scrollbar thumb pos     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_window_client_scroll(GX_WINDOW* window, GX_VALUE x_scroll, GX_VALUE y_scroll)
{
    window->client_scroll_(x_scroll, y_scroll);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_client_width_get                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the client width of the specified window.         */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window             */
/*    return_width                          Pointer to destination for    */
/*                                            client width                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
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
UINT _gx_window_client_width_get(GX_WINDOW* window, GX_VALUE* return_width)
{
    *return_width = window->client.width_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_close                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function closes a window. If the window is executing modally,  */
/*    the modal execution is terminated.                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Window's widget control block */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_window_close(GX_WINDOW* window)
{
    return window->close_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_create                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a window, which is a special type of          */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window control     */
/*                                           block                        */
/*    name                                  Logical name of window        */
/*    parent                                Pointer to parent window      */
/*    style                                 Window Style                  */
/*    window_id                             Application-defined ID of the */
/*                                            window                      */
/*    size                                  Size of the window            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_create                     Create the underlying widget  */
/*    _gx_widget_border_width_get           Get the widget border width   */
/*    _gx_widget_client_get                 Get the widget client         */
/*    _gx_widget_link                       Link the widget to the parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_window_create(GX_WINDOW* window, const GX_CHAR* name, GX_WIDGET* parent,
                        ULONG style, USHORT window_id, const GX_RECTANGLE* size)
{
    return window->create_(name, parent, style, window_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_draw                                                     */
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
/*    window                                Window control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_background_draw            Draw window background        */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_window_draw(GX_WINDOW* window)
{
    window->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_event_process                                            */
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
/*    window                                Window's widget control block */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_shift                      Shift a widget                */
/*    _gx_window_view_update_detect         Detect changes to the window  */
/*                                            viewport                    */
/*    _gx_widget_event_process              Call widget event processing  */
/*    _gx_window_cilent_scroll              Scroll window client area     */
/*    _gx_widget_front_move                 Move window to front          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_window_event_process(GX_WINDOW* window, GX_EVENT* event_ptr)
{
    return window->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_execute                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function modally executes a window.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Window's widget control block */
/*    return_ptr                            return value                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_window_execute(GX_WINDOW* window, ULONG* return_ptr)
{
    return window->execute_(return_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_scroll_info_get                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the window scroll information.                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                               Pointer to window              */
/*    style                                GX_SCROLLBAR_HORIZONTAL        */
/*                                           or GX_SCROLLBAR_VERTICAL     */
/*    return_scroll_info                   Pointer to destination for     */
/*                                           scroll info                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                               Completion status              */
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
UINT _gx_window_scroll_info_get(GX_WINDOW* window, ULONG style, GX_SCROLL_INFO* return_scroll_info)
{
    window->scroll_info_get_(style, return_scroll_info);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_scrollbar_find                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service finds the scrollbar for the specified window.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window             */
/*    type                                  GX_SCROLLBAR_HORIZONTAL or    */
/*                                            GX_SCROLLBAR_VERTICAL       */
/*    return_scrollbar                      Pointer to destination for    */
/*                                            scrollbar                   */
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
UINT _gx_window_scrollbar_find(GX_WINDOW* window, USHORT type, GX_SCROLLBAR** return_scrollbar)
{
    *return_scrollbar = window->scrollbar_find_(type);

    return *return_scrollbar ? GX_SUCCESS : GX_NOT_FOUND;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_wallpaper_get                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the wallpaper for the specified window.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window             */
/*    return_wallpaper_id                   Pointer to destination for    */
/*                                            resource ID of wallpaper    */
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
UINT _gx_window_wallpaper_get(GX_WINDOW* window, GX_RESOURCE_ID* return_wallpaper_id)
{
    *return_wallpaper_id = window->wallpaper;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_wallpaper_set                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the wallpaper for the specified window.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window             */
/*    wallpaper_id                          Resource ID of wallpaper      */
/*    tile                                  Wallpaper is tiled if GX_TRUE,*/
/*                                          otherwise wallpaper is not    */
/*                                          tiled                         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark this text as dirty       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_window_wallpaper_set(GX_WINDOW* window, GX_RESOURCE_ID wallpaper_id, GX_BOOL tile)
{
    window->wallpaper_set_(wallpaper_id, tile);

    return GX_SUCCESS;
}
