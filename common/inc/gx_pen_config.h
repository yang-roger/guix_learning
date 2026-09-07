#ifndef GX_PEN_CONFIG_H
#define GX_PEN_CONFIG_H

#include "gx_fixed_val.h"

/* Define pen configuration structure. */
struct GX_PEN_CONFIGURATION
{
    GX_FIXED_VAL min_drag_dist;
    UINT         max_pen_speed_ticks;
};

#endif
