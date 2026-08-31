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
/**   Text Button Management (Button)                                     */
/**                                                                       */
/**************************************************************************/

#include "gx_text_button.h"

static void gx_text_button_draw_(GX_TEXT_BUTTON* button)
{
    button->draw_();
}

static UINT gx_text_button_event_process_(GX_TEXT_BUTTON* button, GX_EVENT* event_ptr)
{
    return button->event_process_(event_ptr);
}

UINT GX_TEXT_BUTTON::create_(const GX_CHAR* name, GX_WIDGET* parent,
                             GX_RESOURCE_ID text_id,
                             ULONG style, USHORT Id,
                             const GX_RECTANGLE* size)
{
    if ((style & GX_STYLE_TEXT_ALIGNMENT_MASK) == 0)
    {
        style |= GX_STYLE_TEXT_CENTER;
    }

    GX_BUTTON::create_(name, GX_NULL, style, Id, size);

    this->type = GX_TYPE_TEXT_BUTTON;

    this->text_data.create_(this, text_id, GX_FONT_ID_BUTTON);

    this->normal_text_color_()   = GX_COLOR_ID_BUTTON_TEXT;
    this->selected_text_color_() = GX_COLOR_ID_BUTTON_TEXT;
    this->disabled_text_color_() = GX_COLOR_ID_DISABLED_TEXT;
    this->disabled_fill_color    = GX_COLOR_ID_DISABLED_TEXT;

    this->draw_function = (void (*)(GX_WIDGET*))gx_text_button_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_text_button_event_process_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

GX_RESOURCE_ID GX_TEXT_BUTTON::text_draw_info_get_(INT* x_offset, INT* y_offset) const
{
    *x_offset = 0;
    *y_offset = 0;

    if (style & GX_STYLE_ENABLED)
    {
        if (style & GX_STYLE_BUTTON_PUSHED)
        {
            *x_offset = 1;
            *y_offset = 1;

            return text_data.selected_color;
        }
        else
        {
            return text_data.normal_color;
        }
    }
    else
    {
        return text_data.disabled_color;
    }
}

void GX_TEXT_BUTTON::text_draw_()
{
    INT x_offset;
    INT y_offset;
    GX_RESOURCE_ID color_id = text_draw_info_get_(&x_offset, &y_offset);

    GX_STRING text = { GX_NULL, 0 };
    text_get_ext_(&text);

    text_draw_(&text, color_id, x_offset, y_offset);
}

void GX_TEXT_BUTTON::draw_()
{
    border_and_background_draw_();
    text_draw_();
    children_draw_();

    if (!(style & GX_STYLE_ENABLED))
    {
        monochrome_driver_disabled_line_draw_();
    }
}

UINT GX_TEXT_BUTTON::event_process_(GX_EVENT* event_ptr)
{
    switch (event_ptr->type)
    {
    case GX_EVENT_DELETE:
        text_data.string_delete_();
        break;

    default:
        break;
    }

    return GX_BUTTON::event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_button_create                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a text button, which is a special type of     */
/*    button (widget).                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    name                                  Name of button                */
/*    parent                                Parent widget control block   */
/*    text_id                               text resource id              */
/*    style                                 Style of button               */
/*    size                                  Button size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memset                                Set control block to zero     */
/*    _gx_button_create                     Create the underlying button  */
/*    _gx_widget_link                       Link the button to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_button_create(GX_TEXT_BUTTON* button, const GX_CHAR* name, GX_WIDGET* parent,
                            GX_RESOURCE_ID text_id, ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    return button->create_(name, parent, text_id, style, Id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_button_draw                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified text button, which is a special   */
/*    type of widget.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Text button control block     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_button_background_draw            Draw button background        */
/*    _gx_widget_text_draw                  Draw text string              */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_text_button_draw(GX_TEXT_BUTTON* button)
{
    button->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_button_event_process                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes events for the specified text button.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Pointer to text button        */
/*                                            control block               */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_bidi_resolved_text_info_delete                          */
/*                                          Delete dynamic bidi text      */
/*    _gx_button_event_process              Default widget event process  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_button_event_process(GX_TEXT_BUTTON* button, GX_EVENT* event_ptr)
{
    return button->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_button_font_set                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns a font to the specified button.               */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    font_id                               Resource ID for the font      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 <ark this prompt as dirty     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_button_font_set(GX_TEXT_BUTTON* button, GX_RESOURCE_ID font_id)
{
    button->font_set_(font_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_button_color_set                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns the color used to draw text                   */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    normal_text_color_id                  Resource ID of normal text    */
/*    selected_text_color_id                Resource ID of selected text  */
/*    disabled_text_color_id                Resource ID of disabled text  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark this prompt as dirty     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_button_text_color_set(GX_TEXT_BUTTON* button,
                                    GX_RESOURCE_ID normal_text_color_id,
                                    GX_RESOURCE_ID selected_text_color_id,
                                    GX_RESOURCE_ID disabled_text_color_id)
{
    button->text_color_set_(normal_text_color_id,
                            selected_text_color_id,
                            disabled_text_color_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_button_text_draw                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the button text                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Text button control block     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_text_id_draw               Draw text using resource ID   */
/*    _gx_widget_text_draw                  Draw text string              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_text_button_text_draw(GX_TEXT_BUTTON* button)
{
    button->text_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_button_text_get_ext                                        */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns a string literal to the specified prompt      */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    return_text                           String used in the button     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_string_get_ext             Get the string based on ID    */
/*    _gx_system_private_string_get         Get private string copy       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_button_text_get_ext(GX_TEXT_BUTTON* button, GX_STRING* return_text)
{
    return button->text_get_ext_(return_text);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_button_text_id_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns a text ID to the specified button.            */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    text_id                               Resource ID of the text string*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark this prompt as dirty     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Cdoe                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_button_text_id_set(GX_TEXT_BUTTON* button, GX_RESOURCE_ID text_id)
{
    button->text_id_set_(text_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_button_text_set_ext                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns a string literal to the specified prompt.     */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    text                                  pointer to text string        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark this button as dirty     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_button_text_set_ext(GX_TEXT_BUTTON* button, const GX_STRING* string)
{
    return button->text_set_ext_(string);
}


