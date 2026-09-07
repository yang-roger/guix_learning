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
/**   Prompt Management (Slider)                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_slider.h                                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX slide widget,                            */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_SLIDER_H
#define GX_SLIDER_H

#include "gx_widget.h"

/* Define Slider information structure. This structure is used to define the
   appearance of a Pixelmap Slider.  */
struct GX_SLIDER_INFO
{
    INT      min_val;
    INT      max_val;
    INT      current_val;
    INT      increment;
    GX_VALUE min_travel;
    GX_VALUE max_travel;
    GX_VALUE needle_width;
    GX_VALUE needle_height;
    GX_VALUE needle_inset;
    GX_VALUE needle_hotspot_offset;
};

/* Define Slider type.  */
struct GX_SLIDER : public GX_WIDGET
{
    GX_SLIDER_INFO info;
    INT            tick_count;
    GX_RESOURCE_ID tick_color;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 INT tick_count, const GX_SLIDER_INFO* slider_info,
                 ULONG style, USHORT slider_id, const GX_RECTANGLE* size);

    void info_set_(const GX_SLIDER_INFO* slider_info);

    void value_set_(GX_SLIDER_INFO* slider_info, INT new_value);
    void value_set_(INT new_value)
    {
        value_set_(&info, new_value);
    }

    void position_to_value_(GX_SLIDER_INFO* slider_info, INT pen_pos);
    void position_to_value_(INT pen_pos)
    {
        position_to_value_(&info, pen_pos);
    }

    void needle_position_get_(const GX_SLIDER_INFO* slider_info, GX_RECTANGLE* return_position) const;
    void needle_position_get_(GX_RECTANGLE* return_position) const
    {
        needle_position_get_(&info, return_position);
    }

    void travel_get_(const GX_SLIDER_INFO* slider_info, INT* return_min_travel, INT* return_max_travel) const;
    void travel_get_(INT* return_min_travel, INT* return_max_travel) const
    {
        travel_get_(&info, return_min_travel, return_max_travel);
    }

    void tickmarks_draw_();
    void needle_draw_();

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void position_to_value_(const GX_POINT& point);
};


UINT _gx_slider_create(GX_SLIDER* slider, const GX_CHAR* name, GX_WIDGET* parent,
                       INT tick_count, GX_SLIDER_INFO* slider_info,
                       ULONG style, USHORT slider_id, const GX_RECTANGLE* size);
void _gx_slider_draw(GX_SLIDER* slider);
UINT _gx_slider_event_process(GX_SLIDER* slider, GX_EVENT* event_ptr);
UINT _gx_slider_info_set(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info);
void _gx_slider_needle_draw(GX_SLIDER* slider);
UINT _gx_slider_needle_position_get(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info, GX_RECTANGLE* return_position);
void _gx_slider_tickmarks_draw(GX_SLIDER* slider);
UINT _gx_slider_travel_get(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info, INT* return_min_travel, INT* return_max_travel);
UINT _gx_slider_value_calculate(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info, INT pen_pos);
UINT _gx_slider_value_set(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info, INT new_value);


UINT _gxe_slider_create(GX_SLIDER* slider, const GX_CHAR* name, GX_WIDGET* parent,
                        INT tick_count, GX_SLIDER_INFO* slider_info,
                        ULONG style, USHORT slider_id, const GX_RECTANGLE* size,
                        UINT slider_control_block_size);
UINT _gxe_slider_event_process(GX_SLIDER* slider, GX_EVENT* event_ptr);
UINT _gxe_slider_info_set(GX_SLIDER *slider, GX_SLIDER_INFO *info);

UINT _gxe_slider_needle_position_get(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info, GX_RECTANGLE* return_position);

UINT _gxe_slider_travel_get(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info, INT* return_min_travel, INT* return_max_travel);
UINT _gxe_slider_value_calculate(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info, INT new_position);
UINT _gxe_slider_value_set(GX_SLIDER* slider, GX_SLIDER_INFO* slider_info, INT new_value);

#endif

