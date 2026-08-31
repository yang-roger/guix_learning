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
/**   Slider Management (Slider)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_slider.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_slider_create                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the slider create call.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider control block          */
/*    name                                  Name of prompt                */
/*    parent                                Parent widget control block   */
/*    tick_count                            Number of slider ticks        */
/*    slider_info                           Pointer to slider info        */
/*    style                                 Style of slider               */
/*    slider_id                             Application-defined ID of     */
/*                                            slider                      */
/*    size                                  Dimensions of slider          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_create                     Actual slider create call     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_slider_create(GX_SLIDER *slider, const GX_CHAR *name, GX_WIDGET *parent, INT tick_count,
                         GX_SLIDER_INFO *slider_info, ULONG style, USHORT slider_id,
                         const GX_RECTANGLE *size, UINT slider_control_block_size)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((slider == GX_NULL) || (size == GX_NULL) || (slider_info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid control block size. */
    if (slider_control_block_size != sizeof(GX_SLIDER))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (slider->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid widget.  */
    if (parent && (parent->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual slider create function.  */
    status = _gx_slider_create(slider, name, parent, tick_count, slider_info, style, slider_id, size);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_slider_event_process                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in slider event process call.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider widget control block   */
/*    event_ptr                             Pointer to event process      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_event_process              Actual slider event call      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_slider_event_process(GX_SLIDER *slider, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((slider == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual slider event process function.  */
    status = _gx_slider_event_process(slider, event_ptr);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_slider_info_set                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the slider info set function     */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Widget control block          */
/*    info                                  Slider info block             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_slider_info_set(GX_SLIDER *slider, GX_SLIDER_INFO *info)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((slider == GX_NULL) || (info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual slider info set function. */
    status = _gx_slider_info_set(slider, info);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_slider_needle_position_get                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in slider needle position get call. */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider widget control block   */
/*    slider_info                           Pointer to slider information */
/*                                            structure defining the      */
/*                                            slider limits, needle size  */
/*                                            and offset, and other       */
/*                                            slider parameters.          */
/*    return_position                       Pointer to destination for    */
/*                                            needle position             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_needle_position_get        Actual slider needle position */
/*                                            get call                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_slider_needle_position_get(GX_SLIDER *slider, GX_SLIDER_INFO *slider_info, GX_RECTANGLE *return_position)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((slider == GX_NULL) || (slider_info == GX_NULL) || (return_position == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid slider info.  */
    if ((slider_info->current_val < slider_info->min_val) ||
        (slider_info->current_val > slider_info->max_val) ||
        (slider_info->min_val >= slider_info->max_val))
    {
        return(GX_INVALID_VALUE);
    }

    /* Call actual slider needle position get call.  */
    status = _gx_slider_needle_position_get(slider, slider_info, return_position);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_slider_travel_get                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in slider travel get call.          */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider widget control block   */
/*    info                                  Pointer to slider info        */
/*                                            structure                   */
/*    return_min_travel                     Pointer to destination for    */
/*                                            for minimum travel value    */
/*    return_max_travel                     Pointer to destination for    */
/*                                            for maximum travel value    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_travel_get                 Actual slider travel get      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_slider_travel_get(GX_SLIDER *slider, GX_SLIDER_INFO *info, INT *return_min_travel, INT *return_max_travel)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((slider == GX_NULL) || (info == GX_NULL) || (return_min_travel == GX_NULL) || (return_max_travel == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid slider info.  */
    if ((info->current_val < info->min_val) ||
        (info->current_val > info->max_val) ||
        (info->min_val >= info->max_val))
    {
        return(GX_INVALID_VALUE);
    }

    /* Call actual slider travel get.  */
    status = _gx_slider_travel_get(slider, info, return_min_travel, return_max_travel);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_slider_value_calculate                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in slider value calculate call.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider widget control block   */
/*    info                                  Pointer to slider info        */
/*    new_position                          New slider position           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*  _gx_slider_value_calculate              Actual slider value calculate */
/*                                            call                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_slider_value_calculate(GX_SLIDER *slider, GX_SLIDER_INFO *info, INT new_position)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((slider == GX_NULL) || (info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid slider info.  */
    if ((info->current_val < info->min_val) ||
        (info->current_val > info->max_val) ||
        (info->min_val >= info->max_val))
    {
        return(GX_INVALID_VALUE);
    }

    /* Call actual slider value caculate function.  */
    status = _gx_slider_value_calculate(slider, info, new_position);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_slider_value_set                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in slider value set call            */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider widget control block   */
/*    info                                  Pointer to slider info        */
/*    new_value                             New slider value              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_value_set                  Actual slider value set call  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_slider_value_set(GX_SLIDER *slider, GX_SLIDER_INFO *info, INT new_value)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((slider == GX_NULL) || (info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual slider set function.  */
    status = _gx_slider_value_set(slider, info, new_value);

    return(status);
}

