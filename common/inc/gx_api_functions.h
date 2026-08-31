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
/**   Application Interface (API)                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef GX_API_FUNCTIONS_H
#define GX_API_FUNCTIONS_H

/* Determine if error checking is desired.  If so, map API functions
   to the appropriate error checking front-ends.  Otherwise, map API
   functions to the core functions that actually perform the work.
   Note: error checking is enabled by default.  */

#ifdef GX_DISABLE_ERROR_CHECKING

/* Services without error checking.  */

#define gx_accordion_menu_create(a, b, c, d, e, f)               _gx_accordion_menu_create(a, b, c, d, e, f)
#define gx_accordion_menu_draw                                   _gx_accordion_menu_draw
#define gx_accordion_menu_event_process                          _gx_accordion_menu_event_process
#define gx_accordion_menu_position(a)                            _gx_accordion_menu_position(a)

#define gx_animation_canvas_define                               _gx_animation_canvas_define
#define gx_animation_create                                      _gx_animation_create
#define gx_animation_delete                                      _gx_animation_delete
#define gx_animation_drag_disable                                _gx_animation_drag_disable
#define gx_animation_drag_enable                                 _gx_animation_drag_enable
#define gx_animation_landing_speed_set                           _gx_animation_landing_speed_set
#define gx_animation_start                                       _gx_animation_start
#define gx_animation_stop                                        _gx_animation_stop

#define gx_binres_language_count_get                             _gx_binres_language_count_get
#define gx_binres_language_info_load                             _gx_binres_language_info_load
#define gx_binres_language_table_load_ext                        _gx_binres_language_table_load_ext
#define gx_binres_theme_load                                     _gx_binres_theme_load
#define gx_binres_pixelmap_load                                  _gx_binres_pixelmap_load
#define gx_binres_font_load                                      _gx_binres_font_load

#define gx_brush_default                                         _gx_brush_default
#define gx_brush_define                                          _gx_brush_define

#define gx_button_background_draw                                _gx_button_background_draw
#define gx_button_create(a, b, c, d, e, f)                       _gx_button_create(a, b, (GX_WIDGET *)c, d, e, f)
#define gx_button_deselect                                       _gx_button_deselect
#define gx_button_draw                                           _gx_button_draw
#define gx_button_event_process                                  _gx_button_event_process
#define gx_button_select                                         _gx_button_select

#define gx_canvas_alpha_set                                      _gx_canvas_alpha_set
#define gx_canvas_arc_draw                                       _gx_canvas_arc_draw
#define gx_canvas_block_move(a, b, c, d)                         _gx_canvas_block_move(a, b, c, d)
#define gx_canvas_circle_draw                                    _gx_canvas_circle_draw
#define gx_canvas_create                                         _gx_canvas_create
#define gx_canvas_delete                                         _gx_canvas_delete
#define gx_canvas_drawing_complete                               _gx_canvas_drawing_complete
#define gx_canvas_drawing_initiate(a, b, c)                      _gx_canvas_drawing_initiate(a, (GX_WIDGET *)b, c)
#define gx_canvas_ellipse_draw                                   _gx_canvas_ellipse_draw
#define gx_canvas_hardware_layer_bind                            _gx_canvas_hardware_layer_bind
#define gx_canvas_hide                                           _gx_canvas_hide
#define gx_canvas_line_draw                                      _gx_canvas_line_draw
#define gx_canvas_memory_define                                  _gx_canvas_memory_define
#if defined (GX_MOUSE_SUPPORT)
#define gx_canvas_mouse_define                                   _gx_canvas_mouse_define
#define gx_canvas_mouse_hide                                     _gx_canvas_mouse_hide
#define gx_canvas_mouse_show                                     _gx_canvas_mouse_show
#endif
#define gx_canvas_offset_set                                     _gx_canvas_offset_set
#define gx_canvas_pixel_draw                                     _gx_canvas_pixel_draw
#define gx_canvas_pixelmap_blend                                 _gx_canvas_pixelmap_blend
#define gx_canvas_pixelmap_draw                                  _gx_canvas_pixelmap_draw
#define gx_canvas_pixelmap_get                                   _gx_canvas_pixelmap_get
#define gx_canvas_pixelmap_rotate                                _gx_canvas_pixelmap_rotate
#define gx_canvas_pixelmap_tile                                  _gx_canvas_pixelmap_tile
#define gx_canvas_polygon_draw                                   _gx_canvas_polygon_draw
#define gx_canvas_pie_draw                                       _gx_canvas_pie_draw
#define gx_canvas_rectangle_draw                                 _gx_canvas_rectangle_draw
#define gx_canvas_rotated_text_draw_ext                          _gx_canvas_rotated_text_draw_ext
#define gx_canvas_shift                                          _gx_canvas_shift
#define gx_canvas_show                                           _gx_canvas_show
#define gx_canvas_aligned_text_draw                              _gx_canvas_aligned_text_draw
#define gx_canvas_text_draw_ext                                  _gx_canvas_text_draw_ext

#define gx_checkbox_create(a, b, c, d, e, f, g)                  _gx_checkbox_create(a, b, (GX_WIDGET *)c, d, e, f, g)
#define gx_checkbox_draw                                         _gx_checkbox_draw
#define gx_checkbox_event_process                                _gx_checkbox_event_process
#define gx_checkbox_pixelmap_set                                 _gx_checkbox_pixelmap_set
#define gx_checkbox_select                                       _gx_checkbox_select

#define gx_circular_gauge_angle_get                              _gx_circular_gauge_angle_get
#define gx_circular_gauge_angle_set                              _gx_circular_gauge_angle_set
#define gx_circular_gauge_animation_set                          _gx_circular_gauge_animation_set
#define gx_circular_gauge_background_draw                        _gx_circular_gauge_background_draw
#define gx_circular_gauge_create(a, b, c, d, e, f, g, h, i)      _gx_circular_gauge_create((GX_CIRCULAR_GAUGE *) a, b, c, d, e, f, g, h, i)
#define gx_circular_gauge_draw                                   _gx_circular_gauge_draw
#define gx_circular_gauge_event_process                          _gx_circular_gauge_event_process

#define gx_context_brush_default                                 _gx_context_brush_default
#define gx_context_brush_define                                  _gx_context_brush_define
#define gx_context_brush_get                                     _gx_context_brush_get
#define gx_context_brush_pattern_set                             _gx_context_brush_pattern_set
#define gx_context_brush_set                                     _gx_context_brush_set
#define gx_context_brush_style_set                               _gx_context_brush_style_set
#define gx_context_brush_width_set                               _gx_context_brush_width_set
#define gx_context_color_get                                     _gx_context_color_get
#define gx_context_fill_color_set                                _gx_context_fill_color_set
#define gx_context_font_get                                      _gx_context_font_get
#define gx_context_font_set                                      _gx_context_font_set
#define gx_context_line_color_set                                _gx_context_line_color_set
#define gx_context_pixelmap_get                                  _gx_context_pixelmap_get
#define gx_context_pixelmap_set                                  _gx_context_pixelmap_set
#define gx_context_raw_brush_define                              _gx_context_raw_brush_define
#define gx_context_raw_fill_color_set                            _gx_context_raw_fill_color_set
#define gx_context_raw_line_color_set                            _gx_context_raw_line_color_set
#define gx_context_string_get_ext                                _gx_context_string_get_ext

#define gx_display_active_language_set                           _gx_display_active_language_set
#define gx_display_color_set                                     _gx_display_color_set
#define gx_display_color_table_set                               _gx_display_color_table_set
#define gx_display_create                                        _gx_display_create
#define gx_display_delete                                        _gx_display_delete
#define gx_display_font_table_set                                _gx_display_font_table_set
#define gx_display_language_table_get_ext                        _gx_display_language_table_get_ext
#define gx_display_language_table_set_ext                        _gx_display_language_table_set_ext
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
#define gx_display_language_direction_table_set                  _gx_display_language_direction_table_set
#endif
#define gx_display_pixelmap_table_set                            _gx_display_pixelmap_table_set
#define gx_display_string_get_ext                                _gx_display_string_get_ext
#define gx_display_string_table_get_ext                          _gx_display_string_table_get_ext
#define gx_display_theme_install                                 _gx_display_theme_install

#define gx_drop_list_close                                       _gx_drop_list_close
#define gx_drop_list_create(a, b, c, d, e, f, g, h, i)           _gx_drop_list_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, i)
#define gx_drop_list_event_process                               _gx_drop_list_event_process
#define gx_drop_list_open                                        _gx_drop_list_open
#define gx_drop_list_pixelmap_set                                _gx_drop_list_pixelmap_set
#define gx_drop_list_popup_get                                   _gx_drop_list_popup_get

#define gx_generic_scroll_wheel_children_position                _gx_generic_scroll_wheel_children_position
#define gx_generic_scroll_wheel_create                           _gx_generic_scroll_wheel_create
#define gx_generic_scroll_wheel_draw                             _gx_generic_scroll_wheel_draw
#define gx_generic_scroll_wheel_event_process                    _gx_generic_scroll_wheel_event_process
#define gx_generic_scroll_wheel_row_height_set                   _gx_generic_scroll_wheel_row_height_set
#define gx_generic_scroll_wheel_total_rows_set                   _gx_generic_scroll_wheel_total_rows_set

#define gx_horizontal_list_children_position                     _gx_horizontal_list_children_position
#define gx_horizontal_list_create(a, b, c, d, e, f, g, h)        _gx_horizontal_list_create(a, b, (GX_WIDGET *)c, d, e, f, g, h)
#define gx_horizontal_list_event_process                         _gx_horizontal_list_event_process
#define gx_horizontal_list_page_index_set(a, b)                  _gx_horizontal_list_page_index_set((GX_HORIZONTAL_LIST *)a, b)
#define gx_horizontal_list_selected_index_get(a, b)              _gx_horizontal_list_selected_index_get(a, b)
#define gx_horizontal_list_selected_set(a, b)                    _gx_horizontal_list_selected_set(a, b)
#define gx_horizontal_list_selected_widget_get(a, b)             _gx_horizontal_list_selected_widget_get(a, (GX_WIDGET **)b)
#define gx_horizontal_list_total_columns_set(a, b)               _gx_horizontal_list_total_columns_set((GX_HORIZONTAL_LIST *)a, b)

#define gx_icon_background_draw                                  _gx_icon_background_draw
#define gx_icon_create(a, b, c, d, e, f, g, h)                   _gx_icon_create(a, b, (GX_WIDGET *)c, d, e, f, g, h)
#define gx_icon_draw                                             _gx_icon_draw
#define gx_icon_event_process                                    _gx_icon_event_process
#define gx_icon_pixelmap_set                                     _gx_icon_pixelmap_set

#define gx_icon_button_create(a, b, c, d, e, f, g)               _gx_icon_button_create(a, b, (GX_WIDGET *)c, d, e, f, g)
#define gx_icon_button_draw                                      _gx_icon_button_draw
#define gx_icon_button_pixelmap_set                              _gx_icon_button_pixelmap_set

#define gx_image_reader_create                                   _gx_image_reader_create
#define gx_image_reader_palette_set                              _gx_image_reader_palette_set
#define gx_image_reader_start                                    _gx_image_reader_start

#define gx_line_chart_axis_draw                                  _gx_line_chart_axis_draw
#define gx_line_chart_create(a, b, c, d, e, f, g)                _gx_line_chart_create(a, b, (GX_WIDGET *) c, d, e, f, g)
#define gx_line_chart_data_draw                                  _gx_line_chart_data_draw
#define gx_line_chart_draw                                       _gx_line_chart_draw
#define gx_line_chart_update                                     _gx_line_chart_update
#define gx_line_chart_y_scale_calculate                          _gx_line_chart_y_scale_calculate

