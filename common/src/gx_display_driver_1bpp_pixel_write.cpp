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
/**   Display Management (Display)                                        */
/**                                                                       */
/**************************************************************************/

#include "gx_display.h"

#include "gx_context.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_1bpp_pixel_write                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Pixel write function for the 1bpp display driver.                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    x                                     X coordinate                  */
/*    y                                     Y coordinate                  */
/*    color                                 Color of pixel to write       */
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
void _gx_display_driver_1bpp_pixel_write(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color)
{
GX_UBYTE *put = (GX_UBYTE *)context->memory;
GX_UBYTE  mask;
UINT      stride;

    /* Get row pitch in bytes. */
    stride = _gx_display_driver_1bpp_row_pitch_get((USHORT)(context->pitch));

    /* Calculate address of writing byte.  */
    put = (GX_UBYTE *)((INT)put + y * (INT)stride);
    put += x >> 3;

    mask = (GX_UBYTE)(0x80 >> (x & 0x07));

    /* Write the pixel value.  */
    if (color == 0x00)
    {
        *put = (GX_UBYTE)(*put & (~mask));
    }
    else if (color == 0x01)
    {
        *put |= mask;
    }
}

