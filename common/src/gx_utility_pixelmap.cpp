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

#include "gx_utility_pixelmap.h"

#include "gx_system.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_pixelmap_resize                                         */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service resize a pixelmap and return the resized pixelmap.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The source pixelmap           */
/*    destination                           The resized pixelmap to be    */
/*                                            returned                    */
/*    width                                 New width                     */
/*    height                                New height                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_32argb_pixelmap_resize    Resize 32bpp pixelmap         */
/*    _gx_utility_16bpp_pixelmap_resize     Resize 565rgb pixelmap        */
/*    _gx_utility_1555xrgb_pixelmap_resize  Resize 1555xrgb pixelmap      */
/*    _gx_utility_4444argb_pixelmap_resize  Resize 4444argb pixelmap      */
/*    _gx_utility_8bpp_pixelmap_resize      Resize 8bpp pixelmap          */
/*    _gx_utility_8bit_alphamap_resize      Resize 8bit alphamap          */
/*    _gx_utility_4bpp_pixelmap_resize      Resize 4bpp pixelmap          */
/*    _gx_utility_1bpp_pixelmap_resize      Resize 1bpp pixelmap          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height)
{
UINT        status = GX_SUCCESS;
GX_PIXELMAP rotated_src;

    memset(destination, 0, sizeof(GX_PIXELMAP));

    /* Limited pixelmap width to 14 bits. */
    if (src->width > GX_MAX_PIXELMAP_RESOLUTION)
    {
        return GX_INVALID_WIDTH;
    }

    /* Limited pixelmap height to 14 bits. */
    if (src->height > GX_MAX_PIXELMAP_RESOLUTION)
    {
        return GX_INVALID_HEIGHT;
    }

    if ((src->flags & GX_PIXELMAP_ROTATED_90) ||
        (src->flags & GX_PIXELMAP_ROTATED_270))
    {
        rotated_src = (*src);
        GX_SWAP_VALS(rotated_src.width, rotated_src.height);
        GX_SWAP_VALS(width, height);

        src = &rotated_src;
    }

    switch (src->format)
    {
    case GX_COLOR_FORMAT_32ARGB:
    case GX_COLOR_FORMAT_24XRGB:
        /* Call 32argb pixelmap resize.  */
        status = _gx_utility_32argb_pixelmap_resize(src, destination, width, height);
        break;

    case GX_COLOR_FORMAT_565RGB:
    case GX_COLOR_FORMAT_565BGR:
        /* Call 16bpp pixelmap resize.  */
        status = _gx_utility_16bpp_pixelmap_resize(src, destination, width, height);
        break;

    case GX_COLOR_FORMAT_1555XRGB:
        status = _gx_utility_1555xrgb_pixelmap_resize(src, destination, width, height);
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        status = _gx_utility_4444argb_pixelmap_resize(src, destination, width, height);
        break;

    case GX_COLOR_FORMAT_8BIT_PACKED_PIXEL:
    case GX_COLOR_FORMAT_8BIT_PALETTE:
        /* Call 8bpp pixelmap resize.  */
        status = _gx_utility_8bpp_pixelmap_resize(src, destination, width, height);
        break;

    case GX_COLOR_FORMAT_8BIT_ALPHAMAP:
        status = _gx_utility_8bit_alphamap_resize(src, destination, width, height);
        break;

    case GX_COLOR_FORMAT_4BIT_GRAY:
        status = _gx_utility_4bpp_pixelmap_resize(src, destination, width, height);
        break;

    case GX_COLOR_FORMAT_MONOCHROME:
        status = _gx_utility_1bpp_pixelmap_resize(src, destination, width, height);
        break;

    default:
        status = GX_NOT_SUPPORTED;
        break;
    }

    if ((src->flags & GX_PIXELMAP_ROTATED_90) ||
        (src->flags & GX_PIXELMAP_ROTATED_270))
    {
        GX_SWAP_VALS(destination->width, destination->height);
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_pixelmap_rotation                                       */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service rotate a pixelmap.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The pixelmap to rotate        */
/*    angle                                 The angle to rotate           */
/*    destination                           Destination buffer for        */
/*                                            rotated pixelmap.           */
/*    rot_cx                                X coordinate of rotation      */
/*                                            center                      */
/*    rot_cy                                Y coordinate of rotation      */
/*                                            center                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_32argb_pixelmap_rotate    Rotate 32bpp pixelmap         */
/*    _gx_utility_565rgb_pixelmap_rotate    Rotate 565rgb pixelmap        */
/*    _gx_utility_4444argb_pixelmap_rotate  Rotate 4444argb pixelmap      */
/*    _gx_utility_8bpp_pixelmap_rotate      Rotate 8bit palette pixelmap  */
/*    _gx_utility_332rgb_pixelmap_rotate    Rotate 332rgb pixelmap        */
/*    _gx_utility_8bit_alphamap_rotate      Rotate 8bit alphamap          */
/*    _gx_utility_4bpp_pixelmap_rotate      Rotate 4bpp pixelmap          */
/*    _gx_utility_1bpp_pixelmap_rotate      Rotate 1bpp pixelmap          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
UINT        status = GX_SUCCESS;
GX_PIXELMAP rotated_src;

    /* Limit pixelmap width to 14bits. */
    if (src->width > GX_MAX_PIXELMAP_RESOLUTION)
    {
        return GX_INVALID_WIDTH;
    }

    /* Limit pixelmap height to 14bits. */
    if (src->height > GX_MAX_PIXELMAP_RESOLUTION)
    {
        return GX_INVALID_HEIGHT;
    }

    if ((src->flags & GX_PIXELMAP_ROTATED_90) ||
        (src->flags & GX_PIXELMAP_ROTATED_270))
    {
        rotated_src = (*src);
        GX_SWAP_VALS(rotated_src.width, rotated_src.height);
        src = &rotated_src;

        if (rot_cx && rot_cy)
        {
            GX_SWAP_VALS(*rot_cx, *rot_cy);

            if (src->flags & GX_PIXELMAP_ROTATED_90)
            {
                *rot_cy = src->height - 1 - (*rot_cy);
            }
            else
            {
                *rot_cx = src->width - 1 - (*rot_cx);
            }
        }
    }

    angle = angle % 360;

    if (angle == 0)
    {
        return GX_INVALID_VALUE;
    }
    else if (angle < 0)
    {
        angle += 360;
    }

    if ((_gx_system_memory_allocator == GX_NULL) || (_gx_system_memory_free == GX_NULL))
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    if (src->flags & GX_PIXELMAP_COMPRESSED)
    {
        return GX_INVALID_FORMAT;
    }

    if ((angle % 90) == 0)
    {
        _gx_utility_pixelmap_simple_rotate(src, angle, destination, rot_cx, rot_cy);
    }
    else
    {
        memset(destination, 0, sizeof(GX_PIXELMAP));
        destination->format = src->format;
        destination->version_major = src->version_major;
        destination->version_minor = src->version_major;
        destination->flags = src->flags;

        switch (src->format)
        {
        case GX_COLOR_FORMAT_32ARGB:
        case GX_COLOR_FORMAT_24XRGB:
            /* Call 32argb pixelmap rotate. */
            status = _gx_utility_32argb_pixelmap_rotate(src, angle, destination, rot_cx, rot_cy);
            break;

        case GX_COLOR_FORMAT_565RGB:
        case GX_COLOR_FORMAT_565BGR:
            /* Call 565rgb pixelmap rotate.  */
            status = _gx_utility_565rgb_pixelmap_rotate(src, angle, destination, rot_cx, rot_cy);
            break;

        case GX_COLOR_FORMAT_1555XRGB:
            status = _gx_utility_1555xrgb_pixelmap_rotate(src, angle, destination, rot_cx, rot_cy);
            break;

        case GX_COLOR_FORMAT_4444ARGB:
            status = _gx_utility_4444argb_pixelmap_rotate(src, angle, destination, rot_cx, rot_cy);
            break;

        case GX_COLOR_FORMAT_8BIT_PACKED_PIXEL:
            status = _gx_utility_332rgb_pixelmap_rotate(src, angle, destination, rot_cx, rot_cy);
            break;

        case GX_COLOR_FORMAT_8BIT_PALETTE:
            /* Call 8bpp pixelmap rotate.  */
            status = _gx_utility_8bpp_pixelmap_rotate(src, angle, destination, rot_cx, rot_cy);
            break;

        case GX_COLOR_FORMAT_8BIT_ALPHAMAP:
            /* Call 8bit alphamap rotate.  */
            status = _gx_utility_8bit_alphamap_rotate(src, angle, destination, rot_cx, rot_cy);
            break;

        case GX_COLOR_FORMAT_4BIT_GRAY:
            status = _gx_utility_4bpp_pixelmap_rotate(src, angle, destination, rot_cx, rot_cy);
            break;

        case GX_COLOR_FORMAT_MONOCHROME:
            status = _gx_utility_1bpp_pixelmap_rotate(src, angle, destination, rot_cx, rot_cy);
            break;

        default:
            return GX_INVALID_FORMAT;
        }
    }

    if ((src->flags & GX_PIXELMAP_ROTATED_90) ||
        (src->flags & GX_PIXELMAP_ROTATED_270))
    {
        GX_SWAP_VALS(destination->width, destination->height);

        if (rot_cx && rot_cy)
        {
            GX_SWAP_VALS(*rot_cx, *rot_cy);

            if (src->flags & GX_PIXELMAP_ROTATED_90)
            {
                *rot_cx = destination->width - 1 - (*rot_cx);
            }
            else
            {
                *rot_cy = destination->height - 1 - (*rot_cy);
            }
        }
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_pixelmap_simple_rotation                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service rotate a pixelmap by 90, 180 or 270 degree.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The pixelmap to rotate        */
/*    angle                                 The angle to rotate           */
/*    destination                           Destination buffer for        */
/*                                            rotated pixelmap.           */
/*    rot_cx                                X coordinate of rotation      */
/*                                            center                      */
/*    rot_cy                                Y coordinate of rotation      */
/*                                            center                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_32argb_pixelmap_simple_rotate                           */
/*                                          Rotate 32bpp pixelmap with    */
/*                                            simple case                 */
/*    _gx_utility_565rgb_pixelmap_simple_rotate                           */
/*                                          Rotate 565rgb pixelmap with   */
/*                                            simple case                 */
/*    _gx_utility_4444argb_pixelmap_simple_rotate                         */
/*                                          Rotate 4444argb pixelmap with */
/*                                            simple case                 */
/*    _gx_utility_8bpp_pixelmap_simple_rotate                             */
/*                                          Rotate 8bit palette pixelmap  */
/*                                            with simple case            */
/*    _gx_utility_332rgb_pixelmap_simple_rotate                           */
/*                                          Rotate 332rgb pixelmap with   */
/*                                            simple case                 */
/*    _gx_utility_4bpp_pixelmap_simple_rotate                             */
/*                                          Rotate 4bpp pixelmap with     */
/*                                            simple case                 */
/*    _gx_utility_1bpp_pixelmap_simple_rotate                             */
/*                                          Rotate 1bpp pixelmap with     */
/*                                            simple case                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
    angle = angle % 360;

    if ((angle == 0) || (angle % 90))
    {
        return GX_INVALID_VALUE;
    }
    else if (angle < 0)
    {
        angle += 360;
    }

    if ((_gx_system_memory_allocator == GX_NULL) || (_gx_system_memory_free == GX_NULL))
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    if (src->flags & GX_PIXELMAP_COMPRESSED)
    {
        return GX_INVALID_FORMAT;
    }

    memset(destination, 0, sizeof(GX_PIXELMAP));
    destination->format = src->format;
    destination->version_major = src->version_major;
    destination->version_minor = src->version_major;
    destination->flags = src->flags;

    switch (src->format)
    {
    case GX_COLOR_FORMAT_32ARGB:
    case GX_COLOR_FORMAT_24XRGB:
        /* Call 32argb pixelmap rotate. */
        _gx_utility_32argb_pixelmap_simple_rotate(src, angle, destination, rot_cx, rot_cy);
        break;

    case GX_COLOR_FORMAT_565RGB:
    case GX_COLOR_FORMAT_565BGR:
    case GX_COLOR_FORMAT_1555XRGB:
        /* Call 565rgb pixelmap rotate.  */
        _gx_utility_565rgb_pixelmap_simple_rotate(src, angle, destination, rot_cx, rot_cy);
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        _gx_utility_4444argb_pixelmap_simple_rotate(src, angle, destination, rot_cx, rot_cy);
        break;

    case  GX_COLOR_FORMAT_8BIT_PACKED_PIXEL:
        /* Call 332rgb pixelmap rotate.  */
        _gx_utility_332rgb_pixelmap_simple_rotate(src, angle, destination, rot_cx, rot_cy);
        break;

    case GX_COLOR_FORMAT_8BIT_PALETTE:
    case GX_COLOR_FORMAT_8BIT_ALPHAMAP:
        /* Call 8bpp pixelmap rotate.  */
        _gx_utility_8bpp_pixelmap_simple_rotate(src, angle, destination, rot_cx, rot_cy);
        break;

    case GX_COLOR_FORMAT_4BIT_GRAY:
        _gx_utility_4bpp_pixelmap_simple_rotate(src, angle, destination, rot_cx, rot_cy);
        break;

    case GX_COLOR_FORMAT_MONOCHROME:
        _gx_utility_1bpp_pixelmap_simple_rotate(src, angle, destination, rot_cx, rot_cy);
        break;

    default:
        return GX_INVALID_FORMAT;
    }
    if (destination->data)
    {
        return GX_SUCCESS;
    }
    return GX_FAILURE;
}

