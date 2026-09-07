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
/**   Utility (Pixelmap)                                                  */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef GX_UTILITY_PIXELMAP_H
#define GX_UTILITY_PIXELMAP_H

#include "gx_pixelmap.h"

UINT _gx_utility_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT _gx_utility_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT _gx_utility_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);

UINT _gx_utility_1bpp_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT _gx_utility_1bpp_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT _gx_utility_1bpp_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);

UINT _gx_utility_4bpp_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT _gx_utility_4bpp_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT _gx_utility_4bpp_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);

UINT _gx_utility_8bpp_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT _gx_utility_8bpp_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT _gx_utility_8bpp_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);

UINT _gx_utility_8bit_alphamap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT _gx_utility_8bit_alphamap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);

UINT _gx_utility_332rgb_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT _gx_utility_332rgb_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);

UINT _gx_utility_16bpp_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT _gx_utility_565rgb_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT _gx_utility_565rgb_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);

UINT _gx_utility_1555xrgb_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT _gx_utility_1555xrgb_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);

UINT _gx_utility_4444argb_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT _gx_utility_4444argb_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT _gx_utility_4444argb_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);

UINT _gx_utility_32argb_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT _gx_utility_32argb_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT _gx_utility_32argb_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);

#endif
