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

/* This is a small demo of the high-performance GUIX graphics framework. */

#include "demo_guix_car_infotainment.h"

/* Define macros.  */
#define PROGRESS_BAR_ANIMATION_TOTAL_STEPS 20
#define MAX_VOLUME 100
#define MIN_VOLUME 0

/* Define local variables.  */
static GX_BOOL is_mute = GX_FALSE;
static int volume_progress_val = 23;
static int progress_bar_animation_step = 0;
static int progress_bar_animation_target_val = 0;
static int progress_bar_animation_start_val = 0;

extern GX_WINDOW *current_screen;

/******************************************************************************************/
/* Update volume value.                                                                   */
/******************************************************************************************/
static void update_volume_prompt(VOLUME_SCREEN_BASE_CONTROL_BLOCK *base)
{
    INT volume;
    GX_PROGRESS_BAR_INFO* info = &base->volume_screen_base_volume_progress_bar.info;

    volume = info->current_val - info->min_val;
    volume *= (MAX_VOLUME - MIN_VOLUME);
    volume /= info->max_val - info->min_val;
    volume += MIN_VOLUME;

    if (!is_mute)
    {
        volume_progress_val = info->current_val;
    }

    gx_numeric_prompt_value_set(&base->volume_screen_base_volume_value, volume);
}

/******************************************************************************************/
/* Start progress bar animation.                                                          */
/******************************************************************************************/
static void progress_bar_animation_start(VOLUME_SCREEN_BASE_CONTROL_BLOCK *base, int target_val)
{
    GX_PROGRESS_BAR_INFO* info = &base->volume_screen_base_volume_progress_bar.info;

    progress_bar_animation_target_val = target_val;
    progress_bar_animation_start_val = info->current_val;
    progress_bar_animation_step = 0;
    gx_system_timer_start(base, TIMER_ID_PROGRESS_BAR_ANIMATION, 20 / GX_SYSTEM_TIMER_MS, 20 / GX_SYSTEM_TIMER_MS);
}

/******************************************************************************************/
/* Update progress bar animation.                                                         */
/******************************************************************************************/
static void progress_bar_animation_update(VOLUME_SCREEN_BASE_CONTROL_BLOCK *base)
{
    GX_PROGRESS_BAR* progress_bar = &base->volume_screen_base_volume_progress_bar;
    INT val;

    progress_bar_animation_step++;

    val = (progress_bar_animation_target_val - progress_bar_animation_start_val) * progress_bar_animation_step;
    val /= PROGRESS_BAR_ANIMATION_TOTAL_STEPS;
    val += progress_bar_animation_start_val;

    gx_progress_bar_value_set(progress_bar, val);

    if (progress_bar_animation_step == PROGRESS_BAR_ANIMATION_TOTAL_STEPS)
    {
        gx_system_timer_stop(base, TIMER_ID_PROGRESS_BAR_ANIMATION);
    }
}

/******************************************************************************************/
/* Start volume progress bar animation.                                                   */
/******************************************************************************************/
static void volume_progress_bar_animation_start(VOLUME_SCREEN_BASE_CONTROL_BLOCK *base)
{
    GX_PROGRESS_BAR *progress_bar;
    INT progress_val;
    GX_ANIMATION *animation;
    GX_ANIMATION_INFO info;

    gx_system_animation_get(&animation);

    if (animation)
    {
        memset(&info, 0, sizeof(GX_ANIMATION_INFO));
        info.target = (GX_WIDGET*)&base->volume_screen_base_volume_progress_bar_win;
        info.parent = (GX_WIDGET *)base;
        info.start_position.x = 640;
        info.start_position.y = info.target->size.top;
        info.end_position.y = info.start_position.y;
        info.end_position.x = info.target->size.left;
        info.frame_interval = 20 / GX_SYSTEM_TIMER_MS;
        info.steps = 20;
        info.start_alpha = 255;
        info.end_alpha = 255;
        gx_animation_start(animation, &info);
    }

    if (!is_mute)
    {
        progress_bar = &base->volume_screen_base_volume_progress_bar;
        progress_val = progress_bar->info.current_val;
        gx_progress_bar_value_set(progress_bar, 0);
        progress_bar_animation_start(base, progress_val);
    }


}

/******************************************************************************************/
/* Override the default event processing of "phone_screen" to handle signals from my      */
/* child widgets.                                                                         */
/******************************************************************************************/
UINT volume_screen_base_event_process(GX_WINDOW *window, GX_EVENT* event_ptr)
{
    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        volume_progress_bar_animation_start((VOLUME_SCREEN_BASE_CONTROL_BLOCK *)window);
        screen_base_event_process(window, event_ptr);
        break;

    case GX_SIGNAL(ID_VOLUME_PROGRESS_BAR, GX_EVENT_PROGRESS_VALUE):
        update_volume_prompt((VOLUME_SCREEN_BASE_CONTROL_BLOCK *)window);
        break;

    case GX_SIGNAL(ID_BTN_MUTE, GX_EVENT_TOGGLE_ON):
        is_mute = GX_TRUE;
        progress_bar_animation_start((VOLUME_SCREEN_BASE_CONTROL_BLOCK *)window, 0);
        break;

    case GX_SIGNAL(ID_BTN_MUTE, GX_EVENT_TOGGLE_OFF):
        is_mute = GX_FALSE;
        progress_bar_animation_start((VOLUME_SCREEN_BASE_CONTROL_BLOCK *)window, volume_progress_val);
        break;

    case GX_EVENT_TIMER:
        if (event_ptr->payload.timer_id == TIMER_ID_PROGRESS_BAR_ANIMATION)
        {
            progress_bar_animation_update((VOLUME_SCREEN_BASE_CONTROL_BLOCK *)window);
        }
        break;

    default:
        return screen_base_event_process(window, event_ptr);
    }

    return 0;
}

/******************************************************************************************/
/* Override the default event processing of "volume_progress_bar" to handle pen down      */
/* event.                                                                                 */
/******************************************************************************************/
UINT volume_progress_bar_event_process(GX_PROGRESS_BAR *bar, GX_EVENT* event_ptr)
{
    INT val;
    GX_RECTANGLE* size = &bar->size;
    GX_PROGRESS_BAR_INFO* info = &bar->info;
    VOLUME_SCREEN_BASE_CONTROL_BLOCK *base;

    if (event_ptr->type == GX_EVENT_PEN_DOWN)
    {
        if (is_mute)
        {
            /* Unmute.  */
            is_mute = GX_FALSE;

            if (current_screen)
            {
                base = (VOLUME_SCREEN_BASE_CONTROL_BLOCK*)current_screen;
                gx_widget_style_remove(&base->volume_screen_base_mute_btn, GX_STYLE_BUTTON_PUSHED);
            }
        }

        val = (size->bottom - event_ptr->payload.pointdata.y + 2);
        val *= (info->max_val - info->min_val);
        val /= (size->bottom - size->top + 1);
        val += info->min_val;

        gx_progress_bar_value_set(bar, val);
        return GX_SUCCESS;
    }
    else
    {
        return gx_progress_bar_event_process(bar, event_ptr);
    }
}
