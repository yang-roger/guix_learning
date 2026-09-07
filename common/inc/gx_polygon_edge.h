#ifndef GX_POLYGON_EDGE_H
#define GX_POLYGON_EDGE_H

#include "gx_global.h"

/* Define the edge type. */
struct GX_POLYGON_EDGE
{
    INT xi;
    INT dx;
    INT dy;
    INT sign;
    INT x_decision;
    INT ymax;
    INT ymin;
};

#endif
