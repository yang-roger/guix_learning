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

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_arc_clipping_get                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns the clipping area of a circle arc.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    xcenter                               x-coord of center of circle   */
/*    ycenter                               y-coord of center of circle   */
/*    r                                     Radius of circle              */
/*    start_angle                           Start angle for clipping      */
/*    end_argle                             End angle for clipping        */
/*    clip_1                                Clip result                   */
/*    cllp_2                                Clip result                   */
/*    clip_3                                Clip result                   */
/*    clip_4                                Clip result                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_circle_point_get          Get point coord on a circle   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#if defined(GX_ARC_DRAWING_SUPPORT)
void _gx_display_driver_arc_clipping_get(INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle,
                                         GX_RECTANGLE *clip_1, GX_RECTANGLE *clip_2, GX_RECTANGLE *clip_3, GX_RECTANGLE *clip_4)
{

GX_POINT start_point;
GX_POINT end_point;
GX_VALUE ori_r = (GX_VALUE)r;
GX_VALUE neg_r = (GX_VALUE)(-ori_r);

    memset(clip_1, 0, sizeof(GX_RECTANGLE));
    memset(clip_2, 0, sizeof(GX_RECTANGLE));
    memset(clip_3, 0, sizeof(GX_RECTANGLE));
    memset(clip_4, 0, sizeof(GX_RECTANGLE));

    /* Get two endpoint of the arc. */
    _gx_utility_circle_point_get(0, 0, r, start_angle, &start_point);
    _gx_utility_circle_point_get(0, 0, r, end_angle, &end_point);

    /* Calculate clipping. */
    if (start_angle < 90)
    {
        if (end_angle <= 90)
        {
            clip_1->left = end_point.x;
            clip_1->top = end_point.y;
            clip_1->right = start_point.x;
            clip_1->bottom = start_point.y;
        }
        else
        {
            clip_1->left = 0;
            clip_1->top = neg_r;
            clip_1->right = start_point.x;
            clip_1->bottom = start_point.y;

            if (end_angle <= 180)
            {
                clip_2->left = end_point.x;
                clip_2->top = neg_r;
                clip_2->right = 0;
                clip_2->bottom = end_point.y;
            }
            else
            {
                clip_2->left = neg_r;
                clip_2->top = neg_r;
                clip_2->right = 0;
                clip_2->bottom = 0;

                if (end_angle <= 270)
                {
                    clip_3->left = neg_r;
                    clip_3->top = 0;
                    clip_3->right = end_point.x;
                    clip_3->bottom = end_point.y;
                }
                else
                {
                    clip_2->bottom = ori_r;

                    if (end_angle <= 360)
                    {
                        clip_3->left = 0;
                        clip_3->top = end_point.y;
                        clip_3->right = end_point.x;
                        clip_3->bottom = ori_r;
                    }
                    else
                    {
                        clip_3->left = 0;
                        clip_3->top = 0;
                        clip_3->right = ori_r;
                        clip_3->bottom = ori_r;

                        clip_4->left = end_point.x;
                        clip_4->top = end_point.y;
                        clip_4->right = ori_r;
                        clip_4->bottom = 0;
                    }
                }
            }
        }
    }
    else if (start_angle < 180)
    {
        if (end_angle <= 180)
        {
            clip_1->left = end_point.x;
            clip_1->top = start_point.y;
            clip_1->right = start_point.x;
            clip_1->bottom = end_point.y;
        }
        else
        {
            clip_1->left = neg_r;
            clip_1->top = start_point.y;
            clip_1->right = start_point.x;
            clip_1->bottom = 0;

            if (end_angle <= 270)
            {
                clip_2->left = neg_r;
                clip_2->top = 0;
                clip_2->right = end_point.x;
                clip_2->bottom = end_point.y;
            }
            else
            {
                clip_2->left = neg_r;
                clip_2->top = 0;
                clip_2->right = 0;
                clip_2->bottom = ori_r;

                if (end_angle <= 360)
                {
                    clip_3->left = 0;
                    clip_3->top = end_point.y;
                    clip_3->right = end_point.x;
                    clip_3->bottom = ori_r;
                }
                else
                {
                    clip_2->right = ori_r;

                    if (end_angle <= 450)
                    {
                        clip_3->left = end_point.x;
                        clip_3->top = end_point.y;
                        clip_3->right = ori_r;
                        clip_3->bottom = 0;
                    }
                    else
                    {
                        clip_3->left = 0;
                        clip_3->top = neg_r;
                        clip_3->right = ori_r;
                        clip_3->bottom = 0;

                        clip_4->left = end_point.x;
                        clip_4->top = neg_r;
                        clip_4->right = 0;
                        clip_4->bottom = end_point.y;
                    }
                }
            }
        }
    }
    else if (start_angle < 270)
    {
        if (end_angle <= 270)
        {
            clip_1->left = start_point.x;
            clip_1->top = start_point.y;
            clip_1->right = end_point.x;
            clip_1->bottom = end_point.y;
        }
        else
        {
            clip_1->left = start_point.x;
            clip_1->top = start_point.y;
            clip_1->right = 0;
            clip_1->bottom = ori_r;

            if (end_angle <= 360)
            {
                clip_2->left = 0;
                clip_2->top = end_point.y;
                clip_2->right = end_point.x;
                clip_2->bottom = ori_r;
            }
            else
            {
                clip_2->left = 0;
                clip_2->top = 0;
                clip_2->right = ori_r;
                clip_2->bottom = ori_r;

                if (end_angle <= 450)
                {
                    clip_3->left = end_point.x;
                    clip_3->top = end_point.y;
                    clip_3->right = ori_r;
                    clip_3->bottom = ori_r;
                }
                else
                {
                    clip_2->top = neg_r;

                    if (end_angle <= 540)
                    {
                        clip_3->left = end_point.x;
                        clip_3->top = neg_r;
                        clip_3->right = 0;
                        clip_3->bottom = end_point.y;
                    }
                    else
                    {
                        clip_3->left = neg_r;
                        clip_3->top = neg_r;
                        clip_3->right = 0;
                        clip_3->bottom = 0;

                        clip_4->left = neg_r;
                        clip_4->top = 0;
                        clip_4->right = end_point.x;
                        clip_4->bottom = end_point.y;
                    }
                }
            }
        }
    }
    else
    {
        if (end_angle <= 360)
        {
            clip_1->left = start_point.x;
            clip_1->top = end_point.y;
            clip_1->right = end_point.x;
            clip_1->bottom = start_point.y;
        }
        else
        {
            clip_1->left = start_point.x;
            clip_1->top = 0;
            clip_1->right = ori_r;
            clip_1->bottom = start_point.y;

            if (end_angle <= 450)
            {
                clip_2->left = end_point.x;
                clip_2->top = end_point.y;
                clip_2->right = ori_r;
                clip_2->bottom = 0;
            }
            else
            {
                clip_2->left = 0;
                clip_2->top = neg_r;
                clip_2->right = ori_r;
                clip_2->bottom = 0;

                if (end_angle <= 540)
                {
                    clip_3->left = end_point.x;
                    clip_3->top = neg_r;
                    clip_3->right = 0;
                    clip_3->bottom = end_point.y;
                }
                else
                {
                    clip_2->left = neg_r;

                    if (end_angle <= 630)
                    {
                        clip_3->left = neg_r;
                        clip_3->top = 0;
                        clip_3->right = end_point.x;
                        clip_3->bottom = end_point.y;
                    }
                    else
                    {
                        clip_3->left = neg_r;
                        clip_3->top = 0;
                        clip_3->right = 0;
                        clip_3->bottom = ori_r;

                        clip_4->left = 0;
                        clip_4->top = end_point.y;
                        clip_4->right = end_point.x;
                        clip_4->bottom = ori_r;
                    }
                }
            }
        }
    }

    clip_1->left = (GX_VALUE)(clip_1->left + (GX_VALUE)xcenter);
    clip_1->top = (GX_VALUE)(clip_1->top + (GX_VALUE)ycenter);
    clip_1->right = (GX_VALUE)(clip_1->right + (GX_VALUE)xcenter);
    clip_1->bottom = (GX_VALUE)(clip_1->bottom + (GX_VALUE)ycenter);

    clip_2->left = (GX_VALUE)(clip_2->left + (GX_VALUE)xcenter);
    clip_2->top = (GX_VALUE)(clip_2->top + (GX_VALUE)ycenter);
    clip_2->right = (GX_VALUE)(clip_2->right + (GX_VALUE)xcenter);
    clip_2->bottom = (GX_VALUE)(clip_2->bottom + (GX_VALUE)ycenter);

    clip_3->left = (GX_VALUE)(clip_3->left + (GX_VALUE)xcenter);
    clip_3->top = (GX_VALUE)(clip_3->top + (GX_VALUE)ycenter);
    clip_3->right = (GX_VALUE)(clip_3->right + (GX_VALUE)xcenter);
    clip_3->bottom = (GX_VALUE)(clip_3->bottom + (GX_VALUE)ycenter);

    clip_4->left = (GX_VALUE)(clip_4->left + (GX_VALUE)xcenter);
    clip_4->top = (GX_VALUE)(clip_4->top + (GX_VALUE)ycenter);
    clip_4->right = (GX_VALUE)(clip_4->right + (GX_VALUE)xcenter);
    clip_4->bottom = (GX_VALUE)(clip_4->bottom + (GX_VALUE)ycenter);
}
#endif

