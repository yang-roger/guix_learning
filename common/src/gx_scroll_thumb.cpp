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
/**   Scroll Management (Scroll Thumb)                                    */
/**                                                                       */
/**************************************************************************/

#include "gx_scroll_thumb.h"

#include "gx_system.h"
#include "gx_canvas.h"
#include "gx_context.h"
#include "gx_scroll_bar.h"

static void gx_scroll_thumb_draw_(GX_SCROLL_THUMB* scroll_thumb)
{
    return scroll_thumb->draw_();
}

static UINT gx_scroll_thumb_event_process_(GX_SCROLL_THUMB* scroll_thumb, GX_EVENT* event_ptr)
{
    return scroll_thumb->event_process_(event_ptr);
}

UINT GX_SCROLL_THUMB::create_(GX_SCROLLBAR* parent, ULONG style)
{
    GX_RECTANGLE size;

    if (parent)
    {
        size = parent->size;
    }
    else
    {
        size.bottom = size.left = size.right = size.top = 0;
    }

    GX_BUTTON::create_(GX_NULL, GX_NULL, style, GX_ID_SCROLL_THUMB, &size);
    status_remove_(GX_STATUS_ACCEPTS_FOCUS);

    normal_fill_color = GX_COLOR_ID_SCROLL_BUTTON;
    selected_fill_color = GX_COLOR_ID_SCROLL_BUTTON;
    border_color = GX_COLOR_ID_SCROLL_BUTTON;
    pixelmap = 0;

    draw_function = (void (*)(GX_WIDGET*))gx_scroll_thumb_draw_;
    event_process_function = (UINT (*)(GX_WIDGET*, GX_EVENT*))gx_scroll_thumb_event_process_;

    if (parent)
    {
        normal_fill_color = parent->appearance.thumb_color;
        selected_fill_color = parent->appearance.thumb_color;
        border_color = parent->appearance.thumb_border_color;
        pixelmap = parent->appearance.thumb_pixelmap;

        link_to_(parent);
    }

    return GX_SUCCESS;
}

INT GX_SCROLL_THUMB::shift_clamp_(INT shift) const
{
    const GX_SCROLLBAR* bar = (const GX_SCROLLBAR*)parent;

    if (is_vertical_())
    {
        if (shift < 0) // Up
        {
            INT min_shift = bar->top_() + bar->appearance.thumb_travel_min - top_();
            if (shift < min_shift)
            {
                shift = min_shift;
            }
        }
        else if (shift > 0) // Down
        {
            INT max_shift = bar->bottom_() - bar->appearance.thumb_travel_max - bottom_();
            if (shift > max_shift)
            {
                shift = max_shift;
            }
        }
    }
    else
    {
        if (shift < 0) // Left
        {
            INT min_shift = bar->left_() + bar->appearance.thumb_travel_min - left_();
            if (shift < min_shift)
            {
                shift = min_shift;
            }
        }
        else if (shift > 0) // Right
        {
            INT max_shift = bar->right_() - bar->appearance.thumb_travel_max - right_();
            if (shift > max_shift)
            {
                shift = max_shift;
            }
        }
    }

    return shift;
}

void GX_SCROLL_THUMB::draw_()
{
    if (pixelmap)
    {
        GX_PIXELMAP* map;
        _gx_context_pixelmap_get(pixelmap, &map);
        if (map)
        {
            GX_VALUE x = (GX_VALUE)(left_() + (width_() - map->width) / 2);
            GX_VALUE y = (GX_VALUE)(top_() + (height_() - map->height) / 2);

            _gx_canvas_pixelmap_draw(x, y, map);
        }
    }
    else
    {
        border_and_background_draw_(border_color, normal_fill_color, normal_fill_color, GX_TRUE);
    }
}