#define gx_menu_create(a, b, c, d, e, f, g, h)                   _gx_menu_create((GX_MENU *)a, b, (GX_WIDGET *)c, d, e, f, g, h)
#define gx_menu_draw                                             _gx_menu_draw
#define gx_menu_event_process                                    _gx_menu_event_process
#define gx_menu_insert                                           _gx_menu_insert
#define gx_menu_remove                                           _gx_menu_remove
#define gx_menu_text_draw                                        _gx_menu_text_draw
#define gx_menu_text_offset_set                                  _gx_menu_text_offset_set

#define gx_multi_line_text_button_create(a, b, c, d, e, f, g)    _gx_multi_line_text_button_create(a, b, (GX_WIDGET *)c, d, e, f, g)
#define gx_multi_line_text_button_draw                           _gx_multi_line_text_button_draw
#define gx_multi_line_text_button_event_process                  _gx_multi_line_text_button_event_process
#define gx_multi_line_text_button_text_draw                      _gx_multi_line_text_button_text_draw
#define gx_multi_line_text_button_text_id_set                    _gx_multi_line_text_button_text_id_set
#define gx_multi_line_text_button_text_set_ext                   _gx_multi_line_text_button_text_set_ext

#define gx_multi_line_text_input_backspace                       _gx_multi_line_text_input_backspace
#define gx_multi_line_text_input_buffer_clear                    _gx_multi_line_text_input_buffer_clear
#define gx_multi_line_text_input_buffer_get                      _gx_multi_line_text_input_buffer_get
#define gx_multi_line_text_input_char_insert_ext                 _gx_multi_line_text_input_char_insert_ext
#define gx_multi_line_text_input_create(a, b, c, d, e, f, g, h)  _gx_multi_line_text_input_create(a, b, (GX_WIDGET *)c, d, e, f, g, h)
#define gx_multi_line_text_input_cursor_pos_get                  _gx_multi_line_text_input_cursor_pos_get
#define gx_multi_line_text_input_delete                          _gx_multi_line_text_input_delete
#define gx_multi_line_text_input_down_arrow                      _gx_multi_line_text_input_down_arrow
#define gx_multi_line_text_input_end                             _gx_multi_line_text_input_end
#define gx_multi_line_text_input_event_process                   _gx_multi_line_text_input_event_process
#define gx_multi_line_text_input_fill_color_set                  _gx_multi_line_text_input_fill_color_set
#define gx_multi_line_text_input_home                            _gx_multi_line_text_input_home
#define gx_multi_line_text_input_left_arrow                      _gx_multi_line_text_input_left_arrow
#define gx_multi_line_text_input_right_arrow                     _gx_multi_line_text_input_right_arrow
#define gx_multi_line_text_input_style_add                       _gx_multi_line_text_input_style_add
#define gx_multi_line_text_input_style_remove                    _gx_multi_line_text_input_style_remove
#define gx_multi_line_text_input_style_set                       _gx_multi_line_text_input_style_set
#define gx_multi_line_text_input_text_color_set                  _gx_multi_line_text_input_text_color_set
#define gx_multi_line_text_input_text_select                     _gx_multi_line_text_input_text_select
#define gx_multi_line_text_input_text_set_ext                    _gx_multi_line_text_input_text_set_ext
#define gx_multi_line_text_input_up_arrow                        _gx_multi_line_text_input_up_arrow

#define gx_multi_line_text_view_create(a, b, c, d, e, f, g)      _gx_multi_line_text_view_create(a, b, (GX_WIDGET *)c, d, e, f, g)
#define gx_multi_line_text_view_draw                             _gx_multi_line_text_view_draw
#define gx_multi_line_text_view_event_process                    _gx_multi_line_text_view_event_process
#define gx_multi_line_text_view_font_set                         _gx_multi_line_text_view_font_set
#define gx_multi_line_text_view_line_space_set                   _gx_multi_line_text_view_line_space_set
#define gx_multi_line_text_view_scroll_info_get                  _gx_multi_line_text_view_scroll_info_get
#define gx_multi_line_text_view_text_color_set                   _gx_multi_line_text_view_text_color_set
#define gx_multi_line_text_view_text_id_set                      _gx_multi_line_text_view_text_id_set
#define gx_multi_line_text_view_text_set_ext                     _gx_multi_line_text_view_text_set_ext
#define gx_multi_line_text_view_whitespace_set                   _gx_multi_line_text_view_whitespace_set

#define gx_numeric_pixelmap_prompt_create                        _gx_numeric_pixelmap_prompt_create
#define gx_numeric_pixelmap_prompt_format_function_set           _gx_numeric_pixelmap_prompt_format_function_set
#define gx_numeric_pixelmap_prompt_value_set                     _gx_numeric_pixelmap_prompt_value_set

#define gx_numeric_prompt_create                                 _gx_numeric_prompt_create
#define gx_numeric_prompt_format_function_set                    _gx_numeric_prompt_format_function_set
#define gx_numeric_prompt_value_set                              _gx_numeric_prompt_value_set

#define gx_numeric_scroll_wheel_create(a, b, c, d, e, f, g, h)   _gx_numeric_scroll_wheel_create(a, b, (GX_WIDGET *) c, d, e, f, g, h)
#define gx_numeric_scroll_wheel_range_set                        _gx_numeric_scroll_wheel_range_set

#define gx_pixelmap_button_create(a, b, c, d, e, f, g, h, i)     _gx_pixelmap_button_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, i)
#define gx_pixelmap_button_draw                                  _gx_pixelmap_button_draw
#define gx_pixelmap_button_event_process                         _gx_pixelmap_button_event_process
#define gx_pixelmap_button_pixelmap_set                          _gx_pixelmap_button_pixelmap_set

#define gx_pixelmap_prompt_create(a, b, c, d, e, f, g, h)        _gx_pixelmap_prompt_create(a, b, (GX_WIDGET *)c, d, e, f, g, h)
#define gx_pixelmap_prompt_draw                                  _gx_pixelmap_prompt_draw
#define gx_pixelmap_prompt_pixelmap_set                          _gx_pixelmap_prompt_pixelmap_set

#define gx_pixelmap_slider_create(a, b, c, d, e, f, g, h)        _gx_pixelmap_slider_create(a, b, (GX_WIDGET *)c, d, e, f, g, h)
#define gx_pixelmap_slider_draw                                  _gx_pixelmap_slider_draw
#define gx_pixelmap_slider_event_process                         _gx_pixelmap_slider_event_process
#define gx_pixelmap_slider_pixelmap_set                          _gx_pixelmap_slider_pixelmap_set

#define gx_progress_bar_background_draw                          _gx_progress_bar_background_draw
#define gx_progress_bar_create(a, b, c, d, e, f, g)              _gx_progress_bar_create(a, b, (GX_WIDGET *)c, d, e, f, g)
#define gx_progress_bar_draw                                     _gx_progress_bar_draw
#define gx_progress_bar_event_process                            _gx_progress_bar_event_process
#define gx_progress_bar_font_set                                 _gx_progress_bar_font_set
#define gx_progress_bar_info_set(a, b)                           _gx_progress_bar_info_set((GX_PROGRESS_BAR *)a, b)
#define gx_progress_bar_pixelmap_set                             _gx_progress_bar_pixelmap_set
#define gx_progress_bar_range_set                                _gx_progress_bar_range_set
#define gx_progress_bar_text_color_set                           _gx_progress_bar_text_color_set
#define gx_progress_bar_text_draw                                _gx_progress_bar_text_draw
#define gx_progress_bar_value_set                                _gx_progress_bar_value_set

#define gx_prompt_create(a, b, c, d, e, f, g)                    _gx_prompt_create(a, b, (GX_WIDGET *)c, d, e, f, g)
#define gx_prompt_event_process                                  _gx_prompt_event_process
#define gx_prompt_draw                                           _gx_prompt_draw
#define gx_prompt_font_set                                       _gx_prompt_font_set
#define gx_prompt_text_color_set                                 _gx_prompt_text_color_set
#define gx_prompt_text_draw                                      _gx_prompt_text_draw
#define gx_prompt_text_get_ext                                   _gx_prompt_text_get_ext
#define gx_prompt_text_id_set                                    _gx_prompt_text_id_set
#define gx_prompt_text_set_ext                                   _gx_prompt_text_set_ext

#define gx_radial_progress_bar_anchor_set                        _gx_radial_progress_bar_anchor_set
#define gx_radial_progress_bar_background_draw                   _gx_radial_progress_bar_background_draw
#define gx_radial_progress_bar_create                            _gx_radial_progress_bar_create
#define gx_radial_progress_bar_draw                              _gx_radial_progress_bar_draw
#define gx_radial_progress_bar_event_process                     _gx_radial_progress_bar_event_process
#define gx_radial_progress_bar_font_set                          _gx_radial_progress_bar_font_set
#define gx_radial_progress_bar_info_set                          _gx_radial_progress_bar_info_set
#define gx_radial_progress_bar_text_color_set                    _gx_radial_progress_bar_text_color_set
#define gx_radial_progress_bar_text_draw                         _gx_radial_progress_bar_text_draw
#define gx_radial_progress_bar_value_set                         _gx_radial_progress_bar_value_set

#define gx_radial_slider_anchor_angles_set                       _gx_radial_slider_anchor_angles_set
#define gx_radial_slider_angle_set                               _gx_radial_slider_angle_set
#define gx_radial_slider_animation_set                           _gx_radial_slider_animation_set
#define gx_radial_slider_animation_start                         _gx_radial_slider_animation_start
#define gx_radial_slider_create                                  _gx_radial_slider_create
#define gx_radial_slider_draw                                    _gx_radial_slider_draw
#define gx_radial_slider_event_process                           _gx_radial_slider_event_process
#define gx_radial_slider_info_get                                _gx_radial_slider_info_get
#define gx_radial_slider_info_set                                _gx_radial_slider_info_set
#define gx_radial_slider_pixelmap_set                            _gx_radial_slider_pixelmap_set

#define gx_radio_button_create(a, b, c, d, e, f, g)              _gx_radio_button_create(a, b, (GX_WIDGET *)c, d, e, f, g)
#define gx_radio_button_draw                                     _gx_radio_button_draw
#define gx_radio_button_pixelmap_set                             _gx_radio_button_pixelmap_set

#define gx_rich_text_view_create(a, b, c, d, e, f, g, h)         _gx_rich_text_view_create(a, b, (GX_WIDGET *)c, d, e, f, g, h)
#define gx_rich_text_view_draw                                   _gx_rich_text_view_draw
#define gx_rich_text_view_fonts_set                              _gx_rich_text_view_fonts_set
#define gx_rich_text_view_text_draw                              _gx_rich_text_view_text_draw

#define gx_scroll_thumb_create                                   _gx_scroll_thumb_create
#define gx_scroll_thumb_draw                                     _gx_scroll_thumb_draw
#define gx_scroll_thumb_event_process                            _gx_scroll_thumb_event_process

#define gx_horizontal_scrollbar_create(a, b, c, d, e)            _gx_horizontal_scrollbar_create(a, b, (GX_WINDOW *)c, d, e)
#define gx_vertical_scrollbar_create(a, b, c, d, e)              _gx_vertical_scrollbar_create(a, b, (GX_WINDOW *)c, d, e)
#define gx_scrollbar_draw                                        _gx_scrollbar_draw
#define gx_scrollbar_event_process                               _gx_scrollbar_event_process
#define gx_scrollbar_limit_check                                 _gx_scrollbar_limit_check
#define gx_scrollbar_reset                                       _gx_scrollbar_reset
#define gx_scrollbar_value_set                                   _gx_scrollbar_value_set

