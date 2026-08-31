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
/**   Button Management (checkbox)                                        */
/**                                                                       */
/**************************************************************************/

#include "gx_checkbox.h"

static void gx_checkbox_draw_(GX_CHECKBOX* checkbox)
{
    checkbox->draw_();
}

static UINT gx_checkbox_event_process_(GX_CHECKBOX* checkbox, GX_EVENT* event_ptr)
{
    return checkbox->event_process_(event_ptr);
}

static void gx_checkbox_select_(GX_CHECKBOX* checkbox)
{
    checkbox->select_();
}

UINT GX_CHECKBOX::create_(const GX_CHAR* name, GX_WIDGET* parent,
                          GX_RESOURCE_ID text_id, ULONG style, USHORT checkbox_id,
                          const GX_RECTANGLE* size)
{
    style |= GX_STYLE_BUTTON_TOGGLE;

    GX_TEXT_BUTTON::create_(name, GX_NULL, text_id, style, checkbox_id, size);

    this->type = GX_TYPE_CHECKBOX;

    this->on_pixelmap_id = GX_PIXELMAP_CHECKBOX_ON_ID;
    this->off_pixelmap_id = GX_PIXELMAP_CHECKBOX_OFF_ID;

    this->draw_function = (void (*)(GX_WIDGET*))gx_checkbox_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_checkbox_event_process_;
    this->select_handler = (void (*)(GX_BUTTON*))gx_checkbox_select_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_CHECKBOX::select_()
{
    if (is_enabled_())
    {
        if (!is_pushed_())
        {
            set_pushed_();
            signal_parent_(GX_EVENT_TOGGLE_ON, 0);
        }
        else
        {
            clear_pushed_();
            signal_parent_(GX_EVENT_TOGGLE_OFF, 0);
        }

        dirty_mark_();
    }
}

UINT GX_CHECKBOX::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_PEN_DOWN:
        select_handler(this);
        result = parent_event_process_run_(event_ptr);
        break;

    default:
        result = GX_TEXT_BUTTON::event_process_(event_ptr);
        break;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_checkbox_create                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a checkbox, which is a special type of        */
/*    button (widget).                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    checkbox                              Checkbox control block        */
/*    name                                  Name of checkbox              */
/*    parent                                Parent widget control block   */
/*    text_id                               Text resource id              */
/*    style                                 Style of checkbox             */
/*    checkbox_id                           Checkbox id                   */
/*    size                                  Checkbox size                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_button_create                Create the underlying button  */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_checkbox_create(GX_CHECKBOX* checkbox, const GX_CHAR* name, GX_WIDGET* parent,
                         GX_RESOURCE_ID text_id, ULONG style, USHORT checkbox_id,
                         const GX_RECTANGLE* size)
{
    return checkbox->create_(name, parent, text_id, style, checkbox_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_checkbox_draw                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified button, which is a special type   */
/*    of widget.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Checkbox control block        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_pixelmap_get              Retrieve pixelmap image       */
/*    _gx_widget_height_get                 Gets the height of the widget */
/*    _gx_canvas_pixelmap_draw              Draw the pixelmap image       */
/*    _gx_widget_text_id_draw               Draw the text based on ID     */
/*    _gx_widget_text_draw                  Draw the text string          */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_checkbox_draw(GX_CHECKBOX* checkbox)
{
    checkbox->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_checkbox_event_process                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes events for the specified button.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    checkbox                              Checkbox control block        */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [_gx_button_select_handler]           Button select callback        */
/*    _gx_text_button_event_process         Default widget event process  */
/*    _gx_widget_event_to_parent            Pass event to parent          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_checkbox_event_process(GX_CHECKBOX* checkbox, GX_EVENT* event_ptr)
{
    return checkbox->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_checkbox_pixelmap_set                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a pixelmap button, which is a special type of */
/*    button.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    checkbox                              Checkbox control block        */
/*    unchecked_id                          Resource ID of the pixelmap   */
/*                                            used for unchecked box      */
/*    checked_id                            Resource ID of the pixelmap   */
/*                                            used for checked box        */
/*    unchecked_disabled_id                 Resource ID of the pixelmap   */
/*                                            used for unchecked disabled */
/*                                            box                         */
/*    checked_disabled_id                   Resource ID of the pixelmap   */
/*                                            used for checked disabled   */
/*                                            box                         */
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
/*                                                                        */
/**************************************************************************/
UINT _gx_checkbox_pixelmap_set(GX_CHECKBOX* checkbox,
                               GX_RESOURCE_ID unchecked_id,
                               GX_RESOURCE_ID checked_id,
                               GX_RESOURCE_ID unchecked_disabled_id,
                               GX_RESOURCE_ID checked_disabled_id)
{
    return checkbox->pixelmap_set_(unchecked_id,
                                   checked_id,
                                   unchecked_disabled_id,
                                   checked_disabled_id);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_checkbox_select                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deselects one button, invalidating and eventing as    */
/*    necessary.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    checkbox                              Checkbox control block        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_generate             Generate event to notify      */
/*                                             parent widget              */
/*    _gx_system_dirty_mark                 Set the dirty flag            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_checkbox_select(GX_CHECKBOX* checkbox)
{
    checkbox->select_();

    return GX_SUCCESS;
}
