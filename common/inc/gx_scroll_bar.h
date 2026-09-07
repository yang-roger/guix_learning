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
/**   Scroll Management (Scrollbar)                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_scroll_bar.h                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX icon management component,               */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_SCROLLBAR_H
#define GX_SCROLLBAR_H

#include "gx_widget.h"
#include "gx_scroll_bar_appearance.h"
#include "gx_scroll_info.h"
#include "gx_button.h"
#include "gx_scroll_thumb.h"

struct GX_WINDOW;

/* Define Scrollbar type.  */
struct GX_SCROLLBAR : public GX_WIDGET
{
    GX_SCROLLBAR_APPEARANCE appearance;
    GX_SCROLL_INFO          info;
    GX_BUTTON               upleft;
    GX_BUTTON               downright;
    GX_SCROLL_THUMB         thumb;

    UINT create_vertical_(const GX_CHAR* name, GX_WINDOW* parent, const GX_SCROLLBAR_APPEARANCE* appearance, ULONG style)
    {
        style &= ~GX_SCROLLBAR_HORIZONTAL;
        style |= GX_SCROLLBAR_VERTICAL;
        return create_(name, parent, appearance, style);
    }

    UINT create_horizontal_(const GX_CHAR* name, GX_WINDOW* parent, const GX_SCROLLBAR_APPEARANCE* appearance, ULONG style)
    {
        style &= ~GX_SCROLLBAR_VERTICAL;
        style |= GX_SCROLLBAR_HORIZONTAL;
        return create_(name, parent, appearance, style);
    }

    UINT create_(const GX_CHAR* name, GX_WINDOW* parent, const GX_SCROLLBAR_APPEARANCE* appearance, ULONG style);

    bool is_vertical_()   const { return (style & GX_SCROLLBAR_VERTICAL) != 0; }
    bool is_horizontal_() const { return (style & GX_SCROLLBAR_HORIZONTAL) != 0; }

    void reset_(const GX_SCROLL_INFO* info = GX_NULL);
    void value_set_(INT value);
    void value_clamp_();

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void size_update_();

    void value_to_thumb_();
    void thumb_to_value_(INT thumb_pos, INT thumb_length);

    INT value_clamp_(INT value) const;
    void value_changed_(INT old_value) const;

    void parent_window_client_updated_() const;
};


/* Define scrollbar management function prototypes.  */

UINT _gx_horizontal_scrollbar_create(GX_SCROLLBAR* scrollbar, const GX_CHAR* name, GX_WINDOW* parent,
                                     GX_SCROLLBAR_APPEARANCE* appearance, ULONG style);
UINT _gx_vertical_scrollbar_create(GX_SCROLLBAR* scrollbar, const GX_CHAR* name, GX_WINDOW* parent,
                                   GX_SCROLLBAR_APPEARANCE* appearance, ULONG style);
void _gx_scrollbar_draw(GX_SCROLLBAR* scrollbar);
UINT _gx_scrollbar_event_process(GX_SCROLLBAR* scrollbar, GX_EVENT* event_ptr);
UINT _gx_scrollbar_limit_check(GX_SCROLLBAR* scrollbar);
UINT _gx_scrollbar_reset(GX_SCROLLBAR* scrollbar, GX_SCROLL_INFO* info);
UINT _gx_scrollbar_value_set(GX_SCROLLBAR* scrollbar, INT value);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_horizontal_scrollbar_create(GX_SCROLLBAR* scrollbar, const GX_CHAR* name, GX_WINDOW* parent,
                                      GX_SCROLLBAR_APPEARANCE* appearance, ULONG style, UINT scrollbar_control_block_size);
UINT _gxe_vertical_scrollbar_create(GX_SCROLLBAR* scrollbar, const GX_CHAR* name, GX_WINDOW* parent,
                                    GX_SCROLLBAR_APPEARANCE* appearance, ULONG style, UINT scrollbar_control_block_size);

UINT _gxe_scrollbar_event_process(GX_SCROLLBAR* scrollbar, GX_EVENT* event_ptr);
UINT _gxe_scrollbar_limit_check(GX_SCROLLBAR* scrollbar);
UINT _gxe_scrollbar_reset(GX_SCROLLBAR* scrollbar, GX_SCROLL_INFO* info);
UINT _gxe_scrollbar_value_set(GX_SCROLLBAR* scrollbar, INT value);

#endif
