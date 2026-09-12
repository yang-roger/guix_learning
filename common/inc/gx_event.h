#ifndef GX_EVENT_H
#define GX_EVENT_H

#include "gx_point.h"

struct GX_WIDGET;

/* Define Event type.  */
struct GX_EVENT
{
    ULONG      type;                                       /* Global event type                        */
    ULONG      display_handle;
    GX_WIDGET* target;                                     /* receiver of event                        */
    USHORT     sender;                                     /* ID of the event sender                   */
    union
    {
        UINT     timer_id;
        GX_POINT pointdata;
        GX_UBYTE uchardata[4];
        USHORT   ushortdata[2];
        ULONG    ulongdata;
        GX_BYTE  chardata[4];
        SHORT    shortdata[2];
        INT      intdata[2];
        LONG     longdata;
    } payload;
};

#define GX_EVENT_SIZE   sizeof(GX_EVENT)
#define GX_EVENT_ULONGS ((GX_EVENT_SIZE + 3) / sizeof(ULONG))

GX_COMPILE_ASSERT(GX_EVENT_SIZE >= sizeof(GX_EVENT));

#endif
