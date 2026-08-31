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
/**   Rich Text View Management (Rich Text View)                          */
/**                                                                       */
/**************************************************************************/

#include "gx_rich_text_view.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_canvas.h"
#include "gx_font.h"
#include "gx_scroll_bar.h"
#include "gx_text_area_info.h"

// rich text view context stack

static UINT _gx_rich_text_view_context_reset(void);
static UINT _gx_rich_text_view_context_peek(GX_RICH_TEXT_CONTEXT* context);
static UINT _gx_rich_text_view_context_push(GX_RICH_TEXT_CONTEXT* context);
static UINT _gx_rich_text_view_context_pop(void);

static UINT _gx_rich_text_view_context_save(void);
static UINT _gx_rich_text_view_context_restore(void);


static void gx_rich_text_view_draw_(GX_RICH_TEXT_VIEW* view)
{
    view->draw_();
}

static void gx_rich_text_view_scroll_info_get_(GX_RICH_TEXT_VIEW* view, ULONG style, GX_SCROLL_INFO* info)
{
    view->scroll_info_get_(style, info);
}

UINT GX_RICH_TEXT_VIEW::create_(const GX_CHAR* name,
                                GX_WIDGET* parent,
                                GX_RESOURCE_ID text_id,
                                GX_RICH_TEXT_FONTS* fonts,
                                ULONG style,
                                USHORT id,
                                const GX_RECTANGLE* size)
{
    GX_MULTI_LINE_TEXT_VIEW::create_(name, GX_NULL, text_id, style, id, size);

    this->type = GX_TYPE_RICH_TEXT_VIEW;

    this->fonts = *fonts;
    this->text_total_height = 0;

    this->draw_function = (void (*)(GX_WIDGET*))gx_rich_text_view_draw_;
    this->scroll_info_get = (void (*)(GX_WINDOW*, ULONG, GX_SCROLL_INFO*))gx_rich_text_view_scroll_info_get_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

UINT GX_RICH_TEXT_VIEW::fonts_set_(GX_RICH_TEXT_FONTS* fonts)
{
    this->fonts = *fonts;

    line_index_cache_old = GX_TRUE;

    dirty_mark_if_visible_();

    return GX_SUCCESS;
}

UINT GX_RICH_TEXT_VIEW::scroll_info_get_(ULONG style, GX_SCROLL_INFO* info)
{
    GX_PARAMETER_NOT_USED(style);

    GX_FONT* font;
    font_get_(fonts.normal_id, &font);
    if (!font)
    {
        return GX_FAILURE;
    }

    if (line_index_cache_old)
    {
        text_total_height_calculate_();
    }

    GX_VALUE line_height = (GX_VALUE)(font->line_height + line_space);

    GX_RECTANGLE disp_rect;
    display_area_(&disp_rect);

    info->minimum = disp_rect.top;
    info->maximum = info->minimum + text_total_height - 1;
    info->visible = disp_rect.height_();
    if ((GX_VALUE)text_total_height < info->visible)
    {
        info->maximum = info->minimum + info->visible - 1;
    }

    INT value = disp_rect.top - text_scroll_shift;

    if (value < info->minimum)
    {
        value = info->minimum;
    }
    else if (value > info->maximum - info->visible + 1)
    {
        value = info->maximum - info->visible + 1;
    }

    INT shift = disp_rect.top - value;
    if (shift != text_scroll_shift)
    {
        text_scroll_shift = shift;
    }

    info->value = value;
    info->increment = line_height;

    return GX_SUCCESS;
}

UINT GX_RICH_TEXT_VIEW::text_total_height_calculate_()
{
    if (text_().length == 0)
    {
        text_total_height = 0;
        line_index_cache_old = GX_FALSE;
        return GX_SUCCESS;
    }

    INT total_height = 0;

    GX_RECTANGLE disp_rect;
    display_area_(&disp_rect);

    const GX_VALUE x_width = gx_text_area_width_(disp_rect);

    GX_RICH_TEXT_FORMAT format;
    format.font_id = fonts.normal_id;
    format.flags = 0;

    GX_FONT* font;
    GX_RICH_TEXT_LINE_INFO line_info;

    GX_STRING text;
    text_data.get_(&text);

    while (text.length > 0)
    {
        font_get_(format.font_id, &font);
        if (!font)
        {
            return GX_INVALID_FONT;
        }

        line_info.text.ptr = text.ptr;
        line_info.text.length = 0;
        line_info.start_format = format;
        line_info.end_format = format;
        line_info.line_height = font->line_height;
        line_info.text_width = 0;

        line_info_get_(text, &line_info, x_width);

        total_height += (line_info.line_height + line_space);

        format = line_info.end_format;

        text.ptr += line_info.text.length;
        text.length -= line_info.text.length;
    }

    _gx_rich_text_view_context_reset();

    text_total_height = (UINT)total_height;
    line_index_cache_old = GX_FALSE;

    return GX_SUCCESS;
}

UINT GX_RICH_TEXT_VIEW::line_info_get_(GX_STRING text, GX_RICH_TEXT_LINE_INFO* line_info, GX_VALUE availlable_width)
{
    GX_RICH_TEXT_FORMAT text_format = line_info->start_format;
    GX_RESOURCE_ID font_id = text_format.font_id;

    GX_FONT* font;
    font_get_(font_id, &font);
    if (!font)
    {
        return GX_INVALID_FONT;
    }

    GX_RICH_TEXT_LINE_INFO break_info = *line_info;
    bool escape = false;
    GX_UBYTE processed_count;
    UINT glyph_len = 1;
    GX_VALUE glyph_width;
    GX_STRING string;
    INT tail_space_width = 0;

    while (text.length > 0)
    {
        if (!escape && text.ptr[0] == '\\')
        {
            escape = true;

            ++text.ptr;
            --text.length;

            ++line_info->text.length;

            continue;
        }

        if (!escape && text.ptr[0] == '<' &&
            tag_enter_(&text, &text_format, &processed_count) == GX_SUCCESS)
        {
            if (text_format.font_id != font_id)
            {
                font_id = text_format.font_id;

                font_get_(font_id, &font);
                if (!font)
                {
                    return GX_INVALID_FONT;
                }

                if (line_info->line_height < font->line_height)
                {
                    line_info->line_height = font->line_height;
                    line_info->baseline = font->baseline;
                }
            }

            line_info->text.length += processed_count;
            line_info->end_format = text_format;

            text.ptr += processed_count;
            text.length -= processed_count;

            continue;
        }

#if defined GX_UTF8_SUPPORT
        string = text;
        if (_gx_utility_utf8_string_character_get(&string, GX_NULL, &glyph_len) != GX_SUCCESS)
        {
            return GX_INVALID_STRING;
        }
#endif

        glyph_width = _gx_system_string_width_get(font, text.ptr, glyph_len);

        if (text.ptr[0] == ' ')
        {
            tail_space_width += glyph_width;
        }
        else
        {
            tail_space_width = 0;
        }

        if (text.ptr[0] == '\r')
        {
            glyph_len = 1;

            if (text.length > 1 && text.ptr[1] == '\n')
            {
                ++glyph_len;
            }

            line_info->text.length += glyph_len;
            break;
        }
        else if (text.ptr[0] == '\n')
        {
            line_info->text.length += glyph_len;
            break;
        }
        else if ((line_info->text_width + (UINT)glyph_width > (UINT)availlable_width) &&
                 line_info->text_width && (text.ptr[0] != ' '))
        {
            if (break_info.text_width > 0)
            {
                *line_info = break_info;
            }

            break;
        }

        line_info->text_width += (UINT)glyph_width;
        line_info->text.length += glyph_len;

        if ((text.ptr[0] == ' ') || (text.ptr[0] == ',') || (text.ptr[0] == ';'))
        {
            break_info = *line_info;

            if (tail_space_width)
            {
                break_info.text_width -= (UINT)tail_space_width;
            }
        }

        escape = false;

        text.ptr += glyph_len;
        text.length -= glyph_len;
    }

    return GX_SUCCESS;
}

/* Define rich text end tags. */
static const GX_CHAR close_tag_bold[] = "</b>";
static const GX_CHAR close_tag_italic[] = "</i>";
static const GX_CHAR close_tag_underline[] = "</u>";
static const GX_CHAR close_tag_font[] = "</f>";
static const GX_CHAR close_tag_color[] = "</c>";
static const GX_CHAR close_tag_hicolor[] = "</hc>";
static const GX_CHAR close_tag_align[] = "</align>";

/* Define end tag string list. */
static const GX_STRING gx_rich_text_end_tag_list[] =
{
    {close_tag_bold, sizeof(close_tag_bold) - 1},
    {close_tag_italic, sizeof(close_tag_italic) - 1},
    {close_tag_underline, sizeof(close_tag_underline) - 1},
    {close_tag_font, sizeof(close_tag_font) - 1},
    {close_tag_color, sizeof(close_tag_color) - 1},
    {close_tag_hicolor, sizeof(close_tag_hicolor) - 1},
    {close_tag_align, sizeof(close_tag_align) - 1}
};

/* Define tag index to access tag list. */
enum gx_rich_text_tag_index
{
    GX_RICH_TEXT_TAG_INDEX_BOLD = 0,
    GX_RICH_TEXT_TAG_INDEX_ITALIC,
    GX_RICH_TEXT_TAG_INDEX_UNDERLINE,
    GX_RICH_TEXT_TAG_INDEX_FONT,
    GX_RICH_TEXT_TAG_INDEX_COLOR,
    GX_RICH_TEXT_TAG_INDEX_HICOLOR,
    GX_RICH_TEXT_TAG_INDEX_ALIGN
};

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_rich_text_resource_id_read                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to read resource id.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text                                  Pointer to rich text          */
/*    start_index                           Start index of the resource   */
/*                                            text                        */
/*    length                                Length of the input string    */
/*    resource_id                           Retrieved resource id         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    result                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_rich_text_view_tag_open                                         */
/*                                                                        */
/**************************************************************************/
static UINT _gx_rich_text_resource_id_read(GX_STRING* text, UINT* resource_id)
{
    UINT result = GX_FAILURE;
    UINT id = 0;
    CHAR ch;

    if (text->length < 2)
    {
        return result;
    }

    ch = text->ptr[0];

    if ((ch > '9') || (ch < '0'))
    {
        /* Invalid id. */
        return result;
    }

    while (text->length > 0)
    {
        ch = text->ptr[0];

        ++text->ptr;
        --text->length;

        if ((ch <= '9') && (ch >= '0'))
        {
            id = id * 10 + (UINT)(ch - '0');
        }
        else
        {
            if (ch == '>')
            {
                result = GX_SUCCESS;
            }
            break;
        }
    }

    if (result == GX_SUCCESS)
    {
        *resource_id = id;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_rich_text_view_tag_open                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function detects rich text open tag from specified text        */
/*    position and updated rich text draw style accordion to tag type.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    view                                  Rich text view control block  */
/*    text                                  Text for processing           */
/*    format                                Current rich text format      */
/*    handled_bytes                         Bytes been processed          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    result                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_string_compare            Test if two strings equal     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_rich_text_view_tag_enter                                        */
/*                                                                        */
/**************************************************************************/
static UINT _gx_rich_text_view_tag_open(GX_RICH_TEXT_VIEW* view,
                                        const GX_STRING* text,
                                        GX_RICH_TEXT_FORMAT* format,
                                        GX_UBYTE* handled_bytes)
{
    UINT                 result = GX_FAILURE;
    GX_STRING            string;
    GX_STRING            tag_string;
    GX_UBYTE             alignment;
    UINT                 resource_id = 0;
    GX_RICH_TEXT_CONTEXT context;
    UINT                 tag_index = 0;

    string = *text;

    context.format = *format;

    switch (string.ptr[1])
    {
    case 'b':
        /* <b>: bold */
        if (string.ptr[2] == '>')
        {
            string.ptr += 3;
            string.length -= 3;

            format->flags |= GX_RICH_TEXT_BOLD;
            if (format->flags & GX_RICH_TEXT_ITALIC)
            {
                format->font_id = view->fonts.bold_italic_id;
            }
            else
            {
                format->font_id = view->fonts.bold_id;
            }
            tag_index = GX_RICH_TEXT_TAG_INDEX_BOLD;
            result = GX_SUCCESS;
        }
        break;

    case 'i':
        /* <i>: italic */
        if (string.ptr[2] == '>')
        {
            string.ptr += 3;
            string.length -= 3;

            format->flags |= GX_RICH_TEXT_ITALIC;
            if (format->flags & GX_RICH_TEXT_BOLD)
            {
                format->font_id = view->fonts.bold_italic_id;
            }
            else
            {
                format->font_id = view->fonts.italic_id;
            }
            tag_index = GX_RICH_TEXT_TAG_INDEX_ITALIC;
            result = GX_SUCCESS;
        }
        break;

    case 'u':
        /* <u>: underline*/
        if (string.ptr[2] == '>')
        {
            string.ptr += 3;
            string.length -= 3;

            format->flags |= GX_RICH_TEXT_UNDERLINE;

            tag_index = GX_RICH_TEXT_TAG_INDEX_UNDERLINE;
            result = GX_SUCCESS;
        }
        break;

    case 'f':
        /* <f font-id>: select font */
        if (string.ptr[2] == ' ')
        {
            string.ptr += 3;
            string.length -= 3;

            /* font id should <= 0xff */
            result = _gx_rich_text_resource_id_read(&string, &resource_id);

            if (result != GX_SUCCESS)
            {
                return result;
            }

            tag_index = GX_RICH_TEXT_TAG_INDEX_FONT;
            format->font_id = resource_id;
        }
        break;

    case 'c':
        /* <c color_id>: text color */
        if (string.ptr[2] == ' ')
        {
            string.ptr += 3;
            string.length -= 3;

            result = _gx_rich_text_resource_id_read(&string,  &resource_id);

            if (result != GX_SUCCESS)
            {
                return result;
            }

            tag_index = GX_RICH_TEXT_TAG_INDEX_COLOR;
            format->color = resource_id;
        }
        break;

    case 'h':
        /* <hc color_id>: highlight color */
        if (string.length > 3)
        {
            if (string.ptr[2] == 'c' && string.ptr[3] == ' ')
            {
                string.ptr += 4;
                string.length -= 4;
                result = _gx_rich_text_resource_id_read(&string, &resource_id);

                if (result != GX_SUCCESS)
                {
                    return result;
                }

                tag_index = GX_RICH_TEXT_TAG_INDEX_HICOLOR;
                format->highlight_color = resource_id;
            }
        }
        break;

    case 'a':
        /* <align align-value>: alignment */
        tag_string.ptr = "lign ";
        tag_string.length = sizeof("lign ") - 1;

        string.ptr += 2;
        string.length -= 2;

        if (tag_string.length <= string.length)
        {
            if (_gx_utility_string_compare(&string, &tag_string, tag_string.length) == GX_TRUE)
            {
                string.ptr += tag_string.length;
                string.length -= tag_string.length;

                switch (string.ptr[0])
                {
                case 'c':
                    tag_string.ptr = "enter>";
                    tag_string.length = sizeof("enter>") - 1;
                    alignment = GX_RICH_TEXT_CENTER;
                    break;

                case 'r':
                    tag_string.ptr = "ight>";
                    tag_string.length = sizeof("ight>") - 1;
                    alignment = GX_RICH_TEXT_RIGHT;
                    break;

                case 'l':
                    tag_string.ptr = "eft>";
                    tag_string.length = sizeof("eft>") - 1;
                    alignment = GX_RICH_TEXT_LEFT;
                    break;

                default:
                    return GX_FAILURE;
                }

                ++string.ptr;
                --string.length;

                if (tag_string.length <= string.length)
                {
                    if (_gx_utility_string_compare(&string, &tag_string, tag_string.length) == GX_TRUE)
                    {
                        format->flags &= (GX_UBYTE)(~GX_RICH_TEXT_ALIGN_MASK);
                        format->flags |= alignment;
                        string.length -= tag_string.length;

                        tag_index = GX_RICH_TEXT_TAG_INDEX_ALIGN;
                        result = GX_SUCCESS;
                    }
                }
            }
        }
        break;

    default:
        return GX_FAILURE;
    }

    if (result == GX_SUCCESS)
    {
        context.tag = &gx_rich_text_end_tag_list[tag_index];

        /* Push draw style to stack. */
        result = _gx_rich_text_view_context_push(&context);
    }

    if (result == GX_SUCCESS)
    {
        *handled_bytes = (GX_UBYTE)(text->length - string.length);
    }

    return result;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_rich_text_view_tag_close                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function detects rich text close tag from specified text       */
/*    position and updated rich text draw style accordion to tag type.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text                                  Text for processing           */
/*    format                                Current rich text format      */
/*    handled_bytes                         Bytes been processed          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    result                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_string_compare            Test if two strings equal     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_rich_text_view_tag_enter                                        */
/*                                                                        */
/**************************************************************************/
static UINT _gx_rich_text_view_tag_close(const GX_STRING* text, GX_RICH_TEXT_FORMAT* format, GX_UBYTE* handled_bytes)
{
    UINT                 result = GX_FAILURE;
    GX_RICH_TEXT_CONTEXT context;
    const GX_STRING*     tag;

    /* Peek tag in top of the stack. */
    result = _gx_rich_text_view_context_peek(&context);

    if (result == GX_SUCCESS)
    {
        tag = context.tag;

        /* Test if we meet the end tag in stack top. */
        if (_gx_utility_string_compare(text, tag, tag->length) == GX_TRUE)
        {
            /* Yes, it's time to pop the style. */
            _gx_rich_text_view_context_pop();
            *format = context.format;
            *handled_bytes = (GX_UBYTE)tag->length;
        }
        else
        {
            result = GX_FAILURE;
        }
    }

    return result;
}

UINT GX_RICH_TEXT_VIEW::tag_enter_(const GX_STRING* text, GX_RICH_TEXT_FORMAT* format, GX_UBYTE* handled_bytes)
{
    if (text->ptr[0] != '<')
    {
        // Tag must start with '<'.
        return GX_FAILURE;
    }

    if (text->length < 3)
    {
        // Tag contains at least 3 character: <tag-name>, <tag-name tag-value>, </tag-name>.
        return GX_FAILURE;
    }

    if (text->ptr[1] == '/')
    {
        if (text->length < 4)
        {
            // Close tag contains at least 4 character: </tag-name>.
            return GX_FAILURE;
        }

        return _gx_rich_text_view_tag_close(text, format, handled_bytes);
    }
    else
    {
        return _gx_rich_text_view_tag_open(this, text, format, handled_bytes);
    }
}

void GX_RICH_TEXT_VIEW::single_line_draw_(GX_VALUE y_pos, GX_RICH_TEXT_LINE_INFO* line_info)
{
    GX_STRING text = line_info->text;
    GX_RICH_TEXT_FORMAT format = line_info->start_format;
    GX_UBYTE processed_count;

    while (text.ptr[0] == '<')
    {
        if (tag_enter_(&text, &format, &processed_count) == GX_SUCCESS)
        {
            text.ptr += processed_count;
            text.length -= processed_count;
        }
        else
        {
            break;
        }
    }

    GX_RICH_TEXT_FORMAT start_format = format;

    GX_RECTANGLE disp_rect;
    display_area_(&disp_rect);

    const GX_VALUE x_left = gx_text_area_left_(disp_rect);
    const GX_VALUE x_right = gx_text_area_right_(disp_rect);
    const GX_VALUE x_width = gx_text_area_width_(disp_rect);

    GX_VALUE x_pos;

    switch (format.flags & GX_RICH_TEXT_ALIGN_MASK)
    {
    case GX_RICH_TEXT_RIGHT:
        x_pos = (GX_VALUE)(x_right - line_info->text_width);
        break;

    case GX_RICH_TEXT_CENTER:
        x_pos = (GX_VALUE)(x_left + (x_width - line_info->text_width) / 2);
        break;

    case GX_RICH_TEXT_LEFT:
    default:
        x_pos = x_left;
        break;
    }

    GX_STRING draw_text;
    draw_text.ptr = text.ptr;
    draw_text.length = 0;

    bool draw_start = false;
    bool escape = false;

    while (text.length > 0)
    {
        processed_count = 1;

        if (!escape && text.ptr[0] == '\\')
        {
            escape = true;

            if (draw_text.length == 0)
            {
                ++draw_text.ptr;
            }
            else
            {
                draw_start = true;
            }
        }
        else
        {
            if (!escape && text.ptr[0] == '<' &&
                tag_enter_(&text, &format, &processed_count) == GX_SUCCESS)
            {
                if (draw_text.length == 0)
                {
                    draw_text.ptr += processed_count;

                    start_format = format;
                }
                else
                {
                    draw_start = true;
                }
            }
            else
            {
                draw_text.length += processed_count;
            }

            escape = false;
        }

        text.ptr += processed_count;
        text.length -= processed_count;

        // If we come the the last character, or draw_text flag is true, process drawing.
        if ((text.length == 0) || draw_start)
        {
            GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

            GX_FONT* font;
            context->font_get_(start_format.font_id, &font);

            GX_VALUE text_width;
            _gx_system_string_width_get_ext(font, &draw_text, &text_width);

            context->line_color_set_(start_format.color);
            context->font_set_(start_format.font_id);

            if (start_format.highlight_color != normal_fill_color)
            {
                context->brush.width = 0;
                context->fill_color_set_(start_format.highlight_color);

                GX_RECTANGLE draw_area;
                draw_area.left = (GX_VALUE)x_pos;
                draw_area.right = (GX_VALUE)(x_pos + text_width - 1);
                draw_area.top = (GX_VALUE)(y_pos - line_space / 2);
                draw_area.bottom = (GX_VALUE)(draw_area.top + line_info->line_height - 1);

                _gx_canvas_rectangle_draw(&draw_area);
            }

            GX_VALUE y = (GX_VALUE)(y_pos + line_info->baseline - font->baseline);

            _gx_canvas_text_draw_ext(x_pos, y, &draw_text);

            if (start_format.flags & GX_RICH_TEXT_UNDERLINE)
            {
                y = (GX_VALUE)(y_pos + line_info->baseline + 1);

                context->brush.width = 1;

                _gx_canvas_line_draw(x_pos, y, (GX_VALUE)(x_pos + text_width - 1), y);
            }

            x_pos = (GX_VALUE)(x_pos + text_width);

            draw_text.ptr = text.ptr;
            draw_text.length = 0;

            draw_start = false;

            start_format = format;
        }
    }

    line_info->end_format = format;
}

void GX_RICH_TEXT_VIEW::text_draw_()
{
    if (text_().length == 0)
    {
        return;
    }

    if (line_index_cache_old)
    {
        GX_SCROLLBAR* scrollbar = scrollbar_find_(GX_TYPE_VERTICAL_SCROLL);
        if (scrollbar)
        {
            text_total_height_calculate_();
            scrollbar->reset_();
        }
    }

    GX_RICH_TEXT_FORMAT format;
    format.color = normal_text_color_();
    format.highlight_color = normal_fill_color;
    format.font_id = fonts.normal_id;
    format.flags = 0;

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        format.flags |= GX_RICH_TEXT_RIGHT;
        break;

    case GX_STYLE_TEXT_CENTER:
        format.flags |= GX_RICH_TEXT_CENTER;
        break;

    default:
        break;
    }

    GX_STRING text;
    text_data.get_(&text);

    GX_RECTANGLE disp_rect;
    display_area_(&disp_rect);

    const GX_VALUE x_width = gx_text_area_width_(disp_rect);

    GX_VALUE y_pos = (GX_VALUE)(disp_rect.top + text_scroll_shift);
    y_pos = (GX_VALUE)(y_pos + line_space / 2);

    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

    GX_RECTANGLE overlap;
    if (gx_rectangle_intersect_(context->dirty, disp_rect, &overlap))
    {
        GX_DRAWING drawing(context->canvas, this, overlap, GX_FALSE);
        if (drawing.result_() == GX_SUCCESS)
        {
            GX_FONT* font;
            GX_RICH_TEXT_LINE_INFO line_info;

            while (text.length > 0)
            {
                font_get_(format.font_id, &font);
                if (!font)
                {
                    break;
                }

                line_info.text.ptr = text.ptr;
                line_info.text.length = 0;
                line_info.start_format = format;
                line_info.end_format = format;
                line_info.line_height = font->line_height;
                line_info.baseline = font->baseline;
                line_info.text_width = 0;

                _gx_rich_text_view_context_save();

                if (line_info_get_(text, &line_info, x_width) != GX_SUCCESS)
                {
                    break;
                }

                format = line_info.end_format;

                if ((GX_VALUE)(y_pos + line_info.line_height) > disp_rect.top)
                {
                    if (y_pos < disp_rect.bottom)
                    {
                        _gx_rich_text_view_context_restore();

                        single_line_draw_(y_pos, &line_info);
                    }
                    else
                    {
                        break;
                    }
                }

                y_pos = (GX_VALUE)(y_pos + line_info.line_height + line_space);

                text.ptr += line_info.text.length;
                text.length -= line_info.text.length;
            }
        }
    }

    _gx_rich_text_view_context_reset();
}

void GX_RICH_TEXT_VIEW::draw_()
{
    background_draw_();
    text_draw_();
    children_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_rich_text_view_create                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a rich text view widget.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                             Rich text view widget control */
/*                                            block                       */
/*    name                                  Name of the widget            */
/*    parent                                Pointer to parent widget      */
/*    text_id                               Resource ID of the text string*/
/*    fonts                                 Font list                     */
/*    style                                 Style of text view widget     */
/*    id                                    Application-defined ID of text*/
/*                                            view                        */
/*    size                                  Dimension of the widget       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    result                                Completion result             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_create       Create multi line text view   */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_rich_text_view_create(GX_RICH_TEXT_VIEW* view,
                               const GX_CHAR* name,
                               GX_WIDGET* parent,
                               GX_RESOURCE_ID text_id,
                               GX_RICH_TEXT_FONTS* fonts,
                               ULONG style,
                               USHORT id,
                               const GX_RECTANGLE* size)
{
    return view->create_(name, parent, text_id, fonts, style, id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_rich_text_view_text_draw                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws text for a rich text view widget.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                              Rich text view control block */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_background_draw            Draw window background        */
/*    _gx_rich_text_view_text_draw          Draw rich text                */
/*    _gx_widget_children_draw              Draw children                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
void _gx_rich_text_view_draw(GX_RICH_TEXT_VIEW* view)
{
    view->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_rich_text_view_font_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the font of a rich text view widget.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                             Rich text view widget         */
/*                                            control block               */
/*    fonts                                 Font list                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    result                                Completion result             */
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
UINT _gx_rich_text_view_fonts_set(GX_RICH_TEXT_VIEW* view, GX_RICH_TEXT_FONTS* fonts)
{
    return view->fonts_set_(fonts);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_rich_text_view_text_draw                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws text for a rich text view widget.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                              Rich text view control block */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_scrollbar_find             Find scroll bar for specified */
/*                                          window.                       */
/*    _gx_rich_text_view_text_total_height_calculate                      */
/*                                          Calculate text total height   */
/*    _gx_scrollbar_reset                   Reset scrollbar               */
/*    _gx_utility_rectangle_resize          Resize rectangle              */
/*    _gx_widget_string_get_ext             Retrieve text by id           */
/*    _gx_widget_font_get                   Retireve font by id           */
/*    gx_string_get_         Retreive string pointer       */
/*    _gx_system_rich_text_format_stack_clear                             */
/*                                          Clear rich text format stack  */
/*    _gx_system_rich_text_format_stack_switch                            */
/*                                          Switch rich text format stack */
/*    _gx_rich_text_view_line_info_get      Get one line text for drawing */
/*    _gx_rich_text_view_single_line_text_draw                            */
/*                                          Draw a single line            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
void _gx_rich_text_view_text_draw(GX_RICH_TEXT_VIEW* view)
{
    view->text_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    _gx_rich_text_view_context_reset                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function removes all contents of rich text context stack.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    result                                Completion result             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_rich_text_view_context_reset(void)
{
    _gx_system_rich_text_context_stack.top = 0;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    _gx_rich_text_view_context_peek                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves the element at the top of rich text context */
/*    stack.                                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Retrieved top element.        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    result                                Completion result             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_rich_text_view_context_peek(GX_RICH_TEXT_CONTEXT* context)
{
    const GX_RICH_TEXT_CONTEXT_STACK& stack = _gx_system_rich_text_context_stack;

    if (stack.top <= 0 || !context)
    {
        return GX_FAILURE;
    }

    *context = stack.context[stack.top - 1];

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_rich_text_view_context_push                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function adds an element to rich text context stack.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Element to add                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    result                                Completion result             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_rich_text_view_context_push(GX_RICH_TEXT_CONTEXT* context)
{
    if (!context)
    {
        return GX_FAILURE;
    }

    GX_RICH_TEXT_CONTEXT_STACK& stack = _gx_system_rich_text_context_stack;

    if (stack.top >= GX_MAX_RICH_TEXT_CONTEXT_NESTING)
    {
        return GX_RICH_STYLE_NESTING_EXEEDED;
    }

    stack.context[stack.top] = *context;
    ++stack.top;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    _gx_rich_text_view_context_pop                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deletes the top element at the rich text context      */
/*    stack.                                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    result                                Completion result             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_rich_text_view_context_pop(void)
{
    GX_RICH_TEXT_CONTEXT_STACK& stack = _gx_system_rich_text_context_stack;

    if (stack.top <= 0)
    {
        return GX_FAILURE;
    }

    --stack.top;

    return GX_SUCCESS;
}

/**************************************************************************/

static UINT _gx_rich_text_view_context_copy(GX_RICH_TEXT_CONTEXT_STACK& dst,
                                            const GX_RICH_TEXT_CONTEXT_STACK& src)
{
    if (src.top > 0)
    {
        memcpy(dst.context, src.context, src.top * sizeof(GX_RICH_TEXT_CONTEXT));
    }

    dst.top = src.top;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    _gx_rich_text_view_context_save                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function saves current result of rich text context stack.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    stack_type                            Stack type                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    result                                Completion result             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_rich_text_view_context_save(void)
{
    return _gx_rich_text_view_context_copy(_gx_system_rich_text_context_stack_save,
                                           _gx_system_rich_text_context_stack);
}

/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    _gx_rich_text_view_context_restore                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function restores rich text context stack to last save point.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    result                                Completion result             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_rich_text_view_context_restore(void)
{
    return _gx_rich_text_view_context_copy(_gx_system_rich_text_context_stack,
                                           _gx_system_rich_text_context_stack_save);
}
