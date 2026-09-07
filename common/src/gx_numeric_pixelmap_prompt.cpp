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
/**   Numeric Pixelmap Prompt Management (Prompt)                         */
/**                                                                       */
/**************************************************************************/

#include "gx_numeric_pixelmap_prompt.h"

#include "gx_utility.h"

static void gx_numeric_pixelmap_prompt_format_(GX_NUMERIC_PIXELMAP_PROMPT* prompt, INT value)
{
    prompt->format_(value);
}

UINT GX_NUMERIC_PIXELMAP_PROMPT::create_(const GX_CHAR* name, GX_WIDGET* parent,
                                         GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                                         ULONG style, USHORT pixelmap_prompt_id,
                                         const GX_RECTANGLE* size)
{
    GX_PIXELMAP_PROMPT::create_(name, GX_NULL, text_id, fill_id, style, pixelmap_prompt_id, size);

    this->type = GX_TYPE_NUMERIC_PIXELMAP_PROMPT;

    this->format_function = gx_numeric_pixelmap_prompt_format_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_NUMERIC_PIXELMAP_PROMPT::format_(INT value)
{
    _gx_utility_ltoa(value, buffer, GX_NUMERIC_PROMPT_BUFFER_SIZE);
}

UINT GX_NUMERIC_PIXELMAP_PROMPT::value_set_(INT value)
{
    format_function(this, value);

    return text_data.string_set_(buffer, GX_NUMERIC_PROMPT_BUFFER_SIZE - 1);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_numeric_pixelmap_prompt_create                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a numeric pixelmap prompt, which is a special */
/*    type of widget.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Numeric Pixelmap Prompt       */
/*                                            control block               */
/*    name                                  Name of prompt                */
/*    parent                                Parent widget control block   */
/*    text_id                               Resource string id            */
/*    fill_id                               Pixelmap id for fill area     */
/*    style                                 Style of pixelmap prompt      */
/*    pixelmap_prompt_id                    Application-defined ID of     */
/*                                            numeric pixelmap prompt     */
/*    size                                  Dimemsions of numeric pixelmap*/
/*                                            prompt                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_pixelmap_prompt_create            Create a pixelmap prompt      */
/*    _gx_widget_link                       Link a widget to its parent   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_numeric_pixelmap_prompt_create(GX_NUMERIC_PIXELMAP_PROMPT* prompt,
                                        const GX_CHAR* name, GX_WIDGET* parent,
                                        GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                                        ULONG style, USHORT pixelmap_prompt_id, const GX_RECTANGLE* size)
{
    return prompt->create_(name, parent, text_id, fill_id, style, pixelmap_prompt_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_numeric_pixelmap_prompt_format_function_set                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets a new format function for a numeric pixelmap     */
/*    prompt widget.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Numeric pixelmap prompt       */
/*                                            control block               */
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
UINT _gx_numeric_pixelmap_prompt_format_function_set(GX_NUMERIC_PIXELMAP_PROMPT* prompt, void (*format_func)(GX_NUMERIC_PIXELMAP_PROMPT*, INT))
{
    prompt->format_function = format_func;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_numeric_pixelmap_prompt_value_set                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets an integer for a numeric pixelmap prompt widget. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Numeric pixelmap prompt       */
/*                                            control block               */
/*    value                                 Prompt value to be set        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_numeric_pixelmap_prompt_format_function]                        */
/*                                          Value format callback         */
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_numeric_pixelmap_prompt_value_set(GX_NUMERIC_PIXELMAP_PROMPT* prompt, INT value)
{
    return prompt->value_set_(value);
}
