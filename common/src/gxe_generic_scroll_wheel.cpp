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
/**   Scroll Wheel Management (Generic Scroll Wheel)                      */
/**                                                                       */
/**************************************************************************/

#include "gx_generic_scroll_wheel.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_generic_scroll_wheel_children_position                         */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the generic scroll wheel         */
/*    children position function call                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Generic scroll wheel control   */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_generic_scroll_wheel_children_position                          */
/*                                          Actual scroll wheel children  */
/*                                            position function call      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_generic_scroll_wheel_children_position(GX_GENERIC_SCROLL_WHEEL *wheel)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!wheel)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (wheel->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call actual generic scroll wheel children position call. */
    status = _gx_generic_scroll_wheel_children_position(wheel);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_generic_scroll_wheel_create                                    */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the generic scroll wheel create  */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    name                                  Name of widget                */
/*    parent                                Parent widget control block   */
/*    total_rows                            Total rows of the scroll wheel*/
/*    callback                              Callback function to create a */
/*                                            widget row                  */
/*    style                                 Style of widget               */
/*    id                                    Application-defined ID of the */
/*                                            the widget                  */
/*    size                                  Widget size                   */
/*    control_block_size                   Size of the widget scroll      */
/*                                            wheel control block         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gxe_generic_scroll_wheel_create      Actual generic scroll wheel   */
/*                                            create function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_generic_scroll_wheel_create(GX_GENERIC_SCROLL_WHEEL *wheel, const GX_CHAR *name,
                                       GX_WIDGET *parent, INT total_rows,
                                       void (*callback)(GX_GENERIC_SCROLL_WHEEL *, GX_WIDGET *, INT),
                                       ULONG style, USHORT id, const GX_RECTANGLE *size,
                                       UINT control_block_size)
{

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((wheel == GX_NULL) || (size == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    if (control_block_size != sizeof(GX_GENERIC_SCROLL_WHEEL))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for id is created.  */
    if (wheel->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for valid number of rows. */
    if (total_rows <= 0)
    {
        return GX_INVALID_VALUE;
    }

    /* Check for invalid widget. */
    if (parent && (parent->type == 0))
    {
        return GX_INVALID_WIDGET;
    }

    /* Call the actual generic scroll wheel create function.  */
    return _gx_generic_scroll_wheel_create(wheel, name, parent, total_rows, callback, style, id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_generic_scroll_wheel_event_process                             */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the generic scroll wheel event   */
/*    process function call.                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Generic scroll wheel control  */
/*                                            block                       */
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gxe_generic_scroll_wheel_event_process                             */
/*                                          Actual generic scroll wheel   */
/*                                            event process function      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_generic_scroll_wheel_event_process(GX_GENERIC_SCROLL_WHEEL *wheel, GX_EVENT *event_ptr)
{

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((wheel == GX_NULL) || (event_ptr == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid widget. */
    if (wheel->type == 0)
    {
        return GX_INVALID_WIDGET;
    }

    /* Call the actual generic scroll wheel event process function.  */
    return _gx_generic_scroll_wheel_event_process(wheel, event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_generic_scroll_wheel_row_height_set                            */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in generic scroll wheel row height  */
/*    set.                                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Generic scroll wheel control  */
/*                                            block                       */
/*    count                                 Number of rows                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_generic_scroll_wheel_row_height_set                             */
/*                                          Actual generic scroll wheel   */
/*                                            row height set call         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_generic_scroll_wheel_row_height_set(GX_GENERIC_SCROLL_WHEEL *wheel, GX_VALUE row_height)
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

    status = _gx_generic_scroll_wheel_row_height_set(wheel, row_height);
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_generic_scroll_wheel_total_rows_set                            */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in generic scroll wheel total rows  */
/*    set.                                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Generic scroll wheel control  */
/*                                            block                       */
/*    total_rows                            Total rows of the scroll wheel*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_generic_scroll_wheel_total_rows_set                             */
/*                                          Actual scroll wheel total rows*/
/*                                            set call                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_generic_scroll_wheel_total_rows_set(GX_GENERIC_SCROLL_WHEEL *wheel, INT total_rows)
{

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

    return _gx_generic_scroll_wheel_total_rows_set(wheel, total_rows);
}