#define gx_scroll_wheel_create                                   _gx_scroll_wheel_create
#define gx_scroll_wheel_event_process                            _gx_scroll_wheel_event_process
#define gx_scroll_wheel_gradient_alpha_set(a, b, c)              _gx_scroll_wheel_gradient_alpha_set((GX_SCROLL_WHEEL *) a, b, c)
#define gx_scroll_wheel_row_height_set(a, b)                     _gx_scroll_wheel_row_height_set((GX_SCROLL_WHEEL *) a, b)
#define gx_scroll_wheel_selected_background_set(a, b)            _gx_scroll_wheel_selected_background_set((GX_SCROLL_WHEEL *) a, b)
#define gx_scroll_wheel_selected_get(a, b)                       _gx_scroll_wheel_selected_get((GX_SCROLL_WHEEL *) a, b)
#define gx_scroll_wheel_selected_set(a, b)                       _gx_scroll_wheel_selected_set((GX_SCROLL_WHEEL *) a, b)
#define gx_scroll_wheel_speed_set(a, b, c, d, e)                 _gx_scroll_wheel_speed_set((GX_SCROLL_WHEEL *) a, b, c, d, e)
#define gx_scroll_wheel_total_rows_set(a, b)                     _gx_scroll_wheel_total_rows_set((GX_SCROLL_WHEEL *) a, b)

#define gx_single_line_text_input_backspace(a)                   _gx_single_line_text_input_backspace((GX_SINGLE_LINE_TEXT_INPUT *)a)
#define gx_single_line_text_input_buffer_clear(a)                _gx_single_line_text_input_buffer_clear((GX_SINGLE_LINE_TEXT_INPUT *)a)
#define gx_single_line_text_input_buffer_get(a, b, c, d)         _gx_single_line_text_input_buffer_get((GX_SINGLE_LINE_TEXT_INPUT *)a, b, c, d)
#define gx_single_line_text_input_character_delete(a)            _gx_single_line_text_input_character_delete(a)
#define gx_single_line_text_input_character_insert(a, b, c)      _gx_single_line_text_input_character_insert(a, b, c)
#define gx_single_line_text_input_create(a, b, c, d, e, f, g, h) _gx_single_line_text_input_create(a, b, (GX_WIDGET *)c, d, e, f, g, h)
#define gx_single_line_text_input_draw(a)                        _gx_single_line_text_input_draw(a)
#define gx_single_line_text_input_draw_position_get(a, b, c)     _gx_single_line_text_input_draw_position_get(a, b, c)
#define gx_single_line_text_input_end(a)                         _gx_single_line_text_input_end((GX_SINGLE_LINE_TEXT_INPUT *)a)
#define gx_single_line_text_input_event_process(a, b)            _gx_single_line_text_input_event_process((GX_SINGLE_LINE_TEXT_INPUT *)a, b)
#define gx_single_line_text_input_fill_color_set(a, b, c, d, e)  _gx_single_line_text_input_fill_color_set((GX_SINGLE_LINE_TEXT_INPUT *)a, b, c, d, e)
#define gx_single_line_text_input_home(a)                        _gx_single_line_text_input_home((GX_SINGLE_LINE_TEXT_INPUT *)a)
#define gx_single_line_text_input_left_arrow(a)                  _gx_single_line_text_input_left_arrow((GX_SINGLE_LINE_TEXT_INPUT *)a)
#define gx_single_line_text_input_position_get(a, b)             _gx_single_line_text_input_position_get(a, b)
#define gx_single_line_text_input_right_arrow(a)                 _gx_single_line_text_input_right_arrow((GX_SINGLE_LINE_TEXT_INPUT *)a)
#define gx_single_line_text_input_style_add(a, b)                _gx_single_line_text_input_style_add((GX_SINGLE_LINE_TEXT_INPUT *)a, b);
#define gx_single_line_text_input_style_remove(a, b)             _gx_single_line_text_input_style_remove((GX_SINGLE_LINE_TEXT_INPUT *)a, b)
#define gx_single_line_text_input_style_set(a, b)                _gx_single_line_text_input_style_set((GX_SINGLE_LINE_TEXT_INPUT *)a, b)
#define gx_single_line_text_input_text_color_set(a, b, c, d, e)  _gx_single_line_text_input_text_color_set((GX_SINGLE_LINE_TEXT_INPUT *)a, b, c, d, e)
#define gx_single_line_text_input_text_select                    _gx_single_line_text_input_text_select
#define gx_single_line_text_input_text_set_ext                   _gx_single_line_text_input_text_set_ext

#define gx_slider_create(a, b, c, d, e, f, g, h)                 _gx_slider_create(a, b, (GX_WIDGET *)c, d, e, f, g, h)
#define gx_slider_draw                                           _gx_slider_draw
#define gx_slider_event_process                                  _gx_slider_event_process
#define gx_slider_info_set                                       _gx_slider_info_set
#define gx_slider_needle_draw                                    _gx_slider_needle_draw
#define gx_slider_needle_position_get                            _gx_slider_needle_position_get
#define gx_slider_tickmarks_draw                                 _gx_slider_tickmarks_draw
#define gx_slider_travel_get                                     _gx_slider_travel_get
#define gx_slider_value_calculate                                _gx_slider_value_calculate
#define gx_slider_value_set                                      _gx_slider_value_set

#define gx_sprite_create(a, b, c, d, e, f, g, h)                 _gx_sprite_create(a, b, (GX_WIDGET *)c, d, e, f, g, h)
#define gx_sprite_current_frame_set                              _gx_sprite_current_frame_set
#define gx_sprite_frame_list_set                                 _gx_sprite_frame_list_set
#define gx_sprite_start(a, b)                                    _gx_sprite_start(a, b)
#define gx_sprite_stop(a)                                        _gx_sprite_stop(a)

#define gx_string_scroll_wheel_create_ext(a, b, c, d, e, f, g, h) _gx_string_scroll_wheel_create_ext(a, b, c, d, e, f, g, h)
#define gx_string_scroll_wheel_event_process                     _gx_string_scroll_wheel_event_process
#define gx_string_scroll_wheel_string_id_list_set                _gx_string_scroll_wheel_string_id_list_set
#define gx_string_scroll_wheel_string_list_set_ext               _gx_string_scroll_wheel_string_list_set_ext

#define gx_system_active_language_set                            _gx_system_active_language_set
#if (GX_ANIMATION_POOL_SIZE > 0)
#define gx_system_animation_get                                  _gx_system_animation_get
#define gx_system_animation_free                                 _gx_system_animation_free
#endif
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
#define gx_system_bidi_text_enable                               _gx_system_bidi_text_enable
#define gx_system_bidi_text_disable                              _gx_system_bidi_text_disable
#endif
#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
#define gx_system_text_render_style_set                          _gx_system_text_render_style_set
#endif
#define gx_system_canvas_refresh                                 _gx_system_canvas_refresh
#define gx_system_dirty_mark(a)                                  _gx_system_dirty_mark((GX_WIDGET *)a);
#define gx_system_dirty_partial_add(a, b)                        _gx_system_dirty_partial_add((GX_WIDGET *)a, b)
#define gx_system_draw_context_get                               _gx_system_draw_context_get
#define gx_system_event_fold                                     _gx_system_event_fold
#define gx_system_event_send                                     _gx_system_event_send
#define gx_system_focus_claim(a)                                 _gx_system_focus_claim((GX_WIDGET *)a)
#define gx_system_initialize                                     _gx_system_initialize
#define gx_system_memory_allocator_set                           _gx_system_memory_allocator_set
#define gx_system_pen_configure                                  _gx_system_pen_configure
#define gx_system_screen_stack_create                            _gx_system_screen_stack_create
#define gx_system_screen_stack_get                               _gx_system_screen_stack_get
#define gx_system_screen_stack_pop                               _gx_system_screen_stack_pop
#define gx_system_screen_stack_push                              _gx_system_screen_stack_push
#define gx_system_screen_stack_reset                             _gx_system_screen_stack_reset
#define gx_system_scroll_appearance_get                          _gx_system_scroll_appearance_get
#define gx_system_scroll_appearance_set                          _gx_system_scroll_appearance_set
#define gx_system_start                                          _gx_system_start
#define gx_system_string_width_get_ext                           _gx_system_string_width_get_ext
#define gx_system_timer_start(a, b, c, d)                        _gx_system_timer_start((GX_WIDGET *)a, b, c, d)
#define gx_system_timer_stop(a, b)                               _gx_system_timer_stop((GX_WIDGET *)a, b)
#define gx_system_version_string_get_ext                         _gx_system_version_string_get_ext
#define gx_system_widget_find                                    _gx_system_widget_find

#define gx_text_button_create(a, b, c, d, e, f, g)               _gx_text_button_create(a, b, (GX_WIDGET *)c, d, e, f, g)
#define gx_text_button_draw                                      _gx_text_button_draw
#define gx_text_button_event_process                             _gx_text_button_event_process
#define gx_text_button_font_set                                  _gx_text_button_font_set
#define gx_text_button_text_color_set                            _gx_text_button_text_color_set
#define gx_text_button_text_draw                                 _gx_text_button_text_draw
#define gx_text_button_text_get_ext                              _gx_text_button_text_get_ext
#define gx_text_button_text_id_set                               _gx_text_button_text_id_set
#define gx_text_button_text_set_ext                              _gx_text_button_text_set_ext

#define gx_text_input_cursor_blink_interval_set                  _gx_text_input_cursor_blink_interval_set
#define gx_text_input_cursor_height_set                          _gx_text_input_cursor_height_set
#define gx_text_input_cursor_width_set                           _gx_text_input_cursor_width_set

#define gx_text_scroll_wheel_callback_set_ext(a, b)              _gx_text_scroll_wheel_callback_set_ext((GX_TEXT_SCROLL_WHEEL *)a, (UINT(*)(GX_TEXT_SCROLL_WHEEL *, INT, GX_STRING *)) b)
#define gx_text_scroll_wheel_font_set(a, b, c)                   _gx_text_scroll_wheel_font_set((GX_TEXT_SCROLL_WHEEL *) a, b, c)
#define gx_text_scroll_wheel_text_color_set(a, b, c, d)          _gx_text_scroll_wheel_text_color_set((GX_TEXT_SCROLL_WHEEL *) a, b, c, d)
#define gx_text_scroll_wheel_create                              _gx_text_scroll_wheel_create
#define gx_text_scroll_wheel_draw                                _gx_text_scroll_wheel_draw
#define gx_text_scroll_wheel_event_process                       _gx_text_scroll_wheel_event_process

#define gx_tree_view_create(a, b, c, d, e, f)                    _gx_tree_view_create((GX_TREE_VIEW *)a, b, (GX_WIDGET *)c, d, e, f)
#define gx_tree_view_draw                                        _gx_tree_view_draw
#define gx_tree_view_event_process                               _gx_tree_view_event_process
#define gx_tree_view_indentation_set                             _gx_tree_view_indentation_set
#define gx_tree_view_position                                    _gx_tree_view_position
#define gx_tree_view_root_line_color_set                         _gx_tree_view_root_line_color_set
#define gx_tree_view_root_pixelmap_set                           _gx_tree_view_root_pixelmap_set
#define gx_tree_view_selected_get                                _gx_tree_view_selected_get
#define gx_tree_view_selected_set                                _gx_tree_view_selected_set

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
#define gx_utility_bidi_paragraph_reorder                        _gx_utility_bidi_paragraph_reorder
#define gx_utility_bidi_paragraph_reorder_ext                    _gx_utility_bidi_paragraph_reorder_ext
#define gx_utility_bidi_resolved_text_info_delete                _gx_utility_bidi_resolved_text_info_delete
#endif
#define gx_utility_canvas_to_bmp                                 _gx_utility_canvas_to_bmp
#define gx_utility_circle_point_get                              _gx_utility_circle_point_get
#define gx_utility_gradient_create                               _gx_system_gradient_create
#define gx_utility_gradient_delete                               _gx_system_gradient_delete
#define gx_utility_ltoa                                          _gx_utility_ltoa
#define gx_utility_math_acos                                     _gx_utility_math_acos
#define gx_utility_math_asin                                     _gx_utility_math_asin
#define gx_utility_math_cos                                      _gx_utility_math_cos
#define gx_utility_math_sin                                      _gx_utility_math_sin
#define gx_utility_math_sqrt                                     _gx_utility_math_sqrt
#define gx_utility_pixelmap_resize                               _gx_utility_pixelmap_resize
#define gx_utility_pixelmap_rotate                               _gx_utility_pixelmap_rotate
#define gx_utility_pixelmap_simple_rotate                        _gx_utility_pixelmap_simple_rotate
#define gx_utility_rectangle_center                              _gx_utility_rectangle_center
#define gx_utility_rectangle_center_find                         _gx_utility_rectangle_center_find
#define gx_utility_rectangle_combine                             _gx_utility_rectangle_combine
#define gx_utility_rectangle_compare                             _gx_utility_rectangle_compare
#define gx_utility_rectangle_define                              _gx_utility_rectangle_define
#define gx_utility_rectangle_resize                              _gx_utility_rectangle_resize
#define gx_utility_rectangle_overlap_detect                      _gx_utility_rectangle_overlap_detect
#define gx_utility_rectangle_point_detect                        _gx_utility_rectangle_point_detect
#define gx_utility_rectangle_shift                               _gx_utility_rectangle_shift
#define gx_utility_string_to_alphamap_ext                        _gx_utility_string_to_alphamap_ext

