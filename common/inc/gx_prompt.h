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
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_prompt.h                                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX prompt management component,             */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_PROMPT_H
#define GX_PROMPT_H

#include "gx_widget.h"
#include "gx_text.h"

/* Define Prompt type.  */
struct GX_PROMPT : public GX_WIDGET
{
    GX_TEXT text_data;
    UINT  (*text_get_function)(GX_PROMPT*, GX_STRING*);

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_RESOURCE_ID text_id, ULONG style, USHORT prompt_id,
                 const GX_RECTANGLE* size);

    const GX_STRING& string_() const { return text_data.string; }

    UINT text_set_ext_(const GX_STRING* string) { return text_data.string_ext_set_(string); }
    UINT text_get_ext_(GX_STRING* return_text)  { return text_data.get_and_resolve_bidi_text_(return_text); }

    GX_RESOURCE_ID text_id_() const { return text_data.text_id; }
    void text_id_set_(GX_RESOURCE_ID text_id)   { return text_data.text_id_set_(text_id);   }

    GX_RESOURCE_ID font_id_() const { return text_data.font_id; }
    GX_RESOURCE_ID& font_id_() { return text_data.font_id; }
    void font_set_(GX_RESOURCE_ID font_id) { return text_data.font_set_(font_id); }

    GX_RESOURCE_ID enabled_text_color_() const { return text_data.enabled_color_(); }
    GX_RESOURCE_ID text_color_() const { return text_data.color_(); }

    GX_RESOURCE_ID& normal_text_color_()   { return text_data.normal_color;   }
    GX_RESOURCE_ID& selected_text_color_() { return text_data.selected_color; }
    GX_RESOURCE_ID& disabled_text_color_() { return text_data.disabled_color; }

    void text_color_set_(GX_RESOURCE_ID normal_text_color_id,
                         GX_RESOURCE_ID selected_text_color_id,
                         GX_RESOURCE_ID disabled_text_color_id)
    {
        text_data.color_set_(normal_text_color_id,
                             selected_text_color_id,
                             disabled_text_color_id);
    }

    void text_draw_(INT x_offset = 0, INT y_offset = 0);
    void text_draw_(GX_RESOURCE_ID color_id, INT x_offset, INT y_offset);

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);
};


/* Define prompt management function prototypes.  */

UINT _gx_prompt_create(GX_PROMPT* prompt, const GX_CHAR* name, GX_WIDGET* parent,
                       GX_RESOURCE_ID text_id, ULONG style, USHORT prompt_id,
                       const GX_RECTANGLE* size);
void _gx_prompt_draw(GX_PROMPT* prompt);
UINT _gx_prompt_event_process(GX_PROMPT* prompt, GX_EVENT* event_ptr);
UINT _gx_prompt_font_set(GX_PROMPT* prompt, GX_RESOURCE_ID fontid);
UINT _gx_prompt_text_color_set(GX_PROMPT* prompt,
                               GX_RESOURCE_ID normal_text_color_id,
                               GX_RESOURCE_ID selected_text_color_id,
                               GX_RESOURCE_ID disabled_text_color_id);
void _gx_prompt_text_draw(GX_PROMPT* prompt);
UINT _gx_prompt_text_get_ext(GX_PROMPT* prompt, GX_STRING* return_text);
UINT _gx_prompt_text_id_set(GX_PROMPT* prompt, GX_RESOURCE_ID string_id);
UINT _gx_prompt_text_set_ext(GX_PROMPT* prompt, const GX_STRING* text);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_prompt_create(GX_PROMPT* prompt, const GX_CHAR* name, GX_WIDGET* parent,
                        GX_RESOURCE_ID text_id, ULONG style, USHORT prompt_id,
                        const GX_RECTANGLE* size,
                        UINT prompt_control_block_size);
UINT _gxe_prompt_event_process(GX_PROMPT* prompt, GX_EVENT* event_ptr);
UINT _gxe_prompt_font_set(GX_PROMPT* prompt, GX_RESOURCE_ID fontid);
UINT _gxe_prompt_text_color_set(GX_PROMPT* prompt,
                                GX_RESOURCE_ID normal_text_color_id,
                                GX_RESOURCE_ID selected_text_color_id,
                                GX_RESOURCE_ID disabled_text_color_id);

UINT _gxe_prompt_text_get_ext(GX_PROMPT* prompt, GX_STRING* return_text);
UINT _gxe_prompt_text_id_set(GX_PROMPT* prompt, GX_RESOURCE_ID string_id);
UINT _gxe_prompt_text_set_ext(GX_PROMPT* prompt, const GX_STRING* text);

#endif

