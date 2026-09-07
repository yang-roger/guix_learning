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
#ifdef WIN32

#include "gx_win32_display_driver.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    win32_565rgb_bitmap_header_create                                   */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates bitmap header for 565rgb driver.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Pointer to GX_DISPLAY         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    win32_graphics_driver_setup_565rgb                                  */
/*                                                                        */
/**************************************************************************/
static void win32_565rgb_rotated_bitmap_header_create(GX_DISPLAY *display)
{
DWORD                        *putmask;

GX_WIN32_DISPLAY_DRIVER_DATA *instance = (GX_WIN32_DISPLAY_DRIVER_DATA *)display->driver_data;

    instance->bmpinfo.header.biSize = sizeof(BITMAPINFOHEADER);

    // swap width and height because display is mounted in non-native profile mode
    instance->bmpinfo.header.biWidth = display->width;
    instance->bmpinfo.header.biHeight = display->height;

    instance->bmpinfo.header.biPlanes = 1;
    instance->bmpinfo.header.biBitCount = 16;
    instance->bmpinfo.header.biSizeImage = display->width * display->height * 2;
    instance->bmpinfo.header.biClrUsed = 65535;
    instance->bmpinfo.header.biClrImportant = 65535;
    instance->bmpinfo.header.biCompression = BI_BITFIELDS;

    putmask = (DWORD *)&(instance->bmpinfo.colors[0]);

    *putmask++ = 0x0000f800;
    *putmask++ = 0x000007e0;
    *putmask   = 0x0000001f;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    win32_graphics_driver_565rgb_rotated_setup                          */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a Windows specific 565rgb display driver.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Pointer to GX_DISPLAY         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_565rgb_rotated_setup guix display setup funciton.*/
/*    _tx_thread_create                     Create a tx thread            */
/*    win32_565rgb_bitmap_header_create     Create bitmap header info     */
/*    get_data_instance                     Get display data instance     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT win32_graphics_driver_setup_565rgb_rotated(GX_DISPLAY *display)
{
GX_WIN32_DISPLAY_DRIVER_DATA *data;

    /* Initialize the low-level drawing function pointers

       For windows, these are always just the generic funcions,
       but for some hardware, these will be customized,
       optimized functions specific to that hardware. */

    data = gx_win32_get_free_data_instance();
    if (!data)
    {
        /* We don't have any free display data instance */
        return(GX_FAILURE);
    }

    /* Save off format of this display driver.*/
    data->type = GX_COLOR_FORMAT_565RGB;

    _gx_display_driver_565rgb_rotated_setup(display, data, gx_win32_display_buffer_toggle);

    win32_565rgb_rotated_bitmap_header_create(display);

    /* Create the GUIX / Windows event thread
       This thread is a substitute for a touch display
       or keyboard driver thread that would be running
       on embedded hardware. */
    GX_WIN32_EVENT_THREAD_CREATE(data, "GUI-WIN32-565rgb-rotated");

    return(GX_SUCCESS);
}
#endif /* WIN32 */