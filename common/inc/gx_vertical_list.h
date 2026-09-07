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
/**   Vertical List (List)                                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef GX_VERTICAL_LIST_H
#define GX_VERTICAL_LIST_H

#include "gx_widget_list.h"

struct GX_VERTICAL_LIST : public GX_WIDGET_LIST
{
    UINT create_(const GX_CHAR* name, GX_WIDGET* parent, INT total_rows,
                 void (*callback)(GX_VERTICAL_LIST*, GX_WIDGET*, INT),
                 ULONG style, USHORT vertical_list_id, const GX_RECTANGLE* size);
};


UINT _gx_vertical_list_children_position(GX_VERTICAL_LIST* list);
UINT _gx_vertical_list_create(GX_VERTICAL_LIST* list, const GX_CHAR* name,
                              GX_WIDGET* parent, INT total_rows,
                              void (*callback)(GX_VERTICAL_LIST*, GX_WIDGET*, INT),
                              ULONG style, USHORT vertical_list_id, const GX_RECTANGLE* size);

UINT _gx_vertical_list_event_process(GX_VERTICAL_LIST* list, GX_EVENT* event_ptr);
UINT _gx_vertical_list_page_index_set(GX_VERTICAL_LIST* list, INT index);
UINT _gx_vertical_list_selected_index_get(GX_VERTICAL_LIST* list, INT* return_index);
UINT _gx_vertical_list_selected_widget_get(GX_VERTICAL_LIST* list, GX_WIDGET** return_list_entry);
UINT _gx_vertical_list_selected_set(GX_VERTICAL_LIST* list, INT index);
UINT _gx_vertical_list_total_rows_set(GX_VERTICAL_LIST* list, INT count);


UINT _gxe_vertical_list_children_position(GX_VERTICAL_LIST* list);
UINT _gxe_vertical_list_create(GX_VERTICAL_LIST* list, const GX_CHAR* name,
                               GX_WIDGET* parent, INT total_rows,
                               void (*callback)(GX_VERTICAL_LIST*, GX_WIDGET*, INT),
                               ULONG style, USHORT vertical_list_id, const GX_RECTANGLE* size,
                               UINT vertical_list_control_block_size);
UINT _gxe_vertical_list_event_process(GX_VERTICAL_LIST* list, GX_EVENT* event_ptr);
UINT _gxe_vertical_list_page_index_set(GX_VERTICAL_LIST* list, INT list_entry);
UINT _gxe_vertical_list_selected_index_get(GX_VERTICAL_LIST* list, INT* return_index);
UINT _gxe_vertical_list_selected_widget_get(GX_VERTICAL_LIST* list, GX_WIDGET** return_list_entry);
UINT _gxe_vertical_list_selected_set(GX_VERTICAL_LIST* list, INT index);
UINT _gxe_vertical_list_total_rows_set(GX_VERTICAL_LIST* list, INT count);

#endif
