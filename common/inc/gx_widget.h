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
/**   Widget Management (Widget)                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_widget.h                                                         */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX widget management component,             */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_WIDGET_H
#define GX_WIDGET_H

#include "gx_rectangle.h"
#include "gx_event.h"
#include "gx_system_timer.h"
#include "gx_system_dirty.h"

struct GX_FONT;
struct GX_PIXELMAP;
struct GX_CANVAS;
struct GX_WINDOW_ROOT;

/* Define Widget type.  */
struct GX_WIDGET
{
    const GX_CHAR*           name;
    GX_WIDGET*               parent;
    GX_WIDGET*               first_child;   // backmost
    GX_WIDGET*               last_child;    // frontmost
    GX_WIDGET*               next;
    GX_WIDGET*               previous;
    GX_WIDGET*               nav_next;
    GX_WIDGET*               nav_previous;
    GX_RECTANGLE             size;
    GX_RECTANGLE             clip;
    UINT                   (*event_process_function)(GX_WIDGET*, GX_EVENT*);
    void                   (*draw_function)(GX_WIDGET*);
    ULONG                    status;
    ULONG                    style;
    GX_RESOURCE_ID           normal_fill_color;
    GX_RESOURCE_ID           selected_fill_color;
    GX_RESOURCE_ID           disabled_fill_color;
    USHORT                   type;
    USHORT                   id;
#if defined(GX_WIDGET_USER_DATA)
    INT                      user_data;
#endif
#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE                 alpha;
#endif

public:
    UINT event_process_function_run_(GX_EVENT* event_ptr)
    {
        return event_process_function(this, event_ptr);
    }

    void draw_function_run_()
    {
        draw_function(this);
    }

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 ULONG style, USHORT Id, const GX_RECTANGLE* size);
    void delete_();

    bool is_created_() const { return type != 0; }

    void attach_to_(GX_WIDGET* parent);
    void back_attach_to_(GX_WIDGET* parent);
    void detach_();

    UINT move_to_front_();
    UINT move_to_back_();

    void status_add_(ULONG status) { this->status |= status; }
    void status_remove_(ULONG status) { this->status &= ~status; }

    void style_add_(ULONG style);
    void style_remove_(ULONG style);
    void style_set_(ULONG style);

    void border_style_set_(ULONG style);
    void fill_color_set_(GX_RESOURCE_ID normal_color, GX_RESOURCE_ID selected_color, GX_RESOURCE_ID disabled_color);

    void show_();
    void hide_();

    bool is_enabled_() const { return (style & GX_STYLE_ENABLED) != 0; }

    bool is_visible_() const { return (status & GX_STATUS_VISIBLE) != 0; }
    bool is_client_() const { return (status & GX_STATUS_NONCLIENT) == 0; }

    bool is_window_() const { return type >= GX_TYPE_WINDOW; }
    bool is_root_window_() const { return type == GX_TYPE_ROOT_WINDOW; }

    bool is_ancestor_of_(const GX_WIDGET* child) const;

    bool is_child_of_(const GX_WIDGET* parent) const
    {
        return parent->is_ancestor_of_(this);
    }

    GX_VALUE left_() const { return size.left; }
    GX_VALUE top_() const { return size.top; }
    GX_VALUE right_() const { return size.right; }
    GX_VALUE bottom_() const { return size.bottom; }

    GX_VALUE x_center_() const { return size.x_center_(); }
    GX_VALUE y_center_() const { return size.y_center_(); }

    GX_VALUE width_() const { return size.width_(); }
    GX_VALUE height_() const { return size.height_(); }

    GX_VALUE border_width_() const;
    void client_area_get_(GX_RECTANGLE* area, GX_VALUE border_width = -1) const;

    void resize_(const GX_RECTANGLE* new_size);
    void shift_(GX_VALUE x_shift, GX_VALUE y_shift, GX_BOOL mark_dirty);
    void scroll_shift_(INT x_shift, INT y_shift, GX_BOOL clip);
    UINT block_move_(GX_RECTANGLE* block, INT x_shift, INT y_shift);

    UINT timer_start_(UINT timer_id, UINT initial_ticks, UINT reschedule_ticks);
    UINT timer_stop_(UINT timer_id);

    GX_WINDOW_ROOT* root_window_() const;

    GX_WIDGET* child_find_by_id_(USHORT widget_id, INT search_depth) const;
    GX_WIDGET* child_find_by_type_(USHORT type) const; // direct child
    GX_WIDGET* top_visible_child_find_() const; // direct child

    GX_WIDGET* top_child_in_point_(GX_POINT test_point, ULONG status_flag) const;

    GX_WIDGET* first_client_child_get_() const;
    GX_WIDGET* next_client_child_get_() const;
    GX_WIDGET* last_client_child_get_() const;

    GX_WIDGET* first_visible_client_child_get_() const;
    GX_WIDGET* next_visible_client_child_get_() const;
    GX_WIDGET* last_visible_client_child_get_() const;

    INT client_child_index_get_(GX_WIDGET* client_child) const;

    void child_clipping_update_();

    UINT focus_next_();
    UINT focus_previous_();

    GX_CANVAS* canvas_get_() const;

    UINT color_get_(GX_RESOURCE_ID color_id, GX_COLOR* return_color) const;
    UINT font_get_(GX_RESOURCE_ID font_id, GX_FONT** return_font) const;
    UINT pixelmap_get_(GX_RESOURCE_ID pixelmap_id, GX_PIXELMAP** return_map) const;
    UINT string_get_ext_(GX_RESOURCE_ID string_id, GX_STRING* return_string) const;

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    GX_UBYTE text_direction_get_() const;
#endif

    bool pixelmap_is_transparent_(GX_RESOURCE_ID pixelmap_id) const;

    UINT dirty_mark_(const GX_RECTANGLE* area);
    UINT dirty_mark_();
    void dirty_mark_if_visible_();

    void text_id_draw_(UINT color_id, UINT font_id, UINT text_id, INT x_offset, INT y_offset);
    void text_draw_ext_(UINT color_id, UINT font_id, const GX_STRING* string, INT x_offset, INT y_offset);
    void text_blend_ext_(UINT color_id, UINT font_id, const GX_STRING* string, INT x_offset, INT y_offset, UCHAR alpha);

    // send event GX_SIGNAL(id, event_type) to parent
    bool signal_parent_(USHORT event_type, LONG value) const;

    UINT parent_event_process_run_(GX_EVENT* event_ptr)
    {
        return parent ? parent->event_process_function_run_(event_ptr) : GX_SUCCESS;
    }

    void others_input_release_() const;

    void children_draw_();

    void border_and_background_draw_();
    void border_and_background_draw_(GX_RESOURCE_ID border_color,
                                     GX_RESOURCE_ID upper_color,
                                     GX_RESOURCE_ID lower_color,
                                     GX_BOOL fill);

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

