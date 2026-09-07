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
/**   Text Input Management (Single Line Text Input)                      */
/**                                                                       */
/**************************************************************************/

#include "gx_single_line_text_input.h"

#include "gx_utility.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_backspace                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*    create call.                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control blcok               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_backspace                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_single_line_text_input_backspace(GX_SINGLE_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input backspace function.  */
    status = _gx_single_line_text_input_backspace(text_input);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_buffer_clear                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*      buffer clear call.                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gxe_single_line_text_input_buffer_clear                            */
/*                                          Actual single line text input */
/*                                            buffer clear call           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_single_line_text_input_buffer_clear(GX_SINGLE_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input buffer clear function.  */
    status = _gx_single_line_text_input_buffer_clear(text_input);
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_buffer_get                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*      buffer get call.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
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
/*    _gx_single_line_text_input_buffer_get                               */
/*                                          Actual single line text input */
/*                                            buffer get call             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_single_line_text_input_buffer_get(GX_SINGLE_LINE_TEXT_INPUT *text_input_ptr, GX_CHAR **buffer_address,
                                            UINT *content_size, UINT *buffer_size)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input_ptr == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (text_input_ptr->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input buffer get function.  */
    status = _gx_single_line_text_input_buffer_get(text_input_ptr, buffer_address, content_size, buffer_size);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_character_delete                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*    character delete call.                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_character_delete                         */
/*                                          Acutal call to the character  */
/*                                            delete function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_single_line_text_input_character_delete(GX_SINGLE_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (text_input->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call the actual text input character delete function.  */
    status = _gx_single_line_text_input_character_delete(text_input);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_character_insert                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*    character insert call.                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*    insert_str                            Byte string of insert         */
/*                                            character                   */
/*    insert_size                           String size of insert         */
/*                                            character                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_character_insert                         */
/*                                          Acutal call to the character  */
/*                                            insert function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_single_line_text_input_character_insert(GX_SINGLE_LINE_TEXT_INPUT *text_input, GX_UBYTE *insert_str, UINT insert_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (text_input->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call the actual text input character insert function.  */
    status = _gx_single_line_text_input_character_insert(text_input, insert_str, insert_size);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_create                                  */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*    create call.                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                        Single-line text input widget     */
