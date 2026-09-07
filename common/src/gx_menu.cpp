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
/**   Menu Management (Menu)                                              */
/**                                                                       */
/**************************************************************************/

#include "gx_menu.h"

static void gx_menu_draw_(GX_MENU* menu)
{
    menu->draw_();
}

static UINT gx_menu_event_process_(GX_MENU* menu, GX_EVENT* event_ptr)
{
    return menu->event_process_(event_ptr);
}

UINT GX_MENU::create_(const GX_CHAR* name, GX_WIDGET* parent,
                      GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                      ULONG style, USHORT menu_id, const GX_RECTANGLE* size)
{
    GX_PIXELMAP_PROMPT::create_(name, GX_NULL, text_id, fill_id, style, menu_id, size);

    this->type = GX_TYPE_MENU;

    this->text_x_offset = 10;
    this->text_y_offset = 0;

    this->list_total_count = 0;

    this->style |= GX_STYLE_ENABLED;

    if (!(this->style & GX_STYLE_TEXT_ALIGNMENT_MASK))
    {
        this->style |= GX_STYLE_TEXT_LEFT;
    }

    this->draw_function = (void (*)(GX_WIDGET*))gx_menu_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_menu_event_process_;

    this->list.create_("menu_list", GX_NULL, GX_STYLE_TRANSPARENT, menu_id, size);
    this->list.type = GX_TYPE_MENU_LIST;
    this->list.owner = this;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_MENU::insert_(GX_WIDGET* widget)
{
    widget->status_remove_(GX_STATUS_ACCEPTS_FOCUS);

    widget->attach_to_(&list);

    ++list_total_count;
}

void GX_MENU::remove_(GX_WIDGET* widget)
{
    widget->detach_();

    --list_total_count;
}

void GX_MENU::text_offset_set_(GX_VALUE x_offset, GX_VALUE y_offset)
{
    this->text_x_offset = x_offset;
    this->text_y_offset = y_offset;

    dirty_mark_if_visible_();
}

void GX_MENU::position_(GX_VALUE indentation)
{
    GX_MENU_LIST* child_list;

    GX_WIDGET* child = list.first_child;
    while (child)
    {
        if (child->type == GX_TYPE_MENU)
        {
            child_list = &((GX_MENU*)child)->list;

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

        while ((child->next == GX_NULL) && (child != this))
        {
            // child's parent is menu list
            child_list = (GX_MENU_LIST*)child->parent;
            child = child_list->owner;

            ((GX_MENU*)child)->one_level_position_(indentation);
        }

        if (child == this)
        {
            break;
        }

        child = child->next;
    }
}

void GX_MENU::one_level_position_(GX_VALUE indentation)
{
    GX_RECTANGLE new_size;

    new_size.left = (GX_VALUE)(left_() + indentation);
    new_size.right = (GX_VALUE)(right_());
    new_size.top = (GX_VALUE)(bottom_() + 1);
    new_size.bottom = bottom_();

    list.resize_(&new_size);

    if (this->style & GX_STYLE_MENU_EXPANDED)
    {
        if (!list.parent)
        {
            list.link_to_(this->parent);
        }
    }
    else
    {
        if (list.parent)
        {
            list.detach_();
        }
    }

    INT max_right = 0;

    for (GX_WIDGET* item = list.first_child; item; item = item->next)
    {
        if (item->type == GX_TYPE_MENU_LIST)
        {
            continue;
        }

        new_size.top = (GX_VALUE)(new_size.bottom + 1);
        new_size.bottom = (GX_VALUE)(new_size.top + item->height_() - 1);
        new_size.right = (GX_VALUE)(new_size.left + item->width_() - 1);

        if (new_size.right > max_right)
        {
            max_right = new_size.right;
        }

        item->resize_(&new_size);

        if (item->type == GX_TYPE_MENU)
        {
            GX_MENU_LIST* item_list = &((GX_MENU*)item)->list;
            if (item_list->parent)
            {
                item_list->shift_((GX_VALUE)(new_size.left + indentation - item_list->left_()),
                                  (GX_VALUE)(new_size.bottom + 1 - item_list->top_()), GX_FALSE);

                new_size.bottom = item_list->bottom_();

                if (item_list->right_() > max_right)
                {
                    max_right = item_list->right_();
                }
            }
        }
    }

    if (this->style & GX_STYLE_MENU_EXPANDED)
    {
        new_size.top = (GX_VALUE)(bottom_() + 1);
        new_size.right = (GX_VALUE)max_right;

        list.resize_(&new_size);
    }
}

void GX_MENU::text_draw_()
{
    GX_RESOURCE_ID color_id = enabled_text_color_();
    GX_PROMPT::text_draw_(color_id, text_x_offset, text_y_offset);
}

void GX_MENU::draw_()
{
    GX_PIXELMAP_PROMPT::background_draw_();
    text_draw_();
    children_draw_();
}

UINT GX_MENU::event_process_(GX_EVENT* event_ptr)
{
    switch (event_ptr->type)
    {
    case GX_EVENT_DELETE:
        if (list.first_child && !list.parent)
        {
            list.delete_();
        }
        break;

    default:
        break;
    }

    return GX_PROMPT::event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_menu_create                                                     */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates an menu widget.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to the menu control   */
/*                                            block                       */
/*    name                                  Name of the menu              */
/*    parent                                Parent control block          */
/*    text_id                               String id of menu text        */
/*    fill_id                               Pixelmap id for fill area     */
/*    style                                 Style of the widget           */
/*    menu_id                               Application-defined ID of     */
/*                                          the menu                      */
/*    size                                  Menu size                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_pixelmap_prompt_create            Create a pixelmap prompt      */
/*    _gx_widget_create                     Create a widget               */
/*    _gx_widget_link                       Link a child widget to its    */
/*                                            parent                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_menu_create(GX_MENU* menu, const GX_CHAR* name, GX_WIDGET* parent,
                     GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                     ULONG style, USHORT menu_id, const GX_RECTANGLE* size)
{
    return menu->create_(name, parent, text_id, fill_id, style, menu_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_menu_draw                                                       */
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
/*    menu                                  Pointer the to menu control   */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_pixelmap_prompt_background_draw   Draw pixelmap prompt backgrond*/
/*    _gx_menu_text_draw                    Draw menu text                */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_menu_draw(GX_MENU* menu)
{
    menu->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_menu_event_process                                              */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes events for the specified menu.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to menu control       */
/*                                            block                       */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_delete                     Delete a widget               */
/*    _gx_prompt_event_process              Default prompt event process  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_menu_event_process(GX_MENU* menu, GX_EVENT* event_ptr)
{
    return menu->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_menu_insert                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function inserts a new item to the menu.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to menu control block */
/*    widget                                Widget to insert              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_attach                     Attach a widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_menu_insert(GX_MENU* menu, GX_WIDGET* widget)
{
    menu->insert_(widget);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_menu_remove                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function removes a item from the menu.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to menu control block */
/*    widget                                Widget to remove              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_detach                     Detach a widget from its      */
/*                                            parent                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_menu_remove(GX_MENU* menu, GX_WIDGET* widget)
{
    menu->remove_(widget);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_menu_text_offset_set                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets x, y display offset for menu text.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to menu control block */
/*    x_offset                              X coordinate of offset        */
/*    y_offset                              Y coordinate of offset        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark widget area as dirty     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_menu_text_offset_set(GX_MENU* menu, GX_VALUE x_offset, GX_VALUE y_offset)
{
    menu->text_offset_set_(x_offset, y_offset);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_menu_text_draw                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the text of a menu widget.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to menu control block */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_text_draw                  Draw text to a widget         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_menu_text_draw(GX_MENU* menu)
{
    menu->text_draw_();
}
