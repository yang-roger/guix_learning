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

#include "gx_canvas.h"
#include "gx_utility.h"
#include "gx_system.h"
#include "gx_window_root.h"
#include "gx_theme.h"

static inline bool gx_root_window_in_display_(const GX_WINDOW_ROOT* root, const GX_DISPLAY* display)
{
    return root->canvas && root->canvas->display == display;
}

static void gx_display_root_window_event_process_(GX_DISPLAY* display, GX_EVENT* event)
{
    GX_WINDOW_ROOT* root = _gx_system_root_window_created_list;
    while (root)
    {
        if (gx_root_window_in_display_(root, display))
        {
            root->event_process_function_run_(event);
        }

        root = (GX_WINDOW_ROOT*)root->next;
    }
}

static void gx_display_language_changed_(GX_DISPLAY* display)
{
    GX_EVENT e;
    memset(&e, 0, sizeof(GX_EVENT));
    e.type = GX_EVENT_LANGUAGE_CHANGE;

    gx_display_root_window_event_process_(display, &e);

    _gx_system_all_canvas_dirty();
}

UINT GX_DISPLAY::create_(const GX_CHAR* name,
                         UINT (*display_driver_setup)(GX_DISPLAY*),
                         GX_VALUE width, GX_VALUE height)
{
    memset(this, 0, sizeof(GX_DISPLAY));

    this->width = width;
    this->height = height;

    // Call the display driver setup function. This function initializes the underlying
    //   hardware and sets up all the primitive drawing function pointers.
    UINT result = display_driver_setup(this);

    // Determine if the display driver setup was successful.
    if (result)
    {
        /* Error setting up display driver - call system error handler.  */
        _gx_system_error_process(GX_SYSTEM_DRIVER_SETUP_ERROR);

        return GX_SYSTEM_ERROR;
    }

    this->id = GX_DISPLAY_ID;
    this->name = name;

    // Place the display on the list of created displays.

    _gx_system_display_created_count++;

    if (_gx_system_display_created_list)
    {
        _gx_system_display_created_list->created_previous = this;
        this->created_next = _gx_system_display_created_list;
    }

    _gx_system_display_created_list = this;

    return GX_SUCCESS;
}

UINT GX_DISPLAY::delete_(void (*display_driver_cleanup)(GX_DISPLAY*))
{
    if (_gx_system_display_created_count > 0)
    {
        display_driver_cleanup(this);
        --_gx_system_display_created_count;

        if (created_previous)
        {
            created_previous->created_next = created_next;
        }
        else
        {
            _gx_system_display_created_list = created_next;
        }

        if (created_next)
        {
            created_next->created_previous = created_previous;
        }

        memset(this, 0, sizeof(GX_DISPLAY));

        return GX_SUCCESS;
    }

    return GX_FAILURE;
}

UINT GX_DISPLAY::color_get_(GX_RESOURCE_ID color_id, GX_COLOR* return_color)
{
    if (color_id < color_table_size && color_table)
    {
        *return_color = color_table[color_id];
        return GX_SUCCESS;
    }
    else
    {
        *return_color = 0;
        return GX_INVALID_RESOURCE_ID;
    }
}

UINT GX_DISPLAY::color_set_(GX_RESOURCE_ID color_id, GX_COLOR color)
{
    color_table[color_id] = color;

    canvas_dirty_();

    return GX_SUCCESS;
}

UINT GX_DISPLAY::color_table_set_(GX_COLOR* color_table, INT number_of_colors, bool dirty)
{
    this->color_table = color_table;
    this->color_table_size = number_of_colors;

    if (dirty)
    {
        canvas_dirty_();
    }

    return GX_SUCCESS;
}

