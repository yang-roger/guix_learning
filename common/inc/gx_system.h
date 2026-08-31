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
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_system.h                                                         */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX system management component,             */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_SYSTEM_H
#define GX_SYSTEM_H

#include "gx_global.h"
#include "gx_point.h"
#include "gx_rectangle.h"
#include "gx_view.h"
#include "gx_event.h"
#include "gx_pen_config.h"
#include "gx_scroll_bar_appearance.h"
#include "gx_gradient.h"
#include "gx_context.h"
#include "gx_animation.h"
#include "gx_rich_text_info.h"
#include "gx_screen_stack.h"

#include "gx_system_event.h"
#include "gx_system_dirty.h"
#include "gx_system_timer.h"

struct GX_FONT;
struct GX_THEME;

struct GX_DISPLAY;
struct GX_CANVAS;

struct GX_WIDGET;
struct GX_WINDOW;
struct GX_WINDOW_ROOT;

/* Define system management function prototypes.  */

UINT _gx_system_initialize(void);
UINT _gx_system_start(void);

void _gx_system_thread_entry(ULONG id);

void _gx_system_error_process(UINT error_code);
UINT _gx_system_version_string_get_ext(GX_STRING* return_string);
UINT _gx_system_memory_allocator_set(void* (*allocate)(ULONG size), void (*release)(void*));

UINT _gx_system_draw_context_get(GX_DRAW_CONTEXT** current_context);

GX_VIEW* _gx_system_free_view_get(void);
void _gx_system_views_free(GX_VIEW* head);

UINT _gx_system_focus_claim(GX_WIDGET* widget);

UINT _gx_system_input_capture(GX_WIDGET* owner);
UINT _gx_system_input_release(GX_WIDGET* owner);

#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gx_system_animation_get(GX_ANIMATION** animation);
UINT _gx_system_animation_free(GX_ANIMATION* animation);
#endif

UINT _gx_system_gradient_create(GX_GRADIENT* gradient, GX_VALUE width, GX_VALUE height, UCHAR type, GX_UBYTE start_alpha, GX_UBYTE end_alpha);
UINT _gx_system_gradient_delete(GX_GRADIENT* gradient);

UINT _gx_system_scroll_appearance_get(ULONG style, GX_SCROLLBAR_APPEARANCE* appearance);
UINT _gx_system_scroll_appearance_set(ULONG style, const GX_SCROLLBAR_APPEARANCE* appearance);

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT _gx_system_bidi_text_enable(void);
UINT _gx_system_bidi_text_disable(void);
#endif

#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
UINT _gx_system_text_render_style_set(GX_UBYTE style);
#endif

UINT _gx_system_clipboard_get(void** data, UINT* data_size);
UINT _gx_system_clipboard_put(void* data, UINT data_size);

UINT _gx_system_pen_configure(const GX_PEN_CONFIGURATION* pen_configuration);
void _gx_system_pen_speed_init(GX_POINT* current);
void _gx_system_pen_speed_update(GX_POINT* current);
void _gx_system_pen_flick_test(GX_WIDGET* target);

UINT _gx_system_private_string_copy(GX_STRING* ptr_address, const GX_STRING* text);
UINT _gx_system_private_string_list_copy_ext(GX_STRING** ptr_address, USHORT* buffer_size, const GX_STRING* string_list, INT string_count);
void _gx_system_private_string_get(const GX_STRING* input, GX_STRING* output, ULONG style);

GX_VALUE _gx_system_string_width_get(const GX_FONT* font, const GX_CHAR* string, INT string_length);
UINT _gx_system_string_width_get_ext(const GX_FONT* font, const GX_STRING* string, GX_VALUE* return_width);

UINT _gx_system_widget_find(USHORT widget_id, INT search_level, GX_WIDGET** return_search_result);

