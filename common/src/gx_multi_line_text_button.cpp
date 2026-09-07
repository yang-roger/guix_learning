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

#include "gx_multi_line_text_button.h"

#include "gx_system.h"
#include "gx_canvas.h"
#include "gx_font.h"

static void gx_multi_line_text_button_draw_(GX_MULTI_LINE_TEXT_BUTTON* button)
{
    button->draw_();
}

static UINT gx_multi_line_text_button_event_process_(GX_MULTI_LINE_TEXT_BUTTON* button, GX_EVENT* event_ptr)
{
    return button->event_process_(event_ptr);
}

UINT GX_MULTI_LINE_TEXT_BUTTON::create_(const GX_CHAR* name,
                                        GX_WIDGET* parent,
                                        GX_RESOURCE_ID text_id,
                                        ULONG style, USHORT Id,
                                        const GX_RECTANGLE* size)
{
    GX_TEXT_BUTTON::create_(name, GX_NULL, text_id, style, Id, size);

    this->type = GX_TYPE_MULTI_LINE_TEXT_BUTTON;

    this->draw_function = (void (*)(GX_WIDGET*))gx_multi_line_text_button_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_multi_line_text_button_event_process_;

    lines_clear_();

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_MULTI_LINE_TEXT_BUTTON::lines_clear_()
{
    line_count = 0;
    for (INT i = 0; i < GX_MULTI_LINE_TEXT_BUTTON_MAX_LINES; ++i)
    {
        lines[i].ptr = GX_NULL;
        lines[i].length = 0;
    }
}

UINT GX_MULTI_LINE_TEXT_BUTTON::text_set_ext_(const GX_STRING* string)
{
    UINT result = GX_TEXT_BUTTON::text_set_ext_(string);
    if (result == GX_SUCCESS)
    {
        lines_set_();
    }

    return result;
}

void GX_MULTI_LINE_TEXT_BUTTON::text_id_set_(GX_RESOURCE_ID text_id)
{
    GX_TEXT_BUTTON::text_id_set_(text_id);
    lines_set_();
}

void GX_MULTI_LINE_TEXT_BUTTON::lines_set_()
{
    lines_clear_();

    line_count = 1;

    GX_STRING string;
    text_get_ext_(&string);

    if (!string.ptr)
    {
        return;
    }

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)

    if (_gx_system_bidi_text_enabled)
    {
        INT line_index = 0;

        GX_BIDI_RESOLVED_TEXT_INFO* info = text_data.bidi_resolved_text_info;
        while (info && line_index < GX_MULTI_LINE_TEXT_BUTTON_MAX_LINES)
        {
            lines[line_index++] = *info->text;

            info = info->next;
        }

        if (line_index > 0)
        {
            line_count = line_index;
        }
    }
    else
    {
        lines_add_(string.ptr);
    }

#else // !GX_DYNAMIC_BIDI_TEXT_SUPPORT

    lines_add_(string.ptr);

#endif // !GX_DYNAMIC_BIDI_TEXT_SUPPORT
}

void GX_MULTI_LINE_TEXT_BUTTON::lines_add_(const GX_CHAR* text)
{
    INT line_index = 0;
    GX_CHAR ch;

    while ((ch = *text) != 0)
    {
        if ((ch == '\r') || (ch == '\n'))
        {
            if ((ch == '\r') && (text[1] == '\n'))
            {
                text += 2;
            }
            else
            {
                text += 1;
            }

            ++line_index;
            if (line_index >= GX_MULTI_LINE_TEXT_BUTTON_MAX_LINES)
            {
                break;
            }
            else
            {
                ++line_count;
            }
        }
        else
        {
            if (lines[line_index].ptr == GX_NULL)
            {
                lines[line_index].ptr = text;
            }
            ++lines[line_index].length;

            ++text;
        }
    }
}

