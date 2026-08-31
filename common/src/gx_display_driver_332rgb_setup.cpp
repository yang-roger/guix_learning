
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
/*    _gx_display_driver_332rgb_setup                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 8-bit 332RGB color format display driver setup routine.     */
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
void _gx_display_driver_332rgb_setup(GX_DISPLAY *display, void *aux_data,
                                     void (*toggle_function)(GX_CANVAS *canvas,
                                                             GX_RECTANGLE *dirty_area))
{
    _gx_display_driver_8bit_palette_setup(display, aux_data, toggle_function);

    display->color_format                         = GX_COLOR_FORMAT_8BIT_PACKED_PIXEL;
    display->driver_native_color_get              = _gx_display_driver_332rgb_native_color_get;
    display->driver_pixelmap_draw                 = _gx_display_driver_332rgb_pixelmap_draw;
    display->driver_alphamap_draw                 = _gx_display_driver_generic_alphamap_draw;
    display->driver_anti_aliased_line_draw        = _gx_display_driver_generic_aliased_line_draw;
    display->driver_anti_aliased_wide_line_draw   = _gx_display_driver_generic_aliased_wide_line_draw;

#if defined(GX_ARC_DRAWING_SUPPORT)
    display->driver_anti_aliased_circle_draw      = _gx_display_driver_generic_aliased_circle_draw;
    display->driver_anti_aliased_ellipse_draw     = _gx_display_driver_generic_aliased_ellipse_draw;
    display->driver_anti_aliased_arc_draw         = _gx_display_driver_generic_aliased_arc_draw;
    display->driver_anti_aliased_wide_circle_draw = _gx_display_driver_generic_aliased_wide_circle_draw;
    display->driver_anti_aliased_wide_ellipse_draw = _gx_display_driver_generic_aliased_wide_ellipse_draw;
    display->driver_anti_aliased_wide_arc_draw    = _gx_display_driver_generic_aliased_wide_arc_draw;

#endif

    display->driver_horizontal_pixelmap_line_draw = _gx_display_driver_332rgb_horizontal_pixelmap_line_draw;
    display->driver_pixel_blend                   = _gx_display_driver_332rgb_pixel_blend;
    display->driver_pixelmap_blend                = _gx_display_driver_8bpp_pixelmap_blend;
    display->driver_pixelmap_rotate               = _gx_display_driver_332rgb_pixelmap_rotate;
    display->driver_4bit_glyph_draw               = _gx_display_driver_generic_glyph_4bit_draw;
    display->driver_8bit_glyph_draw               = GX_NULL;
    display->driver_8bit_compressed_glyph_draw    = GX_NULL;
    display->driver_4bit_compressed_glyph_draw    = GX_NULL;
    display->driver_1bit_compressed_glyph_draw    = GX_NULL;
}

