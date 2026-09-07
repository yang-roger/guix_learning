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
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_single_line_text_input.h                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX text input management component,         */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_SINGLE_LINE_TEXT_INPUT_H
#define GX_SINGLE_LINE_TEXT_INPUT_H

#include "gx_prompt.h"
#include "gx_text_input_cursor.h"

/* Define Single Line Text Input type.  */
struct GX_SINGLE_LINE_TEXT_INPUT : public GX_PROMPT
{
    GX_CHAR*             buffer;
    UINT                 buffer_size;
    UINT                 string_size;
    GX_VALUE             xoffset;
    GX_VALUE             yoffset;
    UINT                 insert_pos;
    UINT                 was_modified;
    GX_TEXT_INPUT_CURSOR cursor;
    GX_RESOURCE_ID       readonly_text_color;
    GX_RESOURCE_ID       readonly_fill_color;
    UINT                 start_mark;
    UINT                 end_mark;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_CHAR* input_buffer, UINT buffer_size, UINT style, USHORT Id,
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

    void text_set_ext_(const GX_STRING* text);

    void text_select_(UINT start_index, UINT end_index);

    // pixel_position -> insert_pos and cursor.pos.x
    void position_get_(INT pixel_position);

    UINT text_insert_(const GX_UBYTE* str, UINT str_size);
    UINT text_delete_();
    UINT backspace_();

    void home_();
    void end_();

    void left_arrow_();
    void right_arrow_();

    bool draw_position_get_(GX_VALUE* xpos, GX_VALUE* ypos);

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    GX_VALUE first_char_x_pos_get_(const GX_RECTANGLE& client) const;

    // return new insert_pos
    UINT new_relative_pos_get_(GX_VALUE distance, GX_VALUE* relative_cursor_pos) const;

    // pixel_position -> insert_pos and cursor_pos
    // return new insert_pos
    UINT new_pos_get_(const GX_RECTANGLE& client, INT pixel_position, GX_VALUE* new_cursor_pos) const;

    // cursor_pos -> cursor.pos.x, xoffset
    void cursor_pos_set_(const GX_RECTANGLE& client, GX_VALUE cursor_pos);

    // insert_pos, xoffset -> cursor.pos.x
    void cursor_pos_update_();

    void mark_home_();
    void mark_end_();

    void mark_previous_();
    void mark_next_();

    void copy_() const;
    void cut_();
    void paste_();

    GX_VALUE text_width_get_(const GX_CHAR* string, UINT length) const;

    bool text_rectangle_get_(INT mark_diff, GX_RECTANGLE* rect) const;

    void selected_text_delete_();

    void selected_area_dirty_mark_(UINT start_mark_, UINT end_mark_);

    void cursor_area_dirty_mark_();
    void cursor_area_dirty_mark_(GX_RECTANGLE* cursor_rect);

    void selected_area_and_cursor_area_dirty_mark_(UINT start_mark_, UINT end_mark_);

    void cursor_change_for_home_end_(GX_VALUE new_xoffset, GX_VALUE new_cursor_pos,
                                     UINT old_start_mark, UINT old_end_mark);

    void cursor_change_for_prev_next_(GX_VALUE new_cursor_pos,
                                      UINT old_start_mark, UINT old_end_mark,
                                      const GX_RECTANGLE& client);

    void cursor_change_for_mark_home_end_(GX_VALUE new_xoffset, GX_VALUE new_cursor_pos,
                                          UINT old_start_mark, UINT old_end_mark, UINT new_end_mark);

    void cursor_change_for_mark_prev_next_(GX_VALUE new_cursor_pos,
                                           UINT old_start_mark, UINT old_end_mark);

    bool cursor_adjust_(GX_VALUE new_cursor_pos, const GX_RECTANGLE& client);

    void text_draw_(GX_RESOURCE_ID text_color);

    void key_down_process_(GX_EVENT* event_ptr);

    void pen_down_process_(GX_EVENT* event_ptr);
    void pen_drag_process_(GX_EVENT* event_ptr);
    void pen_up_process_(GX_EVENT* event_ptr);
};

inline void GX_SINGLE_LINE_TEXT_INPUT::cursor_area_dirty_mark_()
{
    GX_RECTANGLE cursor_rect;
    cursor_area_dirty_mark_(&cursor_rect);
}

inline void GX_SINGLE_LINE_TEXT_INPUT::cursor_area_dirty_mark_(GX_RECTANGLE* cursor_rect)
{
    cursor.dirty_rectangle_get_(cursor_rect);
    dirty_mark_(cursor_rect);
}


/* Define text input management function prototypes.  */

