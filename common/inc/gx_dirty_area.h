#ifndef GX_DIRTY_AREA_H
#define GX_DIRTY_AREA_H

#include "gx_rectangle.h"

struct GX_WIDGET;

/* Define Dirty Area type.  */
struct GX_DIRTY_AREA
{
    GX_RECTANGLE    rectangle;
    GX_WIDGET*      widget;
};

#endif