#define gx_vertical_list_children_position                       _gx_vertical_list_children_position
#define gx_vertical_list_create(a, b, c, d, e, f, g, h)          _gx_vertical_list_create(a, b, (GX_WIDGET *)c, d, e, f, g, h)
#define gx_vertical_list_event_process                           _gx_vertical_list_event_process
#define gx_vertical_list_page_index_set(a, b)                    _gx_vertical_list_page_index_set((GX_VERTICAL_LIST *)a, b)
#define gx_vertical_list_selected_index_get(a, b)                _gx_vertical_list_selected_index_get(a, b)
#define gx_vertical_list_selected_set(a, b)                      _gx_vertical_list_selected_set(a, b)
#define gx_vertical_list_selected_widget_get(a, b)               _gx_vertical_list_selected_widget_get(a, (GX_WIDGET **)b)
#define gx_vertical_list_total_rows_set(a, b)                    _gx_vertical_list_total_rows_set((GX_VERTICAL_LIST *)a, b)

#define gx_widget_allocate(a, b)                                 _gx_widget_allocate((GX_WIDGET **)a, b)
#define gx_widget_attach(a, b)                                   _gx_widget_attach((GX_WIDGET *)a, (GX_WIDGET *)b)
#define gx_widget_back_attach(a, b)                              _gx_widget_back_attach((GX_WIDGET *)a, (GX_WIDGET *)b)
#define gx_widget_back_move(a, b)                                _gx_widget_back_move((GX_WIDGET *)a, b)
#define gx_widget_background_draw(a)                             _gx_widget_background_draw((GX_WIDGET *)a)
#define gx_widget_block_move(a, b, c, d)                         _gx_widget_block_move((GX_WIDGET *)a, b, c, d)
#define gx_widget_border_draw(a, b, c, d, e)                     _gx_widget_border_draw((GX_WIDGET *)a, b, c, d, e)
#define gx_widget_border_style_set(a, b)                         _gx_widget_border_style_set((GX_WIDGET *)a, b)
#define gx_widget_border_width_get(a, b)                         _gx_widget_border_width_get((GX_WIDGET *)a, b)
#define gx_widget_canvas_get(a, b)                               _gx_widget_canvas_get((GX_WIDGET *)a, b)
#define gx_widget_child_detect(a, b, c)                          _gx_widget_child_detect((GX_WIDGET *)a, b, c)
#define gx_widget_children_draw(a)                               _gx_widget_children_draw((GX_WIDGET *)a)
#define gx_widget_client_get(a, b, c)                            _gx_widget_client_get((GX_WIDGET *)a, b, c)
#define gx_widget_color_get(a, b, c)                             _gx_widget_color_get((GX_WIDGET *)a, b, c)
#define gx_widget_create(a, b, c, d, e, f)                       _gx_widget_create((GX_WIDGET *)a, b, (GX_WIDGET *)c, d, e, f)
#define gx_widget_created_test(a, b)                             _gx_widget_created_test((GX_WIDGET *)a, b)
#define gx_widget_delete(a)                                      _gx_widget_delete((GX_WIDGET *)a)
#define gx_widget_detach(a)                                      _gx_widget_detach((GX_WIDGET *)a)
#define gx_widget_draw(a)                                        _gx_widget_draw((GX_WIDGET *)a)
#define gx_widget_draw_set(a, b)                                 _gx_widget_draw_set((GX_WIDGET *)a, (void (*)(GX_WIDGET *))b)
#define gx_widget_event_generate(a, b, c)                        _gx_widget_event_generate((GX_WIDGET *)a, b, c)
#define gx_widget_event_process(a, b)                            _gx_widget_event_process((GX_WIDGET *)a, b)
#define gx_widget_event_process_set(a, b)                        _gx_widget_event_process_set((GX_WIDGET *)a, (UINT (*)(GX_WIDGET *, GX_EVENT *)) b)
#define gx_widget_event_to_parent(a, b)                          _gx_widget_event_to_parent((GX_WIDGET *)a, b)
#define gx_widget_fill_color_set(a, b, c, d)                     _gx_widget_fill_color_set((GX_WIDGET *)a, b, c, d)
#define gx_widget_find(a, b, c, d)                               _gx_widget_find((GX_WIDGET *)a, b, c, (GX_WIDGET **)d)
#define gx_widget_first_child_get(a, b)                          _gx_widget_first_child_get((GX_WIDGET *) a, (GX_WIDGET **) b)
#define gx_widget_focus_next(a)                                  _gx_widget_focus_next((GX_WIDGET *)a)
#define gx_widget_focus_previous(a)                              _gx_widget_focus_previous((GX_WIDGET *)a)
#define gx_widget_font_get(a, b, c)                              _gx_widget_font_get((GX_WIDGET *)a, b, c)
#define gx_widget_free(a)                                        _gx_widget_free((GX_WIDGET *)a)
#define gx_widget_front_move(a, b)                               _gx_widget_front_move((GX_WIDGET *)a, b)
#define gx_widget_height_get(a, b)                               _gx_widget_height_get((GX_WIDGET *)a, b)
#define gx_widget_hide(a)                                        _gx_widget_hide((GX_WIDGET *)a)
#define gx_widget_last_child_get(a, b)                           _gx_widget_last_child_get((GX_WIDGET *) a, (GX_WIDGET **) b)
#define gx_widget_next_sibling_get(a, b)                         _gx_widget_next_sibling_get((GX_WIDGET *) a, (GX_WIDGET **) b)
#define gx_widget_parent_get(a, b)                               _gx_widget_parent_get((GX_WIDGET *) a, (GX_WIDGET **) b)
#define gx_widget_pixelmap_get(a, b, c)                          _gx_widget_pixelmap_get((GX_WIDGET *)a, b, c)
#define gx_widget_previous_sibling_get(a, b)                     _gx_widget_previous_sibling_get((GX_WIDGET *) a, (GX_WIDGET **) b)
#define gx_widget_resize(a, b)                                   _gx_widget_resize((GX_WIDGET *)a, b)
#define gx_widget_shift(a, b, c, d)                              _gx_widget_shift((GX_WIDGET *)a, b, c, d)
#define gx_widget_show(a)                                        _gx_widget_show((GX_WIDGET *)a)
#define gx_widget_status_add(a, b)                               _gx_widget_status_add((GX_WIDGET *)a, b)
#define gx_widget_status_get(a, b)                               _gx_widget_status_get((GX_WIDGET *)a, b)
#define gx_widget_status_remove(a, b)                            _gx_widget_status_remove((GX_WIDGET *)a, b)
#define gx_widget_status_test(a, b, c)                           _gx_widget_status_test((GX_WIDGET *)a, b, c)
#define gx_widget_string_get_ext(a, b, c)                        _gx_widget_string_get_ext((GX_WIDGET *)a, b, c)
#define gx_widget_style_add(a, b)                                _gx_widget_style_add((GX_WIDGET *)a, b)
#define gx_widget_style_get(a, b)                                _gx_widget_style_get((GX_WIDGET *)a, b)
#define gx_widget_style_remove(a, b)                             _gx_widget_style_remove((GX_WIDGET *)a, b)
#define gx_widget_style_set(a, b)                                _gx_widget_style_set((GX_WIDGET *)a, b)
#define gx_widget_text_blend_ext(a, b, c, d, e, f, g)            _gx_widget_text_blend_ext((GX_WIDGET *)a, b, c, d, e, f, g)
#define gx_widget_text_draw_ext(a, b, c, d, e, f)                _gx_widget_text_draw_ext((GX_WIDGET *)a, b, c, d, e, f)
#define gx_widget_text_id_draw(a, b, c, d, e, f)                 _gx_widget_text_id_draw((GX_WIDGET *)a, b, c, d, e, f)
#define gx_widget_top_visible_child_find(a, b)                   _gx_widget_top_visible_child_find((GX_WIDGET *) a, (GX_WIDGET **) b)
#define gx_widget_type_find(a, b, c)                             _gx_widget_type_find((GX_WIDGET *)a, b, (GX_WIDGET **)c)
#define gx_widget_width_get(a, b)                                _gx_widget_width_get((GX_WIDGET *)a, b)

#define gx_window_background_draw                                _gx_window_background_draw
#define gx_window_client_height_get                              _gx_window_client_height_get
#define gx_window_client_scroll                                  _gx_window_client_scroll
#define gx_window_client_width_get                               _gx_window_client_width_get
#define gx_window_close(a)                                       _gx_window_close((GX_WINDOW *) a)
#define gx_window_create(a, b, c, d, e, f)                       _gx_window_create(a, b, (GX_WIDGET *)c, d, e, f)
#define gx_window_draw                                           _gx_window_draw
#define gx_window_event_process                                  _gx_window_event_process
#define gx_window_execute                                        _gx_window_execute
#define gx_window_scroll_info_get                                _gx_window_scroll_info_get
#define gx_window_scrollbar_find                                 _gx_window_scrollbar_find
#define gx_window_wallpaper_get                                  _gx_window_wallpaper_get
#define gx_window_wallpaper_set(a, b, c)                         _gx_window_wallpaper_set((GX_WINDOW *)a, b, c)

#define gx_window_root_create                                    _gx_window_root_create
#define gx_window_root_delete                                    _gx_window_root_delete
#define gx_window_root_event_process                             _gx_window_root_event_process
#define gx_window_root_find(a, b)                                _gx_window_root_find((GX_WIDGET *)a, b)

#else /* !GX_DISABLE_ERROR_CHECKING */

#define gx_accordion_menu_create(a, b, c, d, e, f)               _gxe_accordion_menu_create(a, b, c, d, e, f, sizeof(GX_ACCORDION_MENU))
#define gx_accordion_menu_draw                                   _gx_accordion_menu_draw
#define gx_accordion_menu_event_process                          _gxe_accordion_menu_event_process
#define gx_accordion_menu_position                               _gxe_accordion_menu_position

#define gx_animation_canvas_define                               _gxe_animation_canvas_define
#define gx_animation_create                                      _gxe_animation_create
#define gx_animation_delete                                      _gxe_animation_delete
#define gx_animation_drag_disable                                _gxe_animation_drag_disable
#define gx_animation_drag_enable                                 _gxe_animation_drag_enable
#define gx_animation_landing_speed_set                           _gxe_animation_landing_speed_set
#define gx_animation_start                                       _gxe_animation_start
#define gx_animation_stop                                        _gxe_animation_stop

#define gx_binres_language_count_get                             _gxe_binres_language_count_get
#define gx_binres_language_info_load                             _gxe_binres_language_info_load
#define gx_binres_language_table_load_ext                        _gxe_binres_language_table_load_ext
#define gx_binres_theme_load                                     _gxe_binres_theme_load
#define gx_binres_pixelmap_load                                  _gxe_binres_pixelmap_load
#define gx_binres_font_load                                      _gxe_binres_font_load

#define gx_brush_default                                         _gxe_brush_default
#define gx_brush_define                                          _gxe_brush_define

