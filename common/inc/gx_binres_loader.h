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
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_binres_loader.h                                                  */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX binres loader component,                 */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_BINRES_LOADER_H
#define GX_BINRES_LOADER_H

#include "gx_global.h"

#define GX_BINRES_VERSION_ADD_STRING_LENGTH 50600

#ifdef GX_BINARY_RESOURCE_SUPPORT

#include "gx_binres_header.h"

struct GX_PIXELMAP;
struct GX_THEME;

/* Define utility component function prototypes.  */

UINT _gx_binres_language_count_get(GX_UBYTE *root_address, GX_VALUE *put_count);
UINT _gx_binres_language_info_load(GX_UBYTE *root_address, GX_LANGUAGE_HEADER *put_info);
UINT _gx_binres_language_table_load_ext(GX_UBYTE *root_address, GX_STRING ***returned_language_table);
UINT _gx_binres_theme_load(GX_UBYTE *root_address, INT theme_id, GX_THEME **returned_theme);
UINT _gx_binres_pixelmap_load(GX_UBYTE *root_address, UINT map_index, GX_PIXELMAP *pixelmap);
UINT _gx_binres_font_load(GX_UBYTE *root_address, UINT font_index, GX_UBYTE *buffer, ULONG *buffer_size);

/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_binres_language_count_get(GX_UBYTE *root_address, GX_VALUE *put_count);
UINT _gxe_binres_language_info_load(GX_UBYTE *root_address, GX_LANGUAGE_HEADER *put_info);
UINT _gxe_binres_language_table_load_ext(GX_UBYTE *root_address, GX_STRING ***returned_language_table);
UINT _gxe_binres_theme_load(GX_UBYTE *root_address, INT theme_id, GX_THEME **returned_theme);
UINT _gxe_binres_pixelmap_load(GX_UBYTE *root_address, UINT map_index, GX_PIXELMAP *pixelmap);
UINT _gxe_binres_font_load(GX_UBYTE *root_address, UINT res_index, GX_UBYTE *buffer, ULONG *buffer_size);

#endif /* GX_BINARY_RESOURCE_SUPPORT */

#endif

