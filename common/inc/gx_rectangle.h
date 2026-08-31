#ifndef GX_RECTANGLE_H
#define GX_RECTANGLE_H

#include "gx_point.h"

/* Define the Rectangle type.  */
struct GX_RECTANGLE
{
    GX_VALUE left;
    GX_VALUE top;
    GX_VALUE right;
    GX_VALUE bottom;

    void set_(GX_VALUE left, GX_VALUE top, GX_VALUE right, GX_VALUE bottom)
    {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }

    GX_VALUE width_() const
    {
        return right - left + 1;
    }

    GX_VALUE height_() const
    {
        return bottom - top + 1;
    }

    bool is_empty_() const
    {
        return left > right || top > bottom;
    }

    bool is_valid_() const
    {
        return left <= right && top <= bottom;
    }

    bool contain_point_(GX_VALUE x, GX_VALUE y) const
    {
        return (left <= x && x <= right) &&
               (top <= y && y <= bottom);
    }

    bool contain_point_(const GX_POINT& point) const
    {
        return contain_point_(point.x, point.y);
    }

    bool contain_rectangle_(const GX_RECTANGLE& rect) const
    {
        return contain_point_(rect.left, rect.top) &&
               contain_point_(rect.right, rect.bottom);
    }

    // get the bounding rectangle of this rectangle and the other rectangle
    void combine_(const GX_RECTANGLE& other);

    void adjust_(GX_VALUE d_left, GX_VALUE d_top, GX_VALUE d_right, GX_VALUE d_bottom)
    {
        left   = static_cast<GX_VALUE>(left + d_left);
        top    = static_cast<GX_VALUE>(top + d_top);
        right  = static_cast<GX_VALUE>(right + d_right);
        bottom = static_cast<GX_VALUE>(bottom + d_bottom);
    }

    void resize_(GX_VALUE delta)
    {
        if (delta != 0)
        {
            adjust_(-delta, -delta, delta, delta);
        }
    }

    void move_(GX_VALUE dx, GX_VALUE dy)
    {
        adjust_(dx, dy, dx, dy);
    }

    // center this rectangle within the |within| rectangle
    void center_(const GX_RECTANGLE& within);

    GX_VALUE x_center_() const { return static_cast<GX_VALUE>((left + right) / 2); }
    GX_VALUE y_center_() const { return static_cast<GX_VALUE>((top + bottom) / 2); }

    void center_find_(GX_POINT* return_center) const
    {
        return_center->x = x_center_();
        return_center->y = y_center_();
    }
};

bool operator==(const GX_RECTANGLE& first, const GX_RECTANGLE& second);

inline bool operator!=(const GX_RECTANGLE& first, const GX_RECTANGLE& second)
{
    return !(first == second);
}

bool gx_rectangle_intersect_(const GX_RECTANGLE& first, const GX_RECTANGLE& second, GX_RECTANGLE* intersected = GX_NULL);

#endif
