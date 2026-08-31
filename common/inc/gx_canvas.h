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
/**   Canvas Management (Canvas)                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_canvas.h                                                         */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX canvas component, including all data     */
/*    types and external references.  It is assumed that gx_api.h and     */
/*    gx_port.h have already been included.                               */
/*                                                                        */
/**************************************************************************/

#ifndef GX_CANVAS_H
#define GX_CANVAS_H

#include "gx_dirty_area.h"

struct GX_GLYPH;
struct GX_PIXELMAP;
struct GX_DISPLAY;
struct GX_DRAW_CONTEXT;

#if defined(GX_MOUSE_SUPPORT)
struct GX_MOUSE_CURSOR_INFO;
#endif

struct GX_CANVAS
{
    // Define the canvas ID and name.
    // The canvas ID is used simply to see if the control block is valid.
    ULONG           id;
    const GX_CHAR*  name;

    // Define pointers to the next and previous canvases in the created list.
    GX_CANVAS* created_next;
    GX_CANVAS* created_previous;

    // Define the canvas memory pointer.
    GX_COLOR* memory;
#ifdef GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
    GX_VALUE memory_width;
    GX_VALUE memory_height;
    GX_VALUE memory_offset_x;
    GX_VALUE memory_offset_y;
#endif

    // padded canvas memory buffer, only needed for Win32
#ifdef GX_TARGET_WIN32
    GX_COLOR* padded_memory;
#endif

    // Define the size of the canvas memory.
    ULONG memory_size;

    // Which display driver is used to paint on this canvas.
    GX_DISPLAY*   display;

    // Prior to drawing on a canvas, we must know what portion needs to be updated
    GX_RECTANGLE  dirty_area;
    GX_DIRTY_AREA dirty_list[GX_MAX_DIRTY_AREAS];
    UINT          dirty_count;
    UINT          draw_count;
    UINT          status;

    GX_VALUE      x_resolution;
    GX_VALUE      y_resolution;
    GX_VALUE      display_offset_x;
    GX_VALUE      display_offset_y;

    GX_UBYTE      alpha;
    GX_UBYTE      draw_nesting;
    GX_BYTE       hardware_layer;

    void display_area_(GX_RECTANGLE* area)
    {
        area->left = display_offset_x;
        area->top = display_offset_y;
        area->right = (GX_VALUE)(display_offset_x + x_resolution - 1);
        area->bottom = (GX_VALUE)(display_offset_y + y_resolution - 1);
    }

    UINT create_(const GX_CHAR* name, GX_DISPLAY* display,
                 UINT type, UINT width, UINT height,
                 GX_COLOR* memory_area, ULONG memory_size);
    UINT delete_();

    UINT memory_set_(GX_COLOR* memory, ULONG memsize);
    UINT offset_set_(GX_VALUE x_offset, GX_VALUE y_offset);
    UINT offset_set_(const GX_POINT& offset);
    UINT alpha_set_(GX_UBYTE alpha);

    UINT hardware_layer_bind_(INT layer);

    UINT shift_(GX_VALUE dx, GX_VALUE dy);

    void dirty_mark_(const GX_RECTANGLE* area = GX_NULL);

    bool dirty_list_is_full_() const { return dirty_count >= GX_MAX_DIRTY_AREAS; }

    bool dirty_list_add_(GX_WIDGET* widget, const GX_RECTANGLE& dirty_rect);
    void dirty_list_entry_shift_(GX_WIDGET* widget, INT dx, INT dy);
    void dirty_list_remove_(GX_WIDGET* removed);
    bool dirty_list_trim_(GX_RECTANGLE* return_dirty_rect);

    UINT drawing_initiate_(GX_WIDGET* who, const GX_RECTANGLE& dirty_rect);
    UINT drawing_complete_(GX_BOOL flush);

    UINT show_();
    UINT hide_();

#if defined(GX_MOUSE_SUPPORT)
    UINT mouse_define_(GX_MOUSE_CURSOR_INFO* info);
    UINT mouse_show_();
    UINT mouse_hide_();
#endif

    void clamp_(GX_RECTANGLE* rect) const;
};

inline UINT GX_CANVAS::offset_set_(const GX_POINT& offset)
{
    return offset_set_(offset.x, offset.y);
}

