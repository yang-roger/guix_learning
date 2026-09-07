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
/**   Button Management (Button)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_pixelmap_button.h"

#include "gx_context.h"

static void gx_pixelmap_button_draw_(GX_PIXELMAP_BUTTON* button)
{
    button->draw_();
}

static UINT gx_pixelmap_button_event_process_(GX_PIXELMAP_BUTTON* button, GX_EVENT* event_ptr)
{
    return button->event_process_(event_ptr);
}

UINT GX_PIXELMAP_BUTTON::create_(const GX_CHAR* name,
                                 GX_WIDGET* parent,
                                 GX_RESOURCE_ID normal_id,
                                 GX_RESOURCE_ID selected_id,
                                 GX_RESOURCE_ID disabled_id,
                                 ULONG style,
                                 USHORT pixelmap_button_id,
                                 const GX_RECTANGLE* size)
{
    GX_BUTTON::create_(name, GX_NULL, style, pixelmap_button_id, size);

    this->type = GX_TYPE_PIXELMAP_BUTTON;

    this->normal_id = normal_id;
    this->selected_id = selected_id;
    this->disabled_id = disabled_id;

    this->draw_function = (void (*)(GX_WIDGET*))gx_pixelmap_button_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_pixelmap_button_event_process_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_PIXELMAP_BUTTON::pixelmap_set_(GX_RESOURCE_ID normal_id, GX_RESOURCE_ID selected_id, GX_RESOURCE_ID disabled_id)
{
    this->normal_id = normal_id;
    this->selected_id = selected_id;
    this->disabled_id = disabled_id;

    if (is_visible_())
    {
        transparent_detect_();
        dirty_mark_();
    }
}

void GX_PIXELMAP_BUTTON::transparent_detect_()
{
    if (!(status & GX_STATUS_TRANSPARENT))
    {
        if (pixelmap_is_transparent_(normal_id) ||
            pixelmap_is_transparent_(selected_id) ||
            pixelmap_is_transparent_(disabled_id))
        {
            status_add_(GX_STATUS_TRANSPARENT);
        }
    }
}

void GX_PIXELMAP_BUTTON::draw_()
{
    GX_RESOURCE_ID pixelmap_id = normal_id;

    if (is_enabled_())
    {
        if (is_pushed_())
        {
            if (selected_id)
            {
                pixelmap_id = selected_id;
            }
        }
    }
    else
    {
        if (disabled_id)
        {
            pixelmap_id = disabled_id;
        }
    }

    GX_PIXELMAP* pixelmap;
    _gx_context_pixelmap_get(pixelmap_id, &pixelmap);
    if (pixelmap)
    {
        pixelmap_draw_(pixelmap);
    }
    else
    {
        if (!(style & GX_STYLE_TRANSPARENT))
        {
            GX_BUTTON::draw_();
        }
    }

    children_draw_();
}

UINT GX_PIXELMAP_BUTTON::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        result = GX_BUTTON::event_process_(event_ptr);
        transparent_detect_();
        break;

    default:
        result = GX_BUTTON::event_process_(event_ptr);
        break;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_pixelmap_button_create                                          */
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
/*    button                                Button control block          */
/*    name                                  Name of button                */
/*    parent                                Parent widget control block   */
/*    normal_id                             Normal state pixelmap id      */
/*    selected_id                           Selected state pixelmap id    */
/*    disabled_id                           Disabled state pixelmap id    */
/*    style                                 Style of button               */
/*    pixelmap_button_id                    Application-defined ID of     */
/*                                            the pixelmap button         */
/*    size                                  Button size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_button_create                     Create the underlying button  */
/*    _gx_pixelmap_transparent_detect       Detect whether or not a       */
/*                                            pixelmap is transparent     */
/*    _gx_widget_status_add                 Set the widget status flag    */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_pixelmap_button_create(GX_PIXELMAP_BUTTON* button,
                                const GX_CHAR* name,
                                GX_WIDGET* parent,
                                GX_RESOURCE_ID normal_id,
                                GX_RESOURCE_ID selected_id,
                                GX_RESOURCE_ID disabled_id,
                                ULONG style,
                                USHORT pixelmap_button_id,
                                const GX_RECTANGLE* size)
{
    return button->create_(name, parent, normal_id, selected_id, disabled_id,
                           style, pixelmap_button_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_pixelmap_button_draw                                            */
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
/*    button                                Button control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_pixelmap_get              Retrieve pixelmap image       */
/*    _gx_canvas_pixelmap_draw              Draw the pixelmap             */
/*    _gx_button_draw                       Draw the button               */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_pixelmap_button_draw(GX_PIXELMAP_BUTTON* button)
{
    button->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_pixelmap_button_event_process                                   */
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
/*    button                                Pointer to button control     */
/*                                            block                       */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_button_event_process              Process button events.        */
/*    _gx_pixelmap_button_transparent_detect                              */
/*                                          Detect whether button         */
/*                                            pixelmaps are transparent   */
/*                                            handler routine             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_pixelmap_button_event_process(GX_PIXELMAP_BUTTON* button, GX_EVENT* event_ptr)
{
    return button->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_pixelmap_button_pixelmap_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets a  pixelmap to the pixelmap button.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    normal_id                             normal state pixelmap id      */
/*    selected_id                           selected state pixelmap id    */
/*    disabled_id                           disabled state pixelmap id    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_pixelmap_button_transparent_detect                              */
/*                                          Detect the transparency       */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_pixelmap_button_pixelmap_set(GX_PIXELMAP_BUTTON* button,
                                      GX_RESOURCE_ID normal_id,
                                      GX_RESOURCE_ID selected_id,
                                      GX_RESOURCE_ID disabled_id)
{
    button->pixelmap_set_(normal_id, selected_id, disabled_id);

    return GX_SUCCESS;
}

