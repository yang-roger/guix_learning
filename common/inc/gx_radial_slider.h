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
/**   Radial Slider Management (Slider)                                   */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_radial_slider.h                                                  */
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

#ifndef GX_RADIAL_SLIDER_H
#define GX_RADIAL_SLIDER_H

#include "gx_widget.h"

struct GX_RADIAL_SLIDER_INFO
{
    GX_VALUE       xcenter;
    GX_VALUE       ycenter;
    USHORT         radius;
    USHORT         track_width;
    GX_VALUE       needle_offset;
    GX_VALUE       current_angle;
    GX_VALUE       min_angle;
    GX_VALUE       max_angle;
    GX_VALUE*      angle_list;
    USHORT         list_count;
    GX_RESOURCE_ID background_pixelmap;
    GX_RESOURCE_ID needle_pixelmap;
};

/* Define Radial Slider type. */
struct GX_RADIAL_SLIDER : public GX_WIDGET
{
    GX_RADIAL_SLIDER_INFO   info;
    GX_VALUE                start_angle;
    GX_VALUE                target_angle;
    GX_POINT                pendown_point;
    USHORT                  animation_total_steps;
    USHORT                  animation_step;
    USHORT                  animation_delay;
    USHORT                  animation_style;
    void                  (*animation_update_callback)(GX_RADIAL_SLIDER* slider);

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent, const GX_RADIAL_SLIDER_INFO* info,
                 ULONG style, USHORT slider_id, const GX_RECTANGLE* size);

    void info_set_(const GX_RADIAL_SLIDER_INFO* info);

    void angle_set_(GX_VALUE new_angle);
    void anchor_angles_set_(GX_VALUE* anchor_values, USHORT anchor_count);

    void pixelmap_set_(GX_RESOURCE_ID background_pixelmap, GX_RESOURCE_ID needle_pixelmap);

    void animation_set_(USHORT steps, USHORT delay, USHORT animation_style,
                        void (*animation_update_callback)(GX_RADIAL_SLIDER* slider));
    void animation_start_(GX_VALUE target_angle);

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void animation_update_();

    void needle_rectangle_calculate_(GX_RECTANGLE* rectangle);

    UINT angle_calculate_(GX_POINT point, GX_VALUE* return_value);
    void adjust_to_nearest_anchor_(GX_VALUE* angle_value);

    UINT pen_down_event_process_(GX_EVENT* event_ptr);
    UINT pen_drag_event_process_(GX_EVENT* event_ptr);
    UINT pen_up_event_process_(GX_EVENT* event_ptr);
};


/* Define radial slider management function prototypes.  */

UINT _gx_radial_slider_anchor_angles_set(GX_RADIAL_SLIDER* slider, GX_VALUE* anchor_values, USHORT anchor_count);
UINT _gx_radial_slider_angle_set(GX_RADIAL_SLIDER* slider, GX_VALUE new_angle);
UINT _gx_radial_slider_animation_set(GX_RADIAL_SLIDER* slider, USHORT steps, USHORT delay, USHORT animation_style,
                                     void (*animation_update_callback)(GX_RADIAL_SLIDER* slider));
UINT _gx_radial_slider_animation_start(GX_RADIAL_SLIDER* slider, GX_VALUE target_angle);
UINT _gx_radial_slider_create(GX_RADIAL_SLIDER* slider, const GX_CHAR* name, GX_WIDGET* parent,
                              GX_RADIAL_SLIDER_INFO* info, ULONG style, USHORT slider_id, const GX_RECTANGLE* size);
void _gx_radial_slider_draw(GX_RADIAL_SLIDER* slider);
UINT _gx_radial_slider_event_process(GX_RADIAL_SLIDER* slider, GX_EVENT* event_ptr);
UINT _gx_radial_slider_info_get(GX_RADIAL_SLIDER* slider, GX_RADIAL_SLIDER_INFO** info);
UINT _gx_radial_slider_info_set(GX_RADIAL_SLIDER* slider, GX_RADIAL_SLIDER_INFO* info);
UINT _gx_radial_slider_pixelmap_set(GX_RADIAL_SLIDER* slider, GX_RESOURCE_ID background_pixelmap, GX_RESOURCE_ID needle_pixelmap);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_radial_slider_anchor_angles_set(GX_RADIAL_SLIDER* slider, GX_VALUE* anchor_values, USHORT anchor_count);
UINT _gxe_radial_slider_angle_set(GX_RADIAL_SLIDER* slider, GX_VALUE new_angle);
UINT _gxe_radial_slider_animation_set(GX_RADIAL_SLIDER* slider, USHORT steps, USHORT delay, USHORT animation_style,
                                      void (*animation_update_callback)(GX_RADIAL_SLIDER* slider));
UINT _gxe_radial_slider_animation_start(GX_RADIAL_SLIDER* slider, GX_VALUE target_angle);
UINT _gxe_radial_slider_create(GX_RADIAL_SLIDER* slider, const GX_CHAR* name, GX_WIDGET* parent,
                               GX_RADIAL_SLIDER_INFO* info, ULONG style, USHORT slider_id, const GX_RECTANGLE* size,
                               UINT control_block_size);
UINT _gxe_radial_slider_event_process(GX_RADIAL_SLIDER* slider, GX_EVENT* event_ptr);
UINT _gxe_radial_slider_info_get(GX_RADIAL_SLIDER* slider, GX_RADIAL_SLIDER_INFO** info);
UINT _gxe_radial_slider_info_set(GX_RADIAL_SLIDER* slider, GX_RADIAL_SLIDER_INFO* info);
UINT _gxe_radial_slider_pixelmap_set(GX_RADIAL_SLIDER* slider, GX_RESOURCE_ID background_pixelmap, GX_RESOURCE_ID needle_pixelmap);

#endif

