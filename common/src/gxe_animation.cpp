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
/**   Animation Management (Animation)                                    */
/**                                                                       */
/**************************************************************************/

#include "gx_animation.h"

#include "gx_display.h"
#include "gx_canvas.h"
#include "gx_widget.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_animation_canvas_define                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks error in animation canvas define function.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Animation control block       */
/*    canvas                                Pointer to animation canvas   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_animation_canvas_define           The actual animation canvas   */
/*                                            define function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT  _gxe_animation_canvas_define(GX_ANIMATION *animation, GX_CANVAS *canvas)
{
UINT  status = GX_SUCCESS;
GX_DISPLAY *display;
ULONG required_size;

    if (animation == GX_NULL ||
        canvas == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    display = canvas->display;
    if (display == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    required_size = (ULONG)(display->driver_row_pitch_get((USHORT) canvas->x_resolution));
    required_size = required_size * (ULONG) canvas->y_resolution;

    if (canvas ->memory_size < required_size)
    {
        return GX_INVALID_MEMORY_SIZE;
    }

    status = _gx_animation_canvas_define(animation, canvas);

    /* Return completion status code. */
    return(status);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_animation_create                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the animation create call.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to window control     */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_animation_create                  Actual animation create call. */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT  _gxe_animation_create(GX_ANIMATION *animation)
{
UINT status;

    /* Check for invalid input pointers.  */
    if (animation == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (animation->status != 0)
    {
        return GX_ALREADY_CREATED;
    }

    /* Call actual window create function.  */
    status = _gx_animation_create(animation);

    /* Return completion status.  */
    return(status);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_animation_delete                                               */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks error in animation delete call.                */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    target                                Pointer to animation control  */
/*                                            block                       */
/*    parent                                Pointer to animation parent   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_animation_delete                  Actual animation delete call  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gxe_animation_delete(GX_ANIMATION *target, GX_WIDGET *parent)
{
    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if ((target == GX_NULL) && (parent == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    return _gx_animation_delete(target, parent);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_drag_disable                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks error in animation slide disable call.         */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to animation control  */
/*                                            block                       */
/*    widget                                Pointer to widget control     */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_animation_drag_disable            Actual animation slide disable*/
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gxe_animation_drag_disable(GX_ANIMATION *animation, GX_WIDGET *widget)
{
    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if ((animation == GX_NULL) || (widget == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* animation drag is not enabled ? */
    if (animation->original_event_process_function == GX_NULL)
    {
        return GX_INVALID_ANIMATION;
    }

    return _gx_animation_drag_disable(animation, widget);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_animation_drag_enable                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for error in animation slide enable call.      */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to animation control  */
/*                                            block                       */
/*    widget                                Pointer to widget control     */
/*                                            block                       */
/*    info                                  Animation information         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_animation_slide_enable            Actual animation slide enable */
/*                                            call.                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gxe_animation_drag_enable(GX_ANIMATION *animation, GX_WIDGET *widget, GX_ANIMATION_INFO *info)
{
    /* Check for invalid pointer. */
    if ((animation == GX_NULL) ||
        (widget == GX_NULL) ||
        (info == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* animation drag is already enabled ? */
    if (animation->original_event_process_function != GX_NULL)
    {
        return GX_INVALID_ANIMATION;
    }

    if ((info->style & GX_ANIMATION_EASING_FUNC_MASK) &&
        (info->steps < 2))
    {
        return GX_INVALID_VALUE;
    }

    if (info->frame_interval < 1)
    {
        return GX_INVALID_VALUE;
    }

    if (info->slide_screen_list == GX_NULL)
    {
        /* Slide screen list has not been created */
        return GX_INVALID_WIDGET;
    }

    return(_gx_animation_drag_enable(animation, widget, info));
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_animation_landing_speed_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for error in animation landing speed set call. */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to animation control  */
/*                                            block                       */
/*    shift_per_step                        Shift distance for each step  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_animation_landing_speed_set       Actual animation landing      */
/*                                            speed set call.             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gxe_animation_landing_speed_set(GX_ANIMATION *animation, USHORT shift_per_step)
{
    /* Check for invalid input pointers.  */
    if (animation == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (shift_per_step == 0)
    {
        return GX_INVALID_VALUE;
    }

    return _gx_animation_landing_speed_set(animation, shift_per_step);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_animation_start                                                */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for error in animation start call.             */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to animation control  */
/*                                            block                       */
/*    info                                  Animation information         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_animation_start                   Actual animation start call.  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gxe_animation_start(GX_ANIMATION *animation, GX_ANIMATION_INFO *info)
{
    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (animation == GX_NULL ||
        info == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (info->steps < 2 || info->frame_interval < 1)
    {
        return GX_INVALID_VALUE;
    }

    if (info->end_alpha == 0 &&
        info->start_alpha == 0)
    {
        return GX_INVALID_VALUE;
    }

    if (info->target == GX_NULL)
    {
        return GX_INVALID_WIDGET;
    }
    if (info->target->type == 0)
    {
        /* target widget has not been created */
        return GX_INVALID_WIDGET;
    }

    /* Check for invalid animation status. */
    if (animation->status != GX_ANIMATION_IDLE)
    {
        return(GX_INVALID_STATUS);
    }

    return(_gx_animation_start(animation, info));
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_animation_stop                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks error in animation stop call.                  */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to window control     */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_animation_stop                    Actual animation stop call    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gxe_animation_stop(GX_ANIMATION *animation)
{
    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (animation == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (animation->status != GX_ANIMATION_ACTIVE)
    {
        return GX_INVALID_STATUS;
    }

    return _gx_animation_stop(animation);
}
#endif
