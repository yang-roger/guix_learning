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

/* Locate GUIX system data in this file.  */
#define GX_SYSTEM_INIT

#include "gx_system.h"
#include "gx_utility.h"

/**************************************************************************/

static void _gx_system_version_string_initialize()
{
    UINT index;
    UINT length;

    // We are allocating 16 bytes for the version string.
    // MAJOR_VERSION and MINOR_VERSION do not exceed 3 bytes each (Version 999.999).
    // Therefore the code below will not exceed the 16 bytes buffer. (8 + 3 + '.' + 3 + NULL).

    memset(_gx_system_version_string, 0, GX_VERSION_STRING_LENGTH);

    length = sizeof("Version ") - 1;
    memcpy(_gx_system_version_string, "Version ", length); /* Use case of memcpy is verified. */

    index = length;

    _gx_utility_ltoa(GUIX_MAJOR_VERSION, _gx_system_version_string + index, GX_VERSION_STRING_LENGTH - index);

    _gx_utility_string_length_check(_gx_system_version_string + index, &length, GX_VERSION_STRING_LENGTH - index);

    index += length;

    _gx_system_version_string[index++] = '.';

    _gx_utility_ltoa(GUIX_MINOR_VERSION, _gx_system_version_string + index, GX_VERSION_STRING_LENGTH - index);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_initialize                                               */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service initializes GUIX.                                      */
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
/*    memset                                Clear various data structures */
/*    _gx_system_error_process              Process system errors         */
/*    _gx_utility_ltoa                      Int to ASCII conversion       */
/*    _gx_utility_string_length_check       Test string length            */
/*    tx_timer_create                       Create ThreadX Timer          */
/*    tx_mutex_create                       Create system protection mutex*/
/*    tx_queue_create                       Create system event queue     */
/*    tx_queue_delete                       Delete system event queue     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_initialize(void)
{
#ifndef TX_DISABLE_ERROR_CHECKING
    UINT status;
#endif
    UINT index;

    _gx_system_version_string_initialize();

    // error
    _gx_system_last_error = GX_SUCCESS;
    _gx_system_error_count = 0;

    // created display list
    _gx_system_display_created_list = GX_NULL;
    _gx_system_display_created_count = 0;

    // created canvas list
    _gx_system_canvas_created_list = GX_NULL;
    _gx_system_canvas_created_count = 0;

    // draw context
    _gx_system_draw_context_stack_end = _gx_system_draw_context_stack;
    _gx_system_draw_context_stack_end += GX_MAX_CONTEXT_NESTING;
    _gx_system_current_draw_context = GX_NULL;

    // views memory
    for (index = 0; index < GX_MAX_VIEWS - 1; index++)
    {
        _gx_system_view_memory[index].next = &_gx_system_view_memory[index + 1];
    }
    _gx_system_view_memory[index].next = GX_NULL;
    _gx_system_free_views = _gx_system_view_memory;

    // created root window list
    _gx_system_root_window_created_list = GX_NULL;
    _gx_system_root_window_created_count = 0;

    // focus owner
    _gx_system_focus_owner = GX_NULL;

    // input capture stack
    memset(_gx_system_input_capture_stack, 0, sizeof(GX_WIDGET*) * GX_MAX_INPUT_CAPTURE_NESTING);
    _gx_system_input_capture_count = 0;
    _gx_system_input_owner = GX_NULL;

    // timers
    memset(_gx_system_timer_memory, 0, sizeof(GX_TIMER) * GX_MAX_ACTIVE_TIMERS);
    for (index = 0; index < GX_MAX_ACTIVE_TIMERS - 1; index++)
    {
        _gx_system_timer_memory[index].next = &_gx_system_timer_memory[index + 1];
    }
    _gx_system_free_timer_list = _gx_system_timer_memory;
    _gx_system_active_timer_list = GX_NULL;

    // dynamic memory
    _gx_system_memory_allocator = GX_NULL;
    _gx_system_memory_free = GX_NULL;


#if (GX_ANIMATION_POOL_SIZE > 0)

    for (index = 0; index < GX_ANIMATION_POOL_SIZE - 1; index++)
    {
        _gx_animation_create(&_gx_system_animation_pool[index]);
        _gx_system_animation_pool[index].next = &_gx_system_animation_pool[index + 1];
    }
    _gx_animation_create(&_gx_system_animation_pool[index]);
    _gx_system_animation_pool[index].next = GX_NULL;

    _gx_system_animation_free_list = _gx_system_animation_pool;

#endif // (GX_ANIMATION_POOL_SIZE > 0)

    _gx_system_animation_active_list = GX_NULL;

    // gradient list
    _gx_system_gradient_list = GX_NULL;

    // scroll bars

    _gx_system_vertical_scrollbar_style = GX_SCROLLBAR_VERTICAL | GX_SCROLLBAR_RELATIVE_THUMB | GX_SCROLLBAR_END_BUTTONS;

    memset(&_gx_system_vertical_scrollbar_appearance, 0, sizeof(GX_SCROLLBAR_APPEARANCE));
    _gx_system_vertical_scrollbar_appearance.width = 20;
    _gx_system_vertical_scrollbar_appearance.thumb_width = 18;
    _gx_system_vertical_scrollbar_appearance.thumb_color = GX_COLOR_ID_SCROLL_BUTTON;
    _gx_system_vertical_scrollbar_appearance.thumb_border_color = GX_COLOR_ID_SCROLL_BUTTON;
    _gx_system_vertical_scrollbar_appearance.button_color = GX_COLOR_ID_SCROLL_BUTTON;
    _gx_system_vertical_scrollbar_appearance.thumb_travel_min = 20;
    _gx_system_vertical_scrollbar_appearance.thumb_travel_max = 20;
    _gx_system_vertical_scrollbar_appearance.thumb_border_style = GX_STYLE_BORDER_THIN;

    _gx_system_horizontal_scrollbar_style = GX_SCROLLBAR_HORIZONTAL | GX_SCROLLBAR_RELATIVE_THUMB | GX_SCROLLBAR_END_BUTTONS;
    _gx_system_horizontal_scrollbar_appearance = _gx_system_vertical_scrollbar_appearance;

    // rich text context stack
    _gx_system_rich_text_context_stack.top = 0;
    _gx_system_rich_text_context_stack_save.top = 0;

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    _gx_system_bidi_text_enabled = GX_FALSE;
#endif

#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
    _gx_system_text_render_style = 0;
#endif

    _gx_system_clipboard = GX_NULL;
    _gx_system_clipboard_size = 0;

    memset(&_gx_system_screen_stack, 0, sizeof(GX_SCREEN_STACK_CONTROL));

    // touch configuration information
    _gx_system_pen_configuration.min_drag_dist = 80;
    _gx_system_pen_configuration.max_pen_speed_ticks = 10;

    // pen tracking variables
    _gx_system_last_clock = 0;
    _gx_system_clock_delta = 0;
    _gx_system_last_pen_point.x = 0;
    _gx_system_last_pen_point.y = 0;
    _gx_system_pen_speed_x = 0;
    _gx_system_pen_speed_y = 0;

    _gx_system_lock_nesting = 0;
    _gx_system_lock_thread = GX_NULL;

#ifdef GX_THREADX_BINDING

    // Create the required ThreadX objects.

    memset(&_gx_system_event_queue, 0, sizeof(_gx_system_event_queue));
    memset(&_gx_system_protect, 0, sizeof(_gx_system_protect));

    ULONG event_size = GX_EVENT_SIZE;
    if (event_size < sizeof(GX_EVENT))
    {
        // The constant GX_EVENT_SIZE is not large enough to hold a GX_EVENT type,
        // which is a system error.
        // GX_EVENT_SIZE must be redefined and the GUIX library must be rebuilt.

        _gx_system_error_process(GX_SYSTEM_EVENT_SIZE_MISMATCH);

        return GX_SYSTEM_ERROR;
    }

#ifndef GX_DISABLE_THREADX_TIMER_SOURCE
    memset(&_gx_system_os_timer, 0, sizeof(_gx_system_os_timer));

    tx_timer_create(&_gx_system_os_timer, "guix timer", _gx_system_timer_expiration, 0,
                    GX_SYSTEM_TIMER_TICKS, GX_SYSTEM_TIMER_TICKS, TX_NO_ACTIVATE);
#endif

#ifdef TX_DISABLE_ERROR_CHECKING
    tx_queue_create(&_gx_system_event_queue, "GUIX System Event Queue", (GX_EVENT_SIZE / sizeof(ULONG)),
                    _gx_system_event_queue_memory, sizeof(_gx_system_event_queue_memory));
#else
    status = tx_queue_create(&_gx_system_event_queue, "GUIX System Event Queue", (GX_EVENT_SIZE / sizeof(ULONG)),
                             _gx_system_event_queue_memory, sizeof(_gx_system_event_queue_memory));
    if (status != TX_SUCCESS)
    {
        _gx_system_error_process(GX_SYSTEM_QUEUE_CREATE_FAILED);

        return GX_SYSTEM_ERROR;
    }
#endif

#ifdef TX_DISABLE_ERROR_CHECKING
    tx_mutex_create(&_gx_system_protect, "GUIX System Protection", TX_NO_INHERIT);
#else
    status =  tx_mutex_create(&_gx_system_protect, "GUIX System Protection", TX_NO_INHERIT);
    if (status != TX_SUCCESS)
    {
        _gx_system_error_process(GX_SYSTEM_MUTEX_CREATE_FAILED);

        tx_queue_delete(&_gx_system_event_queue);

        return GX_SYSTEM_ERROR;
    }
#endif

    // initialize GUIX thread here. It is started later by gx_system_start
    memset(&_gx_system_thread, 0, sizeof(_gx_system_thread));

#ifdef TX_DISABLE_ERROR_CHECKING
    tx_thread_create(&_gx_system_thread, "GUIX System Thread",
                     _gx_system_thread_entry, 0,
                     _gx_system_thread_stack, sizeof(_gx_system_thread_stack),
                     GX_SYSTEM_THREAD_PRIORITY, GX_SYSTEM_THREAD_PRIORITY,
                     GX_SYSTEM_THREAD_TIMESLICE, TX_DONT_START);
#else
    status = tx_thread_create(&_gx_system_thread, "GUIX System Thread",
                              _gx_system_thread_entry, 0,
                              _gx_system_thread_stack, sizeof(_gx_system_thread_stack),
                              GX_SYSTEM_THREAD_PRIORITY, GX_SYSTEM_THREAD_PRIORITY,
                              GX_SYSTEM_THREAD_TIMESLICE, TX_DONT_START);

    if (status != TX_SUCCESS)
    {
        _gx_system_error_process(GX_SYSTEM_THREAD_CREATE_FAILED);

        tx_queue_delete(&_gx_system_event_queue);
        tx_mutex_delete(&_gx_system_protect);

        return GX_SYSTEM_ERROR;
    }
#endif
#else
    GX_RTOS_BINDING_INITIALIZE;
#endif

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_start                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service starts GUIX processing.                                */
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
/*    tx_thread_create                      Create system thread          */
/*    _gx_system_error_process              Process an error              */
/*    tx_queue_delete                       Delete a queue                */
/*    tx_mutex_delete                       Delete a mutex                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_start(void)
{
    UINT status;

#ifdef GX_THREADX_BINDING
    status = tx_thread_resume(&_gx_system_thread);
    if (status == TX_SUCCESS)
    {
        status = GX_SUCCESS;
    }
    else
    {
        _gx_system_error_process(GX_SYSTEM_THREAD_CREATE_FAILED);

        return GX_SYSTEM_ERROR;
    }
#else
    status = GX_SYSTEM_THREAD_START(_gx_system_thread_entry);
#endif

    return status;
}
