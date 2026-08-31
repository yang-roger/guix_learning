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

#include "gx_utility.h"
#include "gx_canvas.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4bpp_canvas_copy                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic 4bpp canvas copy function.                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*   canvas                                 The canvas to copy from       */
/*   composite                              The canvas to copy to         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_shift           Move the rectangle            */
/*    _gx_utility_rectangle_overlap_detect  Detect two rectangles being   */
/*                                            overlap to each other       */
/*    memcpy                                Move canvas data              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_4bpp_canvas_copy(GX_CANVAS *canvas, GX_CANVAS *composite)
{
GX_RECTANGLE dirty;
GX_RECTANGLE overlap;
GX_UBYTE    *read;
GX_UBYTE    *write;
GX_UBYTE     color;
INT          row;
INT          column;
UINT         read_pos;
UINT         write_pos;
GX_UBYTE     read_mask;
GX_UBYTE     write_mask;
INT          readstride;
INT          writestride;
INT          offset;

    dirty.left = dirty.top = 0;
    dirty.right = (GX_VALUE)(canvas->x_resolution - 1);
    dirty.bottom = (GX_VALUE)(canvas->y_resolution - 1);
    readstride = (canvas->x_resolution + 1) >> 1;
    writestride = (composite->x_resolution + 1) >> 1;

    _gx_utility_rectangle_shift(&dirty, canvas->display_offset_x, canvas->display_offset_y);

    if (_gx_utility_rectangle_overlap_detect(&dirty, &composite->dirty_area, &overlap))
    {
        offset = overlap.left - dirty.left;
        read_pos = (UINT)((overlap.top - dirty.top) * readstride + (offset >> 1));
        write_pos = (UINT)(overlap.top * writestride + (overlap.left >> 1));

        for (row = overlap.top; row <= overlap.bottom; row++)
        {
            read = (GX_UBYTE *)canvas->memory;
            write = (GX_UBYTE *)composite->memory;

            read += read_pos;
            write += write_pos;
            /* If position is odd, it means the low bits. */
            if (offset & 0x01)
            {
                read_mask = 0x0f;
            }
            else
            {
                read_mask = 0xf0;
            }

            if (overlap.left & 0x01)
            {
                write_mask = 0x0f;
            }
            else
            {
                write_mask = 0xf0;
            }

            for (column = overlap.left; column <= overlap.right; column++)
            {
                color = (*read) & read_mask;
                *write = (GX_UBYTE)((*write) & (~write_mask));

                if (color)
                {
                    /* Read and write have same mask bits. */
                    if (read_mask & write_mask)
                    {
                        *write |= color;
                    }
                    else
                    {
                        /* Read and write are malposed. */
                        /* If position is odd, it means the low bits. */
                        if (write_mask & 0x01)
                        {
                            *write |= (GX_UBYTE)(color >> 4);
                        }
                        else
                        {
                            *write |= (GX_UBYTE)(color << 4);
                        }
                    }
                }

                read_mask >>= 4;
                write_mask >>= 4;
                if (!read_mask)
                {
                    read++;
                    read_mask = 0xf0;
                }
                if (!write_mask)
                {
                    write++;
                    write_mask = 0xf0;
                }
            }
            write_pos = (UINT)((INT)write_pos + writestride);
            read_pos = (UINT)((INT)read_pos + readstride);
        }
    }
}

