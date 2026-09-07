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
/**   Window Management (Window)                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_window.h                                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX window management component,             */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_WINDOW_H
#define GX_WINDOW_H

#include "gx_widget.h"
#include "gx_view.h"
#include "gx_scroll_info.h"

struct GX_SCROLLBAR;

/* Define Window type.  */
struct GX_WINDOW : public GX_WIDGET
{
    GX_RECTANGLE   client;
    GX_VIEW*       views;
    void         (*scroll_info_get)(GX_WINDOW* window, ULONG style, GX_SCROLL_INFO* info);
    GX_BOOL        move_mode;
    GX_POINT       move_start;
    GX_RESOURCE_ID wallpaper;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 ULONG style, USHORT window_id, const GX_RECTANGLE* size);

    UINT close_();

    UINT execute_(ULONG* return_ptr = GX_NULL);

    void scroll_(GX_VALUE x_scroll, GX_VALUE y_scroll);
    void client_scroll_(GX_VALUE x_scroll, GX_VALUE y_scroll);

    void scroll_info_get_(ULONG style, GX_SCROLL_INFO* return_scroll_info);
    GX_SCROLLBAR* scrollbar_find_(USHORT type) const;

    void wallpaper_set_(GX_RESOURCE_ID wallpaper_id, GX_BOOL tile);

    void background_draw_();
    void background_draw_(GX_COLOR fill_color);

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void view_add_(const GX_RECTANGLE& rect);
    bool view_fold_(const GX_RECTANGLE& rect);

    void views_free_();
    void views_update_detect_();

    friend struct GX_WIDGET;
    friend struct GX_WINDOW_ROOT;
};

#define GX_LIST_CHILD_ID_START  4096
#define GX_SNAP_TIMER           1000
#define GX_FLICK_TIMER          1001


/* Define window management function prototypes.  */

void _gx_window_background_draw(GX_WINDOW* window);
UINT _gx_window_client_height_get(GX_WINDOW* window, GX_VALUE* return_height);
UINT _gx_window_client_scroll(GX_WINDOW* window, GX_VALUE x_scroll, GX_VALUE y_scroll);
UINT _gx_window_client_width_get(GX_WINDOW* window, GX_VALUE* return_width);
UINT _gx_window_close(GX_WINDOW* window);
UINT _gx_window_create(GX_WINDOW* window, const GX_CHAR* name, GX_WIDGET* parent,
                       ULONG style, USHORT Id, const GX_RECTANGLE* size);
void _gx_window_draw(GX_WINDOW* window);
UINT _gx_window_event_process(GX_WINDOW* window, GX_EVENT* event_ptr);
UINT _gx_window_execute(GX_WINDOW* window, ULONG* return_ptr);
UINT _gx_window_scroll_info_get(GX_WINDOW* window, ULONG style, GX_SCROLL_INFO* return_scroll_info);
UINT _gx_window_scrollbar_find(GX_WINDOW* window, USHORT type, GX_SCROLLBAR** return_scrollbar);
UINT _gx_window_wallpaper_get(GX_WINDOW* window, GX_RESOURCE_ID* return_wallpaper_id);
UINT _gx_window_wallpaper_set(GX_WINDOW* window, GX_RESOURCE_ID wallpaper_id, GX_BOOL tile);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_window_client_height_get(GX_WINDOW* window, GX_VALUE* return_height);
UINT _gxe_window_client_scroll(GX_WINDOW* window, GX_VALUE x_scroll, GX_VALUE y_scroll);
UINT _gxe_window_client_width_get(GX_WINDOW* window, GX_VALUE* return_width);
UINT _gxe_window_close(GX_WINDOW* window);
UINT _gxe_window_create(GX_WINDOW* window, const GX_CHAR* name, GX_WIDGET* parent,
                        ULONG style, USHORT Id, const GX_RECTANGLE* size,
                        UINT window_control_block_size);
UINT _gxe_window_event_process(GX_WINDOW* window, GX_EVENT* event_ptr);
UINT _gxe_window_execute(GX_WINDOW* window, ULONG* return_ptr);
UINT _gxe_window_scroll_info_get(GX_WINDOW* window, ULONG style, GX_SCROLL_INFO* return_scroll_info);
UINT _gxe_window_scrollbar_find(GX_WINDOW* window, USHORT type, GX_SCROLLBAR** return_scrollbar);
UINT _gxe_window_wallpaper_get(GX_WINDOW* window, GX_RESOURCE_ID* return_wallpaper_id);
UINT _gxe_window_wallpaper_set(GX_WINDOW* window, GX_RESOURCE_ID wallpaper_id, GX_BOOL tile);

#endif