struct GX_DRAWING
{
    GX_DRAWING(GX_CANVAS* canvas, GX_WIDGET* who, const GX_RECTANGLE& dirty_area, GX_BOOL flush = GX_FALSE)
    {
        this->result = canvas->drawing_initiate_(who, dirty_area);
        this->canvas = canvas;
        this->flush = flush;
    }

    ~GX_DRAWING()
    {
        if (result == GX_SUCCESS || result == GX_NO_VIEWS)
        {
            canvas->drawing_complete_(flush);
        }
    }

    UINT result_() const { return result; }

    void enable_flush_(GX_BOOL flush) { this->flush = flush; }

private:
    UINT result;
    GX_CANVAS* canvas;
    GX_BOOL flush;
};

#define GX_CANVAS_ID ((ULONG)0x43414E56)

GX_CANVAS* _gx_canvas_composite_create(void);
UINT _gx_canvas_rotated_pixelmap_bound_calculate(GX_RECTANGLE* rect, INT angle, INT rot_cx, INT rot_cy);

UINT _gx_canvas_alpha_set(GX_CANVAS* canvas, GX_UBYTE alpha);
UINT _gx_canvas_arc_draw(INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
UINT _gx_canvas_block_move(GX_RECTANGLE* block, GX_VALUE dx, GX_VALUE dy, GX_RECTANGLE* dirty);
UINT _gx_canvas_circle_draw(INT xcenter, INT ycenter, UINT r);

UINT _gx_canvas_create(GX_CANVAS* canvas, const GX_CHAR* name, GX_DISPLAY* display,
                       UINT type, UINT width, UINT height, GX_COLOR* memory_area, ULONG memory_size);
UINT _gx_canvas_delete(GX_CANVAS* canvas);

UINT _gx_canvas_drawing_complete(GX_CANVAS* canvas, GX_BOOL Flush);
UINT _gx_canvas_drawing_initiate(GX_CANVAS* canvas, GX_WIDGET* who, GX_RECTANGLE* dirty_area);

UINT _gx_canvas_ellipse_draw(INT xcenter, INT ycenter, INT a, INT b);

UINT _gx_canvas_hardware_layer_bind(GX_CANVAS* canvas, INT layer);
UINT _gx_canvas_hide(GX_CANVAS* canvas);
UINT _gx_canvas_line_draw(GX_VALUE x_start, GX_VALUE y_start, GX_VALUE x_end, GX_VALUE y_end);
UINT _gx_canvas_memory_define(GX_CANVAS* canvas, GX_COLOR* memory, ULONG memsize);

#if defined(GX_MOUSE_SUPPORT)
UINT _gx_canvas_mouse_define(GX_CANVAS* canvas, GX_MOUSE_CURSOR_INFO* info);
UINT _gx_canvas_mouse_show(GX_CANVAS* canvas);
UINT _gx_canvas_mouse_hide(GX_CANVAS* canvas);
#endif

UINT _gx_canvas_offset_set(GX_CANVAS* canvas, GX_VALUE x, GX_VALUE y);
UINT _gx_canvas_pixel_draw(GX_POINT position);
UINT _gx_canvas_pixelmap_blend(GX_VALUE x, GX_VALUE y, GX_PIXELMAP* pixelmap, GX_UBYTE alpha);
UINT _gx_canvas_pixelmap_draw(GX_VALUE x, GX_VALUE y, GX_PIXELMAP* pixelmap);
UINT _gx_canvas_pixelmap_get(GX_PIXELMAP* pixelmap);
UINT _gx_canvas_pixelmap_tile(GX_RECTANGLE* fill, GX_PIXELMAP* pixelmap);
UINT _gx_canvas_pixelmap_rotate(GX_VALUE x, GX_VALUE y, GX_PIXELMAP* pixelmap, INT angle, INT rot_cx, INT rot_cy);
UINT _gx_canvas_polygon_draw(GX_POINT* point_array, INT number_of_points);
UINT _gx_canvas_pie_draw(INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
UINT _gx_canvas_rectangle_draw(GX_RECTANGLE* rectangle);
UINT _gx_canvas_rotated_text_draw_ext(const GX_STRING* text, GX_VALUE xcenter, GX_VALUE ycenter, INT angle);
UINT _gx_canvas_shift(GX_CANVAS* canvas, GX_VALUE dx, GX_VALUE dy);
UINT _gx_canvas_show(GX_CANVAS* canvas);
UINT _gx_canvas_aligned_text_draw(const GX_STRING* text, GX_RECTANGLE* rectangle, ULONG alignment);
UINT _gx_canvas_text_draw_ext(GX_VALUE x, GX_VALUE y, const GX_STRING* text);


// Define error checking shells for API services.  These are only referenced by the application.

UINT _gxe_canvas_alpha_set(GX_CANVAS* canvas, GX_UBYTE alpha);
UINT _gxe_canvas_arc_draw(INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
UINT _gxe_canvas_block_move(GX_RECTANGLE* block, GX_VALUE dx, GX_VALUE dy, GX_RECTANGLE* dirty);
UINT _gxe_canvas_circle_draw(INT xcenter, INT ycenter, UINT r);

UINT _gxe_canvas_create(GX_CANVAS* canvas, const GX_CHAR* name, GX_DISPLAY* display, UINT type, UINT width,
                        UINT height, GX_COLOR* memory_area, ULONG memory_size, UINT canvas_control_block_size);
UINT _gxe_canvas_delete(GX_CANVAS* canvas);

UINT _gxe_canvas_drawing_complete(GX_CANVAS* canvas, GX_BOOL Flush);
UINT _gxe_canvas_drawing_initiate(GX_CANVAS* canvas, GX_WIDGET* who, GX_RECTANGLE* dirty_area);

UINT _gxe_canvas_ellipse_draw(INT xcenter, INT ycenter, INT a, INT b);

UINT _gxe_canvas_hardware_layer_bind(GX_CANVAS* canvas, INT layer);
UINT _gxe_canvas_hide(GX_CANVAS* canvas);
UINT _gxe_canvas_line_draw(GX_VALUE x_start, GX_VALUE y_start, GX_VALUE x_end, GX_VALUE y_end);
UINT _gxe_canvas_memory_define(GX_CANVAS* canvas, GX_COLOR* memory, ULONG memsize);

#if defined(GX_MOUSE_SUPPORT)
UINT _gxe_canvas_mouse_define(GX_CANVAS* canvas, GX_MOUSE_CURSOR_INFO* info);
UINT _gxe_canvas_mouse_show(GX_CANVAS* canvas);
UINT _gxe_canvas_mouse_hide(GX_CANVAS* canvas);
#endif

UINT _gxe_canvas_offset_set(GX_CANVAS* canvas, GX_VALUE x, GX_VALUE y);
UINT _gxe_canvas_pie_draw(INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
UINT _gxe_canvas_pixel_draw(GX_POINT position);
UINT _gxe_canvas_pixelmap_blend(GX_VALUE x, GX_VALUE y, GX_PIXELMAP* pixelmap, GX_UBYTE alpha);
UINT _gxe_canvas_pixelmap_draw(GX_VALUE x, GX_VALUE y, GX_PIXELMAP* pixelmap);
UINT _gxe_canvas_pixelmap_get(GX_PIXELMAP* pixelmap);
UINT _gxe_canvas_pixelmap_tile(GX_RECTANGLE* fill, GX_PIXELMAP* pixelmap);
UINT _gxe_canvas_pixelmap_rotate(GX_VALUE x, GX_VALUE y, GX_PIXELMAP* pixelmap, INT angle, INT rot_cx, INT rot_cy);
UINT _gxe_canvas_polygon_draw(GX_POINT* point_array, INT number_of_points);
UINT _gxe_canvas_rectangle_draw(GX_RECTANGLE* rectangle);
UINT _gxe_canvas_rotated_text_draw_ext(const GX_STRING* text, GX_VALUE xcenter, GX_VALUE ycenter, INT angle);
UINT _gxe_canvas_shift(GX_CANVAS* canvas, GX_VALUE x, GX_VALUE y);
UINT _gxe_canvas_show(GX_CANVAS* canvas);
UINT _gxe_canvas_aligned_text_draw(const GX_STRING* string, GX_RECTANGLE* rectangle, ULONG alignment);
UINT _gxe_canvas_text_draw_ext(GX_VALUE x, GX_VALUE y, const GX_STRING* string);

#ifdef GX_TARGET_WIN32
void _win32_compatible_canvas_memory_allocate(GX_CANVAS* canvas);
#endif

#endif