UINT GX_DISPLAY::pixelmap_get_(GX_RESOURCE_ID pixelmap_id, GX_PIXELMAP** return_pixelmap)
{
    if (pixelmap_id < pixelmap_table_size && pixelmap_table)
    {
        *return_pixelmap = pixelmap_table[pixelmap_id];
        return GX_SUCCESS;
    }
    else
    {
        *return_pixelmap = GX_NULL;
        return GX_INVALID_RESOURCE_ID;
    }
}

UINT GX_DISPLAY::pixelmap_table_set_(GX_PIXELMAP** pixelmap_table, UINT number_of_pixelmaps, bool dirty)
{
    this->pixelmap_table = pixelmap_table;
    this->pixelmap_table_size = number_of_pixelmaps;

    if (dirty)
    {
        canvas_dirty_();
    }

    return GX_SUCCESS;
}

UINT GX_DISPLAY::font_get_(GX_RESOURCE_ID font_id, GX_FONT** return_font)
{
    if (font_id < font_table_size && font_table)
    {
        *return_font = font_table[font_id];
        return GX_SUCCESS;
    }
    else
    {
        *return_font = GX_NULL;
        return GX_INVALID_RESOURCE_ID;
    }
}

UINT GX_DISPLAY::font_table_set_(GX_FONT** font_table, UINT number_of_fonts, bool dirty)
{
    this->font_table = font_table;
    this->font_table_size = number_of_fonts;

    if (dirty)
    {
        canvas_dirty_();
    }

    return GX_SUCCESS;
}

UINT GX_DISPLAY::language_table_get_ext_(GX_STRING*** table, GX_UBYTE* language_count, UINT* string_count)
{
    if (table)
    {
        *table = (GX_STRING**)this->language_table;
    }

    if (language_count)
    {
        *language_count = this->language_table_size;
    }

    if (string_count)
    {
        *string_count = this->string_table_size;
    }

    return GX_SUCCESS;
}

UINT GX_DISPLAY::language_table_set_ext_(const GX_STRING** table, GX_UBYTE num_languages, UINT number_of_strings)
{
    const GX_STRING** old_table = this->language_table;

    this->language_table = table;
    this->language_table_size = num_languages;
    this->string_table_size = number_of_strings;

    if (old_table)
    {
        gx_display_language_changed_(this);
    }

    return GX_SUCCESS;
}

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)

UINT GX_DISPLAY::language_direction_get_(GX_UBYTE language, GX_UBYTE* language_direction)
{
    GX_UBYTE direction = GX_LANGUAGE_DIRECTION_LTR;

    if (language < language_table_size && language_direction_table)
    {
        direction = language_direction_table[language];
    }

    *language_direction = direction;

    return GX_SUCCESS;
}

UINT GX_DISPLAY::language_direction_table_set_(const GX_UBYTE* language_direction_table, GX_UBYTE num_languages)
{
    if (num_languages != this->language_table_size)
    {
        return GX_INVALID_VALUE;
    }

    this->language_direction_table = language_direction_table;

    return GX_SUCCESS;
}

#endif // GX_DYNAMIC_BIDI_TEXT_SUPPORT

UINT GX_DISPLAY::string_get_ext_(GX_RESOURCE_ID string_id, GX_STRING* return_string)
{
    UINT result = GX_SUCCESS;
    GX_STRING string;

    string.ptr = GX_NULL;
    string.length = 0;

    if (string_id >= string_table_size)
    {
        result = GX_INVALID_RESOURCE_ID;
    }
    else if (string_id > 0)
    {
        if (language_table)
        {
            string = language_table[active_language][string_id];
            if (!string.ptr && active_language)
            {
                string = language_table[0][string_id];
            }
        }
    }

    *return_string = string;

    return result;
}

UINT GX_DISPLAY::string_table_get_ext_(GX_UBYTE language, GX_STRING** table, UINT* size)
{
    if (table)
    {
        if (language_table)
        {
            *table = (GX_STRING*)language_table[language];
        }
        else
        {
            *table = GX_NULL;
        }
    }

    if (size)
    {
        *size = string_table_size;
    }

    return GX_SUCCESS;
}

