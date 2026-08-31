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

#include "gx_vertical_list.h"

UINT GX_VERTICAL_LIST::create_(const GX_CHAR* name, GX_WIDGET* parent, INT total,
                               void (*callback)(GX_VERTICAL_LIST*, GX_WIDGET*, INT),
                               ULONG style, USHORT vertical_list_id, const GX_RECTANGLE* size)
{
    return GX_WIDGET_LIST::create_(GX_TYPE_VERTICAL_LIST, name, parent, total,
                                   (void (*)(GX_WIDGET_LIST*, GX_WIDGET*, INT))callback,
                                   style, vertical_list_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_vertical_list_children_position                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function positions the children for the vertical list          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  Vertical list control block   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_height_get                 retrieves the height of the   */
/*                                            widget                      */
/*    _gx_widget_resize                     resizes the widget            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_vertical_list_children_position(GX_VERTICAL_LIST* list)
{
    return list->children_position_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_vetical_list_create                                             */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service creates a vertical list.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  Vertical list control block   */
/*    name                                  Name of vertical list         */
/*    parent                                Pointer to parent widget      */
/*    total                                 Total number of rows in       */
/*                                            vertical list               */
/*    callback                              User-specified Callback       */
/*                                            function                    */
/*    style                                 Style of scrollbar widget     */
/*    vertical_list_id                      Application-defined ID of     */
/*                                            vertical list               */
/*    size                                  Dimensions of vertical list   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_create                     Create the underlying window  */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_vertical_list_create(GX_VERTICAL_LIST* list,
                              const GX_CHAR* name, GX_WIDGET* parent, INT total,
                              void (*callback)(GX_VERTICAL_LIST*, GX_WIDGET*, INT),
                              ULONG style, USHORT vertical_list_id,
                              const GX_RECTANGLE* size)
{
    return list->create_(name, parent, total, callback, style, vertical_list_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_vertical_list_event_process                                     */
/*                                                           6.1.12       */
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
/*    list                                  Vertical list control block   */
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_vertical_list_selected_set        Set the list entry at the     */
/*                                            current list index          */
/*    _gx_widget_find                       Retrieve the height of the    */
/*                                            widget                      */
/*    _gx_vertical_list_children_position   Position the children for     */
/*                                            the vertical list           */
/*    _gx_first_client_child_get            Get the first client child    */
/*    _gx_vertical_list_scroll              Move up or down the scrollbar */
/*    _gx_vertical_list_slide_back_check    Check the sliding back of     */
/*                                            the scrollbar               */
/*    _gx_window_scrollbar_find             Assign a background wallpaper */
/*                                            to a GX_WINDOW object       */
/*    _gx_scrollbar_reset                   Calculate new scrollbar value */
/*    _gx_system_timer_start                Allocate a free timer and     */
/*                                            activates it                */
/*    _gx_system_top_widget_find            Find top widget under pen     */
/*    _gx_system_timer_stop                 Stop an active GUIX timer     */
/*    _gx_window_event_process              Process events for the        */
/*                                            specified window            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_vertical_list_event_process(GX_VERTICAL_LIST* list, GX_EVENT* event_ptr)
{
    return list->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_vertical_list_page_index_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the starting page index for vertical list.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  Vertical list widget control  */
/*                                          block                         */
/*    index                                 The new top index             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_vertical_list_selected_set        Process drop list event       */
/*                                                                        */
/**************************************************************************/
UINT _gx_vertical_list_page_index_set(GX_VERTICAL_LIST* list, INT index)
{
    return list->page_index_set_(index);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_vertical_list_selected_index_get                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the list entry at the current list index.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  Vertical list widget control  */
/*                                          block                         */
/*    return_list_entry                     Destination for return list   */
/*                                          entry widget                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_vertical_list_selected_index_get(GX_VERTICAL_LIST* list, INT* return_index)
{
    *return_index = list->selected;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_vertical_list_selected_widget_get                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the list entry at the current list index.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  Vertical list widget control  */
/*                                          block                         */
/*    return_list_entry                     Destination for return list   */
/*                                          entry widget                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_vertical_list_selected_widget_get(GX_VERTICAL_LIST* list, GX_WIDGET** return_list_entry)
{
    GX_WIDGET* widget = list->selected_widget_get_();
    *return_list_entry = widget;

    return widget ? GX_SUCCESS : GX_FAILURE;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_vertical_list_selected_set                                      */
/*                                                           6.1.12       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the list entry at the current list index.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  Vertical list widget control  */
/*                                          block                         */
/*    index                                 Index based position of new   */
/*                                          list entry                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_first_client_child_get                                   */
/*    _gx_widget_next_client_child_get                                    */
/*    _gx_vertical_list_page_index_set                                    */
/*    _gx_widget_event_generate                                           */
/*    _gx_system_dirty_mark                                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    _gx_drop_list_event_process           Process drop list event       */
/*                                                                        */
/**************************************************************************/
UINT _gx_vertical_list_selected_set(GX_VERTICAL_LIST* list, INT index)
{
    return list->selected_set_(index);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_vertical_list_total_rows_set                                    */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns total number of list rows.                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  Vertical list control block   */
/*    count                                 Number of rows                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    GX_ENTER_CRITICAL                     Obtain GUIX system lock       */
/*    GX_EXIT_CRITICAL                      Release GUIX system lock      */
/*    _gx_first_client_child_get            Get the first client child    */
/*    [gx_vertical_list_callback]           Vertical list callback        */
/*    _gx_window_scrollbar_find             Find the scrollbar            */
/*    _gx_scrollbar_reset                   Reset the schollbar           */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_vertical_list_total_rows_set(GX_VERTICAL_LIST* list, INT count)
{
    return list->total_set_(count);
}
