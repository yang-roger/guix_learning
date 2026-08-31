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
/**   Utility (Brush)                                                     */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef GX_BRUSH_H
#define GX_BRUSH_H

#include "gx_global.h"

struct GX_PIXELMAP;
struct GX_FONT;

/* Define Brush type.  */
struct GX_BRUSH
{
    GX_PIXELMAP* pixelmap;                             /* Used for fills and line ends             */
    GX_FONT*     font;                                 /* Used for drawing text                    */
    ULONG        line_pattern;                         /* Used for drawing dashed lines            */
    ULONG        pattern_mask;                         /* Used for drawing dashed lines            */
    GX_COLOR     fill_color;                           /* For solid color fills                    */
    GX_COLOR     line_color;                           /* For lines and outlines                   */
    UINT         style;                                /* Flags defined below                      */
    GX_VALUE     width;                                /* Line width                               */
    UCHAR        alpha;                                /* for blending                             */

    void default_init_();
    void set_(GX_COLOR line_color, GX_COLOR fill_color, UINT style);
};

UINT _gx_brush_default(GX_BRUSH* brush);
UINT _gx_brush_define(GX_BRUSH* brush, GX_COLOR line_color, GX_COLOR fill_color, UINT style);

UINT _gxe_brush_default(GX_BRUSH* brush);
UINT _gxe_brush_define(GX_BRUSH* brush, GX_COLOR line_color, GX_COLOR fill_color, UINT style);

#endif
