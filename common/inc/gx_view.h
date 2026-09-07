#ifndef GX_VIEW_H
#define GX_VIEW_H

#include "gx_rectangle.h"

/* Define the View type.  */
struct GX_VIEW
{
    GX_RECTANGLE    rectangle;
    GX_VIEW*        next;

    bool contain_rectangle_(const GX_RECTANGLE& rect) const
    {
        return rectangle.contain_rectangle_(rect);
    }
};

#endif
