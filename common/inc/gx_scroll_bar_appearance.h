#ifndef GX_SCROLLBAR_APPEARANCE_H
#define GX_SCROLLBAR_APPEARANCE_H

#include "gx_global.h"

/* Define Scroll Appearance structure. This structure is used to define the
   appearance of a scroll bar.  */
struct GX_SCROLLBAR_APPEARANCE
{
    GX_VALUE       width;
    GX_VALUE       thumb_width;
    GX_VALUE       thumb_travel_min;
    GX_VALUE       thumb_travel_max;
    GX_UBYTE       thumb_border_style;
    GX_RESOURCE_ID fill_pixelmap;
    GX_RESOURCE_ID thumb_pixelmap;
    GX_RESOURCE_ID up_pixelmap;
    GX_RESOURCE_ID down_pixelmap;
    GX_RESOURCE_ID thumb_color;
    GX_RESOURCE_ID thumb_border_color;
    GX_RESOURCE_ID button_color;
};

#endif
