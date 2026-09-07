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
/**   Multi Line Text View Management (Multi Line Text View)              */
/**                                                                       */
/**************************************************************************/

#include "gx_multi_line_text_view.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_display.h"
#include "gx_canvas.h"
#include "gx_font.h"
#include "gx_scroll_bar.h"
#include "gx_text_area_info.h"

static void gx_multi_line_text_view_draw_(GX_MULTI_LINE_TEXT_VIEW* view)
{
    view->draw_();
}

static UINT gx_multi_line_text_view_event_process_(GX_MULTI_LINE_TEXT_VIEW* view, GX_EVENT* event_ptr)
{
    return view->event_process_(event_ptr);
}

static void gx_multi_line_text_view_scroll_info_get_(GX_MULTI_LINE_TEXT_VIEW* view, ULONG style, GX_SCROLL_INFO* info)
{
    view->scroll_info_get_(style, info);
}

UINT GX_MULTI_LINE_TEXT_VIEW::create_(const GX_CHAR* name, GX_WIDGET* parent,
                                      GX_RESOURCE_ID text_id, ULONG style,
                                      USHORT Id, const GX_RECTANGLE* size)
{

    GX_WINDOW::create_(name, GX_NULL, style, Id, size);

    this->type = GX_TYPE_MULTI_LINE_TEXT_VIEW;

    this->text_data.create_(this, text_id, GX_FONT_ID_TEXT_INPUT);
    this->normal_text_color_() = GX_COLOR_ID_TEXT_INPUT_TEXT;
    this->selected_text_color_() = GX_COLOR_ID_TEXT_INPUT_TEXT;
    this->disabled_text_color_() = GX_COLOR_ID_DISABLED_TEXT;
    this->normal_fill_color = GX_COLOR_ID_TEXT_INPUT_FILL;

    this->text_scroll_shift = 0;
    this->text_total_lines = 0;
    this->text_visible_lines = 0;
    this->margin = 0;
    this->line_space = 0;
    this->line_index_cache_old = GX_TRUE;
    this->first_cache_line = 0;
    this->line_index_cache_size = 0;
    memset(this->line_index_cache, 0, GX_MULTI_LINE_INDEX_CACHE_SIZE * sizeof(UINT));

    this->whitespace_width = whitespace_width_();

    this->draw_function = (void (*)(GX_WIDGET*))gx_multi_line_text_view_draw_;
    this->event_process_function = (UINT(*)(GX_WIDGET*, GX_EVENT*))gx_multi_line_text_view_event_process_;
    this->scroll_info_get = (void (*)(GX_WINDOW*, ULONG, GX_SCROLL_INFO*))gx_multi_line_text_view_scroll_info_get_;

    this->status |= GX_STATUS_RESIZE_NOTIFY;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_MULTI_LINE_TEXT_VIEW::text_id_set_(GX_RESOURCE_ID text_id)
{
    text_data.text_id_set_(text_id);

    string_get_ext_(text_id, &text_data.string);
    text_data.string.ptr = GX_NULL;

    line_index_cache_old = GX_TRUE;
}

UINT GX_MULTI_LINE_TEXT_VIEW::text_set_ext_(const GX_STRING* text)
{
    UINT result = text_data.string_ext_set_(text);

    line_index_cache_old = GX_TRUE;

    return result;
}

void GX_MULTI_LINE_TEXT_VIEW::font_set_(GX_RESOURCE_ID font_id)
{
    text_data.font_set_(font_id);

    whitespace_width = whitespace_width_();

    line_index_cache_old = GX_TRUE;
}

void GX_MULTI_LINE_TEXT_VIEW::line_space_set_(GX_BYTE line_space)
{
    this->line_space = line_space;

    line_index_cache_old = GX_TRUE;

    if (is_visible_())
    {
        client_updated_();
    }
}

void GX_MULTI_LINE_TEXT_VIEW::margin_set_(GX_UBYTE margin)
{
    this->margin = margin;

    line_index_cache_old = GX_TRUE;

    if (is_visible_())
    {
        client_updated_();
    }
}

void GX_MULTI_LINE_TEXT_VIEW::client_updated_() const
{
    GX_EVENT e;
    memset(&e, 0, sizeof(GX_EVENT));
    e.type = GX_EVENT_CLIENT_UPDATED;
    e.target = (GX_WIDGET*)this;
    _gx_system_event_fold(&e);
}

UINT GX_MULTI_LINE_TEXT_VIEW::scroll_info_get_(ULONG style, GX_SCROLL_INFO* info)
{
    GX_PARAMETER_NOT_USED(style);

    GX_FONT* font;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return GX_FAILURE;
    }

    if (line_index_cache_old)
    {
        total_lines_compute_();
    }

    GX_VALUE line_height = (GX_VALUE)(font->line_height + line_space);
    INT text_height = (INT)(line_height * text_total_lines);

    GX_RECTANGLE text_area;
    display_area_(&text_area);

    info->visible = text_area.height_();
    info->minimum = text_area.top;
    info->maximum = info->minimum + GX_MAX(text_height, info->visible) - 1;

    INT value = text_area.top - text_scroll_shift;
    if (value < info->minimum)
    {
        value = info->minimum;
    }
    else if (value > info->maximum - info->visible + 1)
    {
        value = info->maximum - info->visible + 1;
    }

    INT shift = text_area.top - value;
    if (shift != text_scroll_shift)
    {
        text_scroll_shift = shift;
    }

    info->value = value;
    info->increment = line_height;

    if (text_total_lines > line_index_cache_size)
    {
        line_cache_update_();
    }

    return GX_SUCCESS;
}

