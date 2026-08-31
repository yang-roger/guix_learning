#ifndef GX_RICH_TEXT_INFO_H
#define GX_RICH_TEXT_INFO_H

#include "gx_global.h"

/* Define rich tet view fonts structure. */
struct GX_RICH_TEXT_FONTS
{
    GX_RESOURCE_ID normal_id;
    GX_RESOURCE_ID bold_id;
    GX_RESOURCE_ID italic_id;
    GX_RESOURCE_ID bold_italic_id;
};

/* Define rich text flags. */
#define GX_RICH_TEXT_BOLD         0x01
#define GX_RICH_TEXT_ITALIC       0x02
#define GX_RICH_TEXT_UNDERLINE    0x04
#define GX_RICH_TEXT_LEFT         0x00
#define GX_RICH_TEXT_CENTER       0x10
#define GX_RICH_TEXT_RIGHT        0x20
#define GX_RICH_TEXT_ALIGN_MASK   0x30

/* Define rich text view draw style structure. */
struct GX_RICH_TEXT_FORMAT
{
    GX_RESOURCE_ID color;
    GX_RESOURCE_ID highlight_color;
    GX_RESOURCE_ID font_id;
    GX_UBYTE       flags;
};

/* Define rich text context structure. */
struct GX_RICH_TEXT_CONTEXT
{
    GX_RICH_TEXT_FORMAT format;
    const GX_STRING*    tag;
};

/* Define rich text context stack structure. */
struct GX_RICH_TEXT_CONTEXT_STACK
{
    GX_RICH_TEXT_CONTEXT context[GX_MAX_RICH_TEXT_CONTEXT_NESTING];
    GX_UBYTE             top;
};

#endif
