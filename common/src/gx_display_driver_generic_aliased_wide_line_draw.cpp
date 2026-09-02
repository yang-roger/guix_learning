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
#include "gx_context.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_aliased_wide_line_draw                   */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*   Generic display driver function for aliased wide line.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xstart                                x-coord of endpoint           */
/*    ystart                                y-coord of endpoint           */
/*    xend                                  x-coord of endpoint           */
/*    yend                                  y-coord of endpoint           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_simple_wide_line_draw]                           */
/*                                          Basic display driver wide     */
/*                                            line draw function          */
/*    _gx_display_driver_generic_wide_line_points_calculate               */
/*                                          Calculate corners of wide line*/
/*    _gx_display_driver_generic_aliased_filled_circle_draw               */
/*                                          Basic display driver aliased  */
/*                                            circle fill function        */
/*    _gx_display_driver_generic_wide_line_fill                           */
/*                                          Basic display driver wide line*/
/*                                            draw function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_generic_aliased_wide_line_draw(GX_DRAW_CONTEXT *context, INT xstart,
                                                       INT ystart, INT xend, INT yend)
{
GX_DISPLAY     *display = context->display;
INT             brush_width = context->brush.width;
GX_FIXED_POINT *line_points;
GX_FIXED_VAL    sxcenter;
GX_FIXED_VAL    sycenter;
GX_FIXED_VAL    excenter;
GX_FIXED_VAL    eycenter;
GX_RECTANGLE    clip_rect;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
GX_UBYTE old_alpha;
    old_alpha = context->brush.alpha;
    context->brush.alpha = GX_ALPHA_VALUE_OPAQUE;
#endif
    /* calculate the corners of this line, save them
       to our points array
     */
    if (!(context->display->driver_pixel_blend))
    {
        display->driver_simple_wide_line_draw(context, xstart, ystart, xend, yend);
        return;
    }

    if ((context->brush.style & GX_BRUSH_ROUND) &&
        (brush_width > 2))
    {
        sxcenter = GX_FIXED_VAL_MAKE(xstart);
        sycenter = GX_FIXED_VAL_MAKE(ystart);
        excenter = GX_FIXED_VAL_MAKE(xend);
        eycenter = GX_FIXED_VAL_MAKE(yend);

        if (!(brush_width & 0x01))
        {
            if (ystart == yend)
            {
                /* Horizontal line. */
                sycenter -= GX_FIXED_VAL_HALF;
                eycenter -= GX_FIXED_VAL_HALF;
            }
            else if (xstart == xend)
            {
                /* Vertical line. */
                sxcenter -= GX_FIXED_VAL_HALF;
                excenter -= GX_FIXED_VAL_HALF;
            }
        }

        _gx_display_driver_generic_aliased_filled_circle_draw(context, sxcenter, sycenter,
                                                              GX_FIXED_VAL_MAKE(brush_width) >> 1);

        _gx_display_driver_generic_aliased_filled_circle_draw(context, excenter, eycenter,
                                                              GX_FIXED_VAL_MAKE(brush_width) >> 1);
    }

    if (ystart == yend)
    {
        /* Horizontal line. */

        if (xstart > xend)
        {
            GX_SWAP_VALS(xstart, xend);
        }

        clip_rect.left = (GX_VALUE)xstart;
        clip_rect.right = (GX_VALUE)xend;
        clip_rect.top = (GX_VALUE)(ystart - (brush_width >> 1));
        clip_rect.bottom = (GX_VALUE)(clip_rect.top + brush_width - 1);

        if (gx_rectangle_intersect_(clip_rect, *context->clip, &clip_rect))
        {
            display->driver_horizontal_line_draw(context,
                                                 clip_rect.left,
                                                 clip_rect.right,
                                                 clip_rect.top,
                                                 clip_rect.bottom - clip_rect.top + 1,
                                                 context->brush.line_color);
        }
    }
    else if (xstart == xend)
    {
        /* Vertical line. */

        if (ystart > yend)
        {
            GX_SWAP_VALS(ystart, yend);
        }

        clip_rect.left = (GX_VALUE)(xstart - (brush_width >> 1));
        clip_rect.right = (GX_VALUE)(clip_rect.left + brush_width - 1);
        clip_rect.top = (GX_VALUE)ystart;
        clip_rect.bottom = (GX_VALUE)yend;

        if (gx_rectangle_intersect_(clip_rect, *context->clip, &clip_rect))
        {
            display->driver_vertical_line_draw(context,
                                               clip_rect.top,
                                               clip_rect.bottom,
                                               clip_rect.left,
                                               clip_rect.right - clip_rect.left + 1,
                                               context->brush.line_color);
        }
    }
    else
    {
        line_points = _gx_display_driver_generic_wide_line_points_calculate(context, xstart, ystart,
                                                                            xend, yend, brush_width, GX_TRUE);

        if (display->rotation_angle)
        {
            _gx_display_driver_generic_rotated_wide_line_fill(context, line_points);
        }
        else
        {
            _gx_display_driver_generic_wide_line_fill(context, line_points);
        }
    }

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    context->brush.alpha = old_alpha;
#endif
}

