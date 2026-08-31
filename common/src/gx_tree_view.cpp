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
/**   Tree View Management (Tree View)                                    */
/**                                                                       */
/**************************************************************************/

#include "gx_tree_view.h"

#include "gx_context.h"
#include "gx_canvas.h"
#include "gx_pixelmap.h"
#include "gx_menu.h"
#include "gx_scroll_bar.h"

static void gx_tree_view_draw_(GX_TREE_VIEW* tree)
{
    tree->draw_();
}

static UINT gx_tree_view_event_process_(GX_TREE_VIEW* tree, GX_EVENT* event_ptr)
{
    return tree->event_process_(event_ptr);
}

static void gx_tree_view_scroll_info_get_(GX_TREE_VIEW* tree, ULONG style, GX_SCROLL_INFO* return_scroll_info)
{
    tree->scroll_info_get_(style, return_scroll_info);
}

UINT GX_TREE_VIEW::create_(const GX_CHAR* name, GX_WIDGET* parent,
                           ULONG style, USHORT tree_menu_id, const GX_RECTANGLE* size)
{
    GX_WINDOW::create_(name, GX_NULL, style, tree_menu_id, size);

    this->type = GX_TYPE_TREE_VIEW;

    this->collapse_pixelmap_id = 0;
    this->expand_pixelmap_id = 0;
    this->root_line_color = GX_COLOR_ID_SHADOW;
    this->indentation = 22;
    this->x_shift = 0;
    this->y_shift = 0;
    this->tree_width = 0;
    this->tree_height = 0;
    this->selected = GX_NULL;

    this->draw_function = (void (*)(GX_WIDGET*))gx_tree_view_draw_;
    this->event_process_function = (UINT(*)(GX_WIDGET*, GX_EVENT*))gx_tree_view_event_process_;
    this->scroll_info_get = (void (*)(GX_WINDOW*, ULONG, GX_SCROLL_INFO*))gx_tree_view_scroll_info_get_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_TREE_VIEW::selected_set_(GX_WIDGET* selected)
{
    if (this->selected == selected)
    {
        return;
    }

    if (this->selected)
    {
        this->selected->style &= ~GX_STYLE_DRAW_SELECTED;
        this->selected->dirty_mark_();
    }

    selected->style |= GX_STYLE_DRAW_SELECTED;

    this->selected = selected;

    if (this->id)
    {
        signal_parent_(GX_EVENT_TREE_SELECT, selected->id);
    }

    if (!is_visible_())
    {
        selected_visible_();
    }

    dirty_mark_();
}

void GX_TREE_VIEW::selected_visible_()
{
    GX_WIDGET* widget = selected->parent;
    while (widget != this)
    {
        if (widget->type == GX_TYPE_MENU_LIST)
        {
            widget = ((GX_MENU_LIST*)widget)->owner;
            widget->style |= GX_STYLE_MENU_EXPANDED;
        }
        else
        {
            widget = widget->parent;
        }
    }

    position_();
}

void GX_TREE_VIEW::indentation_set_(GX_VALUE indentation)
{
    this->indentation = indentation;

    if (is_visible_())
    {
        position_();

        dirty_mark_();
    }
}

void GX_TREE_VIEW::position_()
{
    GX_RECTANGLE child_size;

    child_size.left = (GX_VALUE)(client.left + x_shift + indentation);
    child_size.bottom = (GX_VALUE)(client.top + y_shift - 1);

    GX_VALUE max_right = 0;

    // Reposition this view items.
    for (GX_WIDGET* child = first_client_child_get_(); child; child = child->next_client_child_get_())
    {
        if (child->type == GX_TYPE_MENU_LIST)
        {
            continue;
        }

        child_size.top = (GX_VALUE)(child_size.bottom + 1);
        child_size.bottom = (GX_VALUE)(child_size.top + child->height_() - 1);
        child_size.right = (GX_VALUE)(child_size.left + child->width_() - 1);

        child->resize_(&child_size);

        if (child->type == GX_TYPE_MENU)
        {
            GX_MENU* menu = (GX_MENU*)child;

            menu->position_(indentation);

            GX_MENU_LIST* list = &menu->list;

            if (list->parent && list->first_child)
            {
                if (list->right_() > max_right)
                {
                    max_right = list->right_();
                }

                child_size.bottom = list->bottom_();
            }
            else
            {
                if (list->parent)
                {
                    list->detach_();
                }

                if (child_size.right > max_right)
                {
                    max_right = child_size.right;
                }
            }
        }

        child->status &= (ULONG)(~GX_STATUS_ACCEPTS_FOCUS);
    }

    tree_width = (GX_VALUE)(max_right - (client.left + x_shift) + 1);
    tree_height = (GX_VALUE)(child_size.bottom - (client.top + y_shift) + 1);

    GX_SCROLLBAR* scrollbar = scrollbar_find_(GX_TYPE_HORIZONTAL_SCROLL);
    if (scrollbar)
    {
        scrollbar->reset_();

        if (tree_width <= client.width_())
        {
            scrollbar->hide_();
        }
        else
        {
            scrollbar->show_();
        }
    }

    scrollbar = scrollbar_find_(GX_TYPE_VERTICAL_SCROLL);
    if (scrollbar)
    {
        scrollbar->reset_();

        if (tree_height <= client.height_())
        {
            scrollbar->hide_();
        }
        else
        {
            scrollbar->show_();
        }
    }

    if (style & GX_STYLE_TREE_VIEW_SHOW_ROOT_LINES)
    {
        dirty_mark_if_visible_();
    }
}

void GX_TREE_VIEW::scroll_info_get_(ULONG style, GX_SCROLL_INFO* info)
{
    INT shift;
    INT value;

    if (style & GX_SCROLLBAR_VERTICAL)
    {
        info->minimum = client.top;
        info->maximum = info->minimum + tree_height - 1;
        info->visible = client.height_();

        if (tree_height < info->visible)
        {
            info->maximum = info->minimum + info->visible - 1;
        }

        shift = y_shift;
        value = client.top - shift;

        if (value < info->minimum)
        {
            value = info->minimum;
        }
        else if (value > info->maximum - info->visible + 1)
        {
            value = info->maximum - info->visible + 1;
        }

        shift = client.top - value;

        if (shift != y_shift)
        {
            scroll_(0, (GX_VALUE)(shift - y_shift));
        }

        info->value = value;
    }
    else
    {
        info->minimum = client.left;
        info->maximum = info->minimum + tree_width - 1;
        info->visible = client.width_();

        if (tree_width < info->visible)
        {
            info->maximum = info->minimum + info->visible - 1;
        }

        shift = x_shift;
        value = client.left - shift;

        if (value < info->minimum)
        {
            value = info->minimum;
        }
        else if (value > info->maximum - info->visible + 1)
        {
            value = info->maximum - info->visible + 1;
        }

        shift = client.left - value;

        if (shift != x_shift)
        {
            scroll_((GX_VALUE)(shift - x_shift), 0);
        }

        info->value = value;
    }

    info->increment = (GX_VALUE)((info->maximum - info->minimum) / 10);
}

void GX_TREE_VIEW::scroll_(GX_VALUE x_scroll, GX_VALUE y_scroll)
{
    GX_RECTANGLE block = client;

    if (!(style & GX_STYLE_TRANSPARENT) &&
        (style & GX_STYLE_BORDER_THIN))
    {
        // Widget with think border have a round corner, which have some effects to block move.

        block.right = (GX_VALUE)(block.left + 1);
        dirty_mark_(&block);

        block = client;
        block.bottom = (GX_VALUE)(block.top + 1);
        dirty_mark_(&block);

        block = client;
        block.left = (GX_VALUE)(block.left + 2);
        block.top = (GX_VALUE)(block.top + 2);
    }

    x_shift = (GX_VALUE)(x_shift + x_scroll);
    y_shift = (GX_VALUE)(y_shift + y_scroll);

    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        if (child->is_client_())
        {
            child->scroll_shift_(x_scroll, y_scroll, GX_TRUE);
        }
    }

    block_move_(&block, x_scroll, y_scroll);

    GX_SCROLLBAR* scrollbar = GX_NULL;

    if (x_scroll)
    {
        scrollbar = scrollbar_find_(GX_TYPE_HORIZONTAL_SCROLL);
        if (scrollbar)
        {
            scrollbar->reset_();
        }
    }

    if (y_scroll)
    {
        scrollbar = scrollbar_find_(GX_TYPE_VERTICAL_SCROLL);
        if (scrollbar)
        {
            scrollbar->reset_();
        }
    }
}

