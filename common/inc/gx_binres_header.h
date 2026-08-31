#ifndef GX_BINRES_HEADER_H
#define GX_BINRES_HEADER_H

#include "gx_global.h"
#include "gx_scroll_bar_appearance.h"

#define GX_LANGUAGE_HEADER_NAME_SIZE 64

/* Degine binary resource header type. */
struct GX_RESOURCE_HEADER
{
    USHORT magic_number;
    USHORT version;
    USHORT theme_count;
    USHORT language_count;
    ULONG  theme_data_size;
    ULONG  string_data_size;
    ULONG  data_size;
};

/* Define binary theme header type. */
struct GX_THEME_HEADER
{
    USHORT                  magic_number;
    USHORT                  index;
    USHORT                  color_count;
    USHORT                  palette_count;
    USHORT                  font_count;
    USHORT                  pixelmap_count;
    GX_SCROLLBAR_APPEARANCE vscroll_appearance;
    GX_SCROLLBAR_APPEARANCE hscroll_appearance;
    ULONG                   vscroll_style;
    ULONG                   hscroll_style;
    ULONG                   color_data_size;
    ULONG                   palette_data_size;
    ULONG                   font_data_size;
    ULONG                   pixelmap_data_size;
    ULONG                   data_size;
};

/* Define binary color header type. */
struct GX_COLOR_HEADER
{
    USHORT magic_number;
    USHORT color_count;
    ULONG  data_size;
};

/* Define binary palette header type.  */
struct GX_PALETTE_HEADER
{
    USHORT magic_number;
    USHORT color_count;
    ULONG  data_size;
};

/* Define font header type. */
struct GX_FONT_HEADER
{
    USHORT    magic_number;
    USHORT    index;
    USHORT    page_count;
    GX_UBYTE  deault;
    GX_UBYTE  bits;
    ULONG     data_size;
    ULONG     data_offset;
};

/* Define font page header type. */
struct GX_PAGE_HEADER
{
    USHORT        magic_number;
    USHORT        index;
    GX_UBYTE      format;
    GX_UBYTE      prespace;
    GX_UBYTE      postspace;
    GX_UBYTE      line_height;
    GX_UBYTE      baseline;
    GX_CHAR_CODE  first_glyph;
    GX_CHAR_CODE  last_glyph;
    ULONG         data_size;
};

/* define glyph header type. */
/* Used for Guix berfore Kerning. */
struct GX_GLYPH_HEADER
{
    USHORT    magic_number;
    USHORT    map_size;
    ULONG     map_offset;
    USHORT    index;
    SHORT     ascent;
    SHORT     descent;
    GX_UBYTE  advance;
    GX_BYTE   leading;
    GX_UBYTE  width;
    GX_UBYTE  height;
    ULONG     data_size;
};

/* Used for Guix after version 5.3.4(include). */
struct GX_KERNING_GLYPH_HEADER : public GX_GLYPH_HEADER
{
    USHORT    kerning_table_size;
};

/* Define pixelmap header type. */
struct GX_PIXELMAP_HEADER
{
    USHORT   magic_number;
    USHORT   index;
    GX_UBYTE version_major;
    GX_UBYTE version_minor;
    GX_UBYTE flags;
    GX_UBYTE format;
    ULONG    map_size;
    ULONG    aux_data_size;
    GX_COLOR transparent_color;
    USHORT   width;
    USHORT   height;
    ULONG    data_size;
    ULONG    data_offset;
};

/* Define string header type. */
struct GX_STRING_HEADER
{
    USHORT magic_number;
    USHORT language_count;
    USHORT string_count;
    ULONG  data_size;
};

/* Define language header type. */
struct GX_LANGUAGE_HEADER
{
    USHORT magic_number;
    USHORT index;
    UCHAR  name[GX_LANGUAGE_HEADER_NAME_SIZE];
    ULONG  data_size;
};

#endif
