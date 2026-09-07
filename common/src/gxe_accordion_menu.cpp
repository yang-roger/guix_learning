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
/**   Accordion Menu Management (Menu)                                    */
/**                                                                       */
/**************************************************************************/

#include "gx_accordion_menu.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_accordion_menu_create                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in accordion menu create function   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    menu                                  Pointer to the accordion menu */
/*                                            control block               */
/*    name                                  Name of the menu              */
/*    parent                                Parent widget control block   */
/*    style                                 Style of the widget           */
/*    tree_menu_id                          Application-defined ID of     */
/*                                          the accordion menu            */
/*    size                                  Accordion menu size           */
/*    control_block_size                    Control block size            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_accordion_menu_create             Actual accordion menu create  */
/*                                            function call               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_accordion_menu_create(GX_ACCORDION_MENU *accordion, const GX_CHAR *name, GX_WIDGET *parent,
                                 ULONG style, USHORT accordion_menu_id, const GX_RECTANGLE *size, UINT control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((!accordion) || (!size))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for widget already created.  */
    if (accordion->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid control block size. */
    if (control_block_size != sizeof(GX_ACCORDION_MENU))
    {
        return(GX_INVALID_SIZE);
    }

    /* Call actual accordion menu create function.  */
    status = _gx_accordion_menu_create(accordion, name, parent, style, accordion_menu_id, size);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_accordion_menu_event_process                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in accordion menu event process     */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    accordion                             Pointer to accordion menu     */
/*                                            control block               */
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_accordion_menu_event_process      Actual accordion menu event   */
/*                                            process function call       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_accordion_menu_event_process(GX_ACCORDION_MENU *accordion, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((!accordion) || (!event_ptr))
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual accordion menu draw function.  */
    status = _gx_accordion_menu_event_process(accordion, event_ptr);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_accordion_menu_position                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in accordion menu position function */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    accordion                             Pointer to the accordion menu */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_accordion_menu_position           Actual accordion menu position*/
/*                                            function call               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_accordion_menu_position(GX_ACCORDION_MENU *accordion)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!accordion)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual accordion menu position function.  */
    status = _gx_accordion_menu_position(accordion);

    return(status);
}