#define gx_button_background_draw                                _gx_button_background_draw
#define gx_button_create(a, b, c, d, e, f)                       _gxe_button_create(a, b, (GX_WIDGET *)c, d, e, f, sizeof(GX_BUTTON))
#define gx_button_deselect                                       _gxe_button_deselect
#define gx_button_draw                                           _gx_button_draw
#define gx_button_event_process                                  _gxe_button_event_process
#define gx_button_select                                         _gxe_button_select

#define gx_canvas_alpha_set                                      _gxe_canvas_alpha_set
#define gx_canvas_arc_draw                                       _gxe_canvas_arc_draw
#define gx_canvas_block_move(a, b, c, d)                         _gxe_canvas_block_move(a, b, c, d)
#define gx_canvas_circle_draw                                    _gxe_canvas_circle_draw
#define gx_canvas_create(a, b, c, d, e, f, g, h)                 _gxe_canvas_create(a, b, c, d, e, f, g, h, sizeof(GX_CANVAS))
#define gx_canvas_delete                                         _gxe_canvas_delete
#define gx_canvas_drawing_complete                               _gxe_canvas_drawing_complete
#define gx_canvas_drawing_initiate(a, b, c)                      _gxe_canvas_drawing_initiate(a, (GX_WIDGET *)b, c)
#define gx_canvas_ellipse_draw                                   _gxe_canvas_ellipse_draw
#define gx_canvas_hardware_layer_bind                            _gxe_canvas_hardware_layer_bind
#define gx_canvas_hide                                           _gxe_canvas_hide
#define gx_canvas_line_draw                                      _gxe_canvas_line_draw
#define gx_canvas_memory_define                                  _gxe_canvas_memory_define
#if defined (GX_MOUSE_SUPPORT)
#define gx_canvas_mouse_define                                   _gxe_canvas_mouse_define
#define gx_canvas_mouse_hide                                     _gxe_canvas_mouse_hide
#define gx_canvas_mouse_show                                     _gxe_canvas_mouse_show
#endif
#define gx_canvas_offset_set                                     _gxe_canvas_offset_set
#define gx_canvas_pie_draw                                       _gxe_canvas_pie_draw
#define gx_canvas_pixel_draw                                     _gxe_canvas_pixel_draw
#define gx_canvas_pixelmap_blend                                 _gxe_canvas_pixelmap_blend
#define gx_canvas_pixelmap_draw                                  _gxe_canvas_pixelmap_draw
#define gx_canvas_pixelmap_get                                   _gxe_canvas_pixelmap_get
#define gx_canvas_pixelmap_rotate                                _gxe_canvas_pixelmap_rotate
#define gx_canvas_pixelmap_tile                                  _gxe_canvas_pixelmap_tile
#define gx_canvas_polygon_draw                                   _gxe_canvas_polygon_draw
#define gx_canvas_rectangle_draw                                 _gxe_canvas_rectangle_draw
#define gx_canvas_rotated_text_draw_ext                          _gxe_canvas_rotated_text_draw_ext
#define gx_canvas_shift                                          _gxe_canvas_shift
#define gx_canvas_show                                           _gxe_canvas_show
#define gx_canvas_aligned_text_draw                              _gxe_canvas_aligned_text_draw
#define gx_canvas_text_draw_ext                                  _gxe_canvas_text_draw_ext

#define gx_checkbox_create(a, b, c, d, e, f, g)                  _gxe_checkbox_create(a, b, (GX_WIDGET *)c, d, e, f, g, sizeof(GX_CHECKBOX))
#define gx_checkbox_draw                                         _gx_checkbox_draw
#define gx_checkbox_event_process                                _gxe_checkbox_event_process
#define gx_checkbox_pixelmap_set                                 _gxe_checkbox_pixelmap_set
#define gx_checkbox_select                                       _gxe_checkbox_select

#define gx_circular_gauge_angle_get                              _gxe_circular_gauge_angle_get
#define gx_circular_gauge_angle_set                              _gxe_circular_gauge_angle_set
#define gx_circular_gauge_animation_set                          _gxe_circular_gauge_animation_set
#define gx_circular_gauge_background_draw                        _gx_circular_gauge_background_draw
#define gx_circular_gauge_create(a, b, c, d, e, f, g, h, i)      _gxe_circular_gauge_create((GX_CIRCULAR_GAUGE *) a, b, c, d, e, f, g, h, i, sizeof(GX_CIRCULAR_GAUGE))
#define gx_circular_gauge_draw                                   _gx_circular_gauge_draw
#define gx_circular_gauge_event_process                          _gxe_circular_gauge_event_process

#define gx_context_brush_default                                 _gxe_context_brush_default
#define gx_context_brush_define                                  _gxe_context_brush_define
#define gx_context_brush_get                                     _gxe_context_brush_get
#define gx_context_brush_pattern_set                             _gxe_context_brush_pattern_set
#define gx_context_brush_set                                     _gxe_context_brush_set
#define gx_context_brush_style_set                               _gxe_context_brush_style_set
#define gx_context_brush_width_set                               _gxe_context_brush_width_set
#define gx_context_color_get                                     _gxe_context_color_get
#define gx_context_fill_color_set                                _gxe_context_fill_color_set
#define gx_context_font_get                                      _gxe_context_font_get
#define gx_context_font_set                                      _gxe_context_font_set
#define gx_context_line_color_set                                _gxe_context_line_color_set
#define gx_context_pixelmap_get                                  _gxe_context_pixelmap_get
#define gx_context_pixelmap_set                                  _gxe_context_pixelmap_set
#define gx_context_raw_brush_define                              _gxe_context_raw_brush_define
#define gx_context_raw_fill_color_set                            _gxe_context_raw_fill_color_set
#define gx_context_raw_line_color_set                            _gxe_context_raw_line_color_set
#define gx_context_string_get_ext                                _gxe_context_string_get_ext

#define gx_display_active_language_set                           _gxe_display_active_language_set
#define gx_display_color_set                                     _gxe_display_color_set
#define gx_display_color_table_set                               _gxe_display_color_table_set
#define gx_display_create(a, b, c, d, e)                         _gxe_display_create(a, b, c, d, e, sizeof(GX_DISPLAY))
#define gx_display_delete                                        _gxe_display_delete
#define gx_display_font_table_set                                _gxe_display_font_table_set
#define gx_display_language_table_get_ext                        _gxe_display_language_table_get_ext
#define gx_display_language_table_set_ext                        _gxe_display_language_table_set_ext
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
#define gx_display_language_direction_table_set                  _gxe_display_language_direction_table_set
#endif
#define gx_display_pixelmap_table_set                            _gxe_display_pixelmap_table_set
#define gx_display_string_get_ext                                _gxe_display_string_get_ext
#define gx_display_string_table_get_ext                          _gxe_display_string_table_get_ext
#define gx_display_theme_install                                 _gxe_display_theme_install

#define gx_drop_list_close                                       _gxe_drop_list_close
#define gx_drop_list_create(a, b, c, d, e, f, g, h, i)           _gxe_drop_list_create((GX_DROP_LIST *)a, b, (GX_WIDGET *)c, d, e, f, g, h, i, sizeof(GX_DROP_LIST))
#define gx_drop_list_event_process                               _gxe_drop_list_event_process
#define gx_drop_list_open                                        _gxe_drop_list_open
#define gx_drop_list_pixelmap_set                                _gxe_drop_list_pixelmap_set
#define gx_drop_list_popup_get                                   _gxe_drop_list_popup_get

#define gx_generic_scroll_wheel_children_position                _gxe_generic_scroll_wheel_children_position
#define gx_generic_scroll_wheel_create(a, b, c, d, e, f, g, h)   _gxe_generic_scroll_wheel_create(a, b, c, d, e, f, g, h, sizeof(GX_GENERIC_SCROLL_WHEEL))
#define gx_generic_scroll_wheel_draw                             _gxe_generic_scroll_wheel_draw
#define gx_generic_scroll_wheel_event_process                    _gxe_generic_scroll_wheel_event_process
#define gx_generic_scroll_wheel_row_height_set                   _gxe_generic_scroll_wheel_row_height_set
#define gx_generic_scroll_wheel_total_rows_set                   _gxe_generic_scroll_wheel_total_rows_set

#define gx_horizontal_list_children_position                     _gxe_horizontal_list_children_position
#define gx_horizontal_list_create(a, b, c, d, e, f, g, h)        _gxe_horizontal_list_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, sizeof(GX_HORIZONTAL_LIST))
#define gx_horizontal_list_event_process                         _gxe_horizontal_list_event_process
#define gx_horizontal_list_page_index_set(a, b)                  _gxe_horizontal_list_page_index_set((GX_HORIZONTAL_LIST *)a, b)
#define gx_horizontal_list_selected_index_get(a, b)              _gxe_horizontal_list_selected_index_get(a, b)
#define gx_horizontal_list_selected_set(a, b)                    _gxe_horizontal_list_selected_set(a, b)
#define gx_horizontal_list_selected_widget_get(a, b)             _gxe_horizontal_list_selected_widget_get(a, (GX_WIDGET **)b)
#define gx_horizontal_list_total_columns_set(a, b)               _gxe_horizontal_list_total_columns_set((GX_HORIZONTAL_LIST *)a, b)

#define gx_icon_background_draw                                  _gx_icon_background_draw
#define gx_icon_create(a, b, c, d, e, f, g, h)                   _gxe_icon_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, sizeof(GX_ICON))
#define gx_icon_draw                                             _gx_icon_draw
#define gx_icon_event_process                                    _gxe_icon_event_process
#define gx_icon_pixelmap_set                                     _gxe_icon_pixelmap_set

#define gx_icon_button_create(a, b, c, d, e, f, g)               _gxe_icon_button_create(a, b, (GX_WIDGET *)c, d, e, f, g, sizeof(GX_ICON_BUTTON))
#define gx_icon_button_draw                                      _gx_icon_button_draw
#define gx_icon_button_pixelmap_set                              _gxe_icon_button_pixelmap_set

#define gx_image_reader_create                                   _gxe_image_reader_create
#define gx_image_reader_palette_set                              _gxe_image_reader_palette_set
#define gx_image_reader_start                                    _gxe_image_reader_start

#define gx_line_chart_axis_draw                                  _gx_line_chart_axis_draw
#define gx_line_chart_create(a, b, c, d, e, f, g)                _gxe_line_chart_create(a, b, (GX_WIDGET *) c, d, e, f, g, sizeof(GX_LINE_CHART))
#define gx_line_chart_data_draw                                  _gx_line_chart_data_draw
#define gx_line_chart_draw                                       _gx_line_chart_draw
#define gx_line_chart_update                                     _gxe_line_chart_update
#define gx_line_chart_y_scale_calculate                          _gxe_line_chart_y_scale_calculate

#define gx_menu_create(a, b, c, d, e, f, g, h)                   _gxe_menu_create((GX_MENU *)a, b, (GX_WIDGET *)c, d, e, f, g, h, sizeof(GX_MENU))
#define gx_menu_draw                                             _gx_menu_draw
#define gx_menu_event_process                                    _gxe_menu_event_process
#define gx_menu_insert                                           _gxe_menu_insert
#define gx_menu_remove                                           _gxe_menu_remove
#define gx_menu_text_draw                                        _gx_menu_text_draw
#define gx_menu_text_offset_set                                  _gxe_menu_text_offset_set

#define gx_multi_line_text_button_create(a, b, c, d, e, f, g)    _gxe_multi_line_text_button_create(a, b, (GX_WIDGET *)c, d, e, f, g, sizeof(GX_MULTI_LINE_TEXT_BUTTON))
#define gx_multi_line_text_button_draw                           _gx_multi_line_text_button_draw
#define gx_multi_line_text_button_event_process                  _gxe_multi_line_text_button_event_process
#define gx_multi_line_text_button_text_draw                      _gx_multi_line_text_button_text_draw
#define gx_multi_line_text_button_text_id_set                    _gxe_multi_line_text_button_text_id_set
#define gx_multi_line_text_button_text_set_ext                   _gxe_multi_line_text_button_text_set_ext

