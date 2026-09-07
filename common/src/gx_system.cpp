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

#include "gx_utility.h"
#include "gx_display.h"
#include "gx_canvas.h"
#include "gx_font.h"
#include "gx_gradient.h"
#include "gx_window_root.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_version_string_get_ext                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION (deprecated)                                              */
/*                                                                        */
/*    This service retrieves system version string.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    return_string                         Pointer to return string      */
/*                                            pointer                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_version_string_get_ext(GX_STRING* return_string)
{
    UINT string_length;
    UINT status;

    status = _gx_utility_string_length_check(_gx_system_version_string,
                                             &string_length,
                                             sizeof(_gx_system_version_string) - 1);
    if (status == GX_SUCCESS)
    {
        return_string->ptr = _gx_system_version_string;
        return_string->length = string_length;
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_memory_allocator_set                                     */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_memory_allocator_set(void* (*allocate)(ULONG size), void (*release)(void *))
{
    _gx_system_memory_allocator = allocate;
    _gx_system_memory_free = release;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_draw_context_get                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*   This service gets the current drawing context.                       */
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
/*    none                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default draw funtions                                          */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_draw_context_get(GX_DRAW_CONTEXT** current_context)
{
    *current_context = _gx_system_current_draw_context;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_free_view_get                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Grabs a free GX_VIEW from the list of free views, updates free      */
/*    view list.                                                          */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Returns a pointer to a free GX_VIEW structure                       */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_error_process              Process a system error        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_view_add                   Link a view to window's       */
/*                                            view list                   */
/*                                                                        */
/**************************************************************************/
GX_VIEW* _gx_system_free_view_get(void)
{
    if (!_gx_system_free_views)
    {
        _gx_system_error_process(GX_SYSTEM_OUT_OF_VIEWS);

        return GX_NULL;
    }

    GX_VIEW* view = _gx_system_free_views;
    _gx_system_free_views = _gx_system_free_views->next;

    return view;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_views_free                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns a list of views to the free list              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    head                                  Pointer to head of list to    */
/*                                            free                        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_all_views_free                    Release all views             */
/*                                                                        */
/**************************************************************************/
void _gx_system_views_free(GX_VIEW* head)
{
    GX_VIEW* tail = head;
    while (tail->next)
    {
        tail = tail->next;
    }
    tail->next = _gx_system_free_views;

    _gx_system_free_views = head;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_focus_claim                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service claims the focus for the specified widget.             */
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
/*    _gx_system_error_process              Processes internal GUIX       */
/*                                            system errors               */
/*    _gx_widget_child_detect               Detect if one widget is child */
/*                                            of another.                 */
/*    [gx_widget_event_process_function]    Widget event process routine  */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_focus_claim(GX_WIDGET* widget)
{
    if (!widget->is_visible_())
    {
        return GX_SUCCESS;
    }

    while (!(widget->status & GX_STATUS_ACCEPTS_FOCUS))
    {
        widget = widget->parent;
        if (!widget)
        {
            // this should not happened. Somehow we have a portion of
            // the widget tree that does not accept focus, including the
            // root window. Return an error.

            _gx_system_error_process(GX_PTR_ERROR);

            return GX_PTR_ERROR;
        }
    }

    if (widget == _gx_system_focus_owner)
    {
        return GX_NO_CHANGE;
    }

    GX_EVENT focus_event;

    focus_event.target = GX_NULL;

    if (_gx_system_focus_owner)
    {
        GX_WIDGET* focused_widget = _gx_system_focus_owner;

        if (focused_widget->is_ancestor_of_(widget))
        {
            focused_widget = GX_NULL;
        }
        else
        {
            while (focused_widget->parent)
            {
                if (focused_widget->parent->is_ancestor_of_(widget))
                {
                    break;
                }

                focused_widget = focused_widget->parent;
            }
        }

        if (focused_widget)
        {
            focus_event.type = GX_EVENT_FOCUS_LOST;
            focused_widget->event_process_function_run_(&focus_event);
        }
    }

    _gx_system_focus_owner = widget;

    focus_event.type = GX_EVENT_FOCUS_GAINED;
    widget->event_process_function_run_(&focus_event);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_input_capture                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Temporarily direct all input events to specified widget             */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    ownert                                Widget that wants to own      */
/*                                            input events                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    widget added to capture stack                                       */
/*    widget receives GX_STATUS_OWNS_INPUT                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _GX_ENTER_CRITICAL                    Enter critical section        */
/*    _gx_widget_status_add                 Add status flag to the widget */
/*    _GX_EXIT_CRITICAL                     Exit critical section         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_input_capture(GX_WIDGET* owner)
{
    UINT status = GX_SUCCESS;

    GX_ENTER_CRITICAL

    if (_gx_system_input_capture_count < GX_MAX_INPUT_CAPTURE_NESTING)
    {
        if (!(owner->status & GX_STATUS_OWNS_INPUT))
        {
            owner->status |= GX_STATUS_OWNS_INPUT;

            _gx_system_input_capture_stack[_gx_system_input_capture_count] = owner;
            ++_gx_system_input_capture_count;

            _gx_system_input_owner = owner;
        }
        else
        {
            status = GX_NO_CHANGE;
        }
    }
    else
    {
        status = GX_INPUT_CAPTURE_NESTING_EXCEEDED;
    }

    GX_EXIT_CRITICAL

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_input_release                                            */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Release previously captured input events                            */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    owner                                 Widget that releases input    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Return status                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _GX_ENTER_CRITICAL                    Enter critical section        */
/*    _gx_widget_status_remove              Remove a flag from the widget */
/*    _GX_EXIT_CRITICAL                     Exit critical section         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_input_release(GX_WIDGET* owner)
{
    UINT status = GX_NO_CHANGE;

    GX_ENTER_CRITICAL

    if (_gx_system_input_capture_count > 0)
    {
        if (owner->status & GX_STATUS_OWNS_INPUT)
        {
            status = GX_PTR_ERROR;

            GX_WIDGET** stack = _gx_system_input_capture_stack;
            INT top = _gx_system_input_capture_count - 1;
            INT current = 0;

            while (current <= top)
            {
                if (stack[current] == owner)
                {
                    owner->status &= ~GX_STATUS_OWNS_INPUT;
                    --_gx_system_input_capture_count;

                    status = GX_SUCCESS;
                    break;
                }

                ++current;
            }

            if (status == GX_SUCCESS)
            {
                while (current < top)
                {
                    stack[current] = stack[current + 1];
                    ++current;
                }
                stack[current] = GX_NULL;

                if (top > 0)
                {
                    _gx_system_input_owner = stack[top - 1];
                }
                else
                {
                    _gx_system_input_owner = GX_NULL;
                }
            }
        }
    }

    GX_EXIT_CRITICAL

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_animation_get                                            */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Obtain an animation structure from the system pool.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Adress to return pointer      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   GX_ENTER_CRITICAL                      lock system mutex             */
/*   GX_EXIT_CRITICAL                       unlock system mutex           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gx_system_animation_get(GX_ANIMATION** animation)
{
    UINT status = GX_SUCCESS;

    GX_ENTER_CRITICAL

    if (_gx_system_animation_free_list)
    {
        GX_ANIMATION* free_block = _gx_system_animation_free_list;
        _gx_system_animation_free_list = free_block->next;
        free_block->next = GX_NULL;
        free_block->system_allocated = GX_TRUE;
        free_block->status = GX_ANIMATION_IDLE;
        free_block->canvas = GX_NULL;

        *animation = free_block;
    }
    else
    {
        *animation = GX_NULL;
        status = GX_OUT_OF_ANIMATIONS;
    }

    GX_EXIT_CRITICAL

    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_animation_free                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Return a free GX_ANIMATION block from the system pool.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             GX_ANIMATION pointer          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   GX_ENTER_CRITICAL                      lock system mutex             */
/*   GX_EXIT_CRITICAL                       unlock system mutex           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gx_system_animation_free(GX_ANIMATION* animation)
{
    GX_ENTER_CRITICAL

    animation->status = 0;
    animation->system_allocated = GX_FALSE;
    animation->next = _gx_system_animation_free_list;
    _gx_system_animation_free_list = animation;

    GX_EXIT_CRITICAL

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_gradient_find                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Helper function for _gx_system_gradient_create. This function       */
/*    searches the gradient list to find a match.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gradient                                   GX_GRADIENT pointer      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_gradient_create                                          */
/*                                                                        */
/**************************************************************************/
static GX_BOOL _gx_system_gradient_find(GX_GRADIENT* gradient)
{
    GX_GRADIENT* search = _gx_system_gradient_list;
    for (; search; search = search->next)
    {
        if (search->pixelmap.data &&
            search->type == gradient->type &&
            search->alpha_start == gradient->alpha_start &&
            search->alpha_end == gradient->alpha_end &&
            search->pixelmap.width == gradient->pixelmap.width &&
            search->pixelmap.height == gradient->pixelmap.height)
        {
            // found a match, just copy the pixelmap
            gradient->pixelmap = search->pixelmap;

            return GX_TRUE;
        }
    }

    return GX_FALSE;
}

/**************************************************************************/

static void _gx_alpha_gradient_column_write(GX_UBYTE* data, INT width, INT height, GX_FIXED_VAL alpha)
{
    GX_UBYTE dataval = (GX_UBYTE)(GX_FIXED_VAL_TO_INT(alpha));

    for (INT row = 0; row < height; row++)
    {
        *data = dataval;
        data += width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_horizontal_alpha_gradient_create                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a horizontal alpha gradient pixelmap.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gradient                                   GX_GRADIENT structure    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_gradient_create                                          */
/*                                                                        */
/**************************************************************************/
static UINT _gx_horizontal_alpha_gradient_create(GX_GRADIENT* gradient)
{
    GX_UBYTE* data_start = (GX_UBYTE*)gradient->pixelmap.data;

    INT height = gradient->pixelmap.height;
    INT width = gradient->pixelmap.width;

    INT column;

    if (gradient->type & GX_GRADIENT_TYPE_MIRROR)
    {
        for (column = 0; column < width / 2; column++)
        {
            _gx_alpha_gradient_column_write(data_start, width, height, gradient->alpha_current);
            data_start++;
            gradient->alpha_current += gradient->alpha_step;
        }
        for (; column < width; column++)
        {
            _gx_alpha_gradient_column_write(data_start, width, height, gradient->alpha_current);
            data_start++;
            gradient->alpha_current -= gradient->alpha_step;
        }
    }
    else
    {
        for (column = 0; column < width; column++)
        {
            _gx_alpha_gradient_column_write(data_start, width, height, gradient->alpha_current);
            data_start++;
            gradient->alpha_current += gradient->alpha_step;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_vertical_alpha_gradient_create                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a vertical alpha gradient pixelmap.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gradient                                   GX_GRADIENT structure    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_gradient_create                                          */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_vertical_alpha_gradient_create(GX_GRADIENT* gradient)
{
    GX_UBYTE* data = (GX_UBYTE*)gradient->pixelmap.data;

    INT height = gradient->pixelmap.height;
    INT width = gradient->pixelmap.width;

    INT row;

    if (gradient->type & GX_GRADIENT_TYPE_MIRROR)
    {
        for (row = 0; row < height / 2; row++)
        {
            memset(data, GX_FIXED_VAL_TO_INT(gradient->alpha_current), (size_t)width);
            data += width;
            gradient->alpha_current += gradient->alpha_step;
        }
        for (; row < height; row++)
        {
            memset(data, GX_FIXED_VAL_TO_INT(gradient->alpha_current), (size_t)width);
            data += width;
            gradient->alpha_current -= gradient->alpha_step;
        }
    }
    else
    {
        for (row = 0; row < height; row++)
        {
            memset(data, GX_FIXED_VAL_TO_INT(gradient->alpha_current), (size_t)width);
            data += width;
            gradient->alpha_current += gradient->alpha_step;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_gradient_create                                          */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a gradient pixelmap.                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gradient                              Pointer to GX_GRADIENT        */
/*    width                                 Requested width               */
/*    height                                Requested height              */
/*    type                                  Gradient type                 */
/*    alpha_start                           Starting alpha value          */
/*    alpha_end                             Ending alpha value            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_gradient_find                                            */
/*    _gx_utility_alphamap_create                                         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_text_scroll_wheel_gradient_create                               */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_gradient_create(GX_GRADIENT* gradient, GX_VALUE width, GX_VALUE height,
                                UCHAR type, GX_UBYTE alpha_start, GX_UBYTE alpha_end)
{
    UINT status = GX_FAILURE;

    // width and height must be >= 2 pixels
    if (width < 2 || height < 2)
    {
        return GX_INVALID_SIZE;
    }

    // only alpha gradient supported currently
    if (!(type & GX_GRADIENT_TYPE_ALPHA))
    {
        return GX_NOT_SUPPORTED;
    }

    memset(&gradient->pixelmap, 0, sizeof(GX_PIXELMAP));
    gradient->pixelmap.width = width;
    gradient->pixelmap.height = height;
    gradient->type = type;
    gradient->alpha_start = alpha_start;
    gradient->alpha_end = alpha_end;
    gradient->previous = GX_NULL;
    gradient->next = GX_NULL;

    if (_gx_system_gradient_find(gradient))
    {
        status = GX_SUCCESS;
    }
    else
    {
        gradient->alpha_current = GX_FIXED_VAL_MAKE(gradient->alpha_start);
        gradient->alpha_step = GX_FIXED_VAL_MAKE(gradient->alpha_end);
        gradient->alpha_step -= GX_FIXED_VAL_MAKE(gradient->alpha_start);

        status = _gx_utility_alphamap_create(width, height, &gradient->pixelmap);
        if (status == GX_SUCCESS)
        {
            if (gradient->type & GX_GRADIENT_TYPE_VERTICAL)
            {
                if (gradient->type & GX_GRADIENT_TYPE_MIRROR)
                {
                    gradient->alpha_step /= (height / 2);
                }
                else
                {
                    gradient->alpha_step /= height;
                }

                status = _gx_utility_vertical_alpha_gradient_create(gradient);
            }
            else
            {
                if (gradient->type & GX_GRADIENT_TYPE_MIRROR)
                {
                    gradient->alpha_step /= (width / 2);
                }
                else
                {
                    gradient->alpha_step /= width;
                }

                status = _gx_horizontal_alpha_gradient_create(gradient);
            }
        }
    }

    if (status == GX_SUCCESS)
    {
        GX_ENTER_CRITICAL

        gradient->next = _gx_system_gradient_list;

        if (_gx_system_gradient_list)
        {
            _gx_system_gradient_list->previous = gradient;
        }
        _gx_system_gradient_list = gradient;

        GX_EXIT_CRITICAL
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_gradient_delete                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deletes a previously created gradient.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gradient                              Pointer to GX_GRADIENT        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_gradient_delete(GX_GRADIENT* gradient)
{
    const GX_UBYTE* data_ptr = gradient->pixelmap.data;
    GX_BOOL         in_use = GX_FALSE;

    GX_ENTER_CRITICAL

    if (data_ptr)
    {
        // check to see if any other gradients are using same pixelmap data,
        // if not, then time to delete pixelmap data

        GX_GRADIENT* search = _gx_system_gradient_list;
        for (; search; search = search->next)
        {
            if (search != gradient &&
                search->pixelmap.data == data_ptr)
            {
                in_use = GX_TRUE;
                break;
            }
        }

        // unlink this gradient from gradient list
        if (gradient->next)
        {
            gradient->next->previous = gradient->previous;
        }
        if (gradient->previous)
        {
            gradient->previous->next = gradient->next;
        }
        else
        {
            _gx_system_gradient_list = gradient->next;
        }
    }

    GX_EXIT_CRITICAL

    if (data_ptr && !in_use && _gx_system_memory_free)
    {
        _gx_system_memory_free((void*)data_ptr);
    }

    memset(gradient, 0, sizeof(GX_GRADIENT));

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_scroll_appearance_get                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the scrollbar appearance.                         */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    _gx_horizontal_scrollbar_create       Create the horizontal         */
/*                                            scrollbar widget            */
/*    _gx_vertical_scrollbar_create         Create the vertical           */
/*                                            scrollbar widget            */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_scroll_appearance_get(ULONG style, GX_SCROLLBAR_APPEARANCE* return_appearance)
{
    if (style & GX_SCROLLBAR_HORIZONTAL)
    {
        *return_appearance = _gx_system_horizontal_scrollbar_appearance;
    }
    else
    {
        *return_appearance = _gx_system_vertical_scrollbar_appearance;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_scroll_appearance_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the default scrollbar appearance.                 */
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
/*    None                                  Completion status             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_scroll_appearance_set(ULONG style, const GX_SCROLLBAR_APPEARANCE* appearance)
{
    if (style & GX_SCROLLBAR_HORIZONTAL)
    {
        _gx_system_horizontal_scrollbar_style = style;
        _gx_system_horizontal_scrollbar_appearance = *appearance;
    }
    else
    {
        _gx_system_vertical_scrollbar_style = (style | GX_SCROLLBAR_VERTICAL);
        _gx_system_vertical_scrollbar_appearance = *appearance;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT _gx_system_bidi_text_enable(GX_BOOL enabled)
{
    _gx_system_bidi_text_enabled = enabled;

    GX_EVENT bidi_text_event;
    memset(&bidi_text_event, 0, sizeof(GX_EVENT));

    if (enabled)
    {
        bidi_text_event.type = GX_EVENT_DYNAMIC_BIDI_TEXT_ENABLE;
    }
    else
    {
        bidi_text_event.type = GX_EVENT_DYNAMIC_BIDI_TEXT_DISABLE;
    }

    GX_WINDOW_ROOT* root = _gx_system_root_window_created_list;
    for (; root; root = (GX_WINDOW_ROOT*)root->next)
    {
        if (root->canvas)
        {
            root->event_process_function_run_(&bidi_text_event);
        }
    }

    _gx_system_all_canvas_dirty();

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_bidi_text_enable                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function enables display of bi-directional text.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT _gx_system_bidi_text_enable(void)
{
    return _gx_system_bidi_text_enable(GX_TRUE);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_bidi_text_disable                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function disables display of bi-directional text.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT _gx_system_bidi_text_disable(void)
{
    return _gx_system_bidi_text_enable(GX_FALSE);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_text_render_style_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets text render styles.                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
UINT _gx_system_text_render_style_set(GX_UBYTE style)
{
    _gx_system_text_render_style = style;

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_clipboard_get                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service puts data to clipboard.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    data                                  Retrieved clipboard data      */
/*    data_size                             Retrieved clipboard data size */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   None                                                                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_clipboard_get(void** data, UINT* data_size)
{
    *data = _gx_system_clipboard;
    *data_size = _gx_system_clipboard_size;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_clipboard_put                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service puts data to clipboard.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    data                                  Pointer the copied data       */
/*    data_size                             The number of bytes to copy   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           Application defined memory    */
/*                                            allocation function         */
/*    _gx_system_memory_free                Application defined memory    */
/*                                            free function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_clipboard_put(void* data, UINT data_size)
{
    if ((!_gx_system_memory_allocator) ||
        (!_gx_system_memory_free))
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    if (_gx_system_clipboard)
    {
        _gx_system_memory_free(_gx_system_clipboard);
    }

    _gx_system_clipboard = _gx_system_memory_allocator(data_size);
    if (_gx_system_clipboard == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    _gx_system_clipboard_size = data_size;

    memcpy(_gx_system_clipboard, data, data_size);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_pen_configure                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets pen configuration.                                */
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
/*    None                                  Completion status             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_pen_configure(const GX_PEN_CONFIGURATION* pen_configuration)
{
    _gx_system_pen_configuration = *pen_configuration;

    return (GX_SUCCESS);
}

/**************************************************************************/

#ifdef GX_THREADX_BINDING
#define GX_SYSTEM_CURRENT_CLOCK     tx_time_get()
#else
#define GX_SYSTEM_CURRENT_CLOCK     GX_SYSTEM_TIME_GET
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_pen_speed_init                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service initiates system pen speed.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    current                               Current pen position          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    tx_timer_get                          Get ThreadX timer tick        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_event_send                 Send GUIX system event        */
/*                                                                        */
/**************************************************************************/
void _gx_system_pen_speed_init(GX_POINT* current)
{
    _gx_system_last_clock = GX_SYSTEM_CURRENT_CLOCK;
    _gx_system_last_pen_point = *current;

    _gx_system_pen_speed_x = 0;
    _gx_system_pen_speed_y = 0;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_pen_speed_update                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sends the specified event for processing.              */
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
/*    GX_ABS                                Compute the absolute value    */
/*    tx_queue_send                         Send message through ThreadX  */
/*                                            queue                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_event_fold                 Fold GUIX system events       */
/*    _gx_system_event_send                 Send GUIX system event        */
/*                                                                        */
/**************************************************************************/
void _gx_system_pen_speed_update(GX_POINT* current)
{
    ULONG current_clock = GX_SYSTEM_CURRENT_CLOCK;

    _gx_system_clock_delta = (LONG)(current_clock - _gx_system_last_clock);

    if (_gx_system_clock_delta > 0)
    {
        if (_gx_system_clock_delta < (LONG)_gx_system_pen_configuration.max_pen_speed_ticks)
        {
            _gx_system_pen_speed_x = GX_FIXED_VAL_MAKE(current->x - _gx_system_last_pen_point.x);
            _gx_system_pen_speed_x /= _gx_system_clock_delta;

            _gx_system_pen_speed_y = GX_FIXED_VAL_MAKE(current->y - _gx_system_last_pen_point.y);
            _gx_system_pen_speed_y /= _gx_system_clock_delta;
        }
        else
        {
            _gx_system_pen_speed_x = 0;
            _gx_system_pen_speed_y = 0;
        }

        _gx_system_last_clock = current_clock;
        _gx_system_last_pen_point = *current;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_pen_flick_test                                           */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service tests for pen flick event.                             */
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
/*    tx_time_get                           Get current timer tick        */
/*    GX_ABS                                Compute the absolute value    */
/*    tx_queue_send                         Send message through ThreadX  */
/*                                            queue                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_event_send                 Send GUIX event               */
/*                                                                        */
/**************************************************************************/
void _gx_system_pen_flick_test(GX_WIDGET* target)
{
    GX_EVENT flick_event;
    INT      abs_speed_x;
    INT      abs_speed_y;
    ULONG    current_clock;

    flick_event.type = 0;

    current_clock = GX_SYSTEM_CURRENT_CLOCK;

    _gx_system_clock_delta = (LONG)(current_clock - _gx_system_last_clock);

    if (_gx_system_clock_delta < (LONG)_gx_system_pen_configuration.max_pen_speed_ticks)
    {
        abs_speed_x = GX_ABS(_gx_system_pen_speed_x);
        abs_speed_y = GX_ABS(_gx_system_pen_speed_y);

        if (abs_speed_y > abs_speed_x)
        {
            if (abs_speed_y > (INT)_gx_system_pen_configuration.min_drag_dist)
            {
                flick_event.type = GX_EVENT_VERTICAL_FLICK;
                flick_event.payload.intdata[0] = _gx_system_pen_speed_y;
            }
        }
        else
        {
            if (abs_speed_x > (INT)_gx_system_pen_configuration.min_drag_dist)
            {
                flick_event.type = GX_EVENT_HORIZONTAL_FLICK;
                flick_event.payload.intdata[0] = _gx_system_pen_speed_x;
            }
        }
    }

    if (flick_event.type)
    {
        flick_event.target = target;
        flick_event.sender = 0;

#ifdef GX_THREADX_BINDING
        tx_queue_send(&_gx_system_event_queue, &flick_event, TX_NO_WAIT);
#else
        GX_EVENT_PUSH(&flick_event);
#endif
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_private_string_copy                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service makes a private copy of text assigned to a widget.     */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    string                                Address of widget's text      */
/*                                            pointer                     */
/*    text                                  Pointer to string             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_string_length_check       Test string length            */
/*    _gx_system_memory_allocator           Allocate dynamic memory       */
/*    _gx_system_memory_free                Deallocate dynamic memory     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX internal code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_private_string_copy(GX_STRING* string, const GX_STRING* text)
{
    GX_UBYTE* ptr;
    USHORT    current_buffer_size;
    USHORT    needed_buffer_size;

    if (!_gx_system_memory_allocator )
    {
        string->ptr = GX_NULL;
        string->length = 0;

        return GX_SYSTEM_MEMORY_ERROR;
    }

    if (text && text->ptr)
    {
        GX_UTILITY_USHORT_CHECK(text->length)
        GX_UTILITY_MATH_USHORT_ADD((USHORT)text->length, 3, needed_buffer_size)
    }
    else
    {
        needed_buffer_size = 0;
    }

    if (needed_buffer_size == 0)
    {
        if (string->ptr)
        {
            _gx_system_memory_free((void*)string->ptr);
            string->ptr = GX_NULL;
        }

        string->length = 0;

        return GX_SUCCESS;
    }

    if (string->ptr)
    {
        ptr = (GX_UBYTE*)string->ptr;

        current_buffer_size = *ptr++;
        current_buffer_size = (USHORT)(current_buffer_size << 8);
        current_buffer_size = (USHORT)(current_buffer_size | (*ptr++));

        if (current_buffer_size >= needed_buffer_size)
        {
            memcpy(ptr, text->ptr, text->length + 1);
            string->length = text->length;

            return GX_SUCCESS;
        }

        // if our buffer is not large enough, free the current buffer
        _gx_system_memory_free((void*)string->ptr);
    }

    string->ptr = (GX_CHAR*)_gx_system_memory_allocator(needed_buffer_size);
    if (!string->ptr)
    {
        string->length = 0;

        return GX_SYSTEM_MEMORY_ERROR;
    }

    ptr = (GX_UBYTE*)string->ptr;

    *ptr++ = (GX_UBYTE)(needed_buffer_size >> 8);
    *ptr++ = (GX_UBYTE)(needed_buffer_size & 0xff);

    memcpy(ptr, text->ptr, text->length + 1);
    string->length = text->length;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_private_string_list_copy_ext                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service makes a private copy of string list assigned to a      */
/*    widget.                                                             */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    ptr_address                           Address of widget's string    */
/*                                            list pointer                */
/*    buffer_size                           String list buffer size       */
/*    string_list                           Pointer to string list        */
/*    string_count                          Number of string in list      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           Allocate dynamic memory       */
/*    _gx_system_memory_free                Deallocate dynamic memory     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX internal code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_private_string_list_copy_ext(GX_STRING** ptr_address, USHORT* buffer_size,
                                             const GX_STRING* string_list, INT string_count)
{
    USHORT          current_buffer_size;
    USHORT          needed_buffer_size;
    INT             index;
    GX_STRING*      list_ptr;
    const GX_CHAR*  string_ptr;
    UINT            length;

    if (_gx_system_memory_allocator == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    if (string_list)
    {
        // calculate size of needed buffer
        needed_buffer_size = (USHORT)(((INT)sizeof(GX_STRING)) * string_count);

        for (index = 0; index < string_count; index++)
        {
            if (string_list[index].ptr)
            {
                GX_UTILITY_USHORT_CHECK(string_list[index].length)
                GX_UTILITY_MATH_USHORT_ADD(needed_buffer_size, (USHORT)string_list[index].length, needed_buffer_size)
                GX_UTILITY_MATH_USHORT_ADD(needed_buffer_size, 1, needed_buffer_size)
            }
        }
    }
    else
    {
        needed_buffer_size = 0;
    }

    list_ptr = *ptr_address;

    if (needed_buffer_size == 0)
    {
        if (list_ptr)
        {
            _gx_system_memory_free((void*)list_ptr);

            *ptr_address = GX_NULL;
            *buffer_size = 0;
        }

        return GX_SUCCESS;
    }

    if (list_ptr)
    {
        current_buffer_size = *buffer_size;

        if (current_buffer_size < needed_buffer_size)
        {
            // if our buffer is not large enough, free the current buffer
            _gx_system_memory_free((void*)list_ptr);
            list_ptr = GX_NULL;
        }
    }

    if (!list_ptr)
    {
        list_ptr = (GX_STRING*)_gx_system_memory_allocator(needed_buffer_size);
        if (!list_ptr)
        {
            return GX_SYSTEM_MEMORY_ERROR;
        }

        *buffer_size = needed_buffer_size;
    }

    *ptr_address = list_ptr;

    string_ptr = (const GX_CHAR*)(list_ptr + string_count);

    for (index = 0; index < string_count; index++)
    {
        length = string_list[index].length;

        list_ptr[index].ptr = string_ptr;
        list_ptr[index].length = length;

        if (string_list[index].ptr)
        {
            memcpy((void*)string_ptr, string_list[index].ptr, length + 1);
            string_ptr += length + 1;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_private_string_get                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service returns the string pointer in a dynamically copied     */
/*    string buffer.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    buffer                                string buffer address         */
/*    style                                 calling widget style          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_checkbox_draw                                                   */
/*                                                                        */
/**************************************************************************/
void _gx_system_private_string_get(const GX_STRING* input, GX_STRING* output, ULONG style)
{
    if (!input->ptr)
    {
        output->ptr = GX_NULL;
        output->length = 0;

        return;
    }

    if (style & GX_STYLE_TEXT_COPY)
    {
        // If the string has been dynamically copied, then the first two bytes of
        // the string buffer hold the buffer size. Skip those bytes and return
        // pointer to the actual string

        GX_UBYTE* ptr = (GX_UBYTE*)input->ptr;
        ptr += 2;

        output->ptr = (GX_CHAR*)ptr;
        output->length = input->length;
    }
    else
    {
        output->ptr = input->ptr;
        output->length = input->length;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_string_width_generic_font_get                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the width (in pixels) of the supplied string,     */
/*    for a given font.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    font                                  Pointer to font of strings    */
/*    string_copy                           Pointer to string             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    width                                 Destination for width of      */
/*                                            string                      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_utf8_string_character_get                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
static INT _gx_system_string_width_generic_font_get(const GX_FONT* font, GX_STRING string_copy)
{
    INT width = 0;

    GX_CHAR_CODE char_val;
    const GX_FONT* font_link;
    const GX_GLYPH* glyph;

    while (string_copy.length > 0)
    {
#ifdef GX_UTF8_SUPPORT
        UINT ret = _gx_utility_utf8_string_character_get(&string_copy, &char_val, GX_NULL);
        if ((ret != GX_SUCCESS) || (char_val == 0))
#else
        char_val = (GX_CHAR_CODE)(*string_copy.ptr);
        string_copy.ptr++;
        string_copy.length--;
        if (char_val == 0)
#endif /* GX_UTF8_SUPPORT */
        {
            break;
        }

        font_link = gx_font_find_(font, char_val);
        if (font_link)
        {
            char_val = (GX_CHAR_CODE)(char_val - font_link->first_glyph);
            glyph = &font_link->glyphs.normal[char_val];

            width += (INT)glyph->advance;
        }
    }

    return width;
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_string_width_compressed_font_get                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the width (in pixels) of the supplied string,     */
/*    for a given compressed font.                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    font                                  Pointer to font of strings    */
/*    string_copy                           Pointer to string             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    width                                 Destination for width of      */
/*                                            string                      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_utf8_string_character_get                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
static INT _gx_system_string_width_compressed_font_get(const GX_FONT* font, GX_STRING string_copy)
{
    INT width = 0;

    GX_CHAR_CODE char_val;
    const GX_FONT* font_link;
    const GX_COMPRESSED_GLYPH* glyph;

    while (string_copy.length > 0)
    {
#ifdef GX_UTF8_SUPPORT
        UINT ret = _gx_utility_utf8_string_character_get(&string_copy, &char_val, GX_NULL);
        if ((ret != GX_SUCCESS) || (char_val == 0))
#else
        char_val = (GX_CHAR_CODE)(*string_copy.ptr);
        string_copy.ptr++;
        string_copy.length--;
        if (char_val == 0)
#endif /* GX_UTF8_SUPPORT */
        {
            break;
        }

        font_link = gx_font_find_(font, char_val);
        if (font_link)
        {
            char_val = (GX_CHAR_CODE)(char_val - font_link->first_glyph);
            glyph = &font_link->glyphs.compressed[char_val];

            width += (INT)glyph->advance;
        }
    }

    return width;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_string_width_kerning_font_get                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the width (in pixels) of the supplied string,     */
/*    for a given compressed font.                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    font                                  Pointer to font of strings    */
/*    string_copy                           Pointer to string             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    width                                 Destination for width of      */
/*                                            string                      */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_utf8_string_character_get                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_FONT_KERNING_SUPPORT)

// Search the kerning table for the kerning value.
static GX_UBYTE _gx_system_kerning_value_get_(const GX_UBYTE* kerning_table,
                                              GX_CHAR_CODE font_first_glyph,
                                              GX_CHAR_CODE pre_char_val)
{
    if (kerning_table && (pre_char_val != 0))
    {
        pre_char_val = (GX_CHAR_CODE)(font_first_glyph + pre_char_val);

        INT kerning_counts = *kerning_table;
        const GX_UBYTE* ptr = (const GX_UBYTE*)(kerning_table + 1);

        for (INT i = 0; i < kerning_counts; ++i)
        {
            if (ptr[0] == pre_char_val)
            {
                return ptr[1];
            }

            ptr += 2;
        }
    }

    return 0;
}

static INT _gx_system_string_width_kerning_font_get(const GX_FONT* font, GX_STRING string_copy)
{
    INT width = 0;

    GX_CHAR_CODE char_val;
    const GX_FONT* font_link;
    const GX_KERNING_GLYPH* glyph;

    // Used for kerning glyph.
    GX_CHAR_CODE pre_char_val = 0;
    GX_UBYTE kerning_offset;

    while (string_copy.length > 0)
    {
#ifdef GX_UTF8_SUPPORT
        UINT ret = _gx_utility_utf8_string_character_get(&string_copy, &char_val, GX_NULL);
        if ((ret != GX_SUCCESS) || (char_val == 0))
#else
        char_val = (GX_CHAR_CODE)(*string_copy.ptr++);
        string_copy.length--;
        if (char_val == 0)
#endif /* GX_UTF8_SUPPORT */
        {
            break;
        }

        font_link = gx_font_find_(font, char_val);
        if (font_link)
        {
            char_val = (GX_CHAR_CODE)(char_val - font_link->first_glyph);
            glyph = &font_link->glyphs.kerning[char_val];

            kerning_offset = _gx_system_kerning_value_get_(glyph->kerning_table,
                                                           font_link->first_glyph,
                                                           pre_char_val);

            width += (INT)glyph->advance;
            width += (INT)kerning_offset;
        }

        pre_char_val = char_val;
    }

    return width;
}
#endif
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_string_width_get                         PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the width (in pixels) of the supplied string,     */
/*    for a given font.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    font                                  Pointer to font of strings    */
/*    string                                Pointer to string             */
/*    string_length                         Raw length of string, in bytes*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    return_width                          Destination for width of      */
/*                                            string                      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_utf8_string_character_get                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
GX_VALUE _gx_system_string_width_get(const GX_FONT* font, const GX_CHAR* string, INT string_length)
{
    GX_STRING str;

    str.length = 0;

    if (string_length >= 0)
    {
        str.length = (UINT)string_length;
    }
    else
    {
        UINT length = 0;
        if (_gx_utility_string_length_check(string, &length, GX_MAX_STRING_LENGTH) == GX_SUCCESS)
        {
            str.length = length;
        }
    }

    GX_VALUE width = 0;

    if (str.length > 0)
    {
        str.ptr = string;
        _gx_system_string_width_get_ext(font, &str, &width);
    }

    return width;
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
/*    This service gets the width (in pixels) of the supplied string,     */
/*    for a given font.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    font                                  Pointer to font of strings    */
/*    string                                Pointer to string             */
/*    string_length                         Raw length of string, in bytes*/
/*    return_width                          Destination for width of      */
/*                                            string                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_utf8_string_character_get                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_string_width_get_ext(const GX_FONT* font, const GX_STRING* string, GX_VALUE* return_width)
{
    if (!font || !string || !string->ptr)
    {
        return GX_PTR_ERROR;
    }

    INT width = 0;

    GX_STRING string_copy = *string;

#if defined(GX_FONT_KERNING_SUPPORT)
    if (font->format & GX_FONT_FORMAT_KERNING)
    {
        width = _gx_system_string_width_kerning_font_get(font, string_copy);
    }
    else
#endif
    if (font->format & GX_FONT_FORMAT_COMPRESSED)
    {
        width = _gx_system_string_width_compressed_font_get(font, string_copy);
    }
    else
    {
        width = _gx_system_string_width_generic_font_get(font, string_copy);
    }

    if (width > GX_VALUE_MAX)
    {
        width = GX_VALUE_MAX;
    }

    *return_width = (GX_VALUE)width;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_widget_find                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service searches for the specified widget ID.                  */
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
/*    _gx_widget_find                      Search throught its children   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application Code                                               */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_widget_find(USHORT widget_id, INT search_level, GX_WIDGET** return_search_result)
{
    GX_WIDGET* root = _gx_system_root_window_created_list;
    for (; root; root = root->next)
    {
        if (root->id == widget_id)
        {
            *return_search_result = root;
            return GX_SUCCESS;
        }

        if (root->first_child)
        {
            *return_search_result = root->child_find_by_id_(widget_id, search_level);
            if (*return_search_result)
            {
                return GX_SUCCESS;
            }
        }
    }

    return GX_NOT_FOUND;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_screen_stack_create                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes a system screen stack.                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    memory                                Memory address at which to    */
/*                                            create screen stack.        */
/*    size                                  Memory size in bytes.         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_screen_stack_create(GX_WIDGET** memory, INT size)
{
    _gx_system_screen_stack.memory = memory;
    _gx_system_screen_stack.max = (INT)((UINT)size / (sizeof(GX_WIDGET*) << 1));
    _gx_system_screen_stack.top = -1;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_screen_stack_put                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function pushes the old screen pointer and its parent pointer  */
/*    onto the system screen stack                                        */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_screen_stack_put(GX_WIDGET* screen)
{
    INT top = _gx_system_screen_stack.top + 1;
    if (top < _gx_system_screen_stack.max)
    {
        _gx_system_screen_stack.top = top;

        // Push the screen and its parent to screen stack.
        top <<= 1;
        _gx_system_screen_stack.memory[top] = screen;
        _gx_system_screen_stack.memory[top + 1] = screen->parent;

        return GX_SUCCESS;
    }
    else
    {
        return GX_FAILURE;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_screen_stack_get                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function removes the topmost entry from the system screen      */
/*    stack, and returns popped parent and screen pointers to the caller. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    popped_parent                         Retrieved stack top parent    */
/*    popped_screen                         Retrieved stack top screen    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_screen_stack_get(GX_WIDGET** popped_parent, GX_WIDGET** popped_screen)
{
    UINT status;

    GX_WIDGET* screen = GX_NULL;
    GX_WIDGET* parent = GX_NULL;

    INT top = _gx_system_screen_stack.top;
    if (top >= 0)
    {
        --_gx_system_screen_stack.top;

        top <<= 1;
        screen = _gx_system_screen_stack.memory[top];
        parent = _gx_system_screen_stack.memory[top + 1];

        status = GX_SUCCESS;
    }
    else
    {
        status = GX_FAILURE;
    }

    if (popped_parent)
    {
        *popped_parent = parent;
    }

    if (popped_screen)
    {
        *popped_screen = screen;
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_screen_stack_push                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function detaches screen from its parent, and pushes the       */
/*    old screen pointer and the parent pointer onto the system screen    */
/*    stack.                                                              */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_screen_stack_push(GX_WIDGET* screen)
{
    if (_gx_system_screen_stack_put(screen) == GX_SUCCESS)
    {
        // Detached the old screen from its parent.
        screen->detach_();

        return GX_SUCCESS;
    }
    else
    {
        return GX_FAILURE;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_screen_stack_pop                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function removes the topmost entry from the system screen      */
/*    stack, and attaches the poped screen to its parent.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    control                               Pointer of stack entry.       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_screen_stack_pop(void)
{
    GX_WIDGET* screen;
    GX_WIDGET* parent;

    if (_gx_system_screen_stack_get(&parent, &screen) == GX_SUCCESS)
    {
        if (parent)
        {
            // Attach the popped screen to its parent.
            screen->attach_to_(parent);
        }

        return GX_SUCCESS;
    }
    else
    {
        return GX_FAILURE;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_screen_stack_reset                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function removes all entries from the system screen stack,     */
/*    and make the current pointer equal to the stack top.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    control                               Pointer of stack entry        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_screen_stack_reset(void)
{
    GX_WIDGET* screen;
    GX_WIDGET* parent;

    while (_gx_system_screen_stack_get(&parent, &screen) == GX_SUCCESS)
    {
        if (screen->status & GX_STATUS_DYNAMICALLY_ALLOCATED)
        {
            screen->delete_();
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_lock                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function locks the GUIX system mutex                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    tx_mutex_get                      Grab a mutex                      */
/*    _tx_thread_identify               Identify current thread           */
/*    _gx_system_error_process          Process an error                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_system_lock(void)
{
    // if the current thread already owns the GUIX system protection
    // then we don't need to lock again, just increment our nesting counter.
    // If no thread has yet locked GUIX or if the caller is not the owning
    // thread, then we need to request the gx_system_protect mutex.

    if (_gx_system_lock_thread != GX_CURRENT_THREAD)
    {
#ifdef GX_THREADX_BINDING
        UINT protection_status = tx_mutex_get(&_gx_system_protect, TX_WAIT_FOREVER);
        if (protection_status)
        {
            _gx_system_error_process(GX_SYSTEM_PROTECTION_ERROR);

            return;
        }
#else
        GX_SYSTEM_MUTEX_LOCK;
#endif
        _gx_system_lock_thread = GX_CURRENT_THREAD;
    }

    ++_gx_system_lock_nesting;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_unlock                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function unlocks the GUIX system mutex                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    tx_mutex_put                          Release the mutex             */
/*     _gx_system_error_process             Process system error          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_system_unlock(void)
{
    if (_gx_system_lock_nesting > 0)
    {
        --_gx_system_lock_nesting;
        if (_gx_system_lock_nesting == 0)
        {
            _gx_system_lock_thread = GX_NULL;

#ifdef GX_THREADX_BINDING
            UINT protection_status = tx_mutex_put(&_gx_system_protect);
            if (protection_status)
            {
                _gx_system_error_process(GX_SYSTEM_PROTECTION_ERROR);

                return;
            }
#else
            GX_SYSTEM_MUTEX_UNLOCK;
#endif
        }
    }
}

/**************************************************************************/

void _gx_system_os_timer_start()
{
#ifdef GX_THREADX_BINDING
#ifndef GX_DISABLE_THREADX_TIMER_SOURCE
    UINT tx_timer_active = 0;
    tx_timer_info_get(&_gx_system_os_timer, (CHAR**)TX_NULL, &tx_timer_active,
                      (ULONG*)TX_NULL, (ULONG*)TX_NULL, (TX_TIMER**)TX_NULL);
    if (!tx_timer_active)
    {
        tx_timer_activate(&_gx_system_os_timer);
    }
#endif
#else // !GX_THREADX_BINDING
    GX_TIMER_START;
#endif // !GX_THREADX_BINDING
}

/**************************************************************************/

void _gx_system_os_timer_stop()
{
#ifdef GX_THREADX_BINDING
#ifndef GX_DISABLE_THREADX_TIMER_SOURCE
    tx_timer_deactivate(&_gx_system_os_timer);
#endif
#else // !GX_THREADX_BINDING
    GX_TIMER_STOP;
#endif // GX_THREADX_BINDING
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_active_language_set                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the active language (deprecated).                 */
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
/*    _gx_display_active_language_set                                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_active_language_set(GX_UBYTE language)
{
    return _gx_display_active_language_set(_gx_system_display_created_list, language);
}
