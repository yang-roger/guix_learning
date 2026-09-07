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
/**   Utility (Utility)                                                   */
/**                                                                       */
/**************************************************************************/

#include "gx_button.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_button_create                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the button create function call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Pointer to button control     */
/*                                            block                       */
/*    name                                  Logical name of button        */
/*    parent                                Pointer to parent widget      */
/*                                            of button                   */
/*    style                                 Button stuyle                 */
/*    button_id                             Application-defined ID of     */
/*                                            the button                  */
/*    size                                  Size of the button            */

/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_button_create                     Actual button create function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_button_create(GX_BUTTON *button, const GX_CHAR *name, GX_WIDGET *parent, ULONG style,
                         USHORT button_id, const GX_RECTANGLE *size, UINT button_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((button == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget control block size. */
    if (button_control_block_size != sizeof(GX_BUTTON))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (button->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Call the actual button create function.  */
    status = _gx_button_create(button, name, parent, style, button_id, size);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_button_deselect                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the button deselect function     */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Pointer to button control     */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_button_deselect                   Actual button deselect        */
/*                                            function.                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_button_deselect(GX_BUTTON *button, GX_BOOL gen_event)
{
UINT       status;
GX_WIDGET *parent;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((button == GX_NULL) || (button->parent == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    parent = button->parent;

    /* Check for the invalid button.  */
    if ((parent->type == 0) ||
        (button->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual button deselect function.  */
    status = _gx_button_deselect(button, gen_event);

    /* Return the completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_button_event_process                                           */
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
UINT  _gxe_button_event_process(GX_BUTTON *button, GX_EVENT *event_ptr)
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
    status = _gx_button_event_process(button, event_ptr);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_button_select                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the button select function call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_button_select                     Actual button select call     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_button_select(GX_BUTTON *button)
{
GX_WIDGET *parent;
UINT       status;

    /* Check for invalid input pointers.  */
    if ((button == GX_NULL) || (button->parent == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    parent = button->parent;

    /* Check for the invalid widget.  */
    if ((parent->type == 0) ||
        (button->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual button select function.  */
    status = _gx_button_select(button);

    /* Return completion status.  */
    return status;
}
