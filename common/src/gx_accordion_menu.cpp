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

#if 0

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
UINT  _gx_accordion_menu_create(GX_ACCORDION_MENU *accordion, const GX_CHAR *name, GX_WIDGET *parent,
                                ULONG style, USHORT accordion_menu_id, const GX_RECTANGLE *size)
{

    /* Call the widget create function.  */
    _gx_widget_create((GX_WIDGET *)accordion, name, GX_NULL, style, accordion_menu_id, size);

    /* Populate the rest of the accordion menu control block - overriding as necessary.  */
    accordion->type = GX_TYPE_ACCORDION_MENU;
    accordion->event_process_function = (UINT (*)(GX_WIDGET *, GX_EVENT *))_gx_accordion_menu_event_process;
    accordion->draw_function = (void (*)(GX_WIDGET *))_gx_accordion_menu_draw;
    accordion->expand_item = GX_NULL;
    accordion->collapse_item = GX_NULL;
    accordion->animation_status = 0;

    /* Determine if a parent widget was provided.  */
    if (parent)
    {
        accordion->link_to_(parent);
    }

    /* Return completion status code. */
    return(GX_SUCCESS);
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
void  _gx_accordion_menu_draw(GX_ACCORDION_MENU *accordion)
{

    /* Draw accordion menu background. */
    _gx_widget_background_draw((GX_WIDGET *)accordion);

    _gx_widget_children_draw((GX_WIDGET *)accordion);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_accordion_menu_shift_helper                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to shift one accordion menu.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    accordion                             Pointer to accordion menu     */
/*                                            control block               */
/*    animation_target                      Animation target              */
/*    shift                                 Shift value                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_scroll_shift               Change the widget position    */
/*    _gx_widget_resize                     Resize the widget             */
/*    _gx_menu_list_shift                   Change the menu list position */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_accordion_menu_shift_helper                                     */
/*                                                                        */
/**************************************************************************/
static void _gx_accordion_menu_shift_helper(GX_ACCORDION_MENU *accordion, GX_WIDGET *animation_target, INT shift)
{
GX_WIDGET   *child;
GX_MENU     *menu;
GX_BOOL      shift_start = GX_FALSE;
GX_RECTANGLE size;

    child = accordion->first_child;
    while (child)
    {
        switch (child->type)
        {
        case GX_TYPE_MENU:
            if (shift_start)
            {
                /* Shift the menu item. */
                child->scroll_shift_(0, shift, GX_TRUE);

                menu = (GX_MENU *)child;

                /* Shift the menu list which is a member of menu item. */
                menu->list.scroll_shift_(0, shift, GX_TRUE);
            }
            break;

        case GX_TYPE_MENU_LIST:
            /* The shift work has done by the menu list owner. */
            break;

        default:
            if (shift_start)
            {
                /* Shift other types of menu item. */
                child->scroll_shift_(0, shift, GX_TRUE);
            }
            break;
        }

        if (child == animation_target)
        {
            shift_start = GX_TRUE;
        }

        child = child->next;
    }

    /* Resize accordion menu. */
    size = accordion->size;
    size.bottom = (GX_VALUE)(size.bottom + shift);
    _gx_widget_resize((GX_WIDGET *)accordion, &size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_accordion_menu_shift                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to shift menu items for an accordion menu. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    accordion                             Pointer to accordion menu     */
/*                                            control block               */
/*    animation_target                      Animation target              */
/*    shift                                 Shift value                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_resize                     Resize the widget             */
/*    _gx_accordion_menu_shift_heloer       Change the accordion menu     */
/*                                            position                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_accordion_menu_close_animation_update                           */
/*    _gx_accordion_menu_open_animation_update                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_accordion_menu_shift(GX_ACCORDION_MENU *accordion, GX_WIDGET *animation_target, INT shift)
{
GX_WIDGET   *temp_target;
GX_WIDGET   *parent;
GX_RECTANGLE size;
GX_WIDGET   *child;

    _gx_accordion_menu_shift_helper(accordion, animation_target, shift);

    temp_target = (GX_WIDGET *)accordion;
    parent = accordion->parent;

    while (parent)
    {

        if (parent->type == GX_TYPE_MENU_LIST)
        {
            /* If the accordion menu is the child of a menu list,
               Resize the menu list.*/
            size = parent->size;
            size.bottom = (GX_VALUE)(size.bottom + shift);
            _gx_widget_resize(parent, &size);

            /* Get the menu that owns the menu list. */
            child = ((GX_MENU_LIST *)parent)->owner;

            /* Get the parent of the menu*/
            if (child)
            {
                parent = child->parent;

                /* Check if the menu parent is an accordion menu. */
                if (parent && parent->type == GX_TYPE_ACCORDION_MENU)
                {
                    _gx_accordion_menu_shift_helper((GX_ACCORDION_MENU *)parent, child, shift);
                }
                else
                {
                    break; /* Break out of the "while(parent)" loop and return. */
                }
            }
            else
            {
                break; /* Break out of the "while(parent)" loop and return. */
            }
        }
        else if (parent->type == GX_TYPE_ACCORDION_MENU)
        {
            _gx_accordion_menu_shift_helper((GX_ACCORDION_MENU *)parent, temp_target, shift);
        }
        else
        {
            break;
        }

        temp_target = parent;
        parent = parent->parent;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_accordion_menu_close_animation_update                           */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to execute one step in menu close          */
/*    animation.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    accordion                             Pointer to accordin menu      */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_height_get                 Get widget height             */
/*    _gx_system_timer_stop                 Stop a timer for widget       */
/*    _gx_widget_detach                     Detach widget from its parent */
/*    _gx_widget_resize                     Resize a widget               */
/*    _gx_accordion_menu_shift              Change accordion menu position*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_accordion_menu_event_process                                    */
/*                                                                        */
/**************************************************************************/
static UINT _gx_accordion_menu_close_animation_update(GX_ACCORDION_MENU *accordion)
{
GX_MENU     *deselected = (GX_MENU *)accordion->collapse_item;
GX_VALUE     list_children_height = 0;
GX_VALUE     height;
GX_VALUE     shift;
GX_RECTANGLE size;
GX_WIDGET   *child;

    if (!deselected)
    {
        return GX_SUCCESS;
    }

    child = deselected->list.first_child;

    /* Calcualte total height of list children. */
    while (child)
    {
        _gx_widget_height_get(child, &height);
        list_children_height = (GX_VALUE)(list_children_height + height);

        child = child->next;
    }

    /* Calcualte shift value for each animation step. */
    shift = (GX_VALUE)(list_children_height / 10);
    if (shift == 0)
    {
        shift = list_children_height;
    }

    /* Get menu list height. */
    _gx_widget_height_get((GX_WIDGET *)&deselected->list, &height);

    if (shift > height)
    {
        shift = height;
        _gx_system_timer_stop((GX_WIDGET *)accordion, GX_MENU_CLOSE_TIMER);
        deselected->style &= (ULONG)(~GX_STYLE_MENU_EXPANDED);
        accordion->animation_status &= (GX_UBYTE)(~GX_MENU_ANIMATION_CLOSE);
        accordion->collapse_item = GX_NULL;
        _gx_widget_detach((GX_WIDGET *)&deselected->list);
    }

    /* Resize menu list. */
    size = deselected->list.size;
    size.bottom = (GX_VALUE)(size.bottom - shift);
    _gx_widget_resize((GX_WIDGET *)&deselected->list, &size);

    /* Shift menus that follow the opening menu. */
    _gx_accordion_menu_shift(accordion, (GX_WIDGET *)deselected, -shift);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_accordion_menu_open_animation_update                            */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to execute one step in menu open animation.*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    accordion                             Pointer to accordin menu      */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_height_get                 Get widget height             */
/*    _gx_system_timer_stop                 Stop a timer for widget       */
/*    _gx_widget_resize                     Resize widget                 */
/*    _gx_accordion_menu_shift              Change accordin menu position */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_accordion_menu_event_process                                    */
/*                                                                        */
/**************************************************************************/
static UINT _gx_accordion_menu_open_animation_update(GX_ACCORDION_MENU *accordion)
{
GX_MENU     *selected = (GX_MENU *)accordion->expand_item;
GX_VALUE     shift;
GX_RECTANGLE size;
GX_VALUE     list_children_height = 0;
GX_VALUE     height = 0;
GX_WIDGET   *child;

    if (!selected)
    {
        return GX_SUCCESS;
    }

    child = selected->list.first_child;

    /* Calcualte total height of list children. */
    while (child)
    {
        _gx_widget_height_get(child, &height);
        list_children_height = (GX_VALUE)(list_children_height + height);

        child = child->next;
    }

    /* Calcualte shift value for each animation step. */
    shift = (GX_VALUE)(list_children_height / 10);
    if (shift == 0)
    {
        shift = list_children_height;
    }

    /* Get menu list height. */
    _gx_widget_height_get((GX_WIDGET *)&selected->list, &height);

    if (height + shift >= list_children_height)
    {
        shift = (GX_VALUE)(list_children_height - height);
        _gx_system_timer_stop((GX_WIDGET *)accordion, GX_MENU_OPEN_TIMER);
        accordion->animation_status &= (GX_UBYTE)(~GX_MENU_ANIMATION_OPEN);
        selected->style |= GX_STYLE_MENU_EXPANDED;
    }

    /* Resize menu list. */
    size = selected->list.size;
    size.bottom = (GX_VALUE)(size.bottom + shift);
    _gx_widget_resize((GX_WIDGET *)&selected->list, &size);

    /* Shift menus that follow the opening menu. */
    _gx_accordion_menu_shift(accordion, (GX_WIDGET *)selected, shift);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_accordion_menu_pen_down_event_process                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to process pen down event for an accordion */
/*    menu.                                                               */
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
/*    _gx_system_top_widget_find            Find toppest widget that      */
/*                                            contain test point          */
/*    _gx_system_input_capture              Temporarily direct all inputs */
/*                                            to specified widget         */
/*    _gx_widget_event_process              Default widget event procses  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
static UINT _gx_accordion_menu_pen_down_event_process(GX_ACCORDION_MENU *accordion, GX_EVENT *event_ptr)
{
GX_WIDGET *widget = (GX_WIDGET *)accordion;
GX_WIDGET *child = GX_NULL;

    child = widget->top_child_in_point_(event_ptr->payload.pointdata, GX_STATUS_SELECTABLE);

    if (child && (child->type == GX_TYPE_MENU) &&
        ((GX_MENU *)child)->list.first_child)
    {
        if ((accordion->animation_status & GX_MENU_ANIMATION_CLOSE) ||
            (accordion->animation_status & GX_MENU_ANIMATION_OPEN))
        {
            /* Animation is going on. */
            return GX_SUCCESS;
        }

        /* Set expand and collapse menu items. */
        if (child->style & GX_STYLE_MENU_EXPANDED)
        {
            accordion->collapse_item = child;
            accordion->expand_item = GX_NULL;
        }
        else
        {
            if (accordion->expand_item != child)
            {
                accordion->collapse_item = accordion->expand_item;
            }
            accordion->expand_item = child;
        }

        _gx_system_input_capture(widget);
    }
    else
    {
        _gx_widget_event_process(widget, event_ptr);
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_accordion_menu_pen_up_event_process                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to process pen up event for an accordion   */
/*    menu.                                                               */
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
/*    _gx_system_input_release              Release event capture         */
/*    _gx_system_timer_start                Start a timer for widget      */
/*    _gx_widget_link                       Link a widget to its parent   */
/*    _gx_widget_shift                      Change a widget's position    */
/*    _gx_widget_event_process              Default widget event process  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
static UINT _gx_accordion_menu_pen_up_event_process(GX_ACCORDION_MENU *accordion, GX_EVENT *event_ptr)
{
GX_WIDGET    *widget = (GX_WIDGET *)accordion;
GX_WIDGET    *child = GX_NULL;
GX_MENU_LIST *menu_list;
GX_VALUE      x_shift;
GX_VALUE      y_shift;

    if (accordion->status & GX_STATUS_OWNS_INPUT)
    {
        _gx_system_input_release(widget);

        if (accordion->collapse_item)
        {
            /* Start a timer to collapse a menu. */
            _gx_system_timer_start((GX_WIDGET *)accordion, GX_MENU_CLOSE_TIMER, 1, 1);
            accordion->animation_status |= GX_MENU_ANIMATION_CLOSE;
        }

        if (accordion->expand_item)
        {

            /* Start a timer to expand a menu. */
            _gx_system_timer_start((GX_WIDGET *)accordion, GX_MENU_OPEN_TIMER, 1, 1);
            accordion->animation_status |= GX_MENU_ANIMATION_OPEN;

            child = accordion->expand_item;
            menu_list = &((GX_MENU *)child)->list;

            /* Link menu list to menu's parent. */
            menu_list->link_to_(child->parent);

            x_shift = (GX_VALUE)(child->size.left - menu_list->size.left);
            y_shift = (GX_VALUE)(child->size.bottom + 1 - menu_list->size.top);

            /* Shift menu list to the bottom of the menu. */
            if (x_shift || y_shift)
            {
                _gx_widget_shift((GX_WIDGET *)menu_list, x_shift, y_shift, GX_FALSE);
            }
        }
    }
    else
    {
        _gx_widget_event_process(widget, event_ptr);
    }

    return GX_SUCCESS;
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
UINT _gx_accordion_menu_event_process(GX_ACCORDION_MENU *accordion, GX_EVENT *event_ptr)
{
UINT timer_id;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        _gx_widget_event_process((GX_WIDGET *)accordion, event_ptr);

        /* Check if the accordion parent is a menu list, if so, this is not a top level accordion,
           Do not call accordion menu position here. We should only call menu position for a top level accordion menu. */
        if (!(accordion->parent && accordion->parent->type == GX_TYPE_MENU_LIST))
        {
            _gx_accordion_menu_position(accordion);
        }
        break;

    case GX_EVENT_PEN_DOWN:
        _gx_accordion_menu_pen_down_event_process(accordion, event_ptr);
        break;

    case GX_EVENT_PEN_UP:
        _gx_accordion_menu_pen_up_event_process(accordion, event_ptr);
        break;

    case GX_EVENT_TIMER:
        timer_id = event_ptr->payload.timer_id;
        if (timer_id == GX_MENU_CLOSE_TIMER)
        {
            _gx_accordion_menu_close_animation_update(accordion);
        }
        else if (timer_id == GX_MENU_OPEN_TIMER)
        {
            _gx_accordion_menu_open_animation_update(accordion);
        }
        break;

    default:
        return _gx_widget_event_process((GX_WIDGET *)accordion, event_ptr);
    }

    return(GX_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_accordion_menu_one_level_position                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function positions first level items for an accordion menu.    */
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
/*    _gx_widget_border_width_get           Get widget border width       */
/*    _gx_widget_client_get                 Get widget client rectangle   */
/*    _gx_widget_height_get                 Get widget height             */
/*    _gx_widget_shift                      Shift a widget                */
/*    _gx_widget_resize                     Resize a widget               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_accordion_menu_position                                         */
/*                                                                        */
/**************************************************************************/
static UINT  _gx_accordion_menu_one_level_position(GX_ACCORDION_MENU *accordion)
{
GX_WIDGET    *child = accordion->first_child;
GX_RECTANGLE  size;
GX_RECTANGLE  client;
GX_VALUE      height;
GX_VALUE      bottom;
GX_VALUE      border_width;
GX_MENU_LIST *list;

    _gx_widget_border_width_get((GX_WIDGET *)accordion, &border_width);
    _gx_widget_client_get((GX_WIDGET *)accordion, border_width, &client);

    size.left = client.left;
    size.right = client.right;
    size.top = client.top;
    size.bottom = (GX_VALUE)(size.top - 1);

    bottom = size.bottom;
    accordion->expand_item = GX_NULL;

    /* Reposition accordion menu items. */
    while (child)
    {
        switch (child->type)
        {
        case GX_TYPE_MENU:
            list = &((GX_MENU *)child)->list;

            if (child->style & GX_STYLE_MENU_EXPANDED)
            {
                if (!accordion->expand_item && list->first_child)
                {
                    accordion->expand_item = child;
                }
                else
                {
                    child->style &= (ULONG)(~GX_STYLE_MENU_EXPANDED);
                    _gx_widget_detach((GX_WIDGET *)list);

                    list->size.bottom = (GX_VALUE)(list->size.top - 1);
                }
            }

            _gx_widget_height_get(child, &height);

            size.top = (GX_VALUE)(bottom + 1);
            size.bottom = (GX_VALUE)(size.top + height - 1);

            _gx_widget_resize(child, &size);

            if (list->parent)
            {
                _gx_widget_shift((GX_WIDGET *)list,
                                 (GX_VALUE)(size.left - list->size.left),
                                 (GX_VALUE)(size.bottom + 1 - list->size.top), GX_FALSE);

                bottom = list->size.bottom;
            }
            else
            {
                bottom = size.bottom;
            }
            break;

        case GX_TYPE_MENU_LIST:
            break;

        default:
            _gx_widget_height_get(child, &height);
            size.top = (GX_VALUE)(bottom + 1);
            size.bottom = (GX_VALUE)(size.top + height - 1);

            _gx_widget_resize(child, &size);
            bottom = size.bottom;
            break;
        }

        child->status &= ~GX_STATUS_ACCEPTS_FOCUS;
        child = child->next;
    }

    size = accordion->size;
    size.bottom = (GX_VALUE)(bottom + border_width);

    /* Resize accordion menu. */
    _gx_widget_resize((GX_WIDGET *)accordion, &size);

    /* Return completion status code. */
    return GX_SUCCESS;
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
UINT  _gx_accordion_menu_position(GX_ACCORDION_MENU *accordion)
{
GX_WIDGET    *parent = (GX_WIDGET *)accordion;
GX_WIDGET    *child;
GX_MENU_LIST *child_list;

    child = accordion->first_child;

    /* Reposition items of menu list. */
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
                _gx_widget_detach((GX_WIDGET *)child_list);
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

        while ((child->next == GX_NULL) && (child != parent))
        {
            child = child->parent;

            if (child->type == GX_TYPE_MENU_LIST)
            {
                child_list = (GX_MENU_LIST *)child;
                child = child_list->owner;
            }

            if ((child->type == GX_TYPE_MENU))
            {
                ((GX_MENU*)child)->one_level_position_(0);
            }
            else
            {
                _gx_accordion_menu_one_level_position((GX_ACCORDION_MENU *)child);
            }
        }

        if (child == parent)
        {
            break;
        }

        child = child->next;
    }

    /* Return completion status code. */
    return(GX_SUCCESS);
}

#endif