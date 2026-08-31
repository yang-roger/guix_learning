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

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_vertical_list_children_position                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the vertical list children       */
/*    position function call                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    vertical_list                         Vertical list control block   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_vertical_list_children_position   Actual vertical list children */
/*                                            position function call      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_vertical_list_children_position(GX_VERTICAL_LIST *vertical_list)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!vertical_list)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (vertical_list->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call actual vertical list children position call. */
    status = _gx_vertical_list_children_position(vertical_list);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_vertical_list_create                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the vertical list create         */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    vertical_list                         Vertical list control block   */
/*    name                                  Name of vertical list         */
/*    parent                                Pointer to parent widget      */
/*    total_rows                            Total number of rows in       */
/*                                            vertical list               */
/*    callback                              User-specified Callback       */
/*                                            function                    */
/*    style                                 Style of scrollbar widget     */
/*    vertical_list_id                      Application-defined ID of     */
/*                                            vertical list               */
/*    size                                  Dimensions of vertical list   */
/*    vertical_list_control_block_size      Size of the vertical list     */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_vertical_list_create              Actual vertical list create   */
/*                                          function                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_vertical_list_create(GX_VERTICAL_LIST *vertical_list, const GX_CHAR *name,
                                GX_WIDGET *parent, INT total_rows,
                                void (*callback)(GX_VERTICAL_LIST *, GX_WIDGET *, INT),
                                ULONG style, USHORT vertical_list_id, const GX_RECTANGLE *size,
                                UINT vertical_list_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((vertical_list == GX_NULL) || (size == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    if (vertical_list_control_block_size != sizeof(GX_VERTICAL_LIST))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for id is created.  */
    if (vertical_list->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for valid number of rows. */
    if (total_rows <= 0)
    {
        return GX_INVALID_VALUE;
    }

    /* Check for invalid widget. */
    if (parent && (parent->type == 0))
    {
        return GX_INVALID_WIDGET;
    }

    /* Call the actual vertical list create function.  */
    status = _gx_vertical_list_create(vertical_list, name, parent, total_rows, callback, style, vertical_list_id, size);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_vertical_list_event_process                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the vertical list event process  */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Vertical list control block   */
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_vertical_list_event_process       Actual vertical list event    */
/*                                            process function            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_vertical_list_event_process(GX_VERTICAL_LIST *list, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((list == GX_NULL) || (event_ptr == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget. */
    if (list->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call the actual vertical list event process function.  */
    status = _gx_vertical_list_event_process(list, event_ptr);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_vertical_list_page_index_set                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the vertical list index set      */
/*      function call.                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    vertical_list                         Vertical list widget control  */
/*                                          block                         */
/*    index                                 The new top index             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_vertical_list_page_index_set      Actual vertial list page      */
/*                                            index set function          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_vertical_list_page_index_set(GX_VERTICAL_LIST *vertical_list, INT index)
{

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (vertical_list == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget. */
    if (vertical_list->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Check for invalid list entry.  */
    if ((index < 0) ||
        (index >= vertical_list->total))
    {
        return GX_INVALID_VALUE;
    }

    /* Call the actual vertical list page index set function.  */
    _gx_vertical_list_page_index_set(vertical_list, index);

    /* Return completion status.  */
    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_vertical_list_selected_index_get                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the vertical list selected get   */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    vertical_list                         Vertical list control block   */
/*    return_list_entry                     Destination for return list   */
/*                                            entry widget                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_vertical_list_selected_get        Actual vertical list selected */
/*                                            get function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_vertical_list_selected_index_get(GX_VERTICAL_LIST *vertical_list,
                                            INT *return_index)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((vertical_list == GX_NULL) || (return_index == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget. */
    if (vertical_list->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call the actual vertical list selected get function.  */
    status = _gx_vertical_list_selected_index_get(vertical_list, return_index);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_vertical_list_selected_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the vertical list selected set   */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    vertical_list                         Vertical list control block   */
/*    list_entry                            Pointer to new list entry     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_vertical_list_selected_set        Actual vertical list selected */
/*                                            set function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_vertical_list_selected_set(GX_VERTICAL_LIST *vertical_list, INT index)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (vertical_list == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget. */
    if (vertical_list->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call the actual vertical list selected set function.  */
    status = _gx_vertical_list_selected_set(vertical_list, index);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_vertical_list_selected_widget_get                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the vertical list selected get   */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    vertical_list                         Vertical list control block   */
/*    return_list_entry                     Destination for return list   */
/*                                            entry widget                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_vertical_list_selected_get        Actual vertical list selected */
/*                                            get function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_vertical_list_selected_widget_get(GX_VERTICAL_LIST *vertical_list,
                                             GX_WIDGET **return_list_entry)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((vertical_list == GX_NULL) || (return_list_entry == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget. */
    if (vertical_list->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call the actual vertical list selected get function.  */
    status = _gx_vertical_list_selected_widget_get(vertical_list, return_list_entry);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_vertical_list_total_rows_set                                   */
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
/*    vertical_list                         Vertical list control block   */
/*    count                                 Number of rows                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_vertical_list_total_rows_set      Actual vertical list total    */
/*                                            row set call                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_vertical_list_total_rows_set(GX_VERTICAL_LIST *list, INT count)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (list == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget. */
    if (list->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    if (count < 0)
    {
        return GX_INVALID_VALUE;
    }

    status = _gx_vertical_list_total_rows_set(list, count);
    return status;
}