void GX_TREE_VIEW::root_line_color_set_(GX_RESOURCE_ID color)
{
    this->root_line_color = color;

    dirty_mark_if_visible_();
}

void GX_TREE_VIEW::root_pixelmap_set_(GX_RESOURCE_ID expand_map_id, GX_RESOURCE_ID collapse_map_id)
{
    this->expand_pixelmap_id = expand_map_id;
    this->collapse_pixelmap_id = collapse_map_id;

    dirty_mark_if_visible_();
}

void GX_TREE_VIEW::root_draw_helper_(GX_WIDGET* start, GX_WIDGET* owner)
{
    GX_PIXELMAP*  map;
    GX_PIXELMAP*  collapse_map;
    GX_PIXELMAP*  expand_map;
    GX_VALUE      xpos;
    GX_VALUE      ypos;
    GX_VALUE      yend;
    GX_VALUE      map_width = 0;
    GX_VALUE      map_height = 0;
    ULONG         vertical_mask = 0x80000000;
    GX_BRUSH*     brush;

    _gx_context_pixelmap_get(this->collapse_pixelmap_id, &collapse_map);
    _gx_context_pixelmap_get(this->expand_pixelmap_id, &expand_map);

    if (collapse_map)
    {
        map_width = collapse_map->width;
        map_height = collapse_map->height;
    }

    GX_WIDGET* previous_child = GX_NULL;
    GX_WIDGET* child = start->first_client_child_get_();

    for (; child; child = child->next_client_child_get_())
    {
        if (child->type == GX_TYPE_MENU_LIST)
        {
            continue;
        }

        map = GX_NULL;
        xpos = (GX_VALUE)(child->left_() - indentation / 2);
        ypos = (GX_VALUE)((child->top_() + child->bottom_() - 1) / 2);

        if (child->type == GX_TYPE_MENU)
        {
            GX_MENU_LIST* list = &((GX_MENU*)child)->list;

            if (list->first_child)
            {
                map = (child->style & GX_STYLE_MENU_EXPANDED) ? collapse_map : expand_map;
                if (map)
                {
                    // Calculate collapse/expand icon draw area.
                    xpos = (GX_VALUE)(xpos - map_width / 2);
                    ypos = (GX_VALUE)(ypos - map_height / 2);

                    // Draw collapse/expand icons.
                    _gx_canvas_pixelmap_draw(xpos, ypos, map);
                }
            }
        }

        if (this->style & GX_STYLE_TREE_VIEW_SHOW_ROOT_LINES)
        {
            // Draw root lines.
            _gx_context_brush_get(&brush);
            brush->pattern_mask = 0x80000000;

            if (map)
            {
                // Get right center of the collapse/expand icon.
                xpos = (GX_VALUE)(xpos + map_width);
                ypos = (GX_VALUE)(ypos + map_height / 2);
            }
            else
            {
                xpos = (GX_VALUE)(child->left_() - indentation / 2 + 1);
                ypos = child->y_center_();
            }

            // Draw horizontal root line that connection collapse/expand icon with the menu item.
            _gx_canvas_line_draw(xpos, ypos, (GX_VALUE)(child->left_() - 1), ypos);

            if (previous_child || owner)
            {
                if (map)
                {
                    // Get top center of the collapse/expand icon.
                    xpos = (GX_VALUE)(xpos - map_width / 2);
                    ypos = (GX_VALUE)(ypos - map_height / 2 - 1);
                }
                else
                {
                    xpos = (GX_VALUE)(xpos - 1);
                }

                if (previous_child)
                {
                    if ((previous_child->type == GX_TYPE_MENU) &&
                        ((GX_MENU*)previous_child)->list.first_child)
                    {
                        yend = (GX_VALUE)(previous_child->y_center_() - map_height / 2 + map_height);
                    }
                    else
                    {
                        yend = (GX_VALUE)(previous_child->y_center_() + 1);
                    }
                }
                else
                {
                    yend = owner->bottom_();
                }

                brush->pattern_mask = vertical_mask;

                // Draw vertical root line that connect to the previous or parent item.
                _gx_canvas_line_draw(xpos, yend, xpos, ypos);

                vertical_mask = brush->pattern_mask;
            }
        }

        previous_child = child;
    }
}