#define gx_multi_line_text_input_backspace                       _gxe_multi_line_text_input_backspace
#define gx_multi_line_text_input_buffer_clear                    _gxe_multi_line_text_input_buffer_clear
#define gx_multi_line_text_input_buffer_get                      _gxe_multi_line_text_input_buffer_get
#define gx_multi_line_text_input_char_insert_ext                 _gxe_multi_line_text_input_char_insert_ext
#define gx_multi_line_text_input_create(a, b, c, d, e, f, g, h)  _gxe_multi_line_text_input_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, sizeof(GX_MULTI_LINE_TEXT_INPUT))
#define gx_multi_line_text_input_cursor_pos_get                  _gxe_multi_line_text_input_cursor_pos_get
#define gx_multi_line_text_input_delete                          _gxe_multi_line_text_input_delete
#define gx_multi_line_text_input_down_arrow                      _gxe_multi_line_text_input_down_arrow
#define gx_multi_line_text_input_end                             _gxe_multi_line_text_input_end
#define gx_multi_line_text_input_event_process                   _gxe_multi_line_text_input_event_process
#define gx_multi_line_text_input_fill_color_set                  _gxe_multi_line_text_input_fill_color_set
#define gx_multi_line_text_input_home                            _gxe_multi_line_text_input_home
#define gx_multi_line_text_input_left_arrow                      _gxe_multi_line_text_input_left_arrow
#define gx_multi_line_text_input_right_arrow                     _gxe_multi_line_text_input_right_arrow
#define gx_multi_line_text_input_style_add                       _gxe_multi_line_text_input_style_add
#define gx_multi_line_text_input_style_remove                    _gxe_multi_line_text_input_style_remove
#define gx_multi_line_text_input_style_set                       _gxe_multi_line_text_input_style_set
#define gx_multi_line_text_input_text_color_set                  _gxe_multi_line_text_input_text_color_set
#define gx_multi_line_text_input_text_select                     _gxe_multi_line_text_input_text_select
#define gx_multi_line_text_input_text_set_ext                    _gxe_multi_line_text_input_text_set_ext
#define gx_multi_line_text_input_up_arrow                        _gxe_multi_line_text_input_up_arrow

#define gx_multi_line_text_view_create(a, b, c, d, e, f, g)      _gxe_multi_line_text_view_create(a, b, (GX_WIDGET *)c, d, e, f, g, sizeof(GX_MULTI_LINE_TEXT_VIEW))
#define gx_multi_line_text_view_draw                             _gx_multi_line_text_view_draw
#define gx_multi_line_text_view_event_process                    _gxe_multi_line_text_view_event_process
#define gx_multi_line_text_view_font_set                         _gxe_multi_line_text_view_font_set
#define gx_multi_line_text_view_line_space_set                   _gxe_multi_line_text_view_line_space_set
#define gx_multi_line_text_view_scroll_info_get                  _gxe_multi_line_text_view_scroll_info_get
#define gx_multi_line_text_view_text_color_set                   _gxe_multi_line_text_view_text_color_set
#define gx_multi_line_text_view_text_id_set                      _gxe_multi_line_text_view_text_id_set
#define gx_multi_line_text_view_text_set_ext                     _gxe_multi_line_text_view_text_set_ext
#define gx_multi_line_text_view_whitespace_set                   _gxe_multi_line_text_view_whitespace_set

#define gx_numeric_pixelmap_prompt_create(a, b, c, d, e, f, g, h) _gxe_numeric_pixelmap_prompt_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, sizeof(GX_NUMERIC_PIXELMAP_PROMPT))
#define gx_numeric_pixelmap_prompt_format_function_set           _gxe_numeric_pixelmap_prompt_format_function_set
#define gx_numeric_pixelmap_prompt_value_set                     _gxe_numeric_pixelmap_prompt_value_set

#define gx_numeric_prompt_create(a, b, c, d, e, f, g)            _gxe_numeric_prompt_create(a, b, (GX_WIDGET *)c, d, e, f, g, sizeof(GX_NUMERIC_PROMPT))
#define gx_numeric_prompt_format_function_set                    _gxe_numeric_prompt_format_function_set
#define gx_numeric_prompt_value_set                              _gxe_numeric_prompt_value_set

#define gx_numeric_scroll_wheel_create(a, b, c, d, e, f, g, h)   _gxe_numeric_scroll_wheel_create(a, b, (GX_WIDGET *) c, d, e, f, g, h, sizeof(GX_NUMERIC_SCROLL_WHEEL))
#define gx_numeric_scroll_wheel_range_set                        _gxe_numeric_scroll_wheel_range_set

#define gx_pixelmap_button_create(a, b, c, d, e, f, g, h, i)     _gxe_pixelmap_button_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, i, sizeof(GX_PIXELMAP_BUTTON))
#define gx_pixelmap_button_draw                                  _gx_pixelmap_button_draw
#define gx_pixelmap_button_event_process                         _gxe_pixelmap_button_event_process
#define gx_pixelmap_button_pixelmap_set                          _gxe_pixelmap_button_pixelmap_set

#define gx_pixelmap_prompt_create(a, b, c, d, e, f, g, h)        _gxe_pixelmap_prompt_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, sizeof(GX_PIXELMAP_PROMPT))
#define gx_pixelmap_prompt_draw                                  _gx_pixelmap_prompt_draw
#define gx_pixelmap_prompt_pixelmap_set                          _gxe_pixelmap_prompt_pixelmap_set

#define gx_pixelmap_slider_create(a, b, c, d, e, f, g, h)        _gxe_pixelmap_slider_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, sizeof(GX_PIXELMAP_SLIDER))
#define gx_pixelmap_slider_draw                                  _gx_pixelmap_slider_draw
#define gx_pixelmap_slider_event_process                         _gxe_pixelmap_slider_event_process
#define gx_pixelmap_slider_pixelmap_set                          _gxe_pixelmap_slider_pixelmap_set

#define gx_progress_bar_background_draw                          _gx_progress_bar_background_draw
#define gx_progress_bar_create(a, b, c, d, e, f, g)              _gxe_progress_bar_create(a, b, (GX_WIDGET *)c, d, e, f, g, sizeof(GX_PROGRESS_BAR))
#define gx_progress_bar_draw                                     _gx_progress_bar_draw
#define gx_progress_bar_event_process                            _gxe_progress_bar_event_process
#define gx_progress_bar_font_set                                 _gxe_progress_bar_font_set
#define gx_progress_bar_info_set(a, b)                           _gxe_progress_bar_info_set((GX_PROGRESS_BAR *)a, b);
#define gx_progress_bar_pixelmap_set                             _gxe_progress_bar_pixelmap_set
#define gx_progress_bar_range_set                                _gxe_progress_bar_range_set
#define gx_progress_bar_text_color_set                           _gxe_progress_bar_text_color_set
#define gx_progress_bar_text_draw                                _gx_progress_bar_text_draw
#define gx_progress_bar_value_set                                _gxe_progress_bar_value_set

#define gx_prompt_create(a, b, c, d, e, f, g)                    _gxe_prompt_create(a, b, (GX_WIDGET *)c, d, e, f, g, sizeof(GX_PROMPT))
#define gx_prompt_event_process                                  _gxe_prompt_event_process
#define gx_prompt_draw                                           _gx_prompt_draw
#define gx_prompt_font_set                                       _gxe_prompt_font_set
#define gx_prompt_text_color_set                                 _gxe_prompt_text_color_set
#define gx_prompt_text_draw                                      _gx_prompt_text_draw
#define gx_prompt_text_get_ext                                   _gxe_prompt_text_get_ext
#define gx_prompt_text_id_set                                    _gxe_prompt_text_id_set
#define gx_prompt_text_set_ext                                   _gxe_prompt_text_set_ext

#define gx_radial_progress_bar_anchor_set                        _gxe_radial_progress_bar_anchor_set
#define gx_radial_progress_bar_background_draw                   _gx_radial_progress_bar_background_draw
#define gx_radial_progress_bar_create(a, b, c, d, e, f)          _gxe_radial_progress_bar_create(a, b, c, d, e, f, sizeof(GX_RADIAL_PROGRESS_BAR))
#define gx_radial_progress_bar_draw                              _gx_radial_progress_bar_draw
#define gx_radial_progress_bar_event_process                     _gxe_radial_progress_bar_event_process
#define gx_radial_progress_bar_font_set                          _gxe_radial_progress_bar_font_set
#define gx_radial_progress_bar_info_set                          _gxe_radial_progress_bar_info_set
#define gx_radial_progress_bar_text_color_set                    _gxe_radial_progress_bar_text_color_set
#define gx_radial_progress_bar_text_draw                         _gx_radial_progress_bar_text_draw
#define gx_radial_progress_bar_value_set                         _gxe_radial_progress_bar_value_set

#define gx_radial_slider_anchor_angles_set                       _gxe_radial_slider_anchor_angles_set
#define gx_radial_slider_angle_set                               _gxe_radial_slider_angle_set
#define gx_radial_slider_animation_set                           _gxe_radial_slider_animation_set
#define gx_radial_slider_animation_start                         _gxe_radial_slider_animation_start
#define gx_radial_slider_create(a, b, c, d, e, f, g)             _gxe_radial_slider_create((GX_RADIAL_SLIDER *)a, b, c, d, e, f, g, sizeof(GX_RADIAL_SLIDER))
#define gx_radial_slider_draw                                    _gx_radial_slider_draw
#define gx_radial_slider_event_process                           _gxe_radial_slider_event_process
#define gx_radial_slider_info_get                                _gxe_radial_slider_info_get
#define gx_radial_slider_info_set                                _gxe_radial_slider_info_set
#define gx_radial_slider_pixelmap_set                            _gxe_radial_slider_pixelmap_set

#define gx_radio_button_create(a, b, c, d, e, f, g)              _gxe_radio_button_create(a, b, (GX_WIDGET *)c, d, e, f, g, sizeof(GX_RADIO_BUTTON))
#define gx_radio_button_draw                                     _gx_radio_button_draw
#define gx_radio_button_pixelmap_set                             _gxe_radio_button_pixelmap_set

#define gx_rich_text_view_create(a, b, c, d, e, f, g, h)         _gxe_rich_text_view_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, sizeof(GX_RICH_TEXT_VIEW))
#define gx_rich_text_view_draw                                   _gx_rich_text_view_draw
#define gx_rich_text_view_fonts_set                              _gxe_rich_text_view_fonts_set
#define gx_rich_text_view_text_draw                              _gx_rich_text_view_text_draw

#define gx_scroll_thumb_create(a, b, c)                          _gxe_scroll_thumb_create(a, b, c, sizeof(GX_SCROLL_THUMB))
#define gx_scroll_thumb_draw                                     _gx_scroll_thumb_draw
#define gx_scroll_thumb_event_process                            _gxe_scroll_thumb_event_process

#define gx_horizontal_scrollbar_create(a, b, c, d, e)            _gxe_horizontal_scrollbar_create(a, b, (GX_WINDOW *)c, d, e, sizeof(GX_SCROLLBAR))
#define gx_vertical_scrollbar_create(a, b, c, d, e)              _gxe_vertical_scrollbar_create(a, b, (GX_WINDOW *)c, d, e, sizeof(GX_SCROLLBAR))
#define gx_scrollbar_draw                                        _gx_scrollbar_draw
#define gx_scrollbar_event_process                               _gxe_scrollbar_event_process
#define gx_scrollbar_limit_check                                 _gxe_scrollbar_limit_check
#define gx_scrollbar_reset                                       _gxe_scrollbar_reset
#define gx_scrollbar_value_set                                   _gxe_scrollbar_value_set

