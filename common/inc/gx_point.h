#ifndef GX_POINT_H
#define GX_POINT_H

#include "gx_global.h"

/* Define the Point type.  */
struct GX_POINT
{
    GX_VALUE x;
    GX_VALUE y;
};

INT gx_point_distance_(GX_VALUE point1_x, GX_VALUE point1_y,
                       GX_VALUE point2_x, GX_VALUE point2_y);

inline INT gx_point_distance_(const GX_POINT& point1, const GX_POINT& point2)
{
    return gx_point_distance_(point1.x, point1.y, point2.x, point2.y);
}

#endif
