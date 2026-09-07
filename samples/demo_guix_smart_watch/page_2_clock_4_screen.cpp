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

#include "demo_guix_smart_watch.h"

#define CLOCK_SLIDE_SHIFT 30

#define TARGET_XPOS_MIN 227
#define TARGET_XPOS_MAX (TARGET_XPOS_MIN + CLOCK_SLIDE_SHIFT)

/* Define clock slide animation control blocks. */
static GX_ANIMATION clock_slide_animation[2];

/******************************************************************************************/
/* Start clock slide animation.                                                           */
/******************************************************************************************/
static void clock_slide_animation_start()
{
    GX_ANIMATION_INFO info;
    GX_WIDGET *target_hour = (GX_WIDGET*)&clock_4_screen.clock_4_screen_hour;
    GX_WIDGET *target_minute = (GX_WIDGET*)&clock_4_screen.clock_4_screen_minute;
    GX_VALUE hour_end_xpos;
    GX_VALUE minute_end_xpos;

    if (target_hour->size.left < target_minute->size.left)
    {
        hour_end_xpos = TARGET_XPOS_MAX;
        minute_end_xpos = TARGET_XPOS_MIN;
    }
    else
    {
        hour_end_xpos = TARGET_XPOS_MIN;
        minute_end_xpos = TARGET_XPOS_MAX;
    }

    memset(&info, 0, sizeof(GX_ANIMATION_INFO));
    info.parent = (GX_WIDGET *)&clock_4_screen;
    info.target = target_hour;
    info.start_alpha = 255;
    info.end_alpha = 255;
    info.frame_interval = 40 / GX_SYSTEM_TIMER_MS;
    info.id = CLOCK_SLIDE_ANIMATION_ID;
    info.start_position.x = target_hour->size.left;
    info.end_position.x = hour_end_xpos;
    info.start_position.y = target_hour->size.top;
    info.end_position.y = info.start_position.y;
    info.steps = GX_ABS(info.end_position.x - info.start_position.x);

    gx_animation_start(&clock_slide_animation[0], &info);

    info.target = target_minute;
    info.start_position.x = target_minute->size.left;
    info.end_position.x = minute_end_xpos;
    info.start_position.y = target_minute->size.top;
    info.end_position.y = info.start_position.y;

    gx_animation_start(&clock_slide_animation[1], &info);
}

/******************************************************************************************/
/* Stop clock slide animation.                                                            */
/******************************************************************************************/
static void clock_slide_animation_stop()
{
    gx_animation_stop(&clock_slide_animation[0]);
    gx_animation_stop(&clock_slide_animation[1]);
}

/******************************************************************************************/
/* Override the default event processing of "clock_screen_template" to handle signals     */
/* from my child widgets.                                                                 */
/******************************************************************************************/
UINT clock_4_screen_event_process(GX_WINDOW* window, GX_EVENT* event_ptr)
{
    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        gx_animation_create(&clock_slide_animation[0]);
        gx_animation_create(&clock_slide_animation[1]);
        clear_screen_clock_record();
        screen_clock_update(&clock_4_screen.clock_4_screen_hour, &clock_4_screen.clock_4_screen_minute, GX_NULL);
        gx_system_timer_start(window, SCREEN_CLOCK_TIMER_ID, GX_TICKS_SECOND, GX_TICKS_SECOND);
        return gx_window_event_process(window, event_ptr);

    case GX_EVENT_HIDE:
        gx_system_timer_stop(window, SCREEN_CLOCK_TIMER_ID);
        gx_system_timer_stop(window, SCREEN_ANIMATION_TIMER_ID);
        return gx_window_event_process(window, event_ptr);

    case USER_EVENT_ANIMATION_START:
        clock_slide_animation_start();
        break;

    case USER_EVENT_ANIMATION_STOP:
        clock_slide_animation_stop();
        break;

    case GX_EVENT_ANIMATION_COMPLETE:
        if (event_ptr->sender == CLOCK_SLIDE_ANIMATION_ID)
        {
            clock_slide_animation_start();
        }
        break;

    case GX_EVENT_TIMER:
        if (event_ptr->payload.timer_id == SCREEN_CLOCK_TIMER_ID)
        {
            screen_clock_update(&clock_4_screen.clock_4_screen_hour, &clock_4_screen.clock_4_screen_minute, GX_NULL);
        }
        break;

    default:
        return gx_window_event_process(window, event_ptr);
    }

    return 0;
}