#ifndef GX_SCROLL_INFO_H
#define GX_SCROLL_INFO_H

#include "gx_global.h"

/* Define Scroll information structure. This structure is used to define the position
   and scaling of the scroll bar thumb button.  */
struct GX_SCROLL_INFO
{
    INT      value;
    INT      minimum;
    INT      maximum;
    GX_VALUE visible;
    GX_VALUE increment;
};

#endif