bool GX_MULTI_LINE_TEXT_VIEW::scroll_(GX_VALUE amount_to_scroll)
{
    if (amount_to_scroll == 0)
    {
        return false;
    }

    text_scroll_shift += amount_to_scroll;

    scrollbar_reset_or_line_cache_update_();

    // If the text view has a thin (rounded) border with no
    // whitespace between border and text, we cannot use block
    // move to scroll because we will capture pixels from the
    // rounded corner. In that case just use dirty_mark,
    // otherwise use block_move.
    if ((style & GX_STYLE_BORDER_THIN) &&
        !(style & GX_STYLE_TRANSPARENT) &&
        (margin == 0))
    {
        dirty_mark_();
    }
    else
    {
        GX_RECTANGLE text_area;
        display_area_(&text_area);

        block_move_(&text_area, 0, amount_to_scroll);
    }

    return true;
}

GX_VALUE GX_MULTI_LINE_TEXT_VIEW::whitespace_width_() const
{
    const GX_VALUE default_space_width = 12;

    GX_FONT* font = GX_NULL;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return default_space_width;
    }

    GX_VALUE space_width = _gx_system_string_width_get(font, " ", 1);
    if (space_width <= 0)
    {
        return default_space_width;
    }

    return space_width;
}

void GX_MULTI_LINE_TEXT_VIEW::line_display_info_get_(UINT start_index, UINT end_index, GX_VALUE available_width,
                                                     GX_MULTI_LINE_TEXT_INFO* text_info) const
{
    text_info->display_width = 0;
    text_info->display_number = 0;

    GX_STRING string = { GX_NULL, 0 };
    text_data.get_(&string);

    string.ptr += start_index;
    string.length = end_index - start_index;

    if (this->status & GX_STATUS_LINE_BREAK_PROCESSED)
    {
        // Line breaking already preprocessed,
        // only need to search for line break characters to do line break.
        while (string.length > 0)
        {
            if (string.ptr[0] == '\r')
            {
                ++text_info->display_number;
                if (string.length > 1 && string.ptr[1] == '\n')
                {
                    ++text_info->display_number;
                }
                break;
            }
            else if (string.ptr[0] == '\n')
            {
                ++text_info->display_number;
                break;
            }
            else
            {
                ++text_info->display_number;
            }

            ++string.ptr;
            --string.length;
        }
    }
    else
    {
        GX_FONT* font;
        font_get_(font_id_(), &font);
        if (!font)
        {
            return;
        }

        USHORT display_width = 0;
        USHORT display_number = 0;
        const GX_CHAR* ptr;
        UINT glyph_len = 1;
        GX_VALUE char_width;

        while (string.length > 0)
        {
            ptr = string.ptr;

#ifdef GX_UTF8_SUPPORT
            _gx_utility_utf8_string_character_get(&string, GX_NULL, &glyph_len);
#else
            ++string.ptr;
            --string.length;
#endif // GX_UTF8_SUPPORT

            char_width = _gx_system_string_width_get(font, ptr, glyph_len);

            if (ptr[0] == '\r')
            {
                ++text_info->display_number;
                if (glyph_len > 1 && ptr[1] == '\n')
                {
                    ++text_info->display_number;
                }
                break;
            }
            else if (ptr[0] == '\n')
            {
                ++text_info->display_number;
                break;
            }
            else if ((text_info->display_width + char_width > available_width) &&
                     (text_info->display_number > 0))
            {
                if (ptr[0] == ' ')
                {
                    // Space itself causes overflow: consume it and any consecutive
                    // trailing spaces so the next line starts at the next word.
                    // Do NOT add to display_width -- the line's visual width stays
                    // within available_width. */
                    text_info->display_number = (USHORT)(text_info->display_number + glyph_len);

                    while (string.length > 0 && string.ptr[0] == ' ')
                    {
                        ++text_info->display_number;

                        ++string.ptr;
                        --string.length;
                    }

                    break;
                }

                if (display_number == 0)
                {
                    break;
                }

                text_info->display_width = display_width;
                text_info->display_number = display_number;
                break;
            }

            text_info->display_width = (USHORT)(text_info->display_width + char_width);
            text_info->display_number = (USHORT)(text_info->display_number + glyph_len);

            if (ptr[0] == ' ' || ptr[0] == ',' || ptr[0] == ';')
            {
                display_width = text_info->display_width;
                display_number = text_info->display_number;
            }
        }
    }
}

