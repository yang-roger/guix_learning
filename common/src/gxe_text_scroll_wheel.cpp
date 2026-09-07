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
/**   Text Scroll Wheel Management (Scroll Wheel)                         */
/**                                                                       */
/**************************************************************************/

#include "gx_text_scroll_wheel.h"

GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_text_scroll_wheel_callback_set_ext                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the scroll wheel callback set    */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget control block          */
/*    callback                              pointer to event process      */
/*                                            function                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_scroll_wheel_callback_set_ext                              */
/*                                          Actual wheel event process    */
/*                                            set function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_text_scroll_wheel_callback_set_ext(GX_TEXT_SCROLL_WHEEL *widget, UINT (*callback)(GX_TEXT_SCROLL_WHEEL *, INT, GX_STRING *))
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((widget == GX_NULL) || (callback == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual widget event process set function.  */
    status = _gx_text_scroll_wheel_callback_set_ext(widget, callback);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_text_scroll_wheel_create                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in text scroll wheel create call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                               Scroll wheel control block      */
/*    name                                Name of widget                  */
/*    parent                              Parent widget control block     */
/*    total_rows                          Total rows of the scroll wheel  */
/*    style                               Style of widget                 */
/*    Id                                  Application-defined ID of the   */
/*                                          the widget                    */
/*    size                                Widget size                     */
/*    control_block_size                  Size of the scroll wheel        */
/*                                          control block                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_scroll_wheel_create          Actual text scroll wheel      */
/*                                            create call                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_text_scroll_wheel_create(GX_TEXT_SCROLL_WHEEL *wheel, const GX_CHAR *name, GX_WIDGET *parent, INT total_rows,
                                    ULONG style, USHORT Id, const GX_RECTANGLE *size, UINT control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if ((wheel == GX_NULL) || (size == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid control block size. */
    if (control_block_size != sizeof(GX_TEXT_SCROLL_WHEEL))
    {
        return GX_INVALID_SIZE;
    }

    /* Check for widget already created.  */
    if (wheel->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid widget. */
    if (parent && (parent->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    status = _gx_text_scroll_wheel_create(wheel, name, parent, total_rows, style, Id, size);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_text_scroll_wheel_event_process                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in text scroll wheel event process  */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Text scroll wheel control     */
/*                                            block                       */
/*    event_ptr                             Event to be processed         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_scroll_wheel_event_process   Actual text scroll wheel      */
/*                                            event process call          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_text_scroll_wheel_event_process(GX_TEXT_SCROLL_WHEEL *wheel, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if ((wheel == GX_NULL) || (event_ptr == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget.  */
    if (wheel->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    status = _gx_text_scroll_wheel_event_process(wheel, event_ptr);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_text_scroll_wheel_font_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in text scroll wheel base font set. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    normal_font                           Normal font id to be set      */
/*    selected_font                         Selected font id to be set    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_scroll_wheel_font_set        Actual text scroll wheel      */
/*                                            font set call               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_text_scroll_wheel_font_set(GX_TEXT_SCROLL_WHEEL *wheel, GX_RESOURCE_ID normal_font, GX_RESOURCE_ID selected_font)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (wheel == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget. */
    if (wheel->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    status = _gx_text_scroll_wheel_font_set(wheel, normal_font, selected_font);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_text_scroll_wheel_text_color_set                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in scroll wheel base text color set.*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                               Scroll wheel control block      */
/*    normal_text_color                   Normal text color id            */
/*    selected_text_color                 Selected text color id          */
/*    disabled_text_color                 Disabled text color id          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_scroll_wheel_text_color_set  Actual text scroll wheel text */
/*                                            color set call              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_text_scroll_wheel_text_color_set(GX_TEXT_SCROLL_WHEEL *wheel,
                                           GX_RESOURCE_ID normal_text_color,
                                           GX_RESOURCE_ID selected_text_color,
                                           GX_RESOURCE_ID disabled_text_color)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (wheel == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget. */
    if (wheel->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    status = _gx_text_scroll_wheel_text_color_set(wheel, normal_text_color, selected_text_color, disabled_text_color);

    return status;
}
