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
/**   Window Management (Window)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_window_root.h"

#include "gx_system.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_root_create                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in window root create function call.*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_window                           Root window control block     */
/*    name                                  Name of window                */
/*    canvas                                Canvas this root window       */
/*                                            belongs to                  */
/*    style                                 Style of window               */
/*    Id                                    User-specified root window ID */
/*    size                                  Window size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_root_create                Actual root window create call*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_window_root_create(GX_WINDOW_ROOT *root_window,
                              const GX_CHAR *name, GX_CANVAS *canvas,
                              ULONG style, USHORT Id, const GX_RECTANGLE *size, UINT root_window_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((root_window == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid input pointers.  */
    if (canvas == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget control block size.  */
    if (root_window_control_block_size != sizeof(GX_WINDOW_ROOT))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (root_window->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Call actual root window create function. */
    status = _gx_window_root_create(root_window, name, canvas, style, Id, size);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_root_delete                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in window root delete call.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_window                           Root window control block     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_root_destory               Actual window root destory    */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_window_root_delete(GX_WINDOW_ROOT *root_window)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!root_window)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (root_window->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    if (root_window->style & (GX_STYLE_TEXT_COPY | GX_STATUS_DYNAMICALLY_ALLOCATED))
    {
        if (_gx_system_memory_free == GX_NULL)
        {
            return(GX_SYSTEM_MEMORY_ERROR);
        }
    }

    /* Call actual window root destory function.  */
    status = _gx_window_root_delete(root_window);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_root_event_process                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window root event process    */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_window                           Root window control blcok     */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_root_event_process         Actual window root event      */
/*                                            process function            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_window_root_event_process(GX_WINDOW_ROOT *root_window, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if ((!root_window) || (!event_ptr))
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual window root event process function */
    status = _gx_window_root_event_process(root_window, event_ptr);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_root_find                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window root find function    */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget control     */
/*                                            block                       */
/*    return_root_window                    Pointer to destination for    */
/*                                            found root window           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_root_find                  Actual window root find       */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_window_root_find(GX_WIDGET *widget, GX_WINDOW_ROOT **return_root_window)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (return_root_window == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual window root find function.  */
    status = _gx_window_root_find(widget, return_root_window);

    /* Return successful completion.  */
    return(status);
}
