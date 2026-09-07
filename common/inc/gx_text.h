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

#ifndef GX_TEXT_H
#define GX_TEXT_H

#include "gx_global.h"
#include "gx_widget.h"

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
#include "gx_utility_bidi.h"
#endif

struct GX_FONT;

struct GX_TEXT
{
    GX_WIDGET*     owner;

    GX_STRING      string;
    GX_RESOURCE_ID text_id;

    GX_RESOURCE_ID font_id;

    GX_RESOURCE_ID normal_color;
    GX_RESOURCE_ID selected_color;
    GX_RESOURCE_ID disabled_color;

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    GX_BIDI_RESOLVED_TEXT_INFO* bidi_resolved_text_info;
#endif

    void create_(GX_WIDGET* owner, GX_RESOURCE_ID text_id, GX_RESOURCE_ID font_id);

    UINT get_(GX_STRING* return_text) const;
    UINT get_and_resolve_bidi_text_(GX_STRING* return_text, GX_FONT* font = GX_NULL, GX_VALUE display_width = -1);

    void text_id_set_(GX_RESOURCE_ID text_id);

    UINT string_set_(const GX_CHAR* string, UINT max_string_length);
    UINT string_ext_set_(const GX_STRING* string);

    void string_delete_();

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    void bidi_resolved_text_info_delete_();
#endif

    void font_set_(GX_RESOURCE_ID font_id);

    void color_set_(GX_RESOURCE_ID normal_color_id,
                    GX_RESOURCE_ID selected_color_id,
                    GX_RESOURCE_ID disabled_color_id);

    GX_RESOURCE_ID enabled_color_() const;
    GX_RESOURCE_ID color_() const;
};

inline void GX_TEXT::font_set_(GX_RESOURCE_ID font_id)
{
    this->font_id = font_id;

    owner->dirty_mark_if_visible_();
}

inline void GX_TEXT::color_set_(GX_RESOURCE_ID normal_color_id,
                                GX_RESOURCE_ID selected_color_id,
                                GX_RESOURCE_ID disabled_color_id)
{
    this->normal_color = normal_color_id;
    this->selected_color = selected_color_id;
    this->disabled_color = disabled_color_id;

    owner->dirty_mark_if_visible_();
}

inline GX_RESOURCE_ID GX_TEXT::enabled_color_() const
{
    return (owner->style & GX_STYLE_DRAW_SELECTED) ? selected_color : normal_color;
}

inline GX_RESOURCE_ID GX_TEXT::color_() const
{
    return (owner->style & GX_STYLE_ENABLED) ? enabled_color_() : disabled_color;
}

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)

inline void GX_TEXT::bidi_resolved_text_info_delete_()
{
    if (bidi_resolved_text_info)
    {
        _gx_utility_bidi_resolved_text_info_delete(&bidi_resolved_text_info);
    }
}

#endif // GX_DYNAMIC_BIDI_TEXT_SUPPORT

#endif
