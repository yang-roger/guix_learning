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
/**   Display Management (Display)                                        */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_display.h                                                        */
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

#ifndef GX_DISPLAY_H
#define GX_DISPLAY_H

#include "gx_fixed_val.h"
#include "gx_point.h"
#include "gx_rectangle.h"
#include "gx_glyph.h"

struct GX_FONT;
struct GX_PIXELMAP;
struct GX_THEME;
struct GX_CANVAS;
struct GX_DRAW_CONTEXT;
struct GX_FILL_PIXELMAP_INFO;

/* Display layer services structure type. These services may be provided by display drivers
   support multiple hardware graphics layers. */
struct GX_DISPLAY_LAYER_SERVICES
{
    UINT (*initialize)(INT layer, GX_CANVAS* canvas);
    void (*show)(INT layer);
    void (*hide)(INT layer);
    void (*alpha_set)(INT layer, GX_UBYTE alpha);
    void (*offset_set)(INT layer, GX_VALUE xoffset, GX_VALUE yoffset);
    void (*active_display_area_set)(INT layer, GX_RECTANGLE* size);
};

#if defined(GX_MOUSE_SUPPORT)

struct GX_MOUSE_CURSOR_INFO
{
    GX_RESOURCE_ID            image_id;
    GX_VALUE                  hotspot_x;
    GX_VALUE                  hotspot_y;
};

struct GX_DISPLAY_MOUSE
{
    GX_MOUSE_CURSOR_INFO*     cursor_info;
    GX_CANVAS*                canvas;
    GX_POINT                  position;
    GX_UBYTE                  status;

#if !defined(GX_HARDWARE_MOUSE_SUPPORT)
    GX_RECTANGLE              rect;
    GX_UBYTE*                 capture_memory;
#endif
};

#endif /* GX_MOUSE_SUPPORT */

/* Define the display control block structure.  */
struct GX_DISPLAY
{
    /* Define the display ID and name. The display ID is used simply to see if the
       control block is valid.  */
    ULONG                     id;                   /* control block ID GX_DISPLAY_ID           */
    ULONG                     handle;               /* used to identify unique display instance */
    const GX_CHAR*            name;

    GX_DISPLAY*               created_next;         /* pointer to next control block            */
    GX_DISPLAY*               created_previous;     /* pointer to previous control block        */

    GX_COLOR*                 color_table;          /* color ID to native value mapping table   */
    GX_PIXELMAP**             pixelmap_table;       /* pixelmap ID to GX_PIXELMAP mapping table */
    GX_FONT**                 font_table;           /* font ID to GX_FONT mapping table         */
    GX_COLOR*                 palette;              /* only used for 8-bpp palette mode driver  */

    const GX_STRING**         language_table;       /* language table.                          */
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    const GX_UBYTE*           language_direction_table; /* language direction table.            */
#endif
    UINT                      color_table_size;     /* number of colors                         */
    UINT                      pixelmap_table_size;  /* number of pixelmaps                      */
    UINT                      font_table_size;      /* number of fonts                          */
    UINT                      string_table_size;    /* number of strings per language           */
    UINT                      palette_size;         /* number of colors in palette              */

    GX_UBYTE                  color_format;
    GX_UBYTE                  active_language;      /* active language.                         */
    GX_UBYTE                  language_table_size;  /* number of language                       */
    GX_UBYTE                  driver_ready;
    USHORT                    rotation_angle;

    GX_VALUE                  width;
    GX_VALUE                  height;

    void*                     driver_data;
    void*                     accelerator;           /* graphics accelerator handle/instance */

    GX_DISPLAY_LAYER_SERVICES* layer_services;       /* optional additional hardware graphics layer services */

    /* Function for installing palette (only used for certain palette mode drivers) */
    void (*driver_palette_set)(GX_DISPLAY* display, GX_COLOR* palette, INT count);

    /* Function to initiate drawing sequence */
    void (*driver_drawing_initiate)(GX_DISPLAY* display, GX_CANVAS* canvas);

    /* Function to terminate drawing sequence */
    void (*driver_drawing_complete)(GX_DISPLAY* display, GX_CANVAS* canvas);

    /* Define driver function pointer for low-level buffer toggle.  */
    void (*driver_buffer_toggle)(GX_CANVAS* canvas, GX_RECTANGLE* dirty_area);

    /* Define driver function pointers for canvas composite */
    void (*driver_canvas_copy)(GX_CANVAS* source, GX_CANVAS* dest);

