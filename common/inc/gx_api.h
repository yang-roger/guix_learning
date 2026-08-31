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
/**************************************************************************/
/*                                                                        */
/*  APPLICATION INTERFACE DEFINITION                       RELEASE        */
/*                                                                        */
/*    gx_api.h                                                            */
/*                                                           6.5.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the basic Application Interface (API) to the      */
/*    high-performance GUIX UI framework.  All service prototypes         */
/*    and data structure definitions are defined in this file.            */
/*    Please note that basic data type definitions and other architecture-*/
/*    specific information is contained in the file gx_port.h.            */
/*                                                                        */
/**************************************************************************/

#ifndef GX_API_H
#define GX_API_H

#include "gx_global.h"

#include "gx_point.h"
#include "gx_rectangle.h"

#include "gx_font.h"
#include "gx_pixelmap.h"
#include "gx_scroll_bar_appearance.h"

#include "gx_theme.h"
#include "gx_binres_loader.h"

#include "gx_brush.h"

#include "gx_display.h"
#include "gx_canvas.h"
#include "gx_context.h"

#include "gx_event.h"

#include "gx_widget.h"
#include "gx_prompt.h"
#include "gx_numeric_prompt.h"
#include "gx_pixelmap_prompt.h"
#include "gx_numeric_pixelmap_prompt.h"
#include "gx_icon.h"
#include "gx_sprite.h"
#include "gx_button.h"
#include "gx_text_button.h"
#include "gx_multi_line_text_button.h"
#include "gx_radio_button.h"
#include "gx_checkbox.h"
#include "gx_icon_button.h"
#include "gx_pixelmap_button.h"
#include "gx_scroll_thumb.h"
#include "gx_scroll_bar.h"
#include "gx_slider.h"
#include "gx_pixelmap_slider.h"
#include "gx_window.h"
#include "gx_window_root.h"
#include "gx_single_line_text_input.h"
#include "gx_multi_line_text_view.h"
#include "gx_multi_line_text_input.h"
#include "gx_rich_text_view.h"
#include "gx_vertical_list.h"
#include "gx_horizontal_list.h"
#include "gx_popup_list.h"
#include "gx_drop_list.h"
#include "gx_progress_bar.h"
#include "gx_circular_gauge.h"
#include "gx_radial_progress_bar.h"
#include "gx_radial_slider.h"
#include "gx_line_chart.h"
#include "gx_scroll_wheel.h"
#include "gx_generic_scroll_wheel.h"
#include "gx_text_scroll_wheel.h"
#include "gx_string_scroll_wheel.h"
#include "gx_numeric_scroll_wheel.h"
#include "gx_menu.h"
#include "gx_accordion_menu.h"
#include "gx_tree_view.h"

#include "gx_animation.h"
#include "gx_image_reader.h"
#include "gx_screen_stack.h"

#include "gx_pen_config.h"
#include "gx_touch.h"

#include "gx_gradient.h"
#include "gx_utility.h"
#include "gx_system.h"

#include "gx_api_functions.h"

#ifndef GX_THREADX_BINDING
#ifdef  GX_STANDALONE_BINDING
#include <string.h>
#include <stdlib.h>
#include "gx_system_standalone_bind.h"
#else
/* pull in custom rtos porting header */
#include <string.h>
#include <stdlib.h>
#include "gx_system_rtos_bind.h"
#endif
#endif

#endif

