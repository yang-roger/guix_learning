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
/**   Utility (Rectangle)                                                 */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef GX_UTILITY_RECTANGLE_H
#define GX_UTILITY_RECTANGLE_H

#include "gx_rectangle.h"

UINT    _gx_utility_rectangle_center(GX_RECTANGLE *rectangle, GX_RECTANGLE *within);
UINT    _gx_utility_rectangle_center_find(GX_RECTANGLE *rectangle, GX_POINT *return_center);
UINT    _gx_utility_rectangle_combine(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle);
GX_BOOL _gx_utility_rectangle_compare(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle);
UINT    _gx_utility_rectangle_define(GX_RECTANGLE *rectangle, GX_VALUE left, GX_VALUE top, GX_VALUE right, GX_VALUE bottom);
GX_BOOL _gx_utility_rectangle_inside_detect(GX_RECTANGLE *outer, GX_RECTANGLE *inner);
GX_BOOL _gx_utility_rectangle_overlap_detect(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle, GX_RECTANGLE *return_overlap_area);
GX_BOOL _gx_utility_rectangle_point_detect(GX_RECTANGLE *rectangle, GX_POINT point);
UINT    _gx_utility_rectangle_resize(GX_RECTANGLE *rectangle, GX_VALUE adjust);
UINT    _gx_utility_rectangle_shift(GX_RECTANGLE *rectangle, GX_VALUE x_shift, GX_VALUE y_shift);

#endif
