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
/**   Widget Management (Widget)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_widget.h"

#include "gx_utility.h"
#include "gx_system.h"

GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_allocate                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function allocates memory for a widget control block           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Return pointer for widget     */
/*                                            control block.              */
/*    memsize                               Size of control block.        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_allocate                   Actual widget allocation call */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_allocate(GX_WIDGET **widget, ULONG memsize)
{
    if (!_gx_system_memory_allocator)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    if (!widget)
    {
        return GX_PTR_ERROR;
    }
    if (!memsize)
    {
        return GX_INVALID_MEMORY_SIZE;
    }
    return _gx_widget_allocate(widget, memsize);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_attach                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget attach function call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Parent widget control block   */
/*    child                                 child widget control block    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_attach                     Actual widget attach function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT  _gxe_widget_attach(GX_WIDGET *parent, GX_WIDGET *child)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!parent || !child)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if ((parent->type == 0) ||
        (child->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget attach function.  */
    status = _gx_widget_attach(parent, child);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_back_attach                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget attach function call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Parent widget control block   */
/*    child                                 child widget control block    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_back_attach                Actual widget back attach     */
/*                                            function call               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT  _gxe_widget_back_attach(GX_WIDGET *parent, GX_WIDGET *child)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!parent || !child)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if ((parent->type == 0) ||
        (child->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget attach function.  */
    status = _gx_widget_back_attach(parent, child);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_back_move                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget back move function    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    widget_moved                          Return flag indicate widget   */
/*                                            moved                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_back_move                  Actual widget back move       */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_back_move(GX_WIDGET *widget, GX_BOOL *widget_moved)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Check for valid widget.  */
    if (widget->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call actual widget back move function.  */
    status = _gx_widget_back_move(widget, widget_moved);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_block_move                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget block move function.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    block                                 Rectangle to move             */
/*    x_shift                               Number of pixels to shift on  */
/*                                            the x-axis                  */
/*    y_shift                               Number of pixels to shift on  */
/*                                            the y-axis                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_block_move                 Actual widget block move      */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_block_move(GX_WIDGET *widget, GX_RECTANGLE *block, INT x_shift, INT y_shift)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (block == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for valid widget.  */
    if (widget->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Check for valid rectangle.  */
    if ((block->left > block->right) ||
        (block->top > block->bottom))
    {
        return GX_INVALID_VALUE;
    }

    /* Call actual widget back move function.  */
    status = _gx_widget_block_move(widget, block, x_shift, y_shift);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_border_style_set                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget border style function */
/*    call                                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    Style                                 Style of border               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_border_style_set           Actual widget border style    */
/*                                            set function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_border_style_set(GX_WIDGET *widget, ULONG Style)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Check for valid widget.  */
    if (widget->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call actual widget back move function.  */
    status = _gx_widget_border_style_set(widget, Style);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_border_width_get                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget border width get      */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    return_width                          Pointer to destination for    */
/*                                            widget border width         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_border_width_get           Actual widget border width    */
/*                                            get function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_border_width_get(GX_WIDGET *widget, GX_VALUE *return_width)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (return_width == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for valid widget.  */
    if (widget->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call actual widget border width get function.  */
    status = _gx_widget_border_width_get(widget, return_width);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_canvas_get                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget canvas get function   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    return_canvas                         Pointer to destination for    */
/*                                            widget's canvas             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_canvas_get                 Actual widget canvas get      */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_canvas_get(GX_WIDGET *widget, GX_CANVAS **return_canvas)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (return_canvas == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for valid widget.  */
    if (widget->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call actual widget canvas get function.  */
    status = _gx_widget_canvas_get(widget, return_canvas);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_child_detect                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget child detect function */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Pointer to parent widget      */
/*    child                                 Pointer to child widget       */
/*    return_detect                         Pointer to destination for    */
/*                                          detection                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_child_detect               Actual widget child detect    */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX application code                                               */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_child_detect(GX_WIDGET *parent, GX_WIDGET *child, GX_BOOL *return_detect)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((parent == GX_NULL) || (child == GX_NULL) || (return_detect == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid parent and child widget.  */
    if ((parent->type == 0) ||
        (child->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget child detect function.  */
    status = _gx_widget_child_detect(parent, child, return_detect);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_client_get                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget client get function   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    border_width                          width of widget border        */
/*    return_size                           Return rectangle pointer      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_width_get                  Get widget width              */
/*    _gx_widget_height_get                 Get widget height             */
/*    _gx_widget_client_get                 Actual widget client get      */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX draw functions                                                 */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_client_get(GX_WIDGET *widget, GX_VALUE border_width, GX_RECTANGLE *return_size)
{
UINT     status;
GX_VALUE widget_width;
GX_VALUE widget_height;

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (return_size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid widget border.  */
    _gx_widget_width_get(widget, &widget_width);
    _gx_widget_height_get(widget, &widget_height);
    if ((border_width > (widget_width / 2)) || (border_width > (widget_height / 2)))
    {
        return(GX_INVALID_VALUE);
    }

    /* Call actual widget client get function.  */
    status = _gx_widget_client_get(widget, border_width, return_size);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_color_get                                               */
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
/*    widget                                caller identify               */
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
/*    _gx_widget_color_get                  Actual widget color get call  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default draw funtions                                          */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_color_get(GX_WIDGET *widget, GX_RESOURCE_ID color_id, GX_COLOR *return_color)
{
UINT status;

    /* Check for invalid pointer.  */
    if ((return_color == GX_NULL) || (widget == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget color get.  */
    status = _gx_widget_color_get(widget, color_id, return_color);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_create                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget create function call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    name                                  Name of widget                */
/*    parent                                Parent widget control block   */
/*    style                                 Style of widget               */
/*    Id                                    Application-defined ID of     */
/*                                          the widget                    */
/*    size                                  Widget size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_create                     Actual widget create function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_create(GX_WIDGET *widget, const GX_CHAR *name, GX_WIDGET *parent,
                         ULONG style, USHORT Id, const GX_RECTANGLE *size, UINT widget_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for widget already created.  */
    if (widget->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid parent widget. */
    if (parent && (parent->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid control block size. */
    if (widget_block_size != sizeof(GX_WIDGET))
    {
        return(GX_INVALID_SIZE);
    }

    /* Call actual widget create function.  */
    status = _gx_widget_create(widget, name, parent, style, Id, size);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_created_test                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget created test function */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    return_test                           Pointer to the return value   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    GX_TRUE or GX_FALSE                                                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_created_test               Actual widget created test    */
/*                                          function                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX application code                                               */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_created_test(GX_WIDGET *widget, GX_BOOL *return_test)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (return_test == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual widget created test function.  */
    status = _gx_widget_created_test(widget, return_test);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_delete                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget delete function call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                               Widget control block           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                               Completion status              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_delete                    Actual widget delete function  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_delete(GX_WIDGET *widget)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    if(widget->style & (GX_STYLE_TEXT_COPY | GX_STATUS_DYNAMICALLY_ALLOCATED))
    {
        if(_gx_system_memory_free == GX_NULL)
        {
            return(GX_SYSTEM_MEMORY_ERROR);
        }
    }

    /* Call actual widget destroy function.  */
    status = _gx_widget_delete(widget);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_detach                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget detach function call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_detach                     Actual widget detach function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_detach(GX_WIDGET *widget)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget detach function.  */
    status = _gx_widget_detach(widget);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_draw_set                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget draw set function     */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                              Widget control block            */
/*    draw_func                           pointer to drawing function     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                              Completion status               */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_draw_set                 Actual widget draw set function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    application                                                         */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_draw_set(GX_WIDGET *widget, void (*draw_func)(GX_WIDGET *))
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (draw_func == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget draw set function.  */
    status = _gx_widget_draw_set(widget, draw_func);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_event_generate                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget event generate        */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    event_type                            Type of event                 */
/*    value                                 Additional event information  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_generate             Actual widget event generate  */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_event_generate(GX_WIDGET *widget, USHORT event_type, LONG value)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget event generate function.  */
    status = _gx_widget_event_generate(widget, event_type, value);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_event_process                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget event process         */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_process              Actual widget event process   */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_event_process(GX_WIDGET *widget, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget event process function.  */
    status = _gx_widget_event_process(widget, event_ptr);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_event_process_set                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget event process set     */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    event_processing_function             pointer to event process      */
/*                                            function                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_process_set          Actual widget event process   */
/*                                            set function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_event_process_set(GX_WIDGET *widget, UINT (*event_processing_function)(GX_WIDGET *, GX_EVENT *))
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (event_processing_function == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget event process set function.  */
    status = _gx_widget_event_process_set(widget, event_processing_function);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_event_to_parent                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget event to parent call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_to_parent            Actual widget event to parent */
/*                                            function call               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_event_to_parent(GX_WIDGET *widget, GX_EVENT *event_ptr)
{
UINT status = GX_SUCCESS;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((!widget) || (!event_ptr))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget event to parent function */
    status = _gx_widget_event_to_parent(widget, event_ptr);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_fill_color_set                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget background set        */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    normal_color                          Normal fill color id          */
/*    selected_color                        Selected fill color id        */
/*    disabled_color                        Disabled fill color id        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_fill_color_set             Actual widget fill color      */
/*                                            set function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_fill_color_set(GX_WIDGET *widget,
                                 GX_RESOURCE_ID normal_color,
                                 GX_RESOURCE_ID selected_color,
                                 GX_RESOURCE_ID disabled_color)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!widget)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget color set function.  */
    status = _gx_widget_fill_color_set(widget, normal_color, selected_color, disabled_color);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_find                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget find function call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Pointer to parent widget to   */
/*                                          start search from             */
/*    widget_id                             Widget ID                     */
/*    search_depth                          how many generations to search*/
/*    return_widget                         Pointer to destination for    */
/*                                            found widget                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_find                       Actual widget find function   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_find(GX_WIDGET *parent, USHORT widget_id, INT search_depth, GX_WIDGET **return_widget)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((parent == GX_NULL) || (return_widget == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (parent->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget find function.  */
    status = _gx_widget_find(parent, widget_id, search_depth, return_widget);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_first_child_get                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the gx_widget_first_child_get    */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Pointer to parent widget to   */
/*                                          start search from             */
/*    return_widget                         Pointer to destination for    */
/*                                            found widget                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_first_child_get            Actual widget_first_child_get */
/*                                            function.                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_first_child_get(GX_WIDGET *parent, GX_WIDGET **return_widget)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((parent == GX_NULL) || (return_widget == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (parent->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget find function.  */
    status = _gx_widget_first_child_get(parent, return_widget);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_focus_next                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function moves focus to the next widget in the nav order       */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget to move     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_focus_next                 Actual widget focus move      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_focus_next(GX_WIDGET *widget)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget focus move function.  */
    status = _gx_widget_focus_next(widget);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_focus_previous                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function moves focus to the previous widget in the nav order   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget to move     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_focus_previous             Actual widget focus move      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_focus_previous(GX_WIDGET *widget)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget focus move function.  */
    status = _gx_widget_focus_previous(widget);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_font_get                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system font get call.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block pointer  */
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
/*    _gx_widget_font_get                   Actual widget font get call   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default drawing functions                                      */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_font_get(GX_WIDGET *widget, GX_RESOURCE_ID resource_id, GX_FONT **return_font)
{
UINT status;

    /* Check for invalid pointer.  */
    if (return_font == GX_NULL || widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual system font get.  */
    status = _gx_widget_font_get(widget, resource_id, return_font);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_free                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in widget free call.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_free                       Actual widget free call       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_free(GX_WIDGET *widget)
{
    if (!widget)
    {
        return GX_PTR_ERROR;
    }
    if (_gx_system_memory_free == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }
    return _gx_widget_free(widget);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_front_move                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget front move function   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget to move     */
/*    return_moved                          Pointer to destination for    */
/*                                            indication widget was moved */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_front_move                 Actual widget front move      */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_front_move(GX_WIDGET *widget, GX_BOOL *return_moved)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget front move function.  */
    status = _gx_widget_front_move(widget, return_moved);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_height_get                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget height get function   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    return_height                         Pointer to destination for    */
/*                                            widget height               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_height_get                 Actual widget height get      */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_height_get(GX_WIDGET *widget, GX_VALUE *return_height)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (return_height == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.*/
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget height get function.  */
    status = _gx_widget_height_get(widget, return_height);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_hide                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget hide function call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_hide                       Actual widget hide function   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_hide(GX_WIDGET *widget)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.*/
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget hide function.  */
    status = _gx_widget_hide(widget);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_last_child_get                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the gx_widget_last_child_get     */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Pointer to parent widget to   */
/*                                          start search from             */
/*    return_widget                         Pointer to destination for    */
/*                                            found widget                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_last_child_get             Actual widget_last_child_get  */
/*                                            function.                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_last_child_get(GX_WIDGET *parent, GX_WIDGET **return_widget)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((parent == GX_NULL) || (return_widget == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (parent->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget find function.  */
    status = _gx_widget_last_child_get(parent, return_widget);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_next_sibling_get                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the gx_widget_next_sibling_get   */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    current                               Pointer to current widget to  */
/*                                            start search from           */
/*    return_widget                         Pointer to destination for    */
/*                                            found widget                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_next_sibling_get           Actual widget_next_sibling_get*/
/*                                            function.                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_next_sibling_get(GX_WIDGET *current, GX_WIDGET **return_widget)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((current == GX_NULL) || (return_widget == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (current->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget find function.  */
    status = _gx_widget_next_sibling_get(current, return_widget);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_parent_get                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the gx_widget_parent_get         */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    current                               Pointer to current widget to  */
/*                                            start search from           */
/*    return_widget                         Pointer to destination for    */
/*                                            found widget                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_parent_get                 Actual widget_parent_get      */
/*                                            function.                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_parent_get(GX_WIDGET *current, GX_WIDGET **return_widget)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((current == GX_NULL) || (return_widget == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (current->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget find function.  */
    status = _gx_widget_parent_get(current, return_widget);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_pixelmap_get                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system pixelmap get call.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                called widget control block   */
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
/*    _gx_widget_pixelmap_get               Actual system pixelmap get    */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_pixelmap_get(GX_WIDGET *widget, GX_RESOURCE_ID resource_id, GX_PIXELMAP **return_pixelmap)
{
UINT status;

    /* Check for invalid pointer.  */
    if (return_pixelmap == GX_NULL || widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.*/
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual system pixelmap get.  */
    status = _gx_widget_pixelmap_get(widget, resource_id, return_pixelmap);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_previous_sibling_get                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the                              */
/*    gx_widget_previous_sibling_get function call.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    current                               Pointer to current widget to  */
/*                                            start search from           */
/*    return_widget                         Pointer to destination for    */
/*                                            found widget                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_previous_sibling_get       Actual                        */
/*                                            widget_previous_sibling_get */
/*                                            function.                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_previous_sibling_get(GX_WIDGET *current, GX_WIDGET **return_widget)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((current == GX_NULL) || (return_widget == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (current->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget find function.  */
    status = _gx_widget_previous_sibling_get(current, return_widget);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_resize                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget resize function call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    new_size                              New widget size               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_resize                     Actual widget resize function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_resize(GX_WIDGET *widget, GX_RECTANGLE *new_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (new_size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget resize function.  */
    status = _gx_widget_resize(widget, new_size);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_shift                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget shift function call.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    x_shift                               Number of pixels to shift     */
/*                                            on x-axis                   */
/*    y_shift                               Number of pixels to shift     */
/*                                            on y-axis                   */
/*    mark_dirty                            GX_TRUE to indicate dirty,    */
/*                                            otherwise GX_FALSE          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_shift                      Actual widget shift function  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_shift(GX_WIDGET *widget, GX_VALUE x_shift, GX_VALUE y_shift, GX_BOOL mark_dirty)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget shift function.  */
    status = _gx_widget_shift(widget, x_shift, y_shift, mark_dirty);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_show                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget show function call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_show                       Actual widget show function   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_show(GX_WIDGET *widget)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    if (widget->type != GX_TYPE_ROOT_WINDOW)
    {
        if (widget->parent == GX_NULL)
        {
            /* cannot show an un-attached widget */
            return GX_INVALID_WIDGET;
        }
        if (!(widget->parent->status & GX_STATUS_VISIBLE))
        {
            /* cannot show a widget whose parent is not visible */
            return GX_INVALID_WIDGET;
        }
    }

    /* Call actual widget show function.  */
    status = _gx_widget_show(widget);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_status_add                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget status add function   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    status                                Status to add                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_status_add                 Actual widget status add      */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_status_add(GX_WIDGET *widget, ULONG status)
{
UINT ret;

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget status add function.  */
    ret = _gx_widget_status_add(widget, status);

    /* Return successful completion.  */
    return(ret);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_status_get                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget status get function   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    return_status                         Pointer to destination for    */
/*                                            status of widget            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_status_get                 Actual widget status get      */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_status_get(GX_WIDGET *widget, ULONG *return_status)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (return_status == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget status add function.  */
    status = _gx_widget_status_get(widget, return_status);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_status_remove                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget status remove         */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    status                                Status to remove              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_status_remove              Actual widget status remove   */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_status_remove(GX_WIDGET *widget, ULONG status)
{
UINT return_value;

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget status remove function.  */
    return_value = _gx_widget_status_remove(widget, status);

    /* Return successful completion.  */
    return(return_value);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_status_test                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget status test function  */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    status                                Status to test                */
/*    return_test                           Pointer to destination for    */
/*                                          result of test                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_status_test                Actual widget status test     */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_status_test(GX_WIDGET *widget, ULONG status, GX_BOOL *return_test)
{
UINT return_value;

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (return_test == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget status test function.  */
    return_value = _gx_widget_status_test(widget, status, return_test);

    /* Return successful completion.  */
    return(return_value);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_string_get_ext                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in widget_string_get_ext call.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                called widget control block   */
/*    string_id                             String resource ID            */
/*    return_string                         Pointer to string             */
/*                                            destination pointer         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_string_get_ext             Actual widget string get      */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT    _gxe_widget_string_get_ext(GX_WIDGET *widget, GX_RESOURCE_ID string_id, GX_STRING *return_string)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (return_string == GX_NULL || widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.*/
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual system pixelmap get.  */
    status = _gx_widget_string_get_ext(widget, string_id, return_string);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_style_add                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget style add function    */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    style                                 New style to add              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_style_add                  Actual widget style add       */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_style_add(GX_WIDGET *widget, ULONG style)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget style add function.  */
    status = _gx_widget_style_add(widget, style);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_style_get                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget style get function    */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    return_style                          Pointer to destination for    */
/*                                            updated style               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_style_get                  Actual widget style get       */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_style_get(GX_WIDGET *widget, ULONG *return_style)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (return_style == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget style add function.  */
    status = _gx_widget_style_get(widget, return_style);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_style_remove                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget style remove function */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                               Pointer to widget              */
/*    style                                Style to remove                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                               Completion status              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_style_remove              Actual widget style remove     */
/*                                           function                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_style_remove(GX_WIDGET *widget, ULONG style)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget style remove function.  */
    status = _gx_widget_style_remove(widget, style);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_style_set                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget style set function    */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    style                                 New style                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_style_set                  Actual widget style set       */
/*                                            function call               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_widget_style_set(GX_WIDGET *widget, ULONG style)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!widget)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    status = _gx_widget_style_set(widget, style);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_text_blend_ext                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget text blend function   */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    tColor                                Text Color                    */
/*    font_id                               Font Id                       */
/*    string                                Drawing string                */
/*    x_offset                              Drawing position adjustment   */
/*    y_offset                              Drawing position adjustment   */
/*    alpha                                 Blending value 0-255          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_text_blend                 Actual widget text blend      */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT    _gxe_widget_text_blend_ext(GX_WIDGET *widget, UINT tColor, UINT font_id,
                                   const GX_STRING *string, INT x_offset, INT y_offset, UCHAR alpha)
{
UINT status;
UINT text_length = 0;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    if (string)
    {
        if (string->ptr)
        {
            status = _gx_utility_string_length_check(string->ptr, &text_length, string->length);

            if (status != GX_SUCCESS)
            {
                return status;
            }
        }

        if (text_length != string->length)
        {
            return GX_INVALID_STRING_LENGTH;
        }
    }

    /* Call actual widget text blend function.  */
    status = _gx_widget_text_blend_ext(widget, tColor, font_id, string, x_offset, y_offset, alpha);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_top_visible_child_find                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the                              */
/*    gx_widget_top_visible_child_find function call.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                               Pointer to parent widget to    */
/*                                            start search from           */
/*    return_widget                         Pointer to destination for    */
/*                                            found widget                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_top_visible_child_find     Actual                        */
/*                                            widget_previous_sibling_get */
/*                                            function.                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_top_visible_child_find(GX_WIDGET *parent, GX_WIDGET **return_widget)
{
UINT status;
    /* Check for invalid input pointers.  */
    if ((parent == GX_NULL) || (return_widget == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (parent->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget find function.  */
    status = _gx_widget_top_visible_child_find(parent, return_widget);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_find                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget find function call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    parent                                Pointer to parent widget to   */
/*                                            start search from           */
/*    widget_id                             Widget ID                     */
/*    return_widget                         Pointer to destination for    */
/*                                            found widget                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_find                       Actual widget find function   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_type_find(GX_WIDGET *parent, USHORT widget_id, GX_WIDGET **return_widget)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((parent == GX_NULL) || (return_widget == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (parent->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget find function.  */
    status = _gx_widget_type_find(parent, widget_id, return_widget);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_widget_width_get                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the widget width get function    */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget             */
/*    return_width                          Pointer to destination for    */
/*                                            widget width                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_width_get                  Actual widget width get       */
/*                                          function                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_widget_width_get(GX_WIDGET *widget, GX_VALUE *return_width)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (return_width == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget width get function.  */
    status = _gx_widget_width_get(widget, return_width);

    /* Return successful completion.  */
    return(status);
}
