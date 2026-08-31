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
/**   Menu Management (Menu)                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_menu.h                                                           */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX menu component, including all data types */
/*    and external references.  It is assumed that gx_api.h and gx_port.h */
/*    have already been included.                                         */
/*                                                                        */
/**************************************************************************/

#ifndef GX_MENU_H
#define GX_MENU_H

#include "gx_pixelmap_prompt.h"

struct GX_MENU_LIST : public GX_WIDGET
{
    GX_WIDGET*   owner;
};

struct GX_MENU : public GX_PIXELMAP_PROMPT
{
    GX_MENU_LIST list;
    GX_VALUE     list_total_count;
    GX_VALUE     text_x_offset;
    GX_VALUE     text_y_offset;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                 ULONG style, USHORT menu_id, const GX_RECTANGLE* size);

    void insert_(GX_WIDGET* widget);
    void remove_(GX_WIDGET* widget);

    void text_offset_set_(GX_VALUE x_offset, GX_VALUE y_offset);

    void text_draw_();

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void position_(GX_VALUE indentation);
    void one_level_position_(GX_VALUE indentation);

    friend struct GX_TREE_VIEW;
    friend struct GX_ACCORDION_MENU;
};


/* Define menu management function prototypes.  */

UINT _gx_menu_create(GX_MENU* menu, const GX_CHAR* name, GX_WIDGET* parent,
                     GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                     ULONG style, USHORT menu_id, const GX_RECTANGLE* size);
void _gx_menu_draw(GX_MENU* menu);
UINT _gx_menu_event_process(GX_MENU* menu, GX_EVENT* event_ptr);
UINT _gx_menu_insert(GX_MENU* menu, GX_WIDGET* widget);
UINT _gx_menu_remove(GX_MENU* menu, GX_WIDGET* widget);
void _gx_menu_text_draw(GX_MENU* menu);
UINT _gx_menu_text_offset_set(GX_MENU* menu, GX_VALUE x_offset, GX_VALUE y_offset);


/* error checking versions of menu API functions.  */

UINT _gxe_menu_create(GX_MENU* menu, const GX_CHAR* name, GX_WIDGET* parent,
                      GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                      ULONG style, USHORT menu_id, const GX_RECTANGLE* size, UINT control_block_size);
UINT _gxe_menu_event_process(GX_MENU* menu, GX_EVENT* event_ptr);
UINT _gxe_menu_insert(GX_MENU* menu, GX_WIDGET* widget);
UINT _gxe_menu_remove(GX_MENU* menu, GX_WIDGET* widget);
UINT _gxe_menu_text_offset_set(GX_MENU* menu, GX_VALUE x_offset, GX_VALUE y_offset);

#endif

