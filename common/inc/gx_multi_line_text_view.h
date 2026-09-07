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
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_multi_line_text_view.h                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX text view management component,          */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_MULTI_LINE_TEXT_VIEW_H
#define GX_MULTI_LINE_TEXT_VIEW_H

#include "gx_window.h"
#include "gx_text.h"

/* Define GX_MULTI_LINE_TEXT_INFO type. */
struct GX_MULTI_LINE_TEXT_INFO
{
    USHORT   display_width;     // pixels
    USHORT   display_number;    // characters
};

/* Define Multi Line Text View type.  */
struct GX_MULTI_LINE_TEXT_VIEW : public GX_WINDOW
{
    GX_TEXT        text_data;

    UINT           text_total_lines;
    UINT           text_visible_lines;
    INT            text_scroll_shift;
    GX_UBYTE       margin;
    GX_BYTE        line_space;

    GX_BOOL        line_index_cache_old;
    UINT           first_cache_line;
    GX_UBYTE       line_index_cache_size;
    UINT           line_index_cache[GX_MULTI_LINE_INDEX_CACHE_SIZE];

    GX_VALUE       whitespace_width;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_RESOURCE_ID text_id, ULONG style, USHORT Id, const GX_RECTANGLE* size);

    GX_RESOURCE_ID text_id_() const { return text_data.text_id; }
    void text_id_set_(GX_RESOURCE_ID text_id);

    const GX_STRING& text_() const { return text_data.string; }
    GX_STRING& text_() { return text_data.string; }
    UINT text_set_ext_(const GX_STRING* text);

    GX_RESOURCE_ID font_id_() const { return text_data.font_id; }
    GX_RESOURCE_ID& font_id_() { return text_data.font_id; }
    void font_set_(GX_RESOURCE_ID font_id);

    GX_RESOURCE_ID normal_text_color_()   const { return text_data.normal_color; }
    GX_RESOURCE_ID selected_text_color_() const { return text_data.selected_color; }
    GX_RESOURCE_ID disabled_text_color_() const { return text_data.disabled_color; }

    GX_RESOURCE_ID& normal_text_color_()   { return text_data.normal_color; }
    GX_RESOURCE_ID& selected_text_color_() { return text_data.selected_color; }
    GX_RESOURCE_ID& disabled_text_color_() { return text_data.disabled_color; }

    void text_color_set_(GX_RESOURCE_ID normal_text_color_id,
                         GX_RESOURCE_ID selected_text_color_id,
                         GX_RESOURCE_ID disabled_text_color_id)
    {
        text_data.color_set_(normal_text_color_id,
                             selected_text_color_id,
                             disabled_text_color_id);
    }

    void line_space_set_(GX_BYTE line_space);
    void margin_set_(GX_UBYTE margin);

    UINT scroll_info_get_(ULONG style, GX_SCROLL_INFO* info);

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

protected:
    void display_area_(GX_RECTANGLE* rect) const
    {
        *rect = this->client;
        rect->resize_((GX_VALUE)(-margin));
    }

    void line_display_info_get_(UINT start_index, UINT end_index, GX_VALUE available_width,
                                GX_MULTI_LINE_TEXT_INFO* text_info) const;

    bool visible_lines_compute_();
    bool total_lines_compute_();

    bool line_cache_update_();

    void scrollbar_reset_or_line_cache_update_();

    GX_VALUE cursor_x_pos_get_(const GX_STRING* string, const GX_FONT* font, const GX_RECTANGLE& input_area) const;
    GX_VALUE text_width_get_(const GX_STRING* string, const GX_FONT* font, GX_VALUE available_width) const;
    GX_VALUE x_value_clamp_(GX_VALUE x_value, GX_VALUE high_limit) const;

    void text_draw_();
    void text_draw_(GX_RESOURCE_ID text_color);

