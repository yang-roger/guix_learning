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

#include <stdio.h>
#include "gx_api.h"

#include "guix_medical_mouse_support_resources.h"
#include "guix_medical_mouse_support_specifications.h"

GX_WINDOW_ROOT    *root;
GX_MOUSE_CURSOR_INFO mouse_cursor;

#define TIME_DATE_TIMER  100

const GX_CHAR *day_names[7] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};

const GX_CHAR *month_names[12] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

/* Define prototypes.   */

VOID  start_guix(VOID);
extern UINT win32_graphics_driver_setup_24xrgb(GX_DISPLAY *display);
VOID patient_list_children_create(PATIENTS_SCREEN_CONTROL_BLOCK *win);
UINT gx_prompt_text_set(GX_PROMPT *prompt, const GX_CHAR *text);

int main(int argc, char ** argv)
{
  tx_kernel_enter();
  return(0);
}


VOID tx_application_define(void *first_unused_memory)
{
    start_guix();
}

VOID  start_guix(VOID)
{
    GX_CANVAS *canvas;

    GX_DISPLAY *display;

    /* Initialize GUIX.  */
    gx_system_initialize();

    gx_studio_display_configure(MAIN_DISPLAY, win32_graphics_driver_setup_24xrgb,
                                LANGUAGE_ENGLISH, MAIN_DISPLAY_DEFAULT_THEME, &root);

    display = root->canvas->display;

    /* create the patients screen */
    gx_studio_named_widget_create("patients_screen", (GX_WIDGET *)root, GX_NULL);
    patient_list_children_create(&patients_screen);

    /* create the vitals screen */
    gx_studio_named_widget_create("vitals_screen", GX_NULL, GX_NULL);

    /* create the meds screen */
    gx_studio_named_widget_create("meds_screen", GX_NULL, GX_NULL);


    canvas = root->canvas;
    mouse_cursor.image_id = GX_PIXELMAP_ID_MOUSE;
    mouse_cursor.hotspot_x = 0;
    mouse_cursor.hotspot_y = 0;

    gx_canvas_mouse_define(canvas, &mouse_cursor);
    gx_canvas_mouse_show(canvas);

    /* Show the root window to make it and patients screen visible.  */
    gx_widget_show(root);

    /* start GUIX thread */
    gx_system_start();
}

void ToggleScreen(GX_WIDGET *new_win, GX_WIDGET *old_win)
{
    if (!new_win->parent)
    {
        gx_widget_attach(root, new_win);
    }
    else
    {
        gx_widget_show(new_win);
    }
    gx_widget_hide(old_win);
}

VOID UpdateTimeDate(MED_SCREEN_BASE_CONTROL_BLOCK *window)
{
#ifdef WIN32
static GX_CHAR time_string[6];
static GX_CHAR am_pm[3];
static GX_CHAR date_string[20];

SYSTEMTIME local_time;
    GetLocalTime(&local_time);
    if (local_time.wHour < 12)
    {
        sprintf(time_string, "%d:%02d", local_time.wHour, local_time.wMinute);
        GX_STRCPY(am_pm, "AM");
    }
    else
    {
        sprintf(time_string, "%d:%02d", local_time.wHour - 12, local_time.wMinute);
        GX_STRCPY(am_pm, "PM");
    }
    gx_prompt_text_set(&window->med_screen_base_time_of_day, time_string);
    gx_prompt_text_set(&window->med_screen_base_AM_PM, am_pm);
    gx_prompt_text_set(&window->med_screen_base_day_prompt, day_names[local_time.wDayOfWeek]);

    sprintf(date_string, "%s %02d, %d", month_names[local_time.wMonth - 1], local_time.wDay, local_time.wYear);
    gx_prompt_text_set(&window->med_screen_base_date_prompt, date_string);

#else
#endif
}

