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
/**   Tree View Management (Tree View)                                    */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_tree_view.h                                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX tree view component, including all data  */
/*    types and external references.  It is assumed that gx_api.h and     */
/*    gx_port.h have already been included.                               */
/*                                                                        */
/**************************************************************************/

#ifndef GX_TREE_VIEW_H
#define GX_TREE_VIEW_H

#include "gx_window.h"

struct GX_TREE_VIEW : public GX_WINDOW
{
    GX_RESOURCE_ID collapse_pixelmap_id;
    GX_RESOURCE_ID expand_pixelmap_id;
    GX_RESOURCE_ID root_line_color;
    GX_VALUE       indentation;
    GX_VALUE       x_shift;
    GX_VALUE       y_shift;
    GX_VALUE       tree_width;
    GX_VALUE       tree_height;
    GX_WIDGET*     selected;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 ULONG style, USHORT tree_menu_id, const GX_RECTANGLE* size);

    void selected_set_(GX_WIDGET* selected);

    void indentation_set_(GX_VALUE indentation);
    void position_();

    void scroll_info_get_(ULONG style, GX_SCROLL_INFO* return_scroll_info);

    void root_line_color_set_(GX_RESOURCE_ID color);
    void root_pixelmap_set_(GX_RESOURCE_ID expand_map_id, GX_RESOURCE_ID collapse_map_id);

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void selected_visible_();

    void scroll_(GX_VALUE x_scroll, GX_VALUE y_scroll);

    void root_draw_();
    void root_draw_helper_(GX_WIDGET* start, GX_WIDGET* owner);

    void pen_down_event_process_(GX_EVENT* event_ptr);

    GX_WIDGET* item_find_(GX_VALUE line_y) const;
    void root_icon_rect_get_(const GX_WIDGET* item, GX_RECTANGLE* rect) const;
    bool selected_find_(const GX_MENU* menu) const;
};

/* Define tree view management function prototypes.  */

UINT _gx_tree_view_create(GX_TREE_VIEW* tree, const GX_CHAR* name, GX_WIDGET* parent,
                          ULONG style, USHORT tree_menu_id, const GX_RECTANGLE* size);
void _gx_tree_view_draw(GX_TREE_VIEW* tree);
UINT _gx_tree_view_event_process(GX_TREE_VIEW* tree, GX_EVENT* event_ptr);
UINT _gx_tree_view_indentation_set(GX_TREE_VIEW* tree, GX_VALUE indentation);
UINT _gx_tree_view_position(GX_TREE_VIEW* tree);
UINT _gx_tree_view_root_line_color_set(GX_TREE_VIEW* tree, GX_RESOURCE_ID color);
UINT _gx_tree_view_root_pixelmap_set(GX_TREE_VIEW* tree, GX_RESOURCE_ID expand_map_id, GX_RESOURCE_ID collapse_map_id);
UINT _gx_tree_view_selected_get(GX_TREE_VIEW* tree, GX_WIDGET** selected);
UINT _gx_tree_view_selected_set(GX_TREE_VIEW* tree, GX_WIDGET* selected);

/* error checking versions of the tree view API functions */

UINT _gxe_tree_view_create(GX_TREE_VIEW* tree, const GX_CHAR* name, GX_WIDGET* parent,
                           ULONG style, USHORT tree_id, const GX_RECTANGLE* size,
                           UINT control_block_size);
UINT _gxe_tree_view_event_process(GX_TREE_VIEW* tree, GX_EVENT* event_ptr);
UINT _gxe_tree_view_indentation_set(GX_TREE_VIEW* tree, GX_VALUE indentation);
UINT _gxe_tree_view_position(GX_TREE_VIEW* tree);
UINT _gxe_tree_view_root_line_color_set(GX_TREE_VIEW* tree, GX_RESOURCE_ID color);
UINT _gxe_tree_view_root_pixelmap_set(GX_TREE_VIEW* tree, GX_RESOURCE_ID expand_map_id, GX_RESOURCE_ID collapse_map_id);
UINT _gxe_tree_view_selected_get(GX_TREE_VIEW* tree, GX_WIDGET** selected);
UINT _gxe_tree_view_selected_set(GX_TREE_VIEW* tree, GX_WIDGET* selected);

#endif

