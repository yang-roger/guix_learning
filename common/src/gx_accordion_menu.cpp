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
/**   Accordion Menu Management (Menu)                                    */
/**                                                                       */
/**************************************************************************/

#include "gx_accordion_menu.h"

#include "gx_system.h"
#include "gx_menu.h"

#define GX_MENU_ANIMATION_OPEN  0x01
#define GX_MENU_ANIMATION_CLOSE 0x02

#define GX_MENU_OPEN_TIMER      0xf0
#define GX_MENU_CLOSE_TIMER     0xf1

static void gx_accordion_menu_draw_(GX_ACCORDION_MENU* menu)
{
    menu->draw_();
}

static UINT gx_accordion_menu_event_process_(GX_ACCORDION_MENU* menu, GX_EVENT* event_ptr)
{
    return menu->event_process_(event_ptr);
}

UINT GX_ACCORDION_MENU::create_(const GX_CHAR* name, GX_WIDGET* parent,
                                ULONG style, USHORT accordion_menu_id, const GX_RECTANGLE* size)
{
    GX_WIDGET::create_(name, GX_NULL, style, accordion_menu_id, size);

    this->type = GX_TYPE_ACCORDION_MENU;

    this->expand_item = GX_NULL;
    this->collapse_item = GX_NULL;
    this->animation_status = 0;

    this->draw_function = (void (*)(GX_WIDGET*))gx_accordion_menu_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_accordion_menu_event_process_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

UINT GX_ACCORDION_MENU::position_()
{
    GX_MENU_LIST* child_list;

    // Reposition items of menu list.
    GX_WIDGET* child = first_child;
    while (child)
    {
        if (child->type == GX_TYPE_MENU)
        {
            child_list = &((GX_MENU *)child)->list;

            if (child_list->first_child)
            {
                child = child_list->first_child;
                continue;
            }
            else if (child_list->parent)
            {
                child_list->detach_();
            }
        }
        else if (child->type == GX_TYPE_ACCORDION_MENU)
        {
            if (child->first_child)
            {
                child = child->first_child;
                continue;
            }
        }

        while ((child->next == GX_NULL) && (child != this))
        {
            child = child->parent;

            if (child->type == GX_TYPE_MENU_LIST)
            {
                child_list = (GX_MENU_LIST *)child;
                child = child_list->owner;
            }

            if (child->type == GX_TYPE_MENU)
            {
                ((GX_MENU*)child)->one_level_position_(0);
            }
            else if (child->type == GX_TYPE_ACCORDION_MENU)
            {
                ((GX_ACCORDION_MENU*)child)->one_level_position_();
            }
        }

        if (child == this)
        {
            break;
        }

        child = child->next;
    }

    return GX_SUCCESS;
}

UINT GX_ACCORDION_MENU::one_level_position_()
{
    GX_VALUE border_width = border_width_();

    GX_RECTANGLE client;
    client_area_get_(&client, border_width);

    GX_RECTANGLE new_size = client;
    new_size.bottom = (GX_VALUE)(new_size.top - 1);

    GX_MENU_LIST* list;

    GX_VALUE bottom = new_size.bottom;
    this->expand_item = GX_NULL;

    // Reposition accordion menu items.
    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        switch (child->type)
        {
        case GX_TYPE_MENU:
            list = &((GX_MENU *)child)->list;

            if (child->style & GX_STYLE_MENU_EXPANDED)
            {
                if (!this->expand_item && list->first_child)
                {
                    this->expand_item = child;
                }
                else
                {
                    child->style &= ~GX_STYLE_MENU_EXPANDED;
                    list->detach_();

                    list->size.bottom = (GX_VALUE)(list->size.top - 1);
                }
            }

            new_size.top = (GX_VALUE)(bottom + 1);
            new_size.bottom = (GX_VALUE)(new_size.top + child->height_() - 1);

            _gx_widget_resize(child, &new_size);

            if (list->parent)
            {
                list->shift_((GX_VALUE)(new_size.left - list->left_()),
                             (GX_VALUE)(new_size.bottom + 1 - list->top_()),
                             GX_FALSE);

                bottom = list->size.bottom;
            }
            else
            {
                bottom = new_size.bottom;
            }
            break;

        case GX_TYPE_MENU_LIST:
            break;

        default:
            new_size.top = (GX_VALUE)(bottom + 1);
            new_size.bottom = (GX_VALUE)(new_size.top + child->height_() - 1);

            child->resize_(&new_size);
            bottom = new_size.bottom;
            break;
        }

        child->status_remove_(GX_STATUS_ACCEPTS_FOCUS);
    }

    new_size = this->size;
    new_size.bottom = (GX_VALUE)(bottom + border_width);

    resize_(&new_size);

    return GX_SUCCESS;
}

