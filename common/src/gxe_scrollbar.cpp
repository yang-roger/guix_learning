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
/**   Scrollbar Management (Scrollbar)                                    */
/**                                                                       */
/**************************************************************************/

#include "gx_scroll_bar.h"

#include "gx_window.h"

GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_horizontal_scrollbar_create                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the horizontal scrollbar create  */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             scroll control block          */
/*    name                                  Name of scroll                */
/*    parent                                parent window                 */
/*    appearance                            style of scroll bar           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_horizontal_scrollbar_create       Actual horizontal scrollbar   */
/*                                          create function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_horizontal_scrollbar_create(GX_SCROLLBAR *scrollbar, const GX_CHAR *name,
                                       GX_WINDOW *parent, GX_SCROLLBAR_APPEARANCE *appearance,
                                       ULONG style, UINT scrollbar_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (scrollbar == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (scrollbar_control_block_size != sizeof(GX_SCROLLBAR))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for id is created.  */
    if (scrollbar->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Call the actual horizontal scroll*/
    status = _gx_horizontal_scrollbar_create(scrollbar, name, parent, appearance, style);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_vertical_scrollbar_create                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the vertical scrollbar create    */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             Scrollbar control block       */
/*    name                                  Name of scrollbar             */
/*    parent                                Pointer to parent widget      */
/*    appearance                            Appearance of vertical        */
/*                                          scrollbar widget              */
/*    scrollbar_control_block_size          Size of the scrollbar control */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_vertical_scrollbar_create         Actual vertical scrollbar     */
/*                                          create function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_vertical_scrollbar_create(GX_SCROLLBAR *scrollbar, const GX_CHAR *name,
                                     GX_WINDOW *parent,
                                     GX_SCROLLBAR_APPEARANCE *appearance,
                                     ULONG style, UINT scrollbar_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (scrollbar == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Check for invalid control block size. */
    if (scrollbar_control_block_size != sizeof(GX_SCROLLBAR))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (scrollbar->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid parent widget. */
    if (parent && (parent->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual vertical list create function.  */
    status = _gx_vertical_scrollbar_create(scrollbar, name, parent, appearance, style);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scrollbar_event_process                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the scrollbar event process function.*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             Scrollbar control block       */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scrollbar_event_process           The actual scrollbar event    */
/*                                            process call                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_scrollbar_event_process(GX_SCROLLBAR *scrollbar, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((scrollbar == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (scrollbar->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual function.  */
    status = _gx_scrollbar_event_process(scrollbar, event_ptr);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scrollbar_limit_check                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    The service checks errors in the scrollbar limit check function.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             scrollbar control block       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_scrollbar_limit_check              The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_scrollbar_limit_check(GX_SCROLLBAR *scrollbar)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (scrollbar == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (scrollbar->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual function.  */
    status = _gx_scrollbar_limit_check(scrollbar);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scrollbar_reset                                                */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks errors in the scrollbar reset function call.    */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             Scrollbar control block       */
/*    info                                  Pointer to GX_SCROLL          */
/*                                            structure that defines the  */
/*                                            scrollbar limits, current   */
/*                                            value, and step/increment.  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status               Completion status                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scrollbar_reset     The actual function                         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_scrollbar_reset(GX_SCROLLBAR *scrollbar, GX_SCROLL_INFO *info)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((scrollbar == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (scrollbar->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for valid scroll info.  */
    if ((info != GX_NULL) && ((info->value > info->maximum) ||
                              (info->value < info->minimum)))
    {
        return(GX_INVALID_VALUE);
    }

    /* Call the actual function.  */
    status = _gx_scrollbar_reset(scrollbar, info);

    /* Return successful completion.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_scrollbar_value_set                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks errors in the scrollbar value set function call */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scrollbar                             Scrollbar control block       */
/*    value                                 New scrollbar value           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status               Completion status                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scrollbar_value_set The actual function                         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_scrollbar_value_set(GX_SCROLLBAR *scrollbar, INT value)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((scrollbar == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (scrollbar->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual function.  */
    status = _gx_scrollbar_value_set(scrollbar, value);

    /* Return successful completion.  */
    return(status);
}
