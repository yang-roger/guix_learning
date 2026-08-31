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

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_horizontal_list_children_position                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the horizontal list children     */
/*    position function call                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    horizontal_list                       Horizontal list widget        */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_horizontal_list_children_position Actual horizontal list        */
/*                                            children position function  */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_horizontal_list_children_position(GX_HORIZONTAL_LIST *horizontal_list)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!horizontal_list)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual horizontal list children position call. */
    status = _gx_horizontal_list_children_position(horizontal_list);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_horizontal_list_create                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the horizontal list create       */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    horizontal_list                       Horizontal list widget control*/
/*                                          block                         */
/*    name                                  Name of horizontal list       */
/*    parent                                Pointer to parent widget      */
/*    total_columns                         Total number of columsn in    */
/*                                          horizontal list               */
/*    style                                 Style of scrollbar widget     */
/*    horizontal_list_id                      Application-defined ID of   */
/*                                          horizontal list               */
/*    size                                  Dimensions of horizontal list */
/*    horizontal_list_control_block_size      Size of the horizontal list */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_horizontal_list_create            Actual horizontal list create */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_horizontal_list_create(GX_HORIZONTAL_LIST *horizontal_list,
                                  const GX_CHAR *name,
                                  GX_WIDGET *parent, INT total_columns,
                                  void (*callback)(GX_HORIZONTAL_LIST *, GX_WIDGET *, INT),
                                  ULONG style, USHORT horizontal_list_id,
                                  const GX_RECTANGLE *size,
                                  UINT horizontal_list_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((horizontal_list == GX_NULL) || (size == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    if (horizontal_list_control_block_size != sizeof(GX_HORIZONTAL_LIST))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for id is created.  */
    if (horizontal_list->type != 0)
    {
        return GX_ALREADY_CREATED;
    }

    /* Check for valid number of rows. */
    if (total_columns <= 0)
    {
        return GX_INVALID_VALUE;
    }

    /* Call the actual horizontal list create function.  */
    status = _gx_horizontal_list_create(horizontal_list, name, parent, total_columns, callback, style, horizontal_list_id, size);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_horizontal_list_event_process                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the horizontal list event        */
/*    process function call.                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  Horizontal list widget        */
/*                                            control block               */
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_horizontal_list_event_process     Actual horizontal list event  */
/*                                            process function            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_horizontal_list_event_process(GX_HORIZONTAL_LIST *list, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((list == GX_NULL) || (event_ptr == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual horizontal list event process function.  */
    status = _gx_horizontal_list_event_process(list, event_ptr);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_horizontal_list_page_index_set                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the horizontal list page index   */
/*      set call.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    horizontal_list                       Horizontal list widget        */
/*                                            control block               */
/*    list_entry                            Pointer to new list entry     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_horizontal_list_page_index_set    Actual horizontal list page   */
/*                                            index set function          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_horizontal_list_page_index_set(GX_HORIZONTAL_LIST *horizontal_list, INT list_entry)
{

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (horizontal_list == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid list entry.  */
    if ((list_entry < 0) ||
        (list_entry >= horizontal_list->total))
    {
        return GX_INVALID_VALUE;
    }

    /* Call the actual horizontal list page index set function.  */
    _gx_horizontal_list_page_index_set(horizontal_list, list_entry);

    /* Return completion status.  */
    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_horizontal_list_selected_index_get                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the horizontal list selected get */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    horizontal_list                       Horizontal list widget        */
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
/*    _gx_horizontal_list_selected_index_get Actual horizontal lis       */
/*                                            selected get function       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_horizontal_list_selected_index_get(GX_HORIZONTAL_LIST *horizontal_list,
                                              INT *return_index)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((horizontal_list == GX_NULL) || (return_index == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual horizontal list selected get function.  */
    status = _gx_horizontal_list_selected_index_get(horizontal_list, return_index);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_horizontal_list_selected_set                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the horizontal list selected set */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    horizontal_list                       Horizontal list widget        */
/*                                            control block               */
/*    list_entry                            Pointer to new list entry     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_horizontal_list_selected_set      Actual horizontal list        */
/*                                            seelcted set function       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_horizontal_list_selected_set(GX_HORIZONTAL_LIST *horizontal_list, INT index)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (horizontal_list == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Check for valid parent size.  */
    if (horizontal_list->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call the actual horizontal list selected set function.  */
    status = _gx_horizontal_list_selected_set(horizontal_list, index);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_horizontal_list_selected_widget_get                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the horizontal list selected get */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    horizontal_list                       Horizontal list widget        */
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
/*    _gx_horizontal_list_selected_widget_get Actual horizontal lis       */
/*                                            selected get function       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_horizontal_list_selected_widget_get(GX_HORIZONTAL_LIST *horizontal_list,
                                               GX_WIDGET **return_list_entry)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((horizontal_list == GX_NULL) || (return_list_entry == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual horizontal list selected get function.  */
    status = _gx_horizontal_list_selected_widget_get(horizontal_list, return_list_entry);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_horizontal_list_total_columns_set                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function scrolls down the vertical list.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    vertical_list                         Vertical list widget control  */
/*                                          block                         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gxe_horizontal_list_total_columns_set(GX_HORIZONTAL_LIST *list, INT count)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (list == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (count < 0)
    {
        return GX_INVALID_VALUE;
    }

    status = _gx_horizontal_list_total_columns_set(list, count);
    return status;
}