/*                                        control block                   */
/*    name                              Name of text input widget         */
/*    parent                            Pointer to parent widget          */
/*    input_buffer                      Pointer to text input buffer      */
/*    buffer_size                       Size of text input buffer         */
/*    style                             Style of text input widget.       */
/*    text_input_id                     Application-defined ID for text   */
/*                                        input                           */
/*    size                              Dimensions of text input widget   */
/*    text_input_control_block_size     Size of the single line text      */
/*                                        input control block             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Completion status                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_create                                   */
/*                                      Actual single line text input     */
/*                                        widget create call              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT  _gxe_single_line_text_input_create(GX_SINGLE_LINE_TEXT_INPUT *text_input,
                                         const GX_CHAR *name,
                                         GX_WIDGET *parent,
                                         GX_CHAR *input_buffer,
                                         UINT buffer_size,
                                         UINT style,
                                         USHORT text_input_id,
                                         const GX_RECTANGLE *size,
                                         UINT text_input_control_block_size)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((text_input == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (text_input_control_block_size != sizeof(GX_SINGLE_LINE_TEXT_INPUT))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for already created. */
    if (text_input->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    if (parent && (parent->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input create function.  */
    status = _gx_single_line_text_input_create(text_input, name, parent, input_buffer, buffer_size, style, text_input_id, size);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_draw_position_get                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input draw  */
/*    position get call.                                                  */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Pointer to single line text   */
/*                                            input control block         */
/*    xpos                                  Retrieved x-coordinate of     */
/*                                            text draw start position    */
/*    ypos                                  Retrieved y-coordinate of     */
/*                                            text draw start position    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_draw_position_get                        */
/*                                          Actual single line text input */
/*                                            draw position get fucntion  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_single_line_text_input_draw_position_get(GX_SINGLE_LINE_TEXT_INPUT *input, GX_VALUE *xpos, GX_VALUE *ypos)
{
    UINT status;

    if ((!input) || (!xpos) || (!ypos))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    status = _gx_single_line_text_input_draw_position_get(input, xpos, ypos);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_end                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*    end call.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_end        Actual single line text input */
/*                                            end call                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_single_line_text_input_end(GX_SINGLE_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input end function.  */
    status = _gx_single_line_text_input_end(text_input);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_event_process                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*    event process call.                                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single line text input widget */
/*                                            control block               */
/*    event_ptr                             Pointer to GX_EVENT structure */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_event_process                            */
/*                                          Actual event process call     */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_single_line_text_input_event_process(GX_SINGLE_LINE_TEXT_INPUT *text_input, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((text_input == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input event process function.  */
    status = _gx_single_line_text_input_event_process(text_input, event_ptr);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_fill_color_set                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input fill  */
/*    color set function call.                                            */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Pointer to single line text   */
/*                                            input control block         */
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
/*    _gx_single_line_text_input_fill_color_set                           */
/*                                          Actual single line text input */
/*                                            fill color set function     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_single_line_text_input_fill_color_set(GX_SINGLE_LINE_TEXT_INPUT *input,
                                                 GX_RESOURCE_ID normal_fill_color_id,
                                                 GX_RESOURCE_ID selected_fill_color_id,
                                                 GX_RESOURCE_ID disabled_fill_color_id,
                                                 GX_RESOURCE_ID readonly_fill_color_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if (!input)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget type. */
    if (input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual text input fill color set. */
    status = _gx_single_line_text_input_fill_color_set(input,
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
/*    _gxe_single_line_text_input_home                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*    home call.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_home       Actual single line text input */
/*                                            home call.                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_single_line_text_input_home(GX_SINGLE_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input home function.  */
    status = _gx_single_line_text_input_home(text_input);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_left_arrow                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input left  */
/*    arrow call.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_left_arrow                               */
/*                                          Actual single line text input */
/*                                            left arrow call.            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_single_line_text_input_left_arrow(GX_SINGLE_LINE_TEXT_INPUT *text_input)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input left arrow function.  */
    status = _gx_single_line_text_input_left_arrow(text_input);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_position_get                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*    position get call.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*    pixel_position                        X value of pixel position     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_position_get                             */
/*                                          Actual position get call      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_single_line_text_input_position_get(GX_SINGLE_LINE_TEXT_INPUT *text_input, INT pixel_position)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input position get function.  */
    status = _gx_single_line_text_input_position_get(text_input, pixel_position);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_right_arrow                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*    right arrow call                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_right_arrow                              */
/*                                          Actual single line text input */
/*                                            right arrow call            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_single_line_text_input_right_arrow(GX_SINGLE_LINE_TEXT_INPUT *text_input)
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
    status = _gx_single_line_text_input_right_arrow(text_input);
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_style_add                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*     style add call.                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*    style                                 New style to add              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_style_add                                */
/*                                          Actual style add call         */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_single_line_text_input_style_add(GX_SINGLE_LINE_TEXT_INPUT *text_input, ULONG style)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input cursor style add function.  */
    status = _gx_single_line_text_input_style_add(text_input, style);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_style_remove                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*      style remove call.                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                           control block                */
/*    style                                 Styles to remove              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_style_remove                             */
/*                                         Actual style remove call       */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_single_line_text_input_style_remove(GX_SINGLE_LINE_TEXT_INPUT *text_input, ULONG style)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input cursor style remove function.  */
    status = _gx_single_line_text_input_style_remove(text_input, style);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_style_set                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input       */
/*      style set call.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
/*                                            control block               */
/*    style                                 Style flas to assign          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_style_set                                */
/*                                          Actual style set call         */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_single_line_text_input_style_set(GX_SINGLE_LINE_TEXT_INPUT *text_input, ULONG style)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (text_input == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (text_input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual single line text input style set function.  */
    status = _gx_single_line_text_input_style_set(text_input, style);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_text_color_set                          */
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
/*    input                                 Pointer to single line text   */
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
/*    _gx_single_line_text_input_text_color_set                           */
/*                                          Actual single line text input */
/*                                            text color set function     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_single_line_text_input_text_color_set(GX_SINGLE_LINE_TEXT_INPUT *input,
                                                 GX_RESOURCE_ID normal_text_color_id,
                                                 GX_RESOURCE_ID selected_text_color_id,
                                                 GX_RESOURCE_ID disabled_text_color_id,
                                                 GX_RESOURCE_ID readonly_text_color_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if (!input)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual text input color set. */
    status = _gx_single_line_text_input_text_color_set(input,
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
/*    _gxe_single_line_text_input_text_select                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function highlighs text with specified start mark and end mark */
/*    index.                                                              */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_input                            Single-line text input widget */
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
/*    _gxe_single_line_text_input_text_select                             */
/*                                          Actual single line text input */
/*                                            text select call            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_single_line_text_input_text_select(GX_SINGLE_LINE_TEXT_INPUT *input, UINT start_index, UINT end_index)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if (!input)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (input->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid value. */
    if ((start_index >= input->string_size) ||
        (end_index >= input->string_size))
    {
        return(GX_INVALID_VALUE);
    }

    /* Call actual text input color set. */
    status = _gx_single_line_text_input_text_select(input, start_index, end_index);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_single_line_text_input_text_set_ext                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the single line text input text  */
/*    set ext function call.                                              */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input                                 Pointer to single line text   */
/*                                            input control block         */
/*    text                                  GX_STRING type text string    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_single_line_text_input_text_set_ext                             */
/*                                          Actual multi line text input  */
/*                                            text set ext fucntion       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_single_line_text_input_text_set_ext(GX_SINGLE_LINE_TEXT_INPUT *input, const GX_STRING *text)
{
UINT status = GX_SUCCESS;
UINT text_length = 0;

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

    status = _gx_single_line_text_input_text_set_ext(input, text);

    return(status);
}
