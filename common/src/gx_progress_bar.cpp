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
/**   Progress Bar Management (Progress Bar)                              */
/**                                                                       */
/**************************************************************************/

#include "gx_progress_bar.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_canvas.h"

void GX_PROGRESS_BAR_INFO::default_init_()
{
    min_val = 0;
    max_val = 100;
    current_val = 0;
    font_id = GX_FONT_ID_DEFAULT;
    normal_text_color = GX_COLOR_ID_TEXT;
    selected_text_color = GX_COLOR_ID_SELECTED_TEXT;
    disabled_text_color = GX_COLOR_ID_DISABLED_TEXT;
    fill_pixelmap = GX_PIXELMAP_NULL;
}

static void gx_progress_bar_draw_(GX_PROGRESS_BAR* progress_bar)
{
    progress_bar->draw_();
}

static UINT gx_progress_bar_event_process_(GX_PROGRESS_BAR* progress_bar, GX_EVENT* event_ptr)
{
    return progress_bar->event_process_(event_ptr);
}

UINT GX_PROGRESS_BAR::create_(const GX_CHAR* name, GX_WIDGET* parent,
                              const GX_PROGRESS_BAR_INFO* progress_bar_info, ULONG style,
                              USHORT progress_bar_id, const GX_RECTANGLE* size)
{
    GX_WIDGET::create_(name, GX_NULL, style, progress_bar_id, size);

    this->type = GX_TYPE_PROGRESS_BAR;

    if (progress_bar_info)
    {
        this->info = *progress_bar_info;
    }
    else
    {
        this->info.default_init_();
    }

    this->draw_function = (void (*)(GX_WIDGET*))gx_progress_bar_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_progress_bar_event_process_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_PROGRESS_BAR::info_set_(const GX_PROGRESS_BAR_INFO* info)
{
    this->info = *info;

    dirty_mark_if_visible_();
}

void GX_PROGRESS_BAR::range_set_(INT min_value, INT max_value)
{
    info.min_val = min_value;
    info.max_val = max_value;

    dirty_mark_if_visible_();
}

void GX_PROGRESS_BAR::value_set_(INT new_value)
{
    if (new_value < info.min_val)
    {
        new_value = info.min_val;
    }
    else if (new_value > info.max_val)
    {
        new_value = info.max_val;
    }

    if (info.current_val != new_value)
    {
        info.current_val = new_value;

        dirty_mark_if_visible_();

        signal_parent_(GX_EVENT_PROGRESS_VALUE, new_value);
    }
}

void GX_PROGRESS_BAR::font_set_(GX_RESOURCE_ID font_id)
{
    info.font_id = font_id;

    dirty_mark_if_visible_();
}

void GX_PROGRESS_BAR::text_color_set_(GX_RESOURCE_ID normal_text_color_id,
                                      GX_RESOURCE_ID selected_text_color_id,
                                      GX_RESOURCE_ID disabled_text_color_id)
{
    info.normal_text_color = normal_text_color_id;
    info.selected_text_color = selected_text_color_id;
    info.disabled_text_color = disabled_text_color_id;

    dirty_mark_if_visible_();
}

void GX_PROGRESS_BAR::pixelmap_set_(GX_RESOURCE_ID pixelmap)
{
    info.fill_pixelmap = pixelmap;

    dirty_mark_if_visible_();
}

void GX_PROGRESS_BAR::fill_rect_get_(GX_RECTANGLE* fill_rect, INT current_range, INT range) const
{
    if (style & GX_STYLE_PROGRESS_VERTICAL)
    {
        fill_rect->top = (GX_VALUE)(fill_rect->bottom - fill_rect->height_() * current_range / range + 1);
    }
    else
    {
        fill_rect->right = (GX_VALUE)(fill_rect->left + fill_rect->width_() * current_range / range - 1);
    }
}

void GX_PROGRESS_BAR::background_draw_()
{
    GX_RESOURCE_ID fill_color = is_enabled_() ? normal_fill_color : disabled_fill_color;
    border_and_background_draw_(GX_COLOR_ID_DEFAULT_BORDER, fill_color, fill_color, GX_TRUE);

    INT range = info.max_val - info.min_val;
    if (range <= 0)
    {
        return;
    }

    INT cur_range = info.current_val - info.min_val;

    GX_RECTANGLE fill_rect;
    client_area_get_(&fill_rect);

    if (info.fill_pixelmap)
    {
        GX_PIXELMAP* map;
        _gx_context_pixelmap_get(info.fill_pixelmap, &map);
        if (map)
        {
            fill_rect_get_(&fill_rect, cur_range, range);
            _gx_canvas_pixelmap_tile(&fill_rect, map);
        }
    }
    else
    {
        _gx_context_fill_color_set(selected_fill_color);

        if (style & GX_STYLE_PROGRESS_SEGMENTED_FILL)
        {
            const INT GX_SEGMENT_BORDER_INTERVAL = 1;
            const INT GX_SEGMENT_INTERVAL = 1;

            INT total_segment;
            INT segment_number;
            INT i;

            if (style & GX_STYLE_PROGRESS_VERTICAL)
            {
                INT segment_height = width_() / 2;
                total_segment = (fill_rect.height_() - GX_SEGMENT_INTERVAL) / (segment_height + GX_SEGMENT_INTERVAL);
                segment_number = total_segment * cur_range / range;

                fill_rect.left = (GX_VALUE)(fill_rect.left + GX_SEGMENT_BORDER_INTERVAL);
                fill_rect.right = (GX_VALUE)(fill_rect.right - GX_SEGMENT_BORDER_INTERVAL);
                fill_rect.top = (GX_VALUE)(fill_rect.bottom - segment_height + 1);

                for (i = 1; i <= segment_number; ++i)
                {
                    _gx_canvas_rectangle_draw(&fill_rect);
                    fill_rect.move_(0, (GX_VALUE)(-(segment_height + GX_SEGMENT_INTERVAL)));
                }
            }
            else
            {
                INT segment_width = height_() / 2;
                total_segment = (fill_rect.width_() - GX_SEGMENT_INTERVAL) / (segment_width + GX_SEGMENT_INTERVAL);
                segment_number = total_segment * cur_range / range;

                fill_rect.top = (GX_VALUE)(fill_rect.top + GX_SEGMENT_BORDER_INTERVAL);
                fill_rect.bottom = (GX_VALUE)(fill_rect.bottom - GX_SEGMENT_BORDER_INTERVAL);
                fill_rect.right = (GX_VALUE)(fill_rect.left + segment_width - 1);

                for (i = 1; i <= segment_number; ++i)
                {
                    _gx_canvas_rectangle_draw(&fill_rect);
                    fill_rect.move_((GX_VALUE)(segment_width + GX_SEGMENT_INTERVAL), 0);
                }
            }
        }
        else
        {
            fill_rect_get_(&fill_rect, cur_range, range);
            _gx_canvas_rectangle_draw(&fill_rect);
        }
    }
}

void GX_PROGRESS_BAR::text_draw_()
{
    GX_RESOURCE_ID color;

    if (is_enabled_())
    {
        if (style & GX_STYLE_DRAW_SELECTED)
        {
            color = info.selected_text_color;
        }
        else
        {
            color = info.normal_text_color;
        }
    }
    else
    {
        color = info.disabled_text_color;
    }

    const UINT GX_MAX_PROGRESS_BAR_TEXT_LENGTH = 12;
    GX_CHAR text[GX_MAX_PROGRESS_BAR_TEXT_LENGTH + 1] = "";

    GX_STRING string;

    string.ptr = text;
    string.length = _gx_utility_value_or_percent_string_get((style & GX_STYLE_PROGRESS_PERCENT),
                                                            info.current_val - info.min_val,
                                                            info.max_val - info.min_val,
                                                            text, sizeof(text));

    text_draw_ext_(color, info.font_id, &string, 0, 0);
}

void GX_PROGRESS_BAR::draw_()
{
    background_draw_();

    if (style & GX_STYLE_PROGRESS_TEXT_DRAW)
    {
        text_draw_();
    }

    children_draw_();
}

UINT GX_PROGRESS_BAR::event_process_(GX_EVENT* event_ptr)
{
    return GX_WIDGET::event_process_(event_ptr);
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_progress_bar_background_draw                    PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws the background of the specified progress bar.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_border_draw                Draw the widget border        */
/*    _gx_context_fill_color_set            Set fill color                */
/*    _gx_context_pixelmap_get              Retrieve pixelmap by ID       */
/*    _gx_widget_client_get                 Get widget client area        */
/*    _gx_widget_width_get                  Get widget width              */
/*    _gx_widget_height_get                 Get widget height             */
/*    _gx_canvas_rectangle_draw             Draw a rectangle on canvas    */
/*    _gx_canvas_pixelmap_tile              Tile a pixelmap on cavnas     */
/*    _gx_utility_rectangle_shift           Shift a rectangle by specified*/
/*                                            x,y values                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_progress_bar_background_draw(GX_PROGRESS_BAR* progress_bar)
{
    progress_bar->background_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_progress_bar_create                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service creates a progress bar.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*    name                                  Name of prompt                */
/*    parent                                Parent widget control block   */
/*    progress_bar_info                     Pointer to progress bar info  */
/*    style                                 Style of progress bar         */
/*    progress_bar_id                       Application-defined ID of     */
/*                                            progress bar                */
/*    size                                  Dimensions of progress bar    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_create                     Create the underlying widget  */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_progress_bar_create(GX_PROGRESS_BAR* progress_bar, const GX_CHAR* name, GX_WIDGET* parent,
                             GX_PROGRESS_BAR_INFO* progress_bar_info, ULONG style,
                             USHORT progress_bar_id, const GX_RECTANGLE* size)
{
    return progress_bar->create_(name, parent, progress_bar_info, style, progress_bar_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_progress_bar_draw                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws the specified progress bar.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_border_draw                Draw border                   */
/*    _gx_context_pixelmap_get              Retrieve pixelmap image       */
/*    _gx_widget_client_get                 Find the client area of a     */
/*                                            widget                      */
/*    _gx_canvas_pixelmap_tile              Tile a pixelmap               */
/*    _gx_widget_width_get                  Find width of a widget        */
/*    _gx_context_fill_color_set            Set the draw context fill     */
/*                                            color                       */
/*    _gx_widget_children_draw              Draw children widgets         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_progress_bar_draw(GX_PROGRESS_BAR* progress_bar)
{
    progress_bar->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_progress_bar_event_process                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service processes a progress bar event.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*    event_ptr                             Pointer to event process      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_process              Default widget event process  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_progress_bar_event_process(GX_PROGRESS_BAR* progress_bar, GX_EVENT* event_ptr)
{
    return progress_bar->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_progress_bar_font_set                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the font of a progress bar widget.                */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*    font_id                               Resource ID of font           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark this progress bar as     */
/*                                            dirty                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_progress_bar_font_set(GX_PROGRESS_BAR* progress_bar, GX_RESOURCE_ID font_id)
{
    progress_bar->font_set_(font_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_progress_bar_info_set                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reset the information structure of a GX_PROGRESS_BAR  */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Widget control block          */
/*    info                                  Slider info block             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_progress_bar_info_set(GX_PROGRESS_BAR* progress_bar, GX_PROGRESS_BAR_INFO* info)
{
    progress_bar->info_set_(info);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_progress_bar_pixelmap_set                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the pixelmap used to fill the progress bar.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*    pixelmap                              Resource ID of the fill       */
/*                                             pixelmap                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_progress_bar_pixelmap_set(GX_PROGRESS_BAR* progress_bar, GX_RESOURCE_ID pixelmap)
{
    progress_bar->pixelmap_set_(pixelmap);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_progress_bar_range_set                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the progress bar value range.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*    min_value                             Minmum value                  */
/*    max_value                             Maxmum value                  */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_progress_bar_range_set(GX_PROGRESS_BAR* progress_bar, INT min_value, INT max_value)
{
    progress_bar->range_set_(min_value, max_value);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_progress_bar_text_color_set                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the text color of a progress bar widget.          */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*    normal_text_color_id                  Resource ID of the normal     */
/*                                             text color                 */
/*    selected_text_color_id                Resource ID of the selected   */
/*                                             text color                 */
/*    disabled_text_color_id                Resource ID of the disabled   */
/*                                             text color                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the progress bar as      */
/*                                            dirty                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_progress_bar_text_color_set(GX_PROGRESS_BAR* progress_bar,
                                     GX_RESOURCE_ID normal_text_color_id,
                                     GX_RESOURCE_ID selected_text_color_id,
                                     GX_RESOURCE_ID disabled_text_color_id)
{
    progress_bar->text_color_set_(normal_text_color_id,selected_text_color_id,disabled_text_color_id);

    return GX_SUCCESS;
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_progress_bar_text_draw                          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified progress bar with text.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_ltoa                      Converts a long integer value */
/*                                            into an ASCII string        */
/*    _gx_utility_string_length_check       Test string length            */
/*    _gx_widget_text_draw                  Draw the text on the widget   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_progress_bar_text_draw(GX_PROGRESS_BAR* progress_bar)
{
    progress_bar->text_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_progress_bar_value_set                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the progress bar value.                           */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*    new_value                             New progress bar value        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_progress_bar_value_set(GX_PROGRESS_BAR* progress_bar, INT new_value)
{
    progress_bar->value_set_(new_value);

    return GX_SUCCESS;
}
