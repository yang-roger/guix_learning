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
/**   Display Driver (Display)                                            */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_display_driver.h                                                 */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX Display component, including all data    */
/*    types and external references.  It is assumed that gx_api.h and     */
/*    gx_port.h have already been included.                               */
/*                                                                        */
/**************************************************************************/

#ifndef GX_DISPLAY_DRIVER_H
#define GX_DISPLAY_DRIVER_H

#include "gx_display.h"

/* Mouse */

#if defined(GX_MOUSE_SUPPORT)

void _gx_display_driver_generic_mouse_define(GX_DISPLAY* display, GX_CANVAS* canvas, GX_MOUSE_CURSOR_INFO* info);

#if !defined(GX_HARDWARE_MOUSE_SUPPORT)

void _gx_display_driver_generic_mouse_position_set(GX_DISPLAY* display, GX_POINT* point);
void _gx_display_driver_generic_mouse_enable(GX_DISPLAY* display, GX_BOOL enable);
void _gx_display_driver_generic_drawing_complete(GX_DISPLAY* display, GX_CANVAS* canvas);
void _gx_display_driver_generic_drawing_initiate(GX_DISPLAY* display, GX_CANVAS* canvas);
void _gx_display_driver_generic_mouse_draw(GX_DISPLAY* display);

void _gx_display_driver_24xrgb_mouse_capture(GX_DISPLAY* display);
void _gx_display_driver_24xrgb_mouse_restore(GX_DISPLAY* display);

void _gx_display_driver_16bpp_mouse_capture(GX_DISPLAY* display);
void _gx_display_driver_16bpp_mouse_restore(GX_DISPLAY* display);

void _gx_display_driver_8bpp_mouse_capture(GX_DISPLAY* display);
void _gx_display_driver_8bpp_mouse_restore(GX_DISPLAY* display);

void _gx_display_driver_4bpp_mouse_capture(GX_DISPLAY* display);
void _gx_display_driver_4bpp_mouse_restore(GX_DISPLAY* display);

void _gx_display_driver_1bpp_mouse_capture(GX_DISPLAY* display);
void _gx_display_driver_1bpp_mouse_restore(GX_DISPLAY* display);

GX_RECTANGLE* _gx_display_driver_mouse_rectangle_set(GX_DISPLAY* display, INT* mouse_rect_width, INT* mouse_rect_height);

#endif /* !GX_HARDWARE_MOUSE_SUPPORT */

#endif /* GX_MOUSE_SUPPORT */


void _gx_display_driver_generic_simple_wide_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, INT xend, INT yend);
void _gx_display_driver_generic_aliased_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, INT xend, INT yend);
void _gx_display_driver_generic_aliased_fixed_point_line_draw(GX_DRAW_CONTEXT* context,
                                                              GX_FIXED_VAL xstart, GX_FIXED_VAL ystart,
                                                              GX_FIXED_VAL xend, GX_FIXED_VAL yend);
void _gx_display_driver_generic_aliased_wide_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, INT xend, INT yend);
GX_FIXED_POINT* _gx_display_driver_generic_wide_line_points_calculate(GX_DRAW_CONTEXT* context, INT xStart, INT yStart,
                                                                      INT xEnd, INT yEnd, INT brush_width, GX_BOOL outline);
void _gx_display_driver_generic_wide_line_fill(GX_DRAW_CONTEXT* context, GX_FIXED_POINT* pPoints);

void _gx_display_driver_generic_alphamap_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);

void _gx_display_driver_generic_glyph_8bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);
void _gx_display_driver_generic_glyph_4bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);

void _gx_display_driver_generic_polygon_draw(GX_DRAW_CONTEXT* context, GX_POINT* vertex, INT num);
void _gx_display_driver_generic_polygon_fill(GX_DRAW_CONTEXT* context, GX_POINT* vertex, INT num);

#if defined(GX_ARC_DRAWING_SUPPORT)

