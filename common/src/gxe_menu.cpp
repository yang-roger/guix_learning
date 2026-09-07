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
/**   Menu Management (Menu)                                              */
/**                                                                       */
/**************************************************************************/

#include "gx_menu.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_menu_create                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in menu create function call.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to the menu control   */
/*                                            block                       */
/*    name                                  Name of the menu              */
/*    parent                                Parent menu control block     */
/*    text_id                               Resource string id            */
/*    fill_id                               Pixelmap id for fill area     */
/*    style                                 Style of the widget           */
/*    menu_id                               Application-defined ID of     */
/*                                          the menu menu                 */
/*    size                                  Menu size                     */
/*    control_block_size                    Control block size            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_menu_create                       Actual menu create call       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_menu_create(GX_MENU *menu, const GX_CHAR *name, GX_WIDGET *parent,
                       GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                       ULONG style, USHORT menu_id, const GX_RECTANGLE *size, UINT control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((!menu) || (!size))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for widget already created.  */
    if (menu->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid control block size. */
    if (control_block_size != sizeof(GX_MENU))
    {
        return(GX_INVALID_SIZE);
    }

    /* Call actual menu create function. */
    status = _gx_menu_create(menu, name, parent, text_id, fill_id, style, menu_id, size);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_menu_event_process                                             */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the menu event process           */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to menu control       */
/*                                            block                       */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_menu_event_process                Actual menu event process     */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_menu_event_process(GX_MENU *menu, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((menu == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (menu->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual menu event process function.  */
    status = _gx_menu_event_process(menu, event_ptr);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_menu_insert                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in menu insert function call.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to the menu control   */
/*                                            block                       */
/*    widget                                Widget to insert              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_menu_insert                       Actual menu insert call       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_menu_insert(GX_MENU *menu, GX_WIDGET *widget)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((!menu) || (!widget))
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual menu insert function.  */
    status = _gx_menu_insert(menu, widget);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_menu_remove                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in menu remove function call.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to the menu control   */
/*                                            block                       */
/*    widget                                Widget to remove              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_menu_remove                       Actual menu remove call       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_menu_remove(GX_MENU *menu, GX_WIDGET *widget)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((!menu) || (!widget))
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual menu remove function.  */
    status = _gx_menu_remove(menu, widget);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_menu_text_offset_set                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in menu text offset set function    */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to menu control block */
/*    x_offset                              X coordinate of offset        */
/*    y_offset                              Y coordinate of offset        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_menu_text_offset_set              Actual menu text offset set   */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_menu_text_offset_set(GX_MENU *menu, GX_VALUE x_offset, GX_VALUE y_offset)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!menu)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual menu text offset set function.  */
    status = _gx_menu_text_offset_set(menu, x_offset, y_offset);

    return(status);
}
