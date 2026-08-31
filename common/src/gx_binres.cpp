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

#include "gx_binres_loader.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_theme.h"
#include "gx_system_fonts.h"

struct GX_BINRES_DATA_INFO
{
    GX_UBYTE *root_address;
    UINT      read_offset;
    GX_UBYTE *buffer;
    UINT      buffer_size;
    UINT      buffer_index;
};

static UINT _gx_binres_resource_header_load(GX_BINRES_DATA_INFO *info, GX_RESOURCE_HEADER *header);
static UINT _gx_binres_string_header_load(GX_BINRES_DATA_INFO *info, GX_STRING_HEADER *header);
static UINT _gx_binres_language_header_load(GX_BINRES_DATA_INFO *info, GX_LANGUAGE_HEADER *header);
static UINT _gx_binres_one_pixelmap_load(GX_BINRES_DATA_INFO *info, GX_PIXELMAP **returned_pixelmap, USHORT *map_id);
static UINT _gx_binres_font_buffer_size_get(GX_BINRES_DATA_INFO *info, UINT *buffer_size, GX_BOOL reset_read_offset);
static UINT _gx_binres_one_font_load(GX_BINRES_DATA_INFO *info, GX_FONT **return_font);
static UINT _gx_binres_font_header_load(GX_BINRES_DATA_INFO *info, GX_FONT_HEADER *header);
static UINT _gx_binres_standalone_resource_seek(GX_BINRES_DATA_INFO *info, UINT res_index);

