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
/*    _gx_utility_4bpp_pixelmap_raw_resize                                */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    4bpp pixelmap resize function that handles uncompress, with or      */
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
/*    _gx_utility_4bpp_pixelmap_resize                                    */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_4bpp_pixelmap_raw_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height)
{
/* The pixelmap resize function is implemented from nearest neighbor
   image scaling algorithm.  */

GX_UBYTE *get;
GX_UBYTE *put;
GX_UBYTE *putrow;
GX_UBYTE  putmask;
INT       putstride;
INT       getstride;
GX_UBYTE  pixel;
INT       xradio;
INT       yradio;
INT       x;
INT       y;
INT       xx;
INT       yy;

    /* Calculate scale ratio and enlarge it by 256 times to keep precision.  */
    xradio = ((src->width) << 8) / width;
    yradio = ((src->height) << 8) / height;

    putstride = (width + 1) >> 1;
    getstride = (src->width + 1) >> 1;

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
        putmask = 0xf0;
        for (x = 0; x < width; x++)
        {
            xx = (xradio * x) >> 8;
            yy = (yradio * y) >> 8;

            get = (GX_UBYTE *)src->data;
            get += yy * getstride;
            get += xx >> 1;
            if (xx & 1)
            {
                pixel = *get & 0x0f;
            }
            else
            {
                pixel = *get >> 4;
            }
            pixel |= (GX_UBYTE)(pixel << 4);

            *put &= (GX_UBYTE)(~putmask);
            *put |= putmask & pixel;

            putmask >>= 4;
            if (putmask == 0)
            {
                put++;
                putmask = 0xf0;
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
/*    _gx_utility_4bpp_pixelmap_transparent_resize                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    4bpp pixelmap resize function that handles uncompress, with or      */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_4bpp_pixelmap_transparent_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height)
{
/* The pixelmap resize function is implemented from nearest neighbor
   image scaling algorithm.  */
GX_UBYTE *get;
GX_UBYTE *getaux;
GX_UBYTE *put;
GX_UBYTE *putrow;
GX_UBYTE *putaux;
GX_UBYTE *putauxrow;
GX_UBYTE  putmask;
INT       putstride;
INT       putauxstride;
INT       getstride;
INT       getauxstride;
GX_UBYTE  transmask;
GX_UBYTE  putauxmask;
GX_UBYTE  pixel;
INT       xradio;
INT       yradio;
INT       x;
INT       y;
INT       xx;
INT       yy;

    /* Calculate scale ratio and enlarge it by 256 times to keep precision.  */
    xradio = ((src->width) << 8) / width;
    yradio = ((src->height) << 8) / height;

    putstride = (width + 1) >> 1;
    putauxstride = (width + 7) >> 3;
    getstride = (src->width + 1) >> 1;
    getauxstride = (src->width + 7) >> 3;

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

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->aux_data_size = (UINT)(height * putauxstride) * sizeof(GX_UBYTE);
    destination->aux_data = (GX_UBYTE *)_gx_system_memory_allocator(destination->aux_data_size);

    if (destination->aux_data == GX_NULL)
    {
        _gx_system_memory_free((void *)destination->data);
        return GX_SYSTEM_MEMORY_ERROR;
    }

    putrow = (GX_UBYTE *)destination->data;
    putauxrow = (GX_UBYTE *)destination->aux_data;

    /* Loop through destination's pixel and fill each pixel with the nearest neighbor.  */
    for (y = 0; y < height; y++)
    {
        put = putrow;
        putaux = putauxrow;
        putmask = 0xf0;
        putauxmask = 0x80;
        for (x = 0; x < width; x++)
        {
            xx = (xradio * x) >> 8;
            yy = (yradio * y) >> 8;

            /* set bits first. */
            *put &= (GX_UBYTE)(~putmask);
            getaux = (GX_UBYTE *)src->aux_data;
            getaux += yy * getauxstride;
            getaux += xx >> 3;

            transmask = (GX_UBYTE)(0x80 >> (xx & 0x07));
            if (transmask & (*getaux))
            {
                /* set tranparent aux bit first. */
                *putaux |= putauxmask;
            }
            else
            {
                *putaux &= (GX_UBYTE)(~putauxmask);

                /* get pixel data */
                get = (GX_UBYTE *)src->data;
                get += yy * getstride;
                get += xx >> 1;
                if (xx & 1)
                {
                    pixel = *get & 0x0f;
                }
                else
                {
                    pixel = *get >> 4;
                }
                pixel |= (GX_UBYTE)(pixel << 4);
                *put |= putmask & pixel;
            }
            putauxmask >>= 1;
            if (putauxmask == 0)
            {
                putauxmask = 0x80;
                putaux++;
            }

            putmask >>= 4;
            if (putmask == 0)
            {
                put++;
                putmask = 0xf0;
            }
        }
        putrow += putstride;
        putauxrow += putauxstride;
    }

    return GX_SUCCESS;
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_4bpp_pixelmap_resize                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    4bpp pixelmap resize function that handles uncompress, with or      */
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
/*    _gx_utility_4bpp_pixelmap_transparent_resize                        */
/*                                          Real pixelmap resize routine  */
/*    _gx_utility_4bpp_pixelmap_raw_resize  Real pixelmap resize routine  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_4bpp_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height)
{
UINT status;

    if (src->flags & GX_PIXELMAP_TRANSPARENT)
    {
        /* transparent, no compression */
        status = _gx_utility_4bpp_pixelmap_transparent_resize(src, destination, width, height);
    }
    else
    {
        /* no compression or alpha */
        status = _gx_utility_4bpp_pixelmap_raw_resize(src, destination, width, height);
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_4bpp_pixelmap_rotate                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    4bpp pixelmap rotation function that handles uncompress, with or    */
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
UINT _gx_utility_4bpp_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
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
INT       putauxstride;
INT       getauxstride;
GX_UBYTE  putmask;
GX_UBYTE  pixel;
GX_UBYTE *putaux;
GX_UBYTE *putauxrow;
GX_UBYTE  putauxmask;
GX_UBYTE  transmask;
GX_UBYTE *getaux;
GX_BOOL   draw = GX_TRUE;

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
    putstride = (width + 1) >> 1;
    putauxstride = (width + 7) >> 3;
    getstride = (src->width + 1) >> 1;
    getauxstride = 0;

    if (src->flags & GX_PIXELMAP_TRANSPARENT)
    {
        getauxstride = (src->width + 7) >> 3;
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

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->aux_data_size = (UINT)(putauxstride * height) * sizeof(GX_UBYTE);

    /* Allocate memory for destination pixelmap to load pixel information.  */
    destination->aux_data = (GX_UBYTE *)_gx_system_memory_allocator(destination->aux_data_size);

    if (destination->aux_data == GX_NULL)
    {
        _gx_system_memory_free((void *)destination->data);
        return GX_SYSTEM_MEMORY_ERROR;
    }

    putrow = (GX_UBYTE *)destination->data;
    putauxrow = (GX_UBYTE *)destination->aux_data;

    /* For every pixel in destination bitmap, find its position in source bitmap,
       and set the pixel with the value in source bitmap.  */
    for (y = 0; y < height; y++)
    {
        put = putrow;
        putaux = putauxrow;
        putmask = 0xf0;
        putauxmask = 0x80;

        for (x = 0; x < width; x++)
        {
            xx = GX_FIXED_VAL_TO_INT((x - xres) * cosv + (y - yres) * sinv);
            yy = GX_FIXED_VAL_TO_INT((y - yres) * cosv - (x - xres) * sinv);

            xx += srcxres;
            yy += srcyres;

            if (getauxstride)
            {
                getaux = (GX_UBYTE *)src->aux_data;
                getaux += yy * getauxstride;
                getaux += xx >> 3;
                transmask = (GX_UBYTE)(0x80 >> (xx & 0x07));

                if (transmask & (*getaux))
                {
                    draw = GX_FALSE;
                }
                else
                {
                    draw = GX_TRUE;
                }
            }

            /* Set bits first. */
            *put &= (GX_UBYTE)(~putmask);
            if (draw && (xx >= 0) && (xx < src->width) &&
                (yy >= 0) && (yy < src->height))
            {
                get = (GX_UBYTE *)src->data;
                get += yy * getstride;
                get += xx >> 1;

                if (xx & 1)
                {
                    pixel = *get & 0x0f;
                }
                else
                {
                    pixel = (*get & 0xf0) >> 4;
                }
                pixel |= (GX_UBYTE)(pixel << 4);
                /* set pixel */
                *put |= pixel & putmask;
                /* Set transparent info. */
                *putaux &= (GX_UBYTE)(~putauxmask);
            }
            else
            {
                *putaux |= putauxmask;
            }

            putauxmask >>= 1;
            if (putauxmask == 0)
            {
                putaux++;
                putauxmask = 0x80;
            }

            putmask >>= 4;
            if (putmask == 0)
            {
                put++;
                putmask = 0xf0;
            }
        }
        putrow += putstride;
        putauxrow += putauxstride;
    }

    return GX_SUCCESS;
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_4bpp_pixelmap_simple_raw_rotate                         */
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
/*   _gx_utility_4bpp_pixelmap_simple_rotate                              */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_4bpp_pixelmap_simple_raw_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
GX_UBYTE *put;
GX_UBYTE *putrow;
GX_UBYTE *get;
INT       width, height;
INT       x, y;
INT       putstride;
GX_UBYTE  putmask;
GX_UBYTE  pixel;
INT       getstride;

    getstride = (src->width + 1) >> 1;

    width = src->height;
    height = src->width;

    if (angle == 180)
    {
        GX_SWAP_VALS(width, height);
    }

    putstride = (width + 1) >> 1;

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
            putmask = 0xf0;
            for (x = 0; x < width; x++)
            {
                get = (GX_UBYTE *)src->data;
                get += (width - 1 - x) * getstride;
                get += y >> 1;

                if (y & 1)
                {
                    pixel = *get & 0x0f;
                }
                else
                {
                    pixel = (*get & 0xf0) >> 4;
                }
                pixel |= (GX_UBYTE)(pixel << 4);
                *put &= (GX_UBYTE)(~putmask);
                *put |= pixel & putmask;

                putmask >>= 4;
                if (putmask == 0)
                {
                    put++;
                    putmask = 0xf0;
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
            putmask = 0xf0;
            for (x = 0; x < width; x++)
            {
                get = (GX_UBYTE *)src->data;
                get += (height - 1 - y) * getstride;
                get += (width - 1 - x) >> 1;

                if ((width - 1 - x) & 1)
                {
                    pixel = *get & 0x0f;
                }
                else
                {
                    pixel = (*get & 0xf0) >> 4;
                }
                pixel |= (GX_UBYTE)(pixel << 4);
                *put &= (GX_UBYTE)(~putmask);
                *put |= pixel & putmask;

                putmask >>= 4;
                if (putmask == 0)
                {
                    put++;
                    putmask = 0xf0;
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
            putmask = 0xf0;

            for (x = 0; x < width; x++)
            {
                get = (GX_UBYTE *)src->data;
                get += x * getstride;
                get += (height - 1 - y) >> 1;

                if ((height - 1 - y) & 1)
                {
                    pixel = *get & 0x0f;
                }
                else
                {
                    pixel = (*get & 0xf0) >> 4;
                }
                pixel |= (GX_UBYTE)(pixel << 4);
                *put &= (GX_UBYTE)(~putmask);
                *put |= pixel & putmask;

                putmask >>= 4;
                if (putmask == 0)
                {
                    put++;
                    putmask = 0xf0;
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
    destination->width  = (GX_VALUE)width;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_4bpp_pixelmap_simple_transparent_rotate                 */
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
/*   _gx_utility_4bpp_pixelmap_simple_rotate                              */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_4bpp_pixelmap_simple_transparent_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
GX_UBYTE *put;
GX_UBYTE *putrow;
GX_UBYTE *putaux;
GX_UBYTE *putauxrow;
GX_UBYTE *get;
INT       width, height;
INT       x, y;
INT       putstride;
INT       putauxstride;
GX_UBYTE  putmask;
GX_UBYTE  pixel;
GX_UBYTE  transmask;
GX_UBYTE  putauxmask;
GX_UBYTE *getaux;
INT       getstride;
INT       getauxstride;

    getstride = (src->width + 1) >> 1;
    getauxstride = (src->width + 7) >> 3;

    width = src->height;
    height = src->width;

    if (angle == 180)
    {
        GX_SWAP_VALS(width, height);
    }

    putstride = (width + 1) >> 1;
    putauxstride = (width + 7) >> 3;

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->data_size = (UINT)(putstride * height) * sizeof(GX_UBYTE);
    destination->data = (GX_UBYTE *)_gx_system_memory_allocator(destination->data_size);

    if (destination->data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->aux_data_size = (UINT)(putauxstride * height) * sizeof(GX_UBYTE);
    destination->aux_data = (GX_UBYTE *)_gx_system_memory_allocator(destination->aux_data_size);

    if (destination->aux_data == GX_NULL)
    {
        _gx_system_memory_free((void *)destination->data);
        return GX_SYSTEM_MEMORY_ERROR;
    }

    if (angle == 90)
    {
        putrow = (GX_UBYTE *)destination->data;
        putauxrow = (GX_UBYTE *)destination->aux_data;

        for (y = 0; y < height; y++)
        {
            put = putrow;
            putaux = putauxrow;
            putmask = 0xf0;
            putauxmask = 0x80;
            for (x = 0; x < width; x++)
            {
                /* set bits first. */
                *put &= (GX_UBYTE)(~putmask);
                getaux = (GX_UBYTE *)src->aux_data;
                getaux += (width - 1 - x) * getauxstride;
                getaux += y >> 3;

                transmask = (GX_UBYTE)(0x80 >> (y & 0x07));
                if (transmask & (*getaux))
                {
                    /* transparent pixel, set destination true */
                    *putaux |= putauxmask;
                }
                else
                {
                    /* not transparent, set destination bit to 0. */
                    *putaux &= (GX_UBYTE)(~putauxmask);

                    get = (GX_UBYTE *)src->data;
                    get += (width - 1 - x) * getstride;
                    get += y >> 1;

                    if (y & 1)
                    {
                        pixel = *get & 0x0f;
                    }
                    else
                    {
                        pixel = (*get & 0xf0) >> 4;
                    }
                    pixel |= (GX_UBYTE)(pixel << 4);

                    /* set data. */
                    *put |= pixel & putmask;
                }

                putmask >>= 4;
                if (putmask == 0)
                {
                    put++;
                    putmask = 0xf0;
                }
                putauxmask >>= 1;
                if (putauxmask == 0)
                {
                    putaux++;
                    putauxmask = 0x80;
                }
            }
            putrow += putstride;
            putauxrow += putauxstride;
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
        putauxrow = (GX_UBYTE *)destination->aux_data;

        for (y = 0; y < height; y++)
        {
            put = putrow;
            putaux = putauxrow;
            putmask = 0xf0;
            putauxmask = 0x80;
            for (x = 0; x < width; x++)
            {
                /* set bits first. */
                *put &= (GX_UBYTE)(~putmask);
                getaux = (GX_UBYTE *)src->aux_data;
                getaux += (height - 1 - y) * getauxstride;
                getaux += (width - 1 - x) >> 3;

                transmask = (GX_UBYTE)(0x80 >> ((width - 1 - x) & 0x07));
                if (transmask & (*getaux))
                {
                    /* transparent pixel, set destination true */
                    *putaux |= putauxmask;
                }
                else
                {
                    /* not transparent, set destination bit to 0. */
                    *putaux &= (GX_UBYTE)(~putauxmask);

                    get = (GX_UBYTE *)src->data;
                    get += (height - 1 - y) * getstride;
                    get += (width - 1 - x) >> 1;

                    if ((width - 1 - x) & 1)
                    {
                        pixel = *get & 0x0f;
                    }
                    else
                    {
                        pixel = (*get & 0xf0) >> 4;
                    }
                    pixel |= (GX_UBYTE)(pixel << 4);
                    *put |= pixel & putmask;
                }
                putmask >>= 4;
                if (putmask == 0)
                {
                    put++;
                    putmask = 0xf0;
                }
                putauxmask >>= 1;
                if (putauxmask == 0)
                {
                    putaux++;
                    putauxmask = 0x80;
                }
            }
            putrow += putstride;
            putauxrow += putauxstride;
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
        putauxrow = (GX_UBYTE *)destination->aux_data;

        for (y = 0; y < height; y++)
        {
            put = putrow;
            putaux = putauxrow;
            putmask = 0xf0;
            putauxmask = 0x80;

            for (x = 0; x < width; x++)
            {
                /* set bits first. */
                *put &= (GX_UBYTE)(~putmask);
                getaux = (GX_UBYTE *)src->aux_data;
                getaux += x * getauxstride;
                getaux += (height - 1 - y) >> 3;

                transmask = (GX_UBYTE)(0x80 >> ((height - 1 - y) & 0x07));
                if (transmask & (*getaux))
                {
                    /* transparent pixel, set destination true */
                    *putaux |= putauxmask;
                }
                else
                {
                    /* not transparent, set destination bit to 0. */
                    *putaux &= (GX_UBYTE)(~putauxmask);

                    get = (GX_UBYTE *)src->data;
                    get += x * getstride;
                    get += (height - 1 - y) >> 1;

                    if ((height - 1 - y) & 1)
                    {
                        pixel = *get & 0x0f;
                    }
                    else
                    {
                        pixel = (*get & 0xf0) >> 4;
                    }
                    pixel |= (GX_UBYTE)(pixel << 4);
                    *put |= pixel & putmask;
                }
                putmask >>= 4;
                if (putmask == 0)
                {
                    put++;
                    putmask = 0xf0;
                }
                putauxmask >>= 1;
                if (putauxmask == 0)
                {
                    putaux++;
                    putauxmask = 0x80;
                }
            }
            putrow += putstride;
            putauxrow += putauxstride;
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
/*    _gx_utility_4bpp_pixelmap_simple_rotate                             */
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
/*    _gx_utility_4bpp_pixelmap_simple_transparent_rotate                 */
/*                                          Real pixelmap rotate function */
/*    _gx_utility_4bpp_pixelmap_simple_raw_rotate                         */
/*                                          Real pixelmap rotate function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_4bpp_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
UINT status;

    if (src->flags & GX_PIXELMAP_TRANSPARENT)
    {
        /* with transparent, no compression */
        status = _gx_utility_4bpp_pixelmap_simple_transparent_rotate(src, angle, destination, rot_cx, rot_cy);
    }
    else
    {
        /* no compression or transparent */
        status = _gx_utility_4bpp_pixelmap_simple_raw_rotate(src, angle, destination, rot_cx, rot_cy);
    }

    return status;
}

