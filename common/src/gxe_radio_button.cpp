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
/**   Button Management (radio_button)                                    */
/**                                                                       */
/**************************************************************************/

#include "gx_radio_button.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radio_button_create                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks for error in radio button create call.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    name                                  Name of button                */
/*    parent                                Parent widget control block   */
/*    text_id                               text resource id              */
/*    style                                 Style of button               */
/*    radio_button_id                       Application-defined ID of     */
/*                                            radio button                */
/*    size                                  Button size                   */
/*    radio_button_control_block_size       Size of the radio button      */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radio_button_create               The actual radio button       */
/*                                            create function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_radio_button_create(GX_RADIO_BUTTON *button, const GX_CHAR *name, GX_WIDGET *parent,
                               GX_RESOURCE_ID text_id, ULONG style, USHORT radio_btton_id,
                               const GX_RECTANGLE *size, UINT radio_button_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if ((button == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for widget already created.  */
    if (button->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid control block size. */
    if (radio_button_control_block_size != sizeof(GX_RADIO_BUTTON))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for invalid parent widget. */
    if (parent && parent->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual function.  */
    status = _gx_radio_button_create(button, name, parent, text_id, style, radio_btton_id, size);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radio_button_pixelmap_set                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the checkbox pixlemap set routine*/
/*    button.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    off_id                                Resource ID of the pixelmap   */
/*                                            used for draw button off    */
/*    on_id                                 Resource ID of the pixelmap   */
/*                                            used for checke  d box      */
/*    off_disabled_id                       Resource ID of the pixelmap   */
/*                                            used for unchecked disabled */
/*                                             box                        */
/*    on_disabled_id                        Resource ID of the pixelmap   */
/*                                            used for checked disabled   */
/*                                            box                         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radio_button_pixelmap_set         Acutal pixelmap set function  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_radio_button_pixelmap_set(GX_RADIO_BUTTON *button,
                                     GX_RESOURCE_ID off_id,
                                     GX_RESOURCE_ID on_id,
                                     GX_RESOURCE_ID off_disabled_id,
                                     GX_RESOURCE_ID on_disabled_id)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if (button == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (button->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    status = _gx_radio_button_pixelmap_set(button, off_id, on_id,
                                           off_disabled_id, on_disabled_id);

    return(status);
}
