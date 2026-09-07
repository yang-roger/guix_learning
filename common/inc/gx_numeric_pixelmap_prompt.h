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
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_numeric_pixelmap_prompt.h                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX numeric pixelmap prompt management       */
/*    component, including all data types and external references.  It is */
/*    assumed that gx_api.h and gx_port.h have already been included.     */
/*                                                                        */
/**************************************************************************/

#ifndef GX_NUMERIC_PIXELMAP_PROMPT_H
#define GX_NUMERIC_PIXELMAP_PROMPT_H

#include "gx_pixelmap_prompt.h"

/* Define Numeric Pixelmap Prompt type.  */
struct GX_NUMERIC_PIXELMAP_PROMPT : public GX_PIXELMAP_PROMPT
{
    GX_CHAR buffer[GX_NUMERIC_PROMPT_BUFFER_SIZE];
    void  (*format_function)(GX_NUMERIC_PIXELMAP_PROMPT*, INT);

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                 ULONG style, USHORT pixelmap_prompt_id, const GX_RECTANGLE* size);

    void format_(INT value);
    UINT value_set_(INT value);
};


/* Define numeric pixelmap prompt management function prototypes.  */

UINT _gx_numeric_pixelmap_prompt_create(GX_NUMERIC_PIXELMAP_PROMPT* prompt,
                                        const GX_CHAR* name, GX_WIDGET* parent,
                                        GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                                        ULONG style, USHORT pixelmap_prompt_id, const GX_RECTANGLE* size);
UINT _gx_numeric_pixelmap_prompt_format_function_set(GX_NUMERIC_PIXELMAP_PROMPT* prompt, void (*format_func)(GX_NUMERIC_PIXELMAP_PROMPT*, INT));
UINT _gx_numeric_pixelmap_prompt_value_set(GX_NUMERIC_PIXELMAP_PROMPT* prompt, INT value);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_numeric_pixelmap_prompt_create(GX_NUMERIC_PIXELMAP_PROMPT* prompt,
                                         const GX_CHAR* name, GX_WIDGET* parent,
                                         GX_RESOURCE_ID text_id, GX_RESOURCE_ID fill_id,
                                         ULONG style, USHORT pixelmap_prompt_id, const GX_RECTANGLE* size, UINT control_block_size);
UINT _gxe_numeric_pixelmap_prompt_format_function_set(GX_NUMERIC_PIXELMAP_PROMPT* prompt, void (*format_func)(GX_NUMERIC_PIXELMAP_PROMPT*, INT));
UINT _gxe_numeric_pixelmap_prompt_value_set(GX_NUMERIC_PIXELMAP_PROMPT* prompt, INT value);

#endif