protected:
    void link_to_(GX_WIDGET* parent);
    void back_link_to_(GX_WIDGET* parent);
    GX_WIDGET* unlink_();

    void nav_order_initialize_();
    void child_focus_assign_();

    GX_RESOURCE_ID enabled_fill_color_() const;
    GX_RESOURCE_ID fill_color_() const;

    GX_VALUE text_x_pos_(const GX_STRING* string, const GX_FONT* font) const;
    void pixelmap_draw_(const GX_PIXELMAP* pixelmap, GX_VALUE border_width = 0, INT shift = 0);

private:
    void delete_helper_();

    void link_as_first_child_of_(GX_WIDGET* parent);
    void link_as_last_child_of_(GX_WIDGET* parent);
    void link_as_only_child_of_(GX_WIDGET* parent);

    GX_WIDGET* client_child_get_from_(GX_WIDGET* start) const;
    GX_WIDGET* visible_client_child_get_from_(GX_WIDGET* start) const;

    void style_changed_(ULONG old_style) const;

    void shift_helper_(GX_VALUE x_shift, GX_VALUE y_shift, GX_BOOL mark_dirty);
    void scroll_shift_helper_(INT x_shift, INT y_shift);

    void resized_notify_() const;

    void clipping_update_();
    void clipping_update_helper_();

    GX_WIDGET* next_nav_find_() const;
    GX_WIDGET* default_focus_child_find_() const;
    GX_WIDGET* accepts_focus_child_find_() const;

    void draw_in_area_(const GX_RECTANGLE& dirty_area, GX_CANVAS* canvas, GX_BOOL flush);

    void children_event_process_run_(GX_EVENT* event_ptr);
    void children_show_event_process_run_(GX_EVENT* event_ptr);

    void background_draw_(GX_RESOURCE_ID upper_color,
                          GX_RESOURCE_ID lower_color,
                          GX_BOOL fill);
    void border_draw_(GX_RESOURCE_ID border_color);

    friend struct GX_DROP_LIST;
    friend struct GX_MENU;
    friend struct GX_ACCORDION_MENU;
};

inline UINT GX_WIDGET::timer_start_(UINT timer_id, UINT initial_ticks, UINT reschedule_ticks)
{
    return _gx_system_timer_start(this, timer_id, initial_ticks, reschedule_ticks);
}

