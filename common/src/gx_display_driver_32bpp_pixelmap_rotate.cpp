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
#include "gx_pixelmap.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_24xrgb_pixelmap_rotate                           */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that rotate an uncompressed pixelmap       */
/*      without alpha.                                                    */
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
/*    _gx_display_driver_32bpp_pixelmap_rotate                            */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_pixelmap_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
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
GX_DISPLAY   *display;
GX_RECTANGLE *clip;
void        (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    clip = context->clip;
    display = context->display;
    blend_func = display->driver_pixel_blend;

    if (!blend_func)
    {
        return;
    }

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
    srcxres = pixelmap->width >> 1;
    srcyres = pixelmap->height >> 1;

    cosv = _gx_utility_math_cos(GX_FIXED_VAL_MAKE(angle));
    sinv = _gx_utility_math_sin(GX_FIXED_VAL_MAKE(angle));

    xres = mx[idxmaxx] * (srcxres + 2) * cosv - my[idxmaxx] * (srcyres + 2) * sinv;
    yres = my[idxmaxy] * (srcyres + 2) * cosv + mx[idxmaxy] * (srcxres + 2) * sinv;

    xres = GX_FIXED_VAL_TO_INT(xres);
    yres = GX_FIXED_VAL_TO_INT(yres);

    /* Calculate the new rotation axis. */

    x = (cx - srcxres) * cosv - (cy - srcyres) * sinv;
    y = (cy - srcyres) * cosv + (cx - srcxres) * sinv;

    xres = GX_FIXED_VAL_TO_INT(x) + xres;
    yres = GX_FIXED_VAL_TO_INT(y) + yres;

    newxpos = xpos + cx - xres;
    newypos = ypos + cy - yres;

    /* Loop through the destination's pixels.  */
    for (y = clip->top - newypos; y <= clip->bottom - newypos; y++)
    {
        for (x = clip->left - newxpos; x <= clip->right - newxpos; x++)
        {
            xx = (x - xres) * cosv + (y - yres) * sinv;
            yy = (y - yres) * cosv - (x - xres) * sinv;

            xdiff = GX_FIXED_VAL_TO_INT(xx << 8) & 0xff;
            ydiff = GX_FIXED_VAL_TO_INT(yy << 8) & 0xff;

            xx = GX_FIXED_VAL_TO_INT(xx) + cx;
            yy = GX_FIXED_VAL_TO_INT(yy) + cy;

            if ((xx >= -1) && (xx < pixelmap->width) &&
                (yy >= -1) && (yy < pixelmap->height))
            {
                if ((xx >= 0) && (xx < pixelmap->width - 1) &&
                    (yy >= 0) && (yy < pixelmap->height - 1))
                {
                    get = (GX_COLOR *)pixelmap->data;
                    get += yy * pixelmap->width;
                    get += xx;

                    a = *get;
                    b = *(get + 1);
                    c = *(get + pixelmap->width);
                    d = *(get + pixelmap->width + 1);

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
                        /* handle left edge.  */
                        if (yy >= 0)
                        {
                            b = *(get + yy * pixelmap->width);
                        }

                        if (yy < pixelmap->height - 1)
                        {
                            d = *(get + (yy + 1) * pixelmap->width);
                        }
                    }
                    else if (yy == -1)
                    {
                        /* handle top edge.  */
                        c = *(get + xx);

                        if (xx < pixelmap->width - 1)
                        {
                            d = *(get + xx + 1);
                        }
                    }
                    else if (xx == pixelmap->width - 1)
                    {
                        /* handle right edget. */
                        a = *(get + yy * pixelmap->width + xx);

                        if (yy < pixelmap->height - 1)
                        {
                            c = *(get + (yy + 1) * pixelmap->width + xx);
                        }
                    }
                    else
                    {
                        /* handle bottom edge. */
                        a = *(get + yy * pixelmap->width + xx);
                        b = *(get + yy * pixelmap->width + xx + 1);
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
/*    _gx_utility_24xrgb_pixelmap_alpha_rotate                            */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that rotate an uncompressed pixelmap       */
/*      with alpha.                                                       */
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
/*    _gx_display_driver_32bpp_pixelmap_rotate                            */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_24xrgb_pixelmap_alpha_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
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
GX_DISPLAY   *display;
GX_RECTANGLE *clip;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    clip = context->clip;
    display = context->display;
    blend_func = display->driver_pixel_blend;

    if (!blend_func)
    {
        return;
    }

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
    srcxres = pixelmap->width >> 1;
    srcyres = pixelmap->height >> 1;

    /* Calculate the new rotation axis. */
    cosv = _gx_utility_math_cos(GX_FIXED_VAL_MAKE(angle));
    sinv = _gx_utility_math_sin(GX_FIXED_VAL_MAKE(angle));

    xres = mx[idxmaxx] * (srcxres + 2) * cosv - my[idxmaxx] * (srcyres + 2) * sinv;
    yres = my[idxmaxy] * (srcyres + 2) * cosv + mx[idxmaxy] * (srcxres + 2) * sinv;

    xres = GX_FIXED_VAL_TO_INT(xres);
    yres = GX_FIXED_VAL_TO_INT(yres);

    x = (cx - srcxres) * cosv - (cy - srcyres) * sinv;
    y = (cy - srcyres) * cosv + (cx - srcxres) * sinv;

    xres = GX_FIXED_VAL_TO_INT(x) + xres;
    yres = GX_FIXED_VAL_TO_INT(y) + yres;

    newxpos = xpos + cx - xres;
    newypos = ypos + cy - yres;

    /* Loop through the source's pixels.  */
    for (y = clip->top - newypos; y <= clip->bottom - newypos; y++)
    {
        for (x = clip->left - newxpos; x <= clip->right - newxpos; x++)
        {
            xx = (x - xres) * cosv + (y - yres) * sinv;
            yy = (y - yres) * cosv - (x - xres) * sinv;

            xdiff = GX_FIXED_VAL_TO_INT(xx << 8) & 0xff;
            ydiff = GX_FIXED_VAL_TO_INT(yy << 8) & 0xff;

            xx = GX_FIXED_VAL_TO_INT(xx) + cx;
            yy = GX_FIXED_VAL_TO_INT(yy) + cy;

            if ((xx >= -1) && (xx < pixelmap->width) &&
                (yy >= -1) && (yy < pixelmap->height))
            {
                if ((xx >= 0) && (xx < pixelmap->width - 1) && \
                    (yy >= 0) && (yy < pixelmap->height - 1))
                {
                    get = (GX_COLOR *)pixelmap->data;
                    get += yy * pixelmap->width;
                    get += xx;

                    a = *get;
                    b = *(get + 1);
                    c = *(get + pixelmap->width);
                    d = *(get + pixelmap->width + 1);
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
                        /* handle left edge.  */
                        if (yy >= 0)
                        {
                            b = *(get + yy * pixelmap->width);
                        }

                        if (yy < pixelmap->height - 1)
                        {
                            d = *(get + (yy + 1) * pixelmap->width);
                        }
                    }
                    else if (yy == -1)
                    {
                        /* handle top edge.  */
                        c = *(get + xx);

                        if (xx < pixelmap->width - 1)
                        {
                            d = *(get + xx + 1);
                        }
                    }
                    else if (xx == pixelmap->width - 1)
                    {
                        /* handle right edget. */
                        a = *(get + yy * pixelmap->width + xx);

                        if (yy < pixelmap->height - 1)
                        {
                            c = *(get + (yy + 1) * pixelmap->width + xx);
                        }
                    }
                    else
                    {
                        /* handle bottom edge. */
                        a = *(get + yy * pixelmap->width + xx);
                        b = *(get + yy * pixelmap->width + xx + 1);
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
/*    _gx_display_driver_24xrgb_pixelmap_simple_rotate                    */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal help function that hangles 90, 180 and 270 degree pixelmap */
/*    rotation.                                                           */
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
static void _gx_display_driver_24xrgb_pixelmap_simple_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
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
INT           newxpos;
INT           newypos;

GX_DISPLAY   *display;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    display = context->display;
    blend_func = display->driver_pixel_blend;

    clip = context->clip;

    if (angle == 90)
    {
        width = pixelmap->height;
        height = pixelmap->width;

        newxpos = xpos + cx - (width - 1 - cy);
        newypos = ypos + cy - cx;

        putrow = context->memory;
        putrow += clip->top * context->pitch;
        putrow += clip->left;

        for (y = clip->top - newypos; y <= clip->bottom - newypos; y++)
        {
            put = putrow;

            for (x = clip->left - newxpos; x <= clip->right - newxpos; x++)
            {
                get = (GX_COLOR *)pixelmap->data;
                get += (width - 1 - x) * height;
                get += y;

                if (pixelmap->flags & GX_PIXELMAP_ALPHA)
                {
                    blend_func(context, clip->left + x, clip->top + y, *get, (GX_UBYTE)((*get) >> 24));
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

        width = pixelmap->width;
        height = pixelmap->height;

        newxpos = xpos + cx - (width - 1 - cx);
        newypos = ypos + cy - (height - 1 - cy);

        putrow = context->memory;
        putrow += clip->top * context->pitch;
        putrow += clip->left;

        for (y = clip->top - newypos; y <= clip->bottom - newypos; y++)
        {
            put = putrow;
            for (x = clip->left - newxpos; x <= clip->right - newxpos; x++)
            {
                get = (GX_COLOR *)pixelmap->data;
                get += (height - 1 - y) * width;
                get += width - 1 - x;

                if (pixelmap->flags & GX_PIXELMAP_ALPHA)
                {
                    blend_func(context, clip->left + x, clip->top + y, *get, (GX_UBYTE)((*get) >> 24));
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
        height = pixelmap->width;

        newxpos = xpos + cx - cy;
        newypos = ypos + cx - (height - 1 - cy);

        putrow = context->memory;
        putrow += clip->top * context->pitch;
        putrow += clip->left;

        for (y = clip->top - newypos; y <= clip->bottom - newypos; y++)
        {
            put = putrow;

            for (x = clip->left - newxpos; x <= clip->right - newxpos; x++)
            {
                get = (GX_COLOR *)pixelmap->data;
                get += x * height;
                get += height - 1 - y;

                if (pixelmap->flags & GX_PIXELMAP_ALPHA)
                {
                    blend_func(context, clip->left + x, clip->top + y, *get, (GX_UBYTE)((*get) >> 24));
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
/*    _gx_display_driver_32bpp_pixelmap_rotate                            */
/*                                                           6.1.9        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service rotate a pixelmap directly to canvas memory.           */
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
/*    _gx_display_driver_24xrgb_pixelmap_simple_rotate                    */
/*                                          Real display driver pixelmap  */
/*                                            rotate function             */
/*    _gx_display_driver_24xrgb_pixelmap_alpha_rotate                     */
/*                                          Real display driver pixelmap  */
/*                                            rotate function             */
/*    _gx_display_driver_24xrgb_pixelmap_rotate                           */
/*                                          Real display driver pixelmap  */
/*                                            rotate function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_32bpp_pixelmap_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                              INT angle, INT rot_cx, INT rot_cy)
{
    switch (pixelmap->format)
    {
    case GX_COLOR_FORMAT_24XRGB:
    case GX_COLOR_FORMAT_32ARGB:
        if (angle % 90 == 0)
        {
            /* Simple angle rotate: 90 degree, 180 degree and 270 degree.  */
            _gx_display_driver_24xrgb_pixelmap_simple_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
        }
        else
        {
            if (pixelmap->flags & GX_PIXELMAP_ALPHA)
            {
                /* with alpha. */
                _gx_display_driver_24xrgb_pixelmap_alpha_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
            }
            else
            {
                /* without alpha */
                _gx_display_driver_24xrgb_pixelmap_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
            }
        }
        break;
    }

    return;
}

