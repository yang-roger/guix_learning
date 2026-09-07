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
/**   Display Management (Display)                                        */
/**                                                                       */
/**************************************************************************/

#include "gx_display.h"

#include "gx_pixelmap.h"
#include "gx_utility.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_display_active_language_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks for errors in active language set call.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               pointer to display instance   */
/*    language                              Language index                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_active_language_set      Actual active language set     */
/*                                            call.                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_display_active_language_set(GX_DISPLAY *display, GX_UBYTE language)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (display == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (display->language_table == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (display->language_table_size <= language)
    {
        return GX_INVALID_VALUE;
    }

    status = _gx_display_active_language_set(display, language);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_display_color_set                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system color get call.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    resrouce_id                           Resource ID for color         */
/*    new_color                             New color                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_color_set                 Actual system color set call  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default draw funtions                                          */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_display_color_set(GX_DISPLAY *display, GX_RESOURCE_ID resource_id, GX_COLOR new_color)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (!display)
    {
        return GX_INVALID_DISPLAY;
    }

    if (resource_id >= display->color_table_size)
    {
        return GX_INVALID_RESOURCE_ID;
    }

    /* Call actual system color set.  */
    status = _gx_display_color_set(display, resource_id, new_color);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_system_color_table_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system color table set call.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    color_table                           Pointer to system color table */
