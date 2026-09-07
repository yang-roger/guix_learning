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
/**   Slider Management (Slider)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_pixelmap_slider.h"

#include "gx_context.h"
#include "gx_canvas.h"
#include "gx_pixelmap.h"

static void gx_pixelmap_slider_draw_(GX_PIXELMAP_SLIDER* slider)
{
    slider->draw_();
}

static UINT gx_pixelmap_slider_event_process_(GX_PIXELMAP_SLIDER* slider, GX_EVENT* event_ptr)
{
    return slider->event_process_(event_ptr);
}

UINT GX_PIXELMAP_SLIDER::create_(const GX_CHAR* name, GX_WIDGET* parent,
                                 const GX_SLIDER_INFO* info,
                                 const GX_PIXELMAP_SLIDER_INFO* pixelmap_info,
                                 ULONG style, USHORT pixelmap_slider_id,
                                 const GX_RECTANGLE* size)
{
    GX_WIDGET::create_(name, GX_NULL, style, pixelmap_slider_id, size);

    this->type = GX_TYPE_PIXELMAP_SLIDER;

    this->info = *info;
    this->pixelmap_info = *pixelmap_info;

    this->info.needle_width = 0;
    this->info.needle_height = 0;

    this->draw_function = (void (*)(GX_WIDGET*))gx_pixelmap_slider_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_pixelmap_slider_event_process_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_PIXELMAP_SLIDER::pixelmap_set_(const GX_PIXELMAP_SLIDER_INFO* info)
{
    pixelmap_info = *info;

    if (is_visible_())
    {
        pixelmap_update_();
        dirty_mark_();
    }
}

void GX_PIXELMAP_SLIDER::pixelmap_update_()
{
    GX_PIXELMAP* pixelmap;

    if (pixelmap_info.needle_pixelmap)
    {
        pixelmap_get_(pixelmap_info.needle_pixelmap, &pixelmap);
        if (pixelmap)
        {
            info.needle_width = pixelmap->width;
            info.needle_height = pixelmap->height;

            if (PIXELMAP_IS_TRANSPARENT(pixelmap))
            {
                status_add_(GX_STATUS_TRANSPARENT);
            }
        }
    }

    if (pixelmap_info.lower_background_pixelmap)
    {
        if (pixelmap_is_transparent_(pixelmap_info.lower_background_pixelmap))
        {
            status_add_(GX_STATUS_TRANSPARENT);
        }
    }
}

void GX_PIXELMAP_SLIDER::draw_()
{
    GX_DRAW_CONTEXT* context = gx_context_current_();

    // draw background

    GX_PIXELMAP* map;
    GX_PIXELMAP* dark_map;

    context->pixelmap_get_(pixelmap_info.lower_background_pixelmap, &map);
    context->pixelmap_get_(pixelmap_info.upper_background_pixelmap, &dark_map);
    context->fill_color_set_(fill_color_());

    GX_RECTANGLE needle_size;
    needle_position_get_(&needle_size);

    GX_RECTANGLE fillrect;

    if ((style & GX_STYLE_TILE_BACKGROUND) && map && dark_map)
    {
        if (style & GX_STYLE_SLIDER_VERTICAL)
        {
            fillrect.left = (GX_VALUE)(left_() + (width_() - map->width) / 2);
            fillrect.right = (GX_VALUE)(fillrect.left + map->width - 1);

            fillrect.top = top_();
            fillrect.bottom = (GX_VALUE)(needle_size.top + info.needle_hotspot_offset);
            _gx_canvas_pixelmap_tile(&fillrect, dark_map);

            fillrect.top = (GX_VALUE)(fillrect.bottom + 1);
            fillrect.bottom = bottom_();
            _gx_canvas_pixelmap_tile(&fillrect, map);
        }
        else
        {
            fillrect.top = (GX_VALUE)(top_() + (height_() - map->height) / 2);
            fillrect.bottom = (GX_VALUE)(fillrect.top + map->height - 1);

            fillrect.left = left_();
            fillrect.right = (GX_VALUE)(needle_size.left + info.needle_hotspot_offset);
            _gx_canvas_pixelmap_tile(&fillrect, map);

            fillrect.left = (GX_VALUE)(fillrect.right + 1);
            fillrect.right = right_();
            _gx_canvas_pixelmap_tile(&fillrect, dark_map);
        }
    }
    else
    {
        if (map)
        {
            if (style & GX_STYLE_TILE_BACKGROUND)
            {
                _gx_canvas_pixelmap_tile(&size, map);
            }
            else
            {
                _gx_canvas_pixelmap_draw(left_(), top_(), map);
            }
        }
        else
        {
            if (!(style & GX_STYLE_TRANSPARENT))
            {
                border_and_background_draw_();
            }
        }
    }

    // draw needle

    context->pixelmap_get_(pixelmap_info.needle_pixelmap, &map);
    if (map)
    {
        _gx_canvas_pixelmap_draw(needle_size.left, needle_size.top, map);
    }

    children_draw_();
}

UINT GX_PIXELMAP_SLIDER::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        result = GX_WIDGET::event_process_(event_ptr);
        pixelmap_update_();
        break;

    case GX_EVENT_PEN_DOWN:
    case GX_EVENT_PEN_UP:
    case GX_EVENT_PEN_DRAG:
        result = GX_SLIDER::event_process_(event_ptr);
        break;

    default:
        result = GX_WIDGET::event_process_(event_ptr);
        break;
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_pixelmap_slider_create                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a pixelmap slider, which is a type of widget  */
/*    for displaying a user-adjustable value in graphical fashion.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider control block          */
/*    name                                  Name of prompt                */
/*    parent                                Parent widget control block   */
/*    info                                  Pointer to a GX_SLIDER_INFO   */
/*    pixelmap_info                         Pointer to the pixelmap       */
/*                                            info block                  */
/*    style                                 Style of slider               */
/*    pixelmap_slider_id                    Application-defined ID of     */
/*                                            pixelmap slider             */
/*    size                                  Dimensions of pixelmap prompt */
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
UINT _gx_pixelmap_slider_create(GX_PIXELMAP_SLIDER* slider,
                                const GX_CHAR* name, GX_WIDGET* parent,
                                GX_SLIDER_INFO* info,
                                GX_PIXELMAP_SLIDER_INFO* pixelmap_info,
                                ULONG style, USHORT pixelmap_slider_id,
                                const GX_RECTANGLE* size)
{
    return slider->create_(name, parent, info, pixelmap_info, style, pixelmap_slider_id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_pixelmap_slider_draw                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified slider, which is a type of        */
/*    of widget designed to allow adjusting or display a value            */
/*    graphically                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_needle_position_get        Get the position of the       */
/*                                            slider needle               */
/*    _gx_canvas_pixelmap_tile              Tile a pixelmap               */
/*    _gx_context_pixelmap_get              Retrieve pixelmap image       */
/*    _gx_sidget_width_get                  Obtain the width of a widget  */
/*    _gx_wdiget_background_draw            Draw widget background        */
/*    _gx_canvas_pixelmap_draw              Draw pixelmap on canvas       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_pixelmap_slider_draw(GX_PIXELMAP_SLIDER* slider)
{
    slider->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_pixelmap_slider_event_process                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes events for the specified slider widget.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider's widget control block */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_slider_value_calcualte            Compute the slider value      */
/*    _gx_widget_event_process              Default widget event process  */
/*    _gx_pixelmap_slider_pixelmap_update   Update the pixelmap in slider */
/*                                            widget                      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_pixelmap_slider_event_process(GX_PIXELMAP_SLIDER* slider, GX_EVENT* event_ptr)
{
    return slider->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_pixelmap_slider_pixelmap_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a pixelmap slider, which is a type of widget  */
/*    for displaying a user-adjustable value in graphical fashion.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider control block          */
/*    pixinfo                               Slider infomration block      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_pixelmap_slider_pixelmap_update   Update the pixelmap for the   */
/*                                            pixlemap slider widget      */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_pixelmap_slider_pixelmap_set(GX_PIXELMAP_SLIDER* slider, GX_PIXELMAP_SLIDER_INFO* info)
{
    slider->pixelmap_set_(info);

    return GX_SUCCESS;
}
