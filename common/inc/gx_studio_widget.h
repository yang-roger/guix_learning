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

#ifndef GX_STUDIO_WIDGET_H
#define GX_STUDIO_WIDGET_H

#include "gx_api.h"

//--------------------------------------------------------------------------------------------------
// Display

struct GX_STUDIO_DISPLAY_INFO
{
    const GX_CHAR*      name;
    const GX_CHAR*      canvas_name;
    const GX_THEME**    theme_table;
    const GX_STRING**   language_table;
    USHORT              theme_table_size;
    USHORT              language_table_size;
    UINT                string_table_size;
    UINT                x_resolution;
    UINT                y_resolution;
    GX_DISPLAY*         display;
    GX_CANVAS*          canvas;
    GX_WINDOW_ROOT*     root_window;
    GX_COLOR*           canvas_memory;
    ULONG               canvas_memory_size;
    USHORT              rotation_angle;
};

UINT gx_studio_display_config(const GX_STUDIO_DISPLAY_INFO* display_info,
                              UINT (*driver)(GX_DISPLAY *),
                              GX_UBYTE language,
                              USHORT theme,
                              GX_WINDOW_ROOT** return_root);

//--------------------------------------------------------------------------------------------------
// Action, Event Process

#define GX_ACTION_FLAG_DYNAMIC_TARGET 0x01
#define GX_ACTION_FLAG_DYNAMIC_PARENT 0x02
#define GX_ACTION_FLAG_POP_TARGET     0x04
#define GX_ACTION_FLAG_POP_PARENT     0x08

struct GX_STUDIO_ACTION
{
    GX_UBYTE opcode;
    GX_UBYTE flags;
    const void* parent;
    const void* target;
    const GX_ANIMATION_INFO* animation;
};

struct GX_STUDIO_EVENT_ENTRY
{
    ULONG event_type;
    USHORT event_sender;
    const GX_STUDIO_ACTION* action_list;
};

struct GX_STUDIO_EVENT_PROCESS
{
    const GX_STUDIO_EVENT_ENTRY* event_table;
    UINT (*chain_event_handler)(GX_WIDGET*, GX_EVENT*);
};

UINT gx_studio_auto_event_handler(GX_WIDGET *widget, GX_EVENT *event_ptr, GX_CONST GX_STUDIO_EVENT_PROCESS *record);

//--------------------------------------------------------------------------------------------------
// Widget properties and create functions

struct GX_STUDIO_WIDGET
{
   GX_CHAR* widget_name;
   USHORT widget_type;
   USHORT widget_id;
#if defined(GX_WIDGET_USER_DATA)
   INT user_data;
#endif
   ULONG style;
   ULONG status;
   ULONG control_block_size;
   GX_RESOURCE_ID normal_fill_color_id;
   GX_RESOURCE_ID selected_fill_color_id;
   GX_RESOURCE_ID disabled_fill_color_id;
   UINT (*create_function)(const GX_STUDIO_WIDGET*, GX_WIDGET*, GX_WIDGET*);
   void (*draw_function)(GX_WIDGET*);
   UINT (*event_function)(GX_WIDGET*, GX_EVENT*);
   GX_RECTANGLE size;
   const GX_STUDIO_WIDGET* next_widget;
   const GX_STUDIO_WIDGET* child_widget;
   ULONG control_block_offset;
   const void* properties;
};

struct GX_TEXT_BUTTON_PROPERTIES
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
};

struct GX_ML_TEXT_BUTTON_PROPERTIES
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
};

struct GX_CHECKBOX_PROPERTIES
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    GX_RESOURCE_ID unchecked_pixelmap_id;
    GX_RESOURCE_ID checked_pixelmap_id;
    GX_RESOURCE_ID unchecked_disabled_pixelmap_id;
    GX_RESOURCE_ID checked_disabled_pixelmap_id;
};

struct GX_RADIO_BUTTON_PROPERTIES
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    GX_RESOURCE_ID off_pixelmap_id;
    GX_RESOURCE_ID on_pixelmap_id;
    GX_RESOURCE_ID off_disabled_pixelmap_id;
    GX_RESOURCE_ID on_disabled_pixelmap_id;
};

