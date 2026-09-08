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
/**   Binres Loader Management (Binres Loader)                            */
/**                                                                       */
/**************************************************************************/

#include "gx_binres.h"

#ifdef GX_BINARY_RESOURCE_SUPPORT

#include <string.h>

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_theme.h"
#include "gx_system_fonts.h"

static inline USHORT gx_binres_read_2bytes(const GX_UBYTE* ptr)
{
    return (USHORT)((ptr[1] << 8) | ptr[0]);
}

static inline ULONG gx_binres_read_4bytes(const GX_UBYTE* ptr)
{
    return (ULONG)((ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0]);
}

struct GX_BINRES_DATA_INFO
{
public:
    UINT language_count_get(const GX_UBYTE* root_address, GX_VALUE* put_count);
    UINT language_info_load(const GX_UBYTE* root_address, GX_LANGUAGE_HEADER* put_info);
    UINT language_table_load_ext(const GX_UBYTE* root_address, GX_STRING*** returned_language_table);

    UINT theme_load(const GX_UBYTE* root_address, INT theme_index, GX_THEME** returned_theme);

    // standalone font and pixelmap
    UINT font_load(const GX_UBYTE* root_address, UINT font_index, GX_UBYTE* buffer, ULONG* buffer_size);
    UINT pixelmap_load(const GX_UBYTE* root_address, UINT map_index, GX_PIXELMAP* pixelmap);

private:
    UINT initialize(const GX_UBYTE* root_address);
    void resource_header_seek();

    UINT string_header_seek(GX_STRING_HEADER* header);
    UINT language_table_buffer_allocate(GX_UBYTE language_table_type_size, GX_UBYTE string_table_type_size);

    UINT theme_seek(INT theme_index, GX_THEME_HEADER* header);
    UINT theme_buffer_allocate(INT theme_index);
    UINT font_buffer_size_get(UINT* buffer_size, bool restore_read_offset);

    UINT color_table_load(USHORT color_count, GX_COLOR** returned_color_table);
    UINT palette_load(USHORT color_count, GX_COLOR** returned_palette);

    UINT glyphs_address_get(USHORT glyph_count, const GX_GLYPH** returned_glyphs);
#ifdef GX_FONT_KERNING_SUPPORT
    UINT kerning_glyphs_address_get(USHORT glyph_count, const GX_KERNING_GLYPH** returned_glyphs);
#endif
    UINT compressed_glyphs_address_get(USHORT glyph_count, const GX_COMPRESSED_GLYPH** returned_glyphs);

    UINT one_font_load(GX_FONT** return_font);
    UINT font_table_load(USHORT table_size, GX_FONT*** returned_font_table);

    UINT one_pixelmap_load(GX_PIXELMAP** returned_pixelmap, USHORT* map_id);
    UINT pixelmap_table_load(USHORT table_size, GX_PIXELMAP** pixelmap_table);

    UINT standalone_resource_seek(UINT res_index);

    void resource_header_load(GX_RESOURCE_HEADER* header);
    void theme_header_load(GX_THEME_HEADER* header);
    void scroll_bar_appearance_load(GX_SCROLLBAR_APPEARANCE* appearance);
    void color_header_load(GX_COLOR_HEADER* header);
    void palette_header_load(GX_PALETTE_HEADER* header);
    UINT font_header_load(GX_FONT_HEADER* header);
    UINT font_page_header_load(GX_PAGE_HEADER* header);
    void glyph_header_load(GX_GLYPH_HEADER* header);
#ifdef GX_FONT_KERNING_SUPPORT
    void kerning_glyph_header_load(GX_KERNING_GLYPH_HEADER* header);
#endif
    UINT pixelmap_header_load(GX_PIXELMAP_HEADER* header);
    void string_header_load(GX_STRING_HEADER* header);
    void language_header_load(GX_LANGUAGE_HEADER* header);

    const GX_UBYTE* ptr_for_read() const { return root_address + read_offset; }
    GX_UBYTE* ptr_of_buffer() const { return buffer + buffer_index; }

    GX_UBYTE read_GX_UBYTE();
    GX_BYTE read_GX_BYTE();
    USHORT read_USHORT();
    SHORT read_SHORT();
    GX_VALUE read_GX_VALUE();
    ULONG read_ULONG();

private:
    const GX_UBYTE *root_address;
    UINT read_offset;

    GX_UBYTE *buffer;
    UINT buffer_size;
    UINT buffer_index;

    GX_RESOURCE_HEADER resource_header;
};

inline GX_UBYTE GX_BINRES_DATA_INFO::read_GX_UBYTE()
{
    GX_UBYTE val = *ptr_for_read();
    read_offset += 1;

    return val;
}

inline GX_BYTE GX_BINRES_DATA_INFO::read_GX_BYTE()
{
    return (GX_BYTE)read_GX_UBYTE();
}

