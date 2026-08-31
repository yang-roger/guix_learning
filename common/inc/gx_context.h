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
/**   Context Management (Context)                                        */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_context.h                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX draw context component, including all    */
/*    data types and external references.  It is assumed that gx_api.h    */
/*    and gx_port.h have already been included.                           */
/*                                                                        */
/**************************************************************************/

#ifndef GX_DRAW_CONTEXT_H
#define GX_DRAW_CONTEXT_H

#include "gx_brush.h"
#include "gx_view.h"

struct GX_CANVAS;
struct GX_DISPLAY;

/* Define Drawing Context type.  */
struct GX_DRAW_CONTEXT
{
    GX_BRUSH                brush;
    GX_CANVAS*              canvas;
    GX_DISPLAY*             display;
    GX_VIEW*                view_head;
    GX_VIEW                 simple_view;
    GX_COLOR*               memory;
    GX_RECTANGLE            dirty;
    GX_RECTANGLE*           clip;
    GX_VALUE                pitch;
#if defined GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
    GX_VALUE                offset_x;
    GX_VALUE                offset_y;
#endif

    void brush_default_init_();
    UINT brush_set_(GX_RESOURCE_ID line_color_id, GX_RESOURCE_ID fill_color_id, UINT style);
    void brush_pattern_set_(ULONG pattern);

    UINT color_get_(GX_RESOURCE_ID id, GX_COLOR* color);
    UINT line_color_set_(GX_RESOURCE_ID line_color_id);
    UINT fill_color_set_(GX_RESOURCE_ID fill_color_id);

    UINT font_get_(GX_RESOURCE_ID resource_id, GX_FONT** return_font);
    UINT font_set_(GX_RESOURCE_ID font_id);

    UINT pixelmap_get_(GX_RESOURCE_ID pixelmap_id, GX_PIXELMAP** return_pixelmap);
    UINT pixelmap_set_(GX_RESOURCE_ID pixelmap_id);

    UINT string_get_ext_(GX_RESOURCE_ID string_id, GX_STRING* return_string);

    UINT raw_brush_define_(GX_COLOR line_color, GX_COLOR fill_color, UINT style);
    UINT raw_line_color_set_(GX_COLOR line_color);
    UINT raw_fill_color_set_(GX_COLOR fill_color);
};

GX_DRAW_CONTEXT* gx_context_current_();

/* Define context management function prototypes.  */

UINT _gx_context_brush_default(GX_DRAW_CONTEXT* context);
UINT _gx_context_brush_define(GX_RESOURCE_ID line_color_id, GX_RESOURCE_ID fill_color_id, UINT style);
UINT _gx_context_brush_get(GX_BRUSH** return_brush);
UINT _gx_context_brush_set(GX_BRUSH* brush);
UINT _gx_context_brush_pattern_set(ULONG pattern);
UINT _gx_context_brush_style_set(UINT style);
UINT _gx_context_brush_width_set(UINT width);
UINT _gx_context_color_get(GX_RESOURCE_ID color_id, GX_COLOR* color);
UINT _gx_context_line_color_set(GX_RESOURCE_ID line_color_id);
UINT _gx_context_fill_color_set(GX_RESOURCE_ID fill_color_id);
UINT _gx_context_font_get(GX_RESOURCE_ID font_id, GX_FONT** return_font);
UINT _gx_context_font_set(GX_RESOURCE_ID font_id);
UINT _gx_context_pixelmap_get(GX_RESOURCE_ID pixelmap_id, GX_PIXELMAP** return_pixelmap);
UINT _gx_context_pixelmap_set(GX_RESOURCE_ID pixelmap_id);
UINT _gx_context_raw_brush_define(GX_COLOR line_color, GX_COLOR fill_color, UINT style);
UINT _gx_context_raw_line_color_set(GX_COLOR line_color);
UINT _gx_context_raw_fill_color_set(GX_COLOR fill_color);
UINT _gx_context_string_get_ext(GX_RESOURCE_ID string_id, GX_STRING* return_string);

/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_context_brush_default(GX_DRAW_CONTEXT* context);
UINT _gxe_context_brush_define(GX_RESOURCE_ID line_color_id, GX_RESOURCE_ID fill_color_id, UINT style);
UINT _gxe_context_brush_get(GX_BRUSH** return_brush);
UINT _gxe_context_brush_set(GX_BRUSH* brush);
UINT _gxe_context_brush_pattern_set(ULONG pattern);
UINT _gxe_context_brush_style_set(UINT style);
UINT _gxe_context_brush_width_set(UINT width);
UINT _gxe_context_color_get(GX_RESOURCE_ID color_id, GX_COLOR* return_color);
UINT _gxe_context_line_color_set(GX_RESOURCE_ID line_color_id);
UINT _gxe_context_fill_color_set(GX_RESOURCE_ID fill_color_id);
UINT _gxe_context_font_get(GX_RESOURCE_ID font_id, GX_FONT** return_font);
UINT _gxe_context_font_set(GX_RESOURCE_ID font_id);
UINT _gxe_context_pixelmap_get(GX_RESOURCE_ID pixelmap_id, GX_PIXELMAP** return_pixelmap);
UINT _gxe_context_pixelmap_set(GX_RESOURCE_ID pixelmap_id);
UINT _gxe_context_raw_brush_define(GX_COLOR line_color, GX_COLOR fill_color, UINT style);
UINT _gxe_context_raw_line_color_set(GX_COLOR line_color);
UINT _gxe_context_raw_fill_color_set(GX_COLOR fill_color);
UINT _gxe_context_string_get_ext(GX_RESOURCE_ID string_id, GX_STRING* return_string);

#endif

