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
/**   Radial Slider Management (Slider)                                   */
/**                                                                       */
/**************************************************************************/

#include "gx_radial_slider.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_slider_anchor_angles_set                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the radial slider anchor angles  */
/*    set function call.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    anchor_angles                         The angle list to set, which  */
/*                                            defines anchor angles for   */
/*                                            radial slider               */
/*    anchor_count                          The count of the anchor angles*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_slider_anchor_angles_set   Actual radial slider anchor   */
/*                                            angles set function         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_radial_slider_anchor_angles_set(GX_RADIAL_SLIDER *slider, GX_VALUE *anchor_angles, USHORT anchor_count)
{
UINT status;

    /* Check for invalid input pointers.  */
    if (slider == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid anchor count. */
    if ((anchor_angles) && (anchor_count == 0))
    {
        return(GX_INVALID_VALUE);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial slider anchor angles set function.  */
    status = _gx_radial_slider_anchor_angles_set(slider, anchor_angles, anchor_count);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_slider_angle_set                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the radial slider angle set      */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    new_angle                             New angle value to be set     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_slider_angle_set           Actual radial slider angle    */
/*                                            set function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_radial_slider_angle_set(GX_RADIAL_SLIDER *slider, GX_VALUE new_angle)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (slider == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial slider value set function.  */
    status = _gx_radial_slider_angle_set(slider, new_angle);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_slider_animation_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the radial slider animation set  */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    steps                                 Total steps for one animation */
/*    delay                                 Delay time for every step     */
/*    animation_style                       Easing function type          */
/*    animation_update_callback             Function that to be called    */
/*                                          after each animation step,    */
/*                                          could be NULL                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_slider_animation_set       Actual radial slider          */
/*                                            animation set function      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_radial_slider_animation_set(GX_RADIAL_SLIDER *slider, USHORT steps, USHORT delay, USHORT animation_style,
                                      void (*animation_update_callback)(GX_RADIAL_SLIDER *slider))
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (slider == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial slider animation set function.  */
    status = _gx_radial_slider_animation_set(slider, steps, delay, animation_style, animation_update_callback);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_slider_animation_start                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the radial slider animation      */
/*    start function call.                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    target_angle                          Target angle value            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_slider_animation_start     Actual radial slider animation*/
/*                                            value set function          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_radial_slider_animation_start(GX_RADIAL_SLIDER *slider, GX_VALUE target_angle)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (slider == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial slider animation start function.  */
    status = _gx_radial_slider_animation_start(slider, target_angle);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_slider_create                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the radial slider create         */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    name                                  Name of radial slider         */
/*    parent                                Parent widget control block   */
/*    info                                  Pointer to radial slider info */
/*    style                                 Style of prompt               */
/*    slider_id                             Application-defined ID of     */
/*                                            radial slider               */
/*    size                                  Widget size                   */
/*    control_block_size                    Size of the radial slider     */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_slider_create              Actual radial slider create   */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_radial_slider_create(GX_RADIAL_SLIDER *slider, const GX_CHAR *name, GX_WIDGET *parent,
                               GX_RADIAL_SLIDER_INFO *info, ULONG style, USHORT slider_id,
                               const GX_RECTANGLE *size, UINT control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((slider == GX_NULL) || (size == GX_NULL) || (info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for widget already created.  */
    if (slider->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid widget control block size. */
    if (control_block_size != sizeof(GX_RADIAL_SLIDER))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for invalid parent widget. */
    if (parent && (parent->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial slider create function.  */
    status = _gx_radial_slider_create(slider, name, parent, info, style, slider_id, size);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_slider_event_process                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the radial slider event process  */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    event_ptr                             Pointer to event process      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_slider_event_process       Actual radial slider event    */
/*                                            process function            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_radial_slider_event_process(GX_RADIAL_SLIDER *slider, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((slider == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial slider event process function.  */
    status = _gx_radial_slider_event_process(slider, event_ptr);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_slider_info_get                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the radial slider info get       */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    info                                  Retrieved radial slider info  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_slider_info_get            Actual radial slider info get */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_radial_slider_info_get(GX_RADIAL_SLIDER *slider, GX_RADIAL_SLIDER_INFO **info)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((slider == GX_NULL) || (info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial slider info get function.  */
    status = _gx_radial_slider_info_get(slider, info);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_slider_info_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the radial slider info set       */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    info                                  Pointer to radial slider info */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_slider_info_set            Actual radial slider info set */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_radial_slider_info_set(GX_RADIAL_SLIDER *slider, GX_RADIAL_SLIDER_INFO *info)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((slider == GX_NULL) || (info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial slider info set function.  */
    status = _gx_radial_slider_info_set(slider, info);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_radial_slider_pixelmap_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the radial slider pixelmap set   */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Radial slider control block   */
/*    background_pixelmap                   Resource id of background     */
/*                                            pixelmap                    */
/*    needle_pixelmap                       Resource id of needle pixelmap*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_slider_pixelmap_set        Actual radial slider pixelmap */
/*                                            set function                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_radial_slider_pixelmap_set(GX_RADIAL_SLIDER *slider, GX_RESOURCE_ID background_pixelmap, GX_RESOURCE_ID needle_pixelmap)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (slider == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual radial slider info set function.  */
    status = _gx_radial_slider_pixelmap_set(slider, background_pixelmap, needle_pixelmap);

    /* Return completion status.  */
    return(status);
}