inline UINT GX_WIDGET::timer_stop_(UINT timer_id)
{
    return _gx_system_timer_stop(this, timer_id);
}

inline GX_WIDGET* GX_WIDGET::first_client_child_get_() const
{
    return client_child_get_from_(first_child);
}

inline GX_WIDGET* GX_WIDGET::next_client_child_get_() const
{
    return client_child_get_from_(next);
}

inline GX_WIDGET* GX_WIDGET::first_visible_client_child_get_() const
{
    return visible_client_child_get_from_(first_child);
}

inline GX_WIDGET* GX_WIDGET::next_visible_client_child_get_() const
{
    return visible_client_child_get_from_(next);
}

inline GX_RESOURCE_ID GX_WIDGET::enabled_fill_color_() const
{
    return (style & GX_STYLE_DRAW_SELECTED) ? selected_fill_color : normal_fill_color;
}

inline GX_RESOURCE_ID GX_WIDGET::fill_color_() const
{
    return (style & GX_STYLE_ENABLED) ? enabled_fill_color_() : disabled_fill_color;
}

inline UINT GX_WIDGET::dirty_mark_(const GX_RECTANGLE* area)
{
    return _gx_system_dirty_partial_add(this, area);
}

inline UINT GX_WIDGET::dirty_mark_()
{
    return _gx_system_dirty_partial_add(this, &size);
}

inline void GX_WIDGET::dirty_mark_if_visible_()
{
    if (is_visible_())
    {
        dirty_mark_();
    }
}


/* Define widget management function prototypes.  */

UINT _gx_widget_allocate(GX_WIDGET** widget, ULONG memsize);
UINT _gx_widget_attach(GX_WIDGET* parent, GX_WIDGET* child);
UINT _gx_widget_back_attach(GX_WIDGET* parent, GX_WIDGET* child);
UINT _gx_widget_back_move(GX_WIDGET* widget, GX_BOOL* widget_moved);
void _gx_widget_background_draw(GX_WIDGET* widget);
UINT _gx_widget_block_move(GX_WIDGET* widget, GX_RECTANGLE* block, INT x_shift, INT y_shift);
void _gx_widget_border_draw(GX_WIDGET* widget, GX_RESOURCE_ID border_color, GX_RESOURCE_ID upper_color, GX_RESOURCE_ID lower_color, GX_BOOL fill);
UINT _gx_widget_border_style_set(GX_WIDGET* widget, ULONG style);
UINT _gx_widget_border_width_get(GX_WIDGET* widget, GX_VALUE* return_width);
UINT _gx_widget_canvas_get(GX_WIDGET* widget, GX_CANVAS** return_canvas);
UINT _gx_widget_child_detect(GX_WIDGET* parent, GX_WIDGET* child, GX_BOOL* return_detect);
void _gx_widget_children_draw(GX_WIDGET* widget);
UINT _gx_widget_client_get(GX_WIDGET* widget, GX_VALUE border_width, GX_RECTANGLE* return_size);
UINT _gx_widget_color_get(GX_WIDGET* widget, GX_RESOURCE_ID color_id, GX_COLOR* color);
UINT _gx_widget_create(GX_WIDGET* widget, const GX_CHAR* name, GX_WIDGET* parent, ULONG style, USHORT Id, const GX_RECTANGLE* size);
UINT _gx_widget_created_test(GX_WIDGET* widget, GX_BOOL* return_test);
UINT _gx_widget_delete(GX_WIDGET* widget);
UINT _gx_widget_detach(GX_WIDGET* child);
void _gx_widget_draw(GX_WIDGET* widget);
UINT _gx_widget_draw_set(GX_WIDGET* widget, void (*draw_func)(GX_WIDGET*));
UINT _gx_widget_event_generate(GX_WIDGET* widget, USHORT event_type, LONG value);
UINT _gx_widget_event_process(GX_WIDGET* widget, GX_EVENT* event_ptr);
UINT _gx_widget_event_process_set(GX_WIDGET* widget, UINT(*event_processing_function)(GX_WIDGET*, GX_EVENT*));
UINT _gx_widget_event_to_parent(GX_WIDGET* widget, GX_EVENT* event_ptr);
UINT _gx_widget_fill_color_set(GX_WIDGET* widget, GX_RESOURCE_ID normal_color, GX_RESOURCE_ID selected_color, GX_RESOURCE_ID disabled_color);
UINT _gx_widget_find(GX_WIDGET* parent, USHORT widget_id, INT search_depth, GX_WIDGET** return_widget);
UINT _gx_widget_first_child_get(GX_WIDGET* parent, GX_WIDGET** child_return);
UINT _gx_widget_font_get(GX_WIDGET* widget, GX_RESOURCE_ID font_id, GX_FONT** return_font);
UINT _gx_widget_focus_next(GX_WIDGET* widget);
UINT _gx_widget_focus_previous(GX_WIDGET* widget);
UINT _gx_widget_free(GX_WIDGET* widget);
UINT _gx_widget_front_move(GX_WIDGET* widget, GX_BOOL* return_moved);
UINT _gx_widget_height_get(GX_WIDGET* widget, GX_VALUE* return_height);
UINT _gx_widget_hide(GX_WIDGET* widget);
UINT _gx_widget_last_child_get(GX_WIDGET* parent, GX_WIDGET** child_return);
UINT _gx_widget_next_sibling_get(GX_WIDGET* current, GX_WIDGET** sibling_return);
UINT _gx_widget_parent_get(GX_WIDGET* current, GX_WIDGET** parent_return);
UINT _gx_widget_pixelmap_get(GX_WIDGET* widget, GX_RESOURCE_ID pixelmap_id, GX_PIXELMAP** return_map);
UINT _gx_widget_previous_sibling_get(GX_WIDGET* current, GX_WIDGET** sibling_return);
UINT _gx_widget_resize(GX_WIDGET* widget, GX_RECTANGLE* new_size);
UINT _gx_widget_shift(GX_WIDGET* widget, GX_VALUE x_shift, GX_VALUE y_shift, GX_BOOL mark_dirty);
UINT _gx_widget_show(GX_WIDGET* widget);
UINT _gx_widget_status_add(GX_WIDGET* widget, ULONG status);
UINT _gx_widget_status_get(GX_WIDGET* widget, ULONG* return_status);
UINT _gx_widget_status_remove(GX_WIDGET* widget, ULONG status);
UINT _gx_widget_status_test(GX_WIDGET* widget, ULONG status, GX_BOOL* return_test);
UINT _gx_widget_string_get_ext(GX_WIDGET* widget, GX_RESOURCE_ID string_id, GX_STRING* return_string);
UINT _gx_widget_style_add(GX_WIDGET* widget, ULONG style);
UINT _gx_widget_style_get(GX_WIDGET* widget, ULONG* return_style);
UINT _gx_widget_style_remove(GX_WIDGET* widget, ULONG style);
UINT _gx_widget_style_set(GX_WIDGET* widget, ULONG style);
UINT _gx_widget_text_blend_ext(GX_WIDGET* widget, UINT tColor, UINT font_id,
                               const GX_STRING* string, INT x_offset, INT y_offset, UCHAR alpha);
