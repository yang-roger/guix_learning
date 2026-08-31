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

static void gx_prompt_draw_(GX_PROMPT* prompt)
{
    prompt->draw_();
}

static UINT gx_prompt_event_process_(GX_PROMPT* prompt, GX_EVENT* event_ptr)
{
    return prompt->event_process_(event_ptr);
}

static UINT gx_prompt_text_get_ext_(GX_PROMPT* prompt, GX_STRING* return_string)
{
    return prompt->text_get_ext_(return_string);
}

UINT GX_PROMPT::create_(const GX_CHAR* name, GX_WIDGET* parent,
                        GX_RESOURCE_ID text_id, ULONG style, USHORT prompt_id,
                        const GX_RECTANGLE* size)
{
    GX_WIDGET::create_(name, GX_NULL, style, prompt_id, size);

    this->type = GX_TYPE_PROMPT;

    this->text_data.create_(this, text_id, GX_FONT_ID_PROMPT);

    this->draw_function = (void (*)(GX_WIDGET*))gx_prompt_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_prompt_event_process_;
    this->text_get_function = gx_prompt_text_get_ext_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_PROMPT::text_draw_(INT x_offset, INT y_offset)
{
    GX_RESOURCE_ID color_id = text_color_();
    text_draw_(color_id, x_offset, y_offset);
}

void GX_PROMPT::text_draw_(GX_RESOURCE_ID color_id, INT x_offset, INT y_offset)
{
    GX_STRING text = { GX_NULL, 0 };
    text_get_function(this, &text);

    text_draw_ext_(color_id, font_id_(), &text, x_offset, y_offset);
}

void GX_PROMPT::draw_()
{
    border_and_background_draw_();
    text_draw_();
    children_draw_();
}

UINT GX_PROMPT::event_process_(GX_EVENT* event_ptr)
{
    switch (event_ptr->type)
    {
    case GX_EVENT_DELETE:
        text_data.string_delete_();
        break;

    default:
        break;
    }

    return GX_WIDGET::event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_prompt_create                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a prompt, which is a special type of          */
/*    widget.                                                             */
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
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_create                     Create the underlying widget  */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_prompt_create(GX_PROMPT* prompt, const GX_CHAR* name, GX_WIDGET* parent,
                       GX_RESOURCE_ID text_id, ULONG style, USHORT prompt_id,
                       const GX_RECTANGLE* size)
{
    return prompt->create_(name, parent, text_id, style, prompt_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_prompt_draw                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws a prompt widget.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Pointer to prompt widget      */
/*                                           control block                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_children_draw              Draw children widgets         */
/*    _gx_widget_background_draw            Draw widget background         */
/*    _gx_prompt_text_draw                  Draw prompt text              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_prompt_draw(GX_PROMPT* prompt)
{
    prompt->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_prompt_event_process                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes events for the specified prompt.            */
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
/*    _gx_system_memory_free                Release memory                */
/*    _gx_utility_bidi_resolved_text_info_delete                          */
/*                                          Delete dynamic bidi text      */
/*    _gx_widget_event_process              Default widget event process  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_prompt_event_process(GX_PROMPT* prompt, GX_EVENT* event_ptr)
{
    return prompt->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_prompt_font_set                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the font of a prompt widget.                      */
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
/*    _gx_system_dirty_mark                 Mark this prompt as dirty     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_prompt_font_set(GX_PROMPT* prompt, GX_RESOURCE_ID font_id)
{
    prompt->font_set_(font_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_prompt_text_color_set                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the text color of a prompt widget.                */
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
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark this prompt as dirty     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_prompt_text_color_set(GX_PROMPT* prompt,
                               GX_RESOURCE_ID normal_text_color_id,
                               GX_RESOURCE_ID selected_text_color_id,
                               GX_RESOURCE_ID disabled_text_color_id)
{
    prompt->text_color_set_(normal_text_color_id,
                            selected_text_color_id,
                            disabled_text_color_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_prompt_text_draw                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified prompt with text.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Prompt control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_prompt_text_get_function]         Prompt-specified text get     */
/*                                            function                    */
/*    _gx_widget_text_draw                  Draw the text on the widget   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_prompt_text_draw(GX_PROMPT* prompt)
{
    prompt->text_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_prompt_text_get_ext                                             */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the text of a prompt widget.                      */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Pointer to prompt widget      */
/*                                            control block               */
/*    return_text                           Pointer to destination for    */
/*                                            the text                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_string_get_ext             Obtain the string             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_prompt_text_get_ext(GX_PROMPT* prompt, GX_STRING* return_string)
{
    return prompt->text_get_ext_(return_string);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_prompt_text_id_set                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the text of a prompt widget.                      */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Pointer to prompt widget      */
/*                                            control block               */
/*    string_id                             Resource ID of the string     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark this prompt as dirty     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_prompt_text_id_set(GX_PROMPT* prompt, GX_RESOURCE_ID string_id)
{
    prompt->text_id_set_(string_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_prompt_text_set_ext                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the text of a prompt widget.                      */
/*                                                                        */
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
/*    _gx_system_dirty_mark                 Mark this prompt as dirty     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_prompt_text_set_ext(GX_PROMPT* prompt, const GX_STRING* string)
{
    return prompt->text_set_ext_(string);
}
