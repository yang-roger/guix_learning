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
/**   Numeric Prompt Management (Prompt)                                  */
/**                                                                       */
/**************************************************************************/

#include "gx_numeric_prompt.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_numeric_prompt_create                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in numeric prompt create function.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Numeric prompt control block  */
/*    name                                  Name of numeric prompt        */
/*    parent                                Parent widget control block   */
/*    text_id                               Resource string id            */
/*    style                                 Style of numeric prompt       */
/*    prompt_id                             Application-defined ID of     */
/*                                            Numeric prompt.             */
/*    size                                  Numeric prompt size           */
/*    control_block_size                    The size of the widget control*/
/*                                            structure in bytes          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_numeric_prompt_create              Actual numeric prompt create  */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_numeric_prompt_create(GX_NUMERIC_PROMPT *prompt, const GX_CHAR *name, GX_WIDGET *parent,
                                GX_RESOURCE_ID text_id, ULONG style, USHORT prompt_id, const GX_RECTANGLE *size,
                                UINT control_block_size)
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

    if (control_block_size != sizeof(GX_NUMERIC_PROMPT))
    {
        return(GX_INVALID_SIZE);
    }

    /* Call actual number prompt create. */
    status = _gx_numeric_prompt_create(prompt, name, parent, text_id, style, prompt_id, size);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_numeric_prompt_format_function_set                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in numeric prompt format function set.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Numeric prompt control block  */
/*    format_func                           Format function to set        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_numeric_prompt_format_function_set Actual numeric prompt format  */
/*                                            function set call           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_numeric_prompt_format_function_set(GX_NUMERIC_PROMPT *prompt, void (*format_func)(GX_NUMERIC_PROMPT *, INT))
{

UINT status;

    /* Check error for valid pointer. */
    if ((prompt == GX_NULL) || (format_func == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual number prompt create. */
    status = _gx_numeric_prompt_format_function_set(prompt, format_func);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_numeric_prompt_value set                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in numeric prompt value set function.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Numeric prompt control block  */
/*    value                                 Prompt value to be set        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_numeric_prompt_value_set           Actual numeric prompt value   */
/*                                           set call                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_numeric_prompt_value_set(GX_NUMERIC_PROMPT *prompt, INT value)
{

UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if (prompt == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual number prompt value set. */
    status = _gx_numeric_prompt_value_set(prompt, value);

    return(status);
}
