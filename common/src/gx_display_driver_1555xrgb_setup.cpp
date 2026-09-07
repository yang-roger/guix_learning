
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
/*    _gx_display_driver_1555xrgb_setup                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 16-bit 1555XRGB color format display driver setup routine.  */
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
/*    _gx_display_driver_565rgb_setup       565rgb format display driver  */
/*                                            setup routine               */
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
void _gx_display_driver_1555xrgb_setup(GX_DISPLAY *display, void *aux_data,
                                       void (*toggle_function)(GX_CANVAS *canvas,
                                                               GX_RECTANGLE *dirty_area))
{
    _gx_display_driver_565rgb_setup(display, aux_data, toggle_function);

    display->color_format = GX_COLOR_FORMAT_1555XRGB;
    display->driver_native_color_get                = _gx_display_driver_1555xrgb_native_color_get;
    display->driver_canvas_blend                    = _gx_display_driver_1555xrgb_canvas_blend;
    display->driver_pixel_blend                     = _gx_display_driver_1555xrgb_pixel_blend;
    display->driver_pixelmap_rotate                 = _gx_display_driver_1555xrgb_pixelmap_rotate;
    display->driver_pixelmap_draw                   = _gx_display_driver_1555xrgb_pixelmap_draw;
    display->driver_pixelmap_blend                  = _gx_display_driver_1555xrgb_pixelmap_blend;

#if defined(GX_SOFTWARE_DECODER_SUPPORT)
    display->driver_jpeg_draw                       = _gx_display_driver_1555xrgb_jpeg_draw;
    display->driver_png_draw                        = _gx_display_driver_1555xrgb_png_draw;
#endif
}

