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

#include "gx_window.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_client_height_get                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window client height get     */
/*    function call.                                                      */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window             */
/*    return_height                         Pointer to destination        */
/*                                            for client height           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*    _gx_window_client_height_get          Actual window client height   */
/*                                            get function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_window_client_height_get(GX_WINDOW *window, GX_VALUE *return_height)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((window == GX_NULL) || (return_height == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (window->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual window client height get function.  */
    status = _gx_window_client_height_get(window, return_height);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_client_scroll                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window client scroll         */
/*    function call.                                                      */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window             */
/*    x_scroll                              Amount to scroll on x-axis    */
/*    y_scroll                              Amount to scroll on y-axis    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_client_scroll              Actual window client scroll   */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_window_client_scroll(GX_WINDOW *window, GX_VALUE x_scroll, GX_VALUE y_scroll)
{
UINT          status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (window == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (window->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual window client scroll function.  */
    status = _gx_window_client_scroll(window, x_scroll, y_scroll);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_client_width_get                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window client width get      */
/*    function call.                                                      */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window             */
/*    return_width                          Pointer to destination        */
/*                                            for client width            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*    _gx_window_client_width_get           Actual window client width    */
/*                                            get function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_window_client_width_get(GX_WINDOW *window, GX_VALUE *return_width)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((window == GX_NULL) || (return_width == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (window->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual window client width get function.  */
    status = _gx_window_client_width_get(window, return_width);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_close                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window close                 */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Window's widget control block */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_close                      Actual window event process   */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_window_close(GX_WINDOW *window)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (window == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (window->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual window draw function.  */
    status = _gx_window_close(window);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_create                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window create function call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window control     */
/*                                           block                        */
/*    name                                  Logical name of window        */
/*    parent                                Pointer to parent window      */
/*    style                                 Window Style                  */
/*    window_id                             Application-defined ID of the */
/*                                            window                      */
/*    size                                  Size of the window            */
/*    window_control_block_size             Size of the window control    */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_create                     Actual window create function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_window_create(GX_WINDOW *window, const GX_CHAR *name, GX_WIDGET *parent,
                         ULONG style, USHORT window_id, const GX_RECTANGLE *size,
                         UINT window_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((window == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for widget already created.  */
    if (window->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid widget control block size. */
    if (window_control_block_size != sizeof(GX_WINDOW))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for invalid parent widget. */
    if (parent && (parent->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual window create function.  */
    status = _gx_window_create(window, name, parent, style, window_id, size);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_event_process                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window event process         */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Window's widget control block */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_event_process              Actual window event process   */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_window_event_process(GX_WINDOW *window, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((window == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (window->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual window draw function.  */
    status = _gx_window_event_process(window, event_ptr);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_execute                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window execute               */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Window's widget control block */
/*    return_ptr                            return value                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_execute                    Actual window event process   */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_window_execute(GX_WINDOW *window, ULONG *return_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (window == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (window->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual window draw function.  */
    status = _gx_window_execute(window, return_ptr);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_scroll_info_get                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window scroll info get       */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                               Pointer to window              */
/*    type                                 GX_SCROLLBAR_HORIZONTAL        */
/*                                           or GX_SCROLLBAR_VERTICAL     */
/*    return_scroll_info                   Pointer to destination for     */
/*                                           scroll info                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                               Completion status              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_scroll_info_get           Actual window scroll info      */
/*                                         get function                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_window_scroll_info_get(GX_WINDOW *window, ULONG type, GX_SCROLL_INFO *return_scroll_info)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((window == GX_NULL) || (return_scroll_info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (window->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid type.  */
    if ((type != GX_SCROLLBAR_HORIZONTAL) && (type != GX_SCROLLBAR_VERTICAL))
    {
        return(GX_INVALID_TYPE);
    }

    /* Call actual window scroll info get function.  */
    status = _gx_window_scroll_info_get(window, type, return_scroll_info);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_scrollbar_find                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window scrollbar find        */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window             */
/*    type                                  GX_SCROLLBAR_HORIZONTAL or    */
/*                                            GX_SCROLLBAR_VERTICAL       */
/*    return_scrollbar                      Pointer to destination for    */
/*                                            scrollbar                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_scrollbar_find             Actual window scrollbar find  */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_window_scrollbar_find(GX_WINDOW *window, USHORT type, GX_SCROLLBAR **return_scrollbar)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((window == GX_NULL) || (return_scrollbar == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (window->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid type.  */
    if ((type != GX_TYPE_HORIZONTAL_SCROLL) && (type != GX_TYPE_VERTICAL_SCROLL))
    {
        return(GX_INVALID_TYPE);
    }

    /* Call actual window scrollbar find function.  */
    status = _gx_window_scrollbar_find(window, type, return_scrollbar);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_wallpaper_get                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window wallpaper get         */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                                Pointer to window             */
/*    return_wallpaper_id                   Pointer to destination for    */
/*                                            resource ID of wallpaper    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_wallpaper_get              Actual window wallpaper get   */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_window_wallpaper_get(GX_WINDOW *window, GX_RESOURCE_ID *return_wallpaper_id)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((window == GX_NULL) || (return_wallpaper_id == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (window->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual window wallpaper get function.  */
    status = _gx_window_wallpaper_get(window, return_wallpaper_id);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_window_wallpaper_set                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the window wallpaper set         */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    window                             Pointer to window                */
/*    wallpaper_id                       Resource ID of wallpaper to use  */
/*    tile                               Wallpaper is tiled if GX_TRUE,   */
/*                                         otherwise wallpaper is         */
/*                                         not tiled                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                             Completion status                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_wallpaper_set           Actual window wallpaper set      */
/*                                         function                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_window_wallpaper_set(GX_WINDOW *window, GX_RESOURCE_ID wallpaper_id, GX_BOOL tile)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (window == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (window->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual window wallpaper set function.  */
    status = _gx_window_wallpaper_set(window, wallpaper_id, tile);

    /* Return successful completion.  */
    return(status);
}
