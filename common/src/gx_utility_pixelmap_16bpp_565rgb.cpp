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


#define REDVAL(_c)     (GX_UBYTE)(((_c) >> 11) & 0x1f)
#define GREENVAL(_c)   (GX_UBYTE)(((_c) >> 5) & 0x3f)
#define BLUEVAL(_c)    (GX_UBYTE)(((_c)) & 0x1f)

#define ASSEMBLECOLOR(_r, _g, _b)                     \
    (USHORT)((((_r) & 0x1f) << 11)                  | \
             (((_g) & 0x3f) << 5)                   | \
             ((_b) & 0x1f))

#define BYTE_RANGE(_c) _c > 255 ? 255 : _c

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_16bpp_pixelmap_raw_resize                               */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that resize an uncompressed pixelmap       */
/*      without alpha.                                                    */
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
/*    _gx_system_memory_allocator                                         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_16bpp_pixelmap_raw_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height)
{
/* The pixelmap resize function is implemented from bilinear interpolation
   image scaling algorithm.  */

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
GX_COLOR red;
GX_COLOR green;
GX_COLOR blue;

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

    /* Allocate memory for destination pixelmap.  */
    destination->data = (GX_UBYTE *)_gx_system_memory_allocator(destination->data_size);
    destination->aux_data = GX_NULL;

    if (destination->data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    put = (USHORT *)destination->data;

    /* Loop through destination's pixel and fill each pixel with
       the interpolation of 4 nearest neighboring pixels.*/
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            /* Find the original source pixel that the destination pixel conrespond to. */
            xx = (xradio * x) >> 8;
            yy = (yradio * y) >> 8;

            /* The coordinates of the original source pixel are truncate value,
               calucate their distance between the mathematical coordinates. */
            xdiff = (xradio * x) & 0xff;
            ydiff = (yradio * y) & 0xff;

            get = (USHORT *)src->data;
            get += yy * src->width;
            get += xx;

            /* Calculate 4 nearest neighboring pixels around the mathematical point of original pixel. */
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
                    /* Handle pixels in right bottom corner.  */
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


            /* Calulate pixel values by interpolating 4 neighboring pixels. */
            red = (REDVAL(neighbor_pixels[0][0]) * (256 - (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff) + \
                   REDVAL(neighbor_pixels[0][1]) * (GX_COLOR)xdiff * (256 - (GX_COLOR)ydiff) +         \
                   REDVAL(neighbor_pixels[1][0]) * (GX_COLOR)ydiff * (256 - (GX_COLOR)xdiff) +         \
                   REDVAL(neighbor_pixels[1][1]) * (GX_COLOR)xdiff * (GX_COLOR)ydiff) >> 16;

            green = (GREENVAL(neighbor_pixels[0][0]) * (256 - (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff) + \
                     GREENVAL(neighbor_pixels[0][1]) * (GX_COLOR)xdiff * (256 - (GX_COLOR)ydiff) +         \
                     GREENVAL(neighbor_pixels[1][0]) * (GX_COLOR)ydiff * (256 - (GX_COLOR)xdiff) +         \
                     GREENVAL(neighbor_pixels[1][1]) * (GX_COLOR)xdiff * (GX_COLOR)ydiff) >> 16;

            blue = (BLUEVAL(neighbor_pixels[0][0]) * (256 - (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff) + \
                    BLUEVAL(neighbor_pixels[0][1]) * (GX_COLOR)xdiff * (256 - (GX_COLOR)ydiff) +         \
                    BLUEVAL(neighbor_pixels[1][0]) * (GX_COLOR)ydiff * (256 - (GX_COLOR)xdiff) +         \
                    BLUEVAL(neighbor_pixels[1][1]) * (GX_COLOR)xdiff * (GX_COLOR)ydiff) >> 16;

            red = BYTE_RANGE(red);
            green = BYTE_RANGE(green);
            blue = BYTE_RANGE(blue);

            *put++ = ASSEMBLECOLOR(red, green, blue);
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_565rgb_pixelmap_alpha_resize                            */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that resize an uncompressed pixelmap       */
/*      with alpha.                                                       */
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
/*    _gx_system_memory_allocator                                         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_16bpp_pixelmap_alpha_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height)
{
/* The pixelmap resize function is implemented from bilinear interpolation
   image scaling algorithm.  */

USHORT   *get;
USHORT   *put;
GX_UBYTE *getalpha;
GX_UBYTE *putalpha;
INT       xdiff;
INT       ydiff;
INT       xradio;
INT       yradio;
INT       x;
INT       y;
INT       xx;
INT       yy;
USHORT    neighbor_pixels[2][2];
GX_COLOR  alpha[4];
GX_COLOR  red;
GX_COLOR  green;
GX_COLOR  blue;

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
    destination->aux_data_size = (UINT)(height * width) * sizeof(GX_UBYTE);

    /* Allocate memory to load pixelmap data. */
    destination->data = (GX_UBYTE *)_gx_system_memory_allocator(destination->data_size);

    if (destination->data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    destination->aux_data = (GX_UBYTE *)_gx_system_memory_allocator(destination->aux_data_size);

    if (destination->aux_data == GX_NULL)
    {
        _gx_system_memory_free((void *)destination->data);
        return GX_SYSTEM_MEMORY_ERROR;
    }

    put = (USHORT *)destination->data;
    putalpha = (GX_UBYTE *)destination->aux_data;

    /* Loop through destination's pixel and fill each pixel with
       the interpolation of 4 nearest neighboring pixels.*/
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            /* Find the original source pixel that the destination pixel conrespond to. */
            xx = (xradio * x) >> 8;
            yy = (yradio * y) >> 8;

            /* The coordinates of the original source pixel are truncate value,
               calucate their distance between the mathematical coordinates. */
            xdiff = (xradio * x) & 0xff;
            ydiff = (yradio * y) & 0xff;

            get = (USHORT *)src->data;
            get += yy * src->width;
            get += xx;

            getalpha = (GX_UBYTE *)src->aux_data;
            getalpha += yy * src->width;
            getalpha += xx;


            /* Calculate 4 nearest neighboring pixels around the mathematical point of original pixel. */
            neighbor_pixels[0][0] = *get;
            alpha[0] = *getalpha;

            if ((xx < src->width - 1) && (yy < src->height - 1))
            {
                neighbor_pixels[0][1] = *(get + 1);
                neighbor_pixels[1][0] = *(get + src->width);
                neighbor_pixels[1][1] = *(get + src->width + 1);

                alpha[1] = *(getalpha + 1);
                alpha[2] = *(getalpha + src->width);
                alpha[3] = *(getalpha + src->width + 1);
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

                    alpha[1] = alpha[0];
                    alpha[2] = alpha[0];
                    alpha[3] = alpha[0];
                }
                else if (xx == src->width - 1)
                {
                    /* Handle pixels in right edge.  */
                    neighbor_pixels[0][1] = neighbor_pixels[0][0];
                    neighbor_pixels[1][0] = *(get + src->width);
                    neighbor_pixels[1][1] = neighbor_pixels[1][0];

                    alpha[1] = alpha[0];
                    alpha[2] = *(getalpha + src->width);
                    alpha[3] = alpha[2];
                }
                else
                {
                    /* Handle pixels in bottom edge.  */
                    neighbor_pixels[0][1] = *(get + 1);
                    neighbor_pixels[1][0] = neighbor_pixels[0][0];
                    neighbor_pixels[1][1] = neighbor_pixels[0][1];

                    alpha[1] = *(getalpha + 1);
                    alpha[2] = alpha[0];
                    alpha[3] = alpha[1];
                }
            }

            /* Calulate pixel values by interpolating 4 neighboring pixels. */
            red = (REDVAL(neighbor_pixels[0][0]) * (alpha[0]) * (256 - (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff) + \
                   REDVAL(neighbor_pixels[0][1]) * (alpha[1]) * (GX_COLOR)xdiff * (256 - (GX_COLOR)ydiff) +         \
                   REDVAL(neighbor_pixels[1][0]) * (alpha[2]) * (GX_COLOR)ydiff * (256 - (GX_COLOR)xdiff) +         \
                   REDVAL(neighbor_pixels[1][1]) * (alpha[3]) * (GX_COLOR)xdiff * (GX_COLOR)ydiff) >> 16;

            green = (GREENVAL(neighbor_pixels[0][0]) * (alpha[0]) * (256 - (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff) + \
                     GREENVAL(neighbor_pixels[0][1]) * (alpha[1]) * (GX_COLOR)xdiff * (256 - (GX_COLOR)ydiff) +         \
                     GREENVAL(neighbor_pixels[1][0]) * (alpha[2]) * (GX_COLOR)ydiff * (256 - (GX_COLOR)xdiff) +         \
                     GREENVAL(neighbor_pixels[1][1]) * (alpha[3]) * (GX_COLOR)xdiff * (GX_COLOR)ydiff) >> 16;

            blue = (BLUEVAL(neighbor_pixels[0][0]) * (alpha[0]) * (256 - (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff) + \
                    BLUEVAL(neighbor_pixels[0][1]) * (alpha[1]) * (GX_COLOR)xdiff * (256 - (GX_COLOR)ydiff) +         \
                    BLUEVAL(neighbor_pixels[1][0]) * (alpha[2]) * (GX_COLOR)ydiff * (256 - (GX_COLOR)xdiff) +         \
                    BLUEVAL(neighbor_pixels[1][1]) * (alpha[3]) * (GX_COLOR)xdiff * (GX_COLOR)ydiff) >> 16;

            alpha[0] = ((alpha[0]) * (256 - (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff) + \
                        (alpha[1]) * (GX_COLOR)xdiff * (256 - (GX_COLOR)ydiff) +         \
                        (alpha[2]) * (GX_COLOR)ydiff * (256 - (GX_COLOR)xdiff) +         \
                        (alpha[3]) * (GX_COLOR)xdiff * (GX_COLOR)ydiff) >> 16;

            if (alpha[0])
            {
                red /= alpha[0];
                green /= alpha[0];
                blue /= alpha[0];
            }

            alpha[0] = BYTE_RANGE(alpha[0]);
            red = BYTE_RANGE(red);
            green = BYTE_RANGE(green);
            blue = BYTE_RANGE(blue);

            *put++ = ASSEMBLECOLOR(red, green, blue);
            *putalpha++ = (GX_UBYTE)alpha[0];
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_16bpp_pixelmap_resize                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    565rgb pixelmap resize function that handles uncompress, with or    */
/*    alpha channel.                                                      */
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
/*     _gx_utility_565rgb_pixelmap_alpha_resize                           */
/*     _gx_utility_565rgb_pixelmap_raw_resize                             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_16bpp_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height)
{
UINT status;

    if (src->flags & GX_PIXELMAP_ALPHA)
    {
        /* alpha, no compression */

        status = _gx_utility_16bpp_pixelmap_alpha_resize(src, destination, width, height);
    }
    else
    {
        /* no compression or alpha */

        status = _gx_utility_16bpp_pixelmap_raw_resize(src, destination, width, height);
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_565rgb_pixelmap_raw_rotate                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that rotate an uncompressed pixelmap       */
/*    without alpha.                                                      */
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
/*    _gx_system_memory_allocator           Memory Allocation routine     */
/*    _gx_utility_math_cos                  Compute the cosine value      */
/*    _gx_utility_math_sin                  Compute the sine value        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_565rgb_pixelmap_raw_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
USHORT   *put;
USHORT   *get;
GX_UBYTE *putalpha;
INT       srcxres;
INT       srcyres;
INT       cosv;
INT       sinv;
USHORT    red, green, blue;
INT       idxminx, idxmaxx, idxmaxy;
INT      *mx;
INT      *my;
INT       xres;
INT       yres;
INT       width, height;
INT       x, y;
INT       xx, yy;
USHORT    a, b, c, d;
INT       alpha;
INT       xdiff, ydiff;

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

        srcxres = (INT)*rot_cx;
        srcyres = (INT)*rot_cy;

        x = GX_FIXED_VAL_TO_INT(x) + xres;
        y = GX_FIXED_VAL_TO_INT(y) + yres;

        *rot_cx = x;
        *rot_cy = y;

        xres = *rot_cx;
        yres = *rot_cy;
    }

    destination->height = (GX_VALUE)height;
    destination->width  = (GX_VALUE)width;
    destination->flags |= GX_PIXELMAP_ALPHA;

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->data_size = (UINT)(height * width) * sizeof(USHORT);
    destination->data = (GX_UBYTE *)_gx_system_memory_allocator(destination->data_size);

    if (destination->data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->aux_data_size = (UINT)(height * width) * sizeof(GX_UBYTE);
    destination->aux_data = (GX_UBYTE *)_gx_system_memory_allocator(destination->aux_data_size);

    if (destination->aux_data == GX_NULL)
    {
        _gx_system_memory_free((void *)destination->aux_data);

        return GX_SYSTEM_MEMORY_ERROR;
    }

    put = (USHORT *)destination->data;
    putalpha = (GX_UBYTE *)destination->aux_data;

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

                    alpha = 0xff;
                }
                else
                {
                    get = (USHORT *)src->data;

                    a = 0;
                    b = 0;
                    c = 0;
                    d = 0;
                    alpha = 0;

                    if (xx == -1)
                    {
                        /* handle left edge.  */
                        if (yy >= 0)
                        {
                            b = *(get + yy * src->width);
                            alpha += xdiff * (256 - ydiff);
                        }

                        if (yy < src->height - 1)
                        {
                            d = *(get + (yy + 1) * src->width);
                            alpha += xdiff * ydiff;
                        }
                    }
                    else if (yy == -1)
                    {
                        /* handle top edge.  */
                        c = *(get + xx);
                        alpha += ydiff * (256 - xdiff);

                        if (xx < src->width - 1)
                        {
                            d = *(get + xx + 1);
                            alpha += xdiff * ydiff;
                        }
                    }
                    else if (xx == src->width - 1)
                    {
                        /* handle right edget. */
                        a = *(get + yy * src->width + xx);
                        alpha += (256 - xdiff) * (256 - ydiff);

                        if (yy < src->height - 1)
                        {
                            c = *(get + (yy + 1) * src->width + xx);
                            alpha += ydiff * (256 - xdiff);
                        }
                    }
                    else
                    {
                        /* handle bottom edge. */
                        a = *(get + yy * src->width + xx);
                        alpha += (256 - xdiff) * (256 - ydiff);

                        b = *(get + yy * src->width + xx + 1);
                        alpha += xdiff * (256 - ydiff);
                    }

                    alpha >>= 8;
                }

                red = (USHORT)((REDVAL(a) * (256 - xdiff) * (256 - ydiff) + \
                                REDVAL(b) * xdiff * (256 - ydiff) +         \
                                REDVAL(c) * ydiff * (256 - xdiff) +         \
                                REDVAL(d) * xdiff * ydiff) >> 16);

                green = (USHORT)((GREENVAL(a) * (256 - xdiff) * (256 - ydiff) + \
                                  GREENVAL(b) * xdiff * (256 - ydiff) +         \
                                  GREENVAL(c) * ydiff * (256 - xdiff) +         \
                                  GREENVAL(d) * xdiff * ydiff) >> 16);

                blue = (USHORT)((BLUEVAL(a) * (256 - xdiff) * (256 - ydiff) + \
                                 BLUEVAL(b) * xdiff * (256 - ydiff) +         \
                                 BLUEVAL(c) * ydiff * (256 - xdiff) +         \
                                 BLUEVAL(d) * xdiff * ydiff) >> 16);

                if (alpha && (alpha < 0xff))
                {
                    red   = (USHORT)((red << 8) / alpha);
                    green = (USHORT)((green << 8) / alpha);
                    blue  = (USHORT)((blue << 8) / alpha);
                }

                red = red > 31 ? 31 : red;
                green = green > 63 ? 63 : green;
                blue = blue > 31 ? 31 : blue;
                alpha = alpha > 255 ? 255 : alpha;

                *put++ = ASSEMBLECOLOR(red, green, blue);
                *putalpha++ = (GX_UBYTE)alpha;
            }
            else
            {
                put++;
                *putalpha++ = 0;
            }
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_565rgb_pixelmap_alpha_rotate                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that rotate an uncompressed pixelmap       */
/*      with alpha.                                                       */
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
/*    _gx_system_memory_allocator           Memory Allocation routine     */
/*    _gx_utility_math_cos                  Compute the cosine value      */
/*    _gx_utility_math_sin                  Compute the sine value        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_565rgb_pixelmap_alpha_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
USHORT   *put;
USHORT   *get;
GX_UBYTE *putalpha;
GX_UBYTE *getalpha;
INT       srcxres;
INT       srcyres;
INT       cosv;
INT       sinv;
USHORT    red, green, blue;
INT       idxminx, idxmaxx, idxmaxy;
INT      *mx;
INT      *my;
INT       xres;
INT       yres;
INT       width, height;
INT       x, y;
INT       xx, yy;
USHORT    a, b, c, d;
USHORT    alpha[4];
INT       xdiff, ydiff;

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

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->aux_data_size = (UINT)(height * width) * sizeof(GX_UBYTE);
    destination->aux_data = (GX_UBYTE *)_gx_system_memory_allocator(destination->aux_data_size);

    if (destination->aux_data == GX_NULL)
    {
        _gx_system_memory_free((void *)destination->data);

        return GX_SYSTEM_MEMORY_ERROR;
    }

    put = (USHORT *)destination->data;
    putalpha = (GX_UBYTE *)destination->aux_data;

    /* Loop through the destination's pixels.  */
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            xx = (x - xres) * cosv + (y - yres) * sinv;
            yy = (y - yres) * cosv - (x - xres) * sinv;

            xdiff = GX_FIXED_VAL_TO_INT(xx << 8) & 0xff;
            ydiff = GX_FIXED_VAL_TO_INT(yy << 8) & 0xff;

            xx = GX_FIXED_VAL_TO_INT(xx);
            yy = GX_FIXED_VAL_TO_INT(yy);

            xx += srcxres;
            yy += srcyres;

            if ((xx >= -1) && (xx < src->width) &&
                (yy >= -1) && (yy < src->height))
            {

                if ((xx >= 0) && (xx < src->width - 1) &&
                    (yy >= 0) && (yy < src->height - 1))
                {
                    get = (USHORT *)src->data;
                    get += yy * src->width;
                    get += xx;

                    getalpha = (GX_UBYTE *)src->aux_data;
                    getalpha += yy * src->width;
                    getalpha += xx;

                    a = *get;
                    alpha[0] = *getalpha;

                    b = *(get + 1);
                    alpha[1] = *(getalpha + 1);

                    c = *(get + src->width);
                    alpha[2] = *(getalpha + src->width);

                    d = *(get + src->width + 1);
                    alpha[3] = *(getalpha + src->width + 1);
                }
                else
                {
                    get = (USHORT *)src->data;
                    getalpha = (GX_UBYTE *)src->aux_data;

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
                            alpha[1] = *(getalpha + yy * src->width);
                        }

                        if (yy < src->height - 1)
                        {
                            d = *(get + (yy + 1) * src->width);
                            alpha[3] = *(getalpha + (yy + 1) * src->width);
                        }
                    }
                    else if (yy == -1)
                    {
                        /* handle top edge.  */
                        c = *(get + xx);
                        alpha[2] = *(getalpha + xx);

                        if (xx < src->width - 1)
                        {
                            d = *(get + xx + 1);
                            alpha[3] = *(getalpha + xx + 1);
                        }
                    }
                    else if (xx == src->width - 1)
                    {
                        /* handle right edget. */
                        a = *(get + yy * src->width + xx);
                        alpha[0] = *(getalpha + yy * src->width + xx);

                        if (yy < src->height - 1)
                        {
                            c = *(get + (yy + 1) * src->width + xx);
                            alpha[2] = *(getalpha + (yy + 1) * src->width + xx);
                        }
                    }
                    else
                    {
                        /* handle bottom edge. */
                        a = *(get + yy * src->width + xx);
                        alpha[0] = *(getalpha + yy * src->width + xx);

                        b = *(get + yy * src->width + xx + 1);
                        alpha[1] = *(getalpha + yy * src->width + xx + 1);
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

                red = (USHORT)((REDVAL(a) * alpha[0] * (256 - xdiff) * (256 - ydiff) + \
                                REDVAL(b) * alpha[1] * xdiff * (256 - ydiff) +         \
                                REDVAL(c) * alpha[2] * ydiff * (256 - xdiff) +         \
                                REDVAL(d) * alpha[3] * xdiff * ydiff) >> 16);

                green = (USHORT)((GREENVAL(a) * alpha[0] * (256 - xdiff) * (256 - ydiff) + \
                                  GREENVAL(b) * alpha[1] * xdiff * (256 - ydiff) +         \
                                  GREENVAL(c) * alpha[2] * ydiff * (256 - xdiff) +         \
                                  GREENVAL(d) * alpha[3] * xdiff * ydiff) >> 16);

                blue = (USHORT)((BLUEVAL(a) * alpha[0] * (256 - xdiff) * (256 - ydiff) + \
                                 BLUEVAL(b) * alpha[1] * xdiff * (256 - ydiff) +         \
                                 BLUEVAL(c) * alpha[2] * ydiff * (256 - xdiff) +         \
                                 BLUEVAL(d) * alpha[3] * xdiff * ydiff) >> 16);

                alpha[0] = (USHORT)((alpha[0] * (256 - xdiff) * (256 - ydiff) + \
                                     alpha[1] * xdiff * (256 - ydiff) +         \
                                     alpha[2] * ydiff * (256 - xdiff) +         \
                                     alpha[3] * xdiff * ydiff) >> 16);

                if (alpha[0])
                {
                    red /= alpha[0];
                    green /=  alpha[0];
                    blue /= alpha[0];
                }

                red = red > 31 ? 31 : red;
                green = green > 63 ? 63 : green;
                blue = blue > 31 ? 31 : blue;

                *put++ = ASSEMBLECOLOR(red, green, blue);
                *putalpha++ = (GX_UBYTE)alpha[0];
            }
            else
            {
                put++;
                *putalpha++ = 0;
            }
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_565rgb_pixelmap_simple_raw_rotate                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles 90, 180 and 270 degree        */
/*    rotation of an uncompressed pixelmap wihout alpha.                  */
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
/*    _gx_system_memory_allocator           Memory Allocation routine     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_565rgb_pixelmap_simple_raw_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
USHORT *put;
USHORT *get;
INT     width, height;
INT     x, y;

    width = 0;
    height = 0;

    destination->aux_data = GX_NULL;

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

            *rot_cx = (width - 1 - x);
            *rot_cy = (height - 1 - y);
        }
    }
    else
    {
        /* angle = 270. */
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
/*    _gx_utility_565rgb_pixelmap_simple_alpha_rotate                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles 90, 180 and 270 degree        */
/*    rotation of an uncompressed pixelmap wih alpha.                     */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_565rgb_pixelmap_simple_alpha_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
USHORT   *put;
GX_UBYTE *putalpha;
USHORT   *get;
GX_UBYTE *getalpha;
INT       width, height;
INT       x, y;

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

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    destination->aux_data_size = (UINT)(width * height) * sizeof(GX_UBYTE);
    destination->aux_data = (GX_UBYTE *)_gx_system_memory_allocator(destination->aux_data_size);

    if (destination->aux_data == GX_NULL)
    {
        _gx_system_memory_free((void *)destination->data);
        return GX_SYSTEM_MEMORY_ERROR;
    }

    if (angle == 90)
    {
        put = (USHORT *)destination->data;
        putalpha = (GX_UBYTE *)destination->aux_data;

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                get = (USHORT *)src->data;
                get += (width - 1 - x) * height;
                get += y;

                getalpha = (GX_UBYTE *)src->aux_data;
                getalpha += (width - 1 - x) * height;
                getalpha += y;

                *put++ = *get;
                *putalpha++ = *getalpha;
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
        putalpha = (GX_UBYTE *)destination->aux_data;

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                get = (USHORT *)src->data;
                get += (height - 1 - y) * width;
                get += width - 1 - x;

                getalpha = (GX_UBYTE *)src->aux_data;
                getalpha += (height - 1 - y) * width;
                getalpha += width - 1 - x;

                *put++ = *get;
                *putalpha++ = *getalpha;
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
        /* angle = 270. */
        put = (USHORT *)destination->data;
        putalpha = (GX_UBYTE *)destination->aux_data;

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                get = (USHORT *)src->data;
                get += x * height;
                get += height - 1 - y;

                getalpha = (GX_UBYTE *)src->aux_data;
                getalpha += x * height;
                getalpha += height - 1 - y;

                *put++ = *get;
                *putalpha++ = *getalpha;
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


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_565rgb_pixelmap_rotate                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function rotates 565rgb format uncompressed pixelmap with or   */
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
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*     _gx_utility_565rgb_pixelmap_raw_rotate                             */
/*                                          Rotate 565rgb format pixelmap */
/*                                            without alpha channel       */
/*     _gx_utility_565rgb_pixelmap_alpha_rotate                           */
/*                                          Rotate 565rgb format pixelmap */
/*                                            with alpha channel          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_565rgb_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
UINT status;

    if (src->flags & GX_PIXELMAP_ALPHA)
    {
        /* alpha, no compression */
        status = _gx_utility_565rgb_pixelmap_alpha_rotate(src, angle, destination, rot_cx, rot_cy);
    }
    else
    {
        /* no compression or alpha */
        status = _gx_utility_565rgb_pixelmap_raw_rotate(src, angle, destination, rot_cx, rot_cy);
    }

    return status;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_565rgb_pixelmap_simple_rotate                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function rotates 565rgb format uncompressed pixelmap with      */
/*    simple rotation angle 90, 180 or 270 degree.                        */
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
/*     _gx_utility_565rgb_pixelmap_simple_raw_rotate                      */
/*                                          Rotate 565rgb format pixelmap */
/*                                            without alpha channel       */
/*     _gx_utility_565rgb_pixelmap_simple_alpha_rotate                    */
/*                                          Rotate 565rgb format pixelmap */
/*                                            with alpha channel          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_565rgb_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
UINT status;

    if (src->flags & GX_PIXELMAP_ALPHA)
    {
        /* alpha, no compression */
        status = _gx_utility_565rgb_pixelmap_simple_alpha_rotate(src, angle, destination, rot_cx, rot_cy);
    }
    else
    {
        /* no compression or alpha */
        status = _gx_utility_565rgb_pixelmap_simple_raw_rotate(src, angle, destination, rot_cx, rot_cy);
    }

    return status;
}

