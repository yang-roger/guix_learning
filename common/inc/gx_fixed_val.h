#ifndef GX_FIXED_VAL_H
#define GX_FIXED_VAL_H

#include "gx_global.h"

/* GUIX fixed point math helpers */
struct GX_FIXED_POINT
{
    GX_FIXED_VAL x;
    GX_FIXED_VAL y;
};

#define GX_FIXED_VAL_SHIFT         10
#define GX_FIXED_VAL_HALF          512
#define GX_FIXED_VAL_ONE           1024
#define GX_FIXED_VAL_FRACTION_MASK 0x3ff

#define GX_FIXED_VAL_MAKE(_a)      (((int)_a) << GX_FIXED_VAL_SHIFT)
#define GX_FIXED_VAL_TO_INT(_a)    ((int)(((int)_a) >> GX_FIXED_VAL_SHIFT))
#define GX_FIXED_VAL_MUL(_a, _b)   ((((int)_a) * (_b)) >> GX_FIXED_VAL_SHIFT)
#define GX_FIXED_VAL_DIV(_a, _b)   ((((int)_a) << GX_FIXED_VAL_SHIFT) / ((int)_b))
#define GX_FIXED_VAL_RND(_a)       (GX_VALUE)((((GX_FIXED_VAL)_a) + GX_FIXED_VAL_HALF) >> GX_FIXED_VAL_SHIFT)
#define GX_FIXED_VAL_RND_UP(_a)    ((((GX_FIXED_VAL)_a) + GX_FIXED_VAL_ONE - 1) >> GX_FIXED_VAL_SHIFT)
#define GX_FIXED_VAL_ADD_ONE(_a)   (((int)_a) + GX_FIXED_VAL_ONE)
#define GX_FIXED_VAL_SUB_ONE(_a)   (((int)_a) - GX_FIXED_VAL_ONE)

#endif
