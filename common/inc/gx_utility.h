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
/**   Utility (Utility)                                                   */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_utility.h                                                        */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX utility component,                       */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_UTILITY_H
#define GX_UTILITY_H

#include "gx_utility_math.h"
#include "gx_utility_rectangle.h"
#include "gx_utility_pixelmap.h"
#include "gx_utility_string.h"
#include "gx_utility_bidi.h"

struct GX_CANVAS;

/* Define utility component function prototypes.  */

UINT    _gx_utility_ltoa(LONG value, GX_CHAR *return_buffer, UINT return_buffer_size);

// return string length
UINT    _gx_utility_value_or_percent_string_get(GX_BOOL is_percent, INT value, INT max_value, GX_CHAR *return_buffer, UINT return_buffer_size);

UINT    _gx_utility_circle_point_get(INT xcenter, INT ycenter, UINT r, INT angle, GX_POINT *point);
UINT    _gx_utility_radial_angle_calculate(GX_POINT point, GX_POINT center, INT *return_angle);

GX_VALUE _gx_utility_value_clamp(GX_VALUE value, GX_VALUE min_value, GX_VALUE max_value);

UINT    _gx_utility_easing_function_calculate(USHORT easing_function_style, INT start_val, INT end_val, INT t, INT d, INT *return_value);

UINT    _gx_utility_alphamap_create(INT width, INT height, GX_PIXELMAP *map);
void    _gx_utility_pixelmap_free(GX_PIXELMAP *map);

UINT    _gx_utility_canvas_to_bmp(GX_CANVAS *canvas, GX_RECTANGLE *rect, UINT (*write_data)(GX_UBYTE *byte_data, UINT data_count));

#ifdef GX_THAI_GLYPH_SHAPING_SUPPORT
UINT    _gx_utility_thai_glyph_shaping(const GX_STRING *string, GX_CHAR_CODE **returned_code_list, UINT *returned_code_count);
#endif

UINT    _gx_utility_string_to_alphamap_ext(const GX_STRING *text, const GX_FONT *font, GX_PIXELMAP *textmap);
void    _gx_utility_string_to_alphamap_draw(const GX_STRING *text, const GX_FONT *font, GX_PIXELMAP *map);

/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT    _gxe_utility_canvas_to_bmp(GX_CANVAS *canvas, GX_RECTANGLE *rect, UINT (*write_data)(GX_UBYTE *byte_data, UINT data_count));
UINT    _gxe_utility_circle_point_get(INT xcenter, INT ycenter, UINT r, INT angle, GX_POINT *point);
UINT    _gxe_utility_ltoa(LONG value, GX_CHAR *return_buffer, UINT return_buffer_size);
UINT    _gxe_utility_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT    _gxe_utility_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gxe_utility_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gxe_utility_rectangle_center(GX_RECTANGLE *rectangle, GX_RECTANGLE *within);
UINT    _gxe_utility_rectangle_center_find(GX_RECTANGLE *rectangle, GX_POINT *return_center);
UINT    _gxe_utility_rectangle_combine(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle);
GX_BOOL _gxe_utility_rectangle_compare(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle);
UINT    _gxe_utility_rectangle_define(GX_RECTANGLE *rectangle, GX_VALUE left, GX_VALUE top, GX_VALUE right, GX_VALUE bottom);
UINT    _gxe_utility_rectangle_resize(GX_RECTANGLE *rectangle, GX_VALUE adjust);
GX_BOOL _gxe_utility_rectangle_overlap_detect(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle, GX_RECTANGLE *return_overlap_area);
GX_BOOL _gxe_utility_rectangle_point_detect(GX_RECTANGLE *rectangle, GX_POINT point);
UINT    _gxe_utility_rectangle_shift(GX_RECTANGLE *rectangle, GX_VALUE x_shift, GX_VALUE y_shift);
UINT    _gxe_utility_string_to_alphamap_ext(const GX_STRING *text, const GX_FONT *font, GX_PIXELMAP *textmap);
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT    _gxe_utility_bidi_paragraph_reorder(GX_BIDI_TEXT_INFO *input_info, GX_BIDI_RESOLVED_TEXT_INFO **resolved_info_head);
UINT    _gxe_utility_bidi_paragraph_reorder_ext(GX_BIDI_TEXT_INFO *input_info, GX_BIDI_RESOLVED_TEXT_INFO **resolved_info_head);
UINT    _gxe_utility_bidi_resolved_text_info_delete(GX_BIDI_RESOLVED_TEXT_INFO **resolved_info_head);
#endif
#endif

