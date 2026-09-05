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
#include "gx_color.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_4444argb_pixelmap_resize                                */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    4444argb pixelmap resize function that handles uncompress,          */
/*    with alpha channel.                                                 */
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
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_4444argb_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height)
{
USHORT  *get;
USHORT  *put;
INT      xdiff;
INT      ydiff;
INT      xradio;
INT      yradio;
INT      x;
INT      y;
INT      xx;
INT      yy;
USHORT   neighbor_pixels[2][2];
USHORT   alpha[4];

    /* Calculate scale ratio and enlarge it by 256 times to keep precision.  */
    xradio = ((src->width) << 8) / width;
    yradio = ((src->height) << 8) / height;

    /* Fill property values into destination pixelmap structure. */
    destination->flags = src->flags;
    destination->format = src->format;

    destination->height = (GX_VALUE)height;
    destination->width = (GX_VALUE)width;

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->data_size = (UINT)(height * width) * sizeof(USHORT);

    /* Allocate memory to load pixelmap data. */
    destination->data = (GX_UBYTE *)_gx_system_memory_allocator(destination->data_size);

    if (destination->data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    put = (USHORT *)destination->data;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            xx = (xradio * x) >> 8;
            yy = (yradio * y) >> 8;

            /* The coordinates of the original source pixel are truncate value,
               calucate their distance between the mathematical coordinates. */
            xdiff = (xradio * x) & 0xff;
            ydiff = (yradio * y) & 0xff;

            get = (USHORT *)src->data;
            get += yy * src->width;
            get += xx;

            neighbor_pixels[0][0] = *get;

            if ((xx < src->width - 1) && (yy < src->height - 1))
            {
                neighbor_pixels[0][1] = *(get + 1);
                neighbor_pixels[1][0] = *(get + src->width);
                neighbor_pixels[1][1] = *(get + src->width + 1);
            }
            else
            {
                if ((xx == src->width - 1) &&
                    (yy == src->height - 1))
                {
                    /* Handle right bottom corder pixel.  */
                    neighbor_pixels[0][1] = neighbor_pixels[0][0];
                    neighbor_pixels[1][0] = neighbor_pixels[0][0];
                    neighbor_pixels[1][1] = neighbor_pixels[0][0];
                }
                else if (xx == src->width - 1)
                {
                    /* Handle pixels in right edge.  */
                    neighbor_pixels[0][1] = neighbor_pixels[0][0];
                    neighbor_pixels[1][0] = *(get + src->width);
                    neighbor_pixels[1][1] = neighbor_pixels[1][0];
                }
                else
                {
                    /* Handle pixels in bottom edge.  */
                    neighbor_pixels[0][1] = *(get + 1);
                    neighbor_pixels[1][0] = neighbor_pixels[0][0];
                    neighbor_pixels[1][1] = neighbor_pixels[0][1];
                }
            }
            alpha[0] = ALPHAVAL_4444ARGB(neighbor_pixels[0][0]);
            alpha[1] = ALPHAVAL_4444ARGB(neighbor_pixels[0][1]);
            alpha[2] = ALPHAVAL_4444ARGB(neighbor_pixels[1][0]);
            alpha[3] = ALPHAVAL_4444ARGB(neighbor_pixels[1][1]);

            /* Calulate pixel values by interpolating 4 neighboring pixels. */
            *put++ = gx_color_4444argb_alpha_from_4colors(neighbor_pixels[0][0],
                                                          neighbor_pixels[0][1],
                                                          neighbor_pixels[1][0],
                                                          neighbor_pixels[1][1],
                                                          xdiff, ydiff,
                                                          alpha[0], alpha[1], alpha[2], alpha[3]);
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_4444argb_pixelmap_rotate                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper funciton that rotate an 4444argb format uncompressed*/
/*    pixelmap.                                                           */
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
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           Application defined memory    */
/*                                            allocation function         */
/*    _gx_utility_math_cos                  Calculate cosine of supplied  */
/*                                            angle                       */
/*    _gx_utility_math_sin                  Calculate sine of supplied    */
/*                                            angle                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_4444argb_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
USHORT *get;
USHORT *put;
INT     srcxres;
INT     srcyres;
INT     cosv;
INT     sinv;
INT     idxminx, idxmaxx, idxmaxy;
INT    *mx;
INT    *my;
INT     xres;
INT     yres;
INT     width, height;
INT     x, y;
INT     xx, yy;
USHORT  a, b, c, d;
USHORT  alpha[4];
INT     xdiff, ydiff;

    mx = _gx_system_scratchpad;
    my = mx + 4;

    mx[0] = mx[3] = -1;
    mx[1] = mx[2] = 1;

    my[0] = my[1] = 1;
    my[2] = my[3] = -1;

    idxminx = (angle / 90) & 0x3;
    idxmaxx = (idxminx + 2) & 0x3;
    idxmaxy = (idxminx + 1) & 0x3;

    /* Calculate the source x and y center. */
    srcxres = src->width >> 1;
    srcyres = src->height >> 1;

    cosv = _gx_utility_math_cos(GX_FIXED_VAL_MAKE(angle));
    sinv = _gx_utility_math_sin(GX_FIXED_VAL_MAKE(angle));

    xres = mx[idxmaxx] * (srcxres + 2) * cosv - my[idxmaxx] * (srcyres + 2) * sinv;
    yres = my[idxmaxy] * (srcyres + 2) * cosv + mx[idxmaxy] * (srcxres + 2) * sinv;

    xres = GX_FIXED_VAL_TO_INT(xres);
    yres = GX_FIXED_VAL_TO_INT(yres);

    /* Calculate destination width and height. */
    width = (xres << 1);
    height = (yres << 1);

    /* Calculate the new rotation axis. */
    if (rot_cx && rot_cy)
    {
        x = ((*rot_cx) - srcxres) * cosv - ((*rot_cy) - srcyres) * sinv;
        y = ((*rot_cy) - srcyres) * cosv + ((*rot_cx) - srcxres) * sinv;

        srcxres = *rot_cx;
        srcyres = *rot_cy;

        x = GX_FIXED_VAL_TO_INT(x) + xres;
        y = GX_FIXED_VAL_TO_INT(y) + yres;

        *rot_cx = x;
        *rot_cy = y;

        xres = *rot_cx;
        yres = *rot_cy;
    }

    destination->height = (GX_VALUE)height;
    destination->width = (GX_VALUE)width;
    destination->flags |= GX_PIXELMAP_ALPHA;

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->data_size = (UINT)(height * width) * sizeof(USHORT);
    destination->data = (GX_UBYTE *)_gx_system_memory_allocator(destination->data_size);

    if (destination->data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    put = (USHORT *)destination->data;

    /* Loop through the destination's pixels.  */
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            xx = (x - xres) * cosv + (y - yres) * sinv;
            yy = (y - yres) * cosv - (x - xres) * sinv;

            xdiff = GX_FIXED_VAL_TO_INT(xx << 8) & 0xff;
            ydiff = GX_FIXED_VAL_TO_INT(yy << 8) & 0xff;

            xx = GX_FIXED_VAL_TO_INT(xx) + srcxres;
            yy = GX_FIXED_VAL_TO_INT(yy) + srcyres;

            if ((xx >= -1) && (xx < src->width) &&
                (yy >= -1) && (yy < src->height))
            {

                if ((xx >= 0) && (xx < src->width - 1) &&
                    (yy >= 0) && (yy < src->height - 1))
                {
                    get = (USHORT *)src->data;
                    get += yy * src->width;
                    get += xx;

                    a = *get;
                    b = *(get + 1);
                    c = *(get + src->width);
                    d = *(get + src->width + 1);
                    alpha[0] = ALPHAVAL_4444ARGB(a);
                    alpha[1] = ALPHAVAL_4444ARGB(b);
                    alpha[2] = ALPHAVAL_4444ARGB(c);
                    alpha[3] = ALPHAVAL_4444ARGB(d);
                }
                else
                {
                    get = (USHORT *)src->data;

                    a = 0;
                    b = 0;
                    c = 0;
                    d = 0;

                    if (xx == -1)
                    {
                        /* handle left edge.  */
                        if (yy >= 0)
                        {
                            b = *(get + yy * src->width);
                            alpha[1] = ALPHAVAL_4444ARGB(b);
                        }

                        if (yy < src->height - 1)
                        {
                            d = *(get + (yy + 1) * src->width);
                            alpha[3] = ALPHAVAL_4444ARGB(d);
                        }
                    }
                    else if (yy == -1)
                    {
                        /* handle top edge.  */
                        c = *(get + xx);
                        alpha[2] = ALPHAVAL_4444ARGB(c);

                        if (xx < src->width - 1)
                        {
                            d = *(get + xx + 1);
                            alpha[3] = ALPHAVAL_4444ARGB(d);
                        }
                    }
                    else if (xx == src->width - 1)
                    {
                        /* handle right edget. */
                        a = *(get + yy * src->width + xx);
                        alpha[0] = ALPHAVAL_4444ARGB(a);

                        if (yy < src->height - 1)
                        {
                            c = *(get + (yy + 1) * src->width + xx);
                            alpha[2] = ALPHAVAL_4444ARGB(c);
                        }
                    }
                    else
                    {
                        /* handle bottom edge. */
                        a = *(get + yy * src->width + xx);
                        alpha[0] = ALPHAVAL_4444ARGB(a);

                        b = *(get + yy * src->width + xx + 1);
                        alpha[1] = ALPHAVAL_4444ARGB(b);
                    }

                    if (!a)
                    {
                        alpha[0] = 0;
                    }

                    if (!b)
                    {
                        alpha[1] = 0;
                    }

                    if (!c)
                    {
                        alpha[2] = 0;
                    }

                    if (!d)
                    {
                        alpha[3] = 0;
                    }
                }

                *put++ = gx_color_4444argb_alpha_from_4colors(a, b, c, d, xdiff, ydiff,
                                                              alpha[0], alpha[1], alpha[2], alpha[3]);
            }
            else
            {
                *put++ = 0;
            }
        }
    }
    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_4444argb_pixelmap_simple_rotate                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    4444argb pixelmap rotation function that handles 90, 180 and 270    */
/*    degree case.                                                        */
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
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           Application defined memory    */
/*                                            allocation function         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_4444argb_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
USHORT *put;
USHORT *get;
INT     width;
INT     height;
INT     x;
INT     y;

    width = src->height;
    height = src->width;

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->data_size = (UINT)(width * height) * sizeof(USHORT);
    destination->data = (GX_UBYTE *)_gx_system_memory_allocator(destination->data_size);

    if (destination->data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    if (angle == 90)
    {
        put = (USHORT *)destination->data;

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                get = (USHORT *)src->data;
                get += (width - 1 - x) * height;
                get += y;

                *put++ = *get;
            }
        }

        if (rot_cx && rot_cy)
        {
            x = *rot_cx;
            y = *rot_cy;

            /* Get new rotation point. */
            *rot_cx = (width - 1 - y);
            *rot_cy = x;
        }
    }
    else if (angle == 180)
    {
        GX_SWAP_VALS(width, height);
        put = (USHORT *)destination->data;
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                get = (USHORT *)src->data;
                get += (height - 1 - y) * width;
                get += width - 1 - x;

                *put++ = *get;
            }
        }

        if (rot_cx && rot_cy)
        {
            x = *rot_cx;
            y = *rot_cy;

            /* Get new rotation point. */
            *rot_cx = (width - 1 - x);
            *rot_cy = (height - 1 - y);
        }
    }
    else
    {
        /* angle  = 270. */
        put = (USHORT *)destination->data;

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                get = (USHORT *)src->data;
                get += x * height;
                get += height - 1 - y;

                *put++ = *get;
            }
        }

        if (rot_cx && rot_cy)
        {
            x = *rot_cx;
            y = *rot_cy;

            /* Get new rotation point. */
            *rot_cx = y;
            *rot_cy = (height - 1 - x);
        }
    }

    destination->height = (GX_VALUE)height;
    destination->width = (GX_VALUE)width;

    return GX_SUCCESS;
}