UINT med_screen_base_event_handler(GX_WINDOW *window, GX_EVENT *myevent)
{
    switch (myevent->type)
    {
    case GX_EVENT_SHOW:
        gx_window_event_process(window, myevent);
        UpdateTimeDate((MED_SCREEN_BASE_CONTROL_BLOCK *) window);
        gx_system_timer_start(window, TIME_DATE_TIMER, GX_TICKS_SECOND * 60, GX_TICKS_SECOND * 30);
        break;

    case GX_EVENT_TIMER:
        UpdateTimeDate((MED_SCREEN_BASE_CONTROL_BLOCK *) window);
        break;

    default:
        return gx_window_event_process(window, myevent);
    }
    return 0;
}

/* Define prototypes.   */
VOID page_button_draw(GX_PIXELMAP_BUTTON *widget)
{
    GX_PIXELMAP *bkgnd;
    GX_PIXELMAP *icon;

    GX_STRING text;
    GX_FONT *font;
    INT width;
    GX_VALUE string_width;
    INT xpos;
    INT ypos;
    INT top;
    INT left;
    INT parent_id = 0;
    GX_BOOL draw_selected = GX_FALSE;

    left = widget->size.left;
    top = widget->size.top;

    if (widget->style & GX_STYLE_BUTTON_PUSHED)
    {
        draw_selected = GX_TRUE;
        top += 2;
    }
    if (widget->parent)
    {
        parent_id = widget->parent->id;
    }

    switch(widget->id)
    {
    case ID_PATIENTS_BUTTON:
        gx_widget_pixelmap_get(widget, GX_PIXELMAP_ID_ICON_PATIENTS, &icon);
        text.ptr = "Patient List";
        text.length = 12;

        if (parent_id == ID_PATIENTS_SCREEN)
        {
            draw_selected = GX_TRUE;
        }
        break;

    case ID_MEDS_BUTTON:
        gx_widget_pixelmap_get(widget, GX_PIXELMAP_ID_ICON_MEDICATIONS, &icon);
        text.ptr = "Medications";
        text.length = 11;

        if (parent_id == ID_MEDS_SCREEN)
        {
            draw_selected = GX_TRUE;
        }
        break;

    case ID_VITALS_BUTTON:
        gx_widget_pixelmap_get(widget, GX_PIXELMAP_ID_ICON_VITALS, &icon);
        text.ptr = "Vitals";
        text.length = 6;

        if (parent_id == ID_VITALS_SCREEN)
        {
            draw_selected = GX_TRUE;
        }
        break;
    }

    if (draw_selected)
    {
        gx_widget_pixelmap_get(widget, GX_PIXELMAP_ID_BUTTON_ACTIVE, &bkgnd);
        gx_context_fill_color_set(GX_COLOR_ID_WHITE);
        gx_context_line_color_set(GX_COLOR_ID_WHITE);
    }
    else
    {
        gx_widget_pixelmap_get(widget, GX_PIXELMAP_ID_BUTTON, &bkgnd);
        gx_context_fill_color_set(GX_COLOR_ID_LIGHT_GRAY);
        gx_context_line_color_set(GX_COLOR_ID_LIGHT_GRAY);
    }

    gx_canvas_pixelmap_draw(left, top, bkgnd);
    gx_widget_font_get(widget, GX_FONT_ID_SMALL_BOLD, &font);
    gx_system_string_width_get_ext(font, &text, &string_width);

    // draw the icon
    width = icon->width + string_width + 10;
    xpos = widget->size.right - widget->size.left + 1;
    xpos -= width;
    xpos = left + (xpos / 2);

    ypos = widget->size.bottom - widget->size.top + 1;
    ypos -= icon->height;
    ypos = top + (ypos / 2);

    gx_canvas_pixelmap_draw(xpos, ypos, icon);

    // draw the text
    xpos += icon->width + 10;

    ypos = widget->size.bottom - widget->size.top + 1;
    ypos -= font->line_height;
    ypos = top + (ypos / 2);
    gx_context_font_set(GX_FONT_ID_SMALL_BOLD);
    gx_canvas_text_draw_ext(xpos, ypos, &text);
}

UINT gx_prompt_text_set(GX_PROMPT *prompt, const GX_CHAR *text)
{
    GX_STRING string;
    UINT result = _gx_utility_string_set(&string, text, GX_MAX_STRING_LENGTH);
    if (result == GX_SUCCESS)
    {
        gx_prompt_text_set_ext(prompt, &string);
    }

    return result;
}

