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
/**   Progress Bar Management (Radial Progress Bar)                       */
/**                                                                       */
/**************************************************************************/

#include "gx_radial_progress_bar.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radia_progress_bar_anchor_set                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in radial progress bar anchor set   */
/*      call.                                                             */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Radial Progress Bar control   */
/*                                            block                       */
/*    angle                                 Starting angle of the circular*/
/*                                            arc                         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_progress_bar_anchor_set    Actual radial progress bar    */
/*                                            anchor set call             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gxe_radial_progress_bar_anchor_set(GX_RADIAL_PROGRESS_BAR *progress_bar, GX_VALUE angle)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (progress_bar == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (progress_bar->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial progress bar value set function.  */
    status = _gx_radial_progress_bar_anchor_set(progress_bar, angle);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_progress_bar_create                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the radial progress bar create   */
/*      call.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*    name                                  Name of radial progress bar   */
/*    parent                                Pointer to parent widget      */
/*    info                                  Pointer to radial progress    */
/*                                            bar info.                   */
/*    style                                 Style of radial progress bar  */
/*    id                                    Application-defined ID of     */
/*                                            progress bar                */
/*    size                                  Dimensions of radial progress */
/*                                            bar                         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_progress_bar_create        Actual radial progress bar    */
/*                                            create call                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_radial_progress_bar_create(GX_RADIAL_PROGRESS_BAR *progress_bar,
                                      const GX_CHAR *name,
                                      GX_WIDGET *parent,
                                      GX_RADIAL_PROGRESS_BAR_INFO *info,
                                      ULONG style,
                                      USHORT id,
                                      UINT progress_bar_control_block_size)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((progress_bar == GX_NULL) || (info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget control block size.  */
    if (progress_bar_control_block_size != sizeof(GX_RADIAL_PROGRESS_BAR))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (progress_bar->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid parent widget.  */
    if (parent && (parent->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial progress bar create function.  */
    status = _gx_radial_progress_bar_create(progress_bar, name, parent, info, style, id);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_progress_bar_event_process                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in radial progress bar event process*/
/*      call.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Radial Progress Bar control   */
/*                                            block                       */
/*    event_ptr                             Pointer to event process      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_progress_bar_event_process Actual radial progress bar    */
/*                                            event process call          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_radial_progress_bar_event_process(GX_RADIAL_PROGRESS_BAR *progress_bar, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((progress_bar == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (progress_bar->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial progress bar event process function.  */
    status = _gx_radial_progress_bar_event_process(progress_bar, event_ptr);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_progress_bar_font_set                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in progress bar font set call.      */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*    font_id                               Resource ID of font           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_progress_bar_font_set      Actual radial progress bar    */
/*                                            font set call               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_radial_progress_bar_font_set(GX_RADIAL_PROGRESS_BAR *progress_bar, GX_RESOURCE_ID font_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (progress_bar == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (progress_bar->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial progress bar font set function.  */
    status = _gx_radial_progress_bar_font_set(progress_bar, font_id);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_progress_bar_info_set                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in radial progress bar info set     */
/*      call.                                                             */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    bar                                   Radial proress bar control    */
/*                                            block                       */
/*    info                                  Radial progess bar info block */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_progress_bar_info_set      The actual radial progress    */
/*                                            bar info set call           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gxe_radial_progress_bar_info_set(GX_RADIAL_PROGRESS_BAR *progress_bar, GX_RADIAL_PROGRESS_BAR_INFO *info)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((progress_bar == GX_NULL) || (info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (progress_bar->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual slider info set function. */
    status = _gx_radial_progress_bar_info_set(progress_bar, info);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_progress_bar_text_color_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in radial progress bar text color   */
/*      set call.                                                         */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Radial Progress Bar control   */
/*                                             block                      */
/*    normal_text_color_id                  Resource ID of the normal     */
/*                                             text color                 */
/*    selected_text_color_id                Resource ID of the selected   */
/*                                             text color                 */
/*    disabled_text_color_id                Resource ID of the disabled   */
/*                                             text color                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_progress_bar_text_color_set                              */
/*                                          Actual radial progress bar    */
/*                                            text color set call         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_radial_progress_bar_text_color_set(GX_RADIAL_PROGRESS_BAR *progress_bar,
                                              GX_RESOURCE_ID normal_text_color_id,
                                              GX_RESOURCE_ID selected_text_color_id,
                                              GX_RESOURCE_ID disabled_text_color_id)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (progress_bar == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (progress_bar->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial progress bar text color set function.  */
    status = _gx_radial_progress_bar_text_color_set(progress_bar, normal_text_color_id, selected_text_color_id, disabled_text_color_id);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radia_progress_bar_value_set                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in radial progress bar value set    */
/*      call.                                                             */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Radial Progress Bar control   */
/*                                            block                       */
/*    new_value                             New progress bar value        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_progress_bar_value_set     Actual radial progress bar    */
/*                                            value set call              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gxe_radial_progress_bar_value_set(GX_RADIAL_PROGRESS_BAR *progress_bar, GX_VALUE new_value)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (progress_bar == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (progress_bar->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial progress bar value set function.  */
    status = _gx_radial_progress_bar_value_set(progress_bar, new_value);

    /* Return completion status.  */
    return(status);
}
