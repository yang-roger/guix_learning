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
/**   Progress Bar Management (Radial Progress Bar)                       */
/**                                                                       */
/**************************************************************************/

#include "gx_radial_progress_bar.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_display.h"
#include "gx_pixelmap.h"

static void gx_radial_progress_bar_draw_(GX_RADIAL_PROGRESS_BAR* progress)
{
    progress->draw_();
}

static UINT gx_radial_progress_bar_event_process_(GX_RADIAL_PROGRESS_BAR* progress,
                                                  GX_EVENT* event_ptr)
{
    return progress->event_process_(event_ptr);
}

UINT GX_RADIAL_PROGRESS_BAR::create_(const GX_CHAR* name, GX_WIDGET* parent,
                                     const GX_RADIAL_PROGRESS_BAR_INFO* progress_info,
                                     ULONG style, USHORT progress_id)
{
    GX_RECTANGLE size;
    size.set_(0, 0, 0, 0);

    GX_WIDGET::create_(name, GX_NULL, style, progress_id, &size);

    this->type = GX_TYPE_RADIAL_PROGRESS_BAR;

    this->info = *progress_info;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    memset(&this->canvas, 0, sizeof(GX_CANVAS));
#endif

    this->draw_function = (void (*)(GX_WIDGET*))gx_radial_progress_bar_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_radial_progress_bar_event_process_;

    size_update_();

    this->status |= GX_STATUS_RESIZE_NOTIFY;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

#if defined(GX_BRUSH_ALPHA_SUPPORT)

void GX_RADIAL_PROGRESS_BAR::canvas_delete_()
{
    if (canvas.id == GX_CANVAS_ID)
    {
        if (canvas.memory && _gx_system_memory_free)
        {
            _gx_system_memory_free((void*)canvas.memory);
        }

        canvas.delete_();
    }
}

#endif // GX_BRUSH_ALPHA_SUPPORT

void GX_RADIAL_PROGRESS_BAR::info_set_(const GX_RADIAL_PROGRESS_BAR_INFO* info)
{
    this->info = *info;

    this->info.anchor_val = _gx_utility_value_clamp(this->info.anchor_val, 0, 360);

    size_update_();

    dirty_mark_if_visible_();
}

void GX_RADIAL_PROGRESS_BAR::anchor_set_(GX_VALUE angle)
{
    angle = _gx_utility_value_clamp(angle, 0, 360);

    if (info.anchor_val != angle)
    {
        info.anchor_val = angle;

        dirty_mark_if_visible_();
    }
}

void GX_RADIAL_PROGRESS_BAR::value_set_(GX_VALUE new_value)
{
    new_value = _gx_utility_value_clamp(new_value, -360, 360);

    if (info.current_val != new_value)
    {
        info.current_val = new_value;

        dirty_mark_if_visible_();

        signal_parent_(GX_EVENT_PROGRESS_VALUE, new_value);
    }
}

void GX_RADIAL_PROGRESS_BAR::font_set_(GX_RESOURCE_ID font_id)
{
    info.font_id = font_id;

    dirty_mark_if_visible_();
}

void GX_RADIAL_PROGRESS_BAR::text_color_set_(GX_RESOURCE_ID normal_text_color_id,
                                             GX_RESOURCE_ID selected_text_color_id,
                                             GX_RESOURCE_ID disabled_text_color_id)
{
    info.normal_text_color = normal_text_color_id;
    info.selected_text_color = selected_text_color_id;
    info.disabled_text_color = disabled_text_color_id;

    dirty_mark_if_visible_();
}

GX_VALUE GX_RADIAL_PROGRESS_BAR::brush_half_width_() const
{
    GX_VALUE width = GX_MAX(info.normal_brush_width, info.selected_brush_width);
    width = (GX_VALUE)((width + 1) / 2);

    return width;
}

void GX_RADIAL_PROGRESS_BAR::size_update_()
{
    GX_VALUE width = brush_half_width_();

    GX_RECTANGLE new_size;

    new_size.left = (GX_VALUE)(info.xcenter - info.radius - width);
    new_size.right = (GX_VALUE)(info.xcenter + info.radius + width);
    new_size.top = (GX_VALUE)(info.ycenter - info.radius - width);
    new_size.bottom = (GX_VALUE)(info.ycenter + info.radius + width);

    if (new_size.left != left_() ||
        new_size.right != right_() ||
        new_size.top != top_())
    {
        resize_(&new_size);
    }
}

// updates the center and radius of radial progress bar according to size.
void GX_RADIAL_PROGRESS_BAR::center_and_radius_update_()
{
    GX_VALUE new_xcenter = (GX_VALUE)((left_() + right_()) / 2);
    GX_VALUE new_ycenter = (GX_VALUE)((top_() + bottom_()) / 2);
    GX_VALUE new_radius = (GX_VALUE)(new_xcenter - left_() - brush_half_width_());

    info.xcenter = new_xcenter;
    info.ycenter = new_ycenter;

    if (new_radius > 0)
    {
        info.radius = new_radius;
    }

#if defined(GX_BRUSH_ALPHA_SUPPORT)

    if (width_() != canvas.x_resolution || height_() != canvas.y_resolution)
    {
        canvas_delete_();
    }

#endif // GX_BRUSH_ALPHA_SUPPORT

    dirty_mark_if_visible_();
}

bool GX_RADIAL_PROGRESS_BAR::value_calculate_(GX_POINT new_position)
{
    GX_POINT center;
    center.x = info.xcenter;
    center.y = info.ycenter;

    INT new_value;
    if (_gx_utility_radial_angle_calculate(new_position, center, &new_value) != GX_SUCCESS)
    {
        return false;
    }

    new_value = new_value - info.anchor_val;

    if (info.current_val <= 0) // CW
    {
        if (new_value > 0)
        {
            new_value -= 360;
        }
    }
    else // CCW
    {
        if (new_value < 0)
        {
            new_value += 360;
        }
    }

    value_set_((GX_VALUE)new_value);

    return true;
}

void GX_RADIAL_PROGRESS_BAR::background_draw_()
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

    GX_BRUSH* brush = &context->brush;

#if defined(GX_BRUSH_ALPHA_SUPPORT)

    UINT result = GX_FAILURE;

    GX_PIXELMAP map;
    GX_DISPLAY* display;
    void* canvas_memory;
    GX_VALUE old_xpos = 0;
    GX_VALUE old_ypos = 0;

    UCHAR brush_alpha = brush->alpha;

    if (brush_alpha == 0)
    {
        return;
    }

    if (brush_alpha != GX_ALPHA_VALUE_OPAQUE)
    {
        map.width = width_();
        map.height = height_();

        display = context->display;

        if (canvas.id != GX_CANVAS_ID && _gx_system_memory_allocator)
        {
            ULONG canvas_size = display->driver_row_pitch_get((USHORT)map.width) * map.height;
            canvas_memory = _gx_system_memory_allocator(canvas_size);
            if (canvas_memory)
            {
                canvas.create_("radial_progress_canvas", display, GX_CANVAS_SIMPLE,
                               (UINT)map.width, (UINT)map.height, (GX_COLOR*)canvas_memory, canvas_size);
            }
        }

        if (canvas.id == GX_CANVAS_ID)
        {
            status_remove_(GX_STATUS_RESIZE_NOTIFY);

            old_xpos = left_();
            old_ypos = top_();

            shift_((GX_VALUE)(-old_xpos), (GX_VALUE)(-old_ypos), GX_FALSE);

            GX_CANVAS* context_canvas = context->canvas;

            GX_RECTANGLE dirty = context_canvas->dirty_area;
            dirty.move_((GX_VALUE)(-old_xpos), (GX_VALUE)(-old_ypos));

            if (gx_rectangle_intersect_(dirty, this->size, &dirty))
            {
                result = canvas.drawing_initiate_(GX_NULL, dirty);
                if (result == GX_SUCCESS || result == GX_NO_VIEWS)
                {
                    context = _gx_system_current_draw_context;

                    if (this->status & GX_STATUS_TRANSPARENT)
                    {
                        GX_VALUE saved_offset_x = context_canvas->display_offset_x;
                        GX_VALUE saved_offset_y = context_canvas->display_offset_y;

                        context_canvas->offset_set_((GX_VALUE)(-old_xpos), (GX_VALUE)(-old_ypos));

                        display->driver_canvas_copy(context_canvas, &canvas);

                        context_canvas->offset_set_(saved_offset_x, saved_offset_y);
                    }

                    brush = &context->brush;
                }
            }
        }
    }

#endif // GX_BRUSH_ALPHA_SUPPORT

    border_and_background_draw_();

    GX_VALUE xcenter = (GX_VALUE)((left_() + right_()) / 2);
    GX_VALUE ycenter = (GX_VALUE)((top_() + bottom_()) / 2);

    brush->style &= ~GX_BRUSH_SOLID_FILL;

    if (style & GX_STYLE_RADIAL_PROGRESS_ALIAS)
    {
        brush->style |= GX_BRUSH_ALIAS;
    }

    if (style & GX_STYLE_RADIAL_PROGRESS_ROUND)
    {
        brush->style |= GX_BRUSH_ROUND;
    }

    if (!(style & GX_STYLE_RADIAL_PROGRESS_NO_BACKTRACK))
    {
        context->line_color_set_(info.normal_brush_color);
        brush->width = (UINT)info.normal_brush_width;

        _gx_canvas_circle_draw(xcenter, ycenter, (UINT)info.radius);
    }

    if (info.current_val)
    {
        context->line_color_set_(info.selected_brush_color);
        brush->width = (UINT)info.selected_brush_width;

        if (info.current_val > 0)
        {
            _gx_canvas_arc_draw(xcenter, ycenter, (UINT)info.radius,
                                info.anchor_val,
                                info.anchor_val + info.current_val);
        }
        else
        {
            _gx_canvas_arc_draw(xcenter, ycenter, (UINT)info.radius,
                                info.anchor_val + info.current_val,
                                info.anchor_val);
        }
    }

#if defined(GX_BRUSH_ALPHA_SUPPORT)

    if (brush_alpha != GX_ALPHA_VALUE_OPAQUE && canvas.id == GX_CANVAS_ID)
    {
        shift_(old_xpos, old_ypos, GX_FALSE);

        if (result == GX_SUCCESS || result == GX_NO_VIEWS)
        {
            _gx_canvas_pixelmap_get(&map);
            canvas.drawing_complete_(GX_FALSE);

            _gx_canvas_pixelmap_blend(old_xpos, old_ypos, &map, brush_alpha);
        }

        status_add_(GX_STATUS_RESIZE_NOTIFY);
    }

#endif // GX_BRUSH_ALPHA_SUPPORT
}

