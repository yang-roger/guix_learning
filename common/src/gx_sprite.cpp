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
/**   Sprite Management (Sprite)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_sprite.h"

#include "gx_context.h"
#include "gx_canvas.h"

#define GX_SPRITE_TIMER   0x1000

static void gx_sprite_draw_(GX_SPRITE* sprite)
{
    sprite->draw_();
}

static UINT gx_sprite_event_process_(GX_SPRITE* sprite, GX_EVENT* event_ptr)
{
    return sprite->event_process_(event_ptr);
}

UINT GX_SPRITE::create_(const GX_CHAR* name, GX_WIDGET* parent,
                        GX_SPRITE_FRAME* frame_list, USHORT frame_count,
                        ULONG style, USHORT sprite_id, const GX_RECTANGLE* size)
{
    GX_WIDGET::create_(name, GX_NULL, style, sprite_id, size);

    this->type = GX_TYPE_SPRITE;

    this->frame_list = frame_list;
    this->frame_count = frame_count;
    this->current_frame = 0;
    this->run_state = GX_SPRITE_IDLE;

    this->draw_function = (void (*)(GX_WIDGET*))gx_sprite_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_sprite_event_process_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_SPRITE::frame_list_set_(GX_SPRITE_FRAME* frame_list, USHORT frame_count)
{
    stop_();

    this->frame_list = frame_list;
    this->frame_count = frame_count;
    this->current_frame = 0;

    dirty_mark_if_visible_();
}

void GX_SPRITE::current_frame_set_(USHORT frame)
{
    if (frame < this->frame_count)
    {
        this->current_frame = frame;
    }

    dirty_mark_if_visible_();
}

UINT GX_SPRITE::start_(USHORT start_frame)
{
    stop_();

    if (!is_visible_() || !frame_list || start_frame >= frame_count)
    {
        return GX_FAILURE;
    }

    current_frame = start_frame;

    UINT result = current_frame_start_();
    if (result != GX_SUCCESS)
    {
        return result;
    }

    run_state = GX_SPRITE_RUNNING;

    dirty_mark_();

    return GX_SUCCESS;
}

void GX_SPRITE::stop_()
{
    if (run_state == GX_SPRITE_RUNNING)
    {
        timer_stop_(GX_SPRITE_TIMER);
        run_state = GX_SPRITE_IDLE;
    }
}

void GX_SPRITE::update_()
{
    if (run_state != GX_SPRITE_RUNNING)
    {
        return;
    }

    if (!frame_list)
    {
        run_state = GX_SPRITE_IDLE;
    }
    else
    {
        USHORT next_frame = current_frame + 1;
        if (next_frame >= frame_count)
        {
            if (style & GX_STYLE_SPRITE_LOOP)
            {
                current_frame = 0;
            }
            else
            {
                run_state = GX_SPRITE_IDLE;
                signal_parent_(GX_EVENT_SPRITE_COMPLETE, 0);

                return;
            }
        }
        else
        {
            current_frame = next_frame;
        }

        current_frame_start_();

        dirty_mark_();
    }
}

UINT GX_SPRITE::current_frame_start_()
{
    const GX_SPRITE_FRAME& frame = frame_list[current_frame];
    UINT delay_value = (frame.delay > 0) ? frame.delay : 1;
    return timer_start_(GX_SPRITE_TIMER, delay_value, 0);
}

void GX_SPRITE::current_frame_draw_()
{
    if (frame_list)
    {
        if (current_frame < frame_count)
        {
            const GX_SPRITE_FRAME& frame = frame_list[current_frame];
            if (frame.pixelmap)
            {
                GX_DRAW_CONTEXT* context = gx_context_current_();

                context->fill_color_set_(enabled_fill_color_());

                GX_PIXELMAP* pixelmap;
                context->pixelmap_get_(frame.pixelmap, &pixelmap);
                if (pixelmap)
                {
                    GX_VALUE x = (GX_VALUE)(left_() + frame.x_offset);
                    GX_VALUE y = (GX_VALUE)(top_() + frame.y_offset);

                    if (frame.alpha == GX_ALPHA_VALUE_OPAQUE)
                    {
                        _gx_canvas_pixelmap_draw(x, y, pixelmap);
                    }
                    else
                    {
                        _gx_canvas_pixelmap_blend(x, y, pixelmap, alpha);
                    }
                }
            }
        }
    }
}

void GX_SPRITE::draw_()
{
    border_and_background_draw_();
    current_frame_draw_();
    children_draw_();
}

UINT GX_SPRITE::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        result = GX_WIDGET::event_process_(event_ptr);

        if (style & GX_STYLE_SPRITE_AUTO)
        {
            start_(current_frame);
        }
        break;

    case GX_EVENT_TIMER:
        if (event_ptr->payload.timer_id == GX_SPRITE_TIMER)
        {
            update_();
        }
        else
        {
            result = GX_WIDGET::event_process_(event_ptr);
        }
        break;

    default:
        result = GX_WIDGET::event_process_(event_ptr);
        break;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_sprite_create                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a sprite, which is a special type of          */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    sprite                                Pointer to sprite widget      */
/*                                            control block               */
/*    name                                  Logical name of sprite        */
/*    parent                                Pointer to parent widget      */
/*                                            of sprite                   */
/*    frame_list                            The frame_list to be assigned */
/*    frame_count                           Number of frames in the frame */
/*    style                                 Sprite stuyle                 */
/*    button_id                             Application-defined ID of     */
/*                                             the sprite                 */
/*    size                                  Size of the sprite            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_create                     Create the underlying widget  */
/*    _gx_widget_link                       Link the sprite to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_sprite_create(GX_SPRITE* sprite, const GX_CHAR* name, GX_WIDGET* parent,
                       GX_SPRITE_FRAME* frame_list, USHORT frame_count,
                       ULONG style, USHORT sprite_id, const GX_RECTANGLE* size)
{
    return sprite->create_(name, parent, frame_list, frame_count, style, sprite_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_sprite_current_frame_set                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service assigns the frame list to the sprite widget.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    sprite                                Pointer to sprite widget      */
/*                                            control block               */
/*    frame_index                           The frame index to display    */
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
UINT _gx_sprite_current_frame_set(GX_SPRITE* sprite, USHORT frame_index)
{
    sprite->current_frame_set_(frame_index);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_sprite_frame_list_set                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service assigns the frame list to the sprite widget.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    sprite                                Pointer to sprite widget      */
/*                                            control block               */
/*    frame_list                            The frame list to be assigned */
/*    frame_count                           Number of frames in the frame */
/*                                            list                        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_sprite_stop                       Stop a sprite widget          */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_sprite_frame_list_set(GX_SPRITE* sprite, GX_SPRITE_FRAME* frame_list, USHORT frame_count)
{
    sprite->frame_list_set_(frame_list, frame_count);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_sprite_start                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service starts the sprite widget from a given frame number.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    sprite                                Pointer to sprite widget      */
/*                                           control block                */
/*    frame_number                          The frame number to start     */
/*                                            with                        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_timer_stop                 Stop an active GUIX timer     */
/*    _gx_system_timer_start                Start a GUIX timer            */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_sprite_start(GX_SPRITE* sprite, USHORT frame_number)
{
    return sprite->start_(frame_number);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_sprite_stop                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service stops a sprite widget.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    sprite                                Pointer to sprite widget      */
/*                                           control block                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_timer_stop                 Stop an active GUIX timer     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_sprite_stop(GX_SPRITE* sprite)
{
    sprite->stop_();

    return GX_SUCCESS;
}
