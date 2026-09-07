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

#include "gx_window_root.h"

#include "gx_system.h"
#include "gx_canvas.h"

static UINT gx_window_root_event_process_(GX_WINDOW_ROOT* root_window, GX_EVENT* event_ptr)
{
    return root_window->event_process_(event_ptr);
}

UINT GX_WINDOW_ROOT::create_(const GX_CHAR* name, GX_CANVAS* canvas,
                             ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    memset(this, 0, sizeof(GX_WINDOW_ROOT));

    GX_WINDOW::create_(name, GX_NULL, style, Id, size);

    this->type = GX_TYPE_ROOT_WINDOW;

    this->canvas = canvas;
    this->views_changed = GX_TRUE;

    this->normal_fill_color = GX_COLOR_ID_CANVAS;
    this->selected_fill_color = GX_COLOR_ID_CANVAS;
    this->disabled_fill_color = GX_COLOR_ID_CANVAS;

    this->status &= ~GX_STATUS_MOVABLE;

    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_window_root_event_process_;

    GX_ENTER_CRITICAL

    ++_gx_system_root_window_created_count;

    if (_gx_system_root_window_created_list)
    {
        next = _gx_system_root_window_created_list;
        _gx_system_root_window_created_list->previous = this;
    }

    _gx_system_root_window_created_list = this;

    GX_EXIT_CRITICAL

    return GX_SUCCESS;
}

UINT GX_WINDOW_ROOT::delete_()
{
    all_views_free_();

    if (previous)
    {
        previous->next = next;
    }
    else
    {
        _gx_system_root_window_created_list = (GX_WINDOW_ROOT*)next;
    }

    if (next)
    {
        next->previous = previous;
    }

    --_gx_system_root_window_created_count;

    GX_WIDGET::delete_();

    memset(this, 0, sizeof(GX_WINDOW_ROOT));

    return GX_SUCCESS;
}

void GX_WINDOW_ROOT::root_view_add_(const GX_RECTANGLE& rect)
{
    GX_RECTANGLE overlap;

    // start from the last (frontmost) child
    for (GX_WIDGET* child = last_child; child; child = child->previous)
    {
        if (child->is_window_() &&
            child->is_visible_() &&
            !(child->status & GX_STATUS_TRANSPARENT))
        {
            GX_WINDOW* child_window = static_cast<GX_WINDOW*>(child);

            if (gx_rectangle_intersect_(child_window->size, rect, &overlap))
            {
                view_split_(child_window->size, rect);

                child_window->view_add_(overlap);

                return;
            }
        }
    }

    view_add_(rect);
}

void GX_WINDOW_ROOT::view_split_(const GX_RECTANGLE& oversize, const GX_RECTANGLE& original)
{
    GX_RECTANGLE split;

    // Is any area on top exposed?
    if (oversize.top > original.top)
    {
        split.left = original.left;
        split.right = original.right;
        split.top = original.top;
        split.bottom = static_cast<GX_VALUE>(oversize.top - 1);

        root_view_add_(split);
    }

    // Is any area on bottom exposed?
    if (oversize.bottom < original.bottom)
    {
        split.left = original.left;
        split.right = original.right;
        split.top = static_cast<GX_VALUE>(oversize.bottom + 1);
        split.bottom = original.bottom;

        root_view_add_(split);
    }

    // Is any area on left exposed?
    if (oversize.left > original.left)
    {
        split.left = original.left;
        split.right = static_cast<GX_VALUE>(oversize.left - 1);
        split.top = GX_MAX(oversize.top, original.top);
        split.bottom = GX_MIN(oversize.bottom, original.bottom);

        root_view_add_(split);
    }

    // Is any area on right exposed?
    if (oversize.right < original.right)
    {
        split.left = static_cast<GX_VALUE>(oversize.right + 1);
        split.right = original.right;
        split.top = GX_MAX(oversize.top, original.top);
        split.bottom = GX_MIN(oversize.bottom, original.bottom);

        root_view_add_(split);
    }
}

#ifndef GX_DISABLE_ERROR_CHECKING

static int _gx_system_free_views_count()
{
    int count = 0;

    for (GX_VIEW* view = _gx_system_free_views; view; view = view->next)
    {
        ++count;
    }

    return count;
}

#endif // !GX_DISABLE_ERROR_CHECKING

void GX_WINDOW_ROOT::all_views_free_()
{
    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        if (child->is_window_())
        {
            GX_WINDOW* child_window = static_cast<GX_WINDOW*>(child);
            child_window->views_free_();
        }
    }

    views_free_();


#ifndef GX_DISABLE_ERROR_CHECKING

    if (_gx_system_root_window_created_count < 2)
    {
        if (_gx_system_free_views_count() != GX_MAX_VIEWS)
        {
            _gx_system_error_process(GX_SYSTEM_OUT_OF_VIEWS);
        }
    }

#endif // !GX_DISABLE_ERROR_CHECKING
}

void GX_WINDOW_ROOT::all_views_update_()
{
    all_views_free_();
    root_view_add_(size);
    views_changed = GX_FALSE;
}

UINT GX_WINDOW_ROOT::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_PEN_DRAG:
        if (move_mode)
        {
            GX_POINT& pos = event_ptr->payload.pointdata;

            // To move a root window we have to move the canvas.
            // Calculate the x,y shift amounts

            GX_VALUE x_shift = static_cast<GX_VALUE>(pos.x - move_start.x);
            GX_VALUE y_shift = static_cast<GX_VALUE>(pos.y - move_start.y);

            // reset the starting point for next time
            // move_start.x = static_cast<GX_VALUE>(pos.x - x_shift);
            // move_start.y = static_cast<GX_VALUE>(pos.y - y_shift);

            canvas->shift_(x_shift, y_shift);
        }
        return GX_SUCCESS;

    default:
        result = GX_WINDOW::event_process_(event_ptr);
        break;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_root_create                                              */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a root window, which is a special type of     */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_window                           Root Window control block     */
/*    name                                  Name of window                */
/*    canvas                                Canvas this root window       */
/*                                            belongs to                  */
/*    style                                 Style of window               */
/*    Id                                    User-specified root window ID */
/*    size                                  Window size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memset                                Set control block to zero     */
/*    _gx_window_create                     Create the underlying window  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_window_root_create(GX_WINDOW_ROOT* root_window, const GX_CHAR* name,
                            GX_CANVAS* canvas, ULONG style, USHORT Id,
                            const GX_RECTANGLE* size)
{
    return root_window->create_(name, canvas, style, Id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_root_delete                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function delets a previous-created root window                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_window                           Window control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memset                                Set control block to zero     */
/*    _gx_system_all_views_free             Free up all view ports        */
/*    _gx_widget_delete                     Delete widget                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_window_root_delete(GX_WINDOW_ROOT* root_window)
{
    return root_window->delete_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_root_event_process                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes events for the specified root window.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Window's widget control block */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_shift                      Shift the canvas              */
/*    _gx_window_event_process              Call widget event processing  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_window_root_event_process(GX_WINDOW_ROOT* root_window, GX_EVENT* event_ptr)
{
    return root_window->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_window_root_find                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service finds the root window.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget control     */
/*                                            block                       */
/*    return_root_window                    Pointer to destination for    */
/*                                            found root window           */
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
UINT _gx_window_root_find(GX_WIDGET* widget, GX_WINDOW_ROOT** return_root_window)
{
    *return_root_window = widget->root_window_();
    return *return_root_window ? GX_SUCCESS : GX_FAILURE;
}
