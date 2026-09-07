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
/**   Window Management (Window)                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_drop_list.h                                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX window management component,             */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_DROP_LIST_H
#define GX_DROP_LIST_H

#include "gx_popup_list.h"

struct GX_DROP_LIST : public GX_WIDGET
{
    GX_POPUP_LIST  popup;
    INT            open_height;
    GX_RESOURCE_ID pixelmap;
    GX_BOOL        popup_open;

    UINT create_(const GX_CHAR* name,
                 GX_WIDGET* parent, INT total_rows, INT open_height,
                 void (*callback)(GX_VERTICAL_LIST*, GX_WIDGET*, INT),
                 ULONG style, USHORT drop_list_id, const GX_RECTANGLE* size);

    UINT open_();
    UINT close_();

    UINT pixelmap_set_(GX_RESOURCE_ID pixelmap_id);

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void background_draw_();
    void selected_children_draw_(GX_WIDGET* widget, GX_VALUE x_shift, GX_VALUE y_shift);
};


UINT _gx_drop_list_close(GX_DROP_LIST* drop_list);
UINT _gx_drop_list_create(GX_DROP_LIST* drop_list, const GX_CHAR* name,
                          GX_WIDGET* parent, INT total_rows, INT open_height,
                          void (*callback)(GX_VERTICAL_LIST*, GX_WIDGET*, INT),
                          ULONG style, USHORT drop_list_id, const GX_RECTANGLE* size);
UINT _gx_drop_list_event_process(GX_DROP_LIST* drop_list, GX_EVENT* event_ptr);
UINT _gx_drop_list_open(GX_DROP_LIST* drop_list);
UINT _gx_drop_list_pixelmap_set(GX_DROP_LIST* drop_list, GX_RESOURCE_ID pixelmap_id);
UINT _gx_drop_list_popup_get(GX_DROP_LIST* drop_list, GX_VERTICAL_LIST** return_list);


UINT _gxe_drop_list_close(GX_DROP_LIST* drop_list);
UINT _gxe_drop_list_create(GX_DROP_LIST* drop_list, const GX_CHAR* name,
                           GX_WIDGET* parent, INT total_rows, INT open_height,
                           void (*callback)(GX_VERTICAL_LIST*, GX_WIDGET*, INT),
                           ULONG style, USHORT drop_list_id, const GX_RECTANGLE* size, UINT drop_list_control_block_size);
UINT _gxe_drop_list_event_process(GX_DROP_LIST* drop_list, GX_EVENT* event_ptr);
UINT _gxe_drop_list_open(GX_DROP_LIST* drop_list);
UINT _gxe_drop_list_pixelmap_set(GX_DROP_LIST* drop_list, GX_RESOURCE_ID pixelmap_id);
UINT _gxe_drop_list_popup_get(GX_DROP_LIST* drop_list, GX_VERTICAL_LIST** return_list);

#endif

