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

#define LINE_LENGTH 388
#define LINE_CORNER_ANGLE_0 50
#define LINE_CORNER_ANGLE_1 130
#define LINE_CORNER_ANGLE_2 230
#define LINE_CORNER_ANGLE_3 310

/********************************************/
/* Line rotation angle illustration.        */
/*                                          */
/*  50     ->      130                      */
/*   ***************                        */
/*   **           **                        */
/*   *  *       *  *                        */
/*   *    *   *    *                        */
/*   *      *      *                        */
/*   *    *   *    *                        */
/*   *  *       *  *                        */
/*   **           **                        */
/*   ***************                        */
/*  310    <-     230                       */
/********************************************/

/* Define rotate angle for background animation.  */
static INT rotate_angle = 50;

/******************************************************************************************/
/* Update rotate angle.                                                                   */
/******************************************************************************************/
static VOID background_rotate_update()
{
    rotate_angle++;

    if (rotate_angle >= 360)
    {
        rotate_angle -= 360;
    }
    gx_system_dirty_mark(&clock_3_screen);
}

/******************************************************************************************/
/* Override the default event processing of "clock_screen_template" to handle signals     */
/* from my child widgets.                                                                 */
/******************************************************************************************/
UINT clock_3_screen_event_process(GX_WINDOW* window, GX_EVENT* event_ptr)
{
    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        clear_screen_clock_record();
        screen_clock_update(&clock_3_screen.clock_3_screen_hour, &clock_3_screen.clock_3_screen_minute, GX_NULL);
        gx_system_timer_start(window, SCREEN_CLOCK_TIMER_ID, GX_TICKS_SECOND, GX_TICKS_SECOND);
        return gx_window_event_process(window, event_ptr);

    case USER_EVENT_ANIMATION_START:
        gx_system_timer_start(window, SCREEN_ANIMATION_TIMER_ID, 100 / GX_SYSTEM_TIMER_MS, 100 / GX_SYSTEM_TIMER_MS);
        break;

    case USER_EVENT_ANIMATION_STOP:
        gx_system_timer_stop(window, SCREEN_ANIMATION_TIMER_ID);
        break;

    case GX_EVENT_TIMER:
        switch (event_ptr->payload.timer_id)
        {
        case SCREEN_CLOCK_TIMER_ID:
            screen_clock_update(&clock_3_screen.clock_3_screen_hour, &clock_3_screen.clock_3_screen_minute, GX_NULL);
            break;

        case SCREEN_ANIMATION_TIMER_ID:
            background_rotate_update();
            break;
        }
        break;

    default:
        return gx_window_event_process(window, event_ptr);
    }

    return 0;
}

/******************************************************************************************/
/* Override the default drawing of "clock_3_screen" to add some animation effects.        */
/******************************************************************************************/
VOID clock_3_screen_draw(GX_WINDOW *window)
{
    INT scaled_angle;
    INT x_dist;
    INT y_dist;
    GX_RECTANGLE* size;
    INT x_center;
    INT y_center;
    GX_POINT polygon[4];

    /* Calculate the polygon for filling.  */
    scaled_angle = GX_FIXED_VAL_MAKE(rotate_angle);
    x_dist = GX_FIXED_VAL_TO_INT(gx_utility_math_cos(scaled_angle) * LINE_LENGTH);
    y_dist = GX_FIXED_VAL_TO_INT(gx_utility_math_sin(scaled_angle) * LINE_LENGTH);

    size = &window->size;
    x_center = ((size->left + size->right) >> 1);
    y_center = ((size->top + size->bottom) >> 1);

    polygon[0].x = x_center - x_dist;
    polygon[0].y = y_center - y_dist;
    polygon[1].x = x_center + x_dist;
    polygon[1].y = y_center + y_dist;

    if (rotate_angle >= LINE_CORNER_ANGLE_0 && rotate_angle < LINE_CORNER_ANGLE_1)
    {
        polygon[2].x = size->right;
        polygon[2].y = polygon[1].y;
        polygon[3].x = size->right;
        polygon[3].y = polygon[0].y;
    }
    else if (rotate_angle >= LINE_CORNER_ANGLE_1 && rotate_angle < LINE_CORNER_ANGLE_2)
    {
        polygon[2].x = polygon[1].x;
        polygon[2].y = size->bottom;
        polygon[3].x = polygon[0].x;
        polygon[3].y = size->bottom;
    }
    else if (rotate_angle >= LINE_CORNER_ANGLE_2 && rotate_angle < LINE_CORNER_ANGLE_3)
    {
        polygon[2].x = size->left;
        polygon[2].y = polygon[1].y;
        polygon[3].x = size->left;
        polygon[3].y = polygon[0].y;
    }
    else
    {
        polygon[2].x = polygon[1].x;
        polygon[2].y = size->top;
        polygon[3].x = polygon[0].x;
        polygon[3].y = size->top;
    }

    /* Draw and fill the polygon with blue color.  */
    gx_context_brush_define(GX_COLOR_ID_BLUE, GX_COLOR_ID_BLUE, GX_BRUSH_ALIAS | GX_BRUSH_SOLID_FILL);
    gx_context_brush_width_set(1);
    gx_canvas_polygon_draw(polygon, 4);

    /* Draw the screen children.  */
    gx_widget_children_draw(window);
}