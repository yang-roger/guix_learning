/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** GUIX Component                                                        */
/**                                                                       */
/**   Utility (Math)                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef GX_UTILITY_MATH_H
#define GX_UTILITY_MATH_H

#include "gx_global.h"
#include "gx_fixed_val.h"

#define GX_UTILITY_MATH_UINT_ADD(augend, addend, result) \
    if ((UINT)-1 - (augend) < (addend))                  \
    {                                                    \
        return GX_MATH_OVERFLOW;                         \
    }                                                    \
    else                                                 \
    {                                                    \
        (result) = (augend) + (addend);                  \
    }

#define GX_UTILITY_MATH_UINT_MULT(multiplicand, multiplier, result) \
    if ((UINT)-1 / (multiplier) < (multiplicand))                   \
    {                                                               \
        return GX_MATH_OVERFLOW;                                    \
    }                                                               \
    else                                                            \
    {                                                               \
        (result) = (multiplicand) * (multiplier);                   \
    }

#define GX_UTILITY_MATH_USHORT_ADD(augend, addend, result) \
    if ((USHORT)-1 - (augend) < (addend))                  \
    {                                                      \
        return GX_MATH_OVERFLOW;                           \
    }                                                      \
    else                                                   \
    {                                                      \
        (result) = (USHORT)((augend) + (addend));          \
    }

#define GX_UTILITY_MATH_USHORT_MULT(multiplicand, multiplier, result) \
    if ((USHORT)-1 / (multiplier) < (multiplicand))                   \
    {                                                                 \
        return GX_MATH_OVERFLOW;                                      \
    }                                                                 \
    else                                                              \
    {                                                                 \
        result = (USHORT)((multiplicand) * (multiplier));             \
    }

#define GX_UTILITY_USHORT_CHECK(a)\
    if((a) > (USHORT)-1)          \
    {                             \
        return GX_MATH_OVERFLOW;  \
    }

INT          _gx_utility_math_acos(GX_FIXED_VAL x);
INT          _gx_utility_math_asin(GX_FIXED_VAL x);
GX_FIXED_VAL _gx_utility_math_cos(GX_FIXED_VAL angle);
GX_FIXED_VAL _gx_utility_math_sin(GX_FIXED_VAL angle);
UINT         _gx_utility_math_sqrt(UINT n);

#endif