void GX_RADIAL_PROGRESS_BAR::text_draw_()
{
    GX_RESOURCE_ID text_color;

    if (is_enabled_())
    {
        if (style & GX_STYLE_DRAW_SELECTED)
        {
            text_color = info.selected_text_color;
        }
        else
        {
            text_color = info.normal_text_color;
        }
    }
    else
    {
        text_color = info.disabled_text_color;
    }

    const UINT GX_MAX_RAIDAL_PROGRESS_BAR_TEXT_LENGTH = 5;
    GX_CHAR text[GX_MAX_RAIDAL_PROGRESS_BAR_TEXT_LENGTH + 1];

    GX_STRING string;

    string.ptr = text;
    string.length = _gx_utility_value_or_percent_string_get((style & GX_STYLE_PROGRESS_PERCENT),
                                                            GX_ABS(info.current_val), 360,
                                                            text, sizeof(text));

    text_draw_ext_(text_color, info.font_id, &string, 0, 0);
}

void GX_RADIAL_PROGRESS_BAR::draw_()
{
    background_draw_();

    if (style & GX_STYLE_PROGRESS_TEXT_DRAW)
    {
        text_draw_();
    }

    children_draw_();
}

UINT GX_RADIAL_PROGRESS_BAR::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        GX_WIDGET::event_process_(event_ptr);
        break;

