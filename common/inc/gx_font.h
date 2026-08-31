#ifndef GX_FONT_H
#define GX_FONT_H

#include "gx_glyph.h"

/* Define the Font type.  */
struct GX_FONT
{
    GX_UBYTE                     format;                /* Format flags, defined below              */
    GX_UBYTE                     prespace;              /* Line spacing above, pixels               */
    GX_UBYTE                     postspace;             /* Line spacing below, pixels               */
    GX_UBYTE                     line_height;           /* Font height, excluding spacing           */
    GX_UBYTE                     baseline;              /* Font baseline                            */
    GX_CHAR_CODE                 first_glyph;           /* First glyph on this page                 */
    GX_CHAR_CODE                 last_glyph;            /* Last glyph on this page                  */
    union {
        const GX_GLYPH*            normal;              /* Array of GX_GLYPH structures             */
        const GX_COMPRESSED_GLYPH* compressed;          /* Array of GX_COMPRESSED_GLYPH structures  */
    #if defined (GX_FONT_KERNING_SUPPORT)
        const GX_KERNING_GLYPH*    kerning;             /* Array of GX_KERNING_GLYPH structures     */
    #endif
    } glyphs;
    const GX_FONT*               next_page;             /* For multiple page fonts (Unicode)        */
};

#define GX_FONT_FORMAT_BPP_MASK       0x03

#define GX_FONT_FORMAT_1BPP           0x00
#define GX_FONT_FORMAT_2BPP           0x01
#define GX_FONT_FORMAT_4BPP           0x02
#define GX_FONT_FORMAT_8BPP           0x03

#define GX_FONT_FORMAT_COMPRESSED     0x10
#define GX_FONT_FORMAT_FREETYPE       0x20
#define GX_FONT_FORMAT_KERNING        0x40
#define GX_FONT_FORMAT_REVERSED_ORDER 0x80  /* For 4bpp font, half bytes order reversed. For 1bpp font, bits order reversed with least signigicant bit in left. */

#define GX_FONT_FORMAT_ROTATED_90     0x04
#define GX_FONT_FORMAT_ROTATED_270    0x08

inline const GX_FONT* gx_font_find_(const GX_FONT* font, GX_CHAR_CODE char_val)
{
    while (font)
    {
        if (char_val >= font->first_glyph &&
            char_val <= font->last_glyph)
        {
            return font;
        }

        font = font->next_page;
    }

    return GX_NULL;
}

#endif
