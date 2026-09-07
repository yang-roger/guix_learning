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

#include "gx_pixelmap_prompt.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_pixelmap_prompt_create                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the pixelmap prompt create function. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prompt                                Prompt control block          */
/*    name                                  Name of prompt                */
/*    parent                                Parent widget control block   */
/*    text_id                               Resource string id            */
/*    left_id                               pixelmap id for left end      */
/*    fill_id                               pixelmap id for fill area     */
/*    style                                 Style of checkbox             */
/*    prompt_id                             Application-defined ID of     */
/*                                            pixelmap prompt             */
/*    size                                  Prompt size                   */
/*    pixelmap_prompt_control_block_size    Size of the pixelmap prompt   */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_pixelmap_prompt_create            The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_pixelmap_prompt_create(GX_PIXELMAP_PROMPT *prompt,
                                  const GX_CHAR *name,
                                  GX_WIDGET *parent, GX_RESOURCE_ID text_id,
                                  GX_RESOURCE_ID fill_id, ULONG style,
                                  USHORT prompt_id,
                                  const GX_RECTANGLE *size,
                                  UINT pixelmap_prompt_control_block_size)
{

UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((prompt == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget control block size. */
    if (pixelmap_prompt_control_block_size != sizeof(GX_PIXELMAP_PROMPT))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (prompt->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    status = _gx_pixelmap_prompt_create(prompt, name, parent, text_id, fill_id, style, prompt_id, size);
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_pixelmap_prompt_pixelmap_set                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the pixelmap prompt pixelmap     */
/*    set function call.                                                  */
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
/*    _gx_pixelmap_prompt_pixelmap_set      Actual pixelmap prompt        */
/*                                            pixelmap set function       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_pixelmap_prompt_pixelmap_set(GX_PIXELMAP_PROMPT *prompt,
                                        GX_RESOURCE_ID normal_left_id,
                                        GX_RESOURCE_ID normal_fill_id,
                                        GX_RESOURCE_ID normal_right_id,
                                        GX_RESOURCE_ID selected_left_id,
                                        GX_RESOURCE_ID selected_fill_id,
                                        GX_RESOURCE_ID selected_right_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!prompt)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_pixelmap_prompt_pixelmap_set(prompt,
                                              normal_left_id,
                                              normal_fill_id,
                                              normal_right_id,
                                              selected_left_id,
                                              selected_fill_id,
                                              selected_right_id);

    return(status);
}
