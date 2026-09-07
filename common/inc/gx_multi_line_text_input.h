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
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_multi_line_text_input.h                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX multi line text input management         */
/*    component, including all data types and external references.  It is */
/*    assumed that gx_api.h and gx_port.h have already been included.     */
/*                                                                        */
/**************************************************************************/
#ifndef GX_MULTI_LINE_TEXT_INPUT_H
#define GX_MULTI_LINE_TEXT_INPUT_H

#include "gx_multi_line_text_view.h"
#include "gx_text_input_cursor.h"

/* Define GX_MULTI_TEXT_INPUT type. */
struct GX_MULTI_LINE_TEXT_INPUT : public GX_MULTI_LINE_TEXT_VIEW
{
    UINT                 buffer_size;
    UINT                 text_insert_position;
    UINT                 text_was_modified;
    UINT                 text_cursor_line; // start from 1
    GX_TEXT_INPUT_CURSOR cursor;
    GX_RESOURCE_ID       readonly_text_color;
    GX_RESOURCE_ID       readonly_fill_color;
    UINT                 start_mark;
    UINT                 end_mark;
    GX_UBYTE             new_line_character[3];
    GX_UBYTE             new_line_character_size;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_CHAR* input_buffer, UINT buffer_size, ULONG style, USHORT Id,
                 const GX_RECTANGLE* size);

    void style_add_(ULONG style);
    void style_remove_(ULONG style);
    void style_set_(ULONG style);

    void fill_color_set_(GX_RESOURCE_ID normal_fill_color_id,
                         GX_RESOURCE_ID selected_fill_color_id,
                         GX_RESOURCE_ID disabled_fill_color_id,
                         GX_RESOURCE_ID readonly_fill_color_id);

    void text_color_set_(GX_RESOURCE_ID normal_text_color_id,
                         GX_RESOURCE_ID selected_text_color_id,
                         GX_RESOURCE_ID disabled_text_color_id,
                         GX_RESOURCE_ID readonly_text_color_id);

    void buffer_get_(GX_CHAR** buffer_address, UINT* content_size, UINT* buffer_size);
    void buffer_clear_();

    UINT text_set_ext_(const GX_STRING* text);

    void text_select_(UINT start_index, UINT end_index);

    UINT text_insert_(const GX_UBYTE* str, UINT str_size);
    UINT text_delete_();
    UINT backspace_();

    UINT up_arrow_();
    UINT down_arrow_();

    UINT left_arrow_();
    UINT right_arrow_();

    UINT home_();
    UINT end_();

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void cursor_blink_timer_start_();

    UINT cursor_pos_calculate_(GX_POINT click_pos);
    UINT cursor_pos_update_(GX_BOOL make_cursor_visible);
    UINT cursor_line_search_(UINT start_cursor_line, UINT& start_index, UINT& end_index, GX_VALUE available_width) const;
    UINT cursor_visible_();

    void line_start_and_end_index_get_(UINT cache_index, UINT* start_index, UINT* end_index);
    UINT line_index_cache_read_(UINT cache_index) const;

    INT scroll_shift_get_(GX_VALUE y_pos, GX_VALUE line_height, const GX_RECTANGLE& input_area) const;
    GX_VALUE cursor_x_pos_get_(UINT start_index, UINT end_index, const GX_FONT* font, const GX_RECTANGLE& input_area) const;

    bool insert_pos_previous_char_is_new_line_() const;

    bool can_add_str_size_(UINT str_size) const
    {
        return text_().length + str_size + 1 <= buffer_size;
    }

    void text_delete_(UINT start_index, UINT del_bytes);
    void selected_text_delete_();
    INT total_lines_update_();
    void dirty_rect_mark_(INT old_scroll_shift, GX_VALUE cursor_pos_y, INT decreased_line_count);

    UINT mark_up_();
    UINT mark_down_();

    UINT mark_previous_();
    UINT mark_next_();

    UINT mark_home_();
    UINT mark_end_();

    UINT copy_();
    UINT cut_();
    UINT paste_();

    void key_down_process_(GX_EVENT* event_ptr);

    void pen_down_process_(GX_EVENT* event_ptr);
    void pen_drag_process_(GX_EVENT* event_ptr);
    void pen_up_process_(GX_EVENT* event_ptr);

    UINT highlight_rectangle_get_(GX_RECTANGLE* rect);

    UINT text_rectangle_get_(GX_POINT start_cursor_pos, GX_POINT end_cursor_pos, GX_RECTANGLE* rect);

    void new_line_character_get_();
};


/* Define multi-line text input management function prototypes. */

