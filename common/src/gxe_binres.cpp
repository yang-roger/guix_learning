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

#include "gx_system.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_binres_font_load                                               */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in binres font load function.       */
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
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_font_load                  The actual binres font load   */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gxe_binres_font_load(GX_UBYTE *root_address, UINT font_index, GX_UBYTE *buffer, ULONG *buffer_size)
{
    if (root_address == GX_NULL || buffer == GX_NULL || buffer_size == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    return _gx_binres_font_load(root_address, font_index, buffer, buffer_size);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_binres_language_count_get                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in binres language count get        */
/*    function.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_address                          Root address of binary        */
/*                                            resource data               */
/*    header                                Returned Language count       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_language_count_get         The actual binres language    */
/*                                            count get function          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gxe_binres_language_count_get(GX_UBYTE *root_address, GX_VALUE *put_count)
{
UINT status;

    if ((root_address == GX_NULL) || (put_count == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    status = _gx_binres_language_count_get(root_address, put_count);

    /* Return completion status code. */
    return(status);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_binres_language_info_load                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in binres language info load        */
/*    function.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    root_address                          Root address of binary        */
/*                                            resource data               */
/*    header                                Returned Language header      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_language_table_load        The actual binres language    */
/*                                            table load function         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gxe_binres_language_info_load(GX_UBYTE *root_address, GX_LANGUAGE_HEADER *put_info)
{
UINT status;

    if ((root_address == GX_NULL) || (put_info == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    status = _gx_binres_language_info_load(root_address, put_info);

    /* Return completion status code. */
    return(status);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_binres_language_table_load_ext                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in binres language table load       */
/*    function.                                                           */
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
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_language_table_load_ext    The actual binres language    */
/*                                            table load function         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gxe_binres_language_table_load_ext(GX_UBYTE *root_address, GX_STRING ***returned_language_table)
{
UINT status;

    if ((root_address == GX_NULL) || (returned_language_table == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    if ((_gx_system_memory_allocator == GX_NULL) ||
        (_gx_system_memory_free == GX_NULL))
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    status = _gx_binres_language_table_load_ext(root_address, returned_language_table);

    /* Return completion status code. */
    return(status);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_binres_pixelmap_load                                           */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in binres pixelmap load function.   */
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
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_pixelmap_load              The actual binres pixelmap    */
/*                                            load function               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gxe_binres_pixelmap_load(GX_UBYTE *root_address, UINT map_index, GX_PIXELMAP *pixelmap)
{
    if (root_address == GX_NULL || pixelmap == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    return _gx_binres_pixelmap_load(root_address, map_index, pixelmap);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_binres_theme_load                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in binres theme load function.      */
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
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_binres_theme_read                 The actual binres theme read  */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#ifdef GX_BINARY_RESOURCE_SUPPORT
UINT _gxe_binres_theme_load(GX_UBYTE *root_address, INT theme_id, GX_THEME **returned_theme)
{
UINT  status;

    if ((root_address == GX_NULL) || (returned_theme == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    if (theme_id < 0)
    {
        return GX_INVALID_VALUE;
    }

    if ((_gx_system_memory_allocator == GX_NULL) ||
        (_gx_system_memory_free == GX_NULL))
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    status = _gx_binres_theme_load(root_address, theme_id, returned_theme);

    /* Return completion status code. */
    return(status);
}
#endif