UINT GX_DISPLAY::theme_install_(const GX_THEME* theme)
{
    color_table_set_(theme->color_table, theme->color_table_size, false);

    if (theme->palette && driver_palette_set)
    {
        driver_palette_set(this, theme->palette, theme->palette_size);
    }

    pixelmap_table_set_(theme->pixelmap_table, theme->pixelmap_table_size, false);
    font_table_set_(theme->font_table, theme->font_table_size, false);

    _gx_system_scroll_appearance_set(theme->horizontal_scroll_style,
                                     &theme->horizontal_scrollbar_appearance);

    _gx_system_scroll_appearance_set(theme->vertical_scroll_style,
                                     &theme->vertical_scrollbar_appearance);

    canvas_dirty_();

    return GX_SUCCESS;
}

UINT GX_DISPLAY::active_language_set_(GX_UBYTE language)
{
    active_language = language;

    gx_display_language_changed_(this);

    return GX_SUCCESS;
}

void GX_DISPLAY::canvas_dirty_()
{
    GX_WINDOW_ROOT* root = _gx_system_root_window_created_list;
    while (root)
    {
        if ((root->status & GX_STATUS_VISIBLE) &&
            gx_root_window_in_display_(root, this))
        {
            _gx_system_dirty_mark(root);

            for (GX_WIDGET* win = root->first_child; win; win = win->next)
            {
                _gx_system_dirty_mark(win);
            }
        }

        root = (GX_WINDOW_ROOT*)root->next;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_active_language_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the active language.                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Pointer to display instance.  */
/*    language                              Language table id             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_all_canvas_dirty           Mark all canvas dirty         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_display_active_language_set(GX_DISPLAY* display, GX_UBYTE language)
{
    return display->active_language_set_(language);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_create                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates the display and calls the display driver      */
/*    setup function.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
/*    name                                  Name of display               */
/*    display_driver_setup                  Display driver setup function */
/*    width                                 Display width in pixels       */
/*    height                                Display height in pixels      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memset                                Set control block to zero     */
/*    _gx_system_error_process              System error handler          */
/*    [dispaly_driver_setup]                Call display driver setup     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_display_create(GX_DISPLAY* display, const GX_CHAR* name,
                        UINT (*display_driver_setup)(GX_DISPLAY*),
                        GX_VALUE width, GX_VALUE height)
{
    return display->create_(name, display_driver_setup, width, height);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_delete                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deletes a previous-created display                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
/*    display_driver_cleanup                Display driver cleanup routine*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [display_driver_cleanup]              Call the user-supplied        */
/*                                            display driver cleanup      */
/*                                            routine                     */
/*    memset                                Cleanup the memory            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_display_delete(GX_DISPLAY* display, void (*display_driver_cleanup)(GX_DISPLAY*))
{
    return display->delete_(display_driver_cleanup);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_canvas_dirty                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function marks all root windows as dirty. This is done when    */
/*    the system resources are changed so that we re-draw everything.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*     _gx_system_dirty_mark                Mark the widget dirty         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_display_color_set                                               */
/*    _gx_display_color_table_set                                         */
/*    _gx_display_font_table_set                                          */
/*    _gx_display_pixelmap_table_set                                      */
/*                                                                        */
/**************************************************************************/
void _gx_display_canvas_dirty(GX_DISPLAY* display)
{
    display->canvas_dirty_();
}

/**************************************************************************/

UINT _gx_display_color_get(GX_DISPLAY* display, GX_RESOURCE_ID id, GX_COLOR* return_color)
{
    return display->color_get_(id, return_color);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_color_set                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the supplied resource ID to the specified         */
/*    color in the system color table.                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
/*    resrouce_id                           Resource ID for color         */
/*    new_color                             New color                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_canvas_dirty              Mark all root windows as dirty*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default draw funtions                                          */
/*                                                                        */
/**************************************************************************/
UINT _gx_display_color_set(GX_DISPLAY* display, GX_RESOURCE_ID id, GX_COLOR color)
{
    return display->color_set_(id, color);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_color_table_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the color table.                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
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
/*    _gx_display_canvas_dirty              Mark all root windows as dirty*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX default draw funtions                                          */
/*                                                                        */
/**************************************************************************/
UINT _gx_display_color_table_set(GX_DISPLAY* display, GX_COLOR* color_table, INT number_of_colors)
{
    return display->color_table_set_(color_table, number_of_colors);
}

/**************************************************************************/

UINT _gx_display_font_get(GX_DISPLAY* display, GX_RESOURCE_ID id, GX_FONT** return_font)
{
    return display->font_get_(id, return_font);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_font_table_set                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the font table.                                   */
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
/*    _gx_display_canvas_dirty              Mark all root windows as dirty*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_display_font_table_set(GX_DISPLAY* display, GX_FONT** font_table, UINT number_of_fonts)
{
    return display->font_table_set_(font_table, number_of_fonts);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_language_table_get_ext                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns a pointer to the display language table.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               pointer to display            */
/*    language_table                        Pointer to string table       */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_display_language_table_get_ext(GX_DISPLAY* display, GX_STRING*** table, GX_UBYTE* language_count, UINT* string_count)
{
    return display->language_table_get_ext_(table, language_count, string_count);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_language_table_set_ext                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the language table.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    language_table                        The language table to be set  */
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
/*    _gx_system_all_canvas_dirty           Mark canvas dirty             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_display_language_table_set_ext(GX_DISPLAY* display, const GX_STRING** table, GX_UBYTE num_languages, UINT number_of_strings)
{
    return display->language_table_set_ext_(table, num_languages, number_of_strings);
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_language_direction_table_set_ext                        */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the language direction table.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Pointer to display            */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT _gx_display_language_direction_table_set(GX_DISPLAY* display, const GX_UBYTE* language_direction_table, GX_UBYTE num_languages)
{
    return display->language_direction_table_set_(language_direction_table, num_languages);
}
#endif // GX_DYNAMIC_BIDI_TEXT_SUPPORT

/**************************************************************************/

UINT _gx_display_pixelmap_get(GX_DISPLAY* display, GX_RESOURCE_ID id, GX_PIXELMAP** return_pixelmap)
{
    return display->pixelmap_get_(id, return_pixelmap);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_pixelmap_table_set                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the pixelmap table.                               */
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
/*    _gx_display_canvas_dirty              Mark all root windows as dirty*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_display_pixelmap_table_set(GX_DISPLAY* display, GX_PIXELMAP** pixelmap_table, UINT number_of_pixelmaps)
{
    return display->pixelmap_table_set_(pixelmap_table, number_of_pixelmaps);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_string_get_ext                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service return the string associated with the specified string */
/*    ID.                                                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Pointer to display instance   */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    _gx_context_string_get                                              */
/*    _gx_widget_string_get                 Drew the text onto the wdiget */
/*                                                                        */
/**************************************************************************/
UINT _gx_display_string_get_ext(GX_DISPLAY* display, GX_RESOURCE_ID string_id, GX_STRING* return_string)
{
    return display->string_get_ext_(string_id, return_string);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_string_table_get_ext                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns a pointer to the display string table.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Pointer to display instance.  */
/*    language                              The language the string       */
/*                                            table is associated with    */
/*    put_table                             Pointer to string table       */
/*    put_size                              Number of strings in table    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_display_string_table_get_ext(GX_DISPLAY* display, GX_UBYTE language, GX_STRING** table, UINT* size)
{
    return display->string_table_get_ext_(language, table, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_theme_install                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service installs themes to the specified display.              */
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
/*    _gx_display_canvas_dirty              Mark all root windows as dirty*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_display_theme_install(GX_DISPLAY* display, const GX_THEME* theme_ptr)
{
    return display->theme_install_(theme_ptr);
}
