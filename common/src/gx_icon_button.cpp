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

#include "gx_icon_button.h"

#include "gx_context.h"

static void gx_icon_button_draw_(GX_ICON_BUTTON* button)
{
    button->draw_();
}

UINT GX_ICON_BUTTON::create_(const GX_CHAR* name, GX_WIDGET* parent,
                             GX_RESOURCE_ID icon_id, ULONG style,
                             USHORT icon_button_id, const GX_RECTANGLE* size)
{
    GX_BUTTON::create_(name, GX_NULL, style, icon_button_id, size);

    this->type = GX_TYPE_ICON_BUTTON;

    this->icon_id = icon_id;

    this->draw_function = (void (*)(GX_WIDGET*))gx_icon_button_draw_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

UINT GX_ICON_BUTTON::icon_set_(GX_RESOURCE_ID icon_id)
{
    this->icon_id = icon_id;

    dirty_mark_if_visible_();

    return GX_SUCCESS;
}

void GX_ICON_BUTTON::draw_()
{
    background_draw_();

    GX_PIXELMAP* pixelmap;
    _gx_context_pixelmap_get(icon_id, &pixelmap);
    if (pixelmap)
    {
        INT shift = is_pushed_() ? 1 : 0;
        pixelmap_draw_(pixelmap, border_width_(), shift);
    }

    children_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_icon_button_create                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates an icon button, which is a special type of    */
/*    button.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    name                                  Name of button                */
/*    parent                                Parent widget control block   */
/*    icon_id                               Resource ID of icon           */
/*    style                                 Style of icon                 */
/*    icon_button_id                        Application-definedID of icon */
/*    size                                  Button size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_button_create                     Create the underlying button  */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_icon_button_create(GX_ICON_BUTTON* button, const GX_CHAR* name,
                            GX_WIDGET* parent, GX_RESOURCE_ID icon_id,
                            ULONG style, USHORT icon_button_id, const GX_RECTANGLE* size)
{
    return button->create_(name, parent, icon_id, style, icon_button_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_icon_button_draw                                                */
/*                                                           6.1.9        */
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
/*    button                                Icon button control block     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_button_background_draw            Draw the button background    */
/*    _gx_context_pixelmap_get              Retrieve pixelmap image       */
/*    _gx_widget_border_width_get           Get the border width          */
/*    _gx_canvas_pixelmap_draw              Draw the pixelmap             */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_icon_button_draw(GX_ICON_BUTTON* button)
{
    button->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_icon_button_pixelmap_set                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Change the pixelmap associated with an icon after the icon has      */
/*    been created.                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Pointer to buttonontrol block */
/*    pixelmap_id                           Normal state pixelmap         */
/*                                            resource ID                 */
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
/*    _gx_drop_list_pixelmap_set                                          */
/*                                                                        */
/**************************************************************************/
UINT _gx_icon_button_pixelmap_set(GX_ICON_BUTTON* button, GX_RESOURCE_ID icon_id)
{
    return button->icon_set_(icon_id);
}

