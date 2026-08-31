#ifndef GX_TEXT_AREA_INFO_H
#define GX_TEXT_AREA_INFO_H

#include "gx_rectangle.h"

static inline GX_VALUE gx_text_area_left_(const GX_RECTANGLE& client)
{
    return (GX_VALUE)(client.left + 1);
}

static inline GX_VALUE gx_text_area_right_(const GX_RECTANGLE& client)
{
    return (GX_VALUE)(client.right - 1);
}

static inline GX_VALUE gx_text_area_width_(const GX_RECTANGLE& client)
{
    return (GX_VALUE)(client.width_() - 3);
}

#endif
