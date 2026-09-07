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
/**   Prompt Management (Prompt)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_icon.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_icon_create                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the icon create function call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    icon                                  Pointer to icon control block */
/*    name                                  Logical name of icon widget   */
/*    parent                                Pointer to the parent widget  */
/*    pixelmap_id                           Resource ID of pixelmap       */
/*    style                                 Style of icon                 */
/*    icon_id                               Application-definedID of icon */
/*    x                                     Starting x-coordinate position*/
/*    y                                     Starting y-coordinate position*/
/*    button_control_block_size             Size of the button control    */
/*                                            block                       */
/*                                                                        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_icon_create                       Actual icon create function   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_icon_create(GX_ICON *icon, const GX_CHAR *name, GX_WIDGET *parent,
                       GX_RESOURCE_ID pixelmap_id, ULONG style, USHORT icon_id,
                       GX_VALUE x, GX_VALUE y, UINT icon_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (icon == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (icon_control_block_size != sizeof(GX_ICON))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (icon->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Call the actual icon create function.  */
    status = _gx_icon_create(icon, name, parent, pixelmap_id, style, icon_id, x, y);

    /* Return completion status.  */
    return status;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_icon_event_process                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the button event process         */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Pointer to button control     */
/*                                            block                       */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_button_event_process              Actual button event process   */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_icon_event_process(GX_ICON *button, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((button == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual button event process function.  */
    status = _gx_icon_event_process(button, event_ptr);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_icon_pixelmap_set                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the icon pixelmap set function.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    icon                            Pointer to icon widget control block*/
/*    normal_pixelmap_id              Normal state pixelamp resource ID   */
/*    selected_pixelmap_id            Selected state pixelmap resource ID */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                          Completion status                   */
/*                                                                        */
/*  CALLS                                                                 */
/*   _gx_icon_pixelmap_set            Actual icon pixelmap set call       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_icon_pixelmap_set(GX_ICON *icon,
                             GX_RESOURCE_ID normal_pixelmap_id,
                             GX_RESOURCE_ID selected_pixelmap_id)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (icon == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual icon pixelmap set function.  */
    status = _gx_icon_pixelmap_set(icon, normal_pixelmap_id, selected_pixelmap_id);

    /* Return completion status.  */
    return status;
}