void _gx_widget_text_draw_ext(GX_WIDGET* widget, UINT tColor, UINT font_id,
                              const GX_STRING* string, INT x_offset, INT y_offset);
void _gx_widget_text_id_draw(GX_WIDGET* widget, UINT tColor, UINT font_id,
                             UINT text_id, INT x_offset, INT y_offset);
UINT _gx_widget_top_visible_child_find(GX_WIDGET* parent, GX_WIDGET** child_return);
UINT _gx_widget_type_find(GX_WIDGET* parent, USHORT widget_type, GX_WIDGET** return_widget);
UINT _gx_widget_width_get(GX_WIDGET* widget, GX_VALUE* return_width);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_widget_allocate(GX_WIDGET** widget, ULONG memsize);
UINT _gxe_widget_attach(GX_WIDGET* parent, GX_WIDGET* child);
UINT _gxe_widget_back_attach(GX_WIDGET* parent, GX_WIDGET* child);
UINT _gxe_widget_back_move(GX_WIDGET* widget, GX_BOOL* widget_moved);

UINT _gxe_widget_block_move(GX_WIDGET* widget, GX_RECTANGLE* block, INT x_shift, INT y_shift);

UINT _gxe_widget_border_style_set(GX_WIDGET* widget, ULONG Style);
UINT _gxe_widget_border_width_get(GX_WIDGET* widget, GX_VALUE* return_width);
UINT _gxe_widget_canvas_get(GX_WIDGET* widget, GX_CANVAS** return_canvas);
UINT _gxe_widget_child_detect(GX_WIDGET* parent, GX_WIDGET* child, GX_BOOL* return_detect);