#define gx_scroll_wheel_create(a, b, c, d, e, f, g)              _gxe_scroll_wheel_create(a, b, c, d, e, f, g, sizeof(GX_SCROLL_WHEEL))
#define gx_scroll_wheel_event_process                            _gxe_scroll_wheel_event_process
#define gx_scroll_wheel_gradient_alpha_set(a, b, c)              _gxe_scroll_wheel_gradient_alpha_set((GX_SCROLL_WHEEL *) a, b, c)
#define gx_scroll_wheel_row_height_set(a, b)                     _gxe_scroll_wheel_row_height_set((GX_SCROLL_WHEEL *) a, b)
#define gx_scroll_wheel_selected_background_set(a, b)            _gxe_scroll_wheel_selected_background_set((GX_SCROLL_WHEEL *) a, b)
#define gx_scroll_wheel_selected_get(a, b)                       _gxe_scroll_wheel_selected_get((GX_SCROLL_WHEEL *) a, b)
#define gx_scroll_wheel_selected_set(a, b)                       _gxe_scroll_wheel_selected_set((GX_SCROLL_WHEEL *) a, b)
#define gx_scroll_wheel_speed_set(a, b, c, d, e)                 _gxe_scroll_wheel_speed_set((GX_SCROLL_WHEEL *) a, b, c, d, e)
#define gx_scroll_wheel_total_rows_set(a, b)                     _gxe_scroll_wheel_total_rows_set((GX_SCROLL_WHEEL *) a, b)

#define gx_single_line_text_input_backspace(a)                   _gxe_single_line_text_input_backspace((GX_SINGLE_LINE_TEXT_INPUT *)a)
#define gx_single_line_text_input_buffer_clear(a)                _gxe_single_line_text_input_buffer_clear((GX_SINGLE_LINE_TEXT_INPUT *)a)
#define gx_single_line_text_input_buffer_get(a, b, c, d)         _gxe_single_line_text_input_buffer_get((GX_SINGLE_LINE_TEXT_INPUT *)a, b, c, d)
#define gx_single_line_text_input_character_delete(a)            _gxe_single_line_text_input_character_delete(a)
#define gx_single_line_text_input_character_insert(a, b, c)      _gxe_single_line_text_input_character_insert(a, b, c)
#define gx_single_line_text_input_create(a, b, c, d, e, f, g, h) _gxe_single_line_text_input_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, sizeof(GX_SINGLE_LINE_TEXT_INPUT))
#define gx_single_line_text_input_draw(a)                        _gx_single_line_text_input_draw(a)
#define gx_single_line_text_input_draw_position_get(a, b, c)     _gxe_single_line_text_input_draw_position_get(a, b, c)
#define gx_single_line_text_input_end(a)                         _gxe_single_line_text_input_end((GX_SINGLE_LINE_TEXT_INPUT *)a)
#define gx_single_line_text_input_event_process(a, b)            _gxe_single_line_text_input_event_process((GX_SINGLE_LINE_TEXT_INPUT *)a, b)
#define gx_single_line_text_input_fill_color_set(a, b, c, d, e)  _gxe_single_line_text_input_fill_color_set((GX_SINGLE_LINE_TEXT_INPUT *)a, b, c, d, e)
#define gx_single_line_text_input_home(a)                        _gxe_single_line_text_input_home((GX_SINGLE_LINE_TEXT_INPUT *)a)
#define gx_single_line_text_input_left_arrow(a)                  _gxe_single_line_text_input_left_arrow((GX_SINGLE_LINE_TEXT_INPUT *)a)
#define gx_single_line_text_input_position_get(a, b)             _gxe_single_line_text_input_position_get(a, b)
#define gx_single_line_text_input_right_arrow(a)                 _gxe_single_line_text_input_right_arrow((GX_SINGLE_LINE_TEXT_INPUT *)a)
#define gx_single_line_text_input_style_add(a, b)                _gxe_single_line_text_input_style_add((GX_SINGLE_LINE_TEXT_INPUT *)a, b)
#define gx_single_line_text_input_style_remove(a, b)             _gxe_single_line_text_input_style_remove((GX_SINGLE_LINE_TEXT_INPUT *)a, b)
#define gx_single_line_text_input_style_set(a, b)                _gxe_single_line_text_input_style_set((GX_SINGLE_LINE_TEXT_INPUT *)a, b)
#define gx_single_line_text_input_text_color_set(a, b, c, d, e)  _gxe_single_line_text_input_text_color_set((GX_SINGLE_LINE_TEXT_INPUT *)a, b, c, d, e)
#define gx_single_line_text_input_text_select                    _gxe_single_line_text_input_text_select
#define gx_single_line_text_input_text_set_ext                   _gxe_single_line_text_input_text_set_ext

#define gx_slider_create(a, b, c, d, e, f, g, h)                 _gxe_slider_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, sizeof(GX_SLIDER))
#define gx_slider_draw                                           _gx_slider_draw
#define gx_slider_event_process                                  _gxe_slider_event_process
#define gx_slider_info_set                                       _gxe_slider_info_set
#define gx_slider_needle_draw                                    _gx_slider_needle_draw
#define gx_slider_needle_position_get                            _gxe_slider_needle_position_get
#define gx_slider_tickmarks_draw                                 _gx_slider_tickmarks_draw
#define gx_slider_travel_get                                     _gxe_slider_travel_get
#define gx_slider_value_calculate                                _gxe_slider_value_calculate
#define gx_slider_value_set                                      _gxe_slider_value_set

#define gx_sprite_create(a, b, c, d, e, f, g, h)                 _gxe_sprite_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, sizeof(GX_SPRITE))
#define gx_sprite_current_frame_set                              _gxe_sprite_current_frame_set
#define gx_sprite_frame_list_set                                 _gxe_sprite_frame_list_set
#define gx_sprite_start(a, b)                                    _gxe_sprite_start(a, b)
#define gx_sprite_stop(a)                                        _gxe_sprite_stop(a)

#define gx_string_scroll_wheel_create_ext(a, b, c, d, e, f, g, h) _gxe_string_scroll_wheel_create_ext(a, b, c, d, e, f, g, h, sizeof(GX_STRING_SCROLL_WHEEL))
#define gx_string_scroll_wheel_event_process                     _gxe_string_scroll_wheel_event_process
#define gx_string_scroll_wheel_string_id_list_set                _gxe_string_scroll_wheel_string_id_list_set
#define gx_string_scroll_wheel_string_list_set_ext               _gxe_string_scroll_wheel_string_list_set_ext

#define gx_system_active_language_set                            _gxe_system_active_language_set
#if (GX_ANIMATION_POOL_SIZE > 0)
#define gx_system_animation_get                                  _gxe_system_animation_get
#define gx_system_animation_free                                 _gxe_system_animation_free
#endif
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
#define gx_system_bidi_text_enable                               _gx_system_bidi_text_enable
#define gx_system_bidi_text_disable                              _gx_system_bidi_text_disable
#endif
#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
#define gx_system_text_render_style_set                          _gx_system_text_render_style_set
#endif
#define gx_system_canvas_refresh                                 _gxe_system_canvas_refresh
#define gx_system_dirty_mark(a)                                  _gxe_system_dirty_mark((GX_WIDGET *)a);
#define gx_system_dirty_partial_add(a, b)                        _gxe_system_dirty_partial_add((GX_WIDGET *)a, b)
#define gx_system_draw_context_get                               _gxe_system_draw_context_get
#define gx_system_event_fold                                     _gxe_system_event_fold
#define gx_system_event_send                                     _gxe_system_event_send
#define gx_system_focus_claim(a)                                 _gxe_system_focus_claim((GX_WIDGET *)a)
#define gx_system_initialize                                     _gxe_system_initialize
#define gx_system_memory_allocator_set                           _gxe_system_memory_allocator_set
#define gx_system_pen_configure                                  _gxe_system_pen_configure
#define gx_system_screen_stack_create(a, b)                      _gxe_system_screen_stack_create(a, b, sizeof(GX_SCREEN_STACK_CONTROL))
#define gx_system_screen_stack_get                               _gxe_system_screen_stack_get
#define gx_system_screen_stack_pop                               _gxe_system_screen_stack_pop
#define gx_system_screen_stack_push                              _gxe_system_screen_stack_push
#define gx_system_screen_stack_reset                             _gx_system_screen_stack_reset
#define gx_system_scroll_appearance_get                          _gxe_system_scroll_appearance_get
#define gx_system_scroll_appearance_set                          _gxe_system_scroll_appearance_set
#define gx_system_start                                          _gxe_system_start
#define gx_system_string_width_get_ext                           _gxe_system_string_width_get_ext
#define gx_system_timer_start(a, b, c, d)                        _gxe_system_timer_start((GX_WIDGET *)a, b, c, d)
#define gx_system_timer_stop(a, b)                               _gxe_system_timer_stop((GX_WIDGET *)a, b)
#define gx_system_version_string_get_ext                         _gxe_system_version_string_get_ext
#define gx_system_widget_find                                    _gxe_system_widget_find

#define gx_text_button_create(a, b, c, d, e, f, g)               _gxe_text_button_create(a, b, (GX_WIDGET *)c, d, e, f, g, sizeof(GX_TEXT_BUTTON))
#define gx_text_button_draw                                      _gx_text_button_draw
#define gx_text_button_event_process                             _gxe_text_button_event_process
#define gx_text_button_font_set                                  _gxe_text_button_font_set
#define gx_text_button_text_color_set                            _gxe_text_button_text_color_set
#define gx_text_button_text_draw                                 _gx_text_button_text_draw
#define gx_text_button_text_get_ext                              _gxe_text_button_text_get_ext
#define gx_text_button_text_id_set                               _gxe_text_button_text_id_set
#define gx_text_button_text_set_ext                              _gxe_text_button_text_set_ext

#define gx_text_input_cursor_blink_interval_set(a, b)            _gxe_text_input_cursor_blink_interval_set((GX_TEXT_INPUT_CURSOR *)a, b)
#define gx_text_input_cursor_height_set(a, b)                    _gxe_text_input_cursor_height_set((GX_TEXT_INPUT_CURSOR *)a, b)
#define gx_text_input_cursor_width_set(a, b)                     _gxe_text_input_cursor_width_set((GX_TEXT_INPUT_CURSOR *)a, b)

#define gx_text_scroll_wheel_callback_set_ext(a, b)              _gxe_text_scroll_wheel_callback_set_ext((GX_TEXT_SCROLL_WHEEL *)a, (UINT(*)(GX_TEXT_SCROLL_WHEEL *, INT, GX_STRING *)) b)
#define gx_text_scroll_wheel_font_set(a, b, c)                   _gxe_text_scroll_wheel_font_set((GX_TEXT_SCROLL_WHEEL *) a, b, c)
#define gx_text_scroll_wheel_text_color_set(a, b, c, d)          _gxe_text_scroll_wheel_text_color_set((GX_TEXT_SCROLL_WHEEL *) a, b, c, d)
#define gx_text_scroll_wheel_create(a, b, c, d, e, f, g)         _gxe_text_scroll_wheel_create(a, b, c, d, e, f, g, sizeof(GX_TEXT_SCROLL_WHEEL))
#define gx_text_scroll_wheel_draw                                _gx_text_scroll_wheel_draw
#define gx_text_scroll_wheel_event_process                       _gxe_text_scroll_wheel_event_process

