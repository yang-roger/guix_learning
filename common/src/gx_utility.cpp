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
/**   Utility (Utility)                                                   */
/**                                                                       */
/**************************************************************************/

#include "gx_utility.h"

#include "gx_system.h"
#include "gx_display.h"
#include "gx_canvas.h"
#include "gx_gradient.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_ltoa                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service converts a long integer value into an ASCII string.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    value                                 Long integer value to convert */
/*    return_buffer                         Destination buffer for ASCII  */
/*                                            string                      */
/*    return_buffer_size                    Size of destination buffer    */
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
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_ltoa(LONG value, GX_CHAR *return_buffer, UINT return_buffer_size)
{
LONG     temp_val;
GX_CHAR  temp_buf[20];
GX_CHAR *mirror;
GX_CHAR *putchar;
INT      quotient;
INT      sign = 0;

    temp_val = value;

    if (value < 0)
    {
        sign = -1;
        temp_val = -value;
    }

    /* build the string in our temp_buf. The string will
       be in reverse (mirrored) order, we will fix that
       when we copy into the output buffer
     */

    mirror = temp_buf;

    do
    {
        quotient = temp_val % 10;
        temp_val = temp_val / 10;
        quotient = quotient | 0x30;
        *mirror++ = (GX_CHAR)quotient;
    } while (temp_val != 0);

    /* put a negative sign at the end if needed */

    if (sign != 0)
    {
        *mirror++ = '-';
    }

    /* now copy the result back into the caller's buffer,
       being careful not to exceed the buffer size
     */

    mirror--;
    putchar = return_buffer;

    while (mirror >= temp_buf && return_buffer_size > 1)
    {
        *putchar++ = *mirror--;
        return_buffer_size--;
    }

    /* termniate the return string */
    *putchar = 0;
    return GX_SUCCESS;
}

/**************************************************************************/

