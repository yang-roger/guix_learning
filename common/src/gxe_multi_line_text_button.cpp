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
/**   Text Button Management (Button)                                     */
/**                                                                       */
/**************************************************************************/

#include "gx_multi_line_text_button.h"

#include "gx_utility.h"

GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_button_create                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the text button create function  */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Button control block          */
/*    name                                  Name of button                */
/*    parent                                Parent widget control block   */
/*    text_id                               text resource id              */
/*    style                                 Style of button               */
/*    size                                  Button size                   */
/*    text_button_control_block_size        Size of the text button       */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_button_create                Actual text button create     */
/*                                          function                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_multi_line_text_button_create(GX_MULTI_LINE_TEXT_BUTTON *button, const GX_CHAR *name, GX_WIDGET *parent,
                                         GX_RESOURCE_ID text_id, ULONG style, USHORT Id,
                                         const GX_RECTANGLE *size, UINT text_button_control_block_size)
{

UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((button == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (text_button_control_block_size != sizeof(GX_MULTI_LINE_TEXT_BUTTON))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for id is created.  */
    if (button->type != 0)
    {
        return GX_ALREADY_CREATED;
    }

    /* Call the actual text button create function.  */
    status = _gx_multi_line_text_button_create(button, name, parent, text_id, style, Id, size);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_button_event_process                           */
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
UINT  _gxe_multi_line_text_button_event_process(GX_MULTI_LINE_TEXT_BUTTON *button, GX_EVENT *event_ptr)
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
    status = _gx_multi_line_text_button_event_process(button, event_ptr);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_button_text_id_set                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the button text assignment       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Pointer to the button         */
/*                                            control block               */
/*    string_id                             Resource ID from string table */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_button_text_id_set Actual function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_multi_line_text_button_text_id_set(GX_MULTI_LINE_TEXT_BUTTON *button,
                                              GX_RESOURCE_ID string_id)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (button == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual button draw function.  */
    status = _gx_multi_line_text_button_text_id_set(button, string_id);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_button_text_set_ext                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the text assigment.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    button                                Pointer to the button         */
/*                                            control block               */
/*    text                                  Text to be set                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_button_text_set_ext                             */
/*                                          Actual text assignment        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_button_text_set_ext(GX_MULTI_LINE_TEXT_BUTTON *button, const GX_STRING *text)
{
UINT status;
UINT text_length = 0;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (button == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (text)
    {
        if (text->ptr)
        {
            status = _gx_utility_string_length_check(text->ptr, &text_length, text->length);

            if (status != GX_SUCCESS)
            {
                return status;
            }
        }

        if (text_length != text->length)
        {
            return GX_INVALID_STRING_LENGTH;
        }
    }

    /* Call the actual button draw function.  */
    status = _gx_multi_line_text_button_text_set_ext(button, text);

    /* Return completion status.  */
    return status;
}
