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
/**************************************************************************/

#ifndef GX_UTILITY_STRING_H
#define GX_UTILITY_STRING_H

#include "gx_global.h"

UINT    _gx_utility_string_length_check(const GX_CHAR *string, UINT *length, UINT max_string_length);
UINT    _gx_utility_string_set(GX_STRING* output, const GX_CHAR* input, UINT max_string_length);
GX_BOOL _gx_utility_string_compare(const GX_STRING *string_1, const GX_STRING *string_2, UINT count);

#ifdef GX_UTF8_SUPPORT
UINT    _gx_utility_unicode_to_utf8(ULONG unicode, GX_UBYTE *return_utf8_str, UINT *return_utf8_size);
UINT    _gx_utility_utf8_string_character_get(GX_STRING *utf8_str, GX_CHAR_CODE *glyph_value, UINT *glyph_len);
UINT    _gx_utility_utf8_string_backward_character_length_get(GX_STRING *utf8_str, INT start_index, UINT *glyph_len);
#endif /* GX_UTF8_SUPPORT */

#endif