struct GX_ICON_BUTTON_PROPERTIES
{
    GX_RESOURCE_ID pixelmap_id;
};

struct GX_PIXELMAP_BUTTON_PROPERTIES
{
    GX_RESOURCE_ID normal_pixelmap_id;
    GX_RESOURCE_ID selected_pixelmap_id;
    GX_RESOURCE_ID disabled_pixelmap_id;
};

struct GX_ICON_PROPERTIES
{
    GX_RESOURCE_ID normal_pixelmap_id;
    GX_RESOURCE_ID selected_pixelmap_id;
};

struct GX_SLIDER_PROPERTIES
{
    int tickmark_count;
    int minval;
    int maxval;
    int current_val;
    int increment;
    GX_VALUE min_travel;
    GX_VALUE max_travel;
    GX_VALUE needle_width;
    GX_VALUE needle_height;
    GX_VALUE needle_inset;
    GX_VALUE needle_hotspot;
};

struct GX_PIXELMAP_SLIDER_PROPERTIES
{
    int min_val;
    int max_val;
    int current_val;
    int increment;
    GX_VALUE min_travel;
    GX_VALUE max_travel;
    GX_VALUE needle_width;
    GX_VALUE needle_height;
    GX_VALUE needle_inset;
    GX_VALUE needle_hotspot;
    GX_RESOURCE_ID lower_pixelmap;
    GX_RESOURCE_ID upper_pixelmap;
    GX_RESOURCE_ID needle_pixelmap;
};

struct GX_RADIAL_SLIDER_PROPERTIES
{
    GX_VALUE       xcenter;
    GX_VALUE       ycenter;
    USHORT         radius;
    USHORT         track_width;
    GX_VALUE       needle_offset;
    GX_VALUE       current_angle;
    GX_VALUE       min_angle;
    GX_VALUE       max_angle;
    GX_RESOURCE_ID background_pixelmap;
    GX_RESOURCE_ID needle_pixelmap;
    USHORT         animation_total_steps;
    USHORT         animation_delay;
    USHORT         animation_style;
    void         (*animation_update_callback)(GX_RADIAL_SLIDER *slider);
};

struct GX_SPRITE_PROPERTIES
{
    GX_SPRITE_FRAME *frame_list;
    USHORT           frame_count;
};

struct GX_PROMPT_PROPERTIES
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
};

struct GX_NUMERIC_PROMPT_PROPERTIES
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    void (*format_func)(GX_NUMERIC_PROMPT *, INT);
    INT            numeric_prompt_value;
};

struct GX_PIXELMAP_PROMPT_PROPERTIES
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    GX_RESOURCE_ID left_map_id;
    GX_RESOURCE_ID fill_map_id;
    GX_RESOURCE_ID right_map_id;
    GX_RESOURCE_ID selected_left_map_id;
    GX_RESOURCE_ID selected_fill_map_id;
    GX_RESOURCE_ID selected_right_map_id;
};

struct GX_NUMERIC_PIXELMAP_PROMPT_PROPERTIES
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    GX_RESOURCE_ID left_map_id;
    GX_RESOURCE_ID fill_map_id;
    GX_RESOURCE_ID right_map_id;
    GX_RESOURCE_ID selected_left_map_id;
    GX_RESOURCE_ID selected_fill_map_id;
    GX_RESOURCE_ID selected_right_map_id;
    void (*format_func)(GX_NUMERIC_PIXELMAP_PROMPT *, INT);
    INT            numeric_prompt_value;
};

struct GX_WINDOW_PROPERTIES
{
    GX_RESOURCE_ID wallpaper_id;
};

struct GX_VERTICAL_LIST_PROPERTIES
{
    GX_RESOURCE_ID wallpaper_id;
    void (*callback)(GX_VERTICAL_LIST *, GX_WIDGET *, INT);
    int total_rows;
};

