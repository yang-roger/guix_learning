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
/**   Utility (String)                                                    */
/**                                                                       */
/**************************************************************************/

#include "gx_utility_string.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_string_compare                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function compares two strings character by character.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    string_1                              String to be compared         */
/*    string_2                              Another string to be compared */
/*    count                                 number of bytes to compare    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    [GX_TRUE | GX_FALSE]                  [equal | not equal]           */
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
GX_BOOL _gx_utility_string_compare(const GX_STRING *string_1, const GX_STRING *string_2, UINT count)
{
UINT index = 0;

    if ((string_1->length < count) || (string_2->length < count))
    {
        /* Not equal.*/
        return GX_FALSE;
    }

    while (index < count)
    {
        if (string_1->ptr[index] != string_2->ptr[index])
        {
            /* Not equal. */
            return GX_FALSE;
        }

        index++;
    }

    /* Equal. */
    return GX_TRUE;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_string_length_check                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function traverses the string and returns the string length,   */
/*    if the string is invalid or the string length is bigger than max    */
/*    string length, returns error.                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input_string                          Pointer to input string       */
/*    string_length                         Pointer to string length      */
/*    max_string_length                     Max string length             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
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
UINT  _gx_utility_string_length_check(const GX_CHAR *input_string, UINT *string_length, UINT max_string_length)
{
UINT i;

    /* Check for invalid input pointers.  */
    if (input_string == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Traverse the string.  */
    for (i = 0; input_string[i]; i++)
    {
        /* Check if the string length is bigger than the max string length.  */
        if (i >= max_string_length)
        {
            return(GX_INVALID_STRING_LENGTH);
        }
    }

    /* Return the string length if string_length is not NULL.
       String_length being NULL indicates the caller needs to check for string
       length within the max_string_length. */
    if (string_length)
    {
        *string_length = i;
    }

    return(GX_SUCCESS);
}

/**************************************************************************/

UINT  _gx_utility_string_set(GX_STRING* output, const GX_CHAR* input, UINT max_string_length)
{
UINT result;
UINT length = 0;

    result = _gx_utility_string_length_check(input, &length, max_string_length);
    if (result == GX_SUCCESS)
    {
        output->ptr = input;
        output->length = length;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_utf8_string_character_get                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function convert a unicode to a utf8.                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    unicode                               Unicode to convert            */
/*    return_utf8_str                       Returned utf8 string          */
/*    return_utf8_size                      utf8 string size              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_UTF8_SUPPORT
UINT  _gx_utility_unicode_to_utf8(ULONG unicode, GX_UBYTE *return_utf8_str, UINT *return_utf8_size)
{
    if (unicode < 0x0080)
    {
        *return_utf8_size = 1;
        return_utf8_str[0] = (GX_UBYTE)unicode;
    }
    else if (unicode < 0x0800)
    {
        *return_utf8_size = 2;
        return_utf8_str[0] = (GX_UBYTE)(0xC0 + (GX_UBYTE)((unicode & 0x7C0) >> 6));
        return_utf8_str[1] = (GX_UBYTE)(0x80 + (unicode & 0x3F));
    }
    else if (unicode < 0x10000)
    {
        *return_utf8_size = 3;
        return_utf8_str[0] = (GX_UBYTE)(0xE0 + (GX_UBYTE)((unicode & 0xF000) >> 12));
        return_utf8_str[1] = (GX_UBYTE)(0x80 + (GX_UBYTE)((unicode & 0xFC0) >> 6));
        return_utf8_str[2] = (GX_UBYTE)(0x80 + (unicode & 0x3F));
    }
    else if (unicode < 0x200000)
    {
        *return_utf8_size = 4;
        return_utf8_str[0] = (GX_UBYTE)(0xF0 + (GX_UBYTE)((unicode & 0x1C0000) >> 18));
        return_utf8_str[1] = (GX_UBYTE)(0x80 + (GX_UBYTE)((unicode & 0x3F000) >> 12));
        return_utf8_str[2] = (GX_UBYTE)(0x80 + (GX_UBYTE)((unicode & 0xFC0) >> 6));
        return_utf8_str[3] = (GX_UBYTE)(0x80 + (unicode & 0x3F));
    }
    else if (unicode < 0x4000000)
    {
        *return_utf8_size = 5;
        return_utf8_str[0] = (GX_UBYTE)(0xFC + (GX_UBYTE)((unicode & 0x3000000) >> 24));
        return_utf8_str[1] = (GX_UBYTE)(0x80 + (GX_UBYTE)((unicode & 0xFC0000) >> 18));
        return_utf8_str[2] = (GX_UBYTE)(0x80 + (GX_UBYTE)((unicode & 0x3F000) >> 12));
        return_utf8_str[3] = (GX_UBYTE)(0x80 + (GX_UBYTE)((unicode & 0xFC0) >> 6));
        return_utf8_str[4] = (GX_UBYTE)(0x80 + (unicode & 0x3F));
    }
    else
    {
        *return_utf8_size = 6;
        return_utf8_str[0] = (GX_UBYTE)(0xFC + (GX_UBYTE)((unicode & 0x40000000) >> 30));
        return_utf8_str[1] = (GX_UBYTE)(0x80 + (GX_UBYTE)((unicode & 0x3F000000) >> 24));
        return_utf8_str[2] = (GX_UBYTE)(0x80 + (GX_UBYTE)((unicode & 0xFC0000) >> 18));
        return_utf8_str[3] = (GX_UBYTE)(0x80 + (GX_UBYTE)((unicode & 0x3F000) >> 12));
        return_utf8_str[4] = (GX_UBYTE)(0x80 + (GX_UBYTE)((unicode & 0xFC0) >> 6));
        return_utf8_str[5] = (GX_UBYTE)(0x80 + (unicode & 0x3F));
    }

    return GX_SUCCESS;
}
#endif /* GX_UTF8_SUPPORT */

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_utf8_string_character_get                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function parses utf8 string to multibyte glyph.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    utf8_str                              UTF-8 string                  */
/*    glyph_value                           Multibyte value of glyph      */
/*    byte_count                            Length of UTF-8 string in byte*/
/*    glyph_len                             Length of glyph value in byte */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_string_width_get                                         */
/*    _gx_display_driver_indexed_color_text_draw                          */
/*                                                                        */
/**************************************************************************/
#ifdef GX_UTF8_SUPPORT
UINT  _gx_utility_utf8_string_character_get(GX_STRING *utf8_str, GX_CHAR_CODE *glyph_value, UINT *glyph_len)
{
const GX_CHAR *ch;
UINT              bytes;
GX_CHAR_CODE      value = 0;
UINT              len;
UINT              byte_count;

    ch = utf8_str->ptr;
    byte_count = utf8_str->length;

    /* BOM check. */
    if ((byte_count >= 3) && (*ch == 0xEF) && (*(ch + 1) == 0xBB) && (*(ch + 2) == 0xBF))
    {
        /* It is BOM. Skip it. */
        ch += 3;
        byte_count -= 3;
    }

    if (byte_count == 0)
    {
        /* Zero length string. Return error. */
        if (glyph_value)
        {
            *glyph_value = 0;
        }

        if (glyph_len)
        {
            *glyph_len = 0;
        }
        return GX_INVALID_VALUE;
    }

    /* Reset glyph length to 1. */
    len = 1;

    /* Check the first byte */
    if ((*ch & 0x80) == 0)
    {

        utf8_str->ptr = ch + 1;
        utf8_str->length -= 1;

        /* One byte glyph. */
        if (glyph_value)
        {
            *glyph_value = (*ch & ~0x80) & 0xFF;
        }

        if (glyph_len)
        {
            *glyph_len = 1;
        }

        return GX_SUCCESS;
    }
    else if ((*ch & 0xE0) == 0xC0)
    {

        /* Two bytes glyph. */
        bytes = 2;
        value = (*ch & ~0xE0) & 0xFF;
    }
    else if ((*ch & 0xF0) == 0xE0)
    {

        /* Three bytes glyph. */
        bytes = 3;
        value = (*ch & ~0xF0) & 0xFF;
    }
    else if ((*ch & 0xF8) == 0xF0)
    {

        /* Four bytes glyph. */
        bytes = 4;
        value = (*ch & ~0xF8) & 0xFF;
    }
    else if ((*ch & 0xFC) == 0xF8)
    {

        /* Five bytes glyph. */
        bytes = 5;
        value = (*ch & ~0xFC) & 0xFF;
    }
    else if ((*ch & 0xFE) == 0xFC)
    {

        /* Six bytes glyph. */
        bytes = 6;
        value = (*ch & ~0xFE) & 0xFF;
    }
    else
    {
        /* Not a valid utf8 glyph. */
        utf8_str->ptr = ch + 1;
        utf8_str->length -= 1;

        if (glyph_value)
        {
            *glyph_value = 0;
        }

        if (glyph_len)
        {
            *glyph_len = 1;
        }

        return GX_INVALID_VALUE;
    }

    if (byte_count < bytes)
    {
        /* Not a valid utf8 glyph. */
        if (glyph_value)
        {
            *glyph_value = 0;
        }

        if (glyph_len)
        {
            *glyph_len = bytes;
        }
        utf8_str->length -= bytes;

        return GX_INVALID_VALUE;
    }

    while (len < bytes)
    {
        ch++;
        len++;

        if ((*ch & 0xC0) != 0x80)
        {

            /* Not a valid utf8 glyph. */
            if (glyph_len)
            {
                *glyph_len = len;
            }

            utf8_str->ptr = ch;
            utf8_str->length -= len;

            return GX_INVALID_VALUE;
        }

        value = (GX_CHAR_CODE)(value << 6);
        value = (GX_CHAR_CODE)(value + ((*ch & ~0xC0) & 0xFF));
    }

    if (glyph_value)
    {
        *glyph_value = value;
    }

    if (glyph_len)
    {
        *glyph_len = len;
    }

    utf8_str->ptr = ch + 1;
    utf8_str->length -= len;

    return GX_SUCCESS;
}

#endif /* GX_UTF8_SUPPORT */

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_utf8_string_backward_character_length_get               */
/*                                                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns the glyph length of the previous character    */
/*    from the specified byte position.                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    string                                UTF-8 string                  */
/*    start_index                           Specified byte position       */
/*    glyph_len                             Length of glyph value in byte */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifdef GX_UTF8_SUPPORT
UINT  _gx_utility_utf8_string_backward_character_length_get(GX_STRING *string, INT start_index, UINT *glyph_len)
{
UINT len = 0;

    /* Get glyph lengh of the character in backward direction. */
    while (start_index >= 0)
    {
        len++;

        if (((string->ptr[start_index] & 0x80) == 0) ||
            ((string->ptr[start_index] & 0xC0) == 0xC0))
        {
            /* End loop when byte match 0xxxxxxx or 11xxxxxx. */
            break;
        }

        start_index--;
    }

    *glyph_len = len;

    return GX_SUCCESS;
}
#endif /* GX_UTF8_SUPPORT */
