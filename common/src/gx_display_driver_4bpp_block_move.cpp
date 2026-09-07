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
/*    _gx_display_driver_4bpp_block_move                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    4-bpp display driver block moving function.                         */
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
void _gx_display_driver_4bpp_block_move(GX_DRAW_CONTEXT *context,
                                        GX_RECTANGLE *block, INT xshift, INT yshift)
{
GX_UBYTE *putrow;
GX_UBYTE *getrow;
GX_UBYTE *pGet;
GX_UBYTE *pPut;
INT       width;
INT       column;
INT       y;
INT       height;
INT       stride;
GX_UBYTE  putmask;
GX_UBYTE  getmask;
INT       move_width;

    putrow = (GX_UBYTE *)context->memory;

    stride = (context->pitch + 1) >> 1;

    if (xshift)
    {
        if (xshift > 0)
        {
            /* Copy from right to left.  */
            width = block->right - block->left + 1;
            width -= xshift;

            putrow += block->top * stride;
            getrow = putrow;

            putrow += block->right >> 1;
            getrow += (block->left + width - 1) >> 1;

            for (y = block->top; y <= block->bottom; y++)
            {
                pPut = putrow;
                pGet = getrow;

                if (block->right & 0x01)
                {
                    putmask = 0x0f;
                }
                else
                {
                    putmask = 0xf0;
                }

                if ((block->left + width - 1) & 0x01)
                {
                    getmask = 0x0f;
                }
                else
                {
                    getmask = 0xf0;
                }

                for (column = 0; column < width; column++)
                {
                    /* Set bits to 0. */
                    *pPut &= (GX_UBYTE)(~putmask);

                    if ((*pGet) & getmask)
                    {
                        if (getmask == putmask)
                        {
                            *pPut |= ((*pGet) & getmask);
                        }
                        else
                        {
                            if (getmask == 0xf0)
                            {
                                *pPut |= (GX_UBYTE)((((*pGet) & getmask) >> 4) & 0x0f);
                            }
                            else
                            {
                                *pPut |= (GX_UBYTE)((GX_UBYTE)(((*pGet) & getmask) << 4) & 0xf0);
                            }
                        }
                    }

                    if (getmask == 0xf0)
                    {
                        getmask = 0x0f;
                        pGet--;
                    }
                    else
                    {
                        getmask = 0xf0;
                    }

                    if (putmask == 0xf0)
                    {
                        putmask = 0x0f;
                        pPut--;
                    }
                    else
                    {
                        putmask = 0xf0;
                    }
                }

                putrow += stride;
                getrow += stride;
            }
        }
        else
        {
            /* Copy from right to left.  */
            width = block->right - block->left + 1;
            width += xshift;

            putrow += block->top * stride;
            getrow = putrow;

            putrow += block->left >> 1;
            getrow += (block->left - xshift) >> 1;

            for (y = block->top; y <= block->bottom; y++)
            {
                pPut = putrow;
                pGet = getrow;

                if (block->left & 0x01)
                {
                    putmask = 0x0f;
                }
                else
                {
                    putmask = 0xf0;
                }

                if ((block->left - xshift) & 0x01)
                {
                    getmask = 0x0f;
                }
                else
                {
                    getmask = 0xf0;
                }

                for (column = 0; column < width; column++)
                {
                    /* Set bits to 0. */
                    *pPut &= (GX_UBYTE)(~putmask);

                    if ((*pGet) & getmask)
                    {
                        if (getmask == putmask)
                        {
                            *pPut |= ((*pGet) & getmask);
                        }
                        else
                        {
                            if (getmask == 0xf0)
                            {
                                *pPut |= (GX_UBYTE)((((*pGet) & getmask) >> 4) & 0x0f);
                            }
                            else
                            {
                                *pPut |= (GX_UBYTE)((GX_UBYTE)(((*pGet) & getmask) << 4) & 0xf0);
                            }
                        }
                    }

                    getmask >>= 4;
                    if (getmask == 0)
                    {
                        getmask = 0xf0;
                        pGet++;
                    }

                    putmask >>= 4;
                    if (putmask == 0)
                    {
                        putmask = 0xf0;
                        pPut++;
                    }
                }

                putrow += stride;
                getrow += stride;
            }
        }
    }
    else
    {
        width = block->right - block->left + 1;

        if (yshift > 0)
        {
            /* Copy from top to bottom.  */
            putrow += (block->bottom * stride);
            putrow += (block->left >> 1);

            getrow = putrow;
            getrow -= yshift * stride;

            height = block->bottom - block->top + 1;
            height -= yshift;

            for (y = 0; y < height; y++)
            {
                pPut = putrow;
                pGet = getrow;

                if (block->left & 0x01)
                {
                    putmask = 0x0f;
                }
                else
                {
                    putmask = 0xf0;
                }

                column = 0;
                while (column < width)
                {
                    if ((putmask == 0xf0) && (width - column >= 2))
                    {
                        move_width = (width - column) >> 1;
                        memmove(pPut, pGet, (size_t)move_width);
                        pPut += move_width;
                        pGet += move_width;
                        column += (move_width << 1);
                    }
                    else
                    {
                        *pPut &= (GX_UBYTE)(~putmask);
                        if ((*pGet) & putmask)
                        {
                            *pPut |= (GX_UBYTE)((*pGet) & putmask);
                        }


                        putmask >>= 4;

                        if (putmask == 0)
                        {
                            putmask = 0xf0;
                            pPut++;
                            pGet++;
                        }

                        column++;
                    }
                }

                putrow -= stride;
                getrow -= stride;
            }
        }
        else
        {
            /* Copy from bottom to top.  */
            putrow += (block->top * stride);
            putrow += (block->left >> 1);

            getrow = putrow;
            getrow -= yshift * stride;

            height = block->bottom - block->top + 1;
            height += yshift;

            for (y = 0; y < height; y++)
            {
                pPut = putrow;
                pGet = getrow;

                if (block->left & 0x01)
                {
                    putmask = 0x0f;
                }
                else
                {
                    putmask = 0xf0;
                }

                column = 0;

                while (column < width)
                {
                    if ((putmask == 0xf0) && (width - column >= 2))
                    {
                        move_width = (width - column) >> 1;
                        memmove(pPut, pGet, (size_t)move_width);
                        pPut += move_width;
                        pGet += move_width;
                        column += (move_width << 1);
                    }
                    else
                    {
                        *pPut &= (GX_UBYTE)(~putmask);
                        if ((*pGet) & putmask)
                        {
                            *pPut |= (GX_UBYTE)((*pGet) & putmask);
                        }

                        putmask >>= 4;

                        if (putmask == 0)
                        {
                            putmask = 0xf0;
                            pPut++;
                            pGet++;
                        }

                        column++;
                    }
                }

                putrow += stride;
                getrow += stride;
            }
        }
    }
}

