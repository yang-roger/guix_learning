#ifndef GX_GLOBAL_H
#define GX_GLOBAL_H

/* Define the major/minor version information that can be used by the application
   and the GUIX source as well.  */

#define AZURE_RTOS_GUIX
#define GUIX_MAJOR_VERSION  6
#define GUIX_MINOR_VERSION  5
#define GUIX_PATCH_VERSION  1
#define GUIX_BUILD_VERSION  202602
#define GUIX_HOTFIX_VERSION ' '

/* The following symbols are defined for backward compatibility reasons.*/
#define __PRODUCT_GUIX__
#define __GUIX_MAJOR_VERSION GUIX_MAJOR_VERSION
#define __GUIX_MINOR_VERSION GUIX_MINOR_VERSION

#ifdef __cplusplus
#define GX_EXTERN_C extern "C"
#else
#define GX_EXTERN_C extern
#endif

/* Include ThreadX API and GUIX port-specific include file.  */
#ifndef GX_DISABLE_THREADX_BINDING
#include "tx_api.h"
#define GX_THREADX_BINDING
#else

/* Define data types used by GUIX that are normally defined within ThreadX headers  */

#define VOID void
typedef char                                    CHAR;
typedef unsigned char                           UCHAR;
typedef int                                     INT;
typedef unsigned int                            UINT;
typedef long                                    LONG;
typedef unsigned long                           ULONG;
typedef short                                   SHORT;
typedef unsigned short                          USHORT;
#endif

#include "gx_port.h"

/* Define signed 8-bit value */
typedef signed char                             GX_BYTE;

/* Define unsigned 8-bit value */
typedef unsigned char                           GX_UBYTE;

/* Define Resource ID.  */
typedef ULONG                                   GX_RESOURCE_ID;

/* Define Color type.  */
typedef ULONG                                   GX_COLOR;

/* Fixed point math data type */
typedef signed long GX_FIXED_VAL;

typedef char GX_CHAR;

/* Define the String type. */
struct GX_STRING
{
    const GX_CHAR*     ptr;     /* Pointer to string. */
    UINT               length;  /* String length in bytes. */
};

/* Define constants for GUIX.  */

/* Define configuration-specific constants. These constants can be redefined externally.  */

/* Default 20ms GUIX system timer.  */
#ifndef GX_SYSTEM_TIMER_MS
#define GX_SYSTEM_TIMER_MS        20
#endif

#if defined(GX_THREADX_BINDING)

/* Set default ThreadX timer tick frequency 100Hz (10ms timer). */
#ifndef TX_TIMER_TICKS_PER_SECOND
#define TX_TIMER_TICKS_PER_SECOND ((ULONG)100)
#endif

/* Derive GX_SYSTEM_TIMER_TICKS based on GX_SYSTEM_TIMER_MS value. */
#ifndef GX_SYSTEM_TIMER_TICKS
#define GX_SYSTEM_TIMER_TICKS     ((GX_SYSTEM_TIMER_MS * TX_TIMER_TICKS_PER_SECOND) / 1000)
#endif

#ifndef GX_TICKS_SECOND
#define GX_TICKS_SECOND           (1000 / GX_SYSTEM_TIMER_MS)
#endif

#endif /* GX_THREADX_BINDING */

#ifndef GX_DISABLE_MULTITHREAD_SUPPORT
#define GX_MULTITHREAD_SUPPORT
#endif

#ifndef GX_DISABLE_UTF8_SUPPORT
#define GX_UTF8_SUPPORT
#endif

#ifndef GX_DISABLE_ARC_DRAWING_SUPPORT
#define GX_ARC_DRAWING_SUPPORT
#endif

#ifndef GX_DISABLE_SOFTWARE_DECODER_SUPPORT
#define GX_SOFTWARE_DECODER_SUPPORT
#endif

#ifndef GX_DISABLE_BINARY_RESOURCE_SUPPORT
#define GX_BINARY_RESOURCE_SUPPORT
#endif

#ifndef GX_DISABLE_BRUSH_ALPHA_SUPPORT
#define GX_BRUSH_ALPHA_SUPPORT
#endif

#ifndef GX_DISABLE_EXPO_ELASTIC_EASING_SUPPORT
#define GX_EXPO_ELASTIC_EASING_SUPPORT
#endif

#ifndef GX_REPEAT_BUTTON_INITIAL_TICS
#define GX_REPEAT_BUTTON_INITIAL_TICS       10
#endif

#ifndef GX_REPEAT_BUTTON_REPEAT_TICS
#define GX_REPEAT_BUTTON_REPEAT_TICS        3
#endif

