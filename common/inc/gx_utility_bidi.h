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
/**   Utility (BIDI)                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef GX_UTILITY_BIDI_H
#define GX_UTILITY_BIDI_H

#include "gx_global.h"

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)

#include "gx_bidi_text_info.h"

/* Define bidi character types. */
#define GX_BIDI_CHARACTER_TYPE_L     0x01 /* Left to Right */
#define GX_BIDI_CHARACTER_TYPE_R     0x02 /* Right to Left */
#define GX_BIDI_CHARACTER_TYPE_AL    0x03 /* Right to Left Arabic */
#define GX_BIDI_CHARACTER_TYPE_EN    0x04 /* European Number */
#define GX_BIDI_CHARACTER_TYPE_ES    0x05 /* European Number Separator */
#define GX_BIDI_CHARACTER_TYPE_ET    0x06 /* European Number Terminator */
#define GX_BIDI_CHARACTER_TYPE_AN    0x07 /* Arabic Number */
#define GX_BIDI_CHARACTER_TYPE_CS    0x08 /* Common Number Separator */
#define GX_BIDI_CHARACTER_TYPE_NSM   0x09 /* Nonspacing Mark */
#define GX_BIDI_CHARACTER_TYPE_BN    0x0A /* Boundary Neutral */
#define GX_BIDI_CHARACTER_TYPE_B     0x0B /* Paragraph Separator */
#define GX_BIDI_CHARACTER_TYPE_S     0x0C /* Segment Separator */
#define GX_BIDI_CHARACTER_TYPE_WS    0x0D /* Whitespace */
#define GX_BIDI_CHARACTER_TYPE_ON    0x0E /* Other Neutrals */
#define GX_BIDI_CHARACTER_TYPE_LRE   0x0F
#define GX_BIDI_CHARACTER_TYPE_LRO   0x10
#define GX_BIDI_CHARACTER_TYPE_RLE   0x11
#define GX_BIDI_CHARACTER_TYPE_RLO   0x12
#define GX_BIDI_CHARACTER_TYPE_PDF   0x13
#define GX_BIDI_CHARACTER_TYPE_LRI   0x14
#define GX_BIDI_CHARACTER_TYPE_RLI   0x15
#define GX_BIDI_CHARACTER_TYPE_FSI   0x16
#define GX_BIDI_CHARACTER_TYPE_PDI   0x17

#define GX_BIDI_CATEGORY_STRONG      0x01
#define GX_BIDI_CATEGORY_WEAK        0x02
#define GX_BIDI_CATEGORY_NEUTRAL     0x03

#define GX_BIDI_DIRECTION_FORMAT_LRE 0x202A /* Left to Right Embedding */
#define GX_BIDI_DIRECTION_FORMAT_LRO 0x202D /* Left to Right Override */
#define GX_BIDI_DIRECTION_FORMAT_RLE 0x202B /* Right to Left Embedding */
#define GX_BIDI_DIRECTION_FORMAT_RLO 0x202E /* Right to Left Override */
#define GX_BIDI_DIRECTION_FORMAT_PDF 0x202C /* Pop Directional Format */
#define GX_BIDI_DIRECTION_FORMAT_LRI 0x2066 /* Left to Right Isolate */
#define GX_BIDI_DIRECTION_FORMAT_RLI 0x2067 /* Right to Left Isolate */
#define GX_BIDI_DIRECTION_FORMAT_FSI 0x2068 /* First Strong Isolate */
#define GX_BIDI_DIRECTION_FORMAT_PDI 0x2069 /* Pop Directional Isolate */


#define GX_BIDI_OVERRIDE_STATUS_N    0x01 /* No override is currently active */
#define GX_BIDI_OVERRIDE_STATUS_R    0x02 /* Characters are to be reset to R */
#define GX_BIDI_OVERRIDE_STATUS_L    0x03 /* Characters are to be reset to L */

#define GX_BIDI_MAX_EXPLICIT_DEPTH   125
#define GX_BIDI_MAX_BRACKET_DEPTH    63

/* Define explicit entry structure. */
struct GX_BIDI_EXPLICIT_ENTRY
{
    GX_UBYTE level;
    GX_BOOL  override_status;
    GX_BOOL  isolate_status;
};

/* Define level run information structure. */
struct GX_BIDI_LEVEL_RUN
{
    INT                start_index;
    INT                end_index;
    GX_UBYTE           level;
    GX_BIDI_LEVEL_RUN *next;
};

/* Define isolate run sequence information structure. */
struct GX_BIDI_ISOLATE_RUN
{
    INT                 *index_list;
    INT                  index_count;
    GX_UBYTE             sos;
    GX_UBYTE             eos;
    GX_BIDI_ISOLATE_RUN *next;
};

/* Define unicode information structure. */
struct GX_BIDI_UNIT
{
    ULONG    code;
    GX_UBYTE level;
    GX_UBYTE type;
    GX_UBYTE org_type;
};

/* Define a truture to keep parameters for a bunch of functions. */
struct GX_BIDI_CONTEXT
{
    GX_BIDI_TEXT_INFO   *input_info;
    UINT                 processced_size;
    UINT                 total_lines;
    GX_BIDI_UNIT        *unit_list;
    INT                  unit_count;
    INT                 *line_index_cache;
    GX_BIDI_LEVEL_RUN   *level_runs;
    GX_BIDI_ISOLATE_RUN *isolate_runs;
    GX_UBYTE            *buffer;
    UINT                 buffer_size;
    UINT                 buffer_index;
    UINT                 bracket_pair_size;
    GX_UBYTE             base_level;
    ULONG                reordered_utf8_size;
};

/* Define bidirectional character infomation structure. */
struct GX_BIDI_CHARACTER_INFO
{
    ULONG    range_start;
    ULONG    range_end;
    GX_UBYTE type;
};

struct GX_BIDI_BRACKET_PAIR
{
    ULONG open;
    ULONG close;
};

#if defined(GX_DYNAMIC_ARABIC_SHAPING_SUPPORT)
UINT _gx_utility_bidi_arabic_shaping(GX_BIDI_CONTEXT *context);
#endif
UINT _gx_utility_bidi_bracket_pair_get(ULONG code, GX_BIDI_BRACKET_PAIR *bracket_pair);
UINT _gx_utility_bidi_character_type_get(ULONG code, GX_UBYTE *type);
UINT _gx_utility_bidi_mirroring_get(USHORT code, USHORT *mirror);
UINT _gx_utility_bidi_paragraph_reorder(GX_BIDI_TEXT_INFO *input_info, GX_BIDI_RESOLVED_TEXT_INFO **resolved_info_head);
UINT _gx_utility_bidi_paragraph_reorder_ext(GX_BIDI_TEXT_INFO *input_info, GX_BIDI_RESOLVED_TEXT_INFO **resolved_info_head);
UINT _gx_utility_bidi_resolved_text_info_delete(GX_BIDI_RESOLVED_TEXT_INFO **resolved_info_head);

#endif // GX_DYNAMIC_BIDI_TEXT_SUPPORT

#endif
