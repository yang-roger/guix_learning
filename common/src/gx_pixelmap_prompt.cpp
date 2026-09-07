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
/**   Pixelmap Prompt Management (Prompt)                                 */
/**                                                                       */
/**************************************************************************/

#include "gx_pixelmap_prompt.h"

#include "gx_system.h"
#include "gx_canvas.h"
#include "gx_pixelmap.h"

static void gx_pixelmap_prompt_draw_(GX_PIXELMAP_PROMPT* prompt)
{
    return prompt->draw_();
}

UINT GX_PIXELMAP_PROMPT::create_(const GX_CHAR *name, GX_WIDGET *parent,
                                 GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                                 ULONG style, USHORT pixelmap_prompt_id,
                                 const GX_RECTANGLE *size)
{
    GX_PROMPT::create_(name, GX_NULL, text_id, style, pixelmap_prompt_id, size);

    this->type = GX_TYPE_PIXELMAP_PROMPT;

    this->normal_fill_pixelmap_id = fill_id;
    this->normal_left_pixelmap_id = 0;
    this->normal_right_pixelmap_id = 0;

    this->selected_fill_pixelmap_id = 0;
    this->selected_left_pixelmap_id = 0;
    this->selected_right_pixelmap_id = 0;

    this->draw_function = (void (*)(GX_WIDGET*))gx_pixelmap_prompt_draw_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

UINT GX_PIXELMAP_PROMPT::pixelmap_set_(GX_RESOURCE_ID normal_left_id,
                                       GX_RESOURCE_ID normal_fill_id,
                                       GX_RESOURCE_ID normal_right_id,
                                       GX_RESOURCE_ID selected_left_id,
                                       GX_RESOURCE_ID selected_fill_id,
                                       GX_RESOURCE_ID selected_right_id)
{
    this->normal_left_pixelmap_id = normal_left_id;
    this->normal_fill_pixelmap_id = normal_fill_id;
    this->normal_right_pixelmap_id = normal_right_id;

    this->selected_left_pixelmap_id = selected_left_id;
    this->selected_fill_pixelmap_id = selected_fill_id;
    this->selected_right_pixelmap_id = selected_right_id;

    dirty_mark_if_visible_();

    return GX_SUCCESS;
}

void GX_PIXELMAP_PROMPT::background_draw_()
{
    GX_RESOURCE_ID left_id = normal_left_pixelmap_id;
    GX_RESOURCE_ID fill_id = normal_fill_pixelmap_id;
    GX_RESOURCE_ID right_id = normal_right_pixelmap_id;

    if (style & GX_STYLE_DRAW_SELECTED)
    {
        if (selected_left_pixelmap_id)
        {
            left_id = selected_left_pixelmap_id;
        }

        if (selected_fill_pixelmap_id)
        {
            fill_id = selected_fill_pixelmap_id;
        }

        if (selected_right_pixelmap_id)
        {
            right_id = selected_right_pixelmap_id;
        }
    }

    if (fill_id == 0 && !(style & GX_STYLE_TRANSPARENT))
    {
        border_and_background_draw_();
    }

    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    GX_PIXELMAP* pixelmap;
    GX_RECTANGLE fill_rect = size;

    if (left_id)
    {
        context->pixelmap_get_(left_id, &pixelmap);
        if (pixelmap)
        {
            context->fill_color_set_(enabled_fill_color_());
            _gx_canvas_pixelmap_draw(fill_rect.left, fill_rect.top, pixelmap);
            fill_rect.left = (GX_VALUE)(fill_rect.left + pixelmap->width);
        }
    }

    if (right_id)
    {
        context->pixelmap_get_(right_id, &pixelmap);
        if (pixelmap)
        {
            context->fill_color_set_(enabled_fill_color_());
            _gx_canvas_pixelmap_draw((GX_VALUE)(fill_rect.right - pixelmap->width + 1),
                                     fill_rect.top, pixelmap);
            fill_rect.right = (GX_VALUE)(fill_rect.right - pixelmap->width);
        }
    }

    if (fill_id)
    {
        context->pixelmap_get_(fill_id, &pixelmap);
        if (pixelmap)
        {
            _gx_canvas_pixelmap_tile(&fill_rect, pixelmap);
        }
    }
}

void GX_PIXELMAP_PROMPT::draw_()
{
    background_draw_();
    text_draw_();
    children_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_pixelmap_prompt_create                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a pixelmap prompt, which is a special type of */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Prompt control block          */
/*    name                                  Name of prompt                */
/*    parent                                Parent widget control block   */
/*    text_id                               Resource string id            */
/*    fill_id                               pixelmap id for fill area     */
/*    style                                 Style of checkbox             */
/*    pixelmap_prompt_id                    Application-defined ID of     */
/*                                            pixelmap prompt             */
/*    size                                  Prompt size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_prompt_create                     Create the underlying prompt  */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_pixelmap_prompt_create(GX_PIXELMAP_PROMPT* prompt,
                                const GX_CHAR* name, GX_WIDGET* parent,
                                GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                                ULONG style, USHORT pixelmap_prompt_id,
                                const GX_RECTANGLE* size)

{
    return prompt->create_(name, parent, text_id, fill_id, style, pixelmap_prompt_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_pixelmap_prompt_draw                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified pixelmap prompt, which is a       */
/*      special type of widget.                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_children_draw              Draw children widgets         */
/*    _gx_prompt_text_draw                  Draw prompt text              */
/*    _gx_context_pixelmap_get              Retrieve pixelmap image       */
/*    _gx_canvas_pixelmap_draw              Draw a pixelmap               */
/*    _gx_canvas_pixelmap_tile              Tile a pixelmap               */
/*    _gx_prompt_text_draw                  Draw a text prompt            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_pixelmap_prompt_draw(GX_PIXELMAP_PROMPT* prompt)
{
    return prompt->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_pixelmap_prompt_pixelmap_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigned the pixelmaps used to draw a pixelmap_prompt */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Prompt control block          */
/*    normal_left_id                        Left pixelmap ID              */
/*    normal_fill_id                        Fill pixelmap ID              */
/*    normal_right_id                       Right pixelmap ID             */
/*    selected_left_id                      Left pixelmap ID              */
/*    selected_fill_id                      Fill pixelmap ID              */
/*    selected_right_id                     Right pixelmap ID             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_pixelmap_prompt_pixelmap_set(GX_PIXELMAP_PROMPT* prompt,
                                      GX_RESOURCE_ID normal_left_id,
                                      GX_RESOURCE_ID normal_fill_id,
                                      GX_RESOURCE_ID normal_right_id,
                                      GX_RESOURCE_ID selected_left_id,
                                      GX_RESOURCE_ID selected_fill_id,
                                      GX_RESOURCE_ID selected_right_id)
{
    return prompt->pixelmap_set_(normal_left_id,
                                 normal_fill_id,
                                 normal_right_id,
                                 selected_left_id,
                                 selected_fill_id,
                                 selected_right_id);
}