bool GX_MULTI_LINE_TEXT_VIEW::line_cache_update_()
{
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    if (_gx_system_bidi_text_enabled)
    {
        // No need to update line index cache when dynamic bidi text option is on.
        return true;
    }
#endif

    GX_FONT* font;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return false;
    }

    INT line_height = font->line_height + line_space;
    if (line_height <= 0)
    {
        return false;
    }

    UINT first_visible_line;

    if (text_scroll_shift < 0)
    {
        first_visible_line = ((UINT)-text_scroll_shift) / (UINT)line_height;
    }
    else
    {
        first_visible_line = 0;
    }

    UINT last_visible_line = first_visible_line + text_visible_lines;

    INT new_first_cache_line = GX_MULTI_LINE_INDEX_CACHE_SIZE;
    new_first_cache_line -= (INT)(last_visible_line - first_visible_line + 1);
    new_first_cache_line /= 2;
    new_first_cache_line = (INT)first_visible_line - new_first_cache_line;

    if ((UINT)new_first_cache_line + GX_MULTI_LINE_INDEX_CACHE_SIZE > text_total_lines)
    {
        new_first_cache_line = (INT)(text_total_lines - GX_MULTI_LINE_INDEX_CACHE_SIZE);
    }

    if (first_visible_line <= first_cache_line)
    {
        if (new_first_cache_line < 0)
        {
            new_first_cache_line = 0;
        }

        if (new_first_cache_line < (INT)first_cache_line)
        {
            line_start_cache_update_((UINT)new_first_cache_line);
        }
    }
    else if (last_visible_line > first_cache_line + line_index_cache_size)
    {
        if (new_first_cache_line > (INT)first_cache_line)
        {
            line_tail_cache_update_((UINT)new_first_cache_line);
        }
    }

    return true;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_line_index_find                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to find the first byte index of the        */
