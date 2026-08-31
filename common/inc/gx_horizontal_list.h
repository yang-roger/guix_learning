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
/**   Horizontal List (List)                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef GX_HORIZONTAL_LIST_H
#define GX_HORIZONTAL_LIST_H

#include "gx_widget_list.h"

struct GX_HORIZONTAL_LIST : public GX_WIDGET_LIST
{
    UINT create_(const GX_CHAR* name,
                 GX_WIDGET* parent, INT total_columns,
                 void (*callback)(GX_HORIZONTAL_LIST*, GX_WIDGET*, INT),
                 ULONG style, USHORT horizontal_list_id, const GX_RECTANGLE* size);
};


UINT _gx_horizontal_list_children_position(GX_HORIZONTAL_LIST* list);
UINT _gx_horizontal_list_create(GX_HORIZONTAL_LIST* list, const GX_CHAR* name,
                                GX_WIDGET* parent, INT total_columns,
                                void (*callback)(GX_HORIZONTAL_LIST*, GX_WIDGET*, INT),
                                ULONG style, USHORT horizontal_list_id, const GX_RECTANGLE* size);

UINT _gx_horizontal_list_event_process(GX_HORIZONTAL_LIST* list, GX_EVENT* event_ptr);
UINT _gx_horizontal_list_page_index_set(GX_HORIZONTAL_LIST* list, INT index);
UINT _gx_horizontal_list_selected_index_get(GX_HORIZONTAL_LIST* list, INT* return_index);
UINT _gx_horizontal_list_selected_widget_get(GX_HORIZONTAL_LIST* list, GX_WIDGET** return_list);
UINT _gx_horizontal_list_selected_set(GX_HORIZONTAL_LIST* list, INT index);
UINT _gx_horizontal_list_total_columns_set(GX_HORIZONTAL_LIST* list, INT count);


UINT _gxe_horizontal_list_children_position(GX_HORIZONTAL_LIST* list);
UINT _gxe_horizontal_list_create(GX_HORIZONTAL_LIST* list, const GX_CHAR* name,
                                 GX_WIDGET* parent, INT total_rows,
                                 void (*callback)(GX_HORIZONTAL_LIST*, GX_WIDGET*, INT),
                                 ULONG style, USHORT horizontal_list_id, const GX_RECTANGLE* size,
                                 UINT horizontal_list_control_block_size);
UINT _gxe_horizontal_list_event_process(GX_HORIZONTAL_LIST* list, GX_EVENT* event_ptr);
UINT _gxe_horizontal_list_page_index_set(GX_HORIZONTAL_LIST* list, INT index);
UINT _gxe_horizontal_list_selected_index_get(GX_HORIZONTAL_LIST* list, INT* return_index);
UINT _gxe_horizontal_list_selected_widget_get(GX_HORIZONTAL_LIST* list, GX_WIDGET** return_list_entry);
UINT _gxe_horizontal_list_selected_set(GX_HORIZONTAL_LIST* list, INT index);
UINT _gxe_horizontal_list_total_columns_set(GX_HORIZONTAL_LIST* list, INT count);

#endif