void GX_TREE_VIEW::root_draw_()
{
    GX_WIDGET* next_child;
    GX_WIDGET* owner;

    GX_WIDGET* child = first_client_child_get_();
    while (child)
    {
        if (child->type == GX_TYPE_MENU)
        {
            GX_MENU_LIST* list = &((GX_MENU*)child)->list;

            if ((list->first_child) &&
                (list->status & GX_STATUS_VISIBLE))
            {
                child = list->first_child;
                continue;
            }
        }

        next_child = child->next_client_child_get_();

        while ((next_child == GX_NULL) && (child != this))
        {
            child = child->parent;

            if (child->type == GX_TYPE_MENU_LIST)
            {
                owner = ((GX_MENU_LIST*)child)->owner;
                next_child = owner;
            }
            else
            {
                owner = GX_NULL;
                next_child = child;
            }

            root_draw_helper_(child, owner);

            child = next_child;
            next_child = next_child->next_client_child_get_();
        }

        if (child == this)
        {
            break;
        }

        child = next_child;
    }
}

void GX_TREE_VIEW::draw_()
{
    GX_WINDOW::background_draw_();

    GX_DRAW_CONTEXT* context = gx_context_current_();

    context->brush_pattern_set_(0x55555555);
    context->brush.width = 1;
    context->line_color_set_(root_line_color);

    GX_RECTANGLE saved_dirty = context->dirty;

    gx_rectangle_intersect_(client, context->dirty, &context->dirty);

    root_draw_();

    context->dirty = saved_dirty;
    context->brush_pattern_set_(0);

    children_draw_();
}


