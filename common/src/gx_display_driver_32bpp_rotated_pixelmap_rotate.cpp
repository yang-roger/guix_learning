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
#include "gx_canvas.h"
#include "gx_pixelmap.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_raw_rotate                */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that rotate an rotated uncompressed        */
/*    pixelmap without alpha.                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    angle                                 The angle to rotate           */
/*    cx                                    x-coord of rotate center      */
/*    cy                                    y-coord of rotate center      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_math_cos                  Compute the cosine value      */
/*    _gx_utility_math_sin                  Compute the sine value        */
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_rotate                    */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_pixelmap_raw_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                                 INT angle, INT cx, INT cy)
{
GX_COLOR     *get;
INT           srcxres;
INT           srcyres;
INT           cosv;
INT           sinv;
INT           alpha;
GX_COLOR      color;
INT           idxminx;
INT           idxmaxx;
INT           idxmaxy;
INT          *mx;
INT          *my;
INT           xres;
INT           yres;
INT           x;
INT           y;
INT           xx;
INT           yy;
GX_COLOR      a;
GX_COLOR      b;
GX_COLOR      c;
GX_COLOR      d;
INT           xdiff;
INT           ydiff;
INT           newxpos;
INT           newypos;
GX_RECTANGLE *clip;
GX_RECTANGLE  rotated_clip;
void        (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    GX_SET_32BPP_BLEND_FUNCTION(blend_func, context->display->color_format);

    clip = context->clip;

    mx = _gx_system_scratchpad;
    my = mx + 4;

    mx[0] = mx[3] = -1;
    mx[1] = mx[2] = 1;

    my[0] = my[1] = 1;
    my[2] = my[3] = -1;

    idxminx = (angle / 90) & 0x3;
    idxmaxx = (idxminx + 2) & 0x3;
    idxmaxy = (idxminx + 1) & 0x3;

    /* Calculate the source x and y center. */
    srcxres = pixelmap->height >> 1;
    srcyres = pixelmap->width >> 1;

    GX_SWAP_VALS(xpos, ypos);
    GX_SWAP_VALS(cx, cy);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        srcyres = pixelmap->width - 1 - srcyres;

        ypos = context->canvas->x_resolution - ypos - pixelmap->width;
        cy = pixelmap->width - cy - 1;

        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - 1 - clip->right);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - 1 - clip->left);
    }
    else
    {
        srcxres = pixelmap->height - 1 - srcxres;

        xpos = context->canvas->y_resolution - xpos - pixelmap->height;
        cx = pixelmap->height - cx - 1;

        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - 1 - clip->bottom);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - 1 - clip->top);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
    }

    cosv = _gx_utility_math_cos(GX_FIXED_VAL_MAKE(angle));
    sinv = _gx_utility_math_sin(GX_FIXED_VAL_MAKE(angle));

    xres = mx[idxmaxx] * (srcxres + 2) * cosv - my[idxmaxx] * (srcyres + 2) * sinv;
    yres = my[idxmaxy] * (srcyres + 2) * cosv + mx[idxmaxy] * (srcxres + 2) * sinv;

    xres = GX_FIXED_VAL_TO_INT(xres);
    yres = GX_FIXED_VAL_TO_INT(yres);

    /* Calculate the new rotation axis.  */

    x = (cx - srcxres) * cosv - (cy - srcyres) * sinv;
    y = (cy - srcyres) * cosv + (cx - srcxres) * sinv;

    x = GX_FIXED_VAL_TO_INT(x) + xres;
    y = GX_FIXED_VAL_TO_INT(y) + yres;

    newxpos = xpos + cx - x;
    newypos = ypos + cy - y;

    /* Loop through the destination's pixels.  */
    for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
    {
        for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
        {
            xx = (x - xres) * cosv + (y - yres) * sinv;
            yy = (y - yres) * cosv - (x - xres) * sinv;

            xdiff = GX_FIXED_VAL_TO_INT(xx << 8) & 0xff;
            ydiff = GX_FIXED_VAL_TO_INT(yy << 8) & 0xff;

            xx = GX_FIXED_VAL_TO_INT(xx) + srcxres;
            yy = GX_FIXED_VAL_TO_INT(yy) + srcyres;

            if ((xx >= -1) && (xx < pixelmap->height) &&
                (yy >= -1) && (yy < pixelmap->width))
            {
                if ((xx >= 0) && (xx < pixelmap->height - 1) &&
                    (yy >= 0) && (yy < pixelmap->width - 1))
                {
                    get = (GX_COLOR *)pixelmap->data;
                    get += yy * pixelmap->height;
                    get += xx;

                    a = *get;
                    b = *(get + 1);
                    c = *(get + pixelmap->height);
                    d = *(get + pixelmap->height + 1);

                    alpha = 0xff;
                }
                else
                {
                    get = (GX_COLOR *)pixelmap->data;

                    a = 0;
                    b = 0;
                    c = 0;
                    d = 0;
                    alpha = 0;

                    if (xx == -1)
                    {
                        /* Handle left edge.  */
                        if (yy >= 0)
                        {
                            b = *(get + yy * pixelmap->height);
                        }

                        if (yy < pixelmap->width - 1)
                        {
                            d = *(get + (yy + 1) * pixelmap->height);
                        }
                    }
                    else if (yy == -1)
                    {
                        /* Handle top edge.  */
                        c = *(get + xx);

                        if (xx < pixelmap->height - 1)
                        {
                            d = *(get + xx + 1);
                        }
                    }
                    else if (xx == pixelmap->height - 1)
                    {
                        /* Handle right edget.  */
                        a = *(get + yy * pixelmap->height + xx);

                        if (yy < pixelmap->width - 1)
                        {
                            c = *(get + (yy + 1) * pixelmap->height + xx);
                        }
                    }
                    else
                    {
                        /* Handle bottom edge.  */
                        a = *(get + yy * pixelmap->height + xx);
                        b = *(get + yy * pixelmap->height + xx + 1);
                    }

                    if (a)
                    {
                        alpha += (256 - xdiff) * (256 - ydiff);
                    }

                    if (b)
                    {
                        alpha += xdiff * (256 - ydiff);
                    }

                    if (c)
                    {
                        alpha += ydiff * (256 - xdiff);
                    }

                    if (d)
                    {
                        alpha += xdiff * ydiff;
                    }

                    alpha >>= 8;
                }

                color = gx_color_32argb_raw_from_4colors_2(a, b, c, d, xdiff, ydiff, alpha);

                blend_func(context, x + newxpos, y + newypos, color, (GX_UBYTE)alpha);
            }
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_32bppted_pixelmap_alpha_rotate                          */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that rotate an rotated uncompressed        */
/*    pixelmap with alpha.                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    angle                                 The angle to rotate           */
/*    cx                                    x-coord of rotate center      */
/*    cy                                    y-coord of rotate center      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_math_cos                  Compute the cosine value      */
/*    _gx_utility_math_sin                  Compute the sine value        */
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_rotate                    */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_pixelmap_alpha_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                                   INT angle, INT cx, INT cy)
{
GX_COLOR     *get;
INT           srcxres;
INT           srcyres;
INT           cosv;
INT           sinv;
INT           alpha;
GX_COLOR      color;
INT           idxminx;
INT           idxmaxx;
INT           idxmaxy;
INT          *mx;
INT          *my;
INT           xres;
INT           yres;
INT           x;
INT           y;
INT           xx;
INT           yy;
GX_COLOR      a;
GX_COLOR      b;
GX_COLOR      c;
GX_COLOR      d;
INT           xdiff;
INT           ydiff;
INT           newxpos;
INT           newypos;
GX_RECTANGLE *clip;
GX_RECTANGLE  rotated_clip;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    GX_SET_32BPP_BLEND_FUNCTION(blend_func, context->display->color_format);

    clip = context->clip;

    mx = _gx_system_scratchpad;
    my = mx + 4;

    mx[0] = mx[3] = -1;
    mx[1] = mx[2] = 1;

    my[0] = my[1] = 1;
    my[2] = my[3] = -1;

    idxminx = (angle / 90) & 0x3;
    idxmaxx = (idxminx + 2) & 0x3;
    idxmaxy = (idxminx + 1) & 0x3;

    /* Calculate the source x and y center.  */
    srcxres = pixelmap->height >> 1;
    srcyres = pixelmap->width >> 1;

    GX_SWAP_VALS(xpos, ypos);
    GX_SWAP_VALS(cx, cy);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        srcyres = pixelmap->width - 1 - srcyres;

        ypos = context->canvas->x_resolution - ypos - pixelmap->width;
        cy = pixelmap->width - cy - 1;

        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - 1 - clip->right);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - 1 - clip->left);
    }
    else
    {
        srcxres = pixelmap->height - 1 - srcxres;

        xpos = context->canvas->y_resolution - xpos - pixelmap->height;
        cx = pixelmap->height - cx - 1;

        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - 1 - clip->bottom);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - 1 - clip->top);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
    }

    cosv = _gx_utility_math_cos(GX_FIXED_VAL_MAKE(angle));
    sinv = _gx_utility_math_sin(GX_FIXED_VAL_MAKE(angle));

    xres = mx[idxmaxx] * (srcxres + 2) * cosv - my[idxmaxx] * (srcyres + 2) * sinv;
    yres = my[idxmaxy] * (srcyres + 2) * cosv + mx[idxmaxy] * (srcxres + 2) * sinv;

    xres = GX_FIXED_VAL_TO_INT(xres);
    yres = GX_FIXED_VAL_TO_INT(yres);

    x = GX_FIXED_VAL_TO_INT(xres) + xres;
    y = GX_FIXED_VAL_TO_INT(yres) + yres;

    newxpos = xpos + cx - x;
    newypos = ypos + cy - y;

    /* Loop through the source's pixels.  */
    for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
    {
        for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
        {
            xx = (x - xres) * cosv + (y - yres) * sinv;
            yy = (y - yres) * cosv - (x - xres) * sinv;

            xdiff = GX_FIXED_VAL_TO_INT(xx << 8) & 0xff;
            ydiff = GX_FIXED_VAL_TO_INT(yy << 8) & 0xff;

            xx = GX_FIXED_VAL_TO_INT(xx);
            yy = GX_FIXED_VAL_TO_INT(yy);

            xx += srcxres;
            yy += srcyres;

            if ((xx >= -1) && (xx < pixelmap->height) &&
                (yy >= -1) && (yy < pixelmap->width))
            {
                if ((xx >= 0) && (xx < pixelmap->height - 1) && \
                    (yy >= 0) && (yy < pixelmap->width - 1))
                {
                    get = (GX_COLOR *)pixelmap->data;
                    get += yy * pixelmap->height;
                    get += xx;

                    a = *get;
                    b = *(get + 1);
                    c = *(get + pixelmap->height);
                    d = *(get + pixelmap->height + 1);
                }
                else
                {
                    get = (GX_COLOR *)pixelmap->data;

                    a = 0;
                    b = a;
                    c = a;
                    d = a;

                    if (xx == -1)
                    {
                        /* Handle left edge.  */
                        if (yy >= 0)
                        {
                            b = *(get + yy * pixelmap->height);
                        }

                        if (yy < pixelmap->width - 1)
                        {
                            d = *(get + (yy + 1) * pixelmap->height);
                        }
                    }
                    else if (yy == -1)
                    {
                        /* Handle top edge.  */
                        c = *(get + xx);

                        if (xx < pixelmap->height - 1)
                        {
                            d = *(get + xx + 1);
                        }
                    }
                    else if (xx == pixelmap->height - 1)
                    {
                        /* Handle right edget.  */
                        a = *(get + yy * pixelmap->height + xx);

                        if (yy < pixelmap->width - 1)
                        {
                            c = *(get + (yy + 1) * pixelmap->height + xx);
                        }
                    }
                    else
                    {
                        /* Handle bottom edge.  */
                        a = *(get + yy * pixelmap->height + xx);
                        b = *(get + yy * pixelmap->height + xx + 1);
                    }
                }

                color = gx_color_32argb_alpha_from_4colors_2(a, b, c, d, xdiff, ydiff, alpha);

                blend_func(context, x + newxpos, y + newypos, color, (GX_UBYTE)alpha);
            }
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_simple_rotate             */
/*                                                                        */
/*                                                                        */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal help function that hangles 90, 180 and 270 degree rotated  */
/*    pixelmap rotation.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    angle                                 The angle to rotate           */
/*    cx                                    x-coord of rotate center      */
/*    cy                                    y-coord of rotate center      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_pixel_blend]       Basic display driver pixel    */
/*                                            blend function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_32bpp_rotated_pixelmap_simple_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                                    INT angle, INT cx, INT cy)
{
GX_COLOR     *putrow;
GX_COLOR     *put;
GX_COLOR     *get;
INT           width;
INT           height;
INT           x;
INT           y;
GX_RECTANGLE *clip;
GX_RECTANGLE  rotated_clip;
INT           newxpos;
INT           newypos;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    GX_SET_32BPP_BLEND_FUNCTION(blend_func, context->display->color_format);

    clip = context->clip;

    GX_SWAP_VALS(xpos, ypos);
    GX_SWAP_VALS(cx, cy);

    if (context->display->rotation_angle == GX_SCREEN_ROTATION_CW)
    {
        ypos = context->canvas->x_resolution - ypos - pixelmap->width;
        cy = pixelmap->width - cy - 1;

        rotated_clip.left = clip->top;
        rotated_clip.right = clip->bottom;
        rotated_clip.top = (GX_VALUE)(context->canvas->x_resolution - 1 - clip->right);
        rotated_clip.bottom = (GX_VALUE)(context->canvas->x_resolution - 1 - clip->left);
    }
    else
    {
        xpos = context->canvas->y_resolution - xpos - pixelmap->height;
        cx = pixelmap->height - cx - 1;

        rotated_clip.left = (GX_VALUE)(context->canvas->y_resolution - 1 - clip->bottom);
        rotated_clip.right = (GX_VALUE)(context->canvas->y_resolution - 1 - clip->top);
        rotated_clip.top = clip->left;
        rotated_clip.bottom = clip->right;
    }

    if (angle == 90)
    {
        width = pixelmap->width;
        height = pixelmap->height;

        newxpos = xpos + cx - (width - 1 - cy);
        newypos = ypos + cy - cx;

        putrow = context->memory;
        putrow += rotated_clip.top * context->pitch;
        putrow += rotated_clip.left;

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            put = putrow;

            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (GX_COLOR *)pixelmap->data;
                get += (width - 1 - x) * height;
                get += y;

                if (pixelmap->flags & GX_PIXELMAP_ALPHA)
                {
                    blend_func(context, rotated_clip.left + x, rotated_clip.top + y, (*get) | 0xff000000, (GX_UBYTE)((*get) >> 24));
                }
                else
                {
                    *put++ = *get;
                }
            }

            putrow += context->pitch;
        }
    }
    else if (angle == 180)
    {

        width = pixelmap->height;
        height = pixelmap->width;

        newxpos = xpos + cx - (width - 1 - cx);
        newypos = ypos + cy - (height - 1 - cy);

        putrow = context->memory;
        putrow += rotated_clip.top * context->pitch;
        putrow += rotated_clip.left;

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            put = putrow;
            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (GX_COLOR *)pixelmap->data;
                get += (height - 1 - y) * width;
                get += width - 1 - x;

                if (pixelmap->flags & GX_PIXELMAP_ALPHA)
                {
                    blend_func(context, rotated_clip.left + x, rotated_clip.top + y, (*get) | 0xff000000, (GX_UBYTE)((*get) >> 24));
                }
                else
                {
                    *put++ = *get;
                }
            }

            putrow += context->pitch;
        }
    }
    else
    {
        height = pixelmap->height;

        newxpos = xpos + cx - cy;
        newypos = ypos + cx - (height - 1 - cy);

        putrow = context->memory;
        putrow += rotated_clip.top * context->pitch;
        putrow += rotated_clip.left;

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            put = putrow;

            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (GX_COLOR *)pixelmap->data;
                get += x * height;
                get += height - 1 - y;

                if (pixelmap->flags & GX_PIXELMAP_ALPHA)
                {
                    blend_func(context, rotated_clip.left + x, rotated_clip.top + y, (*get) | 0xff000000, (GX_UBYTE)((*get) >> 24));
                }
                else
                {
                    *put++ = *get;
                }
            }

            putrow += context->pitch;
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_rotate                    */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service rotate a rotated pixelmap directly to canvas memory.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    angle                                 The angle to rotate           */
/*    rot_cx                                x-coord of rotating center.   */
/*    rot_cy                                y-coord of rotationg center.  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_pixelmap_simple_rotate             */
/*    _gx_display_driver_32bpp_rotated_pixelmap_alpha_rotate              */
/*    _gx_display_driver_32bpp_rotated_pixelmap_rotate                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_32bpp_rotated_pixelmap_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                      INT angle, INT rot_cx, INT rot_cy)
{
    switch (pixelmap->format)
    {
    case GX_COLOR_FORMAT_24XRGB:
    case GX_COLOR_FORMAT_32ARGB:
        if (angle % 90 == 0)
        {
            /* Simple angle rotate: 90 degree, 180 degree and 270 degree.  */
            _gx_display_driver_32bpp_rotated_pixelmap_simple_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
        }
        else
        {
            if (pixelmap->flags & GX_PIXELMAP_ALPHA)
            {
                /* With alpha.  */
                _gx_display_driver_32bpp_rotated_pixelmap_alpha_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
            }
            else
            {
                /* Without alpha.  */
                _gx_display_driver_32bpp_rotated_pixelmap_raw_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
            }
        }
        break;
    }

    return;
}