void GX_ACCORDION_MENU::draw_()
{
    border_and_background_draw_();
    children_draw_();
}

void GX_ACCORDION_MENU::timer_event_process_(UINT timer_id)
{
    switch (timer_id)
    {
    case GX_MENU_OPEN_TIMER:
        open_animation_update_();
        break;

    case GX_MENU_CLOSE_TIMER:
        close_animation_update_();
        break;

    default:
        break;
    }
}

static GX_VALUE gx_menu_list_children_height_(const GX_MENU_LIST& menu_list)
{
    INT height = 0;

    GX_WIDGET* child = menu_list.first_child;
    for (; child; child = child->next)
    {
        height += child->height_();
    }

    return (GX_VALUE)height;
}

// Calcualte shift value for each animation step.
static GX_VALUE gx_menu_animation_shift_value_(GX_VALUE list_children_height)
{
    GX_VALUE shift = (GX_VALUE)(list_children_height / 10);
    if (shift == 0)
    {
        shift = list_children_height;
    }

    return shift;
}

void GX_ACCORDION_MENU::open_animation_update_()
{
    GX_MENU* selected = (GX_MENU*)expand_item;
    if (!selected)
    {
        return;
    }

    GX_MENU_LIST& selected_list = selected->list;

    GX_VALUE list_children_height = gx_menu_list_children_height_(selected_list);
    GX_VALUE shift = gx_menu_animation_shift_value_(list_children_height);

    GX_VALUE height = selected_list.height_();

    if (height + shift >= list_children_height)
    {
        shift = (GX_VALUE)(list_children_height - height);

        timer_stop_(GX_MENU_OPEN_TIMER);
        animation_status &= (GX_UBYTE)(~GX_MENU_ANIMATION_OPEN);

        selected->style |= GX_STYLE_MENU_EXPANDED;
    }

    // Resize menu list.
    GX_RECTANGLE new_size = selected_list.size;
    new_size.bottom = (GX_VALUE)(new_size.bottom + shift);
    selected_list.resize_(&new_size);

    // Shift menus that follow the opening menu.
    shift_(selected, shift);
}

void GX_ACCORDION_MENU::close_animation_update_()
{
    GX_MENU* deselected = (GX_MENU*)collapse_item;
    if (!deselected)
    {
        return;
    }

    GX_MENU_LIST& deselected_list = deselected->list;

    GX_VALUE list_children_height = gx_menu_list_children_height_(deselected_list);
    GX_VALUE shift = gx_menu_animation_shift_value_(list_children_height);

    GX_VALUE height = deselected_list.height_();

    if (shift > height)
    {
        shift = height;

        timer_stop_(GX_MENU_CLOSE_TIMER);
        animation_status &= (GX_UBYTE)(~GX_MENU_ANIMATION_CLOSE);

        deselected->style &= (ULONG)(~GX_STYLE_MENU_EXPANDED);

        collapse_item = GX_NULL;

        deselected_list.detach_();
    }

    GX_RECTANGLE new_size = deselected_list.size;
    new_size.bottom = (GX_VALUE)(new_size.bottom - shift);
    deselected_list.resize_(&new_size);

    shift_(deselected, -shift);
}