UINT _gxe_widget_client_get(GX_WIDGET* widget, GX_VALUE border_width, GX_RECTANGLE* return_size);
UINT _gxe_widget_color_get(GX_WIDGET* widget, GX_RESOURCE_ID color_id, GX_COLOR* return_color);
UINT _gxe_widget_create(GX_WIDGET* widget, const GX_CHAR* name, GX_WIDGET* parent, ULONG style, USHORT Id, const GX_RECTANGLE* size, UINT widget_block_size);
UINT _gxe_widget_created_test(GX_WIDGET* widget, GX_BOOL* return_test);
UINT _gxe_widget_delete(GX_WIDGET* widget);
UINT _gxe_widget_detach(GX_WIDGET* widget);

UINT _gxe_widget_draw_set(GX_WIDGET* widget, void (*draw_func)(GX_WIDGET*));
UINT _gxe_widget_event_generate(GX_WIDGET* widget, USHORT event_type, LONG value);
UINT _gxe_widget_event_process(GX_WIDGET* widget, GX_EVENT* event_ptr);
UINT _gxe_widget_event_process_set(GX_WIDGET* widget, UINT(*event_processing_function)(GX_WIDGET*, GX_EVENT*));
UINT _gxe_widget_event_to_parent(GX_WIDGET* widget, GX_EVENT* event_ptr);
UINT _gxe_widget_fill_color_set(GX_WIDGET* widget, GX_RESOURCE_ID normal_color, GX_RESOURCE_ID selected_color, GX_RESOURCE_ID disabled_color);
UINT _gxe_widget_find(GX_WIDGET* parent, USHORT widget_id, INT search_depth, GX_WIDGET** return_widget);
UINT _gxe_widget_first_child_get(GX_WIDGET* parent, GX_WIDGET** child_return);
UINT _gxe_widget_focus_next(GX_WIDGET* widget);
UINT _gxe_widget_focus_previous(GX_WIDGET* widget);
UINT _gxe_widget_font_get(GX_WIDGET* widget, GX_RESOURCE_ID resource_id, GX_FONT** return_font);
UINT _gxe_widget_free(GX_WIDGET* widget);
UINT _gxe_widget_front_move(GX_WIDGET* widget, GX_BOOL* return_moved);
UINT _gxe_widget_height_get(GX_WIDGET* widget, GX_VALUE* return_height);
UINT _gxe_widget_hide(GX_WIDGET* widget);
UINT _gxe_widget_last_child_get(GX_WIDGET* parent, GX_WIDGET** child_return);
UINT _gxe_widget_next_sibling_get(GX_WIDGET* current, GX_WIDGET** sibling_return);
UINT _gxe_widget_parent_get(GX_WIDGET* current, GX_WIDGET** parent_return);
UINT _gxe_widget_previous_sibling_get(GX_WIDGET* current, GX_WIDGET** sibling_return);
UINT _gxe_widget_pixelmap_get(GX_WIDGET* widget, GX_RESOURCE_ID resource_id, GX_PIXELMAP** return_pixelmap);
UINT _gxe_widget_resize(GX_WIDGET* widget, GX_RECTANGLE* new_size);
UINT _gxe_widget_shift(GX_WIDGET* widget, GX_VALUE x_shift, GX_VALUE y_shift, GX_BOOL mark_dirty);
UINT _gxe_widget_show(GX_WIDGET* widget);
UINT _gxe_widget_status_add(GX_WIDGET* widget, ULONG status);
UINT _gxe_widget_status_get(GX_WIDGET* widget, ULONG* return_status);
UINT _gxe_widget_status_remove(GX_WIDGET* widget, ULONG status);
UINT _gxe_widget_status_test(GX_WIDGET* widget, ULONG status, GX_BOOL* return_test);
UINT _gxe_widget_string_get_ext(GX_WIDGET* widget, GX_RESOURCE_ID string_id, GX_STRING* return_string);
UINT _gxe_widget_style_add(GX_WIDGET* widget, ULONG style);
UINT _gxe_widget_style_get(GX_WIDGET* widget, ULONG* return_style);
UINT _gxe_widget_style_remove(GX_WIDGET* widget, ULONG style);
UINT _gxe_widget_style_set(GX_WIDGET* widget, ULONG style);
UINT _gxe_widget_text_blend_ext(GX_WIDGET* widget, UINT tColor, UINT font_id,
                                const GX_STRING* string, INT x_offset, INT y_offset, UCHAR alpha);




UINT _gxe_widget_top_visible_child_find(GX_WIDGET* parent, GX_WIDGET** child_return);
UINT _gxe_widget_type_find(GX_WIDGET* parent, USHORT widget_type, GX_WIDGET** return_widget);
UINT _gxe_widget_width_get(GX_WIDGET* widget, GX_VALUE* return_width);

#endif

