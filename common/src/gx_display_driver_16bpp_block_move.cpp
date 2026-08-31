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

#include "gx_system.h"
#include "gx_utility.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_16bpp_block_move                                 */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 16bpp color format display driver block moving function.    */
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
void _gx_display_driver_16bpp_block_move(GX_DRAW_CONTEXT *context,
                                         GX_RECTANGLE *block, INT xshift, INT yshift)
{
USHORT *pGet;
USHORT *pPut;
INT     width;
INT     width_in_bytes;
INT     y;
INT     height;

#ifdef GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
    if (context->canvas->status & GX_CANVAS_PARTIAL_FRAME_BUFFER)
    {
        /* Not supported. */
        return;
    }
#endif

    if (xshift)
    {
        if (xshift > 0)
        {
            /* have to copy from left to right. */
            pPut = (USHORT *)context->memory;
            pPut += block->top * context->pitch;
            pPut += block->left + xshift;

            pGet = (USHORT *)context->memory;
            pGet += block->top * context->pitch;
            pGet += block->left;

            width = block->right - block->left + 1 - xshift;
            width_in_bytes = width * (int)sizeof(USHORT);

            if (width_in_bytes <= 0)
            {
                return;
            }

            for (y = block->top; y <= block->bottom; y++)
            {
                memmove(pPut, pGet, (size_t)width_in_bytes);

                pPut += context->pitch;
                pGet += context->pitch;
            }
        }
        else
        {
            /* have to copy from right to left */
            pPut = (USHORT *)context->memory;
            pPut += block->top * context->pitch;
            pPut += block->left;

            pGet = (USHORT *)context->memory;
            pGet += block->top * context->pitch;
            pGet += block->left - xshift;

            width = block->right - block->left + 1 + xshift;
            width_in_bytes = width * (int)sizeof(USHORT);

            if (width_in_bytes <= 0)
            {
                return;
            }

            for (y = block->top; y <= block->bottom; y++)
            {
                memmove(pPut, pGet, (size_t)width_in_bytes);

                pPut += context->pitch;
                pGet += context->pitch;
            }
        }
    }
    else
    {
        width = block->right - block->left + 1;
        width_in_bytes = width * (int)sizeof(USHORT);

        if (yshift > 0)
        {
            /* have to copy from top to bottom */
            pPut = (USHORT *)context->memory;
            pPut += block->bottom * context->pitch;
            pPut += block->left;

            pGet = (USHORT *)context->memory;
            pGet += (block->bottom - yshift) * context->pitch;
            pGet += block->left;

            height = block->bottom - block->top + 1 - yshift;

            for (y = 0; y < height; y++)
            {
                memmove(pPut, pGet, (size_t)width_in_bytes);

                pPut -= context->pitch;
                pGet -= context->pitch;
            }
        }
        else
        {
            /* have to copy from bottom to top */
            pPut = (USHORT *)context->memory;
            pPut += block->top * context->pitch;
            pPut += block->left;

            pGet = (USHORT *)context->memory;
            pGet += (block->top - yshift) * context->pitch;
            pGet += block->left;

            height = block->bottom - block->top + 1 + yshift;

            for (y = 0; y < height; y++)
            {
                memmove(pPut, pGet, (size_t)width_in_bytes);

                pPut += context->pitch;
                pGet += context->pitch;
            }
        }
    }
}

