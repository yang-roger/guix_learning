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

#include "gx_prompt.h"

#include "gx_system.h"
#include "gx_utility.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_prompt_create                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the prompt create function.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Prompt control block          */
/*    name                                  Name of prompt                */
/*    parent                                Parent widget control block   */
/*    text_id                               Resource string id            */
/*    style                                 Style of prompt               */
/*    prompt_id                             Application-defined ID of     */
/*                                            prompt.                     */
/*    size                                  Prompt size                   */
/*    prompt_control_block_size             Size of the prompt control    */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_prompt_create                     the actual function            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_prompt_create(GX_PROMPT *prompt, const GX_CHAR *name, GX_WIDGET *parent,
                         GX_RESOURCE_ID text_id, ULONG style, USHORT prompt_id,
                         const GX_RECTANGLE *size, UINT prompt_control_block_size)
{

UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if ((prompt == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for widget already created.  */
    if (prompt->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid control block size. */
    if (prompt_control_block_size != sizeof(GX_PROMPT))
    {
        return(GX_INVALID_SIZE);
    }

    status = _gx_prompt_create(prompt, name, parent, text_id, style, prompt_id, size);
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_prompt_event_process                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the prompt event process         */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Pointer to prompt control     */
/*                                            block                       */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_prompt_event_process              Actual prompt event process   */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_prompt_event_process(GX_PROMPT *prompt, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((prompt == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (prompt->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual text button event process function.  */
    status = _gx_prompt_event_process(prompt, event_ptr);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_prompt_font_set                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks errors in the prompt font set function.         */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Pointer to prompt widget      */
/*                                            control block               */
/*    font_id                               Resource ID of font           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_prompt_font_set                   The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_prompt_font_set(GX_PROMPT *prompt, GX_RESOURCE_ID font_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (prompt == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget widget size.  */
    if (prompt->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual slider draw function.  */
    status = _gx_prompt_font_set(prompt, font_id);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_prompt_text_color_set                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks errors in the prompt text color get function.   */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Pointer to prompt control     */
/*                                             block                      */
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
/*    _gx_prompt_text_color_set             The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_prompt_text_color_set(GX_PROMPT *prompt,
                                 GX_RESOURCE_ID normal_text_color_id,
                                 GX_RESOURCE_ID selected_text_color_id,
                                 GX_RESOURCE_ID disabled_text_color_id)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if (prompt == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget size.  */
    if (prompt->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual function.  */
    status = _gx_prompt_text_color_set(prompt, normal_text_color_id, selected_text_color_id, disabled_text_color_id);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_prompt_text_get_ext                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the prompt text get function.        */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                          Pointer to prompt widget            */
/*                                      control block                     */
/*    return_text                     Pointer to destination for text     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                          Completion status                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_prompt_text_get_ext         The actual function                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_prompt_text_get_ext(GX_PROMPT *prompt, GX_STRING *return_text)
{
UINT status;

    /* Check error for valid pointer. */
    if (prompt == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (prompt->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual function.  */
    status = _gx_prompt_text_get_ext(prompt, return_text);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_prompt_text_id_set                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the orinot text id set function  */
/*    call.                                                               */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Prompt control block          */
/*    string_id                             Resource ID of the string     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_prompt_text_id_set                Actual prompt text id set     */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Cdoe                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_prompt_text_id_set(GX_PROMPT *prompt, GX_RESOURCE_ID string_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!prompt)
    {
        return(GX_PTR_ERROR);
    }

    if (prompt->string_().ptr && (prompt->style & GX_STYLE_TEXT_COPY))
    {
        if (_gx_system_memory_free == GX_NULL)
        {
            return GX_SYSTEM_MEMORY_ERROR;
        }
    }

    /* Call the actual function.  */
    status = _gx_prompt_text_id_set(prompt, string_id);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_prompt_text_set_ext                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the prompt text set function.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Pointer to prompt widget      */
/*                                            control block               */
/*    text                                  Pointer to text               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_prompt_text_set_ext               The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_prompt_text_set_ext(GX_PROMPT *prompt, const GX_STRING *text)
{
UINT status;
UINT text_length = 0;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if (prompt == GX_NULL)
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

    /* Call the actual function.  */
    status = _gx_prompt_text_set_ext(prompt, text);

    /* Return completion status.  */
    return(status);
}
