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
/**   Button Management (Button)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_pixelmap_button.h"

GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_pixelmap_button_create                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the pixelmap button create       */
/*                                         function.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    name                                  Name of button                */
/*    parent                                Parent widget control block   */
/*    normal_id                             Normal state pixelmap id      */
/*    selected_id                           Selected state pixelmap id    */
/*    disabled_id                           Disabled state pixelmap id    */
/*    style                                 Style of checkbox             */
/*    pixelmap_button_id                    Application-defined ID of     */
/*                                            the pixelmap button         */
/*    size                                  Button size                   */
/*    button_control_block_size             Size of the pixelmap button   */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_pixelmap_button_create            the actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_pixelmap_button_create(GX_PIXELMAP_BUTTON *button,
                                  const GX_CHAR *name,
                                  GX_WIDGET *parent,
                                  GX_RESOURCE_ID normal_id,
                                  GX_RESOURCE_ID selected_id,
                                  GX_RESOURCE_ID disabled_id,
                                  ULONG style,
                                  USHORT pixelmap_button_id,
                                  const GX_RECTANGLE *size,
                                  UINT button_control_block_size)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer.  */
    if ((button == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid control block size. */
    if (button_control_block_size != sizeof(GX_PIXELMAP_BUTTON))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check error for valid widget.  */
    if (button->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    status = _gx_pixelmap_button_create(button, name, parent, normal_id, selected_id, disabled_id, style, pixelmap_button_id, size);
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_pixelmap_button_event_process                                  */
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
/*                                          function                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_pixelmap_button_event_process(GX_PIXELMAP_BUTTON *button, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((button == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual button event process function.  */
    status = _gx_pixelmap_button_event_process(button, event_ptr);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_pixelmap_button_pixelmap_set                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the pixelmap button pixelmap     */
/*    set function call.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    normal_id                             normal state pixelmap id      */
/*    selected_id                           selected state pixelmap id    */
/*    disabled_id                           disabled state pixelmap id    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_pixelmap_button_pixelmap_set      Actual pixelmap button        */
/*                                            pixelmap set function       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_pixelmap_button_pixelmap_set(GX_PIXELMAP_BUTTON *button,
                                        GX_RESOURCE_ID normal_id,
                                        GX_RESOURCE_ID selected_id,
                                        GX_RESOURCE_ID disabled_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!button)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_pixelmap_button_pixelmap_set(button, normal_id, selected_id,
                                              disabled_id);

    return(status);
}
