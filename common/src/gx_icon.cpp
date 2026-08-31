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
/**   Icon Management (Icon)                                              */
/**                                                                       */
/**************************************************************************/

#include "gx_icon.h"

#include "gx_context.h"
#include "gx_canvas.h"
#include "gx_pixelmap.h"

static void gx_icon_draw_(GX_ICON* icon)
{
    icon->draw_();
}

static UINT gx_icon_event_process_(GX_ICON* icon, GX_EVENT* event_ptr)
{
    return icon->event_process_(event_ptr);
}

UINT GX_ICON::create_(const GX_CHAR* name, GX_WIDGET* parent,
                      GX_RESOURCE_ID pixelmap_id,
                      ULONG style, USHORT icon_id, GX_VALUE x, GX_VALUE y)
{
    GX_RECTANGLE size;
    size.set_(x, y, (GX_VALUE)(x + 1), (GX_VALUE)(y + 1));

    GX_WIDGET::create_(name, GX_NULL, style, icon_id, &size);

    this->type = GX_TYPE_ICON;

    this->normal_pixelmap = pixelmap_id;
    this->selected_pixelmap = 0;

    this->draw_function = (void (*)(GX_WIDGET*))gx_icon_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_icon_event_process_;

    if (icon_id > 0)
    {
        status_add_(GX_STATUS_SELECTABLE);
    }

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

UINT GX_ICON::pixelmap_set_(GX_RESOURCE_ID normal_id, GX_RESOURCE_ID selected_id)
{
    this->normal_pixelmap = normal_id;
    this->selected_pixelmap = selected_id;

    if (is_visible_())
    {
        resize_to_pixelmap_();
        dirty_mark_();
    }

    return GX_SUCCESS;
}

void GX_ICON::resize_to_pixelmap_()
{
    GX_PIXELMAP* pixelmap;
    pixelmap_get_(normal_pixelmap, &pixelmap);

    if (pixelmap)
    {
        GX_RECTANGLE new_size;
        new_size.set_(left_(), top_(),
                      (GX_VALUE)(left_() + pixelmap->width - 1),
                      (GX_VALUE)(top_() + pixelmap->height - 1));
        resize_(&new_size);
    }
}

void GX_ICON::background_draw_()
{
    GX_DRAW_CONTEXT* context = gx_context_current_();

    GX_PIXELMAP* pixelmap;

    if (selected_pixelmap && (style & GX_STYLE_DRAW_SELECTED))
    {
        context->pixelmap_get_(selected_pixelmap, &pixelmap);
    }
    else
    {
        context->pixelmap_get_(normal_pixelmap, &pixelmap);
    }

    if (pixelmap)
    {
        pixelmap_draw_(pixelmap);
    }
    else
    {
        if (!(style & GX_STYLE_TRANSPARENT))
        {
            border_and_background_draw_();
        }
    }
}

void GX_ICON::draw_()
{
    background_draw_();
    children_draw_();
}

UINT GX_ICON::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        result = GX_WIDGET::event_process_(event_ptr);
        resize_to_pixelmap_();
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
/*    _gx_icon_background_draw                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the background of specified icon.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    icon                                  Icon widget control block     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_pixelmap_get              Retrieve pixelmap image       */
/*    _gx_canvas_pixelmap_draw              Draw pixelmap                 */
/*    _gx_widget_context_fill_set           Set fill color                */
/*    _gx_widget_background_draw            Retrieve pixelmap image       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_icon_background_draw(GX_ICON* icon)
{
    icon->background_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_icon_create                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a bitmap prompt, which is a special type of   */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    icon                                  Pointer to icon control block */
/*    name                                  Logical name of icon widget   */
/*    parent                                Pointer to the parent widget  */
/*    pixelmap_id                           Resource ID of pixelmap       */
/*    style                                 Style of icon                 */
/*    icon_id                               Application-definedID of icon */
/*    x                                     Starting x-coordinate position*/
/*    y                                     Starting y-coordinate position*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define          Define the icon area          */
/*    _gx_widget_create                     Create the underlying widget  */
/*    _gx_widget_status_add                 Set widget status             */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_icon_create(GX_ICON* icon, const GX_CHAR* name, GX_WIDGET* parent,
                     GX_RESOURCE_ID pixelmap_id, ULONG style, USHORT icon_id,
                     GX_VALUE x, GX_VALUE y)
{
    return icon->create_(name, parent, pixelmap_id, style, icon_id, x, y);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_icon_draw                                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified icon, which is a special type     */
/*      of widget.                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    icon                                  Icon widget control block     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_icon_background_draw              Draw icon background          */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_icon_draw(GX_ICON* icon)
{
    icon->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_icon_event_process                                              */
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
/*    icon                                  Pointer to icon control       */
/*                                            block                       */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_process              Default widget event process  */
/*    _gx_icon_pixelmap_update              Change the pixelmap           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_icon_event_process(GX_ICON* icon, GX_EVENT* event_ptr)
{
    return icon->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_icon_pixelmap_set                                               */
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
/*    icon                                  Pointer to icon control block */
/*    normal_pixelmap_id                    Normal state pixelamp         */
/*                                            resource ID                 */
/*    selected_pixelmap_id                  Selected state pixelmap       */
/*                                            resource ID                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_icon_pixelmap_update              Change the pixelmap           */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_icon_pixelmap_set(GX_ICON* icon,
                           GX_RESOURCE_ID normal_pixelmap_id,
                           GX_RESOURCE_ID selected_pixelmap_id)
{
    return icon->pixelmap_set_(normal_pixelmap_id, selected_pixelmap_id);
}