UINT _gx_single_line_text_input_backspace(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gx_single_line_text_input_buffer_clear(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gx_single_line_text_input_buffer_get(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_CHAR** buffer_address,
                                           UINT* content_size, UINT* buffer_size);
UINT _gx_single_line_text_input_create(GX_SINGLE_LINE_TEXT_INPUT* text_input, const GX_CHAR* name, GX_WIDGET* parent,
                                       GX_CHAR* input_buffer, UINT buffer_size, UINT style, USHORT Id,
                                       const GX_RECTANGLE* size);
UINT _gx_single_line_text_input_character_delete(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gx_single_line_text_input_character_insert(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_UBYTE* str, UINT str_size);
void _gx_single_line_text_input_draw(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gx_single_line_text_input_draw_position_get(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_VALUE* xpos, GX_VALUE* ypos);
UINT _gx_single_line_text_input_end(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gx_single_line_text_input_event_process(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_EVENT* event_ptr);
UINT _gx_single_line_text_input_fill_color_set(GX_SINGLE_LINE_TEXT_INPUT* text_input,
                                               GX_RESOURCE_ID normal_fill_color_id,
                                               GX_RESOURCE_ID selected_fill_color_id,
                                               GX_RESOURCE_ID disabled_fill_color_id,
                                               GX_RESOURCE_ID readonly_fill_color_id);
UINT _gx_single_line_text_input_home(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gx_single_line_text_input_left_arrow(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gx_single_line_text_input_position_get(GX_SINGLE_LINE_TEXT_INPUT* text_input, INT pixel_position);
UINT _gx_single_line_text_input_right_arrow(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gx_single_line_text_input_style_add(GX_SINGLE_LINE_TEXT_INPUT* text_input, ULONG style);
UINT _gx_single_line_text_input_style_remove(GX_SINGLE_LINE_TEXT_INPUT* text_input, ULONG style);
UINT _gx_single_line_text_input_style_set(GX_SINGLE_LINE_TEXT_INPUT* text_input, ULONG style);
UINT _gx_single_line_text_input_text_color_set(GX_SINGLE_LINE_TEXT_INPUT* text_input,
                                               GX_RESOURCE_ID normal_text_color_id,
                                               GX_RESOURCE_ID selected_text_color_id,
                                               GX_RESOURCE_ID disabled_text_color_id,
                                               GX_RESOURCE_ID readonly_text_color_id);
UINT _gx_single_line_text_input_text_select(GX_SINGLE_LINE_TEXT_INPUT* text_input, UINT start_index, UINT end_index);
UINT _gx_single_line_text_input_text_set_ext(GX_SINGLE_LINE_TEXT_INPUT* text_input, const GX_STRING* text);


/* error checking versions of the animation API functions */

UINT _gxe_single_line_text_input_backspace(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gxe_single_line_text_input_buffer_clear(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gxe_single_line_text_input_buffer_get(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_CHAR** buffer_address,
                                            UINT* content_size, UINT* buffer_size);
UINT _gxe_single_line_text_input_create(GX_SINGLE_LINE_TEXT_INPUT* text_input, const GX_CHAR* name, GX_WIDGET* parent,
                                        GX_CHAR* input_buffer, UINT buffer_size, UINT style, USHORT Id,
                                        const GX_RECTANGLE* size, UINT text_input_control_block_size);
UINT _gxe_single_line_text_input_character_delete(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gxe_single_line_text_input_character_insert(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_UBYTE* str, UINT str_size);

UINT _gxe_single_line_text_input_draw_position_get(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_VALUE* xpos, GX_VALUE* ypos);
UINT _gxe_single_line_text_input_event_process(GX_SINGLE_LINE_TEXT_INPUT* text_input, GX_EVENT* event_ptr);
UINT _gxe_single_line_text_input_end(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gxe_single_line_text_input_home(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gxe_single_line_text_input_left_arrow(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gxe_single_line_text_input_position_get(GX_SINGLE_LINE_TEXT_INPUT* text_input, INT pixel_position);
UINT _gxe_single_line_text_input_right_arrow(GX_SINGLE_LINE_TEXT_INPUT* text_input);
UINT _gxe_single_line_text_input_style_add(GX_SINGLE_LINE_TEXT_INPUT* text_input, ULONG style);
UINT _gxe_single_line_text_input_style_remove(GX_SINGLE_LINE_TEXT_INPUT* text_input, ULONG style);
UINT _gxe_single_line_text_input_style_set(GX_SINGLE_LINE_TEXT_INPUT* text_input, ULONG style);
UINT _gxe_single_line_text_input_fill_color_set(GX_SINGLE_LINE_TEXT_INPUT* text_input,
                                                GX_RESOURCE_ID normal_fill_color_id,
                                                GX_RESOURCE_ID selected_fill_color_id,
                                                GX_RESOURCE_ID disabled_fill_color_id,
                                                GX_RESOURCE_ID readonly_fill_color_id);
UINT _gxe_single_line_text_input_text_color_set(GX_SINGLE_LINE_TEXT_INPUT* text_input,
                                                GX_RESOURCE_ID normal_text_color_id,
                                                GX_RESOURCE_ID selected_text_color_id,
                                                GX_RESOURCE_ID disabled_text_color_id,
                                                GX_RESOURCE_ID readonly_text_color_id);
UINT _gxe_single_line_text_input_text_select(GX_SINGLE_LINE_TEXT_INPUT* text_input, UINT start_index, UINT end_index);
UINT _gxe_single_line_text_input_text_set_ext(GX_SINGLE_LINE_TEXT_INPUT* text_input, const GX_STRING* text);

#endif

