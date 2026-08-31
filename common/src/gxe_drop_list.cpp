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
/**   Drop List Management (List)                                         */
/**                                                                       */
/**************************************************************************/

#include "gx_drop_list.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_drop_list_close                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the drop list close call.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    drop_list                             Drop list control block       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_drop_list_close                   Actual drop list close call   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_drop_list_close(GX_DROP_LIST *drop_list)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!drop_list)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual drop list close function */
    status = _gx_drop_list_close(drop_list);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_drop_list_create                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the drop list create function    */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    drop_list                             Drop list control block       */
/*    name                                  Name of drop list             */
/*    parent                                Pointer to parent widget      */
/*    total_rows                            Total number of rows in       */
/*                                            drop list                   */
/*    open_height                           Height of the vertical list   */
/*    callback                              Callback function             */
/*    style                                 Style of scrollbar widget     */
/*    drop_list_id                          Application-defined ID of     */
/*                                            the drop list               */
/*    size                                  Dimensions of the drop list   */
/*    drop_list_control_block_size          Control block size            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_drop_list_create                  Actual drop list create call  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_drop_list_create(GX_DROP_LIST *drop_list, const GX_CHAR *name,
                            GX_WIDGET *parent, INT total_rows, INT open_height,
                            void (*callback)(GX_VERTICAL_LIST *, GX_WIDGET *, INT),
                            ULONG style, USHORT drop_list_id, const GX_RECTANGLE *size, UINT drop_list_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!drop_list || !size)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for control block size. */
    if (drop_list_control_block_size != sizeof(GX_DROP_LIST))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (drop_list->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Call actual widget hide function.  */
    status = _gx_drop_list_create(drop_list, name, parent, total_rows, open_height,
                                  callback, style, drop_list_id, size);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_drop_list_event_process                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the drop list event process      */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    drop_list                             Drop list widget control block*/
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_drop_list_event_process           Actual drop list event        */
/*                                            process function            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_drop_list_event_process(GX_DROP_LIST *list, GX_EVENT *event_ptr)
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
    status = _gx_drop_list_event_process(list, event_ptr);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_drop_list_open                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the drop list open function      */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    drop_list                             Drop list control block       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_drop_list_open                    Actual drop list open call    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_drop_list_open(GX_DROP_LIST *drop_list)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!drop_list)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual widget hide function.  */
    status = _gx_drop_list_open(drop_list);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_drop_list_pixelmap_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the drop list pixelmap set       */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    drop_list                             Drop list control block       */
/*    id                                    Resource ID to the pixelmap   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_drop_list_pixelmap_set            Actual drop list pixelmap set */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_drop_list_pixelmap_set(GX_DROP_LIST *drop_list, GX_RESOURCE_ID id)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!drop_list)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual widget hide function.  */
    status = _gx_drop_list_pixelmap_set(drop_list, id);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_drop_list_popup_get                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the drop list popup get          */
/*    function call                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    drop_list                             Drop list control block       */
/*    return_list                           Point to the list stored in   */
/*                                            the drop list.              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_drop_list_popup_get               Actual drop list popup get    */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_drop_list_popup_get(GX_DROP_LIST *drop_list, GX_VERTICAL_LIST **return_list)
{
UINT status;

    /* Check for invalid input pointers.  */
    if (!drop_list)
    {
        return(GX_PTR_ERROR);
    }

    if (!return_list)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_drop_list_popup_get(drop_list, return_list);
    return(status);
}

