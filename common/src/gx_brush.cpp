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
/**   Utility (Brush)                                                     */
/**                                                                       */
/**************************************************************************/

#include "gx_brush.h"

void GX_BRUSH::default_init_()
{
    pixelmap = GX_NULL;
    font = GX_NULL;
    fill_color = 0;
    line_color = 0;
    line_pattern = 0;
    pattern_mask = 0;
    style = 0;
    width = 1;
    alpha = GX_ALPHA_VALUE_OPAQUE;
}

void GX_BRUSH::set_(GX_COLOR line_color, GX_COLOR fill_color, UINT style)
{
    this->fill_color = fill_color;
    this->line_color = line_color;
    this->style = style;
    this->line_pattern = 0;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_brush_default                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function defines the brush type.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    brush                                 Pointer to brush              */
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
/*    _gx_context_brush_default                                           */
/*                                                                        */
/**************************************************************************/
UINT _gx_brush_default(GX_BRUSH* brush)
{
    brush->default_init_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_brush_define                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service defines a brush with the specified line color, fill    */
/*    color and style.                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    brush                                 Pointer to brush              */
/*    line_color                            Color of brush line           */
/*    fill_color                            Color of brush fill           */
/*    style                                 Brush style                   */
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
/*    _gx_context_brush_define              Define the brush for context  */
/*    _gx_context_raw_brush_define          Define the brush for context  */
/*                                                                        */
/**************************************************************************/
UINT _gx_brush_define(GX_BRUSH* brush, GX_COLOR line_color, GX_COLOR fill_color, UINT style)
{
    brush->set_(line_color, fill_color, style);

    return GX_SUCCESS;
}
