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
/**   System Management (Timer)                                           */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef GX_SYSTEM_TIMER_H
#define GX_SYSTEM_TIMER_H

#include "gx_global.h"

struct GX_WIDGET;
struct GX_EVENT;

UINT _gx_system_timer_start(GX_WIDGET* owner, UINT timer_id, UINT initial_ticks, UINT reschedule_ticks);
UINT _gx_system_timer_stop(GX_WIDGET* owner, UINT timer_id);

void _gx_system_timer_expiration(ULONG val);
void _gx_system_timer_update(ULONG ticks);
UINT _gx_system_timer_event_dispatch(GX_EVENT* event);

#endif
