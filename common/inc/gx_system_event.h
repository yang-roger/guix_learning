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
/**   System Management (Event)                                           */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef GX_SYSTEM_EVENT_H
#define GX_SYSTEM_EVENT_H

#include "gx_global.h"

struct GX_EVENT;
struct GX_WIDGET;

UINT _gx_system_event_dispatch(GX_EVENT* new_event);

UINT _gx_system_event_fold(GX_EVENT* in_event);
UINT _gx_system_event_send(GX_EVENT* in_event);
void _gx_system_event_remove(const GX_WIDGET* widget);

#endif