#define gx_tree_view_create(a, b, c, d, e, f)                    _gxe_tree_view_create((GX_TREE_VIEW *)a, b, (GX_WIDGET *)c, d, e, f, sizeof(GX_TREE_VIEW))
#define gx_tree_view_draw                                        _gx_tree_view_draw
#define gx_tree_view_event_process                               _gxe_tree_view_event_process
#define gx_tree_view_indentation_set                             _gxe_tree_view_indentation_set
#define gx_tree_view_position                                    _gxe_tree_view_position
#define gx_tree_view_root_line_color_set                         _gxe_tree_view_root_line_color_set
#define gx_tree_view_root_pixelmap_set                           _gxe_tree_view_root_pixelmap_set
#define gx_tree_view_selected_get                                _gxe_tree_view_selected_get
#define gx_tree_view_selected_set                                _gxe_tree_view_selected_set

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
#define gx_utility_bidi_paragraph_reorder                        _gxe_utility_bidi_paragraph_reorder
#define gx_utility_bidi_paragraph_reorder_ext                    _gxe_utility_bidi_paragraph_reorder_ext
#define gx_utility_bidi_resolved_text_info_delete                _gxe_utility_bidi_resolved_text_info_delete
#endif
#define gx_utility_canvas_to_bmp                                 _gxe_utility_canvas_to_bmp
#define gx_utility_circle_point_get                              _gxe_utility_circle_point_get
#define gx_utility_gradient_create                               _gxe_system_gradient_create
#define gx_utility_gradient_delete                               _gxe_system_gradient_delete
#define gx_utility_ltoa                                          _gxe_utility_ltoa
#define gx_utility_math_acos                                     _gx_utility_math_acos
#define gx_utility_math_asin                                     _gx_utility_math_asin
#define gx_utility_math_cos                                      _gx_utility_math_cos
#define gx_utility_math_sin                                      _gx_utility_math_sin
#define gx_utility_math_sqrt                                     _gx_utility_math_sqrt
#define gx_utility_pixelmap_resize                               _gxe_utility_pixelmap_resize
#define gx_utility_pixelmap_rotate                               _gxe_utility_pixelmap_rotate
#define gx_utility_pixelmap_simple_rotate                        _gxe_utility_pixelmap_simple_rotate
#define gx_utility_rectangle_center                              _gxe_utility_rectangle_center
#define gx_utility_rectangle_center_find                         _gxe_utility_rectangle_center_find
#define gx_utility_rectangle_combine                             _gxe_utility_rectangle_combine
#define gx_utility_rectangle_compare                             _gxe_utility_rectangle_compare
#define gx_utility_rectangle_define                              _gxe_utility_rectangle_define
#define gx_utility_rectangle_overlap_detect                      _gxe_utility_rectangle_overlap_detect
#define gx_utility_rectangle_point_detect                        _gxe_utility_rectangle_point_detect
#define gx_utility_rectangle_resize                              _gxe_utility_rectangle_resize
#define gx_utility_rectangle_shift                               _gxe_utility_rectangle_shift
#define gx_utility_string_to_alphamap_ext                        _gxe_utility_string_to_alphamap_ext

#define gx_vertical_list_children_position                       _gxe_vertical_list_children_position
#define gx_vertical_list_create(a, b, c, d, e, f, g, h)          _gxe_vertical_list_create(a, b, (GX_WIDGET *)c, d, e, f, g, h, sizeof(GX_VERTICAL_LIST))
#define gx_vertical_list_event_process                           _gxe_vertical_list_event_process
#define gx_vertical_list_page_index_set(a, b)                    _gxe_vertical_list_page_index_set((GX_VERTICAL_LIST *)a, b)
#define gx_vertical_list_selected_index_get(a, b)                _gxe_vertical_list_selected_index_get(a, b)
#define gx_vertical_list_selected_set(a, b)                      _gxe_vertical_list_selected_set(a, b)
#define gx_vertical_list_selected_widget_get(a, b)               _gxe_vertical_list_selected_widget_get(a, (GX_WIDGET **)b)
#define gx_vertical_list_total_rows_set(a, b)                    _gxe_vertical_list_total_rows_set((GX_VERTICAL_LIST *)a, b)

#define gx_widget_allocate(a, b)                                 _gxe_widget_allocate((GX_WIDGET **)a, b)
#define gx_widget_attach(a, b)                                   _gxe_widget_attach((GX_WIDGET *)a, (GX_WIDGET *)b)
#define gx_widget_back_attach(a, b)                              _gxe_widget_back_attach((GX_WIDGET *)a, (GX_WIDGET *)b)
#define gx_widget_back_move(a, b)                                _gxe_widget_back_move((GX_WIDGET *)a, b)
#define gx_widget_background_draw(a)                             _gx_widget_background_draw((GX_WIDGET *)a)
#define gx_widget_block_move(a, b, c, d)                         _gxe_widget_block_move((GX_WIDGET *)a, b, c, d)
#define gx_widget_border_draw(a, b, c, d, e)                     _gx_widget_border_draw((GX_WIDGET *)a, b, c, d, e)
#define gx_widget_border_style_set(a, b)                         _gxe_widget_border_style_set((GX_WIDGET *)a, b)
#define gx_widget_border_width_get(a, b)                         _gxe_widget_border_width_get((GX_WIDGET *)a, b)
#define gx_widget_canvas_get(a, b)                               _gxe_widget_canvas_get((GX_WIDGET *)a, b)
#define gx_widget_child_detect(a, b, c)                          _gxe_widget_child_detect((GX_WIDGET *)a, b, c)
#define gx_widget_children_draw(a)                               _gx_widget_children_draw((GX_WIDGET *)a)
#define gx_widget_color_get(a, b, c)                             _gxe_widget_color_get((GX_WIDGET *)a, b, c)
#define gx_widget_client_get(a, b, c)                            _gxe_widget_client_get((GX_WIDGET *)a, b, c)
#define gx_widget_create(a, b, c, d, e, f)                       _gxe_widget_create((GX_WIDGET *)a, b, (GX_WIDGET *)c, d, e, f, sizeof(GX_WIDGET))
#define gx_widget_created_test(a, b)                             _gxe_widget_created_test((GX_WIDGET *)a, b)
#define gx_widget_delete(a)                                      _gxe_widget_delete((GX_WIDGET *)a)
#define gx_widget_detach(a)                                      _gxe_widget_detach((GX_WIDGET *)a)
#define gx_widget_draw(a)                                        _gx_widget_draw((GX_WIDGET *)a)
#define gx_widget_draw_set(a, b)                                 _gxe_widget_draw_set((GX_WIDGET *)a, (void (*)(GX_WIDGET *))b)
#define gx_widget_event_generate(a, b, c)                        _gxe_widget_event_generate((GX_WIDGET *)a, b, c)
#define gx_widget_event_process(a, b)                            _gxe_widget_event_process((GX_WIDGET *)a, b)
#define gx_widget_event_process_set(a, b)                        _gxe_widget_event_process_set((GX_WIDGET *)a, (UINT (*)(GX_WIDGET *, GX_EVENT *)) b)
#define gx_widget_event_to_parent(a, b)                          _gxe_widget_event_to_parent((GX_WIDGET *)a, b)
#define gx_widget_fill_color_set(a, b, c, d)                     _gxe_widget_fill_color_set((GX_WIDGET *)a, b, c, d)
#define gx_widget_find(a, b, c, d)                               _gxe_widget_find((GX_WIDGET *)a, b, c, (GX_WIDGET **)d)
#define gx_widget_first_child_get(a, b)                          _gxe_widget_first_child_get((GX_WIDGET *) a, (GX_WIDGET **) b)
#define gx_widget_focus_next(a)                                  _gxe_widget_focus_next((GX_WIDGET *)a)
#define gx_widget_focus_previous(a)                              _gxe_widget_focus_previous((GX_WIDGET *)a)
#define gx_widget_font_get(a, b, c)                              _gxe_widget_font_get((GX_WIDGET *)a, b, c)
#define gx_widget_free(a)                                        _gxe_widget_free((GX_WIDGET *)a)
#define gx_widget_front_move(a, b)                               _gxe_widget_front_move((GX_WIDGET *)a, b)
#define gx_widget_height_get(a, b)                               _gxe_widget_height_get((GX_WIDGET *)a, b)
#define gx_widget_hide(a)                                        _gxe_widget_hide((GX_WIDGET *)a)
#define gx_widget_last_child_get(a, b)                           _gxe_widget_last_child_get((GX_WIDGET *) a, (GX_WIDGET **) b)
#define gx_widget_next_sibling_get(a, b)                         _gxe_widget_next_sibling_get((GX_WIDGET *) a, (GX_WIDGET **) b)
#define gx_widget_parent_get(a, b)                               _gxe_widget_parent_get((GX_WIDGET *) a, (GX_WIDGET **) b)
#define gx_widget_pixelmap_get(a, b, c)                          _gxe_widget_pixelmap_get((GX_WIDGET *)a, b, c)
#define gx_widget_previous_sibling_get(a, b)                     _gxe_widget_previous_sibling_get((GX_WIDGET *) a, (GX_WIDGET **) b)
#define gx_widget_resize(a, b)                                   _gxe_widget_resize((GX_WIDGET *)a, b)
#define gx_widget_shift(a, b, c, d)                              _gxe_widget_shift((GX_WIDGET *)a, b, c, d)
#define gx_widget_show(a)                                        _gxe_widget_show((GX_WIDGET *)a)
#define gx_widget_status_add(a, b)                               _gxe_widget_status_add((GX_WIDGET *)a, b)
#define gx_widget_status_get(a, b)                               _gxe_widget_status_get((GX_WIDGET *)a, b)
#define gx_widget_status_remove(a, b)                            _gxe_widget_status_remove((GX_WIDGET *)a, b)
#define gx_widget_status_test(a, b, c)                           _gxe_widget_status_test((GX_WIDGET *)a, b, c)
#define gx_widget_string_get_ext(a, b, c)                        _gxe_widget_string_get_ext((GX_WIDGET *)a, b, c)
#define gx_widget_style_add(a, b)                                _gxe_widget_style_add((GX_WIDGET *)a, b)
#define gx_widget_style_get(a, b)                                _gxe_widget_style_get((GX_WIDGET *)a, b)
#define gx_widget_style_remove(a, b)                             _gxe_widget_style_remove((GX_WIDGET *)a, b)
#define gx_widget_style_set(a, b)                                _gxe_widget_style_set((GX_WIDGET *)a, b)
#define gx_widget_text_blend_ext(a, b, c, d, e, f, g)            _gxe_widget_text_blend_ext((GX_WIDGET *)a, b, c, d, e, f, g)
#define gx_widget_text_draw_ext(a, b, c, d, e, f)                _gx_widget_text_draw_ext((GX_WIDGET *)a, b, c, d, e, f)
#define gx_widget_text_id_draw(a, b, c, d, e, f)                 _gx_widget_text_id_draw((GX_WIDGET *)a, b, c, d, e, f)
#define gx_widget_top_visible_child_find(a, b)                   _gxe_widget_top_visible_child_find((GX_WIDGET *) a, (GX_WIDGET **) b)
#define gx_widget_type_find(a, b, c)                             _gxe_widget_type_find((GX_WIDGET *)a, b, (GX_WIDGET **)c)
#define gx_widget_width_get(a, b)                                _gxe_widget_width_get((GX_WIDGET *)a, b)

#define gx_window_background_draw                                _gx_window_background_draw
#define gx_window_client_height_get                              _gxe_window_client_height_get
#define gx_window_client_scroll                                  _gxe_window_client_scroll
#define gx_window_client_width_get                               _gxe_window_client_width_get
#define gx_window_close                                          _gxe_window_close
#define gx_window_create(a, b, c, d, e, f)                       _gxe_window_create(a, b, (GX_WIDGET *)c, d, e, f, sizeof(GX_WINDOW))
#define gx_window_draw                                           _gx_window_draw
#define gx_window_event_process                                  _gxe_window_event_process
#define gx_window_execute                                        _gxe_window_execute
#define gx_window_scroll_info_get                                _gxe_window_scroll_info_get
#define gx_window_scrollbar_find                                 _gxe_window_scrollbar_find
#define gx_window_wallpaper_get                                  _gxe_window_wallpaper_get
#define gx_window_wallpaper_set(a, b, c)                         _gxe_window_wallpaper_set((GX_WINDOW *)a, b, c)

#define gx_window_root_create(a, b, c, d, e, f)                  _gxe_window_root_create(a, b, c, d, e, f, sizeof(GX_WINDOW_ROOT))
#define gx_window_root_delete                                    _gxe_window_root_delete
#define gx_window_root_event_process                             _gxe_window_root_event_process
#define gx_window_root_find                                      _gxe_window_root_find

#endif /* !GX_DISABLE_ERROR_CHECKING */

#endif