UINT _gx_multi_line_text_input_backspace(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gx_multi_line_text_input_buffer_clear(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gx_multi_line_text_input_buffer_get(GX_MULTI_LINE_TEXT_INPUT* text_input, GX_CHAR** buffer_address,
                                          UINT* content_size, UINT* buffer_size);
UINT _gx_multi_line_text_input_char_insert_ext(GX_MULTI_LINE_TEXT_INPUT* text_input, const GX_STRING* str);
UINT _gx_multi_line_text_input_create(GX_MULTI_LINE_TEXT_INPUT* text_input,
                                      const GX_CHAR* name, GX_WIDGET* parent,
                                      GX_CHAR* input_buffer, UINT buffer_size,
                                      ULONG style, USHORT Id, const GX_RECTANGLE* size);
UINT _gx_multi_line_text_input_cursor_pos_get(GX_MULTI_LINE_TEXT_INPUT* text_input, GX_POINT* cursor_pos);
UINT _gx_multi_line_text_input_delete(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gx_multi_line_text_input_event_process(GX_MULTI_LINE_TEXT_INPUT* text_input, GX_EVENT* event_ptr);
UINT _gx_multi_line_text_input_down_arrow(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gx_multi_line_text_input_end(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gx_multi_line_text_input_fill_color_set(GX_MULTI_LINE_TEXT_INPUT* text_input,
                                              GX_RESOURCE_ID normal_fill_color_id,
                                              GX_RESOURCE_ID selected_fill_color_id,
                                              GX_RESOURCE_ID disabled_fill_color_id,
                                              GX_RESOURCE_ID readonly_fill_color_id);
UINT _gx_multi_line_text_input_home(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gx_multi_line_text_input_left_arrow(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gx_multi_line_text_input_right_arrow(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gx_multi_line_text_input_style_add(GX_MULTI_LINE_TEXT_INPUT* text_input, ULONG style);
UINT _gx_multi_line_text_input_style_remove(GX_MULTI_LINE_TEXT_INPUT* text_input, ULONG style);
UINT _gx_multi_line_text_input_style_set(GX_MULTI_LINE_TEXT_INPUT* text_input, ULONG style);
UINT _gx_multi_line_text_input_text_color_set(GX_MULTI_LINE_TEXT_INPUT* text_input,
                                              GX_RESOURCE_ID normal_text_color_id,
                                              GX_RESOURCE_ID selected_text_color_id,
                                              GX_RESOURCE_ID disabled_text_color_id,
                                              GX_RESOURCE_ID readonly_text_color_id);
UINT _gx_multi_line_text_input_text_select(GX_MULTI_LINE_TEXT_INPUT* text_input, UINT start_index, UINT end_index);
UINT _gx_multi_line_text_input_text_set_ext(GX_MULTI_LINE_TEXT_INPUT* text_input, const GX_STRING* text);
UINT _gx_multi_line_text_input_up_arrow(GX_MULTI_LINE_TEXT_INPUT* text_input);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_multi_line_text_input_backspace(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gxe_multi_line_text_input_buffer_clear(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gxe_multi_line_text_input_buffer_get(GX_MULTI_LINE_TEXT_INPUT* text_input, GX_CHAR** buffer_address,
                                           UINT* content_size, UINT* buffer_size);
UINT _gxe_multi_line_text_input_char_insert_ext(GX_MULTI_LINE_TEXT_INPUT* text_input, const GX_STRING* str);
UINT _gxe_multi_line_text_input_create(GX_MULTI_LINE_TEXT_INPUT* text_input,
                                       const GX_CHAR* name, GX_WIDGET* parent,
                                       GX_CHAR* input_buffer, UINT buffer_size,
                                       ULONG style, USHORT Id, const GX_RECTANGLE* size, UINT text_input_control_block_size);
UINT _gxe_multi_line_text_input_cursor_pos_get(GX_MULTI_LINE_TEXT_INPUT* text_input, GX_POINT* cursor_pos);
UINT _gxe_multi_line_text_input_delete(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gxe_multi_line_text_input_down_arrow(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gxe_multi_line_text_input_end(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gxe_multi_line_text_input_event_process(GX_MULTI_LINE_TEXT_INPUT* text_input, GX_EVENT* event_ptr);
UINT _gxe_multi_line_text_input_fill_color_set(GX_MULTI_LINE_TEXT_INPUT* text_input,
                                               GX_RESOURCE_ID normal_fill_color_id,
                                               GX_RESOURCE_ID selected_fill_color_id,
                                               GX_RESOURCE_ID disabled_fill_color_id,
                                               GX_RESOURCE_ID readonly_fill_color_id);
UINT _gxe_multi_line_text_input_home(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gxe_multi_line_text_input_left_arrow(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gxe_multi_line_text_input_right_arrow(GX_MULTI_LINE_TEXT_INPUT* text_input);
UINT _gxe_multi_line_text_input_style_add(GX_MULTI_LINE_TEXT_INPUT* text_input, ULONG style);
UINT _gxe_multi_line_text_input_style_remove(GX_MULTI_LINE_TEXT_INPUT* text_input, ULONG style);
UINT _gxe_multi_line_text_input_style_set(GX_MULTI_LINE_TEXT_INPUT* text_input, ULONG style);
UINT _gxe_multi_line_text_input_text_color_set(GX_MULTI_LINE_TEXT_INPUT* text_input,
                                               GX_RESOURCE_ID normal_text_color_id,
                                               GX_RESOURCE_ID selected_text_color_id,
                                               GX_RESOURCE_ID disabled_text_color_id,
                                               GX_RESOURCE_ID readonly_text_color_id);
UINT _gxe_multi_line_text_input_text_select(GX_MULTI_LINE_TEXT_INPUT* text_input, UINT start_index, UINT end_index);
UINT _gxe_multi_line_text_input_text_set_ext(GX_MULTI_LINE_TEXT_INPUT* text_input, const GX_STRING* text);
UINT _gxe_multi_line_text_input_up_arrow(GX_MULTI_LINE_TEXT_INPUT* text_input);

#endif
