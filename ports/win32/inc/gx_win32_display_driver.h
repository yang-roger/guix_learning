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
/**   Win32 Display Management (Display)                                  */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_win32_driver.h                                                   */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX Win32 display drivers.                   */
/*                                                                        */
/**************************************************************************/

#ifndef GX_WIN32_DRIVER_H
#define GX_WIN32_DRIVER_H

#include "tx_api.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_display.h"
#include "gx_canvas.h"

#include "windows.h"

#define GX_MAX_WIN32_DISPLAYS 2
#define GX_WIN32_STACK_SIZE   (16 * 1024)

/* Define Windows bitmap structure. */
struct GX_BMP_INFO
{
    BITMAPINFOHEADER header;
    DWORD            colors[256];
};

/* Define win32 display driver data structure. */
struct GX_WIN32_DISPLAY_DRIVER_DATA
{
    ULONG       type;
    GX_BMP_INFO bmpinfo;
    HWND        winhandle;
    INT         ready;
    HANDLE      thread_handle;
    GX_WIN32_DISPLAY_DRIVER_EXTRA_MEMBERS_DECLEARE
};

/* Define win32 driver functions. */
UINT                          win32_graphics_driver_setup_monochrome(GX_DISPLAY *display);
UINT                          win32_graphics_driver_setup_4bpp_grayscale(GX_DISPLAY *display);
UINT                          win32_graphics_driver_setup_8bit_palette(GX_DISPLAY *display);
UINT                          win32_graphics_driver_setup_8bit_palette_rotated(GX_DISPLAY* display);
UINT                          win32_graphics_driver_setup_565rgb(GX_DISPLAY *display);
UINT                          win32_graphics_driver_setup_565rgb_rotated(GX_DISPLAY *display);
UINT                          win32_graphics_driver_setup_565bgr(GX_DISPLAY *display);
UINT                          win32_graphics_driver_setup_1555xrgb(GX_DISPLAY *display);
UINT                          win32_graphics_driver_setup_4444argb(GX_DISPLAY *display);
UINT                          win32_graphics_driver_setup_24xrgb(GX_DISPLAY *display);
UINT                          win32_graphics_driver_setup_24xrgb_rotated(GX_DISPLAY *display);
UINT                          win32_graphics_driver_setup_32argb(GX_DISPLAY *display);
UINT                          win32_graphics_driver_setup_32argb_rotated(GX_DISPLAY *display);

UINT                          win32_chromeart_graphics_driver_setup_565rgb(GX_DISPLAY *display);
UINT                          win32_dave2d_graphics_driver_setup_8bit_palette(GX_DISPLAY *display);
UINT                          win32_dave2d_graphics_driver_setup_24xrgb(GX_DISPLAY *display);
UINT                          win32_dave2d_graphics_driver_setup_24xrgb_rotated(GX_DISPLAY *display);
UINT                          win32_dave2d_graphics_driver_setup_565rgb(GX_DISPLAY *display);
UINT                          win32_dave2d_graphics_driver_setup_565rgb_rotated(GX_DISPLAY *display);

VOID                          win32_32bpp_bitmap_header_create(GX_DISPLAY *display);
VOID                          win32_display_driver_8bit_palette_set(GX_DISPLAY *display, GX_COLOR *palette, INT count);
VOID                          win32_8bit_palette_bitmap_header_create(GX_DISPLAY *display);
VOID                          win32_dave2d_simulation_24xrgb_bitmap_header_create(GX_DISPLAY *display);
void                          gx_win32_display_buffer_toggle(GX_CANVAS *canvas, GX_RECTANGLE *dirty);
void                          gx_win32_driver_thread_entry(ULONG thread_input);
void                          gx_win32_driver_thread_initialize(GX_WIN32_DISPLAY_DRIVER_DATA *instance);
HWND                          gx_win32_window_create(GX_WIN32_DISPLAY_DRIVER_DATA *gx_driver_ptr, WNDPROC gx_win32_event_process, INT xpos, INT ypos);
LRESULT CALLBACK              gx_win32_event_process(HWND, UINT, WPARAM, LPARAM);
void                          gx_win32_message_to_guix(USHORT event_type);
void                          gx_win32_input_driver(GX_WIN32_DISPLAY_DRIVER_DATA *instance);
GX_WIN32_DISPLAY_DRIVER_DATA *gx_win32_get_free_data_instance();
GX_WIN32_DISPLAY_DRIVER_DATA *gx_win32_get_data_instance_by_win_handle(HWND winHandle);

#endif /* GX_WIN32_DRIVER_H */

