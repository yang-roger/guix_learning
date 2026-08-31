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
/**   Image Reader Management (Image Reader)                              */
/**                                                                       */
/**************************************************************************/

#include "gx_image_reader.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_display.h"
#include "gx_canvas.h"

#define GX_FS_SCALE             10
#define GX_DITHER_SPECKLE_LIMIT 400

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_one_row_dither                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This helper function does dithering for one row of the pixelmap.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block    */
/*    error_ptr                             Memory for error information  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_image_reader_pixel_read]          Read a pixel from input buffer*/
/*    [gx_image_reader_pixel_write]         Write a pixel out with        */
/*                                            specified color format      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_colorspace_convert                                 */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
static void _gx_image_reader_one_row_dither(GX_IMAGE_READER *image_reader, INT *error_ptr)
{
UINT     width;
UINT     err_width;
UINT     col;
GX_PIXEL want_pixel;
GX_PIXEL written_pixel;
INT      red_err;
INT      green_err;
INT      blue_err;
INT     *next_err;
INT     *cur_err;

    width = image_reader->image_width;
    err_width = width + 2;

    cur_err = error_ptr;
    next_err = error_ptr + err_width * 3;
    memset(next_err, 0, sizeof(INT) * err_width * 3);

    for (col = 0; col < width; col++)
    {
        image_reader->pixel_read(image_reader, (INT)col, &want_pixel);

        red_err = (cur_err[col + 1] >> GX_FS_SCALE);
        green_err = (cur_err[err_width + col + 1] >> GX_FS_SCALE);
        blue_err = (cur_err[err_width * 2 + col + 1] >> GX_FS_SCALE);

        if (want_pixel.red + red_err > 255)
        {
            want_pixel.red = 255;
        }
        else if (want_pixel.red + red_err < 0)
        {
            want_pixel.red = 0;
        }
        else
        {
            want_pixel.red = (GX_UBYTE)(want_pixel.red + red_err);
        }

        if (want_pixel.green + green_err > 255)
        {
            want_pixel.green = 255;
        }
        else if (want_pixel.green + green_err < 0)
        {
            want_pixel.green = 0;
        }
        else
        {
            want_pixel.green = (GX_UBYTE)(want_pixel.green + green_err);
        }

        if (want_pixel.blue + blue_err > 255)
        {
            want_pixel.blue = 255;
        }
        else if (want_pixel.blue + blue_err < 0)
        {
            want_pixel.blue = 0;
        }
        else
        {
            want_pixel.blue = (GX_UBYTE)(want_pixel.blue + blue_err);
        }

        written_pixel = want_pixel;

        image_reader->pixel_write(image_reader, &written_pixel);

        /* Calculate color space convert error. */
        red_err = ((want_pixel.red - written_pixel.red) << GX_FS_SCALE);
        green_err = ((want_pixel.green - written_pixel.green) << GX_FS_SCALE);
        blue_err = ((want_pixel.blue - written_pixel.blue) << GX_FS_SCALE);

        /* Red error.  */
        if (GX_ABS(red_err) > GX_DITHER_SPECKLE_LIMIT)
        {
            cur_err[col + 2]  += ((red_err * 7) >> 4);
            next_err[col]     += ((red_err * 3) >> 4);
            next_err[col + 1] += ((red_err * 5) >> 4);
            next_err[col + 2] += ((red_err) >> 4);
        }
        else
        {
            cur_err[col + 2]  = 0;
            next_err[col]     = 0;
            next_err[col + 1] = 0;
            next_err[col + 2] = 0;
        }

        /* Green error. */
        if (GX_ABS(green_err) > GX_DITHER_SPECKLE_LIMIT)
        {
            cur_err[err_width + col + 2]  += ((green_err * 7) >> 4);
            next_err[err_width + col]     += ((green_err * 3) >> 4);
            next_err[err_width + col + 1] += ((green_err * 5) >> 4);
            next_err[err_width + col + 2] += ((green_err) >> 4);
        }
        else
        {
            cur_err[err_width + col + 2]  = 0;
            next_err[err_width + col]     = 0;
            next_err[err_width + col + 1] = 0;
            next_err[err_width + col + 2] = 0;
        }

        /* Blue error.  */
        if (GX_ABS(blue_err) > GX_DITHER_SPECKLE_LIMIT)
        {
            cur_err[err_width * 2 + col + 2] += ((blue_err * 7) >> 4);
            next_err[err_width * 2 + col] += ((blue_err * 3) >> 4);
            next_err[err_width * 2 + col + 1] += ((blue_err * 5) >> 4);
            next_err[err_width * 2 + col + 2] += ((blue_err) >> 4);
        }
        else
        {
            cur_err[err_width * 2 + col + 2]  = 0;
            next_err[err_width * 2 + col]     = 0;
            next_err[err_width * 2 + col + 1] = 0;
            next_err[err_width * 2 + col + 2] = 0;
        }
    }

    memcpy((void *)cur_err, (void *)next_err, err_width * 3 * sizeof(INT)); /* Use case of memcpy is verified. */
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_one_row_convert                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This helper function converts pixel data to specified color format  */
/*    for one row of the pixelmap.                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block    */
/*    width                                 Pointer to pixelmap width     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_image_reader_pixel_read]          Read a pixel from input buffer*/
/*    [gx_image_reader_pixel_write]         Write a pixel out with        */
/*                                            specified color format      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_colorspace_convert                                 */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
static void _gx_image_reader_one_row_convert(GX_IMAGE_READER *image_reader, INT *width)
{
INT      xval;
GX_PIXEL pixel;

    for (xval = 0; xval < (*width); xval++)
    {
        image_reader->pixel_read(image_reader, xval, &pixel);
        image_reader->pixel_write(image_reader, &pixel);
    }
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_one_row_rotated_convert                            */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This helper function converts pixel data to specified color format  */
/*    for one row of the pixelmap with rotation.                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block    */
/*    width                                 Pointer to pixelmap width     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_image_reader_pixel_read]          Read a pixel from input buffer*/
/*    [gx_image_reader_pixel_write]         Write a pixel out with        */
/*                                            specified color format      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_colorspace_convert                                 */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
static void _gx_image_reader_one_row_rotated_convert(GX_IMAGE_READER *image_reader, INT *width)
{
INT      xval;
GX_PIXEL pixel;

    image_reader->putdata = image_reader->putdatarow;
    image_reader->putauxdata = image_reader->putauxdatarow;

    for (xval = 0; xval < (*width); xval++)
    {
        image_reader->pixel_read(image_reader, xval, &pixel);
        image_reader->pixel_write(image_reader, &pixel);
    }

    image_reader->putdatarow += image_reader->putdatarow_stride;
    image_reader->putauxdatarow += image_reader->putauxdatarow_stride;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_colorspace_convert                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function converts a pixelmap to the specified color format     */
/*    with or without dithering process.                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block    */
/*    outmap                                Output pixelmap               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_one_row_dither      Process dithering for one row  */
/*    _gx_image_reader_one_row_convert     Process direct convert for one */
/*                                           row                          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
UINT _gx_image_reader_colorspace_convert(GX_IMAGE_READER *image_reader, GX_PIXELMAP *outmap)
{
INT  yval;
INT *param = GX_NULL;
INT  width;
INT  height;
void (*row_convert)(GX_IMAGE_READER *, INT *);

    width = outmap->width;
    height = outmap->height;

    if (image_reader->mode & GX_IMAGE_READER_MODE_DITHER)
    {
        /* This function is called to convert decoded png/jpeg data to guix format pixelmap,
           max png/jpeg width and heigt is limited to 14 bits, so overflow cannot occur. */
        param = (INT *)_gx_system_memory_allocator(sizeof(INT) * (UINT)(width + 2) * 6);

        if (!param)
        {
            return GX_SYSTEM_MEMORY_ERROR;
        }

        memset((void *)param, 0, (sizeof(INT) * (UINT)(width + 2) * 3));
        row_convert = _gx_image_reader_one_row_dither;
    }
    else
    {
        param = &width;

        if (image_reader->mode & (GX_IMAGE_READER_MODE_ROTATE_CW | GX_IMAGE_READER_MODE_ROTATE_CCW))
        {
            row_convert = _gx_image_reader_one_row_rotated_convert;
        }
        else
        {
            row_convert = _gx_image_reader_one_row_convert;
        }
    }

    image_reader->size_testing = GX_FALSE;

    /* Run color space convert.  */
    for (yval = 0; yval < height; yval++)
    {
        row_convert(image_reader, param);

        image_reader->getdata += image_reader->input_stride;
        image_reader->getauxdata += width;

        switch (image_reader->color_format)
        {
        case GX_COLOR_FORMAT_4BIT_GRAY:
            if (image_reader->putdata_mask != 0xf0)
            {
                image_reader->putdata++;
                image_reader->putdata_mask = 0xf0;
            }

            if (image_reader->putauxdata)
            {
                if (image_reader->putauxdata_mask != 0x80)
                {
                    image_reader->putauxdata++;
                    image_reader->putauxdata_mask = 0x80;
                }
            }
            break;

        case GX_COLOR_FORMAT_MONOCHROME:
            if (image_reader->putdata_mask != 0x80)
            {
                image_reader->putdata++;
                image_reader->putdata_mask = 0x80;
            }
            break;
        }
    }

    if (image_reader->mode & GX_IMAGE_READER_MODE_DITHER)
    {
        _gx_system_memory_free((void *)param);
    }

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_create                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a runtime raw image reader / decoder.         */
/*    Currently only jpeg and png raw image types are supported.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader              Image reader control block.               */
/*    read_data                 Pointer to raw input data.                */
/*    read_data_size            Size of raw input data.                   */
/*    color_format              The requested output color format         */
/*    mode                      Compress dither and alpha modes flags     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
UINT _gx_image_reader_create(GX_IMAGE_READER *image_reader,
                             const GX_UBYTE *data,
                             INT data_size,
                             GX_UBYTE color_format,
                             GX_UBYTE mode)
{
    image_reader->color_format     = color_format;
    image_reader->mode             = mode;
    image_reader->image_type       = 0;
    image_reader->source_data      = data;
    image_reader->source_data_size = (UINT)data_size;
    image_reader->getdata          = GX_NULL;
    image_reader->getauxdata       = GX_NULL;
    image_reader->putauxdata       = GX_NULL;
    image_reader->palette          = GX_NULL;
    image_reader->palette_size     = 0;
    image_reader->image_height     = 0;
    image_reader->image_width      = 0;
    image_reader->png_trans        = 0;
    image_reader->png_palette      = GX_NULL;
    image_reader->png_palette_size = 0;
    image_reader->putdata          = GX_NULL;
    image_reader->putauxdata       = GX_NULL;
    image_reader->pixel_read       = GX_NULL;
    image_reader->pixel_write      = GX_NULL;
    image_reader->size_testing     = GX_FALSE;
    image_reader->mono_shreshold   = 128;
    image_reader->putdata_mask     = 0;
    image_reader->putauxdata_mask  = 0;

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_gx_image_type_get                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves the image type and stores the image type    */
/*    in the GX_IMAGE_READER control block.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
static UINT _gx_image_reader_image_type_get(GX_IMAGE_READER *image_reader)
{
GX_UBYTE        image_type = 0;
const GX_UBYTE *buffer;

    buffer = image_reader->source_data;

    if (buffer)
    {
        if (buffer[1] == 'P')
        {
            image_type = GX_IMAGE_TYPE_PNG;
        }
        else
        {
            if (buffer[6] == 'J')
            {
                image_type = GX_IMAGE_TYPE_JPG;
            }
            else
            {
                if (buffer[1] == 0xd8 && buffer[0] == 0xff && (buffer[2] == 0xff))
                {
                    image_type = GX_IMAGE_TYPE_JPG;
                }
            }
        }
    }

    image_reader->image_type = image_type;
    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_image_decode                                       */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function decodes input image to raw pixelmap format.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block    */
/*    outmap                                Output pixelmap               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_image_type_get                                     */
/*    _gx_utility_jpeg_decode                                             */
/*    _gx_utility_png_decode                                              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_colorspace_convert                                 */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
UINT _gx_image_reader_image_decode(GX_IMAGE_READER *image_reader, GX_PIXELMAP *outmap)
{
UINT status;

    /* Get image type. */
    _gx_image_reader_image_type_get(image_reader);

    switch (image_reader->image_type)
    {
    case GX_IMAGE_TYPE_JPG:
        status = _gx_image_reader_jpeg_decode(image_reader, outmap);
        break;

    case GX_IMAGE_TYPE_PNG:
        status = _gx_image_reader_png_decode(image_reader->source_data, image_reader->source_data_size, outmap);
        if (outmap->transparent_color)
        {
            image_reader->png_trans = (GX_COLOR *)outmap->transparent_color;
        }
        break;

    default:
        return GX_NOT_SUPPORTED;
    }

    if (!(outmap->flags & GX_PIXELMAP_ALPHA))
    {
        image_reader->mode = (GX_UBYTE)(image_reader->mode & (ULONG)(~GX_IMAGE_READER_MODE_ALPHA));
    }

    return status;
}
#endif

#if defined(GX_SOFTWARE_DECODER_SUPPORT)

#define GX_SATURATE_TO_UBYTE(result, i) \
    {                                   \
        if ((i) < 0)                    \
        {                               \
            (result) = 0;               \
        }                               \
        else if ((i) > 255)             \
        {                               \
            (result) = 255;             \
        }                               \
        else                            \
        {                               \
            (result) = (GX_UBYTE)(i);   \
        }                               \
    }

#define GX_SATURATE_TO_BYTE(result, i) \
    {                                  \
        if ((i) < -128)                \
        {                              \
            (result) = -128;           \
        }                              \
        else if ((i) > 127)            \
        {                              \
            (result) = 127;            \
        }                              \
        else                           \
        {                              \
            (result) = (GX_BYTE)(i);   \
        }                              \
    }

#define GX_JPEG_BITS_GET(jpeg_info, num_of_bits)                                                                                                                    \
    while (jpeg_info->bit_count <= num_of_bits)                                                                                                           \
    {                                                                                                                                                               \
        if ((jpeg_info->data_index < jpeg_info->data_size) && (jpeg_info->bit_count <= 24))                                           \
        {                                                                                                                                                           \
            jpeg_info->bit_buffer |= ((UINT)(jpeg_info->data[jpeg_info->data_index]) << (UINT)(24 - jpeg_info->bit_count)); \
                                                                                                                                                                    \
            /* 2 byte 'FF00' sequence should be considered as just a byte 0xFF. */                                                                                  \
            if ((jpeg_info->data[jpeg_info->data_index] == 0xff) &&                                                                             \
                (jpeg_info->data_index + 1 < jpeg_info->data_size) &&                                                                           \
                (jpeg_info->data[jpeg_info->data_index + 1] == 0x00))                                                                           \
            {                                                                                                                                                       \
                jpeg_info->data_index += 2;                                                                                                               \
            }                                                                                                                                                       \
            else                                                                                                                                                    \
            {                                                                                                                                                       \
                jpeg_info->data_index += 1;                                                                                                               \
            }                                                                                                                                                       \
        }                                                                                                                                                           \
                                                                                                                                                                    \
        jpeg_info->bit_count += 8;                                                                                                                        \
    }

#define GX_JPEG_BITS_SKIP(jpeg_info, skip_bits)        \
    (jpeg_info)->bit_buffer <<= (skip_bits); \
    (jpeg_info)->bit_count -= (skip_bits);

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_dislay_driver_jpeg_quantization_table_set                       */
/*                                                           6.2.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Sets the JPEG quantization table.                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG data control block       */
/*    segment_len                           Initial length of the segment */
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
/*    _gx_image_reader_jpeg_decode                                        */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_jpeg_quantization_table_set(GX_JPEG_INFO *jpeg_info, UINT segment_len)
{
GX_UBYTE  table_index;
GX_UBYTE *jpeg_data = jpeg_info->data + jpeg_info->data_index;
INT       index;

    /* Minus two-byte length. */
    jpeg_info->data_index += (INT)segment_len;
    segment_len -= 2;
    jpeg_data += 2;

    while (segment_len)
    {
        /* The upper 4 bits specify the element precision: 0 indicates 8-bit, 1 indecates 16-bit. */
        if ((*jpeg_data) & 0xf0)
        {
            /* Baseline DCT-based jpeg only support 8-bit precision. */
            return GX_NOT_SUPPORTED;
        }

        /* The lower 4 bits specify the table destination identifier, specify one of four possible destinations. */
        table_index = (*jpeg_data++) & 0x03;

        /* Read quantization table element. */
        for (index = 0; index < 64; index++)
        {
            jpeg_info->quantization_table[table_index][index] = *jpeg_data++;
        }

        if (segment_len < 65)
        {
            return GX_INVALID_FORMAT;
        }
        segment_len -= 65;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_huffcode_find                                 */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Lookup the huffman code.                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG data control block       */
/*    table_class                           Table class, 0 = DC table,    */
/*                                          1 = AC table                  */
/*    table_id                              Table index                   */
/*    scan_buffer                           Buffer to search from         */
/*    bit_len                               Retrieved Huffman Code Length */
/*    code_value                            Retrieved Huffman Code        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status Code                                                         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_dc_decode                                     */
/*    _gx_image_reader_jpeg_ac_decode                                     */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_jpeg_huffcode_find(GX_JPEG_INFO *jpeg_info,
                                                UINT table_class,
                                                UINT table_id,
                                                UINT *bit_len,
                                                GX_UBYTE *code_value)
{
GX_UBYTE          index;
USHORT            code;
USHORT            code_index;
GX_HUFFCODE_INFO *code_info;

    for (index = 0; index < 16; index++)
    {
        code_info = &jpeg_info->huffman_code_info[table_class][table_id][index];
        if (code_info->bits)
        {
            code = (USHORT)((jpeg_info->bit_buffer) >> (32 - code_info->bits));

            if (code <= code_info->end)
            {
                code_index = (USHORT)(code_info->index + code - code_info->start);
                *bit_len = code_info->bits;
                *code_value = jpeg_info->huffman_table[table_class][table_id][code_index];
                return GX_SUCCESS;
            }
        }
        else
        {
            break;
        }
    }

    return GX_FAILURE;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_huffman_table_set                                  */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Sets up the huffman table.                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG data control block       */
/*    segment_len                           Initial length of the segment */
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
/*    _gx_image_reader_jpeg_decode_blocks                                 */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_huffman_table_set(GX_JPEG_INFO *jpeg_info, UINT segment_len)
{
GX_UBYTE         *jpeg_data = jpeg_info->data + jpeg_info->data_index;
GX_UBYTE          table_class;
GX_UBYTE          table_id;
GX_UBYTE          bit_count;
UINT              i_bits;
USHORT            i_table_size;
GX_HUFFCODE_INFO *code_info;
INT               index = 0;
USHORT            code = 0;

    /* must have at least one code for each of 16 huffman bit lengths */
    if (segment_len < 19)
    {
        return GX_INVALID_FORMAT;
    }

    /* Minus two-byte length. */
    jpeg_info->data_index += (INT)segment_len;
    segment_len -= 2;
    jpeg_data += 2;

    while (segment_len)
    {
        /* table_calss: 0 DC 1 AC */
        table_class = ((*jpeg_data) >> 4) & 1;
        table_id = (*jpeg_data++) & 0x01;

        if (segment_len < 17)
        {
            return GX_INVALID_FORMAT;
        }
        segment_len -= 17;

        i_table_size = 0;

        index = 0;
        code = 0;

        /* Read the number of Huffman codes for each bit length, from 1 to 16. */
        for (i_bits = 0; i_bits < 16; i_bits++)
        {
            bit_count = *jpeg_data++;

            if (bit_count)
            {
                code_info = &jpeg_info->huffman_code_info[table_class][table_id][index++];
                code_info->index = i_table_size;
                code_info->start = code;
                code_info->end = (USHORT)(code + bit_count - 1);
                code_info->bits = (GX_UBYTE)(i_bits + 1);
            }
            code = (USHORT)((code + bit_count) << 1);

            i_table_size = (USHORT)(i_table_size + bit_count);
        }

        if (segment_len < i_table_size)
        {
            return GX_INVALID_FORMAT;
        }

        segment_len -= i_table_size;

        /* Load the start address of the specified huffman table. */
        jpeg_info->huffman_table[table_class][table_id] = jpeg_data;
        jpeg_data += i_table_size;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_jpeg_frame_header_read                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Reads in the frame header infomration.                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG data control block       */
/*    segment_len                           Initial length of the segment */
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
/*    _gx_image_reader_jpeg_decode_blocks                                 */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_jpeg_frame_header_read(GX_JPEG_INFO *jpeg_info, UINT segment_len)
{
GX_UBYTE *jpeg_data = jpeg_info->data + jpeg_info->data_index;
INT       i_component;

    jpeg_info->data_index += (INT)segment_len;
    jpeg_data += 2;

    /* Skip precision field. */
    jpeg_data++;

    /* Read image width, WORD */
    jpeg_info->height = *jpeg_data++;
    jpeg_info->height = (USHORT)(jpeg_info->height << 8);
    jpeg_info->height = (USHORT)(jpeg_info->height | (*jpeg_data++));

    /* Limit max jpeg height to 14 bits. */
    if (jpeg_info->height > GX_MAX_PIXELMAP_RESOLUTION)
    {
        return GX_INVALID_HEIGHT;
    }

    /* Read image height, WORD */
    jpeg_info->width = *jpeg_data++;
    jpeg_info->width = (USHORT)(jpeg_info->width << 8);
    jpeg_info->width = (USHORT)(jpeg_info->width | (*jpeg_data++));

    /* Limit max jpeg width to 14 bits. */
    if (jpeg_info->width > GX_MAX_PIXELMAP_RESOLUTION)
    {
        return GX_INVALID_WIDTH;
    }

    /* Read image components. */
    jpeg_info->num_of_components = *jpeg_data++;

    if (jpeg_info->num_of_components > JPG_MAX_COMPONENTS)
    {
        return GX_FAILURE;
    }

    for (i_component = 0; i_component < jpeg_info->num_of_components; i_component++)
    {
        /* Read component id */
        jpeg_info->component_id[i_component] = *jpeg_data++;

        /* Read sample factor */
        jpeg_info->sample_factor[i_component] = *jpeg_data++;

        /* Read quantization table index */
        jpeg_info->qantization_table_index[i_component] = *jpeg_data++;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_scan_header_read                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Reads in the scan header information.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG data control block       */
/*    segment_len                           Initial length of the segment */
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
/*    _gx_image_reader_jpeg_decode_blocks                                 */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_jpeg_scan_header_read(GX_JPEG_INFO *jpeg_info, UINT segment_len)
{
GX_UBYTE *jpeg_data = jpeg_info->data + jpeg_info->data_index;
INT       index;

    jpeg_data += 2;
    jpeg_info->data_index += (INT)segment_len;

    /* Read the number of image components.  */
    jpeg_info->num_of_components = *jpeg_data++;

    if (jpeg_info->num_of_components > JPG_MAX_COMPONENTS)
    {
        return GX_FAILURE;
    }

    for (index = 0; index < jpeg_info->num_of_components; index++)
    {
        /* skip image component */
        jpeg_data++;

        /* high bits correspond to DC table index.
           low bits correspond to AC table index
           0: Y Huffman table
           1: Chrominance Huffman table. */
        jpeg_info->dc_table_index[index] = ((*jpeg_data) & 0xf0) >> 4;
        jpeg_info->ac_table_index[index] = (*jpeg_data++) & 0x0f;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_dc_decode                                     */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Decode the DC component.                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG data control block       */
/*    i_component                           Component index               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    GX_JPEG_BITS_GET                       Extract a specified number of*/
/*                                             bits from JPEG data stream */
/*    GX_JPEG_BITS_SKIP                      Skips bits from tempory JPEG */
/*                                             data stream                */
/*    _gx_image_reader_jpeg_huffman_code_find                             */
/*                                           Lookup the huffman code      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_block_decode                              */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_jpeg_dc_decode(GX_JPEG_INFO *jpeg_info, UINT i_component)
{
UINT     i_bits = 0;
GX_UBYTE code_value;
INT      diff;
UINT     table_index = jpeg_info->dc_table_index[i_component];
GX_BOOL  negative;

    if (table_index >= HUFF_TABLE_DIMENSION)
    {
        return GX_FAILURE;
    }

    GX_JPEG_BITS_GET(jpeg_info, 16);

    if (_gx_image_reader_jpeg_huffcode_find(jpeg_info, 0, table_index, &i_bits, &code_value) == 0)
    {
        GX_JPEG_BITS_SKIP(jpeg_info, i_bits);

        if (code_value == 0)
        {
            jpeg_info->vecter[0] = jpeg_info->pre_dc[i_component];
        }
        else
        {

            GX_JPEG_BITS_GET(jpeg_info, code_value);
            diff = (INT)(((UINT)jpeg_info->bit_buffer) >> (32 - code_value));
            GX_JPEG_BITS_SKIP(jpeg_info, code_value);

            negative = !(diff >> (code_value - 1));

            if (negative)
            {
                diff += 1 - (1 << code_value);
            }

            jpeg_info->vecter[0] = diff + jpeg_info->pre_dc[i_component];
            jpeg_info->pre_dc[i_component] = jpeg_info->vecter[0];
        }
    }
    else
    {
        return GX_FAILURE;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_ac_decode                                     */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Decode the AC component.                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG data control block       */
/*    i_component                           Component index               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status Code                                                         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    GX_JPEG_BITS_GET                       Extract a specified number of*/
/*                                             bits from JPEG data stream */
/*    GX_JPEG_BITS_SKIP                      Skips bits from tempory JPEG */
/*                                             data stream                */
/*    _gx_image_reader_jpeg_huffman_code_find                             */
/*                                           Lookup the huffman code      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_block_decode                              */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_jpeg_ac_decode(GX_JPEG_INFO *jpeg_info, UINT i_component)
{
UINT     i_bits = 0;
GX_UBYTE code_value;
GX_UBYTE catogory;
GX_UBYTE runs_of_zero;
INT      ac_coefficient;
UINT     ac_counter = 1;
UINT     table_index = jpeg_info->ac_table_index[i_component];
INT      negative;

    if (table_index >= HUFF_TABLE_DIMENSION)
    {
        return GX_FAILURE;
    }

    while (ac_counter < 64)
    {
        i_bits = 0;
        GX_JPEG_BITS_GET(jpeg_info, 16);

        if (_gx_image_reader_jpeg_huffcode_find(jpeg_info, 1, table_index, &i_bits, &code_value) == 0)
        {
            GX_JPEG_BITS_SKIP(jpeg_info, i_bits);

            runs_of_zero = (0xf0 & code_value) >> 4;
            catogory = 0x0f & code_value;

            if (catogory == 0)
            {
                /* EOB encountered. */
                if (runs_of_zero == 0)
                {
                    ac_counter = 64;
                }
                else if (runs_of_zero == 0x0f)
                {
                    ac_counter += 16;
                }
            }
            else
            {
                ac_counter += runs_of_zero;

                GX_JPEG_BITS_GET(jpeg_info, catogory);
                ac_coefficient = (INT)((jpeg_info->bit_buffer) >> (32 - catogory));
                GX_JPEG_BITS_SKIP(jpeg_info, catogory);

                negative = !(ac_coefficient >> (catogory - 1));

                if (negative)
                {
                    ac_coefficient += 1 - (1 << catogory);
                }

                if (ac_counter < 64)
                {
                    jpeg_info->vecter[ac_counter++] = ac_coefficient;
                }
            }
        }
        else
        {
            return GX_FAILURE;
        }
    }

    return GX_SUCCESS;
}

#if defined(GX_ENABLE_ARM_HELIUM)
/* Define the triple Bufferfly Addition operation */
#define VBUTTERFLY_ADDITION(a, b) \
    vtemp = vaddq_s32(a, b);      \
    b = vsubq_s32(a, b);          \
    a = vtemp

/* Define the butterfly Multiplication */
#define VBUTTERFLY_MULTIPLICATION(a, b, cos, sin) \
    vtempa = vmulq_n_s32(a, cos);                 \
    vtempb = vmulq_n_s32(b, sin);                 \
    vtemp = vaddq_s32(vtempa, vtempb);            \
    vtempa = vmulq_n_s32(a, sin);                 \
    vtempb = vmulq_n_s32(b, cos);                 \
    b = vsubq_s32(vtempb, vtempa);                \
    a = vtemp

#define VBUTTERFLY_MULTIPLICATION_SHR6(a, b, cos, sin) \
    vtempa = vmulq_n_s32(a, cos);                      \
    vtempb = vmulq_n_s32(b, sin);                      \
    vtemp = vaddq_s32(vtempa, vtempb);                 \
    vtempa = vmulq_n_s32(a, sin);                      \
    vtempb = vmulq_n_s32(b, cos);                      \
    b = vsubq_s32(vtempb, vtempa);                     \
    b = vshrq_n_s32(b, 6);                             \
    a = vshrq_n_s32(vtemp, 6)
#else

/* Define the triple Bufferfly Addition operation */
#define BUTTERFLY_ADDITION(a, b) \
    t = a + b;                   \
    b = a - b;                   \
    a = t

/* Define the butterfly Multiplication */
#define BUTTERFLY_MULTIPLICATION(a, b, cos, sin) \
    t = (a * cos + b * sin);                     \
    b = (b * cos - a * sin);                     \
    a = t

#define BUTTERFLY_MULTIPLICATION_SHR6(a, b, cos, sin) \
    t = (a * cos + b * sin) >> 6;                     \
    b = (b * cos - a * sin) >> 6;                     \
    a = t

#endif

/* Define constants.  */
#define R2_SHR7   181 /* = sqrt(2) << 7.  */
#define C1_SHR8   251 /* = cos(pi/16) << 8.  */
#define S1_SHR8   50  /* = sin(pi/16) << 8.  */
#define C3_SHR8   213 /* = cos(3pi/16)*sqrt(2) << 8.  */
#define S3_SHR8   142 /* = sin(3pi/16)*sqrt(2) << 8.  */
#define C6R2_SHR9 277 /* = cos(pi/16)*sqrt(2) << 9.  */
#define S6R2_SHR9 669 /* = sin(3pi/16)*sqrt(2) << 9.  */
#define POSTSH1   9
#define POSTSH2   12

#if defined(GX_ENABLE_ARM_HELIUM)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_dequantize_idct                               */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Dequatilizes decoded data and performs Inverse Discrete Consine     */
/*    Transformation using helium intrinsics.                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    block                                 Pointer to decoded data       */
/*    quant_table                           Pointer to quantization table */
/*    out                                   Buffer for output data        */
/*    stride                                Stride of output data         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    vldrwq_gather_shifted_offset_s32      Gather data from memory       */
/*    vmulq_s32                             Multiply two vectors          */
/*    vmulq_n_s32                           Multiply by scaler            */
/*    vqrshlq_n_s32                         Shift each element of a       */
/*                                            vector register left by     */
/*                                            the immediate value         */
/*    vaddq_s32                             Add two vectors               */
/*    vrshrq_n_s32                          Shift each element of a       */
/*                                            vector register right by    */
/*                                            the immediate value         */
/*    vstrhq_scatter_offset_s32             Scatter store data to memory  */
/*    vldrhq_s32                            Load vector register          */
/*    vqrshrnbq_n_s32                       Shift right with saturation,  */
/*                                            and write the result to the */
/*                                            bottom half of the result   */
/*                                            element                     */
/*    vqmovntq_s16                          Saturate to half width and    */
/*                                            write the result to the     */
/*                                            top of the result element   */
/*    vstrbq_scatter_offset_s8              Scatter store data to memory  */
/*    VBUTTERFLY_ADDITION                   Perform butterfly addition    */
/*    VBUTTERFLY_MULTIPLICATION             Perform butterfly             */
/*                                            multiplication              */
/*    VBUTTERFLY_MULTIPLICATION_SHR6        Perform butterfly             */
/*                                            multiplication with shift   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_block_decode                              */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_jpeg_dequantize_idct(INT *block, INT *quant_table, GX_BYTE *out, INT stride)
{
static uint32x4_t voffset[8][2] = {
    {{0,  2,  3,  9}, {10, 20, 21, 35}},
    {{1,  4,  8,  11}, {19, 22, 34, 36}},
    {{5,  7,  12, 18}, {23, 33, 37, 48}},
    {{6,  13, 17, 24}, {32, 38, 47, 49}},
    {{14, 16, 25, 31}, {39, 46, 50, 57}},
    {{15, 26, 30, 40}, {45, 51, 56, 58}},
    {{27, 29, 41, 44}, {52, 55, 59, 62}},
    {{28, 42, 43, 53}, {54, 60, 61, 63}}
};
static uint32x4_t vstroffset1= {0, 16, 32, 48};
INT               index;
GX_VALUE          temp_block[64];
GX_VALUE         *output_data;
int32x4_t         vrow0, vrow1,  vrow2, vrow3, vrow4, vrow5, vrow6, vrow7, vtemp, vtempa, vtempb;
uint8x16_t        vstroffset = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};
int16x8_t         vtemp16;
int8x16_t         vtemp8;
int16_t const    *base;

    /* Perform 2-D DCT by applying first a 1-D DCT over the rows
       followed by a 1-D DCT over the columns of the input-data matrix.*/
    for (index = 0; index < 4; index++)
    {
        vstroffset[0 + index * 4] += (stride * 3);
        vstroffset[1 + index * 4] += stride;
        vstroffset[2 + index * 4] += (stride << 1);
    }


    for (index = 0; index < 2; index++)
    {
        output_data = temp_block + 32 * index;

        /* Load data. */
        vrow0 = vldrwq_gather_shifted_offset_s32(block, voffset[0][index]);
        vtemp = vldrwq_gather_shifted_offset_s32(quant_table, voffset[0][index]);
        vrow0 = vmulq_s32(vrow0, vtemp);

        vrow1 = vldrwq_gather_shifted_offset_s32(block, voffset[1][index]);
        vtemp = vldrwq_gather_shifted_offset_s32(quant_table, voffset[1][index]);
        vrow1 = vmulq_s32(vrow1, vtemp);

        vrow2 = vldrwq_gather_shifted_offset_s32(block, voffset[2][index]);
        vtemp = vldrwq_gather_shifted_offset_s32(quant_table, voffset[2][index]);
        vrow2 = vmulq_s32(vrow2, vtemp);

        vrow3 = vldrwq_gather_shifted_offset_s32(block, voffset[3][index]);
        vtemp = vldrwq_gather_shifted_offset_s32(quant_table, voffset[3][index]);
        vrow3 = vmulq_s32(vrow3, vtemp);

        vrow4 = vldrwq_gather_shifted_offset_s32(block, voffset[4][index]);
        vtemp = vldrwq_gather_shifted_offset_s32(quant_table, voffset[4][index]);
        vrow4 = vmulq_s32(vrow4, vtemp);

        vrow5 = vldrwq_gather_shifted_offset_s32(block, voffset[5][index]);
        vtemp = vldrwq_gather_shifted_offset_s32(quant_table, voffset[5][index]);
        vrow5 = vmulq_s32(vrow5, vtemp);

        vrow6 = vldrwq_gather_shifted_offset_s32(block, voffset[6][index]);
        vtemp = vldrwq_gather_shifted_offset_s32(quant_table, voffset[6][index]);
        vrow6 = vmulq_s32(vrow6, vtemp);

        vrow7 = vldrwq_gather_shifted_offset_s32(block, voffset[7][index]);
        vtemp = vldrwq_gather_shifted_offset_s32(quant_table, voffset[7][index]);
        vrow7 = vmulq_s32(vrow7, vtemp);

        /*  Prescale.  */
        vrow0 = vqrshlq_n_s32(vrow0, 9);
        vrow1 = vqrshlq_n_s32(vrow1, 7);
        vrow4 = vqrshlq_n_s32(vrow4, 9);
        vrow7 = vqrshlq_n_s32(vrow7, 7);

        /* stage 1.  */
        VBUTTERFLY_ADDITION(vrow1, vrow7);

        vrow3 = vmulq_n_s32(vrow3, R2_SHR7);
        vrow5 = vmulq_n_s32(vrow5, R2_SHR7);

        /* stage 2.  */
        VBUTTERFLY_ADDITION(vrow0, vrow4);
        VBUTTERFLY_MULTIPLICATION(vrow6, vrow2, C6R2_SHR9, S6R2_SHR9);
        VBUTTERFLY_ADDITION(vrow7, vrow5);
        VBUTTERFLY_ADDITION(vrow1, vrow3);

        /* stage 3.  */
        VBUTTERFLY_ADDITION(vrow0, vrow6);
        VBUTTERFLY_ADDITION(vrow4, vrow2);
        VBUTTERFLY_MULTIPLICATION_SHR6(vrow5, vrow3, C1_SHR8, S1_SHR8);
        VBUTTERFLY_MULTIPLICATION_SHR6(vrow1, vrow7, C3_SHR8, S3_SHR8);

        /* stage 4.  */
        vtemp = vaddq_s32(vrow0, vrow1);
        vtemp = vrshrq_n_s32(vtemp, POSTSH1);
        vstrhq_scatter_offset_s32(output_data, vstroffset1, vtemp);

        vtemp = vaddq_s32(vrow4, vrow5);
        vtemp = vrshrq_n_s32(vtemp, POSTSH1);
        vstrhq_scatter_offset_s32(output_data + 1, vstroffset1, vtemp);

        vtemp = vaddq_s32(vrow2, vrow3);
        vtemp = vrshrq_n_s32(vtemp, POSTSH1);
        vstrhq_scatter_offset_s32(output_data + 2, vstroffset1, vtemp);

        vtemp = vaddq_s32(vrow6, vrow7);
        vtemp = vrshrq_n_s32(vtemp, POSTSH1);
        vstrhq_scatter_offset_s32(output_data + 3, vstroffset1, vtemp);

        vtemp = vsubq_s32(vrow6, vrow7);
        vtemp = vrshrq_n_s32(vtemp, POSTSH1);
        vstrhq_scatter_offset_s32(output_data + 4, vstroffset1, vtemp);

        vtemp = vsubq_s32(vrow2, vrow3);
        vtemp = vrshrq_n_s32(vtemp, POSTSH1);
        vstrhq_scatter_offset_s32(output_data + 5, vstroffset1, vtemp);

        vtemp = vsubq_s32(vrow4, vrow5);
        vtemp = vrshrq_n_s32(vtemp, POSTSH1);
        vstrhq_scatter_offset_s32(output_data + 6, vstroffset1, vtemp);

        vtemp = vsubq_s32(vrow0, vrow1);
        vtemp = vrshrq_n_s32(vtemp, POSTSH1);
        vstrhq_scatter_offset_s32(output_data + 7, vstroffset1, vtemp);
    }

    for (index = 0; index < 2; index++)
    {
        base = (int16_t const *)(temp_block + 4 * index);

        /* Load data. */
        vrow0 = vldrhq_s32(base);
        vrow1 = vldrhq_s32(base + 8);
        vrow2 = vldrhq_s32(base + 16);
        vrow3 = vldrhq_s32(base + 24);
        vrow4 = vldrhq_s32(base + 32);
        vrow5 = vldrhq_s32(base + 40);
        vrow6 = vldrhq_s32(base + 48);
        vrow7 = vldrhq_s32(base + 56);

        /*  Prescale.  */
        vrow0 = vqrshlq_n_s32(vrow0, 9);
        vrow1 = vqrshlq_n_s32(vrow1, 7);
        vrow4 = vqrshlq_n_s32(vrow4, 9);
        vrow7 = vqrshlq_n_s32(vrow7, 7);

        /* stage 1.  */
        VBUTTERFLY_ADDITION(vrow1, vrow7);

        vrow3 = vmulq_n_s32(vrow3, R2_SHR7);
        vrow5 = vmulq_n_s32(vrow5, R2_SHR7);

        /* stage 2.  */
        VBUTTERFLY_ADDITION(vrow0, vrow4);
        VBUTTERFLY_MULTIPLICATION(vrow6, vrow2, C6R2_SHR9, S6R2_SHR9);
        VBUTTERFLY_ADDITION(vrow7, vrow5);
        VBUTTERFLY_ADDITION(vrow1, vrow3);

        /* stage 3.  */
        VBUTTERFLY_ADDITION(vrow0, vrow6);
        VBUTTERFLY_ADDITION(vrow4, vrow2);
        VBUTTERFLY_MULTIPLICATION_SHR6(vrow5, vrow3, C1_SHR8, S1_SHR8);
        VBUTTERFLY_MULTIPLICATION_SHR6(vrow1, vrow7, C3_SHR8, S3_SHR8);

        /* stage 4.  */

        vtemp = vaddq_s32(vrow0, vrow1);
        vtemp16 = vqrshrntq_n_s32(vtemp16, vtemp, POSTSH2);

        vtemp = vaddq_s32(vrow4, vrow5);
        vtemp16 = vqrshrnbq_n_s32(vtemp16, vtemp, POSTSH2);
        vtemp8 = vqmovntq_s16(vtemp8, vtemp16);

        vtemp = vaddq_s32(vrow2, vrow3);
        vtemp16 = vqrshrntq_n_s32(vtemp16, vtemp, POSTSH2);

        vtemp = vaddq_s32(vrow6, vrow7);
        vtemp16 = vqrshrnbq_n_s32(vtemp16, vtemp, POSTSH2);
        vtemp8 = vqmovnbq_s16(vtemp8, vtemp16);
        vstrbq_scatter_offset_s8(out + 4 * index, vstroffset, vtemp8);

        vtemp = vsubq_s32(vrow6, vrow7);
        vtemp16 = vqrshrntq_n_s32(vtemp16, vtemp, POSTSH2);

        vtemp = vsubq_s32(vrow2, vrow3);
        vtemp16 = vqrshrnbq_n_s32(vtemp16, vtemp, POSTSH2);
        vtemp8 = vqmovntq_s16(vtemp8, vtemp16);

        vtemp = vsubq_s32(vrow4, vrow5);
        vtemp16 = vqrshrntq_n_s32(vtemp16, vtemp, POSTSH2);

        vtemp = vsubq_s32(vrow0, vrow1);
        vtemp16 = vqrshrnbq_n_s32(vtemp16, vtemp, POSTSH2);
        vtemp8 = vqmovnbq_s16(vtemp8, vtemp16);
        vstrbq_scatter_offset_s8(out + (stride << 2) + 4 * index, vstroffset, vtemp8);
    }
}
#else

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_1d_idct                                       */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Performs 1D Inverse Discrete Consine Transformation.  It is an      */
/*    implementation of LLM (Loeffler, Lighenberg, Moschytz) algorithm.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input_data                            Input data                    */
/*    output_data                           Buffer for output data        */
/*    post_scale                            Post scale value              */
/*    round                                 Value to reduce round error   */
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
/*    _gx_image_reader_jpeg_2d_idct                                       */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_jpeg_1d_idct(INT *input_data, INT *output_data, INT post_scale, INT round)
{
INT t;

    input_data[0] <<= 9;
    input_data[1] <<= 7;
    input_data[4] <<= 9;
    input_data[7] <<= 7;

    /* iDCT computation. */

    /* stage 1.  */
    BUTTERFLY_ADDITION(input_data[1], input_data[7]);

    input_data[3] *= R2_SHR7;
    input_data[5] *= R2_SHR7;

    /* stage 2.  */
    BUTTERFLY_ADDITION(input_data[0], input_data[4]);
    BUTTERFLY_MULTIPLICATION(input_data[6], input_data[2], C6R2_SHR9, S6R2_SHR9);
    BUTTERFLY_ADDITION(input_data[7], input_data[5]);
    BUTTERFLY_ADDITION(input_data[1], input_data[3]);

    /* staget 3.  */
    BUTTERFLY_ADDITION(input_data[0], input_data[6]);
    BUTTERFLY_ADDITION(input_data[4], input_data[2]);
    BUTTERFLY_MULTIPLICATION_SHR6(input_data[5], input_data[3], C1_SHR8, S1_SHR8);
    BUTTERFLY_MULTIPLICATION_SHR6(input_data[1], input_data[7], C3_SHR8, S3_SHR8);

    /* stage 4.  */
    input_data[0] += round;
    input_data[4] += round;
    input_data[2] += round;
    input_data[6] += round;

    output_data[0] = (input_data[0] + input_data[1]) >> post_scale;
    output_data[8] = (input_data[4] + input_data[5]) >> post_scale;
    output_data[16] = (input_data[2] + input_data[3]) >> post_scale;
    output_data[24] = (input_data[6] + input_data[7]) >> post_scale;
    output_data[32] = (input_data[6] - input_data[7]) >> post_scale;
    output_data[40] = (input_data[2] - input_data[3]) >> post_scale;
    output_data[48] = (input_data[4] - input_data[5]) >> post_scale;
    output_data[56] = (input_data[0] - input_data[1]) >> post_scale;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_dequantize_idct                               */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Dequatilizes decoded data and performs Inverse Discrete Consine     */
/*    Transformation.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    block                                 Pointer to decoded data       */
/*    quant_table                           Pointer to quantization table */
/*    out                                   Buffer for output data        */
/*    stride                                Stride of output data         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_jpeg_1d_idct         Perform 1D Inverse Discrete   */
/*                                            Consine Transformation      */
/*    GX_SATURATE_TO_BYTE                   Saturate to [-128, 127]       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_block_decode                              */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_jpeg_dequantize_idct(INT *block, INT *quant_table, GX_BYTE *out, INT stride)
{
static const GX_UBYTE reorder_index[] = {
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};
INT                      index;
INT                      jpeg_block[64];
INT                      temp_block[64];
INT                      row;

    for (index = 0; index < 64; index++)
    {
        block[index] *= quant_table[index];

        /* Reorder from zig-zag order to 8*8 block */
        jpeg_block[reorder_index[index]] = block[index];
    }

    for (index = 0; index < 8; index++)
    {
        _gx_image_reader_jpeg_1d_idct(jpeg_block + index * 8, temp_block + index, 9, 256);     /* row */
    }

    for (index = 0; index < 8; index++)
    {
        _gx_image_reader_jpeg_1d_idct(temp_block + index * 8, jpeg_block + index, 12, 2048);     /* col */
    }

    for (row = 0; row < 8; row++)
    {
        for (index = 0; index < 8; index++)
        {
            GX_SATURATE_TO_BYTE(out[index], jpeg_block[row * 8 + index]);
        }

        out += stride;
    }
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_block_decode                              */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Decode one block of JPEG data.                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG data control block       */
/*    icomponent                            Component index               */
/*    block_data                            Pointer to decoded data       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status Code                                                         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memset                                                              */
/*    _gx_image_reader_jpeg_dc_decode       Decode dc component           */
/*    _gx_image_reader_jpeg_ac_decode       Decode ac component           */
/*    _gx_image_reader_jpeg_dequantize_idct Dequatilize decoded data      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_decode                                        */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_jpeg_one_block_decode(GX_JPEG_INFO *jpeg_info, UINT i_component, GX_BYTE *block_data)
{
INT table_index;
INT stride;

    memset(jpeg_info->vecter, 0, 64 * sizeof(UINT));
    _gx_image_reader_jpeg_dc_decode(jpeg_info, i_component);
    _gx_image_reader_jpeg_ac_decode(jpeg_info, i_component);

    if (i_component >= JPG_MAX_COMPONENTS)
    {
        return GX_FAILURE;
    }

    stride = ((jpeg_info->sample_factor[i_component] & 0xf0) >> 1);

    if (i_component == 0)
    {
        if (stride > 32)
        {
            return GX_FAILURE;
        }
    }
    else
    {
        if (stride > 8)
        {
            return GX_FAILURE;
        }
    }

    table_index = jpeg_info->qantization_table_index[i_component];

    if (table_index >= JPG_QUANT_TABLE_DIMENSION)
    {
        return GX_FAILURE;
    }

    _gx_image_reader_jpeg_dequantize_idct(jpeg_info->vecter, jpeg_info->quantization_table[table_index], block_data, stride);
    return GX_SUCCESS;
}


#if defined(GX_ENABLE_ARM_HELIUM)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_24xrgb_pixel_write_helium                     */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Write 24xrgb pixel to memory using Helium intrinsics.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG control block            */
/*    vred                                  Red value vector              */
/*    vgreen                                Green value vector            */
/*    vblue                                 Blue value vector             */
/*    size                                  Number of pixels to write     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    vstrbq_scatter_offset_u8              Scatter 8-bit values to       */
/*                                            memory                      */
/*    vstrbq_scatter_offset_p_u8            Optionaly scatter 8-bit       */
/*                                            values to memory            */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_mcu_write                                 */
/*    _gx_image_reader_jpeg_one_mcu_rotated_write                         */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_jpeg_24xrgb_pixel_write_helium(GX_JPEG_INFO *jpeg_info, uint8x16_t vred, uint8x16_t vgreen, uint8x16_t vblue, INT size)
{
INT index;

    for(index = 0; index < size; index++)
    {
        *((GX_COLOR *)jpeg_info->putdata) = 0xff000000 | ((ULONG)vred[index] << 16) | ((ULONG)vgreen[index] << 8) | (ULONG)vblue[index];

        jpeg_info->putdata += 4;
    }
}
#else
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_1555xrgb_pixel_write                          */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Write 24xrgb pixel to memory.                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG control block            */
/*    red                                   Red value                     */
/*    green                                 Green value                   */
/*    blue                                  Blue value                    */
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
/*    _gx_image_reader_jpeg_one_mcu_write                                 */
/*    _gx_image_reader_jpeg_one_mcu_rotated_write                         */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_jpeg_24xrgb_pixel_write(GX_JPEG_INFO *jpeg_info, GX_UBYTE red, GX_UBYTE green, GX_UBYTE blue)
{
    *((GX_COLOR *)jpeg_info->putdata) = 0xff000000 | ((ULONG)red << 16) | ((ULONG)green << 8) | (ULONG)blue;

    jpeg_info->putdata += 4;
}
#endif

#if defined(GX_ENABLE_ARM_HELIUM)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_24bpp_pixel_write_helium                      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to write 24xrgb pixel to memory using      */
/*    Helium intrinsics.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG control block            */
/*    vred                                  Red value vector              */
/*    vgreen                                Green value vector            */
/*    vblue                                 Blue value vector            */
/*    size                                  Number of pixels to write     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    vstrbq_scatter_offset_u8              Scatter 8-bit values to       */
/*                                            memory                      */
/*    vstrbq_scatter_offset_p_u8            Optionally scatter 8-bit      */
/*                                            values to memory            */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_mcu_write                                 */
/*    _gx_image_reader_jpeg_one_mcu_rotated_write                         */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_jpeg_24bpp_pixel_write_helium(GX_JPEG_INFO *jpeg_info, uint8x16_t vred, uint8x16_t vgreen, uint8x16_t vblue, INT size)
{
static uint8x16_t voffset = {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45};
mve_pred16_t      p;

    if (size == 16)
    {
        vstrbq_scatter_offset_u8(jpeg_info->putdata++, voffset, vred);
        vstrbq_scatter_offset_u8(jpeg_info->putdata++, voffset, vgreen);
        vstrbq_scatter_offset_u8(jpeg_info->putdata++, voffset, vblue);
        jpeg_info->putdata += 45;
    }
    else
    {
        /* Write the specified size of RGB values to memory.  */
        p = 0xffff >> (16 - size);
        vstrbq_scatter_offset_p_u8(jpeg_info->putdata++, voffset, vred, p);
        vstrbq_scatter_offset_p_u8(jpeg_info->putdata++, voffset, vgreen, p);
        vstrbq_scatter_offset_p_u8(jpeg_info->putdata++, voffset, vblue, p);
        jpeg_info->putdata += (size - 1) * 3;
    }
}
#else
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_24bpp_pixel_write                             */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Write 24bpp pixel to memory.                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG control block            */
/*    red                                   Red value                     */
/*    green                                 Green value                   */
/*    blue                                  Blue value                    */
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
/*    _gx_image_reader_jpeg_one_mcu_write                                 */
/*    _gx_image_reader_jpeg_one_mcu_rotated_write                         */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_jpeg_24bpp_pixel_write(GX_JPEG_INFO *jpeg_info, GX_UBYTE red, GX_UBYTE green, GX_UBYTE blue)
{
    *jpeg_info->putdata++ = red;
    *jpeg_info->putdata++ = green;
    *jpeg_info->putdata++ = blue;
}
#endif

#if defined(GX_ENABLE_ARM_HELIUM)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_565rgb_pixel_write_helium                     */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to write 565rgb pixel to memory using      */
/*    Helium intrinsics.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG control block            */
/*    vred                                  Red value vector              */
/*    vgreen                                Green value vector            */
/*    vblue                                 Blue value vector            */
/*    size                                  Number of pixels to write     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    vshrq_n_u8                            Unsigned 8-bit shift right    */
/*    vldrbq_u16                            Load 8-bit value to a         */
/*                                            destination register        */
/*    vshlq_n_u16                           Unsigned 16-bit shift left    */
/*    vorrq_u16                             Unsigned 16-bit OR            */
/*    vstrhq_u16                            Store 16-bit values from      */
/*                                            register to memory          */
/*    vstrhq_p_u16                          Optionally store 16-bit       */
/*                                            values from register to     */
/*                                            memory                      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_mcu_write                                 */
/*    _gx_image_reader_jpeg_one_mcu_rotated_write                         */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_jpeg_565rgb_pixel_write_helium(GX_JPEG_INFO *jpeg_info, uint8x16_t vred, uint8x16_t vgreen, uint8x16_t vblue, INT size)
{
uint16x8_t   vresult;
uint16x8_t   vtemp;
INT          index;
mve_pred16_t p;
uint16_t    *put = (uint16_t *)jpeg_info->putdata;
GX_UBYTE     red[16];
GX_UBYTE     green[16];
GX_UBYTE     blue[16];

    vred = vshrq_n_u8(vred, 3);
    vgreen = vshrq_n_u8(vgreen, 2);
    vblue = vshrq_n_u8(vblue, 3);

    vstrbq(red, vred);
    vstrbq(green, vgreen);
    vstrbq(blue, vblue);

    for (index = 0; index <= 8; index += 8)
    {
        vtemp = vldrbq_u16(&red[index]);
        vresult = vshlq_n_u16(vtemp, 11);

        vtemp = vldrbq_u16(&green[index]);
        vtemp = vshlq_n_u16(vtemp, 5);
        vresult = vorrq_u16(vresult, vtemp);

        vtemp = vldrbq_u16(&blue[index]);
        vresult = vorrq_u16(vresult, vtemp);

        if (size >= 8)
        {
            vstrhq_u16(put, vresult);
            put += 8;
            size -= 8;
        }
        else
        {
            p = 0xffff >> (16 - (size << 1));
            vstrhq_p_u16(put, vresult, p);
            put += size;
            break;
        }
    }

    jpeg_info->putdata = (GX_UBYTE *)put;
}
#else
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_1555xrgb_pixel_write                          */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Write 1555xrgb pixel to memory.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG control block            */
/*    red                                   Red value                     */
/*    green                                 Green value                   */
/*    blue                                  Blue value                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    GX_SATURATE_TO_5BIT                   Saturate the value to 5 bits  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_mcu_write                                 */
/*    _gx_image_reader_jpeg_one_mcu_rotated_write                         */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_jpeg_565rgb_pixel_write(GX_JPEG_INFO *jpeg_info, GX_UBYTE red, GX_UBYTE green, GX_UBYTE blue)
{
    /* Make sure the range of the RGB values are within bound. */
    red >>= 3;
    green >>= 2;
    blue >>= 3;

    *((USHORT *)jpeg_info->putdata) = (USHORT)((red << 11) | (green << 5 | blue));
    jpeg_info->putdata += 2;
}
#endif

#if defined(GX_ENABLE_ARM_HELIUM)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_1555xrgb_pixel_write_helium                   */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to write 1555xrgb pixel to memory using    */
/*    Helium intrinsics.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG control block            */
/*    vred                                  Red value vector              */
/*    vgreen                                Green value vector            */
/*    vblue                                 Blue value vector            */
/*    size                                  Number of pixels to write     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    vshrq_n_u8                            Unsigned 8-bit shift right    */
/*    vldrbq_u16                            Load 8-bit value to a         */
/*                                            destination register        */
/*    vshlq_n_u16                           Unsigned 16-bit shift left    */
/*    vorrq_u16                             Unsigned 16-bit OR            */
/*    vstrhq_u16                            Store 16-bit values from      */
/*                                            register to memory          */
/*    vstrhq_p_u16                          Optionally store 16-bit       */
/*                                            values from register to     */
/*                                            memory                      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_mcu_write                                 */
/*    _gx_image_reader_jpeg_one_mcu_rotated_write                         */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_jpeg_1555xrgb_pixel_write_helium(GX_JPEG_INFO *jpeg_info, uint8x16_t vred, uint8x16_t vgreen, uint8x16_t vblue, INT size)
{
uint16x8_t   vresult;
uint16x8_t   vtemp;
INT          index;
uint16_t    *put = (uint16_t *)jpeg_info->putdata;
mve_pred16_t p;
GX_UBYTE     red[16];
GX_UBYTE     green[16];
GX_UBYTE     blue[16];

    vred = vshrq_n_u8(vred, 3);
    vgreen = vshrq_n_u8(vgreen, 3);
    vblue = vshrq_n_u8(vblue, 3);

    vstrbq(red, vred);
    vstrbq(green, vgreen);
    vstrbq(blue, vblue);

    for (index = 0; index <= 8; index += 8)
    {
        vtemp = vldrbq_u16(&red[index]);
        vresult = vshlq_n_u16(vtemp, 10);

        vtemp = vldrbq_u16(&green[index]);
        vtemp = vshlq_n_u16(vtemp, 5);
        vresult = vorrq_u16(vresult, vtemp);

        vtemp = vldrbq_u16(&blue[index]);
        vresult = vorrq_u16(vresult, vtemp);

        if (size >= 8)
        {
            vstrhq_u16(put, vresult);
            put += 8;
            size -= 8;
        }
        else
        {
            p = 0xffff >> (16 - (size << 1));
            vstrhq_p_u16(put, vresult, p);
            put += size;
            break;
        }
    }

    jpeg_info->putdata = (GX_UBYTE *)put;
}
#else
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_1555xrgb_pixel_write                          */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Write 1555xrgb pixel to memory.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG control block            */
/*    red                                   Red value                     */
/*    green                                 Green value                   */
/*    blue                                  Blue value                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    GX_SATURATE_TO_5BIT                   Saturate the value to 5 bits  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_mcu_write                                 */
/*    _gx_image_reader_jpeg_one_mcu_rotated_write                         */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_jpeg_1555xrgb_pixel_write(GX_JPEG_INFO *jpeg_info, GX_UBYTE red, GX_UBYTE green, GX_UBYTE blue)
{
    /* Make sure the range of the RGB values are within bound. */
    red >>= 3;
    green >>= 3;
    blue >>= 3;

    *((USHORT *)jpeg_info->putdata) = (USHORT)((red << 10) | (green << 5 | blue));
    jpeg_info->putdata += 2;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_pixel_write_info_set                          */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Set information for writing pixel to memory.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG control block            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status Code                                                         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_jpeg_565rgb_pixel_write_helium                     */
/*                                          Write 565rgb pixel to memory  */
/*    _gx_image_reader_jpeg_565rgb_pixel_write                            */
/*                                          Write 565rgb pixel to memory  */
/*    _gx_image_reader_jpeg_1555xrgb_pixel_write_helium                   */
/*                                          Write 1555xrgb pixel to memory*/
/*    _gx_image_reader_jpeg_1555xrgb_pixel_write                          */
/*                                          Write 1555xrgb pixel to memory*/
/*    _gx_image_reader_jpeg_24xrgb_24bpp_pixel_write_helium               */
/*                                          Write 24xrgb or 24rgb pixel   */
/*                                            to memory                   */
/*    _gx_image_reader_jpeg_24xrgb_24bpp_pixel_write                      */
/*                                          Write 24xrgb or 24rgb pixel   */
/*                                            to memory                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_decompress                                    */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_jpeg_pixel_write_info_set(GX_JPEG_INFO *jpeg_info)
{
    if (!jpeg_info->output_width)
    {
        jpeg_info->output_width = jpeg_info->width;
        jpeg_info->output_height = jpeg_info->height;

        _gx_utility_rectangle_define(&jpeg_info->output_clip, 0, 0, (GX_VALUE)(jpeg_info->width - 1), (GX_VALUE)(jpeg_info->height - 1));
    }

    jpeg_info->output_stride = jpeg_info->output_rotation_angle == 0 ? jpeg_info->output_width : jpeg_info->output_height;

    /* Set pixel write callback.  */
    switch (jpeg_info->output_color_format)
    {
    case GX_COLOR_FORMAT_565RGB:
#if defined(GX_ENABLE_ARM_HELIUM)
        jpeg_info->pixel_write_helium = _gx_image_reader_jpeg_565rgb_pixel_write_helium;
#else
        jpeg_info->pixel_write = _gx_image_reader_jpeg_565rgb_pixel_write;
#endif
        jpeg_info->output_bpp = 2;
        jpeg_info->output_stride <<= 1;
        break;

    case GX_COLOR_FORMAT_1555XRGB:
#if defined(GX_ENABLE_ARM_HELIUM)
        jpeg_info->pixel_write_helium = _gx_image_reader_jpeg_1555xrgb_pixel_write_helium;
#else
        jpeg_info->pixel_write = _gx_image_reader_jpeg_1555xrgb_pixel_write;
#endif
        jpeg_info->output_bpp = 2;
        jpeg_info->output_stride <<= 1;
        break;

    case GX_COLOR_FORMAT_32ARGB:
    case GX_COLOR_FORMAT_24XRGB:
#if defined(GX_ENABLE_ARM_HELIUM)
        jpeg_info->pixel_write_helium = _gx_image_reader_jpeg_24xrgb_pixel_write_helium;
#else
        jpeg_info->pixel_write = _gx_image_reader_jpeg_24xrgb_pixel_write;
#endif
        jpeg_info->output_bpp = 4;
        jpeg_info->output_stride <<= 2;
        break;

    case GX_IMAGE_FORMAT_24BPP:
    default:
#if defined(GX_ENABLE_ARM_HELIUM)
        jpeg_info->pixel_write_helium = _gx_image_reader_jpeg_24bpp_pixel_write_helium;
#else
        jpeg_info->pixel_write = _gx_image_reader_jpeg_24bpp_pixel_write;
#endif
        jpeg_info->output_bpp = 3;
        jpeg_info->output_stride = (jpeg_info->output_width * 3);
        jpeg_info->output_color_format = GX_IMAGE_FORMAT_24BPP;

        if (jpeg_info->output_buffer)
        {
            return GX_NOT_SUPPORTED;
        }
        break;
    }

    if (!jpeg_info->output_buffer)
    {
        jpeg_info->output_buffer = (GX_UBYTE *)_gx_system_memory_allocator((ULONG)(jpeg_info->height * jpeg_info->width * jpeg_info->output_bpp));
        if (!jpeg_info->output_buffer)
        {
            return GX_SYSTEM_MEMORY_ERROR;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_mcu_write                                 */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Write decoded data of one MCU block into specified memory.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG control block            */
/*    xpos                                  X position in image           */
/*    ypos                                  y position in image           */
/*    h                                     Horizontal sampling factor    */
/*    v                                     Vertical sampling factor      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status Code                                                         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    vldrbq_gather_offset_s8               Gather bytes from memory      */
/*    GX_JPEG_DECODE_YCBCR2RGB_HELIUM       Convert YCbCr to RGB          */
/*    GX_JPEG_DECODE_YCBCR2RGB              Convert YCbCr to RGB          */
/*    [gx_jpeg_pixel_write_helium]          Write pixel to memory         */
/*    [gx_jpeg_pixel_write]                 Write pixel to memory         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_decode                                        */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_jpeg_one_mcu_write(GX_JPEG_INFO *jpeg_info, INT xpos, INT ypos, INT h, INT v)
{
GX_UBYTE *put;
INT       x;
INT       y;
INT       coff;
INT       xstart = 0;
INT       xend;
INT       ystart = 0;
INT       yend;

#if defined(GX_ENABLE_ARM_HELIUM)
int8x16_t  vY;
int8x16_t  vCb;
int8x16_t  vCr;
GX_UBYTE   size;
uint8x16_t vred;
uint8x16_t vgreen;
uint8x16_t vblue;
GX_UBYTE   index;
#else
GX_BYTE    Y;
GX_BYTE    Cb;
GX_BYTE    Cr;
INT        red;
INT        green;
INT        blue;
#endif

    yend = (v << 3);
    xend = (h << 3);


    if (xpos < jpeg_info->output_clip.left)
    {
        xstart = jpeg_info->output_clip.left - xpos;
    }

    if (xpos + xend > jpeg_info->output_clip.right + 1)
    {
        xend = jpeg_info->output_clip.right + 1 - xpos;
    }

    if (ypos < jpeg_info->output_clip.top)
    {
        ystart = jpeg_info->output_clip.top - ypos;
    }

    if (ypos + yend > jpeg_info->output_clip.bottom + 1)
    {
        yend = jpeg_info->output_clip.bottom  + 1 - ypos;
    }

    put = (GX_UBYTE *)jpeg_info->output_buffer;
#if defined(GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER)
    put += (ypos + ystart - jpeg_info->output_buffer_offset_y) * jpeg_info->output_stride;
    put += (xpos + xstart - jpeg_info->output_buffer_offset_x) * jpeg_info->output_bpp;
#else
    put += (ypos + ystart) * jpeg_info->output_stride;
    put += (xpos + xstart) * jpeg_info->output_bpp;
#endif

#if defined(GX_ENABLE_ARM_HELIUM)
    index = (h == 1 ? 0 : (h - 1 + (xstart % h)));
#endif

    for (y = ystart; y < yend; y++)
    {
        jpeg_info->putdata = put;

#if defined(GX_ENABLE_ARM_HELIUM)
        for (x = xstart; x < xend; x += size)
        {
            size = xend - x;

            if (size > 16)
            {
                size = 16;
            }

            coff = x / h + ((y / v) << 3);

            vY = vldrbq_s8(jpeg_info->Y_block + x + y * h * 8);
            vCb = vldrbq_gather_offset_s8(jpeg_info->Cb_block + coff, _gx_jpeg_cbcr_offset_table[index]);
            vCr = vldrbq_gather_offset_s8(jpeg_info->Cr_block + coff, _gx_jpeg_cbcr_offset_table[index]);

            /* Convert YCbCr to RGB.  */
            GX_JPEG_DECODE_YCBCR2RGB_HELIUM(vred, vgreen, vblue, vY, vCb, vCr);

            jpeg_info->pixel_write_helium(jpeg_info, vred, vgreen, vblue, size);
        }
#else
        for (x = xstart; x < xend; x++)
        {
            coff = x / h + ((y / v) << 3);

            Y = jpeg_info->Y_block[x + y * h * 8];
            Cb = jpeg_info->Cb_block[coff];
            Cr = jpeg_info->Cr_block[coff];

            GX_JPEG_DECODE_YCBCR2RGB(red, green, blue, Y, Cb, Cr);

            GX_SATURATE_TO_UBYTE(red, red);
            GX_SATURATE_TO_UBYTE(green, green);
            GX_SATURATE_TO_UBYTE(blue, blue);

            jpeg_info->pixel_write(jpeg_info, (GX_UBYTE)red, (GX_UBYTE)green, (GX_UBYTE)blue);
        }
#endif

        put += jpeg_info->output_stride;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_mcu_rotated_write                         */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Write decoded data of one MCU block into specified memory.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG control block            */
/*    xpos                                  X position in image           */
/*    ypos                                  y position in image           */
/*    h                                     Horizontal sampling factor    */
/*    v                                     Vertical sampling factor      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status Code                                                         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    vldrbq_gather_offset_s8               Gather bytes from memory      */
/*    GX_JPEG_DECODE_YCBCR2RGB_HELIUM       Convert YCbCr to RGB          */
/*    GX_JPEG_DECODE_YCBCR2RGB              Convert YCbCr to RGB          */
/*    [gx_jpeg_pixel_write_helium]          Write pixel to memory         */
/*    [gx_jpeg_pixel_write]                 Write pixel to memory         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_decode                                        */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_jpeg_one_mcu_rotated_write(GX_JPEG_INFO *jpeg_info, INT xpos, INT ypos, INT h, INT v)
{
GX_UBYTE *put;
INT       x;
INT       coff;
INT       xstart = 0;
INT       xend;
INT       ystart = 0;
INT       yend;
INT       stride;

#if defined(GX_ENABLE_ARM_HELIUM)
int8x16_t  vY;
int8x16_t  vCb;
int8x16_t  vCr;
uint8x16_t vred;
uint8x16_t vgreen;
uint8x16_t vblue;
INT        size;
GX_UBYTE   index;
uint8x16_t yoffset;
uint8x16_t cbcroffset;
#else
GX_BYTE    Y;
GX_BYTE    Cb;
GX_BYTE    Cr;
INT        red;
INT        green;
INT        blue;
INT        y;
GX_BYTE    sign = 1;
#endif

    xend = (h << 3) - 1;
    yend = (v << 3) - 1;

    if (xpos < jpeg_info->output_clip.left)
    {
        xstart = jpeg_info->output_clip.left - xpos;
    }

    if (xpos + xend > jpeg_info->output_clip.right)
    {
        xend = jpeg_info->output_clip.right - xpos;
    }

    if (xstart > xend)
    {
        return GX_SUCCESS;
    }

    if (ypos < jpeg_info->output_clip.top)
    {
        ystart = jpeg_info->output_clip.top - ypos;
    }

    if (ypos + yend > jpeg_info->output_clip.bottom)
    {
        yend = jpeg_info->output_clip.bottom - ypos;
    }

    if (ystart > yend)
    {
        return GX_SUCCESS;
    }

#if defined(GX_ENABLE_ARM_HELIUM)
    size = yend - ystart + 1;
#endif

    stride = jpeg_info->output_stride;
    put = (GX_UBYTE *)jpeg_info->output_buffer;

    if (jpeg_info->output_rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        put += (jpeg_info->output_width - xpos - 1 - xstart) * stride;
        put += (ypos + ystart) * jpeg_info->output_bpp;
        stride = (-stride);
    }
    else
    {
        put += (xpos + xstart) * stride;
        put += (jpeg_info->output_height - ypos - 1 - yend) * jpeg_info->output_bpp;

#if defined(GX_ENABLE_ARM_HELIUM)
        ystart = (v << 3) - 1 - yend;
#else
        GX_SWAP_VALS(ystart, yend);
        sign = -1;
#endif
    }

#if defined(GX_ENABLE_ARM_HELIUM)
    if (jpeg_info->output_rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        index = (v == 2 ? (1 + (ystart % 2)) : 0);

        yoffset = _gx_jpeg_y_offset_rotated_table_cw[h >> 1];
        cbcroffset = _gx_jpeg_cbcr_offset_rotated_table_cw[index];
    }
    else
    {
        yoffset = _gx_jpeg_y_offset_rotated_table_ccw[h + v - 2];
        cbcroffset = _gx_jpeg_cbcr_offset_rotated_table_ccw[v - 1];

        if(ystart)
        {
            for (x = 0; x < 16 - ystart; x++)
            {
                yoffset[x] = yoffset[ystart + x];
                cbcroffset[x] = cbcroffset[ystart + x];
            }

            ystart = 0;
        }
    }
#endif

    for (x = xstart; x <= xend; x++)
    {
        jpeg_info->putdata = put;

#if defined(GX_ENABLE_ARM_HELIUM)
        coff = x / h + ((ystart / v) << 3);

        vY = vldrbq_gather_offset_s8(jpeg_info->Y_block + x + ystart * h * 8, yoffset);
        vCb = vldrbq_gather_offset_s8(jpeg_info->Cb_block + coff, cbcroffset);
        vCr = vldrbq_gather_offset_s8(jpeg_info->Cr_block + coff, cbcroffset);

        /* Convert YCbCr to RGB.  */
        GX_JPEG_DECODE_YCBCR2RGB_HELIUM(vred, vgreen, vblue, vY, vCb, vCr);

        jpeg_info->pixel_write_helium(jpeg_info, vred, vgreen, vblue, size);

#else
        for (y = ystart; y != yend + sign; y += sign)
        {
            coff = x / h + ((y / v) << 3);

            Y = jpeg_info->Y_block[x + y * h * 8];
            Cb = jpeg_info->Cb_block[coff];
            Cr = jpeg_info->Cr_block[coff];

            GX_JPEG_DECODE_YCBCR2RGB(red, green, blue, Y, Cb, Cr);

            GX_SATURATE_TO_UBYTE(red, red);
            GX_SATURATE_TO_UBYTE(green, green);
            GX_SATURATE_TO_UBYTE(blue, blue);

            jpeg_info->pixel_write(jpeg_info, (GX_UBYTE)red, (GX_UBYTE)green, (GX_UBYTE)blue);
        }
#endif
        put += stride;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_decompress                                    */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Decompress JPG data stream.                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             JPEG control block            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status Code                                                         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_jpeg_one_block_decode                              */
/*                                          Decode one blcok of jpeg data */
/*    _gx_image_reader_jpeg_one_mcu_write   Write decoded data to         */
/*                                            specified memory            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_decode                                        */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_jpeg_decompress(GX_JPEG_INFO *jpeg_info)
{
int  h;
int  v;
int  x;
int  y;
int  xx;
int  yy;
UINT status = GX_SUCCESS;
UINT (*one_mcu_write)(GX_JPEG_INFO *jpeg_info, INT xpos, INT ypos, INT h, INT v);

    h = (jpeg_info->sample_factor[0] >> 4);
    v = (jpeg_info->sample_factor[0] & 0x0f);

    if (v > 2)
    {
        return GX_INVALID_FORMAT;
    }

    status = _gx_image_reader_jpeg_pixel_write_info_set(jpeg_info);

    if ((jpeg_info->output_rotation_angle != 0) &&
        (jpeg_info->output_color_format != GX_IMAGE_FORMAT_24BPP))
    {
        one_mcu_write = _gx_image_reader_jpeg_one_mcu_rotated_write;
    }
    else
    {
        one_mcu_write = _gx_image_reader_jpeg_one_mcu_write;
    }

    for (y = 0; y < jpeg_info->height; y += 8 * v)
    {
        for (x = 0; x < jpeg_info->width; x += 8 * h)
        {
            /* Decode one MCU */
            for (yy = 0; yy < v && status == GX_SUCCESS; yy++)
            {
                for (xx = 0; xx < h && status == GX_SUCCESS; xx++)
                {
                    /* Y */
                    status = _gx_image_reader_jpeg_one_block_decode(jpeg_info, 0, jpeg_info->Y_block + yy * 128 + xx * 8);
                }
            }

            if (status == GX_SUCCESS && jpeg_info->num_of_components > 1)
            {
                /* Cb */
                status = _gx_image_reader_jpeg_one_block_decode(jpeg_info, 1, jpeg_info->Cb_block);

                /* Cr */
                if (status == GX_SUCCESS)
                {
                    status = _gx_image_reader_jpeg_one_block_decode(jpeg_info, 2, jpeg_info->Cr_block);
                }
            }

            if (status == GX_SUCCESS)
            {
                one_mcu_write(jpeg_info, jpeg_info->output_xpos + x, jpeg_info->output_ypos + y, h, v);
            }
        }
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_decode_blocks                                 */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function decode a jpg format image.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    jpeg_info                             Jpeg decode control block     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_jpeg_quantization_table_set                        */
/*                                          Set jpeg quantization table   */
/*    _gx_image_reader_jpeg_frame_header_read                             */
/*                                          Read frame header information */
/*    _gx_image_reader_huffman_table_set    Set up huffman table          */
/*    _gx_image_reader_jpeg_scan_header_read                              */
/*                                          Read scan header information  */
/*    _gx_image_reader_jpeg_decompress      Decompress jped data stream   */
/*    _gx_system_memory_free                Application defined memory    */
/*                                            free function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_jpeg_decode                                        */
/*    _gx_image_reader_jpeg_mcu_decode                                    */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_jpeg_decode_blocks(GX_JPEG_INFO *jpeg_info)
{
GX_UBYTE *jpeg_data;
GX_UBYTE  marker;
UINT      segment_len;
UINT      status = GX_SUCCESS;

    if (jpeg_info->data == GX_NULL || jpeg_info->data_size < 10)
    {
        return GX_INVALID_VALUE;
    }

    jpeg_data = jpeg_info->data;

    /* Read JPEG File flag that mark the start of a JPEG image. */
    if ((*jpeg_data++ != 0xff) || (*jpeg_data++ != 0xd8))
    {
        return GX_INVALID_FORMAT; /*Not a jpeg file */
    }

    jpeg_info->data_index += 2;

    while ((jpeg_info->data_index + 3 < jpeg_info->data_size) &&
           (status == GX_SUCCESS))
    {
        jpeg_data = (jpeg_info->data + jpeg_info->data_index);
        marker = *(jpeg_data + 1);

        if ((*jpeg_data == 0xff) && (marker != 0) && (marker != 0xff))
        {
            jpeg_data += 2;
            jpeg_info->data_index += 2;

            /* Read WORD length */
            segment_len = *(jpeg_data);
            segment_len <<= 8;
            segment_len |= *(jpeg_data + 1);

            if ((UINT)jpeg_info->data_index + segment_len > (UINT)jpeg_info->data_size)
            {
                /* Invalid data size. */
                status = GX_FAILURE;
                break;
            }

            switch (marker)
            {
            case 0xdb:
                /* Define Quantization Table */
                status = _gx_image_reader_jpeg_quantization_table_set(jpeg_info, segment_len);
                break;

            case 0xc0:
                /* Start of Frame */
                status = _gx_image_reader_jpeg_frame_header_read(jpeg_info, segment_len);
                break;

            case 0xc4:
                /* Define Huffman Table */
                status = _gx_image_reader_huffman_table_set(jpeg_info, segment_len);
                break;

            case 0xdd:
                /* Read restart interval which specifies the number of MCU in restart interval. */
                jpeg_data += 2;
                jpeg_info->restart_interval = *jpeg_data++;
                jpeg_info->restart_interval <<= 8;
                jpeg_info->restart_interval |= *jpeg_data++;
                jpeg_info->data_index += (INT)segment_len;
                break;

            case 0xda:
                /* Start of Scan, stores which Huffman tables are associated with which components
                   The program start decoding the data section directly after it reads in this header. */
                _gx_image_reader_jpeg_scan_header_read(jpeg_info, segment_len);

                /* Start decoding jpeg data stream. */
                status = _gx_image_reader_jpeg_decompress(jpeg_info);
                break;

            default:
                /* Unkown marker, skip */
                jpeg_info->data_index += (INT)segment_len;
                break;
            }
        }
        else
        {
            jpeg_info->data_index++;
        }
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_decode                                        */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function decodes a jpg format image and saves the decoded data */
/*    to a GX_PIXELMAP structure.                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    read_data                             Input JPEG data stream        */
/*    data_size                             JPEG size in bytes            */
/*    outmap                                Output pixelmap               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_jpeg_decode_blocks   Decode a jpeg format image    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_image_decode                                       */
/*                                                                        */
/**************************************************************************/
UINT _gx_image_reader_jpeg_decode(GX_IMAGE_READER *image_reader, GX_PIXELMAP *outmap)
{
UINT          status;
GX_JPEG_INFO *jpeg_info;


    if ((!_gx_system_memory_allocator) || (!_gx_system_memory_free))
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    jpeg_info = (GX_JPEG_INFO *)_gx_system_memory_allocator(sizeof(GX_JPEG_INFO));

    if (!jpeg_info)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    memset(jpeg_info, 0, sizeof(GX_JPEG_INFO));


    jpeg_info->data = (GX_UBYTE *)image_reader->source_data;
    jpeg_info->data_size = (INT)image_reader->source_data_size;
    jpeg_info->data_index = 0;

    if (image_reader->mode & GX_IMAGE_READER_MODE_DITHER)
    {
        jpeg_info->output_color_format  = GX_IMAGE_FORMAT_24BPP;
    }
    else
    {
        jpeg_info->output_color_format  = image_reader->color_format;
    }

    if (image_reader->mode & GX_IMAGE_READER_MODE_ROTATE_CW)
    {
        jpeg_info->output_rotation_angle =  GX_SCREEN_ROTATION_CW;
    }
    else if (image_reader->mode & GX_IMAGE_READER_MODE_ROTATE_CCW)
    {
        jpeg_info->output_rotation_angle =  GX_SCREEN_ROTATION_CCW;
    }

    status = _gx_image_reader_jpeg_decode_blocks(jpeg_info);

    if (status == GX_SUCCESS)
    {
        outmap->data = jpeg_info->output_buffer;
        outmap->data_size = (ULONG)(jpeg_info->output_stride * jpeg_info->height);
        outmap->width = (GX_VALUE)jpeg_info->width;
        outmap->height = (GX_VALUE)jpeg_info->height;
        outmap->flags = 0;
        outmap->format = jpeg_info->output_color_format;
    }

    _gx_system_memory_free((void *)jpeg_info);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_jpeg_mcu_decode                                    */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function decodes a jpg format image and draw it to canvas      */
/*    directly.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    read_data                             Input JPEG data               */
/*    data_size                             JPEG size in bytes            */
/*    context                               Drawing contex                */
/*    xpos                                  X-coord of draw start point in*/
/*                                            canvas                      */
/*    ypos                                  Y-coord of draw start point in*/
/*                                            canvas                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_jpeg_decode_blocks   Decode a jpeg format image    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_image_reader_jpeg_mcu_decode(const GX_UBYTE *read_data, ULONG data_size,
                                      GX_DRAW_CONTEXT *context, INT xpos, INT ypos)
{
UINT          status;
GX_JPEG_INFO *jpeg_info;

    if ((!_gx_system_memory_allocator) || (!_gx_system_memory_free))
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    jpeg_info = (GX_JPEG_INFO *)_gx_system_memory_allocator(sizeof(GX_JPEG_INFO));

    if (!jpeg_info)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    if (!context)
    {
        return GX_INVALID_CONTEXT;
    }

    memset(jpeg_info, 0, sizeof(GX_JPEG_INFO));

    jpeg_info->data = (GX_UBYTE *)read_data;
    jpeg_info->data_size = (INT)data_size;
    jpeg_info->data_index = 0;
    jpeg_info->output_xpos = xpos;
    jpeg_info->output_ypos = ypos;
    jpeg_info->output_color_format = context->display->color_format;
    jpeg_info->output_rotation_angle = context->display->rotation_angle;
    jpeg_info->output_buffer = (GX_UBYTE *)context->memory;
#ifdef GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
    jpeg_info->output_width = (USHORT)context->canvas->memory_width;
    jpeg_info->output_height = (USHORT)context->canvas->memory_height;
    jpeg_info->output_buffer_offset_x = context->offset_x;
    jpeg_info->output_buffer_offset_y = context->offset_y;
#else
    jpeg_info->output_width = (USHORT)context->canvas->x_resolution;
    jpeg_info->output_height = (USHORT)context->canvas->y_resolution;
#endif
    jpeg_info->output_clip = *context->clip;


    status = _gx_image_reader_jpeg_decode_blocks(jpeg_info);

    _gx_system_memory_free((void *)jpeg_info);

    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_nearest_palette_color_get                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves the nearest palette color.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    want_color                            Wanted color value.           */
/*    index                                 Retrieved color index.        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
UINT _gx_image_reader_nearest_palette_color_get(GX_IMAGE_READER *image_reader, GX_PIXEL *want_color, INT *index)
{
ULONG     nearest_dist = 0x7fffffff;
ULONG     dist;
GX_UBYTE  red_dist;
GX_UBYTE  green_dist;
GX_UBYTE  blue_dist;
UINT      pal_index;
INT       nearest_index = 0;
GX_COLOR *palette = image_reader->palette;
UINT      palsize = image_reader->palette_size;
GX_PIXEL  pal_color;

    if (palette)
    {
        for (pal_index = 0; pal_index < palsize; pal_index++)
        {
            pal_color.red = (GX_UBYTE)(palette[pal_index] >> 16);
            pal_color.green = (GX_UBYTE)(palette[pal_index] >> 8);
            pal_color.blue = (GX_UBYTE)palette[pal_index];

            red_dist = (GX_UBYTE)GX_ABS(want_color->red - pal_color.red);
            green_dist = (GX_UBYTE)GX_ABS(want_color->green - pal_color.green);
            blue_dist = (GX_UBYTE)GX_ABS(want_color->blue - pal_color.blue);

            dist = (ULONG)((red_dist * red_dist) + (green_dist * green_dist) + (blue_dist * blue_dist));

            if (dist < nearest_dist)
            {
                nearest_dist = dist;
                nearest_index = (INT)pal_index;
            }
        }

        want_color->red = (GX_UBYTE)(palette[nearest_index] >> 16);
        want_color->green = (GX_UBYTE)(palette[nearest_index] >> 8);
        want_color->blue = (GX_UBYTE)palette[nearest_index];

        *index = nearest_index;
    }
    else
    {
        want_color->red = 0;
        want_color->green = 0;
        want_color->blue = 0;
    }

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_palette_set                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets up palette for the image reader.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pal                                   Pointer to palette.           */
/*    palsize                               The size of palette.          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
UINT _gx_image_reader_palette_set(GX_IMAGE_READER *image_reader, GX_COLOR *pal, UINT palsize)
{
    image_reader->palette = pal;
    image_reader->palette_size = palsize;

    return GX_SUCCESS;
}
#endif

#if defined(GX_SOFTWARE_DECODER_SUPPORT)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_8bit_alpha_read                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads 8bit alpha value from input pixelmap data.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_8bit_alpha_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;

    pLine = image_reader->getdata;
    pLine += index;

    pixel->red = 0;
    pixel->green = 0;
    pixel->blue = 0;
    pixel->alpha = *pLine;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_1bit_pixel_read                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_1bit_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;
GX_COLOR *pal;
UINT      palindex;

    pLine = image_reader->getdata;
    pLine += (index >> 3);

    palindex = *pLine;
    palindex >>= (7 - (index & 0x7));
    palindex &= 0x01;

    if ((image_reader->png_trans) &&
        (palindex == image_reader->png_trans[0]))
    {
        /* Transparent */
        pixel->alpha = 0;
        pixel->red = 0;
        pixel->green = 0;
        pixel->blue = 0;
    }
    else
    {
        pal = (GX_COLOR *)image_reader->png_palette;

        if (pal)
        {
            /* Palette */
            if (palindex < image_reader->png_palette_size)
            {
                pixel->red = (GX_UBYTE)(pal[palindex] >> 16);
                pixel->green = (GX_UBYTE)(pal[palindex] >> 8);
                pixel->blue = (GX_UBYTE)pal[palindex];
                pixel->alpha = 0xff;
            }
            else
            {
                memset(pixel, 0, sizeof(GX_PIXEL));
            }
        }
        else
        {
            /* Gray */
            palindex *= 255;
            pixel->red = (GX_UBYTE)palindex;
            pixel->green = (GX_UBYTE)palindex;
            pixel->blue = (GX_UBYTE)palindex;
            pixel->alpha = 0xff;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_2bit_pixel_read                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_2bit_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;
GX_COLOR *pal;
UINT      palindex;

    pLine = image_reader->getdata;
    pLine += (index >> 2);

    palindex = *pLine;
    palindex >>= (3 - (index & 0x3)) * 2;
    palindex &= 0x03;

    if ((image_reader->png_trans) &&
        (palindex == image_reader->png_trans[0]))
    {
        /* Transparent */
        pixel->alpha = 0;
        pixel->red = 0;
        pixel->green = 0;
        pixel->blue = 0;
    }
    else
    {
        pal = image_reader->png_palette;

        if (pal)
        {
            /* Palette */
            if (palindex < image_reader->png_palette_size)
            {
                pixel->red = (GX_UBYTE)(pal[palindex] >> 16);
                pixel->green = (GX_UBYTE)(pal[palindex] >> 8);
                pixel->blue = (GX_UBYTE)pal[palindex];
                pixel->alpha = 0xff;
            }
            else
            {
                memset(pixel, 0, sizeof(GX_PIXEL));
            }
        }
        else
        {
            /* Gray */
            palindex = palindex * 255 / 3;
            pixel->red = (GX_UBYTE)palindex;
            pixel->green = (GX_UBYTE)palindex;
            pixel->blue = (GX_UBYTE)palindex;
            pixel->alpha = 0xff;
        }
    }

    return GX_SUCCESS;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_4bit_pixel_read                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_4bit_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;
GX_COLOR *pal;
UINT      palindex;

    pLine = image_reader->getdata;
    pLine += (index >> 1);

    palindex = *pLine;
    palindex >>= (1 - (index & 0x01)) * 4;
    palindex &= 0x0f;

    if ((image_reader->png_trans) &&
        (palindex == image_reader->png_trans[0]))
    {
        /* Transparent */
        pixel->alpha = 0;
        pixel->red = 0;
        pixel->green = 0;
        pixel->blue = 0;
    }
    else
    {
        pal = image_reader->png_palette;

        if (pal)
        {
            /* Pixel */
            if (palindex < image_reader->png_palette_size)
            {
                pixel->red = (GX_UBYTE)(pal[palindex] >> 16);
                pixel->green = (GX_UBYTE)(pal[palindex] >> 8);
                pixel->blue = (GX_UBYTE)pal[palindex];
                pixel->alpha = 0xff;
            }
            else
            {
                memset(pixel, 0, sizeof(GX_PIXEL));
            }
        }
        else
        {
            /* Gray */
            palindex = palindex * 255 / 15;
            pixel->red = (GX_UBYTE)palindex;
            pixel->green = (GX_UBYTE)palindex;
            pixel->blue = (GX_UBYTE)palindex;
            pixel->alpha = 0xff;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_8bit_pixel_read                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_8bit_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_COLOR *palette;
UINT      palsize;
UINT      palindex;

    palindex = *(image_reader->getdata + index);

    if ((image_reader->png_trans) &&
        (palindex == image_reader->png_trans[0]))
    {
        /* Transparent */
        pixel->alpha = 0;
        pixel->red = 0;
        pixel->green = 0;
        pixel->blue = 0;
    }
    else
    {
        palette = image_reader->png_palette;
        palsize = image_reader->png_palette_size;

        if (palette)
        {
            /* Palette */
            if (palindex < palsize)
            {
                pixel->red = (GX_UBYTE)(palette[palindex] >> 16);
                pixel->green = (GX_UBYTE)(palette[palindex] >> 8);
                pixel->blue = (GX_UBYTE)palette[palindex];
                pixel->alpha = 0xff;
            }
            else
            {
                memset(pixel, 0, sizeof(GX_PIXEL));
            }
        }
        else
        {
            /* Gray */
            pixel->red = (GX_UBYTE)palindex;
            pixel->green = (GX_UBYTE)palindex;
            pixel->blue = (GX_UBYTE)palindex;
            pixel->alpha = 0xff;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_8bit_palette_pixel_read                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_8bit_palette_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_COLOR *palette = image_reader->palette;
UINT      palindex;

    palindex = *(image_reader->getdata + index);

    if (palette)
    {
        /* Palette */
        if ((image_reader->mode & GX_IMAGE_READER_MODE_ALPHA) &&
            (GX_TRANSPARENT_COLOR == palindex))
        {
            pixel->alpha = 0;
            pixel->red = 0;
            pixel->green = 0;
            pixel->blue = 0;
        }
        else
        {
            pixel->red = (GX_UBYTE)(palette[palindex] >> 16);
            pixel->green = (GX_UBYTE)(palette[palindex] >> 8);
            pixel->blue = (GX_UBYTE)palette[palindex];
            pixel->alpha = 0xff;
        }
    }
    else
    {
        memset(pixel, 0, sizeof(GX_PIXEL));
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_16bit_gray_alpha_read                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_16bit_gray_alpha_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;

    pLine = image_reader->getdata;
    pLine += (index << 1);

    /* Trans is prohibited for this color types, since a full alpha channel is already present. */
    pixel->red = *pLine;
    pixel->green = *pLine;
    pixel->blue = *pLine++;
    pixel->alpha = *pLine;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_32bit_gray_alpha_read                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_32bit_gray_alpha_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;

    pLine = image_reader->getdata;
    pLine += (index << 2);

    pixel->red = *pLine;
    pixel->green = pixel->red;
    pixel->blue = pixel->red;
    pLine += 2;
    pixel->alpha = (GX_UBYTE)*pLine;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_24bit_pixel_read                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_24bit_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;
GX_COLOR *pTrans = image_reader->png_trans;

    pLine = image_reader->getdata;
    pLine += index * 3;

    pixel->red = *pLine++;
    pixel->green = *pLine++;
    pixel->blue = *pLine;

    if ((pTrans) &&
        (pixel->red == pTrans[0]) &&
        (pixel->green == pTrans[1]) &&
        (pixel->blue == pTrans[2]))
    {
        pixel->alpha = 0;
    }
    else
    {
        pixel->alpha = 0xff;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_48bit_pixel_read                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_48bit_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;
GX_COLOR *pTrans = image_reader->png_trans;

    pLine = image_reader->getdata;
    pLine += index * 6;

    pixel->red = *pLine;
    pLine += 2;
    pixel->green = *pLine;
    pLine += 2;
    pixel->blue = *pLine;

    if ((pTrans) &&
        (pixel->red == (GX_UBYTE)pTrans[0]) &&
        (pixel->green == (GX_UBYTE)pTrans[1]) &&
        (pixel->blue == (GX_UBYTE)pTrans[2]))
    {
        pixel->alpha = 0;
    }
    else
    {
        pixel->alpha = 0xff;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_64bit_pixel_read                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_64bit_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;

    pLine = image_reader->getdata;
    pLine += (index << 3);

    pixel->red = *pLine;
    pLine += 2;
    pixel->green = *pLine;
    pLine += 2;
    pixel->blue = *pLine;
    pLine += 2;
    pixel->alpha = *pLine;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_32argb_pixel_read                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_32argb_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_COLOR *pLine;

    pLine = (GX_COLOR *)image_reader->getdata;
    pLine += index;

    pixel->alpha = (GX_UBYTE)((*pLine) >> 24);
    pixel->red = (GX_UBYTE)((*pLine) >> 16);
    pixel->green = (GX_UBYTE)((*pLine) >> 8);
    pixel->blue = (GX_UBYTE)(*pLine);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_32bit_pixel_read                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_32bit_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE *pget;

    pget = (GX_UBYTE *)image_reader->getdata;
    pget += (index << 2);

    pixel->red = *pget++;
    pixel->green = *pget++;
    pixel->blue = *pget++;
    pixel->alpha = *pget++;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_16bit_gray_read                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_16bit_gray_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE *pget;

    pget = (GX_UBYTE *)image_reader->getdata;
    pget += (index << 1);

    if ((image_reader->png_trans) &&
        ((*pget) == (GX_UBYTE)image_reader->png_trans[0]))
    {
        pixel->alpha = 0;
        pixel->red = 0;
        pixel->green = 0;
        pixel->blue = 0;
    }
    else
    {
        pixel->red = *pget;
        pixel->green = *pget;
        pixel->blue = *pget;
        pixel->alpha = 0xff;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_565rgb_pixel_read                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads a pixel from input pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_565rgb_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
USHORT   *pLine;
GX_UBYTE *pAlpha;

    pLine = (USHORT *)image_reader->getdata;
    pLine += index;

    pixel->red = (GX_UBYTE)(((*pLine) & 0xf800) >> 8);
    pixel->green = (GX_UBYTE)(((*pLine) & 0x07e0) >> 3);
    pixel->blue = (GX_UBYTE)(((*pLine) & 0x001f) << 3);

    if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
    {
        pAlpha = (GX_UBYTE *)image_reader->getauxdata;
        pAlpha += index;

        pixel->alpha = (*pAlpha);
    }
    else
    {
        pixel->alpha = 0;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_1555xrgb_pixel_read                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads an 1555xrgb format pixel from input pixelmap    */
/*    data structure.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_1555xrgb_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
USHORT   *pLine;
GX_UBYTE *pAlpha;

    pLine = (USHORT *)image_reader->getdata;
    pLine += index;

    pixel->red = (GX_UBYTE)(((*pLine) & 0x7c00) >> 7);
    pixel->green = (GX_UBYTE)(((*pLine) & 0x03e0) >> 2);
    pixel->blue = (GX_UBYTE)(((*pLine) & 0x001f) << 3);

    if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
    {
        pAlpha = (GX_UBYTE *)image_reader->getauxdata;
        pAlpha += index;

        pixel->alpha = (*pAlpha);
    }
    else
    {
        pixel->alpha = 0;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_4444argb_pixel_read                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads an 4444argb format pixel from input pixelmap    */
/*    data structure.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_4444argb_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
USHORT        color;
const USHORT *data = (USHORT *)(image_reader->getdata);

    color = *(data + index);

    pixel->alpha = (GX_UBYTE)((color & 0xf000) >> 8);
    pixel->red = (GX_UBYTE)((color & 0xf00) >> 4);
    pixel->green = (GX_UBYTE)(color & 0xf0);
    pixel->blue = (GX_UBYTE)((color & 0xf) << 4);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_4bit_grayscale_pixel_read                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads an 4bit grayscale format pixel (no transparency)*/
/*    from input pixelmap data structure.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_4bit_grayscale_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE color;

    color = *(image_reader->getdata + (index >> 1));

    if (index & 1)
    {
        color &= 0x0f;
    }
    else
    {
        color &= 0xf0;
        color >>= 4;
    }

    color |= (GX_UBYTE)(color << 4);
    pixel->red = color;
    pixel->green = color;
    pixel->blue = color;
    pixel->alpha = 0xff;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_4bit_grayscale_transparent_read                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads an 4bit grayscale format pixel                  */
/*    (with transparency)from input pixelmap data structure.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_4bit_grayscale_transparent_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
const GX_UBYTE *data = image_reader->getdata;
const GX_UBYTE *auxdata = image_reader->getauxdata;
INT             aux_index = (index >> 3);
INT             color_index = (index >> 1);
GX_UBYTE        color = *(data + color_index);
GX_UBYTE        transparency = *(auxdata + aux_index);

    /* Get transparent mask. */
    transparency = (GX_UBYTE)(transparency >> (7 - (index & 0x07)));

    if (transparency & 0x01)
    {
        color = 0;
        pixel->alpha = 0;
    }
    else
    {
        if (index & 1)
        {
            color &= 0xf;
        }
        else
        {
            color &= 0xf0;
            color >>= 4;
        }
        color = (GX_UBYTE)(color | color << 4);
        pixel->alpha = 0xff;
    }

    pixel->red = color;
    pixel->green = color;
    pixel->blue = color;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_1bpp_pixel_read                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads an 1bpp format pixel (no transparency) from     */
/*    input pixelmap data structure.                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_1bpp_pixel_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE color;

    color = *(image_reader->getdata + (index >> 3));

    if (color & (0x80 >> (index & 0x07)))
    {
        pixel->red = 0xff;
        pixel->green = 0xff;
        pixel->blue = 0xff;
    }
    else
    {
        pixel->red = 0;
        pixel->green = 0;
        pixel->blue = 0;
    }
    pixel->alpha = 0xff;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_1bpp_transparent_read                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads an 1bpp format pixel (no transparency) from     */
/*    input pixelmap data structure.                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block     */
/*    index                                Row index.                     */
/*    pixel                                Retrieved pixel.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_1bpp_transparent_read(GX_IMAGE_READER *image_reader, INT index, GX_PIXEL *pixel)
{
GX_UBYTE color;

    color = *(image_reader->getdata + (index >> 2));

    if (color & (0x40 >> ((index & 0x03) << 1)))
    {
        if (color & (0x80 >> ((index & 0x03) << 1)))
        {
            pixel->red = 0xff;
            pixel->green = 0xff;
            pixel->blue = 0xff;
        }
        else
        {
            pixel->red = 0;
            pixel->green = 0;
            pixel->blue = 0;
        }
        pixel->alpha = 0xff;
    }
    else
    {
        pixel->red = 0;
        pixel->green = 0;
        pixel->blue = 0;
        pixel->alpha = 0;
    }

    return GX_SUCCESS;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets pixel read callback of the image reader.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block    */
/*    inmap                                 Input pixelmap                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_1bit_pixel_read      Read 1bit internal format     */
/*                                            pixel                       */
/*    _gx_image_reader_2bit_pixel_read      Read 2bit internal format     */
/*                                            pixel                       */
/*    _gx_image_reader_4bit_pixel_read      Read 4bit internal format     */
/*                                            pixel                       */
/*    _gx_image_reader_8bit_pixel_read      Read 8bit internal format     */
/*                                            pixel                       */
/*    _gx_image_reader_16bit_gray_read      Read 16bit internal format    */
/*                                            pixel                       */
/*    _gx_image_reader_16bit_gray_alpha_read                              */
/*                                          Read 16bit gray:alpha internal*/
/*                                            format pixel                */
/*    _gx_image_reader_32bit_gray_alpha_read                              */
/*                                          Read 32bit gray:alpha internal*/
/*                                            format pixel                */
/*    _gx_image_reader_24bit_pixel_read     Read 24bit internal format    */
/*                                            pixel                       */
/*    _gx_image_reader_32bit_pixel_read     Read 32bit internal format    */
/*                                            pixel                       */
/*    _gx_image_reader_48bit_pixel_read     Read 48bit internal format    */
/*                                            pixel                       */
/*    _gx_image_reader_64bit_pixel_read     Read 64bit internal format    */
/*                                            pixel                       */
/*    _gx_image_reader_32argb_pixel_read    Read 32argb guix format pixel */
/*                                            pixel                       */
/*    _gx_image_reader_565rgb_pixel_read    Read 565rgb guix format pixel */
/*    _gx_image_reader_1555xrgb_pixel_read  Read 1555xrgb guix format     */
/*                                            pixel                       */
/*    _gx_image_reader_8bit_palette_pixel_read                            */
/*                                          Read 8bit palette guix format */
/*                                            pixel                       */
/*    _gx_image_reader_8bit_alpha_read      Read 8bit alphamap guix format*/
/*                                            pixel                       */
/*    _gx_image_reader_4444argb_pixel_read  Read 4444argb guix format     */
/*                                            pixel                       */
/*    _gx_image_reader_4bit_grayscale_transparent_read                    */
/*                                          Read 4bit grayscale guix      */
/*                                            format pixel with           */
/*                                            transparency                */
/*    _gx_image_reader_4bit_grayscale_pixel_read                          */
/*                                          Read 4bit grayscale guix      */
/*                                            format pxiel                */
/*    _gx_image_reader_1bpp_transparent_read                              */
/*                                          Read 1bpp guix format pixel   */
/*                                            with transparency           */
/*    _gx_image_reader_1bpp_pixel_read      Read 1bpp guix format pixel   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    gx_image_reader_colorspace_convert                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_image_reader_pixel_read_callback_set(GX_IMAGE_READER *image_reader, GX_PIXELMAP *inmap)
{

    switch (inmap->format)
    {
    case GX_IMAGE_FORMAT_1BPP:
        if (inmap->aux_data)
        {
            image_reader->png_palette = (GX_COLOR *)inmap->aux_data;
            image_reader->png_palette_size = inmap->aux_data_size / sizeof(GX_COLOR);
        }
        image_reader->pixel_read = _gx_image_reader_1bit_pixel_read;
        image_reader->input_stride = (UINT)(inmap->width >> 3);
        break;

    case GX_IMAGE_FORMAT_2BPP:
        if (inmap->aux_data)
        {
            image_reader->png_palette = (GX_COLOR *)inmap->aux_data;
            image_reader->png_palette_size = inmap->aux_data_size / sizeof(GX_COLOR);
        }
        image_reader->pixel_read = _gx_image_reader_2bit_pixel_read;
        image_reader->input_stride = (UINT)(inmap->width >> 2);
        break;

    case GX_IMAGE_FORMAT_4BPP:
        if (inmap->aux_data)
        {
            image_reader->png_palette = (GX_COLOR *)inmap->aux_data;
            image_reader->png_palette_size = inmap->aux_data_size / sizeof(GX_COLOR);
        }

        image_reader->pixel_read = _gx_image_reader_4bit_pixel_read;
        image_reader->input_stride = (UINT)(inmap->width >> 1);
        break;

    case GX_IMAGE_FORMAT_8BPP:
        if (inmap->aux_data)
        {
            image_reader->png_palette = (GX_COLOR *)inmap->aux_data;
            image_reader->png_palette_size = inmap->aux_data_size / sizeof(GX_COLOR);
        }

        image_reader->pixel_read = _gx_image_reader_8bit_pixel_read;
        image_reader->input_stride = (UINT)inmap->width;
        break;

    case GX_IMAGE_FORMAT_16BPP_GRAY:
        image_reader->pixel_read = _gx_image_reader_16bit_gray_read;
        image_reader->input_stride = (UINT)(inmap->width << 1);
        break;

    case GX_IMAGE_FORMAT_16BPP_GRAY_ALPHA: /* Internal format: gray: alpha byte stream. */
        image_reader->pixel_read = _gx_image_reader_16bit_gray_alpha_read;
        image_reader->input_stride = (UINT)(inmap->width << 1);
        break;

    case GX_IMAGE_FORMAT_32BPP_GRAY_ALPHA: /* Internal format: gray: alpha byte stream. */
        image_reader->pixel_read = _gx_image_reader_32bit_gray_alpha_read;
        image_reader->input_stride = (UINT)(inmap->width << 2);
        break;

    case GX_IMAGE_FORMAT_24BPP: /* Internal format: r:g:b byte stream. */
        image_reader->pixel_read = _gx_image_reader_24bit_pixel_read;
        image_reader->input_stride = (UINT)(inmap->width * 3);
        break;

    case GX_IMAGE_FORMAT_32BPP: /* Internal format: r:g:b:a byte strem. */
        image_reader->pixel_read = _gx_image_reader_32bit_pixel_read;
        image_reader->input_stride = (UINT)(inmap->width << 2);
        break;

    case GX_IMAGE_FORMAT_48BPP:
        image_reader->pixel_read = _gx_image_reader_48bit_pixel_read;
        image_reader->input_stride = (UINT)(inmap->width * 6);
        break;

    case GX_IMAGE_FORMAT_64BPP:
        image_reader->pixel_read = _gx_image_reader_64bit_pixel_read;
        image_reader->input_stride = (UINT)(inmap->width << 3);
        break;

    case GX_COLOR_FORMAT_32ARGB:
    case GX_COLOR_FORMAT_24XRGB:
        image_reader->pixel_read = _gx_image_reader_32argb_pixel_read;
        image_reader->input_stride = (UINT)(inmap->width << 2);
        break;

    case GX_COLOR_FORMAT_565RGB:
        image_reader->pixel_read = _gx_image_reader_565rgb_pixel_read;
        image_reader->input_stride = (UINT)(inmap->width << 1);
        break;

    case GX_COLOR_FORMAT_1555XRGB:
        image_reader->pixel_read = _gx_image_reader_1555xrgb_pixel_read;
        image_reader->input_stride = (UINT)(inmap->width << 1);
        break;

    case GX_COLOR_FORMAT_8BIT_PALETTE:
        image_reader->pixel_read = _gx_image_reader_8bit_palette_pixel_read;
        image_reader->input_stride = (UINT)inmap->width;
        break;

    case GX_COLOR_FORMAT_8BIT_ALPHAMAP:
        image_reader->pixel_read = _gx_image_reader_8bit_alpha_read;
        image_reader->input_stride = (UINT)inmap->width;
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        image_reader->pixel_read = _gx_image_reader_4444argb_pixel_read;
        image_reader->input_stride = (UINT)(inmap->width << 1);
        break;

    case GX_COLOR_FORMAT_4BIT_GRAY:
        if (inmap->flags & GX_PIXELMAP_TRANSPARENT)
        {
            image_reader->pixel_read = _gx_image_reader_4bit_grayscale_transparent_read;
        }
        else
        {
            image_reader->pixel_read = _gx_image_reader_4bit_grayscale_pixel_read;
        }
        image_reader->input_stride = (UINT)((inmap->width + 1) >> 1);
        break;

    case GX_COLOR_FORMAT_MONOCHROME:
        if (inmap->flags & GX_PIXELMAP_TRANSPARENT)
        {
            image_reader->pixel_read = _gx_image_reader_1bpp_transparent_read;
            image_reader->input_stride = (UINT)((inmap->width + 3) >> 2);
        }
        else
        {
            image_reader->pixel_read = _gx_image_reader_1bpp_pixel_read;
            image_reader->input_stride = (UINT)((inmap->width + 7) >> 3);
        }
        break;

    default:
        return GX_NOT_SUPPORTED;
    }

    image_reader->getdata = (GX_UBYTE *)inmap->data;
    image_reader->getauxdata = (GX_UBYTE *)inmap->aux_data;

    return GX_SUCCESS;
}
#endif

#if defined(GX_SOFTWARE_DECODER_SUPPORT)

extern void _gx_image_reader_rgb2gray(GX_PIXEL *pixel, GX_UBYTE *gray);
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_8bit_alpha_write                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes a pixel to output pixemap data structure.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_8bit_alpha_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
GX_UBYTE *palpha;

    if (!image_reader->size_testing)
    {
        palpha = (GX_UBYTE *)image_reader->putdata;

        *palpha = pixel->alpha;
    }

    image_reader->putdata += 1;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_8bit_palette_write                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes a palette index to output pixemap data         */
/*    structure.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
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
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_8bit_palette_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;
INT       palindex;

    if (!image_reader->size_testing)
    {
        pLine = (GX_UBYTE *)image_reader->putdata;

        if ((image_reader->mode & GX_IMAGE_READER_MODE_ALPHA) && (pixel->alpha < 128))
        {
            *pLine = GX_TRANSPARENT_COLOR;
        }
        else
        {
            /* Find the palette color that is nearest to the wanted color. */
            _gx_image_reader_nearest_palette_color_get(image_reader, pixel, &palindex);

            *pLine = (GX_UBYTE)palindex;
        }
    }

    image_reader->putdata += 1;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_24xrgb_pixel_write                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes a pixel to output pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_24xrgb_pixel_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
GX_COLOR *pLine;

    if (!image_reader->size_testing)
    {
        pLine = (GX_COLOR *)image_reader->putdata;

        (*pLine) = (GX_COLOR)((pixel->red << 16));
        (*pLine) = (GX_COLOR)((*pLine) | ((ULONG)(pixel->green) << 8));
        (*pLine) = (GX_COLOR)((*pLine) | pixel->blue);
        (*pLine) = (GX_COLOR)((*pLine) | 0xff000000);
    }

    image_reader->putdata += 4;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_24xrgb_rotated_pixel_write                         */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes a pixel to output rotated pixelmap data        */
/*    structure.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_24xrgb_rotated_pixel_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
GX_COLOR *pLine;

    if (!image_reader->size_testing)
    {
        pLine = (GX_COLOR *)image_reader->putdata;

        (*pLine) = (GX_COLOR)((pixel->red << 16));
        (*pLine) = (GX_COLOR)((*pLine) | ((ULONG)(pixel->green) << 8));
        (*pLine) = (GX_COLOR)((*pLine) | pixel->blue);
        (*pLine) = (GX_COLOR)((*pLine) | ((ULONG)(pixel->alpha) << 24));
    }

    if (image_reader->mode & GX_IMAGE_READER_MODE_ROTATE_CW)
    {
        image_reader->putdata -= (image_reader->image_height << 2);
    }
    else
    {
        image_reader->putdata += (image_reader->image_height << 2);
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_32argb_pixel_write                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes a pixel to output pixelmap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_32argb_pixel_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
GX_COLOR *pLine;

    if (!image_reader->size_testing)
    {
        pLine = (GX_COLOR *)image_reader->putdata;

        (*pLine) = (GX_COLOR)(pixel->alpha << 24);
        (*pLine) = (GX_COLOR)((*pLine) | ((ULONG)(pixel->red) << 16));
        (*pLine) = (GX_COLOR)((*pLine) | ((ULONG)(pixel->green) << 8));
        (*pLine) = (GX_COLOR)((*pLine) | pixel->blue);
    }

    image_reader->putdata += 4;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_565rgb_rle_pixel_write                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes a pixel to output pixemap data structure.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_565rgb_rle_pixel_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
USHORT   *pLine;
GX_UBYTE *pAlpha;

    if (!image_reader->size_testing)
    {
        pLine = (USHORT *)image_reader->putdata;

        if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
        {
            pAlpha = (GX_UBYTE *)image_reader->putdata;

            /* Skip count. */
            pAlpha++;
            pLine++;

            (*pAlpha) = pixel->alpha;
        }

        pixel->red &= 0xf8;
        pixel->green &= 0xfc;
        pixel->blue &= 0xf8;

        (*pLine) = (USHORT)(pixel->red << 8);
        (*pLine) = (USHORT)((*pLine) | (pixel->green << 3));
        (*pLine) = (USHORT)((*pLine) | (pixel->blue >> 3));
    }

    if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
    {
        image_reader->putdata += 4;
    }
    else
    {
        image_reader->putdata += 2;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_565rgb_pixel_write                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes a pixel to output pixemap data structure.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_565rgb_pixel_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
USHORT   *pLine;
GX_UBYTE *pAlpha;

    pLine = (USHORT *)image_reader->putdata;

    pixel->red &= 0xf8;
    pixel->green &= 0xfc;
    pixel->blue &= 0xf8;

    (*pLine) = (USHORT)(pixel->red << 8);
    (*pLine) = (USHORT)((*pLine) | (pixel->green << 3));
    (*pLine) = (USHORT)((*pLine) | (pixel->blue >> 3));

    if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
    {
        pAlpha = image_reader->putauxdata;

        (*pAlpha) = pixel->alpha;
    }

    image_reader->putauxdata += 1;
    image_reader->putdata += 2;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_565rgb_rotated_pixel_write                         */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function rotates a pixel to output pixemap data structure.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_565rgb_rotated_pixel_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
USHORT   *pLine;
GX_UBYTE *pAlpha;

    if(!image_reader->size_testing)
    {
        pLine = (USHORT *)image_reader->putdata;

        pixel->red &= 0xf8;
        pixel->green &= 0xfc;
        pixel->blue &= 0xf8;

        (*pLine) = (USHORT)(pixel->red << 8);
        (*pLine) = (USHORT)((*pLine) | (pixel->green << 3));
        (*pLine) = (USHORT)((*pLine) | (pixel->blue >> 3));

        if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
        {
            pAlpha = image_reader->putauxdata;

            (*pAlpha) = pixel->alpha;
        }
    }

    if (image_reader->mode & GX_IMAGE_READER_MODE_ROTATE_CW)
    {
        image_reader->putauxdata -= image_reader->image_height;
        image_reader->putdata -= (image_reader->image_height << 1);
    }
    else
    {
        image_reader->putauxdata += image_reader->image_height;
        image_reader->putdata += (image_reader->image_height << 1);
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_1555xrgb_rle_pixel_write                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes an 1555xrgb format pixel to output pixemap     */
/*    data structure.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_1555xrgb_rle_pixel_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
USHORT   *pLine;
GX_UBYTE *pAlpha;

    if (!image_reader->size_testing)
    {
        pLine = (USHORT *)image_reader->putdata;

        if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
        {
            pAlpha = (GX_UBYTE *)image_reader->putdata;

            /* Skip count. */
            pAlpha++;
            pLine++;

            (*pAlpha) = pixel->alpha;
        }

        pixel->red &= 0xf8;
        pixel->green &= 0xf8;
        pixel->blue &= 0xf8;

        (*pLine) = (USHORT)(pixel->red << 7);
        (*pLine) = (USHORT)((*pLine) | (pixel->green << 2));
        (*pLine) = (USHORT)((*pLine) | (pixel->blue >> 3));
    }

    if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
    {
        image_reader->putdata += 4;
    }
    else
    {
        image_reader->putdata += 2;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_1555xrgb_pixel_write                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes an 1555xrgb format pixel to output pixemap     */
/*    data structure.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_1555xrgb_pixel_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
USHORT   *pLine;
GX_UBYTE *pAlpha;

    pLine = (USHORT *)image_reader->putdata;

    pixel->red &= 0xf8;
    pixel->green &= 0xf8;
    pixel->blue &= 0xf8;

    (*pLine) = (USHORT)(pixel->red << 7);
    (*pLine) = (USHORT)((*pLine) | (pixel->green << 2));
    (*pLine) = (USHORT)((*pLine) | (pixel->blue >> 3));

    if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
    {
        pAlpha = image_reader->putauxdata;

        (*pAlpha) = pixel->alpha;
    }

    image_reader->putauxdata += 1;
    image_reader->putdata += 2;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_4444argb_pixel_write                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes an 4444argb format pixel (no transparency) to  */
/*    output pixemap data structure.                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_4444argb_pixel_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
USHORT *pLine;

    if (!image_reader->size_testing)
    {
        pLine = (USHORT *)image_reader->putdata;

        pixel->alpha &= 0xf0;
        pixel->red &= 0xf0;
        pixel->green &= 0xf0;
        pixel->blue &= 0xf0;

        (*pLine) = (USHORT)((*pLine) | (pixel->red << 4));
        (*pLine) = (USHORT)((*pLine) | (pixel->green << 0));
        (*pLine) = (USHORT)((*pLine) | pixel->blue >> 4);
        (*pLine) = (USHORT)((*pLine) | 0xf000);
    }

    image_reader->putdata += 2;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_4444argb_transparent_write                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes an 4444argb format pixel (with transparency)   */
/*    to output pixemap data structure.                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_4444argb_transparent_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
USHORT *pLine;

    if (!image_reader->size_testing)
    {
        pLine = (USHORT *)image_reader->putdata;

        pixel->alpha &= 0xf0;
        pixel->red &= 0xf0;
        pixel->green &= 0xf0;
        pixel->blue &= 0xf0;

        (*pLine) = (USHORT)(pixel->alpha << 8);
        (*pLine) = (USHORT)((*pLine) | (pixel->red << 4));
        (*pLine) = (USHORT)((*pLine) | (pixel->green << 0));
        (*pLine) = (USHORT)((*pLine) | pixel->blue >> 4);
    }

    image_reader->putdata += 2;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_4bit_grayscale_pixel_write                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes an 4bit grayscale format pixel                 */
/*    (no transparency)to output pixelmap data structure.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_rgb2gray             Convert RGB value to grayscale*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_4bit_grayscale_pixel_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;
GX_UBYTE  gray;
GX_UBYTE  color;

    if (!image_reader->size_testing)
    {
        _gx_image_reader_rgb2gray(pixel, &gray);
        gray >>= 4;

        color = (GX_UBYTE)(gray | (gray << 4));
        pixel->red = color;
        pixel->green = color;
        pixel->blue = color;

        pLine = image_reader->putdata;

        if (image_reader->putdata_mask & 0xf0)
        {
            (*pLine) = (GX_UBYTE)(gray << 4);
        }
        else
        {
            (*pLine) |= gray;
        }
    }

    image_reader->putdata_mask >>= 4;

    if (!image_reader->putdata_mask)
    {
        image_reader->putdata++;
        image_reader->putdata_mask = 0xf0;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_4bit_grayscale_transparent_write                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes an 4bit grayscale format pixel                 */
/*    (with transparency)to output pixelmap data structure.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_rgb2gray             Convert RGB value to grayscale*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_4bit_grayscale_transparent_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;
GX_UBYTE *pAuxData;
GX_UBYTE  gray;
GX_UBYTE  color;

    if (!image_reader->size_testing)
    {
        _gx_image_reader_rgb2gray(pixel, &gray);
        gray >>= 4;

        color = (GX_UBYTE)(gray | (gray << 4));
        pixel->red = color;
        pixel->green = color;
        pixel->blue = color;

        pLine = image_reader->putdata;

        if (image_reader->mode & GX_IMAGE_READER_MODE_COMPRESS)
        {
            /* with compression, with transparent: 1 byte color
               0x00-0x0f: pixel value
               0xff:      transparent value */
            if (pixel->alpha < 128)
            {
                (*pLine) = GX_TRANSPARENT_COLOR;
            }
            else
            {
                (*pLine) = gray;
            }
        }
        else
        {
            /* without compression, with transparent: half byte color,
               1-bit transparent go into auxdata. */
            if (image_reader->putdata_mask & 0xf0)
            {
                (*pLine) = (GX_UBYTE)(gray << 4);
            }
            else
            {
                (*pLine) |= gray;
            }

            pAuxData = image_reader->putauxdata;

            if (pixel->alpha > 128)
            {
                (*pAuxData) &= (GX_UBYTE)(~image_reader->putauxdata_mask);
            }
            else
            {
                (*pAuxData) |= image_reader->putauxdata_mask;
            }
        }
    }

    if (image_reader->mode & GX_IMAGE_READER_MODE_COMPRESS)
    {
        image_reader->putdata++;
    }
    else
    {

        image_reader->putdata_mask >>= 4;

        if (!image_reader->putdata_mask)
        {
            image_reader->putdata++;
            image_reader->putdata_mask = 0xf0;
        }

        image_reader->putauxdata_mask >>= 1;
        if (!image_reader->putauxdata_mask)
        {
            image_reader->putauxdata++;
            image_reader->putauxdata_mask = 0x80;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_monochrome_pixel_write                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes a monochrome format pixel (no transparency) to */
/*    output pixelmap data structure.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_rgb2gray             Convert RGB value to grayscale*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_monochrome_pixel_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;
GX_UBYTE  gray;
GX_UBYTE  color;

    pLine = image_reader->putdata;

    if (image_reader->mode & GX_IMAGE_READER_MODE_COMPRESS)
    {
        /* [7-bit counter][1-bit color]*/
        if (!image_reader->size_testing)
        {
            _gx_image_reader_rgb2gray(pixel, &gray);
            if (gray > image_reader->mono_shreshold)
            {
                (*pLine) |= 0x01;
            }
            else
            {
                (*pLine) &= 0xfe;
            }
        }
        image_reader->putdata++;
    }
    else
    {
        /* [1-bit color]*/
        _gx_image_reader_rgb2gray(pixel, &gray);
        if (gray > image_reader->mono_shreshold)
        {
            (*pLine) |= image_reader->putdata_mask;
            color = 255;
        }
        else
        {
            (*pLine) &= (GX_UBYTE)(~image_reader->putdata_mask);
            color = 0;
        }

        pixel->red = color;
        pixel->green = color;
        pixel->blue = color;
        pixel->alpha = 0xff;

        image_reader->putdata_mask >>= 1;

        if (!image_reader->putdata_mask)
        {
            image_reader->putdata++;
            image_reader->putdata_mask = 0x80;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_monochrome_transparent_write                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes a monochrome format pixel (with transparency)  */
/*    to output pixelmap data structure.                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pixel                                 Pixel to write.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_rgb2gray             Convert RGB value to grayscale*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_monochrome_transparent_write(GX_IMAGE_READER *image_reader, GX_PIXEL *pixel)
{
GX_UBYTE *pLine;
GX_UBYTE  gray;
GX_UBYTE  trans_mask;
GX_UBYTE  color;

    pLine = image_reader->putdata;

    if (image_reader->mode & GX_IMAGE_READER_MODE_COMPRESS)
    {
        /* [6-bit counter][1-bit color][1-bit transparency]*/
        if (!image_reader->size_testing)
        {
            if (pixel->alpha < 128)
            {
                /* Mark as transparency. */
                (*pLine) &= 0xfc;
            }
            else
            {
                (*pLine) |= 0x01;

                /* Write color: 0 or 1. */
                _gx_image_reader_rgb2gray(pixel, &gray);
                if (gray > image_reader->mono_shreshold)
                {
                    (*pLine) |= 0x02;
                }
                else
                {
                    (*pLine) &= 0xfd;
                }
            }
        }
        image_reader->putdata++;
    }
    else
    {
        /* [1-bit color][1-bit transparency]*/
        trans_mask = image_reader->putdata_mask >> 1;
        if (pixel->alpha < 128)
        {
            /* Mark as transparency. */
            (*pLine) &= (GX_UBYTE)(~trans_mask);
            (*pLine) &= (GX_UBYTE)(~image_reader->putdata_mask);
        }
        else
        {
            (*pLine) |= trans_mask;

            _gx_image_reader_rgb2gray(pixel, &gray);
            if (gray > image_reader->mono_shreshold)
            {
                (*pLine) |= image_reader->putdata_mask;
                color = 255;
            }
            else
            {
                (*pLine) &= (GX_UBYTE)(~image_reader->putdata_mask);
                color = 0;
            }
            pixel->red = color;
            pixel->green = color;
            pixel->blue = color;
        }

        image_reader->putdata_mask >>= 2;

        if (!image_reader->putdata_mask)
        {
            image_reader->putdata++;
            image_reader->putdata_mask = 0x80;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_pixel_rotated_write_callback_set                   */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets pixel write callback of the image reader when    */
/*    rotation mode is set.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block    */
/*    outmap                                Outpu pixelmap.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_565rgb_rotated_pixel_write                         */
/*                                          Write 565rgb format pixel in  */
/*                                            rotation mode               */
/*    _gx_image_reader_24xrgb_rotated_pixel_write                         */
/*                                          Write 24xrgn format pixel in  */
/*                                            rotation mode               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_pixel_rotated_write_callback_set(GX_IMAGE_READER *image_reader, GX_PIXELMAP *outmap)
{
    switch (outmap->format)
    {
    case GX_COLOR_FORMAT_565RGB:
        image_reader->pixel_write = _gx_image_reader_565rgb_rotated_pixel_write;

        if (image_reader->mode & GX_IMAGE_READER_MODE_ROTATE_CW)
        {
            outmap->flags |= GX_PIXELMAP_ROTATED_90;

            image_reader->putdatarow = (GX_UBYTE *)outmap->data;
            image_reader->putdatarow += (outmap->width - 1) * (outmap->height << 1);

            if (outmap->aux_data)
            {
                image_reader->putauxdatarow = (GX_UBYTE *)outmap->aux_data;
                image_reader->putauxdatarow += (outmap->width - 1) * outmap->height;
            }

            image_reader->putdatarow_stride = 2;
            image_reader->putauxdatarow_stride = 1;
        }
        else
        {
            outmap->flags |= GX_PIXELMAP_ROTATED_270;

            image_reader->putdatarow = (GX_UBYTE *)outmap->data;
            image_reader->putdatarow += (outmap->height - 1) * 2;

            if (outmap->aux_data)
            {
                image_reader->putauxdatarow = (GX_UBYTE *)outmap->aux_data;
                image_reader->putauxdatarow += (outmap->height - 1);
            }

            image_reader->putdatarow_stride = -2;
            image_reader->putauxdatarow_stride = -1;
        }
        break;

    case GX_COLOR_FORMAT_24XRGB:
        image_reader->pixel_write = _gx_image_reader_24xrgb_rotated_pixel_write;

        if (image_reader->mode & GX_IMAGE_READER_MODE_ROTATE_CW)
        {
            outmap->flags |= GX_PIXELMAP_ROTATED_90;

            image_reader->putdatarow = (GX_UBYTE *)outmap->data;
            image_reader->putdatarow += (outmap->width - 1) * (outmap->height << 2);

            image_reader->putdatarow_stride = 4;
        }
        else
        {
            outmap->flags |= GX_PIXELMAP_ROTATED_270;

            image_reader->putdatarow = (GX_UBYTE *)outmap->data;
            image_reader->putdatarow += ((outmap->height - 1) << 2);

            image_reader->putdatarow_stride = -4;
        }
        break;

    default:
        return GX_NOT_SUPPORTED;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets pixel write callback of the image reader.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block    */
/*    outmap                                Outpu pixelmap.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_565rgb_rle_pixel_write                             */
/*                                          Write 565 rgb fromat pixel    */
/*                                            in compress mode            */
/*    _gx_image_reader_565rgb_pixel_write   Write 565 rgb format pixel    */
/*    _gx_image_reader_1555xrgb_rle_pixel_write                           */
/*                                          Write 1555xrgb format pixel   */
/*                                            in compress mode            */
/*    _gx_image_reader_1555xrgb_pixel_write Write 1555xrgb format pixel   */
/*    _gx_image_reader_4444argb_transparent_write                         */
/*                                          Write 4444argb format pixel   */
/*                                            with transparency           */
/*    _gx_image_reader_4444argb_pixel_write Write 4444argb format pixel   */
/*    _gx_image_reader_32argb_pixel_write   Write 32argb format pixel     */
/*    _gx_image_reader_24xrgb_pixel_write   Write 24xrgb format pixel     */
/*    _gx_image_reader_8bit_alpha_write     Write 8bit alphamap format    */
/*                                            pixel                       */
/*    _gx_image_reader_8bit_palette_write   Write 8bit palette format     */
/*                                            pixel                       */
/*    _gx_image_reader_4bit_grayscale_transparent_write                   */
/*                                          Write 4bit grayscale format   */
/*                                            pixel with transparency     */
/*    _gx_image_reader_4bit_grayscale_pixel_write                         */
/*                                          Write 4bit grayscale format   */
/*                                            pixel                       */
/*    _gx_image_reader_monochrome_transparent_write                       */
/*                                          Write 1bpp format pixel with  */
/*                                            transparency                */
/*    _gx_image_reader_monochrome_pixel_write                             */
/*                                          Write 1bpp format pixel       */
/*    _gx_system_memory_allocator           Application defined memory    */
/*                                            allocator function          */
/*    _gx_system_memory_free                Application defined memory    */
/*                                            free function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_colorspace_convert                                 */
/*                                                                        */
/**************************************************************************/
UINT _gx_image_reader_pixel_write_callback_set(GX_IMAGE_READER *image_reader, GX_PIXELMAP *outmap)
{
    /* Allocate memory for data of the output pixelmap. */
    if (outmap->data_size)
    {
        outmap->data = (GX_UBYTE *)_gx_system_memory_allocator(outmap->data_size);

        if (outmap->data == GX_NULL)
        {
            return GX_SYSTEM_MEMORY_ERROR;
        }

        memset((void *)outmap->data, 0, outmap->data_size);

        image_reader->putdata = (GX_UBYTE *)outmap->data;
    }

    /* Allocate memory for aux data of the output pixelmap. */
    if (outmap->aux_data_size)
    {
        outmap->aux_data = (GX_UBYTE *)_gx_system_memory_allocator(outmap->aux_data_size);

        if (outmap->aux_data == GX_NULL)
        {
            _gx_system_memory_free((void *)outmap->data);
            outmap->data = GX_NULL;
            return GX_SYSTEM_MEMORY_ERROR;
        }

        memset((void *)outmap->aux_data, 0, outmap->aux_data_size);

        image_reader->putauxdata = (GX_UBYTE *)outmap->aux_data;
    }

    if (image_reader->mode & (GX_IMAGE_READER_MODE_ROTATE_CW | GX_IMAGE_READER_MODE_ROTATE_CCW))
    {
        return _gx_image_reader_pixel_rotated_write_callback_set(image_reader, outmap);
    }

    /* Set pixel write callback.  */
    switch (outmap->format)
    {
    case GX_COLOR_FORMAT_565RGB:
        if (outmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            image_reader->pixel_write = _gx_image_reader_565rgb_rle_pixel_write;
        }
        else
        {
            image_reader->pixel_write = _gx_image_reader_565rgb_pixel_write;
        }
        break;

    case GX_COLOR_FORMAT_1555XRGB:
        if (outmap->flags & GX_PIXELMAP_COMPRESSED)
        {
            image_reader->pixel_write = _gx_image_reader_1555xrgb_rle_pixel_write;
        }
        else
        {
            image_reader->pixel_write = _gx_image_reader_1555xrgb_pixel_write;
        }
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        if (outmap->flags & GX_PIXELMAP_ALPHA)
        {
            image_reader->pixel_write = _gx_image_reader_4444argb_transparent_write;
        }
        else
        {
            image_reader->pixel_write = _gx_image_reader_4444argb_pixel_write;
        }
        break;

    case GX_COLOR_FORMAT_32ARGB:
    case GX_COLOR_FORMAT_24XRGB:
        if (outmap->flags & GX_PIXELMAP_ALPHA)
        {
            image_reader->pixel_write = _gx_image_reader_32argb_pixel_write;
        }
        else
        {
            image_reader->pixel_write = _gx_image_reader_24xrgb_pixel_write;
        }
        break;

    case GX_COLOR_FORMAT_8BIT_ALPHAMAP:
        image_reader->pixel_write = _gx_image_reader_8bit_alpha_write;
        break;

    case GX_COLOR_FORMAT_8BIT_PALETTE:
        image_reader->pixel_write = _gx_image_reader_8bit_palette_write;
        break;

    case GX_COLOR_FORMAT_4BIT_GRAY:
        if (outmap->flags & GX_PIXELMAP_TRANSPARENT)
        {
            image_reader->pixel_write = _gx_image_reader_4bit_grayscale_transparent_write;
        }
        else
        {
            image_reader->pixel_write = _gx_image_reader_4bit_grayscale_pixel_write;
        }
        image_reader->putdata_mask = 0xf0;
        image_reader->putauxdata_mask = 0x80;
        break;

    case GX_COLOR_FORMAT_MONOCHROME:
        if (outmap->flags & GX_PIXELMAP_TRANSPARENT)
        {
            image_reader->pixel_write = _gx_image_reader_monochrome_transparent_write;
        }
        else
        {
            image_reader->pixel_write = _gx_image_reader_monochrome_pixel_write;
        }
        image_reader->putdata_mask = 0x80;
        break;

    default:
        return GX_NOT_SUPPORTED;
    }

    return GX_SUCCESS;
}
#endif

#if defined(GX_SOFTWARE_DECODER_SUPPORT)
static UINT _bit_buffer;
static UINT _bit_count;

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_crc_table_make                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates crc table that speed the crc calculation.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block             */
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
/*    _gx_image_reader_png_decode                                         */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_png_crc_table_make(GX_PNG *png)
{
UINT crc;
UINT n;
UINT k;

    for (n = 0; n < 256; n++)
    {
        crc = n;
        for (k = 0; k < 8; k++)
        {
            if (crc & 1)
            {
                crc = 0xedb88320 ^ (crc >> 1);
            }
            else
            {
                crc = crc >> 1;
            }
        }
        png->crc_table[n] = crc;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_png_crc_get                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function calculates Cyclic Redundancy Check of a pecified      */
/*    data stream.                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block             */
/*    crc                                   Generated crc value           */
/*    len                                   Data length                   */
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
/*    _gx_image_reader_png_decode                                         */
/*    _gx_image_reader_png_bits_get                                       */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_png_crc_get(GX_PNG *png, UINT *crc, UINT len)
{
UINT      n;
GX_UBYTE *buf = png->data + png->data_index;
UINT      crc_cal = (*crc);

    for (n = 0; n < len; n++)
    {
        crc_cal = png->crc_table[(crc_cal ^ buf[n]) & 0xff] ^ (crc_cal >> 8);
    }

    (*crc) = crc_cal ^ 0xffffffff;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_4bytes_read                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads 4 bytes and packed it as an INT value.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block             */
/*    value                                 Retrieved INT value           */
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
/*    _gx_image_reader_png_decode                                         */
/*    _gx_image_reader_png_bits_get                                       */
/*    _gx_image_reader_png_IHDR_chunk_read                                */
/*    _gx_image_reader_png_gAMA_chunk_read                                */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_png_4bytes_read(GX_PNG *png, INT *value)
{
INT get_value;

    get_value = (png->data[png->data_index] << 24);
    png->data_index++;

    get_value |= (png->data[png->data_index] << 16);
    png->data_index++;

    get_value |= (png->data[png->data_index] << 8);
    png->data_index++;

    get_value |= png->data[png->data_index];
    png->data_index++;

    (*value) = get_value;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_chunk_type_read                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads the type information of a chunk.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block             */
/*    chunk_type                            Retrieved chunk type info     */
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
/*    _gx_image_reader_png_bits_get                                       */
/*    _gx_image_reader_png_decode                                         */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_png_chunk_type_read(GX_PNG *png, CHAR *chunk_type)
{
    chunk_type[0] = (CHAR)png->data[png->data_index++];
    chunk_type[1] = (CHAR)png->data[png->data_index++];
    chunk_type[2] = (CHAR)png->data[png->data_index++];
    chunk_type[3] = (CHAR)png->data[png->data_index++];
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_bits_get                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function Extract a specified number of bits from PNG data      */
/*    stream and advance the read pointer of the PNG data stream.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block             */
/*    num_of_bits                           number of bits to retrieve    */
/*    return_value                          Retrieved bits data           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_png_4bytes_read      Read 4 bytes and pack it to a */
/*                                            INT type value              */
/*    _gx_image_reader_png_crc_get          Calculate crc value with      */
/*                                            specified data              */
/*    _gx_image_reader_png_chunk_type_read  Read type information         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_png_clen_huffman_read                              */
/*    _gx_image_reader_png_ll_huffman_read                                */
/*    _gx_image_reader_png_huffcode_decode                                */
/*    _gx_image_reader_png_IDAT_chunk_read                                */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_png_bits_get(GX_PNG *png, UINT num_of_bits, UINT *return_value)
{
GX_UBYTE get_byte;
INT      checksum;
CHAR     chunk_type[4];
INT      index = png->trunk_end_index;

    while (_bit_count < num_of_bits)
    {
        if (png->data_index >= png->data_size)
        {
            return GX_FAILURE;
        }

        get_byte = png->data[png->data_index++];

        _bit_buffer |= (UINT)(get_byte << (UINT)_bit_count);
        _bit_count += 8;

        if (png->data_index >= index)
        {
            /* data_len*/
            _gx_image_reader_png_4bytes_read(png, &checksum);

            if (png->trunk_crc != (UINT)checksum)
            {
                return GX_FAILURE;
            }

            _gx_image_reader_png_4bytes_read(png, &index);

            png->trunk_crc = 0xffffffff;

            _gx_image_reader_png_crc_get(png, &png->trunk_crc, (UINT)(4 + index));

            _gx_image_reader_png_chunk_type_read(png, chunk_type);

            if (strncmp(chunk_type, "IDAT", 4) == 0)
            {
                index += png->data_index;
                png->trunk_end_index = index;
            }
        }
    }

    (*return_value) = _bit_buffer;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_bits_revert                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reorders bits stream in opposite order.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    bits                                  Variable contain bits stream  */
/*    bits_len                              The number of bits to revert  */
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
/*    _gx_image_reader_png_dynamic_huffman_code_find                      */
/*    _gx_image_reader_png_fixed_ll_huffman_code_find                     */
/*    _gx_image_reader_png_huffcode_decode                                */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_png_bits_revert(UINT *bits, UINT bits_len)
{
UINT reverted = 0;
UINT i;

    for (i = 0; i < bits_len; i++)
    {
        reverted |= ((*bits) & 0x1) << (bits_len - i - 1);
        (*bits) >>= 1;
    }

    (*bits) = reverted;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_bits_skip                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function skips bits from tempory PNG data buffer.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    num_of_skip_bits                      The number of bits to skip    */
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
/*    _gx_image_reader_png_clen_huffman_read                              */
/*    _gx_image_reader_png_huffcode_decode                                */
/*    _gx_image_reader_png_tRNS_chunk_read                                */
/*    _gx_image_reader_png_IDAT_chunk_read                                */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_png_bits_skip(UINT num_of_skip_bits)
{
    _bit_buffer >>= num_of_skip_bits;

    _bit_count -= num_of_skip_bits;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_IHDR_chunk_read                                */
/*                                                           6.2.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads in IHDR chunk information.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_png_4bytes_read    Read 4 bytes and pack it to a */
/*                                            INT type value              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_png_decode                                         */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_png_IHDR_chunk_read(GX_PNG *png)
{
    /* The IHDR chunk contains:
        Width: 4 bytes
        Height: 4 bytes
        Bit depth: 1 byte
        Color type: 1 byte
        Compression method: 1 byte
        Filter method: 1 byte
        Interlace method: 1 byte */

    if (png->data_index + 13 > png->trunk_end_index)
    {
        return GX_INVALID_SIZE;
    }

    _gx_image_reader_png_4bytes_read(png, &png->width);
    _gx_image_reader_png_4bytes_read(png, &png->height);

    /* Limited max png width to 14 bits. */
    if ((png->width < 0) || (png->width > GX_MAX_PIXELMAP_RESOLUTION))
    {
        return GX_INVALID_WIDTH;
    }

    /* Limited max png height to 14 bits. */
    if ((png->height < 0) || (png->height > GX_MAX_PIXELMAP_RESOLUTION))
    {
        return GX_INVALID_HEIGHT;
    }

    /* Bits per sample or per palette index, valid values are 1, 2 ,4, 8, 16. */
    png->bit_depth = png->data[png->data_index++];

    switch (png->bit_depth)
    {
    case 1:
    case 2:
    case 4:
    case 8:
    case 16:
        break;

    default:
        return GX_INVALID_FORMAT;
    }

    /* 0 gray sample
       2 RGB tripple
       3 palette index
       4 gray sample followed by an alpha sample
       6 RGB triple followed by an alpha sample */
    png->color_type = png->data[png->data_index++];

    switch (png->color_type)
    {
    case 0:
    /* Each pixel is agrayscale sample. */
    case 3:
        /* Each pixel is a palette index, a PLTE chunk must appear. */
        png->bpp = png->bit_depth;
        break;
    case 2:
        /* Each pixel is an RGB triple. */
        png->bpp = (GX_UBYTE)(png->bit_depth * 3);
        break;

    case 4:
        /* Each pixel is a grayscale sample, followed by an alpha sample. */
        png->bpp = (GX_UBYTE)(png->bit_depth * 2);
        break;
    case 6:
        /* Each pixel is an R,G,B triple, followed by an alpha sample. */
        png->bpp = (GX_UBYTE)(png->bit_depth * 4);
        break;

    default:
        return GX_FAILURE;
    }

    /* At present, only method 0 is defined. */
    png->compression_method = png->data[png->data_index++];

    /* At present, only method 0 is defined. */
    png->filter_method = png->data[png->data_index++];

    if ((png->compression_method != 0) || (png->filter_method != 0))
    {
        /* Invalid value. */
        return GX_FAILURE;
    }

    /* 0 no interlace
       1 Adam7 interlace */
    png->interlace_method = png->data[png->data_index++];

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_dynamic_huffman_code_find                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves code value from dynamic huffman table.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block             */
/*    huffman_bits_count                    Array records codes number for*/
/*                                          every code len                */
/*    scan_buffer                           Buffer to search from         */
/*    bit_len                               Retrieved Huffman Code Length */
/*    code_value                            Retrieved Huffman Code        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status code                                                         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_png_bits_revert      Reorder bits stream in        */
/*                                            opposite order              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_png_huffcode_decode                                */
/*    _gx_image_reader_png_ll_huffman_read                                */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_png_dynamic_huffman_code_find(INT  *huffman_table,
                                                           UINT  *huffman_bits_count,
                                                           UINT   scan_buffer,
                                                           UINT  *bit_len,
                                                           UINT *code_value)
{
UINT i_bit;
UINT bit_count;
UINT code;
UINT code_cal = 0;
UINT code_index = 0;

    for (i_bit = 1; i_bit < 16; i_bit++)
    {
        bit_count = huffman_bits_count[i_bit] - huffman_bits_count[i_bit - 1];

        if (bit_count)
        {
            code = (scan_buffer) << (32 - i_bit);
            code = (code) >> (32 - i_bit);

            _gx_image_reader_png_bits_revert(&code, i_bit);

            if (code <= code_cal + bit_count - 1)
            {
                *bit_len = i_bit;
                *code_value = (UINT)huffman_table[code_index + code - code_cal];

                return GX_SUCCESS;
            }
            else
            {
                code_index += bit_count;
                code_cal += bit_count;
            }
        }

        code_cal <<= 1;
    }

    return GX_NOT_FOUND;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_fixed_ll_huffman_code_find                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves code value from fixed huffman table.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scan_buffer                           Buffer that contains Huffman  */
/*                                          code                          */
/*    bit_len                               Retrieved Huffman Code length */
/*    code_value                            Retrieved code value of the   */
/*                                          matched Huffman Code          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_png_bits_revert      Reorder bits stream in        */
/*                                            opposite order              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_png_huffcode_decode                                */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_png_fixed_ll_huffman_code_find(UINT scan_buffer,
                                                            UINT *bit_len,
                                                            UINT *code_value)
{
UINT i_bit;
UINT code;

    for (i_bit = 7; i_bit <= 9; i_bit++)
    {
        code = scan_buffer << (32 - i_bit);
        code = code >> (32 - i_bit);

        _gx_image_reader_png_bits_revert(&code, i_bit);

        if (i_bit == 7)
        {
            if (code <= 0x17)
            {
                *bit_len = 7;
                *code_value = 256 + code;
                break;
            }
        }
        else if (i_bit == 8)
        {
            if (code <= 0xbf)
            {
                /* code >= 0x30. */
                *bit_len = 8;
                *code_value = code - 0x30;
                break;
            }
            else if (code <= 0xc7)
            {
                /* code >= 0xc0. */
                *bit_len = 8;
                *code_value = 280 + code - 0xc0;
                break;
            }
        }
        else
        {
            /* code >= 0x190 && code <= 0x1ff. */
            *bit_len = 9;
            *code_value = 144 + code - 0x190;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_clen_huffman_read                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads code length huffman table.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block             */
/*    hclen                                 The number of "length" codes  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_png_bits_get         Extract bits from PNG data    */
/*                                            stream                      */
/*    _gx_image_reader_png_bits_skip        Skip bits from PNG data stream*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_png_IDAT_chunk_read                                */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_png_clen_huffman_read(GX_PNG *png, UINT hclen)
{

INT  code_value[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
INT  code_len[19] = {0};
UINT len;
UINT i;
INT  pos[16];

    for (i = 0; i < hclen; i++)
    {
        if (_gx_image_reader_png_bits_get(png, 3, &len) != GX_SUCCESS)
        {
            return GX_FAILURE;
        }
        _gx_image_reader_png_bits_skip(3);
        len &= 0x7;

        /* record code len for code len alphabet */
        code_len[code_value[i]] = (INT)len;

        /* record codes number for every code len */
        if (len)
        {
            png->huffman_clen_bits_count[len]++;
        }
    }

    for (i = 1; i < 16; i++)
    {
        png->huffman_clen_bits_count[i] += png->huffman_clen_bits_count[i - 1];
    }

    memset(pos, -1, 16 * sizeof(INT));

    for (i = 0; i < 19; i++)
    {
        if (code_len[i])
        {
            if (pos[code_len[i]] == -1)
            {
                pos[code_len[i]] = png->huffman_clen_bits_count[code_len[i] - 1];
            }
            png->huffman_clen_table[pos[code_len[i]]++] = (INT)i;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_ll_huffman_read                                */
/*                                                           6.2.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads in literal and length huffman table.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block.            */
/*    hlit                                  The number of "literal" codes */
/*    hdist                                 The number of "distance" codes*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_png_bits_get         Extract bits from PNG data    */
/*                                            stream                      */
/*    _gx_image_reader_png_bits_skip        Skip bits from PNG data stream*/
/*    _gx_image_reader_png_dynamic_huffman_code_find                      */
/*                                          Retrieve code value from      */
/*                                            dynamic huffman table       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_png_IDAT_chunk_read                                */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_png_ll_huffman_read(GX_PNG *png, UINT hlit, UINT hdist)
{
UINT scan_buffer;
UINT bit_len;
UINT code_value;
UINT repeat;
UINT i;
UINT count = 0;
INT  pos[16];
INT  index;

    memset(png->huffman_lit_code_len, 0, sizeof(INT) * GX_PNG_HUFFMAN_LIT_CODE_LEN_TABLE_SIZE);
    memset(png->huffman_dist_code_len, 0, sizeof(INT) * GX_PNG_HUFFMAN_DIST_CODE_LEN_TABLE_SIZE);

    for (i = 0; i < hlit + hdist;)
    {
        count++;
        if (_gx_image_reader_png_bits_get(png, 16, &scan_buffer) != GX_SUCCESS)
        {
            return GX_FAILURE;
        }

        if (_gx_image_reader_png_dynamic_huffman_code_find(png->huffman_clen_table,
                                                           (UINT *)png->huffman_clen_bits_count,
                                                           scan_buffer,
                                                           &bit_len,
                                                           &code_value) == 0)
        {
            _gx_image_reader_png_bits_skip(bit_len);

            if (code_value <= 15)
            {
                /* Represent code lengths of 0-15 */
                if (i < hlit)
                {
                    png->huffman_lit_code_len[i] = (INT)code_value;

                    png->huffman_lit_bits_count[code_value]++;
                }
                else
                {
                    png->huffman_dist_code_len[i - hlit] = (INT)code_value;
                    png->huffman_dist_bits_count[code_value]++;
                }

                i++;
            }
            else
            {
                if (code_value == 16)
                {
                    /* repeat previous */
                    /* 2 bits repeat length*/
                    _gx_image_reader_png_bits_get(png, 2, &repeat);
                    _gx_image_reader_png_bits_skip(2);
                    repeat &= 0x3;
                    repeat += 3;

                    if (i < 1)
                    {
                        return GX_FALSE;
                    }
                    else if ((i - 1) < hlit)
                    {
                        code_value = (UINT)png->huffman_lit_code_len[i - 1];
                    }
                    else
                    {
                        code_value = (UINT)png->huffman_dist_code_len[i - 1 - hlit];
                    }

                    if (i < hlit)
                    {
                        png->huffman_lit_bits_count[code_value] += (INT)repeat;

                        while (repeat--)
                        {
                            png->huffman_lit_code_len[i++] = (INT)code_value;
                        }
                    }
                    else
                    {
                        png->huffman_dist_bits_count[code_value] += (INT)repeat;

                        while (repeat--)
                        {
                            png->huffman_dist_code_len[i - hlit] = (INT)code_value;
                            i++;
                        }
                    }
                }
                else
                {
                    if (code_value == 17)
                    {
                        /* repeat 0 */
                        /* 3 bits repeat length */
                        _gx_image_reader_png_bits_get(png, 3, &repeat);
                        _gx_image_reader_png_bits_skip(3);
                        repeat &= 0x7;
                        repeat += 3;
                    }
                    else
                    {
                        /* code_value = 18. */
                        /* repeat 0*/
                        /* 7 bits repeat length */
                        _gx_image_reader_png_bits_get(png, 7, &repeat);
                        _gx_image_reader_png_bits_skip(7);
                        repeat &= 0x7f;
                        repeat += 11;
                    }

                    if (i < hlit)
                    {
                        memset(png->huffman_lit_code_len + i, 0, repeat * sizeof(INT));
                    }
                    else
                    {
                        memset(png->huffman_dist_code_len + i - hlit, 0, repeat * sizeof(INT));
                    }

                    i += repeat;
                }
            }
        }
        else
        {
            /*error*/
            return GX_FAILURE;
        }
    }

    png->huffman_lit_bits_count[0] = 0;
    png->huffman_dist_bits_count[0] = 0;

    for (i = 2; i <= 16; i++)
    {
        png->huffman_lit_bits_count[i] += png->huffman_lit_bits_count[i - 1];
        png->huffman_dist_bits_count[i] += png->huffman_dist_bits_count[i - 1];
    }

    memset(pos, -1, 16 * sizeof(INT));

    for (i = 0; i < hlit; i++)
    {
        index = png->huffman_lit_code_len[i];
        if (index)
        {
            if (pos[index] == -1)
            {
                pos[index] = png->huffman_lit_bits_count[index - 1];
            }
            png->huffman_lit_table[pos[index]++] = (INT)i;
        }
    }

    memset(pos, -1, 16 * sizeof(INT));

    for (i = 0; i < hdist; i++)
    {
        index = png->huffman_dist_code_len[i];
        if (index)
        {
            if (pos[index] == -1)
            {
                pos[index] = png->huffman_dist_bits_count[index - 1];
            }

            if (pos[index] >= GX_PNG_HUFFMAN_DIST_TABLE_SIZE)
            {
                /* Invalid data. */
                return GX_FAILURE;
            }

            png->huffman_dist_table[pos[index]++] = (INT)i;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_huffcode_decode                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function decodes the Huffman codes.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block.            */
/*    dynamic                               True, use dynamic huff table  */
/*                                          False, use fixed huff table   */
/*    decoded_data_size                     Expected decoded data size    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_png_bits_get         Extract bits from PNG data    */
/*                                            stream                      */
/*    _gx_image_reader_png_bits_skip        Skip bits from PNG data stream*/
/*    _gx_image_reader_png_dynamic_huffman_code_find                      */
/*                                          Retrieve code value from      */
/*                                            dynamic huffman table       */
/*    _gx_image_reader_png_fixed_ll_huffman_code_find                     */
/*                                          Retrieve code value from      */
/*                                            fixed huffman table         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_png_IDAT_chunk_read                                */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_png_huffcode_decode(GX_PNG *png, GX_BOOL dynamic, UINT decoded_data_size)
{
UINT scan_buffer;
UINT bit_len = 0;
UINT code_value = 300;
UINT length;
UINT distance;
UINT extra_bits;
UINT copy_len;

    for (;;)
    {
        if (_gx_image_reader_png_bits_get(png, 16, &scan_buffer) != GX_SUCCESS)
        {
            return GX_FAILURE;
        }

        /* Decode literal/length value from input stream */
        if (dynamic)
        {
            _gx_image_reader_png_dynamic_huffman_code_find(png->huffman_lit_table,
                                                           (UINT *)png->huffman_lit_bits_count,
                                                           scan_buffer,
                                                           &bit_len,
                                                           &code_value);
        }
        else
        {
            _gx_image_reader_png_fixed_ll_huffman_code_find(scan_buffer, &bit_len, &code_value);
        }

        _gx_image_reader_png_bits_skip(bit_len);

        if (code_value < 256)
        {
            if ((UINT)(png->decoded_data_len + 1) > decoded_data_size)
            {
                /* Decoded data size exceed allocated buffer size. */
                return GX_FAILURE;
            }

            png->decoded_data[png->decoded_data_len++] = (GX_UBYTE)code_value;
        }
        else if (code_value == 256)
        {
            /* End of the block */
            break;
        }
        else
        {
            if (code_value < 265)
            {
                /* 0 extra bits */
                length = 3 + code_value - 257;
            }
            else if (code_value < 285)
            {
                extra_bits = 1 + (code_value - 265) / 4;
                _gx_image_reader_png_bits_get(png, extra_bits, &length);
                _gx_image_reader_png_bits_skip(extra_bits);
                length <<= (32 - extra_bits);
                length >>= (32 - extra_bits);

                length += 3u + (1u << (extra_bits + 2)) + ((1u << extra_bits) * ((code_value - 265) & 0x3));
            }
            else if (code_value == 285)
            {
                /* 0 extra bits */
                length = 258;
            }
            else
            {
                /* This should not happen. */
                return GX_FAILURE;
            }

            if (dynamic)
            {
                _gx_image_reader_png_bits_get(png, 16, &scan_buffer);

                /*decode distance from input stream */
                _gx_image_reader_png_dynamic_huffman_code_find(png->huffman_dist_table,
                                                               (UINT *)png->huffman_dist_bits_count,
                                                               scan_buffer,
                                                               &bit_len,
                                                               &code_value);

                _gx_image_reader_png_bits_skip(bit_len);
            }
            else
            {
                /* Decodes 5-bits distance codes. */
                _gx_image_reader_png_bits_get(png, 5, &scan_buffer);
                _gx_image_reader_png_bits_skip(5);
                code_value = scan_buffer & 0x1f;
                _gx_image_reader_png_bits_revert(&code_value, 5);
            }

            if (code_value < 4)
            {
                distance = 1 + code_value;
            }
            else if (code_value < 30)
            {
                extra_bits = 1 + (code_value - 4) / 2;

                _gx_image_reader_png_bits_get(png, extra_bits, &distance);
                _gx_image_reader_png_bits_skip(extra_bits);

                distance <<= (32 - extra_bits);
                distance >>= (32 - extra_bits);

                distance += 1u + (1u << (extra_bits + 1)) + (1u << extra_bits) * ((code_value - 4) & 0x1);
            }
            else
            {
                /* This should not happen. */
                return GX_FALSE;
            }

            /* move backwards distance bytes in the output stream, and copy
               length bytes from this position to the output stream. */

            while (length)
            {
                if (length > distance)
                {
                    copy_len = distance;
                    length -= distance;
                }
                else
                {
                    copy_len = length;
                    length = 0;
                }

                if ((distance > (UINT)png->decoded_data_len) ||
                    ((UINT)png->decoded_data_len + copy_len > decoded_data_size))
                {

                    /* Distance exceed current decoded data length or copied length exceed remaining buffer size. */
                    return GX_FAILURE;
                }

                memcpy(png->decoded_data + png->decoded_data_len, /* Use case of memcpy is verified. */
                       png->decoded_data + png->decoded_data_len - distance,
                       copy_len * sizeof(GX_UBYTE));
                png->decoded_data_len += (INT)copy_len;
            }
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_tRNS_chunk_read                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads in the tRNS chunk information.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block.            */
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
/*    _gx_image_reader_png_decode                                         */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_png_tRNS_chunk_read(GX_PNG *png)
{
INT index;

    if ((png->color_type == 4) || (png->color_type == 6))
    {
        /* tRNS is prohibited for the other color types. */
        return GX_FAILURE;
    }

    if (png->color_type == 3)
    {
        /* Contains a series of one-byte alpha values.  */
        png->trans_num = png->trunk_end_index - png->data_index;

        /* For color type 3, tRNS can contain fewer values than there are palette entries. */
        if (png->trans_num > 256)
        {
            return GX_INVALID_VALUE;
        }
    }
    else
    {
        /* For color type 0, the tRNS chunk contains a single two-bytes gray level value.
           For color type 2, the tRNS chunk contains a single two-bytes RGB color value. */
        png->trans_num = (png->trunk_end_index - png->data_index) >> 1;

        if (png->trans_num > 3)
        {
            return GX_INVALID_VALUE;
        }
    }

    /* Then trans num is no bigger than 256, multiplication will not overflow. */
    png->trans = (GX_COLOR *)_gx_system_memory_allocator((ULONG)png->trans_num * sizeof(GX_COLOR));

    if (png->trans == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    for (index = 0; index < png->trans_num; index++)
    {
        png->trans[index] = png->data[png->data_index++];

        if (png->color_type != 3)
        {
            png->trans[index] <<= 8;
            png->trans[index] |= png->data[png->data_index++];
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_IDAT_chunk_read                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads in the IDAT chunk information and start         */
/*    decoding.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block.            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_png_bits_get         Extract bits from PNG data    */
/*                                            stream                      */
/*    _gx_image_reader_png_bits_skip        Skip bits from PNG data stream*/
/*    _gx_image_reader_png_clen_huffman_read                              */
/*                                          Read code length huffman table*/
/*    _gx_image_reader_png_ll_huffman_read  Read literal and length       */
/*                                            huffman table               */
/*    _gx_image_reader_png_huffcode_decode  Decode huffman codes          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_png_decode                                         */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_png_IDAT_chunk_read(GX_PNG *png)
{
/*zlib format
   +---+---+
   |CMF|FLG|
   +---+---+
   if FLG.FDICT set
   +---+---+---+---+
   |     DTCTID    |
   +---+---+---+---+
   +=====================+---+---+---+---+
   |...compressed data...| 32bit checksum|
   +=====================+---+---+---+---+
*/
UINT CMF;
UINT FLG;
INT  compress_method;
INT  window_size;
INT  fdict;
UINT header;
INT  hlit;
INT  hdist;
INT  hclen;
UINT alloc_size;
INT  len;
INT  nlen;

    if (_gx_image_reader_png_bits_get(png, 8, &CMF) != GX_SUCCESS)
    {
        return GX_FAILURE;
    }

    _gx_image_reader_png_bits_skip(8);

    if (_gx_image_reader_png_bits_get(png, 8, &FLG) != GX_SUCCESS)
    {
        return GX_FAILURE;
    }

    _gx_image_reader_png_bits_skip(8);

    compress_method = CMF & 0x0f;
    window_size = 2 << (((CMF & 0xf0) >> 4) + 8);
    fdict = FLG & 0x20;

    if ((fdict != 0) ||
        (window_size > 65536) ||
        (compress_method != 8))
    {
        return GX_FAILURE;
    }

    /* Safe int math is not required here, calling function limits max width, height to 14 bits so
       overflow cannot occur. */
    alloc_size = (UINT)(png->height * ((png->width * png->bpp + 7) / 8));
    alloc_size = alloc_size + (UINT)png->height;
    png->decoded_data = (GX_UBYTE *)_gx_system_memory_allocator(alloc_size);
    png->decoded_data_len = 0;

    if (png->decoded_data == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    do
    {
        /* The header of one block */
        if (_gx_image_reader_png_bits_get(png, 8, &header) != GX_SUCCESS)
        {
            return GX_FAILURE;
        }

        if ((header & 0x6) == 0x00)
        {
            _gx_image_reader_png_bits_skip(3);

            /* no compression */
            _gx_image_reader_png_bits_skip(_bit_count & 0x7);
            if (_bit_count)
            {
                png->data_index -= (INT)(_bit_count / 8);
                _gx_image_reader_png_bits_skip(_bit_count);
            }

            /* 4 = 2 bytes len + 2 bytes nlen. */
            if (png->data_index + 4 > png->data_size)
            {
                return GX_INVALID_SIZE;
            }

            /* Read "LEN".  */
            len = png->data[png->data_index];
            png->data_index++;
            len |= (png->data[png->data_index] << 8);
            png->data_index++;

            /* Read "NLEN".  */
            nlen = png->data[png->data_index];
            png->data_index++;
            nlen |= (png->data[png->data_index] << 8);
            png->data_index++;

            if (len + nlen != 65535)
            {
                /* NLEN is not one's complement of LEN. */
                return GX_FAILURE;
            }

            /* Read data to outbuffer directly */
            if (len)
            {
                if ((UINT)(png->decoded_data_len + len) > alloc_size)
                {
                    /* Copied size exceed remaining buffer size. */
                    return GX_FAILURE;
                }

                if (png->data_index + len > png->data_size)
                {
                    return GX_INVALID_SIZE;
                }

                memcpy(png->decoded_data + png->decoded_data_len, png->data + png->data_index, (UINT)len); /* Use case of memcpy is verified. */
                png->data_index += len;
                png->decoded_data_len += len;
            }
        }
        else if ((header & 0x6) == 0x2)
        {
            _gx_image_reader_png_bits_skip(3);

            /* compressed with fixed Huffman codes */
            if (_gx_image_reader_png_huffcode_decode(png, GX_FALSE, alloc_size) != GX_SUCCESS)
            {
                return GX_FAILURE;
            }
        }
        else if ((header & 0x6) == 0x4)
        {
            /* compressed with dynamic Huffman codes */
            _gx_image_reader_png_bits_skip(3);

            /* # of Literal/Length codes */
            _gx_image_reader_png_bits_get(png, 5, (unsigned int *)&hlit);

            _gx_image_reader_png_bits_skip(5);
            hlit = (hlit & 0x1f) + 257;

            /* # of Distance codes */
            if (_gx_image_reader_png_bits_get(png, 5, (unsigned int *)&hdist) != GX_SUCCESS)
            {
                return GX_FAILURE;
            }
            _gx_image_reader_png_bits_skip(5);
            hdist = (hdist & 0x1f) + 1;

            /* # of Code Length codes */
            if (_gx_image_reader_png_bits_get(png, 4, (unsigned int *)&hclen) != GX_SUCCESS)
            {
                return GX_FAILURE;
            }
            _gx_image_reader_png_bits_skip(4);
            hclen = (hclen & 0xf) + 4;

            memset(png->huffman_clen_bits_count, 0, 17 * sizeof(INT));
            memset(png->huffman_clen_table, 0, 20 * sizeof(INT));
            memset(png->huffman_lit_bits_count, 0, 17 * sizeof(INT));
            memset(png->huffman_lit_table, 0, GX_PNG_HUFFMAN_LIT_TABLE_SIZE * sizeof(int));
            memset(png->huffman_dist_bits_count, 0, 17 * sizeof(INT));
            memset(png->huffman_dist_table, 0, 30 * sizeof(INT));

            if (_gx_image_reader_png_clen_huffman_read(png, (UINT)hclen) != GX_SUCCESS)
            {
                return GX_FAILURE;
            }

            if (_gx_image_reader_png_ll_huffman_read(png, (UINT)hlit, (UINT)hdist) != GX_SUCCESS)
            {
                return GX_FAILURE;
            }

            if (_gx_image_reader_png_huffcode_decode(png, GX_TRUE, alloc_size) != GX_SUCCESS)
            {
                return GX_FAILURE;
            }
        }
        else
        {
            /* error */
            return GX_FAILURE;
        }
    } while ((header & 0x1) != 0x1);

    /* Skip checksum */
    png->data_index = png->trunk_end_index;
    _gx_image_reader_png_bits_skip(_bit_count);

    if (alloc_size != (UINT)png->decoded_data_len)
    {
        /* Decoded data size doesn't match prediction. */
        return GX_FAILURE;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_gAMA_chunk_read                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads in the gAMA chunk information.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block.            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_png_4bytes_read      Read 4 bytes and pact it to   */
/*                                            an INT value                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_png_decode                                         */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_png_gAMA_chunk_read(GX_PNG *png)
{
    if (png->data_index + 4 > png->trunk_end_index)
    {
        return GX_INVALID_SIZE;
    }

    /* Specifies the gamma of the camera that produced the image. */

    /* A gamma of 0.45 would be stored as teh integer 45000. */
    _gx_image_reader_png_4bytes_read(png, &png->gamma);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_PLTE_chunk_read                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads in the PLTE chunk information.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block.            */
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
/*    _gx_image_reader_png_decode                                         */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_png_PLTE_chunk_read(GX_PNG *png)
{
INT      index;
GX_UBYTE red;
GX_UBYTE green;
GX_UBYTE blue;

    png->palette_table_size = (png->trunk_end_index - png->data_index) / 3;

    /* The PLTE chunk contains from 1 to 256 palette entries. */
    if (png->palette_table_size > 256)
    {
        return GX_INVALID_VALUE;
    }

    for (index = 0; index < png->palette_table_size; index++)
    {
        red = png->data[png->data_index++];
        green = png->data[png->data_index++];
        blue = png->data[png->data_index++];

        png->palette_table[index] = (GX_COLOR)((red << 16) | (green << 8) | blue);
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_paeth_predictor                                */
/*                                                           6.1.8        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function performs Paeth filter algorithm.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    a                                     Byte of left pixel.           */
/*    b                                     Byte of above pixel.          */
/*    c                                     Byte of upper left pixel.     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                The nearest of a, b, c.       */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_png_unfilter                                       */
/*                                                                        */
/**************************************************************************/
static GX_UBYTE _gx_image_reader_png_paeth_predictor(GX_UBYTE a, GX_UBYTE b, GX_UBYTE c)
{
INT p;
INT pa, pb, pc;

    p = a + b - c;
    pa = GX_ABS(p - a);
    pb = GX_ABS(p - b);
    pc = GX_ABS(p - c);

    /*return nearest of a, b, c */

    if ((pa <= pb) && (pa <= pc))
    {
        return a;
    }
    else if (pb <= pc)
    {
        return b;
    }
    else
    {
        return c;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_unfilter                                       */
/*                                                           6.2.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reverts the effect of filter.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    png                                   PNG control block.            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memmove                                                             */
/*    _gx_image_reader_png_paeth_predictor  Perform Paeth filter algorithm*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_png_decode                                         */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_png_unfilter(GX_PNG *png)
{
INT filter_type;
INT byte_width;
INT pos;
INT bpp = png->bpp;
INT x;
INT y;

    if (png->decoded_data == GX_NULL)
    {
        /* This happens when IDAT chunk is missing. */
        return GX_FAILURE;
    }

    byte_width = (png->width * bpp + 7) >> 3;
    bpp = (bpp + 7) >> 3;

    for (y = 0; y < png->height; y++)
    {
        filter_type = png->decoded_data[y * byte_width + y];

        pos = y * byte_width;
        memmove(png->decoded_data + pos, png->decoded_data + pos + y + 1, (UINT)byte_width);

        switch (filter_type)
        {
        case 0:
            break;
        case 1:
            /* sub(x) + raw(x-bpp)*/
            for (x = bpp; x < byte_width; x++)
            {
                png->decoded_data[pos + x] = (GX_UBYTE)(png->decoded_data[pos + x] + png->decoded_data[pos + x - bpp]);
            }
            break;
        case 2:
            /*up(x) + prior(x)*/
            if (y == 0)
            {
                break;
            }

            for (x = 0; x < byte_width; x++)
            {
                png->decoded_data[pos + x] = (GX_UBYTE)(png->decoded_data[pos + x] + png->decoded_data[pos + x - byte_width]);
            }
            break;
        case 3:
            /* average(x) + floor((raw(x-bpp)+prior(x))/2 */
            if (y == 0)
            {
                for (x = bpp; x < byte_width; x++)
                {
                    png->decoded_data[pos + x] = (GX_UBYTE)(png->decoded_data[pos + x] + (png->decoded_data[pos + x - bpp] >> 1));
                }
            }
            else
            {
                for (x = 0; x < bpp; x++)
                {
                    png->decoded_data[pos + x] = (GX_UBYTE)(png->decoded_data[pos + x] + (png->decoded_data[pos + x - byte_width] >> 1));
                }

                for (x = bpp; x < byte_width; x++)
                {
                    png->decoded_data[pos + x] = (GX_UBYTE)(png->decoded_data[pos + x] + ((png->decoded_data[pos + x - bpp] +
                                                                                                             png->decoded_data[pos + x - byte_width]) >> 1));
                }
            }
            break;
        case 4:
            /* paeth(x) + paethpredictor(raw(x-bpp), prior(x), prior(x-bpp)) */
            if (y == 0)
            {
                for (x = bpp; x < byte_width; x++)
                {
                    png->decoded_data[pos + x] = (GX_UBYTE)(png->decoded_data[pos + x] + png->decoded_data[pos + x - bpp]);
                }
            }
            else
            {
                for (x = 0; x < bpp; x++)
                {
                    png->decoded_data[pos + x] = (GX_UBYTE)(png->decoded_data[pos + x] + png->decoded_data[pos + x - byte_width]);
                }

                for (x = bpp; x < byte_width; x++)
                {
                    png->decoded_data[pos + x] = (GX_UBYTE)(png->decoded_data[pos + x] + _gx_image_reader_png_paeth_predictor(png->decoded_data[pos + x - bpp],
                                                                                                                                                png->decoded_data[pos + x - byte_width],
                                                                                                                                                png->decoded_data[pos + x - byte_width - bpp]));
                }
            }
            break;
        default:
            return GX_FAILURE;
        }
    }

    png->decoded_data_len -= png->height;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_png_decode                                         */
/*                                                           6.2.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function decode a PNG format image.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    inmap                                 Input pixelmap                */
/*    outmap                                Output pixelmap               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    strncmp                                                             */
/*    _gx_system_memory_allocator           Application defined memory    */
/*                                            allocation function         */
/*    _gx_system_memory_free                Application defined memory    */
/*                                            free function               */
/*    _gx_image_reader_png_crc_table_make   Create a crc table            */
/*    _gx_image_reader_png_crc_get          Calculate crc value of        */
/*                                            specified data              */
/*    _gx_image_reader_png_4bytes_read      Read 4 bytes and pack it to   */
/*                                            INT type value              */
/*    _gx_image_reader_png_chunk_type_read  Read chunk type information   */
/*    _gx_image_reader_png_IDAT_chunk_read  Read IDAT chunk information   */
/*                                            and start decoding          */
/*    _gx_image_reader_png_IHDR_chunk_read  Read IHDR chunk information   */
/*    _gx_image_reader_png_gAMA_chunk_read  Read gAMA chunk information   */
/*    _gx_image_reader_png_PLTE_chunk_read  Read PLTE chunk information   */
/*    _gx_image_reader_png_tRNS_chunk_read  Read tRNS chunk information   */
/*    _gx_image_reader_png_unfilter         Revert effect of filter       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_image_decode                                       */
/*                                                                        */
/**************************************************************************/
UINT _gx_image_reader_png_decode(const GX_UBYTE *read_data, ULONG data_size, GX_PIXELMAP *outmap)
{
UINT    status = GX_SUCCESS;
GX_PNG  png;
INT     data_len;
CHAR    chunk_type[4];
INT     checksum;
GX_BOOL idat_done = GX_FALSE;
INT    *scratch_buffer;
INT     scratch_index = 0;
GX_BOOL decoded_done = GX_FALSE;

    if (data_size < 8)
    {
        return GX_INVALID_SIZE;
    }

    if ((read_data[0] == 137) &&
        (read_data[1] == 80) &&
        (read_data[2] == 78) &&
        (read_data[3] == 71) &&
        (read_data[4] == 13) &&
        (read_data[5] == 10) &&
        (read_data[6] == 26) &&
        (read_data[7] == 10))
    {
        if (!_gx_system_memory_allocator)
        {
            return GX_SYSTEM_MEMORY_ERROR;
        }

        memset(&png, 0, sizeof(GX_PNG));
        scratch_buffer = (INT *)_gx_system_memory_allocator(GX_PNG_SCRATCH_BUFFER_SIZE * sizeof(ULONG));

        if (scratch_buffer == GX_NULL)
        {
            return GX_SYSTEM_MEMORY_ERROR;
        }

        png.crc_table = (UINT *)scratch_buffer;
        scratch_index += GX_PNG_CRC_TABLE_SIZE;
        png.huffman_lit_table = scratch_buffer + scratch_index;
        scratch_index += GX_PNG_HUFFMAN_LIT_TABLE_SIZE;
        png.huffman_lit_code_len = scratch_buffer + scratch_index;
        scratch_index += GX_PNG_HUFFMAN_LIT_CODE_LEN_TABLE_SIZE;
        png.huffman_dist_code_len = scratch_buffer + scratch_index;
        scratch_index += GX_PNG_HUFFMAN_DIST_CODE_LEN_TABLE_SIZE;
        png.palette_table = (GX_COLOR *)scratch_buffer + scratch_index;

        png.data = (GX_UBYTE *)read_data;
        png.data_size = (INT)data_size;
        png.data_index = 8;

        _gx_image_reader_png_crc_table_make(&png);

        _bit_buffer = 0;
        _bit_count = 0;

        while (png.data_index < png.data_size - 4)
        {
            /* data_len*/
            _gx_image_reader_png_4bytes_read(&png, &data_len);

            png.trunk_crc = 0xffffffff;

            if ((data_len < 0) ||
                (data_len > png.data_size - png.data_index - 4))
            {
                status = GX_INVALID_SIZE;
                break;
            }

            /* Calculate checksum of the chunk data.  */
            _gx_image_reader_png_crc_get(&png, &png.trunk_crc, (UINT)(4 + data_len));

            /* Read chunk type.  */
            _gx_image_reader_png_chunk_type_read(&png, chunk_type);

            png.trunk_end_index = png.data_index + data_len;

            if ((strncmp(chunk_type, "IDAT", 4) == 0) && (!idat_done))
            {
                idat_done = GX_TRUE;

                status = _gx_image_reader_png_IDAT_chunk_read(&png);
            }
            else if (strncmp(chunk_type, "IHDR", 4) == 0)
            {
                status = _gx_image_reader_png_IHDR_chunk_read(&png);
            }
            else if (strncmp(chunk_type, "gAMA", 4) == 0)
            {
                status = _gx_image_reader_png_gAMA_chunk_read(&png);
            }
            else if (strncmp(chunk_type, "PLTE", 4) == 0)
            {
                status = _gx_image_reader_png_PLTE_chunk_read(&png);
            }
            else if ((strncmp(chunk_type, "tRNS", 4) == 0) && (png.trans == GX_NULL))
            {
                /* Read transparent information. */
                status = _gx_image_reader_png_tRNS_chunk_read(&png);
            }
            else if (strncmp(chunk_type, "IEND", 4) == GX_SUCCESS)
            {
                /* End */
                decoded_done = GX_TRUE;
                break;
            }
            else
            {
                /* Ignore chunk */
                png.data_index += data_len;
            }

            if (status != GX_SUCCESS)
            {
                break;
            }

            _gx_image_reader_png_4bytes_read(&png, &checksum);

            if (checksum != (INT)png.trunk_crc)
            {
                status = GX_FAILURE;
                break;
            }
        }

        if(status == GX_SUCCESS && (!decoded_done))
        {
            status = GX_FAILURE;
        }

        if (status == GX_SUCCESS)
        {
            /* revert filter */
            status = _gx_image_reader_png_unfilter(&png);
        }

        if (status == GX_SUCCESS)
        {
            outmap->data = png.decoded_data;
            outmap->data_size = (ULONG)png.decoded_data_len;
            outmap->width = (GX_VALUE)png.width;
            outmap->height = (GX_VALUE)png.height;
            outmap->flags = GX_PIXELMAP_RAW_FORMAT;

            if (png.trans_num)
            {
                outmap->flags |= GX_PIXELMAP_ALPHA;
                outmap->transparent_color = (ULONG)png.trans;
            }

            switch (png.color_type)
            {
            case 6:
                /* Each pixel is an RGB triple, followed by an alpha sample. */
                if (png.bit_depth == 8)
                {
                    outmap->format = GX_IMAGE_FORMAT_32BPP;
                }
                else
                {
                    /* Bit depth = 16. */
                    outmap->format = GX_IMAGE_FORMAT_64BPP;
                }
                outmap->flags |= GX_PIXELMAP_ALPHA;
                break;
            case 4:
                /* Each pixel is a grayscale sample followed by an alpha sample. */
                if (png.bit_depth == 8)
                {
                    outmap->format = GX_IMAGE_FORMAT_16BPP_GRAY_ALPHA;
                }
                else
                {
                    /* Bit depth = 16. */
                    outmap->format = GX_IMAGE_FORMAT_32BPP_GRAY_ALPHA;
                }
                outmap->flags |= GX_PIXELMAP_ALPHA;
                break;
            case 3:
                /* Each pixel is a palette index */
                switch (png.bit_depth)
                {
                case 1:
                    outmap->format = GX_IMAGE_FORMAT_1BPP;
                    break;
                case 2:
                    outmap->format = GX_IMAGE_FORMAT_2BPP;
                    break;
                case 4:
                    outmap->format = GX_IMAGE_FORMAT_4BPP;
                    break;
                default:
                    /* Bit depth = 8. */
                    outmap->format = GX_IMAGE_FORMAT_8BPP;
                    break;
                }

                /* Palette table size is no bigger than 256, aux data size will not overflow. */
                outmap->aux_data_size = (ULONG)(png.palette_table_size) * sizeof(GX_COLOR);
                outmap->aux_data = (GX_UBYTE *)_gx_system_memory_allocator(outmap->aux_data_size);

                if (!outmap->aux_data)
                {
                    return GX_SYSTEM_MEMORY_ERROR;
                }

                memcpy((GX_UBYTE *)outmap->aux_data, png.palette_table, outmap->aux_data_size); /* Use case of memcpy is verified. */
                break;
            case 2:
                /* Each pixel is an RGB triple. */
                if (png.bit_depth == 8)
                {
                    outmap->format = GX_IMAGE_FORMAT_24BPP;
                }
                else
                {
                    /* Bit depth = 16. */
                    outmap->format = GX_IMAGE_FORMAT_48BPP;
                }
                break;
            default:
                /* Each pixel is a grayscale sample. */
                switch (png.bit_depth)
                {
                case 16:
                    outmap->format = GX_IMAGE_FORMAT_16BPP_GRAY;
                    break;
                case 8:
                    outmap->format = GX_IMAGE_FORMAT_8BPP;
                    break;
                case 4:
                    outmap->format = GX_IMAGE_FORMAT_4BPP;
                    break;
                case 2:
                    outmap->format = GX_IMAGE_FORMAT_2BPP;
                    break;
                default:
                    /* Bit depth = 1. */
                    outmap->format = GX_IMAGE_FORMAT_1BPP;
                    break;
                }
                break;
            }
        }

        if (status != GX_SUCCESS)
        {
            if (png.decoded_data)
            {
                _gx_system_memory_free(png.decoded_data);
            }

            if (png.trans)
            {
                _gx_system_memory_free(png.trans);
            }
        }

        /* Free scratch buffer. */
        _gx_system_memory_free(scratch_buffer);
    }
    else
    {
        return GX_FAILURE;
    }

    return status;
}
#endif

#if defined(GX_SOFTWARE_DECODER_SUPPORT)

#define PIXEL_CMP(_p1, _p2)                       \
    (_p1.alpha == _p2.alpha) && \
    (_p1.red == _p2.red) &&     \
    (_p1.green == _p2.green) && \
    (_p1.blue == _p2.blue)

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_rle_count_location_get                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves the count put position.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    put_count                             Retrieved count put position. */
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
/*    _gx_image_reader_rle_one_row_encode                                 */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_rle_count_location_get(GX_IMAGE_READER *image_reader, GX_UBYTE **put_count)
{
    switch (image_reader->color_format)
    {
    case GX_COLOR_FORMAT_32ARGB:
    case GX_COLOR_FORMAT_24XRGB:
        (*put_count) = image_reader->putauxdata++;
        break;

    case GX_COLOR_FORMAT_565RGB:
    case GX_COLOR_FORMAT_1555XRGB:
        if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
        {
            (*put_count) = image_reader->putdata;
        }
        else
        {
            (*put_count) = image_reader->putdata;
            image_reader->putdata += 2;
        }
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        (*put_count) = image_reader->putdata;
        image_reader->putdata += 2;
        break;

    case GX_COLOR_FORMAT_8BIT_GRAY:
    case GX_COLOR_FORMAT_8BIT_ALPHAMAP:
    case GX_COLOR_FORMAT_8BIT_PALETTE:
        (*put_count) = image_reader->putdata++;
        break;

    case GX_COLOR_FORMAT_4BIT_GRAY:
        (*put_count) = image_reader->putauxdata++;
        break;

    case GX_COLOR_FORMAT_MONOCHROME:
        (*put_count) = image_reader->putdata;
        break;

    default:
        return GX_NOT_SUPPORTED;
    }

    return GX_SUCCESS;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_rle_count_write                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function writes repeat count to output pixelmap data structure.*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    count                                 The number of pixels that     */
/*                                            follows.                    */
/*    put_count                             The position to put "count".  */
/*    repeat                                Wheather the following pixels */
/*                                            are repeated or not.        */
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
/*    _gx_image_reader_rle_one_row_encode                                 */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_rle_count_write(GX_IMAGE_READER *image_reader, INT *count,
                                             GX_UBYTE *put_count, GX_BOOL repeat)
{
INT write_count;

    write_count = (*count) - 1;

    switch (image_reader->color_format)
    {
    case GX_COLOR_FORMAT_565RGB:
    case GX_COLOR_FORMAT_1555XRGB:
        if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
        {
            if (write_count > 127)
            {
                write_count = 127;
            }

            if (repeat)
            {
                write_count |= 0x80;
            }

            if (!image_reader->size_testing)
            {
                *put_count = (GX_UBYTE)write_count;
            }

            (*count) = (write_count & 0x7f) + 1;
        }
        else
        {
            if (repeat)
            {
                write_count |= 0x8000;
            }

            if (!image_reader->size_testing)
            {
                *((USHORT *)put_count) = (USHORT)write_count;
            }

            (*count) = (write_count & 0x7fff) + 1;
        }
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        if (repeat)
        {
            write_count |= 0x8000;
        }

        if (!image_reader->size_testing)
        {
            *((USHORT *)put_count) = (USHORT)write_count;
        }

        *count = (write_count & 0x7fff) + 1;
        break;

    case GX_COLOR_FORMAT_MONOCHROME:
        if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
        {
            if (write_count > 0x1f)
            {
                write_count = 0x1f;
            }
            if (repeat)
            {
                write_count |= 0x20;
            }
            if (!image_reader->size_testing)
            {
                *put_count |= (GX_UBYTE)(write_count << 2);
            }

            (*count)  = (write_count & 0x1f) + 1;
        }
        else
        {
            if (write_count > 0x3f)
            {
                write_count = 0x3f;
            }
            if (repeat)
            {
                write_count |= 0x40;
            }
            if (!image_reader->size_testing)
            {
                (*put_count) |= (GX_UBYTE)(write_count << 1);
            }

            (*count)  = (write_count & 0x3f) + 1;
        }
        break;

    case GX_COLOR_FORMAT_4BIT_GRAY:
    default:
        if (write_count > 127)
        {
            write_count = 127;
        }

        if (repeat)
        {
            write_count |= 0x80;
        }

        if (!image_reader->size_testing)
        {
            (*put_count) = (GX_UBYTE)write_count;
        }

        (*count) = (write_count & 0x7f) + 1;
        break;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_rle_duplicates_count                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function calculates the number of duplicate pixels.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                         Image reader control block.    */
/*    index                                Position of the pixel to       */
/*                                           calcualte from.              */
/*    repeat_counts                        The number of duplicate pixels.*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_rle_one_row_encode                                 */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_rle_duplicates_count(GX_IMAGE_READER *image_reader, INT index, INT *repeat_counts)
{
UINT     width;
INT      duplicates = 1;
GX_PIXEL pre_pixel;
GX_PIXEL cur_pixel;


    image_reader->pixel_read(image_reader, index++, &pre_pixel);

    width = image_reader->image_width;

    while ((UINT)index < width)
    {
        image_reader->pixel_read(image_reader, index, &cur_pixel);

        if (PIXEL_CMP(pre_pixel, cur_pixel))
        {
            duplicates++;
        }
        else
        {
            break;
        }
        index++;
    }

    (*repeat_counts) = duplicates;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_rle_one_row_encode                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This funcitons encodes one row of the input pixelmap.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_rle_duplicates_count Calculate the number of       */
/*                                            duplicate pixels            */
/*    _gx_image_reader_rle_count_write      Write out duplicated pixel    */
/*                                            count                       */
/*    _gx_image_reader_rle_count_location_get                             */
/*                                          Retrieve the address to put   */
/*                                            duplicated pixel count      */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_rle_encode                                         */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_rle_one_row_encode(GX_IMAGE_READER *image_reader)
{
UINT      status = GX_SUCCESS;
UINT      col = 0;
INT       raw_count = 0;
INT       count = 0;
GX_UBYTE *putCount = GX_NULL;
UINT      width;
GX_PIXEL  pixel;

    width = image_reader->image_width;

    while (col < width)
    {
        /* Calculate the number of duplicate pixels. */
        _gx_image_reader_rle_duplicates_count(image_reader, (INT)col, &count);

        if (count >= 3)
        {
            if (raw_count)
            {
                _gx_image_reader_rle_count_write(image_reader, &raw_count, putCount, GX_FALSE);
            }

            /* Retrieve the count put position.  */
            status = _gx_image_reader_rle_count_location_get(image_reader, &putCount);

            if (status != GX_SUCCESS)
            {
                return status;
            }

            /* Write count. */
            _gx_image_reader_rle_count_write(image_reader, &count, putCount, GX_TRUE);

            image_reader->pixel_read(image_reader, (INT)col, &pixel);

            /* Write pixel value. */
            image_reader->pixel_write(image_reader, &pixel);

            col += (UINT)count;
            raw_count = 0;
        }
        else
        {

            if (!raw_count)
            {
                status = _gx_image_reader_rle_count_location_get(image_reader, &putCount);

                if (status != GX_SUCCESS)
                {
                    return status;
                }
            }

            raw_count++;

            image_reader->pixel_read(image_reader, (INT)col, &pixel);

            /* Write pixel value. */
            image_reader->pixel_write(image_reader, &pixel);

            col++;

            if (raw_count == 128 || col == width)
            {
                _gx_image_reader_rle_count_write(image_reader, &raw_count, putCount, GX_FALSE);

                raw_count = 0;
            }
        }
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_rle_encode_size_get                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function gets the size of encoded pixelmap.                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    datasize                              Retrieved data size.          */
/*    auxsize                               Retrieved aux data size.      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_rle_one_row_encode    Process one row encoding     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_rle_colorspace_convert                             */
/*                                                                        */
/**************************************************************************/
UINT _gx_image_reader_rle_encode_size_get(GX_IMAGE_READER *image_reader, UINT *data_size, UINT *aux_size)
{
UINT status = GX_SUCCESS;
UINT row;
UINT height;
UINT width;
UINT aux_stride;


    height = image_reader->image_height;
    width = image_reader->image_width;
    aux_stride = width;

    /* Process rle encode logic, but not output compressed data. */
    image_reader->size_testing = GX_TRUE;
    image_reader->putdata = GX_NULL;
    image_reader->putauxdata = GX_NULL;

    switch (image_reader->color_format)
    {
    case GX_COLOR_FORMAT_4BIT_GRAY:
        if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
        {
            aux_stride = (aux_stride + 7) >> 3;
        }
        break;
    }

    /* Test size of compressed data to see if it is smaller.  */
    for (row = 0; row < height; row++)
    {
        status = _gx_image_reader_rle_one_row_encode(image_reader);

        if (status != GX_SUCCESS)
        {
            return status;
        }

        image_reader->getdata += image_reader->input_stride;
        image_reader->getauxdata += aux_stride;
    }

    if (image_reader->color_format == GX_COLOR_FORMAT_4BIT_GRAY)
    {
        if (!(image_reader->mode & GX_IMAGE_READER_MODE_ALPHA) &&
            image_reader->putdata_mask == 0x0f)
        {
            image_reader->putdata++;
        }
    }

    (*data_size) = (UINT)image_reader->putdata;
    (*aux_size) = (UINT)image_reader->putauxdata;

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_rle_encode                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function converts a raw map into a compressed map.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    outmap                                Encoded pixelmap.             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_rle_one_row_encode   Process one row encoding      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_rle_colorspace_convert                             */
/*                                                                        */
/**************************************************************************/
UINT _gx_image_reader_rle_encode(GX_IMAGE_READER *image_reader, GX_PIXELMAP *outmap)
{
UINT     status = GX_SUCCESS;
GX_VALUE height;
GX_VALUE width;
INT      row;
GX_VALUE aux_stride;

    height = outmap->height;
    width = outmap->width;
    aux_stride = width;

    /* If size testing is TRUE, process rle encode logic but not output compressed data.
       If size testing is FALSE, process rle encode logic, and output compressed data. */
    image_reader->size_testing = GX_FALSE;

    switch (image_reader->color_format)
    {
    case GX_COLOR_FORMAT_4BIT_GRAY:
        if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
        {
            aux_stride = (GX_VALUE)((aux_stride + 7) >> 3);
        }
        break;
    }

    /* Process RLE encode.  */
    for (row = 0; row < height; row++)
    {
        /* Encode one row data. */
        status = _gx_image_reader_rle_one_row_encode(image_reader);

        if (status != GX_SUCCESS)
        {
            return status;
        }

        image_reader->getdata += image_reader->input_stride;
        image_reader->getauxdata += aux_stride;
    }

    return status;
}
#endif

#if defined(GX_SOFTWARE_DECODER_SUPPORT)

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_rgb2gray                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function converts RGB value to grayscale.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pixel                                 RGB value                     */
/*    gray                                  Grayscale value to return     */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_image_reader_rgb2gray(GX_PIXEL *pixel, GX_UBYTE *gray)
{
    (*gray) = (GX_UBYTE)((pixel->red * 299 +
                          pixel->green * 587 +
                          pixel->blue * 114) / 1000);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_gray_threshold_calculate                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function calcualtes thredshold value used to create monochrome */
/*    pixlemap.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_image_reader_pixel_read]          Pixel read callback           */
/*    _gx_image_reader_rgb2gray             Conver RGB value to grayscale */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_start                                              */
/*                                                                        */
/**************************************************************************/
static void _gx_image_reader_gray_threshold_calculate(GX_IMAGE_READER *image_reader)
{
INT      threshold = 0;
INT      xval;
INT      yval;
INT      count = 0;
GX_PIXEL pixel;
GX_UBYTE gray;

    for (yval = 0; yval < (INT)image_reader->image_height; yval++)
    {
        for (xval = 0; xval < (INT)image_reader->image_width; xval++)
        {
            image_reader->pixel_read(image_reader, xval, &pixel);

            if (pixel.alpha > 128)
            {
                /* Convert RGB value to grayscale. */
                _gx_image_reader_rgb2gray(&pixel, &gray);
                threshold += gray;
                count++;
            }
        }

        image_reader->getdata += image_reader->input_stride;
    }

    if (count)
    {
        image_reader->mono_shreshold = (GX_UBYTE)(threshold / count);
    }

    if (image_reader->mono_shreshold == 0)
    {
        /* All opaque pixels are black. */
        image_reader->mono_shreshold = 255;
    }
    else if (image_reader->mono_shreshold == 255)
    {
        /* All opqaute pixels are white. */
        image_reader->mono_shreshold = 0;
    }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_pixelmap_info_set                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function prepares for image converting.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block    */
/*    outmap                                Pointer to a pixelmap that is */
/*                                            used to loading converted   */
/*                                            image                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_image_reader_start                                              */
/*                                                                        */
/**************************************************************************/
static UINT _gx_image_reader_pixelmap_info_set(GX_IMAGE_READER *image_reader, GX_PIXELMAP *outmap)
{
    outmap->width = (GX_VALUE)image_reader->image_width;
    outmap->height = (GX_VALUE)image_reader->image_height;
    outmap->data_size = 0;
    outmap->aux_data_size = 0;
    outmap->format = image_reader->color_format;

    /* Set alpha flag.  */
    if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
    {
        switch (outmap->format)
        {
        case GX_COLOR_FORMAT_8BIT_PALETTE:
        case GX_COLOR_FORMAT_4BIT_GRAY:
        case GX_COLOR_FORMAT_MONOCHROME:
            outmap->flags = GX_PIXELMAP_TRANSPARENT;
            outmap->transparent_color = GX_TRANSPARENT_COLOR;
            break;

        default:
            outmap->flags = GX_PIXELMAP_ALPHA;
            break;
        }
    }

    /* Calculate pixelmap data size, which will be used for memory allocation lator. Max width and height is limited to 14 bits,
       so overflow cannot occur. */
    switch (outmap->format)
    {
    case GX_COLOR_FORMAT_565RGB:
    case GX_COLOR_FORMAT_1555XRGB:
        outmap->data_size = (ULONG)(outmap->width * outmap->height * (INT)sizeof(USHORT));

        if (outmap->flags & GX_PIXELMAP_ALPHA)
        {
            outmap->aux_data_size = (ULONG)(outmap->width * outmap->height);
        }
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        outmap->data_size = (ULONG)(outmap->width * outmap->height * (INT)sizeof(USHORT));
        break;

    case GX_COLOR_FORMAT_32ARGB:
    case GX_COLOR_FORMAT_24XRGB:
        outmap->data_size = (ULONG)(outmap->width * outmap->height * (INT)sizeof(GX_COLOR));
        image_reader->mode = (GX_UBYTE)(image_reader->mode & (~GX_IMAGE_READER_MODE_DITHER));
        break;

    case GX_COLOR_FORMAT_8BIT_ALPHAMAP:
    case GX_COLOR_FORMAT_8BIT_PALETTE:
        outmap->data_size = (ULONG)(outmap->width * outmap->height);
        break;

    case GX_COLOR_FORMAT_4BIT_GRAY:
        outmap->data_size = (ULONG)(((outmap->width + 1) >> 1) * outmap->height);
        if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
        {
            outmap->aux_data_size = (ULONG)(((outmap->width + 7) >> 3) * outmap->height);
        }
        break;

    case GX_COLOR_FORMAT_MONOCHROME:
        if (image_reader->mode & GX_IMAGE_READER_MODE_ALPHA)
        {
            outmap->data_size = (ULONG)(((outmap->width + 3) >> 2) * outmap->height);
        }
        else
        {
            outmap->data_size = (ULONG)(((outmap->width + 7) >> 3) * outmap->height);
        }
        break;

    default:
        return GX_NOT_SUPPORTED;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_image_reader_start                                              */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function converts pixelmap to a specified color format.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block    */
/*    outmap                                Output pixelmap               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memcpy                                                              */
/*    memset                                                              */
/*    _gx_system_memory_allocator           Application defined memory    */
/*                                            allocation function         */
/*    _gx_system_memory_free                Application defined memory    */
/*                                            free function               */
/*    _gx_image_reader_image_decode         Decode specified image        */
/*    _gx_image_reader_pixel_read_callback_set                            */
/*                                          Set pixel read callback       */
/*    _gx_image_reader_pixel_write_callback_set                           */
/*                                          Set pixel write callback      */
/*    _gx_image_reader_dither               Process dither algorithm      */
/*    _gx_image_reader_rle_encode           Compress image with RLE       */
/*                                            algorithm                   */
/*    _gx_image_reader_raw_convert          Convert raw format map to     */
/*                                            specified format            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_image_reader_start(GX_IMAGE_READER *image_reader, GX_PIXELMAP *outmap)
{
UINT        status;
GX_PIXELMAP srcmap;
GX_PIXELMAP tempmap;
UINT        compressed_data_size;
UINT        compressed_aux_size;
UINT        raw_size;
GX_BOOL     do_compress;

    memset(outmap, 0, sizeof(GX_PIXELMAP));
    memset(&srcmap, 0, sizeof(GX_PIXELMAP));

    if (!_gx_system_memory_allocator)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    /* Decode input image to raw format. */
    status = _gx_image_reader_image_decode(image_reader, &srcmap);

    if (status == GX_SUCCESS)
    {
        /* Set output pixelmap information. */
        image_reader->image_width = (UINT)srcmap.width;
        image_reader->image_height = (UINT)srcmap.height;

        status = _gx_image_reader_pixelmap_info_set(image_reader, outmap);
    }

    if (image_reader->mode & GX_IMAGE_READER_MODE_COMPRESS)
    {
        do_compress = GX_TRUE;
    }
    else
    {
        do_compress = GX_FALSE;
    }

    if((image_reader->image_type == GX_IMAGE_TYPE_JPG && srcmap.format == GX_IMAGE_FORMAT_24BPP) ||
       (image_reader->image_type == GX_IMAGE_TYPE_PNG))
    {

        if (status == GX_SUCCESS)
        {
            if (image_reader->color_format == GX_COLOR_FORMAT_MONOCHROME)
            {
                status = _gx_image_reader_pixel_read_callback_set(image_reader, &srcmap);
                _gx_image_reader_gray_threshold_calculate(image_reader);
            }
        }

        image_reader->mode = (GX_UBYTE)(image_reader->mode & (~GX_IMAGE_READER_MODE_COMPRESS));

        /* Color Space Convert.  */
        if (status == GX_SUCCESS)
        {
            tempmap = *outmap;

            status = _gx_image_reader_pixel_read_callback_set(image_reader, &srcmap);
        }

        if (status == GX_SUCCESS)
        {
            status = _gx_image_reader_pixel_write_callback_set(image_reader, &tempmap);
        }

        if (status == GX_SUCCESS)
        {
            status = _gx_image_reader_colorspace_convert(image_reader, &tempmap);

            _gx_system_memory_free((void *)srcmap.data);

            if (srcmap.aux_data)
            {
                _gx_system_memory_free((void *)srcmap.aux_data);
            }

            srcmap = tempmap;
        }
    }

    /* Compare compressed size and raw size. */
    if (do_compress)
    {
        image_reader->mode |= GX_IMAGE_READER_MODE_COMPRESS;

        /* Seet pixel read and write callback.  */
        if (status == GX_SUCCESS)
        {
            status = _gx_image_reader_pixel_read_callback_set(image_reader, &srcmap);
        }

        if (status == GX_SUCCESS)
        {
            tempmap = *outmap;
            tempmap.data_size = 0;
            tempmap.aux_data_size = 0;
            tempmap.flags |= GX_PIXELMAP_COMPRESSED;

            status = _gx_image_reader_pixel_write_callback_set(image_reader, &tempmap);
        }

        /* Calculate the storage size that needed for rle encode. */
        if (status == GX_SUCCESS)
        {
            _gx_image_reader_rle_encode_size_get(image_reader, &compressed_data_size, &compressed_aux_size);

            raw_size = outmap->aux_data_size + outmap->data_size;

            /* Test wheather the encoded data size is smaller that raw size.  */
            if (compressed_aux_size + compressed_data_size < raw_size)
            {
                outmap->data_size = compressed_data_size;
                outmap->aux_data_size = compressed_aux_size;
                outmap->flags |= GX_PIXELMAP_COMPRESSED;

                _gx_image_reader_pixel_read_callback_set(image_reader, &srcmap);

                status = _gx_image_reader_pixel_write_callback_set(image_reader, outmap);

                if (status == GX_SUCCESS)
                {
                    status = _gx_image_reader_rle_encode(image_reader, outmap);
                }
            }
            else
            {
                *outmap = srcmap;
                memset(&srcmap, 0, sizeof(GX_PIXELMAP));
            }
        }
    }
    else
    {
        *outmap = srcmap;
        memset(&srcmap, 0, sizeof(GX_PIXELMAP));
    }

    /* Release memory that used to load decoded pixelmap data. */
    if (srcmap.data)
    {
        _gx_system_memory_free((void *)srcmap.data);
    }

    if (srcmap.aux_data)
    {
        _gx_system_memory_free((void *)srcmap.aux_data);
    }

    if (image_reader->png_trans)
    {
        _gx_system_memory_free((void *)image_reader->png_trans);
    }

    if (status != GX_SUCCESS)
    {
        if (outmap->data)
        {
            _gx_system_memory_free((void *)outmap->data);
            outmap->data = GX_NULL;
        }

        if (outmap->aux_data)
        {
            _gx_system_memory_free((void *)outmap->aux_data);
            outmap->aux_data = GX_NULL;
        }
    }

    return status;
}
#endif

