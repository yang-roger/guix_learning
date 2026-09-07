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
/**   Progress Bar Management (Progress Bar)                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_progress_bar.h                                                   */
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

#ifndef GX_PROGRESS_BAR_H
#define GX_PROGRESS_BAR_H

#include "gx_widget.h"

struct GX_PROGRESS_BAR_INFO
{
    INT            min_val;
    INT            max_val;
    INT            current_val;
    GX_RESOURCE_ID font_id;
    GX_RESOURCE_ID normal_text_color;
    GX_RESOURCE_ID selected_text_color;
    GX_RESOURCE_ID disabled_text_color;
    GX_RESOURCE_ID fill_pixelmap;

    void default_init_();
};

/* Define GX_PROGRESS_BAR type.  */
struct GX_PROGRESS_BAR : public GX_WIDGET
{
    GX_PROGRESS_BAR_INFO info;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 const GX_PROGRESS_BAR_INFO* progress_bar_info, ULONG style,
                 USHORT progress_bar_id, const GX_RECTANGLE* size);

    void info_set_(const GX_PROGRESS_BAR_INFO* info);

    void range_set_(INT min_value, INT max_value);
    void value_set_(INT new_value);

    void font_set_(GX_RESOURCE_ID font_id);
    void text_color_set_(GX_RESOURCE_ID normal_text_color_id,
                         GX_RESOURCE_ID selected_text_color_id, GX_RESOURCE_ID disabled_text_color_id);
    void pixelmap_set_(GX_RESOURCE_ID pixelmap);

    void background_draw_();
    void text_draw_();

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void fill_rect_get_(GX_RECTANGLE* fill_rect, INT current_range, INT range) const;
};


/* Define progress bar management function prototypes.  */

void _gx_progress_bar_background_draw(GX_PROGRESS_BAR* progress_bar);
UINT _gx_progress_bar_create(GX_PROGRESS_BAR* progress_bar, const GX_CHAR* name, GX_WIDGET* parent,
                             GX_PROGRESS_BAR_INFO* progress_bar_info, ULONG style,
                             USHORT progress_bar_id, const GX_RECTANGLE* size);
void _gx_progress_bar_draw(GX_PROGRESS_BAR* progress_bar);
UINT _gx_progress_bar_event_process(GX_PROGRESS_BAR* progress_bar, GX_EVENT* event_ptr);
UINT _gx_progress_bar_font_set(GX_PROGRESS_BAR* progress_bar, GX_RESOURCE_ID font_id);
UINT _gx_progress_bar_info_set(GX_PROGRESS_BAR* progress_bar, GX_PROGRESS_BAR_INFO* info);
UINT _gx_progress_bar_pixelmap_set(GX_PROGRESS_BAR* progress_bar, GX_RESOURCE_ID pixelmap);
UINT _gx_progress_bar_range_set(GX_PROGRESS_BAR* progress_bar, INT min_value, INT max_value);
UINT _gx_progress_bar_text_color_set(GX_PROGRESS_BAR* progress_bar, GX_RESOURCE_ID normal_text_color_id,
                                     GX_RESOURCE_ID selected_text_color_id, GX_RESOURCE_ID disabled_text_color_id);
void _gx_progress_bar_text_draw(GX_PROGRESS_BAR* progress_bar);
UINT _gx_progress_bar_value_set(GX_PROGRESS_BAR* progress_bar, INT new_value);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_progress_bar_create(GX_PROGRESS_BAR* progress_bar, const GX_CHAR* name, GX_WIDGET* parent,
                              GX_PROGRESS_BAR_INFO* progress_bar_info, ULONG style,
                              USHORT progress_bar_id, const GX_RECTANGLE* size,
                              UINT progress_bar_control_block_size);
UINT _gxe_progress_bar_event_process(GX_PROGRESS_BAR* progress_bar, GX_EVENT* event_ptr);
UINT _gxe_progress_bar_font_set(GX_PROGRESS_BAR* progress_bar, GX_RESOURCE_ID font_id);
UINT _gxe_progress_bar_info_set(GX_PROGRESS_BAR* progress_bar, GX_PROGRESS_BAR_INFO* info);
UINT _gxe_progress_bar_pixelmap_set(GX_PROGRESS_BAR* progress_bar, GX_RESOURCE_ID pixelmap);
UINT _gxe_progress_bar_range_set(GX_PROGRESS_BAR* progress_bar, INT min_value, INT max_value);
UINT _gxe_progress_bar_text_color_set(GX_PROGRESS_BAR* progress_bar, GX_RESOURCE_ID normal_text_color_id,
                                      GX_RESOURCE_ID selected_text_color_id, GX_RESOURCE_ID disabled_text_color_id);

UINT _gxe_progress_bar_value_set(GX_PROGRESS_BAR* progress_bar, INT new_value);

#endif