/*    number_of_colors                      Number of colors in the new   */
/*                                            table                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_color_table_set           Actual system color table set */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default draw funtions                                          */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_display_color_table_set(GX_DISPLAY *display, GX_COLOR *color_table, INT number_of_colors)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /*Check for invalid pointer.  */
    if ((number_of_colors != 0) && color_table == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (display == GX_NULL)
    {
        return GX_INVALID_DISPLAY;
    }

    /* Call actual system color table set.  */
    status = _gx_display_color_table_set(display, color_table, number_of_colors);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_display_create                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the display create function      */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
/*    name                                  Name of display               */
/*    display_driver_setup                  Display driver setup function */
/*    width                                 Display width in pixels       */
/*    height                                Display height in pixels      */
/*    display_control_block_size            Size of the display control   */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_create                    Actual display create         */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_display_create(GX_DISPLAY *display, const GX_CHAR *name,
                          UINT (*display_driver_setup)(GX_DISPLAY *),
                          GX_VALUE width, GX_VALUE height, UINT display_control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((display == GX_NULL) || (display_driver_setup == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (display_control_block_size != sizeof(GX_DISPLAY))
    {
        return(GX_INVALID_SIZE);
    }

    if (display->id == GX_DISPLAY_ID)
    {
        return (GX_ALREADY_CREATED);
    }

    /* Call actual window create function.  */
    status = _gx_display_create(display, name, display_driver_setup, width, height);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_display_delete                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the display delete function      */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
/*    display_driver_cleanup                Display driver cleanup        */
/*                                            function                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_delete                    Actual display delete         */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_display_delete(GX_DISPLAY *display, void (*display_driver_cleanup)(GX_DISPLAY *))
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((display == GX_NULL) || (display_driver_cleanup == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual window create function.  */
    status = _gx_display_delete(display, display_driver_cleanup);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_display_font_table_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system font table set call.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
/*    font_table                            Pointer to font table         */
/*    number_of_fonts                       Number of fonts in the table  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_font_table_set            Actual display font table set */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_display_font_table_set(GX_DISPLAY *display, GX_FONT **font_table, UINT number_of_fonts)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (((number_of_fonts != 0) && (font_table == GX_NULL)) || (display == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual system font table set.  */
    status = _gx_display_font_table_set(display, font_table, number_of_fonts);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_display_language_table_get_ext                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in display language table get call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Pointer to display instance.  */
/*    table                                 Pointer to string table       */
/*    language_count                        Number of languages in table  */
/*    string_count                          Number of strings in each     */
/*                                            language                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_language_table_get_ext    Actual display language table */
/*                                            get ext call                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_display_language_table_get_ext(GX_DISPLAY *display, GX_STRING ***table, GX_UBYTE *language_count, UINT *string_count)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if (display == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Call actual system language table get routine. */
    status = _gx_display_language_table_get_ext(display, table, language_count, string_count);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_display_language_table_set_ext                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in display language table set call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Pointer to display instance.  */
/*    table                                 The language table to be set  */
/*    num_languages                         Number of languages in the    */
/*                                            table                       */
/*    number_of_strings                     Number of strings in each     */
/*                                            language                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_language_table_set_ext    Actual display language table */
/*                                            set ext call                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_display_language_table_set_ext(GX_DISPLAY *display, const GX_STRING **table, GX_UBYTE num_languages, UINT number_of_strings)
{
UINT                status;
UINT                language;
UINT                string_id;
const GX_STRING *string_table;
const GX_STRING *string;
UINT                string_length;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (display == GX_NULL)
    {
        /* Check for null pointer*/
        return(GX_PTR_ERROR);
    }

    /* Allow string table to be NULL if number_of_strings is zero. */
    if (number_of_strings)
    {
        if (table == GX_NULL)
        {
            /* Return error.  */
            return(GX_PTR_ERROR);
        }

        for (language = 0; language < num_languages; language++)
        {
            string_table = table[language];
            for (string_id = 0; string_id < number_of_strings; string_id++)
            {
                string = &string_table[string_id];

                if (string->ptr)
                {
                    status = _gx_utility_string_length_check(string->ptr, &string_length, string->length);

                    if (status != GX_SUCCESS)
                    {
                        return status;
                    }
                }
                else
                {
                    string_length = 0;
                }

                if (string_length != string->length)
                {
                    return GX_INVALID_STRING_LENGTH;
                }
            }
        }
    }

    /* Call actual system string table set.  */
    status = _gx_display_language_table_set_ext(display, table, num_languages, number_of_strings);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_display_language_direction_table_set                           */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in display language direction table */
/*    set call.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Pointer to display instance.  */
/*    language_direction_table              The language direction table  */
/*                                            to be set                   */
/*    num_languages                         Number of languages in the    */
/*                                            table                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_language_direciton_table_set                            */
/*                                          Actual display language table */
/*                                            set ext call                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT _gxe_display_language_direction_table_set(GX_DISPLAY *display, const GX_UBYTE *language_direction_table, GX_UBYTE num_languages)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (display == GX_NULL)
    {
        /* Check for null pointer*/
        return(GX_PTR_ERROR);
    }

    /* Call actual system string table set.  */
    status = _gx_display_language_direction_table_set(display, language_direction_table, num_languages);

    /* Return status.  */
    return(status);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_display_pixelmap_table_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in system pixelmap table set call.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
/*    pixelmap_table                        Pixelmap table                */
/*    number_of_pixelmaps                   Number of pixelmaps in table  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_pixelmap_table_set        Actual display pixelmap table */
/*                                            set call.                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_display_pixelmap_table_set(GX_DISPLAY *display, GX_PIXELMAP **pixelmap_table, UINT number_of_pixelmaps)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (((number_of_pixelmaps != 0) && (pixelmap_table == GX_NULL)) || (display == GX_NULL))
    {
        /* Return error.  */
        return(GX_PTR_ERROR);
    }

    /* Call actual system pixelmap table set.  */
    status = _gx_display_pixelmap_table_set(display, pixelmap_table, number_of_pixelmaps);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_display_string_get_ext                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in display_string_get call.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    string_id                             String resource ID            */
/*    return_string                         Pointer to return string      */
/*                                            pointer                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_string_get                 Actual system string get call */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_display_string_get_ext(GX_DISPLAY *display, GX_RESOURCE_ID string_id, GX_STRING *return_string)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if (display == GX_NULL || return_string == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call actual system string get.  */
    status = _gx_display_string_get_ext(display, string_id, return_string);

    /* Return status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_display_string_table_get_ext                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the display string table get     */
/*    function call                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Pointer to display instance.  */
/*    language                              The language the string       */
/*                                            table is associated with    */
/*    get_table                             Pointer to string table       */
/*    get_size                              Number of strings in table    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_string_table_get         Actual display string table    */
/*                                            get routine                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_display_string_table_get_ext(GX_DISPLAY *display, GX_UBYTE language, GX_STRING **put_table, UINT *put_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (display == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (language >= display->language_table_size)
    {
        return GX_NOT_FOUND;
    }

    /* Call actual string table get function */
    status = _gx_display_string_table_get_ext(display, language,
                                              put_table, put_size);
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_display_theme_install                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in display theme install call.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
/*    theme_table                           Theme table to be installed   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_theme_install             Actual theme install call     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default draw funtions                                          */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_display_theme_install(GX_DISPLAY *display, const GX_THEME *theme_ptr)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if (display == GX_NULL)
    {
        return GX_INVALID_DISPLAY;
    }

    if (theme_ptr == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    status = _gx_display_theme_install(display, theme_ptr);

    /* Return status.  */
    return(status);
}
