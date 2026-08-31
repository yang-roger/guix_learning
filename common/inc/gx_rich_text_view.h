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
/**   Rich Text View Management (Rich Text View)                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_rich_text_view.h                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX text view management component,          */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/
#ifndef GX_RICH_TEXT_VIEW_H
#define GX_RICH_TEXT_VIEW_H

#include "gx_multi_line_text_view.h"
#include "gx_rich_text_info.h"

struct GX_RICH_TEXT_LINE_INFO
{
    GX_STRING           text;
    GX_RICH_TEXT_FORMAT start_format;
    GX_RICH_TEXT_FORMAT end_format;
    UINT                text_width;
    GX_UBYTE            line_height;
    GX_UBYTE            baseline;
};

/* Define Rich Text View type.  */
struct GX_RICH_TEXT_VIEW : public GX_MULTI_LINE_TEXT_VIEW
{
    GX_RICH_TEXT_FONTS fonts;
    ULONG              text_total_height;

    UINT create_(const GX_CHAR* name,
                 GX_WIDGET* parent,
                 GX_RESOURCE_ID text_id,
                 GX_RICH_TEXT_FONTS* fonts,
                 ULONG style,
                 USHORT id,
                 const GX_RECTANGLE* size);

    UINT fonts_set_(GX_RICH_TEXT_FONTS* fonts);

    void text_draw_();
    void draw_();

    UINT scroll_info_get_(ULONG style, GX_SCROLL_INFO* info);

private:
    UINT text_total_height_calculate_();
    UINT line_info_get_(GX_STRING text, GX_RICH_TEXT_LINE_INFO* line_info, GX_VALUE availlable_width);
    UINT tag_enter_(const GX_STRING* text, GX_RICH_TEXT_FORMAT* format, GX_UBYTE* handled_bytes);

    void single_line_draw_(GX_VALUE ypos, GX_RICH_TEXT_LINE_INFO* line_info);
};


/* Define rich text view management function prototypes. */

UINT _gx_rich_text_view_create(GX_RICH_TEXT_VIEW* view,
                               const GX_CHAR* name,
                               GX_WIDGET* parent,
                               GX_RESOURCE_ID text_id,
                               GX_RICH_TEXT_FONTS* fonts,
                               ULONG style,
                               USHORT id,
                               const GX_RECTANGLE* size);
void _gx_rich_text_view_draw(GX_RICH_TEXT_VIEW* view);
UINT _gx_rich_text_view_fonts_set(GX_RICH_TEXT_VIEW* view, GX_RICH_TEXT_FONTS* fonts);
void _gx_rich_text_view_text_draw(GX_RICH_TEXT_VIEW* view);


/* Define error checking shells for API services. */
UINT _gxe_rich_text_view_create(GX_RICH_TEXT_VIEW* view,
                                const GX_CHAR* name,
                                GX_WIDGET* parent,
                                GX_RESOURCE_ID text_id,
                                GX_RICH_TEXT_FONTS* fonts,
                                ULONG style,
                                USHORT id,
                                const GX_RECTANGLE* size, UINT control_block_size);
UINT _gxe_rich_text_view_fonts_set(GX_RICH_TEXT_VIEW* view, GX_RICH_TEXT_FONTS* fonts);

#endif