void GX_ACCORDION_MENU::shift_(GX_WIDGET* animation_target, INT shift)
{
    GX_RECTANGLE new_size;
    GX_WIDGET* child;

    shift_helper_(animation_target, shift);

    GX_WIDGET* temp_target = this;
    GX_WIDGET* parent = temp_target->parent;

    while (parent)
    {
        if (parent->type == GX_TYPE_MENU_LIST)
        {
            // If the accordion menu is the child of a menu list,
            // Resize the menu list.
            new_size = parent->size;
            new_size.bottom = (GX_VALUE)(new_size.bottom + shift);
            parent->resize_(&new_size);

            child = ((GX_MENU_LIST*)parent)->owner;
            if (child)
            {
                parent = child->parent;

                if (parent && parent->type == GX_TYPE_ACCORDION_MENU)
                {
                    ((GX_ACCORDION_MENU*)parent)->shift_helper_(child, shift);
                }
                else
                {
                    break; // Break out of the "while(parent)" loop and return.
                }
            }
            else
            {
                break; // Break out of the "while(parent)" loop and return.
            }
        }
        else if (parent->type == GX_TYPE_ACCORDION_MENU)
        {
            ((GX_ACCORDION_MENU*)parent)->shift_helper_(temp_target, shift);
        }
        else
        {
            break; // Break out of the "while(parent)" loop and return.
        }

        temp_target = parent;
        parent = temp_target->parent;
    }
}

void GX_ACCORDION_MENU::shift_helper_(GX_WIDGET* animation_target, INT shift)
{
    bool shift_start = false;

    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        switch (child->type)
        {
        case GX_TYPE_MENU:
            if (shift_start)
            {
                // Shift the menu item.
                child->scroll_shift_(0, shift, GX_TRUE);

                // Shift the menu list which is a member of menu item.
                ((GX_MENU*)child)->list.scroll_shift_(0, shift, GX_TRUE);
            }
            break;

        case GX_TYPE_MENU_LIST:
            // The shift work has done by the menu list owner.
            break;

        default:
            if (shift_start)
            {
                // Shift other types of menu item.
                child->scroll_shift_(0, shift, GX_TRUE);
            }
            break;
        }

        if (child == animation_target)
        {
            shift_start = true;
        }
    }

    GX_RECTANGLE new_size = this->size;
    new_size.bottom = (GX_VALUE)(new_size.bottom + shift);
    resize_(&new_size);
}

void GX_ACCORDION_MENU::pen_down_event_process_(GX_EVENT* event_ptr)
{
    GX_WIDGET* child = top_child_in_point_(event_ptr->payload.pointdata, GX_STATUS_SELECTABLE);

    if (child && (child->type == GX_TYPE_MENU) && ((GX_MENU*)child)->list.first_child)
    {
        if ((animation_status & GX_MENU_ANIMATION_CLOSE) ||
            (animation_status & GX_MENU_ANIMATION_OPEN))
        {
            // Animation is going on.
            return;
        }

        // Set expand and collapse menu items.
        if (child->style & GX_STYLE_MENU_EXPANDED)
        {
            collapse_item = child;
            expand_item = GX_NULL;
        }
        else
        {
            if (expand_item != child)
            {
                collapse_item = expand_item;
            }
            expand_item = child;
        }

        _gx_system_input_capture(this);
    }
    else
    {
        GX_WIDGET::event_process_(event_ptr);
    }
}

