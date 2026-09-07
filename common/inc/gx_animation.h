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
/**   Animation Management (Animation)                                    */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_animation.h                                                      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX animation component, including all data  */
/*    types and external references.  It is assumed that gx_api.h and     */
/*    gx_port.h have already been included.                               */
/*                                                                        */
/**************************************************************************/

#ifndef GX_ANIMATION_H
#define GX_ANIMATION_H

#include "gx_global.h"
#include "gx_point.h"

struct GX_WIDGET;
struct GX_EVENT;
struct GX_CANVAS;

/* Animation information structure. This structure is used to define the
   parameters of a screen transition animation.  */
struct GX_ANIMATION_INFO
{
    GX_WIDGET*  target;
    GX_WIDGET*  parent;
    GX_WIDGET** slide_screen_list;
    USHORT      style;
    USHORT      id;
    USHORT      start_delay;
    USHORT      frame_interval;
    GX_POINT    start_position;
    GX_POINT    end_position;
    GX_UBYTE    start_alpha;
    GX_UBYTE    end_alpha;
    GX_UBYTE    steps;
};

/* Animation control block */
struct GX_ANIMATION
{
    GX_ANIMATION_INFO    info;
    GX_CANVAS*           canvas;
    GX_ANIMATION*        next;
    UINT               (*original_event_process_function)(GX_WIDGET*, GX_EVENT*);

    GX_VALUE             slide_target_index_1;
    GX_VALUE             slide_target_index_2;
    GX_VALUE             slide_tracking_start_pos;
    GX_VALUE             slide_tracking_current_pos;
    USHORT               landing_speed;
    USHORT               timer_ticks;
    USHORT               slide_screen_list_size;
    GX_UBYTE             status;
    GX_UBYTE             slide_direction;
    GX_UBYTE             system_allocated;
    GX_UBYTE             steps;
};

#if (GX_ANIMATION_POOL_SIZE > 0)

#define GX_ANIMATION_SLIDE_LEFT       0x0001
#define GX_ANIMATION_SLIDE_RIGHT      0x0002
#define GX_ANIMATION_SLIDE_UP         0x0040
#define GX_ANIMATION_SLIDE_DOWN       0x0080

#define GX_ANIMATION_SLIDE_TIMER      1000
#define GX_ANIMATION_MIN_SLIDING_DIST 5


/* Define animation management function prototypes.  */

UINT _gx_animation_create(GX_ANIMATION* animation);
UINT _gx_animation_delete(GX_ANIMATION* target, GX_WIDGET* parent);

UINT _gx_animation_canvas_define(GX_ANIMATION* animation, GX_CANVAS* canvas);

UINT _gx_animation_start(GX_ANIMATION* animation, GX_ANIMATION_INFO* info);
UINT _gx_animation_stop(GX_ANIMATION* animation);
void _gx_animation_update(void);

UINT _gx_animation_drag_enable(GX_ANIMATION* animation, GX_WIDGET* widget, GX_ANIMATION_INFO* info);
UINT _gx_animation_drag_disable(GX_ANIMATION* animation, GX_WIDGET* widget);
UINT _gx_animation_landing_speed_set(GX_ANIMATION* animation, USHORT shift_per_step);

/* error checking versions of the animation API functions */

UINT _gxe_animation_create(GX_ANIMATION* animation);
UINT _gxe_animation_canvas_define(GX_ANIMATION* animation, GX_CANVAS* canvas);
UINT _gxe_animation_delete(GX_ANIMATION* target, GX_WIDGET* parent);
UINT _gxe_animation_drag_disable(GX_ANIMATION* animation, GX_WIDGET* widget);
UINT _gxe_animation_drag_enable(GX_ANIMATION* animation, GX_WIDGET* widget, GX_ANIMATION_INFO* info);
UINT _gxe_animation_landing_speed_set(GX_ANIMATION* animation, USHORT shift_per_step);
UINT _gxe_animation_start(GX_ANIMATION* animation, GX_ANIMATION_INFO* info);
UINT _gxe_animation_stop(GX_ANIMATION* animation);

#endif /* GX_ANIMATION_POOL_SIZE > 0 */

#endif