    /* Define driver function pointers for canvas composite */
    void (*driver_canvas_blend)(GX_CANVAS* source, GX_CANVAS* dest);

    /* Define driver function pointer to convert 32-bit color to native format.  */
    GX_COLOR (*driver_native_color_get)(GX_DISPLAY* display, GX_COLOR rawcolor);

    /* Define driver function pointer to return row pitch, in bytes, for given canvas width.  */
    USHORT (*driver_row_pitch_get)(USHORT width);

    /* Define driver function for block move. */
    void (*driver_block_move)(GX_DRAW_CONTEXT* context, GX_RECTANGLE* block, INT xshift, INT yshift);

    /* Define driver function pointer for low-level pixel writing.  */
    void (*driver_pixel_write)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color);

    /* Define driver function pointer for low-level pixel blending.  */
    void (*driver_pixel_blend)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    /* Function for drawing non-aliased, single pixel line */
    void (*driver_simple_line_draw)(GX_DRAW_CONTEXT* context, INT x1, INT y1, INT x2, INT y2);

    /* Function for drawing non-aliased, wide line */
    void (*driver_simple_wide_line_draw)(GX_DRAW_CONTEXT* context, INT x1, INT y1, INT x2, INT y2);

    /* Function for drawing anti-aliased aliased, single-pixel line */
    void (*driver_anti_aliased_line_draw)(GX_DRAW_CONTEXT* context, INT x1, INT y1, INT x2, INT y2);

    /* Function for drawing anti-aliased aliased, wide line */
    void (*driver_anti_aliased_wide_line_draw)(GX_DRAW_CONTEXT* context, INT x1, INT y1, INT x2, INT y2);

    void (*driver_horizontal_line_draw)(GX_DRAW_CONTEXT* context, INT x1, INT x2, INT y, INT width, GX_COLOR color);
    void (*driver_vertical_line_draw)(GX_DRAW_CONTEXT* context, INT y1, INT y2, INT x, INT width, GX_COLOR color);
    void (*driver_horizontal_pattern_line_draw)(GX_DRAW_CONTEXT* context, INT x1, INT x2, INT y);
    void (*driver_vertical_pattern_line_draw)(GX_DRAW_CONTEXT* context, INT y1, INT y2, INT x);
    void (*driver_horizontal_pixelmap_line_draw)(GX_DRAW_CONTEXT* context, INT xstart, INT xend, INT y, GX_FILL_PIXELMAP_INFO* info);

    /* Define driver function pointers for pixelmap drawing */
    void (*driver_pixelmap_draw)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
    void (*driver_pixelmap_blend)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap, GX_UBYTE alpha);
    void (*driver_alphamap_draw)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
    /* Driver function for decode raw jpg, png directly to frame buffer */
    void (*driver_jpeg_draw)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
    void (*driver_png_draw)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap);
#endif

    void (*driver_pixelmap_rotate)(GX_DRAW_CONTEXT* context, INT x, INT y, GX_PIXELMAP* pixelmap,
                                   INT angle, INT rot_cx, INT rot_cy);

    /* Define driver function pointer for drawing polygon.  */
    void (*driver_polygon_draw)(GX_DRAW_CONTEXT* context, GX_POINT* vertex, INT num);

    /* Define driver function pointer for filling polygon shape.  */
    void (*driver_polygon_fill)(GX_DRAW_CONTEXT* context, GX_POINT* vertex, INT num);

    /* Define driver function pointer for drawing aliased 8bit glyph (may be NULL).  */
    void (*driver_8bit_glyph_draw)(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);

    /* Define driver function pointer for drawing aliased 4bit glyph (may be NULL).  */
    void (*driver_4bit_glyph_draw)(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);

    /* Define driver function pointer for drawing 1bit (monochrome) glyph.  */
    void (*driver_1bit_glyph_draw)(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);

    /* Define driver function pointer for drawing aliased 8bit compressed glyph (may be NULL).  */
    void (*driver_8bit_compressed_glyph_draw)(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);

    /* Define driver function pointer for drawing aliased 4bit compressed glyph (may be NULL).  */
    void (*driver_4bit_compressed_glyph_draw)(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);

    /* Define driver function pointer for drawing 1bit (monochrome) compressed glyph.  */
    void (*driver_1bit_compressed_glyph_draw)(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area, GX_POINT* map_offset, const GX_GLYPH* glyph);

    void (*driver_callback_assign)(UINT (*wait_func)(void *), void*);