UINT _gx_system_screen_stack_create(GX_WIDGET** memory, INT size);
UINT _gx_system_screen_stack_push(GX_WIDGET* screen);
UINT _gx_system_screen_stack_pop(void);
UINT _gx_system_screen_stack_get(GX_WIDGET** popped_parent, GX_WIDGET** popped_screen);
UINT _gx_system_screen_stack_reset(void);

void _gx_system_lock(void);
void _gx_system_unlock(void);

void _gx_system_os_timer_start();
void _gx_system_os_timer_stop();

UINT _gx_system_active_language_set(GX_UBYTE language);

/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_system_active_language_set(GX_UBYTE language);

#if (GX_ANIMATION_POOL_SIZE > 0)
UINT _gxe_system_animation_get(GX_ANIMATION** animation);
UINT _gxe_system_animation_free(GX_ANIMATION* animation);
#endif

UINT _gxe_system_canvas_refresh(void);
UINT _gxe_system_dirty_mark(GX_WIDGET* widget);
UINT _gxe_system_dirty_partial_add(GX_WIDGET* widget, GX_RECTANGLE* dirty_area);
UINT _gxe_system_draw_context_get(GX_DRAW_CONTEXT** current_context);
UINT _gxe_system_event_fold(GX_EVENT* event_ptr);
UINT _gxe_system_event_send(GX_EVENT* event_ptr);
UINT _gxe_system_focus_claim(GX_WIDGET* widget);
UINT _gxe_system_gradient_create(GX_GRADIENT* gradient, GX_VALUE width, GX_VALUE height, UCHAR type, GX_UBYTE start_alpha, GX_UBYTE end_alpha);
UINT _gxe_system_gradient_delete(GX_GRADIENT* gradient);
UINT _gxe_system_initialize(void);
UINT _gxe_system_memory_allocator_set(void* (*allocate)(ULONG size), void (*release)(void*));
UINT _gxe_system_pen_configure(GX_PEN_CONFIGURATION* pen_configuration);
UINT _gxe_system_screen_stack_create(GX_WIDGET** memory, INT size, UINT control_block_size);
UINT _gxe_system_screen_stack_get(GX_WIDGET** popped_parent, GX_WIDGET** popped_screen);
UINT _gxe_system_screen_stack_pop(void);
UINT _gxe_system_screen_stack_push(GX_WIDGET* screen);
UINT _gxe_system_scroll_appearance_get(ULONG style, GX_SCROLLBAR_APPEARANCE* return_appearance);
UINT _gxe_system_scroll_appearance_set(ULONG style, GX_SCROLLBAR_APPEARANCE* appearance);
UINT _gxe_system_start(void);
UINT _gxe_system_string_width_get_ext(const GX_FONT* font, const GX_STRING* string, GX_VALUE* return_width);
UINT _gxe_system_timer_start(GX_WIDGET* owner, UINT timer_id, UINT initial_ticks, UINT reschedule_ticks);
UINT _gxe_system_timer_stop(GX_WIDGET* owner, UINT timer_id);
UINT _gxe_system_version_string_get_ext(GX_STRING* return_string);
UINT _gxe_system_widget_find(USHORT widget_id, INT search_level, GX_WIDGET** return_search_result);

#ifdef GX_MULTITHREAD_SUPPORT

#define GX_ENTER_CRITICAL _gx_system_lock();
#define GX_EXIT_CRITICAL  _gx_system_unlock();

#else

#define GX_ENTER_CRITICAL
#define GX_EXIT_CRITICAL

#endif

struct GX_SYSTEM_AUTO_LOCK
{
    GX_SYSTEM_AUTO_LOCK()
    {
        GX_ENTER_CRITICAL
    }

    ~GX_SYSTEM_AUTO_LOCK()
    {
        GX_EXIT_CRITICAL
    }
};

/* internal structure for implementing gx_system_timer API. */
struct GX_TIMER
{
    UINT       initial_ticks;
    UINT       reschedule_ticks;
    UINT       id;
    GX_WIDGET* owner;
    GX_TIMER*  next;
};

