
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
/**   Synergy Simulation Display Management (Display)                     */
/**                                                                       */
/**************************************************************************/

#include "gx_dave2d_simulation_display_driver.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_dave2d_s_display_driver_565rgb_rotated_setup                    */
/*                                                           6.4.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 16-bit 565RGB color format display driver setup routine.    */
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

VOID _gx_dave2d_simulation_display_driver_565rgb_rotated_setup(GX_DISPLAY *display, VOID *aux_data,
                                                        VOID (*toggle_function)(GX_CANVAS *canvas,
                                                                                GX_RECTANGLE *dirty_area))
{

    _gx_display_driver_565rgb_rotated_setup(display, aux_data, toggle_function);

    display->driver_pixelmap_draw                 = _gx_dave2d_simulation_display_driver_565rgb_rotated_pixelmap_draw;
    display->driver_pixelmap_blend                = _gx_dave2d_simulation_display_driver_565rgb_rotated_pixelmap_blend;
    display->driver_4bit_glyph_draw               = _gx_dave2d_simulation_display_driver_rotated_glyph_4bit_draw;
    display->driver_1bit_glyph_draw               = _gx_dave2d_simulation_display_driver_16bpp_rotated_glyph_1bit_draw;
    display->driver_8bit_compressed_glyph_draw    = _gx_dave2d_simulation_display_driver_rotated_compressed_glyph_8bit_draw;
    display->driver_4bit_compressed_glyph_draw    = _gx_dave2d_simulation_display_driver_rotated_compressed_glyph_4bit_draw;
    display->driver_1bit_compressed_glyph_draw    = _gx_dave2d_simulation_display_driver_16bpp_rotated_compressed_glyph_1bit_draw;
    display->driver_horizontal_pixelmap_line_draw = _gx_dave2d_simulation_display_driver_565rgb_rotated_horizontal_pixelmap_line_draw;
}

