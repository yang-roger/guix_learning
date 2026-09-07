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
/**   Button Management (checkbox)                                        */
/**                                                                       */
/**************************************************************************/

#include "gx_checkbox.h"

GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_checkbox_create                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the checkbox create function     */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    checkbox                              Checkbox control block        */
/*    name                                  Name of checkbox              */
/*    parent                                Parent widget control block   */
/*    text_id                               text resource id              */
/*    style                                 Style of checkbox             */
/*    checkbox_id                           Application-defined ID of     */
/*                                          checkbox                      */
/*    size                                  Checkbox size                 */
/*    checkbox_control_block_size           Size of the checkbox control  */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_checkbox_create                   Actual checkbox create call   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_checkbox_create(GX_CHECKBOX *checkbox, const GX_CHAR *name, GX_WIDGET *parent,
                           GX_RESOURCE_ID text_id, ULONG style, USHORT checkbox_id,
                           const GX_RECTANGLE *size, UINT checkbox_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if ((checkbox == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (checkbox_control_block_size != sizeof(GX_CHECKBOX))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for the checkbox is already created.  */
    if (checkbox->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Call the actual checkbox create funtion.  */
    status = _gx_checkbox_create(checkbox, name, parent, text_id, style, checkbox_id, size);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_checkbox_event_process                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the checkbox event process       */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    checkbox                              Checkbox control block        */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_checkbox_event_process            Actual checkbox event process */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_checkbox_event_process(GX_CHECKBOX *checkbox, GX_EVENT *event_ptr)
{
UINT       status;
GX_WIDGET *parent;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if ((checkbox == GX_NULL) || (checkbox->parent == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    parent = checkbox->parent;

    /* Check for the invalid widget.  */
    if ((parent->type == 0) ||
        (checkbox->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual checkbox event process function.  */
    status = _gx_checkbox_event_process(checkbox, event_ptr);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_checkbox_pixelmap_set                                          */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the checkbox pixlemap set        */
/*    routine.                                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    unchecked_id                          Resource ID of the pixelmap   */
/*                                            used for unchecked box      */
/*    checked_id                            Resource ID of the pixelmap   */
/*                                            used for checked box        */
/*    unchecked_disabled_id                 Resource ID of the pixelmap   */
/*                                            used for unchecked disabled */
/*                                             box                        */
/*    checked_disabled_id                   Resource ID of the pixelmap   */
/*                                            used for checked disabled   */
/*                                            box                         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_checkbox_pixelmap_set             Actual function sets the      */
/*                                            pixelmap                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_checkbox_pixelmap_set(GX_CHECKBOX *checkbox,
                                 GX_RESOURCE_ID unchecked_id,
                                 GX_RESOURCE_ID checked_id,
                                 GX_RESOURCE_ID unchecked_disabled_id,
                                 GX_RESOURCE_ID checked_disabled_id)
{
UINT status;

    /* Check for the invalid input pointers.  */
    if (checkbox == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for the invalid widget.  */
    if (checkbox->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    status = _gx_checkbox_pixelmap_set(checkbox, unchecked_id, checked_id,
                                       unchecked_disabled_id, checked_disabled_id);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_checkbox_select                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the checkbox select function     */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    checkbox                              Checkbox control block        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_checkbox_select                   Actual checkbox select call   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_checkbox_select(GX_CHECKBOX *checkbox)
{
UINT       status;
GX_WIDGET *parent;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if ((checkbox == GX_NULL) || (checkbox->parent == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    parent = checkbox->parent;

    /* Check for the invalid widget.  */
    if ((parent->type == 0) ||
        (checkbox->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual checkbox select function.  */
    status = _gx_checkbox_select(checkbox);

    /* Return completion status.  */
    return status;
}
