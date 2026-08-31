
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
/*    _gx_display_driver_4444argb_setup                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 16-bit 4444ARGB color format display driver setup routine.  */
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
/*    _gx_display_driver_565rgb_setup       Basic display driver setup.   */
/*                                            function for generic 565rgb */
/*                                            color format                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_4444argb_setup(GX_DISPLAY *display, void *aux_data,
                                       void (*toggle_function)(GX_CANVAS *canvas,
                                                               GX_RECTANGLE *dirty_area))
{
    _gx_display_driver_565rgb_setup(display, aux_data, toggle_function);
    display->color_format                           = GX_COLOR_FORMAT_4444ARGB;
    display->driver_native_color_get                = _gx_display_driver_4444argb_native_color_get;
    display->driver_pixelmap_draw                   = _gx_display_driver_4444argb_pixelmap_draw;
    display->driver_pixelmap_blend                  = _gx_display_driver_4444argb_pixelmap_blend;
    display->driver_canvas_blend                    = _gx_display_driver_4444argb_canvas_blend;
    display->driver_pixel_blend                     = _gx_display_driver_4444argb_pixel_blend;
    display->driver_horizontal_pixelmap_line_draw   = _gx_display_driver_4444argb_horizontal_pixelmap_line_draw;
    display->driver_pixelmap_rotate                 = _gx_display_driver_4444argb_pixelmap_rotate;
}

