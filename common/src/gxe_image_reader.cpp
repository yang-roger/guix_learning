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

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_image_reader_create                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in creating an image reader         */
/*    instance.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader              Image reader control block.               */
/*    read_data                 pointer to raw input data.                */
/*    read_data_size            size of raw input data.                   */
/*    color_format              the requested output color format         */
/*    mode                      compress dither and alpha modes flags     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                    Completion status                         */
/*                                                                        */
/*  CALLS                                                                 */
/*   _gx_image_reader_create    Actual image reader create call.          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
UINT  _gxe_image_reader_create(GX_IMAGE_READER *image_reader,
                               const GX_UBYTE *read_data,
                               INT read_data_size,
                               GX_UBYTE color_format,
                               GX_UBYTE mode)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((image_reader == GX_NULL) || (read_data == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (read_data_size <= 20)
    {
        return(GX_INVALID_VALUE);
    }

    /* Call the actual image reader info set function.  */
    status = _gx_image_reader_create(image_reader, read_data, read_data_size, color_format, mode);

    /* Return completion status.  */
    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_image_reader_palette_set                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the image reader palette set     */
/*    function.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                          Image reader control block.   */
/*    pal                                   Pointer to palette.           */
/*    palsize                               The size of pallete.          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                          Completion status                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_image_reader_info_set        Actual image reader palette set     */
/*                                    call.                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
UINT _gxe_image_reader_palette_set(GX_IMAGE_READER *image_reader, GX_COLOR *pal, UINT palsize)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((image_reader == GX_NULL) || (pal == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid input values.  */
    if (palsize == 0)
    {
        return(GX_INVALID_VALUE);
    }

    /* Call the actual image reader palette set function.  */
    status = _gx_image_reader_palette_set(image_reader, pal, palsize);

    /* Return completion status.  */
    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_image_reader_start                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the image reader start           */
/*    function.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader                    Image reader control block.         */
/*    outmap                          Converted pixelmap.                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                          Completion status                   */
/*                                                                        */
/*  CALLS                                                                 */
/*   _gx_image_reader_start           Actual image reader start call.     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
UINT  _gxe_image_reader_start(GX_IMAGE_READER *image_reader, GX_PIXELMAP *outmap)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((image_reader == GX_NULL) || (outmap == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual image reader colorspace convert function.  */
    status = _gx_image_reader_start(image_reader, outmap);

    /* Return completion status.  */
    return status;
}
#endif
