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
/**   Circular Gauge Management (Circular Gauge)                          */
/**                                                                       */
/**************************************************************************/

#include "gx_circular_gauge.h"

GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_circular_gauge_angle_get                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in circular gauge angle range get   */
/*      call.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    circular_gauge                        Circular gauge control block  */
/*    start_angle                           Retrieved current angle.      */
/*    target_angle                          Retrieved target angle.       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_circular_gauge_angle_get          Actual circular gauge angle   */
/*                                          get call.                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_circular_gauge_angle_get(GX_CIRCULAR_GAUGE *circular_gauge, INT *angle)
{
UINT status;

    /* Check for invalid input pointers.  */
    if (circular_gauge == GX_NULL || angle == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual gauge angle get function.  */
    status = _gx_circular_gauge_angle_get(circular_gauge, angle);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_circular_gauge_angle_set                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in circular gauge angle set call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    circular_gauge                        Circular gauge control block  */
/*    start_angle                           New current angle             */
/*    target_angle                          New target angle              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_circular_gauge_angle_range_set    Actual circular gauge angle   */
/*                                            range set call.             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_circular_gauge_angle_set(GX_CIRCULAR_GAUGE *circular_gauge, INT angle)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (circular_gauge == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual gauge angle range set function.  */
    status = _gx_circular_gauge_angle_set(circular_gauge, angle);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_circular_gauge_angle_range_set                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in circular gauge animation steps   */
/*      set call.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    circular_gauge                        Circular gauge control block  */
/*    info                                  New circular gauge info       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    circular_gauge                        Circular gauge control block. */
/*    animation_steps                       New animation steps.          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

UINT _gxe_circular_gauge_animation_set(GX_CIRCULAR_GAUGE *circular_gauge, INT steps, INT delay)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (circular_gauge == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (steps > 0 && delay <= 0)
    {
        return GX_INVALID_VALUE;
    }

    /* Call the actual gauge angle range set function.  */
    status = _gx_circular_gauge_animation_set(circular_gauge, steps, delay);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_circular_gauge_create                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in circular gauge create call.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    circular_gauge                        Circular gauge control block  */
/*    name                                  Name of the widget            */
/*    parent                                Parent widget control block   */
/*    circular_gauge_info                   Pointer to circular gauge info*/
/*    style                                 Style of circular gauge       */
/*    circular_gauge_id                     Application-defined ID of     */
/*                                            circular gauge              */
/*    size                                  Dimensions of circular gauge  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_circular_gauge_create             Actual circular gauge create. */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_circular_gauge_create(GX_CIRCULAR_GAUGE *circular_gauge,
                                 const GX_CHAR *name,
                                 GX_WIDGET *parent,
                                 GX_CIRCULAR_GAUGE_INFO *circular_gauge_info,
                                 GX_RESOURCE_ID background,
                                 ULONG style,
                                 USHORT circular_gauge_id,
                                 GX_VALUE xpos, GX_VALUE ypos, UINT checkbox_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((circular_gauge == GX_NULL) || (circular_gauge_info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (checkbox_control_block_size != sizeof(GX_CIRCULAR_GAUGE))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (circular_gauge->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Call the actual gauge create function.  */
    status = _gx_circular_gauge_create(circular_gauge, name, parent,
                                       circular_gauge_info, background,
                                       style, circular_gauge_id, xpos, ypos);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_circular_gauge_event_process                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the circular gauge event process */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gauge                                 Pointer to circualr gauge     */
/*                                           control block                */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_circular_gauge_event_process      Actual circular gauge event   */
/*                                            process function            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_circular_gauge_event_process(GX_CIRCULAR_GAUGE *gauge, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((gauge == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual circular gauge event process function.  */
    status = _gx_circular_gauge_event_process(gauge, event_ptr);

    /* Return completion status.  */
    return status;
}
