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
/**   Context Management (Context)                                        */
/**                                                                       */
/**************************************************************************/

#include "gx_context.h"

#include "gx_system.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_brush_default                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for the errors in the context brush default    */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Context control block         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_brush_default             Actual context brush default  */
/*                                          call                          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_brush_default(GX_DRAW_CONTEXT *context)
{
UINT status;

    /* Check for invalid input pointers.  */
    if (context == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual context brush default function.  */
    status = _gx_context_brush_default(context);

    /* Return completino status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_brush_define                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context brush define function*/
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    line_color_id                         Resource ID of line color     */
/*    fill_color_id                         Resource ID of fill color     */
/*    style                                 Style of brush                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_brush_define              Actual context brush define   */
/*                                          function                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_brush_define(GX_RESOURCE_ID line_color_id, GX_RESOURCE_ID fill_color_id, UINT style)
{
UINT             status;

    /* Check for invalid draw context.  */
    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual context brush define function.  */
    status = _gx_context_brush_define(line_color_id, fill_color_id, style);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_brush_get                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context brush get function.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    return_brush                          Pointer to destination for    */
/*                                            brush                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_brush_get                 Actual context brush get call */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_context_brush_get(GX_BRUSH **return_brush)
{
    UINT status;

    /* Check for invalid input pointers.  */
    if (return_brush == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for no active drawing context. */
    if (_gx_system_current_draw_context == GX_NULL)
    {
        *return_brush = GX_NULL;
        return GX_INVALID_CONTEXT;
    }

    /* Call the actua; context brush get function.  */
    status = _gx_context_brush_get(return_brush);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_brush_pattern_set                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context brush pattern        */
/*    set function call.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pattern                               Brush pattern of current      */
/*                                            context                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_bursh_pattern_set         Actual brush pattern set      */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_brush_pattern_set(ULONG pattern)
{
    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    return(_gx_context_brush_pattern_set(pattern));
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_brush_set                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context brush set function.  */
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
/*    _gx_context_brush_set                 Actual context brush set call */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_context_brush_set(GX_BRUSH *brush)
{
UINT status;

    /* Check for invalid input pointers.  */
    if (brush == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return(GX_INVALID_CONTEXT);
    }

    /* Call the actua; context brush set function.  */
    status = _gx_context_brush_set(brush);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_brush_style_set                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context brush style set      */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    style                                 Brush style of current context*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_brush_style_set           Actual context brush style set*/
/*                                          call                          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_brush_style_set(UINT style)
{
UINT status;

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual context brush style set function.  */
    status = _gx_context_brush_style_set(style);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_brush_width_set                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context brush width set      */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    width                                 Brush width in pixels of      */
/*                                            current context             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_brush_width_set           Actual context brush width    */
/*                                            set function.               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_brush_width_set(UINT width)
{
UINT status;

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual context brush width set function.  */
    status = _gx_context_brush_width_set(width);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_color_get                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system color get call.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    color_id                              Resource ID of color          */
/*    return_color                          Pointer to destination for    */
/*                                            color                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_color_get                  Actual system color get call  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_color_get(GX_RESOURCE_ID color_id, GX_COLOR *return_color)
{
UINT  status;

    /* Check for invalid pointer.  */
    if (return_color == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (_gx_system_current_draw_context == GX_NULL)
    {
        *return_color = 0;
        return GX_INVALID_CONTEXT;
    }

    /* Call actual system color get.  */
    status = _gx_context_color_get(color_id, return_color);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_fill_color_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context fill color set call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    fill_color_id                         Resource ID of fill color     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_fill_color_set            Actual context fill color set */
/*                                            function.                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_fill_color_set(GX_RESOURCE_ID fill_color_id)
{
UINT             status;

    /* Check for invalid resource id.  */
    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual context fill color set function.  */
    status = _gx_context_fill_color_set(fill_color_id);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_font_get                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in context font get call.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    resource_id                           Resource ID of font           */
/*    return_font                           Pointer to destination for    */
/*                                            font pointer                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_font_get                  Actual context font get call  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_font_get(GX_RESOURCE_ID font_id, GX_FONT **return_font)
{
UINT             status;

    /* Check for invalid pointer.  */
    if (return_font == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call actual system color get.  */
    status = _gx_context_font_get(font_id, return_font);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_font_set                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context font set function.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    font_id                               Font resource ID of current   */
/*                                          context                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_font_set                  Actual context font set       */
/*                                          function                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_font_set(GX_RESOURCE_ID font_id)
{
UINT             status;

    if (!_gx_system_current_draw_context)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual context font set function.  */
    status = _gx_context_font_set(font_id);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_line_color_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context line color set       */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    line_color_id                         Line color of current context */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_line_color_set            Actual context line color set */
/*                                          function                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_line_color_set(GX_RESOURCE_ID line_color_id)
{
UINT             status;

    /* Check for invalid resource id.  */
    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual context line color set function.  */
    status = _gx_context_line_color_set(line_color_id);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_pixelmap_get                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in context pixelmap set call.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pixelmap_id                           Pixelmap resource ID          */
/*    return_pixelmap                       Pointer to pixelmap           */
/*                                            destination pointer         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_pixelmap_get              Actual context pixelmap get   */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default draw funtions                                          */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_pixelmap_get(GX_RESOURCE_ID pixelmap_id, GX_PIXELMAP **return_pixelmap)
{
UINT             status;

    /* Check for invalid pointer.  */
    if (return_pixelmap == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (_gx_system_current_draw_context == GX_NULL)
    {
        *return_pixelmap = GX_NULL;
        return GX_INVALID_CONTEXT;
    }

    /* Call actual pixelmap.  */
    status = _gx_context_pixelmap_get(pixelmap_id, return_pixelmap);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_pixelmap_set                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context pixelmap set         */
/*      function call.                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pixelmap_id                           Pixelmap resource ID          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_pixelmap_set              Actual context pixelmap set   */
/*                                          function                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_pixelmap_set(GX_RESOURCE_ID pixelmap_id)
{
UINT             status;

    if (!_gx_system_current_draw_context)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual context pixelmap set function.  */
    status = _gx_context_pixelmap_set(pixelmap_id);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_raw_brush_define                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context raw brush define     */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    line_color                            Color of line                 */
/*    fill_color                            Color of fill                 */
/*    style                                 Style of brush                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_raw_brush_define          Actual context raw brush      */
/*                                          define function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_raw_brush_define(GX_COLOR line_color, GX_COLOR fill_color, UINT style)
{
UINT status;

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return(GX_INVALID_CONTEXT);
    }

    /* Call the actual context raw brush define function.  */
    status = _gx_context_raw_brush_define(line_color, fill_color, style);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_raw_fill_color_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context raw fill color set   */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    line_color                            Color of line                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_raw_fill_color_set        Actual context raw fill color */
/*                                            set function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_raw_fill_color_set(GX_COLOR line_color)
{
UINT status;

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual context raw fill color set function.  */
    status = _gx_context_raw_fill_color_set(line_color);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_raw_line_color_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the context raw line color set   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    line_color                            Line color of current context */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_raw_line_color_set        Actual context raw line color */
/*                                            set function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_raw_line_color_set(GX_COLOR line_color)
{
UINT status;

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual context raw line color set function.  */
    status = _gx_context_raw_line_color_set(line_color);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_context_string_get_ext                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in context pixelmap set call.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pixelmap_id                           Pixelmap resource ID          */
/*    return_pixelmap                       Pointer to pixelmap           */
/*                                            destination pointer         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_pixelmap_get              Actual context pixelmap get   */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default draw funtions                                          */
/*                                                                        */
/**************************************************************************/
UINT _gxe_context_string_get_ext(GX_RESOURCE_ID string_id, GX_STRING *return_string)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (return_string == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return_string->ptr = GX_NULL;
        return_string->length = 0;
        return GX_INVALID_CONTEXT;
    }

    /* Call actual string.  */
    status = _gx_context_string_get_ext(string_id, return_string);

    /* Return status.  */
    return(status);
}