#define GX_BINRES_READ_GX_UBYTE(dest, src) (dest) = (GX_UBYTE)(src)[0]
#define GX_BINRES_READ_GX_BYTE(dest, src)  (dest) = (GX_BYTE)(src)[0]
#define GX_BINRES_READ_USHORT(dest, src)   (dest) = (USHORT)(((src)[1] << 8) | (src)[0])
#define GX_BINRES_READ_SHORT(dest, src)    (dest) = (SHORT)(((src)[1] << 8) | (src)[0])
#define GX_BINRES_READ_GX_VALUE(dest, src) (dest) = (GX_VALUE)(((src)[1] << 8) | (src)[0])
#define GX_BINRES_READ_ULONG(dest, src)    (dest) = (ULONG)(((src)[3] << 24) | ((src)[2] << 16) | ((src)[1] << 8) | (src)[0])

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
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_font_load(GX_UBYTE *root_address, UINT font_index, GX_UBYTE *buffer, ULONG *buffer_size)
{
UINT                status = GX_SUCCESS;
GX_BINRES_DATA_INFO info;
UINT                required_size;

    /* file format
     +--------+
     |        | <-- represents one bytes
     +--------+

     |+========+
     |         | <-- represents a variable number of bytes
     |+========+

     |+--------+--------+--------+--------+
     |    magic number  | resource count  |
     |+--------+--------+--------+--------+
     |+--------+--------+--------+--------+
     |         resource offset            |
     |+--------+--------+--------+--------+
     |+--------+--------+--------+--------+
     |              ...                   |
     |+--------+--------+--------+--------+
     |+===================================+
     |         resource data              |
     |+===================================+
     */

    memset(&info, 0, sizeof(GX_BINRES_DATA_INFO));

    info.root_address = (GX_UBYTE *)root_address;
    info.buffer = (GX_UBYTE *)buffer;
    info.buffer_size = *buffer_size;

    status = _gx_binres_standalone_resource_seek(&info, font_index);

    if (status != GX_SUCCESS)
    {
        return status;
    }

    status = _gx_binres_font_buffer_size_get(&info, &required_size, GX_TRUE);

    if (status != GX_SUCCESS)
    {
        return status;
    }

    if (required_size > *buffer_size)
    {
        *buffer_size = required_size;
        return GX_INVALID_MEMORY_SIZE;
    }

    status = _gx_binres_one_font_load(&info, GX_NULL);

    return status;
}
#endif

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
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_language_count_get(GX_UBYTE *root_address, GX_VALUE *put_count)
{
UINT                status = GX_SUCCESS;
GX_BINRES_DATA_INFO info;
GX_RESOURCE_HEADER  header;
GX_STRING_HEADER    string_header;

    memset(&info, 0, sizeof(GX_BINRES_DATA_INFO));

    info.root_address = root_address;

    /* Read Resource header. */
    info.read_offset = 0;
    _gx_binres_resource_header_load(&info, &header);

    /* Skip theme info.  */
    info.read_offset += header.theme_data_size;

    if (header.magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    /* Read string header. */
    _gx_binres_string_header_load(&info, &string_header);

    if (string_header.magic_number != GX_MAGIC_NUMBER)
    {
        status = GX_INVALID_FORMAT;
    }
    else
    {
        *put_count = (GX_VALUE)(string_header.language_count);
    }

    return status;
}
#endif

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
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_language_info_load(GX_UBYTE *root_address, GX_LANGUAGE_HEADER *put_info)
{
UINT                status = GX_SUCCESS;
GX_BINRES_DATA_INFO info;
GX_RESOURCE_HEADER  header;
GX_STRING_HEADER    string_header;
UINT                lang_index;

    memset(&info, 0, sizeof(GX_BINRES_DATA_INFO));

    info.root_address = root_address;

    /* Read Resource header. */
    info.read_offset = 0;
    _gx_binres_resource_header_load(&info, &header);

    /* Skip theme info.  */
    info.read_offset += header.theme_data_size;

    if (header.magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    /* Read string header. */
    _gx_binres_string_header_load(&info, &string_header);

    if (string_header.magic_number != GX_MAGIC_NUMBER)
    {
        status = GX_INVALID_FORMAT;
    }
    else
    {
        for (lang_index = 0; lang_index < string_header.language_count; lang_index++)
        {
            /* Read language header.  */
            _gx_binres_language_header_load(&info, put_info);

            if (put_info->magic_number != GX_MAGIC_NUMBER)
            {
                status = GX_INVALID_FORMAT;
                break;
            }
            info.read_offset += put_info->data_size;
            put_info++;
        }
    }

    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_resource_header_load                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads resource header from a binary data buffer.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary read control block     */
/*    header                                Returned resource header      */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_resource_header_load(GX_BINRES_DATA_INFO *info, GX_RESOURCE_HEADER *header)
{
    GX_BINRES_READ_USHORT(header->magic_number, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->version, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->theme_count, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->language_count, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_ULONG(header->theme_data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_ULONG(header->string_data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_ULONG(header->data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_string_header_load                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads string header from a binary data buffer.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary read control block     */
/*    header                                Returned string header        */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_string_header_load(GX_BINRES_DATA_INFO *info, GX_STRING_HEADER *header)
{
    GX_BINRES_READ_USHORT(header->magic_number, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->language_count, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->string_count, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_ULONG(header->data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_language_header_load                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads language header from a binary data buffer.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary read control block     */
/*    header                                Returned language header      */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_language_header_load(GX_BINRES_DATA_INFO *info, GX_LANGUAGE_HEADER *header)
{
    GX_BINRES_READ_USHORT(header->magic_number, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->index, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    memcpy(&header->name, info->root_address + info->read_offset, sizeof(header->name)); /* Use case of memcpy is verified. */
    info->read_offset += sizeof(header->name);

    GX_BINRES_READ_ULONG(header->data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_language_table_buffer_allocate                           */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function allocates needed memory buffer for loading language   */
/*    table.                                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*    language_table_type_size              Size of language table type   */
/*    string_table_type_size                Size of string table type     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           Application defined memory    */
/*                                            allocation function         */
/*    _gx_binres_resource_header_load       Read binary resource header   */
/*    _gx_binres_string_header_load         Read string data header       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_language_table_buffer_allocate(GX_BINRES_DATA_INFO *info, GX_UBYTE language_table_type_size, GX_UBYTE string_table_type_size)
{
GX_RESOURCE_HEADER res_header;
GX_STRING_HEADER   string_header;
USHORT             language_count;
USHORT             string_count;
UINT               language_table_size;
UINT               string_table_size;

    info->read_offset = 0;

    /* Read resource header.  */
    _gx_binres_resource_header_load(info, &res_header);
    info->read_offset += res_header.theme_data_size;

    if (res_header.magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    _gx_binres_string_header_load(info, &string_header);

    if (string_header.magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    language_count = string_header.language_count;

    if (language_count == 0)
    {
        return GX_INVALID_FORMAT;
    }
    string_count = string_header.string_count;

    /* Calcualte memory size needed for string tables. */
    string_table_size = (UINT)(string_count * string_table_type_size);
    GX_UTILITY_MATH_UINT_MULT(string_table_size, language_count, string_table_size)

    /* Calculate memory size needed for language table. */
    language_table_size = (UINT)(language_table_type_size * language_count);

    /* Calculate memory size needed.  */
    GX_UTILITY_MATH_UINT_ADD(string_table_size, language_table_size, info->buffer_size)

    info->buffer = (GX_UBYTE *)_gx_system_memory_allocator(info->buffer_size);

    if (!info->buffer)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    memset(info->buffer, 0, info->buffer_size);
    info->buffer_index = 0;

    return GX_SUCCESS;
}
#endif

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
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_language_table_load_ext(GX_UBYTE *root_address, GX_STRING ***returned_language_table)
{
UINT                status;
GX_BINRES_DATA_INFO info;
GX_RESOURCE_HEADER  header;
GX_STRING_HEADER    string_header;
GX_LANGUAGE_HEADER  language_header;
GX_STRING         **language_table;
UINT                lang_index;
UINT                string_index;
USHORT              string_length;
GX_UBYTE           *get_data;

    memset(&info, 0, sizeof(GX_BINRES_DATA_INFO));

    info.root_address = root_address;

    /* Allocate memory that needed for language table.  */
    status = _gx_binres_language_table_buffer_allocate(&info, sizeof(GX_STRING *), sizeof(GX_STRING));

    /* Read Resource header. */
    info.read_offset = 0;
    _gx_binres_resource_header_load(&info, &header);

    /* Skip theme info.  */
    info.read_offset += header.theme_data_size;

    if (header.magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    language_table = GX_NULL;

    /* Read language table.  */
    if (status == GX_SUCCESS)
    {
        /* Read string header. */
        _gx_binres_string_header_load(&info, &string_header);

        language_table = (GX_STRING **)(info.buffer + info.buffer_index);
        info.buffer_index += sizeof(GX_STRING *) * string_header.language_count;

        for (lang_index = 0; lang_index < string_header.language_count; lang_index++)
        {
            /* Read language header.  */
            _gx_binres_language_header_load(&info, &language_header);

            if (language_header.magic_number != GX_MAGIC_NUMBER)
            {
                status = GX_INVALID_FORMAT;
                break;
            }

            /* Read string table.  */
            language_table[lang_index] = (GX_STRING *)(info.buffer + info.buffer_index);
            info.buffer_index += sizeof(GX_STRING) * string_header.string_count;

            for (string_index = 1; string_index < string_header.string_count; string_index++)
            {
                /* Read string length. */
                get_data = info.root_address + info.read_offset;
                string_length = *(get_data + 1);
                string_length = (USHORT)(string_length << 8);
                string_length = (USHORT)(string_length | (*get_data));
                info.read_offset += sizeof(USHORT);

                if (string_length)
                {
                    language_table[lang_index][string_index].ptr = (GX_CHAR *)(info.root_address + info.read_offset);
                }
                else
                {
                    language_table[lang_index][string_index].ptr = GX_NULL;
                }

                language_table[lang_index][string_index].length = string_length;
                info.read_offset += (UINT)(string_length + 1);
            }
        }
    }

    if (status == GX_SUCCESS)
    {
        *returned_language_table = language_table;
    }
    else
    {
        /* Free allocated memory when language loading failed. */
        if (info.buffer)
        {
            _gx_system_memory_free(info.buffer);
        }

        *returned_language_table = GX_NULL;
    }


    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_standalone_resource_seek                                 */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function locates the resource data in the binary data memory.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*    res_index                             The index of the resource to  */
/*                                            be located                  */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_standalone_resource_seek(GX_BINRES_DATA_INFO *info, UINT res_index)
{
USHORT type;
ULONG  count;

    GX_BINRES_READ_USHORT(type, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    if (type != GX_RESOURCE_TYPE_BINRES_STANDALONE)
    {
        return GX_INVALID_FORMAT;
    }

    /* Skip 2 bytes version.  */
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_ULONG(count, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    if ((USHORT)res_index >= count)
    {
        return GX_NOT_FOUND;
    }

    if (count > 1)
    {
        GX_BINRES_READ_ULONG(info->read_offset, info->root_address + info->read_offset + sizeof(ULONG) * res_index);
    }

    return GX_SUCCESS;
}
#endif

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
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_pixelmap_load(GX_UBYTE *root_address, UINT map_index, GX_PIXELMAP *pixelmap)
{
UINT                status = GX_SUCCESS;
GX_BINRES_DATA_INFO info;

    /* file format
     +--------+
     |        | <-- represents one bytes
     +--------+

     |+========+
     |        | <-- represents a variable number of bytes
     |+========+

     |+--------+--------+--------+--------+
     |    magic number  | resource count  |
     |+--------+--------+--------+--------+
     |+--------+--------+--------+--------+
     |         resource offset            |
     |+--------+--------+--------+--------+
     |+--------+--------+--------+--------+
     |              ...                   |
     |+--------+--------+--------+--------+
     |+===================================+
     |         resource data              |
     |+===================================+
     */

    memset(&info, 0, sizeof(GX_BINRES_DATA_INFO));

    info.root_address = (GX_UBYTE *)root_address;
    info.buffer = (GX_UBYTE *)pixelmap;
    info.buffer_size = sizeof(GX_PIXELMAP);

    status = _gx_binres_standalone_resource_seek(&info, map_index);

    if (status == GX_SUCCESS)
    {
        status = _gx_binres_one_pixelmap_load(&info, GX_NULL, GX_NULL);
    }

    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_theme_header_load                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads theme header from a binary data buffer.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary read control block     */
/*    header                                Returned theme header         */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT

static UINT _gx_binres_theme_header_load(GX_BINRES_DATA_INFO *info, GX_THEME_HEADER *header)
{
    GX_BINRES_READ_USHORT(header->magic_number, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->index, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->color_count, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->palette_count, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->font_count, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->pixelmap_count, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_GX_VALUE(header->vscroll_appearance.width, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_VALUE);

    GX_BINRES_READ_GX_VALUE(header->vscroll_appearance.thumb_width, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_VALUE);

    GX_BINRES_READ_GX_VALUE(header->vscroll_appearance.thumb_travel_min, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_VALUE);

    GX_BINRES_READ_GX_VALUE(header->vscroll_appearance.thumb_travel_max, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_VALUE);

    GX_BINRES_READ_GX_UBYTE(header->vscroll_appearance.thumb_border_style, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_ULONG(header->vscroll_appearance.fill_pixelmap, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->vscroll_appearance.thumb_pixelmap, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->vscroll_appearance.up_pixelmap, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->vscroll_appearance.down_pixelmap, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->vscroll_appearance.thumb_color, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->vscroll_appearance.thumb_border_color, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->vscroll_appearance.button_color, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_GX_VALUE(header->hscroll_appearance.width, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_VALUE);

    GX_BINRES_READ_GX_VALUE(header->hscroll_appearance.thumb_width, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_VALUE);

    GX_BINRES_READ_GX_VALUE(header->hscroll_appearance.thumb_travel_min, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_VALUE);

    GX_BINRES_READ_GX_VALUE(header->hscroll_appearance.thumb_travel_max, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_VALUE);

    GX_BINRES_READ_GX_UBYTE(header->hscroll_appearance.thumb_border_style, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_ULONG(header->hscroll_appearance.fill_pixelmap, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->hscroll_appearance.thumb_pixelmap, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->hscroll_appearance.up_pixelmap, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->hscroll_appearance.down_pixelmap, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->hscroll_appearance.thumb_color, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->hscroll_appearance.thumb_border_color, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->hscroll_appearance.button_color, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_RESOURCE_ID);

    GX_BINRES_READ_ULONG(header->vscroll_style, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_ULONG(header->hscroll_style, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_ULONG(header->color_data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_ULONG(header->palette_data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_ULONG(header->font_data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_ULONG(header->pixelmap_data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_ULONG(header->data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_color_header_load                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads color header from a binary data buffer.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary read control block     */
/*    header                                Returned color header         */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_color_header_load(GX_BINRES_DATA_INFO *info, GX_COLOR_HEADER *header)
{
    GX_BINRES_READ_USHORT(header->magic_number, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->color_count, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_ULONG(header->data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_palette_header_load                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads palette header from a binary data buffer.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary read control block     */
/*    header                                Returned palette header       */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_palette_header_load(GX_BINRES_DATA_INFO *info, GX_PALETTE_HEADER *header)
{
    GX_BINRES_READ_USHORT(header->magic_number, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->color_count, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_ULONG(header->data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_font_header_load                                         */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads font header from a binary data buffer.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary read control block     */
/*    header                                Returned font header          */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_font_header_load(GX_BINRES_DATA_INFO *info, GX_FONT_HEADER *header)
{
    GX_BINRES_READ_USHORT(header->magic_number, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    if(header->magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    GX_BINRES_READ_USHORT(header->index, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->page_count, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_GX_UBYTE(header->deault, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_GX_UBYTE(header->bits, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_ULONG(header->data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_ULONG(header->data_offset, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_page_header_load                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads page header from a binary data buffer.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary read control block     */
/*    header                                Returned page header          */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_page_header_load(GX_BINRES_DATA_INFO *info, GX_PAGE_HEADER *header)
{
ULONG read_data = 0;

    GX_BINRES_READ_USHORT(header->magic_number, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    if(header->magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    GX_BINRES_READ_USHORT(header->index, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_GX_UBYTE(header->format, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_GX_UBYTE(header->prespace, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_GX_UBYTE(header->postspace, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_GX_UBYTE(header->line_height, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_GX_UBYTE(header->baseline, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    /* In the binary resource file, CHAR_CODE is always stored as 4-byte value. */
    GX_BINRES_READ_ULONG(read_data, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);
    header->first_glyph = (GX_CHAR_CODE)read_data;

    /* In the binary resource file, CHAR_CODE is always stored as 4-byte value. */
    GX_BINRES_READ_ULONG(read_data, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);
    header->last_glyph = (GX_CHAR_CODE)read_data;

    GX_BINRES_READ_ULONG(header->data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_glyph_header_load                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads glyph header from a binary data buffer.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary read control block     */
/*    header                                Returned glyph header         */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_glyph_header_load(GX_BINRES_DATA_INFO *info, GX_GLYPH_HEADER *header)
{
    GX_BINRES_READ_USHORT(header->magic_number, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->map_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_ULONG(header->map_offset, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_USHORT(header->index, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_SHORT(header->ascent, info->root_address + info->read_offset);
    info->read_offset += sizeof(SHORT);

    GX_BINRES_READ_SHORT(header->descent, info->root_address + info->read_offset);
    info->read_offset += sizeof(SHORT);

    GX_BINRES_READ_GX_UBYTE(header->advance, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_GX_BYTE(header->leading, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_BYTE);

    GX_BINRES_READ_GX_UBYTE(header->width, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_GX_UBYTE(header->height, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_ULONG(header->data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    return GX_SUCCESS;
}
#endif


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_kerning_glyph_header_load                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads glyph header from a binary data buffer.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary read control block     */
/*    header                                Returned glyph header         */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_FONT_KERNING_SUPPORT
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_kerning_glyph_header_load(GX_BINRES_DATA_INFO *info, GX_KERNING_GLYPH_HEADER *header)
{
    _gx_binres_glyph_header_load(info, (GX_GLYPH_HEADER *)header);

    GX_BINRES_READ_USHORT(header->kerning_table_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    return GX_SUCCESS;
}
#endif
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_pixelmap_header_load                                     */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads pixelmap header from a binary data buffer.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary read control block     */
/*    header                                Returned pixelmap header      */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_pixelmap_header_load(GX_BINRES_DATA_INFO *info, GX_PIXELMAP_HEADER *header)
{
    GX_BINRES_READ_USHORT(header->magic_number, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    if (header->magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    GX_BINRES_READ_USHORT(header->index, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_GX_UBYTE(header->version_major, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_GX_UBYTE(header->version_minor, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_GX_UBYTE(header->flags, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_GX_UBYTE(header->format, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_UBYTE);

    GX_BINRES_READ_ULONG(header->map_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_ULONG(header->aux_data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_ULONG(header->transparent_color, info->root_address + info->read_offset);
    info->read_offset += sizeof(GX_COLOR);

    GX_BINRES_READ_USHORT(header->width, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_USHORT(header->height, info->root_address + info->read_offset);
    info->read_offset += sizeof(USHORT);

    GX_BINRES_READ_ULONG(header->data_size, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    GX_BINRES_READ_ULONG(header->data_offset, info->root_address + info->read_offset);
    info->read_offset += sizeof(ULONG);

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_font_buffer_size_get                                     */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function calculates the required buffer size needed for        */
/*    loading the font from the current position.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*    buffer_size                           The required buffer size      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_font_header_load           Read font header              */
/*    _gx_binres_page_header_load           Read font page header         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_font_buffer_size_get(GX_BINRES_DATA_INFO *info, UINT *buffer_size, GX_BOOL reset_read_offset)
{
UINT           status;
GX_FONT_HEADER font_header;
GX_PAGE_HEADER page_header;
USHORT         page_index;
UINT           glyph_count;
UINT           read_offset = 0;
UINT           temp;
UINT           size = 0;
UINT           old_offset = info->read_offset;

    status = _gx_binres_font_header_load(info, &font_header);

    if (status != GX_SUCCESS)
    {
        return status;
    }

    if (font_header.data_offset)
    {
        /* The font data is not follow the header directly,
           the data offset gives the position where the font
           data located. */

        /* Record current data offset. */
        read_offset = info->read_offset;

        /* Temporarily reset data offset of access font data. */
        info->read_offset = font_header.data_offset;

        /* Load font header. */
        _gx_binres_font_header_load(info, &font_header);
    }

    for (page_index = 0; page_index < font_header.page_count; page_index++)
    {
        /* Load page header. */
        status = _gx_binres_page_header_load(info, &page_header);

        if(status != GX_SUCCESS)
        {
            return status;
        }

        info->read_offset += page_header.data_size;

#if defined(GX_EXTENDED_UNICODE_SUPPORT)
        if (page_header.last_glyph > GX_MAX_GLYPH_CODE)
        {
            return GX_INVALID_FONT;
        }
#endif

        /* Max glyph code is 0x10f000, overflow cannot occur. */
        glyph_count = (UINT)(page_header.last_glyph - page_header.first_glyph + 1);

        /* Calculate size for loading font page. */
        temp = sizeof(GX_FONT);

        /* Calculate size for loading glyphs. */
        if (page_header.format & GX_FONT_FORMAT_COMPRESSED)
        {
            temp += sizeof(GX_COMPRESSED_GLYPH) * glyph_count;
        }
#if defined(GX_FONT_KERNING_SUPPORT)
        else if (page_header.format & GX_FONT_FORMAT_KERNING)
        {
            temp += sizeof(GX_KERNING_GLYPH) * glyph_count;
        }
#endif
        else
        {
            temp += sizeof(GX_GLYPH) * glyph_count;
        }

        GX_UTILITY_MATH_UINT_ADD(size, temp, size);
    }

    if (read_offset)
    {
        /* Restore data offset. */
        info->read_offset = read_offset;
        read_offset = 0;
    }

    *buffer_size = size;

    if (reset_read_offset)
    {
        /* Reset offset.  */
        info->read_offset = old_offset;
    }

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_theme_table_buffer_allocate                              */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function allocates needed memory buffer for loading theme      */
/*    table.                                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_allocator           Application defined memory    */
/*                                            allocation function         */
/*    _gx_binres_resource_header_load       Read resource header          */
/*    _gx_binres_theme_header_load          Read theme header             */
/*    _gx_binres_font_buffer_size_get       Get required font buffer size */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_theme_buffer_allocate(GX_BINRES_DATA_INFO *info, INT theme_id)
{
UINT               status = GX_SUCCESS;
GX_RESOURCE_HEADER res_header;
GX_THEME_HEADER    theme_header;
USHORT             theme_index;
USHORT             font_index;
UINT               temp;

    /* Read resource header.  */
    info->read_offset = 0;

    _gx_binres_resource_header_load(info, &res_header);

    if (res_header.magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    info->buffer_size = 0;

    for (theme_index = 0; theme_index < res_header.theme_count; theme_index++)
    {
        _gx_binres_theme_header_load(info, &theme_header);

        if (theme_header.magic_number != GX_MAGIC_NUMBER)
        {
            return GX_INVALID_FORMAT;
        }

        if (theme_index == theme_id)
        {
            info->buffer_size = sizeof(GX_THEME);

            /* Color table buffer size.  */
            if (theme_header.color_data_size)
            {
                info->read_offset += theme_header.color_data_size;
            }

            /* Palette table buffer size.  */
            if (theme_header.palette_data_size)
            {
                info->read_offset += theme_header.palette_data_size;
            }

            /* Font table buffer size.  */
            if (theme_header.font_data_size)
            {
                /* Font count is USHORT type, overflow cannot occur. */
                info->buffer_size += sizeof(GX_FONT *) * theme_header.font_count;

                for (font_index = 0; font_index < theme_header.font_count; font_index++)
                {
                    status = _gx_binres_font_buffer_size_get(info, &temp, GX_FALSE);

                    if (status != GX_SUCCESS)
                    {
                        return status;
                    }

                    GX_UTILITY_MATH_UINT_ADD(info->buffer_size, temp, info->buffer_size);
                }
            }

            /* Pixelmap table buffer size.  */
            if (theme_header.pixelmap_data_size)
            {
                /* Pixelmap count is USHORT type, overflow can not occur. */
                temp = ((UINT)(theme_header.pixelmap_count + 1)) * sizeof(GX_PIXELMAP *);
                temp += sizeof(GX_PIXELMAP) * theme_header.pixelmap_count;

                GX_UTILITY_MATH_UINT_ADD(info->buffer_size, temp, info->buffer_size);

                info->read_offset += theme_header.pixelmap_data_size;
            }

            break;
        }
        else
        {
            info->read_offset += theme_header.data_size;
        }
    }

    info->buffer = (GX_UBYTE *)_gx_system_memory_allocator(info->buffer_size);

    if (!info->buffer)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    memset(info->buffer, 0, info->buffer_size);
    info->buffer_index = 0;

    /* Reset read offset.  */
    info->read_offset = 0;

    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_glyphs_address_get                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function finds the memory address of specified glyph data from */
/*    a binary data buffer.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*    glyph_count                           The number of glyphs that the */
/*                                            font page contains.         */
/*    returned_glyphs                       Returned glyph address        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_glyph_header_load          Read glyph data header        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_glyphs_address_get(GX_BINRES_DATA_INFO *info, USHORT glyph_count, const GX_GLYPH **returned_glyphs)
{
GX_GLYPH_HEADER header;
GX_GLYPH       *glyphs;
USHORT          index = 0;

    glyphs = (GX_GLYPH *)(info->buffer + info->buffer_index);
    info->buffer_index += sizeof(GX_GLYPH) * glyph_count;

    for (index = 0; index < glyph_count; index++)
    {
        _gx_binres_glyph_header_load(info, &header);

        if (header.magic_number != GX_MAGIC_NUMBER)
        {
            return GX_INVALID_FORMAT;
        }

        glyphs[index].advance = header.advance;
        glyphs[index].ascent = header.ascent;
        glyphs[index].descent = header.descent;
        glyphs[index].height = header.height;
        glyphs[index].leading = header.leading;
        glyphs[index].width = header.width;

        /* Read glyph data.  */
        if (header.data_size)
        {
            glyphs[index].map = (GX_UBYTE *)(info->root_address + header.map_offset);
            info->read_offset += header.data_size;
        }
        else
        {
            glyphs[index].map = GX_NULL;
        }
    }

    *returned_glyphs = glyphs;

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_kerning_glyphs_address_get                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the memory address of specified kerning glyph     */
/*      from a binary data buffer.                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*    glyph_count                           The number of glyphs that the */
/*                                            font page contains.         */
/*    returned_glyphs                       Returned glyph address        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_kerning_glyph_header_load                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_FONT_KERNING_SUPPORT
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_kerning_glyphs_address_get(GX_BINRES_DATA_INFO *info, USHORT glyph_count,
                                                  const GX_KERNING_GLYPH **returned_glyphs)
{
GX_KERNING_GLYPH_HEADER header;
GX_KERNING_GLYPH       *glyphs;
USHORT                  index = 0;

    glyphs = (GX_KERNING_GLYPH *)(info->buffer + info->buffer_index);
    info->buffer_index += sizeof(GX_KERNING_GLYPH) * glyph_count;

    for (index = 0; index < glyph_count; index++)
    {
        _gx_binres_kerning_glyph_header_load(info, &header);

        if (header.magic_number != GX_MAGIC_NUMBER)
        {
            return GX_INVALID_FORMAT;
        }

        glyphs[index].advance = header.advance;
        glyphs[index].ascent = header.ascent;
        glyphs[index].descent = header.descent;
        glyphs[index].height = header.height;
        glyphs[index].leading = header.leading;
        glyphs[index].width = header.width;

        /* Read glyph data.  */
        if (header.data_size)
        {
            glyphs[index].map = (GX_UBYTE *)(info->root_address + header.map_offset);
            info->read_offset += header.data_size;
        }
        else
        {
            glyphs[index].map = GX_NULL;
        }

        /* Set glyph kerning table. */
        if (header.kerning_table_size)
        {
            glyphs[index].kerning_table = (GX_UBYTE *)(info->root_address + info->read_offset);
            info->read_offset += header.kerning_table_size;
        }
    }

    *returned_glyphs = glyphs;

    return GX_SUCCESS;
}
#endif
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_compressed_glyphs_address_get                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the memory address of specified compressed glyph  */
/*    from a binary data buffer.                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*    glyph_count                           The number of glyphs that the */
/*                                            font page contains.         */
/*    returned_glyphs                       Returned glyph address        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_glyph_header_load          Read glyph data header        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_compressed_glyphs_address_get(GX_BINRES_DATA_INFO *info, USHORT glyph_count,
                                                     const GX_COMPRESSED_GLYPH **returned_glyphs)
{
GX_GLYPH_HEADER      header;
GX_COMPRESSED_GLYPH *glyphs;
USHORT               index = 0;

    glyphs = (GX_COMPRESSED_GLYPH *)(info->buffer + info->buffer_index);
    info->buffer_index += sizeof(GX_COMPRESSED_GLYPH) * glyph_count;

    for (index = 0; index < glyph_count; index++)
    {
        _gx_binres_glyph_header_load(info, &header);

        if (header.magic_number != GX_MAGIC_NUMBER)
        {
            return GX_INVALID_FORMAT;
        }

        glyphs[index].advance = header.advance;
        glyphs[index].ascent = header.ascent;
        glyphs[index].descent = header.descent;
        glyphs[index].height = header.height;
        glyphs[index].leading = header.leading;
        glyphs[index].map_size = header.map_size;
        glyphs[index].width = header.width;

        /* Read glyph data.  */
        if (header.data_size)
        {
            glyphs[index].map = (GX_UBYTE *)(info->root_address + header.map_offset);
            info->read_offset += header.data_size;
        }
        else
        {
            glyphs[index].map = GX_NULL;
        }
    }

    *returned_glyphs = glyphs;

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_one_font_load                                            */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service loads a font from a binary data buffer.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*    return_font                           Returned font                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_font_header_load           Read font header              */
/*    _gx_binres_page_header_load           Read font page header         */
/*    _gx_binres_compressed_glyphs_read     Read compressed glyph data    */
/*    _gx_binres_glyphs_read                Read glyph data               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_one_font_load(GX_BINRES_DATA_INFO *info, GX_FONT **return_font)
{
UINT           status = GX_SUCCESS;
GX_FONT_HEADER font_header;
GX_PAGE_HEADER header;
GX_FONT       *font;
GX_FONT       *head_page = GX_NULL;
GX_FONT       *pre_page = GX_NULL;
USHORT         index;
USHORT         glyph_count;
UINT           read_offset = 0;

    /* Read font header.  */
    status = _gx_binres_font_header_load(info, &font_header);

    if (status != GX_SUCCESS)
    {
        return status;
    }

    if (font_header.deault)
    {
        switch (font_header.bits)
        {
        case 1:
            head_page = (GX_FONT *)&_gx_system_font_mono;
            break;

        case 4:
            head_page = (GX_FONT *)&_gx_system_font_4bpp;
            break;

        case 8:
            head_page = (GX_FONT *)&_gx_system_font_8bpp;
            break;
        }
    }
    else
    {
        if (font_header.data_offset)
        {
            /* The font data is not follow the header directly,
               the data offset gives the position where the font
               data located. */

            /* Record current data offset. */
            read_offset = info->read_offset;

            /* Temporarily reset data offset to access font data. */
            info->read_offset = font_header.data_offset;

            /* Read font header. */
            _gx_binres_font_header_load(info, &font_header);
        }

        for (index = 0; index < font_header.page_count; index++)
        {
            /* Read page header.  */
            status = _gx_binres_page_header_load(info, &header);

            if (status != GX_SUCCESS)
            {
                return status;
            }

            font = (GX_FONT *)(info->buffer + info->buffer_index);
            info->buffer_index += sizeof(GX_FONT);

            font->baseline = header.baseline;
            font->first_glyph = header.first_glyph;
            font->format = header.format;
            font->last_glyph = header.last_glyph;
            font->line_height = header.line_height;
            font->postspace = header.postspace;
            font->prespace = header.prespace;

            /* Read glyphs data.  */
            glyph_count = (USHORT)(font->last_glyph - font->first_glyph + 1);

            if (font->format & GX_FONT_FORMAT_COMPRESSED)
            {
                status = _gx_binres_compressed_glyphs_address_get(info, glyph_count, &font->glyphs.compressed);
            }
#if defined(GX_FONT_KERNING_SUPPORT)
            else if (font->format & GX_FONT_FORMAT_KERNING)
            {
                status = _gx_binres_kerning_glyphs_address_get(info, glyph_count, &font->glyphs.kerning);
            }
#endif /* GX_FONT_KERNING_SUPPORT */
            else
            {
                status = _gx_binres_glyphs_address_get(info, glyph_count, &font->glyphs.normal);
            }

            if (status != GX_SUCCESS)
            {
                break;
            }

            font->next_page = GX_NULL;

            if (!head_page)
            {
                head_page = font;
            }

            if (pre_page)
            {
                pre_page->next_page = font;
            }

            pre_page = font;
        }
    }

    if (read_offset)
    {
        /* Restore data offset. */
        info->read_offset = read_offset;
        read_offset = 0;
    }

    if (return_font)
    {
        *return_font = head_page;
    }

    return status;
}
#endif
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_color_table_load                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function loads a color table from a binary data buffer.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*    color_count                           The number of colors to read  */
/*    returned_color_table                  Returned color table          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_color_header_load          Read color data header        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_color_table_load(GX_BINRES_DATA_INFO *info, USHORT color_count, GX_COLOR **returned_color_table)
{
GX_COLOR_HEADER header;
GX_COLOR       *color_table = GX_NULL;

    if (!color_count)
    {
        return GX_SUCCESS;
    }

    /* Read color table header.  */
    _gx_binres_color_header_load(info, &header);

    if (header.magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    color_table = (GX_COLOR *)(info->root_address + info->read_offset);
    info->read_offset += header.data_size;

    *returned_color_table = color_table;

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_palette_table_load                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function loads a palette table from a binary data buffer.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*    color_count                           The number of colors to read  */
/*    returned_palette_table                Returned palette table        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_palette_header_load        Read palette data header      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_palette_table_load(GX_BINRES_DATA_INFO *info, USHORT color_count, GX_COLOR **returned_palette_table)
{
GX_PALETTE_HEADER header;
GX_COLOR         *palette_table = GX_NULL;

    if (!color_count)
    {
        return GX_SUCCESS;
    }

    /* Read palette table header.  */
    _gx_binres_palette_header_load(info, &header);

    if (header.magic_number != GX_MAGIC_NUMBER)
    {
        return GX_INVALID_FORMAT;
    }

    palette_table = (GX_COLOR *)(info->root_address + info->read_offset);
    info->read_offset += header.data_size;

    *returned_palette_table = palette_table;

    return GX_SUCCESS;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_font_table_load                                          */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function loads a font table from resource data memory.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*    table_size                            Font table size               */
/*    returned_font_table                   Returned font table           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_one_font_load              Load one font                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
static UINT _gx_binres_font_table_load(GX_BINRES_DATA_INFO *info, USHORT table_size, GX_FONT ***returned_font_table)
{
UINT      status = GX_SUCCESS;
GX_FONT **font_table = GX_NULL;
USHORT    index;

    font_table = (GX_FONT **)(info->buffer + info->buffer_index);
    info->buffer_index += sizeof(GX_FONT *) * table_size;

    for (index = 0; index < table_size; index++)
    {
        status = _gx_binres_one_font_load(info, &font_table[index]);

        if (status != GX_SUCCESS)
        {
            return status;
        }
    }

    *returned_font_table = font_table;

    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_one_pixelmap_load                                        */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function loads one pixelmap from resource data memory.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*    returned_pixelmap                     Returned pixelmap table       */
/*    map_id                                Loaded pixelmap ID            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_pixelmap_header_load       Read pixelmap header          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_one_pixelmap_load(GX_BINRES_DATA_INFO *info, GX_PIXELMAP **returned_pixelmap, USHORT *map_id)
{
UINT               status = GX_SUCCESS;
GX_PIXELMAP_HEADER header;
ULONG              size;
UINT               read_offset = 0;

GX_PIXELMAP       *pixelmap;

    /* Read pixelmap header.  */
    status = _gx_binres_pixelmap_header_load(info, &header);

    if (status)
    {
        return status;
    }

    if (header.data_offset)
    {
        /* The pixelmap data is not follow the header directly,
           the data offset gives the position where the pixelmap
           data located. */

        /* Record current data offset. */
        read_offset = info->read_offset;

        /* Temporarily reset data offset to access pixelmap data. */
        info->read_offset = header.data_offset;

        /* Read pixelmap header. */
        _gx_binres_pixelmap_header_load(info, &header);
    }

    /* Allocate memory for pixelmap.  */
    pixelmap = (GX_PIXELMAP *)(info->buffer + info->buffer_index);
    info->buffer_index += sizeof(GX_PIXELMAP);

    pixelmap->aux_data_size = header.aux_data_size;
    pixelmap->data_size = header.map_size;
    pixelmap->flags = header.flags;
    pixelmap->format = header.format;
    pixelmap->width = (GX_VALUE)header.width;
    pixelmap->height = (GX_VALUE)header.height;
    pixelmap->transparent_color = header.transparent_color;
    pixelmap->version_major = header.version_major;
    pixelmap->version_minor = header.version_minor;

    /* Skip padding bytes. */
    info->read_offset = (info->read_offset + 3) & (~0x03UL);

    /* Read pixelmap data.  */
    size = pixelmap->data_size;
    if (size)
    {
        pixelmap->data = (GX_UBYTE *)(info->root_address + info->read_offset);
        info->read_offset += size;
    }

    /* Read pixelmap aux data.  */
    size = pixelmap->aux_data_size;
    if (size)
    {
        pixelmap->aux_data = (GX_UBYTE *)(info->root_address + info->read_offset);
        info->read_offset += size;
    }

    if (read_offset)
    {
        /* Restore data offset. */
        info->read_offset = read_offset;
        read_offset = 0;
    }

    if (returned_pixelmap)
    {
        *returned_pixelmap = pixelmap;
    }

    if (map_id)
    {
        *map_id = header.index;
    }

    return status;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_binres_pixelmap_table_load                                      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function loads a pixelmap table from resource data memory.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    info                                  Binary resource control block */
/*    table_size                            Pixelmap table size           */
/*    returned_pixelmp_table                Returned pixelmap table       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_one_pixelmap_load          Load one pixelmap             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_binres_pixelmap_table_load(GX_BINRES_DATA_INFO *info, USHORT table_size, GX_PIXELMAP **pixelmap_table)
{
UINT         status = GX_SUCCESS;
USHORT       index;
USHORT       map_id;
GX_PIXELMAP *pixelmap;

    for (index = 1; index < table_size; index++)
    {
        status = _gx_binres_one_pixelmap_load(info, &pixelmap, &map_id);

        while (index < map_id)
        {
            pixelmap_table[index++] = GX_NULL;
        }

        pixelmap_table[index] = pixelmap;

        if (status)
        {
            return status;
        }
    }

    return status;
}
#endif

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
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gx_binres_theme_load(GX_UBYTE *root_address, INT theme_id, GX_THEME **returned_theme)
{
UINT                status;
GX_BINRES_DATA_INFO info;
GX_RESOURCE_HEADER  header;
GX_THEME_HEADER     theme_header;
GX_THEME           *theme;
INT                 index;

    memset(&info, 0, sizeof(GX_BINRES_DATA_INFO));
    memset(&theme_header, 0, sizeof(GX_THEME_HEADER));

    info.root_address = (GX_UBYTE *)root_address;

    /* Allocate memory that needed for theme table.  */
    status = _gx_binres_theme_buffer_allocate(&info, theme_id);

    /* Read binary resource header.  */

    if (status == GX_SUCCESS)
    {
        status = _gx_binres_resource_header_load(&info, &header);
    }

    /* Read theme table.  */
    if (status == GX_SUCCESS)
    {
        for (index = 0;; index++)
        {

            /* Read theme header.  */
            _gx_binres_theme_header_load(&info, &theme_header);

            if (index == theme_id)
            {
                theme = (GX_THEME *)(info.buffer + info.buffer_index);
                info.buffer_index += sizeof(GX_THEME);

                theme->color_table_size = theme_header.color_count;
                theme->palette_size = theme_header.palette_count;
                theme->font_table_size = theme_header.font_count;
                theme->pixelmap_table_size = (USHORT)(theme_header.pixelmap_count + 1);
                theme->vertical_scrollbar_appearance = theme_header.vscroll_appearance;
                theme->horizontal_scrollbar_appearance = theme_header.hscroll_appearance;
                theme->vertical_scroll_style = theme_header.hscroll_style;
                theme->horizontal_scroll_style = theme_header.hscroll_style;

                /* Read color table.  */
                status = _gx_binres_color_table_load(&info,
                                                     theme_header.color_count,
                                                     &theme->color_table);

                /* Read palette table.  */
                if ((status == GX_SUCCESS) && theme_header.palette_data_size)
                {
                    status = _gx_binres_palette_table_load(&info,
                                                           theme_header.palette_count,
                                                           &theme->palette);
                }

                /* Read font table.  */
                if ((status == GX_SUCCESS) && theme_header.font_data_size && theme_header.font_count)
                {
                    status = _gx_binres_font_table_load(&info,
                                                        theme_header.font_count,
                                                        &theme->font_table);
                }

                /* Read pixelmap table.  */
                if ((status == GX_SUCCESS) && theme_header.pixelmap_data_size && theme_header.pixelmap_count)
                {
                    /* Allocate pixelmap table size.  */
                    theme->pixelmap_table = (GX_PIXELMAP **)(info.buffer + info.buffer_index);
                    info.buffer_index += sizeof(GX_PIXELMAP *) * (UINT)(theme_header.pixelmap_count + 1);

                    /* Load pixelmap table.  */
                    status = _gx_binres_pixelmap_table_load(&info,
                                                            theme->pixelmap_table_size,
                                                            theme->pixelmap_table);
                }

                if (status == GX_SUCCESS)
                {
                    *returned_theme = theme;
                }
                else
                {
                    /* Free allocated buffer if theme loading failed.  */
                    _gx_system_memory_free(info.buffer);

                    *returned_theme = GX_NULL;
                }

                break;
            }
            else
            {
                info.read_offset += theme_header.data_size;
            }
        }
    }

    return status;
}
#endif

