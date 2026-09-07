#ifndef GX_GRADIENT_H
#define GX_GRADIENT_H

#include "gx_global.h"
#include "gx_pixelmap.h"

/* GX_GRADIENT structure. This structure is used to support
   runtime gradient pixelmap creation  */
struct GX_GRADIENT
{
    GX_GRADIENT*  next;
    GX_GRADIENT*  previous;
    UCHAR         type;
    UCHAR         alpha_start;
    UCHAR         alpha_end;
    GX_FIXED_VAL  alpha_step;
    GX_FIXED_VAL  alpha_current;
    GX_PIXELMAP   pixelmap;
};

#endif
