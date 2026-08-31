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
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_text_input_cursor.h                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX cursor management component,             */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_CURSOR_H
#define GX_CURSOR_H

#include "gx_point.h"
#include "gx_rectangle.h"

/* Define text input cursor control block */
struct GX_TEXT_INPUT_CURSOR
{
    GX_VALUE blink_interval;

    /* User defined cursor width and height */
    GX_VALUE width;
    GX_VALUE height;
    GX_UBYTE flags;

    /* Cursor position information, managed by the cursor control code.*/
    GX_POINT pos;

    void width_set_(GX_UBYTE width);
    void height_set_(GX_UBYTE height);

    void blink_interval_set_(GX_UBYTE blink_interval);

    void dirty_rectangle_get_(GX_RECTANGLE* dirty_rect);

    void draw_();
};

inline void GX_TEXT_INPUT_CURSOR::width_set_(GX_UBYTE width)
{
    this->width = width;
}

inline void GX_TEXT_INPUT_CURSOR::blink_interval_set_(GX_UBYTE blink_interval)
{
    this->blink_interval = blink_interval;
}

/* Define cursor management function prototypes.  */

UINT _gx_text_input_cursor_width_set(GX_TEXT_INPUT_CURSOR* cursor_input, GX_UBYTE width);
UINT _gx_text_input_cursor_height_set(GX_TEXT_INPUT_CURSOR* cursor_input, GX_UBYTE height);
UINT _gx_text_input_cursor_blink_interval_set(GX_TEXT_INPUT_CURSOR* cursor_input, GX_UBYTE blink_interval);

/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_text_input_cursor_width_set(GX_TEXT_INPUT_CURSOR* cursor_input, GX_UBYTE width);
UINT _gxe_text_input_cursor_height_set(GX_TEXT_INPUT_CURSOR* cursor_input, GX_UBYTE height);
UINT _gxe_text_input_cursor_blink_interval_set(GX_TEXT_INPUT_CURSOR* cursor_input, GX_UBYTE blink_interval);

#endif