UINT _gx_utility_value_or_percent_string_get(GX_BOOL is_percent, INT value, INT max_value, GX_CHAR *return_buffer, UINT return_buffer_size)
{
    if (return_buffer_size < 2 || value == 0)
    {
        *return_buffer = '\0';

        return 0;
    }

    UINT length = 0;

    if (is_percent)
    {
        INT percent = 100 * value / max_value;

        _gx_utility_ltoa(percent, return_buffer, return_buffer_size - 1);
        _gx_utility_string_length_check(return_buffer, &length, return_buffer_size - 2);
        return_buffer[length] = '%';
        ++length;
        return_buffer[length] = '\0';
    }
    else
    {
        _gx_utility_ltoa(value, return_buffer, return_buffer_size);
        _gx_utility_string_length_check(return_buffer, &length, return_buffer_size - 1);
    }

    return length;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_circle_point_get                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function gets the point on circle with specified angle         */
/*    and radius.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    xcenter                               x-coord of center of circle   */
/*    ycenter                               y-coord of center of circle   */
/*    r                                     Radius of circle              */
/*    angle                                 The angle where the point is  */
/*    point                                 Return value of the point at  */
/*                                            the angle                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_math_cos                  Compute cosine                */
/*    _gx_utility_math_sin                  Compute sine                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_circle_point_get(INT xcenter, INT ycenter, UINT r, INT angle, GX_POINT *point)
{

INT     x;
INT     y;
INT     d;
INT     x_sign;
INT     y_sign;
GX_BOOL swap = GX_FALSE;

    angle %= 360;

    if (angle < 0)
    {
        angle += 360;
    }

    if (angle == 0)
    {
        point->x = (GX_VALUE)(xcenter + (INT)r);
        point->y = (GX_VALUE)ycenter;
    }
    else if (angle == 90)
    {
        point->x = (GX_VALUE)xcenter;
        point->y = (GX_VALUE)(ycenter - (INT)r);
    }
    else if (angle == 180)
    {
        point->x = (GX_VALUE)(xcenter - (INT)r);
        point->y = (GX_VALUE)ycenter;
    }
    else if (angle == 270)
    {
        point->x = (GX_VALUE)xcenter;
        point->y = (GX_VALUE)(ycenter + (INT)r);
    }
    else
    {
        point->x = (GX_VALUE)(GX_FIXED_VAL_TO_INT(r * _gx_utility_math_cos(GX_FIXED_VAL_MAKE(angle))));
        point->y = (GX_VALUE)(GX_FIXED_VAL_TO_INT(r * _gx_utility_math_sin(GX_FIXED_VAL_MAKE(angle))));

        if (angle <= 90)
        {
            x_sign = 1;
            y_sign = 1;

            if (angle < 45)
            {
                swap = GX_TRUE;
            }
        }
        else if (angle <= 180)
        {
            x_sign = -1;
            y_sign = 1;

            if (angle > 135)
            {
                swap = GX_TRUE;
            }
        }
        else if (angle <= 270)
        {
            x_sign = -1;
            y_sign = -1;

            if (angle < 225)
            {
                swap = GX_TRUE;
            }
        }
        else
        {
            x_sign = 1;
            y_sign = -1;

            if (angle > 315)
            {
                swap = GX_TRUE;
            }
        }

        x = 0;
        y = (INT)r;
        d = (INT)(5 - 4 * r);

        point->x = (GX_VALUE)(point->x * x_sign);
        point->y = (GX_VALUE)(point->y * y_sign);

        if (swap)
        {
            GX_SWAP_VALS(point->x, point->y);
        }

        while (x <= y)
        {
            if ((x > point->x) || (y < point->y))
            {
                break;
            }

            if (d < 0)
            {
                d += 8 * x + 12;
            }
            else
            {
                d += 8 * (x - y) + 20;
                y--;
            }
            x++;
        }

        if (swap)
        {
            GX_SWAP_VALS(x, y);
        }

        x *= x_sign;
        y *= y_sign;

        point->x = (GX_VALUE)(xcenter + x);
        point->y = (GX_VALUE)(ycenter - y);
    }

    return GX_SUCCESS;
}

/**************************************************************************/

UINT _gx_utility_radial_angle_calculate(GX_POINT point, GX_POINT center, INT *return_angle)
{
    INT x_dist = (INT)(point.x - center.x);
    INT y_dist = (INT)(point.y - center.y);

    INT dist = (INT)_gx_utility_math_sqrt(x_dist * x_dist + y_dist * y_dist);

    if (dist == 0)
    {
        *return_angle = 0;

        return GX_FAILURE;
    }

    INT angle_value;

    if (point.y <= center.y) // [0, 180]
    {
        angle_value = _gx_utility_math_acos(GX_FIXED_VAL_MAKE(x_dist) / dist);
    }
    else // [180, 360]
    {
        angle_value = _gx_utility_math_asin(GX_FIXED_VAL_MAKE(y_dist) / dist);

        if (point.x < center.x)
        {
            angle_value = 180 + angle_value;
        }
        else
        {
            angle_value = 360 - angle_value;
        }
    }

    *return_angle = angle_value;

    return GX_SUCCESS;
}

GX_VALUE _gx_utility_value_clamp(GX_VALUE value, GX_VALUE min_value, GX_VALUE max_value)
{
    if (value < min_value)
    {
        return min_value;
    }
    else if (value > max_value)
    {
        return max_value;
    }
    else
    {
        return value;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_alphamap_create                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates an 8bpp alpha-map pixelmap.                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    width                                 width of desired alphamap     */
/*    height                                height of desired alphamap    */
/*    map                                   pointer to pixlmap structure  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           Dynamically allocate memory   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_string_to_alphamap                                      */
/*    application software                                                */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_alphamap_create(INT width, INT height, GX_PIXELMAP *map)
{
/* try to allocate memory for alphamap */
GX_UBYTE *memptr;
ULONG     memsize = (ULONG)(width * height);

    if (width > GX_MAX_PIXELMAP_RESOLUTION)
    {
        return GX_INVALID_WIDTH;
    }

    if (height > GX_MAX_PIXELMAP_RESOLUTION)
    {
        return GX_INVALID_HEIGHT;
    }

    if (!memsize || _gx_system_memory_allocator == GX_NULL)
    {
        return GX_FAILURE;
    }

    memptr = (GX_UBYTE *)(_gx_system_memory_allocator(memsize));

    if (memptr == GX_NULL)
    {
        map->data = GX_NULL;
        return GX_FAILURE;
    }

    map->aux_data = GX_NULL;
    map->aux_data_size = 0;
    map->data = memptr;
    map->data_size = memsize;
    map->flags = GX_PIXELMAP_TRANSPARENT;
    map->transparent_color = 0;
    map->format = GX_COLOR_FORMAT_8BIT_ALPHAMAP;
    map->height = (GX_VALUE)height;
    map->width = (GX_VALUE)width;
    map->version_major = map->version_minor = 0;
    memset(memptr, 0, (size_t)memsize);
    return GX_SUCCESS;
}

/**************************************************************************/

void _gx_utility_pixelmap_free(GX_PIXELMAP *map)
{
    if (map && _gx_system_memory_free)
    {
        if (map->aux_data)
        {
            _gx_system_memory_free((void*)map->aux_data);
            map->aux_data = GX_NULL;
        }

        _gx_system_memory_free((void*)map->data);
        map->data = GX_NULL;
    }
}

/**************************************************************************/

#define BI_RGB       0L
#define BI_BITFIELDS 3L

#define DATA_OFFSET  54

typedef struct bitmap_file_header_struct
{
    USHORT filetype;
    UINT   filesize;
    UINT   reserved;
    UINT   offset;
} bitmap_file_header;

typedef struct bmp_info_struct
{
    UINT   bi_Size;
    INT    bi_Width;
    INT    bi_Height;
    USHORT bi_Planes;
    USHORT bi_BitCount;
    UINT   bi_Compression;
    UINT   bi_SizeImage;
    UINT   bi_XPelsPerMeter;
    UINT   bi_YPelsPerMeter;
    UINT   bi_ClrUsed;
    UINT   bi_ClrImportant;
} bmp_info;

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_write_bitmap_header                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function create bitmap header and write it to bmp file.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    rectangle                             Rectangle specification       */
/*    write_data                            Write data callback function  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_canvas_to_bmp                                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_write_bitmap_header(GX_CANVAS *canvas, GX_RECTANGLE *rect, UINT (*write_data)(GX_UBYTE *byte_data, UINT data_count))
{
bitmap_file_header bmp_fheader;
bmp_info           b_info;
GX_COLOR           color;
GX_COLOR          *palette;
INT                count;
INT                r;
INT                g;
INT                b;
INT                red;
INT                green;
INT                blue;

    memset(&bmp_fheader, 0, sizeof(bitmap_file_header));
    memset(&b_info, 0, sizeof(bmp_info));
    bmp_fheader.filetype = 0x4d42; /* "MB" */
    bmp_fheader.reserved = 0;

    /* calculate data offset */
    bmp_fheader.offset = DATA_OFFSET;

    /* set common datas for bmp info. */
    b_info.bi_Width = rect->right - rect->left + 1;
    b_info.bi_Height = rect->bottom - rect->top + 1;
    b_info.bi_Size = sizeof(bmp_info);
    b_info.bi_Planes = 1;
    b_info.bi_Compression = BI_RGB;
    b_info.bi_XPelsPerMeter = 0;
    b_info.bi_YPelsPerMeter = 0;
    b_info.bi_ClrUsed = 0;
    b_info.bi_ClrImportant = 0;

    switch (canvas->display->color_format)
    {
    case GX_COLOR_FORMAT_32ARGB:
    case GX_COLOR_FORMAT_24XRGB:
        b_info.bi_BitCount = 32;
        b_info.bi_SizeImage = (UINT)((b_info.bi_Width << 2) * b_info.bi_Height);
        break;

    case GX_COLOR_FORMAT_1555XRGB:
    case GX_COLOR_FORMAT_4444ARGB:
    case GX_COLOR_FORMAT_565RGB:
        b_info.bi_BitCount = 16;
        b_info.bi_SizeImage = (UINT)((((UINT)(b_info.bi_Width + 1) & 0xfffffffe) << 1) * (UINT)b_info.bi_Height);
        b_info.bi_ClrUsed = 3;
        b_info.bi_ClrImportant = 3;
        b_info.bi_Compression = BI_BITFIELDS;
        /* Three color entries in palette */
        bmp_fheader.offset += 3 * sizeof(GX_COLOR);
        break;

    case GX_COLOR_FORMAT_8BIT_PALETTE:
        if ((canvas->display->palette == GX_NULL) ||
            (canvas->display->palette_size == 0))
        {
            return GX_FAILURE;
        }
        b_info.bi_BitCount = 8;
        b_info.bi_SizeImage = (UINT)(((UINT)(b_info.bi_Width + 3) & 0xfffffffc) * (UINT)b_info.bi_Height);
        b_info.bi_ClrUsed = canvas->display->palette_size;
        b_info.bi_ClrImportant = canvas->display->palette_size;
        bmp_fheader.offset += canvas->display->palette_size * sizeof(GX_COLOR);
        break;

    case GX_COLOR_FORMAT_8BIT_PACKED_PIXEL:
        b_info.bi_BitCount = 8;
        b_info.bi_SizeImage = (UINT)(((UINT)(b_info.bi_Width + 3) & 0xfffffffc) * (UINT)b_info.bi_Height);
        /* Palette for 332rgb is default to 256. */
        b_info.bi_ClrImportant = 256;
        b_info.bi_ClrUsed = 256;
        bmp_fheader.offset += 256 * sizeof(GX_COLOR);
        break;

    case GX_COLOR_FORMAT_4BIT_GRAY:
        b_info.bi_BitCount = 4;
        b_info.bi_SizeImage = (UINT)((((UINT)(b_info.bi_Width + 7) & 0xfffffff8) >> 1) * (UINT)b_info.bi_Height);
        b_info.bi_ClrUsed = 16;
        b_info.bi_ClrImportant = 16;
        bmp_fheader.offset += 16 * sizeof(GX_COLOR);
        break;

    case GX_COLOR_FORMAT_MONOCHROME:
        b_info.bi_BitCount = 1;
        b_info.bi_SizeImage = (UINT)((((UINT)(b_info.bi_Width + 63) & 0xffffffc0) >> 3) * (UINT)b_info.bi_Height);
        b_info.bi_ClrUsed = 2;
        b_info.bi_ClrImportant = 2;
        bmp_fheader.offset += 2 * sizeof(GX_COLOR);
        break;

    default:
        break;
    }

    /* total file size */
    bmp_fheader.filesize = bmp_fheader.offset + b_info.bi_SizeImage;

    /* write bmp file header */
    write_data((GX_UBYTE *)&bmp_fheader.filetype, sizeof(USHORT));
    write_data((GX_UBYTE *)&bmp_fheader.filesize, sizeof(UINT));
    write_data((GX_UBYTE *)&bmp_fheader.reserved, sizeof(UINT));
    write_data((GX_UBYTE *)&bmp_fheader.offset, sizeof(UINT));

    /* height value is negtive means we can write image data from top to bottom. */
    b_info.bi_Height = -b_info.bi_Height;
    /* write bmp info */
    write_data((GX_UBYTE *)&b_info, sizeof(bmp_info));

    /* I don't want to add a matrix with 256 to store the palette.
       so let's write the palette by colors.*/
    switch (canvas->display->color_format)
    {
    case GX_COLOR_FORMAT_565RGB:
        color = 0x0000f800;
        write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
        color = 0x000007e0;
        write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
        color = 0x0000001f;
        write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
        break;

    case GX_COLOR_FORMAT_1555XRGB:
        color = 0x00007c00;
        write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
        color = 0x000003e0;
        write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
        color = 0x0000001f;
        write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        color = 0x00000f00;
        write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
        color = 0x000000f0;
        write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
        color = 0x0000000f;
        write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
        break;

    case GX_COLOR_FORMAT_8BIT_PALETTE:
        palette = canvas->display->palette;
        count = (INT)(canvas->display->palette_size);
        while (count)
        {
            write_data((GX_UBYTE *)palette, sizeof(GX_COLOR));
            palette++;
            count--;
        }
        break;

    case GX_COLOR_FORMAT_8BIT_PACKED_PIXEL:
        for (r = 0; r < 8; r++)
        {
            red = r << 21;
            if (red & 0x200000)
            {
                red |= 0x1f0000;
            }
            for (g = 0; g < 8; g++)
            {
                green = g << 13;
                if (green & 0x2000)
                {
                    green |= 0x1f00;
                }
                for (b = 0; b < 4; b++)
                {
                    blue = b << 6;
                    if (blue & 0x40)
                    {
                        blue |= 0x3f;
                    }
                    color = (GX_COLOR)(red | green | blue);
                    write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
                }
            }
        }
        break;

    case GX_COLOR_FORMAT_4BIT_GRAY:
        for (r = 0; r <= 0xff; r += 0x11)
        {
            color = (GX_COLOR)((GX_COLOR)((r << 16) | (r << 8) | r) | 0xff000000);
            write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
        }
        break;

    case GX_COLOR_FORMAT_MONOCHROME:
        color = 0xff000000;
        write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
        color = 0xffffffff;
        write_data((GX_UBYTE *)&color, sizeof(GX_COLOR));
        break;

    default:
        break;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_write_bitmap_data_32bpp                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function read canvas memory data and write it to bmp file.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    rectangle                             Rectangle specification       */
/*    write_data                            Write data callback function  */
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
/*    _gx_utility_canvas_to_bmp                                           */
/*                                                                        */
/**************************************************************************/
static void _gx_utility_write_bitmap_data_32bpp(GX_CANVAS *canvas, GX_RECTANGLE *rect, UINT (*write_data)(GX_UBYTE *byte_data, UINT data_count))
{
GX_COLOR *get;
GX_COLOR *get_row;
INT       x;
INT       y;

    get_row = (GX_COLOR *)(canvas->memory);
    get_row += canvas->x_resolution * rect->top;
    get_row += rect->left;

    for (y = rect->top; y <= rect->bottom; y++)
    {
        get = get_row;
        for (x = rect->left; x <= rect->right; x++)
        {
            write_data((GX_UBYTE *)get, sizeof(GX_COLOR));
            get++;
        }
        get_row += canvas->x_resolution;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_write_bitmap_data_16bpp                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function read canvas memory data and write it to bmp file.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    rectangle                             Rectangle specification       */
/*    write_data                            Write data callback function  */
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
/*    _gx_utility_canvas_to_bmp                                           */
/*                                                                        */
/**************************************************************************/
static void _gx_utility_write_bitmap_data_16bpp(GX_CANVAS *canvas, GX_RECTANGLE *rect, UINT (*write_data)(GX_UBYTE *byte_data, UINT data_count))
{
USHORT *get;
USHORT *get_row;
INT     x;
INT     y;
INT     width;

    get_row = (USHORT *)(canvas->memory);
    get_row += canvas->x_resolution * rect->top;
    get_row += rect->left;
    width = rect->right - rect->left + 1;

    for (y = rect->top; y <= rect->bottom; y++)
    {
        get = get_row;
        for (x = 0; x < width; x++)
        {
            write_data((GX_UBYTE *)get, sizeof(USHORT));
            get++;
        }
        if (width & 1)
        {
            /* Fix data to 4 bytes aligned. */
            write_data((GX_UBYTE *)get, sizeof(USHORT));
        }
        get_row += canvas->x_resolution;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_write_bitmap_data_8bpp                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function read canvas memory data and write it to bmp file.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    rectangle                             Rectangle specification       */
/*    write_data                            Write data callback function  */
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
/*    _gx_utility_canvas_to_bmp                                           */
/*                                                                        */
/**************************************************************************/
static void _gx_utility_write_bitmap_data_8bpp(GX_CANVAS *canvas, GX_RECTANGLE *rect, UINT (*write_data)(GX_UBYTE *byte_data, UINT data_count))
{
GX_UBYTE *get;
GX_UBYTE *get_row;
INT       x;
INT       y;
INT       width;
INT       fixed_count;

    get_row = (GX_UBYTE *)(canvas->memory);
    get_row += canvas->x_resolution * rect->top;
    get_row += rect->left;
    width = rect->right - rect->left + 1;
    fixed_count = width & 3;
    if (fixed_count)
    {
        fixed_count = 4 - fixed_count;
    }

    for (y = rect->top; y <= rect->bottom; y++)
    {
        get = get_row;
        for (x = 0; x < width; x++)
        {
            write_data((GX_UBYTE *)get, sizeof(GX_UBYTE));
            get++;
        }

        /* Fix data to 4 bytes aligned. */
        for (x = 0; x < fixed_count; x++)
        {
            write_data((GX_UBYTE *)get, sizeof(GX_UBYTE));
        }
        get_row += canvas->x_resolution;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_write_bitmap_data_4bpp                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function read canvas memory data and write it to bmp file.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    rectangle                             Rectangle specification       */
/*    write_data                            Write data callback function  */
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
/*    _gx_utility_canvas_to_bmp                                           */
/*                                                                        */
/**************************************************************************/
static void _gx_utility_write_bitmap_data_4bpp(GX_CANVAS *canvas, GX_RECTANGLE *rect, UINT (*write_data)(GX_UBYTE *byte_data, UINT data_count))
{
GX_UBYTE *get;
GX_UBYTE *get_row;
GX_UBYTE  color = 0;
INT       x;
INT       y;
INT       fixed_count;
INT       row_count;

    get_row = (GX_UBYTE *)(canvas->memory);
    get_row += ((canvas->x_resolution + 1) >> 1) * rect->top;
    get_row += rect->left >> 1;
    row_count = rect->right - rect->left + 1;
    row_count = (row_count + 1) >> 1;
    fixed_count = row_count & 3;
    if (fixed_count)
    {
        fixed_count = 4 - fixed_count;
    }

    if (rect->left & 1)
    {
        /* If start pixel is not at the first bit of byte. We need to make it aligned. */
        for (y = rect->top; y <= rect->bottom; y++)
        {
            get = get_row;
            for (x = 0; x < row_count; x++)
            {
                color = (GX_UBYTE)((*get & 0x0f) << 4);
                get++;
                color |= (*get & 0xf0) >> 4;

                write_data((GX_UBYTE *)&color, sizeof(GX_UBYTE));
            }

            /* Fix data to 4 bytes aligned. */
            for (x = 0; x < fixed_count; x++)
            {
                write_data((GX_UBYTE *)&color, sizeof(GX_UBYTE));
            }

            get_row += (canvas->x_resolution + 1) >> 1;
        }
    }
    else
    {
        for (y = rect->top; y <= rect->bottom; y++)
        {
            get = get_row;
            for (x = 0; x < row_count; x++)
            {
                write_data((GX_UBYTE *)get, sizeof(GX_UBYTE));
                get++;
            }

            /* Fix data to 4 bytes aligned. */
            for (x = 0; x < fixed_count; x++)
            {
                write_data((GX_UBYTE *)&color, sizeof(GX_UBYTE));
            }
            get_row += (canvas->x_resolution + 1) >> 1;
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_write_bitmap_data_monochrome                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function read canvas memory data and write it to bmp file.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    rectangle                             Rectangle specification       */
/*    write_data                            Write data callback function  */
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
/*    _gx_utility_canvas_to_bmp                                           */
/*                                                                        */
/**************************************************************************/
static void _gx_utility_write_bitmap_data_monochrome(GX_CANVAS *canvas, GX_RECTANGLE *rect, UINT (*write_data)(GX_UBYTE *byte_data, UINT data_count))
{
GX_UBYTE *get;
GX_UBYTE *get_row;
INT       x;
INT       y;
INT       row_count;
INT       fixed_count;
INT       bits_in_first_byte;
GX_UBYTE  color = 0;

    get_row = (GX_UBYTE *)(canvas->memory);
    get_row += ((canvas->x_resolution + 7) >> 3) * rect->top;
    get_row += rect->left >> 3;
    row_count = rect->right - rect->left + 1;
    row_count = (row_count + 7) >> 3;
    bits_in_first_byte = rect->left & 7;
    fixed_count = row_count & 3;
    if (fixed_count)
    {
        fixed_count = 4 - fixed_count;
    }

    if (bits_in_first_byte)
    {
        bits_in_first_byte = 8 - (rect->left & 7);

        /* If start pixel is not at the first bit of byte. We need to make it aligned. */
        for (y = rect->top; y <= rect->bottom; y++)
        {
            get = get_row;
            for (x = 0; x < row_count; x++)
            {
                color = (GX_UBYTE)((*get) << (8 - bits_in_first_byte));
                get++;
                color = (GX_UBYTE)(color | ((*get) >> bits_in_first_byte));

                write_data((GX_UBYTE *)&color, sizeof(GX_UBYTE));
            }

            /* Fix data to 4 bytes aligned. */
            for (x = 0; x < fixed_count; x++)
            {
                write_data((GX_UBYTE *)&color, sizeof(GX_UBYTE));
            }

            get_row += (canvas->x_resolution + 7) >> 3;
        }
    }
    else
    {
        for (y = rect->top; y <= rect->bottom; y++)
        {
            get = get_row;
            for (x = 0; x < row_count; x++)
            {
                write_data((GX_UBYTE *)get, sizeof(GX_UBYTE));
                get++;
            }

            /* Fix data to 4 bytes aligned. */
            for (x = 0; x < fixed_count; x++)
            {
                write_data((GX_UBYTE *)&color, sizeof(GX_UBYTE));
            }

            get_row += (canvas->x_resolution + 7) >> 3;
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_canvas_to_bmp                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Covert canvas memory data to bmp file.                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    rectangle                             Rectangle specification       */
/*    write_data                            Write data callback function  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define          Define a rectangle            */
/*    _gx_utility_rectangle_inside_detect   Detect rect inside of another */
/*    _gx_utility_write_bitmap_header       Create bmp file header        */
/*    _gx_utility_write_bitmap_data_monochrome                            */
/*    _gx_utility_write_bitmap_data_4bpp                                  */
/*    _gx_utility_write_bitmap_data_8bpp                                  */
/*    _gx_utility_write_bitmap_data_16bpp                                 */
/*    _gx_utility_write_bitmap_data_32bpp   Above functions read canvas   */
/*                                            memory and write data to    */
/*                                            file.                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gxe_utility_canvas_to_bmp            Error checking function       */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_canvas_to_bmp(GX_CANVAS *canvas, GX_RECTANGLE *rect, UINT (*write_data)(GX_UBYTE *byte_data, UINT data_count))
{
UINT         status;
UINT         pixelmap_format;
GX_RECTANGLE canvas_size;

    _gx_utility_rectangle_define(&canvas_size, 0, 0, (GX_VALUE)(canvas->x_resolution - 1), (GX_VALUE)(canvas->y_resolution - 1));

    if (!_gx_utility_rectangle_inside_detect(&canvas_size, rect))
    {
        return GX_INVALID_SIZE;
    }

    /* write bmp file header. */
    status = _gx_utility_write_bitmap_header(canvas, rect, write_data);

    if (status == GX_SUCCESS)
    {
        /* write bmp image data*/
        pixelmap_format = canvas->display->color_format;

        switch (pixelmap_format)
        {
        case GX_COLOR_FORMAT_MONOCHROME:
            _gx_utility_write_bitmap_data_monochrome(canvas, rect, write_data);
            break;


        case GX_COLOR_FORMAT_4BIT_GRAY:
            _gx_utility_write_bitmap_data_4bpp(canvas, rect, write_data);
            break;

        case GX_COLOR_FORMAT_8BIT_PACKED_PIXEL:
        case GX_COLOR_FORMAT_8BIT_PALETTE:
            _gx_utility_write_bitmap_data_8bpp(canvas, rect, write_data);
            break;

        case GX_COLOR_FORMAT_1555XRGB:
        case GX_COLOR_FORMAT_4444ARGB:
        case GX_COLOR_FORMAT_565RGB:
            _gx_utility_write_bitmap_data_16bpp(canvas, rect, write_data);
            break;

        case GX_COLOR_FORMAT_24XRGB:
        case GX_COLOR_FORMAT_32ARGB:
            _gx_utility_write_bitmap_data_32bpp(canvas, rect, write_data);
            break;

        case GX_COLOR_FORMAT_2BIT_GRAY: /* Not support now. */
        default:
            return GX_NOT_SUPPORTED;
        }
    }

    return status;
}