UINT GX_SCROLL_THUMB::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        result = GX_WIDGET::event_process_(event_ptr);
        if (pixelmap && pixelmap_is_transparent_(pixelmap))
        {
            status_add_(GX_STATUS_TRANSPARENT);
        }
        break;

    case GX_EVENT_PEN_DOWN:
        _gx_system_input_capture(this);

        if (is_vertical_())
        {
            start_drag = event_ptr->payload.pointdata.y;
        }
        else
        {
            start_drag = event_ptr->payload.pointdata.x;
        }
        break;

    case GX_EVENT_PEN_UP:
        drag_mode = GX_FALSE;

        if (status & GX_STATUS_OWNS_INPUT)
        {
            _gx_system_input_release(this);
        }
        break;

    case GX_EVENT_PEN_DRAG:
        drag_mode = GX_TRUE;

        if (is_vertical_())
        {
            if (event_ptr->payload.pointdata.y != start_drag)
            {
                INT shift = shift_clamp_(event_ptr->payload.pointdata.y - start_drag);
                if (shift)
                {
                    shift_(0, (GX_VALUE)shift, GX_TRUE);

                    start_drag = event_ptr->payload.pointdata.y;

                    GX_EVENT e;
                    e.type = GX_EVENT_VERTICAL_SCROLL;
                    e.sender = GX_ID_SCROLL_THUMB;
                    e.payload.intdata[0] = top_();
                    e.payload.intdata[1] = height_();
                    parent_event_process_run_(&e);
                }
            }
        }
        else
        {
            if (event_ptr->payload.pointdata.x != start_drag)
            {
                INT shift = shift_clamp_(event_ptr->payload.pointdata.x - start_drag);
                if (shift)
                {
                    shift_((GX_VALUE)shift, 0, GX_TRUE);

                    start_drag = event_ptr->payload.pointdata.x;

                    GX_EVENT e;
                    e.type = GX_EVENT_HORIZONTAL_SCROLL;
                    e.sender = GX_ID_SCROLL_THUMB;
                    e.payload.intdata[0] = left_();
                    e.payload.intdata[1] = width_();
                    parent_event_process_run_(&e);
                }
            }
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
/*    _gx_scroll_thumb_create                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a bitmap prompt, which is a special type of   */
/*    widget.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scroll_thumb                          Scroll thumb control block    */
/*    parent                                Pointer to parent scrollbar   */
/*    style                                 Style of scroll bar           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_button_create                     Create a button               */
/*    _gx_widget_status_remove              Clear the widget status flag  */
/*    _gx_widget_link                       Link the widget to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_scroll_thumb_create(GX_SCROLL_THUMB* scroll_thumb, GX_SCROLLBAR* parent, ULONG style)
{
    return scroll_thumb->create_(parent, style);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scroll_thumb_draw                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service draws a scroll thumb.                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scroll_thumb                          Scroll thumb widget control   */
/*                                          block                         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_pixelmap_get              Retrieve pixelmap image       */
/*    _gx_widget_width_get                  Gets the width of the widget  */
/*    _gx_widget_height_get                 Gets the height of the widget */
/*    _gx_canvas_pixelmap_draw              Draw pixelmap                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_scroll_thumb_draw(GX_SCROLL_THUMB* scroll_thumb)
{
    return scroll_thumb->draw_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_scroll_thumb_event_process                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes events for a scrollbar thumb button         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    scroll_thumb                          Scroll thumb widget control   */
/*                                            block                       */
/*    event                                 Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_button_event_process              Default button event process  */
/*    _gx_system_input_capture              Default widget event process  */
/*    _gx_system_input_release              Direct all input events       */
/*    _gx_widget_shift                      Shift the widget              */
/*    _gx_widget_height_get                 Get the height of widget      */
/*    _gx_scroll_thumb_shift_limit          Limit the travel of the       */
/*                                            scrollbar thumb button      */
/*    _gx_widget_width_get                  Get the width of widget       */
/*    [_gx_widget_event_process_function]   Widget event process routine  */
/*    _gx_widget_event_process              Default widget event process  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_scroll_thumb_event_process(GX_SCROLL_THUMB* scroll_thumb, GX_EVENT* event_ptr)
{
    return scroll_thumb->event_process_(event_ptr);
}
