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

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_display.h"
#include "gx_canvas.h"
#include "gx_window_root.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_create                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes the animation widget.                     */
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
/*    memset                                Cleanup the memory            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gx_animation_create(GX_ANIMATION* animation)
{
    memset(animation, 0, sizeof(GX_ANIMATION));
    animation->status = GX_ANIMATION_IDLE;
    animation->landing_speed = 10;

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_delete                                                */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deletes an animation sequence if the input animation  */
/*    pointer is not NULL, otherwise, deletes all animations belong to    */
/*    the animation parent.                                               */
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
/*   _gx_animation_stop                     Deactivate an animation       */
/*   _gx_system_animation_free              Free system animation         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)

static void _gx_animation_delete_helper(GX_ANIMATION* animation)
{
    _gx_animation_stop(animation);

    if (animation->system_allocated)
    {
        _gx_system_animation_free(animation);
    }
    else
    {
        animation->status = 0; // Invalid animation
    }
}

UINT _gx_animation_delete(GX_ANIMATION* target, GX_WIDGET* parent)
{
    if (target)
    {
        _gx_animation_delete_helper(target);
    }
    else
    {
        GX_ANIMATION* next_animation;

        GX_ANIMATION* animation = _gx_system_animation_active_list;
        for (; animation; animation = next_animation)
        {
            next_animation = animation->next;

            if (animation->info.parent == parent)
            {
                _gx_animation_delete_helper(animation);
            }
        }
    }

    return GX_SUCCESS;
}

#endif /* (GX_ANIMATION_POOL_SIZE > 0) */

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_canvas_define                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function defines canvas for animation manager to use.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Animation control block       */
/*    canvas                                Memory canvas used to         */
/*                                            implement the translation   */
/*                                            animation                   */
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
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gx_animation_canvas_define(GX_ANIMATION* animation, GX_CANVAS* canvas)
{
    if (animation->status != GX_ANIMATION_IDLE)
    {
        return GX_INVALID_STATUS;
    }

    animation->canvas = canvas;

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_complete_event_send                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to send an animation complete event.       */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to animation control  */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_event_send                 Send GUIX event               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_animation_complete                                              */
/*    _gx_animation_slide_landing                                         */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
static void _gx_animation_complete_event_send(GX_ANIMATION* animation)
{
    const GX_ANIMATION_INFO& info = animation->info;

    if (info.id)
    {
        GX_EVENT complete_event;

        if (info.target)
        {
            complete_event.target = info.target;
        }
        else
        {
            complete_event.target = info.parent;
        }

        complete_event.type = GX_EVENT_ANIMATION_COMPLETE;
        complete_event.sender = info.id;
        _gx_system_event_send(&complete_event);
    }
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_complete                                              */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Called internally when an animation sequence is finished.           */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to window control     */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_animation_stop                    Stop an animation             */
/*    _gx_widget_detach                     Detach a widget from its      */
/*                                            parent                      */
/*    _gx_widget_hide                       Hide a widget                 */
/*    _gx_widget_shift                      Shift a widget                */
/*    _gx_widget_attach                     Attach a widget to its parent */
/*    _gx_system_event_send                 Send GUIX event               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_animation_update                                                */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
static void _gx_animation_complete(GX_ANIMATION* animation)
{
    _gx_animation_stop(animation);

    const GX_ANIMATION_INFO& info = animation->info;
    GX_WIDGET* target = info.target;

    if (animation->canvas)
    {
        // hide the animation root
        if (target->parent)
        {
            target->parent->hide_();
        }

        // hide animation target.
        target->detach_();

        if (info.style & GX_ANIMATION_PUSH_STACK)
        {
            _gx_system_screen_stack_push(target);
        }
        else if (info.style & GX_ANIMATION_DETACH)
        {
            if (target->status & GX_STATUS_STUDIO_CREATED)
            {
                target->delete_();
                animation->info.target = GX_NULL;
            }
        }
        else
        {
            target->shift_(info.end_position.x, info.end_position.y, GX_FALSE);

            target->attach_to_(info.parent);
        }

        animation->canvas->hide_();

        _gx_system_canvas_refresh();
    }
    else
    {
#if defined(GX_BRUSH_ALPHA_SUPPORT)
        if (info.start_alpha != info.end_alpha)
        {
            target->style &= ~GX_STYLE_USE_LOCAL_ALPHA;
            target->dirty_mark_();
        }
#endif

        if (info.style & GX_ANIMATION_PUSH_STACK)
        {
            _gx_system_screen_stack_push(target);
        }
        else if (info.style & GX_ANIMATION_DETACH)
        {
            target->hide_();

            if (target->status & GX_STATUS_STUDIO_CREATED)
            {
                target->delete_();
                 animation->info.target = GX_NULL;
            }
        }
        else
        {
            GX_VALUE xshift = (GX_VALUE)(info.end_position.x - info.start_position.x);
            GX_VALUE yshift = (GX_VALUE)(info.end_position.y - info.start_position.y);

            if (xshift || yshift)
            {
                xshift = (GX_VALUE)(info.end_position.x - target->left_());
                yshift = (GX_VALUE)(info.end_position.y - target->top_());

                target->shift_(xshift, yshift, GX_TRUE);
            }
        }
    }

    _gx_animation_complete_event_send(animation);

    if (animation->system_allocated)
    {
        _gx_system_animation_free(animation);
    }
}
#endif

/**************************************************************************/
static GX_WINDOW_ROOT* _gx_system_canvas_root_window_find(const GX_CANVAS* canvas)
{
    GX_WINDOW_ROOT* root = _gx_system_root_window_created_list;

    while (root && root->canvas != canvas)
    {
        root = (GX_WINDOW_ROOT*)root->next;
    }

    return root;
}

/**************************************************************************/
static void _gx_animation_root_window_show(GX_WINDOW_ROOT* root,
                                           GX_CANVAS* canvas,
                                           GX_UBYTE alpha)
{
    root->show_();

    UINT status = canvas->drawing_initiate_(root, root->size);
    if (status == GX_SUCCESS || status == GX_NO_VIEWS)
    {
        root->children_draw_();

        canvas->drawing_complete_(GX_FALSE);
    }

    // set the initial alpha and make our canvas visible
    canvas->alpha_set_(alpha);
    canvas->show_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_start                                                 */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function starts an animation sequence.                         */
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
/*    _gx_widget_detach                     Detach a widget from its      */
/*                                            parent                      */
/*    _gx_widget_shift                      Shift a widget                */
/*    _gx_canvas_offset_set                 Set the offset of canvas      */
/*    _gx_canvas_alpha_set                  Set the alpha of canvas       */
/*    _gx_widget_attach                     Attach a widget to its parent */
/*    _gx_widget_show                       Show a widget                 */
/*    tx_timer_info_get                     Get the information of        */
/*                                            ThreadX timer               */
/*    tx_timer_activate                     Start the ThreadX timer       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gx_animation_start(GX_ANIMATION* animation, GX_ANIMATION_INFO* info)
{
    UINT status = GX_SUCCESS;

    GX_VALUE left;
    GX_VALUE top;

    GX_WIDGET* target = info->target;

    animation->steps = info->steps;

    if (animation->canvas)
    {
        GX_CANVAS* canvas = animation->canvas;

        // Make sure the widget is not attached to any parent
        target->detach_();

        // position the target widget at 0,0 within the animation canvas
        left = target->left_();
        top = target->top_();
        if (left || top)
        {
            target->shift_((GX_VALUE)-left, (GX_VALUE)-top, GX_FALSE);
        }

        // position the canvas at the animation starting position
        canvas->offset_set_(info->start_position);

        GX_WINDOW_ROOT* root = _gx_system_canvas_root_window_find(canvas);
        if (root)
        {
            // link the target to the animation root window
            target->attach_to_(root);

            // and show the animation root window to make everything visible
            _gx_animation_root_window_show(root, canvas, info->start_alpha);
        }
        else
        {
            status = GX_FAILURE;
        }
    }
    else
    {
        if (info->start_alpha != info->end_alpha)
        {
#ifdef GX_BRUSH_ALPHA_SUPPORT
            target->style |= GX_STYLE_USE_LOCAL_ALPHA;
            target->alpha = info->start_alpha;
#else
            status = GX_INVALID_VALUE;
#endif
        }

        // position the target at the starting position
        left = (GX_VALUE)(info->start_position.x - target->left_());
        top  = (GX_VALUE)(info->start_position.y - target->top_());
        if (left || top)
        {
            target->shift_(left, top, GX_TRUE);
        }

        // link the target to the animation root window
        target->attach_to_(info->parent);
    }

    // If we were able to start this animation, link it into the active list
    if (status == GX_SUCCESS)
    {
        animation->info = *info;

        if (info->start_delay)
        {
            animation->timer_ticks = info->start_delay;
        }
        else
        {
            animation->timer_ticks = info->frame_interval;
        }

        animation->status = GX_ANIMATION_ACTIVE;

        GX_ENTER_CRITICAL

        animation->next = _gx_system_animation_active_list;
        _gx_system_animation_active_list = animation;

        GX_EXIT_CRITICAL

        _gx_system_os_timer_start();
    }

    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_stop                                                  */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function stops an animation sequence.                          */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to animation control  */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   tx_timer_deactivate                    Deactivate a timer            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_animation_complete                                              */
/*    _gx_animation_update                                                */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gx_animation_stop(GX_ANIMATION* animation)
{
    UINT status = GX_SUCCESS;

    animation->status = GX_ANIMATION_IDLE;

    GX_ENTER_CRITICAL

    if (_gx_system_animation_active_list == animation)
    {
        _gx_system_animation_active_list = _gx_system_animation_active_list->next;

        if (!_gx_system_active_timer_list &&
            !_gx_system_animation_active_list)
        {
            _gx_system_os_timer_stop();
        }
    }
    else
    {
        GX_ANIMATION* previous = _gx_system_animation_active_list;
        for (; previous; previous = previous->next)
        {
            if (previous->next == animation)
            {
                previous->next = animation->next;
                break;
            }
        }
    }

    GX_EXIT_CRITICAL

    return status;
}
#endif

/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
static void _gx_animation_update_alpha(GX_ANIMATION* animation, const GX_ANIMATION_INFO& info)
{
    INT test_alpha;

    _gx_utility_easing_function_calculate(info.style,
                                          info.start_alpha,
                                          info.end_alpha,
                                          info.steps - animation->steps,
                                          info.steps,
                                          &test_alpha);

    if (animation->canvas)
    {
        animation->canvas->alpha_set_((GX_UBYTE)test_alpha);
    }
#if defined(GX_BRUSH_ALPHA_SUPPORT)
    else
    {
        GX_WIDGET* target = info.target;

        target->alpha = (GX_UBYTE)test_alpha;

        if (test_alpha == GX_ALPHA_VALUE_OPAQUE)
        {
            target->dirty_mark_();
        }
        else
        {
            target->parent->dirty_mark_(&target->size);
        }
    }
#endif
}
#endif

/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
static void _gx_animation_update_position(GX_ANIMATION* animation, const GX_ANIMATION_INFO& info)
{
    GX_WIDGET* target = info.target;

    INT x_current;
    INT y_current;
    INT x_trans = 0;
    INT y_trans = 0;

    if (animation->canvas)
    {
        x_current = animation->canvas->display_offset_x;
        y_current = animation->canvas->display_offset_y;
    }
    else
    {
        x_current = target->left_();
        y_current = target->top_();
    }

    if (info.start_position.x != info.end_position.x)
    {
        _gx_utility_easing_function_calculate(info.style,
                                              info.start_position.x,
                                              info.end_position.x,
                                              info.steps - animation->steps,
                                              info.steps,
                                              &x_trans);

        x_trans -= x_current;
    }
    else
    {
        x_trans = 0;
    }

    if (info.start_position.y != info.end_position.y)
    {
        _gx_utility_easing_function_calculate(info.style,
                                              info.start_position.y,
                                              info.end_position.y,
                                              info.steps - animation->steps,
                                              info.steps,
                                              &y_trans);

        y_trans -= y_current;
    }
    else
    {
        y_trans = 0;
    }

    if (animation->canvas)
    {
        animation->canvas->offset_set_((GX_VALUE)(x_current + x_trans), (GX_VALUE)(y_current + y_trans));
    }
    else
    {
        if (info.style & GX_ANIMATION_BLOCK_MOVE)
        {
            GX_WIDGET* parent = info.parent;

            GX_RECTANGLE block;
            if (gx_rectangle_intersect_(target->size, parent->size, &block))
            {
                target->scroll_shift_((GX_VALUE)x_trans, (GX_VALUE)y_trans, GX_TRUE);
                parent->block_move_(&block, (GX_VALUE)x_trans, (GX_VALUE)y_trans);
            }
            else
            {
                target->shift_((GX_VALUE)x_trans, (GX_VALUE)y_trans, GX_TRUE);
            }
        }
        else
        {
            target->shift_((GX_VALUE)x_trans, (GX_VALUE)y_trans, GX_TRUE);
        }
    }
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_update                                                */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function executes one step on an animation sequence.           */
/*                                                                        */
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
/*    _gx_canvas_alpha_set                  Set the alpha of canvas       */
/*    _gx_animation_complete                Called when an animation      */
/*                                            sequence is finished        */
/*    _gx_canvas_offset_set                 Set the offset of canvas      */
/*    _gx_widget_shift                      Shift a widget                */
/*    _gx_animation_stop                    Stop an animation             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_timer_update               Update active system timers   */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
void _gx_animation_update(void)
{
    GX_ANIMATION* next_animation;

    GX_ANIMATION* animation = _gx_system_animation_active_list;
    for (; animation; animation = next_animation)
    {
        next_animation = animation->next;

        if (animation->timer_ticks > 1)
        {
            --animation->timer_ticks;
            continue;
        }

        const GX_ANIMATION_INFO& info = animation->info;

        if (info.style & GX_ANIMATION_SCREEN_DRAG)
        {
            continue;
        }

        animation->steps = (GX_UBYTE)(animation->steps - 1);

        if (animation->steps <= 0)
        {
            _gx_animation_complete(animation);
            continue;
        }

        animation->timer_ticks = info.frame_interval;

        // fade style animation
        if (info.end_alpha != info.start_alpha)
        {
            _gx_animation_update_alpha(animation, info);
        }

        // slide style animation
        if ((info.end_position.x != info.start_position.x) ||
            (info.end_position.y != info.start_position.y))
        {
            _gx_animation_update_position(animation, info);
        }
    }
}
#endif

/**************************************************************************/

static UINT _gx_animation_drag_event_process(GX_WIDGET* widget, GX_EVENT* event_ptr);

/**************************************************************************/
static inline void _gx_animation_targets_shift(GX_WIDGET* target_1, GX_WIDGET* target_2,
                                               GX_VALUE shift_x, GX_VALUE shift_y)
{
    target_1->shift_(shift_x, shift_y, GX_TRUE);

    if (target_2)
    {
        target_2->shift_(shift_x, shift_y, GX_TRUE);
    }
}

/**************************************************************************/
static inline void _gx_animation_targets_scroll_shift(GX_WIDGET* target_1, GX_WIDGET* target_2,
                                                      GX_VALUE shift_x, GX_VALUE shift_y)
{
    target_1->scroll_shift_(shift_x, shift_y, GX_TRUE);

    if (target_2)
    {
        target_2->scroll_shift_(shift_x, shift_y, GX_TRUE);
    }
}

/**************************************************************************/
static inline void _gx_widget_client_block_move(GX_WIDGET* widget, INT shift_x, INT shift_y)
{
    GX_RECTANGLE block;
    widget->client_area_get_(&block);

    widget->block_move_(&block, shift_x, shift_y);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_drag_enable                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets screen slide animation event process as a hook   */
/*    procedure of the widget's default event process.                    */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gx_animation_drag_enable(GX_ANIMATION* animation, GX_WIDGET* widget, GX_ANIMATION_INFO* info)
{
    if (animation->status != GX_ANIMATION_IDLE)
    {
        return GX_INVALID_STATUS;
    }

    // Save the widget's event process function, and replace it with the slide animation event process.
    animation->original_event_process_function = widget->event_process_function;
    widget->event_process_function = _gx_animation_drag_event_process;

    animation->info = *info;
    animation->timer_ticks = info->frame_interval;
    animation->steps = info->steps;

    GX_ENTER_CRITICAL

    animation->next = _gx_system_animation_active_list;
    _gx_system_animation_active_list = animation;

    GX_EXIT_CRITICAL

    return GX_SUCCESS;
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
/*    This function removes the screen slide aimation hook from the       */
/*    widget's default event process.                                     */
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
/*    _gx_animation_stop                    Stop an animation sequence    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gx_animation_drag_disable(GX_ANIMATION* animation, GX_WIDGET* widget)
{
    // Restore the widget's original event process function
    widget->event_process_function = animation->original_event_process_function;
    animation->original_event_process_function = GX_NULL;

    return _gx_animation_stop(animation);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_drag_tracking_start                                   */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function prepares for screen slide animation.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to animation control  */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_resize                     Resize widget                 */
/*    _gx_widget_attach                     Attach a widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_animation_drag_event_check                                      */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
static UINT _gx_animation_drag_tracking_start(GX_ANIMATION* animation, GX_POINT penpos)
{
    const GX_ANIMATION_INFO& info = animation->info;

    INT index;
    GX_WIDGET* target_1;
    GX_WIDGET* target_2;
    INT current_pos;

    // Search for visible screen, which is the first animation target.
    index = 0;
    while ((target_1 = info.slide_screen_list[index]) != GX_NULL)
    {
        if (target_1->is_visible_())
        {
            animation->slide_target_index_1 = (GX_VALUE)index;
        }

        ++index;
    }
    animation->slide_screen_list_size = (USHORT)index;

    if (animation->slide_target_index_1 < 0)
    {
        return GX_FAILURE;
    }

    if (info.style & GX_ANIMATION_VERTICAL)
    {
        current_pos = penpos.y;
    }
    else
    {
        current_pos = penpos.x;
    }

    // Find second animation target according to sliding direction.
    if (current_pos < animation->slide_tracking_start_pos)
    {
        animation->slide_direction =
            (info.style & GX_ANIMATION_VERTICAL) ?
                GX_ANIMATION_SLIDE_UP : GX_ANIMATION_SLIDE_LEFT;

        index = animation->slide_target_index_1 + 1;

        if ((index >= animation->slide_screen_list_size) &&
            (info.style & GX_ANIMATION_WRAP))
        {
            index = 0;
        }
    }
    else
    {
        animation->slide_direction =
            (info.style & GX_ANIMATION_VERTICAL) ?
                GX_ANIMATION_SLIDE_DOWN : GX_ANIMATION_SLIDE_RIGHT;

        index = animation->slide_target_index_1 - 1;

        if ((index < 0) && (info.style & GX_ANIMATION_WRAP))
        {
            index = animation->slide_screen_list_size - 1;
        }
    }

    target_1 = info.slide_screen_list[animation->slide_target_index_1];

    if ((index < 0) || (index >= animation->slide_screen_list_size))
    {
        animation->slide_target_index_2 = -1;
        target_2 = GX_NULL;
    }
    else
    {
        animation->slide_target_index_2 = (GX_VALUE)(index);
        target_2 = info.slide_screen_list[index];

        GX_RECTANGLE target_2_size;

        if (info.style & GX_ANIMATION_VERTICAL)
        {
            switch (animation->slide_direction)
            {
            case GX_ANIMATION_SLIDE_UP:
                target_2_size.top = (GX_VALUE)(target_1->bottom_() + 1);
                target_2_size.bottom = (GX_VALUE)(target_2_size.top + target_2->height_() - 1);
                break;

            default: // Down
                target_2_size.bottom = (GX_VALUE)(target_1->top_() - 1);
                target_2_size.top = (GX_VALUE)(target_2_size.bottom - target_2->height_() + 1);
                break;
            }
            target_2_size.left = target_1->left_();
            target_2_size.right = target_1->right_();
        }
        else
        {
            switch (animation->slide_direction)
            {
            case GX_ANIMATION_SLIDE_LEFT:
                target_2_size.left = (GX_VALUE)(target_1->right_() + 1);
                target_2_size.right = (GX_VALUE)(target_2_size.left + target_2->width_() - 1);
                break;

            default: // Right
                target_2_size.right = (GX_VALUE)(target_1->left_() - 1);
                target_2_size.left = (GX_VALUE)(target_2_size.right - target_2->width_() + 1);
                break;
            }
            target_2_size.top = target_1->top_();
            target_2_size.bottom = target_1->bottom_();
        }

        target_2->resize_(&target_2_size);

        if (!animation->canvas)
        {
            target_2->attach_to_(info.parent);
        }
    }

    if (animation->canvas)
    {
        GX_CANVAS* canvas = animation->canvas;

        GX_WINDOW_ROOT* root = _gx_system_canvas_root_window_find(canvas);

        GX_VALUE left;
        GX_VALUE top;

        if (animation->slide_direction == GX_ANIMATION_SLIDE_UP ||
            animation->slide_direction == GX_ANIMATION_SLIDE_LEFT ||
            (!target_2))
        {
            left = target_1->left_();
            top = target_1->top_();
        }
        else
        {
            left = target_2->left_();
            top = target_2->top_();
        }

        if (left || top)
        {
            _gx_animation_targets_shift(target_1, target_2, (GX_VALUE)-left, (GX_VALUE)-top);
        }

        // Position the canvas at the animation starting position.
        canvas->offset_set_(left, top);

        if (canvas->hardware_layer >= 0)
        {
            void (*active_display_area_set)(INT layer, GX_RECTANGLE* size);

            active_display_area_set = canvas->display->layer_services->active_display_area_set;
            if (active_display_area_set)
            {
                active_display_area_set(canvas->hardware_layer, &info.parent->size);
            }
        }

        if (root)
        {
            // link the target to the animation root window
            target_1->attach_to_(root);

            if (target_2)
            {
                target_2->attach_to_(root);
            }

            // and show the animation root window to make everything visible
            _gx_animation_root_window_show(root, canvas, info.start_alpha);
        }
    }

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
static inline bool _gx_animation_direction_changed(GX_UBYTE direction, INT delta)
{
    return (delta > 0 && (direction == GX_ANIMATION_SLIDE_LEFT  || direction == GX_ANIMATION_SLIDE_UP  )) ||
           (delta < 0 && (direction == GX_ANIMATION_SLIDE_RIGHT || direction == GX_ANIMATION_SLIDE_DOWN));
}

/**************************************************************************/
static inline GX_WIDGET* _gx_animation_target_2_get(const GX_ANIMATION* animation)
{
    if (animation->slide_target_index_2 >= 0)
    {
        return animation->info.slide_screen_list[animation->slide_target_index_2];
    }
    else
    {
        return GX_NULL;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_drag_tracking                                         */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function moves the animation screens according to current pen  */
/*      position.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to animation control  */
/*                                            block                       */
/*    penpos                                Current pen position          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_detach                     Detach a widget from its      */
/*                                            parent                      */
/*    _gx_widget_shift                      Change widget's position      */
/*    _gx_animation_drag_tracking_start     Prepare for screen draw       */
/*                                            animation                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_animation_drag_event_check                                      */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
static UINT _gx_animation_drag_tracking(GX_ANIMATION* animation, GX_POINT penpos)
{
    const GX_ANIMATION_INFO& info = animation->info;

    GX_WIDGET*  target_1;
    GX_WIDGET*  target_2;

    GX_VALUE    last_pos;
    GX_VALUE    current_pos;

    GX_VALUE    delta;
    GX_VALUE    delta_x = 0;
    GX_VALUE    delta_y = 0;

    target_1 = info.slide_screen_list[animation->slide_target_index_1];
    target_2 = _gx_animation_target_2_get(animation);

    last_pos = animation->slide_tracking_current_pos;

    if (info.style & GX_ANIMATION_VERTICAL)
    {
        current_pos = penpos.y;
        delta = delta_y = (GX_VALUE)(current_pos - last_pos);
    }
    else
    {
        current_pos = penpos.x;
        delta = delta_x = (GX_VALUE)(current_pos - last_pos);
    }

    if (delta)
    {
        GX_VALUE start_pos = animation->slide_tracking_start_pos;

        GX_VALUE shift_x = 0;
        GX_VALUE shift_y = 0;

        if (_gx_animation_direction_changed(animation->slide_direction, (INT)delta))
        {
            // Sliding direction changed, detach the second animation target
            // and call tracking start again.

            if (target_2)
            {
                target_2->detach_();
            }

            if (animation->canvas)
            {
                target_1->shift_(animation->canvas->display_offset_x,
                                 animation->canvas->display_offset_y, GX_TRUE);
            }

            _gx_animation_drag_tracking_start(animation, penpos);

            target_2 = _gx_animation_target_2_get(animation);

            if (info.style & GX_ANIMATION_VERTICAL)
            {
                shift_y = (GX_VALUE)(start_pos - last_pos);
                delta_y = (GX_VALUE)(current_pos - start_pos);
            }
            else
            {
                shift_x = (GX_VALUE)(start_pos - last_pos);
                delta_x = (GX_VALUE)(current_pos - start_pos);
            }
        }

        if (!target_2)
        {
            if (info.style & GX_ANIMATION_VERTICAL)
            {
                animation->slide_tracking_start_pos = (GX_VALUE)(start_pos + (delta_y + 1) / 2);
                delta_y /= 2;
            }
            else
            {
                animation->slide_tracking_start_pos = (GX_VALUE)(start_pos + (delta_x + 1) / 2);
                delta_x /= 2;
            }
        }

        shift_x = (GX_VALUE)(shift_x + delta_x);
        shift_y = (GX_VALUE)(shift_y + delta_y);

        if (animation->canvas)
        {
            animation->canvas->offset_set_((GX_VALUE)(animation->canvas->display_offset_x + shift_x),
                                           (GX_VALUE)(animation->canvas->display_offset_y + shift_y));
        }
        else
        {
            if (info.style & GX_ANIMATION_BLOCK_MOVE)
            {
                _gx_animation_targets_scroll_shift(target_1, target_2, shift_x, shift_y);

                _gx_widget_client_block_move(info.parent, shift_x, shift_y);
            }
            else
            {
                _gx_animation_targets_shift(target_1, target_2, shift_x, shift_y);
            }
        }

        animation->slide_tracking_current_pos = current_pos;
    }

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_landing_speed_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets landing speed for screen drag animation.         */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gx_animation_landing_speed_set(GX_ANIMATION* animation, USHORT shift_per_step)
{
    animation->landing_speed = shift_per_step;

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_slide_landing_start                                   */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function starts a timer to move the sliding screens to the     */
/*      target position step by step.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to animation control  */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_timer_start                Start a timer for a widget    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
static UINT _gx_animation_slide_landing_start(GX_ANIMATION* animation)
{
    GX_WIDGET* target;
    GX_RECTANGLE* target_size;

    if ((animation->slide_target_index_1 >= 0) &&
        (animation->status == GX_ANIMATION_IDLE))
    {
        GX_ANIMATION_INFO& info = animation->info;

        if (animation->slide_target_index_2 >= 0)
        {
            target = info.slide_screen_list[animation->slide_target_index_2];
        }
        else
        {
            target = info.slide_screen_list[animation->slide_target_index_1];
        }

        target_size = &target->size;

        if (animation->canvas)
        {
            info.start_position.x = (GX_VALUE)(target_size->left + animation->canvas->display_offset_x);
            info.start_position.y = (GX_VALUE)(target_size->top + animation->canvas->display_offset_y);
        }
        else
        {
            info.start_position.x = target_size->left;
            info.start_position.y = target_size->top;
        }

        info.parent->timer_start_(GX_ANIMATION_SLIDE_TIMER, info.frame_interval, info.frame_interval);

        animation->steps = info.steps;
        animation->status = GX_ANIMATION_SLIDE_LANDING;
    }

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_slide_landing                                         */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function shifts the sliding screens one step to target         */
/*      position.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to animation control  */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_detach                     Detach widget from its parent */
/*    _gx_widget_shift                      Change widget's position      */
/*    _gx_system_timer_stop                 Stop a timer for a widget     */
/*    _gx_animation_complete_event_send     Send a complete event to      */
/*                                            widget's parent             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_animation_drag_event_check                                      */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
static UINT _gx_animation_slide_landing(GX_ANIMATION* animation)
{
    const GX_ANIMATION_INFO& info = animation->info;
    GX_WIDGET* parent = info.parent;

    GX_RECTANGLE target_size;

    INT shift_x = 0;
    INT shift_y = 0;

    if (animation->slide_target_index_1 < 0)
    {
        // Sliding animation should have 1 target at least, this should not happen.
        parent->timer_stop_(GX_ANIMATION_SLIDE_TIMER);

        return GX_FAILURE;
    }

    GX_WIDGET* target_1 = info.slide_screen_list[animation->slide_target_index_1];
    GX_WIDGET* target_2 = _gx_animation_target_2_get(animation);

    if (target_2)
    {
        target_size = target_2->size;
    }
    else
    {
        target_size = target_1->size;
    }

    GX_CANVAS* canvas = animation->canvas;

    if (canvas)
    {
        target_size.move_(canvas->display_offset_x, canvas->display_offset_y);
    }

    if (info.style & GX_ANIMATION_EASING_FUNC_MASK)
    {
        animation->steps = (GX_UBYTE)(animation->steps - 1);

        switch (animation->slide_direction)
        {
        case GX_ANIMATION_SLIDE_LEFT:
        case GX_ANIMATION_SLIDE_RIGHT:
            _gx_utility_easing_function_calculate(info.style,
                                                  info.start_position.x,
                                                  parent->left_(),
                                                  info.steps - animation->steps,
                                                  info.steps,
                                                  &shift_x);

            shift_x -= target_size.left;
            break;

        default:
            _gx_utility_easing_function_calculate(info.style,
                                                  info.start_position.y,
                                                  parent->top_(),
                                                  info.steps - animation->steps,
                                                  info.steps,
                                                  &shift_y);

            shift_y -= target_size.top;
            break;
        }
    }
    else
    {
        switch (animation->slide_direction)
        {
        case GX_ANIMATION_SLIDE_LEFT:
            shift_x = -animation->landing_speed;
            break;

        case GX_ANIMATION_SLIDE_RIGHT:
            shift_x = animation->landing_speed;
            break;

        case GX_ANIMATION_SLIDE_UP:
            shift_y = -animation->landing_speed;
            break;

        default:
            shift_y = animation->landing_speed;
            break;
        }
    }

    if (((info.style & GX_ANIMATION_EASING_FUNC_MASK) && (info.steps != 0)) ||
        ((shift_x < 0) && ((GX_VALUE)(target_size.left + shift_x) > parent->left_())) ||
        ((shift_x > 0) && ((GX_VALUE)(target_size.left + shift_x) < parent->left_())) ||
        ((shift_y < 0) && ((GX_VALUE)(target_size.top + shift_y) > parent->top_())) ||
        ((shift_y > 0) && ((GX_VALUE)(target_size.top + shift_y) < parent->top_())))
    {
        if (canvas)
        {
            canvas->offset_set_((GX_VALUE)(canvas->display_offset_x + shift_x),
                                (GX_VALUE)(canvas->display_offset_y + shift_y));
        }
        else
        {
            if (info.style & GX_ANIMATION_BLOCK_MOVE)
            {
                _gx_animation_targets_scroll_shift(target_1, target_2, (GX_VALUE)shift_x, (GX_VALUE)shift_y);
            }
            else
            {
                _gx_animation_targets_shift(target_1, target_2, (GX_VALUE)shift_x, (GX_VALUE)shift_y);
            }
        }
    }
    else
    {
        parent->timer_stop_(GX_ANIMATION_SLIDE_TIMER);

        if (info.style & GX_ANIMATION_VERTICAL)
        {
            shift_y = parent->top_() - target_size.top;
        }
        else
        {
            shift_x = parent->left_() - target_size.left;
        }

        if (canvas)
        {
            // hide the animation root
            if (target_1->parent)
            {
                target_1->parent->hide_();
            }

            if (target_2)
            {
                target_1->detach_();

                target_1 = target_2;
            }

            target_1->shift_((GX_VALUE)(shift_x + canvas->display_offset_x),
                             (GX_VALUE)(shift_y + canvas->display_offset_y), GX_TRUE);

            target_1->attach_to_(parent);

            canvas->hide_();

            _gx_system_canvas_refresh();
        }
        else
        {
            if (target_2)
            {
                target_1->detach_();

                target_1 = target_2;
            }

            if (info.style & GX_ANIMATION_BLOCK_MOVE)
            {
                target_1->scroll_shift_((GX_VALUE)shift_x, (GX_VALUE)shift_y, GX_TRUE);
            }
            else
            {
                target_1->shift_((GX_VALUE)shift_x, (GX_VALUE)shift_y, GX_TRUE);
            }
        }

        // Reset animation information.
        animation->slide_target_index_1 = -1;
        animation->slide_target_index_2 = -1;
        animation->status = GX_ANIMATION_IDLE;

        _gx_animation_complete_event_send(animation);
    }

    if (info.style & GX_ANIMATION_BLOCK_MOVE)
    {
        _gx_widget_client_block_move(parent, shift_x, shift_y);
    }

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
static void _gx_animation_slide_direction_back(GX_ANIMATION* animation)
{
    switch (animation->slide_direction)
    {
    case GX_ANIMATION_SLIDE_LEFT:
        animation->slide_direction = GX_ANIMATION_SLIDE_RIGHT;
        break;

    case GX_ANIMATION_SLIDE_RIGHT:
        animation->slide_direction = GX_ANIMATION_SLIDE_LEFT;
        break;

    case GX_ANIMATION_SLIDE_UP:
        animation->slide_direction = GX_ANIMATION_SLIDE_DOWN;
        break;

    // case GX_ANIMATION_SLIDE_DOWN:
    default:
        animation->slide_direction = GX_ANIMATION_SLIDE_UP;
        break;
    }
}

/**************************************************************************/
static void _gx_animation_slide_back(GX_ANIMATION* animation)
{
    if (animation->slide_target_index_2 >= 0)
    {
        GX_SWAP_VALS(animation->slide_target_index_1, animation->slide_target_index_2);
    }

    _gx_animation_slide_direction_back(animation);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_drag_event_check                                      */
/*                                                           6.1.8        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles incoming events for screen    */
/*    drag animation.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    animation                             Pointer to animation control  */
/*                                            block                       */
/*    widget                                parent of animation           */
/*    event_ptr                             Event to process              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_animation_slide_landing           Move animation screens one    */
/*                                            step toward target position */
/*    _gx_animation_slide_landing_start     Prepare for screen landing    */
/*    _gx_animation_drag_tracking_start     Prepare for screen slide      */
/*                                            animation                   */
/*    _gx_animation_drag_tracking           Shift animation screens       */
/*                                            accordion to pen pos        */
/*    _gx_system_event_send                 Send an event for processing  */
/*    _gx_system_input_capture              Temporarily direct all input  */
/*                                            events to specified widget  */
/*    _gx_system_input_release              Release captured input events */
/*    _gx_system_timer_stop                 Stop a timer for a widget     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_animation_drag_event_process                                    */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
static UINT _gx_animation_drag_event_check(GX_ANIMATION* animation, GX_WIDGET* widget, GX_EVENT* event_ptr)
{
    const GX_ANIMATION_INFO& info = animation->info;

    INT delta;
    INT shift;

    switch (event_ptr->type)
    {
    case GX_EVENT_PEN_DOWN:
        if (animation->status == GX_ANIMATION_IDLE)
        {
            _gx_system_input_capture(widget);

            animation->status = GX_ANIMATION_SLIDE_TRACKING;

            if (info.style & GX_ANIMATION_VERTICAL)
            {
                animation->slide_tracking_start_pos = event_ptr->payload.pointdata.y;
                animation->slide_tracking_current_pos = event_ptr->payload.pointdata.y;
            }
            else
            {
                animation->slide_tracking_start_pos = event_ptr->payload.pointdata.x;
                animation->slide_tracking_current_pos = event_ptr->payload.pointdata.x;
            }
            animation->slide_target_index_1 = -1;
            animation->slide_target_index_2 = -1;
        }
        break;

    case GX_EVENT_PEN_DRAG:
        if (animation->status == GX_ANIMATION_SLIDE_TRACKING)
        {
            if (animation->slide_target_index_1 < 0)
            {
                if (info.style & GX_ANIMATION_VERTICAL)
                {
                    delta = GX_ABS(event_ptr->payload.pointdata.y - animation->slide_tracking_start_pos);
                }
                else
                {
                    delta = GX_ABS(event_ptr->payload.pointdata.x - animation->slide_tracking_start_pos);
                }

                if (delta > GX_ANIMATION_MIN_SLIDING_DIST)
                {
                    // Start swiping, remove other widgets from input capture stack.
                    widget->others_input_release_();

                    _gx_animation_drag_tracking_start(animation, event_ptr->payload.pointdata);
                }
            }

            // _gx_animation_drag_tracking_start() changes the |animation->slide_target_index_1|
            if (animation->slide_target_index_1 >= 0)
            {
                _gx_animation_drag_tracking(animation, event_ptr->payload.pointdata);
            }
        }
        break;

    case GX_EVENT_PEN_UP:
        if (animation->status == GX_ANIMATION_SLIDE_TRACKING)
        {
            _gx_system_input_release(widget);

            animation->status = GX_ANIMATION_IDLE;

            delta = animation->slide_tracking_current_pos - animation->slide_tracking_start_pos;

            if (animation->slide_target_index_2 < 0)
            {
                _gx_animation_slide_direction_back(animation);
            }
            else
            {
                shift = (info.style & GX_ANIMATION_VERTICAL) ?
                            widget->height_() / 2 : widget->width_() / 2;

                if (GX_ABS(delta) < shift)
                {
                    // slide back to original when slide distance is less than half screen width/height.
                    _gx_animation_slide_back(animation);
                }
            }

            if (delta)
            {
                _gx_animation_slide_landing_start(animation);
            }
        }
        break;

    case GX_EVENT_HORIZONTAL_FLICK:
    case GX_EVENT_VERTICAL_FLICK:
        if (animation->status == GX_ANIMATION_SLIDE_LANDING)
        {
            delta = event_ptr->payload.intdata[0];
            if (delta)
            {
                if (_gx_animation_direction_changed(animation->slide_direction, delta))
                {
                    // landing direction is different to flick direction
                    _gx_animation_slide_back(animation);

                    animation->status = GX_ANIMATION_IDLE;
                }

                _gx_animation_slide_landing_start(animation);
            }
        }
        break;

    case GX_EVENT_TIMER:
        if (event_ptr->payload.timer_id == GX_ANIMATION_SLIDE_TIMER)
        {
            if (animation->status != GX_ANIMATION_SLIDE_LANDING)
            {
                widget->timer_stop_(GX_ANIMATION_SLIDE_TIMER);
                break;
            }

            _gx_animation_slide_landing(animation);
        }
        break;
    }

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_animation_drag_event_process                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function handles incoming events for screen drag animation.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget control     */
/*                                            block                       */
/*    event_ptr                             Event to process              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_animation_slide_event_check       Handle incoming events for    */
/*                                            screen drag animation       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gx_animation_drag_event_process(GX_WIDGET* widget, GX_EVENT* event_ptr)
{
    GX_ANIMATION* animation = _gx_system_animation_active_list;
    for (; animation; animation = animation->next)
    {
        if ((animation->info.parent == widget) &&
            (animation->original_event_process_function))
        {
            _gx_animation_drag_event_check(animation, widget, event_ptr);

            animation->original_event_process_function(widget, event_ptr);
        }
    }

    return GX_SUCCESS;
}
#endif
