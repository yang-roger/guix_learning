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

#include "gx_system_timer.h"

#include "gx_system.h"
#include "gx_widget.h"

/**************************************************************************/

static GX_TIMER* _gx_system_running_timer_find(const GX_WIDGET* owner, UINT timer_id)
{
    GX_TIMER* timer = _gx_system_active_timer_list;
    for (; timer; timer = timer->next)
    {
        if (timer->id == timer_id && timer->owner == owner)
        {
            return timer;
        }
    }

    return GX_NULL;
}

static bool _gx_system_timer_is_running(const GX_WIDGET* owner, UINT timer_id)
{
    GX_ENTER_CRITICAL

    GX_TIMER* timer = _gx_system_running_timer_find(owner, timer_id);

    GX_EXIT_CRITICAL

    return timer != GX_NULL;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_timer_start                                              */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service starts a timer for the specified widget.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    owner                                 Pointer to widget control     */
/*                                            block                       */
/*    timer_id                              ID of timer                   */
/*    initial ticks                         Initial expiration ticks      */
/*    reschedule_ticks                      Periodic expiration ticks     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   tx_timer_activate                      start the ThreadX timer       */
/*   GX_ENTER_CRITICAL                      lock system mutex             */
/*   GX_EXIT_CRITICAL                       unlock system mutex           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_timer_start(GX_WIDGET* owner, UINT timer_id, UINT initial_ticks, UINT reschedule_ticks)
{
    if (!owner)
    {
        return GX_PTR_ERROR;
    }

    GX_ENTER_CRITICAL

    GX_TIMER* timer = _gx_system_running_timer_find(owner, timer_id);
    if (timer)
    {
        timer->initial_ticks = initial_ticks;
        timer->reschedule_ticks = reschedule_ticks;

        GX_EXIT_CRITICAL

        return GX_SUCCESS;
    }

    if (!_gx_system_free_timer_list)
    {
        GX_EXIT_CRITICAL

        return GX_OUT_OF_TIMERS;
    }

    // get a new timer from the free timer list
    timer = _gx_system_free_timer_list;
    _gx_system_free_timer_list = timer->next;

    timer->initial_ticks = initial_ticks;
    timer->reschedule_ticks = reschedule_ticks;
    timer->id = timer_id;
    timer->owner = owner;

    // link new timer to the active timer list
    timer->next = _gx_system_active_timer_list;
    _gx_system_active_timer_list = timer;

    _gx_system_os_timer_start();

    GX_EXIT_CRITICAL

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_timer_stop                                               */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service stops the timer for the specified widget.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    owner                                 Pointer to widget control     */
/*                                            block                       */
/*    timer_id                              ID of timer                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    GX_ENTER_CRITICAL                     lock critical section         */
/*    GX_EXIT_CRITICAL                      unlock critical section       */
/*    tx_timer_deactivate                   stop the ThreadX timer        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_timer_stop(GX_WIDGET* owner, UINT timer_id)
{
    UINT status = GX_FAILURE;

    GX_TIMER* previous_timer = GX_NULL;
    GX_TIMER* next_timer;

    GX_ENTER_CRITICAL

    GX_TIMER* timer = _gx_system_active_timer_list;
    while (timer)
    {
        next_timer = timer->next;

        if (timer->owner == owner)
        {
            if (timer_id == 0 || timer->id == timer_id)
            {
                status = GX_SUCCESS;

                // unlink this timer from the active timer list
                if (previous_timer == GX_NULL)
                {
                    _gx_system_active_timer_list = next_timer;
                }
                else
                {
                    previous_timer->next = next_timer;
                }

                // add this timer to head of free list
                timer->next = _gx_system_free_timer_list;
                _gx_system_free_timer_list = timer;

                // if just looking for one timer, we are done
                if (timer_id != 0)
                {
                    break;
                }
                else
                {
                    timer = next_timer;
                    continue;
                }
            }
        }

        previous_timer = timer;
        timer = next_timer;
    }

    GX_EXIT_CRITICAL

    if (!_gx_system_active_timer_list &&
        !_gx_system_animation_active_list)
    {
        _gx_system_os_timer_stop();
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_timer_expiration                                         */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This is the callback function for the gx_system_timer. This         */
/*    function updates all of the GUIX application timers, and sends      */
/*    events as needed                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_system_event_send                   Send GUI event               */
/*   tx_timer_deactivate                     Deactivate a timer           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    tx_timer                                                            */
/*                                                                        */
/**************************************************************************/
void _gx_system_timer_expiration(ULONG val)
{
    GX_PARAMETER_NOT_USED(val);

    if (!_gx_system_active_timer_list &&
        !_gx_system_animation_active_list)
    {
        _gx_system_os_timer_stop();
        return;
    }

    GX_EVENT timer_event;
    timer_event.type = GX_EVENT_TIMER;
    timer_event.sender = 0;
    timer_event.target = GX_NULL;
    timer_event.payload.ulongdata = 1;
    _gx_system_event_fold(&timer_event);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_timer_update                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is called when the GUIX timers need to be updated.    */
/*    It updates all of the GUIX application timers, and sends events     */
/*    as needed.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   GX_ENTER_CRITICAL                      Lock system mutex             */
/*   GX_EXIT_CRITICAL                       Unlock system mutex           */
/*   [gx_widget_event_process_function]     Event handler of timer owner  */
/*   _gx_system_timer_stop                  Stop the system timer         */
/*   _gx_animation_update                   Update the animation sequence */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    tx_timer                                                            */
/*                                                                        */
/**************************************************************************/
void _gx_system_timer_update(ULONG ticks)
{
    GX_TIMER* next_timer;
    GX_WIDGET* timer_owner;
    UINT timer_id;
    GX_EVENT timer_event;

    GX_ENTER_CRITICAL

    timer_event.type = GX_EVENT_TIMER;

    GX_TIMER* current_timer = _gx_system_active_timer_list;
    while (current_timer)
    {
        next_timer = current_timer->next;

        if (current_timer->initial_ticks > ticks)
        {
            current_timer->initial_ticks -= ticks;
        }
        else
        {
            // timer has expired

            timer_owner = current_timer->owner;
            timer_id = current_timer->id;

            if (current_timer->reschedule_ticks > 0)
            {
                current_timer->initial_ticks = current_timer->reschedule_ticks;
                timer_event.sender = 1; // repeated timer
            }
            else
            {
                _gx_system_timer_stop(current_timer->owner, current_timer->id);
                timer_event.sender = 0; // single-shot timer
            }

            if (timer_owner && timer_owner->event_process_function)
            {
                timer_event.payload.timer_id = timer_id;
                timer_event.target = timer_owner;
                _gx_system_event_send(&timer_event);
            }
        }

        current_timer = next_timer;
    }

#if (GX_ANIMATION_POOL_SIZE > 0)
    if (_gx_system_animation_active_list)
    {
        _gx_animation_update();
    }
#endif

    GX_EXIT_CRITICAL
}

/**************************************************************************/

UINT _gx_system_timer_event_dispatch(GX_EVENT* event_ptr)
{
    if (event_ptr->sender) // repeated timer
    {
        if (!_gx_system_timer_is_running(event_ptr->target, event_ptr->payload.timer_id))
        {
            return GX_NOT_FOUND;
        }
    }

    return _gx_system_event_dispatch(event_ptr);
}
