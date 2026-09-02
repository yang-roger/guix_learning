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
#include "gx_system.h"

#if defined(GX_ARC_DRAWING_SUPPORT)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_aliased_wide_ellipse_draw                */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Display driver function to draw anti-aliased ellipse with wide      */
/*    outline.                                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xcenter                               x-coord of center of ellipse  */
/*    ycenter                               y-coord of center of ellipse  */
/*    a                                     Length of the Semi-major Axis */
/*    b                                     Length of the Semi-minor Axis */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*    _gx_utility_rectangle_point_detect    Detect whether a pixel is     */
/*                                            inside rectangle            */
/*    _gx_utility_math_sqrt                 Compute the square root value */
/*    [gx_display_driver_horizontal_line_draw]                            */
/*                                          Basic display driver          */
/*                                            horizontal line draw routine*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_generic_aliased_wide_ellipse_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b)
{
GX_DISPLAY   *display;
GX_RECTANGLE *clip;
GX_BRUSH     *brush;
INT           x;
INT           x2;
INT           y;
INT           y2;
GX_POINT      point;
INT           sign[4][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
INT           index;
INT           aa;
INT           bb;
INT          *pLineEnds;
INT           ymin;
INT           ymax;
INT           height;
INT           error;
INT           loop;
INT           brush_width;
GX_UBYTE      alpha1;
GX_UBYTE      alpha2;

void          (*blend_func)(GX_DRAW_CONTEXT *context,
                            INT x, INT y, GX_COLOR color, GX_UBYTE error);

#if defined(GX_BRUSH_ALPHA_SUPPORT)
GX_UBYTE old_alpha;

    old_alpha = context->brush.alpha;
    context->brush.alpha = GX_ALPHA_VALUE_OPAQUE;
#endif
    display = context->display;
    blend_func = display->driver_pixel_blend;

    if (blend_func == GX_NULL)
    {
        return;
    }
    clip = context->clip;
    brush = &context->brush;
    brush_width = brush->width;

    if ((a <= ((brush_width - 1) >> 1)) ||
        (b <= ((brush_width - 1) >> 1)))
    {
        return;
    }

    ymin = ycenter - b - (brush_width >> 1);
    ymax = ycenter + b + (brush_width >> 1);

    if (ymin < clip->top)
    {
        ymin = clip->top;
    }

    if (ymax > clip->bottom)
    {
        ymax = clip->bottom;
    }

    height = ymax - ymin + 1;
    pLineEnds = _gx_system_scratchpad;

    for (y = 0; y <= height * 2; y++)
    {
        pLineEnds[y] = 0;
    }

    /* loop = 0: draw inner ellipse, and save point array.
       loop = 1: draw outer ellipse, and save point array. */
    for (loop = 0; loop < 2; loop++)
    {
        if (loop == 0)
        {
            a -= ((brush_width - 1) >> 1);
            b -= ((brush_width - 1) >> 1);
        }
        else
        {
            a += brush_width - 1;
            b += brush_width - 1;
        }

        aa = a * a;
        bb = b * b;
        x = 0;
        y = b;
        error = 0;

        /* Region I of the first quarter of the ellipse.  */
        while (2 * bb * (x + 1) < aa * (2 * y - 1))
        {
            alpha1 = (GX_UBYTE)(255 - error);
            alpha2 = (GX_UBYTE)error;

            /* error is the distance between mathmatic point to
               drawing point, which is used to do pixel blending.  */
            y2 = bb - bb * (x + 1) * (x + 1) / aa;
            error = (y << 8) - (INT)(_gx_utility_math_sqrt((UINT)(y2 << 10)) << 3);

            if (error >= 510)
            {
                /* The slope in point(x + 1, y) is greater than -1,
                   make point(x, y) the delimit pixel, break here. */
                y2 = bb - bb * x * x / aa;
                error = (y << 8) - (INT)(_gx_utility_math_sqrt((UINT)(y2 << 10)) << 3);
                break;
            }

            for (index = 0; index < 4; index++)
            {
                point.x = (GX_VALUE)(x * sign[index][0] + xcenter);
                point.y = (GX_VALUE)(y * sign[index][1] + ycenter);

                if (clip->contain_point_(point))
                {
                    /* Draw point(x, y).  */
                    blend_func(context, point.x, point.y, brush->line_color, alpha1);
                }

                point.y = (GX_VALUE)((y - 1) * sign[index][1] + ycenter);

                if (clip->contain_point_(point))
                {
                    /* Draw point(x, y - 1).  */
                    blend_func(context, point.x, point.y, brush->line_color, alpha2);
                }
            }


            if (error >= 255)
            {
                error -= 255;
                y--;

                for (index = 0; index < 2; index++)
                {
                    y2 = y * sign[index][0] + ycenter;

                    if ((y2 >= ymin) && (y2 <= ymax))
                    {
                        if (loop == 0)
                        {
                            /* Inner circle points.  */
                            pLineEnds[(y2 - ymin) << 1] = x + 1;
                        }
                        else
                        {
                            /* Outer circle points.  */
                            pLineEnds[((y2 - ymin) << 1) + 1] = x;
                        }
                    }
                }
            }

            x++;
        }

        /* Draw delimit pixel where der*/
        for (index = 0; index < 4; index++)
        {
            point.x = (GX_VALUE)(x * sign[index][0] + xcenter);
            point.y = (GX_VALUE)(y * sign[index][1] + ycenter);

            if (clip->contain_point_(point))
            {
                blend_func(context, point.x, point.y, brush->line_color, (GX_UBYTE)(255 - error));
            }
        }

        /* Region II of the first quarter of the ellipse.  */
        while (y > 0)
        {
            y--;

            y2 = aa - aa * y * y / bb;
            error = (INT)(_gx_utility_math_sqrt((UINT)(y2 << 10)) << 3) - (x << 8);

            while (error >= 255)
            {
                error -= 255;
                x++;
            }

            alpha1 = (GX_UBYTE)(255 - error);
            alpha2 = (GX_UBYTE)error;

            for (index = 0; index < 2; index++)
            {
                y2 = y * sign[index][0] + ycenter;

                if ((y2 >= ymin) && (y2 <= ymax))
                {
                    if (loop == 0)
                    {
                        /* Inner circle points.  */
                        pLineEnds[(y2 - ymin) << 1] = x + 1;
                    }
                    else
                    {
                        /* Outer circle points.  */
                        pLineEnds[((y2 - ymin) << 1) + 1] = x;
                    }
                }
            }

            for (index = 0; index < 4; index++)
            {
                point.x = (GX_VALUE)(x * sign[index][0] + xcenter);
                point.y = (GX_VALUE)(y * sign[index][1] + ycenter);

                if (clip->contain_point_(point))
                {
                    /* Draw point(x, y). */
                    blend_func(context, point.x, point.y, brush->line_color, alpha1);
                }

                point.x = (GX_VALUE)((x + 1) * sign[index][0] + xcenter);

                if (clip->contain_point_(point))
                {
                    /* Draw point(x + 1, y). */
                    blend_func(context, point.x, point.y, brush->line_color, alpha2);
                }
            }
        }
    }

    index = 0;

    /* Filling outlines with horizontal line. */
    for (y = ymin; y <= ymax; y++)
    {
        for (loop = 0; loop < 2; loop++)
        {
            x = pLineEnds[index] * sign[loop][0] + xcenter;
            x2 = pLineEnds[index + 1] * sign[loop][0] + xcenter;

            if (x > x2)
            {
                GX_SWAP_VALS(x, x2);
            }

            if (x < clip->left)
            {
                x = clip->left;
            }

            if (x2 > clip->right)
            {
                x2 = clip->right;
            }

            display->driver_horizontal_line_draw(context, x, x2, y, 1, brush->line_color);
        }

        index += 2;
    }

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    context->brush.alpha = old_alpha;
#endif
}
#endif