void _gx_display_driver_generic_circle_draw(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r);
void _gx_display_driver_generic_wide_circle_draw(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r);
void _gx_display_driver_generic_aliased_circle_draw(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r);
void _gx_display_driver_generic_aliased_wide_circle_draw(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r);
void _gx_display_driver_generic_circle_fill(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r);
void _gx_display_driver_generic_filled_circle_draw(GX_DRAW_CONTEXT* context, GX_FIXED_VAL xcenter, GX_FIXED_VAL ycenter, GX_FIXED_VAL r);
void _gx_display_driver_generic_aliased_filled_circle_draw(GX_DRAW_CONTEXT* context, GX_FIXED_VAL xcenter, GX_FIXED_VAL ycenter, GX_FIXED_VAL r);

void _gx_display_driver_generic_ellipse_draw(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, INT a, INT b);
void _gx_display_driver_generic_wide_ellipse_draw(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, INT a, INT b);
void _gx_display_driver_generic_aliased_ellipse_draw(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, INT a, INT b);
void _gx_display_driver_generic_aliased_wide_ellipse_draw(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, INT a, INT b);
void _gx_display_driver_generic_ellipse_fill(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, INT a, INT b);

void _gx_display_driver_generic_arc_draw(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
void _gx_display_driver_generic_wide_arc_draw(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
void _gx_display_driver_generic_aliased_arc_draw(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
void _gx_display_driver_generic_aliased_wide_arc_draw(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
void _gx_display_driver_generic_arc_fill(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);

void _gx_display_driver_generic_pie_fill(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
void _gx_display_driver_generic_simple_pie_fill(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle, GX_BOOL skip_end);

void _gx_display_driver_arc_clipping_get(INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle,
                                         GX_RECTANGLE* clip_1, GX_RECTANGLE* clip_2, GX_RECTANGLE* clip_3, GX_RECTANGLE* clip_4);

#endif /* GX_ARC_DRAWING_SUPPORT */

#if defined(GX_BRUSH_ALPHA_SUPPORT)
void _gx_display_driver_horizontal_line_alpha_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, INT width, GX_COLOR color, GX_UBYTE alpha);
void _gx_display_driver_vertical_line_alpha_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x, INT width, GX_COLOR color, GX_UBYTE alpha);
void _gx_display_driver_simple_line_alpha_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, INT xend, INT yend, GX_UBYTE alpha);
#endif

void _gx_display_driver_generic_rotated_wide_line_fill(GX_DRAW_CONTEXT* context, GX_FIXED_POINT* pPoints);
void _gx_display_driver_generic_rotated_alphamap_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
void _gx_display_driver_generic_rotated_glyph_4bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);
void _gx_display_driver_generic_rotated_glyph_8bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);

void _gx_display_driver_generic_rotated_polygon_fill(GX_DRAW_CONTEXT* context, GX_POINT* vertex, INT num);

#if defined(GX_ARC_DRAWING_SUPPORT)

void _gx_display_driver_generic_rotated_circle_fill(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r);
void _gx_display_driver_generic_rotated_ellipse_fill(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, INT a, INT b);
void _gx_display_driver_generic_rotated_arc_fill(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
void _gx_display_driver_generic_rotated_pie_fill(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);

#endif /* GX_ARC_DRAWING_SUPPORT */


void     _gx_display_driver_32bpp_canvas_copy(GX_CANVAS* source, GX_CANVAS* dest);
USHORT   _gx_display_driver_32bpp_row_pitch_get(USHORT width);
void     _gx_display_driver_32bpp_block_move(GX_DRAW_CONTEXT* context, GX_RECTANGLE* src, INT xshift, INT yshift);
void     _gx_display_driver_32bpp_pixel_write(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color);
void     _gx_display_driver_32bpp_simple_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, INT xend, INT yend);
void     _gx_display_driver_32bpp_horizontal_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, INT width, GX_COLOR color);
void     _gx_display_driver_32bpp_vertical_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x, INT width, GX_COLOR color);
void     _gx_display_driver_32bpp_horizontal_pattern_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y);
void     _gx_display_driver_32bpp_vertical_pattern_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x);
void     _gx_display_driver_32bpp_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info);
void     _gx_display_driver_32bpp_pixelmap_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT rot_cx, INT rot_cy);
void     _gx_display_driver_32bpp_glyph_1bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);


