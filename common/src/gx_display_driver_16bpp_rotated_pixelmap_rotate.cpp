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

#define REDVAL(_c)   (GX_UBYTE)(((_c) >> 11) & 0x1f)
#define GREENVAL(_c) (GX_UBYTE)(((_c) >> 5) & 0x3f)
#define BLUEVAL(_c)  (GX_UBYTE)(((_c)) & 0x1f)

#define ASSEMBLECOLOR(_r, _g, _b) \
    ((((_r) & 0x1f) << 11) |      \
     (((_g) & 0x3f) << 5) |       \
     (((_b) & 0x1f)))



#include "gx_display.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_canvas.h"
#include "gx_pixelmap.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_rotated_pixelmap_raw_rotate               */
/*                                                                        */
/*                                                           6.1.3        */
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
/*    [gx_display_driver_pixel_blend]       Display driver basic pixel    */
/*                                             blend function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_rotated_pixelmap_raw_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                                  INT angle, INT cx, INT cy)
{
USHORT       *get;
INT           srcxres;
INT           srcyres;
INT           cosv;
INT           sinv;
INT           alpha;
USHORT        red;
USHORT        green;
USHORT        blue;
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
USHORT        a;
USHORT        b;
USHORT        c;
USHORT        d;
INT           xdiff;
INT           ydiff;
INT           newxpos;
INT           newypos;
GX_RECTANGLE *clip;
GX_RECTANGLE  rotated_clip;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    clip = context->clip;
    blend_func = _gx_display_driver_565rgb_pixel_blend;

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

    xres = GX_FIXED_VAL_TO_INT(mx[idxmaxx] * (srcxres + 2) * cosv - my[idxmaxx] * (srcyres + 2) * sinv);
    yres = GX_FIXED_VAL_TO_INT(my[idxmaxy] * (srcyres + 2) * cosv + mx[idxmaxy] * (srcxres + 2) * sinv);

    /* Calculate the new rotation axis. */

    x = GX_FIXED_VAL_TO_INT((cx - srcxres) * cosv - (cy - srcyres) * sinv);
    y = GX_FIXED_VAL_TO_INT((cy - srcyres) * cosv + (cx - srcxres) * sinv);

    x += xres;
    y += yres;

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
                    get = (USHORT *)pixelmap->data;
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
                    get = (USHORT *)pixelmap->data;

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
                            b = *(get + yy * pixelmap->height);
                            alpha += xdiff * (256 - ydiff);
                        }

                        if (yy < pixelmap->width - 1)
                        {
                            d = *(get + (yy + 1) * pixelmap->height);
                            alpha += xdiff * ydiff;
                        }
                    }
                    else if (yy == -1)
                    {
                        /* handle top edge.  */
                        c = *(get + xx);
                        alpha += ydiff * (256 - xdiff);

                        if (xx < pixelmap->height - 1)
                        {
                            d = *(get + xx + 1);
                            alpha += xdiff * ydiff;
                        }
                    }
                    else if (xx == pixelmap->height - 1)
                    {
                        /* handle right edge. */
                        a = *(get + yy * pixelmap->height + xx);
                        alpha += (256 - xdiff) * (256 - ydiff);

                        if (yy < pixelmap->width - 1)
                        {
                            c = *(get + (yy + 1) * pixelmap->height + xx);
                            alpha += ydiff * (256 - xdiff);
                        }
                    }
                    else
                    {
                        /* handle bottom edge. */
                        a = *(get + yy * pixelmap->height + xx);
                        alpha += (256 - xdiff) * (256 - ydiff);

                        b = *(get + yy * pixelmap->height + xx + 1);
                        alpha += xdiff * (256 - ydiff);
                    }

                    alpha >>= 8;
                }

                red = (USHORT)((REDVAL(a) * (256 - xdiff) * (256 - ydiff) +
                                REDVAL(b) * xdiff * (256 - ydiff) +
                                REDVAL(c) * ydiff * (256 - xdiff) +
                                REDVAL(d) * xdiff * ydiff) >> 16);

                green = (USHORT)((GREENVAL(a) * (256 - xdiff) * (256 - ydiff) +
                                  GREENVAL(b) * xdiff * (256 - ydiff) +
                                  GREENVAL(c) * ydiff * (256 - xdiff) +
                                  GREENVAL(d) * xdiff * ydiff) >> 16);

                blue = (USHORT)((BLUEVAL(a) * (256 - xdiff) * (256 - ydiff) +
                                 BLUEVAL(b) * xdiff * (256 - ydiff) +
                                 BLUEVAL(c) * ydiff * (256 - xdiff) +
                                 BLUEVAL(d) * xdiff * ydiff) >> 16);

                if ((alpha > 0) && (alpha < 0xff))
                {
                    red = (USHORT)((red << 8) / alpha);
                    green = (USHORT)((green << 8) / alpha);
                    blue = (USHORT)((blue << 8) / alpha);
                }

                red = red > 31 ? 31 : red;
                green = green > 63 ? 63 : green;
                blue = blue > 31 ? 31 : blue;
                alpha = alpha > 255 ? 255 : alpha;

                blend_func(context, x + newxpos, y + newypos, (GX_COLOR)ASSEMBLECOLOR(red, green, blue), (GX_UBYTE)alpha);
            }
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_rotated_pixelmap_alpha_rotate             */
/*                                                                        */
/*                                                           6.1.3        */
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
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_math_cos                  Compute the cosine value      */
/*    _gx_utility_math_sin                  Compute the sine value        */
/*    [gx_display_driver_pixel_blend]       Display driver basic pixel    */
/*                                             blend function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_rotated_pixelmap_alpha_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                                    INT angle, INT cx, INT cy)
{
USHORT       *get;
GX_UBYTE     *getalpha;
INT           srcxres;
INT           srcyres;
INT           cosv;
INT           sinv;
USHORT        red;
USHORT        green;
USHORT        blue;
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
USHORT        a;
USHORT        b;
USHORT        c;
USHORT        d;
USHORT        alpha[4];
GX_FIXED_VAL  xdiff;
GX_FIXED_VAL  ydiff;
INT           newxpos;
INT           newypos;
GX_RECTANGLE *clip;
GX_RECTANGLE  rotated_clip;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    clip = context->clip;
    blend_func = _gx_display_driver_565rgb_pixel_blend;

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

    xres = GX_FIXED_VAL_TO_INT(mx[idxmaxx] * (srcxres + 2) * cosv - my[idxmaxx] * (srcyres + 2) * sinv);
    yres = GX_FIXED_VAL_TO_INT(my[idxmaxy] * (srcyres + 2) * cosv + mx[idxmaxy] * (srcxres + 2) * sinv);

    x = GX_FIXED_VAL_TO_INT((cx - srcxres) * cosv - (cy - srcyres) * sinv);
    y = GX_FIXED_VAL_TO_INT((cy - srcyres) * cosv + (cx - srcxres) * sinv);

    x += xres;
    y += yres;

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
                    get = (USHORT *)pixelmap->data;
                    get += yy * pixelmap->height;
                    get += xx;

                    getalpha = (GX_UBYTE *)pixelmap->aux_data;
                    getalpha += yy * pixelmap->height;
                    getalpha += xx;

                    a = *get;
                    alpha[0] = *getalpha;

                    b = *(get + 1);
                    alpha[1] = *(getalpha + 1);

                    c = *(get + pixelmap->height);
                    alpha[2] = *(getalpha + pixelmap->height);

                    d = *(get + pixelmap->height + 1);
                    alpha[3] = *(getalpha + pixelmap->height + 1);
                }
                else
                {
                    get = (USHORT *)pixelmap->data;
                    getalpha = (GX_UBYTE *)pixelmap->aux_data;

                    a = 0;
                    b = 0;
                    c = 0;
                    d = 0;

                    if (xx == -1)
                    {
                        /* handle left edge.  */
                        if (yy >= 0)
                        {
                            b = *(get + yy * pixelmap->height);
                            alpha[1] = *(getalpha + yy * pixelmap->height);
                        }

                        if (yy < pixelmap->width - 1)
                        {
                            d = *(get + (yy + 1) * pixelmap->height);
                            alpha[3] = *(getalpha + (yy + 1) * pixelmap->height);
                        }
                    }
                    else if (yy == -1)
                    {
                        /* handle top edge.  */
                        c = *(get + xx);
                        alpha[2] = *(getalpha + xx);

                        if (xx < pixelmap->height - 1)
                        {
                            d = *(get + xx + 1);
                            alpha[3] = *(getalpha + xx + 1);
                        }
                    }
                    else if (xx == pixelmap->height - 1)
                    {
                        /* handle right edget. */
                        a = *(get + yy * pixelmap->height + xx);
                        alpha[0] = *(getalpha + yy * pixelmap->height + xx);

                        if (yy < pixelmap->width - 1)
                        {
                            c = *(get + (yy + 1) * pixelmap->height + xx);
                            alpha[2] = *(getalpha + (yy + 1) * pixelmap->height + xx);
                        }
                    }
                    else
                    {
                        /* handle bottom edge. */
                        a = *(get + yy * pixelmap->height + xx);
                        alpha[0] = *(getalpha + yy * pixelmap->height + xx);

                        b = *(get + yy * pixelmap->height + xx + 1);
                        alpha[1] = *(getalpha + yy * pixelmap->height + xx + 1);
                    }

                    if (!a)
                    {
                        alpha[0] = 0;
                    }

                    if (!b)
                    {
                        alpha[1] = 0;
                    }

                    if (!c)
                    {
                        alpha[2] = 0;
                    }

                    if (!d)
                    {
                        alpha[3] = 0;
                    }
                }

                red = (USHORT)((REDVAL(a) * alpha[0] * (256 - xdiff) * (256 - ydiff) +
                                REDVAL(b) * alpha[1] * xdiff * (256 - ydiff) +
                                REDVAL(c) * alpha[2] * ydiff * (256 - xdiff) +
                                REDVAL(d) * alpha[3] * xdiff * ydiff) >> 16);

                green = (USHORT)((GREENVAL(a) * alpha[0] * (256 - xdiff) * (256 - ydiff) +
                                  GREENVAL(b) * alpha[1] * xdiff * (256 - ydiff) +
                                  GREENVAL(c) * alpha[2] * ydiff * (256 - xdiff) +
                                  GREENVAL(d) * alpha[3] * xdiff * ydiff) >> 16);

                blue = (USHORT)((BLUEVAL(a) * alpha[0] * (256 - xdiff) * (256 - ydiff) +
                                 BLUEVAL(b) * alpha[1] * xdiff * (256 - ydiff) +
                                 BLUEVAL(c) * alpha[2] * ydiff * (256 - xdiff) +
                                 BLUEVAL(d) * alpha[3] * xdiff * ydiff) >> 16);

                alpha[0] = (USHORT)((alpha[0] * (256 - xdiff) * (256 - ydiff) +
                                     alpha[1] * xdiff * (256 - ydiff) +
                                     alpha[2] * ydiff * (256 - xdiff) +
                                     alpha[3] * xdiff * ydiff) >> 16);

                if (alpha[0])
                {
                    red /= alpha[0];
                    green /= alpha[0];
                    blue /= alpha[0];
                }

                red = red > 31 ? 31 : red;
                green = green > 63 ? 63 : green;
                blue = blue > 31 ? 31 : blue;
                alpha[0] = alpha[0] > 255 ? 255 : alpha[0];

                blend_func(context, x + newxpos, y + newypos, (GX_COLOR)ASSEMBLECOLOR(red, green, blue), (GX_UBYTE)alpha[0]);
            }
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_16bpp_rotated_pixelmap_simple_rotate             */
/*                                                                        */
/*                                                           6.1.3        */
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
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_16bpp_rotated_pixelmap_simple_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                                    INT angle, INT cx, INT cy)
{
USHORT       *putrow;
USHORT       *put;
USHORT       *get;
INT           width;
INT           height;
INT           x;
INT           y;
GX_RECTANGLE *clip;
GX_RECTANGLE  rotated_clip;
INT           newxpos;
INT           newypos;

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

        putrow = (USHORT *)context->memory;
        putrow += rotated_clip.top * context->pitch;
        putrow += rotated_clip.left;

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            put = putrow;

            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (USHORT *)pixelmap->data;
                get += (width - 1 - x) * height;
                get += y;

                *put++ = *get;
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

        putrow = (USHORT *)context->memory;
        putrow += rotated_clip.top * context->pitch;
        putrow += rotated_clip.left;

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            put = putrow;
            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (USHORT *)pixelmap->data;
                get += (height - 1 - y) * width;
                get += width - 1 - x;

                *put++ = *get;
            }

            putrow += context->pitch;
        }
    }
    else
    {
        height = pixelmap->height;

        newxpos = xpos + cx - cy;
        newypos = ypos + cx - (height - 1 - cy);

        putrow = (USHORT *)context->memory;
        putrow += rotated_clip.top * context->pitch;
        putrow += rotated_clip.left;

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            put = putrow;

            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (USHORT *)pixelmap->data;
                get += x * height;
                get += height - 1 - y;

                *put++ = *get;
            }

            putrow += context->pitch;
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_rotated_pixelmap_simple_alpha_rotate      */
/*                                                                        */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal help function that hangles 90, 180 and 270 degree pixelmap */
/*    rotation with alpha channel.                                        */
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
/*    [gx_display_driver_pixel_blend]       Display driver basic pixel    */
/*                                             blend function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_565rgb_rotated_pixelmap_simple_alpha_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                                           INT angle, INT cx, INT cy)
{
USHORT       *get;
GX_UBYTE     *getalpha;
INT           width;
INT           height;
INT           x;
INT           y;
GX_RECTANGLE *clip;
GX_RECTANGLE  rotated_clip;
INT           newxpos;
INT           newypos;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    clip = context->clip;
    blend_func = _gx_display_driver_565rgb_pixel_blend;

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

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (USHORT *)pixelmap->data;
                getalpha = (GX_UBYTE *)pixelmap->aux_data;
                get += (width - 1 - x) * height;
                get += y;
                getalpha += (width - 1 - x) * height;
                getalpha += y;
                blend_func(context, x + newxpos, y + newypos, *get, *getalpha);
            }
        }
    }
    else if (angle == 180)
    {

        width = pixelmap->height;
        height = pixelmap->width;

        newxpos = xpos + cx - (width - 1 - cx);
        newypos = ypos + cy - (height - 1 - cy);


        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (USHORT *)pixelmap->data;
                getalpha = (GX_UBYTE *)pixelmap->aux_data;
                get += (height - 1 - y) * width;
                get += width - 1 - x;
                getalpha += (height - 1 - y) * width;
                getalpha += width - 1 - x;

                blend_func(context, x + newxpos, y + newypos, *get, *getalpha);
            }
        }
    }
    else
    {
        height = pixelmap->height;

        newxpos = xpos + cx - cy;
        newypos = ypos + cx - (height - 1 - cy);

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (USHORT *)pixelmap->data;
                getalpha = (GX_UBYTE *)pixelmap->aux_data;
                get += x * height;
                get += height - 1 - y;
                getalpha += x * height;
                getalpha += height - 1 - y;

                blend_func(context, x + newxpos, y + newypos, *get, *getalpha);
            }
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_rotated_pixelmap_rotate                   */
/*                                                           6.1.3        */
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
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_565rgb_rotated_pixelmap_simple_alpha_rotate      */
/*                                          Real rotate functin           */
/*    _gx_display_driver_16bpp_rotated_pixelmap_simple_rotate             */
/*                                          Real rotate functin           */
/*    _gx_display_driver_565rgb_rotated_pixelmap_alpha_rotate             */
/*                                          Real rotate functin           */
/*    _gx_display_driver_565rgb_rotated_pixelmap_raw_rotate               */
/*                                          Real rotate functin           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_565rgb_rotated_pixelmap_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                       INT angle, INT rot_cx, INT rot_cy)
{
    switch (pixelmap->format)
    {
    case GX_COLOR_FORMAT_565RGB:
        if (angle % 90 == 0)
        {
            /* Simple angle rotate: 90 degree, 180 degree and 270 degree.  */
            if (pixelmap->flags & GX_PIXELMAP_ALPHA)
            {
                _gx_display_driver_565rgb_rotated_pixelmap_simple_alpha_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
            }
            else
            {
                _gx_display_driver_16bpp_rotated_pixelmap_simple_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
            }
            break;
        }
        else
        {
            /* General rotation case. */
            if (pixelmap->flags & GX_PIXELMAP_ALPHA)
            {
                /* alpha, no compression */
                _gx_display_driver_565rgb_rotated_pixelmap_alpha_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
            }
            else
            {

                /* no compression or alpha */
                _gx_display_driver_565rgb_rotated_pixelmap_raw_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
            }
            break;
        }
    }
    return;
}

