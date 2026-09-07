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
/**   Text Scroll Wheel Management (Scroll Wheel)                         */
/**                                                                       */
/**************************************************************************/

#include "gx_text_scroll_wheel.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_display.h"
#include "gx_canvas.h"
#include "gx_font.h"

static void gx_text_scroll_wheel_draw_(GX_TEXT_SCROLL_WHEEL* wheel)
{
    wheel->draw_();
}

static UINT gx_text_scroll_wheel_event_process_(GX_TEXT_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr)
{
    return wheel->event_process_(event_ptr);
}

UINT GX_TEXT_SCROLL_WHEEL::create_(const GX_CHAR* name, GX_WIDGET* parent, INT total_rows,
                                   ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    GX_SCROLL_WHEEL::create_(name, GX_NULL, total_rows, style, Id, size);

    this->type = GX_TYPE_TEXT_SCROLL_WHEEL;

    this->normal_font = GX_FONT_ID_DEFAULT;
    this->selected_font = GX_FONT_ID_DEFAULT;

    this->normal_text_color = GX_COLOR_ID_SHADOW;
    this->selected_text_color = GX_COLOR_ID_TEXT;
    this->disabled_text_color = GX_COLOR_ID_DISABLED_TEXT;

    this->draw_function = (void (*)(GX_WIDGET*))gx_text_scroll_wheel_draw_;
    this->event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_text_scroll_wheel_event_process_;

    this->text_get = GX_NULL;

#ifdef GX_DYNAMIC_BIDI_TEXT_SUPPORT
    this->bidi_resolved_text_info = GX_NULL;
#endif  // GX_DYNAMIC_BIDI_TEXT_SUPPORT

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_TEXT_SCROLL_WHEEL::text_get_callback_set_ext_(UINT (*callback)(GX_TEXT_SCROLL_WHEEL*, INT, GX_STRING*))
{
    this->text_get = callback;

#ifdef GX_DYNAMIC_BIDI_TEXT_SUPPORT
    dynamic_bidi_text_delete_();
#endif  // GX_DYNAMIC_BIDI_TEXT_SUPPORT
}

#ifdef GX_DYNAMIC_BIDI_TEXT_SUPPORT

void GX_TEXT_SCROLL_WHEEL::dynamic_bidi_text_delete_()
{
    if (bidi_resolved_text_info)
    {
        for (INT row = 0; row < total_rows; ++row)
        {
            if (bidi_resolved_text_info[row])
            {
                _gx_utility_bidi_resolved_text_info_delete(&bidi_resolved_text_info[row]);
            }
        }

        if (_gx_system_memory_free)
        {
            _gx_system_memory_free(bidi_resolved_text_info);
            bidi_resolved_text_info = GX_NULL;
        }
    }
}

#endif  // GX_DYNAMIC_BIDI_TEXT_SUPPORT

UINT GX_TEXT_SCROLL_WHEEL::text_get_(INT row, GX_STRING* string)
{
    text_get(this, row, string);

#ifdef GX_DYNAMIC_BIDI_TEXT_SUPPORT

    if (_gx_system_bidi_text_enabled)
    {
        if (!_gx_system_memory_allocator)
        {
            return GX_SYSTEM_MEMORY_ERROR;
        }

        if (!bidi_resolved_text_info)
        {
            bidi_resolved_text_info = (GX_BIDI_RESOLVED_TEXT_INFO **)_gx_system_memory_allocator(sizeof(GX_BIDI_RESOLVED_TEXT_INFO *) * (UINT)total_rows);
            if (!bidi_resolved_text_info)
            {
                return GX_SYSTEM_MEMORY_ERROR;
            }

            memset(bidi_resolved_text_info, 0, sizeof(GX_BIDI_RESOLVED_TEXT_INFO *) * (UINT)total_rows);
        }

        if (!bidi_resolved_text_info[row])
        {
            GX_BIDI_TEXT_INFO text_info;

            text_info.text = *string;
            text_info.font = GX_NULL;
            text_info.display_width = 0;
            text_info.direction = text_direction_get_();

            GX_BIDI_RESOLVED_TEXT_INFO* resolved_info;
            if (_gx_utility_bidi_paragraph_reorder_ext(&text_info, &resolved_info) == GX_SUCCESS)
            {
                bidi_resolved_text_info[row] = resolved_info;
            }
        }

        if (bidi_resolved_text_info[row])
        {
            *string = *bidi_resolved_text_info[row]->text;
        }
    }

#endif // GX_DYNAMIC_BIDI_TEXT_SUPPORT

    return GX_SUCCESS;
}

void GX_TEXT_SCROLL_WHEEL::font_set_(GX_RESOURCE_ID normal_font, GX_RESOURCE_ID selected_font)
{
    this->normal_font = normal_font;
    this->selected_font = selected_font;

    dirty_mark_if_visible_();
}

void GX_TEXT_SCROLL_WHEEL::text_color_set_(GX_RESOURCE_ID normal_text_color,
                                           GX_RESOURCE_ID selected_text_color,
                                           GX_RESOURCE_ID disabled_text_color)
{
    this->normal_text_color = normal_text_color;
    this->selected_text_color = selected_text_color;
    this->disabled_text_color = disabled_text_color;

    dirty_mark_if_visible_();
}

void GX_TEXT_SCROLL_WHEEL::draw_()
{
    GX_WINDOW::draw_();

    if (total_rows > 0 && text_get)
    {
        if (style & GX_STYLE_TEXT_SCROLL_WHEEL_ROUND)
        {
            round_draw_();
        }
        else
        {
            flat_draw_();
        }
    }
}

bool GX_TEXT_SCROLL_WHEEL::trans_row_height_(INT dist, GX_VALUE client_height, INT* return_trans_height) const
{
    INT trans_height = GX_FIXED_VAL_MAKE(GX_ABS(client_height * 3 / 4 - dist));
    trans_height = 120 * trans_height / client_height;
    trans_height = GX_FIXED_VAL_TO_INT(_gx_utility_math_sin(trans_height) * row_height);

    *return_trans_height = trans_height;

    return trans_height >= row_height / 5;
}

void GX_TEXT_SCROLL_WHEEL::round_draw_()
{
    UINT         result = GX_SUCCESS;
    GX_PIXELMAP* map;
    GX_VALUE     client_height;
    INT          trans_height;
    INT          dist;
    GX_VALUE     ycenter;
    GX_VALUE     ypos;
    GX_RECTANGLE selected_area;
    GX_RECTANGLE draw_area;
    GX_STRING    string;
    INT          row;

    client_height = client.height_();

    if (client.width_() <= 0 || client_height <= 0)
    {
        return;
    }

    ycenter = (GX_VALUE)(client.top + client_height / 2);

    // Draw selected background.

    selected_area.left = client.left;
    selected_area.top = (GX_VALUE)(ycenter - row_height / 2);
    selected_area.right = client.right;
    selected_area.bottom = (GX_VALUE)(selected_area.top + row_height - 1);

    _gx_context_pixelmap_get(selected_background, &map);
    if (map)
    {
        _gx_canvas_pixelmap_tile(&selected_area, map);
    }

    // Draw scroll wheel rows.

    ypos = (GX_VALUE)(selected_area.top + selected_yshift);
    row = selected_row;

    while (result == GX_SUCCESS && ypos < client.bottom)
    {
        if (row > (INT)(total_rows - 1))
        {
            if (style & GX_STYLE_WRAP)
            {
                row -= total_rows;
            }
            else
            {
                break;
            }
        }

        result = text_get_(row, &string);
        if (result != GX_SUCCESS)
        {
            return;
        }

        dist = GX_ABS(ypos + row_height / 2 - ycenter);
        if (dist == 0)
        {
            trans_height = row_height;
        }
        else
        {
            if (!trans_row_height_(dist, client_height, &trans_height))
            {
                break;
            }
        }

        draw_area.set_(client.left, ypos,
                       client.right, (GX_VALUE)(ypos + trans_height - 1));

        result = row_draw_(selected_area, draw_area, &string);

        ypos = (GX_VALUE)(ypos + trans_height);
        ++row;
    }

    ypos = (GX_VALUE)(selected_area.top + selected_yshift);
    row = selected_row - 1;

    while (result == GX_SUCCESS && ypos > client.top)
    {
        if (row < 0)
        {
            if (style & GX_STYLE_WRAP)
            {
                row += total_rows;
            }
            else
            {
                break;
            }
        }

        result = text_get_(row, &string);
        if (result != GX_SUCCESS)
        {
            return;
        }

        dist = ycenter + (row_height / 2) - ypos;

        if (!trans_row_height_(dist, client_height, &trans_height))
        {
            break;
        }

        draw_area.set_(client.left, (GX_VALUE)(ypos - trans_height),
                       client.right, (GX_VALUE)(ypos - 1));

        result = row_draw_(selected_area, draw_area, &string);

        ypos = (GX_VALUE)(ypos - trans_height);
        --row;
    }

    // Draw the overlay pixelmap, if there is one:
    if (gradient.pixelmap.data)
    {
        _gx_canvas_pixelmap_tile(&size, &gradient.pixelmap);
    }
}

void GX_TEXT_SCROLL_WHEEL::flat_draw_()
{
    UINT         result;
    GX_PIXELMAP* map;
    GX_VALUE     client_height;
    GX_VALUE     ycenter;
    GX_VALUE     ypos;
    GX_RECTANGLE selected_area;
    GX_RECTANGLE draw_area;
    GX_STRING    string;
    INT          row;
    INT          top_rows;

    client_height = client.height_();

    if (client.width_() <= 0 || client_height <= 0)
    {
        return;
    }

    ycenter = (GX_VALUE)(client.top + client_height / 2);

    // Draw selected background.

    selected_area.left = client.left;
    selected_area.top = (GX_VALUE)(ycenter - row_height / 2);
    selected_area.right = client.right;
    selected_area.bottom = (GX_VALUE)(selected_area.top + row_height - 1);

    _gx_context_pixelmap_get(selected_background, &map);
    if (map)
    {
        _gx_canvas_pixelmap_tile(&selected_area, map);
    }

    // Draw scroll wheel rows.

    ypos = (GX_VALUE)(selected_area.top + selected_yshift);

    top_rows = (ypos - client.top + row_height - 1) / row_height;
    ypos = (GX_VALUE)(ypos - (top_rows * row_height));

    row = selected_row - top_rows;

    while (row < 0)
    {
        if (style & GX_STYLE_WRAP)
        {
            row += total_rows;
        }
        else
        {
            ypos = (GX_VALUE)(ypos - row * row_height);
            row = 0;
        }
    }

    while (ypos < client.bottom)
    {
        if (row > (INT)(total_rows - 1))
        {
            if (style & GX_STYLE_WRAP)
            {
                row -= total_rows;
            }
            else
            {
                break;
            }
        }

        result = text_get_(row, &string);
        if (result != GX_SUCCESS)
        {
            return;
        }

        draw_area.set_(client.left, ypos,
                       client.right, (GX_VALUE)(ypos + row_height - 1));

        row_draw_(selected_area, draw_area, &string);

        ypos = (GX_VALUE)(ypos + row_height);
        ++row;
    }

    // Draw the overlay pixelmap, if there is one:
    if (gradient.pixelmap.data)
    {
        _gx_canvas_pixelmap_tile(&size, &gradient.pixelmap);
    }
}

UINT GX_TEXT_SCROLL_WHEEL::row_draw_(const GX_RECTANGLE& selected_area,
                                     const GX_RECTANGLE& draw_area,
                                     const GX_STRING* string)
{
    if (string->length == 0)
    {
        return GX_SUCCESS;
    }

    GX_RESOURCE_ID text_color;
    GX_RESOURCE_ID font;

    if ((status & GX_STATUS_TRACKING_PEN) ||
        (animation_steps > 0) ||
        (selected_area != draw_area))
    {
        if (is_enabled_())
        {
            text_color = normal_text_color;
        }
        else
        {
            text_color = disabled_text_color;
        }

        font = normal_font;
    }
    else
    {
        if (is_enabled_())
        {
            text_color = selected_text_color;
        }
        else
        {
            text_color = disabled_text_color;
        }

        font = selected_font;
    }

    if (style & GX_STYLE_TEXT_SCROLL_WHEEL_ROUND)
    {
        return round_row_draw_(text_color, font, string, draw_area);
    }
    else
    {
        return flat_row_draw_(text_color, font, string, draw_area);
    }
}

UINT GX_TEXT_SCROLL_WHEEL::round_row_draw_(GX_RESOURCE_ID tColor, GX_RESOURCE_ID font_id,
                                           const GX_STRING* string,
                                           const GX_RECTANGLE& draw_area)
{
    GX_DRAW_CONTEXT* context = gx_context_current_();

    context->line_color_set_(tColor);
    context->font_set_(font_id);

    GX_BRUSH& brush = context->brush;

    if (!brush.font)
    {
        return GX_FAILURE;
    }

    if (row_height <= 0)
    {
        return GX_FAILURE;
    }

    GX_VALUE text_height = brush.font->line_height;

    text_height = (GX_VALUE)(draw_area.height_() * text_height / row_height);
    if (text_height <= 0)
    {
        return GX_FAILURE;
    }

    GX_VALUE text_width = _gx_system_string_width_get(brush.font, string->ptr, string->length);
    if (text_width <= 0)
    {
        return GX_FAILURE;
    }

    GX_VALUE x_pos;
    GX_VALUE y_pos;
    draw_position_get_(text_width, text_height, draw_area, &x_pos, &y_pos);

    GX_PIXELMAP textmap;
    UINT result = _gx_utility_string_to_alphamap_ext(string, brush.font, &textmap);
    if (result == GX_SUCCESS)
    {
        GX_PIXELMAP resized_map;
        result = _gx_utility_pixelmap_resize(&textmap, &resized_map, text_width, text_height);
        if (result == GX_SUCCESS)
        {
            GX_COLOR old_fill_color = brush.fill_color;

            context->fill_color_set_(tColor);

#if defined(GX_RENESAS_DAVE2D_DRAW)
            resized_map.flags |= GX_PIXELMAP_DYNAMICALLY_ALLOCATED;
#endif

            _gx_canvas_pixelmap_draw(x_pos, y_pos, &resized_map);

            brush.fill_color = old_fill_color;

            _gx_system_memory_free((void*)resized_map.data);
        }

        _gx_system_memory_free((void*)(textmap.data));
    }

    return result;
}

UINT GX_TEXT_SCROLL_WHEEL::flat_row_draw_(GX_RESOURCE_ID tColor, GX_RESOURCE_ID font_id,
                                          const GX_STRING* string,
                                          const GX_RECTANGLE& draw_area)
{
    GX_DRAW_CONTEXT* context = gx_context_current_();

    context->line_color_set_(tColor);
    context->font_set_(font_id);

    const GX_BRUSH& brush = context->brush;

    if (!brush.font)
    {
        return GX_FAILURE;
    }

    GX_VALUE text_height = brush.font->line_height;

    GX_VALUE text_width = _gx_system_string_width_get(brush.font, string->ptr, string->length);
    if (text_width <= 0)
    {
        return GX_FAILURE;
    }

    GX_VALUE x_pos;
    GX_VALUE y_pos;
    draw_position_get_(text_width, text_height, draw_area, &x_pos, &y_pos);

    _gx_canvas_text_draw_ext(x_pos, y_pos, string);

    return GX_SUCCESS;
}

void GX_TEXT_SCROLL_WHEEL::draw_position_get_(GX_VALUE text_width, GX_VALUE text_height,
                                              const GX_RECTANGLE& draw_area,
                                              GX_VALUE* x_pos, GX_VALUE* y_pos)
{
    *y_pos = (GX_VALUE)(draw_area.top + (draw_area.height_() - text_height) / 2);

    switch (style & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        *x_pos = (GX_VALUE)(draw_area.right - text_width);
        break;

    case GX_STYLE_TEXT_CENTER:
    default:
        *x_pos = (GX_VALUE)(draw_area.left + (draw_area.width_() - text_width) / 2);
        break;

    case GX_STYLE_TEXT_LEFT:
        *x_pos = draw_area.left;
        break;
    }
}

UINT GX_TEXT_SCROLL_WHEEL::event_process_(GX_EVENT* event_ptr)
{
    switch (event_ptr->type)
    {
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)

    case GX_EVENT_DELETE:
        dynamic_bidi_text_delete_();
        return GX_SCROLL_WHEEL::event_process_(event_ptr);

#endif // GX_DYNAMIC_BIDI_TEXT_SUPPORT

    default:
        return GX_SCROLL_WHEEL::event_process_(event_ptr);
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_scroll_wheel_callback_set_ext                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function updates the pointer to the callback function          */
/*    used to determine the string to be displayed at a give wheel row    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Text scroll wheel control     */
/*                                            block                       */
/*    callback                              Scroll wheel text get callback*/
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
UINT _gx_text_scroll_wheel_callback_set_ext(GX_TEXT_SCROLL_WHEEL* wheel,
                                            UINT (*callback)(GX_TEXT_SCROLL_WHEEL*, INT, GX_STRING*))
{
    wheel->text_get_callback_set_ext_(callback);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_scroll_wheel_create                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a scroll wheel selector widget.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    name                                  Name of widget                */
/*    parent                                Parent widget control block   */
/*    total_rows                            Total rows of the scroll wheel*/
/*    style                                 Style of widget               */
/*    Id                                    Application-defined ID of the */
/*                                            the widget                  */
/*    size                                  Widget size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_create               Create a text scroll wheel    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_scroll_wheel_create(GX_TEXT_SCROLL_WHEEL* wheel,
                                  const GX_CHAR* name, GX_WIDGET* parent, INT total_rows,
                                  ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    return wheel->create_(name, parent, total_rows, style, Id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_scroll_wheel_draw                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws a text scroll wheel widget.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Text scroll wheel control     */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_window_draw                       Draw a window                 */
/*    _gx_text_scroll_wheel_round_draw      Draw round style scroll wheel */
/*    _gx_text_scroll_wheel_flat_draw       Draw flat style scroll wheel  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_text_scroll_wheel_draw(GX_TEXT_SCROLL_WHEEL* wheel)
{
    wheel->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_scroll_wheel_event_process                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes the comming events for a scroll wheel       */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Text scroll wheel control     */
/*                                            block                       */
/*    event_ptr                             Event to be processed         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_scroll_wheel_dynamic_bidi_text_delete                     */
/*                                          Release dynamic bidi text     */
/*    _gx_scroll_wheel_event_process        Default event process         */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_scroll_wheel_event_process(GX_TEXT_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr)
{
    return wheel->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_scroll_wheel_font_set                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets font for text scroll wheel widget.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    normal_font                           Normal font id to be set      */
/*    selected_font                         Selected font id to be set    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_scroll_wheel_font_set(GX_TEXT_SCROLL_WHEEL* wheel,
                                    GX_RESOURCE_ID normal_font,
                                    GX_RESOURCE_ID selected_font)
{
    wheel->font_set_(normal_font, selected_font);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_text_scroll_wheel_text_color_set                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets text color for a text scroll wheel widget.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    normal_text_color                     Normal text color id to be set*/
/*    selected_text_color                   Selected text color id        */
/*    disabled_text_color                   Disabled text color id        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_mark                 Mark a widget as dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_text_scroll_wheel_text_color_set(GX_TEXT_SCROLL_WHEEL* wheel,
                                          GX_RESOURCE_ID normal_text_color,
                                          GX_RESOURCE_ID selected_text_color,
                                          GX_RESOURCE_ID disabled_text_color)
{
    wheel->text_color_set_(normal_text_color, selected_text_color, disabled_text_color);

    return GX_SUCCESS;
}
