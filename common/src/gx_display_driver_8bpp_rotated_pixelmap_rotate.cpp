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
#include "gx_canvas.h"
#include "gx_pixelmap.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_rotated_pixelmap_raw_rotate                 */
/*                                                           6.1.4        */
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
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_math_cos                  Compute the cosine value      */
/*    _gx_utility_math_sin                  Compute the sine value        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_8bpp_pixelmap_rotate                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_rotated_pixelmap_raw_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                        INT angle, INT cx, INT cy)
{
GX_UBYTE     *putrow;
GX_UBYTE     *put;
GX_UBYTE     *get;
INT           srcxres;
INT           srcyres;
INT           cosv;
INT           sinv;
INT           idxminx;
INT           idxmaxx;
INT           idxmaxy;
INT           mx[] = {-1, 1, 1, -1};
INT           my[] = {1, 1, -1, -1};
INT           xres;
INT           yres;
INT           x;
INT           y;
INT           xx;
INT           yy;
GX_RECTANGLE *clip;
GX_RECTANGLE  rotated_clip;
INT           newxpos;
INT           newypos;

    clip = context->clip;

    /* Set transparent color.  */
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

    xres = GX_FIXED_VAL_TO_INT((mx[idxmaxx] * (srcxres + 2) * cosv - my[idxmaxx] * (srcyres + 2) * sinv));
    yres = GX_FIXED_VAL_TO_INT((my[idxmaxy] * (srcyres + 2) * cosv + mx[idxmaxy] * (srcxres + 2) * sinv));

    putrow = (GX_UBYTE *)context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += rotated_clip.left;

    /* Calculate the new rotation axis. */
    x = GX_FIXED_VAL_TO_INT((cx - srcxres) * cosv - (cy - srcyres) * sinv);
    y = GX_FIXED_VAL_TO_INT((cy - srcyres) * cosv + (cx - srcxres) * sinv);

    x += xres;
    y += yres;

    newxpos = xpos + cx - x;
    newypos = ypos + cy - y;

    /* For every pixel in destination bitmap, find its position in source bitmap,
       and set the pixel with the value in source bitmap.  */
    for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
    {
        put = putrow;

        for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
        {
            xx = GX_FIXED_VAL_TO_INT((x - xres) * cosv + (y - yres) * sinv);
            yy = GX_FIXED_VAL_TO_INT((y - yres) * cosv - (x - xres) * sinv);

            xx += srcxres;
            yy += srcyres;

            if ((xx >= 0) && (xx < pixelmap->height) &&
                (yy >= 0) && (yy < pixelmap->width))
            {
                get = (GX_UBYTE *)pixelmap->data;
                get += yy * pixelmap->height;
                get += xx;

                *put = *get;
            }

            put++;
        }
        putrow += context->pitch;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_rotated_pixelmap_transparent_rotate         */
/*                                                                        */
/*                                                           6.1.4        */
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
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_math_cos                  Compute the cosine value      */
/*    _gx_utility_math_sin                  Compute the sine value        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_rotated_pixelmap_transparent_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                                INT angle, INT cx, INT cy)
{
GX_UBYTE     *putrow;
GX_UBYTE     *put;
GX_UBYTE     *get;
INT           srcxres;
INT           srcyres;
INT           cosv;
INT           sinv;
INT           idxminx;
INT           idxmaxx;
INT           idxmaxy;
INT           mx[] = {-1, 1, 1, -1};
INT           my[] = {1, 1, -1, -1};
INT           xres;
INT           yres;
INT           x;
INT           y;
INT           xx;
INT           yy;
GX_RECTANGLE *clip;
GX_RECTANGLE  rotated_clip;
INT           newxpos;
INT           newypos;

    clip = context->clip;

    /* Set transparent color.  */
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

    xres = GX_FIXED_VAL_TO_INT((mx[idxmaxx] * (srcxres + 2) * cosv - my[idxmaxx] * (srcyres + 2) * sinv));
    yres = GX_FIXED_VAL_TO_INT((my[idxmaxy] * (srcyres + 2) * cosv + mx[idxmaxy] * (srcxres + 2) * sinv));

    putrow = (GX_UBYTE *)context->memory;
    putrow += rotated_clip.top * context->pitch;
    putrow += rotated_clip.left;

    /* Calculate the new rotation axis. */
    x = (cx - srcxres) * cosv - (cy - srcyres) * sinv;
    y = (cy - srcyres) * cosv + (cx - srcxres) * sinv;

    x = GX_FIXED_VAL_TO_INT(x) + xres;
    y = GX_FIXED_VAL_TO_INT(y) + yres;

    newxpos = xpos + cx - x;
    newypos = ypos + cy - y;

    /* For every pixel in destination bitmap, find its position in source bitmap,
       and set the pixel with the value in source bitmap.  */
    for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
    {
        put = putrow;

        for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
        {
            xx = GX_FIXED_VAL_TO_INT((x - xres) * cosv + (y - yres) * sinv);
            yy = GX_FIXED_VAL_TO_INT((y - yres) * cosv - (x - xres) * sinv);

            xx += srcxres;
            yy += srcyres;

            if ((xx >= 0) && (xx < pixelmap->height) &&
                (yy >= 0) && (yy < pixelmap->width))
            {
                get = (GX_UBYTE *)pixelmap->data;
                get += yy * pixelmap->height;
                get += xx;

                if ((*get) != pixelmap->transparent_color)
                {
                    *put = *get;
                }
            }
            put++;
        }
        putrow += context->pitch;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_rotated_pixelmap_simple_rotate              */
/*                                                                        */
/*                                                           6.1.4        */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_8bpp_pixelmap_rotate                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_rotated_pixelmap_simple_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                           INT angle, INT cx, INT cy)
{
GX_UBYTE     *putrow;
GX_UBYTE     *put;
GX_UBYTE     *get;
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

        putrow = (GX_UBYTE *)context->memory;
        putrow += rotated_clip.top * context->pitch;
        putrow += rotated_clip.left;

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            put = putrow;

            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (GX_UBYTE *)pixelmap->data;
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

        putrow = (GX_UBYTE *)context->memory;
        putrow += rotated_clip.top * context->pitch;
        putrow += rotated_clip.left;

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            put = putrow;
            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (GX_UBYTE *)pixelmap->data;
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

        putrow = (GX_UBYTE *)context->memory;
        putrow += rotated_clip.top * context->pitch;
        putrow += rotated_clip.left;

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            put = putrow;

            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (GX_UBYTE *)pixelmap->data;
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
/*    _gx_display_driver_8bpp_rotated_pixelmap_simple_transparent_rotate  */
/*                                                                        */
/*                                                           6.1.4        */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_driver_8bpp_pixelmap_rotate                             */
/*                                                                        */
/**************************************************************************/
static void _gx_display_driver_8bpp_rotated_pixelmap_simple_transparent_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                                                       INT angle, INT cx, INT cy)
{
GX_UBYTE     *putrow;
GX_UBYTE     *put;
GX_UBYTE     *get;
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

        putrow = (GX_UBYTE *)context->memory;
        putrow += rotated_clip.top * context->pitch;
        putrow += rotated_clip.left;

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            put = putrow;

            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (GX_UBYTE *)pixelmap->data;
                get += (width - 1 - x) * height;
                get += y;

                if ((*get) != pixelmap->transparent_color)
                {
                    *put = *get;
                }
                put++;
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

        putrow = (GX_UBYTE *)context->memory;
        putrow += rotated_clip.top * context->pitch;
        putrow += rotated_clip.left;

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            put = putrow;
            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (GX_UBYTE *)pixelmap->data;
                get += (height - 1 - y) * width;
                get += width - 1 - x;

                if ((*get) != pixelmap->transparent_color)
                {
                    *put = *get;
                }
                put++;
            }

            putrow += context->pitch;
        }
    }
    else
    {
        height = pixelmap->height;

        newxpos = xpos + cx - cy;
        newypos = ypos + cx - (height - 1 - cy);

        putrow = (GX_UBYTE *)context->memory;
        putrow += rotated_clip.top * context->pitch;
        putrow += rotated_clip.left;

        for (y = rotated_clip.top - newypos; y <= rotated_clip.bottom - newypos; y++)
        {
            put = putrow;

            for (x = rotated_clip.left - newxpos; x <= rotated_clip.right - newxpos; x++)
            {
                get = (GX_UBYTE *)pixelmap->data;
                get += x * height;
                get += height - 1 - y;

                if ((*get) != pixelmap->transparent_color)
                {
                    *put = *get;
                }
                put++;
            }

            putrow += context->pitch;
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_8bpp_rotated_pixelmap_rotate                     */
/*                                                           6.1.4        */
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
/*    _gx_display_driver_8bpp_rotated_pixelmap_simple_transparent_rotate  */
/*    _gx_display_driver_8bpp_rotated_pixelmap_simple_rotate              */
/*    _gx_display_driver_8bpp_rotated_pixelmap_transparent_rotate         */
/*    _gx_display_driver_8bpp_rotated_pixelmap_raw_rotate                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_8bpp_rotated_pixelmap_rotate(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap,
                                             INT angle, INT rot_cx, INT rot_cy)
{
    if (angle % 90 == 0)
    {
        /* Simple angle rotate: 90 degree, 180 degree and 270 degree.  */
        if (pixelmap->flags & GX_PIXELMAP_TRANSPARENT)
        {
            _gx_display_driver_8bpp_rotated_pixelmap_simple_transparent_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
        }
        else
        {
            _gx_display_driver_8bpp_rotated_pixelmap_simple_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
        }
    }
    else
    {
        if (pixelmap->flags & GX_PIXELMAP_TRANSPARENT)
        {
            /* no compression or alpha */
            _gx_display_driver_8bpp_rotated_pixelmap_transparent_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
        }
        else
        {
            /* no compression or alpha */
            _gx_display_driver_8bpp_rotated_pixelmap_raw_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
        }
    }

    return;
}

