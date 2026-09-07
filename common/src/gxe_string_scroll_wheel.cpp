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
/**   String Scroll Wheel Management (Scroll Wheel)                       */
/**                                                                       */
/**************************************************************************/

#include "gx_string_scroll_wheel.h"

#include "gx_utility.h"

GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_string_scroll_wheel_create_ext                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in string scroll wheel create call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    name                                  Name of widget                */
/*    parent                                Parent widget control block   */
/*    total_rows                            Total rows of the scroll wheel*/
/*    string_list                           String list for display       */
/*    callback                              Callback for retrieved row    */
/*                                            string                      */
/*    style                                 Style of widget               */
/*    Id                                    Application-defined ID of the */
/*                                            the widget                  */
/*    size                                  Widget size                   */
/*    control_block_size                    Size of the scroll wheel      */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_scroll_wheel_create_ext      Actual text scroll wheel      */
/*                                            create ext call             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_string_scroll_wheel_create_ext(GX_STRING_SCROLL_WHEEL *wheel, const GX_CHAR *name, GX_WIDGET *parent, INT total_rows,
                                         const GX_STRING *string_list,
                                         ULONG style, USHORT Id, const GX_RECTANGLE *size, UINT control_block_size)
{
UINT                status;
INT                 string_id;
const GX_STRING *string;
UINT                string_length;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if ((wheel == GX_NULL) || (size == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid value. */
    if (control_block_size != sizeof(GX_STRING_SCROLL_WHEEL))
    {
        return GX_INVALID_SIZE;
    }

    /* Check for widget already created.  */
    if (wheel->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid widget. */
    if (parent && (parent->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    if (string_list)
    {
        /* Test string length. */
        for (string_id = 0; string_id < total_rows; string_id++)
        {
            string = &string_list[string_id];
            if (string->ptr)
            {
                status = _gx_utility_string_length_check(string->ptr, &string_length, string->length);

                if (status != GX_SUCCESS)
                {
                    return status;
                }
            }
            else
            {
                string_length = 0;
            }

            if (string_length != string->length)
            {
                return GX_INVALID_STRING_LENGTH;
            }
        }
    }

    status = _gx_string_scroll_wheel_create_ext(wheel, name, parent, total_rows, string_list, style, Id, size);
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_string_scroll_wheel_event_process                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in string scroll wheel event        */
/*    process call.                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Text scroll wheel control     */
/*                                            block                       */
/*    event_ptr                             Event to be processed         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_string_scroll_wheel_event_process Actual string scroll wheel    */
/*                                            event process call          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_string_scroll_wheel_event_process(GX_STRING_SCROLL_WHEEL *wheel, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if ((wheel == GX_NULL) || (event_ptr == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget.  */
    if (wheel->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    status = _gx_string_scroll_wheel_event_process(wheel, event_ptr);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_string_scroll_wheel_string_id_list_set                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in string scroll wheel string id    */
/*    list set call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    string_id_list                        String id list to be set      */
/*    id_count                              The number of string ids      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_string_scroll_wheel_string_id_list_set                          */
/*                                          Actual string scroll wheel    */
/*                                            string id set call          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_string_scroll_wheel_string_id_list_set(GX_STRING_SCROLL_WHEEL *wheel,
                                                  const GX_RESOURCE_ID *string_id_list,
                                                  INT id_count)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if ((wheel == GX_NULL) || (string_id_list == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget. */
    if (wheel->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid value. */
    if (id_count <= 0)
    {
        return(GX_INVALID_VALUE);
    }

    status = _gx_string_scroll_wheel_string_id_list_set(wheel, string_id_list, id_count);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_string_scroll_wheel_string_list_set_ext                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in string scroll wheel string list  */
/*    set call.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    string_list                           String list to be set         */
/*    string_count                          The number of assigned strings*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_string_scroll_wheel_string_list_set_ext                         */
/*                                          Actual string scroll wheel    */
/*                                            string list set ext call    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_string_scroll_wheel_string_list_set_ext(GX_STRING_SCROLL_WHEEL *wheel,
                                                  const GX_STRING *string_list,
                                                  INT string_count)
{
UINT                status;
INT                 string_id;
const GX_STRING *string;
UINT                string_length;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if (wheel == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget. */
    if (wheel->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Check for invalid list size. */
    if (string_count < 0)
    {
        return GX_INVALID_VALUE;
    }

    if (string_list)
    {
        /* Test string length. */
        for (string_id = 0; string_id < string_count; string_id++)
        {
            string = &string_list[string_id];
            if (string->ptr)
            {
                status = _gx_utility_string_length_check(string->ptr, &string_length, string->length);

                if (status != GX_SUCCESS)
                {
                    return status;
                }
            }
            else
            {
                string_length = 0;
            }

            if (string_length != string->length)
            {
                return GX_INVALID_STRING_LENGTH;
            }
        }
    }

    status = _gx_string_scroll_wheel_string_list_set_ext(wheel, string_list, string_count);

    return status;
}