/* System management component data declarations follow.  */

/* Determine if the initialization function of this component is including
   this file.  If so, make the data definitions really happen.  Otherwise,
   make them extern so other functions in the component can access them.  */

#ifdef GX_SYSTEM_INIT
#define SYSTEM_DECLARE
#else
#define SYSTEM_DECLARE          extern
#endif

#define GX_VERSION_STRING_LENGTH    16

/* Define string for holding system version information */
SYSTEM_DECLARE GX_CHAR _gx_system_version_string[GX_VERSION_STRING_LENGTH];

/* Define the storage for the last system error code.  */
SYSTEM_DECLARE UINT _gx_system_last_error;

/* Define the number of system errors.  */
SYSTEM_DECLARE ULONG _gx_system_error_count;

/* Define the created display list head pointer.  */
SYSTEM_DECLARE GX_DISPLAY* _gx_system_display_created_list;

/* Define the number of created displays.  */
SYSTEM_DECLARE ULONG _gx_system_display_created_count;

/* Define the created canvas list head pointer.  */
SYSTEM_DECLARE GX_CANVAS* _gx_system_canvas_created_list;

/* Define the number of created canvases.  */
SYSTEM_DECLARE ULONG _gx_system_canvas_created_count;

/* define memory for holding nested drawing context */
SYSTEM_DECLARE GX_DRAW_CONTEXT _gx_system_draw_context_stack[GX_MAX_CONTEXT_NESTING];

/* define pointer to the current active drawing context */
SYSTEM_DECLARE GX_DRAW_CONTEXT* _gx_system_current_draw_context;

/* define pointer to the end of the draw context stack */
SYSTEM_DECLARE GX_DRAW_CONTEXT* _gx_system_draw_context_stack_end;

/* storage for the system views */
SYSTEM_DECLARE GX_VIEW _gx_system_view_memory[GX_MAX_VIEWS];

/* linked list of free GX_VIEWS */
SYSTEM_DECLARE GX_VIEW* _gx_system_free_views;

/* Define the root window list head pointer.  */
SYSTEM_DECLARE GX_WINDOW_ROOT* _gx_system_root_window_created_list;

/* Define the number of created root windows.  */
SYSTEM_DECLARE ULONG _gx_system_root_window_created_count;

/* Define pointer to widget that owns input focus */
SYSTEM_DECLARE GX_WIDGET* _gx_system_focus_owner;

/* stack of input owners */
SYSTEM_DECLARE GX_WIDGET* _gx_system_input_capture_stack[GX_MAX_INPUT_CAPTURE_NESTING];
SYSTEM_DECLARE GX_WIDGET* _gx_system_input_owner;
SYSTEM_DECLARE INT        _gx_system_input_capture_count;

/* define storage for GX_TIMERs */
SYSTEM_DECLARE GX_TIMER _gx_system_timer_memory[GX_MAX_ACTIVE_TIMERS];

/* linked list of running timers */
SYSTEM_DECLARE GX_TIMER* _gx_system_active_timer_list;

/* linked list of free timers */
SYSTEM_DECLARE GX_TIMER* _gx_system_free_timer_list;

/* application defined memory allocation function */
SYSTEM_DECLARE void* (*_gx_system_memory_allocator)(ULONG);

/* application defined memory free function */
SYSTEM_DECLARE void (*_gx_system_memory_free)(void*);

/* define scratchpad area for storing line end points
   used by wide line, circle, and arc functions */

/* Scratch area. */
SYSTEM_DECLARE INT _gx_system_scratchpad[GX_MAX_DISPLAY_HEIGHT * 2];

/* Define memory for holding GX_ANIMATION structure pool */
#if (GX_ANIMATION_POOL_SIZE > 0)
SYSTEM_DECLARE GX_ANIMATION  _gx_system_animation_pool[GX_ANIMATION_POOL_SIZE];
SYSTEM_DECLARE GX_ANIMATION* _gx_system_animation_free_list;
#endif

