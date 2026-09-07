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
/**   Button Management (Button)                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_button.h                                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX button management component,             */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_BUTTON_H
#define GX_BUTTON_H

#include "gx_widget.h"

/* Define Button type.  */
struct GX_BUTTON : public GX_WIDGET
{
    void (*select_handler)(GX_BUTTON* button);
    void (*deselect_handler)(GX_BUTTON* button, GX_BOOL generate_event);

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 ULONG style, USHORT button_id, const GX_RECTANGLE* size);

    void select_();
    void deselect_(GX_BOOL generate_event);

    void background_draw_();

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

protected:
    bool is_pushed_() const { return (style & GX_STYLE_BUTTON_PUSHED) != 0; }
    void set_pushed_() { style |= GX_STYLE_BUTTON_PUSHED; }
    void clear_pushed_() { style &= ~GX_STYLE_BUTTON_PUSHED; }

    void monochrome_driver_disabled_line_draw_();

private:
    void siblings_deselect_();

    void select_process_();
    void deselect_process_(GX_EVENT* event_ptr);
};


/* Define button management function prototypes.  */

void _gx_button_background_draw(GX_BUTTON* button);
UINT _gx_button_create(GX_BUTTON* button, const GX_CHAR* name, GX_WIDGET* parent,
                       ULONG style, USHORT button_id, const GX_RECTANGLE* size);
UINT _gx_button_deselect(GX_BUTTON* button, GX_BOOL generate_event);
void _gx_button_draw(GX_BUTTON* button);
UINT _gx_button_event_process(GX_BUTTON* button, GX_EVENT* event_ptr);
UINT _gx_button_select(GX_BUTTON* button);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_button_create(GX_BUTTON* button, const GX_CHAR* name, GX_WIDGET* parent,
                        ULONG style, USHORT button_id, const GX_RECTANGLE* size, UINT button_control_block_size);
UINT _gxe_button_deselect(GX_BUTTON* button, GX_BOOL gen_event);

UINT _gxe_button_event_process(GX_BUTTON* button, GX_EVENT* event_ptr);
UINT _gxe_button_select(GX_BUTTON* button);

#endif