#if defined(GX_ARC_DRAWING_SUPPORT)

    /* Define driver function pointer for drawing circle.  */
    void (*driver_circle_draw)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r);

    /* Define driver function pointer for drawing anti-aliased circle.  */
    void (*driver_anti_aliased_circle_draw)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r);

    /* Define driver function pointer for drawing circle with wide outlines.  */
    void (*driver_wide_circle_draw)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r);

    /* Define driver function pointer for drawing anti-aliased circle with wide outlines.  */
    void (*driver_anti_aliased_wide_circle_draw)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r);

    /* Define driver function pointer for filling circle shape.  */
    void (*driver_circle_fill)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r);

    /* Define driver function pointer for drawing circle arc. */
    void (*driver_arc_draw)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);

    /* Define driver function pointer for drawing anti-aliased circle arc.  */
    void (*driver_anti_aliased_arc_draw)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);

    /* Define driver function pointer for drawing circle arc with wide outlines. */
    void (*driver_wide_arc_draw)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);

    /* Define driver function pointer for drawing anti-aliased circle arc with wide outlines. */
    void (*driver_anti_aliased_wide_arc_draw)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);

    /* Define driver function pointer for filling circle arc shape.  */
    void (*driver_arc_fill)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);

    /* Define driver function pointer for filling pie shape.  */
    void (*driver_pie_fill)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);

    /* Define driver function pointer for drawing ellipse.  */
    void (*driver_ellipse_draw)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, INT a, INT b);

    /* Define driver function pointer for drawing anti-aliased ellipse.  */
    void (*driver_anti_aliased_ellipse_draw)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, INT a, INT b);

    /* Define driver function pointer for drawing ellipse with wide outlines.  */
    void (*driver_wide_ellipse_draw)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, INT a, INT b);

    /* Define driver function pointer for drawing anti-aliased ellipse with wide outlines.  */
    void (*driver_anti_aliased_wide_ellipse_draw)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, INT a, INT b);

    /* Define driver function pointer for drawing a filled ellipse.  */
    void (*driver_ellipse_fill)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, INT a, INT b);

#endif /* GX_ARC_DRAWING_SUPPORT */

#if defined (GX_MOUSE_SUPPORT)
    GX_DISPLAY_MOUSE mouse;
    /* Define driver function pointer for setting pixelmap for mouse. */
    void (*mouse_define)(GX_DISPLAY* display, GX_CANVAS* canvas, GX_MOUSE_CURSOR_INFO* info);
    void (*mouse_position_set)(GX_DISPLAY* display, GX_POINT* pos);
    void (*mouse_enable)(GX_DISPLAY* display, GX_BOOL enable);
#if !defined(GX_HARDWARE_MOUSE_SUPPORT)
    void (*mouse_capture)(GX_DISPLAY* display);
    void (*mouse_restore)(GX_DISPLAY* display);
    void (*mouse_draw)(GX_DISPLAY* display);
#endif /* GX_HARDWARE_MOUSE_SUPPORT */

    void mouse_position_set_run_(GX_POINT* pos)
    {
        if (mouse_position_set)
        {
            mouse_position_set(this, pos);
        }
    }

#endif /* GX_MOUSE_SUPPORT */

    UINT create_(const GX_CHAR* name, UINT (*display_driver_setup)(GX_DISPLAY*), GX_VALUE width, GX_VALUE height);
    UINT delete_(void (*display_driver_cleanup)(GX_DISPLAY*));

    UINT color_get_(GX_RESOURCE_ID color_id, GX_COLOR* return_color);
    UINT color_set_(GX_RESOURCE_ID color_id, GX_COLOR color);
    UINT color_table_set_(GX_COLOR* color_table, INT number_of_colors, bool dirty = true);

    UINT pixelmap_get_(GX_RESOURCE_ID pixelmap_id, GX_PIXELMAP** return_pixelmap);
    UINT pixelmap_table_set_(GX_PIXELMAP** pixelmap_table, UINT number_of_pixelmaps, bool dirty = true);

    UINT font_get_(GX_RESOURCE_ID font_id, GX_FONT** return_font);
    UINT font_table_set_(GX_FONT** font_table, UINT number_of_fonts, bool dirty = true);

    UINT language_table_get_ext_(GX_STRING*** table, GX_UBYTE* language_count, UINT* string_count);
    UINT language_table_set_ext_(const GX_STRING** table, GX_UBYTE num_languages, UINT number_of_strings);

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    UINT language_direction_get_(GX_UBYTE language, GX_UBYTE* language_direction);
    UINT language_direction_table_set_(const GX_UBYTE* language_direction_table, GX_UBYTE num_languages);
