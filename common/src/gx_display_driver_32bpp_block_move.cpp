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
/*    _gx_display_driver_32bpp_block_move                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 32bpp color format display driver block moving function.    */
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
/*    memmove                                Move memory content          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_32bpp_block_move(GX_DRAW_CONTEXT *context,
                                         GX_RECTANGLE *block, INT xshift, INT yshift)
{
GX_COLOR *pGet;
GX_COLOR *pPut;
int       width;
int       width_in_bytes;
int       y;
int       height;

    if (xshift)
    {
        if (xshift > 0)
        {
            /* have to copy from left to right. */
            pPut = context->memory;
            pPut += block->top * context->pitch;
            pPut += block->left + xshift;

            pGet = context->memory;
            pGet += block->top * context->pitch;
            pGet += block->left;

            width = block->right - block->left + 1 - xshift;
            width_in_bytes = width * (int)sizeof(GX_COLOR);

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
            pPut = context->memory;
            pPut += block->top * context->pitch;
            pPut += block->left;

            pGet = context->memory;
            pGet += block->top * context->pitch;
            pGet += block->left - xshift;

            width = block->right - block->left + 1 + xshift;
            width_in_bytes = width * (int)sizeof(GX_COLOR);

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
        width_in_bytes = width * (int)sizeof(GX_COLOR);

        if (yshift > 0)
        {
            /* have to copy from top to bottom */
            pPut = context->memory;
            pPut += block->bottom * context->pitch;
            pPut += block->left;

            pGet = context->memory;
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
            pPut = context->memory;
            pPut += block->top * context->pitch;
            pPut += block->left;

            pGet = context->memory;
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

