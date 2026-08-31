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
/**   Progress Bar Management (Radial Progress Bar)                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_radial_progress_bar.h                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX progress bar widget,                     */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_RADIAL_PROGRESS_BAR_H
#define GX_RADIAL_PROGRESS_BAR_H

#include "gx_widget.h"

#if defined(GX_BRUSH_ALPHA_SUPPORT)
#include "gx_canvas.h"
#endif

struct GX_RADIAL_PROGRESS_BAR_INFO
{
    GX_VALUE       xcenter;
    GX_VALUE       ycenter;
    GX_VALUE       radius;                 /* radius of circular track.  */
    GX_VALUE       current_val;            /* current value.  */
    GX_VALUE       anchor_val;             /* starting angle for the circular arc.  */
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color;      /* resource ID of color for normal text.  */
    GX_RESOURCE_ID selected_text_color;    /* resource ID of color for selected text.  */
    GX_RESOURCE_ID disabled_text_color;
    GX_VALUE       normal_brush_width;     /* width of lower track.  */
    GX_VALUE       selected_brush_width;   /* width of upper track.  */
    GX_RESOURCE_ID normal_brush_color;     /* resource ID of color for lower track.  */
    GX_RESOURCE_ID selected_brush_color;   /* resource ID of color for upper track.  */
};

/* Define Radial Progress Bar type */
struct GX_RADIAL_PROGRESS_BAR : public GX_WIDGET
{
    GX_RADIAL_PROGRESS_BAR_INFO info;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_CANVAS canvas;
#endif

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 const GX_RADIAL_PROGRESS_BAR_INFO* progress_info,
                 ULONG style, USHORT progress_id);

    void info_set_(const GX_RADIAL_PROGRESS_BAR_INFO* info);

    void anchor_set_(GX_VALUE angle);
    void value_set_(GX_VALUE new_value);

    void font_set_(GX_RESOURCE_ID font_id);

    void text_color_set_(GX_RESOURCE_ID normal_text_color_id,
                         GX_RESOURCE_ID selected_text_color_id,
                         GX_RESOURCE_ID disabled_text_color_id);

    void background_draw_();
    void text_draw_();

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    GX_VALUE brush_half_width_() const;
    INT distance_to_center_(GX_POINT pos) const;

    void size_update_();
    void center_and_radius_update_();
    bool value_calculate_(GX_POINT new_position);

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    void canvas_delete_();
#endif
};


/* Define radial progress bar management function prototypes.  */

UINT _gx_radial_progress_bar_anchor_set(GX_RADIAL_PROGRESS_BAR* progress, GX_VALUE angle);
void _gx_radial_progress_bar_background_draw(GX_RADIAL_PROGRESS_BAR* progress);
UINT _gx_radial_progress_bar_create(GX_RADIAL_PROGRESS_BAR* progress,
                                    const GX_CHAR* name,
                                    GX_WIDGET* parent,
                                    GX_RADIAL_PROGRESS_BAR_INFO* progress_info,
                                    ULONG style,
                                    USHORT progress_id);
void _gx_radial_progress_bar_draw(GX_RADIAL_PROGRESS_BAR* progress);
UINT _gx_radial_progress_bar_event_process(GX_RADIAL_PROGRESS_BAR* progress, GX_EVENT* event_ptr);
UINT _gx_radial_progress_bar_font_set(GX_RADIAL_PROGRESS_BAR* progress, GX_RESOURCE_ID font_id);
UINT _gx_radial_progress_bar_info_set(GX_RADIAL_PROGRESS_BAR* progress, GX_RADIAL_PROGRESS_BAR_INFO* info);
UINT _gx_radial_progress_bar_text_color_set(GX_RADIAL_PROGRESS_BAR* progress,
                                            GX_RESOURCE_ID normal_text_color_id,
                                            GX_RESOURCE_ID selected_text_color_id,
                                            GX_RESOURCE_ID disabled_text_color_id);
void _gx_radial_progress_bar_text_draw(GX_RADIAL_PROGRESS_BAR* progress);
UINT _gx_radial_progress_bar_value_set(GX_RADIAL_PROGRESS_BAR* progress, GX_VALUE new_value);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_radial_progress_bar_anchor_set(GX_RADIAL_PROGRESS_BAR* progress, GX_VALUE angle);

UINT _gxe_radial_progress_bar_create(GX_RADIAL_PROGRESS_BAR* progress,
                                     const GX_CHAR* name,
                                     GX_WIDGET* parent,
                                     GX_RADIAL_PROGRESS_BAR_INFO* progress_info,
                                     ULONG style,
                                     USHORT progress_id,
                                     UINT progress_bar_control_block_size);
UINT _gxe_radial_progress_bar_event_process(GX_RADIAL_PROGRESS_BAR* progress, GX_EVENT* event_ptr);
UINT _gxe_radial_progress_bar_font_set(GX_RADIAL_PROGRESS_BAR* progress, GX_RESOURCE_ID font_id);
UINT _gxe_radial_progress_bar_info_set(GX_RADIAL_PROGRESS_BAR* progress, GX_RADIAL_PROGRESS_BAR_INFO* info);
UINT _gxe_radial_progress_bar_text_color_set(GX_RADIAL_PROGRESS_BAR* progress,
                                             GX_RESOURCE_ID normal_text_color_id,
                                             GX_RESOURCE_ID selected_text_color_id,
                                             GX_RESOURCE_ID disabled_text_color_id);

UINT _gxe_radial_progress_bar_value_set(GX_RADIAL_PROGRESS_BAR* progress, GX_VALUE new_value);

#endif

