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
/**   Text Input Management (Single Line Text Input)                      */
/**                                                                       */
/**************************************************************************/

#include "gx_single_line_text_input.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_canvas.h"
#include "gx_font.h"
#include "gx_text_area_info.h"

#define GX_TEXT_INPUT_TIMER     2
#define GX_MARK_TIMER           3

#define GX_MARK_INTERVAL        5

static void gx_single_line_text_input_draw_(GX_SINGLE_LINE_TEXT_INPUT* text_input)
{
    text_input->draw_();
}

static UINT gx_single_line_text_input_event_process_(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_EVENT* event_ptr)
{
    return text_input->event_process_(event_ptr);
}

UINT GX_SINGLE_LINE_TEXT_INPUT::create_(const GX_CHAR* name,
                                        GX_WIDGET* parent,
                                        GX_CHAR* input_buffer,
                                        UINT buffer_size,
                                        UINT style,
                                        USHORT Id,
                                        const GX_RECTANGLE* size)
{
    style &= ~GX_STYLE_TEXT_COPY;

    GX_PROMPT::create_(name, GX_NULL, 0, style, Id, size);

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

    this->type = GX_TYPE_SINGLE_LINE_TEXT_INPUT;

    this->style |= GX_STYLE_CURSOR_BLINK;

    this->buffer = input_buffer;
    this->buffer_size = buffer_size;

    this->text_data.font_id = GX_FONT_ID_TEXT_INPUT;

    this->text_data.normal_color = GX_COLOR_ID_TEXT_INPUT_TEXT;
    this->text_data.selected_color = GX_COLOR_ID_SELECTED_TEXT;
    this->readonly_text_color = GX_COLOR_ID_READONLY_TEXT;

    this->normal_fill_color = GX_COLOR_ID_TEXT_INPUT_FILL;
    this->selected_fill_color = GX_COLOR_ID_SELECTED_FILL;
    this->readonly_fill_color = GX_COLOR_ID_READONLY_FILL;

    this->cursor.blink_interval = GX_CURSOR_BLINK_INTERVAL;
    this->cursor.height = 0;
    this->cursor.width = 1;
    this->cursor.pos.x = 0;
    this->cursor.pos.y = 0;
    this->cursor.flags = 0;

    this->draw_function = (void (*)(GX_WIDGET*))gx_single_line_text_input_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_single_line_text_input_event_process_;

    this->was_modified = GX_FALSE;
    this->start_mark = 0;
    this->end_mark = 0;

    GX_RECTANGLE client;
    client_area_get_(&client);

    switch (this->style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        this->xoffset = (GX_VALUE)(-gx_text_area_width_(client));
        break;

    case GX_STYLE_TEXT_CENTER:
        this->xoffset = (GX_VALUE)(client.width_() / 2);
        break;

    case GX_STYLE_TEXT_LEFT:
    default:
        this->xoffset = 0;
        break;
    }

    this->yoffset = 0;

    if (buffer_size > 0)
    {
        if (_gx_utility_string_length_check(input_buffer, &this->string_size, buffer_size - 1) != GX_SUCCESS)
        {
            this->string_size = 0;
        }
    }
    else
    {
        this->string_size = 0;
    }

    this->insert_pos = this->string_size;

    if (this->style & GX_STYLE_CURSOR_ALWAYS)
    {
        this->status |= (GX_STATUS_CURSOR_SHOW | GX_STATUS_CURSOR_DRAW);
        this->status |= GX_STATUS_RESIZE_NOTIFY;
    }

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_SINGLE_LINE_TEXT_INPUT::style_add_(ULONG style)
{
    GX_WIDGET::style_add_(style);

    if (this->style & GX_STYLE_CURSOR_ALWAYS)
    {
        status_add_(GX_STATUS_CURSOR_SHOW | GX_STATUS_CURSOR_DRAW);

        if (is_visible_() && (this->style & GX_STYLE_CURSOR_BLINK))
        {
            UINT blink_interval = (UINT)cursor.blink_interval;
            timer_start_(GX_TEXT_INPUT_TIMER, blink_interval, blink_interval);
        }
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::style_remove_(ULONG style)
{
    GX_WIDGET::style_remove_(style);

    if (is_visible_() && (style & GX_STYLE_CURSOR_BLINK))
    {
        timer_stop_(GX_TEXT_INPUT_TIMER);

        status_add_(GX_STATUS_CURSOR_DRAW);
    }

    if ((style & GX_STYLE_CURSOR_ALWAYS) &&
        _gx_system_focus_owner != this)
    {
        status_remove_(GX_STATUS_CURSOR_SHOW);
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::style_set_(ULONG style)
{
    style_remove_(style);
    style_add_(style);
}

void GX_SINGLE_LINE_TEXT_INPUT::fill_color_set_(GX_RESOURCE_ID normal_fill_color_id,
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

void GX_SINGLE_LINE_TEXT_INPUT::text_color_set_(GX_RESOURCE_ID normal_text_color_id,
                                                GX_RESOURCE_ID selected_text_color_id,
                                                GX_RESOURCE_ID disabled_text_color_id,
                                                GX_RESOURCE_ID readonly_text_color_id)
{
    this->readonly_text_color = readonly_text_color_id;
    text_data.color_set_(normal_text_color_id,
                         selected_text_color_id,
                         disabled_text_color_id);
}

void GX_SINGLE_LINE_TEXT_INPUT::buffer_get_(GX_CHAR** buffer_address,
                                            UINT* content_size,
                                            UINT* buffer_size)
{
    if (buffer_address)
    {
        *buffer_address = this->buffer;
    }

    if (content_size)
    {
        *content_size = this->string_size;
    }

    if (buffer_size)
    {
        *buffer_size = this->buffer_size;
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::buffer_clear_()
{
    GX_RECTANGLE client;
    client_area_get_(&client);

    switch (this->style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        xoffset = (GX_VALUE)(-gx_text_area_width_(client));
        cursor.pos.x = gx_text_area_right_(client);
        break;

    case GX_STYLE_TEXT_CENTER:
        xoffset = (GX_VALUE)(client.width_() / 2);
        cursor.pos.x = (GX_VALUE)(gx_text_area_left_(client) + xoffset);
        break;

    case GX_STYLE_TEXT_LEFT:
    default:
        xoffset = 0;
        cursor.pos.x = gx_text_area_left_(client);
        break;
    }

    buffer[0] = '\0';
    string_size = 0;
    insert_pos = 0;

    dirty_mark_();
}

void GX_SINGLE_LINE_TEXT_INPUT::text_set_ext_(const GX_STRING* text)
{
    UINT copy_size;

    if (text && text->ptr)
    {
        if (text->length < buffer_size)
        {
            copy_size = text->length;
        }
        else
        {
#if defined GX_UTF8_SUPPORT

            GX_STRING string;
            string.ptr = text->ptr;
            string.length = buffer_size - 1;

            copy_size = 0;

            UINT glyph_len = 0;
            while (_gx_utility_utf8_string_character_get(&string, GX_NULL, &glyph_len) == GX_SUCCESS)
            {
                copy_size += glyph_len;
            }

#else // !GX_UTF8_SUPPORT

            copy_size = buffer_size - 1;

#endif // !GX_UTF8_SUPPORT
        }

        memcpy(buffer, text->ptr, copy_size);
    }
    else
    {
        copy_size = 0;
    }

    buffer[copy_size] = '\0';

    string_size = copy_size;
    insert_pos = copy_size;

    cursor_pos_update_();

    dirty_mark_if_visible_();
}

void GX_SINGLE_LINE_TEXT_INPUT::selected_text_delete_()
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

UINT GX_SINGLE_LINE_TEXT_INPUT::text_insert_(const GX_UBYTE* insert_str, UINT insert_size)
{
    selected_text_delete_();

    if (string_size + insert_size + 1 > buffer_size)
    {
        return GX_FAILURE;
    }

    GX_FONT* font = GX_NULL;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return GX_FAILURE;
    }

    GX_VALUE text_width = _gx_system_string_width_get(font, buffer, string_size);
    GX_VALUE insert_width = _gx_system_string_width_get(font, (const GX_CHAR*)insert_str, insert_size);

    memmove(buffer + insert_pos + insert_size, buffer + insert_pos, string_size - insert_pos);
    memmove(buffer + insert_pos, insert_str, insert_size);
    string_size += insert_size;
    buffer[string_size] = '\0';

    insert_pos += insert_size;

    GX_RECTANGLE client;
    client_area_get_(&client);

    const GX_VALUE x_left = gx_text_area_left_(client);
    const GX_VALUE x_right = gx_text_area_right_(client);

    GX_VALUE new_cursor_pos;
    GX_VALUE new_xoffset;

    GX_RECTANGLE dirty_area = client;

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        xoffset = (GX_VALUE)(xoffset + insert_width);

        dirty_area.right = cursor.pos.x;
        break;

    case GX_STYLE_TEXT_CENTER:
        text_width = (GX_VALUE)(text_width + insert_width);
        new_xoffset = (GX_VALUE)((text_width + 1) / 2);
        new_cursor_pos = (GX_VALUE)(cursor.pos.x - (new_xoffset - xoffset) + insert_width);
        xoffset = new_xoffset;

        if (new_cursor_pos < x_right)
        {
            dirty_area.left  = (GX_VALUE)(x_left + client.width_() / 2 - xoffset);
            dirty_area.right = (GX_VALUE)(dirty_area.left + text_width);

            dirty_area.left  = (GX_VALUE)(dirty_area.left - cursor.width / 2);
            dirty_area.right = (GX_VALUE)(dirty_area.right + (cursor.width + 1) / 2 - 1);
        }

        cursor.pos.x = new_cursor_pos;
        break;

    case GX_STYLE_TEXT_LEFT:
    default:
        new_cursor_pos = (GX_VALUE)(cursor.pos.x + insert_width);

        if (new_cursor_pos > x_right)
        {
            xoffset = (GX_VALUE)(xoffset + new_cursor_pos - x_right);
            new_cursor_pos = x_right;
        }
        else
        {
            dirty_area.left  = (GX_VALUE)(cursor.pos.x - cursor.width / 2);
            dirty_area.right = (GX_VALUE)(new_cursor_pos + text_width + cursor.width - 1);
        }

        cursor.pos.x = new_cursor_pos;
        break;
    }

    was_modified = GX_TRUE;

    dirty_mark_(&dirty_area);

    return GX_SUCCESS;
}

UINT GX_SINGLE_LINE_TEXT_INPUT::text_delete_()
{
    if (this->end_mark > this->start_mark)
    {
        return backspace_();
    }

    if (insert_pos >= string_size)
    {
        return GX_SUCCESS;
    }

    GX_STRING string;

    GX_RECTANGLE client;
    client_area_get_(&client);

    const GX_VALUE x_left = gx_text_area_left_(client);
    const GX_VALUE x_right = gx_text_area_right_(client);

    UINT glyph_len = 1;

    if (this->start_mark > this->end_mark)
    {
        glyph_len = this->start_mark - this->end_mark;

        if (cursor.pos.x < x_left || cursor.pos.x > x_right)
        {
            left_arrow_();
        }

        this->start_mark = 0;
        this->end_mark = 0;
    }
#ifdef GX_UTF8_SUPPORT
    else // this->start_mark == this->end_mark
    {
        string.ptr = buffer + insert_pos;
        string.length = string_size - insert_pos;

        _gx_utility_utf8_string_character_get(&string, GX_NULL, &glyph_len);
    }
#endif

    GX_FONT* font;
    font_get_(font_id_(), &font);

    GX_VALUE text_width = _gx_system_string_width_get(font, buffer, string_size);
    GX_VALUE deleted_width = _gx_system_string_width_get(font, buffer + insert_pos, glyph_len);

    GX_RECTANGLE dirty_area = client;

    GX_VALUE x_pos;
    GX_VALUE offset;

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        x_pos = (GX_VALUE)(x_right - xoffset);

        if (x_pos + text_width > x_right)
        {
            offset = (GX_VALUE)(x_pos + text_width - deleted_width - x_right);
            if (offset < 0)
            {
                xoffset = (GX_VALUE)(xoffset + offset);
                cursor.pos.x = (GX_VALUE)(cursor.pos.x - offset);
            }
        }
        else
        {
            xoffset = (GX_VALUE)(xoffset - deleted_width);
            cursor.pos.x = (GX_VALUE)(cursor.pos.x + deleted_width);

            dirty_area.right = cursor.pos.x;
        }
        break;

    case GX_STYLE_TEXT_CENTER:
        x_pos = (GX_VALUE)(x_left + client.width_() / 2 - xoffset);

        if (text_width <= client.width_() - border_width_())
        {
            dirty_area.left = (GX_VALUE)(x_pos - cursor.width / 2);
            dirty_area.right = (GX_VALUE)(x_pos + text_width + (cursor.width + 1) / 2 - 1);
        }

        xoffset = (GX_VALUE)((text_width - deleted_width + 1) / 2);
        cursor.pos.x = (GX_VALUE)(cursor.pos.x + ((text_width + 1) / 2 - xoffset));

        break;

    case GX_STYLE_TEXT_LEFT:
    default:
        x_pos = (GX_VALUE)(x_left - xoffset);

        if (x_pos < x_left)
        {
            offset = (GX_VALUE)(x_right - (x_pos + text_width - deleted_width));
            if (offset > 0)
            {
                if (offset > x_left - x_pos)
                {
                    offset = (GX_VALUE)(x_left - x_pos);
                }

                xoffset = (GX_VALUE)(xoffset - offset);
                cursor.pos.x = (GX_VALUE)(cursor.pos.x + offset);
            }
        }
        else
        {
            dirty_area.left = (GX_VALUE)(cursor.pos.x - cursor.width / 2);
            dirty_area.right = (GX_VALUE)(x_pos + text_width + cursor.width - 1);
        }
        break;
    }

    memmove(buffer + insert_pos, buffer + insert_pos + glyph_len, string_size - insert_pos - glyph_len);
    string_size -= glyph_len;
    buffer[string_size] = '\0';

    was_modified = GX_TRUE;

    dirty_mark_(&dirty_area);

    return GX_SUCCESS;
}

UINT GX_SINGLE_LINE_TEXT_INPUT::backspace_()
{
    if (this->start_mark > this->end_mark)
    {
        return text_delete_();
    }

    if (insert_pos == 0)
    {
        return GX_SUCCESS;
    }

    GX_STRING string;
    GX_RECTANGLE client;
    client_area_get_(&client);

    const GX_VALUE x_left = gx_text_area_left_(client);
    const GX_VALUE x_right = gx_text_area_right_(client);

    UINT glyph_len = 1;

    if (this->start_mark < this->end_mark)
    {
        glyph_len = this->end_mark - this->start_mark;

        if (cursor.pos.x < x_left || cursor.pos.x > x_right)
        {
            right_arrow_();
        }

        this->start_mark = 0;
        this->end_mark = 0;
    }
#ifdef GX_UTF8_SUPPORT
    else // this->start_mark == this->end_mark
    {
        string.ptr = buffer;
        string.length = string_size;

        _gx_utility_utf8_string_backward_character_length_get(&string, (INT)(insert_pos - 1), &glyph_len);
    }
#endif

    GX_FONT* font;
    font_get_(font_id_(), &font);

    GX_VALUE text_width = _gx_system_string_width_get(font, buffer, string_size);
    GX_VALUE deleted_width = _gx_system_string_width_get(font, buffer + insert_pos - glyph_len, glyph_len);

    GX_RECTANGLE dirty_area = client;

    GX_VALUE x_pos;
    GX_VALUE left_offset;
    GX_VALUE right_offset;
    GX_VALUE new_x_pos;

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        x_pos = (GX_VALUE)(x_right - xoffset);

        left_offset = (GX_VALUE)(x_left - x_pos);
        right_offset = (GX_VALUE)(x_pos + text_width - x_right);

        if (left_offset > deleted_width || right_offset == 0)
        {
            xoffset = (GX_VALUE)(xoffset - deleted_width);

            dirty_area.right = cursor.pos.x;
        }
        else if (left_offset == 0 || right_offset > deleted_width)
        {
            cursor.pos.x = (GX_VALUE)(cursor.pos.x - deleted_width);

            dirty_area.left = cursor.pos.x;
        }
        else
        {
            right_offset = (GX_VALUE)(right_offset - (deleted_width - left_offset));
            if (right_offset > 0)
            {
                xoffset = (GX_VALUE)(xoffset - left_offset);
                cursor.pos.x = (GX_VALUE)(cursor.pos.x - deleted_width + left_offset);
            }
            else
            {
                xoffset = (GX_VALUE)(xoffset - right_offset);
                cursor.pos.x = (GX_VALUE)(cursor.pos.x - deleted_width - right_offset);
            }
        }
        break;

    case GX_STYLE_TEXT_CENTER:
        x_pos = (GX_VALUE)(x_left + client.width_() / 2 - xoffset);

        if (text_width >= client.width_() - border_width_())
        {
            dirty_area = this->size;
        }
        else
        {
            dirty_area.left = (GX_VALUE)(x_pos - cursor.width / 2);
            dirty_area.right = (GX_VALUE)(x_pos + text_width + (cursor.width + 1) / 2 - 1);
        }

        xoffset = (GX_VALUE)((text_width - deleted_width + 1) / 2);
        cursor.pos.x = (GX_VALUE)(cursor.pos.x + ((text_width + 1) / 2 - xoffset) - deleted_width);
        break;

    case GX_STYLE_TEXT_LEFT:
    default:
        x_pos = (GX_VALUE)(x_left - xoffset);

        new_x_pos = (GX_VALUE)(x_pos + deleted_width);
        if (new_x_pos < x_left)
        {
            xoffset = (GX_VALUE)(xoffset - deleted_width);

            dirty_area.right = (GX_VALUE)(cursor.pos.x + (cursor.width + 1) / 2 - 1);
        }
        else if (x_pos < x_left)
        {
            cursor.pos.x = (GX_VALUE)(cursor.pos.x - deleted_width + xoffset);
            xoffset = 0;

            dirty_area = this->size;
        }
        else
        {
            cursor.pos.x = (GX_VALUE)(cursor.pos.x - deleted_width);

            dirty_area.left = (GX_VALUE)(cursor.pos.x - cursor.width / 2);
        }
        break;
    }

    dirty_mark_(&dirty_area);

    memmove(buffer + insert_pos - glyph_len, buffer + insert_pos, string_size - insert_pos);
    string_size -= glyph_len;
    buffer[string_size] = '\0';

    insert_pos -= glyph_len;

    was_modified = GX_TRUE;

    return GX_SUCCESS;
}

void GX_SINGLE_LINE_TEXT_INPUT::home_()
{
    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    if (start_mark_ != end_mark_)
    {
        this->start_mark = 0;
        this->end_mark = 0;
    }

    if (insert_pos == 0)
    {
        return;
    }

    GX_RECTANGLE client;
    client_area_get_(&client);

    const GX_VALUE x_left = gx_text_area_left_(client);
    const GX_VALUE x_right = gx_text_area_right_(client);

    GX_VALUE text_width;

    GX_VALUE new_xoffset;
    GX_VALUE new_cursor_pos;

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        text_width = text_width_get_(buffer, string_size);

        new_xoffset = gx_text_area_width_(client);
        if (new_xoffset > text_width)
        {
            new_xoffset = text_width;
        }

        new_cursor_pos = (GX_VALUE)(x_right - new_xoffset);
        break;

    case GX_STYLE_TEXT_CENTER:
        new_xoffset = 0;
        new_cursor_pos = (GX_VALUE)(x_left + client.width_() / 2 - xoffset);
        break;

    case GX_STYLE_TEXT_LEFT:
    default:
        new_xoffset = 0;
        new_cursor_pos = x_left;
        break;
    }

    cursor_change_for_home_end_(new_xoffset, new_cursor_pos, start_mark_, end_mark_);

    insert_pos = 0;
}

void GX_SINGLE_LINE_TEXT_INPUT::end_()
{
    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    if (start_mark_ != end_mark_)
    {
        this->start_mark = 0;
        this->end_mark = 0;
    }

    if (insert_pos >= string_size)
    {
        return;
    }

    GX_RECTANGLE client;
    client_area_get_(&client);

    const GX_VALUE x_left = gx_text_area_left_(client);
    const GX_VALUE x_right = gx_text_area_right_(client);

    GX_VALUE text_width;

    GX_VALUE new_xoffset;
    GX_VALUE new_cursor_pos;

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        new_xoffset = 0;
        new_cursor_pos = x_right;
        break;

    case GX_STYLE_TEXT_CENTER:
        new_xoffset = 0;
        new_cursor_pos = (GX_VALUE)(x_left + client.width_() / 2 - xoffset);
        break;

    case GX_STYLE_TEXT_LEFT:
    default:
        text_width = text_width_get_(buffer, string_size);

        new_xoffset = (GX_VALUE)(text_width - gx_text_area_width_(client));
        if (new_xoffset < 0)
        {
            new_xoffset = 0;
        }

        new_cursor_pos = (GX_VALUE)(x_left - new_xoffset + text_width);
        break;
    }

    cursor_change_for_home_end_(new_xoffset, new_cursor_pos, start_mark_, end_mark_);

    insert_pos = string_size;
}

void GX_SINGLE_LINE_TEXT_INPUT::selected_area_dirty_mark_(UINT start_mark_, UINT end_mark_)
{
    GX_RECTANGLE dirty_area;
    if (text_rectangle_get_((INT)(start_mark_ - end_mark_), &dirty_area))
    {
        dirty_mark_(&dirty_area);
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::selected_area_and_cursor_area_dirty_mark_(UINT start_mark_, UINT end_mark_)
{
    GX_RECTANGLE cursor_rect;
    cursor.dirty_rectangle_get_(&cursor_rect);

    GX_RECTANGLE dirty_area;
    if (text_rectangle_get_((INT)(start_mark_ - end_mark_), &dirty_area))
    {
        dirty_area.combine_(cursor_rect);
        dirty_mark_(&dirty_area);
    }
    else
    {
        dirty_mark_(&cursor_rect);
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::cursor_change_for_home_end_(GX_VALUE new_xoffset,
                                                            GX_VALUE new_cursor_pos,
                                                            UINT old_start_mark,
                                                            UINT old_end_mark)
{
    bool mark_new_cursor_dirty = false;

    if (xoffset != new_xoffset)
    {
        dirty_mark_();
    }
    else
    {
        if (old_start_mark != old_end_mark)
        {
            selected_area_dirty_mark_(old_start_mark, old_end_mark);
        }
        else
        {
            cursor_area_dirty_mark_(); // mark old cursor area dirty
        }

        mark_new_cursor_dirty = true;
    }

    cursor.pos.x = new_cursor_pos;
    xoffset = new_xoffset;

    if (mark_new_cursor_dirty)
    {
        cursor_area_dirty_mark_(); // mark new cursor area dirty
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::left_arrow_()
{
    GX_RECTANGLE client;
    client_area_get_(&client);

    GX_VALUE cursor_pos = cursor.pos.x;

    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    if (start_mark_ != end_mark_)
    {
        this->start_mark = 0;
        this->end_mark = 0;

        if (end_mark_ < start_mark_)
        {
            if ((client.left < cursor_pos && cursor_pos < client.right) ||
                ((style & GX_STYLE_TEXT_ALIGNMENT_MASK) == GX_STYLE_TEXT_CENTER))
            {
                // No need to update cursor position, just need
                // mark cursor and highlight area as dirty
                selected_area_and_cursor_area_dirty_mark_(start_mark_, end_mark_);

                return;
            }
        }
    }

    if (insert_pos > 0 || start_mark_ > end_mark_)
    {
        if (end_mark_ >= start_mark_)
        {
            UINT glyph_len = 1;

            if (end_mark_ > start_mark_)
            {
                glyph_len = end_mark_ - start_mark_;
            }
#ifdef GX_UTF8_SUPPORT
            else // end_mark_ == start_mark_
            {
                GX_STRING string;
                string.ptr = buffer;
                string.length = string_size;
                _gx_utility_utf8_string_backward_character_length_get(&string, (INT)(insert_pos - 1), &glyph_len);
            }
#endif

            insert_pos -= glyph_len;

            GX_VALUE text_width = text_width_get_(buffer + insert_pos, glyph_len);

            cursor_pos = (GX_VALUE)(cursor_pos - text_width);
        }

        cursor_change_for_prev_next_(cursor_pos, start_mark_, end_mark_, client);
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::right_arrow_()
{
    GX_RECTANGLE client;
    client_area_get_(&client);

    GX_VALUE cursor_pos = cursor.pos.x;

    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    if (start_mark_ != end_mark_)
    {
        this->start_mark = 0;
        this->end_mark = 0;

        if (end_mark_ > start_mark_)
        {
            if ((client.left < cursor_pos && cursor_pos < client.right) ||
                ((style & GX_STYLE_TEXT_ALIGNMENT_MASK) == GX_STYLE_TEXT_CENTER))
            {
                // No need to update cursor position, just need
                // mark cursor and highlight area as dirty
                selected_area_and_cursor_area_dirty_mark_(start_mark_, end_mark_);

                return;
            }
        }
    }

    if (insert_pos < string_size || end_mark_ > start_mark_)
    {
        if (start_mark_ >= end_mark_)
        {
            UINT glyph_len = 1;

            if (start_mark_ > end_mark_)
            {
                glyph_len = start_mark_ - end_mark_;
            }
#ifdef GX_UTF8_SUPPORT
            else // start_mark_ == end_mark_
            {
                GX_STRING string;
                string.ptr = buffer + insert_pos;
                string.length = string_size - insert_pos;
                _gx_utility_utf8_string_character_get(&string, GX_NULL, &glyph_len);
            }
#endif

            GX_VALUE text_width = text_width_get_(buffer + insert_pos, glyph_len);

            insert_pos += glyph_len;

            cursor_pos = (GX_VALUE)(cursor_pos + text_width);
        }

        cursor_change_for_prev_next_(cursor_pos, start_mark_, end_mark_, client);
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::cursor_change_for_prev_next_(GX_VALUE new_cursor_pos,
                                                             UINT old_start_mark,
                                                             UINT old_end_mark,
                                                             const GX_RECTANGLE& client)
{
    GX_RECTANGLE dirty_area;

    GX_VALUE old_cursor_pos = cursor.pos.x;

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
    case GX_STYLE_TEXT_LEFT:
    default:
        if (cursor_adjust_(new_cursor_pos, client))
        {
            return;
        }
        // fallthrough
    case GX_STYLE_TEXT_CENTER:
        if (old_start_mark == old_end_mark)
        {
            cursor_area_dirty_mark_(&dirty_area); // mark old cursor area dirty
        }

        cursor.pos.x = new_cursor_pos;

        cursor_area_dirty_mark_(&dirty_area); // mark new cursor area dirty
        break;
    }

    if (old_start_mark != old_end_mark)
    {
        if (new_cursor_pos < old_cursor_pos)
        {
            dirty_area.left = new_cursor_pos;
            dirty_area.right = (GX_VALUE)(old_cursor_pos - 1);
        }
        else if (old_cursor_pos < new_cursor_pos)
        {
            dirty_area.left = old_cursor_pos;
            dirty_area.right = (GX_VALUE)(new_cursor_pos - 1);
        }
        else
        {
            return;
        }

        dirty_mark_(&dirty_area);
    }
}

bool GX_SINGLE_LINE_TEXT_INPUT::cursor_adjust_(GX_VALUE new_cursor_pos, const GX_RECTANGLE& client)
{
    GX_VALUE x_left = gx_text_area_left_(client);
    GX_VALUE x_right = gx_text_area_right_(client);

    if (new_cursor_pos < x_left)
    {
        xoffset = (GX_VALUE)(xoffset - (x_left - new_cursor_pos));
        cursor.pos.x = x_left;

        dirty_mark_(&client);

        return true;
    }
    else if (new_cursor_pos > x_right)
    {
        xoffset = (GX_VALUE)(xoffset + (new_cursor_pos - x_right));
        cursor.pos.x = x_right;

        dirty_mark_(&client);

        return true;
    }
    else
    {
        return false;
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::key_down_process_(GX_EVENT* event_ptr)
{
    GX_UBYTE utf8_str[10];

    USHORT key_value = event_ptr->payload.ushortdata[0];
    switch (key_value)
    {
    case GX_KEY_HOME:
        home_();
        break;

    case GX_KEY_END:
        end_();
        break;

    case GX_KEY_BACKSPACE:
        backspace_();
        break;

    case GX_KEY_DELETE:
        text_delete_();
        break;

    case GX_KEY_LEFT_ARROW:
        left_arrow_();
        break;

    case GX_KEY_RIGHT_ARROW:
        right_arrow_();
        break;

    case GX_KEY_SELECT:
        if (was_modified)
        {
            signal_parent_(GX_EVENT_TEXT_EDITED, 0);
            was_modified = GX_FALSE;
        }
        break;

    case GX_KEY_SPACE:
        utf8_str[0] = ' ';
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
            UINT utf8_size;
            _gx_utility_unicode_to_utf8(key_value, utf8_str, &utf8_size);

            text_insert_(utf8_str, utf8_size);
        }
#endif
        break;
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::text_select_(UINT start_index, UINT end_index)
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
        if (start_mark_ != end_mark_)
        {
            selected_area_dirty_mark_(start_mark_, end_mark_);
        }
        else
        {
            cursor_area_dirty_mark_();
        }

        start_mark_ = this->start_mark;
        end_mark_ = this->end_mark;

        if (insert_pos != end_mark_)
        {
            insert_pos = end_mark_;
            cursor_pos_update_();
        }

        selected_area_dirty_mark_(start_mark_, end_mark_);
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::mark_home_()
{
    if (insert_pos == 0)
    {
        return;
    }

    if (this->start_mark == this->end_mark)
    {
        this->start_mark = this->insert_pos;
        this->end_mark = this->insert_pos;
    }

    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    GX_RECTANGLE client;
    client_area_get_(&client);

    const GX_VALUE x_left = gx_text_area_left_(client);
    const GX_VALUE x_right = gx_text_area_right_(client);

    GX_VALUE new_xoffset;
    GX_VALUE new_cursor_pos;

    GX_VALUE text_width;

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        text_width = text_width_get_(buffer, string_size);
        new_xoffset = GX_MIN(text_width, gx_text_area_width_(client));
        new_cursor_pos = (GX_VALUE)(x_right - new_xoffset);
        break;

    case GX_STYLE_TEXT_CENTER:
        new_xoffset = 0;
        new_cursor_pos = (GX_VALUE)(x_left + client.width_() / 2 - xoffset);
        break;

    case GX_STYLE_TEXT_LEFT:
    default:
        new_xoffset = 0;
        new_cursor_pos = x_left;
        break;
    }

    cursor_change_for_mark_home_end_(new_xoffset, new_cursor_pos, start_mark_, end_mark_, 0);

    this->end_mark = 0;
    this->insert_pos = 0;
}

void GX_SINGLE_LINE_TEXT_INPUT::mark_end_()
{
    if (insert_pos >= string_size)
    {
        return;
    }

    if (this->start_mark == this->end_mark)
    {
        this->start_mark = this->insert_pos;
        this->end_mark = this->insert_pos;
    }

    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    GX_RECTANGLE client;
    client_area_get_(&client);

    const GX_VALUE x_left = gx_text_area_left_(client);
    const GX_VALUE x_right = gx_text_area_right_(client);

    GX_VALUE new_xoffset;
    GX_VALUE new_cursor_pos;

    GX_VALUE text_width;

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        new_xoffset = 0;
        new_cursor_pos = x_right;
        break;

    case GX_STYLE_TEXT_CENTER:
        new_xoffset = 0;
        new_cursor_pos = (GX_VALUE)(x_left + client.width_() / 2 - xoffset);
        break;

    case GX_STYLE_TEXT_LEFT:
    default:
        text_width = text_width_get_(buffer, string_size);
        new_xoffset = (GX_VALUE)(text_width - gx_text_area_width_(client));
        if (new_xoffset < 0)
        {
            new_xoffset = 0;
        }
        new_cursor_pos = (GX_VALUE)(x_left + text_width - new_xoffset);
        break;
    }

    cursor_change_for_mark_home_end_(new_xoffset, new_cursor_pos, start_mark_, end_mark_, string_size);

    this->end_mark = string_size;
    this->insert_pos = string_size;
}

void GX_SINGLE_LINE_TEXT_INPUT::cursor_change_for_mark_home_end_(GX_VALUE new_xoffset,
                                                                 GX_VALUE new_cursor_pos,
                                                                 UINT old_start_mark,
                                                                 UINT old_end_mark,
                                                                 UINT new_end_mark)
{
    if (xoffset != new_xoffset)
    {
        dirty_mark_();
    }
    else
    {
        if (old_start_mark == old_end_mark)
        {
            cursor_area_dirty_mark_();
        }

        selected_area_dirty_mark_(new_end_mark, old_end_mark);
    }

    cursor.pos.x = new_cursor_pos;
    xoffset = new_xoffset;
}

void GX_SINGLE_LINE_TEXT_INPUT::mark_previous_()
{
    if (this->start_mark == this->end_mark)
    {
        this->start_mark = this->insert_pos;
        this->end_mark = this->insert_pos;
    }

    if (this->end_mark == 0)
    {
        return;
    }

    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    UINT glyph_len = 1;

#ifdef GX_UTF8_SUPPORT
    GX_STRING string;
    string.ptr = buffer;
    string.length = string_size;
    _gx_utility_utf8_string_backward_character_length_get(&string, (INT)(end_mark_ - 1), &glyph_len);
#endif

    this->end_mark -= glyph_len;
    this->insert_pos -= glyph_len;

    GX_VALUE text_width = text_width_get_(buffer + this->end_mark, glyph_len);
    GX_VALUE new_cursor_pos = (GX_VALUE)(cursor.pos.x - text_width);

    cursor_change_for_mark_prev_next_(new_cursor_pos, start_mark_, end_mark_);
}

void GX_SINGLE_LINE_TEXT_INPUT::mark_next_()
{
    if (this->start_mark == this->end_mark)
    {
        this->start_mark = this->insert_pos;
        this->end_mark = this->insert_pos;
    }

    if (this->end_mark >= string_size)
    {
        return;
    }

    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    UINT glyph_len = 1;

#ifdef GX_UTF8_SUPPORT
    GX_STRING string;
    string.ptr = buffer + end_mark;
    string.length = string_size - end_mark;
    _gx_utility_utf8_string_character_get(&string, GX_NULL, &glyph_len);
#endif

    this->end_mark += glyph_len;
    this->insert_pos += glyph_len;

    GX_VALUE text_width = text_width_get_(buffer + end_mark_, glyph_len);
    GX_VALUE new_cursor_pos = (GX_VALUE)(cursor.pos.x + text_width);

    cursor_change_for_mark_prev_next_(new_cursor_pos, start_mark_, end_mark_);
}

void GX_SINGLE_LINE_TEXT_INPUT::cursor_change_for_mark_prev_next_(GX_VALUE new_cursor_pos,
                                                                  UINT old_start_mark,
                                                                  UINT old_end_mark)
{
    GX_VALUE old_cursor_pos = cursor.pos.x;

    GX_RECTANGLE client;
    client_area_get_(&client);

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_CENTER:
        break;

    case GX_STYLE_TEXT_RIGHT:
    case GX_STYLE_TEXT_LEFT:
    default:
        if (cursor_adjust_(new_cursor_pos, client))
        {
            return;
        }
        break;
    }

    if (new_cursor_pos < old_cursor_pos)
    {
        client.left = new_cursor_pos;
        client.right = (GX_VALUE)(old_cursor_pos - 1);
    }
    else if (old_cursor_pos < new_cursor_pos)
    {
        client.left = old_cursor_pos;
        client.right = (GX_VALUE)(new_cursor_pos - 1);
    }
    else
    {
        return;
    }

    if (old_start_mark == old_end_mark)
    {
        GX_RECTANGLE cursor_rect;
        cursor.dirty_rectangle_get_(&cursor_rect); // old cursor rectangle

        client.combine_(cursor_rect);
    }

    cursor.pos.x = new_cursor_pos;

    dirty_mark_(&client);
}

void GX_SINGLE_LINE_TEXT_INPUT::copy_() const
{
    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    if (start_mark_ != end_mark_)
    {
        if (start_mark_ > end_mark_)
        {
            GX_SWAP_VALS(start_mark_, end_mark_);
        }

        _gx_system_clipboard_put(buffer + start_mark_, end_mark_ - start_mark_);
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::cut_()
{
    copy_();
    selected_text_delete_();
}

void GX_SINGLE_LINE_TEXT_INPUT::paste_()
{
    GX_CHAR* data;
    UINT data_size;

    _gx_system_clipboard_get((void**)&data, &data_size);

    if (data)
    {
        text_insert_((const GX_UBYTE *)data, data_size);
    }
}

GX_VALUE GX_SINGLE_LINE_TEXT_INPUT::text_width_get_(const GX_CHAR* string, UINT length) const
{
    GX_FONT* font = GX_NULL;
    font_get_(font_id_(), &font);
    if (!font)
    {
        return 0;
    }

    return _gx_system_string_width_get(font, string, (INT)length);
}

bool GX_SINGLE_LINE_TEXT_INPUT::text_rectangle_get_(INT offset_index, GX_RECTANGLE* rect) const
{
    if (offset_index == 0)
    {
        return false;
    }

    INT start_index;

    if (offset_index > 0)
    {
        start_index = (INT)insert_pos;
    }
    else
    {
        start_index = (INT)insert_pos + offset_index;
    }

    GX_VALUE text_width = text_width_get_(buffer + start_index, (UINT)(GX_ABS(offset_index)));

    client_area_get_(rect);

    if (offset_index > 0)
    {
        rect->left = cursor.pos.x;
        rect->right = (GX_VALUE)(rect->left + text_width - 1);
    }
    else
    {
        rect->right = (GX_VALUE)(cursor.pos.x - 1);
        rect->left = (GX_VALUE)(rect->right - text_width);
    }

    return true;
}

GX_VALUE GX_SINGLE_LINE_TEXT_INPUT::first_char_x_pos_get_(const GX_RECTANGLE& client) const
{
    GX_VALUE x_pos;

    const GX_VALUE x_left = gx_text_area_left_(client);
    const GX_VALUE x_right = gx_text_area_right_(client);

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        x_pos = x_right;
        break;

    case GX_STYLE_TEXT_CENTER:
        x_pos = (GX_VALUE)(x_left + client.width_() / 2);
        break;

    case GX_STYLE_TEXT_LEFT:
    default:
        x_pos = x_left;
        break;
    }

    x_pos = (GX_VALUE)(x_pos - xoffset);

    return x_pos;
}

// pixel_position -> insert_pos, cursor.pos.x
void GX_SINGLE_LINE_TEXT_INPUT::position_get_(INT pixel_position)
{
    GX_RECTANGLE client;
    client_area_get_(&client);

    GX_VALUE new_cursor_pos;
    insert_pos = new_pos_get_(client, pixel_position, &new_cursor_pos);

    cursor_pos_set_(client, new_cursor_pos);
}

UINT GX_SINGLE_LINE_TEXT_INPUT::new_relative_pos_get_(GX_VALUE distance, GX_VALUE* new_cursor_pos) const
{
    GX_FONT* font = GX_NULL;
    font_get_(font_id_(), &font);
    if (!font)
    {
        *new_cursor_pos = 0;
        return 0;
    }

    GX_STRING string;
    string.ptr = this->buffer;
    string.length = this->string_size;

    UINT new_insert_pos = string.length;
    UINT char_pos = 0;
    GX_VALUE cursor_pos = 0;

    const CHAR* str;
    UINT glyph_len = 1;
    GX_VALUE width;

    while (string.length > 0)
    {
        str = string.ptr;

#ifdef GX_UTF8_SUPPORT
        _gx_utility_utf8_string_character_get(&string, GX_NULL, &glyph_len);
#else
        ++string.ptr;
        --string.length;
#endif

        width = _gx_system_string_width_get(font, str, glyph_len);
        if ((cursor_pos + width / 2) > distance)
        {
            new_insert_pos = char_pos;
            break;
        }

        cursor_pos = (GX_VALUE)(cursor_pos + width);
        char_pos += glyph_len;
    }

    *new_cursor_pos = cursor_pos;

    return new_insert_pos;
}

// pixel_position -> insert_pos and cursor_pos
UINT GX_SINGLE_LINE_TEXT_INPUT::new_pos_get_(const GX_RECTANGLE& client, INT pixel_position, GX_VALUE* new_cursor_pos) const
{
    const GX_VALUE x_left = gx_text_area_left_(client);
    const GX_VALUE x_right = gx_text_area_right_(client);

    // Reset mouse down position when it was out of the text show area.
    if (pixel_position < x_left)
    {
        pixel_position = x_left;
    }
    else if (pixel_position > x_right)
    {
        pixel_position = x_right;
    }

    GX_VALUE first_char_x_pos = first_char_x_pos_get_(client);

    // Compute the distance from the first character position to the mouse down position.
    GX_VALUE distance = 0;
    if (pixel_position > first_char_x_pos)
    {
        distance = (GX_VALUE)(pixel_position - first_char_x_pos);
    }

    GX_VALUE relative_cursor_pos = 0;
    UINT new_insert_pos = new_relative_pos_get_(distance, &relative_cursor_pos);

    *new_cursor_pos = (GX_VALUE)(first_char_x_pos + relative_cursor_pos);

    return new_insert_pos;
}

// cursor_pos -> cursor.pos.x, xoffset
void GX_SINGLE_LINE_TEXT_INPUT::cursor_pos_set_(const GX_RECTANGLE& client, GX_VALUE cursor_pos)
{
    const GX_VALUE x_left = gx_text_area_left_(client);
    const GX_VALUE x_right = gx_text_area_right_(client);

    if (cursor_pos < x_left)
    {
        // cursor is beyond widget left, adjust text offset to show cursor.
        cursor.pos.x = (GX_VALUE)x_left;
        xoffset = (GX_VALUE)(xoffset - (x_left - cursor_pos));

        dirty_mark_();
    }
    else if (cursor_pos > x_right)
    {
        // cursor is beyond widget right, adjust text offset to show cursor.
        cursor.pos.x = (GX_VALUE)x_right;
        xoffset = (GX_VALUE)(xoffset + (cursor_pos - x_right));

        dirty_mark_();
    }
    else if (cursor_pos != cursor.pos.x)
    {
        // For this situation, we only need to mark old and new cursor position dirty.

        cursor_area_dirty_mark_();

        cursor.pos.x = cursor_pos;

        cursor_area_dirty_mark_();
    }
}

// insert_pos, xoffset -> cursor.pos.x
void GX_SINGLE_LINE_TEXT_INPUT::cursor_pos_update_()
{
    GX_VALUE text_width = text_width_get_(buffer, insert_pos);

    GX_RECTANGLE client;
    client_area_get_(&client);

    GX_VALUE first_char_x_pos = first_char_x_pos_get_(client);
    cursor.pos.x = (GX_VALUE)(first_char_x_pos + text_width);
}

bool GX_SINGLE_LINE_TEXT_INPUT::draw_position_get_(GX_VALUE* xpos, GX_VALUE* ypos)
{
    GX_BRUSH* brush;
    _gx_context_brush_get(&brush);
    if (!brush->font)
    {
        return false;
    }

    bool update_cursor_pos = ((style & GX_STYLE_CURSOR_ALWAYS) != 0);

    GX_RECTANGLE client;
    client_area_get_(&client);

    GX_VALUE text_width = _gx_system_string_width_get(brush->font, buffer, string_size);
    GX_VALUE text_height = brush->font->line_height;

    const GX_VALUE x_left = gx_text_area_left_(client);
    const GX_VALUE x_right = gx_text_area_right_(client);
    const GX_VALUE x_width = gx_text_area_width_(client);

    GX_VALUE x_pos;
    GX_VALUE new_offset;

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        x_pos = (GX_VALUE)(x_right - xoffset);
        new_offset = text_width;

        if (((text_width < x_width) && (xoffset != new_offset)) ||
            (x_pos > x_left))
        {
            xoffset = new_offset;
            x_pos = (GX_VALUE)(x_right - xoffset);
            update_cursor_pos = true;
        }
        break;

    case GX_STYLE_TEXT_CENTER:
        x_pos = (GX_VALUE)(x_left + client.width_() / 2 - xoffset);
        new_offset = (text_width + 1) / 2;

        if (((text_width < x_width) && (xoffset != new_offset)) ||
            (x_pos > x_left) ||
            (x_pos + text_width < x_right))
        {
            xoffset = new_offset;
            x_pos = (GX_VALUE)(x_left + client.width_() / 2 - xoffset);
            update_cursor_pos = true;
        }
        break;

    case GX_STYLE_TEXT_LEFT:
    default:
        x_pos = (GX_VALUE)(x_left - xoffset);
        new_offset = 0;

        if (((text_width < x_width) && (xoffset != new_offset)) ||
            (x_pos + text_width < x_right))
        {
            xoffset = new_offset;
            x_pos = (GX_VALUE)(x_left - xoffset);
            update_cursor_pos = true;
        }
        break;
    }

    if (update_cursor_pos)
    {
        cursor_pos_update_();
    }

    *xpos = x_pos;
    *ypos = (GX_VALUE)(client.top + (client.height_() - text_height) / 2 - yoffset);

    return true;
}

void GX_SINGLE_LINE_TEXT_INPUT::text_draw_(GX_RESOURCE_ID text_color)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

    context->font_set_(font_id_());

    GX_VALUE x_pos;
    GX_VALUE y_pos;
    if (!draw_position_get_(&x_pos, &y_pos))
    {
        return;
    }

    GX_RECTANGLE client;
    client_area_get_(&client);

    GX_RECTANGLE overlap;
    if (gx_rectangle_intersect_(context->dirty, client, &overlap))
    {
        GX_DRAWING drawing(context->canvas, this, overlap);
        if (drawing.result_() == GX_SUCCESS)
        {
            GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

            context->line_color_set_(text_color);

            UINT start_mark_ = this->start_mark;
            UINT end_mark_ = this->end_mark;

            if ((start_mark_ == end_mark_) &&
                (this->status & GX_STATUS_CURSOR_SHOW) &&
                (this->status & GX_STATUS_CURSOR_DRAW))
            {
                if (!(cursor.flags & GX_CURSOR_USE_CUSTOM_HEIGHT))
                {
                    cursor.height = (GX_VALUE)(client.height_() - 4);
                }
                cursor.pos.y = (GX_VALUE)(client.top + client.height_() / 2);

                cursor.draw_();
            }

            if (buffer && *buffer)
            {
                GX_STRING string;

                if (start_mark_ == end_mark_)
                {
                    string.ptr = buffer;
                    string.length = string_size;
                    _gx_canvas_text_draw_ext(x_pos, y_pos, &string);
                }
                else
                {
                    if (start_mark_ > end_mark_)
                    {
                        GX_SWAP_VALS(start_mark_, end_mark_);
                    }

                    GX_VALUE text_width;

                    if (start_mark_ > 0)
                    {
                        // draw text [0, start_mark_) with normal text color

                        string.ptr = buffer;
                        string.length = start_mark_;
                        _gx_system_string_width_get_ext(context->brush.font, &string, &text_width);
                        _gx_canvas_text_draw_ext(x_pos, y_pos, &string);
                        x_pos = (GX_VALUE)(x_pos + text_width);
                    }

                    // draw text [start_mark_, end_mark_) with selected text color

                    context->line_color_set_(selected_text_color_());
                    context->fill_color_set_(selected_fill_color);
                    context->brush.width = 0;

                    string.ptr = buffer + start_mark_;
                    string.length = (UINT)(end_mark_ - start_mark_);
                    _gx_system_string_width_get_ext(context->brush.font, &string, &text_width);

                    if (!(cursor.flags & GX_CURSOR_USE_CUSTOM_HEIGHT))
                    {
                        cursor.height = (GX_VALUE)(client.height_() - 4);
                    }

                    client.left = x_pos;
                    client.right = (GX_VALUE)(x_pos + text_width - 1);
                    client.top = (GX_VALUE)(client.top + (client.height_() - cursor.height) / 2);
                    client.bottom = (GX_VALUE)(client.top + cursor.height - 1);

                    _gx_canvas_rectangle_draw(&client);
                    _gx_canvas_text_draw_ext(x_pos, y_pos, &string);
                    x_pos = (GX_VALUE)(x_pos + text_width);

                    if (end_mark < string_size)
                    {
                        // draw text [end_mark_, string_size) with normal text color

                        context->line_color_set_(text_color);

                        string.ptr = buffer + end_mark_;
                        string.length = string_size - end_mark_;
                        _gx_canvas_text_draw_ext(x_pos, y_pos, &string);
                    }
                }
            }
        }
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::draw_()
{
    GX_RESOURCE_ID text_color;
    GX_RESOURCE_ID fill_color;

    if (is_enabled_())
    {
        if (style & GX_STYLE_TEXT_INPUT_READONLY)
        {
            text_color = readonly_text_color;
            fill_color = readonly_fill_color;
        }
        else
        {
            text_color = normal_text_color_();
            fill_color = normal_fill_color;
        }
    }
    else
    {
        text_color = disabled_text_color_();
        fill_color = disabled_fill_color;
    }

    border_and_background_draw_(GX_COLOR_ID_WINDOW_BORDER, fill_color, fill_color, GX_TRUE);

    text_draw_(text_color);

    children_draw_();
}

void GX_SINGLE_LINE_TEXT_INPUT::pen_down_process_(GX_EVENT* event_ptr)
{
    _gx_system_input_capture(this);

    UINT start_mark_ = this->start_mark;
    UINT end_mark_ = this->end_mark;

    if (start_mark_ != end_mark_)
    {
        selected_area_dirty_mark_(start_mark_, end_mark_);
    }

    position_get_((INT)(event_ptr->payload.pointdata.x));

    this->start_mark = insert_pos;
    this->end_mark = insert_pos;

    ((GX_WIDGET*)this)->event_process_(event_ptr);
}

void GX_SINGLE_LINE_TEXT_INPUT::pen_drag_process_(GX_EVENT* event_ptr)
{
    if (this->status & GX_STATUS_OWNS_INPUT)
    {
        UINT start_mark_ = this->start_mark;
        UINT end_mark_ = this->end_mark;

        GX_VALUE click_x = event_ptr->payload.pointdata.x;

        position_get_((INT)click_x);

        this->end_mark = insert_pos;

        GX_RECTANGLE client;
        client_area_get_(&client);

        if ((click_x < client.left) && end_mark > 0)
        {
            // start a timer to move text right

            if (!(this->status & (GX_STATUS_MARK_NEXT | GX_STATUS_MARK_PREVIOUS)))
            {
                timer_start_(GX_MARK_TIMER, GX_MARK_INTERVAL, GX_MARK_INTERVAL);
            }

            status_remove_(GX_STATUS_MARK_NEXT);
            status_add_(GX_STATUS_MARK_PREVIOUS);
        }
        else if ((click_x > client.right) && (end_mark < string_size))
        {
            // start a timer to move text left

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
                status_remove_(GX_STATUS_MARK_NEXT | GX_STATUS_MARK_PREVIOUS);
            }
        }

        if ((start_mark_ != this->end_mark) ||
            (start_mark_ != end_mark_))
        {
            // retrieve text bounding rectangle between old and new end mark.
            selected_area_dirty_mark_(end_mark_, this->end_mark);
        }
    }
    else
    {
        ((GX_WIDGET*)this)->event_process_(event_ptr);
    }
}

void GX_SINGLE_LINE_TEXT_INPUT::pen_up_process_(GX_EVENT* event_ptr)
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

    ((GX_WIDGET*)this)->event_process_(event_ptr);
}

UINT GX_SINGLE_LINE_TEXT_INPUT::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        GX_WIDGET::event_process_(event_ptr);

        if ((this->style & GX_STYLE_CURSOR_ALWAYS) &&
            (this->style & GX_STYLE_CURSOR_BLINK))
        {
            timer_start_(GX_TEXT_INPUT_TIMER, GX_CURSOR_BLINK_INTERVAL, GX_CURSOR_BLINK_INTERVAL);
        }
        cursor_pos_update_();
        break;

    case GX_EVENT_RESIZED:
        cursor_pos_update_();
        break;

    case GX_EVENT_STYLE_CHANGED:
        if (is_visible_())
        {
            ULONG old_style = event_ptr->payload.ulongdata;
            if ((old_style & (GX_STYLE_BORDER_MASK | GX_STYLE_TEXT_ALIGNMENT_MASK)) !=
                (this->style & (GX_STYLE_BORDER_MASK | GX_STYLE_TEXT_ALIGNMENT_MASK)))
            {
                cursor_pos_update_();
            }
        }
        break;

    case GX_EVENT_KEY_DOWN:
        key_down_process_(event_ptr);
        if (this->style & GX_STYLE_TEXT_INPUT_NOTIFY_ALL)
        {
            signal_parent_(GX_EVENT_TEXT_EDITED, 0);
        }
        break;

    case GX_EVENT_FOCUS_GAINED:
        if (!(this->style & GX_STYLE_CURSOR_ALWAYS))
        {
            status_add_(GX_STATUS_CURSOR_SHOW | GX_STATUS_CURSOR_DRAW);

            if (this->style & GX_STYLE_CURSOR_BLINK)
            {
                timer_start_(GX_TEXT_INPUT_TIMER, GX_CURSOR_BLINK_INTERVAL, GX_CURSOR_BLINK_INTERVAL);
            }
        }

        text_select_(0, string_size - 1);

        status_add_(GX_STATUS_RESIZE_NOTIFY);

        result = GX_WIDGET::event_process_(event_ptr);
        break;

    case GX_EVENT_FOCUS_LOST:
        if (!(this->style & GX_STYLE_CURSOR_ALWAYS))
        {
            if (this->style & GX_STYLE_CURSOR_BLINK)
            {
                timer_stop_(GX_TEXT_INPUT_TIMER);
            }

            status_remove_(GX_STATUS_CURSOR_SHOW);

            if (this->start_mark == this->end_mark)
            {
                cursor_area_dirty_mark_();
            }

            status_remove_(GX_STATUS_RESIZE_NOTIFY);
        }

        if (this->start_mark != this->end_mark)
        {
            selected_area_dirty_mark_(this->start_mark, this->end_mark);

            this->start_mark = 0;
            this->end_mark = 0;
        }

        if (was_modified)
        {
            signal_parent_(GX_EVENT_TEXT_EDITED, 0);
            was_modified = GX_FALSE;
        }

        result = GX_WIDGET::event_process_(event_ptr);
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

    case GX_EVENT_TIMER:
        if ((event_ptr->payload.timer_id == GX_TEXT_INPUT_TIMER) &&
            (this->status & GX_STATUS_CURSOR_SHOW))
        {
            if (this->status & GX_STATUS_CURSOR_DRAW)
            {
                status_remove_(GX_STATUS_CURSOR_DRAW);
            }
            else
            {
                status_add_(GX_STATUS_CURSOR_DRAW);
            }

            cursor_area_dirty_mark_();
        }
        else if (event_ptr->payload.timer_id == GX_MARK_TIMER)
        {
            if (this->status & GX_STATUS_MARK_PREVIOUS)
            {
                mark_previous_();
            }
            else
            {
                mark_next_();
            }
        }
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

    case GX_EVENT_MARK_NEXT:
        mark_next_();
        break;

    case GX_EVENT_MARK_PREVIOUS:
        mark_previous_();
        break;

    case GX_EVENT_MARK_HOME:
        mark_home_();
        break;

    case GX_EVENT_MARK_END:
        mark_end_();
        break;

    case GX_EVENT_DELETE:
        if (this->status & GX_STATUS_DYNAMIC_BUFFER)
        {
            if (!_gx_system_memory_free)
            {
                return GX_SYSTEM_MEMORY_ERROR;
            }

            _gx_system_memory_free(buffer);
            buffer = GX_NULL;
        }
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
/*    _gx_single_line_text_input_backspace                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service processes a backspace character.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control blcok               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memmove                               Move a block of memory        */
/*    _gx_widget_font_get                   Get font by specified ID      */
/*    _gx_widget_client_get                 Get client rectangle          */
/*    _gx_widget_border_width_get           Get the widget border width   */
/*    _gx_system_string_width_get           Get the width of a string     */
/*    _gx_system_dirty_partial_add          Mark the partial area of a    */
/*                                            widget as dirty             */
/*    _gx_utility_utf8_string_character_get Parse utf8 string to          */
/*                                            multi-byte glyph            */
/*    _gx_single_line_text_input_position_update                          */
/*                                          Update cursor position        */
/*                                            according to insert position*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_backspace(GX_SINGLE_LINE_TEXT_INPUT* text_input)
{
    return text_input->backspace_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_buffer_clear                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service deletes all characters from the text input buffer.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_border_width_get           Get the widget border width   */
/*    _gx_widget_client_get                 Get the client rectangle      */
/*    _gx_system_dirty_mark                 Mark the widget area as dirty */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_buffer_clear(GX_SINGLE_LINE_TEXT_INPUT* text_input)
{
    text_input->buffer_clear_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    gx_single_line_text_input_buffer_get                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service retrieves buffer information of the text input widget. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input_ptr                        Single line text input        */
/*                                            control blcok               */
/*    buffer_address                        The address of the input      */
/*                                            buffer                      */
/*    content_size                          The count of the input data   */
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
UINT _gx_single_line_text_input_buffer_get(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_CHAR** buffer_address,
                                           UINT* content_size, UINT* buffer_size)
{
    text_input->buffer_get_(buffer_address, content_size, buffer_size);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_create                                   */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service creates a single-line text input widget.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                        Single-line text input widget     */
/*                                        control block                   */
/*    name                              Name of text input widget         */
/*    parent                            Pointer to parent widget          */
/*    input_buffer                      Pointer to text input buffer      */
/*    buffer_size                       Size of text input buffer         */
/*    style                             Style of text input widget.       */
/*    text_input_id                     Application-defined ID for text   */
/*                                        input                           */
/*    size                              Dimensions of text input widget   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Completion status                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_prompt_create                 Create a prompt                   */
/*    _gx_widget_border_width_get       Get widget border width           */
/*    _gx_widget_client_get             Get widget client rectangle       */
/*    _gx_widget_link                   Link the widget to its parent     */
/*    gx_string_length_get_   Test string length                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_create(GX_SINGLE_LINE_TEXT_INPUT* text_input,
                                       const GX_CHAR* name,
                                       GX_WIDGET* parent,
                                       GX_CHAR* input_buffer,
                                       UINT buffer_size,
                                       UINT style,
                                       USHORT Id,
                                       const GX_RECTANGLE* size)
{
    return text_input->create_(name, parent, input_buffer, buffer_size, style, Id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_character_delete                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service deletes the character after the text input cursor      */
/*    position.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                             Single line text input       */
/*                                            control blcok               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memmove                               Move a block of memory        */
/*    _gx_widget_border_width_get           Get widget border width       */
/*    _gx_widget_client_get                 Get widget client rectangle   */
/*    _gx_widget_font_get                   Get font by specified ID      */
/*    _gx_system_string_width_get           Get the width of a string     */
/*    _gx_system_dirty_partial_add          Mark the partial area of a    */
/*                                            widget as dirty             */
/*    _gx_utility_utf8_string_character_get Parse utf8 string to          */
/*                                            multi-byte glyph            */
/*    _gx_single_line_text_input_position_update                          */
/*                                          Update cursor position        */
/*                                            according to insert position*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_character_delete(GX_SINGLE_LINE_TEXT_INPUT* text_input)
{
    return text_input->text_delete_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_character_insert                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This services inserts a character into the text input string at the */
/*    current cursor position.                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*    insert_str                            Byte string of insert         */
/*                                            character                   */
/*    insert_size                           String size of insert         */
/*                                            character                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memmove                               Move a block of memory        */
/*    _gx_widget_font_get                   Get font by specified ID      */
/*    _gx_widget_border_width_get           Get the widget border width   */
/*    _gx_widget_client_get                 Get widget client rectangle   */
/*    _gx_system_string_width_get           Get the width of a string     */
/*    _gx_system_dirty_partial_add          Mark the partial area of a    */
/*                                            widget as dirty             */
/*    _gx_single_line_text_input_backspace  Remove text before the cursor */
/*    _gx_single_line_text_input_character_delete                         */
/*                                          Remove text after the cursor  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_character_insert(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_UBYTE* str, UINT str_size)
{
    return text_input->text_insert_(str, str_size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_draw                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws a text input widget. This service is normally    */
/*    called internally during canvas refresh, but can also be called from*/
/*    custom text input drawing functions.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block.              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_background_draw            Draw widget background        */
/*    _gx_widget_border_draw                Draw widget border            */
/*    _gx_widget_border_width_get           Get widget border width       */
/*    _gx_widget_client_get                 Get widget client rectangle   */
/*   _gx_widget_children_draw               Draw widget children          */
/*    _gx_context_line_color_set            Set the line color for the    */
/*                                            context                     */
/*    _gx_context_font_set                  Set the font in the context   */
/*    _gx_context_brush_width_set           Set the width of brush        */
/*    _gx_canvas_text_draw                  Draw the text                 */
/*    _gx_canvas_drawing_initiate           Initiate drawing on specified */
/*                                            canvas                      */
/*    _gx_canvas_drawing_complete           Complete drawing on specified */
/*                                            canvas                      */
/*    _gx_text_input_cursor_draw            Draw a text input cursor      */
/*    _gx_utility_rectangle_overlap_detect  Detect overlap of the         */
/*                                            supplied rectangles         */
/*    _gx_single_line_text_input_draw_position_get                        */
/*                                          Get text draw start position  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_single_line_text_input_draw(GX_SINGLE_LINE_TEXT_INPUT* text_input)
{
    text_input->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_draw_position_get                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service calculates draw start position of text input text.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block.              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_brush_get                 Get brush of current context  */
/*    _gx_widget_border_width_get           Get widget border width       */
/*    _gx_widget_client_get                 Get widget client rectangle   */
/*    _gx_system_string_width_get           Get the width of a string     */
/*    _gx_single_line_text_input_position_update                          */
/*                                          Update cursor position by     */
/*                                            insert index                */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_single_line_text_input_draw                                     */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_draw_position_get(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_VALUE* xpos, GX_VALUE* ypos)
{
    return text_input->draw_position_get_(xpos, ypos) ? GX_SUCCESS : GX_FAILURE;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_end                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service positions the text input widget cursor at the end of   */
/*    the input string.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single line text input widget */
/*                                            control blcok               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_font_get                   Get font by specified ID      */
/*    _gx_widget_border_width_get           Get the widget border width   */
/*    _gx_widget_client_get                 Retrieves client area of the  */
/*                                            widget                      */
/*    _gx_system_string_width_get           Get the width of a string     */
/*    _gx_system_dirty_mark                 Mart the area of the widget   */
/*                                            dirty                       */
/*    _gx_system_dirty_partial_add          Mark the partial area of a    */
/*                                            widget as dirty             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_end(GX_SINGLE_LINE_TEXT_INPUT* text_input)
{
    text_input->end_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_event_process                            */
/*                                                           6.4.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service processes a single line text input event. This function*/
/*    is internally referenced by the gx_single_line_text_input_create    */
/*    function, but is exposed for use by the application in those cases  */
/*    where the application defines a custom single line text input event */
/*    processing function.                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*    event_ptr                             Pointer to GX_EVENT structure */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_process              Default widget event process  */
/*    _gx_widget_event_generate             Create a event and send it to */
/*                                            parent                      */
/*    _gx_system_timer_start                Start the system timer        */
/*    _gx_system_timer_stop                 Stop the system timer         */
/*    _gx_system_dirty_partial_add          Mark the partial area of a    */
/*                                            widget as dirty             */
/*    _gx_text_input_cursor_dirty_rectangle_get                           */
/*                                          Get cursor rectangle          */
/*    _gx_single_line_text_input_keydown_process                          */
/*                                          Keydown event process function*/
/*    _gx_single_line_text_input_position_update                          */
/*                                          Update cursor position by     */
/*                                            insert position             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_event_process(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_EVENT* event_ptr)
{
    return text_input->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_fill_color_set                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the fill color of a single line text input widget.*/
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Single-line text input widget */
/*                                            control block               */
/*    normal_fill_color_id                  Resource ID of the normal     */
/*                                            fill color                  */
/*    selected_fill_color_id                Resource ID of the selected   */
/*                                            fill color                  */
/*    disabled_fill_color_id                Resource ID of the disabled   */
/*                                            fill color                  */
/*    readonly_fill_color_id                Resource ID of the read only  */
/*                                            fill color                  */
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
UINT _gx_single_line_text_input_fill_color_set(GX_SINGLE_LINE_TEXT_INPUT* text_input,
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
/*    _gx_single_line_text_input_home                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service moves the text input cursor position to the start of   */
/*    the input string.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_font_get                   Get font by specified ID      */
/*    _gx_widget_border_width_get           Get the widget border width   */
/*    _gx_widget_client_get                 Retrieves client area of the  */
/*                                            widget                      */
/*    _gx_system_string_width_get           Get the width of a string     */
/*    _gx_system_dirty_mark                 Mart the area of the widget   */
/*                                            dirty                       */
/*    _gx_system_dirty_partial_add          Mark the partial area of a    */
/*                                            widget as dirty             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_home(GX_SINGLE_LINE_TEXT_INPUT* text_input)
{
    text_input->home_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_left_arrow                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service moves the text input cursor one character position to  */
/*      the left.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_utf8_string_character_get Parse utf8 string to          */
/*                                            multi-byte glyph            */
/*    _gx_widget_border_width_get           Get the widget border width   */
/*    _gx_widget_client_get                 Get widget client rectangle   */
/*    _gx_widget_font_get                   Get font by specified ID      */
/*    _gx_system_string_width_get           Get the width of a string     */
/*    _gx_system_dirty_mark                 Mark the area of the widget   */
/*                                            as dirty                    */
/*    _gx_system_dirty_partial_add          Mark the partial area of a    */
/*                                            widget as dirty             */
/*    _gx_text_input_cursor_dirty_rectangle_get                           */
/*                                          Get cursor rectangle          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_left_arrow(GX_SINGLE_LINE_TEXT_INPUT* text_input)
{
    text_input->left_arrow_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_position_get                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service positions the text input cursor based on the requested */
/*    cursor position. The text input cursor index will be calculated     */
/*    based on the x value of the pixel position.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                                Single-line text input    */
/*                                                widget control block    */
/*    pixel_position                            X value of pixel position */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                    Completion status         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_font_get                   Get widget font               */
/*    _gx_widget_border_width_get           Get the widget border width   */
/*    _gx_widget_client_get                 Get client rectangle          */
/*    _gx_utility_utf8_string_character_get         Parse utf8 string to          */
/*                                            multi-byte glyph            */
/*    _gx_system_string_width_get           Get the width of a string     */
/*    _gx_system_dirty_mark                 Mark a widget area dirty      */
/*    _gx_system_dirty_partial_add          Mark the partial area of a    */
/*                                            widget as dirty             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_position_get(GX_SINGLE_LINE_TEXT_INPUT* text_input, INT pixel_position)
{
    text_input->position_get_(pixel_position);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_right_arrow                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service moves the text input cursor one character position to  */
/*    the right.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                           Single-line text input widget  */
/*                                           control block                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_font_get                   Get the font of the text      */
/*    _gx_widget_border_width_get           Get the widget border width   */
/*    _gx_widget_client_get                 Get widget client rectangle   */
/*    _gx_system_string_width_get           Get the width of a string     */
/*    _gx_system_dirty_partial_add          Mark the specified area of    */
/*                                            a widget as dirty           */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*    _gx_text_input_cursor_dirty_rectangle_get                           */
/*                                          Get cursor rectangle          */
/*    _gx_utility_utf8_string_character_get         Parse utf8 string to          */
/*                                            multi-byte glyph            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_right_arrow(GX_SINGLE_LINE_TEXT_INPUT* text_input)
{
    text_input->right_arrow_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_style_add                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service adds styles to a single line text input widget.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*    style                                 New style to add              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_timer_start                Start the GUIX system timer   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_style_add(GX_SINGLE_LINE_TEXT_INPUT* text_input, ULONG style)
{
    text_input->style_add_(style);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_style_remove                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service removes styles from a single line text input widget.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                           control block                */
/*    style                                 Styles to remove              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_style_remove(GX_SINGLE_LINE_TEXT_INPUT* text_input, ULONG style)
{
    text_input->style_remove_(style);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_style_set                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This Service sets styles for single line text input wigdet.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*    style                                 Style flas to assign          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_timer_start                Start the GUIX system timer   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_style_set(GX_SINGLE_LINE_TEXT_INPUT* text_input, ULONG style)
{
    text_input->style_set_(style);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_text_color_set                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the text color of a single line text input widget.*/
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Single-line text input widget */
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
UINT _gx_single_line_text_input_text_color_set(GX_SINGLE_LINE_TEXT_INPUT* text_input,
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
/*    _gx_single_line_text_input_text_select                              */
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
/*    text_input                            Single-line text input widget */
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
UINT _gx_single_line_text_input_text_select(GX_SINGLE_LINE_TEXT_INPUT* text_input, UINT start_index, UINT end_index)
{
    text_input->text_select_(start_index, end_index);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_single_line_text_input_text_set_ext                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns a text string to a single line text input     */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
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
/*    memcpy                                Copy block of memory          */
/*    _gx_utility_utf8_string_character_get         Retrieve glyph code and length*/
/*    _gx_single_line_text_input_position_update                          */
/*                                          Update cursor position        */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_single_line_text_input_text_set_ext(GX_SINGLE_LINE_TEXT_INPUT* text_input, const GX_STRING* text)
{
    text_input->text_set_ext_(text);

    return GX_SUCCESS;
}