#endif

    UINT string_get_ext_(GX_RESOURCE_ID string_id, GX_STRING* return_string);
    UINT string_table_get_ext_(GX_UBYTE language, GX_STRING** table, UINT* size);

    UINT theme_install_(const GX_THEME* theme);

    UINT active_language_set_(GX_UBYTE language);

private:
    void canvas_dirty_();
};

/* Define Display management constants.  */

#define GX_DISPLAY_ID               ((ULONG)0x53435245)
#define GX_MAX_DISPLAY_RESOLUTION   8192

/* Define display management function prototypes.  */

UINT _gx_display_active_language_set(GX_DISPLAY* display, GX_UBYTE language);
UINT _gx_display_create(GX_DISPLAY* display, const GX_CHAR* name, UINT (*display_driver_setup)(GX_DISPLAY*), GX_VALUE width, GX_VALUE height);
UINT _gx_display_delete(GX_DISPLAY* display, void (*display_driver_cleanup)(GX_DISPLAY*));
UINT _gx_display_color_set(GX_DISPLAY* display, GX_RESOURCE_ID id, GX_COLOR color);
UINT _gx_display_color_table_set(GX_DISPLAY* display, GX_COLOR* color_table, INT number_of_colors);
UINT _gx_display_font_table_set(GX_DISPLAY* display, GX_FONT** font_table, UINT number_of_fonts);
UINT _gx_display_language_table_get_ext(GX_DISPLAY* display, GX_STRING*** table, GX_UBYTE* language_count, UINT* string_count);
UINT _gx_display_language_table_set_ext(GX_DISPLAY* display, const GX_STRING** table, GX_UBYTE num_languages, UINT number_of_strings);
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT _gx_display_language_direction_table_set(GX_DISPLAY* display, const GX_UBYTE* language_direction_table, GX_UBYTE num_languages);
#endif
UINT _gx_display_pixelmap_table_set(GX_DISPLAY* display, GX_PIXELMAP** pixelmap_table, UINT number_of_pixelmaps);
UINT _gx_display_string_get_ext(GX_DISPLAY* display, GX_RESOURCE_ID string_id, GX_STRING* return_string);
UINT _gx_display_string_table_get_ext(GX_DISPLAY* display, GX_UBYTE language, GX_STRING** table, UINT* size);
UINT _gx_display_theme_install(GX_DISPLAY* display, const GX_THEME* theme_ptr);

/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_display_active_language_set(GX_DISPLAY* display, GX_UBYTE language);
UINT _gxe_display_create(GX_DISPLAY* display, const GX_CHAR* name, UINT (*display_driver_setup)(GX_DISPLAY*), GX_VALUE width, GX_VALUE height, UINT display_control_block_size);
UINT _gxe_display_color_set(GX_DISPLAY* display, GX_RESOURCE_ID resource_id, GX_COLOR new_color);
UINT _gxe_display_color_table_set(GX_DISPLAY* display, GX_COLOR* color_table, INT number_of_colors);
UINT _gxe_display_delete(GX_DISPLAY* display, void (*display_driver_cleanup)(GX_DISPLAY*));
UINT _gxe_display_font_table_set(GX_DISPLAY* display, GX_FONT** font_table, UINT number_of_fonts);
UINT _gxe_display_language_table_get_ext(GX_DISPLAY* display, GX_STRING*** table, GX_UBYTE* language_count, UINT* string_count);
UINT _gxe_display_language_table_set_ext(GX_DISPLAY* display, const GX_STRING** table, GX_UBYTE num_languages, UINT number_of_strings);
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT _gxe_display_language_direction_table_set(GX_DISPLAY* display, const GX_UBYTE* language_direction_table, GX_UBYTE num_languages);
#endif
UINT _gxe_display_pixelmap_table_set(GX_DISPLAY* display, GX_PIXELMAP** pixelmap_table, UINT number_of_pixelmaps);
UINT _gxe_display_string_get_ext(GX_DISPLAY* display, GX_RESOURCE_ID string_id, GX_STRING* return_string);
UINT _gxe_display_string_table_get_ext(GX_DISPLAY* display, GX_UBYTE language, GX_STRING** table, UINT* size);
UINT _gxe_display_theme_install(GX_DISPLAY* display, const GX_THEME* theme_ptr);

#endif

