#include "gx_point.h"

#include "gx_utility_math.h"

INT gx_point_distance_(GX_VALUE point1_x, GX_VALUE point1_y,
                       GX_VALUE point2_x, GX_VALUE point2_y)
{
    INT x_dist = (INT)(point1_x - point2_x);
    INT y_dist = (INT)(point1_y - point2_y);

    return (INT)_gx_utility_math_sqrt((UINT)(x_dist * x_dist + y_dist * y_dist));
}