void     _gx_display_driver_32bpp_rotated_canvas_copy(GX_CANVAS* source, GX_CANVAS* dest);
void     _gx_display_driver_32bpp_rotated_block_move(GX_DRAW_CONTEXT* context, GX_RECTANGLE* src, INT xshift, INT yshift);
void     _gx_display_driver_32bpp_rotated_pixel_write(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color);
void     _gx_display_driver_32bpp_rotated_simple_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, INT xend, INT yend);
void     _gx_display_driver_32bpp_rotated_horizontal_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, INT width, GX_COLOR color);
void     _gx_display_driver_32bpp_rotated_vertical_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x, INT width, GX_COLOR color);
void     _gx_display_driver_32bpp_rotated_horizontal_pattern_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y);
void     _gx_display_driver_32bpp_rotated_vertical_pattern_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x);
void     _gx_display_driver_32bpp_rotated_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info);
void     _gx_display_driver_32bpp_rotated_pixelmap_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_32bpp_rotated_pixelmap_blend(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap, GX_UBYTE alpha);
void     _gx_display_driver_32bpp_rotated_pixelmap_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT rot_cx, INT rot_cy);
void     _gx_display_driver_32bpp_rotated_glyph_1bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);


GX_COLOR _gx_display_driver_32argb_native_color_get(GX_DISPLAY*, GX_COLOR rawcolor);
void     _gx_display_driver_32argb_pixel_blend(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
void     _gx_display_driver_32argb_pixelmap_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_32argb_pixelmap_blend(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap, GX_UBYTE alpha);


void     _gx_display_driver_32argb_rotated_pixel_blend(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);


void     _gx_display_driver_24xrgb_canvas_blend(GX_CANVAS* source, GX_CANVAS* dest);
GX_COLOR _gx_display_driver_24xrgb_native_color_get(GX_DISPLAY*, GX_COLOR rawcolor);
void     _gx_display_driver_24xrgb_pixel_blend(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
void     _gx_display_driver_24xrgb_pixelmap_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_24xrgb_pixelmap_blend(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap, GX_UBYTE alpha);
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
void     _gx_display_driver_24xrgb_jpeg_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_24xrgb_png_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
#endif


void     _gx_display_driver_24xrgb_rotated_canvas_blend(GX_CANVAS* source, GX_CANVAS* dest);
void     _gx_display_driver_24xrgb_rotated_pixel_blend(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
void     _gx_display_driver_24xrgb_rotated_jpeg_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_24xrgb_rotated_png_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
#endif


void     _gx_display_driver_16bpp_canvas_copy(GX_CANVAS* source, GX_CANVAS* dest);
void     _gx_display_driver_565rgb_canvas_blend(GX_CANVAS* source, GX_CANVAS* dest);
GX_COLOR _gx_display_driver_565rgb_native_color_get(GX_DISPLAY*, GX_COLOR rawcolor);
USHORT   _gx_display_driver_16bpp_row_pitch_get(USHORT width);
void     _gx_display_driver_16bpp_block_move(GX_DRAW_CONTEXT* context, GX_RECTANGLE* src, INT xshift, INT yshift);
void     _gx_display_driver_16bpp_pixel_write(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color);
void     _gx_display_driver_565rgb_pixel_blend(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
void     _gx_display_driver_16bpp_simple_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, INT xend, INT yend);
void     _gx_display_driver_16bpp_horizontal_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, INT width, GX_COLOR color);
void     _gx_display_driver_16bpp_vertical_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x, INT width, GX_COLOR color);
void     _gx_display_driver_16bpp_horizontal_pattern_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y);
void     _gx_display_driver_16bpp_vertical_pattern_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x);
void     _gx_display_driver_565rgb_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info);
void     _gx_display_driver_565rgb_pixelmap_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_565rgb_pixelmap_blend(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap, GX_UBYTE alpha);
void     _gx_display_driver_16bpp_pixelmap_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT rot_cx, INT rot_cy);
void     _gx_display_driver_16bpp_pixelmap_simple_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT cx, INT cy);
void     _gx_display_driver_16bpp_pixelmap_simple_alpha_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT cx, INT cy);
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
void     _gx_display_driver_565rgb_jpeg_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_565rgb_png_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
#endif
void     _gx_display_driver_16bpp_glyph_1bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);


