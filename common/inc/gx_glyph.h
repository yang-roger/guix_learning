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

#ifndef GX_GLYPH_H
#define GX_GLYPH_H

#include "gx_global.h"

/* Define macro for GX_GLYPH_STRUCT members. */
#define GX_GLYPH_MEMBERS_DECLARE                                                           \
    const GX_UBYTE*    map;          /* Pointer to glyph data                           */ \
    SHORT              ascent;       /* Glyph ascent above baseline                     */ \
    SHORT              descent;      /* Glyph descent below baseline                    */ \
    GX_UBYTE           advance;      /* Glyph advance                                   */ \
    GX_BYTE            leading;      /* Pen x-pre-advance                               */ \
    GX_UBYTE           width;                                                              \
    GX_UBYTE           height;

/* Define the Glyph type.  */
struct GX_GLYPH
{
    GX_GLYPH_MEMBERS_DECLARE
};

#if defined (GX_FONT_KERNING_SUPPORT)
/* Define the Kerning Glyph type.  */
typedef struct GX_KERNING_GLYPH_STRUCT
{
    GX_GLYPH_MEMBERS_DECLARE
    GX_CONST GX_UBYTE* kerning_table;                   /* Kerning table */
} GX_KERNING_GLYPH;
#endif

/* Define the Compressed Glyph type.  */
struct GX_COMPRESSED_GLYPH
{
    GX_GLYPH_MEMBERS_DECLARE
    USHORT             map_size;                       /* Glyph size */
};

#endif
