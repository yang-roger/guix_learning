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
/**   Multi Line Text View Management (Multi Line Text)                   */
/**                                                                       */
/**************************************************************************/

#include "gx_multi_line_text_view.h"

#include "gx_utility.h"

GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_view_create                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text view create  */
/*    function.                                                           */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view_ptr                         Multi Line view control block */
/*    name                                  Wigdget name                  */
/*    parent                                Parent widget control block   */
/*    input_buffer                          User-supplied input buffer    */
/*    style                                 The style of the widget border*/
/*    text_view_id                          The ID number of the widget   */
/*    size                                  Parent widget control block   */
/*    text_view_control_block_size          Size of the multi line text   */
/*                                            view control_block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_create       Actual multi line text view   */
/*                                            create function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT    _gxe_multi_line_text_view_create(GX_MULTI_LINE_TEXT_VIEW *text_view_ptr,
                                         const GX_CHAR *name_ptr,
                                         GX_WIDGET *parent,
                                         GX_RESOURCE_ID text_id,
                                         ULONG style,
                                         USHORT Id,
                                         const GX_RECTANGLE *size,
                                         UINT text_view_control_block_size)
{
UINT status;

    /* Check for invalid caller of this function.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((text_view_ptr == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (text_view_control_block_size != sizeof(GX_MULTI_LINE_TEXT_VIEW))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (text_view_ptr->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Call actual multi line text input create function.  */
    status = _gx_multi_line_text_view_create(text_view_ptr, name_ptr, parent, text_id, style, Id, size);

    /* Return the error status from window create.  */
    return(status);
}

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_view_event_process                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text view event   */
/*    process function                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Multi-line text view widget   */
/*                                            control block               */
/*    event_ptr                             Point to event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_event_process                              */
/*                                          The actual multi-line text    */
/*                                            view event process function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_multi_line_text_view_event_process(GX_MULTI_LINE_TEXT_VIEW *view, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((view == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (view->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual multi line text view event process function. */
    status = _gx_multi_line_text_view_event_process(view, event_ptr);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_view_font_set                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text view font    */
/*    set function call.                                                  */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    view                                  Pointer to multi line text    */
/*                                            view control block          */
/*    font_id                               Resource ID of font           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_font_set     Actual multi line text view   */
/*                                            font set function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_multi_line_text_view_font_set(GX_MULTI_LINE_TEXT_VIEW *view, GX_RESOURCE_ID font_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!view)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_multi_line_text_view_font_set(view, font_id);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_view_line_space_set                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text view         */
/*    line space set function call.                                       */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    view                                  Pointer to multi line text    */
/*                                            view control block          */
/*    line space                            Value to be set               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_line_space_set                             */
/*                                          Actual multi line text view   */
/*                                            line space set fucntion     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_view_line_space_set(GX_MULTI_LINE_TEXT_VIEW *view, GX_BYTE line_space)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!view)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_multi_line_text_view_line_space_set(view, line_space);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_view_scroll_info_get                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text view scroll  */
/*    info get function call.                                             */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view                            Multi line text view widget    */
/*                                           control block                */
/*    style                                 Style                         */
/*    return_scroll_info                   Pointer to destination for     */
/*                                           scroll info                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_text_set     Actual multi line text view   */
/*                                            text set fucntion           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_view_scroll_info_get(GX_MULTI_LINE_TEXT_VIEW *text_view, ULONG style, GX_SCROLL_INFO *return_scroll_info)
{
UINT status;

    if ((text_view == GX_NULL) || (return_scroll_info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_multi_line_text_view_scroll_info_get(text_view, style, return_scroll_info);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_view_text_color_set                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text view text    */
/*    color set function call.                                            */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    view                                  Pointer to multi line text    */
/*                                            view control block          */
/*    normal_text_color_id                  Resource ID of the normal     */
/*                                             text color                 */
/*    selected_text_color_id                Resource ID of the selected   */
/*                                             text color                 */
/*    disabled_text_color_id                Resource ID of the disabled   */
/*                                             text color                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_text_color_set                             */
/*                                          Actual multi line text view   */
/*                                            text color set function     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_multi_line_text_view_text_color_set(GX_MULTI_LINE_TEXT_VIEW *view,
                                               GX_RESOURCE_ID normal_text_color_id,
                                               GX_RESOURCE_ID selected_text_color_id,
                                               GX_RESOURCE_ID disabled_text_color_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!view)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_multi_line_text_view_text_color_set(view, normal_text_color_id, selected_text_color_id, disabled_text_color_id);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_view_text_id_set                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text view text    */
/*    id set function call.                                               */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_view_ptr                         Pointer to multi line text    */
/*                                            view control block          */
/*    text_id                               Resource ID for the text      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_text_id_set  Actual multi line text view   */
/*                                            text id set fucntion        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_view_text_id_set(GX_MULTI_LINE_TEXT_VIEW *text_view_ptr,
                                           GX_RESOURCE_ID text_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!text_view_ptr)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_multi_line_text_view_text_id_set(text_view_ptr, text_id);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_view_text_set_ext                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text view text    */
/*    set ext function call.                                              */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    view                                  Pointer to multi line text    */
/*                                            view control block          */
/*    text                                  Null-terminated text string   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_text_set_ext Actual multi line text view   */
/*                                            text set ext fucntion       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_view_text_set_ext(GX_MULTI_LINE_TEXT_VIEW *view,
                                            const GX_STRING *text)
{
UINT status = GX_SUCCESS;
UINT text_length = 0;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!view)
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

    status = _gx_multi_line_text_view_text_set_ext(view, text);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_multi_line_text_view_whitespace_set                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the multi line text view         */
/*    whitespace set function call.                                       */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    view                                  Pointer to multi line text    */
/*                                            view control block          */
/*    whitespace                            Value to be set               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_multi_line_text_view_whitespace_set                             */
/*                                          Actual multi line text view   */
/*                                            whitespace set fucntion     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_multi_line_text_view_whitespace_set(GX_MULTI_LINE_TEXT_VIEW *view, GX_UBYTE whitespace)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!view)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_multi_line_text_view_whitespace_set(view, whitespace);

    return(status);
}
