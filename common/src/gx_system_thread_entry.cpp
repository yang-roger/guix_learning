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
/**   System Management (System)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_system.h"

#if !defined(GX_THREAD_SLEEPING)
#define GX_THREAD_SLEEPING
#endif

#if !defined(GX_THREAD_AWAKE)
#define GX_THREAD_AWAKE
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_thread_entry                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This is the main processing thread for GUIX. All events and drawing */
/*    are done from the context of this thread.                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    id                                    Thread ID                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    tx_mutex_get                          Get protection mutex          */
/*    tx_mutex_put                          Release protection mutex      */
/*    tx_queue_receive                      Receive GUIX events from queue*/
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*    _gx_system_error_process              Process system errors         */
/*    _gx_system_event_dispatch             Dispatch GUIX events          */
/*    _gx_system_canvas_refresh             Refresh the canvas            */
/*    _gx_system_timer_update               Update active system timers   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX                                                             */
/*                                                                        */
/**************************************************************************/
void _gx_system_thread_entry(ULONG id)
{
    UINT status = GX_FAILURE;

#ifdef GX_THREADX_BINDING
    ULONG event_memory[GX_EVENT_ULONGS];
#else
    GX_EVENT event_memory;
#endif

    GX_EVENT* event_ptr;

    GX_PARAMETER_NOT_USED(id);

    for (;;)
    {
#ifdef GX_THREADX_BINDING
        status = tx_queue_receive(&_gx_system_event_queue, &event_memory[0], TX_NO_WAIT);
        if (status == TX_QUEUE_EMPTY)
        {
            _gx_system_canvas_refresh();

            // Now block this thread until an event is received.
            GX_THREAD_SLEEPING
            status = tx_queue_receive(&_gx_system_event_queue, &event_memory[0], TX_WAIT_FOREVER);
        }

        if (status == TX_SUCCESS)
        {
            status = GX_SUCCESS;
        }
#else
        status = GX_EVENT_POP(&event_memory, GX_FALSE);
        if (status == GX_FAILURE)
        {
            _gx_system_canvas_refresh();

            GX_THREAD_SLEEPING
            status = GX_EVENT_POP(&event_memory, GX_TRUE);
        }
#endif

        if (status != GX_SUCCESS)
        {
            _gx_system_error_process(GX_SYSTEM_EVENT_RECEIVE_ERROR);

            return;
        }

        GX_THREAD_AWAKE

        event_ptr = (GX_EVENT*)(&event_memory);

        switch (event_ptr->type)
        {
        case GX_EVENT_REDRAW:
            _gx_system_all_canvas_dirty();
            break;

        case GX_EVENT_TERMINATE:
            return;

        case GX_EVENT_TIMER:
            if (event_ptr->target == GX_NULL)
            {
                // the event is from gx_system_timer_expiration
                _gx_system_timer_update(event_ptr->payload.ulongdata);
            }
            else
            {
                _gx_system_timer_event_dispatch(event_ptr);
            }
            break;

        case 0: // the event has been purged
            break;

        default:
            _gx_system_event_dispatch(event_ptr);
            break;
        }
    }
}

