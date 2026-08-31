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
/**   Multi Line Text Input Management (Multi Line Text Input)            */
/**                                                                       */
/**************************************************************************/

#include "gx_multi_line_text_input.h"

#include "gx_utility.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_backspace                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input        */
/*    backspace call.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input         */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_backspace                                 */
/*                                          Actual multi line text input  */
/*                                            backspace call              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_backspace(GX_MULTI_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers. */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget check. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual multi line text input backspace function.  */
    status = _gx_multi_line_text_input_backspace(text_input);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_buffer_clear                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input buffer */
/*      clear call.                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input_ptr                        Multi Line input control block*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_buffer_clear                              */
/*                                          Actual multi line text input  */
/*                                            buffer clear call           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_multi_line_text_input_buffer_clear(GX_MULTI_LINE_TEXT_INPUT *text_input_ptr)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input_ptr == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual multi line text input buffer clear function.  */
    status = _gx_multi_line_text_input_buffer_clear(text_input_ptr);
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_buffer_get                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input        */
/*      buffer get call.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input_ptr                        multi line text input         */
/*                                            control blcok               */
/*    buffer_address                        The address of the input      */
/*                                            buffer                      */
/*    content_size                          The count of the input data   */
/*    buffer_size                           The size of the input buffer. */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_buffer_get                                */
/*                                          Actual multi line text input  */
/*                                            buffer get call             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_multi_line_text_input_buffer_get(GX_MULTI_LINE_TEXT_INPUT *text_input_ptr, GX_CHAR **buffer_address,
                                           UINT *content_size, UINT *buffer_size)
{
UINT status;

    /* Check for invalid input pointers.  */
    if (text_input_ptr == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual single line text input buffer clear function.  */
    status = _gx_multi_line_text_input_buffer_get(text_input_ptr, buffer_address, content_size, buffer_size);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_char_insert_ext                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input        */
/*    char insert call.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input widget  */
/*                                            control block               */
/*    str                                   Utf8 string to be inserted    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_char_insert_ext                           */
/*                                          Actual multi line text input  */
/*                                            char insert ext call        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_char_insert_ext(GX_MULTI_LINE_TEXT_INPUT *text_input, const GX_STRING *str)
{
UINT status;
UINT string_length = 0;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers. */
    if ((text_input == GX_NULL) || (str == GX_NULL) || (str->ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_utility_string_length_check(str->ptr, &string_length, str->length);

    if (status != GX_SUCCESS)
    {
        return status;
    }

    /* Check for invalid string length. */
    if (string_length != str->length)
    {
        return GX_INVALID_STRING_LENGTH;
    }

    /* Check for invalid widget check. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual multi line text input char insert function.  */
    status = _gx_multi_line_text_input_char_insert_ext(text_input, str);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_create                                   */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input        */
/*  create.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input_ptr                        Multi Line input control block*/
/*    nampe_ptr                             Wigdget name                  */
/*    parent                                Parent widget control block   */
/*    size                                  Parent widget control block   */
/*    input_buffer                          User-supplied input buffer    */
/*    buffer_size                           Size of the user-supplied     */
/*                                            input buffer                */
/*    style                                 The style of the widget border*/
/*    Id                                    The ID number of the widget   */
/*    text_input_control_block_size         Size of the multi line text   */
/*                                            input control block         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_create                                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_multi_line_text_input_create(GX_MULTI_LINE_TEXT_INPUT *text_input_ptr,
                                        const GX_CHAR *name_ptr,
                                        GX_WIDGET *parent,
                                        GX_CHAR *input_buffer,
                                        UINT buffer_size,
                                        ULONG style,
                                        USHORT Id,
                                        const GX_RECTANGLE *size,
                                        UINT text_input_control_block_size)
{
UINT status;

    /* Check for invalid caller of this function.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((text_input_ptr == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid control block size. */
    if (text_input_control_block_size != sizeof(GX_MULTI_LINE_TEXT_INPUT))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (text_input_ptr->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid parent widget.  */
    if (parent && (parent->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual multi line text input create function.  */
    status = _gx_multi_line_text_input_create(text_input_ptr, name_ptr, parent, input_buffer, buffer_size, style, Id, size);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_cursor_pos_get                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input cursor */
/*    pos get function call.                                              */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Multi line text input widget  */
/*                                            control block               */
/*    cursor_pos                            Retrieved cursor position     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_cursor_pos_get                            */
/*                                          Actual multi line text input  */
/*                                            cursor pos get function     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_multi_line_text_input_cursor_pos_get(GX_MULTI_LINE_TEXT_INPUT *input, GX_POINT *cursor_pos)
{
UINT status;

    /* Check for invalid pointer. */
    if ((input == GX_NULL) || (cursor_pos == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    status = _gx_multi_line_text_input_cursor_pos_get(input, cursor_pos);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_multi_line_text_input_delete                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input        */
/*    delete call.                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input widget  */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_delete                                    */
/*                                          Actual multi line text input  */
/*                                            delete call                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_delete(GX_MULTI_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers. */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget check. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual multi line text input delete function.  */
    status = _gx_multi_line_text_input_delete(text_input);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_down_arrow                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input        */
/*    down arrow call.                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input widget  */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_down_arrow                                */
/*                                          Actual multi line text input  */
/*                                            down arrow call             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_down_arrow(GX_MULTI_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers. */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget check. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual multi line text input down arrow function.  */
    status = _gx_multi_line_text_input_down_arrow(text_input);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_end                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input end    */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input widget  */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_end                                       */
/*                                          Actual multi line text input  */
/*                                            end call                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_end(GX_MULTI_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers. */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget check. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual multi line text input end function.  */
    status = _gx_multi_line_text_input_end(text_input);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_event_process                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input event  */
/*    process function                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Multi-line text input widget  */
/*                                            control block               */
/*    event_ptr                             Point to event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_event_process                             */
/*                                          The actual multi-line text    */
/*                                            input event process function*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_multi_line_text_input_event_process(GX_MULTI_LINE_TEXT_INPUT *input, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((input == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual multi line text input event process function. */
    status = _gx_multi_line_text_input_event_process(input, event_ptr);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_fill_color_set                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input fill   */
/*    color set function call.                                            */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Pointer to multi line text    */
/*                                            view control block          */
/*    normal_fill_color_id                  Resource ID of the normal     */
/*                                             fill color                 */
/*    selected_fill_color_id                Resource ID of the selected   */
/*                                             fill color                 */
/*    disabled_fill_color_id                Resource ID of the disabled   */
/*                                             fill color                 */
/*    readonly_fill_color_id                Resource ID of the read only  */
/*                                             fill color                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_fill_color_set                            */
/*                                          Actual multi line text input  */
/*                                            fill color set function     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_multi_line_text_input_fill_color_set(GX_MULTI_LINE_TEXT_INPUT *input,
                                                GX_RESOURCE_ID normal_fill_color_id,
                                                GX_RESOURCE_ID selected_fill_color_id,
                                                GX_RESOURCE_ID disabled_fill_color_id,
                                                GX_RESOURCE_ID readonly_fill_color_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!input)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_multi_line_text_input_fill_color_set(input,
                                                      normal_fill_color_id,
                                                      selected_fill_color_id,
                                                      disabled_fill_color_id,
                                                      readonly_fill_color_id);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_home                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input home   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input widget  */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_home                                      */
/*                                          Actual multi line text input  */
/*                                            home call                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_home(GX_MULTI_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers. */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget check. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual multi line text input home function.  */
    status = _gx_multi_line_text_input_home(text_input);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_left                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input left   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input widget  */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_left                                      */
/*                                          Actual multi line text input  */
/*                                            left call                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_left_arrow(GX_MULTI_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers. */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget check. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual multi line text input left function.  */
    status = _gx_multi_line_text_input_left_arrow(text_input);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_right_arrow                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input right  */
/*    arrow call.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input widget  */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_right_arrow                               */
/*                                          Actual multi line text input  */
/*                                            right arrow call            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_right_arrow(GX_MULTI_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers. */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget check. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input right arrow function.  */
    status = _gx_multi_line_text_input_right_arrow(text_input);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_style_set                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input style  */
/*    add function call.                                                  */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input_ptr                        Pointer to multi line text    */
/*                                            input control block         */
/*    style                                 New style to add              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_style_add                                 */
/*                                          Actual multi line text input  */
/*                                            style add fucntion          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_style_add(GX_MULTI_LINE_TEXT_INPUT *text_input_ptr, ULONG style)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!text_input_ptr)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_multi_line_text_input_style_add(text_input_ptr, style);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_style_remove                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input style  */
/*    remove function call.                                               */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input_ptr                        Pointer to multi line text    */
/*                                            input control block         */
/*    cursor_type                           Cursor type flag to clear     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_style_remove                              */
/*                                          Actual multi line text input  */
/*                                            style remove function       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_style_remove(GX_MULTI_LINE_TEXT_INPUT *text_input_ptr, ULONG style)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!text_input_ptr)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_multi_line_text_input_style_remove(text_input_ptr, style);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_style_set                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input style  */
/*    set function call.                                                  */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input_ptr                        Pointer to multi line text    */
/*                                            input control block         */
/*    style                                 New style                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_style_set                                 */
/*                                          Actual multi line text input  */
/*                                            style set fucntion          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_style_set(GX_MULTI_LINE_TEXT_INPUT *text_input_ptr, ULONG style)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!text_input_ptr)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_multi_line_text_input_style_set(text_input_ptr, style);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_text_color_set                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input text   */
/*    color set function call.                                            */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Pointer to multi line text    */
/*                                            input control block         */
/*    normal_text_color_id                  Resource ID of the normal     */
/*                                             text color                 */
/*    selected_text_color_id                Resource ID of the selected   */
/*                                             text color                 */
/*    disabled_text_color_id                Resource ID of the disabled   */
/*                                             text color                 */
/*    readonly_text_color_id                Resource ID of the read only  */
/*                                             text color                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_text_color_set                            */
/*                                          Actual multi line text input  */
/*                                            text color set function     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_multi_line_text_input_text_color_set(GX_MULTI_LINE_TEXT_INPUT *input,
                                                GX_RESOURCE_ID normal_text_color_id,
                                                GX_RESOURCE_ID selected_text_color_id,
                                                GX_RESOURCE_ID disabled_text_color_id,
                                                GX_RESOURCE_ID readonly_text_color_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!input)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_multi_line_text_input_text_color_set(input,
                                                      normal_text_color_id,
                                                      selected_text_color_id,
                                                      disabled_text_color_id,
                                                      readonly_text_color_id);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_text_select                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input text   */
/*    select function call.                                               */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Multi-line text input widget  */
/*                                            control block               */
/*    start_mark                            The index of the first        */
/*                                            highlight character.        */
/*    end_mark                              The index of the last         */
/*                                            highlight character         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_text_select Actual multi line text input  */
/*                                            text select fucntion        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_text_select(GX_MULTI_LINE_TEXT_INPUT *input, UINT start_index, UINT end_index)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!input)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid index value. */
    if ((start_index >= input->text_().length) ||
        (end_index >= input->text_().length))
    {
        return(GX_INVALID_VALUE);
    }

    status = _gx_multi_line_text_input_text_select(input, start_index, end_index);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_text_set_ext                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input text   */
/*    set ext function call.                                              */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Pointer to multi line text    */
/*                                            input control block         */
/*    text                                  GX_STRING type text string    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_text_set_ext                              */
/*                                          Actual multi line text input  */
/*                                            text set fucntion           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_text_set_ext(GX_MULTI_LINE_TEXT_INPUT *input, const GX_STRING *text)
{
UINT status;
UINT text_length = 0;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!input)
    {
        return(GX_PTR_ERROR);
    }

    if (text)
    {
        if (text->ptr)
        {
            /* Calculate string length. */
            status = _gx_utility_string_length_check(text->ptr, &text_length, text->length);

            if (status != GX_SUCCESS)
            {
                return status;
            }
        }

        /* Validate string length. */
        if (text_length != text->length)
        {
            return GX_INVALID_STRING_LENGTH;
        }
    }

    status = _gx_multi_line_text_input_text_set_ext(input, text);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_input_up                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text input up     */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Multi line text input widget  */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_input_up                                        */
/*                                          Actual multi line text input  */
/*                                            up call                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_input_up_arrow(GX_MULTI_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers. */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget check. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual multi line text input up function.  */
    status = _gx_multi_line_text_input_up_arrow(text_input);

    return status;
}
