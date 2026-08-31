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
/**   Utility (Pixelmap)                                                  */
/**                                                                       */
/**************************************************************************/

#include "gx_utility_pixelmap.h"

#include "gx_utility_math.h"
#include "gx_system.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_1bpp_pixelmap_raw_resize                                */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    1bpp pixelmap resize function that handles uncompress, without      */
/*    transparent channel.                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The source pixelmap           */
/*    destination                           The resized pixelmap to be    */
/*                                            returned                    */
/*    width                                 New width                     */
/*    height                                New height                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           Memory Allocation routine     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_1bpp_pixelmap_resize                                    */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_1bpp_pixelmap_raw_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height)
{
/* The pixelmap resize function is implemented from nearest neighbor
   image scaling algorithm.  */
GX_UBYTE *get;
GX_UBYTE *put;
GX_UBYTE *putrow;
GX_UBYTE  putmask;
GX_UBYTE  getmask;
INT       putstride;
INT       getstride;
INT       xradio;
INT       yradio;
INT       x;
INT       y;
INT       xx;
INT       yy;

    /* Calculate scale ratio and enlarge it by 256 times to keep precision.  */
    xradio = ((src->width) << 8) / width;
    yradio = ((src->height) << 8) / height;

    putstride = (width + 7) >> 3;
    getstride = (src->width + 7) >> 3;

    /* Fill property values into destination pixelmap structure. */
    destination->flags = src->flags;
    destination->format = src->format;
    destination->transparent_color = src->transparent_color;
    destination->version_major = src->version_major;
    destination->version_minor = src->version_minor;

    destination->height = (GX_VALUE)height;
    destination->width = (GX_VALUE)width;

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->data_size = (UINT)(height * putstride) * sizeof(GX_UBYTE);

    /* Allocate memory to load pixelmap data. */
    destination->data = (GX_UBYTE *)_gx_system_memory_allocator(destination->data_size);

    if (destination->data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    putrow = (GX_UBYTE *)destination->data;

    /* Loop through destination's pixel and fill each pixel with the nearest neighbor.  */
    for (y = 0; y < height; y++)
    {
        put = putrow;
        putmask = 0x80;
        for (x = 0; x < width; x++)
        {
            xx = (xradio * x) >> 8;
            yy = (yradio * y) >> 8;

            get = (GX_UBYTE *)src->data;
            get += yy * getstride;
            get += xx >> 3;

            getmask = (GX_UBYTE)(0x80 >> (xx & 0x07));

            if (*get & getmask)
            {
                *put |= putmask;
            }
            else
            {
                *put &= (GX_UBYTE)(~putmask);
            }

            putmask >>= 1;
            if (putmask == 0)
            {
                put++;
                putmask = 0x80;
            }
        }
        putrow += putstride;
    }

    return GX_SUCCESS;
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_1bpp_pixelmap_transparent_resize                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    1bpp pixelmap resize function that handles uncompress, with         */
/*    transparent channel.                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The source pixelmap           */
/*    destination                           The resized pixelmap to be    */
/*                                            returned                    */
/*    width                                 New width                     */
/*    height                                New height                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           Memory Allocation routine     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_1bpp_pixelmap_resize                                    */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_1bpp_pixelmap_transparent_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height)
{
/* The pixelmap resize function is implemented from nearest neighbor
image scaling algorithm.  */
GX_UBYTE *get;
GX_UBYTE *put;
GX_UBYTE *putrow;
GX_UBYTE  putmask;
GX_UBYTE  puttransmask;
INT       putstride;
INT       getstride;
GX_UBYTE  getmask;
GX_UBYTE  gettransmask;
INT       xradio;
INT       yradio;
INT       x;
INT       y;
INT       xx;
INT       yy;

    /* Calculate scale ratio and enlarge it by 256 times to keep precision.  */
    xradio = ((src->width) << 8) / width;
    yradio = ((src->height) << 8) / height;

    putstride = (width + 3) >> 2;
    getstride = (src->width + 3) >> 2;

    /* Fill property values into destination pixelmap structure. */
    destination->flags = src->flags;
    destination->format = src->format;
    destination->transparent_color = src->transparent_color;
    destination->version_major = src->version_major;
    destination->version_minor = src->version_minor;

    destination->height = (GX_VALUE)height;
    destination->width = (GX_VALUE)width;

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->data_size = (UINT)(height * putstride) * sizeof(GX_UBYTE);

    /* Allocate memory to load pixelmap data. */
    destination->data = (GX_UBYTE *)_gx_system_memory_allocator(destination->data_size);

    if (destination->data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    putrow = (GX_UBYTE *)destination->data;

    /* Loop through destination's pixel and fill each pixel with the nearest neighbor.  */
    for (y = 0; y < height; y++)
    {
        put = putrow;
        putmask = 0x80;
        puttransmask = 0x40;
        for (x = 0; x < width; x++)
        {
            xx = (xradio * x) >> 8;
            yy = (yradio * y) >> 8;

            /* set bits first. */
            *put &= (GX_UBYTE)(~putmask);
            *put &= (GX_UBYTE)(~puttransmask);

            /* get pixel data */
            get = (GX_UBYTE *)src->data;
            get += yy * getstride;
            get += xx >> 2;

            gettransmask = (GX_UBYTE)(0x40 >> ((xx & 0x03) << 1));
            if (*get & gettransmask)
            {
                *put |= puttransmask;
                getmask = (GX_UBYTE)(gettransmask << 1);
                if (*get & getmask)
                {
                    *put |= putmask;
                }
            }

            putmask >>= 2;
            puttransmask >>= 2;
            if (puttransmask == 0)
            {
                put++;
                putmask = 0x80;
                puttransmask = 0x40;
            }
        }
        putrow += putstride;
    }

    return GX_SUCCESS;
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_1bpp_pixelmap_resize                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    1bpp pixelmap resize function that handles uncompress, with or      */
/*    without transparent channel.                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The source pixelmap           */
/*    destination                           The resized pixelmap to be    */
/*                                            returned                    */
/*    width                                 New width                     */
/*    height                                New height                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*     _gx_utility_1bpp_pixelmap_transparent_resize                       */
/*                                           Real pixelmap resize routine */
/*     _gx_utility_1bpp_pixelmap_raw_resize  Real pixelmap resize routine */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_1bpp_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height)
{
UINT status;

    if (src->flags & GX_PIXELMAP_TRANSPARENT)
    {
        /* transparent, no compression */
        status = _gx_utility_1bpp_pixelmap_transparent_resize(src, destination, width, height);
    }
    else
    {
        /* no compression or alpha */
        status = _gx_utility_1bpp_pixelmap_raw_resize(src, destination, width, height);
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_1bpp_pixelmap_rotate                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    1bpp pixelmap rotation function that handles uncompress, with or    */
/*    without alpha channel.                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The pixelmap to be rotated    */
/*    angle                                 The angle to be rotated       */
/*    destination                           The rotated bitmap to be      */
/*                                            returned                    */
/*    rot_cx                                X coordinate of rotation      */
/*                                            center                      */
/*    rot_cy                                Y coordinate of rotation      */
/*                                            center                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           Memory Allocation routine     */
/*    _gx_utility_math_cos                  Compute the cosine value      */
/*    _gx_utility_math_sin                  Compute the sine value        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_1bpp_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
GX_UBYTE *putrow;
GX_UBYTE *put;
GX_UBYTE *get;
INT       srcxres;
INT       srcyres;
INT       cosv;
INT       sinv;
INT       idxminx, idxmaxx, idxmaxy;
INT       mx[] = {-1, 1, 1, -1};
INT       my[] = {1, 1, -1, -1};
INT       xres;
INT       yres;
INT       width, height;
INT       x, y;
INT       xx, yy;
INT       putstride;
INT       getstride;
GX_UBYTE  putmask;
GX_UBYTE  putTransmask;
GX_UBYTE  getTransmask;
GX_UBYTE  getmask;
GX_BOOL   InputAlpha = GX_FALSE;

    idxminx = (angle / 90) & 0x3;
    idxmaxx = (idxminx + 2) & 0x3;
    idxmaxy = (idxminx + 1) & 0x3;

    /* Calculate the source x and y center. */
    srcxres = src->width >> 1;
    srcyres = src->height >> 1;

    cosv = _gx_utility_math_cos(GX_FIXED_VAL_MAKE(angle));
    sinv = _gx_utility_math_sin(GX_FIXED_VAL_MAKE(angle));

    xres = GX_FIXED_VAL_TO_INT((mx[idxmaxx] * (srcxres + 2) * cosv - my[idxmaxx] * (srcyres + 2) * sinv));
    yres = GX_FIXED_VAL_TO_INT((my[idxmaxy] * (srcyres + 2) * cosv + mx[idxmaxy] * (srcxres + 2) * sinv));

    /* Calculate destination width and height. */
    width = (xres << 1);
    height = (yres << 1);

    if (rot_cx && rot_cy)
    {
        /* Calculate the new rotation axis. */
        x = ((*rot_cx) - srcxres) * cosv - ((*rot_cy) - srcyres) * sinv;
        y = ((*rot_cy) - srcyres) * cosv + ((*rot_cx) - srcxres) * sinv;

        x = GX_FIXED_VAL_TO_INT(x) + xres;
        y = GX_FIXED_VAL_TO_INT(y) + yres;

        srcxres = *rot_cx;
        srcyres = *rot_cy;

        *rot_cx = x;
        *rot_cy = y;

        xres = *rot_cx;
        yres = *rot_cy;
    }

    /* Set width and height of destination pixelmap.  */
    destination->width = (GX_VALUE)width;
    destination->height = (GX_VALUE)height;
    destination->flags |= GX_PIXELMAP_TRANSPARENT;
    putstride = (width + 3) >> 2;

    if (src->flags & GX_PIXELMAP_TRANSPARENT)
    {
        getstride = (src->width + 3) >> 2;
        InputAlpha = GX_TRUE;
    }
    else
    {
        getstride = (src->width + 7) >> 3;
    }

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->data_size = (UINT)(putstride * height) * sizeof(GX_UBYTE);
    /* Allocate memory for destination pixelmap to load pixel information.  */
    destination->data = (GX_UBYTE *)_gx_system_memory_allocator(destination->data_size);

    if (destination->data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    putrow = (GX_UBYTE *)destination->data;

    /* For every pixel in destination bitmap, find its position in source bitmap,
       and set the pixel with the value in source bitmap.  */
    for (y = 0; y < height; y++)
    {
        put = putrow;
        putmask = 0x80;
        putTransmask = 0x40;

        for (x = 0; x < width; x++)
        {
            xx = (x - xres) * cosv + (y - yres) * sinv;
            yy = (y - yres) * cosv - (x - xres) * sinv;

            xx = GX_FIXED_VAL_TO_INT(xx) + srcxres;
            yy = GX_FIXED_VAL_TO_INT(yy) + srcyres;

            *put = (GX_UBYTE)(*put & (~putTransmask));
            *put = (GX_UBYTE)(*put & (~putmask));
            if ((xx >= 0) && (xx < src->width) &&
                (yy >= 0) && (yy < src->height))
            {
                get = (GX_UBYTE *)src->data;
                get += yy * getstride;
                if (InputAlpha)
                {
                    get += xx >> 2;
                    getTransmask = (GX_UBYTE)(0x40 >> ((xx & 0x03) << 1));
                    if (*get & getTransmask)
                    {
                        *put |= putTransmask;
                        getmask = (GX_UBYTE)(getTransmask << 1);
                        if (*get & getmask)
                        {
                            *put |= putmask;
                        }
                    }
                }
                else
                {
                    *put |= putTransmask;
                    get += xx >> 3;
                    getmask = (GX_UBYTE)(0x80 >> (xx & 0x07));
                    if (*get & getmask)
                    {
                        *put |= putmask;
                    }
                }
            }

            putTransmask >>= 2;
            putmask >>= 2;

            if (putTransmask == 0)
            {
                put++;
                putTransmask = 0x40;
                putmask = 0x80;
            }
        }
        putrow += putstride;
    }

    return GX_SUCCESS;
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_1bpp_pixelmap_simple_raw_rotate                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles 90, 180 and 270 degree        */
/*      rotation of an uncompressed pixelmap with or wihout alpha.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The pixelmap to be rotated    */
/*    angle                                 The angle to be rotated       */
/*    destination                           The rotated bitmap to be      */
/*                                            returned                    */
/*    rot_cx                                X coordinate of rotation      */
/*                                            center                      */
/*    rot_cy                                Y coordinate of rotation      */
/*                                            center                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           Memory Allocation routine     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_1bpp_pixelmap_simple_rotate                             */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_1bpp_pixelmap_simple_raw_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
GX_UBYTE *put;
GX_UBYTE *putrow;
GX_UBYTE *get;
INT       width, height;
INT       x, y;
INT       putstride;
GX_UBYTE  getmask;
GX_UBYTE  putmask;
INT       getstride;

    getstride = (src->width + 7) >> 3;

    width = src->height;
    height = src->width;

    if (angle == 180)
    {
        GX_SWAP_VALS(width, height);
    }

    putstride = (width + 7) >> 3;

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->data_size = (UINT)(putstride * height) * sizeof(GX_UBYTE);
    destination->data = (GX_UBYTE *)_gx_system_memory_allocator(destination->data_size);

    if (destination->data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    if (angle == 90)
    {
        putrow = (GX_UBYTE *)destination->data;

        for (y = 0; y < height; y++)
        {
            put = putrow;
            putmask = 0x80;
            for (x = 0; x < width; x++)
            {
                get = (GX_UBYTE *)src->data;
                get += (width - 1 - x) * getstride;
                get += y >> 3;
                getmask = (GX_UBYTE)(0x80 >> (y & 0x07));

                if (*get & getmask)
                {
                    *put |= putmask;
                }
                else
                {
                    *put &= (GX_UBYTE)(~putmask);
                }
                putmask >>= 1;
                if (putmask == 0)
                {
                    put++;
                    putmask = 0x80;
                }
            }
            putrow += putstride;
        }

        if (rot_cx && rot_cy)
        {
            x = *rot_cx;
            y = *rot_cy;

            *rot_cx = (width - 1 - y);
            *rot_cy = x;
        }
    }
    else if (angle == 180)
    {

        putrow = (GX_UBYTE *)destination->data;

        for (y = 0; y < height; y++)
        {
            put = putrow;
            putmask = 0x80;
            for (x = 0; x < width; x++)
            {
                getmask = (GX_UBYTE)(0x80 >> ((width - 1 - x) & 0x07));

                get = (GX_UBYTE *)src->data;
                get += (height - 1 - y) * getstride;
                get += (width - 1 - x) >> 3;

                if (*get & getmask)
                {
                    *put |= putmask;
                }
                else
                {
                    *put &= (GX_UBYTE)(~putmask);
                }
                putmask >>= 1;
                if (putmask == 0)
                {
                    put++;
                    putmask = 0x80;
                }
            }
            putrow += putstride;
        }
        if (rot_cx && rot_cy)
        {
            x = *rot_cx;
            y = *rot_cy;

            *rot_cx = (width - 1 - x);
            *rot_cy = (height - 1 - y);
        }
    }
    else
    {
        /* angle = 270. */
        putrow = (GX_UBYTE *)destination->data;

        for (y = 0; y < height; y++)
        {
            put = putrow;
            putmask = 0x80;

            for (x = 0; x < width; x++)
            {
                get = (GX_UBYTE *)src->data;
                get += x * getstride;
                get += (height - 1 - y) >> 3;

                getmask = (GX_UBYTE)(0x80 >> ((height - 1 - y) & 0x07));

                if (getmask & (*get))
                {
                    *put |= putmask;
                }
                else
                {
                    *put &= (GX_UBYTE)(~putmask);
                }
                putmask >>= 1;
                if (putmask == 0)
                {
                    put++;
                    putmask = 0x80;
                }
            }
            putrow += putstride;
        }

        if (rot_cx && rot_cy)
        {
            x = *rot_cx;
            y = *rot_cy;

            *rot_cx = y;
            *rot_cy = (height - 1 - x);
        }
    }

    destination->height = (GX_VALUE)height;
    destination->width = (GX_VALUE)width;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_1bpp_pixelmap_simple_transparent_rotate                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles 90, 180 and 270 degree        */
/*      rotation of an uncompressed pixelmap with or wihout alpha.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The pixelmap to be rotated    */
/*    angle                                 The angle to be rotated       */
/*    destination                           The rotated bitmap to be      */
/*                                            returned                    */
/*    rot_cx                                X coordinate of rotation      */
/*                                            center                      */
/*    rot_cy                                Y coordinate of rotation      */
/*                                            center                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           Memory Allocation routine     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_1bpp_pixelmap_simple_rotate                             */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_1bpp_pixelmap_simple_transparent_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
GX_UBYTE *put;
GX_UBYTE *putrow;
GX_UBYTE *get;
INT       width, height;
INT       x, y;
INT       putstride;
GX_UBYTE  getmask;
GX_UBYTE  putmask;
GX_UBYTE  puttransmask;
GX_UBYTE  gettransmask;
INT       getstride;

    getstride = (src->width + 3) >> 2;

    width = src->height;
    height = src->width;

    if (angle == 180)
    {
        GX_SWAP_VALS(width, height);
    }

    putstride = (width + 3) >> 2;

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->data_size = (UINT)(putstride * height) * sizeof(GX_UBYTE);
    destination->data = (GX_UBYTE *)_gx_system_memory_allocator(destination->data_size);

    if (destination->data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    if (angle == 90)
    {
        putrow = (GX_UBYTE *)destination->data;

        for (y = 0; y < height; y++)
        {
            put = putrow;
            puttransmask = 0x40;
            putmask = 0x80;

            getmask = (GX_UBYTE)(0x80 >> ((y & 0x03) << 1));
            gettransmask = (getmask >> 1);

            for (x = 0; x < width; x++)
            {
                *put &= (GX_UBYTE)(~puttransmask);
                *put &= (GX_UBYTE)(~putmask);

                get = (GX_UBYTE *)src->data;
                get += (width - 1 - x) * getstride;
                get += y >> 2;

                if (*get & gettransmask)
                {
                    *put |= puttransmask;

                    if (*get & getmask)
                    {
                        *put |= putmask;
                    }
                }
                puttransmask >>= 2;
                putmask >>= 2;
                if (puttransmask == 0)
                {
                    put++;
                    putmask = 0x80;
                    puttransmask = 0x40;
                }
            }
            putrow += putstride;
        }

        if (rot_cx && rot_cy)
        {
            x = *rot_cx;
            y = *rot_cy;

            *rot_cx = (width - 1 - y);
            *rot_cy = x;
        }
    }
    else if (angle == 180)
    {
        putrow = (GX_UBYTE *)destination->data;

        for (y = 0; y < height; y++)
        {
            put = putrow;
            putmask = 0x80;
            puttransmask = 0x40;
            for (x = 0; x < width; x++)
            {
                /* set bits first. */
                *put &= (GX_UBYTE)(~putmask);
                *put &= (GX_UBYTE)(~puttransmask);

                gettransmask = (GX_UBYTE)(0x40 >> (((width - 1 - x) & 0x03) << 1));

                get = (GX_UBYTE *)src->data;
                get += (height - 1 - y) * getstride;
                get += (width - 1 - x) >> 2;

                if (*get & gettransmask)
                {
                    *put |= puttransmask;
                    getmask = (GX_UBYTE)(gettransmask << 1);
                    if (*get & getmask)
                    {
                        *put |= putmask;
                    }
                }

                putmask >>= 2;
                puttransmask >>= 2;
                if (puttransmask == 0)
                {
                    put++;
                    putmask = 0x80;
                    puttransmask = 0x40;
                }
            }
            putrow += putstride;
        }
        if (rot_cx && rot_cy)
        {
            x = *rot_cx;
            y = *rot_cy;

            *rot_cx = (width - 1 - x);
            *rot_cy = (height - 1 - y);
        }
    }
    else
    {
        /* angle = 270. */
        putrow = (GX_UBYTE *)destination->data;

        for (y = 0; y < height; y++)
        {
            put = putrow;
            putmask = 0x80;
            puttransmask = 0x40;

            for (x = 0; x < width; x++)
            {
                /* set bits first. */
                *put &= (GX_UBYTE)(~putmask);
                *put &= (GX_UBYTE)(~puttransmask);

                get = (GX_UBYTE *)src->data;
                get += x * getstride;
                get += (height - 1 - y) >> 2;

                gettransmask = (GX_UBYTE)(0x40 >> (((height - 1 - y) & 0x03) << 1));
                if (gettransmask & (*get))
                {
                    *put |= puttransmask;
                    getmask = (GX_UBYTE)(gettransmask << 1);
                    if (getmask & (*get))
                    {
                        *put |= putmask;
                    }
                }

                putmask >>= 2;
                puttransmask >>= 2;
                if (puttransmask == 0)
                {
                    put++;
                    putmask = 0x80;
                    puttransmask = 0x40;
                }
            }
            putrow += putstride;
        }

        if (rot_cx && rot_cy)
        {
            x = *rot_cx;
            y = *rot_cy;

            *rot_cx = y;
            *rot_cy = (height - 1 - x);
        }
    }

    destination->height = (GX_VALUE)height;
    destination->width = (GX_VALUE)width;

    return GX_SUCCESS;
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_1bpp_pixelmap_simple_rotate                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles 90, 180 and 270 degree        */
/*      rotation of an uncompressed pixelmap with or wihout alpha.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The pixelmap to be rotated    */
/*    angle                                 The angle to be rotated       */
/*    destination                           The rotated bitmap to be      */
/*                                            returned                    */
/*    rot_cx                                X coordinate of rotation      */
/*                                            center                      */
/*    rot_cy                                Y coordinate of rotation      */
/*                                            center                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_1bpp_pixelmap_simple_transparent_rotate                 */
/*                                          Real pixelmap rotate function */
/*    _gx_utility_1bpp_pixelmap_simple_raw_rotate                         */
/*                                          Real pixelmap rotate function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_1bpp_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
UINT status;

    if (src->flags & GX_PIXELMAP_TRANSPARENT)
    {
        /* with transparent, no compression */
        status = _gx_utility_1bpp_pixelmap_simple_transparent_rotate(src, angle, destination, rot_cx, rot_cy);
    }
    else
    {
        /* no compression or transparent */
        status = _gx_utility_1bpp_pixelmap_simple_raw_rotate(src, angle, destination, rot_cx, rot_cy);
    }

    return status;
}