/* define size of the GUIX event queue */
#ifndef GX_MAX_QUEUE_EVENTS
#define GX_MAX_QUEUE_EVENTS                 48
#endif

#ifndef GX_MAX_DIRTY_AREAS
#define GX_MAX_DIRTY_AREAS                  64
#endif

/* define depth of drawing context stack */
#ifndef GX_MAX_CONTEXT_NESTING
#define GX_MAX_CONTEXT_NESTING              8
#endif

/* define depth of input capture stack */

#ifndef GX_MAX_INPUT_CAPTURE_NESTING
#define GX_MAX_INPUT_CAPTURE_NESTING        4
#endif

#ifndef GX_SYSTEM_THREAD_PRIORITY
#define GX_SYSTEM_THREAD_PRIORITY           16
#endif

#ifndef GX_SYSTEM_THREAD_TIMESLICE
#define GX_SYSTEM_THREAD_TIMESLICE          10
#endif

#ifndef GX_CURSOR_BLINK_INTERVAL
#define GX_CURSOR_BLINK_INTERVAL            20
#endif

#ifndef GX_MULTI_LINE_INDEX_CACHE_SIZE
#define GX_MULTI_LINE_INDEX_CACHE_SIZE      32
#endif

#ifndef GX_MULTI_LINE_TEXT_BUTTON_MAX_LINES
#define GX_MULTI_LINE_TEXT_BUTTON_MAX_LINES 4
#endif

/* Define rich text context stack size. */
#ifndef GX_MAX_RICH_TEXT_CONTEXT_NESTING
#define GX_MAX_RICH_TEXT_CONTEXT_NESTING    10
#endif

/* maximum number of polygon edges */
#ifndef GX_POLYGON_MAX_EDGE_NUM
#define GX_POLYGON_MAX_EDGE_NUM             10
#endif

#ifndef GX_NUMERIC_SCROLL_WHEEL_STRING_BUFFER_SIZE
#define GX_NUMERIC_SCROLL_WHEEL_STRING_BUFFER_SIZE 16
#endif

#ifndef GX_DEFAULT_CIRCULAR_GAUGE_ANIMATION_DELAY
#define GX_DEFAULT_CIRCULAR_GAUGE_ANIMATION_DELAY 5
#endif

/* define the largest string needed by a NUMERIC prompt widget */
#ifndef GX_NUMERIC_PROMPT_BUFFER_SIZE
#define GX_NUMERIC_PROMPT_BUFFER_SIZE        16
#endif

/* define size of the GX_ANIMATION pool, i.e. max sumultaneous animations */
#ifndef GX_ANIMATION_POOL_SIZE
#define GX_ANIMATION_POOL_SIZE 6
#endif

/* define maximum string length. */
#ifndef GX_MAX_STRING_LENGTH
#define GX_MAX_STRING_LENGTH 102400
#endif

#ifndef GX_PALETTE_MODE_AA_TEXT_COLORS
#define GX_PALETTE_MODE_AA_TEXT_COLORS       8
#endif

#if defined(GX_MOUSE_SUPPORT)
#if !defined(GX_MOUSE_MAX_RESOLUTION)
#define GX_MOUSE_MAX_RESOLUTION 32
#endif
#endif

/* defines for backwards compatiblity */
#ifndef GX_RENESAS_DAVE2D_DRAW
#ifdef  GX_USE_SYNERGY_DRW
#define GX_RENESAS_DAVE2D_DRAW
#endif
#endif

#ifndef GX_RENESAS_DAVE2D_FONT_SUPPORT
#ifdef  GX_SYNERGY_FONT_FORMAT_SUPPORT
#define GX_RENESAS_DAVE2D_FONT_SUPPORT
#endif
#endif
/* end of defines for backwards compatiblity */

#define GX_MAX_PIXELMAP_RESOLUTION 0x3FFF

#if defined(GX_UTF8_SUPPORT)
#if defined(GX_EXTENDED_UNICODE_SUPPORT)
/* support 32 bit character codes */
typedef ULONG GX_CHAR_CODE;
#define GX_MAX_GLYPH_CODE 0x10ffff
#else
/* support 16 bit character codes */
typedef USHORT GX_CHAR_CODE;
#define GX_MAX_GLYPH_CODE 0xffff
#endif
#else
/* support 8 bit character codes */
typedef GX_UBYTE GX_CHAR_CODE;
#define GX_MAX_GLYPH_CODE 0xff
#endif

#include "gx_consts.h"
#include "gx_macros.h"

#endif
