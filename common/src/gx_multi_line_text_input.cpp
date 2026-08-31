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
/**   Multi Line Text Input Management (Multi Line Text Input)            */
/**                                                                       */
/**************************************************************************/

#include "gx_multi_line_text_input.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_canvas.h"
#include "gx_font.h"
#include "gx_scroll_bar.h"
#include "gx_text_area_info.h"

#define GX_CURSOR_BLINK_TIMER   2
#define GX_MARK_TIMER           3

#define GX_MARK_INTERVAL        5

static inline bool gx_is_new_line_(GX_CHAR c)
{
    return (c == '\r') || (c == '\n');
}

static void gx_multi_line_text_input_draw_(GX_MULTI_LINE_TEXT_INPUT* text_input)
{
    text_input->draw_();
}

static UINT gx_multi_line_text_input_event_process_(GX_MULTI_LINE_TEXT_INPUT* text_input, GX_EVENT* event_ptr)
{
    return text_input->event_process_(event_ptr);
}

UINT GX_MULTI_LINE_TEXT_INPUT::create_(const GX_CHAR* name, GX_WIDGET* parent,
                                       GX_CHAR* input_buffer, UINT buffer_size,
                                       ULONG style, USHORT Id,
                                       const GX_RECTANGLE* size)
{
    GX_MULTI_LINE_TEXT_VIEW* base = (GX_MULTI_LINE_TEXT_VIEW*)this;

    // Don't allow the GX_STYLE_TEXT_COPY style, the text input always uses a dynamic buffer
    style &= ~GX_STYLE_TEXT_COPY;

    base->create_(name, GX_NULL, 0, style, Id, size);

    if (!input_buffer && buffer_size)
    {
        if (!_gx_system_memory_allocator)
        {
            return GX_SYSTEM_MEMORY_ERROR;
        }

        input_buffer = (GX_CHAR*)_gx_system_memory_allocator(buffer_size);
        if (!input_buffer)
        {
            return GX_SYSTEM_MEMORY_ERROR;
        }

        this->status |= GX_STATUS_DYNAMIC_BUFFER;
    }

    GX_STRING string;
    if (_gx_utility_string_length_check(input_buffer, &string.length, buffer_size - 1) == GX_SUCCESS)
    {
        string.ptr = input_buffer;
        base->text_set_ext_(&string);
    }
    else
    {
        this->text_().ptr = input_buffer;
        this->text_().length = 0;
    }

    this->type = GX_TYPE_MULTI_LINE_TEXT_INPUT;

    this->text_was_modified = GX_FALSE;
    this->buffer_size = buffer_size;

    this->text_cursor_line = 1;
    this->text_insert_position = 0;

    this->new_line_character[0] = '\n';
    this->new_line_character[1] = '\0';
    this->new_line_character_size = 1;

    this->selected_text_color_() = GX_COLOR_ID_SELECTED_TEXT;
    this->selected_fill_color = GX_COLOR_ID_SELECTED_FILL;
    this->readonly_text_color = GX_COLOR_ID_READONLY_TEXT;
    this->readonly_fill_color = GX_COLOR_ID_READONLY_FILL;

    this->start_mark = 0;
    this->end_mark = 0;

    this->style |= GX_STYLE_CURSOR_BLINK;

    GX_RECTANGLE input_area;
    display_area_(&input_area);

    cursor.height = 0;
    cursor.width = 1;
    cursor.flags = 0;
    cursor.blink_interval = GX_CURSOR_BLINK_INTERVAL;
    cursor.pos.y = 0;
    cursor.pos.x = gx_text_area_left_(input_area);

    this->draw_function = (void (*)(GX_WIDGET*))gx_multi_line_text_input_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_multi_line_text_input_event_process_;

    if (this->style & GX_STYLE_CURSOR_ALWAYS)
    {
        this->status |= (GX_STATUS_CURSOR_SHOW | GX_STATUS_CURSOR_DRAW);
    }

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_MULTI_LINE_TEXT_INPUT::cursor_blink_timer_start_()
{
    if (is_visible_() && (style & GX_STYLE_CURSOR_BLINK))
    {
        UINT blink_interval = (UINT)cursor.blink_interval;
        timer_start_(GX_CURSOR_BLINK_TIMER, blink_interval, blink_interval);
    }
}

void GX_MULTI_LINE_TEXT_INPUT::style_add_(ULONG style)
{
    GX_WIDGET::style_add_(style);

    if (this->style & GX_STYLE_CURSOR_ALWAYS)
    {
        status_add_(GX_STATUS_CURSOR_SHOW | GX_STATUS_CURSOR_DRAW);

        cursor_blink_timer_start_();
    }
}

void GX_MULTI_LINE_TEXT_INPUT::style_remove_(ULONG style)
{
    GX_WIDGET::style_remove_(style);

    if (is_visible_() && (style & GX_STYLE_CURSOR_BLINK))
    {
        timer_stop_(GX_CURSOR_BLINK_TIMER);

        status_add_(GX_STATUS_CURSOR_DRAW);
    }

    if ((style & GX_STYLE_CURSOR_ALWAYS) &&
        _gx_system_focus_owner != this)
    {
        status_remove_(GX_STATUS_CURSOR_SHOW);
    }
}

void GX_MULTI_LINE_TEXT_INPUT::style_set_(ULONG style)
{
    style_remove_(style);
    style_add_(style);
}

void GX_MULTI_LINE_TEXT_INPUT::fill_color_set_(GX_RESOURCE_ID normal_fill_color_id,
                                               GX_RESOURCE_ID selected_fill_color_id,
                                               GX_RESOURCE_ID disabled_fill_color_id,
                                               GX_RESOURCE_ID readonly_fill_color_id)
{
    this->normal_fill_color = normal_fill_color_id;
    this->selected_fill_color = selected_fill_color_id;
    this->disabled_fill_color = disabled_fill_color_id;
    this->readonly_fill_color = readonly_fill_color_id;

    dirty_mark_if_visible_();
}

void GX_MULTI_LINE_TEXT_INPUT::text_color_set_(GX_RESOURCE_ID normal_text_color_id,
                                               GX_RESOURCE_ID selected_text_color_id,
                                               GX_RESOURCE_ID disabled_text_color_id,
                                               GX_RESOURCE_ID readonly_text_color_id)
{
    this->readonly_text_color = readonly_text_color_id;
    text_data.color_set_(normal_text_color_id,
                         selected_text_color_id,
                         disabled_text_color_id);
}

void GX_MULTI_LINE_TEXT_INPUT::buffer_get_(GX_CHAR** buffer_address, UINT* content_size, UINT* buffer_size)
{
    if (buffer_address)
    {
        *buffer_address = const_cast<GX_CHAR*>(text_().ptr);
    }

    if (content_size)
    {
        *content_size = text_().length;
    }

    if (buffer_size)
    {
        *buffer_size = this->buffer_size;
    }
}

void GX_MULTI_LINE_TEXT_INPUT::buffer_clear_()
{
    if (text_().length > 0)
    {
        text_delete_(0, text_().length);

        first_cache_line = 0;
        memset(line_index_cache, 0, sizeof(line_index_cache));

        total_lines_compute_();

        text_insert_position = 0;

        GX_SCROLLBAR* scrollbar = scrollbar_find_(GX_TYPE_VERTICAL_SCROLL);
        if (scrollbar)
        {
            scrollbar->reset_();
        }

        cursor_pos_update_(GX_TRUE);

        dirty_mark_();
    }
}

UINT GX_MULTI_LINE_TEXT_INPUT::text_set_ext_(const GX_STRING* text)
{
    UINT copy_size;
    GX_STRING string;

    GX_CHAR* this_text_ptr = const_cast<GX_CHAR*>(text_().ptr);

    if (text && text->ptr)
    {
        copy_size = buffer_size - 1;

        if (text->length > copy_size - 1)
        {
#if defined GX_UTF8_SUPPORT

            string.ptr = text->ptr;
            string.length = copy_size;

            UINT index = 0;
            UINT glyph_len = 0;
            while (_gx_utility_utf8_string_character_get(&string, GX_NULL, &glyph_len) == GX_SUCCESS)
            {
                index += glyph_len;
            }

            copy_size = index;

#endif // GX_UTF8_SUPPORT
        }
        else
        {
            copy_size = text->length;
        }

        memcpy(this_text_ptr, text->ptr, copy_size);
    }
    else
    {
        copy_size = 0;
    }

    this_text_ptr[copy_size] = '\0';
    text_().length = copy_size;

    new_line_character_get_();

    if (is_visible_())
    {
        total_lines_compute_();

        text_insert_position = copy_size;

        scrollbar_reset_or_line_cache_update_();

        cursor_pos_update_(GX_TRUE);

        dirty_mark_();
    }
    else
    {
        line_index_cache_old = GX_TRUE;
        text_insert_position = 0;
    }

    return GX_SUCCESS;
}

void GX_MULTI_LINE_TEXT_INPUT::new_line_character_get_()
{
    GX_STRING string = text_();
    const GX_CHAR* ch;

    while (string.length > 0)
    {
        ch = string.ptr;

#ifdef GX_UTF8_SUPPORT
        UINT glyph_len;
        _gx_utility_utf8_string_character_get(&string, GX_NULL, &glyph_len);
#else
        ++string.ptr;
        --string.length;
#endif // GX_UTF8_SUPPORT

        if (ch[0] == '\r')
        {
            if (ch[1] == '\n')
            {
                memcpy(new_line_character, "\r\n", 3);
                new_line_character_size = 2;
            }
            else
            {
                memcpy(new_line_character, "\n", 2);
                new_line_character_size = 1;
            }

            break;
        }
        else if (ch[0] == '\n')
        {
            memcpy(new_line_character, "\n", 2);
            new_line_character_size = 1;

            break;
        }
    }
}

// click_pos -> text_cursor_line, cursor.pos, text_insert_position
UINT GX_MULTI_LINE_TEXT_INPUT::cursor_pos_calculate_(GX_POINT click_pos)
{
    GX_FONT* font;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return GX_FAILURE;
    }

    GX_VALUE line_height = (GX_VALUE)(font->line_height + line_space);
    if (!line_height)
    {
        return GX_FAILURE;
    }

    GX_RECTANGLE input_area;
    display_area_(&input_area);

    UINT cursor_line;

    GX_VALUE y_pos = (GX_VALUE)(input_area.top + text_scroll_shift);

    if (click_pos.y < (GX_VALUE)(y_pos + line_height) || text_total_lines == 0)
    {
        cursor_line = 1;
    }
    else
    {
        cursor_line = (UINT)(1 + (click_pos.y - y_pos) / line_height);
        if (cursor_line > text_total_lines)
        {
            cursor_line = text_total_lines;
        }
    }

    y_pos = (GX_VALUE)(y_pos + (cursor_line - 1) * line_height);

    INT shift = scroll_shift_get_(y_pos, line_height, input_area);
    if (shift)
    {
        text_scroll_shift += shift;
        y_pos = (GX_VALUE)(y_pos + shift);

        scrollbar_reset_or_line_cache_update_();
    }

    text_cursor_line = cursor_line;

    UINT cache_index = cursor_line - 1 - first_cache_line;
    if (cache_index >= line_index_cache_size)
    {
        return GX_FAILURE;
    }

    UINT start_index;
    UINT end_index;
    line_start_and_end_index_get_(cache_index, &start_index, &end_index);

    GX_VALUE x_pos = cursor_x_pos_get_(start_index, end_index, font, input_area);

    GX_STRING string;
    string.ptr = text_().ptr + start_index;
    string.length = end_index - start_index;

    const GX_CHAR* ptr;
    UINT glyph_len = 1;
    GX_VALUE char_width;

    UINT index;
    for (index = start_index; index < end_index; index += glyph_len)
    {
        if (gx_is_new_line_(string.ptr[0]))
        {
            break;
        }

        ptr = string.ptr;

#ifdef GX_UTF8_SUPPORT
        _gx_utility_utf8_string_character_get(&string, GX_NULL, &glyph_len);
#else
        ++string.ptr;
        --string.length;
#endif

        char_width = _gx_system_string_width_get(font, ptr, glyph_len);

        if (x_pos + char_width / 2 > click_pos.x)
        {
            x_pos = x_value_clamp_(x_pos, gx_text_area_right_(input_area));
            break;
        }

        x_pos = (GX_VALUE)(x_pos + char_width);
    }

    cursor.pos.x = (GX_VALUE)x_pos;
    cursor.pos.y = (GX_VALUE)(y_pos + line_height / 2);

    text_insert_position = index;

    return GX_SUCCESS;
}

