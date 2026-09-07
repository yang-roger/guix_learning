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
/**   Scroll Wheel Management (Scroll Wheel)                              */
/**                                                                       */
/**************************************************************************/

#include "gx_scroll_wheel.h"

GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scroll_wheel_create                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in scroll wheel create call.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    name                                  Name of widget                */
/*    parent                                Parent widget control block   */
/*    total_rows                            Total rows of the scroll wheel*/
/*    style                                 Style of widget               */
/*    Id                                    Application-defined ID of the */
/*                                            the widget                  */
/*    size                                  Widget size                   */
/*    control_block_size                    Size of the scroll wheel      */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_create               Actual scroll wheel           */
/*                                            create call                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_scroll_wheel_create(GX_SCROLL_WHEEL *wheel, const GX_CHAR *name, GX_WIDGET *parent, INT total_rows,
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
    if (control_block_size != sizeof(GX_SCROLL_WHEEL))
    {
        return GX_INVALID_SIZE;
    }

    /* Check for widget already created.  */
    if (wheel->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid parent widget. */
    if (parent && parent->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    status = _gx_scroll_wheel_create(wheel, name, parent, total_rows, style, Id, size);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scroll_wheel_event_process                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in scroll wheel event process.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    event_ptr                             Event to be processed         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_event_process        Actual scroll wheel event     */
/*                                            process                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_scroll_wheel_event_process(GX_SCROLL_WHEEL *wheel, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if ((wheel == GX_NULL) || (event_ptr == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    if (wheel->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    status = _gx_scroll_wheel_event_process(wheel, event_ptr);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scroll_wheel_gradient_alpha_set                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in scroll wheel gradient alpha set. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    start_alpha                           Start alpha                   */
/*    end_alpha                             End alpha                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_gradient_alpha_set   Actual scroll wheel gradient  */
/*                                          gradient alpha set call       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_scroll_wheel_gradient_alpha_set(GX_SCROLL_WHEEL *wheel, GX_UBYTE start_alpha, GX_UBYTE end_alpha)
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

    status = _gx_scroll_wheel_gradient_alpha_set(wheel, start_alpha, end_alpha);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scroll_wheel_row_height_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in scroll wheel row height set.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    vertical_list                         Vertical list control block   */
/*    count                                 Number of rows                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_row_height_set       Actual scroll wheel row height*/
/*                                            set call                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_scroll_wheel_row_height_set(GX_SCROLL_WHEEL *wheel, GX_VALUE row_height)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (wheel == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (wheel->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    if (row_height <= 0)
    {
        return GX_INVALID_VALUE;
    }

    status = _gx_scroll_wheel_row_height_set(wheel, row_height);
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scroll_wheel_selected_background_set                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in scroll wheel selected background */
/*    set.                                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    selected_bg                           Selected background to be set */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_selecgted_background_set                           */
/*                                          Actual scroll wheel selected  */
/*                                            background set call         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_scroll_wheel_selected_background_set(GX_SCROLL_WHEEL *wheel, GX_RESOURCE_ID selected_bg)
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

    status = _gx_scroll_wheel_selected_background_set(wheel, selected_bg);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scroll_wheel_selected_get                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in scroll wheel selected get.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    row                                   Selected row to be retrieved  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_selected_get         Actual scroll wheel selected  */
/*                                            get call                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_scroll_wheel_selected_get(GX_SCROLL_WHEEL *wheel, INT *row)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if ((wheel == GX_NULL) || (row == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget. */
    if (wheel->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    status = _gx_scroll_wheel_selected_get(wheel, row);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scroll_wheel_selected_set                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in scroll wheel selected set.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    row                                   Selected row to be set        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_selected_set         Actual scroll wheel selected  */
/*                                            set call                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_scroll_wheel_selected_set(GX_SCROLL_WHEEL *wheel, INT row)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (wheel == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (wheel->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    status = _gx_scroll_wheel_selected_set(wheel, row);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scroll_wheel_speed_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in scroll wheel speed set.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    wheel                                 Scroll wheel control block    */
/*    start_speed_rate                      The rate of start speed to    */
/*                                            flick speed.                */
/*    end_speed_rate                        The rate of end speed to      */
/*                                            flick speed.                */
/*    max_steps                             Max steps for scrolling       */
/*    delay                                 Delay time of each step       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_speed_set            Actual scroll wheel speed set */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_scroll_wheel_speed_set(GX_SCROLL_WHEEL *wheel, GX_FIXED_VAL start_speed_rate, GX_FIXED_VAL end_speed_rate,
    GX_VALUE max_steps, GX_VALUE delay)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (wheel == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (wheel->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    if (delay <= 0)
    {
        return GX_INVALID_VALUE;
    }

    status = _gx_scroll_wheel_speed_set(wheel, start_speed_rate, end_speed_rate, max_steps, delay);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scroll_wheel_total_rows_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in scroll wheel total rows set.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    total_rows                            Total rows of the scroll wheel*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_total_rows_set       Actual scroll wheel total rows*/
/*                                            set call                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_scroll_wheel_total_rows_set(GX_SCROLL_WHEEL *wheel, INT total_rows)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (wheel == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (wheel->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    if (total_rows < 0)
    {
        return GX_INVALID_VALUE;
    }

    status = _gx_scroll_wheel_total_rows_set(wheel, total_rows);

    return status;
}
