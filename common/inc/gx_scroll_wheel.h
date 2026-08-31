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
/**   Scroll Wheel Management (Scroll Wheel)                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_scroll_wheel.h                                                   */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX scroll wheel management component,       */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_SCROLL_WHEEL_H
#define GX_SCROLL_WHEEL_H

#include "gx_window.h"
#include "gx_gradient.h"

/* Define GX_SCROLL_WHEEL control block type */
struct GX_SCROLL_WHEEL : public GX_WINDOW
{
    UINT            (*scroll)(GX_SCROLL_WHEEL* wheel, GX_VALUE amount);
    GX_BOOL         (*wrap_style_check)(GX_SCROLL_WHEEL* wheel);

    INT               selected_row;
    INT               total_rows;

    GX_RESOURCE_ID    selected_background;
    GX_GRADIENT       gradient;

    GX_VALUE          selected_yshift;

    GX_VALUE          animation_speed;
    GX_VALUE          animation_end_speed;
    GX_VALUE          animation_steps;
    GX_VALUE          animation_delay;
    GX_VALUE          animation_max_steps;
    GX_FIXED_VAL      animation_start_speed_rate;
    GX_FIXED_VAL      animation_end_speed_rate;

    GX_VALUE          row_height;

    GX_VALUE          shift_error;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent, INT total_rows,
                 ULONG style, USHORT Id, const GX_RECTANGLE* size);

    void total_rows_set_(INT total_rows);
    void selected_set_(INT row);

    void row_height_set_(GX_VALUE row_height);

    void selected_background_set_(GX_RESOURCE_ID selected_bg);

    void speed_set_(GX_FIXED_VAL start_speed_rate, GX_FIXED_VAL end_speed_rate,
                    GX_VALUE max_steps, GX_VALUE delay);

    void gradient_alpha_set_(GX_UBYTE start_alpha, GX_UBYTE end_alpha);

    void scroll_(GX_VALUE shift);
    GX_BOOL is_wrap_style_() const;
    UINT event_process_(GX_EVENT* event_ptr);

protected:
    GX_VALUE shift_update_(GX_VALUE shift);
    void selected_row_calculate_();

private:
    void gradient_create_();

    void pen_up_event_process_();
    void flick_event_process_(INT flick_speed);
    void timer_event_process_(UINT timer_id);
};

#define GX_ANIMATION_TIMER 0x1100


/* Define scroll wheel management function prototypes.  */

UINT    _gx_scroll_wheel_create(GX_SCROLL_WHEEL* wheel, const GX_CHAR* name, GX_WIDGET* parent, INT total_rows,
                                ULONG style, USHORT Id, const GX_RECTANGLE* size);
UINT    _gx_scroll_wheel_event_process(GX_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr);
UINT    _gx_scroll_wheel_gradient_alpha_set(GX_SCROLL_WHEEL* wheel, GX_UBYTE start_alpha, GX_UBYTE end_alpha);
UINT    _gx_scroll_wheel_row_height_set(GX_SCROLL_WHEEL* wheel, GX_VALUE row_height);
UINT    _gx_scroll_wheel_selected_background_set(GX_SCROLL_WHEEL* wheel, GX_RESOURCE_ID selected_bg);
UINT    _gx_scroll_wheel_selected_get(GX_SCROLL_WHEEL* wheel, INT* row);
UINT    _gx_scroll_wheel_selected_set(GX_SCROLL_WHEEL* wheel, INT row);
UINT    _gx_scroll_wheel_speed_set(GX_SCROLL_WHEEL* wheel, GX_FIXED_VAL start_speed_rate, GX_FIXED_VAL end_speed_rate,
                                   GX_VALUE max_steps, GX_VALUE delay);
UINT    _gx_scroll_wheel_total_rows_set(GX_SCROLL_WHEEL* wheel, INT total_rows);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_scroll_wheel_create(GX_SCROLL_WHEEL* wheel, const GX_CHAR* name, GX_WIDGET* parent, INT total_rows,
                              ULONG style, USHORT Id, const GX_RECTANGLE* size, UINT control_block_size);
UINT _gxe_scroll_wheel_event_process(GX_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr);
UINT _gxe_scroll_wheel_gradient_alpha_set(GX_SCROLL_WHEEL* wheel, GX_UBYTE start_alpha, GX_UBYTE end_alpha);
UINT _gxe_scroll_wheel_row_height_set(GX_SCROLL_WHEEL* wheel, GX_VALUE row_height);
UINT _gxe_scroll_wheel_selected_background_set(GX_SCROLL_WHEEL* wheel, GX_RESOURCE_ID selected_bg);
UINT _gxe_scroll_wheel_selected_get(GX_SCROLL_WHEEL* wheel, INT* row);
UINT _gxe_scroll_wheel_selected_set(GX_SCROLL_WHEEL* wheel, INT row);
UINT _gxe_scroll_wheel_speed_set(GX_SCROLL_WHEEL* wheel, GX_FIXED_VAL start_speed_rate, GX_FIXED_VAL end_speed_rate,
                                 GX_VALUE max_steps, GX_VALUE delay);
UINT _gxe_scroll_wheel_total_rows_set(GX_SCROLL_WHEEL* wheel, INT total_rows);

#endif

