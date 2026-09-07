#include "gx_text.h"

#include "gx_utility_string.h"
#include "gx_system.h"

void GX_TEXT::create_(GX_WIDGET* owner, GX_RESOURCE_ID text_id, GX_RESOURCE_ID font_id)
{
    this->owner = owner;

    this->text_id = text_id;
    this->string.ptr = GX_NULL;
    this->string.length = 0;

    this->font_id = font_id;

    this->normal_color = GX_COLOR_ID_TEXT;
    this->selected_color = GX_COLOR_ID_SELECTED_TEXT;
    this->disabled_color = GX_COLOR_ID_DISABLED_TEXT;

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    this->bidi_resolved_text_info = GX_NULL;
#endif
}

UINT GX_TEXT::get_(GX_STRING* return_text) const
{
    UINT result = GX_SUCCESS;

    if (text_id)
    {
        result = owner->string_get_ext_(text_id, return_text);
    }
    else
    {
        _gx_system_private_string_get(&string, return_text, owner->style);
    }

    return result;
}

// |font| text font, if not set, the text is handled as a single line
// |display_width| The max width of text in one line, if not set, the text is handled as a single line
UINT GX_TEXT::get_and_resolve_bidi_text_(GX_STRING* return_text, GX_FONT* font, GX_VALUE display_width)
{
    UINT result = GX_SUCCESS;

    result = get_(return_text);

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)

    if (_gx_system_bidi_text_enabled)
    {
        if (bidi_resolved_text_info)
        {
            *return_text = *bidi_resolved_text_info->text;
        }
        else
        {
            GX_BIDI_TEXT_INFO text_info;
            GX_BIDI_RESOLVED_TEXT_INFO* resolved_text_info;

            text_info.text = *return_text;
            text_info.font = font;
            text_info.display_width = display_width;
            text_info.direction = owner->text_direction_get_();

            result = _gx_utility_bidi_paragraph_reorder_ext(&text_info, &resolved_text_info);
            if (result == GX_SUCCESS)
            {
                bidi_resolved_text_info = resolved_text_info;
                *return_text = *resolved_text_info->text;
            }
        }
    }

#endif // GX_DYNAMIC_BIDI_TEXT_SUPPORT

    return result;
}

void GX_TEXT::text_id_set_(GX_RESOURCE_ID text_id)
{
    this->text_id = text_id;

    string_delete_();

    owner->dirty_mark_if_visible_();
}

UINT GX_TEXT::string_set_(const GX_CHAR* str, UINT max_string_length)
{
    string_delete_();

    owner->style &= ~GX_STYLE_TEXT_COPY;

    UINT result = _gx_utility_string_set(&string, str, max_string_length);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    text_id = 0;

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    if (bidi_resolved_text_info)
    {
        _gx_utility_bidi_resolved_text_info_delete(&bidi_resolved_text_info);
    }
#endif

    owner->dirty_mark_if_visible_();

    return result;
}

UINT GX_TEXT::string_ext_set_(const GX_STRING* str)
{
    UINT result = GX_SUCCESS;

    if (owner->style & GX_STYLE_TEXT_COPY)
    {
        result = _gx_system_private_string_copy(&string, str);
    }
    else
    {
        if (str)
        {
            string = *str;
        }
        else
        {
            string.ptr = GX_NULL;
            string.length = 0;
        }
    }

    text_id = 0;

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    if (bidi_resolved_text_info)
    {
        _gx_utility_bidi_resolved_text_info_delete(&bidi_resolved_text_info);
    }
#endif

    owner->dirty_mark_if_visible_();

    return result;
}

void GX_TEXT::string_delete_()
{
    if (owner->style & GX_STYLE_TEXT_COPY)
    {
        if (string.ptr)
        {
            if (_gx_system_memory_free)
            {
                _gx_system_memory_free((void*)string.ptr);
            }
        }
    }

    string.ptr = GX_NULL;
    string.length = 0;

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    bidi_resolved_text_info_delete_();
#endif
}
