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
/**   Canvas Management (Canvas)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_canvas.h"

#include "gx_display.h"
#include "gx_utility.h"
#include "gx_system.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_alpha_set                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the canvas alpha set function    */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    alpha                                 alpha value                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_alpha_set                  Actual canvas alpha set call  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_alpha_set(GX_CANVAS *canvas, GX_UBYTE alpha)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if (canvas == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (canvas ->id != GX_CANVAS_ID)
    {
        return (GX_INVALID_CANVAS);
    }

    /* Call the actual canvas alpha set function.  */
    status = _gx_canvas_alpha_set(canvas, alpha);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_arc_draw                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in canvas arc draw function call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    xcenter                               x-coord of center of circle   */
/*                                            arc                         */
/*    ycenter                               y-coord of center of circle   */
/*                                            arc                         */
/*    r                                     Radius of circle arc          */
/*    start_angle                           The start angle of circle arc */
/*    end_angle                             The end angle of circle arc   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_arc_draw                   Actual canvas arc draw        */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_canvas_arc_draw(INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid value. */
    if (r == 0)
    {
        return GX_INVALID_VALUE;
    }

    /* Check for invalid draw context. */
    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call actual widget draw function.  */
    status = _gx_canvas_arc_draw(xcenter, ycenter, r, start_angle, end_angle);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_block_move                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the canvas block move function       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    block                                 Rectangle to move             */
/*    x_shift                               Distance to move in x         */
/*                                            direction                   */
/*    y_shift                               Distance to move in y         */
/*                                            direction                   */
/*    dirty                                 Flag of dirty block           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_block_move                 The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_canvas_block_move(GX_RECTANGLE *block, GX_VALUE x_shift, GX_VALUE y_shift, GX_RECTANGLE *dirty)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if (block == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual function.  */
    status = _gx_canvas_block_move(block, x_shift, y_shift, dirty);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_circle_draw                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in canvas circle draw function call.*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    xcenter                               x-coord of center of circle   */
/*    ycenter                               y-coord of center of circle   */
/*    r                                     Radius of circle              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gxe_canvas_circle_draw               Actual canvas circle draw     */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_canvas_circle_draw(INT xcenter, INT ycenter, UINT r)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid value. */
    if (r == 0)
    {
        return GX_INVALID_VALUE;
    }

    /* Check for invalid context. */
    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call actual widget draw function.  */
    status = _gx_canvas_circle_draw(xcenter, ycenter, r);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_create                                                  */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the canvas function call.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    name                                  Name of canvas                */
/*    display                               Display control block         */
/*    type                                  Type of canvas                */
/*    width                                 Width of canvas               */
/*    height                                Height of canvas              */
/*    memory_area                           Memory area of canvas with    */
/*                                            each pixel of GX_COLOR      */
/*    memory_size                           Size of canvas memory area    */
/*    canvas_control_block_size             Size of the canvas control    */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_create                     Actual canvas create call     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_create(GX_CANVAS *canvas, const GX_CHAR *name, GX_DISPLAY *display, UINT type,
                         UINT width, UINT height, GX_COLOR *memory_area, ULONG memory_size,
                         UINT canvas_control_block_size)
{
UINT status;
UINT pitch;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((canvas == GX_NULL) || (display == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (canvas_control_block_size != sizeof(GX_CANVAS))
    {
        return(GX_INVALID_CANVAS_SIZE);
    }

    /* Check for widget already created.  */
    if (canvas->display != GX_NULL)
    {
        return(GX_ALREADY_CREATED);
    }

    if (memory_area)
    {
        pitch = (UINT)(display->driver_row_pitch_get((USHORT)width));
        if (memory_size < pitch * height)
        {
            return GX_INVALID_SIZE;
        }
    }

    if (type & (~((UINT)(GX_CANVAS_SIMPLE | GX_CANVAS_MANAGED | GX_CANVAS_VISIBLE |
                         GX_CANVAS_MODIFIED | GX_CANVAS_COMPOSITE))))
    {
        return(GX_INVALID_TYPE);
    }

    /* Call the actual canvas create function.  */
    status = _gx_canvas_create(canvas, name, display, type, width, height, memory_area, memory_size);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_delete                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the canvas delete call.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_delete                     Actual canvas delete call     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_delete(GX_CANVAS *canvas)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if (canvas == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (canvas ->id != GX_CANVAS_ID)
    {
        return (GX_INVALID_CANVAS);
    }

    /* Call the actual canvas delete function.  */
    status = _gx_canvas_delete(canvas);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_drawing_complete                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the canvas drawing complete      */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    flush                                 If GX_TRUE, the content of    */
/*                                            the canvas is flushed to    */
/*                                            the display                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_drawing_complete           Actual canvas drawing         */
/*                                          complete call                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_drawing_complete(GX_CANVAS *canvas, GX_BOOL flush)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Checks for the invalid input pointers.  */
    if ((canvas == GX_NULL) ||
        (canvas->display == GX_NULL) ||
        (canvas->memory == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual canvas drawing complete function.  */
    status = _gx_canvas_drawing_complete(canvas, flush);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_drawing_initiate                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the canvas drawing initiate      */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    who                                   Widget control block pointer  */
/*    dirty_area                            Area to draw on               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_drawing_initiate           Actual canvas drawing         */
/*                                          initiate call                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_canvas_drawing_initiate(GX_CANVAS *canvas, GX_WIDGET *who, GX_RECTANGLE *dirty_area)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if ((canvas == GX_NULL) ||
        (dirty_area == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (canvas->id != GX_CANVAS_ID)
    {
        return(GX_INVALID_CANVAS);
    }

    /* Call the actual canvas drawing initiate function.  */
    status = _gx_canvas_drawing_initiate(canvas, who, dirty_area);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_ellipse_draw                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in canvas ellipse draw function     */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    xcenter                               x-coord of center of ellipse  */
/*    ycenter                               y-coord of center of ellipse  */
/*    r                                     Radius of circle              */
/*    a                                     Length of the Semi-major Axis */
/*    b                                     Length of the Semi-minor Axis */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_ellipse_draw               Actual canvas ellipse draw    */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_canvas_ellipse_draw(INT xcenter, INT ycenter, INT a, INT b)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input values.  */
    if ((a <= 0) || (b <= 0))
    {
        return(GX_INVALID_VALUE);
    }

    /* Check for invalid draw context. */
    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call actual widget draw function.  */
    status = _gx_canvas_ellipse_draw(xcenter, ycenter, a, b);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_hardware_layer_bind                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the canvas hardware layer bind   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    layer                                 hardware layer                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_hardware_layer_bind        Actual canvas bind call       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_hardware_layer_bind(GX_CANVAS *canvas, INT layer)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if (canvas == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (canvas ->id != GX_CANVAS_ID)
    {
        return (GX_INVALID_CANVAS);
    }

    if (canvas->display == GX_NULL)
    {
        return (GX_INVALID_DISPLAY);
    }
    if (canvas->display->layer_services == GX_NULL)
    {
        return (GX_NOT_SUPPORTED);
    }

    /* Call the actual canvas alpha set function.  */
    status = _gx_canvas_hardware_layer_bind(canvas, layer);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_hide                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the canvas hide function         */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_alpha_set                  Actual canvas alpha set call  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_hide(GX_CANVAS *canvas)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if (canvas == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (canvas->id != GX_CANVAS_ID)
    {
        return (GX_INVALID_CANVAS);
    }

    /* Call the actual canvas alpha set function.  */
    status = _gx_canvas_hide(canvas);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_line_draw                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the canvas line draw function.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    x_start                               x-coord of endpoint1          */
/*    y_start                               y-coord of endpoint1          */
/*    x_end                                 x-coord of endpoint2          */
/*    y_end                                 y-coord of endpoint2          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_line_draw                  The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_line_draw(GX_VALUE x_start, GX_VALUE y_start, GX_VALUE x_end, GX_VALUE y_end)
{
UINT             status;
GX_DRAW_CONTEXT *context;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* pick up the current drawing context */
    context = _gx_system_current_draw_context;
    if (context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual function.  */
    status = _gx_canvas_line_draw(x_start, y_start, x_end, y_end);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_memory_define                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the canvas memory define         */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    memory                                Canvas memory address         */
/*    memsize                               Canvas memory size            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_alpha_set                  Actual canvas alpha set call  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_memory_define(GX_CANVAS *canvas, GX_COLOR *memory, ULONG memsize)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if (canvas == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }
    if (canvas ->id != GX_CANVAS_ID)
    {
        return (GX_INVALID_CANVAS);
    }

    /* Call the actual canvas alpha set function.  */
    status = _gx_canvas_memory_define(canvas, memory, memsize);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_mouse_define                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors for the canvas mouse define call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    info                                  Mouse cursor info control     */
/*                                              block                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_mouse_define               Actual canvas mouse define    */
/*                                              call                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_MOUSE_SUPPORT)
UINT _gxe_canvas_mouse_define(GX_CANVAS *canvas, GX_MOUSE_CURSOR_INFO *info)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if (info == GX_NULL || canvas == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual mouse hotspot set function.  */
    status = _gx_canvas_mouse_define(canvas, info);

    /* Return completion  status.  */
    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_mouse_hide                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors for the canvas mouse hide call.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_mouse_hide                 Actual canvas mouse hide      */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_MOUSE_SUPPORT)
UINT _gxe_canvas_mouse_hide(GX_CANVAS *canvas)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if ((canvas == GX_NULL) ||
        (canvas->display == GX_NULL) ||
        (canvas->memory == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual mouse hotspot set function.  */
    status = _gx_canvas_mouse_hide(canvas);

    /* Return completion  status.  */
    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_mouse_show                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors for the canvas mouse show call.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_mouse_show                 Actual canvas mouse show      */
/*                                              call                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_MOUSE_SUPPORT)
UINT _gxe_canvas_mouse_show(GX_CANVAS *canvas)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if ((canvas == GX_NULL) ||
        (canvas->display == GX_NULL) ||
        (canvas->memory == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual mouse hotspot set function.  */
    status = _gx_canvas_mouse_show(canvas);

    /* Return completion  status.  */
    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_offset_set                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the canvas offset set function   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    x                                     X coordinate of offset        */
/*    y                                     Y coordinate of offset        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_offset_set                 Actual canvas offset set call */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_offset_set(GX_CANVAS *canvas, GX_VALUE x, GX_VALUE y)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if ((canvas == GX_NULL) ||
        (canvas->display == GX_NULL) ||
        (canvas->memory == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (canvas->id != GX_CANVAS_ID)
    {
        return (GX_INVALID_CANVAS);
    }

    /* Call the actual canvas offset set function.  */
    status = _gx_canvas_offset_set(canvas, x, y);

    /* Return completion  status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_pie_draw                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in canvas pie draw function call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    xcenter                               x-coord of center of circle   */
/*                                            arc                         */
/*    ycenter                               y-coord of center of circle   */
/*                                            arc                         */
/*    r                                     Radius of circle arc          */
/*    start_angle                           The start angle of the pie    */
/*    end_angle                             The end angle of the pie      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_pie_draw                   Actual canvas pie draw        */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_canvas_pie_draw(INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid value. */
    if (r == 0)
    {
        return GX_INVALID_VALUE;
    }

    /* Check for invalid draw context. */
    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call actual widget draw function.  */
    status = _gx_canvas_pie_draw(xcenter, ycenter, r, start_angle, end_angle);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_pixel_draw                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the canvas pixel draw function.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    position                              x,y position to draw to       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_pixel_draw                 The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_pixel_draw(GX_POINT position)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual function.  */
    status = _gx_canvas_pixel_draw(position);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_pixelmap_blend                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the canvas pixelmap blend function.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    x_position                            Top-left x-coord to place     */
/*                                            pixelmap                    */
/*    y_position                            Top-left y-coord to place     */
/*                                            pixelmap                    */
/*    pixelmap                              Pointer to actual pixelmap    */
/*                                            to draw                     */
/*    alpha                                 blending value 0-255          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_pixelmap_draw              The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_pixelmap_blend(GX_VALUE x_position, GX_VALUE y_position,
                                 GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if (pixelmap == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual function.  */
    status = _gx_canvas_pixelmap_blend(x_position, y_position, pixelmap, alpha);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_pixelmap_draw                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the canvas pixelmap draw function.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    x_position                            Top-left x-coord to place     */
/*                                            pixelmap                    */
/*    y_position                            Top-left y-coord to place     */
/*                                            pixelmap                    */
/*    pixelmap                              Pointer to actual pixelmap    */
/*                                            to draw                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_pixelmap_draw              The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_pixelmap_draw(GX_VALUE x_position, GX_VALUE y_position, GX_PIXELMAP *pixelmap)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if (pixelmap == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual function.  */
    status = _gx_canvas_pixelmap_draw(x_position, y_position, pixelmap);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_offset_set                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the canvas pixelmap get function */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pixelmap                              Pointer to destination for    */
/*                                            the retrieved pixlemap      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gxe_canvas_pixelmap_get              Actual canvas pixelmap get    */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_pixelmap_get(GX_PIXELMAP *pixelmap)
{
    UINT status;

    /* Check for the invalid input pointers.  */
    if (pixelmap == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual canvas pixlemap get function.  */
    status = _gx_canvas_pixelmap_get(pixelmap);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_pixelmap_rotate                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the canvas pixelmap rotate function. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    x_position                            Top-left x-coord to place     */
/*                                            pixelmap                    */
/*    y_position                            Top-left y-coord to place     */
/*                                            pixelmap                    */
/*    pixelmap                              Pointer to actual pixelmap    */
/*                                            to draw                     */
/*    angle                                 The angle to rotate           */
/*    rot_cx                                x-coord of rotating center, if*/
/*                                            -1 is set, default it to    */
/*                                            image center.               */
/*    rot_cy                                y-coord of rotationg center.  */
/*                                            -1 is set, default it to    */
/*                                            image center.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_pixelmap_draw              The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_pixelmap_rotate(GX_VALUE x_position, GX_VALUE y_position, GX_PIXELMAP *pixelmap,
                                  INT angle, INT rot_cx, INT rot_cy)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if (pixelmap == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual function.  */
    status = _gx_canvas_pixelmap_rotate(x_position, y_position, pixelmap, angle, rot_cx, rot_cy);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_pixelmap_tile                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the canvas pixelmap tile function.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    fill                                  Area to fill with pixelmap    */
/*    pixelmap                              Pointer to actual pixelmap    */
/*                                            to draw                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_canvas_pixelmap_tile               The actual pixelmap tile      */
/*                                            routine                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_pixelmap_tile(GX_RECTANGLE *fill, GX_PIXELMAP *pixelmap)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if ((fill == GX_NULL) || (pixelmap == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Check for invalid widget.  */
    if ((fill->left > fill->right) ||
        (fill->top > fill->bottom))
    {
        return(GX_INVALID_VALUE);
    }

    /* Call the actual function.  */
    status = _gx_canvas_pixelmap_tile(fill, pixelmap);

    /* Return completion status.  */
    return(status);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_polygon_draw                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks errors in the canavs polygon draw function.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    point_array                           Array of points of the        */
/*                                            polygon terminated by NULL  */
/*                                            entry.                      */
/*    number_of_points                      Number of points of polygon   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_polygon_draw               The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_polygon_draw(GX_POINT *point_array, INT number_of_points)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if (point_array == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }
    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual function.  */
    status = _gx_canvas_polygon_draw(point_array, number_of_points);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_rectangle_draw                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks errors in the canvas rectangle draw function.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    rectangle                             Rectangle specification       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_rectangle_draw             The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_rectangle_draw(GX_RECTANGLE *rectangle)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if (rectangle == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }
    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    /* Call the actual function.  */
    status = _gx_canvas_rectangle_draw(rectangle);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_rotated_text_draw_ext                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks error in canvas rotated text draw function.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text                                  pointer to string             */
/*    xcenter                               center point for text drawing */
/*    ycenter                               center point for text drawing */
/*    angle                                 angle at which to rotate text */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_rotated_text_draw_ext      The actual canvas rotated     */
/*                                            text draw function          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_canvas_rotated_text_draw_ext(const GX_STRING *text, GX_VALUE xcenter, GX_VALUE ycenter, INT angle)
{
UINT status;
UINT text_length = 0;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((text == GX_NULL) || (text->ptr == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    if (!_gx_system_memory_allocator || !_gx_system_memory_free)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    status = _gx_utility_string_length_check(text->ptr, &text_length, text->length);

    if (status != GX_SUCCESS)
    {
        return status;
    }

    if (text_length != text->length)
    {
        return GX_INVALID_STRING_LENGTH;
    }

    status = _gx_canvas_rotated_text_draw_ext(text, xcenter, ycenter, angle);

    /* Return completion status code. */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_shift                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the canvas shift function call.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                canvas control block          */
/*    x                                     x-axis shift amount in pixels */
/*    y                                     y-axis shift amount in pixels */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_canvas_shift                       Actual canvas shift call.     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_shift(GX_CANVAS *canvas, GX_VALUE x, GX_VALUE y)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if ((canvas == GX_NULL) ||
        (canvas->display == GX_NULL) ||
        (canvas->memory == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (canvas->id != GX_CANVAS_ID)
    {
        return (GX_INVALID_CANVAS);
    }

    /* Call the actual canvas shift function.  */
    status = _gx_canvas_shift(canvas, x, y);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_show                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the canvas show function         */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    alpha                                 alpha value                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_show                       Actual canvas alpha set call  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_canvas_show(GX_CANVAS *canvas)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if (canvas == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (canvas->id != GX_CANVAS_ID)
    {
        return (GX_INVALID_CANVAS);
    }

    /* Call the actual canvas alpha set function.  */
    status = _gx_canvas_show(canvas);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_text_draw_ext                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the screen text draw function call.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    x_start                               x coordinate, text left edge  */
/*    y_start                               y coordinate, text baseline   */
/*    string                                String to draw                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_text_draw_ext              The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_canvas_text_draw_ext(GX_VALUE x_start, GX_VALUE y_start, const GX_STRING *string)
{
UINT status;
UINT text_length = 0;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((string == GX_NULL) || (string->ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    status = _gx_utility_string_length_check(string->ptr, &text_length, string->length);

    if (status != GX_SUCCESS)
    {
        return status;
    }

    if (text_length < string->length)
    {
        return GX_INVALID_STRING_LENGTH;
    }

    /* Call actual widget height get function.  */
    status = _gx_canvas_text_draw_ext(x_start, y_start, string);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_canvas_aligned_text_draw_ext                                   */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the canvas aligned text draw         */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    string                                String to draw                */
/*    rectangle                             Drawing area                  */
/*    alignment                             Alignment style               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_aligned_text_draw_ext      The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_canvas_aligned_text_draw(const GX_STRING *string, GX_RECTANGLE *rectangle, ULONG alignment)
{
UINT status;
UINT text_length = 0;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((string == GX_NULL) || (rectangle == GX_NULL) || (string->ptr == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    if (_gx_system_current_draw_context == GX_NULL)
    {
        return GX_INVALID_CONTEXT;
    }

    status = _gx_utility_string_length_check(string->ptr, &text_length, string->length);

    if (status != GX_SUCCESS)
    {
        return status;
    }

    if (text_length < string->length)
    {
        return GX_INVALID_STRING_LENGTH;
    }

    /* Call actual widget height get function.  */
    status = _gx_canvas_aligned_text_draw(string, rectangle, alignment);

    /* Return successful completion.  */
    return(status);
}