void GX_ACCORDION_MENU::pen_up_event_process_(GX_EVENT* event_ptr)
{
    if (this->status & GX_STATUS_OWNS_INPUT)
    {
        _gx_system_input_release(this);

        if (collapse_item)
        {
            // Start a timer to collapse a menu.
            timer_start_(GX_MENU_CLOSE_TIMER, 1, 1);
            animation_status |= GX_MENU_ANIMATION_CLOSE;
        }

        if (expand_item)
        {
            // Start a timer to expand a menu.
            timer_start_(GX_MENU_OPEN_TIMER, 1, 1);
            animation_status |= GX_MENU_ANIMATION_OPEN;

            GX_MENU_LIST* menu_list = &((GX_MENU*)expand_item)->list;

            // Link menu list to menu's parent.
            menu_list->link_to_(expand_item->parent);

            GX_VALUE x_shift = (GX_VALUE)(expand_item->left_() - menu_list->left_());
            GX_VALUE y_shift = (GX_VALUE)(expand_item->bottom_() + 1 - menu_list->top_());

            // Shift menu list to the bottom of the menu.
            if (x_shift || y_shift)
            {
                menu_list->shift_(x_shift, y_shift, GX_FALSE);
            }
        }
    }
    else
    {
        GX_WIDGET::event_process_(event_ptr);
    }
}

UINT GX_ACCORDION_MENU::event_process_(GX_EVENT* event_ptr)
{
    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        GX_WIDGET::event_process_(event_ptr);

        // We should only call menu position for a top level accordion menu.
        if (!(parent && parent->type == GX_TYPE_MENU_LIST))
        {
            position_();
        }
        break;

    case GX_EVENT_PEN_DOWN:
        pen_down_event_process_(event_ptr);
        break;

    case GX_EVENT_PEN_UP:
        pen_up_event_process_(event_ptr);
        break;

    case GX_EVENT_TIMER:
        timer_event_process_(event_ptr->payload.timer_id);
        break;

    default:
        return GX_WIDGET::event_process_(event_ptr);
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_accordion_menu_create                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates an accordion menu.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to the accordion menu */
/*                                            control block               */
/*    name                                  Name of the menu              */
/*    parent                                Parent widget control block   */
/*    style                                 Style of the widget           */
/*    accordion_menu_id                     Application-defined ID of     */
/*                                          the accordion menu            */
/*    size                                  Accordion menu size           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_create                     Create a widget               */
/*    _gx_widget_link                       Link a widget to its parent   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_accordion_menu_create(GX_ACCORDION_MENU* menu, const GX_CHAR* name, GX_WIDGET* parent,
                               ULONG style, USHORT accordion_menu_id, const GX_RECTANGLE* size)
{
    return menu->create_(name, parent, style, accordion_menu_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_accordion_menu_draw                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified menu, which is a special type of  */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    accordion                               Pointer the to accordion    */
/*                                              menu control block        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_background_draw            Draw widget backgrond         */
/*    _gx_widget_children_draw              Draw widget children          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_accordion_menu_draw(GX_ACCORDION_MENU* menu)
{
    menu->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_accordion_menu_event_process                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service processes an event for the accordion menu.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    accordion                             Pointer to accordion menu     */
/*                                            control block               */
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_process              Default widget event process  */
/*    _gx_accordion_menu_position           Position an accordion menu    */
/*    _gx_accordion_menu_open_animation_update                            */
/*                                          Execute one step menu open    */
/*                                            animation                   */
/*    _gx_accordion_menu_close_animation_update                           */
/*                                          Execute one step menu close   */
/*                                            animation                   */
/*    _gx_accordion_menu_pen_down_event_process                           */
/*                                          Handle pen down event         */
/*    _gx_accordion_menu_pen_up_event_process                             */
/*                                          Handle pen up event           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_accordion_menu_event_process(GX_ACCORDION_MENU* menu, GX_EVENT* event_ptr)
{
    return menu->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_accordion_menu_position                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function positions an accordion menu and its items.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    accordion                             Accordion menu control block  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_menu_one_level_position           Position a menu widget        */
/*    _gx_accordion_one_levelmenu_position  Position an accordion menu    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_accordion_menu_position(GX_ACCORDION_MENU* menu)
{
    return menu->position_();
}