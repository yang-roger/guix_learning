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
/**   System Management (System)                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "gx_system_dirty.h"

#include "gx_system.h"
#include "gx_display.h"
#include "gx_canvas.h"
#include "gx_window_root.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_dirty_mark                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service marks the area of this widget as dirty.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget control     */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_dirty_partial_add          Mark the partial areal of     */
/*                                            this widget as dirty        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Application                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_dirty_mark(GX_WIDGET* widget)
{
    if (!widget)
    {
        return GX_PTR_ERROR;
    }

    return _gx_system_dirty_partial_add(widget, &widget->size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_dirty_partial_add                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service marks the partial area of this widget as dirty.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    widget                                Pointer to widget control     */
/*                                            block                       */
/*    dirty_area                            Dirty area of widget to add   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
#ifndef GUIX_STUDIO
UINT _gx_system_dirty_partial_add(GX_WIDGET* widget, const GX_RECTANGLE* dirty_area)
{
    if (!widget)
    {
        return GX_PTR_ERROR;
    }

    if (!(widget->status & GX_STATUS_VISIBLE))
    {
        return GX_PTR_ERROR;
    }

    const GX_RECTANGLE& dirty = dirty_area ? *dirty_area : widget->size;

    {
        GX_SYSTEM_AUTO_LOCK lock;

        // if a widget is transparent, we actually need to
        // mark its first non-transparent parent as being dirty
        while ((widget->status & GX_STATUS_TRANSPARENT) && widget->parent)
        {
            widget = widget->parent;
        }

        GX_WINDOW_ROOT* root = widget->root_window_();
        if (!root || !root->canvas)
        {
            return GX_PTR_ERROR;
        }

        GX_CANVAS* canvas = root->canvas;

        if (canvas->dirty_list_is_full_())
        {
            widget = root;
        }

        canvas->dirty_list_add_(widget, dirty);
    }

    return GX_SUCCESS;
}
#endif // GUIX_STUDIO

/**************************************************************************/
void _gx_system_dirty_entry_shift(GX_WIDGET* widget, INT xShift, INT yShift)
{
    if (!widget)
    {
        return;
    }

    if ((widget->status & (GX_STATUS_VISIBLE | GX_STATUS_DIRTY)) != (GX_STATUS_VISIBLE | GX_STATUS_DIRTY))
    {
        return;
    }

    {
        GX_SYSTEM_AUTO_LOCK lock;

        GX_WINDOW_ROOT* root = widget->root_window_();
        if (root && root->canvas)
        {
            root->canvas->dirty_list_entry_shift_(widget, xShift, yShift);
        }
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_dirty_list_remove                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function removes a widget from the dirty list.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    remove                                Widget to be removed          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_child_detect               Detect a child widget         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_dirty_list_remove          Remove widget from dirty list */
/*                                                                        */
/**************************************************************************/
void _gx_system_dirty_list_remove(GX_WIDGET* removed)
{
    GX_CANVAS* canvas = _gx_system_canvas_created_list;
    while (canvas)
    {
        canvas->dirty_list_remove_(removed);

        canvas = canvas->created_next;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_all_canvas_dirty                                         */
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
/*    None                                                                */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
void _gx_system_all_canvas_dirty(void)
{
    GX_WIDGET* root = _gx_system_root_window_created_list;
    for (; root; root = root->next)
    {
        if (root->is_visible_())
        {
            root->dirty_mark_();

            for (GX_WIDGET* child = root->first_child; child; child = child->next)
            {
                child->dirty_mark_();
            }
        }
    }
}

/**************************************************************************/
static UINT _gx_system_widget_draw(GX_CANVAS* canvas,
                                   GX_WIDGET* drawit, const GX_RECTANGLE& dirty_area,
                                   GX_BOOL flush)
{
    GX_DRAWING drawing(canvas, drawit, dirty_area, flush);

    switch (drawing.result_())
    {
    case GX_SUCCESS:
        drawit->draw_function(drawit);
        break;

    case GX_NO_VIEWS:
        // If we are attempting to draw the root window and it has no views,
        // just draw the children of the root
        if (drawit->is_root_window_())
        {
            drawit->children_draw_();
        }
        break;

    default:
        break;
    }

    return drawing.result_();
}

/**************************************************************************/
static UINT _gx_system_canvas_draw(GX_WINDOW_ROOT* root)
{
    UINT result = GX_SUCCESS;
    GX_CANVAS* canvas = root->canvas;
    GX_RECTANGLE dirty_sum;

    // Trim any redundant dirty areas prior to doing the update.
    if (canvas->dirty_list_trim_(&dirty_sum))
    {
        GX_DRAWING drawing(canvas, root, dirty_sum);

        result = drawing.result_();
        if (result == GX_SUCCESS || result == GX_NO_VIEWS)
        {
            GX_DIRTY_AREA* dirty = canvas->dirty_list;
            GX_DIRTY_AREA* dirty_end = dirty + canvas->dirty_count;

            for (; dirty < dirty_end; ++dirty)
            {
                GX_WIDGET* drawit = dirty->widget;
                if (drawit)
                {
                    result = _gx_system_widget_draw(canvas, drawit, dirty->rectangle, GX_FALSE);
                }
            }
        }
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_canvas_draw_partial                                      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws the dirty area of the specified canvas with     */
/*    partial canvas buffer and toggle the dirty area to the display or   */
/*    a composite canvas.                                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_canvas_refresh                                           */
/*                                                                        */
/**************************************************************************/
#ifdef GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
static UINT _gx_system_canvas_draw_partial(GX_WINDOW_ROOT* root)
{
    UINT           status = GX_SUCCESS;
    GX_CANVAS*     canvas = root->canvas;
    GX_RECTANGLE   dirty_sum;
    GX_RECTANGLE   dirty_frame;
    GX_WIDGET*     drawit;
    GX_DIRTY_AREA* dirty_list_entry;
    GX_VALUE       dirty_width;
    GX_VALUE       dirty_height;

#if defined(GX_CANVAS_REFRESH_DIRECTION_HORIZONTAL) || defined(GX_CANVAS_REFRESH_DIRECTION_VERTICAL)
    GX_RECTANGLE dirty_mask;
    if (!canvas->dirty_list_trim_(&dirty_sum))
    {
        return GX_SUCCESS;
    }

    dirty_mask = dirty_sum;
#endif

    // Refresh canvas in the dirty area.
    if (canvas->draw_count > 0)
    {
        _gx_system_dirty_partial_add((GX_WIDGET*)root, &canvas->dirty_area);
        canvas->draw_count = 0;
    }

#if defined(GX_CANVAS_REFRESH_DIRECTION_HORIZONTAL)
    dirty_height = dirty_sum.height_();
    dirty_width = (GX_VALUE)(canvas->memory_size / canvas->display->driver_row_pitch_get((USHORT)dirty_height));
    dirty_width = (GX_VALUE)(dirty_width & 0xFFFC);
    dirty_mask.right = (GX_VALUE)(dirty_mask.left + dirty_width - 1);

    while (dirty_mask.left <= dirty_sum.right)
    {
        if (dirty_mask.right > dirty_sum.right)
        {
            dirty_mask.right = dirty_sum.right;
        }
#elif defined(GX_CANVAS_REFRESH_DIRECTION_VERTICAL)
    dirty_width = dirty_sum.width_();
    dirty_height = (GX_VALUE)(canvas->memory_size / canvas->display->driver_row_pitch_get((USHORT)((dirty_width + 3) & 0xFFFC)));
    dirty_mask.bottom = (GX_VALUE)(dirty_mask.top + dirty_height - 1);

    while (dirty_mask.top <= dirty_sum.bottom)
    {
        if (dirty_mask.bottom > dirty_sum.bottom)
        {
            dirty_mask.bottom = dirty_sum.bottom;
        }
#endif

        // Initialize dirty area pointers.
        dirty_list_entry = canvas->dirty_list;
        dirty_list_end = dirty_list_entry + canvas->dirty_count;

        // Loop through dirty areas to redraw as needed.
        for (; dirty_list_entry < dirty_list_end; ++dirty_list_entry)
        {
            drawit = dirty_list_entry->widget;

            if (drawit && drawit->is_visible_())
            {
                if (drawit->style & (GX_STYLE_BORDER_RAISED | GX_STYLE_BORDER_RECESSED))
                {
                    while (drawit->parent)
                    {
                        drawit = drawit->parent;

                        if (!(drawit->status & GX_STATUS_TRANSPARENT))
                        {
                            // we need to start drawing at this non-transparent background widget
                            drawit->status |= GX_STATUS_DIRTY;
                            break;
                        }
                    }
                }

#if defined(GX_CANVAS_REFRESH_DIRECTION_HORIZONTAL) || defined(GX_CANVAS_REFRESH_DIRECTION_VERTICAL)
                if (gx_rectangle_intersect_(dirty_list_entry->rectangle, dirty_mask, &dirty_frame) &&
                    gx_rectangle_intersect_(dirty_frame, drawit->clip, &dirty_frame))
                {
                    _gx_system_widget_draw(canvas, drawit, dirty_frame, GX_TRUE);
                }
#else
                dirty_sum = dirty_list_entry->rectangle;
                if (gx_rectangle_intersect_(dirty_sum, drawit->clip, &dirty_sum))
                {
                    dirty_frame = dirty_sum;

                    // Split dirty area into small pieces.
                    dirty_width = dirty_frame.width_();
                    dirty_height = (GX_VALUE)(canvas->memory_size / canvas->display->driver_row_pitch_get((USHORT)((dirty_width + 3) & 0xFFFC)));
                    dirty_frame.bottom = (GX_VALUE)(dirty_frame.top + dirty_height - 1);

                    while (dirty_frame.top <= dirty_sum.bottom)
                    {
                        if (dirty_frame.bottom > dirty_sum.bottom)
                        {
                            dirty_frame.bottom = dirty_sum.bottom;
                        }

                        _gx_system_widget_draw(canvas, drawit, dirty_frame, GX_TRUE);

                        dirty_frame.move_(0, dirty_height);
                    }
                }
#endif
            }
        }

#ifdef GX_CANVAS_REFRESH_DIRECTION_HORIZONTAL
        dirty_mask.move_(dirty_width, 0);
    }
#elif defined(GX_CANVAS_REFRESH_DIRECTION_VERTICAL)
        dirty_mask.move_(0, dirty_height);
    }
#endif

    canvas->dirty_count = 0;

    return GX_SUCCESS;
}
#endif // GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_system_canvas_refresh                                           */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function refreshes the screen(s) of GUIX.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_system_canvas_refresh(void)
{
    UINT status = GX_SUCCESS;

    if (!_gx_system_canvas_created_list)
    {
        return GX_INVALID_CANVAS;
    }

    {
        GX_SYSTEM_AUTO_LOCK lock;

        GX_WINDOW_ROOT* root = _gx_system_root_window_created_list;
        for (; root; root = (GX_WINDOW_ROOT*)root->next)
        {
            if (!root->is_visible_())
            {
                continue;
            }

            if (root->views_changed)
            {
                root->all_views_update_();
            }

        #ifdef GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
            if (root->canvas->status & GX_CANVAS_PARTIAL_FRAME_BUFFER)
            {
                status = _gx_system_canvas_draw_partial(root);
                continue;
            }
        #endif

            status = _gx_system_canvas_draw(root);
        }

        GX_CANVAS* canvas = _gx_canvas_composite_create();
        if (canvas)
        {
            if (canvas->draw_count > 0)
            {
                canvas->display->driver_buffer_toggle(canvas, &canvas->dirty_area);
                canvas->draw_count = 0;
            }
        }
        else
        {
            root = _gx_system_root_window_created_list;
            for (; root; root = (GX_WINDOW_ROOT*)root->next)
            {
                canvas = root->canvas;

                canvas->dirty_count = 0;

                if (root->is_visible_())
                {
                    if (canvas->draw_count > 0)
                    {
                        if ((canvas->status & GX_CANVAS_MANAGED_VISIBLE) == GX_CANVAS_MANAGED_VISIBLE)
                        {
                            canvas->display->driver_buffer_toggle(canvas, &canvas->dirty_area);
                        }
                    }
                }

                canvas->draw_count = 0;
            }
        }
    }

    return status;
}