/*    sepcified line from the specified position.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    start_line                            Line index to search from     */
/*    start_index                           Byte index to search from     */
/*    search_line                           Index of the line to search   */
/*    available_width                       Width available to display    */
/*                                            text in current line        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    return_index                          Retrieved byte index of the   */
/*                                            line                        */
/*                                                                        */
/**************************************************************************/
UINT GX_MULTI_LINE_TEXT_VIEW::line_index_find_(UINT start_line, UINT start_index,
                                               UINT search_line, GX_VALUE available_width)
{
    GX_MULTI_LINE_TEXT_INFO text_info;

    while (start_line < search_line)
    {
        line_display_info_get_(start_index, text_().length, available_width, &text_info);

        start_index += text_info.display_number;
        ++start_line;
    }

    return start_index;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_line_start_cache_create                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to create line index cache for specified   */
/*    line range.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    first_line                            The line to start cache       */
/*    updated_size                          The numer of lines to cache   */
/*                                                                        */
/**************************************************************************/
void GX_MULTI_LINE_TEXT_VIEW::line_start_cache_create_(UINT first_line, UINT updated_size)
{
    GX_MULTI_LINE_TEXT_INFO text_info;

    GX_RECTANGLE text_area;
    display_area_(&text_area);

    const GX_VALUE x_width = gx_text_area_width_(text_area);

    UINT index = line_index_find_(0, 0, first_line, x_width);

    UINT j = 0;
    line_index_cache[j++] = index;

    for (; j < updated_size; ++j)
    {
        line_display_info_get_(index, text_().length, x_width, &text_info);
        index += text_info.display_number;

        line_index_cache[j] = index;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_line_start_cache_update                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to update line index cache to cover        */
/*    previous lines.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    new_first_cache_line                  The new line index that cache */
/*                                            start                       */
/*                                                                        */
/**************************************************************************/
void GX_MULTI_LINE_TEXT_VIEW::line_start_cache_update_(UINT new_first_cache_line)
{
    UINT updated_size = first_cache_line - new_first_cache_line;

    if (updated_size < GX_MULTI_LINE_INDEX_CACHE_SIZE)
    {
        memmove(line_index_cache + updated_size, line_index_cache,
                sizeof(UINT) * (GX_MULTI_LINE_INDEX_CACHE_SIZE - updated_size));
    }
    else
    {
        updated_size = GX_MULTI_LINE_INDEX_CACHE_SIZE;
    }

    line_start_cache_create_(new_first_cache_line, updated_size);

    line_index_cache_size = (GX_UBYTE)(line_index_cache_size + updated_size);
    if (line_index_cache_size > GX_MULTI_LINE_INDEX_CACHE_SIZE)
    {
        line_index_cache_size = GX_MULTI_LINE_INDEX_CACHE_SIZE;
    }

    first_cache_line = new_first_cache_line;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_line_tail_cache_create                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to create line index cache for specified   */
/*    line range with specified byte index and line index to start        */
/*    calculating needed line index.                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*   start_index                            The first byte index of the   */
/*                                            line to search from         */
/*   start_line                             The line to search from       */
/*   first_line                             The line to start cache       */
/*   updated_size                           The number of lines to cache  */
/*                                                                        */
/**************************************************************************/
void GX_MULTI_LINE_TEXT_VIEW::line_tail_cache_create_(UINT start_index, UINT start_line,
                                                      UINT first_line, UINT updated_size)
{
    GX_MULTI_LINE_TEXT_INFO text_info;

    GX_RECTANGLE text_area;
    display_area_(&text_area);

    const GX_VALUE x_width = gx_text_area_width_(text_area);

    UINT index = line_index_find_(start_line, start_index, first_line, x_width);

    UINT j = GX_MULTI_LINE_INDEX_CACHE_SIZE - updated_size;
    line_index_cache[j++] = index;

    for (; j < GX_MULTI_LINE_INDEX_CACHE_SIZE; ++j)
    {
        line_display_info_get_(index, text_().length, x_width, &text_info);
        index += text_info.display_number;

        line_index_cache[j] = index;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_line_start_cache_update                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to update line index cache to cover        */
/*    previous lines.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    new_first_cache_line                  The new line index that cache */
/*                                            start                       */
/*                                                                        */
/**************************************************************************/
void GX_MULTI_LINE_TEXT_VIEW::line_tail_cache_update_(UINT new_first_cache_line)
{
    UINT updated_size = new_first_cache_line - first_cache_line;

    UINT last_cache_index = line_index_cache[GX_MULTI_LINE_INDEX_CACHE_SIZE - 1];
    UINT last_cache_line = first_cache_line + GX_MULTI_LINE_INDEX_CACHE_SIZE - 1;

    if (updated_size < GX_MULTI_LINE_INDEX_CACHE_SIZE)
    {
        memmove(line_index_cache, line_index_cache + updated_size,
                sizeof(UINT) * (GX_MULTI_LINE_INDEX_CACHE_SIZE - updated_size));
    }
    else
    {
        updated_size = GX_MULTI_LINE_INDEX_CACHE_SIZE;
    }

    UINT first_line = new_first_cache_line + GX_MULTI_LINE_INDEX_CACHE_SIZE - updated_size;
    line_tail_cache_create_(last_cache_index, last_cache_line, first_line, updated_size);

    first_cache_line = new_first_cache_line;
}

bool GX_MULTI_LINE_TEXT_VIEW::visible_lines_compute_()
{
    GX_FONT* font;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return false;
    }

    INT line_height = font->line_height + this->line_space;
    if (line_height > 0)
    {
        GX_RECTANGLE text_area;
        display_area_(&text_area);

        text_visible_lines = (UINT)((text_area.height_() + line_height - 1) / line_height);
    }

    return true;
}

bool GX_MULTI_LINE_TEXT_VIEW::total_lines_compute_()
{
    GX_FONT* font;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return false;
    }

    if (text_data.string.length == 0)
    {
        text_total_lines = 0;

        first_cache_line = 0;
        line_index_cache_size = 0;
        line_index_cache[0] = 0;

        line_index_cache_old = GX_FALSE;

        return true;
    }

    UINT total_lines = 0;

    GX_RECTANGLE text_area;
    display_area_(&text_area);

    const GX_VALUE x_width = gx_text_area_width_(text_area);

    GX_STRING string = { GX_NULL, 0 };
    text_data.get_and_resolve_bidi_text_(&string, font, x_width);

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    if (_gx_system_bidi_text_enabled && (this->type == GX_TYPE_MULTI_LINE_TEXT_VIEW))
    {
        GX_BIDI_RESOLVED_TEXT_INFO* info = text_data.bidi_resolved_text_info;
        while (info)
        {
            total_lines += info->total_lines;
            info = info->next;
        }
    }
    else
#endif // GX_DYNAMIC_BIDI_TEXT_SUPPORT
    {
        GX_MULTI_LINE_TEXT_INFO text_info;
        UINT cache_index = 0;

        UINT index = 0;
        while (index < string.length)
        {
            // save line index
            if (total_lines >= first_cache_line &&
                total_lines < first_cache_line + GX_MULTI_LINE_INDEX_CACHE_SIZE)
            {
                cache_index = total_lines - first_cache_line;
                line_index_cache[cache_index] = index;
            }

            line_display_info_get_(index, string.length, x_width, &text_info);

            ++total_lines;

            index += text_info.display_number;
        }

        if (index > 0)
        {
            if (string.ptr[index - 1] == '\r' ||
                string.ptr[index - 1] == '\n')
            {
                // save line index
                if (total_lines >= first_cache_line &&
                    total_lines < first_cache_line + GX_MULTI_LINE_INDEX_CACHE_SIZE)
                {
                    cache_index = total_lines - first_cache_line;
                    line_index_cache[cache_index] = index;
                }

                ++total_lines;
            }
        }

        line_index_cache_size = (GX_UBYTE)(cache_index + 1);
    }

    text_total_lines = total_lines;

    line_index_cache_old = GX_FALSE;

    return true;
}

void GX_MULTI_LINE_TEXT_VIEW::scrollbar_reset_or_line_cache_update_()
{
    GX_SCROLLBAR* scrollbar = scrollbar_find_(GX_TYPE_VERTICAL_SCROLL);
    if (scrollbar)
    {
        scrollbar->reset_();
    }
    else
    {
        if (text_total_lines > line_index_cache_size)
        {
            line_cache_update_();
        }
    }
}

GX_VALUE GX_MULTI_LINE_TEXT_VIEW::cursor_x_pos_get_(const GX_STRING* string,
                                                    const GX_FONT* font,
                                                    const GX_RECTANGLE& text_area) const
{
    const GX_VALUE x_left = gx_text_area_left_(text_area);
    const GX_VALUE x_right = gx_text_area_right_(text_area);
    const GX_VALUE x_width = gx_text_area_width_(text_area);

    GX_VALUE text_width;

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        text_width = text_width_get_(string, font, x_width);
        return (GX_VALUE)(x_right - text_width);

    case GX_STYLE_TEXT_CENTER:
    default:
        text_width = text_width_get_(string, font, x_width);
        return (GX_VALUE)(text_area.left + (text_area.width_() - text_width) / 2);

    case GX_STYLE_TEXT_LEFT:
        return x_left;
    }
}

GX_VALUE GX_MULTI_LINE_TEXT_VIEW::text_width_get_(const GX_STRING* string,
                                                  const GX_FONT* font,
                                                  GX_VALUE available_width) const
{
    GX_VALUE text_width = _gx_system_string_width_get(font, string->ptr, string->length);

    return x_value_clamp_(text_width, available_width);
}

GX_VALUE GX_MULTI_LINE_TEXT_VIEW::x_value_clamp_(GX_VALUE x_value, GX_VALUE high_limit) const
{
    while (x_value > high_limit)
    {
        x_value = (GX_VALUE)(x_value - whitespace_width);
    }

    return x_value;
}

void GX_MULTI_LINE_TEXT_VIEW::text_draw_()
{
    GX_RESOURCE_ID text_color;

    if (is_enabled_())
    {
        if (style & GX_STYLE_DRAW_SELECTED)
        {
            text_color = selected_text_color_();
        }
        else
        {
            text_color = normal_text_color_();
        }
    }
    else
    {
        text_color = disabled_text_color_();
    }

    text_draw_(text_color);
}

void GX_MULTI_LINE_TEXT_VIEW::text_draw_(GX_RESOURCE_ID text_color)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

    context->line_color_set_(text_color);
    context->font_set_(font_id_());
    context->brush.width = 1;

    if (line_index_cache_old)
    {
        visible_lines_compute_();
        total_lines_compute_();

        scrollbar_reset_or_line_cache_update_();
    }

    GX_FONT* font;
    context->font_get_(font_id_(), &font);

    if (text_().length == 0 || !font)
    {
        return;
    }

    INT line_height = font->line_height + line_space;
    if (line_height <= 0)
    {
        return;
    }

    GX_RECTANGLE text_area;
    display_area_(&text_area);

    if ((this->type == GX_TYPE_MULTI_LINE_TEXT_VIEW) &&
        (this->style & GX_STYLE_VALIGN_CENTER))
    {
        GX_SCROLLBAR* scrollbar = scrollbar_find_(GX_TYPE_VERTICAL_SCROLL);
        if (!scrollbar)
        {
            GX_VALUE height = text_area.height_() - 1;
            height = (GX_VALUE)((INT)height - (INT)text_total_lines * line_height);
            text_scroll_shift = height / 2;
        }
    }

    GX_RECTANGLE overlap;
    if (gx_rectangle_intersect_(context->dirty, text_area, &overlap))
    {
        GX_DRAWING drawing(context->canvas, this, overlap, GX_FALSE);
        if (drawing.result_() == GX_SUCCESS)
        {
            INT first_visible_line = -text_scroll_shift / line_height;
            if (first_visible_line < 0)
            {
                first_visible_line = 0;
            }

            INT last_visible_line = first_visible_line + (INT)text_visible_lines;
            if (last_visible_line > (INT)(text_total_lines - 1))
            {
                last_visible_line = (INT)(text_total_lines - 1);
            }

            INT y_pos = text_area.top + text_scroll_shift;
            y_pos += (INT)(first_visible_line * line_height);
            y_pos += line_space / 2;

            GX_STRING string;
            text_data.get_and_resolve_bidi_text_(&string, font, gx_text_area_width_(text_area));

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
            GX_BIDI_RESOLVED_TEXT_INFO* info = text_data.bidi_resolved_text_info;
            UINT bidi_text_line_index = 0;
#endif

            GX_STRING line_string;

            for (INT index = first_visible_line; index <= last_visible_line; ++index)
            {
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
                if (_gx_system_bidi_text_enabled)
                {
                    line_string.ptr = GX_NULL;
                    line_string.length = 0;

                    while (info)
                    {
                        if (bidi_text_line_index + info->total_lines > (UINT)index)
                        {
                            if (info->text)
                            {
                                line_string = info->text[(UINT)index - bidi_text_line_index];
                            }
                            break;
                        }

                        bidi_text_line_index += info->total_lines;

                        info = info->next;
                    }
                }
                else
#endif // GX_DYNAMIC_BIDI_TEXT_SUPPORT
                {
                    UINT line_cache_start = (UINT)index - first_cache_line;
                    UINT line_start_index = line_index_cache[line_cache_start];
                    UINT line_end_index;
                    if (line_cache_start >= (UINT)(line_index_cache_size - 1))
                    {
                        line_end_index = string.length;
                    }
                    else
                    {
                        line_end_index = line_index_cache[line_cache_start + 1];
                    }

                    line_string.ptr = string.ptr + line_start_index;
                    line_string.length = line_end_index - line_start_index;
                }

                GX_VALUE x_pos = cursor_x_pos_get_(&line_string, font, text_area);

                _gx_canvas_text_draw_ext(x_pos, (GX_VALUE)y_pos, &line_string);

                y_pos += line_height;
            }
        }
    }
}

void GX_MULTI_LINE_TEXT_VIEW::draw_()
{
    background_draw_();
    text_draw_();
    children_draw_();
}

UINT GX_MULTI_LINE_TEXT_VIEW::event_process_(GX_EVENT* event_ptr)
{
    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        GX_WINDOW::event_process_(event_ptr);

        if (text_id_())
        {
            text_id_set_(text_id_());
        }

        visible_lines_compute_();
        break;

    case GX_EVENT_RESIZED:
        if (is_visible_())
        {
            line_index_cache_old = GX_TRUE;

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
            text_data.bidi_resolved_text_info_delete_();
#endif
        }
        break;

    case GX_EVENT_CLIENT_UPDATED:
        if (is_visible_())
        {
            line_index_cache_old = GX_TRUE;
            dirty_mark_();
        }
        break;

    case GX_EVENT_STYLE_CHANGED:
        if (is_visible_())
        {
            if ((event_ptr->payload.ulongdata & GX_STYLE_BORDER_MASK) !=
                (this->style & GX_STYLE_BORDER_MASK))
            {
                line_index_cache_old = GX_TRUE;
            }
        }
        break;

    case GX_EVENT_LANGUAGE_CHANGE:
        if (text_id_())
        {
            text_id_set_(text_id_());
        }
        break;

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    case GX_EVENT_DYNAMIC_BIDI_TEXT_ENABLE:
    case GX_EVENT_DYNAMIC_BIDI_TEXT_DISABLE:
        line_index_cache_old = GX_TRUE;
        break;
#endif

    case GX_EVENT_DELETE:
        text_data.string_delete_();
        break;

    case GX_EVENT_VERTICAL_SCROLL:
        scroll_((GX_VALUE)(event_ptr->payload.intdata[1] - event_ptr->payload.intdata[0]));
        return GX_SUCCESS;

    default:
        break;
    }

    return GX_WINDOW::event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_create                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a multi-line text view widget.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                             Multi-line text view widget   */
/*                                            control block               */
/*    name                                  Name of the text input widget */
/*    parent                                Pointer to parent widget      */
/*    text_id                               Resource ID of the text string*/
/*    style                                 Style of text view widget     */
/*    id                                    Application-defined ID of text*/
/*                                            view                        */
/*    size                                  Dimension of text view widget */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_create                     Create the window             */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_view_create(GX_MULTI_LINE_TEXT_VIEW* view,
                                     const GX_CHAR* name,
                                     GX_WIDGET* parent,
                                     GX_RESOURCE_ID text_id,
                                     ULONG style,
                                     USHORT Id,
                                     const GX_RECTANGLE* size)
{
    return view->create_(name, parent, text_id, style, Id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_draw                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws a multi-line-text-view widget.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                              Multi-line_text_view widget  */
/*                                           control block                */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_border_draw                Draw window background with   */
/*                                            specified fill color        */
/*    _gx_multi_line_text_view_text_draw    Draw text                     */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_multi_line_text_view_draw(GX_MULTI_LINE_TEXT_VIEW* view)
{
    view->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_event_process                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service processes an event for a multi-line text view widget.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                             Multi-line text view widget   */
/*                                            control block               */
/*    event_ptr                             Point to event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_text_id_set  Set new text id               */
/*    _gx_multi_line_text_view_scroll       Scroll the text view window   */
/*    _gx_window_event_process              Window event process          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_view_event_process(GX_MULTI_LINE_TEXT_VIEW* view, GX_EVENT* event_ptr)
{
    return view->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_font_set                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the font of a multi-line text view widget.        */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                             Multi-line text view widget   */
/*                                            control block               */
/*    font_id                               Resource ID for the font      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_scrollbar_find             Find scrollbar for a window   */
/*    _gx_scrollbar_reset                   Reset scrollbar information   */
/*    _gx_multi_line_text_view_line_cache_update                          */
/*                                          Update line cache             */
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_view_font_set(GX_MULTI_LINE_TEXT_VIEW* view, GX_RESOURCE_ID font_id)
{
    view->font_set_(font_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_line_pace_set                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the line space of a multi line text view widget. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    view                                  Multi-line text view widget   */
/*                                            control block               */
/*    line_space                            Value to set                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_string_total_rows_compute                  */
/*                                          Update string total rows      */
/*    _gx_window_scrollbar_find             Find scrollbar for a window   */
/*    _gx_scrollbar_reset                   Reset scrollbar information   */
/*    _gx_multi_line_text_view_line_cache_update                          */
/*                                          Update line cache             */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*    _gx_system_event_fold                 Fold a event                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_view_line_space_set(GX_MULTI_LINE_TEXT_VIEW* view, GX_BYTE line_space)
{
    view->line_space_set_(line_space);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_scroll_info_get                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function gets the multi-line text view scroll information.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                            Multi line text view widget    */
/*                                           control block                */
/*    style                                 Style                         */
/*    info                                 Pointer to destination for     */
/*                                           scroll info                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_font_get                   Obtain the font               */
/*    _gx_multi_line_text_view_string_total_rows_computer                 */
/*                                          Compute the number of rows    */
/*                                            for the text view text      */
/*    _gx_utility_rectangle_resize          Offset rectangle              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_view_scroll_info_get(GX_MULTI_LINE_TEXT_VIEW* view, ULONG style, GX_SCROLL_INFO* info)
{
    return view->scroll_info_get_(style, info);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_view_text_color_set                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the text color of a multi line text view widget.  */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                             Multi-line text view widget   */
/*                                            control block               */
/*    normal_text_color_id                  Resource ID of the normal     */
/*                                            text color                  */
/*    selected_text_color_id                Resource ID of the selected   */
/*                                            text color                  */
/*    disabled_text_color_id                Resource ID of the disabled   */
/*                                            text color                  */
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
UINT _gx_multi_line_text_view_text_color_set(GX_MULTI_LINE_TEXT_VIEW* view,
                                             GX_RESOURCE_ID normal_text_color_id,
                                             GX_RESOURCE_ID selected_text_color_id,
                                             GX_RESOURCE_ID disabled_text_color_id)
{
    view->text_color_set_(normal_text_color_id,
                          selected_text_color_id,
                          disabled_text_color_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_text_id_set                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a text input widget.                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                             Multi-line view control block */
/*    text_id                               Resource ID for the text      */
/*                                            string                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    gx_string_length_get_       Test string length            */
/*    _gx_system_string_get                 Get the string from the ID    */
/*    _gx_system_memory_free                Application-defined memory    */
/*                                            free function               */
/*    _gx_multi_line_text_view_string_total_rows_compute                  */
/*                                          Compute the total rows of     */
/*                                            of the display text         */
/*    _gx_multi_line_text_view_line_cache_update                          */
/*                                          Update line cache             */
/*    _gx_window_scrollbar_find             Find scrollbar for a window   */
/*    _gx_scrollbar_reset                   Reset scrollbar information   */
/*    _gx_utility_utf8_string_character_count_get                         */
/*                                          Get character count of utf8   */
/*                                            string                      */
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_view_text_id_set(GX_MULTI_LINE_TEXT_VIEW* view, GX_RESOURCE_ID text_id)
{
    view->text_id_set_(text_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_text_set_ext                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns a text string to the multi line text view     */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                             Multi-line text view widget   */
/*                                            control block               */
/*    text                                  GX_STRING type text string    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    gx_string_length_get_                 Test string length            */
/*    gx_string_copy_                       Make a private copy for       */
/*                                            assigned text               */
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_view_text_set_ext(GX_MULTI_LINE_TEXT_VIEW* view, const GX_STRING* text)
{
    return view->text_set_ext_(text);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_view_whitespace_set                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets whitespace for a multi line text view widget.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                             Multi-line text view widget   */
/*                                            control block               */
/*    whitespace                            Value to set                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_string_total_rows_compute                  */
/*                                          Calculate total rows of       */
/*                                            text view text              */
/*    _gx_window_scrollbar_find             Find scrollbar for a window   */
/*    _gx_scrollbar_reset                   Reset scrollbar information   */
/*    _gx_multi_line_text_view_line_cache_update                          */
/*                                          Update line cache             */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*    _gx_system_event_fold                 Fold a event                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_view_whitespace_set(GX_MULTI_LINE_TEXT_VIEW* view, GX_UBYTE whitespace)
{
    view->margin_set_(whitespace);

    return GX_SUCCESS;
}
