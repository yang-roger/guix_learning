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

#include "gx_horizontal_list.h"

UINT GX_HORIZONTAL_LIST::create_(const GX_CHAR* name,
                                 GX_WIDGET* parent, INT total,
                                 void (*callback)(GX_HORIZONTAL_LIST*, GX_WIDGET*, INT),
                                 ULONG style, USHORT horizontal_list_id, const GX_RECTANGLE* size)
{
    return GX_WIDGET_LIST::create_(GX_TYPE_HORIZONTAL_LIST, name, parent, total,
                                   (void (*)(GX_WIDGET_LIST*, GX_WIDGET*, INT))callback,
                                   style, horizontal_list_id, size);
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
/*    This service creates a horizontal list.                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                horizontal list widget control  */
/*                                          block                         */
/*    name                                Name of horizontal list         */
/*    parent                                Pointer to parent widget      */
/*    total                                    Total number of colums in  */
/*                                        horizontal list                 */
/*    callback                            Function called to create       */
/*                                          new widgets when the list     */
/*                                          is scrolled.                  */
/*    style                                 Style of scrollbar widget     */
/*    horizontal_list_id                    Application-defined ID of     */
/*                                        horizontal list                 */
/*    size                                Dimensions of horizontal list   */
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
UINT _gx_horizontal_list_create(GX_HORIZONTAL_LIST* list, const GX_CHAR* name,
                                GX_WIDGET* parent, INT total,
                                void (*callback)(GX_HORIZONTAL_LIST*, GX_WIDGET*, INT),
                                ULONG style, USHORT horizontal_list_id, const GX_RECTANGLE* size)
{
    return list->create_(name, parent, total, callback, style, horizontal_list_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_horizontal_list_event_process                                   */
/*                                                           6.1.12       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service processes an event for the horizontal list.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  horizontal list widget control*/
/*                                            block                       */
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_horizontal_list_selected_set      Set the list entry at the     */
/*                                            current list index          */
/*    _gx_widget_find                       Retrieve the width of the     */
/*                                            widget                      */
/*    _gx_horizontal_list_children_position                               */
/*                                          Position the children for the */
/*                                            horizontal list             */
/*    _gx_first_client_child_get            Get the first client child    */
/*    _gx_horizontal_list_scroll            Move up or down the scrollbar */
/*    _gx_horizontal_list_slide_back_check  Check the sliding back of     */
/*                                          the scrollbar                 */
/*    _gx_window_scrollbar_find             Assign a background wallpaper */
/*                                          to a GX_WINDOW object         */
/*    _gx_scrollbar_reset                   Calculate new scrollbar value */
/*    _gx_system_timer_start                Allocate a free timer and     */
/*                                          activates it                  */
/*    _gx_system_top_widget_find            Find top widget under pen     */
/*    _gx_system_timer_stop                 Stop an active GUIX timer     */
/*    _gx_window_event_process              Process events for the        */
/*                                          specified window              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_horizontal_list_event_process(GX_HORIZONTAL_LIST* list, GX_EVENT* event_ptr)
{
    return list->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_horizontal_list_scroll_info_get                                 */
/*                                                           6.4.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function gets the scrollbar information.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  horizontal list widget control*/
/*                                            block                       */
/*    style                                 Style                         */
/*    info                                  Information of scrollbar      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_first_client_child_get            Get the first client child    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
void _gx_horizontal_list_scroll_info_get(GX_HORIZONTAL_LIST* list, ULONG style, GX_SCROLL_INFO* info)
{
    list->scroll_info_get_(style, info);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_horizontal_list_children_position                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function positions the children for the horizontal list.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  Horizontal list widget        */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_width_get                  Retrieves the width of the    */
/*                                            widget                      */
/*    _gx_widget_resize                     Resizes the widget            */
/*    _gx_window_client_width_get           Retrieves the width of the    */
/*                                            client                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_horizontal_list_event_process                                   */
/*                                                                        */
/**************************************************************************/
UINT _gx_horizontal_list_children_position(GX_HORIZONTAL_LIST* list)
{
    return list->children_position_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_horizontal_list_page_index_set                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the index of first list item                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  Horizontal list widget        */
/*                                            control block               */
/*    list_entry                            Pointer to new list entry     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_generate             Creates a new GX_EVENT        */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_horizontal_list_event_process                                   */
/*                                                                        */
/**************************************************************************/
UINT _gx_horizontal_list_page_index_set(GX_HORIZONTAL_LIST* list, INT index)
{
    return list->page_index_set_(index);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_horizontal_list_selected_index_get                              */
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
/*    list                                  Horizontal list widget        */
/*                                            control block               */
/*    return_list_entry                     Destination for return list   */
/*                                            entry widget                */
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
UINT _gx_horizontal_list_selected_index_get(GX_HORIZONTAL_LIST* list, INT* return_index)
{
    *return_index = list->selected;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_horizontal_list_selected_widget_get                             */
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
/*    list                                  Horizontal list widget        */
/*                                            control block               */
/*    return_list                           Destination for return list   */
/*                                            entry widget                */
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
UINT _gx_horizontal_list_selected_widget_get(GX_HORIZONTAL_LIST* list, GX_WIDGET** return_list)
{
    GX_WIDGET* widget = list->selected_widget_get_();
    *return_list = widget;

    return widget ? GX_SUCCESS : GX_FAILURE;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_horizontal_list_selected_set                                    */
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
/*    list                                  Horizontal list widget        */
/*                                            control block               */
/*    index                                 Index based position of new   */
/*                                            list entry                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_first_client_child_get                                   */
/*    _gx_widget_next_client_child_get                                    */
/*    _gx_horizontal_list_page_index_set                                  */
/*    _gx_widget_event_generate                                           */
/*    _gx_system_dirty_mark                                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_horizontal_list_event_process                                   */
/*                                                                        */
/**************************************************************************/
UINT _gx_horizontal_list_selected_set(GX_HORIZONTAL_LIST* list, INT index)
{
    return list->selected_set_(index);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_horizontal_list_total_columns_set                               */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns the number of list columns                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  Horizontal list widget        */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    GX_ENTER_CRITICAL                     Obtain GUIX system lock       */
/*    GX_EXIT_CRITICAL                      Release GUIX system lock      */
/*    _gx_first_client_child_get            Get the first client child    */
/*    _gx_window_scrollbar_find             Find the scrollbar            */
/*    _gx_scrollbar_reset                   Reset the schollbar           */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_horizontal_list_total_columns_set(GX_HORIZONTAL_LIST* list, INT count)
{
    return list->total_set_(count);
}
