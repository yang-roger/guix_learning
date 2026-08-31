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
/*    win32_4bpp_grayscale_bitmap_header_create                           */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates bitmap header for 4bpp driver.                */
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
/*    win32_graphics_driver_setup_4bpp_grayscale                          */
/*                                                                        */
/**************************************************************************/
VOID win32_4bpp_grayscale_bitmap_header_create(GX_DISPLAY *display)
{
GX_COLOR                      color;
int                           loop;
INT                           temp = 0;
GX_WIN32_DISPLAY_DRIVER_DATA *instance = (GX_WIN32_DISPLAY_DRIVER_DATA *)display->driver_data;

    instance->bmpinfo.header.biSize = sizeof(BITMAPINFOHEADER);
    instance->bmpinfo.header.biWidth = display->width;
    instance->bmpinfo.header.biHeight = display->height;

    instance->bmpinfo.header.biPlanes = 1;
    instance->bmpinfo.header.biBitCount = 4;
    instance->bmpinfo.header.biSizeImage = display->width * display->height;
    instance->bmpinfo.header.biClrUsed = 16;
    instance->bmpinfo.header.biClrImportant = 16;
    instance->bmpinfo.header.biCompression = BI_RGB;

    for (loop = 0; loop < 16; loop++)
    {
        color = (temp << 16) | (temp << 8) | temp | 0xff000000;
        instance->bmpinfo.colors[loop] = color;
        temp += 0x11;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    win32_graphics_driver_setup_4bpp_grayscale                          */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a Windows specific 4bpp display driver.       */
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
/*    _gx_display_driver_4bpp_grayscale_setup                             */
/*    win32_4bpp_grayscale_bitmap_header_create                           */
/*    gx_win32_get_free_data_instance                                     */
/*    GX_WIN32_EVENT_THREAD_CREATE                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT win32_graphics_driver_setup_4bpp_grayscale(GX_DISPLAY *display)
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

    /* Save off the format of display driver. */
    data->type = GX_COLOR_FORMAT_4BIT_GRAY;

    _gx_display_driver_4bpp_grayscale_setup(display, data, gx_win32_display_buffer_toggle);

    win32_4bpp_grayscale_bitmap_header_create(display);

    /* Create the GUIX / Windows event thread.
       This thread is a substitute for a touch display
       or keyboard driver thread that would be running
       on embedded hardware. */
    GX_WIN32_EVENT_THREAD_CREATE(data, "GUI-WIN32-4bpp-grayscale");

    return(GX_SUCCESS);
}
#endif /* WIN32 */

