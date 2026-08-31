#ifndef GX_THEME_H
#define GX_THEME_H

#include "gx_global.h"
#include "gx_scroll_bar_appearance.h"

struct GX_FONT;
struct GX_PIXELMAP;

/* Define Theme type */
struct GX_THEME
{
    GX_COLOR*               color_table;
    GX_FONT**               font_table;
    GX_PIXELMAP**           pixelmap_table;
    GX_COLOR*               palette;
    GX_SCROLLBAR_APPEARANCE vertical_scrollbar_appearance;
    GX_SCROLLBAR_APPEARANCE horizontal_scrollbar_appearance;
    ULONG                   vertical_scroll_style;
    ULONG                   horizontal_scroll_style;

    USHORT                  color_table_size;
    USHORT                  font_table_size;
    USHORT                  pixelmap_table_size;
    USHORT                  palette_size;
};

#endif
