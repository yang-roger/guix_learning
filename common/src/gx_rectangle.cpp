#include "gx_rectangle.h"

void GX_RECTANGLE::combine_(const GX_RECTANGLE& other)
{
    if (other.left < left)
    {
        left = other.left;
    }

    if (other.top < top)
    {
        top = other.top;
    }

    if (other.right > right)
    {
        right = other.right;
    }

    if (other.bottom > bottom)
    {
        bottom = other.bottom;
    }
}

void GX_RECTANGLE::center_(const GX_RECTANGLE& within)
{
    INT x_space = (within.width_() - width_()) / 2;
    INT y_space = (within.height_() - height_()) / 2;

    INT dx = within.left + x_space - left;
    INT dy = within.top + y_space - top;

    move_(static_cast<GX_VALUE>(dx), static_cast<GX_VALUE>(dy));
}

bool operator==(const GX_RECTANGLE& first, const GX_RECTANGLE& second)
{
    return first.left == second.left &&
           first.top == second.top &&
           first.right == second.right &&
           first.bottom == second.bottom;
}

bool gx_rectangle_intersect_(const GX_RECTANGLE& first, const GX_RECTANGLE& second, GX_RECTANGLE* intersected)
{
    GX_RECTANGLE rect;

    rect.left   = GX_MAX(first.left, second.left);
    rect.top    = GX_MAX(first.top, second.top);
    rect.right  = GX_MIN(first.right, second.right);
    rect.bottom = GX_MIN(first.bottom, second.bottom);

    if (intersected)
    {
        *intersected = rect;
    }

    return rect.is_valid_();
}