private:
    GX_VALUE whitespace_width_() const;

    bool scroll_(GX_VALUE amount_to_scroll);
    void client_updated_() const;

    UINT line_index_find_(UINT start_line, UINT start_index, UINT search_line, GX_VALUE available_width);

    void line_start_cache_create_(UINT first_line, UINT updated_size);
    void line_start_cache_update_(UINT new_first_cache_line);

    void line_tail_cache_create_(UINT start_index, UINT start_line, UINT first_line, UINT updated_size);
    void line_tail_cache_update_(UINT new_first_cache_line);
};


/* Define multi-line text view management function prototypes. */

UINT _gx_multi_line_text_view_create(GX_MULTI_LINE_TEXT_VIEW* view, const GX_CHAR* name_ptr, GX_WIDGET* parent,
                                     GX_RESOURCE_ID text_id, ULONG style, USHORT Id, const GX_RECTANGLE* size);
void _gx_multi_line_text_view_draw(GX_MULTI_LINE_TEXT_VIEW* view);
UINT _gx_multi_line_text_view_event_process(GX_MULTI_LINE_TEXT_VIEW* view, GX_EVENT* event_ptr);
UINT _gx_multi_line_text_view_font_set(GX_MULTI_LINE_TEXT_VIEW* view, GX_RESOURCE_ID font_id);
UINT _gx_multi_line_text_view_line_space_set(GX_MULTI_LINE_TEXT_VIEW* view, GX_BYTE line_space);
UINT _gx_multi_line_text_view_scroll_info_get(GX_MULTI_LINE_TEXT_VIEW* view, ULONG style, GX_SCROLL_INFO* info);
UINT _gx_multi_line_text_view_text_color_set(GX_MULTI_LINE_TEXT_VIEW* view,
                                             GX_RESOURCE_ID normal_text_color_id,
                                             GX_RESOURCE_ID selected_text_color_id,
                                             GX_RESOURCE_ID disabled_text_color_id);
UINT _gx_multi_line_text_view_text_id_set(GX_MULTI_LINE_TEXT_VIEW* view, GX_RESOURCE_ID text_id);
UINT _gx_multi_line_text_view_text_set_ext(GX_MULTI_LINE_TEXT_VIEW* view, const GX_STRING* text);
UINT _gx_multi_line_text_view_whitespace_set(GX_MULTI_LINE_TEXT_VIEW* view, GX_UBYTE whitespace);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_multi_line_text_view_create(GX_MULTI_LINE_TEXT_VIEW* view, const GX_CHAR* name_ptr, GX_WIDGET* parent,
                                      GX_RESOURCE_ID text_id, ULONG style, USHORT Id, const GX_RECTANGLE* size,
                                      UINT text_view_control_block_size);
UINT _gxe_multi_line_text_view_event_process(GX_MULTI_LINE_TEXT_VIEW* view, GX_EVENT* event_ptr);
UINT _gxe_multi_line_text_view_font_set(GX_MULTI_LINE_TEXT_VIEW* view, GX_RESOURCE_ID font_id);
UINT _gxe_multi_line_text_view_line_space_set(GX_MULTI_LINE_TEXT_VIEW* view, GX_BYTE line_space);
UINT _gxe_multi_line_text_view_scroll_info_get(GX_MULTI_LINE_TEXT_VIEW* view, ULONG style, GX_SCROLL_INFO* info);
UINT _gxe_multi_line_text_view_text_color_set(GX_MULTI_LINE_TEXT_VIEW* view,
                                              GX_RESOURCE_ID normal_text_color_id,
                                              GX_RESOURCE_ID selected_text_color_id,
                                              GX_RESOURCE_ID disabled_text_color_id);
UINT _gxe_multi_line_text_view_text_id_set(GX_MULTI_LINE_TEXT_VIEW* view, GX_RESOURCE_ID text_id);
UINT _gxe_multi_line_text_view_text_set_ext(GX_MULTI_LINE_TEXT_VIEW* view, const GX_STRING* text);
UINT _gxe_multi_line_text_view_whitespace_set(GX_MULTI_LINE_TEXT_VIEW* view, GX_UBYTE whitespace);

#endif
