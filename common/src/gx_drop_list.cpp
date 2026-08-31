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
/**   Drop List (List)                                                    */
/**                                                                       */
/**************************************************************************/

#include "gx_drop_list.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_canvas.h"
#include "gx_window_root.h"

static void gx_drop_list_draw_(GX_DROP_LIST* drop_list)
{
    drop_list->draw_();
}

static UINT gx_drop_list_event_process_(GX_DROP_LIST* drop_list, GX_EVENT* event_ptr)
{
    return drop_list->event_process_(event_ptr);
}

UINT GX_DROP_LIST::create_(const GX_CHAR* name,
                           GX_WIDGET* parent, INT total_rows, INT open_height,
                           void (*callback)(GX_VERTICAL_LIST*, GX_WIDGET*, INT),
                           ULONG style, USHORT drop_list_id, const GX_RECTANGLE* size)
{
    GX_WIDGET::create_(name, GX_NULL, style, drop_list_id, size);

    this->type = GX_TYPE_DROP_LIST;

    this->open_height = open_height;
    this->pixelmap = 0;
    this->popup_open = GX_FALSE;

    this->draw_function = (void (*)(GX_WIDGET*))gx_drop_list_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_drop_list_event_process_;

    popup.create_(this, total_rows, callback);

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

UINT GX_DROP_LIST::open_()
{
    if (popup.is_visible_())
    {
        popup_open = GX_TRUE;

        return GX_SUCCESS;
    }

    GX_RECTANGLE popup_size;
    popup_size.left = left_();
    popup_size.right = right_();
    popup_size.top = (GX_VALUE)(bottom_() + 1);
    popup_size.bottom = (GX_VALUE)(popup_size.top + open_height - 1);
    popup.resize_(&popup_size);

    popup.children_position_();
    popup.link_to_(this->parent);

    GX_WIDGET* child = popup.selected_widget_get_();
    if (child)
    {
        child->style |= GX_STYLE_DRAW_SELECTED;
    }

    popup_open = GX_TRUE;
    _gx_system_focus_claim(&popup);

    return GX_SUCCESS;
}

UINT GX_DROP_LIST::close_()
{
    if (popup_open)
    {
        popup_open = GX_FALSE;

        GX_WIDGET* child = popup.first_child;
        while (child)
        {
            child->style &= ~GX_STYLE_DRAW_SELECTED;
            child = child->next;
        }

        popup.unlink_();
    }

    return GX_SUCCESS;
}

UINT GX_DROP_LIST::pixelmap_set_(GX_RESOURCE_ID pixelmap_id)
{
    this->pixelmap = pixelmap_id;

    dirty_mark_if_visible_();

    return GX_SUCCESS;
}

void GX_DROP_LIST::background_draw_()
{
    GX_PIXELMAP* map = GX_NULL;

    if (pixelmap)
    {
        _gx_context_pixelmap_get(pixelmap, &map);
    }

    if (map)
    {
        border_and_background_draw_(GX_COLOR_ID_DEFAULT_BORDER, normal_fill_color, normal_fill_color, GX_TRUE);

        GX_VALUE border_width = border_width_();

        if (style & GX_STYLE_TILE_BACKGROUND)
        {
            GX_RECTANGLE client;
            client_area_get_(&client);

            _gx_canvas_pixelmap_tile(&client, map);
        }
        else
        {
            GX_VALUE x = (GX_VALUE)(clip.left + border_width + 1);
            GX_VALUE y = (GX_VALUE)(clip.top + border_width + 1);
            _gx_canvas_pixelmap_draw(x, y, map);
        }
    }
    else
    {
        border_and_background_draw_();
    }
}

void GX_DROP_LIST::selected_children_draw_(GX_WIDGET* widget, GX_VALUE x_shift, GX_VALUE y_shift)
{
    GX_RECTANGLE saved_size;
    GX_RECTANGLE saved_clip;
    ULONG saved_style;
    GX_WIDGET* saved_child;

    for (GX_WIDGET* child = widget->first_child; child; child = child->next)
    {
        saved_size = child->size;
        saved_clip = child->clip;
        saved_style = child->style;

        child->size.move_(x_shift, y_shift);
        child->clip.move_(x_shift, y_shift);

        if (this->status & GX_STATUS_HAS_FOCUS)
        {
            child->style |= GX_STYLE_DRAW_SELECTED;
        }
        else
        {
            child->style &= ~GX_STYLE_DRAW_SELECTED;
        }

        saved_child = child->first_child;
        child->first_child = GX_NULL;

        child->draw_function_run_();

        if (saved_child)
        {
            child->first_child = saved_child;
            selected_children_draw_(child, x_shift, y_shift);
        }

        child->size = saved_size;
        child->clip = saved_clip;
        child->style = saved_style;
    }
}

void GX_DROP_LIST::draw_()
{
    background_draw_();

    bool reuse_list_item = false;;

    GX_WIDGET* selected_widget = popup.selected_widget_get_();

    if (!selected_widget && popup.callback)
    {
        // If not be able to retrieve the selected widget, its possible that the selcted item
        // has been reused for displaying the other list items during the scrolling.
        // In this case, try to get the selected index first,
        // then reuse the list child to draw the selected item.

        INT selected_index = popup.selected;
        if (popup.is_valid_index_(selected_index))
        {
            selected_widget = popup.first_client_child_get_();
            popup.callback_run_(selected_widget, selected_index);

            reuse_list_item = true;
        }
    }

    if (selected_widget)
    {
        GX_RECTANGLE saved_size = selected_widget->size;
        GX_RECTANGLE saved_clip = selected_widget->clip;
        ULONG saved_style = selected_widget->style;

        GX_WIDGET* saved_child = GX_NULL;

        GX_RECTANGLE client;
        client_area_get_(&client);

        selected_widget->size = client;
        selected_widget->clip = client;

        if (this->status & GX_STATUS_HAS_FOCUS)
        {
            selected_widget->style |= GX_STYLE_DRAW_SELECTED;
        }
        else
        {
            selected_widget->style &= ~GX_STYLE_DRAW_SELECTED;
        }

        GX_VALUE x_shift = 0;
        GX_VALUE y_shift = 0;

        if (selected_widget->first_child)
        {
            saved_child = selected_widget->first_child;
            selected_widget->first_child = GX_NULL;

            // Calculate the distance from the selected widget to the client area.
            x_shift = (GX_VALUE)(client.left - selected_widget->left_());
            y_shift = (GX_VALUE)(((client.top + client.bottom) -
                                  (selected_widget->top_() + selected_widget->bottom_())) / 2);
        }

        selected_widget->draw_function_run_();

        if (saved_child)
        {
            selected_widget->first_child = saved_child;
            selected_children_draw_(selected_widget, x_shift, y_shift);
        }

        selected_widget->size = saved_size;
        selected_widget->clip = saved_clip;
        selected_widget->style = saved_style;
    }

    if (reuse_list_item)
    {
        popup.callback_run_(selected_widget, popup.first_visible_index);
    }

    children_draw_();
}

UINT GX_DROP_LIST::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_HIDE:
        if (popup_open)
        {
            close_();
        }
        result = GX_WIDGET::event_process_(event_ptr);
        break;

    case GX_EVENT_CLOSE_POPUP:
        close_();
        break;

    case GX_SIGNAL(GX_ID_DROP_LIST_BUTTON, GX_EVENT_CLICKED):
        if (popup_open)
        {
            close_();
        }
        else
        {
            open_();
        }
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
/*    _gx_drop_list_close                                 PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service closes a drop list.                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    drop_list                             Drop list control block       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_unlink                     Unlink a widget               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    _gx_drop_list_event_process                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_drop_list_close(GX_DROP_LIST* drop_list)
{
    return drop_list->close_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_drop_list_create                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service creates a drop list.                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    drop_list                             Drop list control block       */
/*    name                                  Name of drop list             */
/*    parent                                Pointer to parent widget      */
/*    total_rows                            Total number of rows in       */
/*                                            drop list                   */
/*    open_height                           Height of the vertical list   */
/*    callback                              function called to create     */
/*                                            new widgets when the list   */
/*                                            is scrolled.                */
/*    style                                 Style of drop list            */
/*    drop_list_id                          Application-defined ID of     */
/*                                            the drop list               */
/*    size                                  Dimensions of the drop list   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_create                     Create the underlying widget  */
/*    memset                                Clear the memory block        */
/*    _gx_vertical_list_create              Create the underlying         */
/*                                            vertical list               */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_drop_list_create(GX_DROP_LIST* drop_list, const GX_CHAR* name,
                          GX_WIDGET* parent, INT total_rows, INT open_height,
                          void (*callback)(GX_VERTICAL_LIST*, GX_WIDGET*, INT),
                          ULONG style, USHORT drop_list_id, const GX_RECTANGLE* size)
{
    return drop_list->create_(name, parent, total_rows, open_height, callback, style, drop_list_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_drop_list_event_process                         PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service processes an event for the vertical list.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    drop_list                             Drop list widget control block*/
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_vertical_list_selected_set        Set the list entry at the     */
/*                                          current list index            */
/*    _gx_drop_list_close                   Close a drop list             */
/*    _gx_drop_list_open                    Open a drop list              */
/*    _gx_first_client_child_get            Get the first client child    */
/*    _gx_widget_event_process              Process events for the        */
/*                                          specified window              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_drop_list_event_process(GX_DROP_LIST* drop_list, GX_EVENT* event_ptr)
{
    return drop_list->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_drop_list_open                                  PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service opens a drop list.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    drop_list                             Drop list control block       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_root_find                  Find the root window          */
/*    _gx_widget_resize                     Resize a widget               */
/*    _gx_vertical_list_children_position   Position the children for     */
/*                                          the vertical list             */
/*    _gx_widget_link                       Link the widget to its root   */
/*    _gx_system_focus_claim                Claim the input focus         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_drop_list_event_process                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_drop_list_open(GX_DROP_LIST* drop_list)
{
    return drop_list->open_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_drop_list_pixelmap_set                          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Assign a pixelmap to the displayed on a drop list.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    drop_list                             Drop list control block       */
/*    id                                    Resource ID to the pixelmap   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_drop_list_pixelmap_set(GX_DROP_LIST* drop_list, GX_RESOURCE_ID pixelmap_id)
{
    return drop_list->pixelmap_set_(pixelmap_id);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_drop_list_popup_get                             PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service retrieves the popup list from the drop list.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    drop_list                             Drop list control block       */
/*    return_list                           Point to the list stored in   */
/*                                           the drop list.               */
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
UINT _gx_drop_list_popup_get(GX_DROP_LIST* drop_list, GX_VERTICAL_LIST** return_list)
{
    *return_list = &drop_list->popup;

    return GX_SUCCESS;
}