struct GX_HORIZONTAL_LIST_PROPERTIES
{
    GX_RESOURCE_ID wallpaper_id;
    void (*callback)(GX_HORIZONTAL_LIST *, GX_WIDGET *, INT);
    int total_rows;
};

struct GX_DROP_LIST_PROPERTIES
{
    GX_RESOURCE_ID pixelmap_id;
    GX_RESOURCE_ID wallpaper_id;
    void (*callback)(GX_VERTICAL_LIST *, GX_WIDGET *, INT);
    int total_rows;
    int open_height;
};

struct GX_SCROLL_WHEEL_PROPERTIES
{
    int                 total_rows;
    int                 selected_row;
    GX_VALUE            row_height;
    GX_UBYTE            start_alpha;
    GX_UBYTE            end_alpha;
    GX_RESOURCE_ID      wallpaper_id;
    GX_RESOURCE_ID      selected_background;
};

struct GX_GENERIC_SCROLL_WHEEL_PROPERTIES
{
    int                 total_rows;
    int                 selected_row;
    GX_VALUE            row_height;
    GX_UBYTE            start_alpha;
    GX_UBYTE            end_alpha;
    GX_RESOURCE_ID      wallpaper_id;
    GX_RESOURCE_ID      selected_background;
    void (*callback)(GX_GENERIC_SCROLL_WHEEL*, GX_WIDGET*, INT);
};

struct GX_TEXT_SCROLL_WHEEL_PROPERTIES
{
    int                 total_rows;
    int                 selected_row;
    GX_VALUE            row_height;
    GX_UBYTE            start_alpha;
    GX_UBYTE            end_alpha;
    GX_RESOURCE_ID      normal_font;
    GX_RESOURCE_ID      selected_font;
    GX_RESOURCE_ID      normal_text_color;
    GX_RESOURCE_ID      selected_text_color;
    GX_RESOURCE_ID      disabled_text_color;
    GX_RESOURCE_ID      wallpaper_id;
    GX_RESOURCE_ID      selected_background;
};

struct GX_STRING_SCROLL_WHEEL_PROPERTIES
{
    int                 total_rows;
    int                 selected_row;
    GX_VALUE            row_height;
    GX_UBYTE            start_alpha;
    GX_UBYTE            end_alpha;
    GX_RESOURCE_ID      normal_font;
    GX_RESOURCE_ID      selected_font;
    GX_RESOURCE_ID      normal_text_color;
    GX_RESOURCE_ID      selected_text_color;
    GX_RESOURCE_ID      disabled_text_color;
    GX_RESOURCE_ID      wallpaper_id;
    GX_RESOURCE_ID      selected_background;
    const GX_RESOURCE_ID  *string_id_list;
    UINT (*callback)(GX_STRING_SCROLL_WHEEL *, INT, GX_STRING *);
};

struct GX_NUMERIC_SCROLL_WHEEL_PROPERTIES
{
    int                 total_rows;
    int                 selected_row;
    GX_VALUE            row_height;
    GX_UBYTE            start_alpha;
    GX_UBYTE            end_alpha;
    GX_RESOURCE_ID      normal_font;
    GX_RESOURCE_ID      selected_font;
    GX_RESOURCE_ID      normal_text_color;
    GX_RESOURCE_ID      selected_text_color;
    GX_RESOURCE_ID      disabled_text_color;
    GX_RESOURCE_ID      wallpaper_id;
    GX_RESOURCE_ID      selected_background;
    UINT (*callback)(GX_NUMERIC_SCROLL_WHEEL *, INT, GX_STRING *);
    int                 start_val;
    int                 end_val;
};

struct GX_SINGLE_LINE_TEXT_INPUT_PROPERTIES
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    GX_RESOURCE_ID readonly_fill_color_id;
    GX_RESOURCE_ID readonly_text_color_id;
    GX_CHAR *buffer;
    UINT buffer_size;
};

struct GX_ML_TEXT_VIEW_PROPERTIES
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    GX_UBYTE       whitespace;
    GX_BYTE        line_space;
};

