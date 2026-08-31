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
/**   Prompt Management (pixelmap prompt)                                 */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_pixelmap_prompt.h                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX bitmap prompt management component,      */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_PIXELMAP_PROMPT_H
#define GX_PIXELMAP_PROMPT_H

#include "gx_prompt.h"

/* Define Pixelmap type.  */
struct GX_PIXELMAP_PROMPT : public GX_PROMPT
{
    GX_RESOURCE_ID normal_left_pixelmap_id;
    GX_RESOURCE_ID normal_right_pixelmap_id;
    GX_RESOURCE_ID normal_fill_pixelmap_id;
    GX_RESOURCE_ID selected_left_pixelmap_id;
    GX_RESOURCE_ID selected_right_pixelmap_id;
    GX_RESOURCE_ID selected_fill_pixelmap_id;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                 ULONG style, USHORT pixelmap_prompt_id,
                 const GX_RECTANGLE* size);

    UINT pixelmap_set_(GX_RESOURCE_ID normal_left_id,
                       GX_RESOURCE_ID normal_fill_id,
                       GX_RESOURCE_ID normal_right_id,
                       GX_RESOURCE_ID selected_left_id,
                       GX_RESOURCE_ID selected_fill_id,
                       GX_RESOURCE_ID selected_right_id);

    void background_draw_();

    void draw_();
};


/* Define prompt management function prototypes.  */

UINT _gx_pixelmap_prompt_create(GX_PIXELMAP_PROMPT* prompt, const GX_CHAR* name, GX_WIDGET* parent,
                                GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                                ULONG style, USHORT pixelmap_prompt_id,
                                const GX_RECTANGLE* size);
void _gx_pixelmap_prompt_draw(GX_PIXELMAP_PROMPT* prompt);
UINT _gx_pixelmap_prompt_pixelmap_set(GX_PIXELMAP_PROMPT* prompt,
                                      GX_RESOURCE_ID normal_left_id,
                                      GX_RESOURCE_ID normal_fill_id,
                                      GX_RESOURCE_ID normal_right_id,
                                      GX_RESOURCE_ID selected_left_id,
                                      GX_RESOURCE_ID selected_fill_id,
                                      GX_RESOURCE_ID selected_right_id);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_pixelmap_prompt_create(GX_PIXELMAP_PROMPT* prompt, const GX_CHAR* name, GX_WIDGET* parent,
                                 GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                                 ULONG style, USHORT pixelmap_prompt_id,
                                 const GX_RECTANGLE* size,
                                 UINT pixelmap_prompt_control_block_size);
UINT _gxe_pixelmap_prompt_pixelmap_set(GX_PIXELMAP_PROMPT* prompt,
                                       GX_RESOURCE_ID normal_left_id,
                                       GX_RESOURCE_ID normal_fill_id,
                                       GX_RESOURCE_ID normal_right_id,
                                       GX_RESOURCE_ID selected_left_id,
                                       GX_RESOURCE_ID selected_fill_id,
                                       GX_RESOURCE_ID selected_right_id);

#endif

