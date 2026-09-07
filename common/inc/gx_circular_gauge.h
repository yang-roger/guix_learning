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
/**   Circular Gauge Management (Circular Gauge)                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_circular_gauge.h                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX circular gauge widget,                   */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_CIRCULAR_GAUGE_H
#define GX_CIRCULAR_GAUGE_H

#include "gx_icon.h"
#include "gx_pixelmap.h"

struct GX_CIRCULAR_GAUGE_INFO
{
    INT animation_steps; // rotating steps
    INT animation_delay; // delay time between each step.

    GX_VALUE needle_xpos; // distance from the left of the gauge widget to the center-of-rotation of the gauge needle
    GX_VALUE needle_ypos; // distance from the top of the gauge widget to the center-of-rotation of the gauge needle

    GX_VALUE needle_xcor; // distance from the left of the needle image to the center-of-rotation of the gauge needle
    GX_VALUE needle_ycor; // distance from the top of the needle image to the center-of-rotation of the gauge needle

    GX_RESOURCE_ID needle_pixelmap;
};

/* Circular gauge control block */
struct GX_CIRCULAR_GAUGE : public GX_ICON
{
    GX_CIRCULAR_GAUGE_INFO info;
    GX_PIXELMAP            needle_rotated;
    GX_PIXELMAP*           needle_source;
    INT                    current_needle_x;
    INT                    current_needle_y;
    INT                    start_angle;
    INT                    current_angle;
    INT                    target_angle;
    INT                    angle_increment;
    INT                    animation_step;
    GX_RECTANGLE           current_needle_rectangle;

    UINT create_(const GX_CHAR* name,
                 GX_WIDGET* parent,
                 GX_CIRCULAR_GAUGE_INFO* circular_gauge_info,
                 GX_RESOURCE_ID background,
                 ULONG style,
                 USHORT circular_gauge_id,
                 GX_VALUE xpos, GX_VALUE ypos);

    void angle_set_(INT angle);
    void animation_set_(INT steps, INT delay);

    void background_draw_();

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

    UINT needle_rotate_();

private:
    void angle_increment_calculate_();
    bool needle_rectangle_calculate_(INT angle, GX_RECTANGLE* rect);
    void needle_dirty_mark_();
};


/* Define prompt management function prototypes.  */

UINT _gx_circular_gauge_angle_get(GX_CIRCULAR_GAUGE* gauge, INT* angle);
UINT _gx_circular_gauge_angle_set(GX_CIRCULAR_GAUGE* gauge, INT angle);
UINT _gx_circular_gauge_animation_set(GX_CIRCULAR_GAUGE* gauge, INT steps, INT delay);
void _gx_circular_gauge_background_draw(GX_CIRCULAR_GAUGE* gauge);
UINT _gx_circular_gauge_create(GX_CIRCULAR_GAUGE* gauge,
                               const GX_CHAR* name,
                               GX_WIDGET* parent,
                               GX_CIRCULAR_GAUGE_INFO* circular_gauge_info,
                               GX_RESOURCE_ID background,
                               ULONG style,
                               USHORT circular_gauge_id,
                               GX_VALUE xpos, GX_VALUE ypos);
void _gx_circular_gauge_draw(GX_CIRCULAR_GAUGE* gauge);
UINT _gx_circular_gauge_event_process(GX_CIRCULAR_GAUGE* gauge, GX_EVENT* event_ptr);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_circular_gauge_angle_get(GX_CIRCULAR_GAUGE* gauge, INT* angle);
UINT _gxe_circular_gauge_angle_set(GX_CIRCULAR_GAUGE* gauge, INT angle);
UINT _gxe_circular_gauge_animation_set(GX_CIRCULAR_GAUGE* gauge, INT steps, INT delay);

UINT _gxe_circular_gauge_create(GX_CIRCULAR_GAUGE* gauge,
                                const GX_CHAR* name,
                                GX_WIDGET* parent,
                                GX_CIRCULAR_GAUGE_INFO* circular_gauge_info,
                                GX_RESOURCE_ID background,
                                ULONG style,
                                USHORT circular_gauge_id,
                                GX_VALUE xpos, GX_VALUE ypos,
                                UINT checkbox_control_block_size);
UINT _gxe_circular_gauge_event_process(GX_CIRCULAR_GAUGE* gauge, GX_EVENT* event_ptr);

#endif

