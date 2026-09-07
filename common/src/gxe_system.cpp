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
/**   System Management (System)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_system.h"

#include "gx_font.h"
#include "gx_display.h"
#include "gx_utility.h"
#include "gx_widget.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_active_language_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks for errors in active language set call.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    language                              Language table id             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gxe_display_active_language_set      Actual active language set    */
/*                                            call.                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_active_language_set(GX_UBYTE language)
{
UINT status;

    status = _gxe_display_active_language_set(_gx_system_display_created_list, language);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_animation_free                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system_animation_free call.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to animation struct   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_system_animation_free              Actual animation free call    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT  _gxe_system_animation_free(GX_ANIMATION *animation)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (animation == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (animation->system_allocated == 0)
    {
        return (GX_INVALID_ANIMATION);
    }

    if (animation->status != GX_ANIMATION_IDLE)
    {
        return (GX_INVALID_ANIMATION);
    }

    /* Call actual animation get  */
    status = _gx_system_animation_free(animation);

    /* Return status.  */
    return(status);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_animation_get                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in gx_system_animaton_get call      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to recieve pointer    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_system_animation_get               Actual animation get call     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)

UINT  _gxe_system_animation_get(GX_ANIMATION **animation)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (animation == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual animation get  */
    status = _gx_system_animation_get(animation);

    /* Return status.  */
    return(status);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_canvas_refresh                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks for errors in system canvas refresh call.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_canvas_fresh               Actual system canvas refresh  */
/*                                            call.                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_canvas_refresh(void)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Call actual system canvas refresh function.  */
    status = _gx_system_canvas_refresh();

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_dirty_mark                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system dirty mark call.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Widget pointer                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Actual system dirty mark call */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_dirty_mark(GX_WIDGET *widget)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual system dirty mark.  */
    status = _gx_system_dirty_mark(widget);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_dirty_partial_add                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system dirty partial add call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget control     */
/*                                            block                       */
/*    dirty_area                            Dirty area of widget to add   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_partial_add          Actual system dirty partial   */
/*                                            add call                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_dirty_partial_add(GX_WIDGET *widget, GX_RECTANGLE *dirty_area)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((widget == GX_NULL) || (dirty_area == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid size of dirty area.  */
    if((dirty_area->left > dirty_area->right) ||
       (dirty_area->top > dirty_area->bottom) ||
       (!_gx_utility_rectangle_overlap_detect(dirty_area, &widget->size, GX_NULL)))
    {
        return(GX_INVALID_SIZE);
    }

    /* Call actual system dirty partial add.  */
    status = _gx_system_dirty_partial_add(widget, dirty_area);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_draw_context_get                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system draw context get call.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    current_context                       Pointer to destination for    */
/*                                            current drawing context     */
/*                                            pointer                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_draw_context_get           Actual system draw context    */
/*                                            get call                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default draw funtions                                          */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_draw_context_get(GX_DRAW_CONTEXT **current_context)
{
UINT status;

    /* Check for invalid pointer.  */
    if (current_context == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual system draw context get.  */
    status = _gx_system_draw_context_get(current_context);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_event_fold                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system event send call.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    event                                 Pointer to event              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_event_fold                 Actual system event fold call */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_event_fold(GX_EVENT *event_ptr)
{
UINT status = GX_SUCCESS;

    /* Check for invalid pointer.  */
    if (event_ptr == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual system event send.  */
    status = _gx_system_event_fold(event_ptr);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_event_send                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system event send call.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    event                                 Pointer to event              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_event_send                 Actual system event send call */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_event_send(GX_EVENT *event_ptr)
{
UINT status = GX_SUCCESS;

    /* Check for invalid pointer.  */
    if (event_ptr == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual system event send.  */
    status = _gx_system_event_send(event_ptr);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_focus_claim                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system focus claim call.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget control     */
/*                                            block to claim focus        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_focus_claim                Actual system focus claim     */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_system_focus_claim(GX_WIDGET *widget)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (widget == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (widget->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual system focus claim.  */
    status = _gx_system_focus_claim(widget);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_gradient_create                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility gradient create      */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gradient                          Pointer to GX_GRADIENT            */
/*    width                             Width of gradient to create       */
/*    height                            Height of gradient to create      */
/*    type                              Gradient type to create           */
/*    alpha_start                       Gradient starting alpha value     */
/*    alpha_end                         Gradient ending alpha value       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_gradient_create           Actual utility gradient create*/
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_system_gradient_create(GX_GRADIENT *gradient, GX_VALUE width, GX_VALUE height,
                                  UCHAR type, GX_UBYTE alpha_start, GX_UBYTE alpha_end)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (gradient == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    status = _gx_system_gradient_create(gradient, width, height, type, alpha_start, alpha_end);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_gradient_delete                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility gradient delete      */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gradient                              GX_GRADIENT pointer           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_gradient_delete           Actual utility gradient delete*/
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_system_gradient_delete(GX_GRADIENT *gradient)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (gradient == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    status = _gx_system_gradient_delete(gradient);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_initialize                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system initialize call.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_initialize                 Actual system initialize call */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_initialize(void)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Call actual system initialize.  */
    status = _gx_system_initialize();

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_memory_allocator_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns the memory alloc/free functions               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    allocate                               User supplied allocation     */
/*                                             function                   */
/*    release                                User supplied release        */
/*                                             function                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                 Completion status            */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator_set        Actual memory allocator      */
/*                                             call                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_system_memory_allocator_set(void *(*allocate)(ULONG size), void (*release)(void *))
{
UINT status;

    GX_INIT_AND_THREADS_CALLER_CHECKING

    status = _gx_system_memory_allocator_set(allocate, release);
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_pen_configure                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system pen configure call.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pen_configuration                     Pointer to pen configuration  */
/*                                           structure.                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_pen_configurec             Actual system touch           */
/*                                            configuration call.         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_system_pen_configure(GX_PEN_CONFIGURATION *pen_configuration)
{
UINT status;

    /* Check invalid pointer. */
    if (pen_configuration == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual system pen configuration get.  */
    status = _gx_system_pen_configure(pen_configuration);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_screen_stack_create                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the system screen stack create       */
/*    function.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    memory                                Memory address at which to    */
/*                                            create system screen stack. */
/*    size                                  Memory size in bytes.         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_screen_stack_create        The actual screen stack       */
/*                                            create routine              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_screen_stack_create(GX_WIDGET **memory, INT size, UINT control_block_size)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((memory == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid value.  */
    if (size <= 0)
    {
        return(GX_INVALID_VALUE);
    }

    /* Check for invalid control block size. */
    if (control_block_size != sizeof(GX_SCREEN_STACK_CONTROL))
    {
        return(GX_INVALID_SIZE);
    }

    /* Call actual screen stack create function.  */
    status = _gx_system_screen_stack_create(memory, size);

    /* Return the error status from screen stack create.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_screen_stack_get                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the screen stack get function.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_screen_stack_get           The actual system screen      */
/*                                            stack get routine           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_screen_stack_get(GX_WIDGET **popped_parent, GX_WIDGET **popped_screen)
{
    UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!_gx_system_screen_stack.memory)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual system screen stack get function.  */
    status = _gx_system_screen_stack_get(popped_parent, popped_screen);

    /* Return the error status from system screen stack pop.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_screen_stack_pop                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the screen stack pop function.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_screen_stack_pop           The actual system screen      */
/*                                            stack pop routine           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_screen_stack_pop(void)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!_gx_system_screen_stack.memory)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual system screen stack pop function.  */
    status = _gx_system_screen_stack_pop();

    /* Return the error status from system screen stack pop.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_screen_stack_push                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the system screen stack push         */
/*    function.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    screen                                Screen pointer to push        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_screen_stack_push          The actual system screen      */
/*                                            stack push routine          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_screen_stack_push(GX_WIDGET *screen)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (screen == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual screen stack PUSH function.  */
    status = _gx_system_screen_stack_push(screen);

    /* Return the error status from screen stack push.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_scroll_appearance_get                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system scroll appearance get     */
/*      call.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    style                                 Scrollbar style               */
/*                                            GX_SCROLLBAR_HORIZONTAL or  */
/*                                            GX_SCROLLBAR_VERTICAL       */
/*    return_appearance                     Pointer to destination for    */
/*                                            appearance.                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_scroll_appearance_get      Actual system scroll          */
/*                                            appearance call             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_scroll_appearance_get(ULONG style, GX_SCROLLBAR_APPEARANCE *return_appearance)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (return_appearance == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual system scroll appearance get.  */
    status = _gx_system_scroll_appearance_get(style, return_appearance);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_scroll_appearance_set                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system scroll appearance set     */
/*      call.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    style                                 Scrollbar style               */
/*                                            GX_SCROLLBAR_HORIZONTAL or  */
/*                                            GX_SCROLLBAR_VERTICAL       */
/*    appearance                            Pointer to appearance         */
/*                                            structure initialized with  */
/*                                            various scrollbar           */
/*                                            appearance attributes.      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_scroll_appearance_set      Actual system scroll          */
/*                                            appearance call.            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_system_scroll_appearance_set(ULONG style, GX_SCROLLBAR_APPEARANCE *appearance)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (appearance == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }


    /* Call actual system scroll appearance get.  */
    status = _gx_system_scroll_appearance_set(style, appearance);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_start                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system start call.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_start                      Actual system start call      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_start(void)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Call actual system start.  */
    status = _gx_system_start();

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_string_width_get_ext                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system string width get call.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    font                                  Font used by the string       */
/*    string                                Pointer to string             */
/*    return_width                          Destination for width of      */
/*                                            string                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_string_length_check       Test string length            */
/*    _gx_system_string_width_get_ext       Actual system string width    */
/*                                            get exe call                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_string_width_get_ext(const GX_FONT *font, const GX_STRING *string, GX_VALUE *return_width)
{
UINT status;
UINT length;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((font == GX_NULL) || (string == GX_NULL) || (return_width == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid font.  */
    if (font->glyphs.normal == GX_NULL)
    {
        return(GX_INVALID_FONT);
    }

    /* Calculate input string length. */
    status = _gx_utility_string_length_check(string->ptr, &length, string->length);

    if (status != GX_SUCCESS)
    {
        return status;
    }

    /* Check for invalid string length.  */
    if (string->length > length)
    {
        return(GX_INVALID_STRING_LENGTH);
    }

    /* Call actual system string width get.  */
    status = _gx_system_string_width_get_ext(font, string, return_width);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_timer_start                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system timer start call.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    owner                                 Pointer to widget control     */
/*                                            block                       */
/*    timer_id                              ID of timer                   */
/*    initial ticks                         Initial expiration ticks      */
/*    reschedule_ticks                      Periodic expiration ticks     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   _gx_system_timer_start                 Actual system timer start call*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_timer_start(GX_WIDGET *owner, UINT timer_id, UINT initial_ticks, UINT reschedule_ticks)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for no more timers.  */
    if (!_gx_system_free_timer_list)
    {
        return(GX_OUT_OF_TIMERS);
    }

    /* Check for invalid pointer.  */
    if (owner == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (owner->type <= 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid time values.  */
    if (initial_ticks == 0)
    {
        return(GX_INVALID_VALUE);
    }

    /* Call actual system timer start.  */
    status = _gx_system_timer_start(owner, timer_id, initial_ticks, reschedule_ticks);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_timer_stop                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system timer stop call.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    owner                                 Pointer to widget control     */
/*                                            block                       */
/*    timer_id                              ID of timer                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_timer_stop                 Actual system timer stop call */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_system_timer_stop(GX_WIDGET *owner, UINT timer_id)
{
UINT      status;
GX_TIMER *next;
GX_BOOL   id_flag = GX_FALSE;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for timer ID not found.  */

    if (timer_id)
    {
        next = _gx_system_active_timer_list;
        while (next)
        {
            if (next->id == timer_id)
            {
                id_flag = GX_TRUE;
                break;
            }
            next = next->next;
        }
        if (id_flag == GX_FALSE)
        {
            return(GX_NOT_FOUND);
        }
    }

    /* Check for invalid pointer.  */
    if (owner == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (owner->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual system timer stop.  */
    status = _gx_system_timer_stop(owner, timer_id);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_version_string_get_ext                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in new system version string get    */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    return_string                         Pointer to return string      */
/*                                            pointer                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    void                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_version_string_get_ext     Actual system version string  */
/*                                            get call                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_system_version_string_get_ext(GX_STRING *return_string)
{
UINT status;

    /* Check for invalid pointer.  */
    if (return_string == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_system_version_string_get_ext(return_string);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_widget_find                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system widget search call.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget_id                            Widget ID to search for        */
/*    search_level                         Specifies the depth of the     */
/*                                           search                       */
/*    return_search_result                 Pointer to destination for     */
/*                                           widget found                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                               Completion status              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_widget_find               Actual widget find function    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application Code                                               */
/*                                                                        */
/**************************************************************************/
UINT _gxe_system_widget_find(USHORT widget_id, INT search_level, GX_WIDGET **return_search_result)
{
UINT status;

    /* Check for invalid pointer.  */
    if (return_search_result == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual system widget search.  */
    status = _gx_system_widget_find(widget_id, search_level, return_search_result);

    /* Return status.  */
    return status;
}