#if defined(GX_BRUSH_ALPHA_SUPPORT)

    case GX_EVENT_HIDE:
        GX_WIDGET::event_process_(event_ptr);
        canvas_delete_();
        break;

#endif // GX_BRUSH_ALPHA_SUPPORT

    case GX_EVENT_RESIZED:
        center_and_radius_update_();
        break;

    case GX_EVENT_PEN_DOWN:
        if (is_enabled_())
        {
            _gx_system_input_capture(this);

            GX_POINT new_position = event_ptr->payload.pointdata;

            GX_POINT center;
            center.x = info.xcenter;
            center.y = info.ycenter;

            INT dist = gx_point_distance_(new_position, center);

            INT width = (INT)(info.selected_brush_width / 2);

            if (dist <= (INT)(info.radius + width) &&
                dist >= (INT)(info.radius - width))
            {
                value_calculate_(new_position);
            }
        }
        else
        {
            parent_event_process_run_(event_ptr);
        }
        break;

    case GX_EVENT_PEN_UP:
        if (this->status & GX_STATUS_OWNS_INPUT)
        {
            _gx_system_input_release(this);
        }
        else
        {
            parent_event_process_run_(event_ptr);
        }
        break;

    case GX_EVENT_PEN_DRAG:
        if (this->status & GX_STATUS_OWNS_INPUT)
        {
            value_calculate_(event_ptr->payload.pointdata);
        }
        else
        {
            parent_event_process_run_(event_ptr);
        }
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
/*    _gx_radial_progress_bar_anchor_set                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the starting angle for radial progress bar.       */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Radial Progress Bar control   */
/*                                            block                       */
/*    angle                                 Starting angle of the         */
/*                                            circular arc                */
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
UINT _gx_radial_progress_bar_anchor_set(GX_RADIAL_PROGRESS_BAR* progress,
                                        GX_VALUE angle)
{
    progress->anchor_set_(angle);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_progress_bar_background_draw                             */
/*                                                           6.1.8        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws a radial progress bar background.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Radial Progress Bar control   */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_background_draw                                          */
/*    _gx_context_brush_get                                               */
/*    context->line_color_set_                                          */
/*    _gx_context_brush_width_set                                         */
/*    _gx_canvas_circle_draw                                              */
/*    _gx_canvas_arc_draw                                                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_radial_progress_bar_background_draw(GX_RADIAL_PROGRESS_BAR* progress)
{
    progress->background_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_progress_bar_create                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service creates a radial progress bar.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Progress Bar control block    */
/*    name                                  Name of radial progress bar   */
/*    parent                                Pointer to parent widget      */
/*    info                                  Pointer to radial progress    */
/*                                            bar info.                   */
/*    style                                 Style of radial progress bar  */
/*    id                                    Application-defined ID of     */
/*                                            progress bar                */
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
UINT _gx_radial_progress_bar_create(GX_RADIAL_PROGRESS_BAR* progress,
                                    const GX_CHAR* name,
                                    GX_WIDGET* parent,
                                    GX_RADIAL_PROGRESS_BAR_INFO* progress_bar_info,
                                    ULONG style,
                                    USHORT progress_id)
{
    return progress->create_(name, parent, progress_bar_info, style, progress_id);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_progress_bar_draw                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws a radial progress bar.                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Radial Progress Bar control   */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_progress_bar_background_draw                             */
/*    _gx_radial_progress_bar_text_draw                                   */
/*    _gx_widget_children_draw                                            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_radial_progress_bar_draw(GX_RADIAL_PROGRESS_BAR* progress)
{
    progress->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_progress_bar_event_process                               */
/*                                                           6.1.8        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service processes a radial progress bar event.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    radial_progress_bar                   Radial Progress Bar control   */
/*                                            block                       */
/*    event_ptr                             Pointer to event process      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_process              Default widget event process  */
/*    _gx_radial_progress_bar_resize        Resize the progress bar       */
/*    _gx_radial_progress_bar_value_calculate                             */
/*                                          Calculate the value of the    */
/*                                            progress bar                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_progress_bar_event_process(GX_RADIAL_PROGRESS_BAR* progress,
                                           GX_EVENT* event_ptr)
{
    return progress->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_progress_bar_font_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the font of a radial progress bar widget.         */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Radial Progress Bar control   */
/*                                            block                       */
/*    font_id                               Resource ID of font           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the area of the widget   */
/*                                            as dirty                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_progress_bar_font_set(GX_RADIAL_PROGRESS_BAR* progress,
                                      GX_RESOURCE_ID font_id)
{
    progress->font_set_(font_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_progress_bar_info_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reset the information structure of a radial progress  */
/*      bar                                                               */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    bar                                   Radial proress bar control    */
/*                                            block                       */
/*    info                                  Radial progess bar info block */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_radial_progrss_bar_size_update    Update the size of the        */
/*                                            progress bar                */
/*    _gx_system_dirty_mark                 Mark the widget as dirty      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_progress_bar_info_set(GX_RADIAL_PROGRESS_BAR* progress,
                                      GX_RADIAL_PROGRESS_BAR_INFO* info)
{
    progress->info_set_(info);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_progress_bar_text_color_set                              */
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
UINT _gx_radial_progress_bar_text_color_set(GX_RADIAL_PROGRESS_BAR* progress,
                                            GX_RESOURCE_ID normal_text_color_id,
                                            GX_RESOURCE_ID selected_text_color_id,
                                            GX_RESOURCE_ID disabled_text_color_id)
{
    progress->text_color_set_(normal_text_color_id,
                              selected_text_color_id,
                              disabled_text_color_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_progress_bar_text_draw                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the specified radial progress bar with text.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Radial Progress Bar control   */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_ltoa                      Converts a long integer value */
/*                                            into an ASCII string        */
/*    gx_string_length_get_       Test string length            */
/*    _gx_widget_text_draw                  Draw the text on the widget   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_radial_progress_bar_text_draw(GX_RADIAL_PROGRESS_BAR* progress)
{
    progress->text_draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_radial_progress_bar_value_set                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the radial progress bar value.                    */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    progress_bar                          Radial Progress Bar control   */
/*                                            block                       */
/*    new_value                             New progress bar value        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark the widget dirty         */
/*    _gx_widget_event_generate             Generate an event to the      */
/*                                            GUIX system                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_radial_progress_bar_value_set(GX_RADIAL_PROGRESS_BAR* progress, GX_VALUE new_value)
{
    progress->value_set_(new_value);

    return GX_SUCCESS;
}
