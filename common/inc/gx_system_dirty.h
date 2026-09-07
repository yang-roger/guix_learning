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
/**   System Management (Dirty Areas)                                     */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef GX_SYSTEM_DIRTY_H
#define GX_SYSTEM_DIRTY_H

#include "gx_global.h"

struct GX_WIDGET;
struct GX_RECTANGLE;

UINT _gx_system_dirty_mark(GX_WIDGET* widget);
UINT _gx_system_dirty_partial_add(GX_WIDGET* widget, const GX_RECTANGLE* dirty_area);
void _gx_system_dirty_entry_shift(GX_WIDGET* widget, INT xShift, INT yShift);
void _gx_system_dirty_list_remove(GX_WIDGET* removed);

void _gx_system_all_canvas_dirty(void);

UINT _gx_system_canvas_refresh(void);

#endif