inline USHORT GX_BINRES_DATA_INFO::read_USHORT()
{
    USHORT val = gx_binres_read_2bytes(ptr_for_read());
    read_offset += 2;

    return val;
}

inline SHORT GX_BINRES_DATA_INFO::read_SHORT()
{
    return (SHORT)read_USHORT();
}

inline GX_VALUE GX_BINRES_DATA_INFO::read_GX_VALUE()
{
    return (GX_VALUE)read_USHORT();
}

inline ULONG GX_BINRES_DATA_INFO::read_ULONG()
{
    ULONG val = gx_binres_read_4bytes(ptr_for_read());
    read_offset += 4;

    return val;
}

void GX_BINRES_DATA_INFO::resource_header_load(GX_RESOURCE_HEADER* header)
{
    header->magic_number        = read_USHORT();
    header->version             = read_USHORT();
    header->theme_count         = read_USHORT();
    header->language_count      = read_USHORT();
    header->theme_data_size     = read_ULONG();
    header->string_data_size    = read_ULONG();
    header->data_size           = read_ULONG();
}

void GX_BINRES_DATA_INFO::theme_header_load(GX_THEME_HEADER* header)
{
    header->magic_number        = read_USHORT();
    header->index               = read_USHORT();
    header->color_count         = read_USHORT();
    header->palette_count       = read_USHORT();
    header->font_count          = read_USHORT();
    header->pixelmap_count      = read_USHORT();
    scroll_bar_appearance_load(&header->vscroll_appearance);
    scroll_bar_appearance_load(&header->hscroll_appearance);
    header->vscroll_style       = read_ULONG();
    header->hscroll_style       = read_ULONG();
    header->color_data_size     = read_ULONG();
    header->palette_data_size   = read_ULONG();
    header->font_data_size      = read_ULONG();
    header->pixelmap_data_size  = read_ULONG();
    header->data_size           = read_ULONG();
}

void GX_BINRES_DATA_INFO::scroll_bar_appearance_load(GX_SCROLLBAR_APPEARANCE* appearance)
{
    appearance->width               = read_GX_VALUE();
    appearance->thumb_width         = read_GX_VALUE();
    appearance->thumb_travel_min    = read_GX_VALUE();
    appearance->thumb_travel_max    = read_GX_VALUE();
    appearance->thumb_border_style  = read_GX_UBYTE();
    appearance->fill_pixelmap       = read_ULONG();
    appearance->thumb_pixelmap      = read_ULONG();
    appearance->up_pixelmap         = read_ULONG();
    appearance->down_pixelmap       = read_ULONG();
    appearance->thumb_color         = read_ULONG();
    appearance->thumb_border_color  = read_ULONG();
    appearance->button_color        = read_ULONG();
}

void GX_BINRES_DATA_INFO::color_header_load(GX_COLOR_HEADER* header)
{
    header->magic_number    = read_USHORT();
    header->color_count     = read_USHORT();
    header->data_size       = read_ULONG();
}

void GX_BINRES_DATA_INFO::palette_header_load(GX_PALETTE_HEADER* header)
{
    header->magic_number    = read_USHORT();
    header->color_count     = read_USHORT();
    header->data_size       = read_ULONG();
}