struct GX_MULTI_LINE_TEXT_INPUT_PROPERTIES
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    GX_RESOURCE_ID readonly_fill_color_id;
    GX_RESOURCE_ID readonly_text_color_id;
    GX_UBYTE whitespace;
    GX_BYTE  line_space;
    GX_CHAR *buffer;
    UINT buffer_size;
};

struct GX_CIRCULAR_GAUGE_PROPERTIES
{
    GX_VALUE start_angle;
    GX_VALUE animation_steps;
    GX_VALUE animation_delay;
    GX_VALUE needle_xpos;
    GX_VALUE needle_ypos;
    GX_VALUE needle_xcor;
    GX_VALUE needle_ycor;
    GX_RESOURCE_ID normal_pixelmap_id;
    GX_RESOURCE_ID selected_pixelmap_id;
    GX_RESOURCE_ID needle_pixelmap_id;
};

struct GX_TEMPLATE_PROPERTIES
{
   GX_CONST GX_STUDIO_WIDGET* base_info;
   UINT (*base_create_function)(GX_CONST GX_STUDIO_WIDGET*, GX_WIDGET*, GX_WIDGET*);
   GX_RECTANGLE size;
};

struct GX_MENU_PROPERTIES
{
    GX_RESOURCE_ID string_id;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color_id;
    GX_RESOURCE_ID selected_text_color_id;
    GX_RESOURCE_ID disabled_text_color_id;
    GX_RESOURCE_ID left_map_id;
    GX_RESOURCE_ID fill_map_id;
    GX_RESOURCE_ID right_map_id;
    GX_RESOURCE_ID selected_left_map_id;
    GX_RESOURCE_ID selected_fill_map_id;
    GX_RESOURCE_ID selected_right_map_id;
    GX_VALUE       text_x_offset;
    GX_VALUE       text_y_offset;
    GX_VALUE       list_total_count;
};

struct GX_TREE_VIEW_PROPERTIES
{
    GX_RESOURCE_ID expand_map_id;
    GX_RESOURCE_ID collapse_map_id;
    GX_RESOURCE_ID root_line_color_id;
    GX_VALUE       indentation;
};

struct GX_RICH_TEXT_VIEW_PROPERTIES
{
    GX_RESOURCE_ID      string_id;
    GX_RESOURCE_ID      normal_text_color_id;
    GX_RESOURCE_ID      selected_text_color_id;
    GX_RESOURCE_ID      disabled_text_color_id;
    GX_UBYTE            whitespace;
    GX_BYTE             line_space;
    GX_RICH_TEXT_FONTS *fonts;
};

UINT gx_studio_button_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_text_button_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_multi_line_text_button_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_checkbox_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_radio_button_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_icon_button_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_pixelmap_button_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_icon_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_slider_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_pixelmap_slider_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_progress_bar_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_radial_progress_bar_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_radial_slider_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_sprite_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_prompt_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_numeric_prompt_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_pixelmap_prompt_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_numeric_pixelmap_prompt_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_window_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_vertical_list_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_horizontal_list_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_drop_list_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_scroll_wheel_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_generic_scroll_wheel_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_text_scroll_wheel_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_string_scroll_wheel_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_numeric_scroll_wheel_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_text_input_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_multi_line_text_view_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_multi_line_text_input_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_horizontal_scrollbar_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_vertical_scrollbar_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_circular_gauge_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_line_chart_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_template_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_menu_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_accordion_menu_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_tree_view_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);
UINT gx_studio_rich_text_view_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent);

GX_WIDGET* gx_studio_widget_create(GX_BYTE* control, const GX_STUDIO_WIDGET* definition, GX_WIDGET* parent);

struct GX_STUDIO_WIDGET_ENTRY
{
    const GX_STUDIO_WIDGET* widget_information;
    GX_WIDGET* widget;
};

UINT gx_studio_named_widget_create_from_table(const char* name, GX_WIDGET* parent, GX_WIDGET** new_widget,
                                              const GX_STUDIO_WIDGET_ENTRY* widget_table);

#endif
