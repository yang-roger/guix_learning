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
/**   Button Management (radio_button)                                    */
/**                                                                       */
/**************************************************************************/

#include "gx_radio_button.h"

#include "gx_canvas.h"
#include "gx_context.h"
#include "gx_pixelmap.h"

static void gx_radio_button_draw_(GX_RADIO_BUTTON* button)
{
    button->draw_();
}

UINT GX_RADIO_BUTTON::create_(const GX_CHAR* name, GX_WIDGET* parent,
                              GX_RESOURCE_ID text_id, ULONG style,
                              USHORT radio_button_id, const GX_RECTANGLE* size)
{
    style |= GX_STYLE_BUTTON_RADIO;

    GX_TEXT_BUTTON::create_(name, GX_NULL, text_id, style, radio_button_id, size);

    this->type = GX_TYPE_RADIO_BUTTON;

    this->on_pixelmap_id = GX_PIXELMAP_RADIO_ON_ID;
    this->off_pixelmap_id = GX_PIXELMAP_RADIO_OFF_ID;
    this->on_disabled_pixelmap_id = GX_PIXELMAP_RADIO_ON_ID;
    this->off_disabled_pixelmap_id = GX_PIXELMAP_RADIO_OFF_ID;

    this->draw_function = (void (*)(GX_WIDGET*))gx_radio_button_draw_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

UINT GX_RADIO_BUTTON::pixelmap_set_(GX_RESOURCE_ID off_id,
                                    GX_RESOURCE_ID on_id,
                                    GX_RESOURCE_ID off_disabled_id,
                                    GX_RESOURCE_ID on_disabled_id)
{

    if (off_id)
    {
        this->off_pixelmap_id = off_id;
    }

    if (on_id)
    {
        this->on_pixelmap_id = on_id;
    }

    if (off_disabled_id)
    {
        this->off_disabled_pixelmap_id = off_disabled_id;
    }

    if (on_disabled_id)
    {
        this->on_disabled_pixelmap_id = on_disabled_id;
    }

    dirty_mark_if_visible_();

    return GX_SUCCESS;
}

void GX_RADIO_BUTTON::draw_()
{
    GX_COLOR text_color;
    GX_COLOR fill_color;
    GX_RESOURCE_ID pixelmap_id;

    if (is_enabled_())
    {
        if (is_pushed_())
        {
            text_color = selected_text_color_();
            fill_color = selected_fill_color;

            pixelmap_id = on_pixelmap_id;
        }
        else
        {
            text_color = normal_text_color_();
            fill_color = normal_fill_color;

            pixelmap_id = off_pixelmap_id;
        }
    }
    else
    {
        text_color = disabled_text_color_();
        fill_color = disabled_fill_color;

        if (is_pushed_())
        {
            pixelmap_id = on_disabled_pixelmap_id;
        }
        else
        {
            pixelmap_id = off_disabled_pixelmap_id;
        }
    }

    if (!(this->status & GX_STATUS_TRANSPARENT))
    {
        border_and_background_draw_(GX_COLOR_ID_DEFAULT_BORDER, fill_color, fill_color, GX_TRUE);
    }

    GX_PIXELMAP* pixelmap = GX_NULL;
    if (pixelmap_id)
    {
        _gx_context_pixelmap_get(pixelmap_id, &pixelmap);
    }

    GX_VALUE text_x_offset = 0;

    if (pixelmap)
    {
        GX_VALUE y = (GX_VALUE)(top_() + (height_() - pixelmap->height) / 2);
        GX_VALUE x;

        switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
        {
        case GX_STYLE_TEXT_RIGHT:
            x = (GX_VALUE)(right_() - pixelmap->width + 1);
            text_x_offset = (GX_VALUE)(-(pixelmap->width * 3 / 2));
            break;

        case GX_STYLE_TEXT_CENTER:
            x = (GX_VALUE)(left_() + (width_() - pixelmap->width) / 2);
            text_x_offset = 0;
            break;

        case GX_STYLE_TEXT_LEFT:
        default:
            x = left_();
            text_x_offset = (GX_VALUE)(pixelmap->width * 3 / 2);
            break;
        }

        _gx_canvas_pixelmap_draw(x, y, pixelmap);
    }

    GX_STRING text = { GX_NULL, 0 };
    text_get_ext_(&text);
    text_draw_(&text, text_color, text_x_offset, 0);

    children_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radio_button_create                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service creates a radio button widget.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    name                                  Name of button                */
/*    parent                                Parent widget control block   */
/*    text_id                               text resource id              */
/*    style                                 Style of button               */
/*    radio_button_id                       Application-defined ID of     */
/*                                            radio button                */
/*    size                                  Button size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_button_create                Create text button            */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_radio_button_create(GX_RADIO_BUTTON* button, const GX_CHAR* name, GX_WIDGET* parent,
                             GX_RESOURCE_ID text_id, ULONG style, USHORT radio_button_id,
                             const GX_RECTANGLE* size)
{
    return button->create_(name, parent, text_id, style, radio_button_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radio_button_draw                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws a radio button widget.                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Pointer to radio button       */
/*                                            widget control block        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_pixelmap_get              Retrieve pixelmap image       */
/*    _gx_canvas_pixelmap_draw              Draw the pixelmap             */
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
void _gx_radio_button_draw(GX_RADIO_BUTTON* button)
{
    button->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radio_button_pixelmap_set                                       */
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
/*    off_id                                Resource ID of the pixelmap   */
/*                                            used for drawing button off */
/*    on_id                                 Resource ID of the pixelmap   */
/*                                            used for drawing button on  */
/*    off_disabled_id                       Resource ID of the pixelmap   */
/*                                            used for draw button off    */
/*                                            disabled                    */
/*    on_disabled_id                        Resource ID of the pixelmap   */
/*                                            used for drawing button on  */
/*                                            disabled                    */
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
UINT _gx_radio_button_pixelmap_set(GX_RADIO_BUTTON* button, GX_RESOURCE_ID off_id, GX_RESOURCE_ID on_id,
                                   GX_RESOURCE_ID off_disabled_id, GX_RESOURCE_ID on_disabled_id)
{
    return button->pixelmap_set_(off_id, on_id, off_disabled_id, on_disabled_id);
}