// text_insert_position -> text_cursor_line and cursor.pos
UINT GX_MULTI_LINE_TEXT_INPUT::cursor_pos_update_(GX_BOOL make_cursor_visible)
{
    GX_FONT* font;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return GX_FAILURE;
    }

    GX_VALUE line_height = (GX_VALUE)(font->line_height + line_space);
    if (line_height <= 0)
    {
        return GX_FAILURE;
    }

    GX_RECTANGLE input_area;
    display_area_(&input_area);

    UINT start_index;
    UINT end_index;
    UINT cache_index = 0;
    UINT cursor_line = 0;

    if ((cache_index < line_index_cache_size) &&
        (text_insert_position < line_index_cache[0]))
    {
        // insert position is before cached lines. search from start.
        start_index = 0;
        end_index = text_().length;
        cursor_line = cursor_line_search_(1, start_index, end_index, gx_text_area_width_(input_area));
    }
    else
    {
        // find cursor line
        for (; cache_index < line_index_cache_size; ++cache_index)
        {
            if (line_index_cache[cache_index] >= text_insert_position)
            {
                break;
            }
        }

        if (cache_index < line_index_cache_size)
        {
            // found insert position

            if (text_insert_position > 0 &&
                !insert_pos_previous_char_is_new_line_() &&
                cache_index > 0)
            {
                --cache_index;
            }

            cursor_line = 1 + first_cache_line + cache_index;

            line_start_and_end_index_get_(cache_index, &start_index, &end_index);
        }
        else if (line_index_cache_size > 0)
        {
            // insert position is after cached lines, search from last cached line
            cache_index = (UINT)(line_index_cache_size - 1);
            cursor_line = 1 + first_cache_line + cache_index;
            start_index = line_index_cache[cache_index];
            end_index = text_().length;
            cursor_line = cursor_line_search_(cursor_line, start_index, end_index, gx_text_area_width_(input_area));
        }
        else
        {
            // no character
            start_index = 0;
            end_index = 0;
            cursor_line = 1;
        }
    }

    text_cursor_line = cursor_line;

    GX_VALUE x_pos = cursor_x_pos_get_(start_index, end_index, font, input_area);

    if (text_insert_position > 0 && !insert_pos_previous_char_is_new_line_())
    {
        GX_STRING string;
        string.ptr = text_().ptr + start_index;
        string.length = text_insert_position - start_index;

        GX_VALUE text_width = 0;
        _gx_system_string_width_get_ext(font, &string, &text_width);

        x_pos = (GX_VALUE)(x_pos + text_width);
    }

    x_pos = x_value_clamp_(x_pos, gx_text_area_right_(input_area));

    INT y_pos = input_area.top + text_scroll_shift;
    y_pos += (cursor_line - 1) * line_height;

    cursor.pos.x = x_pos;
    cursor.pos.y = (GX_VALUE)(y_pos + line_height / 2);

    if (make_cursor_visible)
    {
        cursor_visible_();
    }

    return GX_SUCCESS;
}

void GX_MULTI_LINE_TEXT_INPUT::line_start_and_end_index_get_(UINT cache_index, UINT* start_index, UINT* end_index)
{
    *start_index = line_index_cache[cache_index];
    *end_index = line_index_cache_read_(cache_index + 1);
}

UINT GX_MULTI_LINE_TEXT_INPUT::line_index_cache_read_(UINT cache_index) const
{
    if (cache_index >= line_index_cache_size)
    {
        return text_().length;
    }
    else
    {
        return line_index_cache[cache_index];
    }
}

UINT GX_MULTI_LINE_TEXT_INPUT::cursor_visible_()
{
    GX_FONT* font;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return GX_FAILURE;
    }

    GX_VALUE line_height = (GX_VALUE)(font->line_height + line_space);
    if (!line_height)
    {
        return GX_FAILURE;
    }

    GX_RECTANGLE input_area;
    display_area_(&input_area);

    GX_VALUE y_pos = (GX_VALUE)(cursor.pos.y - line_height / 2);

    INT shift = scroll_shift_get_(y_pos, line_height, input_area);
    if (shift)
    {
        text_scroll_shift += shift;
        cursor.pos.y = (GX_VALUE)(cursor.pos.y + shift);

        scrollbar_reset_or_line_cache_update_();
    }

    return GX_SUCCESS;
}

INT GX_MULTI_LINE_TEXT_INPUT::scroll_shift_get_(GX_VALUE y_pos, GX_VALUE line_height, const GX_RECTANGLE& input_area) const
{
    if (y_pos < input_area.top)
    {
        return input_area.top - y_pos;
    }
    else if ((GX_VALUE)(y_pos + line_height - 1) > input_area.bottom)
    {
        return input_area.bottom - (y_pos + line_height - 1);
    }
    else
    {
        return 0;
    }
}

GX_VALUE GX_MULTI_LINE_TEXT_INPUT::cursor_x_pos_get_(UINT start_index, UINT end_index,
                                                     const GX_FONT* font,
                                                     const GX_RECTANGLE& input_area) const
{
    GX_STRING string;
    string.ptr = text_().ptr + start_index;
    string.length = end_index - start_index;

    return GX_MULTI_LINE_TEXT_VIEW::cursor_x_pos_get_(&string, font, input_area);
}

UINT GX_MULTI_LINE_TEXT_INPUT::cursor_line_search_(UINT start_cursor_line,
                                                   UINT& start_index, UINT& end_index,
                                                   GX_VALUE available_width) const
{
    GX_MULTI_LINE_TEXT_INFO text_info;

    UINT cursor_line = start_cursor_line;

    while (start_index < end_index)
    {
        line_display_info_get_(start_index, end_index, available_width, &text_info);

        start_index += text_info.display_number;
        if (start_index >= text_insert_position)
        {
            end_index = start_index;
            start_index -= text_info.display_number;
            break;
        }

        ++cursor_line;
    }

    if (text_insert_position > 0 && insert_pos_previous_char_is_new_line_())
    {
        ++cursor_line;
    }

    return cursor_line;
}

bool GX_MULTI_LINE_TEXT_INPUT::insert_pos_previous_char_is_new_line_() const
{
    GX_CHAR ch = text_().ptr[text_insert_position - 1];
    return gx_is_new_line_(ch);
}

void GX_MULTI_LINE_TEXT_INPUT::text_delete_(UINT start_index, UINT del_bytes)
{
    GX_CHAR* text_ptr = const_cast<GX_CHAR*>(text_().ptr);
    UINT& text_length = text_().length;

    text_length -= del_bytes;
    memmove(text_ptr + start_index, text_ptr + start_index + del_bytes, text_length - start_index);
    text_ptr[text_length] = '\0';

    text_was_modified = GX_TRUE;
}

