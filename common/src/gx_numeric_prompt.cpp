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

#include "gx_utility.h"

static void gx_numeric_prompt_format_(GX_NUMERIC_PROMPT* prompt, INT value)
{
    prompt->format_(value);
}

UINT GX_NUMERIC_PROMPT::create_(const GX_CHAR *name, GX_WIDGET *parent,
                                GX_RESOURCE_ID text_id, ULONG style,
                                USHORT prompt_id, const GX_RECTANGLE *size)
{
    GX_PROMPT::create_(name, GX_NULL, text_id, style, prompt_id, size);

    this->type = GX_TYPE_NUMERIC_PROMPT;

    this->format_function = gx_numeric_prompt_format_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_NUMERIC_PROMPT::format_(INT value)
{
    _gx_utility_ltoa(value, buffer, GX_NUMERIC_PROMPT_BUFFER_SIZE);
}

UINT GX_NUMERIC_PROMPT::value_set_(INT value)
{
    format_function(this, value);

    return text_data.string_set_(buffer, GX_NUMERIC_PROMPT_BUFFER_SIZE - 1);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_numeric_prompt_create                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a numeric prompt, which is a special type of  */
/*    widget.                                                             */
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
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_prompt_create                     Create a prompt               */
/*    _gx_widget_link                       Link a widget to its parent   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_numeric_prompt_create(GX_NUMERIC_PROMPT* prompt, const GX_CHAR* name, GX_WIDGET* parent,
                               GX_RESOURCE_ID text_id, ULONG style, USHORT prompt_id, const GX_RECTANGLE* size)
{
    return prompt->create_(name, parent, text_id, style, prompt_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_numeric_prompt_format_function_set                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets a new format function for a numeric prompt       */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Numeric prompt control block  */
/*    format_func                           Format function to be set     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_numeric_prompt_format_function_set(GX_NUMERIC_PROMPT* prompt, void (*format_func)(GX_NUMERIC_PROMPT*, INT))
{
    prompt->format_function = format_func;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_numeric_prompt_value_set                                        */
/*                                                           6.2.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets an integer for a numeric prompt widget           */
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
/*    [gx_numeric_prompt_format_function]   Value format callback         */
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_numeric_prompt_value_set(GX_NUMERIC_PROMPT* prompt, INT value)
{
    return prompt->value_set_(value);
}


