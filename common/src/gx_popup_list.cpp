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
/**   Popup List (List)                                                   */
/**                                                                       */
/**************************************************************************/

#include "gx_popup_list.h"

void GX_POPUP_LIST::create_(GX_WIDGET* owner, INT total_rows,
                            void (*callback)(GX_VERTICAL_LIST*, GX_WIDGET*, INT))
{
    GX_VERTICAL_LIST::create_(owner->name, GX_NULL, total_rows, callback,
                              owner->style, owner->id, &owner->size);

    this->type = GX_TYPE_POPUP_LIST;
    this->owner = owner;

    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))_gx_popup_list_event_process;
}

UINT GX_POPUP_LIST::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    GX_EVENT send_event;
    memset(&send_event, 0, sizeof(GX_EVENT));

    switch (event_ptr->type)
    {
    case GX_EVENT_FOCUS_LOST:
        send_event.type = GX_EVENT_CLOSE_POPUP;
        owner->event_process_function_run_(&send_event);
        break;

    case GX_EVENT_PEN_UP:
        // Only close popup list after click event.
        //   If PEN_UP event is following PEN_DRAG, pen index is -1,
        //   and list should not be closed.
        if (pen_index >= 0)
        {
            send_event.type = GX_EVENT_CLOSE_POPUP;
        }
        result = GX_VERTICAL_LIST::event_process_(event_ptr);
        if (send_event.type)
        {
            owner->event_process_function_run_(&send_event);
            owner->dirty_mark_();
        }
        break;

    default:
        result = GX_VERTICAL_LIST::event_process_(event_ptr);
        break;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_popup_list_event_process                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service processes an event for the vertical list.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    popup_list                            Popup List widget control     */
/*                                            block                       */
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_vertical_list_event_process       Process vertical list events  */
/*    [gx_widget_event_process_function]    Widget-specified event        */
/*                                            process routine             */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_popup_list_event_process(GX_POPUP_LIST* popup_list, GX_EVENT* event_ptr)
{
    return popup_list->event_process_(event_ptr);
}

