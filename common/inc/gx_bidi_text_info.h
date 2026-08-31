#ifndef GX_BIDI_TEXT_INFO_H
#define GX_BIDI_TEXT_INFO_H

#include "gx_global.h"

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)

struct GX_FONT;

struct GX_BIDI_TEXT_INFO
{
    GX_STRING text;
    GX_FONT  *font;
    GX_VALUE  display_width;
    GX_BYTE   direction;
};

struct GX_BIDI_RESOLVED_TEXT_INFO
{
    GX_STRING                  *text;
    UINT                        total_lines;
    GX_BIDI_RESOLVED_TEXT_INFO *next;
};

#endif /* GX_DYNAMIC_BIDI_TEXT_SUPPORT */

#endif