/* linked list of active animations */
SYSTEM_DECLARE GX_ANIMATION* _gx_system_animation_active_list;

/* linked list of created gradients */
SYSTEM_DECLARE GX_GRADIENT* _gx_system_gradient_list;

/* storage for default vertical scrollbar appearance */
SYSTEM_DECLARE ULONG                   _gx_system_vertical_scrollbar_style;
SYSTEM_DECLARE GX_SCROLLBAR_APPEARANCE _gx_system_vertical_scrollbar_appearance;

/* storage for default horizontal scrollbar appearance */
SYSTEM_DECLARE ULONG                   _gx_system_horizontal_scrollbar_style;
SYSTEM_DECLARE GX_SCROLLBAR_APPEARANCE _gx_system_horizontal_scrollbar_appearance;

/* stack of rich text context. */
SYSTEM_DECLARE GX_RICH_TEXT_CONTEXT_STACK _gx_system_rich_text_context_stack;

/* temporary stack used to save rich text context stack. */
SYSTEM_DECLARE GX_RICH_TEXT_CONTEXT_STACK _gx_system_rich_text_context_stack_save;

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
/* Define text style. */
SYSTEM_DECLARE GX_BOOL _gx_system_bidi_text_enabled;
#endif

#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
SYSTEM_DECLARE GX_UBYTE _gx_system_text_render_style;
#endif

SYSTEM_DECLARE void* _gx_system_clipboard;
SYSTEM_DECLARE UINT  _gx_system_clipboard_size;

/* Define touch configuration information. */
SYSTEM_DECLARE GX_PEN_CONFIGURATION _gx_system_pen_configuration;

/* Define variables for tracking pen speed */
SYSTEM_DECLARE ULONG    _gx_system_last_clock;
SYSTEM_DECLARE LONG     _gx_system_clock_delta;
SYSTEM_DECLARE GX_POINT _gx_system_last_pen_point;
SYSTEM_DECLARE INT      _gx_system_pen_speed_x;
SYSTEM_DECLARE INT      _gx_system_pen_speed_y;

SYSTEM_DECLARE GX_SCREEN_STACK_CONTROL _gx_system_screen_stack;

#ifdef GX_THREADX_BINDING

/* guix system thread control block */
SYSTEM_DECLARE TX_THREAD _gx_system_thread;

/* guix system thread stack */
SYSTEM_DECLARE GX_UBYTE _gx_system_thread_stack[GX_THREAD_STACK_SIZE];

/* Keep track of which thread has GUIX locked */
SYSTEM_DECLARE TX_THREAD* _gx_system_lock_thread;

/* Keep track of which thread has block input */
SYSTEM_DECLARE TX_THREAD* _gx_system_input_lock_thread;

/* Define the system event queue.  */
SYSTEM_DECLARE TX_QUEUE _gx_system_event_queue;

/* Define the system protection mutex.  */
SYSTEM_DECLARE TX_MUTEX _gx_system_protect;

/* Define the system input lock mutex. */
SYSTEM_DECLARE TX_MUTEX _gx_system_input_protect;

/* Define the system timer.  */
#ifndef GX_DISABLE_THREADX_TIMER_SOURCE
SYSTEM_DECLARE TX_TIMER _gx_system_os_timer;
#endif

/* Define the memory area for the system event queue.  */
SYSTEM_DECLARE GX_UBYTE _gx_system_event_queue_memory[GX_MAX_QUEUE_EVENTS * GX_EVENT_SIZE];

#define GX_CURRENT_THREAD _tx_thread_identify()

#else

SYSTEM_DECLARE void *_gx_system_lock_thread;

#endif  /* GX_THREADX_BINDING */


/* Define the system mutex lock count.  */
SYSTEM_DECLARE UINT _gx_system_lock_nesting;

/* Define the system input lock count. */
SYSTEM_DECLARE UINT _gx_system_input_lock_nesting;

#endif