void GX_MULTI_LINE_TEXT_INPUT::selected_text_delete_()
{
    if (this->start_mark != this->end_mark)
    {
        if (this->end_mark > this->start_mark)
        {
            backspace_();
        }
        else
        {
            text_delete_();
        }
    }
}

UINT GX_MULTI_LINE_TEXT_INPUT::text_insert_(const GX_UBYTE* str, UINT str_size)
{
    selected_text_delete_();

    if (!can_add_str_size_(str_size))
    {
        return GX_FAILURE;
    }

    GX_STRING insert_str;
    insert_str.ptr = (const GX_CHAR*)str;
    insert_str.length = str_size;

    GX_CHAR* input_buffer = const_cast<GX_CHAR*>(text_().ptr);

    UINT glyph_len = 1;
    GX_UBYTE* insert_char;

    while (insert_str.length > 0)
    {
        if (gx_is_new_line_(insert_str.ptr[0]))
        {
            insert_char = new_line_character;
            glyph_len = new_line_character_size;

            UINT handled_size = 1;
            if (insert_str.ptr[0] == '\r')
            {
                if (insert_str.length > 1 && insert_str.ptr[1] == '\n')
                {
                    ++handled_size;
                }
            }

            insert_str.ptr += handled_size;
            insert_str.length -= handled_size;
        }
        else
        {
            insert_char = (GX_UBYTE*)insert_str.ptr;

#if defined GX_UTF8_SUPPORT
            _gx_utility_utf8_string_character_get(&insert_str, GX_NULL, &glyph_len);
#else
            glyph_len = 1;
            ++insert_str.ptr;
            --insert_str.length;
#endif
        }

        if (!can_add_str_size_(glyph_len))
        {
            break;
        }

        memmove(input_buffer + text_insert_position + glyph_len,
                input_buffer + text_insert_position,
                text_().length - text_insert_position + 1);
        memmove(input_buffer + text_insert_position, insert_char, glyph_len);

        text_insert_position += glyph_len;
        text_().length += glyph_len;
    }

    total_lines_update_();

    INT old_scroll_shift = text_scroll_shift;
    GX_VALUE old_cursor_y = cursor.pos.y;

    cursor_pos_update_(GX_TRUE);
    text_was_modified = GX_TRUE;

    dirty_rect_mark_(old_scroll_shift, old_cursor_y, 0);

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::text_delete_()
{
    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    if (end_mark_ > start_mark_)
    {
        return backspace_();
    }

    UINT insert_pos = text_insert_position;
    if (insert_pos >= text_().length)
    {
        return GX_SUCCESS;
    }

    UINT glyph_len = 1;
    if (start_mark_ != end_mark_)
    {
        glyph_len = (UINT)GX_ABS((INT)start_mark - (INT)end_mark);

        this->start_mark = 0;
        this->end_mark = 0;
    }
    else
    {
        GX_STRING string;

#ifdef GX_UTF8_SUPPORT
        string.ptr = text_().ptr + insert_pos;
        string.length = text_().length - insert_pos;
        _gx_utility_utf8_string_character_get(&string, GX_NULL, &glyph_len);
#endif
        string.ptr = text_().ptr + insert_pos;
        if (gx_is_new_line_(string.ptr[0]))
        {
            glyph_len = new_line_character_size;
        }
    }

    INT old_scroll_shift = text_scroll_shift;

    text_delete_(insert_pos, glyph_len);

    INT decreased_line_count = total_lines_update_();

    cursor_pos_update_(GX_TRUE);

    dirty_rect_mark_(old_scroll_shift, cursor.pos.y, decreased_line_count);

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::backspace_()
{
    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    if (end_mark_ < start_mark_)
    {
        return text_delete_();
    }

    UINT insert_pos = text_insert_position;
    if (insert_pos == 0)
    {
        return GX_SUCCESS;
    }

    UINT glyph_len = 1;
    if (start_mark_ != end_mark_)
    {
        glyph_len = (UINT)GX_ABS((INT)start_mark - (INT)end_mark);

        this->start_mark = 0;
        this->end_mark = 0;
    }
    else
    {
        cursor_visible_();

#ifdef GX_UTF8_SUPPORT
        _gx_utility_utf8_string_backward_character_length_get(&text_(), (INT)(insert_pos - 1), &glyph_len);
#endif
        if (gx_is_new_line_(text_().ptr[insert_pos - glyph_len]))
        {
            glyph_len = new_line_character_size;
        }
    }

    INT old_scroll_shift = text_scroll_shift;

    text_delete_(insert_pos - glyph_len, glyph_len);

    INT decreased_line_count = total_lines_update_();

    text_insert_position -= glyph_len;
    cursor_pos_update_(GX_TRUE);

    dirty_rect_mark_(old_scroll_shift, cursor.pos.y, decreased_line_count);

    return GX_SUCCESS;
}

INT GX_MULTI_LINE_TEXT_INPUT::total_lines_update_()
{
    UINT old_text_total_lines = text_total_lines;

    total_lines_compute_();

    INT decreased_line_count = (INT)old_text_total_lines - (INT)text_total_lines;
    if (decreased_line_count)
    {
        scrollbar_reset_or_line_cache_update_();
    }

    return decreased_line_count;
}

void GX_MULTI_LINE_TEXT_INPUT::dirty_rect_mark_(INT old_scroll_shift, GX_VALUE cursor_pos_y, INT decreased_line_count)
{
    if (old_scroll_shift != text_scroll_shift)
    {
        dirty_mark_();
        return;
    }

    GX_FONT* font = GX_NULL;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return;
    }

    GX_VALUE line_height = (GX_VALUE)(font->line_height + line_space);
    if (line_height <= 0)
    {
        return;
    }

    GX_RECTANGLE input_area;
    display_area_(&input_area);

    GX_RECTANGLE rect = input_area;

    GX_VALUE y_pos = (GX_VALUE)(cursor_pos_y - line_height / 2);
    rect.top = (GX_VALUE)(y_pos - line_height);

    INT first_visible_line = -text_scroll_shift / line_height;
    if (first_visible_line < 0)
    {
        first_visible_line = 0;
    }

    INT last_visible_line = first_visible_line + (INT)text_visible_lines + 1;
    if (last_visible_line > (INT)text_total_lines)
    {
        last_visible_line = (INT)text_total_lines;
    }

    INT bottom = input_area.top + old_scroll_shift;
    bottom += (last_visible_line + decreased_line_count) * line_height;
    if (bottom < rect.bottom)
    {
        rect.bottom = (GX_VALUE)bottom;
    }

    dirty_mark_(&rect);
}

UINT GX_MULTI_LINE_TEXT_INPUT::up_arrow_()
{
    GX_FONT* font = GX_NULL;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return GX_FAILURE;
    }

    GX_VALUE line_height = (GX_VALUE)(font->line_height + line_space);
    if (line_height <= 0)
    {
        return GX_FAILURE;
    }

    if (this->start_mark != this->end_mark)
    {
        left_arrow_();
    }

    GX_RECTANGLE rect;
    cursor.dirty_rectangle_get_(&rect);

    INT old_scroll_shift = text_scroll_shift;

    GX_POINT cursor_pos = cursor.pos;
    cursor_pos.y = (GX_VALUE)(cursor_pos.y - line_height); // Up
    cursor_pos_calculate_(cursor_pos);

    if (old_scroll_shift == text_scroll_shift)
    {
        dirty_mark_(&rect);

        cursor.dirty_rectangle_get_(&rect);
        dirty_mark_(&rect);
    }
    else
    {
        dirty_mark_();
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::down_arrow_()
{
    GX_FONT* font = GX_NULL;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return GX_FAILURE;
    }

    GX_VALUE line_height = (GX_VALUE)(font->line_height + line_space);
    if (line_height <= 0)
    {
        return GX_FAILURE;
    }

    if (this->start_mark != this->end_mark)
    {
        right_arrow_();
    }

    GX_RECTANGLE rect;
    cursor.dirty_rectangle_get_(&rect);

    INT old_scroll_shift = text_scroll_shift;

    GX_POINT cursor_pos = cursor.pos;
    cursor_pos.y = (GX_VALUE)(cursor_pos.y + line_height); // Down
    cursor_pos_calculate_(cursor_pos);

    if (old_scroll_shift == text_scroll_shift)
    {
        dirty_mark_(&rect);

        cursor.dirty_rectangle_get_(&rect);
        dirty_mark_(&rect);
    }
    else
    {
        dirty_mark_();
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::left_arrow_()
{
    GX_RECTANGLE rect;

    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    if (start_mark_ != end_mark_)
    {
        if (end_mark_ < start_mark_)
        {
            cursor_visible_();

            highlight_rectangle_get_(&rect);
        }
        else // end_mark_ > start_mark_
        {
            text_insert_position = start_mark_;

            GX_POINT old_cursor_pos = cursor.pos;
            INT old_scroll_shift = text_scroll_shift;

            cursor_pos_update_(GX_TRUE);

            if (old_scroll_shift == text_scroll_shift)
            {
                text_rectangle_get_(cursor.pos, old_cursor_pos, &rect);
            }
            else
            {
                rect = size;
            }
        }

        this->start_mark = 0;
        this->end_mark = 0;

        dirty_mark_(&rect);

        cursor.dirty_rectangle_get_(&rect);
        dirty_mark_(&rect);
    }
    else
    {
        INT insert_pos = text_insert_position;
        if (insert_pos <= 0)
        {
            return GX_SUCCESS;
        }

        cursor_visible_();

        UINT glyph_len = 1;

#ifdef GX_UTF8_SUPPORT
        _gx_utility_utf8_string_backward_character_length_get(&text_(), (INT)(insert_pos - 1), &glyph_len);
#endif

        if (gx_is_new_line_(text_().ptr[insert_pos - glyph_len]))
        {
            glyph_len = new_line_character_size;
        }

        text_insert_position -= glyph_len;

        cursor.dirty_rectangle_get_(&rect);

        INT old_scroll_shift = text_scroll_shift;

        cursor_pos_update_(GX_TRUE);

        if (old_scroll_shift == text_scroll_shift)
        {
            dirty_mark_(&rect);

            cursor.dirty_rectangle_get_(&rect);
            dirty_mark_(&rect);
        }
        else
        {
            dirty_mark_();
        }
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::right_arrow_()
{
    GX_RECTANGLE rect;

    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    if (start_mark_ != end_mark_)
    {
        if (end_mark_ > start_mark_)
        {
            cursor_visible_();

            highlight_rectangle_get_(&rect);
        }
        else // end_mark_ < start_mark_
        {
            text_insert_position = start_mark_;

            GX_POINT old_cursor_pos = cursor.pos;
            INT old_scroll_shift = text_scroll_shift;

            cursor_pos_update_(GX_TRUE);

            if (old_scroll_shift == text_scroll_shift)
            {
                text_rectangle_get_(cursor.pos, old_cursor_pos, &rect);
            }
            else
            {
                rect = size;
            }
        }

        this->start_mark = 0;
        this->end_mark = 0;

        dirty_mark_(&rect);

        cursor.dirty_rectangle_get_(&rect);
        dirty_mark_(&rect);
    }
    else
    {
        UINT insert_pos = text_insert_position;
        if (insert_pos >= text_().length)
        {
            return GX_SUCCESS;
        }

        UINT glyph_len = 1;

        if (gx_is_new_line_(text_().ptr[insert_pos]))
        {
            glyph_len = new_line_character_size;
        }
#ifdef GX_UTF8_SUPPORT
        else
        {
            GX_STRING c;
            c.ptr = text_().ptr + insert_pos;
            c.length = text_().length - insert_pos;
            _gx_utility_utf8_string_character_get(&c, GX_NULL, &glyph_len);
        }
#endif

        text_insert_position += glyph_len;

        cursor.dirty_rectangle_get_(&rect);

        INT old_scroll_shift = text_scroll_shift;

        cursor_pos_update_(GX_TRUE);

        if (old_scroll_shift == text_scroll_shift)
        {
            dirty_mark_(&rect);

            cursor.dirty_rectangle_get_(&rect);
            dirty_mark_(&rect);
        }
        else
        {
            dirty_mark_();
        }
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::home_()
{
    if (this->start_mark != this->end_mark)
    {
        left_arrow_();
    }

    GX_POINT cursor_pos = cursor.pos;

    GX_RECTANGLE cursor_rect;
    cursor.dirty_rectangle_get_(&cursor_rect);

    cursor_pos.x = (GX_VALUE)(client.left + margin);

    cursor_pos_calculate_(cursor_pos);

    if (cursor_rect.left != cursor.pos.x)
    {
        dirty_mark_(&cursor_rect); // mark OLD cursor dirty

        cursor.dirty_rectangle_get_(&cursor_rect);
        dirty_mark_(&cursor_rect); // mark NEW cursor dirty
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::end_()
{
    if (this->start_mark != this->end_mark)
    {
        right_arrow_();
    }

    GX_POINT cursor_pos = cursor.pos;

    GX_RECTANGLE cursor_rect;
    cursor.dirty_rectangle_get_(&cursor_rect);

    cursor_pos.x = (GX_VALUE)(client.right - margin);

    cursor_pos_calculate_(cursor_pos);

    // move the insert position to the end of cursor line
    UINT cache_index = text_cursor_line - 1 - first_cache_line;
    text_insert_position = line_index_cache_read_(cache_index + 1);

    if (cursor_rect.left != cursor.pos.x)
    {
        dirty_mark_(&cursor_rect); // mark OLD cursor dirty

        cursor.dirty_rectangle_get_(&cursor_rect);
        dirty_mark_(&cursor_rect); // mark NEW cursor dirty
    }

    return GX_SUCCESS;
}

void GX_MULTI_LINE_TEXT_INPUT::key_down_process_(GX_EVENT* event_ptr)
{
    GX_UBYTE utf8_str[10];

    USHORT key_value = event_ptr->payload.ushortdata[0];
    switch (key_value)
    {
    case GX_KEY_BACKSPACE:
        backspace_();
        break;

    case GX_KEY_DELETE:
        text_delete_();
        break;

    case GX_KEY_UP_ARROW:
        up_arrow_();
        break;

    case GX_KEY_DOWN_ARROW:
        down_arrow_();
        break;

    case GX_KEY_LEFT_ARROW:
        left_arrow_();
        break;

    case GX_KEY_RIGHT_ARROW:
        right_arrow_();
        break;

    case GX_KEY_HOME:
        home_();
        break;

    case GX_KEY_END:
        end_();
        break;

    case GX_KEY_SPACE:
        utf8_str[0] = ' ';
        text_insert_(utf8_str, 1);
        break;

    case '\r':
    case '\n':
        utf8_str[0] = (GX_UBYTE)key_value;
        text_insert_(utf8_str, 1);
        break;

    default:
        if (key_value < 0x20 ||
            (key_value >= GX_KEY_PAGE_UP && key_value <= GX_KEY_SEPARATOR))
        {
            GX_WIDGET::event_process_(event_ptr);
            return;
        }

        if (key_value < 0x7F)
        {
            utf8_str[0] = (GX_UBYTE)key_value;
            text_insert_(utf8_str, 1);
        }
#ifdef GX_UTF8_SUPPORT
        else
        {
            UINT str_size = 0;
            _gx_utility_unicode_to_utf8(key_value, utf8_str, &str_size);
            text_insert_(utf8_str, str_size);
        }
#endif
        break;
    }
}

void GX_MULTI_LINE_TEXT_INPUT::pen_down_process_(GX_EVENT* event_ptr)
{
    _gx_system_input_capture(this);

    INT old_scroll_shift = text_scroll_shift;

    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    GX_RECTANGLE rect;

    if (start_mark_ != end_mark_)
    {
        highlight_rectangle_get_(&rect);
    }
    else
    {
        cursor.dirty_rectangle_get_(&rect);
    }

    cursor_pos_calculate_(event_ptr->payload.pointdata);

    this->start_mark = text_insert_position;
    this->end_mark = text_insert_position;

    if (old_scroll_shift != text_scroll_shift)
    {
        dirty_mark_();
    }
    else
    {
        dirty_mark_(&rect);

        if (start_mark_ == end_mark_)
        {
            cursor.dirty_rectangle_get_(&rect);
            dirty_mark_(&rect);
        }
    }

    GX_WINDOW::event_process_(event_ptr);
}

void GX_MULTI_LINE_TEXT_INPUT::pen_drag_process_(GX_EVENT* event_ptr)
{
    if (this->status & GX_STATUS_OWNS_INPUT)
    {
        GX_POINT click_pos = event_ptr->payload.pointdata;
        GX_VALUE click_y_pos = click_pos.y;

        if (this->start_mark == this->end_mark)
        {
            GX_RECTANGLE rect;
            cursor.dirty_rectangle_get_(&rect);
            dirty_mark_(&rect);
        }

        GX_POINT old_cursor_pos = cursor.pos;
        INT old_scroll_shift = text_scroll_shift;
        UINT old_end_mark = this->end_mark;

        if (click_pos.y < client.top)
        {
            click_pos.y = client.top;
        }
        else if (click_pos.y > client.bottom)
        {
            click_pos.y = client.bottom;
        }

        cursor_pos_calculate_(click_pos);

        this->end_mark = text_insert_position;

        if (click_y_pos < client.top && text_cursor_line > 1)
        {
            if (!(this->status & (GX_STATUS_MARK_NEXT | GX_STATUS_MARK_PREVIOUS)))
            {
                timer_start_(GX_MARK_TIMER, GX_MARK_INTERVAL, GX_MARK_INTERVAL);
            }

            status_remove_(GX_STATUS_MARK_NEXT);
            status_add_(GX_STATUS_MARK_PREVIOUS);
        }
        else if (click_y_pos > client.bottom && text_cursor_line < text_total_lines)
        {
            if (!(this->status & (GX_STATUS_MARK_NEXT | GX_STATUS_MARK_PREVIOUS)))
            {
                timer_start_(GX_MARK_TIMER, GX_MARK_INTERVAL, GX_MARK_INTERVAL);
            }

            status_remove_(GX_STATUS_MARK_PREVIOUS);
            status_add_(GX_STATUS_MARK_NEXT);
        }
        else
        {
            if (this->status & (GX_STATUS_MARK_NEXT | GX_STATUS_MARK_PREVIOUS))
            {
                timer_stop_(GX_MARK_TIMER);
            }
        }

        if (old_scroll_shift != text_scroll_shift)
        {
            dirty_mark_();
        }
        else
        {
            if (this->start_mark != this->end_mark ||
                this->start_mark != old_end_mark)
            {
                GX_RECTANGLE rect;
                text_rectangle_get_(old_cursor_pos, cursor.pos, &rect);
                dirty_mark_(&rect);
            }
        }
    }
    else
    {
        GX_WINDOW::event_process_(event_ptr);
    }
}

void GX_MULTI_LINE_TEXT_INPUT::pen_up_process_(GX_EVENT* event_ptr)
{
    if (this->status & GX_STATUS_OWNS_INPUT)
    {
        _gx_system_input_release(this);

        if (this->status & (GX_STATUS_MARK_NEXT | GX_STATUS_MARK_PREVIOUS))
        {
            timer_stop_(GX_MARK_TIMER);
            status_remove_(GX_STATUS_MARK_NEXT | GX_STATUS_MARK_PREVIOUS);
        }
    }

    GX_WINDOW::event_process_(event_ptr);
}

void GX_MULTI_LINE_TEXT_INPUT::text_select_(UINT start_index, UINT end_index)
{
    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    if (start_index <= end_index)
    {
        this->start_mark = start_index;
        this->end_mark = end_index + 1;
    }
    else
    {
        this->start_mark = start_index + 1;
        this->end_mark = end_index;
    }

    if (is_visible_())
    {
        GX_RECTANGLE rect;

        if (start_mark_ != end_mark_)
        {
            highlight_rectangle_get_(&rect);
            dirty_mark_(&rect);
        }
        else
        {
            cursor.dirty_rectangle_get_(&rect);
            dirty_mark_(&rect);
        }

        start_mark_ = this->start_mark;
        end_mark_ = this->end_mark;

        if (text_insert_position != end_mark_)
        {
            text_insert_position = end_mark_;
            cursor_pos_update_(GX_FALSE);
        }

        highlight_rectangle_get_(&rect);
        dirty_mark_(&rect);
    }
}

UINT GX_MULTI_LINE_TEXT_INPUT::mark_up_()
{
    if (this->start_mark == this->end_mark)
    {
        this->start_mark = text_insert_position;
        this->end_mark = text_insert_position;
    }

    GX_FONT* font = GX_NULL;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return GX_FAILURE;
    }

    INT line_height = font->line_height + line_space;
    if (line_height <= 0)
    {
        return GX_FAILURE;
    }

    INT old_scroll_shift = text_scroll_shift;

    GX_POINT cursor_pos = cursor.pos;
    cursor_pos.y = (GX_VALUE)(cursor_pos.y - line_height); // Up
    cursor_pos_calculate_(cursor_pos);

    this->end_mark = text_insert_position;

    if (old_scroll_shift == text_scroll_shift)
    {
        GX_RECTANGLE rect;
        client_area_get_(&rect);

        GX_VALUE half_line_height = (GX_VALUE)((line_height + 1) / 2);

        rect.top = (GX_VALUE)(cursor_pos.y - half_line_height);
        rect.bottom = (GX_VALUE)(cursor_pos.y + line_height + half_line_height);

        dirty_mark_(&rect);
    }
    else
    {
        dirty_mark_();
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::mark_down_()
{
    if (this->start_mark == this->end_mark)
    {
        this->start_mark = text_insert_position;
        this->end_mark = text_insert_position;
    }

    GX_FONT* font = GX_NULL;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return GX_FAILURE;
    }

    INT line_height = font->line_height + line_space;
    if (line_height <= 0)
    {
        return GX_FAILURE;
    }

    INT old_scroll_shift = text_scroll_shift;

    GX_POINT cursor_pos = cursor.pos;
    cursor_pos.y = (GX_VALUE)(cursor_pos.y + line_height); // Down
    cursor_pos_calculate_(cursor_pos);

    this->end_mark = text_insert_position;

    if (old_scroll_shift == text_scroll_shift)
    {
        GX_RECTANGLE rect;
        client_area_get_(&rect);

        GX_VALUE half_line_height = (GX_VALUE)((line_height + 1) / 2);

        rect.top = (GX_VALUE)(cursor_pos.y - line_height - half_line_height);
        rect.bottom = (GX_VALUE)(cursor_pos.y + half_line_height);

        dirty_mark_(&rect);
    }
    else
    {
        dirty_mark_();
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::mark_previous_()
{
    if (this->start_mark == this->end_mark)
    {
        this->start_mark = text_insert_position;
        this->end_mark = text_insert_position;
    }

    UINT end_mark_ = this->end_mark;
    if (end_mark_ == 0)
    {
        return GX_SUCCESS;
    }

    UINT glyph_len = 1;

#ifdef GX_UTF8_SUPPORT
    _gx_utility_utf8_string_backward_character_length_get(&text_(), (INT)(end_mark - 1), &glyph_len);
#endif

    GX_CHAR c = text_().ptr[end_mark_ - glyph_len];
    if (gx_is_new_line_(c))
    {
        glyph_len = new_line_character_size;
    }

    this->end_mark -= glyph_len; // Previous

    this->text_insert_position = this->end_mark;

    INT old_scroll_shift = text_scroll_shift;
    GX_POINT old_cursor_pos = cursor.pos;

    cursor_pos_update_(GX_TRUE);

    if (old_scroll_shift == text_scroll_shift)
    {
        GX_RECTANGLE rect;
        text_rectangle_get_(old_cursor_pos, cursor.pos, &rect);
        dirty_mark_(&rect);
    }
    else
    {
        dirty_mark_();
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::mark_next_()
{
    if (this->start_mark == this->end_mark)
    {
        this->start_mark = text_insert_position;
        this->end_mark = text_insert_position;
    }

    UINT end_mark_ = this->end_mark;
    if (end_mark_ >= text_().length)
    {
        return GX_SUCCESS;
    }

    UINT glyph_len = 1;

#ifdef GX_UTF8_SUPPORT

    GX_STRING string;
    string.ptr = text_().ptr + end_mark_;
    string.length = text_().length - end_mark_;
    _gx_utility_utf8_string_character_get(&string, GX_NULL, &glyph_len);

#endif

    GX_CHAR c = text_().ptr[end_mark_ ];
    if (gx_is_new_line_(c))
    {
        glyph_len = new_line_character_size;
    }

    this->end_mark += glyph_len; // Next

    this->text_insert_position = this->end_mark;

    INT old_scroll_shift = text_scroll_shift;
    GX_POINT old_cursor_pos = cursor.pos;

    cursor_pos_update_(GX_TRUE);

    if (old_scroll_shift == text_scroll_shift)
    {
        GX_RECTANGLE rect;
        text_rectangle_get_(old_cursor_pos, cursor.pos, &rect);
        dirty_mark_(&rect);
    }
    else
    {
        dirty_mark_();
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::mark_home_()
{
    if (this->start_mark == this->end_mark)
    {
        this->start_mark = text_insert_position;
        this->end_mark = text_insert_position;
    }

    GX_POINT old_cursor_pos = cursor.pos;

    INT old_scroll_shift = text_scroll_shift;

    GX_POINT cursor_pos;
    cursor_pos.x = (GX_VALUE)(client.left + margin);
    cursor_pos.y = old_cursor_pos.y;

    cursor_pos_calculate_(cursor_pos);

    this->end_mark = text_insert_position;

    if (old_scroll_shift == text_scroll_shift)
    {
        GX_RECTANGLE rect;
        text_rectangle_get_(old_cursor_pos, cursor.pos, &rect);
        dirty_mark_(&rect);
    }
    else
    {
        dirty_mark_();
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::mark_end_()
{
    if (this->start_mark == this->end_mark)
    {
        this->start_mark = text_insert_position;
        this->end_mark = text_insert_position;
    }

    GX_POINT old_cursor_pos = cursor.pos;

    INT old_scroll_shift = text_scroll_shift;

    GX_POINT cursor_pos;
    cursor_pos.x = (GX_VALUE)(client.right - margin); // End
    cursor_pos.y = old_cursor_pos.y;

    cursor_pos_calculate_(cursor_pos);

    this->end_mark = text_insert_position;

    if (old_scroll_shift == text_scroll_shift)
    {
        GX_RECTANGLE rect;
        text_rectangle_get_(old_cursor_pos, cursor.pos, &rect);
        dirty_mark_(&rect);
    }
    else
    {
        dirty_mark_();
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::copy_()
{
    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    if (start_mark_ != end_mark_)
    {
        if (start_mark_ > end_mark_)
        {
            GX_SWAP_VALS(start_mark_, end_mark_);
        }

        _gx_system_clipboard_put(const_cast<GX_CHAR*>(text_().ptr) + start_mark_, end_mark_ - start_mark_);
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::cut_()
{
    copy_();
    selected_text_delete_();

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::paste_()
{
    GX_UBYTE* str = GX_NULL;
    UINT str_size;

    _gx_system_clipboard_get((void**)&str, &str_size);

    if (str)
    {
        text_insert_(str, str_size);
    }

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::highlight_rectangle_get_(GX_RECTANGLE* rect)
{
    GX_FONT* font = GX_NULL;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return GX_FAILURE;
    }

    INT line_height = font->line_height + line_space;
    if (line_height <= 0)
    {
        return GX_FAILURE;
    }

    INT first_visible_line = -text_scroll_shift / line_height;
    if (first_visible_line < 0)
    {
        first_visible_line = 0;
    }

    INT last_visible_line = first_visible_line + (INT)text_visible_lines;
    if (last_visible_line > (INT)(text_total_lines - 1))
    {
        last_visible_line = (INT)text_total_lines - 1;
    }

    GX_POINT start_pos;
    GX_POINT end_pos;

    if (this->start_mark <= line_index_cache[first_visible_line - first_cache_line])
    {
        start_pos.x = left_();
        start_pos.y = top_();
    }
    else if (this->start_mark >= line_index_cache[last_visible_line - first_cache_line])
    {
        start_pos.x = left_();
        start_pos.y = bottom_();
    }
    else
    {
        end_pos = cursor.pos;

        UINT old_cursor_line = text_cursor_line;

        text_insert_position = this->start_mark;
        cursor_pos_update_(GX_FALSE);

        start_pos = cursor.pos;

        text_insert_position = this->end_mark;
        text_cursor_line = old_cursor_line;

        cursor.pos = end_pos;
    }

    text_rectangle_get_(start_pos, cursor.pos, rect);

    return GX_SUCCESS;
}

UINT GX_MULTI_LINE_TEXT_INPUT::text_rectangle_get_(GX_POINT start_cursor_pos, GX_POINT end_cursor_pos, GX_RECTANGLE* rect)
{
    GX_FONT* font = GX_NULL;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return GX_FAILURE;
    }

    GX_VALUE line_height = (GX_VALUE)(font->line_height + line_space);
    GX_VALUE half_line_height = (GX_VALUE)(line_height / 2);

    if (start_cursor_pos.y == end_cursor_pos.y)
    {
        if (start_cursor_pos.x > end_cursor_pos.x)
        {
            GX_SWAP_VALS(start_cursor_pos.x, end_cursor_pos.x);
        }

        rect->top = (GX_VALUE)(start_cursor_pos.y - half_line_height);
        rect->bottom = (GX_VALUE)(start_cursor_pos.y + half_line_height);
        rect->left = (GX_VALUE)(start_cursor_pos.x - cursor.width / 2);
        rect->right = (GX_VALUE)(end_cursor_pos.x + (cursor.width + 1) / 2 - 1);
    }
    else
    {
        if (start_cursor_pos.y > end_cursor_pos.y)
        {
            GX_SWAP_VALS(start_cursor_pos.y, end_cursor_pos.y);
        }

        client_area_get_(rect);

        rect->top = (GX_VALUE)(start_cursor_pos.y - half_line_height);
        rect->bottom = (GX_VALUE)(end_cursor_pos.y + half_line_height);
    }

    return GX_SUCCESS;
}

void GX_MULTI_LINE_TEXT_INPUT::draw_()
{
    GX_RESOURCE_ID fill_color;
    GX_RESOURCE_ID text_color;

    if (is_enabled_())
    {
        if (style & GX_STYLE_TEXT_INPUT_READONLY)
        {
            fill_color = readonly_fill_color;
            text_color = readonly_text_color;
        }
        else
        {
            fill_color = normal_fill_color;
            text_color = normal_text_color_();
        }
    }
    else
    {
        fill_color = disabled_fill_color;
        text_color = disabled_text_color_();
    }

    background_draw_(fill_color);

    if (line_index_cache_old)
    {
        visible_lines_compute_();
        total_lines_compute_();
        scrollbar_reset_or_line_cache_update_();
        cursor_pos_update_(GX_TRUE);
    }

    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

    GX_FONT* font = GX_NULL;
    context->font_get_(font_id_(), &font);
    if (!font)
    {
        return;
    }

    GX_VALUE line_height = (GX_VALUE)(font->line_height + line_space);
    if (line_height <= 0)
    {
        return;
    }

    if (this->start_mark == this->end_mark || !is_enabled_())
    {
        text_draw_(text_color);

        if ((this->status & GX_STATUS_CURSOR_SHOW) &&
            (this->status & GX_STATUS_CURSOR_DRAW))
        {
            if (!(cursor.flags & GX_CURSOR_USE_CUSTOM_HEIGHT))
            {
                cursor.height = line_height;
            }

            cursor.draw_();
        }

        return;
    }

    if (!text_().ptr || !text_().length)
    {
        return;
    }

    GX_RECTANGLE input_area;
    display_area_(&input_area);

    GX_RECTANGLE draw_area;
    if (gx_rectangle_intersect_(input_area, context->dirty, &draw_area))
    {
        GX_DRAWING drawing(context->canvas, this, draw_area);
        if (drawing.result_() != GX_SUCCESS)
        {
            return;
        }

        context = _gx_system_current_draw_context;
        context->font_set_(font_id_());
        context->brush.width = 0;

        UINT line_start_mark = this->start_mark;
        UINT end_mark_ = this->end_mark;

        if (line_start_mark > end_mark_)
        {
            GX_SWAP_VALS(line_start_mark, end_mark_);
        }

        UINT line_end_mark;

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
        if (last_visible_line > text_total_lines - 1)
        {
            last_visible_line = text_total_lines - 1;
        }

        INT y_pos = input_area.top + text_scroll_shift;
        y_pos += (INT)first_visible_line * line_height;
        y_pos += line_space / 2;

        const GX_VALUE x_right = gx_text_area_right_(input_area);

        GX_STRING string;
        _gx_system_private_string_get(&text_(), &string, style);

        UINT line_start_index;
        UINT line_end_index;

        GX_STRING draw_string;
        GX_VALUE text_width;

        for (UINT index = first_visible_line; index <= last_visible_line; ++index)
        {
            line_start_and_end_index_get_(index - first_cache_line, &line_start_index, &line_end_index);

            INT x_pos = cursor_x_pos_get_(line_start_index, line_end_index, font, input_area);

            if ((line_start_mark < line_end_index) && (end_mark_ > line_start_index))
            {
                if (line_start_mark < line_start_index)
                {
                    line_start_mark = line_start_index;
                }

                // Draw text[line_start_index : start_mark - 1] with normal text color.

                if (line_start_mark > line_start_index)
                {
                    context->line_color_set_(text_color);

                    draw_string.ptr = string.ptr + line_start_index;
                    draw_string.length = line_start_mark - line_start_index;
                    _gx_system_string_width_get_ext(font, &draw_string, &text_width);

                    _gx_canvas_text_draw_ext((GX_VALUE)x_pos, (GX_VALUE)y_pos, &draw_string);

                    x_pos += text_width;
                }

                line_end_mark = GX_MIN(end_mark_, line_end_index);

                // Draw text[start_mark: end_mark - 1] with hightlight text color.

                context->line_color_set_(selected_text_color_());
                context->fill_color_set_(selected_fill_color);

                draw_string.ptr = string.ptr + line_start_mark;
                draw_string.length = line_end_mark - line_start_mark;
                _gx_system_string_width_get_ext(font, &draw_string, &text_width);

                draw_area.left = (GX_VALUE)x_pos;
                draw_area.right = (GX_VALUE)(x_pos + text_width - 1);
                draw_area.top = (GX_VALUE)(y_pos - line_space / 2);
                draw_area.bottom = (GX_VALUE)(draw_area.top + line_height - 1);

                draw_area.right = x_value_clamp_(draw_area.right, x_right);

                _gx_canvas_rectangle_draw(&draw_area);

                draw_string.ptr = string.ptr + line_start_mark;
                draw_string.length = line_end_mark - line_start_mark;
                _gx_canvas_text_draw_ext((GX_VALUE)x_pos, (GX_VALUE)y_pos, &draw_string);

                x_pos += text_width;

                // Draw text[end_mark : line_end_index] width normal text color.

                if (line_end_mark < line_end_index)
                {
                    context->line_color_set_(text_color);

                    draw_string.ptr = string.ptr + line_end_mark;
                    draw_string.length = line_end_index - line_end_mark;

                    _gx_canvas_text_draw_ext((GX_VALUE)x_pos, (GX_VALUE)y_pos, &draw_string);
                }

                if (end_mark_ > line_end_index)
                {
                    line_start_mark = line_end_index;
                }
            }
            else
            {
                context->line_color_set_(text_color);

                draw_string.ptr = string.ptr + line_start_index;
                draw_string.length = line_end_index - line_start_index;

                _gx_canvas_text_draw_ext((GX_VALUE)x_pos, (GX_VALUE)y_pos, &draw_string);
            }

            y_pos += line_height;
        }
    }

    children_draw_();
}

UINT GX_MULTI_LINE_TEXT_INPUT::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    GX_RECTANGLE rect;

    switch (event_ptr->type)
    {
    case GX_EVENT_DELETE:
        if (this->status & GX_STATUS_DYNAMIC_BUFFER)
        {
            if (!_gx_system_memory_free)
            {
                return GX_SYSTEM_MEMORY_ERROR;
            }

            _gx_system_memory_free((void *)text_().ptr);
            text_().ptr = GX_NULL;
        }
        break;

    case GX_EVENT_SHOW:
        GX_WINDOW::event_process_(event_ptr);

        cursor_pos_update_(GX_TRUE);

        if (style & GX_STYLE_CURSOR_ALWAYS)
        {
            cursor_blink_timer_start_();
        }

        visible_lines_compute_();
        break;

    case GX_EVENT_TIMER:
        if (event_ptr->payload.timer_id == GX_MARK_TIMER)
        {
            if (this->status & GX_STATUS_MARK_PREVIOUS)
            {
                mark_up_();
            }
            else
            {
                mark_down_();
            }
        }
        else if ((event_ptr->payload.timer_id == GX_CURSOR_BLINK_TIMER) &&
                 (this->status & GX_STATUS_CURSOR_SHOW) &&
                 (this->start_mark == this->end_mark))
        {
            if (this->status & GX_STATUS_CURSOR_DRAW)
            {
                status_remove_(GX_STATUS_CURSOR_DRAW);
            }
            else
            {
                status_add_(GX_STATUS_CURSOR_DRAW);
            }

            cursor.dirty_rectangle_get_(&rect);
            dirty_mark_(&rect);
        }
        break;

    case GX_EVENT_STYLE_CHANGED:
        if (is_visible_())
        {
            GX_MULTI_LINE_TEXT_VIEW::event_process_(event_ptr);

            ULONG old_style = event_ptr->payload.ulongdata;
            if ((old_style & (GX_STYLE_BORDER_MASK | GX_STYLE_TEXT_ALIGNMENT_MASK)) !=
                (this->style & (GX_STYLE_BORDER_MASK | GX_STYLE_TEXT_ALIGNMENT_MASK)))
            {
                line_index_cache_old = GX_TRUE;
            }
        }
        break;

    case GX_EVENT_LANGUAGE_CHANGE:
        break;

    case GX_EVENT_FOCUS_GAINED:
        GX_WINDOW::event_process_(event_ptr);

        // Do not do anything if the CURSOR_ALWAYS flag is set.
        if (!(style & GX_STYLE_CURSOR_ALWAYS))
        {
            status_add_(GX_STATUS_CURSOR_SHOW | GX_STATUS_CURSOR_DRAW);

            cursor_blink_timer_start_();
        }

        text_select_(0, text_().length - 1);
        break;

    case GX_EVENT_FOCUS_LOST:
        GX_WINDOW::event_process_(event_ptr);

        /* Do not do anything if the CURSOR_ALWAYS flag is set. */
        if (!(style & GX_STYLE_CURSOR_ALWAYS))
        {
            status_remove_(GX_STATUS_CURSOR_SHOW);

            if (style & GX_STYLE_CURSOR_BLINK)
            {
                timer_stop_(0);
            }

            cursor.dirty_rectangle_get_(&rect);
            dirty_mark_(&rect);
        }

        if (this->start_mark != this->end_mark)
        {
            highlight_rectangle_get_(&rect);

            this->start_mark = 0;
            this->end_mark = 0;

            dirty_mark_(&rect);
        }

        if (text_was_modified)
        {
            signal_parent_(GX_EVENT_TEXT_EDITED, 0);
            text_was_modified = GX_FALSE;
        }
        break;

    case GX_EVENT_VERTICAL_SCROLL:
        {
            INT scroll_shift = event_ptr->payload.intdata[1] - event_ptr->payload.intdata[0];
            cursor.pos.y = (GX_VALUE)(cursor.pos.y + scroll_shift);
            GX_MULTI_LINE_TEXT_VIEW::event_process_(event_ptr);
        }
        break;

    case GX_EVENT_KEY_DOWN:
        key_down_process_(event_ptr);
        if (style & GX_STYLE_TEXT_INPUT_NOTIFY_ALL)
        {
            signal_parent_(GX_EVENT_TEXT_EDITED, 0);
        }
        break;

    case GX_EVENT_PEN_DOWN:
        pen_down_process_(event_ptr);
        break;

    case GX_EVENT_PEN_DRAG:
        pen_drag_process_(event_ptr);
        break;

    case GX_EVENT_PEN_UP:
        pen_up_process_(event_ptr);
        break;

    case GX_EVENT_COPY:
        copy_();
        break;

    case GX_EVENT_CUT:
        cut_();
        break;

    case GX_EVENT_PASTE:
        paste_();
        break;

    case GX_EVENT_MARK_UP:
        mark_up_();
        break;

    case GX_EVENT_MARK_DOWN:
        mark_down_();
        break;

    case GX_EVENT_MARK_PREVIOUS:
        mark_previous_();
        break;

    case GX_EVENT_MARK_NEXT:
        mark_next_();
        break;

    case GX_EVENT_MARK_HOME:
        mark_home_();
        break;

    case GX_EVENT_MARK_END:
        mark_end_();
        break;

    default:
        result = GX_MULTI_LINE_TEXT_VIEW::event_process_(event_ptr);
        break;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_backspace                                 */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deletes a character before the cursor.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input widget  */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_font_get                   Retrieve font                 */
/*    _gx_utility_rectangle_resize          Increase/Shrink rectangle with*/
/*                                            specified value             */
/*    _gx_utility_utf8_string_character_get Parses utf8 string to         */
/*                                            multi-byte glyph            */
/*    gx_string_length_get_       Test string length            */
/*    _gx_window_scrollbar_find             Find scrollbar for a window   */
/*    _gx_window_client_width_get           Get client width              */
/*    _gx_scrollbar_reset                   Reset scrollbar information   */
/*    _gx_multi_line_text_view_line_cache_update                          */
/*                                          Update line cache             */
/*    _gx_multi_line_text_view_display_info_get                           */
/*                                          Get the number of characters  */
/*                                            that a line can display     */
/*    _gx_multi_line_text_input_char_remove Remove a character            */
/*    _gx_multi_line_text_input_cursor_pos_update                         */
/*                                          Update cursor position        */
/*                                            according to insert index   */
/*    _gx_multi_line_text_view_string_total_rows_compute                  */
/*    _gx_system_string_width_get           Get string width              */
/*    _gx_system_dirty_partial_add          Add one dirty area to dirty   */
/*                                            list                        */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_multi_line_text_input_keydown_process                           */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_backspace(GX_MULTI_LINE_TEXT_INPUT* text_input)
{
    return text_input->backspace_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_buffer_clear                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deletes all characters from the text input buffer.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi-line text input widget  */
/*                                            control blcok               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_multi_line_text_input_char_remove  Remove characters from input  */
/*                                            buffer                      */
/*   _gx_multi_line_text_view_string_total_rows_compute                   */
/*                                          Calculate total rows          */
/*   _gx_multi_line_text_input_cursor_pos_update                          */
/*                                          Update cursor position        */
/*   _gx_window_scrollbar_find              Find scrollbar for a window   */
/*   _gx_scrollbar_reset                    Reset scroll bar information  */
/*   _gx_system_dirty_mark                  Mark the widget area as dirty */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_buffer_clear(GX_MULTI_LINE_TEXT_INPUT* text_input)
{
    text_input->buffer_clear_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_buffer_get                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves buffer infomation of a multi-line text input*/
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi-line text input widget  */
/*                                            control block               */
/*    buffer_address                        The address of the input      */
/*                                            buffer                      */
/*    content_size                          The byte count of the input   */
/*                                            data                        */
/*    buffer_size                           The size of the input buffer. */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_buffer_get(GX_MULTI_LINE_TEXT_INPUT* text_input, GX_CHAR** buffer_address,
                                          UINT* content_size, UINT* buffer_size)
{
    text_input->buffer_get_(buffer_address, content_size, buffer_size);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_char_insert_ext                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function inserts a character into the input buffer.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input widget  */
/*                                            control block               */
/*    str                                   Utf8 string to be inserted    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memmove                               Move block of memory          */
/*    _gx_widget_font_get                   Retrieve font                 */
/*    _gx_window_scrollbar_find             Find scrollbar for a window   */
/*    _gx_scrollbar_reset                   Reset scrollbar information   */
/*    _gx_multi_line_text_view_string_total_rows_compute                  */
/*                                          Calculate total rows          */
/*    _gx_multi_line_text_view_line_cache_update                          */
/*                                          Update line cache             */
/*    _gx_multi_line_text_input_cursor_pos_update                         */
/*                                          Update cursor position        */
/*                                            according to insert index   */
/*    _gx_utility_rectangle_resize          Increase/Shrink rectangle by  */
/*                                            specified value             */
/*    gx_string_length_get_       Test string length            */
/*    _gx_system_dirty_partial_add          Add one dirty area to dirty   */
/*                                            list                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_multi_line_text_input_keydown_process                           */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_char_insert_ext(GX_MULTI_LINE_TEXT_INPUT* text_input, const GX_STRING* str)
{
    return text_input->text_insert_((GX_UBYTE*)str->ptr, str->length);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_create                                    */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a multi-line text input widget.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                          Multi-line input widget control */
/*                                          block                         */
/*    name                                Name of text input widget       */
/*    parent                              Pointer to parent widget        */
/*    input_buffer                        Pointer to text input buffer    */
/*    buffer_size                         Size of text input buffer in    */
/*                                          bytes                         */
/*    style                               Style of text input widget      */
/*    Id                                  Application-defined ID of the   */
/*                                          text input widget             */
/*    Size                                Dimensions of text input widget */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_create     Create the multi line text view */
/*                                          widget                        */
/*    _gx_multi_line_text_view_text_set   Set the multi line text view    */
/*                                          text                          */
/*    _gx_widget_link                     Link the widget to its parent   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_create(GX_MULTI_LINE_TEXT_INPUT* text_input,
                                      const GX_CHAR* name,
                                      GX_WIDGET* parent,
                                      GX_CHAR* input_buffer,
                                      UINT buffer_size,
                                      ULONG style,
                                      USHORT Id,
                                      const GX_RECTANGLE* size)
{
    return text_input->create_(name, parent, input_buffer, buffer_size, style, Id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_cursor_pos_get                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves current cursor position.                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Multi line text input widget  */
/*                                            control block               */
/*    cursor_pos                            Retrieved cursor position     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_cursor_pos_get(GX_MULTI_LINE_TEXT_INPUT* text_input, GX_POINT* cursor_pos)
{
    *cursor_pos = text_input->cursor.pos;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_delete                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deletes a character after the cursor.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                        Multi line text input             */
/*                                        control block                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_font_get                   Retrieve font                 */
/*    _gx_window_client_width_get           Get the client width          */
/*    _gx_window_scrollbar_find             Find scrollbar for a window   */
/*    _gx_scrollbar_reset                   Reset scrollbar information   */
/*    _gx_multi_line_text_input_char_remove Remove characters from input  */
/*                                            string                      */
/*    _gx_multi_line_text_view_display_info_get                           */
/*                                          Get the number of characters  */
/*                                            that a line can display     */
/*    _gx_multi_line_text_view_string_total_rows_compute                  */
/*                                          Calculate total rows of input */
/*                                            string                      */
/*    _gx_multi_line_text_view_line_cache_update                          */
/*                                          Update line cache             */
/*    _gx_multi_line_text_input_cursor_pos_update                         */
/*                                          Update cursor position        */
/*                                            according to insert position*/
/*    _gx_system_dirty_partial_add          Add one dirty area to dirty   */
/*                                            list                        */
/*    _gx_utility_utf8_string_character_get Parses utf8 string to         */
/*                                            multi-byte glyph            */
/*    gx_string_length_get_       Test string length            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_multi_line_text_input_keydown_process                           */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_delete(GX_MULTI_LINE_TEXT_INPUT* text_input)
{
    return text_input->text_delete_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_down_arrow                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function move the cursor to down by one line.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                        Multi line text input             */
/*                                        control block                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_font_get                   Retrieve font                 */
/*    _gx_text_input_cursor_dirty_rectangle_get                           */
/*                                          Get cursor rectangle          */
/*    _gx_multi_line_text_input_cursor_pos_calculate                      */
/*                                          Update cursor position        */
/*                                            according to click position */
/*    _gx_system_dirty_partial_add          Add one dirty area to         */
/*                                            dirty list                  */
/*    _gx_system_dirty_mark                 Mark widget area dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_multi_line_text_input_keydown_process                           */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_down_arrow(GX_MULTI_LINE_TEXT_INPUT* text_input)
{
    return text_input->down_arrow_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_end                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deletes a character after the cursor.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                           Multi line text input          */
/*                                           control block                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_cursor_pos_calculate                      */
/*                                          Calculate cursor position     */
/*                                            according to click point    */
/*    _gx_multi_line_text_input_cursor_rectangle_define                   */
/*                                          Define a rectangle for the    */
/*                                            cursor                      */
/*    _gx_system_dirty_partial_add          Add one dirty area to         */
/*                                            dirty list                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_multi_line_text_input_keydown_process                           */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_end(GX_MULTI_LINE_TEXT_INPUT* text_input)
{
    return text_input->end_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_event_process                             */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes events for the specified text input widget. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input         */
/*                                            control block               */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_process              Default widget event process  */
/*    _gx_system_timer_start                Allocate a free timer and     */
/*                                          activates it                  */
/*    _gx_system_timer_stop                 Stop an active GUIX timer     */
/*    _gx_multi_line_text_input_keydown_process                           */
/*                                          Process the keydown event     */
/*    _gx_widget_event_generate             Generate an event             */
/*    _gx_text_input_cursor_dirty_rectangle_get                           */
/*                                          Get cursor rectangle          */
/*    _gx_multi_line_text_input_cursor_pos_calculate                      */
/*                                          Calculate cursor position     */
/*                                            according to click positin  */
/*    _gx_multi_line_text_input_cursor_pos_update                         */
/*                                          Update cursor position        */
/*                                            according to insert position*/
/*    _gx_system_dirty_partial_add          Add a dirty area to the       */
/*                                            specified widget            */
/*    _gx_multi_line_text_view_event_process                              */
/*                                          Invoke the text view event    */
/*                                            process routine             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_event_process(GX_MULTI_LINE_TEXT_INPUT* text_input, GX_EVENT* event_ptr)
{
    return text_input->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_fill_color_set                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the fill color of a mutli line text input widget. */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                Multi-line text input widget   */
/*                                            control block               */
/*    normal_text_color_id                  Resource ID of the normal     */
/*                                            text color                  */
/*    selected_text_color_id                Resource ID of the selected   */
/*                                            text color                  */
/*    disabled_text_color_id                Resource ID of the disabled   */
/*                                            text color                  */
/*    readonly_text_color_id                Resource ID of the read only  */
/*                                            text color                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_fill_color_set(GX_MULTI_LINE_TEXT_INPUT* text_input,
                                              GX_RESOURCE_ID normal_fill_color_id,
                                              GX_RESOURCE_ID selected_fill_color_id,
                                              GX_RESOURCE_ID disabled_fill_color_id,
                                              GX_RESOURCE_ID readonly_fill_color_id)
{
    text_input->fill_color_set_(normal_fill_color_id,
                                selected_fill_color_id,
                                disabled_fill_color_id,
                                readonly_fill_color_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_home                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function move the cursor to the position                       */
/*    before the first character.                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input         */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_cursor_pos_calculate                      */
/*                                          Calculate cursor position     */
/*                                            according to click point    */
/*    _gx_multi_line_text_input_cursor_rectangle_define                   */
/*                                          Define a rectangle for the    */
/*                                            cursor                      */
/*    _gx_system_dirty_partial_add          Add one dirty area to         */
/*                                            dirty list                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_multi_line_text_input_keydown_process                           */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_home(GX_MULTI_LINE_TEXT_INPUT* text_input)
{
    return text_input->home_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_left_arrow                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function move the cursor to left by one character.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                                Multi line text input     */
/*                                                control block           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_font_get                   Retrieve font                 */
/*    _gx_text_input_cursor_dirty_rectangle_get                           */
/*                                          Get cursor rectangle          */
/*    _gx_system_string_width_get           Get the pixel width of one    */
/*                                            specificed string           */
/*    _gx_multi_line_text_input_cursor_pos_update                         */
/*                                          Calculate cursor position     */
/*                                            according to insert index   */
/*    _gx_system_dirty_partial_add          Add one dirty area to         */
/*                                            dirty list                  */
/*    _gx_system_dirty_mark                 Mark widget area dirty        */
/*    _gx_utility_utf8_string_character_get Parses utf8 string to         */
/*                                            multi-byte glyph            */
/*    gx_string_length_get_       Test string length            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_multi_line_text_input_keydown_process                           */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_left_arrow(GX_MULTI_LINE_TEXT_INPUT* text_input)
{
    return text_input->left_arrow_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_right_arrow                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function move the cursor to right by one pixel.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input         */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_input_cursor_dirty_rectangle_get                           */
/*                                          Get cursor rectangle          */
/*    _gx_multi_line_text_input_cursor_pos_update                         */
/*                                          Calculate cursor position     */
/*                                            according to insert index   */
/*    _gx_system_dirty_partial_add          Add one dirty area to         */
/*                                            dirty list                  */
/*    _gx_system_dirty_mark                 Mark widget area dirty        */
/*    _gx_utility_utf8_string_character_get Parses utf8 string to         */
/*                                            multi-byte glyph            */
/*    gx_string_length_get_       Test string length            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_multi_line_text_input_keydown_process                           */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_right_arrow(GX_MULTI_LINE_TEXT_INPUT* text_input)
{
    return text_input->right_arrow_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_style_add                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function adds styles to a multi-line text input widget.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                             Multi-line text input widget */
/*                                             control block              */
/*    style                                  styles to add                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_timer_start                Allocate a free timer and     */
/*                                          activates it                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_style_add(GX_MULTI_LINE_TEXT_INPUT* text_input, ULONG style)
{
    text_input->style_add_(style);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_style_remove                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function removes styles from a multi-line text input widget.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                             Multi-lien text input widget */
/*                                             control block              */
/*    style                                  Styles to remove             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_timer_stop                 Stop an active GUIX timer     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_style_remove(GX_MULTI_LINE_TEXT_INPUT* text_input, ULONG style)
{
    text_input->style_remove_(style);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_style_set                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets styles for a multi-line text input widget.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                             Multi-line text input widget */
/*                                             control block              */
/*    style                                  Styles to set                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_timer_start                Allocate a free timer and     */
/*                                          activates it                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_style_set(GX_MULTI_LINE_TEXT_INPUT* text_input, ULONG style)
{
    text_input->style_set_(style);

    return GX_SUCCESS;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_text_color_set            PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the text color of a mutli line text input widget. */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Multi-line text input widget  */
/*                                            control block               */
/*    normal_text_color_id                  Resource ID of the normal     */
/*                                            text color                  */
/*    selected_text_color_id                Resource ID of the selected   */
/*                                            text color                  */
/*    disabled_text_color_id                Resource ID of the disabled   */
/*                                            text color                  */
/*    readonly_text_color_id                Resource ID of the read only  */
/*                                            text color                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_text_color_set(GX_MULTI_LINE_TEXT_INPUT* text_input,
                                              GX_RESOURCE_ID normal_text_color_id,
                                              GX_RESOURCE_ID selected_text_color_id,
                                              GX_RESOURCE_ID disabled_text_color_id,
                                              GX_RESOURCE_ID readonly_text_color_id)
{
    text_input->text_color_set_(normal_text_color_id,
                                selected_text_color_id,
                                disabled_text_color_id,
                                readonly_text_color_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_text_select                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function highlighs text with specified start mark and end mark */
/*    index.                                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Multi-line text input widget  */
/*                                            control block               */
/*    start_mark                            The index of the first        */
/*                                            highlight character.        */
/*    end_mark                              The index of the last         */
/*                                            highlight character         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                Mark widget as drity           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_text_select(GX_MULTI_LINE_TEXT_INPUT* text_input, UINT start_index, UINT end_index)
{
    text_input->text_select_(start_index, end_index);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_text_set_ext              PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns a text string to a multi line text input      */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                             Multi-line text input widget  */
/*                                            control block               */
/*    text                                  Null-terminated text string   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_new_line_character_get                    */
/*                                          Get the new line character    */
/*    _gx_multi_line_text_view_string_total_rows_compute                  */
/*                                          Compute the total rows of     */
/*                                            the input text              */
/*    _gx_multi_line_text_view_line_cache_update                          */
/*                                          Update line cache             */
/*    _gx_window_scrollbar_find             Find scrollbar for a window   */
/*    _gx_scrollbar_reset                   Reset scrollbar information   */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_text_set_ext(GX_MULTI_LINE_TEXT_INPUT* text_input, const GX_STRING* text)
{
    return text_input->text_set_ext_(text);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_up_arrow                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function move the cursor up by one line.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input         */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_font_get                   Retrieve font                 */
/*    _gx_text_input_cursor_dirty_rectangle_get                           */
/*                                          Get cursor rectangle          */
/*    _gx_multi_line_text_input_cursor_pos_calculate                      */
/*                                          Calculate cursor position     */
/*                                            according to click index    */
/*    _gx_system_dirty_partial_add          Add one dirty area to         */
/*                                            dirty list                  */
/*    _gx_system_dirty_mark                 Mark widget area dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_multi_line_text_input_keydown_process                           */
/*                                                                        */
/**************************************************************************/
UINT _gx_multi_line_text_input_up_arrow(GX_MULTI_LINE_TEXT_INPUT* text_input)
{
    return text_input->up_arrow_();
}