void     _gx_display_driver_16bpp_rotated_canvas_copy(GX_CANVAS* canvas, GX_CANVAS* composite);
void     _gx_display_driver_565rgb_rotated_canvas_blend(GX_CANVAS* canvas, GX_CANVAS* composite);
void     _gx_display_driver_16bpp_rotated_block_move(GX_DRAW_CONTEXT* context, GX_RECTANGLE* block, INT xshift, INT yshift);
void     _gx_display_driver_16bpp_rotated_pixel_write(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color);
void     _gx_display_driver_565rgb_rotated_pixel_blend(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
void     _gx_display_driver_16bpp_rotated_simple_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, INT xend, INT yend);
void     _gx_display_driver_16bpp_rotated_horizontal_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, INT width, GX_COLOR color);
void     _gx_display_driver_16bpp_rotated_vertical_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x, INT width, GX_COLOR color);
void     _gx_display_driver_16bpp_rotated_horizontal_pattern_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y);
void     _gx_display_driver_16bpp_rotated_vertical_pattern_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x);
void     _gx_display_driver_565rgb_rotated_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info);
void     _gx_display_driver_565rgb_rotated_pixelmap_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_565rgb_rotated_pixelmap_blend(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap, GX_UBYTE alpha);
void     _gx_display_driver_565rgb_rotated_pixelmap_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT rot_cx, INT rot_cy);
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
void     _gx_display_driver_565rgb_rotated_jpeg_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_565rgb_rotated_png_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
#endif
void     _gx_display_driver_565rgb_rotated_glyph_1bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);


void     _gx_display_driver_4444argb_canvas_blend(GX_CANVAS* canvas, GX_CANVAS* composite);
GX_COLOR _gx_display_driver_4444argb_native_color_get(GX_DISPLAY* display, GX_COLOR rawcolor);
void     _gx_display_driver_4444argb_pixel_blend(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
void     _gx_display_driver_4444argb_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info);
void     _gx_display_driver_4444argb_pixelmap_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_4444argb_pixelmap_blend(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, GX_UBYTE alpha);
void     _gx_display_driver_4444argb_pixelmap_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT rot_cx, INT rot_cy);


void     _gx_display_driver_1555xrgb_canvas_blend(GX_CANVAS* canvas, GX_CANVAS* composite);
GX_COLOR _gx_display_driver_1555xrgb_native_color_get(GX_DISPLAY* display, GX_COLOR rawcolor);
void     _gx_display_driver_1555xrgb_pixel_blend(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
void     _gx_display_driver_1555xrgb_pixelmap_blend(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, GX_UBYTE alpha);
void     _gx_display_driver_1555xrgb_pixelmap_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_1555xrgb_pixelmap_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT rot_cx, INT rot_cy);
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
void     _gx_display_driver_1555xrgb_jpeg_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_1555xrgb_png_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
#endif