GX_WIDGET* GX_TREE_VIEW::item_find_(GX_VALUE line_y) const
{
    GX_WIDGET* found = GX_NULL;

    GX_WIDGET* child = first_client_child_get_();
    while (child && child->type == GX_TYPE_MENU_LIST)
    {
        child = child->next_client_child_get_();
    }

    while (child)
    {
        GX_WIDGET* next_child = child->next_client_child_get_();
        while (next_child && next_child->type == GX_TYPE_MENU_LIST)
        {
            next_child = next_child->next_client_child_get_();
        }

        if (child->top_() <= line_y && line_y <= child->bottom_())
        {
            found = child;
            break;
        }

        bool search_child = false;

        if (next_child)
        {
            if (line_y < next_child->top_())
            {
                search_child = true;
            }
            else if (line_y <= next_child->bottom_())
            {
                found = next_child;
                break;
            }
        }
        else
        {
            search_child = true;
        }

        if (search_child)
        {
            // Line y is before next item.
            if (child->type == GX_TYPE_MENU)
            {
                GX_MENU_LIST* list = &((GX_MENU *)child)->list;

                if (list->is_visible_())
                {
                    child = list->first_child;
                    continue;
                }
            }
            else
            {
                // Not found.
                break;
            }
        }

        child = next_child;
    }

    return found;
}

