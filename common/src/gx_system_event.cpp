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

#include "gx_system_event.h"

#include "gx_system.h"
#include "gx_display.h"
#include "gx_canvas.h"
#include "gx_window_root.h"

static void pen_position_map_to_root_window_(GX_POINT& pen_pos, const GX_WINDOW_ROOT* root)
{
    pen_pos.x = (GX_VALUE)(pen_pos.x - root->canvas->display_offset_x);
    pen_pos.y = (GX_VALUE)(pen_pos.y - root->canvas->display_offset_y);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_top_root_find                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function finds the top root window for a given incoming event. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    in_event                              Pointer to event              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    GX_WINDOW_ROOT                        Pointer to found root window  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_shift           Shift a rectangle             */
/*    _gx_utility_rectangle_point_detect    Detect point in rectangle     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_event_dispatch             Dispatch GUIX event           */
/*                                                                        */
/**************************************************************************/
static GX_WINDOW_ROOT* _gx_system_top_root_find(const GX_EVENT* in_event)
{
    GX_POINT pen_pos;

    GX_WINDOW_ROOT* root = _gx_system_root_window_created_list;
    while (root)
    {
        if (root->is_visible_())
        {
            if (in_event->display_handle == (ULONG)(root->canvas->display->driver_data))
            {
                // get absolute click position
                pen_pos = in_event->payload.pointdata;

                pen_position_map_to_root_window_(pen_pos, root);

                if (root->size.contain_point_(pen_pos))
                {
                    return root;
                }
            }
        }

        root = (GX_WINDOW_ROOT*)(root->next);
    }

    return GX_NULL;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_event_dispatch                                           */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function dispatches new system events to the appropriate       */
/*    widget(s).                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    in_event                              New event                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_top_root_find              Find the root widget          */
/*    _gx_system_top_widget_find            Find top widget               */
/*    _gx_system_focus_claim                Mark the widget to receive    */
/*                                            GUIX input focus            */
/*    [gx_widget_event_process_function]    Widget's event processing     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_event_dispatch(GX_EVENT* in_event)
{
    if (in_event->type == 0) // event has been purged
    {
        return 0;
    }

    UINT return_code = 0;

    GX_WIDGET* target = GX_NULL;
    GX_WINDOW_ROOT* root_window = GX_NULL;
    GX_POINT pen_pos;

    GX_EVENT out_event = *in_event;

    if (out_event.target)
    {
        target = out_event.target;
        return_code = target->event_process_function_run_(&out_event);
    }
    else
    {
        switch (out_event.type)
        {
#if defined(GX_MOUSE_SUPPORT)
        case GX_EVENT_PEN_MOVE:
            // get absolute click position
            pen_pos = out_event.payload.pointdata;

            root_window = _gx_system_top_root_find(&out_event);
            if (root_window)
            {
                root_window->canvas->display->mouse_position_set_run_(&pen_pos);
            }
            break;
#endif // GX_MOUSE_SUPPORT

        case GX_EVENT_PEN_DOWN:
        case GX_EVENT_PEN_UP:
        case GX_EVENT_PEN_DRAG:
            // get absolute click position
            pen_pos = out_event.payload.pointdata;

            if (_gx_system_input_capture_count > 0)
            {
                target = *_gx_system_input_capture_stack;
                if (target)
                {
                    root_window = target->root_window_();
                }
            }
            else
            {
                root_window = _gx_system_top_root_find(&out_event);
            }

            if (root_window)
            {
#if defined(GX_MOUSE_SUPPORT)
                // mouse cursor coordinates are display relative, not canvas relative,
                // so set cursor position before canvas offset is applied
                root_window->canvas->display->mouse_position_set_run_(&pen_pos);
#endif

                pen_position_map_to_root_window_(pen_pos, root_window);

                if (!target)
                {
                    target = root_window->top_child_in_point_(pen_pos, GX_STATUS_SELECTABLE);
                }
            }

            if (target)
            {
                out_event.payload.pointdata = pen_pos;
                out_event.target = target;

                if (out_event.type == GX_EVENT_PEN_DOWN)
                {
                    _gx_system_focus_claim(target);
                }

                return_code = target->event_process_function_run_(&out_event);
            }
            break;

        default:
            if (_gx_system_focus_owner)
            {
                return_code = _gx_system_focus_owner->event_process_function_run_(&out_event);
            }
            break;
        }
    }

    return return_code;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_event_send                                               */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sends the specified event for processing.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    event                                 Pointer to event              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    GX_ABS                                Compute the absolute value    */
/*    tx_queue_send                         Send message through ThreadX  */
/*                                            queue                       */
/*    _gx_system_pen_speed_init             Initialize pen speed          */
/*    _gx_system_pen_speed_update           Update pen speed              */
/*    _gx_system_pen_flick_test             Check for pen flick event     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_event_send(GX_EVENT* in_event)
{
    UINT       status = GX_SYSTEM_ERROR;
    bool       check_send_flick = false;
    GX_WIDGET* target = GX_NULL;

    switch (in_event->type)
    {
    case GX_EVENT_PEN_DOWN:
        _gx_system_pen_speed_init(&in_event->payload.pointdata);
        break;

    case GX_EVENT_PEN_DRAG:
        _gx_system_pen_speed_update(&in_event->payload.pointdata);
        break;

    case GX_EVENT_PEN_UP:
        check_send_flick = true;

        if (_gx_system_input_capture_count > 0)
        {
            target = *_gx_system_input_capture_stack;
        }
        break;
    }

#ifdef GX_THREADX_BINDING
    if (tx_queue_send(&_gx_system_event_queue, in_event, TX_NO_WAIT) == TX_SUCCESS)
    {
        status = GX_SUCCESS;
    }
#else
    status = GX_EVENT_PUSH(in_event);
#endif

    if (check_send_flick)
    {
        _gx_system_pen_flick_test(target);
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_event_fold                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service folds GUIX events.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    event                                 Pointer to event              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    TX_DISABLE                                                          */
/*    TX_RESTORE                                                          */
/*    _gx_system_pen_speed_update                                         */
/*    _gx_system_event_send                 Send GUIX events              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_event_fold(GX_EVENT* in_event)
{
#ifdef GX_THREADX_BINDING
    TX_INTERRUPT_SAVE_AREA

    GX_EVENT* pEvent;
    ULONG*    pSrc;
    TX_QUEUE* pQueue;

    TX_DISABLE

    pQueue = &_gx_system_event_queue;

    if (pQueue->tx_queue_enqueued)
    {
        pSrc =  pQueue->tx_queue_read;

        do
        {
            pEvent = (GX_EVENT*)pSrc;

            if (pEvent->type == in_event->type &&
                pEvent->target == in_event->target)
            {
                // we found a matching event, just update the existing event data
                // rather than posting a new event

                // for timer event, update tick count
                if (pEvent->type == GX_EVENT_TIMER)
                {
                    pEvent->payload.ulongdata++;
                }
                else
                {
                    // for all other event types, just copy payload
                    pEvent->payload.ulongdata = in_event->payload.ulongdata;
                }

                TX_RESTORE

                if (pEvent->type == GX_EVENT_PEN_DRAG)
                {
                    _gx_system_pen_speed_update(&pEvent->payload.pointdata);
                }

                return GX_SUCCESS;
            }

            pSrc += pQueue->tx_queue_message_size;

            if (pSrc >= pQueue->tx_queue_end)
            {
                pSrc = pQueue->tx_queue_start;
            }
        }
        while (pSrc != pQueue->tx_queue_write);
    }

    TX_RESTORE

    // we didn't find a matching event, so post a new event
    return _gx_system_event_send(in_event);

#else
    return (GX_EVENT_FOLD(in_event));
#endif
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_event_remove                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function removes events targeted to indicated widget           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    remove                                Widget to be removed          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_child_detect               Detect a child widget         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_widget_delete                     Delete a widget               */
/*                                                                        */
/**************************************************************************/
void _gx_system_event_remove(const GX_WIDGET* widget)
{
#ifdef GX_THREADX_BINDING

    TX_INTERRUPT_SAVE_AREA

    GX_EVENT* pEvent;
    ULONG*    pSrc;
    TX_QUEUE* pQueue;

    TX_DISABLE

    pQueue = &_gx_system_event_queue;

    if (pQueue->tx_queue_enqueued)
    {
        pSrc =  pQueue->tx_queue_read;

        do
        {
            pEvent = (GX_EVENT*)pSrc;

            if (pEvent->target)
            {
                if (pEvent->target == widget ||
                    pEvent->target->is_child_of_(widget))
                {
                    pEvent->target = GX_NULL;
                    pEvent->type = 0; // event is purged
                }
            }

            pSrc += pQueue->tx_queue_message_size;

            if (pSrc >= pQueue->tx_queue_end)
            {
                pSrc = pQueue->tx_queue_start;
            }
        }
        while (pSrc != pQueue->tx_queue_write);
    }

    TX_RESTORE

#else
    GX_EVENT_PURGE(widget);
#endif
}