void     _gx_display_driver_8bpp_canvas_copy(GX_CANVAS* source, GX_CANVAS* dest);
GX_COLOR _gx_display_driver_8bit_palette_native_color_get(GX_DISPLAY*, GX_COLOR rawcolor);
USHORT   _gx_display_driver_8bpp_row_pitch_get(USHORT width);
void     _gx_display_driver_8bpp_block_move(GX_DRAW_CONTEXT* context, GX_RECTANGLE* src, INT xshift, INT yshift);
void     _gx_display_driver_8bpp_pixel_write(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color);
void     _gx_display_driver_8bpp_simple_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, INT xend, INT yend);
void     _gx_display_driver_8bpp_horizontal_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, INT width, GX_COLOR color);
void     _gx_display_driver_8bpp_vertical_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x, INT width, GX_COLOR color);
void     _gx_display_driver_8bpp_horizontal_pattern_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y);
void     _gx_display_driver_8bpp_vertical_pattern_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x);
void     _gx_display_driver_8bpp_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info);
void     _gx_display_driver_8bpp_pixelmap_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_8bpp_pixelmap_blend(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap, GX_UBYTE alpha);
void     _gx_display_driver_8bpp_pixelmap_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT rot_cx, INT rot_cy);
void     _gx_display_driver_8bpp_pixelmap_simple_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT cx, INT cy);
void     _gx_display_driver_8bpp_glyph_4bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);
void     _gx_display_driver_8bpp_glyph_3bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);
void     _gx_display_driver_8bpp_glyph_1bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);


void     _gx_display_driver_8bpp_rotated_canvas_copy(GX_CANVAS* source, GX_CANVAS* dest);
void     _gx_display_driver_8bpp_rotated_block_move(GX_DRAW_CONTEXT* context, GX_RECTANGLE* src, INT xshift, INT yshift);
void     _gx_display_driver_8bpp_rotated_pixel_write(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color);
void     _gx_display_driver_8bpp_rotated_simple_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, INT xend, INT yend);
void     _gx_display_driver_8bpp_rotated_horizontal_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, INT width, GX_COLOR color);
void     _gx_display_driver_8bpp_rotated_vertical_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x, INT width, GX_COLOR color);
void     _gx_display_driver_8bpp_rotated_horizontal_pattern_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y);
void     _gx_display_driver_8bpp_rotated_vertical_pattern_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x);
void     _gx_display_driver_8bpp_rotated_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info);
void     _gx_display_driver_8bpp_rotated_pixelmap_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_8bpp_rotated_pixelmap_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT cx, INT cy);
void     _gx_display_driver_8bpp_rotated_glyph_4bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);
void     _gx_display_driver_8bpp_rotated_glyph_3bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);
void     _gx_display_driver_8bpp_rotated_glyph_1bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);


GX_COLOR _gx_display_driver_332rgb_native_color_get(GX_DISPLAY* display, GX_COLOR rawcolor);
void     _gx_display_driver_332rgb_pixel_blend(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
void     _gx_display_driver_332rgb_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info);
void     _gx_display_driver_332rgb_pixelmap_draw(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_332rgb_pixelmap_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT rot_cx, INT rot_cy);


void     _gx_display_driver_4bpp_canvas_copy(GX_CANVAS* source, GX_CANVAS* dest);
GX_COLOR _gx_display_driver_4bpp_native_color_get(GX_DISPLAY*, GX_COLOR rawcolor);
USHORT   _gx_display_driver_4bpp_row_pitch_get(USHORT width);
void     _gx_display_driver_4bpp_block_move(GX_DRAW_CONTEXT* context, GX_RECTANGLE* src, INT xshift, INT yshift);
void     _gx_display_driver_4bpp_pixel_write(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color);
void     _gx_display_driver_4bpp_simple_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, INT xend, INT yend);
void     _gx_display_driver_4bpp_horizontal_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, INT width, GX_COLOR color);
void     _gx_display_driver_4bpp_vertical_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x, INT width, GX_COLOR color);
void     _gx_display_driver_4bpp_horizontal_pattern_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y);
void     _gx_display_driver_4bpp_vertical_pattern_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x);
void     _gx_display_driver_4bpp_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info);
void     _gx_display_driver_4bpp_pixelmap_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_4bpp_pixelmap_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT rot_cx, INT rot_cy);
void     _gx_display_driver_4bpp_glyph_4bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);
void     _gx_display_driver_4bpp_glyph_1bit_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);


