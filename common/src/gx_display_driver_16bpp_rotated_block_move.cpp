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
#include "gx_canvas.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_16bpp_rotated_block_move                         */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 16bpp color format display driver rotated block moving      */
/*    function.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Draw context                  */
/*    block                                 The rectangle to be moved     */
/*    xshift                                Amount to move on X-axis      */
/*    yshift                                Amount to move on Y-axis      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memmove                               Move a block of data          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_16bpp_rotated_block_move(GX_DRAW_CONTEXT *context,
                                                 GX_RECTANGLE *block, INT xshift, INT yshift)
{
USHORT      *pGet;
USHORT      *pPut;
INT          width;
INT          width_in_bytes;
INT          y;
INT          height;
GX_RECTANGLE rotated_block;

    GX_SWAP_VALS(xshift, yshift);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        rotated_block.left = block->top;
        rotated_block.right = block->bottom;
        rotated_block.top = (GX_VALUE)(context->canvas->x_resolution - block->right - 1);
        rotated_block.bottom = (GX_VALUE)(context->canvas->x_resolution - block->left - 1);

        yshift = -yshift;
    }
    else
    {
        rotated_block.left = (GX_VALUE)(context->canvas->y_resolution - block->bottom - 1);
        rotated_block.right = (GX_VALUE)(context->canvas->y_resolution - block->top - 1);
        rotated_block.top = block->left;
        rotated_block.bottom = block->right;

        xshift = -xshift;
    }

    if (xshift)
    {
        if (xshift > 0)
        {

            /* Have to copy from left to right. */
            pPut = (USHORT *)context->memory;
            pPut += rotated_block.top * context->pitch;
            pPut += rotated_block.left + xshift;

            pGet = (USHORT *)context->memory;
            pGet += rotated_block.top * context->pitch;
            pGet += rotated_block.left;

            width = rotated_block.right - rotated_block.left + 1 - xshift;
            width_in_bytes = width * (int)sizeof(USHORT);

            if (width_in_bytes <= 0)
            {
                return;
            }

            for (y = rotated_block.top; y <= rotated_block.bottom; y++)
            {
                memmove(pPut, pGet, (size_t)width_in_bytes);

                pPut += context->pitch;
                pGet += context->pitch;
            }
        }
        else
        {

            /* Have to copy from right to left. */
            pPut = (USHORT *)context->memory;
            pPut += rotated_block.top * context->pitch;
            pPut += rotated_block.left;

            pGet = (USHORT *)context->memory;
            pGet += rotated_block.top * context->pitch;
            pGet += rotated_block.left - xshift;

            width = rotated_block.right - rotated_block.left + 1 + xshift;
            width_in_bytes = width * (int)sizeof(USHORT);

            if (width_in_bytes <= 0)
            {
                return;
            }

            for (y = rotated_block.top; y <= rotated_block.bottom; y++)
            {
                memmove(pPut, pGet, (size_t)width_in_bytes);

                pPut += context->pitch;
                pGet += context->pitch;
            }
        }
    }
    else
    {
        width = rotated_block.right - rotated_block.left + 1;
        width_in_bytes = width * (int)sizeof(USHORT);

        if (yshift > 0)
        {

            /* Have to copy from top to bottom. */
            pPut = (USHORT *)context->memory;
            pPut += rotated_block.bottom * context->pitch;
            pPut += rotated_block.left;

            pGet = (USHORT *)context->memory;
            pGet += (rotated_block.bottom - yshift) * context->pitch;
            pGet += rotated_block.left;

            height = rotated_block.bottom - rotated_block.top + 1 - yshift;

            for (y = 0; y < height; y++)
            {
                memmove(pPut, pGet, (size_t)width_in_bytes);

                pPut -= context->pitch;
                pGet -= context->pitch;
            }
        }
        else
        {

            /* Have to copy from bottom to top. */
            pPut = (USHORT *)context->memory;
            pPut += rotated_block.top * context->pitch;
            pPut += rotated_block.left;

            pGet = (USHORT *)context->memory;
            pGet += (rotated_block.top - yshift) * context->pitch;
            pGet += rotated_block.left;

            height = rotated_block.bottom - rotated_block.top + 1 + yshift;

            for (y = 0; y < height; y++)
            {
                memmove(pPut, pGet, (size_t)width_in_bytes);

                pPut += context->pitch;
                pGet += context->pitch;
            }
        }
    }
}