void GX_MULTI_LINE_TEXT_BUTTON::text_draw_()
{
    GX_DRAW_CONTEXT* context = gx_context_current_();

    GX_FONT* font;
    context->font_get_(font_id_(), &font);
    if (font)
    {
        context->brush.font = font;

        INT x_offset;
        INT y_offset;
        GX_RESOURCE_ID color_id = text_draw_info_get_(&x_offset, &y_offset);

        context->line_color_set_(color_id);

        INT y = top_() + (height_() - line_count * font->line_height) / 2;
        INT x;

        for (INT line_index = 0; line_index < line_count; ++line_index)
        {
            GX_STRING* line_text = &lines[line_index];
            if (line_text->ptr)
            {
                x = text_x_pos_(line_text, font);

                _gx_canvas_text_draw_ext((GX_VALUE)(x + x_offset), (GX_VALUE)(y + y_offset), line_text);
            }

            y += font->line_height;
        }
    }
}

void GX_MULTI_LINE_TEXT_BUTTON::draw_()
{
    border_and_background_draw_();
    text_draw_();
    children_draw_();

    if (!(style & GX_STYLE_ENABLED))
    {
        monochrome_driver_disabled_line_draw_();
    }
}

UINT GX_MULTI_LINE_TEXT_BUTTON::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
    case GX_EVENT_LANGUAGE_CHANGE:
        result = GX_TEXT_BUTTON::event_process_(event_ptr);
        lines_set_();
        break;

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    case GX_EVENT_DYNAMIC_BIDI_TEXT_ENABLE:
    case GX_EVENT_DYNAMIC_BIDI_TEXT_DISABLE:
        lines_set_();
        break;
#endif

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
/*    _gx_multi_line_text_button_create                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a multi-line text button, which is a special  */
/*    type of button (widget).                                            */
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
UINT _gx_multi_line_text_button_create(GX_MULTI_LINE_TEXT_BUTTON* button, const GX_CHAR* name,
                                       GX_WIDGET* parent, GX_RESOURCE_ID text_id,
                                       ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    return button->create_(name, parent, text_id, style, Id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_button_draw                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified multi-line text button, which is  */
/*    special type of button (widget).                                    */
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
/*   _gx_button_background_draw             Draw button background        */
/*   _gx_multi_line_text_button_text_draw   Draw multi-line button text   */
/*   _gx_widget_children_draw               Draw children widgets         */
/*   _gx_monochrome_driver_disabled_button_line_draw                      */
/*                                          Draw a line for disabled      */
/*                                            monochrome button           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    System and Application Code                                         */
/*                                                                        */
/**************************************************************************/
void _gx_multi_line_text_button_draw(GX_MULTI_LINE_TEXT_BUTTON* button)
{
    button->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_button_event_process                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a multi-line text button, which is a special  */
/*    type of button (widget).                                            */
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
/*    _gx_text_button_event_process                                       */
/*    _gx_multi_line_text_button_line_pointers_set                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    System and Application Code                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_button_event_process(GX_MULTI_LINE_TEXT_BUTTON* button, GX_EVENT* event_ptr)
{
    return button->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_button_text_draw                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the text of a multi-line text button.           */
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
/*   _gx_widget_font_get                    Get the font                  */
/*   _gx_widget_border_width_get            Get border width              */
/*   _gx_context_font_set                   Set font                      */
/*   _gx_context_line_color_set             Set line color                */
/*   _gx_system_string_width_get            Get the width of the string   */
/*   _gx_canvas_text_draw                   Draw text string              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    System and Application Code                                         */
/*                                                                        */
/**************************************************************************/
void _gx_multi_line_text_button_text_draw(GX_MULTI_LINE_TEXT_BUTTON* button)
{
    button->text_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_button_text_id_set                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This assigns text to a multi line text button                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    text_id                               text resource id              */
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
UINT _gx_multi_line_text_button_text_id_set(GX_MULTI_LINE_TEXT_BUTTON* button, GX_RESOURCE_ID string_id)
{
    button->text_id_set_(string_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_button_text_set_ext                             */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION (Deprecated)                                              */
/*                                                                        */
/*    This function assigns text to a multi-line text button              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    text                                  text pointer                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_button_text_set_ext                                        */
/*    _gx_multi_line_text_button_line_pointers_set                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_button_text_set_ext(GX_MULTI_LINE_TEXT_BUTTON* button, const GX_STRING* text)
{
    return button->text_set_ext_(text);
}