void GX_TREE_VIEW::root_icon_rect_get_(const GX_WIDGET* item, GX_RECTANGLE* rect) const
{
    GX_PIXELMAP* map;
    pixelmap_get_(expand_pixelmap_id, &map);

    if (map)
    {
        rect->left = (GX_VALUE)(item->left_() - indentation / 2 - map->width / 2);
        rect->top = (GX_VALUE)((item->top_() + item->bottom_() - 1) / 2 - map->height / 2);
        rect->right = (GX_VALUE)(rect->left + map->width - 1);
        rect->bottom = (GX_VALUE)(rect->top + map->height - 1);
    }
    else
    {
        rect->set_(0, 0, 0, 0);
    }
}

bool GX_TREE_VIEW::selected_find_(const GX_MENU* menu) const
{
    if (!this->selected)
    {
        return false;
    }

    bool found = false;

    GX_WIDGET* child = menu->list.first_child;
    for (; child; child = child->next)
    {
        if (child == this->selected)
        {
            found = true;
            break;
        }

        if (child->type == GX_TYPE_MENU)
        {
            found = selected_find_((GX_MENU*)child);
            if (found)
            {
                break;
            }
        }
    }

    return found;
}

void GX_TREE_VIEW::pen_down_event_process_(GX_EVENT* event_ptr)
{
    bool check_selection = false;

    GX_POINT point = event_ptr->payload.pointdata;

    // Find tree view item that is intersecting the horizontal line of the click point.
    GX_WIDGET* child = item_find_(point.y);

    if (child && (child->type == GX_TYPE_MENU) && ((GX_MENU*)child)->list.first_child)
    {
        GX_RECTANGLE root_rect;
        root_icon_rect_get_(child, &root_rect);

        if (root_rect.contain_point_(point))
        {
            if (child->style & GX_STYLE_MENU_EXPANDED)
            {
                child->style &= ~GX_STYLE_MENU_EXPANDED;

                if (selected_find_((const GX_MENU*)child))
                {
                    selected_set_(child);
                }
            }
            else
            {
                child->style |= GX_STYLE_MENU_EXPANDED;
            }

            position_();

            root_rect = client;
            if (!(style & GX_STYLE_TREE_VIEW_SHOW_ROOT_LINES))
            {
                root_rect.top = child->size.top;
            }

            dirty_mark_(&root_rect);
        }
        else
        {
            check_selection = true;
        }
    }
    else
    {
        check_selection = true;

        GX_WINDOW::event_process_(event_ptr);
    }

    if (check_selection)
    {
        child = top_child_in_point_(point, GX_STATUS_SELECTABLE);
        if (child)
        {
            if ((child->type == GX_TYPE_MENU) ||
                (child->parent->type == GX_TYPE_MENU_LIST))
            {
                selected_set_(child);
            }
        }
    }
}