UINT GX_BINRES_DATA_INFO::font_header_load(GX_FONT_HEADER* header)
{
    header->magic_number = read_USHORT();
    if (header->magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    header->index       = read_USHORT();
    header->page_count  = read_USHORT();
    header->built_in    = read_GX_UBYTE();
    header->bits        = read_GX_UBYTE();
    header->data_size   = read_ULONG();
    header->data_offset = read_ULONG();

    return GX_SUCCESS;
}

UINT GX_BINRES_DATA_INFO::font_page_header_load(GX_PAGE_HEADER* header)
{
    header->magic_number = read_USHORT();
    if (header->magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    header->index       = read_USHORT();
    header->format      = read_GX_UBYTE();
    header->prespace    = read_GX_UBYTE();
    header->postspace   = read_GX_UBYTE();
    header->line_height = read_GX_UBYTE();
    header->baseline    = read_GX_UBYTE();

    // In the binary resource file, GX_CHAR_CODE is always stored as 4-byte value.
    header->first_glyph = (GX_CHAR_CODE)read_ULONG();
    header->last_glyph  = (GX_CHAR_CODE)read_ULONG();

    header->data_size   = read_ULONG();

    return GX_SUCCESS;
}

void GX_BINRES_DATA_INFO::glyph_header_load(GX_GLYPH_HEADER* header)
{
    header->magic_number    = read_USHORT();
    header->map_size        = read_USHORT();
    header->map_offset      = read_ULONG();
    header->index           = read_USHORT();
    header->ascent          = read_SHORT();
    header->descent         = read_SHORT();
    header->advance         = read_GX_UBYTE();
    header->leading         = read_GX_BYTE();
    header->width           = read_GX_UBYTE();
    header->height          = read_GX_UBYTE();
    header->data_size       = read_ULONG();
}

#ifdef GX_FONT_KERNING_SUPPORT
void GX_BINRES_DATA_INFO::kerning_glyph_header_load(GX_KERNING_GLYPH_HEADER* header)
{
    glyph_header_load((GX_GLYPH_HEADER*)header);
    header->kerning_table_size = read_USHORT();
}
#endif

UINT GX_BINRES_DATA_INFO::pixelmap_header_load(GX_PIXELMAP_HEADER* header)
{
    header->magic_number = read_USHORT();
    if (header->magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    header->index               = read_USHORT();
    header->version_major       = read_GX_UBYTE();
    header->version_minor       = read_GX_UBYTE();
    header->flags               = read_GX_UBYTE();
    header->format              = read_GX_UBYTE();
    header->map_size            = read_ULONG();
    header->aux_data_size       = read_ULONG();
    header->transparent_color   = read_ULONG();
    header->width               = read_USHORT();
    header->height              = read_USHORT();
    header->data_size           = read_ULONG();
    header->data_offset         = read_ULONG();

    return GX_SUCCESS;
}

void GX_BINRES_DATA_INFO::string_header_load(GX_STRING_HEADER* header)
{
    header->magic_number    = read_USHORT();
    header->language_count  = read_USHORT();
    header->string_count    = read_USHORT();
    header->data_size       = read_ULONG();
}

void GX_BINRES_DATA_INFO::language_header_load(GX_LANGUAGE_HEADER* header)
{
    header->magic_number    = read_USHORT();
    header->index           = read_USHORT();

    memcpy(header->name, ptr_for_read(), sizeof(header->name));
    read_offset += sizeof(header->name);

    header->data_size       = read_ULONG();
}

UINT GX_BINRES_DATA_INFO::initialize(const GX_UBYTE* root_address)
{
    memset(this, 0, sizeof(GX_BINRES_DATA_INFO));

    this->root_address = root_address;

    resource_header_seek();

    if (resource_header.magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    return GX_SUCCESS;
}

inline void GX_BINRES_DATA_INFO::resource_header_seek()
{
    read_offset = 0;
    resource_header_load(&resource_header);
}

UINT GX_BINRES_DATA_INFO::language_count_get(const GX_UBYTE* root_address, GX_VALUE* put_count)
{
    UINT result = initialize(root_address);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    GX_STRING_HEADER string_header;
    result = string_header_seek(&string_header);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    *put_count = (GX_VALUE)string_header.language_count;

    return GX_SUCCESS;
}

UINT GX_BINRES_DATA_INFO::language_info_load(const GX_UBYTE* root_address, GX_LANGUAGE_HEADER* put_info)
{
    UINT result = initialize(root_address);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    GX_STRING_HEADER string_header;
    result = string_header_seek(&string_header);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    for (INT i = 0; i < string_header.language_count; ++i)
    {
        language_header_load(put_info);

        if (put_info->magic_number != GX_MAGIC_NUMBER)
        {
            return GX_INVALID_FORMAT;
        }

        read_offset += put_info->data_size;
        ++put_info;
    }

    return GX_SUCCESS;
}

UINT GX_BINRES_DATA_INFO::language_table_load_ext(const GX_UBYTE* root_address, GX_STRING*** returned_language_table)
{
    UINT result = initialize(root_address);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    result = language_table_buffer_allocate(sizeof(GX_STRING*), sizeof(GX_STRING));
    if (result != GX_SUCCESS)
    {
        return result;
    }

    GX_STRING_HEADER string_header;

    resource_header_seek();
    string_header_seek(&string_header);

    GX_STRING* string;

    GX_STRING** language_table = (GX_STRING**)ptr_of_buffer();
    buffer_index += string_header.language_count * sizeof(GX_STRING*);

    GX_LANGUAGE_HEADER language_header;

    for (INT lang_index = 0; lang_index < string_header.language_count; ++lang_index)
    {
        language_header_load(&language_header);

        if (language_header.magic_number != GX_MAGIC_NUMBER)
        {
            _gx_system_memory_free(buffer);

            return GX_INVALID_FORMAT;
        }

        string = (GX_STRING*)ptr_of_buffer();

        language_table[lang_index] = string;
        buffer_index += string_header.string_count * sizeof(GX_STRING);

        ++string;
        for (INT string_index = 1; string_index < string_header.string_count; ++string_index)
        {
            string->length = read_USHORT();
            if (string->length)
            {
                string->ptr = (const GX_CHAR*)ptr_for_read();
            }
            else
            {
                string->ptr = GX_NULL;
            }

            read_offset += (string->length + 1);
            ++string;
        }
    }

    *returned_language_table = language_table;

    return GX_SUCCESS;
}

UINT GX_BINRES_DATA_INFO::language_table_buffer_allocate(GX_UBYTE language_table_type_size, GX_UBYTE string_table_type_size)
{
    GX_STRING_HEADER string_header;
    if (string_header_seek(&string_header) != GX_SUCCESS)
    {
        return GX_INVALID_FORMAT;
    }

    USHORT language_count = string_header.language_count;
    if (language_count == 0)
    {
        return GX_INVALID_FORMAT;
    }

    USHORT string_count = string_header.string_count;

    UINT string_table_size = (UINT)(string_count * string_table_type_size);
    GX_UTILITY_MATH_UINT_MULT(string_table_size, language_count, string_table_size);

    UINT language_table_size = (UINT)(language_count * language_table_type_size);
    GX_UTILITY_MATH_UINT_ADD(string_table_size, language_table_size, buffer_size);

    buffer = (GX_UBYTE*)_gx_system_memory_allocator(buffer_size);
    if (!buffer)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    memset(buffer, 0, buffer_size);
    buffer_index = 0;

    return GX_SUCCESS;
}

UINT GX_BINRES_DATA_INFO::string_header_seek(GX_STRING_HEADER* header)
{
    // skip theme
    read_offset += resource_header.theme_data_size;

    string_header_load(header);

    if (header->magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    return GX_SUCCESS;
}

UINT GX_BINRES_DATA_INFO::theme_load(const GX_UBYTE* root_address, INT theme_index, GX_THEME** returned_theme)
{
    *returned_theme = GX_NULL;

    UINT result = initialize(root_address);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    result = theme_buffer_allocate(theme_index);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    GX_THEME_HEADER theme_header;

    resource_header_seek();
    theme_seek(theme_index, &theme_header);

    GX_THEME* theme = (GX_THEME*)ptr_of_buffer();
    buffer_index += sizeof(GX_THEME);

    theme->color_table_size = theme_header.color_count;
    theme->palette_size = theme_header.palette_count;
    theme->font_table_size = theme_header.font_count;
    theme->pixelmap_table_size = (USHORT)(theme_header.pixelmap_count + 1);
    theme->vertical_scrollbar_appearance = theme_header.vscroll_appearance;
    theme->horizontal_scrollbar_appearance = theme_header.hscroll_appearance;
    theme->vertical_scrollbar_style = theme_header.vscroll_style;
    theme->horizontal_scrollbar_style = theme_header.hscroll_style;

    result = color_table_load(theme_header.color_count, &theme->color_table);

    if ((result == GX_SUCCESS) && theme_header.palette_data_size && theme_header.palette_count)
    {
        result = palette_load(theme_header.palette_count, &theme->palette);
    }

    if ((result == GX_SUCCESS) && theme_header.font_data_size && theme_header.font_count)
    {
        result = font_table_load(theme_header.font_count, &theme->font_table);
    }

    if ((result == GX_SUCCESS) && theme_header.pixelmap_data_size && theme_header.pixelmap_count)
    {
        theme->pixelmap_table = (GX_PIXELMAP**)ptr_of_buffer();
        buffer_index += (UINT)(theme_header.pixelmap_count + 1) * sizeof(GX_PIXELMAP*);

        result = pixelmap_table_load(theme->pixelmap_table_size, theme->pixelmap_table);
    }

    if (result != GX_SUCCESS)
    {
        _gx_system_memory_free(buffer);
        buffer = GX_NULL;

        return result;
    }

    *returned_theme = theme;

    return GX_SUCCESS;
}

UINT GX_BINRES_DATA_INFO::theme_seek(INT theme_index, GX_THEME_HEADER* header)
{
    for (INT i = 0; i < resource_header.theme_count; ++i)
    {
        theme_header_load(header);

        if (header->magic_number != GX_MAGIC_NUMBER)
        {
            return GX_INVALID_FORMAT;
        }

        if (i == theme_index)
        {
            return GX_SUCCESS;
        }

        read_offset += header->data_size;
    }

    return GX_NOT_FOUND;
}

UINT GX_BINRES_DATA_INFO::theme_buffer_allocate(INT theme_index)
{
    GX_THEME_HEADER theme_header;
    UINT result = theme_seek(theme_index, &theme_header);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    buffer_size = sizeof(GX_THEME);

    // color table
    if (theme_header.color_data_size)
    {
        read_offset += theme_header.color_data_size;
    }

    // palette
    if (theme_header.palette_data_size)
    {
        read_offset += theme_header.palette_data_size;
    }

    UINT size;

    // fonts
    if (theme_header.font_data_size)
    {
        buffer_size += theme_header.font_count * sizeof(GX_FONT*);

        for (INT i = 0; i < theme_header.font_count; ++i)
        {
            result = font_buffer_size_get(&size, /*restore_read_offset*/false);
            if (result != GX_SUCCESS)
            {
                return result;
            }

            GX_UTILITY_MATH_UINT_ADD(buffer_size, size, buffer_size);
        }
    }

    // pixelmaps
    if (theme_header.pixelmap_data_size)
    {
        size = (UINT)(theme_header.pixelmap_count + 1) * sizeof(GX_PIXELMAP*);
        size += theme_header.pixelmap_count * sizeof(GX_PIXELMAP);

        GX_UTILITY_MATH_UINT_ADD(buffer_size, size, buffer_size);

        read_offset += theme_header.pixelmap_data_size;
    }

    buffer = (GX_UBYTE*)_gx_system_memory_allocator(buffer_size);
    if (!buffer)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    memset(buffer, 0, buffer_size);
    buffer_index = 0;

    return GX_SUCCESS;
}

UINT GX_BINRES_DATA_INFO::font_buffer_size_get(UINT* buffer_size, bool restore_read_offset)
{
    UINT saved_read_offset = read_offset;

    GX_FONT_HEADER font_header;
    UINT result = font_header_load(&font_header);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    UINT data_read_offset = 0;

    if (font_header.data_offset)
    {
        // The font data is not follow the header directly,
        // the data offset gives the position where the font data located.

        // record current data offset
        data_read_offset = read_offset;

        read_offset = font_header.data_offset;
        font_header_load(&font_header);
    }

    UINT font_data_size = 0;
    UINT size;
    UINT glyph_count;
    GX_PAGE_HEADER page_header;

    for (INT i = 0; i < font_header.page_count; ++i)
    {
        result = font_page_header_load(&page_header);
        if (result != GX_SUCCESS)
        {
            font_data_size = 0;
            break;
        }

        read_offset += page_header.data_size;

#if defined(GX_EXTENDED_UNICODE_SUPPORT)
        if (page_header.last_glyph > GX_MAX_GLYPH_CODE)
        {
            font_data_size = 0;
            result = GX_INVALID_FONT;
            break;
        }
#endif

        if (page_header.format & GX_FONT_FORMAT_COMPRESSED)
        {
            size = sizeof(GX_COMPRESSED_GLYPH);
        }
#if defined(GX_FONT_KERNING_SUPPORT)
        else if (page_header.format & GX_FONT_FORMAT_KERNING)
        {
            size = sizeof(GX_KERNING_GLYPH);
        }
#endif
        else
        {
            size = sizeof(GX_GLYPH);
        }

        glyph_count = (UINT)(page_header.last_glyph - page_header.first_glyph + 1);

        size *= glyph_count;
        size += sizeof(GX_FONT);

        GX_UTILITY_MATH_UINT_ADD(font_data_size, size, font_data_size);
    }

    if (data_read_offset)
    {
        // restore data offset
        read_offset = data_read_offset;
    }

    *buffer_size = font_data_size;

    if (restore_read_offset)
    {
        read_offset = saved_read_offset;
    }

    return result;
}

UINT GX_BINRES_DATA_INFO::color_table_load(USHORT color_count, GX_COLOR** returned_color_table)
{
    if (color_count == 0)
    {
        return GX_SUCCESS;
    }

    GX_COLOR_HEADER header;
    color_header_load(&header);

    if (header.magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    GX_COLOR* color_table = (GX_COLOR*)ptr_for_read();
    read_offset += header.data_size;

    *returned_color_table = color_table;

    return GX_SUCCESS;
}

UINT GX_BINRES_DATA_INFO::palette_load(USHORT color_count, GX_COLOR** returned_palette)
{
    if (color_count == 0)
    {
        return GX_SUCCESS;
    }

    GX_PALETTE_HEADER header;
    palette_header_load(&header);

    if (header.magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    GX_COLOR* palette = (GX_COLOR*)ptr_for_read();
    read_offset += header.data_size;

    *returned_palette = palette;

    return GX_SUCCESS;
}

UINT GX_BINRES_DATA_INFO::glyphs_address_get(USHORT glyph_count, const GX_GLYPH** returned_glyphs)
{
    GX_GLYPH_HEADER header;

    GX_GLYPH* glyph = (GX_GLYPH*)ptr_of_buffer();
    buffer_index += glyph_count * sizeof(GX_GLYPH);

    *returned_glyphs = glyph;

    for (INT i = 0; i < glyph_count; ++i)
    {
        glyph_header_load(&header);

        if (header.magic_number != GX_MAGIC_NUMBER)
        {
            *returned_glyphs = GX_NULL;

            return GX_INVALID_FORMAT;
        }

        glyph->ascent = header.ascent;
        glyph->descent = header.descent;
        glyph->advance = header.advance;
        glyph->leading = header.leading;
        glyph->width = header.width;
        glyph->height = header.height;

        if (header.data_size)
        {
            glyph->map = (GX_UBYTE*)(root_address + header.map_offset);
            read_offset += header.data_size;
        }
        else
        {
            glyph->map = GX_NULL;
        }

        ++glyph;
    }

    return GX_SUCCESS;
}

#ifdef GX_FONT_KERNING_SUPPORT
UINT GX_BINRES_DATA_INFO::kerning_glyphs_address_get(USHORT glyph_count, const GX_KERNING_GLYPH** returned_glyphs)
{
    GX_KERNING_GLYPH_HEADER header;

    GX_KERNING_GLYPH* glyph = (GX_KERNING_GLYPH*)ptr_of_buffer();
    buffer_index += glyph_count * sizeof(GX_KERNING_GLYPH);

    *returned_glyphs = glyph;

    for (INT i = 0; i < glyph_count; ++i)
    {
        kerning_glyph_header_load(&header);

        if (header.magic_number != GX_MAGIC_NUMBER)
        {
            *returned_glyphs = GX_NULL;

            return GX_INVALID_FORMAT;
        }

        glyph->ascent = header.ascent;
        glyph->descent = header.descent;
        glyph->advance = header.advance;
        glyph->leading = header.leading;
        glyph->width = header.width;
        glyph->height = header.height;

        if (header.data_size)
        {
            glyph->map = (GX_UBYTE*)(root_address + header.map_offset);
            read_offset += header.data_size;
        }
        else
        {
            glyph->map = GX_NULL;
        }

        if (header.kerning_table_size)
        {
            glyph->kerning_table = (GX_UBYTE*)ptr_for_read();
            read_offset += header.kerning_table_size;
        }

        ++glyph;
    }

    return GX_SUCCESS;
}
#endif

UINT GX_BINRES_DATA_INFO::compressed_glyphs_address_get(USHORT glyph_count, const GX_COMPRESSED_GLYPH** returned_glyphs)
{
    GX_GLYPH_HEADER header;

    GX_COMPRESSED_GLYPH* glyph = (GX_COMPRESSED_GLYPH*)ptr_of_buffer();
    buffer_index += glyph_count * sizeof(GX_COMPRESSED_GLYPH);

    *returned_glyphs = glyph;

    for (INT i = 0; i < glyph_count; ++i)
    {
        glyph_header_load(&header);

        if (header.magic_number != GX_MAGIC_NUMBER)
        {
            *returned_glyphs = GX_NULL;

            return GX_INVALID_FORMAT;
        }

        glyph->ascent = header.ascent;
        glyph->descent = header.descent;
        glyph->advance = header.advance;
        glyph->leading = header.leading;
        glyph->width = header.width;
        glyph->height = header.height;
        glyph->map_size = header.map_size;

        if (header.data_size)
        {
            glyph->map = (GX_UBYTE*)(root_address + header.map_offset);
            read_offset += header.data_size;
        }
        else
        {
            glyph->map = GX_NULL;
        }

        ++glyph;
    }

    return GX_SUCCESS;
}

UINT GX_BINRES_DATA_INFO::one_font_load(GX_FONT** return_font)
{
    GX_FONT_HEADER font_header;
    UINT result = font_header_load(&font_header);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    GX_FONT* head_page = GX_NULL;

    UINT data_read_offset = 0;

    if (font_header.built_in)
    {
        switch (font_header.bits)
        {
        case 1:
            head_page = (GX_FONT*)&_gx_system_font_mono;
            break;

        case 4:
            head_page = (GX_FONT*)&_gx_system_font_4bpp;
            break;

        case 8:
            head_page = (GX_FONT*)&_gx_system_font_8bpp;
            break;

        default:
            break;
        }
    }
    else
    {
        if (font_header.data_offset)
        {
            // The font data is not follow the header directly,
            // the data offset gives the position where the font data located.

            // record current data offset
            data_read_offset = read_offset;

            read_offset = font_header.data_offset;
            font_header_load(&font_header);
        }

        GX_FONT* font;
        GX_FONT* prev_page = GX_NULL;
        GX_PAGE_HEADER page_header;
        USHORT glyph_count;

        for (INT i = 0; i < font_header.page_count; ++i)
        {
            result = font_page_header_load(&page_header);
            if (result != GX_SUCCESS)
            {
                break;
            }

            font = (GX_FONT*)ptr_of_buffer();
            buffer_index += sizeof(GX_FONT);

            font->format = page_header.format;
            font->prespace = page_header.prespace;
            font->postspace = page_header.postspace;
            font->line_height = page_header.line_height;
            font->baseline = page_header.baseline;
            font->first_glyph = page_header.first_glyph;
            font->last_glyph = page_header.last_glyph;

            glyph_count = (USHORT)(font->last_glyph - font->first_glyph + 1);

            if (font->format & GX_FONT_FORMAT_COMPRESSED)
            {
                result = compressed_glyphs_address_get(glyph_count, &font->glyphs.compressed);
            }
#if defined(GX_FONT_KERNING_SUPPORT)
            else if (font->format & GX_FONT_FORMAT_KERNING)
            {
                result = kerning_glyphs_address_get(glyph_count, &font->glyphs.kerning);
            }
#endif
            else
            {
                result = glyphs_address_get(glyph_count, &font->glyphs.normal);
            }

            font->next_page = GX_NULL;

            if (!head_page)
            {
                head_page = font;
            }

            if (prev_page)
            {
                prev_page->next_page = font;
            }

            prev_page = font;
        }
    }

    if (data_read_offset)
    {
        // restore data offset
        read_offset = data_read_offset;
    }

    if (return_font)
    {
        *return_font = head_page;
    }

    return result;
}

UINT GX_BINRES_DATA_INFO::font_table_load(USHORT table_size, GX_FONT*** returned_font_table)
{
    GX_FONT** font_table = (GX_FONT**)ptr_of_buffer();
    buffer_index += table_size * sizeof(GX_FONT*);

    *returned_font_table = font_table;

    UINT result = GX_SUCCESS;

    for (INT i = 0; i < table_size; ++i)
    {
        result = one_font_load(&font_table[i]);
        if (result != GX_SUCCESS)
        {
            *returned_font_table = GX_NULL;

            return result;
        }
    }

    return result;
}

UINT GX_BINRES_DATA_INFO::one_pixelmap_load(GX_PIXELMAP** returned_pixelmap, USHORT* map_id)
{
    GX_PIXELMAP_HEADER header;
    UINT result = pixelmap_header_load(&header);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    UINT data_read_offset = 0;

    if (header.data_offset)
    {
        // The pixelmap data is not follow the header directly,
        // the data offset gives the position where the pixelmap data located.

        // record current data offset
        data_read_offset = read_offset;

        read_offset = header.data_offset;
        pixelmap_header_load(&header);
    }

    GX_PIXELMAP* pixelmap = (GX_PIXELMAP*)ptr_of_buffer();
    buffer_index += sizeof(GX_PIXELMAP);

    pixelmap->version_major = header.version_major;
    pixelmap->version_minor = header.version_minor;
    pixelmap->flags = header.flags;
    pixelmap->format = header.format;
    pixelmap->width = (GX_VALUE)header.width;
    pixelmap->height = (GX_VALUE)header.height;
    pixelmap->transparent_color = header.transparent_color;

    pixelmap->data_size = header.map_size;
    pixelmap->aux_data_size = header.aux_data_size;

    read_offset = (read_offset + 3UL) & (~3UL);

    if (pixelmap->data_size)
    {
        pixelmap->data = ptr_for_read();
        read_offset += pixelmap->data_size;
    }

    if (pixelmap->aux_data_size)
    {
        pixelmap->aux_data = ptr_for_read();
        read_offset += pixelmap->aux_data_size;
    }

    if (data_read_offset)
    {
        // restore data offset
        read_offset = data_read_offset;
    }

    if (returned_pixelmap)
    {
        *returned_pixelmap = pixelmap;
    }

    if (map_id)
    {
        *map_id = header.index;
    }

    return GX_SUCCESS;
}

UINT GX_BINRES_DATA_INFO::pixelmap_table_load(USHORT table_size, GX_PIXELMAP** pixelmap_table)
{
    UINT result = GX_SUCCESS;

    GX_PIXELMAP* pixelmap;
    USHORT map_id;

    for (INT i = 1; i < table_size; ++i)
    {
        result = one_pixelmap_load(&pixelmap, &map_id);

        while (i < map_id)
        {
            pixelmap_table[i] = GX_NULL;
            ++i;
        }

        pixelmap_table[i] = pixelmap;

        if (result != GX_SUCCESS)
        {
            return result;
        }
    }

    return result;
}

UINT GX_BINRES_DATA_INFO::font_load(const GX_UBYTE* root_address, UINT font_index, GX_UBYTE* buffer, ULONG* buffer_size)
{
    memset(this, 0, sizeof(GX_BINRES_DATA_INFO));

    this->root_address = root_address;
    this->buffer = buffer;
    this->buffer_size = *buffer_size;

    UINT result = standalone_resource_seek(font_index);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    UINT required_size;
    result = font_buffer_size_get(&required_size, /*restore_read_offset*/true);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    if (required_size > *buffer_size)
    {
        *buffer_size = required_size;

        return GX_INVALID_MEMORY_SIZE;
    }

    return one_font_load(GX_NULL);
}

UINT GX_BINRES_DATA_INFO::pixelmap_load(const GX_UBYTE* root_address, UINT map_index, GX_PIXELMAP* pixelmap)
{
    memset(this, 0, sizeof(GX_BINRES_DATA_INFO));

    this->root_address = root_address;
    this->buffer = (GX_UBYTE*)pixelmap;
    this->buffer_size = sizeof(GX_PIXELMAP);

    UINT result = standalone_resource_seek(map_index);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    return one_pixelmap_load(GX_NULL, GX_NULL);
}

UINT GX_BINRES_DATA_INFO::standalone_resource_seek(UINT res_index)
{
    USHORT type = read_USHORT();
    if (type != GX_RESOURCE_TYPE_BINRES_STANDALONE)
    {
        return GX_INVALID_FORMAT;
    }

    // skip 2 bytes version
    read_offset += sizeof(USHORT);

    ULONG count = read_ULONG();

    if (res_index >= count)
    {
        return GX_NOT_FOUND;
    }

    if (count > 1)
    {
        const GX_UBYTE* ptr = ptr_for_read() + sizeof(ULONG) * res_index;
        read_offset = gx_binres_read_4bytes(ptr);
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_language_count_get                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrives language count of specified binary resource. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_address                          Root address of binary        */
/*                                            resource data               */
/*    header                                Returned Language count       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_binres_language_count_get(GX_UBYTE *root_address, GX_VALUE *put_count)
{
    GX_BINRES_DATA_INFO info;
    return info.language_count_get(root_address, put_count);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    gx_binres_language_info_load                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrives language header information.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_address                          Root address of binary        */
/*                                            resource data               */
/*    header                                Returned Language header      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_binres_language_info_load(GX_UBYTE *root_address, GX_LANGUAGE_HEADER *put_info)
{
    GX_BINRES_DATA_INFO info;
    return info.language_info_load(root_address, put_info);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_language_table_load_ext                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service loads a language table from a binary data buffer.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_address                          Root address of binary        */
/*                                            resource data               */
/*    returned_language_table               Pointer to loaded language    */
/*                                           table                        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_language_table_buffer_allocate                           */
/*                                          Allocate needed buffer for    */
/*                                            loading language table      */
/*    _gx_binres_resource_header_load                                     */
/*                                          Read resource header          */
/*    _gx_binres_string_header_load         Read string data header       */
/*    _gx_binres_language_header_load       Read language data header     */
/*    _gx_system_memory_free                Application defined memory    */
/*                                            free function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_binres_language_table_load_ext(GX_UBYTE *root_address, GX_STRING ***returned_language_table)
{
    GX_BINRES_DATA_INFO info;
    return info.language_table_load_ext(root_address, returned_language_table);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_theme_load                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service loads a theme table from a resource data memory.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_address                          Root address of binary        */
/*                                            resource data               */
/*    theme_id                              The indentifier of the theme  */
/*    returned_theme                        Pointer to loaded theme       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_theme_buffer_allocate      Allocate needed buffer for    */
/*                                            loading theme table         */
/*    _gx_binres_resource_header_load       Read resource header          */
/*    _gx_binres_theme_header_load          Read theme header             */
/*    _gx_binres_color_table_load           Load color table              */
/*    _gx_binres_palette_table_load         Load palette table            */
/*    _gx_binres_font_table_load            Load font table               */
/*    _gx_binres_pixelmap_table_load        Load pixelmap table           */
/*    _gx_system_memory_free                Memory free function          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_binres_theme_load(GX_UBYTE *root_address, INT theme_id, GX_THEME **returned_theme)
{
    GX_BINRES_DATA_INFO info;
    return info.theme_load(root_address, theme_id, returned_theme);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_font_load                                                */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service loads a font from a resource data memory.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_address                          Pointer to the binary data    */
/*                                            memory                      */
/*    font_index                            Resource index of the font    */
/*                                            to be loaded                */
/*    buffer                                Pointer to the buffer to      */
/*                                            store the loaded font       */
/*    buffer_size                           Size of the buffer. It will   */
/*                                            be overwritten with the     */
/*                                            required buffer size if the */
/*                                            input buffer size is        */
/*                                            insufficient                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_standalone_resource_seek  Locate the resource data       */
/*    _gx_binres_font_buffer_size_get      Get the required font buffer   */
/*                                            size                        */
/*    _gx_binres_one_font_load             Load one font                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_binres_font_load(GX_UBYTE *root_address, UINT font_index, GX_UBYTE *buffer, ULONG *buffer_size)
{
    GX_BINRES_DATA_INFO info;
    return info.font_load(root_address, font_index, buffer, buffer_size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_pixelmap_load                                            */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service loads a pixelmap from a resource data memory.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_address                          Pointer to the binary data    */
/*                                            memory                      */
/*    map_index                             Resource index of the pixelmap*/
/*                                            to be loaded                */
/*    pixelmap                              Pointer to the returned       */
/*                                            pixelmap                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_standalone_resource_seek  Locate the resource data       */
/*    _gx_binres_one_pixelmap_load         Load one pixelmap              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_binres_pixelmap_load(GX_UBYTE *root_address, UINT map_index, GX_PIXELMAP *pixelmap)
{
    GX_BINRES_DATA_INFO info;
    return info.pixelmap_load(root_address, map_index, pixelmap);
}

#endif  /* GX_BINARY_RESOURCE_SUPPORT */
