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
/**   Cursor Management (Cursor)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_text_input_cursor.h"

#include "gx_context.h"
#include "gx_canvas.h"

void GX_TEXT_INPUT_CURSOR::height_set_(GX_UBYTE height)
{
    if (height > 0)
    {
        this->height = height;

        this->flags |= GX_CURSOR_USE_CUSTOM_HEIGHT;
    }
    else
    {
        this->flags &= ~GX_CURSOR_USE_CUSTOM_HEIGHT;
    }
}

void GX_TEXT_INPUT_CURSOR::dirty_rectangle_get_(GX_RECTANGLE* dirty_rect)
{
    dirty_rect->left = (GX_VALUE)(pos.x - width / 2);
    dirty_rect->right = (GX_VALUE)(dirty_rect->left + width - 1);
    dirty_rect->top = (GX_VALUE)(pos.y - height / 2);
    dirty_rect->bottom = (GX_VALUE)(dirty_rect->top + height - 1);
}

void GX_TEXT_INPUT_CURSOR::draw_()
{
    if (!height)
    {
        return;
    }

    GX_VALUE y_start = (GX_VALUE)(pos.y - height / 2);
    GX_VALUE y_end = (GX_VALUE)(y_start + height - 1);

    _gx_context_brush_width_set(width);
    _gx_canvas_line_draw(pos.x, y_start, pos.x, y_end);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_input_cursor_height_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the blink interval value of the cursor.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    cursor_input                          Cursor control block          */
/*    blink_interval                        Value to be set               */
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
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_input_cursor_blink_interval_set(GX_TEXT_INPUT_CURSOR* cursor_input, GX_UBYTE blink_interval)
{
    cursor_input->blink_interval_set_(blink_interval);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_input_cursor_width_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the width value of the cursor                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    cursor_input                          Cursor control block          */
/*    width                                 Value to be set               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_input_cursor_width_set(GX_TEXT_INPUT_CURSOR* cursor_input, GX_UBYTE width)
{
    cursor_input->width_set_(width);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_input_cursor_height_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the height value of the cursor                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    cursor_input                          Cursor control block          */
/*    height                                Value to be set               */
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
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_input_cursor_height_set(GX_TEXT_INPUT_CURSOR* cursor_input, GX_UBYTE height)
{
    cursor_input->height_set_(height);

    return GX_SUCCESS;
}