UINT GX_TREE_VIEW::event_process_(GX_EVENT* event_ptr)
{
    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        GX_WINDOW::event_process_(event_ptr);

        if (parent && (parent->type != GX_TYPE_MENU_LIST))
        {
            position_();
        }
        break;

    case GX_EVENT_PEN_DOWN:
        pen_down_event_process_(event_ptr);
        break;

    case GX_EVENT_VERTICAL_SCROLL:
        scroll_(0, (GX_VALUE)(event_ptr->payload.intdata[1] - event_ptr->payload.intdata[0]));
        break;

    case GX_EVENT_HORIZONTAL_SCROLL:
        scroll_((GX_VALUE)(event_ptr->payload.intdata[1] - event_ptr->payload.intdata[0]), 0);
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
/*    _gx_tree_view_create                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates an tree view.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to the tree view      */
/*                                            control block               */
/*    name                                  Name of the tree view         */
/*    parent                                Parent widget control block   */
/*    style                                 Style of the widget           */
/*    tree_menu_id                          Application-defined ID of     */
/*                                          the tree view                 */
/*    size                                  Tree view size                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_create                     Create a window               */
/*    _gx_widget_link                       Link a widget to its parent   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_tree_view_create(GX_TREE_VIEW* tree, const GX_CHAR* name, GX_WIDGET* parent,
                          ULONG style, USHORT tree_menu_id, const GX_RECTANGLE* size)
{
    return tree->create_(name, parent, style, tree_menu_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_tree_view_draw                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified tree view, which is a special type*/
/*    of widget.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer the to tree view      */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_background_draw            Draw window background        */
/*    _gx_context_brush_pattern_set         Set brush pattern             */
/*    _gx_context_brush_width_set           Set brush width               */
/*    _gx_context_line_color_set            Set line color                */
/*    _gx_utility_rectangle_overlap_detect  Detect overlap of supplied    */
/*                                            rectangles                  */
/*    _gx_tree_view_root_draw               Draw root lines and icons     */
/*    _gx_widget_children_draw              Draw widget children          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_tree_view_draw(GX_TREE_VIEW* tree)
{
    tree->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_tree_view_event_process                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes an event for the tree menu.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to tree menu control  */
/*                                            block                       */
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_event_process              Default window event process  */
/*    _gx_tree_view_position                Position the children of the  */
/*                                            tree view                   */
/*    _gx_tree_menu_pen_down_event_process  Handle pen down event         */
/*    _gx_tree_view_scroll                  Scroll tree view client area  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_tree_view_event_process(GX_TREE_VIEW* tree, GX_EVENT* event_ptr)
{
    return tree->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_tree_view_indentation_set                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets indentation for the tree view.                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to the tree view      */
/*                                            control block               */
/*    indentation                           Indentation to set            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget area as dirty */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_tree_view_indentation_set(GX_TREE_VIEW* tree, GX_VALUE indentation)
{
    tree->indentation_set_(indentation);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_tree_view_position                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function positions a tree menu.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Tree view control block       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_first_client_child_get     Get the first client child    */
/*    _gx_widget_next_client_child_get      Get the next client child     */
/*    _gx_widget_height_get                 Get widget height             */
/*    _gx_widget_width_get                  Get widget width              */
/*    _gx_widget_resize                     Resize a widget               */
/*    _gx_menu_position                     Position a menu widget        */
/*    _gx_window_scrollbar_find             Find scrollbar for a window   */
/*    _gx_scrollbar_reset                   Reset scrollbar information   */
/*    _gx_widget_hide                       Hide a widget                 */
/*    _gx_widget_show                       Show a widget                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_tree_view_position(GX_TREE_VIEW* tree)
{
    tree->position_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_tree_view_root_line_color_set                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets root line color for the tree view.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to the tree view      */
/*                                            control block               */
/*    color_id                              Resource ID of root line color*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget area as dirty */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_tree_view_root_line_color_set(GX_TREE_VIEW* tree, GX_RESOURCE_ID color)
{
    tree->root_line_color_set_(color);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_tree_view_root_pixelmap_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets expand and collapse pixelmap for the tree view.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to the tree view      */
/*                                            control block               */
/*    expand_map_id                         Resource id of expand pixelmap*/
/*    collapse_map_id                       Reource id of collapse        */
/*                                             pixlemap                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget area as dirty */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_tree_view_root_pixelmap_set(GX_TREE_VIEW* tree, GX_RESOURCE_ID expand_map_id, GX_RESOURCE_ID collapse_map_id)
{
    tree->root_pixelmap_set_(expand_map_id, collapse_map_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_tree_view_selected_get                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves the selected item.                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to the tree view      */
/*                                            control block               */
/*    selected                              Selected item to retrieve     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget area as dirty */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_tree_view_selected_get(GX_TREE_VIEW* tree, GX_WIDGET** selected)
{
    *selected = tree->selected;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_tree_view_selected_set                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function set selected item for the specified tree view.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to the tree view      */
/*                                            control block               */
/*    selected                              New selected item             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget area as dirty */
/*    _gx_widget_event_generate             Create an event and send it to*/
/*                                            the widget parent           */
/*    _gx_tree_view_selected_visible        Make selected item visible.   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_tree_view_selected_set(GX_TREE_VIEW* tree, GX_WIDGET* selected)
{
    tree->selected_set_(selected);

    return GX_SUCCESS;
}