void     _gx_display_driver_1bpp_canvas_copy(GX_CANVAS* source, GX_CANVAS* dest);
GX_COLOR _gx_display_driver_1bpp_native_color_get(GX_DISPLAY*, GX_COLOR rawcolor);
USHORT   _gx_display_driver_1bpp_row_pitch_get(USHORT width);
void     _gx_display_driver_1bpp_block_move(GX_DRAW_CONTEXT* context, GX_RECTANGLE* src, INT xshift, INT yshift);
void     _gx_display_driver_1bpp_pixel_write(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color);
void     _gx_display_driver_1bpp_simple_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, INT xend, INT yend);
void     _gx_display_driver_1bpp_horizontal_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, INT width, GX_COLOR color);
void     _gx_display_driver_1bpp_vertical_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x, INT width, GX_COLOR color);
void     _gx_display_driver_1bpp_horizontal_pattern_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y);
void     _gx_display_driver_1bpp_vertical_pattern_line_draw(GX_DRAW_CONTEXT* context, INT ystart, INT yend, INT x);
void     _gx_display_driver_1bpp_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info);
void     _gx_display_driver_1bpp_pixelmap_draw(GX_DRAW_CONTEXT* context, INT xstart, INT ystart, GX_PIXELMAP* pixelmap);
void     _gx_display_driver_1bpp_pixelmap_rotate(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, INT angle, INT rot_cx, INT rot_cy);
void     _gx_display_driver_1bpp_glyph_1bpp_draw(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);


/* Setup */

typedef void GX_CANVAS_TOGGLE_FUNCTION(GX_CANVAS* canvas, GX_RECTANGLE* dirty_area);

void _gx_display_driver_32argb_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);
void _gx_display_driver_32argb_rotated_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);

void _gx_display_driver_24xrgb_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);
void _gx_display_driver_24xrgb_rotated_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);

void _gx_display_driver_565rgb_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);
void _gx_display_driver_565rgb_rotated_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);

void _gx_display_driver_4444argb_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);

void _gx_display_driver_1555xrgb_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);

void _gx_display_driver_8bit_palette_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);
void _gx_display_driver_8bit_palette_rotated_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);

void _gx_display_driver_332rgb_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);

void _gx_display_driver_4bpp_grayscale_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);

void _gx_display_driver_monochrome_setup(GX_DISPLAY* display, void* aux_data, GX_CANVAS_TOGGLE_FUNCTION* toggle_function);


#include "gx_color.h"

#define GX_SET_BLEND_FUNCTION(blend_func, color_format)     \
    switch (color_format)                                   \
    {                                                       \
    case GX_COLOR_FORMAT_565RGB:                            \
        blend_func = _gx_display_driver_565rgb_pixel_blend; \
        break;                                              \
    case GX_COLOR_FORMAT_24XRGB:                            \
        blend_func = _gx_display_driver_24xrgb_pixel_blend; \
        break;                                              \
    case GX_COLOR_FORMAT_32ARGB:                            \
        blend_func = _gx_display_driver_32argb_pixel_blend; \
        break;                                              \
    default:                                                \
        /* Not supported. */                                \
        return;                                             \
    }

#define GX_SET_32BPP_BLEND_FUNCTION(blend_func, color_format) \
    switch (color_format)                                     \
    {                                                         \
    case GX_COLOR_FORMAT_24XRGB:                              \
        blend_func = _gx_display_driver_24xrgb_pixel_blend;   \
        break;                                                \
    case GX_COLOR_FORMAT_32ARGB:                              \
        blend_func = _gx_display_driver_32argb_pixel_blend;   \
        break;                                                \
    default:                                                  \
        /* Not supported. */                                  \
        return;                                               \
    }

#if defined GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
#define GX_CALCULATE_PUTROW(putrow, x, y, context)          \
    putrow += (y - context->offset_y) * context->pitch;     \
    putrow += (x - context->offset_x)
#else
#define GX_CALCULATE_PUTROW(putrow, x, y, context)          \
    putrow += y * context->pitch;                           \
    putrow += x
#endif

#endif
