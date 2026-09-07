
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

#include "gx_display.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_rotated_setup                             */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic rotated 32-bit XRGB color format display driver setup       */
/*    routine.                                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               The display control block     */
/*    aux_data                              Driver-defined auxiliary data */
/*    toggle_function                       Driver-defined screen toggle  */
/*                                            function                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_24xrgb_rotated_setup(GX_DISPLAY *display, void *aux_data,
                                             void (*toggle_function)(GX_CANVAS *canvas,
                                                                     GX_RECTANGLE *dirty_area))
{
    _gx_display_driver_24xrgb_setup(display, aux_data, toggle_function);

#if defined(GX_MOUSE_SUPPORT)
    display->driver_drawing_initiate = GX_NULL;
    display->driver_drawing_complete = GX_NULL;
    display->mouse_position_set = GX_NULL;
    display->mouse_enable = GX_NULL;
#if !defined(GX_HARDWARE_MOUSE_SUPPORT)
    display->mouse_capture = GX_NULL;
    display->mouse_restore = GX_NULL;
    display->mouse_draw = GX_NULL;
#endif
#endif

    display->rotation_angle                       = GX_SCREEN_ROTATION_CW;
    display->driver_canvas_copy                   = _gx_display_driver_32bpp_rotated_canvas_copy;
    display->driver_simple_line_draw              = _gx_display_driver_32bpp_rotated_simple_line_draw;
    display->driver_horizontal_line_draw          = _gx_display_driver_32bpp_rotated_horizontal_line_draw;
    display->driver_vertical_line_draw            = _gx_display_driver_32bpp_rotated_vertical_line_draw;
    display->driver_horizontal_pattern_line_draw  = _gx_display_driver_32bpp_rotated_horizontal_pattern_line_draw;
    display->driver_vertical_pattern_line_draw    = _gx_display_driver_32bpp_rotated_vertical_pattern_line_draw;
    display->driver_horizontal_pixelmap_line_draw = _gx_display_driver_32bpp_rotated_horizontal_pixelmap_line_draw;
    display->driver_pixel_write                   = _gx_display_driver_32bpp_rotated_pixel_write;
    display->driver_block_move                    = _gx_display_driver_32bpp_rotated_block_move;

    display->driver_pixelmap_draw                 = _gx_display_driver_32bpp_rotated_pixelmap_draw;
    display->driver_pixelmap_rotate               = _gx_display_driver_32bpp_rotated_pixelmap_rotate;
    display->driver_alphamap_draw                 = _gx_display_driver_generic_rotated_alphamap_draw;
    display->driver_polygon_fill                  = _gx_display_driver_generic_rotated_polygon_fill;

#if defined(GX_ARC_DRAWING_SUPPORT)
    display->driver_circle_fill                   = _gx_display_driver_generic_rotated_circle_fill;
    display->driver_pie_fill                      = _gx_display_driver_generic_rotated_pie_fill;
    display->driver_arc_fill                      = _gx_display_driver_generic_rotated_arc_fill;
    display->driver_ellipse_fill                  = _gx_display_driver_generic_rotated_ellipse_fill;
#endif

    display->driver_palette_set                   = GX_NULL;
    display->driver_buffer_toggle                 = toggle_function;

    display->driver_canvas_blend                  = _gx_display_driver_24xrgb_rotated_canvas_blend;
    display->driver_pixel_blend                   = _gx_display_driver_24xrgb_rotated_pixel_blend;
    display->driver_pixelmap_blend                = _gx_display_driver_32bpp_rotated_pixelmap_blend;

    display->driver_8bit_glyph_draw               = _gx_display_driver_generic_rotated_glyph_8bit_draw;
    display->driver_4bit_glyph_draw               = _gx_display_driver_generic_rotated_glyph_4bit_draw;
    display->driver_1bit_glyph_draw               = _gx_display_driver_32bpp_rotated_glyph_1bit_draw;

    display->driver_8bit_compressed_glyph_draw    = GX_NULL;
    display->driver_4bit_compressed_glyph_draw    = GX_NULL;
    display->driver_1bit_compressed_glyph_draw    = GX_NULL;

#if defined(GX_SOFTWARE_DECODER_SUPPORT)
    display->driver_jpeg_draw                     = _gx_display_driver_24xrgb_rotated_jpeg_draw;
    display->driver_png_draw                      = _gx_display_driver_24xrgb_rotated_png_draw;
#endif
}

