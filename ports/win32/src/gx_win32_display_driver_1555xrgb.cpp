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
/*    win32_1555xrgb_bitmap_header_create                                 */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates bitmap header for 1555xrgb display driver.    */
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
/*    win32_graphics_driver_setup_1555xrgb                                */
/*                                                                        */
/**************************************************************************/
static void win32_1555xrgb_bitmap_header_create(GX_DISPLAY *display)
{
DWORD                        *putmask;

GX_WIN32_DISPLAY_DRIVER_DATA *instance = (GX_WIN32_DISPLAY_DRIVER_DATA *)display->driver_data;

    instance->bmpinfo.header.biSize = sizeof(BITMAPINFOHEADER);
    instance->bmpinfo.header.biWidth = display->width;
    instance->bmpinfo.header.biHeight = display->height;

    instance->bmpinfo.header.biPlanes = 1;
    instance->bmpinfo.header.biBitCount = 16;
    instance->bmpinfo.header.biSizeImage = display->width * display->height * 2;
    instance->bmpinfo.header.biClrUsed = 65535;
    instance->bmpinfo.header.biClrImportant = 65535;
    instance->bmpinfo.header.biCompression = BI_BITFIELDS;

    putmask = (DWORD *)&(instance->bmpinfo.colors[0]);

    *putmask++ = 0x00007c00;
    *putmask++ = 0x000003e0;
    *putmask   = 0x0000001f;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    win32_graphics_driver_setup_1555xrgb                                */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a Windows specific 1555xrgb display driver.   */
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
/*    _gx_display_driver_1555xgb_setup      guix display setup funciton.  */
/*    win32_1555xrgb_bitmap_header_create   Create bitmap header info     */
/*    gx_win32_get_free_data_instance       Get display data instance     */
/*    GX_WIN32_EVENT_THREAD_CREATE          Create event thread           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT win32_graphics_driver_setup_1555xrgb(GX_DISPLAY *display)
{
GX_WIN32_DISPLAY_DRIVER_DATA *data;

    /* Initialize the low-level drawing function pointers.

       For windows, these are always just the generic funcions,
       but for some hardware, these will be customized,
       optimized functions specific to that hardware. */

    data = gx_win32_get_free_data_instance();
    if (!data)
    {
        /* We don't have any free display data instance. */
        return(GX_FAILURE);
    }

    /* Save off the format display driver.*/
    data->type = GX_COLOR_FORMAT_1555XRGB;

    _gx_display_driver_1555xrgb_setup(display, data, gx_win32_display_buffer_toggle);

    win32_1555xrgb_bitmap_header_create(display);

    /* Create the GUIX / Windows event thread
       This thread is a substitute for a touch display
       or keyboard driver thread that would be running
       on embedded hardware. */
    GX_WIN32_EVENT_THREAD_CREATE(data, "GUI-WIN32-565rgb");

    return(GX_SUCCESS);
}
#endif /* WIN32 */

