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
/*    _gx_display_driver_generic_polygon_draw                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic polygon draw function.                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    vertex                                Array of vertexes             */
/*    num                                   Number of vertexes            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_line_draw                  Draw a line into canvas       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_generic_polygon_draw(GX_DRAW_CONTEXT *context, GX_POINT *vertex, INT num)
{
INT index;
INT brush_width = context->brush.width;
#if defined (GX_BRUSH_ALPHA_SUPPORT)
GX_UBYTE     temp_alpha;
UINT         brush_style;
    temp_alpha = context->brush.alpha;
    brush_style = context->brush.style;
    if (brush_width > 1)
    {
        if (temp_alpha != 0xff)
        {
            if ((brush_style & GX_BRUSH_ROUND) || (brush_style & GX_BRUSH_ALIAS))
            {
                context->brush.alpha = GX_ALPHA_VALUE_OPAQUE;
            }
        }
    }
#endif

    if (brush_width > 0)
    {
        /*round end*/
        for (index = 0; index < num; index++)
        {
            _gx_canvas_line_draw(vertex[index].x, vertex[index].y,
                                 vertex[(index + 1) % num].x, vertex[(index + 1) % num].y);
        }
    }
#if defined (GX_BRUSH_ALPHA_SUPPORT)
    context->brush.alpha = temp_alpha;
#endif
}

