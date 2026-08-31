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
/**   Canvas Management (Canvas)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_canvas.h"

#include "gx_utility.h"
#include "gx_system.h"
#include "gx_display.h"
#include "gx_font.h"
#include "gx_window_root.h"

static void _gx_canvas_glyphs_draw(GX_DRAW_CONTEXT* context, const GX_POINT* draw_position,
                                   const GX_STRING* string, const GX_RECTANGLE* view,
                                   void(*draw_glyph)(GX_DRAW_CONTEXT*, GX_RECTANGLE*, GX_POINT*, const GX_GLYPH*));

#ifdef GX_TARGET_WIN32

void _win32_compatible_canvas_memory_allocate(GX_CANVAS* canvas)
{
    // Windows bitmaps must be padded to an even multiple of 4 bytes in width.
    // When the GUIX canvas does not meet this requirement, we create a padded canvas memory
    // so that we can pass the padded canvas memory off to Windows for display as a bitmap.
    // This happens often when running at sub-byte color depth, but can occur at any color depth
    // if the display resolution is very odd. */

    GX_DISPLAY* display = canvas->display;
    if (display == GX_NULL)
    {
        return;
    }

    INT padded_width = canvas->x_resolution;

    USHORT row_byte_width = display->driver_row_pitch_get((USHORT)padded_width);
    while (row_byte_width % 4)
    {
        ++padded_width;
        row_byte_width = display->driver_row_pitch_get((USHORT)padded_width);
    }

    if ((padded_width != canvas->x_resolution) || display->rotation_angle)
    {
        // We are forced to create a padded buffer to hold Win32 compatible canvas memory.
        canvas->padded_memory = (GX_COLOR*)malloc(row_byte_width * canvas->y_resolution);
    }
}

#endif // GX_TARGET_WIN32

UINT GX_CANVAS::create_(const GX_CHAR* name, GX_DISPLAY* display,
                        UINT type, UINT width, UINT height,
                        GX_COLOR* memory_area, ULONG memory_size)
{
    memset(this, 0, sizeof(GX_CANVAS));

    this->id = GX_CANVAS_ID;
    this->name = name;

    this->display = display;

    this->memory = memory_area;
    this->memory_size = memory_size;

    this->alpha = GX_ALPHA_VALUE_OPAQUE;
    this->draw_count = 0;
    this->draw_nesting = 0;
    this->dirty_count = 0;
    this->status = type;

    this->display_offset_x = 0;
    this->display_offset_y = 0;
    this->x_resolution = (GX_VALUE)width;
    this->y_resolution = (GX_VALUE)height;

#ifdef GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
    this->memory_width = (GX_VALUE)width;
    this->memory_height =  (GX_VALUE)height;
#endif

    this->hardware_layer = (GX_BYTE)-1;

    if (memory_area)
    {
        memset(memory_area, 0, memory_size);
    }

    this->dirty_area.set_(0, 0, -1, -1);

    // If running on Win32, create padded memory, only if needed
#ifdef GX_TARGET_WIN32
    _win32_compatible_canvas_memory_allocate(this);
#endif

    // Now link the canvas control block on the list of created canvases.

    this->created_previous = GX_NULL;
    this->created_next = GX_NULL;

    GX_ENTER_CRITICAL

    ++_gx_system_canvas_created_count;

    if (_gx_system_canvas_created_count > 1)
    {
        _gx_system_canvas_created_list->created_previous = this;
        this->created_next = _gx_system_canvas_created_list;
    }

    _gx_system_canvas_created_list = this;

    GX_EXIT_CRITICAL

    return GX_SUCCESS;
}

UINT GX_CANVAS::delete_()
{
    GX_ENTER_CRITICAL

    if (_gx_system_canvas_created_count > 0)
    {
        if (created_previous)
        {
            created_previous->created_next = created_next;
        }
        else
        {
            _gx_system_canvas_created_list = created_next;
        }

        if (created_next)
        {
            created_next->created_previous = created_previous;
        }

        --_gx_system_canvas_created_count;

        // Only for Win32, we sometimes have to create module 4 aligned scratch memory buffer
    #ifdef GX_TARGET_WIN32
        if (padded_memory)
        {
            free(padded_memory);
        }
    #endif

        memset(this, 0, sizeof(GX_CANVAS));
    }

    GX_EXIT_CRITICAL

    return GX_SUCCESS;
}

UINT GX_CANVAS::memory_set_(GX_COLOR* memory, ULONG memsize)
{
#ifdef GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER

    if (!display)
    {
        return GX_INVALID_DISPLAY;
    }

    ULONG canvas_size = (ULONG)(display->driver_row_pitch_get((USHORT)x_resolution) * y_resolution);

    if (memsize < canvas_size)
    {
        status |= GX_CANVAS_PARTIAL_FRAME_BUFFER;
    }

#endif // GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER

    this->memory = memory;
    this->memory_size = memsize;

    dirty_mark_();

    return GX_SUCCESS;
}

UINT GX_CANVAS::offset_set_(GX_VALUE x_offset, GX_VALUE y_offset)
{
    if (hardware_layer >= 0)
    {
        void (*layer_offset_set)(INT layer, GX_VALUE x, GX_VALUE y);

        layer_offset_set = display->layer_services->offset_set;
        if (layer_offset_set)
        {
            layer_offset_set(hardware_layer, x_offset, y_offset);

            this->display_offset_x = x_offset;
            this->display_offset_y = y_offset;

            return GX_SUCCESS;
        }
    }

    if ((status & GX_CANVAS_MANAGED) && created_next)
    {
        // find the bottom layer canvas
        GX_CANVAS* back_canvas = created_next;
        while (back_canvas->created_next)
        {
            back_canvas = back_canvas->created_next;
        }

        GX_RECTANGLE old_area;
        display_area_(&old_area);

        if (back_canvas->draw_count > 0)
        {
            old_area.combine_(back_canvas->dirty_area);
        }

        // mark the background canvas as dirty
        back_canvas->dirty_mark_(&old_area);
    }

    this->display_offset_x = x_offset;
    this->display_offset_y = y_offset;

    // mark the foreground canvas as dirty
    dirty_mark_();

    return GX_SUCCESS;
}

UINT GX_CANVAS::alpha_set_(GX_UBYTE alpha)
{
    this->alpha = alpha;

    if (hardware_layer >= 0)
    {
        void (*layer_alpha_set)(INT layer, GX_UBYTE alpha);

        layer_alpha_set = display->layer_services->alpha_set;
        if (layer_alpha_set)
        {
            layer_alpha_set(hardware_layer, alpha);

            return GX_SUCCESS;
        }
    }

    dirty_mark_();

    return GX_SUCCESS;
}

UINT GX_CANVAS::hardware_layer_bind_(INT layer)
{
    UINT (*layer_initialize)(INT, GX_CANVAS*);

    layer_initialize = display->layer_services->initialize;
    if (!layer_initialize)
    {
        return GX_INVALID_DISPLAY;
    }

    UINT result = layer_initialize(layer, this);
    if (result == GX_SUCCESS)
    {
        hardware_layer = (GX_BYTE)layer;
    }

    return result;
}

UINT GX_CANVAS::shift_(GX_VALUE dx, GX_VALUE dy)
{
    GX_VALUE offset_x = (GX_VALUE)(display_offset_x + dx);
    GX_VALUE offset_y = (GX_VALUE)(display_offset_y + dy);
    return offset_set_(offset_x, offset_y);
}

void GX_CANVAS::dirty_mark_(const GX_RECTANGLE* area)
{
    ++draw_count;

    if (area)
    {
        dirty_area = *area;
    }
    else
    {
        dirty_area.set_(0, 0, (GX_VALUE)(x_resolution - 1), (GX_VALUE)(y_resolution - 1));
    }
}

bool GX_CANVAS::dirty_list_add_(GX_WIDGET* widget, const GX_RECTANGLE& dirty_rect)
{
    if (widget->status & GX_STATUS_DIRTY)
    {
        GX_DIRTY_AREA* dirty_end = dirty_list + dirty_count;

        for (GX_DIRTY_AREA* dirty = dirty_list; dirty < dirty_end; ++dirty)
        {
            if (dirty->widget == widget)
            {
                if (dirty_list_is_full_() ||
                    gx_rectangle_intersect_(dirty->rectangle, dirty_rect))
                {
                    dirty->rectangle.combine_(dirty_rect);
                    return true;
                }
            }
        }
    }

    if (dirty_list_is_full_())
    {
        return false;
    }

    GX_DIRTY_AREA& dirty = dirty_list[dirty_count];

    widget->status |= GX_STATUS_DIRTY;

    dirty.widget = widget;
    dirty.rectangle = dirty_rect;

    ++dirty_count;

    return true;
}

void GX_CANVAS::dirty_list_entry_shift_(GX_WIDGET* widget, INT dx, INT dy)
{
    GX_DIRTY_AREA* dirty_end = dirty_list + dirty_count;

    for (GX_DIRTY_AREA* dirty = dirty_list; dirty < dirty_end; ++dirty)
    {
        if (dirty->widget == widget)
        {
            dirty->rectangle.move_((GX_VALUE)dx, (GX_VALUE)dy);
        }
    }
}

void GX_CANVAS::dirty_list_remove_(GX_WIDGET* removed)
{
    GX_DIRTY_AREA* dirty_end = dirty_list + dirty_count;

    for (GX_DIRTY_AREA* dirty = dirty_list; dirty < dirty_end; ++dirty)
    {
        if (dirty->widget && dirty->widget == removed)
        {
            dirty->widget = GX_NULL;
        }

        // No need to test for the dirty list entry being a child of the widget being deleted,
        // since child widgets are always deleted before the parent.
    }
}

static void gx_dirty_overlap_check_(GX_DIRTY_AREA* dirty)
{
    GX_WIDGET* widget = dirty->widget;
    GX_WIDGET* parent = widget->parent;

    while (parent)
    {
        // check to see if this is a viewport owner
        if (dirty->widget->is_window_() && parent->is_root_window_())
        {
            // this widget is a view owner, no need to go further
            return;
        }

        // not a view owner, so check to see if it has overlapping siblings
        for (GX_WIDGET* sibling = widget->next; sibling; sibling = sibling->next)
        {
            if (gx_rectangle_intersect_(sibling->size, dirty->rectangle))
            {
                // mark the parent as dirty, so that both children are drawn
                dirty->widget = parent;
                break;
            }
        }

        widget = parent;
        parent = widget->parent;
    }
}

bool GX_CANVAS::dirty_list_trim_(GX_RECTANGLE* dirty_rect)
{
    dirty_rect->set_(x_resolution, y_resolution, -1, -1);

    GX_DIRTY_AREA* dirty_end = dirty_list + dirty_count;

    // This loop looks for invalid entries (invisible) and
    // entries that have overlappers in the z order, in
    // which case we need to back up the drawing layer
    for (GX_DIRTY_AREA* dirty = dirty_list; dirty < dirty_end; ++dirty)
    {
        if (dirty->widget)
        {
            if (dirty->widget->is_visible_())
            {
                gx_dirty_overlap_check_(dirty);
            }
            else
            {
                dirty->widget = GX_NULL;
            }
        }
    }

    // This loop tests to see if two entries is for same widget or
    // if a dirty entry also has a parent that is marked as dirty.
    // If the dirty rectangles overlap, just combine the rectangles
    // and get rid of the entry
    for (GX_DIRTY_AREA* dirty = dirty_list; dirty < dirty_end; ++dirty)
    {
        if (!dirty->widget)
        {
            continue;
        }

        for (GX_DIRTY_AREA* dirty2 = dirty_list; dirty2 < dirty_end; ++dirty2)
        {
            if (!dirty2->widget || dirty == dirty2)
            {
                continue;
            }

            if (dirty->widget == dirty2->widget ||
                dirty->widget->is_ancestor_of_(dirty2->widget))
            {
                if (gx_rectangle_intersect_(dirty->rectangle, dirty2->rectangle))
                {
                    dirty->rectangle.combine_(dirty2->rectangle);
                    dirty2->widget = GX_NULL;
                }
            }
        }

        dirty_rect->combine_(dirty->rectangle);
    }

    return dirty_rect->is_valid_();
}

static GX_WINDOW* gx_canvas_find_viewport_(GX_WIDGET* who)
{
    if (who->is_root_window_())
    {
        return (GX_WINDOW*)who;
    }

    while (who->parent)
    {
        if (who->parent->is_root_window_())
        {
            if (who->is_window_() &&
                !(who->status & GX_STATUS_TRANSPARENT))
            {
                return (GX_WINDOW*)who;
            }
            else
            {
                return (GX_WINDOW*)who->parent;
            }
        }

        who = who->parent;
    }

    return GX_NULL;
}

UINT GX_CANVAS::drawing_initiate_(GX_WIDGET* who, const GX_RECTANGLE& dirty_rect)
{
    GX_DRAW_CONTEXT* current_context = _gx_system_current_draw_context;

    // allocate a new context

    GX_DRAW_CONTEXT* new_context;
    if (current_context)
    {
        new_context = current_context - 1;

        if (new_context < _gx_system_draw_context_stack)
        {
            _gx_system_error_process(GX_DRAW_NESTING_EXCEEDED);

            return GX_DRAW_NESTING_EXCEEDED;
        }
    }
    else
    {
        new_context = _gx_system_draw_context_stack_end - 1;
    }

    // set the dirty area of this canvas

    if (draw_count > 0)
    {
        this->dirty_area.combine_(dirty_rect);
    }
    else
    {
        this->dirty_area = dirty_rect;

#ifdef GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
        if (status & GX_CANVAS_PARTIAL_FRAME_BUFFER)
        {
            memory_width = dirty_rect.width_();
            memory_width = (GX_VALUE)((memory_width + 3) & 0xFFFC);
            memory_height = (GX_VALUE)(memory_size / display->driver_row_pitch_get((USHORT)memory_width));
            if (memory_height < dirty_rect.height_())
            {
                return GX_INVALID_MEMORY_SIZE;
            }
            memory_offset_x = dirty_rect.left;
            memory_offset_y = dirty_rect.top;
        }
#endif // GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
    }

    // initialize the new context

    if (draw_nesting > 0 && current_context)
    {
        *new_context = *current_context;
    }
    else
    {
        new_context->brush_default_init_();

        new_context->canvas = this;
        new_context->display = this->display;
        new_context->memory = this->memory;

        if (new_context->display->rotation_angle == GX_SCREEN_ROTATION_NONE ||
            new_context->display->rotation_angle == GX_SCREEN_ROTATION_FLIP)
        {
#ifdef GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER
            if (status & GX_CANVAS_PARTIAL_FRAME_BUFFER)
            {
                new_context->pitch = memory_width;
                new_context->offset_x = memory_offset_x;
                new_context->offset_y = memory_offset_y;
            }
            else
            {
                new_context->pitch = x_resolution;
                new_context->offset_x = 0;
                new_context->offset_y = 0;
            }

#else
            new_context->pitch = x_resolution;
#endif
        }
        else
        {
            new_context->pitch = y_resolution;
        }
    }

    ++draw_nesting;
    ++draw_count;

    if (display->driver_drawing_initiate)
    {
        display->driver_drawing_initiate(display, this);
    }

    // set the dirty area of new context

    if (who)
    {
        gx_rectangle_intersect_(dirty_rect, who->clip, &new_context->dirty);

#if defined(GX_BRUSH_ALPHA_SUPPORT)
        if (who->style & GX_STYLE_USE_LOCAL_ALPHA)
        {
            new_context->brush.alpha = who->alpha;
        }
#endif
    }
    else
    {
        new_context->dirty = dirty_rect;
    }

    // set the view list of new context

    if (status & GX_CANVAS_SIMPLE)
    {
        new_context->simple_view.rectangle = new_context->dirty;
        new_context->simple_view.next = GX_NULL;

        new_context->view_head = &new_context->simple_view;
    }
    else
    {
        new_context->view_head = GX_NULL;

        if (who)
        {
            GX_WINDOW* win = gx_canvas_find_viewport_(who);
            if (win)
            {
                new_context->view_head = win->views;
            }

            // the widget being drawn is no longer marked dirty
            who->status &= ~GX_STATUS_DIRTY;
        }
    }

    _gx_system_current_draw_context = new_context;

    if (!new_context->view_head)
    {
        return GX_NO_VIEWS;
    }

    return GX_SUCCESS;
}

UINT GX_CANVAS::drawing_complete_(GX_BOOL flush)
{
    if (draw_nesting > 0)
    {
        if (display->driver_drawing_complete)
        {
            display->driver_drawing_complete(display, this);
        }

        draw_nesting = (GX_UBYTE)(draw_nesting - 1);

        // pop the previous draw context
        ++_gx_system_current_draw_context;
        if (_gx_system_current_draw_context == _gx_system_draw_context_stack_end)
        {
            _gx_system_current_draw_context = GX_NULL;
        }

        if (draw_nesting == 0 && flush)
        {
            display->driver_buffer_toggle(this, &dirty_area);

            draw_count = 0; // reset the canvas dirty count
        }
    }

    return GX_SUCCESS;
}

UINT GX_CANVAS::show_()
{
    status |= GX_CANVAS_VISIBLE;

    if (hardware_layer >= 0)
    {
        void (*layer_show)(INT layer);

        layer_show = display->layer_services->show;
        if (layer_show)
        {
            layer_show(hardware_layer);

            return GX_SUCCESS;
        }
    }

    dirty_mark_();

    return GX_SUCCESS;
}

UINT GX_CANVAS::hide_()
{
    status &= ~GX_CANVAS_VISIBLE;

    if (hardware_layer >= 0)
    {
        void (*layer_hide)(INT layer);

        layer_hide = display->layer_services->hide;
        if (layer_hide)
        {
            layer_hide(hardware_layer);

            return GX_SUCCESS;
        }
    }

    GX_RECTANGLE dirty;
    this->display_area_(&dirty);

    GX_RECTANGLE dirty_sum;

    GX_CANVAS* canvas = _gx_system_canvas_created_list;
    for (; canvas; canvas = canvas->created_next)
    {
        if (canvas->status & GX_CANVAS_VISIBLE)
        {
            canvas->display_area_(&dirty_sum);
            dirty_sum.combine_(dirty);

            canvas->dirty_mark_(&dirty_sum);
        }
    }

    return GX_SUCCESS;
}

#if defined(GX_MOUSE_SUPPORT)

UINT GX_CANVAS::mouse_define_(GX_MOUSE_CURSOR_INFO* info)
{
    if (!info)
    {
        return GX_FAILURE;
    }

    display->mouse_define(display, this, info);

    return GX_SUCCESS;
}

UINT GX_CANVAS::mouse_show_()
{
#if defined(GX_ENABLE_CANVAS_PARTIAL_FRAME_BUFFER)
    if (status & GX_CANVAS_PARTIAL_FRAME_BUFFER)
    {
        return GX_FAILURE; // Not supported.
    }
#endif

    GX_DISPLAY_MOUSE& mouse = display->mouse;

    if (!mouse.cursor_info || (mouse.canvas != this) || !display->mouse_enable)
    {
        return GX_FAILURE;
    }

    display->mouse_enable(display, GX_TRUE);

    return GX_SUCCESS;
}

UINT GX_CANVAS::mouse_hide_()
{
    if ((display->mouse.canvas != this) || !display->mouse_enable)
    {
        return GX_FAILURE;
    }

    display->mouse_enable(display, GX_FALSE);

    return GX_SUCCESS;
}

#endif // GX_MOUSE_SUPPORT

void GX_CANVAS::clamp_(GX_RECTANGLE* rect) const
{
    if (rect->left < 0)
    {
        rect->left = 0;
    }

    if (rect->top < 0)
    {
        rect->top = 0;
    }

    if (rect->right >= x_resolution)
    {
        rect->right = (GX_VALUE)(x_resolution - 1);
    }

    if (rect->bottom >= y_resolution)
    {
        rect->bottom = (GX_VALUE)(y_resolution - 1);
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_create                                                   */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a canvas associated with the specified        */
/*    display.                                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    name                                  Name of canvas                */
/*    display                               Display control block         */
/*    type                                  Type of canvas                */
/*    width                                 Width of canvas               */
/*    height                                Height of canvas              */
/*    memory_area                           Memory area of canvas with    */
/*                                            each pixel of GX_COLOR      */
/*    memory_size                           Size of canvas memory area    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memset                                Set control block and canvas  */
/*                                            memory to zero              */
/*    _gx_utility_rectangle_define          Define a rectangle            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    _gx_animation_canvas_define                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_create(GX_CANVAS* canvas, const GX_CHAR* name, GX_DISPLAY* display,
                       UINT type, UINT width, UINT height, GX_COLOR* memory_area, ULONG memory_size)
{
    return canvas->create_(name, display, type, width, height, memory_area, memory_size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_delete                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deletes a canvas.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    memset                                Set control block and canvas  */
/*                                            memory to zero              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_delete(GX_CANVAS* canvas)
{
    return canvas->delete_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_memory_define                                            */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns canvas memory pointer and size                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    memory                                Canvas memory address         */
/*    memsize                               Canvas memory size            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_dirty_mark                 Set the canvas dirty flag     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application software                                                */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_memory_define(GX_CANVAS* canvas, GX_COLOR* memory, ULONG memsize)
{
    return canvas->memory_set_(memory, memsize);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_offset_set                                               */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns a canvas x,y display offset                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    x_offset                               X coordinate of offset        */
/*    y_offset                               Y coordinate of offset        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    gx_canvas_dirty_mark                  mark canvas as needing refresh*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_animation_start                                                 */
/*    _gx_animation_update                                                */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_offset_set(GX_CANVAS* canvas, GX_VALUE x_offset, GX_VALUE y_offset)
{
    return canvas->offset_set_(x_offset, y_offset);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_alpha_set                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns the alpha-blend value of a canvas             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    alpha                                 alpha value                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_dirty_mark                 Set the canvas dirty flag     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_animation_start                                                 */
/*    _gx_animation_update                                                */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_alpha_set(GX_CANVAS* canvas, GX_UBYTE alpha)
{
    return canvas->alpha_set_(alpha);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_hardware_layer_bind                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function binds a GUIX canvas to a hardware graphics layer      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    layer                                 Hardware layer                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_dirty_mark                 Set the canvas dirty flag     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_hardware_layer_bind(GX_CANVAS* canvas, INT layer)
{
    return canvas->hardware_layer_bind_(layer);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_shift                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function moves the canvas associated with a root window        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    dx                                    x-axis shift amount in pixels */
/*    dy                                    y-axis shift amount in pixels */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*   gx_canvas_offset_set                   Set the canvas offset         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_window_root_event_process                                       */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_shift(GX_CANVAS* canvas, GX_VALUE dx, GX_VALUE dy)
{
    return canvas->shift_(dx, dy);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_drawing_initiate                                         */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initiates drawing on the specified canvas.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    who                                   Widget control block pointer  */
/*    dirty_area                            Area to draw on               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define          Define rectangle area.        */
/*    _gx_utility_rectangle_combine         Combine adjacent dirty areas. */
/*    _gx_context_brush_default             Define a brush                */
/*    _gx_system_error_process              Process an error              */
/*    _gx_utility_rectangle_overlap_detect  Detects two rectangles being  */
/*                                            overlap                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    _gx_multi_line_text_view_draw                                       */
/*    _gx_single_line_text_view_draw                                      */
/*    _gx_system_canvas_refresh                                           */
/*    _gx_widget_block_move                                               */
/*    _gx_widget_children_draw                                            */
/*    _gx_window_client_scroll                                            */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_drawing_initiate(GX_CANVAS* canvas, GX_WIDGET* who, GX_RECTANGLE* dirty_area)
{
    return canvas->drawing_initiate_(who, *dirty_area);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_drawing_complete                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function completes drawing on the specified canvas.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    flush                                 If GX_TRUE, the content of    */
/*                                            the canvas is flushed to    */
/*                                            the display                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_buffer_toggle]     Toggle visible frame buffer   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_drawing_complete(GX_CANVAS* canvas, GX_BOOL flush)
{
    return canvas->drawing_complete_(flush);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_show                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function makes a canvas visible                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_dirty_mark                 Set the canvas dirty flag     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_show(GX_CANVAS* canvas)
{
    return canvas->show_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_hide                                                     */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function makes a canvas invisible                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_canvas_dirty_mark                 Set the canvas dirty flag     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_hide(GX_CANVAS* canvas)
{
    return canvas->hide_();
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_mouse_define                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service defines mouse information for current display.         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    info                                  Mouse cursor info control     */
/*                                              block                     */
/*                                                                        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_mouse_define]             Define mouse info for driver  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_MOUSE_SUPPORT)
UINT _gx_canvas_mouse_define(GX_CANVAS* canvas, GX_MOUSE_CURSOR_INFO* info)
{
    return canvas->mouse_define_(info);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_mouse_show                                               */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service makes the mouse visible.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_mouse_enable]             enabled or disabled mouse     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_MOUSE_SUPPORT)
UINT _gx_canvas_mouse_show(GX_CANVAS* canvas)
{
    return canvas->mouse_show_();
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_mouse_hide                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service makes the mouse hidden.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_mouse_enable]             enabled or disabled mouse     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_MOUSE_SUPPORT)
UINT _gx_canvas_mouse_hide(GX_CANVAS* canvas)
{
    return canvas->mouse_hide_();
}
#endif

/**************************************************************************/

static inline void gx_canvas_angle_normalize_(INT& angle)
{
    while (angle < 0)
    {
        angle += 360;
    }

    if (angle >= 360)
    {
        angle %= 360;
    }
}

static void gx_canvas_angle_normalize_(INT& start_angle, INT& end_angle)
{
    gx_canvas_angle_normalize_(start_angle);
    gx_canvas_angle_normalize_(end_angle);

    if (end_angle <= start_angle)
    {
        end_angle += 360;
    }
}

static inline void gx_canvas_ellipse_bound_get_(UINT brush_width,
                                                INT xcenter, INT ycenter,
                                                UINT a, UINT b,
                                                GX_RECTANGLE* bound)
{
    UINT brush_half_width = ((brush_width + 1) / 2);

    a += brush_half_width;
    b += brush_half_width;

    bound->set_((GX_VALUE)((UINT)xcenter - a),
                 (GX_VALUE)((UINT)ycenter - b),
                 (GX_VALUE)((UINT)xcenter + a),
                 (GX_VALUE)((UINT)ycenter + b));
}

static inline void gx_canvas_circle_bound_get_(UINT brush_width,
                                               INT xcenter, INT ycenter,
                                               UINT r,
                                               GX_RECTANGLE* bound)
{
    gx_canvas_ellipse_bound_get_(brush_width, xcenter, ycenter, r, r, bound);
}

static inline void gx_canvas_draw_radius_(INT xcenter, INT ycenter, UINT r, INT angle)
{
    GX_POINT point;
    _gx_utility_circle_point_get(xcenter, ycenter, r, angle, &point);
    _gx_canvas_line_draw((GX_VALUE)xcenter, (GX_VALUE)ycenter, point.x, point.y);
}

typedef void (*gx_arc_draw_fptr)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);

static gx_arc_draw_fptr gx_arc_draw_function_get_(UINT brush_width, UINT brush_style, const GX_DISPLAY* display)
{
    if (brush_width == 1)
    {
        if ((brush_style & GX_BRUSH_ALIAS) && display->driver_anti_aliased_arc_draw)
        {
            return display->driver_anti_aliased_arc_draw;
        }
        else
        {
            return display->driver_arc_draw;
        }
    }
    else if (brush_width > 1)
    {
        if ((brush_style & GX_BRUSH_ALIAS) && display->driver_anti_aliased_wide_arc_draw)
        {
            return display->driver_anti_aliased_wide_arc_draw;
        }
        else
        {
            return display->driver_wide_arc_draw;
        }
    }
    else
    {
        return GX_NULL; // no outline
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_pixel_draw                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function prepares to draw one pixel using current context      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    position                              x,y coordinate to draw        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_point_detect    Detect whether a pixel is     */
/*                                            inside rectangle            */
/*    [gx_display_driver_pixel_write]       Actually write to canvas      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    _gx_widget_border_draw                                              */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_pixel_draw(GX_POINT pos)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->brush.alpha;
    if (brush_alpha == 0)
    {
        return GX_SUCCESS;
    }
#endif

    if (!context->dirty.contain_point_(pos))
    {
        return GX_SUCCESS;
    }

    GX_COLOR pixel_color = context->brush.line_color;

    GX_DISPLAY* display = context->display;

    // test to determine if the bounding rectangle overlaps the region we are allowed to draw
    // into. For each view that overlaps the bounding rectangle, do some drawing.

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (view->rectangle.contain_point_(pos))
        {
#if defined(GX_BRUSH_ALPHA_SUPPORT)

            if (brush_alpha == GX_ALPHA_VALUE_OPAQUE)
            {
                if (display->driver_pixel_write)
                {
                    display->driver_pixel_write(context, pos.x, pos.y, pixel_color);
                }
            }
            else
            {
                if (display->driver_pixel_blend)
                {
                    display->driver_pixel_blend(context, pos.x, pos.y, pixel_color, brush_alpha);
                }
            }

#else // !GX_BRUSH_ALPHA_SUPPORT

            if (display->driver_pixel_write)
            {
                display->driver_pixel_write(context, pos.x, pos.y, pixel_color);
            }

#endif // !GX_BRUSH_ALPHA_SUPPORT
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_line_draw                                                */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This draws the specified line on the current context.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    x_start                            x-coord of endpoint1             */
/*    y_start                            y-coord of endpoint1             */
/*    x_end                              x-coord of endpoint2             */
/*    y_end                              y-coord of endpoint2             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                             Completion status                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    gx_utility_rectanlge_define        Define a rectangle               */
/*    gx_utility_rectangle_overlap_detect                                 */
/*                                       Detect rectangle overlap         */
/*    [gx_display_driver_line_draw]      The generic display driver line  */
/*                                         drawing routine                */
/*    [gx_display_driver_horizontal_line_draw]                            */
/*                                       The display driver horizontal    */
/*                                         line drawing function          */
/*    [gx_display_driver_vertical_line_draw]                              */
/*                                       The display driver vertical      */
/*                                         line drawing function          */
/*    [gx_display_driver_horizontal_pattern_line_draw]                    */
/*                                       The display driver horizontal    */
/*                                         pattern line drawing function  */
/*    [gx_display_driver_vertical_pattern_line_draw]                      */
/*                                       The display driver vertical      */
/*                                         pattern line drawing function  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_line_draw(GX_VALUE x_start, GX_VALUE y_start, GX_VALUE x_end, GX_VALUE y_end)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    GX_BRUSH& brush = context->brush;
    GX_VALUE brush_width = brush.width;
    GX_VALUE width;

    if (brush_width == 0)
    {
        return GX_SUCCESS;
    }

    GX_RECTANGLE bound;

    if (x_start <= x_end)
    {
        if (y_start <= y_end)
        {
            bound.set_(x_start, y_start, x_end, y_end);
        }
        else
        {
            bound.set_(x_start, y_end, x_end, y_start);
        }
    }
    else
    {
        if (y_start <= y_end)
        {
            bound.set_(x_end, y_start, x_start, y_end);
        }
        else
        {
            bound.set_(x_end, y_end, x_start, y_start);
        }
    }

    bool simple_line = false;

    if ((brush_width == 1) || !(brush.style & GX_BRUSH_ROUND))
    {
        // brush is 1 pixel wide or not round, check for horizontal or vertical
        if ((x_start == x_end) || (y_start == y_end))
        {
            simple_line = true;
        }
    }

    GX_DISPLAY* display = context->display;

    bool anti_aliased = false;

    if (brush.style & GX_BRUSH_ALIAS)
    {
        if (brush_width == 1)
        {
            if (display->driver_anti_aliased_line_draw)
            {
                anti_aliased = true;
            }
        }
        else
        {
            if (display->driver_anti_aliased_wide_line_draw)
            {
                anti_aliased = true;
            }
        }
    }

    if (simple_line)
    {
        if (!brush.line_pattern)
        {
            if (x_start == x_end)
            {
                bound.left = (GX_VALUE)(x_start - brush_width / 2);
                bound.right = (GX_VALUE)(bound.left + brush_width - 1);
            }
            else
            {
                bound.top = (GX_VALUE)(y_start - brush_width / 2);
                bound.bottom = (GX_VALUE)(bound.top + brush_width - 1);
            }
        }
    }
    else
    {
        width = (GX_VALUE)((brush_width + 1) / 2);
        if (anti_aliased)
        {
            width = (GX_VALUE)(width + 1);
        }

        bound.resize_(width);
    }

    if (!gx_rectangle_intersect_(bound, context->dirty, &bound))
    {
        return GX_SUCCESS;
    }

    // test to determine if the bounding rectangle overlaps the region we are allowed to draw
    // into. For each view that overlaps the bounding rectangle, do some drawing.

    GX_RECTANGLE clip_rect;

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (!gx_rectangle_intersect_(view->rectangle, bound, &clip_rect))
        {
            continue;
        }

        context->clip = &clip_rect;

        if (simple_line)
        {
            if (y_start == y_end) // horizontal_line
            {
                if (brush.line_pattern)
                {
                    if (clip_rect.left > x_start)
                    {
                        width = (GX_VALUE)((clip_rect.left - x_start) & 0x1F);
                        brush.pattern_mask >>= width;
                    }

                    display->driver_horizontal_pattern_line_draw(context,
                                                                 clip_rect.left,
                                                                 clip_rect.right,
                                                                 y_start);

                    if (clip_rect.right < x_end)
                    {
                        width = (GX_VALUE)((x_end - clip_rect.left) & 0x1F);
                        if ((brush.pattern_mask >> width) == 0)
                        {
                            brush.pattern_mask <<= (32 - width);
                        }
                        else
                        {
                            brush.pattern_mask >>= width;
                        }
                    }
                }
                else
                {
                    display->driver_horizontal_line_draw(context,
                                                         clip_rect.left,
                                                         clip_rect.right,
                                                         clip_rect.top,
                                                         clip_rect.height_(),
                                                         brush.line_color);
                }
            }
            else // vertical_line
            {
                if (brush.line_pattern)
                {
                    if (clip_rect.top > y_start)
                    {
                        width = (GX_VALUE)((clip_rect.top - y_start) & 0x1F);
                        brush.pattern_mask >>= width;
                    }

                    display->driver_vertical_pattern_line_draw(context,
                                                               clip_rect.top,
                                                               clip_rect.bottom,
                                                               x_start);

                    if (clip_rect.bottom < y_end)
                    {
                        width = (GX_VALUE)((y_end - clip_rect.bottom) & 0x1F);
                        if ((brush.pattern_mask >> width) == 0)
                        {
                            brush.pattern_mask <<= (32 - width);
                        }
                        else
                        {
                            brush.pattern_mask >>= width;
                        }
                    }
                }
                else
                {
                    display->driver_vertical_line_draw(context,
                                                       clip_rect.top,
                                                       clip_rect.bottom,
                                                       clip_rect.left,
                                                       clip_rect.width_(),
                                                       brush.line_color);
                }
            }
        }
        else // !simple_line
        {
            if (anti_aliased)
            {
                if (brush_width == 1)
                {
                    display->driver_anti_aliased_line_draw(context, x_start, y_start, x_end, y_end);
                }
                else
                {
                    display->driver_anti_aliased_wide_line_draw(context, x_start, y_start, x_end, y_end);
                }
            }
            else
            {
                if (brush_width == 1)
                {
                    display->driver_simple_line_draw(context, x_start, y_start, x_end, y_end);
                }
                else
                {
                    display->driver_simple_wide_line_draw(context, x_start, y_start, x_end, y_end);
                }
            }
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/

static void _gx_canvas_rectangle_fill_helper(GX_DRAW_CONTEXT* context,
                                             const GX_RECTANGLE& bound,
                                             GX_COLOR fill_color)
{
    GX_DISPLAY* display = context->display;

    // test to determine if the bounding rectangle overlaps the region we are allowed to draw
    // into. For each view that overlaps the bounding rectangle, do some drawing.

    GX_RECTANGLE fill_rect;

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (!gx_rectangle_intersect_(view->rectangle, bound, &fill_rect))
        {
            continue;
        }

        context->clip = &fill_rect;

        display->driver_horizontal_line_draw(context,
                                             fill_rect.left,
                                             fill_rect.right,
                                             fill_rect.top,
                                             fill_rect.height_(),
                                             fill_color);
    }
}

static UINT _gx_canvas_rectangle_fill(GX_DRAW_CONTEXT* context,
                                      const GX_RECTANGLE& rectangle,
                                      GX_COLOR fill_color)
{
    GX_RECTANGLE bound;

    if (!gx_rectangle_intersect_(rectangle, context->dirty, &bound))
    {
        return GX_SUCCESS;
    }

    _gx_canvas_rectangle_fill_helper(context, bound, fill_color);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_rectangle_draw                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This draws the specified rectangle on the current context.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    rectangle                             Rectangle specification       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_overlap_detect  Detect rectangle overlap      */
/*    _gx_canvas_line_draw                  Draw a line                   */
/*    _gx_canvas_rectangle_draw             Draw a rectangle              */
/*    [gx_display_driver_horizontal_line_draw]                            */
/*                                          Display driver horizontal     */
/*                                            line draw routine           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_scrollbar_draw                                                  */
/*    _gx_slider_needle_draw                                              */
/*    _gx_widget_border_draw                                              */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_rectangle_draw(GX_RECTANGLE* rectangle)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

    GX_RECTANGLE bound;

    if (!gx_rectangle_intersect_(*rectangle, context->dirty, &bound))
    {
        return GX_SUCCESS;
    }

    const GX_BRUSH& brush = context->brush;
    GX_VALUE brush_width = brush.width;
    switch (brush_width)
    {
    case 0: // no border
        break;

    case 1: // thin border

        // left side
        _gx_canvas_line_draw(rectangle->left,
                             (GX_VALUE)(rectangle->top + 1),
                             rectangle->left,
                             (GX_VALUE)(rectangle->bottom - 1));

        // right side
        _gx_canvas_line_draw(rectangle->right,
                             (GX_VALUE)(rectangle->top + 1),
                             rectangle->right,
                             (GX_VALUE)(rectangle->bottom - 1));

        // top side
        _gx_canvas_line_draw(rectangle->left,
                             rectangle->top,
                             rectangle->right,
                             rectangle->top);

        // bottom side
        _gx_canvas_line_draw(rectangle->left,
                             rectangle->bottom,
                             rectangle->right,
                             rectangle->bottom);

        break;

    default:
        {
            GX_RECTANGLE fill_rect = *rectangle;
            GX_COLOR line_color = brush.line_color;

            // top side
            fill_rect.bottom = (GX_VALUE)(fill_rect.top + brush_width - 1);
            _gx_canvas_rectangle_fill(context, fill_rect, line_color);

            // bottom side
            fill_rect.bottom = rectangle->bottom;
            fill_rect.top = (GX_VALUE)(fill_rect.bottom - brush_width + 1);
            _gx_canvas_rectangle_fill(context, fill_rect, line_color);

            // left side
            fill_rect.top = (GX_VALUE)(rectangle->top + brush_width);
            fill_rect.bottom = (GX_VALUE)(fill_rect.bottom - brush_width);
            fill_rect.right = (GX_VALUE)(fill_rect.left + brush_width - 1);
            _gx_canvas_rectangle_fill(context, fill_rect, line_color);

            // right side
            fill_rect.right = rectangle->right;
            fill_rect.left = (GX_VALUE)(fill_rect.right - brush_width + 1);
            _gx_canvas_rectangle_fill(context, fill_rect, line_color);
        }
        break;
    }

    if (!(brush.style & GX_BRUSH_SOLID_FILL))
    {
        return GX_SUCCESS;
    }

    if (brush_width != 0)
    {
        GX_RECTANGLE fill_rect;

        fill_rect.left = (GX_VALUE)(rectangle->left + brush_width);
        fill_rect.top = (GX_VALUE)(rectangle->top + brush_width);
        fill_rect.right = (GX_VALUE)(rectangle->right - brush_width);
        fill_rect.bottom = (GX_VALUE)(rectangle->bottom - brush_width);

        if (!gx_rectangle_intersect_(fill_rect, context->dirty, &bound))
        {
            return GX_SUCCESS;
        }
    }

    _gx_canvas_rectangle_fill_helper(context, bound, brush.fill_color);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_polygon_draw                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This draws a polygon into the current context.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    point_array                           Array of points of the polygon*/
/*                                            terminated by NULL entry.   */
/*    number_of_points                      Number of points of polygon   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define          Define a rectangle            */
/*    _gx_utility_rectangle_overlap_detect  Detect rectangle overlap      */
/*    [gx_display_driver_polygon_draw]      The display driver basic      */
/*                                            polygon drawing routine     */
/*    [gx_display_driver_polygon_fill]      The display driver basic      */
/*                                            polygon drawing routine     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_polygon_draw(GX_POINT* point_array, INT number_of_points)
{
    GX_VALUE left = point_array[0].x;
    GX_VALUE top = point_array[0].y;
    GX_VALUE right = point_array[0].x;
    GX_VALUE bottom = point_array[0].y;

    for (INT i = 0; i < number_of_points; ++i)
    {
        if (point_array[i].x < left)
        {
            left = point_array[i].x;
        }

        if (point_array[i].y < top)
        {
            top = point_array[i].y;
        }

        if (point_array[i].x > right)
        {
            right = point_array[i].x;
        }

        if (point_array[i].y > bottom)
        {
            bottom = point_array[i].y;
        }
    }

    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    const GX_BRUSH& brush = context->brush;

    GX_VALUE width = (GX_VALUE)((brush.width + 1) >> 1);

    GX_RECTANGLE bound;
    bound.set_((GX_VALUE)(left - width), (GX_VALUE)(top - width),
                (GX_VALUE)(right + width), (GX_VALUE)(bottom + width));

    if (!gx_rectangle_intersect_(bound, context->dirty, &bound))
    {
        return GX_SUCCESS;
    }

    GX_DISPLAY* display = context->display;

    // test to determine if the bounding rectangle overlaps the region we are allowed to draw
    // into. For each view that overlaps the bounding rectangle, do some drawing.

    GX_RECTANGLE* saved_clip_rect = context->clip;
    GX_RECTANGLE clip_rect;

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (!gx_rectangle_intersect_(view->rectangle, bound, &clip_rect))
        {
            continue;
        }

        context->clip = &clip_rect;

        if (brush.style & (GX_BRUSH_SOLID_FILL | GX_BRUSH_PIXELMAP_FILL))
        {
            if (display->driver_polygon_fill)
            {
                display->driver_polygon_fill(context, point_array, number_of_points);
            }
        }

        if (display->driver_polygon_draw)
        {
            display->driver_polygon_draw(context, point_array, number_of_points);
        }
    }

    context->clip = saved_clip_rect;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_circle_draw                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This draws a circle into the currrent context.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    xcenter                               x-coord of center of circle   */
/*    ycenter                               y-coord of center of circle   */
/*    r                                     Radius of circle              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define          Define a rectangle            */
/*    _gx_utility_rectangle_overlap_detect  Detects two rectangles being  */
/*                                            overlap                     */
/*    [gx_display_driver_arc_draw]          Display driver basic          */
/*                                            arc drawing routine         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_ARC_DRAWING_SUPPORT)
UINT _gx_canvas_circle_draw(INT xcenter, INT ycenter, UINT r)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    const GX_BRUSH& brush = context->brush;
    UINT brush_width = (UINT)brush.width;

    GX_RECTANGLE bound;
    gx_canvas_circle_bound_get_(brush_width, xcenter, ycenter, r, &bound);

    if (!gx_rectangle_intersect_(bound, context->dirty, &bound))
    {
        return GX_SUCCESS;
    }

    GX_DISPLAY* display = context->display;

    void (*outline_function)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, UINT r);

    if (brush_width == 1)
    {
        if ((brush.style & GX_BRUSH_ALIAS) && display->driver_anti_aliased_circle_draw)
        {
            outline_function = display->driver_anti_aliased_circle_draw;
        }
        else
        {
            outline_function = display->driver_circle_draw;
        }
    }
    else if (brush_width > 1)
    {
        if ((brush.style & GX_BRUSH_ALIAS) && display->driver_anti_aliased_wide_circle_draw)
        {
            outline_function = display->driver_anti_aliased_wide_circle_draw;
        }
        else
        {
            outline_function = display->driver_wide_circle_draw;
        }
    }
    else
    {
        outline_function = GX_NULL; // no outline
    }

    // test to determine if the bounding rectangle overlaps the region we are allowed to draw
    // into. For each view that overlaps the bounding rectangle, do some drawing.

    GX_RECTANGLE* saved_clip_rect = context->clip;
    GX_RECTANGLE clip_rect;

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (!gx_rectangle_intersect_(view->rectangle, bound, &clip_rect))
        {
            continue;
        }

        context->clip = &clip_rect;

        if (brush.style & (GX_BRUSH_SOLID_FILL | GX_BRUSH_PIXELMAP_FILL))
        {
            if (display->driver_circle_fill)
            {
                display->driver_circle_fill(context, xcenter, ycenter, r);
            }
        }

        if (outline_function)
        {
            outline_function(context, xcenter, ycenter, r);
        }
    }

    context->clip = saved_clip_rect;

    return GX_SUCCESS;
}
#endif // GX_ARC_DRAWING_SUPPORT

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_arc_draw                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This draws a circle arc into the currrent context.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    xcenter                               x-coord of center of circle   */
/*                                            arc                         */
/*    ycenter                               y-coord of center of circle   */
/*                                            arc                         */
/*    r                                     Radius of circle arc          */
/*    start_angle                           The start angle of circle arc */
/*    end_angle                             The end angle of circle arc   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define                                        */
/*    _gx_utility_rectangle_overlap_detect                                */
/*    _gx_display_driver_arc_draw                                         */
/*    _gx_display_driver_anti_aliased_arc_draw                            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_ARC_DRAWING_SUPPORT)
UINT _gx_canvas_arc_draw(INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    const GX_BRUSH& brush = context->brush;
    UINT brush_width = (UINT)brush.width;

    GX_RECTANGLE bound;
    gx_canvas_circle_bound_get_(brush_width, xcenter, ycenter, r, &bound);

    if (!gx_rectangle_intersect_(bound, context->dirty, &bound))
    {
        return GX_SUCCESS;
    }

    GX_DISPLAY* display = context->display;

    gx_arc_draw_fptr outline_function = gx_arc_draw_function_get_(brush_width, brush.style, display);

    gx_canvas_angle_normalize_(start_angle, end_angle);

    // test to determine if the bounding rectangle overlaps the region we are allowed to draw
    // into. For each view that overlaps the bounding rectangle, do some drawing.

    GX_RECTANGLE* saved_clip_rect = context->clip;
    GX_RECTANGLE clip_rect;

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (!gx_rectangle_intersect_(view->rectangle, bound, &clip_rect))
        {
            continue;
        }

        context->clip = &clip_rect;

        if (brush.style & (GX_BRUSH_SOLID_FILL | GX_BRUSH_PIXELMAP_FILL))
        {
            if (display->driver_arc_fill)
            {
                display->driver_arc_fill(context, xcenter, ycenter, r, start_angle, end_angle);
            }
        }

        if (outline_function)
        {
            outline_function(context, xcenter, ycenter, r, start_angle, end_angle);
        }
    }

    context->clip = saved_clip_rect;

    return GX_SUCCESS;
}
#endif // GX_ARC_DRAWING_SUPPORT

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_pie_draw                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This draws a pie into the currrent context.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    xcenter                               x-coord of center of circle   */
/*                                            arc                         */
/*    ycenter                               y-coord of center of circle   */
/*                                            arc                         */
/*    r                                     Radius of circle arc          */
/*    start_angle                           The start angle of circle arc */
/*    end_angle                             The end angle of circle arc   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define          Define a rectangle            */
/*    _gx_utility_rectangle_overlap_detect  Detect rectangle overlap      */
/*    [gx_display_driver_pie_fill]          Display driver basic          */
/*                                            pie drawing routine         */
/*    [gx_display_driver_arc_draw]          Display driver basic          */
/*                                            arc drawing routine         */
/*    _gx_display_driver_anti_aliased_arc_draw                            */
/*                                          Display driver basic aliased  */
/*                                            arc drawing routine         */
/*    _gx_utility_circle_point_get          Get point position on a circle*/
/*    _gx_canvas_line_draw                  Draw a line                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_ARC_DRAWING_SUPPORT)
UINT _gx_canvas_pie_draw(INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    GX_BRUSH& brush = context->brush;
    UINT brush_width = (UINT)brush.width;

    GX_RECTANGLE bound;
    gx_canvas_circle_bound_get_(brush_width, xcenter, ycenter, r, &bound);

    if (!gx_rectangle_intersect_(bound, context->dirty, &bound))
    {
        return GX_SUCCESS;
    }

    GX_DISPLAY* display = context->display;

    gx_arc_draw_fptr outline_function = gx_arc_draw_function_get_(brush_width, brush.style, display);

    gx_canvas_angle_normalize_(start_angle, end_angle);

    // test to determine if the bounding rectangle overlaps the region we are allowed to draw
    // into. For each view that overlaps the bounding rectangle, do some drawing.

    GX_RECTANGLE* saved_clip_rect = context->clip;
    GX_RECTANGLE clip_rect;

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (!gx_rectangle_intersect_(view->rectangle, bound, &clip_rect))
        {
            continue;
        }

        context->clip = &clip_rect;

        if (brush_width > 0)
        {
            brush_width -= 1;
            brush_width >>= 1;
        }

        if ((brush.style & (GX_BRUSH_SOLID_FILL | GX_BRUSH_PIXELMAP_FILL)) &&
            (r > brush_width))
        {
            if (display->driver_pie_fill)
            {
                display->driver_pie_fill(context, xcenter, ycenter, r - brush_width, start_angle, end_angle);
            }
        }

#if defined(GX_BRUSH_ALPHA_SUPPORT)
        GX_UBYTE saved_brush_alpha = brush.alpha;
        if (brush.width > 1)
        {
            brush.alpha = GX_ALPHA_VALUE_OPAQUE;
        }
#endif

        if (outline_function)
        {
            outline_function(context, xcenter, ycenter, r, start_angle, end_angle);
        }

        if (r > brush_width)
        {
            // Draw delimiting lines.
            UINT rr = r - brush_width;
            gx_canvas_draw_radius_(xcenter, ycenter, rr, start_angle);
            gx_canvas_draw_radius_(xcenter, ycenter, rr, end_angle);
        }

#if defined(GX_BRUSH_ALPHA_SUPPORT)
        brush.alpha = saved_brush_alpha;
#endif
    }

    context->clip = saved_clip_rect;

    return GX_SUCCESS;
}
#endif // GX_ARC_DRAWING_SUPPORT

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_ellipse_draw                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This draws a ellipse into the currrent context.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    xcenter                               x-coord of center of ellipse  */
/*    ycenter                               y-coord of center of ellipse  */
/*    r                                     Radius of circle              */
/*    a                                     Length of the Semi-major Axis */
/*    b                                     Length of the Semi-minor Axis */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define                                        */
/*    _gx_utility_rectangle_overlap_detect                                */
/*    [gx_display_driver_generic_ellipse_draw]                            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_ARC_DRAWING_SUPPORT)
UINT _gx_canvas_ellipse_draw(INT xcenter, INT ycenter, INT a, INT b)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    const GX_BRUSH& brush = context->brush;
    UINT brush_width = (UINT)brush.width;

    GX_RECTANGLE bound;
    gx_canvas_ellipse_bound_get_(brush_width, xcenter, ycenter, a, b, &bound);

    if (!gx_rectangle_intersect_(bound, context->dirty, &bound))
    {
        return GX_SUCCESS;
    }

    GX_DISPLAY* display = context->display;

    void (*outline_function)(GX_DRAW_CONTEXT* context, INT xcenter, INT ycenter, INT a, INT b);

    if (brush_width == 1)
    {
        if ((brush.style & GX_BRUSH_ALIAS) && display->driver_anti_aliased_ellipse_draw)
        {
            outline_function = display->driver_anti_aliased_ellipse_draw;
        }
        else
        {
            outline_function = display->driver_ellipse_draw;
        }
    }
    else if (brush_width > 1)
    {
        if ((brush.style & GX_BRUSH_ALIAS) && display->driver_anti_aliased_wide_ellipse_draw)
        {
            outline_function = display->driver_anti_aliased_wide_ellipse_draw;
        }
        else
        {
            outline_function = display->driver_wide_ellipse_draw;
        }
    }
    else
    {
        outline_function = GX_NULL; // no outline
    }

    // test to determine if the bounding rectangle overlaps the region we are allowed to draw
    // into. For each view that overlaps the bounding rectangle, do some drawing.

    GX_RECTANGLE* saved_clip_rect = context->clip;
    GX_RECTANGLE clip_rect;

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (!gx_rectangle_intersect_(view->rectangle, bound, &clip_rect))
        {
            continue;
        }

        context->clip = &clip_rect;

        if (brush.style & (GX_BRUSH_SOLID_FILL | GX_BRUSH_PIXELMAP_FILL))
        {
            if (display->driver_ellipse_fill)
            {
                display->driver_ellipse_fill(context, xcenter, ycenter, a, b);
            }
        }

        if (outline_function)
        {
            outline_function(context, xcenter, ycenter, a, b);
        }
    }

    context->clip = saved_clip_rect;

    return GX_SUCCESS;
}
#endif // GX_ARC_DRAWING_SUPPORT

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_pixelmap_get                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a GX_PIXELMAP structure pointing to the       */
/*    canvas data.                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pixelmap                              Pointer to destination for    */
/*                                            the retrieved pixlemap      */
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
UINT _gx_canvas_pixelmap_get(GX_PIXELMAP* pixelmap)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    if (!context)
    {
        return GX_INVALID_CONTEXT;
    }

    GX_CANVAS* canvas = context->canvas;
    if (!canvas)
    {
        return GX_INVALID_CANVAS;
    }

    GX_DISPLAY* display = context->display;
    if (!display)
    {
        return GX_INVALID_DISPLAY;
    }

    memset(pixelmap, 0, sizeof(GX_PIXELMAP));

    pixelmap->width = canvas->x_resolution;
    pixelmap->height = canvas->y_resolution;
    pixelmap->format = (GX_UBYTE)display->color_format;
    pixelmap->data = (GX_UBYTE*)canvas->memory;
    pixelmap->data_size = canvas->memory_size;
    pixelmap->version_major = 1;
    pixelmap->version_minor = 0;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_pixelmap_draw                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function prepares to draw the specified pixelmap at the        */
/*    requested position.                                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    x_position                            Top-left x-coord to place     */
/*                                            pixelmap                    */
/*    y_position                            Top-left y-coord to place     */
/*                                            pixelmap                    */
/*    pixelmap                              Pointer to actual pixelmap    */
/*                                            to draw                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*    _gx_utility_rectangle_define          Define a rectangle            */
/*    _gx_utility_rectangle_overlap_detect  Detect rectangle overlap      */
/*    [gx_display_driver_pixelmap_draw]     The display driver pixelmap   */
/*                                            draw routine                */
/*    [gx_display_driver_jpeg_draw]         The display driver JPEG draw  */
/*                                            routine                     */
/*    [gx_display_driver_png_draw]          The display driver PNG draw   */
/*                                            routine                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_canvas_pixelmap_blend                                           */
/*    _gx_canvas_pixelmap_tile                                            */
/*    _gx_checkbox_draw                                                   */
/*    _gx_icon_button_draw                                                */
/*    _gx_icon_draw                                                       */
/*    _gx_pixelmap_button_draw                                            */
/*    _gx_pixelmap_prompt_draw                                            */
/*    _gx_pixelmap_slider_draw                                            */
/*    _gx_radio_button_draw                                               */
/*    _gx_scroll_thumb_draw                                               */
/*    _gx_scrollbar_draw                                                  */
/*    _gx_window_draw                                                     */
/*    _gx_scroll_thumb_draw                                               */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_pixelmap_draw(GX_VALUE x, GX_VALUE y, GX_PIXELMAP* pixelmap)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

    GX_RECTANGLE bound;
    bound.set_(x, y, (GX_VALUE)(x + pixelmap->width - 1), (GX_VALUE)(y + pixelmap->height - 1));

    if (!gx_rectangle_intersect_(bound, context->dirty, &bound))
    {
        return GX_SUCCESS;
    }

    GX_DISPLAY* display = context->display;

    void (*pmp_function)(GX_DRAW_CONTEXT*, INT, INT, GX_PIXELMAP*);
    pmp_function = GX_NULL;

#if defined(GX_SOFTWARE_DECODER_SUPPORT)
    if (pixelmap->flags & GX_PIXELMAP_RAW_FORMAT)
    {
        if ((pixelmap->data[0] == 0xFF) && (pixelmap->data[1] == 0xD8))
        {
            pmp_function = display->driver_jpeg_draw;
        }
        else if (pixelmap->data[1] == 'P')
        {
            pmp_function = display->driver_png_draw;
        }
        else
        {
            pmp_function = GX_NULL;
        }

    }
    else
#endif // GX_SOFTWARE_DECODER_SUPPORT
    {
        if (pixelmap->format == GX_COLOR_FORMAT_8BIT_ALPHAMAP)
        {
            pmp_function = display->driver_alphamap_draw;
        }
        else
        {
            pmp_function = display->driver_pixelmap_draw;
        }
    }

    if (!pmp_function)
    {
        return GX_FAILURE;
    }

    GX_RECTANGLE clip_rect;

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (!gx_rectangle_intersect_(view->rectangle, bound, &clip_rect))
        {
            continue;
        }

        context->clip = &clip_rect;

        pmp_function(context, x, y, pixelmap);
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_pixelmap_blend                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function prepares to blend the specified pixelmap with         */
/*      background at the requested position.                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    x_position                            Top-left x-coord to place     */
/*                                            pixelmap                    */
/*    y_position                            Top-left y-coord to place     */
/*                                            pixelmap                    */
/*    pixelmap                              Pointer to actual pixelmap    */
/*                                            to draw                     */
/*    alpha                                 blending value 0-255          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define          Define a rectangle            */
/*    _gx_utility_rectangle_overlap_detect  Detect rectangle overlap      */
/*    [gx_display_driver_pixelmap_draw]     Driver level pixelmap blend   */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    _gx_sprite_draw                                                     */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_pixelmap_blend(GX_VALUE x, GX_VALUE y,
                               GX_PIXELMAP* pixelmap, GX_UBYTE alpha)
{
    if (alpha == 0)
    {
        return GX_SUCCESS;
    }

    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

    GX_RECTANGLE bound;
    bound.set_(x, y, (GX_VALUE)(x + pixelmap->width - 1), (GX_VALUE)(y + pixelmap->height - 1));

    if (!gx_rectangle_intersect_(bound, context->dirty, &bound))
    {
        return GX_SUCCESS;
    }

    GX_DISPLAY* display = context->display;

    void (*pmp_function)(GX_DRAW_CONTEXT*, INT, INT, GX_PIXELMAP*);

    if (pixelmap->format == GX_COLOR_FORMAT_8BIT_ALPHAMAP)
    {
        pmp_function = display->driver_alphamap_draw;
    }
    else
    {
        pmp_function = display->driver_pixelmap_draw;
    }

    if (!pmp_function)
    {
        return GX_NOT_SUPPORTED;
    }

    GX_UBYTE saved_brush_alpha = context->brush.alpha;
    context->brush.alpha = alpha;

    GX_RECTANGLE clip_rect;

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (!gx_rectangle_intersect_(view->rectangle, bound, &clip_rect))
        {
            continue;
        }

        context->clip = &clip_rect;

        pmp_function(context, x, y, pixelmap);
    }

    context->brush.alpha = saved_brush_alpha;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_pixelmap_tile                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function prepares to draw the specified pixelmap at the        */
/*    requested position.                                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    fill                                  Area to fill with pixelmap    */
/*    pixelmap                              Pointer to actual pixelmap    */
/*                                            to draw                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_overlap_detect  Detect rectangle overlap      */
/*    _gx_canvas_pixelmap_draw              Screen pixelmap draw routine  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_pixelmap_tile(GX_RECTANGLE* fill, GX_PIXELMAP* pixelmap)
{
    if (!pixelmap->width || !pixelmap->height)
    {
        return GX_FAILURE;
    }

    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

    GX_RECTANGLE dirty_rect;

    if (!gx_rectangle_intersect_(*fill, context->dirty, &dirty_rect))
    {
        return GX_SUCCESS;
    }

    GX_RECTANGLE saved_dirty_rect = context->dirty;
    context->dirty = dirty_rect;

    for (GX_VALUE y = fill->top; y <= fill->bottom; y = (GX_VALUE)(y + pixelmap->height))
    {
        for (GX_VALUE x = fill->left; x <= fill->right; x = (GX_VALUE)(x + pixelmap->width))
        {
            _gx_canvas_pixelmap_draw(x, y, pixelmap);
        }
    }

    context->dirty = saved_dirty_rect;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_rotated_pixelmap_bound_calculate                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function calcuates the bounding rectangle of a rotated         */
/*    pixelmap.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    rect                                  Rectangle of source pixelmap. */
/*    angle                                 Angle of rotation in degrees. */
/*    rot_cx                                x-coord of rotating center    */
/*    rot_cy                                y-coord of rotating center    */
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
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_rotated_pixelmap_bound_calculate(GX_RECTANGLE* rect,
                                                 INT angle,
                                                 INT rot_cx,
                                                 INT rot_cy)
{
    const INT mx[4] = { -1, 1, 1, -1 };
    const INT my[4] = { 1, 1, -1, -1 };
    INT idxminx;
    INT idxmaxx;
    INT idxmaxy;
    INT srcxres;
    INT srcyres;
    INT cosv;
    INT sinv;
    INT xres;
    INT yres;
    INT width;
    INT height;
    INT x;
    INT y;

    gx_canvas_angle_normalize_(angle);

    if (angle == 0)
    {
        return GX_SUCCESS;
    }

    width = rect->width_();
    height = rect->height_();

    if (angle == 90)
    {
        GX_SWAP_VALS(width, height);

        rect->left = (GX_VALUE)(rect->left + rot_cx - (width - 1 - rot_cy));
        rect->top = (GX_VALUE)(rect->top + rot_cy - rot_cx);
    }
    else if (angle == 180)
    {
        rect->left = (GX_VALUE)(rect->left + rot_cx - (width - 1 - rot_cx));
        rect->top = (GX_VALUE)(rect->top + rot_cy - (height - 1 - rot_cy));
    }
    else if (angle == 270)
    {
        GX_SWAP_VALS(width, height);

        rect->left = (GX_VALUE)(rect->left + rot_cx - rot_cy);
        rect->top = (GX_VALUE)(rect->top + rot_cy - (height - 1 - rot_cx));
    }
    else
    {
        idxminx = (angle / 90) & 0x3;
        idxmaxx = (idxminx + 2) & 0x3;
        idxmaxy = (idxminx + 1) & 0x3;

        // Calculate the source x and y center.
        srcxres = width >> 1;
        srcyres = height >> 1;

        cosv = _gx_utility_math_cos(GX_FIXED_VAL_MAKE(angle));
        sinv = _gx_utility_math_sin(GX_FIXED_VAL_MAKE(angle));

        xres = mx[idxmaxx] * (srcxres + 2) * cosv - my[idxmaxx] * (srcyres + 2) * sinv;
        yres = my[idxmaxy] * (srcyres + 2) * cosv + mx[idxmaxy] * (srcxres + 2) * sinv;

        xres = GX_FIXED_VAL_TO_INT(xres);
        yres = GX_FIXED_VAL_TO_INT(yres);

        // Calculate destination width and height.
        width = (xres << 1);
        height = (yres << 1);

        // Calculate the new rotation axis.
        x = GX_FIXED_VAL_TO_INT((rot_cx - srcxres) * cosv - (rot_cy - srcyres) * sinv);
        y = GX_FIXED_VAL_TO_INT((rot_cy - srcyres) * cosv + (rot_cx - srcxres) * sinv);

        x += xres;
        y += yres;

        rect->left = (GX_VALUE)(rect->left + rot_cx - x);
        rect->top = (GX_VALUE)(rect->top + rot_cy - y);
    }
    rect->right = (GX_VALUE)(rect->left + width - 1);
    rect->bottom = (GX_VALUE)(rect->top + height - 1);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_pixelmap_rotate                                          */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function prepares to draw the specified pixelmap at the        */
/*    requested position.                                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    x_position                            Top-left x-coord to place     */
/*                                            pixelmap                    */
/*    y_position                            Top-left y-coord to place     */
/*                                            pixelmap                    */
/*    pixelmap                              Pointer to actual pixelmap    */
/*                                            to draw                     */
/*    angle                                 The angle to rotate           */
/*    rot_cx                                x-coord of rotating center, if*/
/*                                            -1 is set, default it to    */
/*                                            image center.               */
/*    rot_cy                                y-coord of rotationg center.  */
/*                                            -1 is set, default it to    */
/*                                            image center.               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define                                        */
/*    _gx_utility_rectangle_overlap_detect                                */
/*    _gx_canvas_rotated_pixelmap_bound_calculate                         */
/*    [gx_display_driver_pixelmap_rotate]                                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_pixelmap_rotate(GX_VALUE x, GX_VALUE y, GX_PIXELMAP* pixelmap,
                                INT angle, INT rot_cx, INT rot_cy)
{
    if (pixelmap->flags & GX_PIXELMAP_COMPRESSED)
    {
        return GX_NOT_SUPPORTED;
    }

    INT cx;
    INT cy;

    if (rot_cx == -1)
    {
        cx = pixelmap->width >> 1;
    }
    else
    {
        cx = rot_cx;
    }

    if (rot_cy == -1)
    {
        cy = pixelmap->height >> 1;
    }
    else
    {
        cy = rot_cy;
    }

    gx_canvas_angle_normalize_(angle);

    GX_RECTANGLE bound;
    bound.set_(x, y, (GX_VALUE)(x + pixelmap->width - 1), (GX_VALUE)(y + pixelmap->height - 1));

    _gx_canvas_rotated_pixelmap_bound_calculate(&bound, angle, cx, cy);

    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;

    if (!gx_rectangle_intersect_(bound, context->dirty, &bound))
    {
        return GX_SUCCESS;
    }

    GX_DISPLAY* display = context->display;

    if (angle == 0)
    {
        if (!display->driver_pixelmap_draw)
        {
            return GX_FAILURE;
        }
    }
    else
    {
        if (!display->driver_pixelmap_rotate)
        {
            return GX_FAILURE;
        }
    }

    GX_RECTANGLE clip_rect;

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (!gx_rectangle_intersect_(view->rectangle, bound, &clip_rect))
        {
            continue;
        }

        context->clip = &clip_rect;

        if (angle == 0)
        {
            display->driver_pixelmap_draw(context, x, y, pixelmap);
        }
        else
        {
            display->driver_pixelmap_rotate(context, x, y, pixelmap, angle, cx, cy);
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_text_draw_ext                                            */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function prepares to draw text.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    x_start                               X-coordinate, text left edge  */
/*    y_start                               Y-coordinate, text baseline   */
/*    text                                  Text to draw                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define          Initialize a rectangle        */
/*    _gx_system_string_width_get           Get width of the string in    */
/*                                            pixels                      */
/*    _gx_utility_rectangle_overlap_detect  Determine if two rectangles   */
/*                                            overlap                     */
/*    _gx_canvas_glyphs_draw                Draw glyphs on canvas         */
/*    [gx_display_driver_horizontal_line_draw]                            */
/*                                       The display driver horizontal    */
/*                                         line drawing function          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_multi_line_text_view_draw                                       */
/*    _gx_single_line_text_input_draw                                     */
/*    _gx_widget_text_draw                                                */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_text_draw_ext(GX_VALUE x, GX_VALUE y, const GX_STRING* text)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    const GX_BRUSH& brush = context->brush;

    GX_FONT* font = brush.font;
    if (!font || !font->glyphs.normal)
    {
        return GX_INVALID_FONT;
    }

    GX_VALUE width = 0;
    _gx_system_string_width_get_ext(font, text, &width);

    GX_RECTANGLE bound;
    bound.set_(x, y, (GX_VALUE)(x + width - 1), (GX_VALUE)(y + font->line_height));

    if (!gx_rectangle_intersect_(bound, context->dirty, &bound))
    {
        return GX_SUCCESS;
    }

    GX_DISPLAY* display = context->display;

    void (*glyph_draw)(GX_DRAW_CONTEXT* context, GX_RECTANGLE* draw_area,
                       GX_POINT* map_offset, const GX_GLYPH* glyph);

    switch (font->format & GX_FONT_FORMAT_BPP_MASK)
    {
    case GX_FONT_FORMAT_1BPP:
        if (font->format & GX_FONT_FORMAT_COMPRESSED)
        {
            glyph_draw = display->driver_1bit_compressed_glyph_draw;
        }
        else
        {
            glyph_draw = display->driver_1bit_glyph_draw;
        }
        break;

    case GX_FONT_FORMAT_4BPP:
        if (font->format & GX_FONT_FORMAT_COMPRESSED)
        {
            glyph_draw = display->driver_4bit_compressed_glyph_draw;
        }
        else
        {
            glyph_draw = display->driver_4bit_glyph_draw;
        }
        break;

    case GX_FONT_FORMAT_8BPP:
        if (font->format & GX_FONT_FORMAT_COMPRESSED)
        {
            glyph_draw = display->driver_8bit_compressed_glyph_draw;
        }
        else
        {
            glyph_draw = display->driver_8bit_glyph_draw;
        }
        break;

    default:
        glyph_draw = GX_NULL;
        break;
    }

    if (!glyph_draw)
    {
        return GX_FAILURE;
    }

    GX_POINT pos;
    pos.x = x;
    pos.y = y;

    y = (GX_VALUE)(y + font->baseline + 1); // for underline

    GX_RECTANGLE clip_rect;

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (!gx_rectangle_intersect_(view->rectangle, bound, &clip_rect))
        {
            continue;
        }

        _gx_canvas_glyphs_draw(context, &pos, text, &clip_rect, glyph_draw);

        if (brush.style & GX_BRUSH_UNDERLINE)
        {
            if (clip_rect.top <= y && y <= clip_rect.bottom)
            {
                width = brush.width;

                if (width <= 0)
                {
                    width = 1;
                }

                if (width > (GX_VALUE)(clip_rect.bottom - y + 1))
                {
                    width = (GX_VALUE)(clip_rect.bottom - y + 1);
                }

                display->driver_horizontal_line_draw(context,
                                                     clip_rect.left,
                                                     clip_rect.right,
                                                     y,
                                                     width,
                                                     brush.line_color);
            }
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_aligned_text_draw                                        */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws text to canvas with specified alignment style.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text                                  Text to draw                  */
/*    rect                                  Drawing area                  */
/*    align                                 Alignment style               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_brush_get                 Get context brush             */
/*    _gx_system_string_width_get_ext              Get string width              */
/*    _gx_canvas_text_draw_ext              Actual text draw function     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_aligned_text_draw(const GX_STRING* text, GX_RECTANGLE* rect, ULONG align)
{
    GX_BRUSH* brush;
    _gx_context_brush_get(&brush);

    if (!brush->font)
    {
        return GX_INVALID_FONT;
    }

    GX_VALUE x;
    GX_VALUE y;

    GX_VALUE text_width;

    switch (align & GX_STYLE_TEXT_ALIGNMENT_MASK)
    {
    case GX_STYLE_TEXT_RIGHT:
        _gx_system_string_width_get_ext(brush->font, text, &text_width);
        x = (GX_VALUE)(rect->right - text_width);
        break;

    case GX_STYLE_TEXT_CENTER:
        _gx_system_string_width_get_ext(brush->font, text, &text_width);
        x = (GX_VALUE)(rect->left + (rect->width_() - text_width) / 2);
        break;

    default: // GX_STYLE_TEXT_LEFT
        x = rect->left;
        break;
    }

    GX_VALUE text_height = brush->font->line_height;
    y = (GX_VALUE)(rect->top + (rect->height_() - text_height) / 2);

    return _gx_canvas_text_draw_ext(x, y, text);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_rotated_text_draw_ext                                    */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function draws rotated text.                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text                                  Pointer to string             */
/*    xcenter                               Center point for text drawing */
/*    ycenter                               Center point for text drawing */
/*    angle                                 Angle at which to rotate text */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_string_width_get_ext              Get width of the string in    */
/*                                           pixels                       */
/*    _gx_canvas_text_draw                  Draw glyphs on canvas         */
/*    _gx_utility_string_to_alphamap        Convert string to alpha-map   */
/*    _gx_utiity_pixelmap_rotate            Rotate alphaap to desired     */
/*                                           angle                        */
/*    _gx_canvas_pixelmap_draw              Draw text alphamap            */
/*    _gx_system_memory_free                Free memory used for rotated  */
/*                                           alphamap and canvas          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Software                                                */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_rotated_text_draw_ext(const GX_STRING* text,
                                      GX_VALUE xcenter,
                                      GX_VALUE ycenter,
                                      INT angle)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    if (!context)
    {
        return GX_FAILURE;
    }

    GX_FONT* font = context->brush.font;
    if (!font)
    {
        return GX_FAILURE;
    }

    while (angle >= 360)
    {
        angle -= 360;
    }
    while (angle <= -360)
    {
        angle += 360;
    }

    if (angle == 0)
    {
        GX_VALUE text_width;
        _gx_system_string_width_get_ext(font, text, &text_width);
        GX_VALUE x = (GX_VALUE)(xcenter - text_width / 2);
        GX_VALUE y = (GX_VALUE)(ycenter - font->line_height / 2);
        _gx_canvas_text_draw_ext(x, y, text);

        return GX_SUCCESS;
    }

    GX_PIXELMAP textmap;
    UINT result = _gx_utility_string_to_alphamap_ext(text, font, &textmap);
    if (result == GX_SUCCESS)
    {
        GX_PIXELMAP rotated_map;
        INT x = 0;
        INT y = 0;
        result = _gx_utility_pixelmap_rotate(&textmap, angle, &rotated_map, &x, &y);
        if (result == GX_SUCCESS)
        {
            x = xcenter - rotated_map.width / 2;
            y = ycenter - rotated_map.height / 2;

#if defined(GX_RENESAS_DAVE2D_DRAW)
            rotated_map.flags |= GX_PIXELMAP_DYNAMICALLY_ALLOCATED;
#endif
            _gx_canvas_pixelmap_draw((GX_VALUE)x, (GX_VALUE)y, &rotated_map);

            _gx_system_memory_free((void*)rotated_map.data);
        }

        _gx_system_memory_free((void*)textmap.data);
    }

    return result;
}

/**************************************************************************/

static void gx_canvas_glyph_map_draw_(GX_DRAW_CONTEXT* context,
                                      GX_VALUE xstart, GX_VALUE ystart,
                                      const GX_RECTANGLE* view,
                                      const GX_FONT* font_link,
                                      const GX_GLYPH* glyph,
                                      void (*draw_glyph)(GX_DRAW_CONTEXT*, GX_RECTANGLE*, GX_POINT*, const GX_GLYPH*),
                                      GX_BYTE kerning_offset = 0)
{
    GX_VALUE x_offset = (GX_VALUE)(xstart + glyph->leading + kerning_offset);
    GX_VALUE y_offset = (GX_VALUE)(ystart + font_link->baseline - glyph->ascent);

    GX_RECTANGLE draw_area;

    draw_area.left = x_offset;
    draw_area.top = y_offset;
    draw_area.right = (GX_VALUE)(draw_area.left + glyph->width - 1);
    draw_area.bottom = (GX_VALUE)(draw_area.top + glyph->height - 1);

    if (draw_area.bottom >= view->top &&
        draw_area.top <= view->bottom &&
        draw_area.right >= view->left &&
        draw_area.left <= view->right)
    {
        GX_POINT map_offset;

        map_offset.x = 0;
        map_offset.y = 0;

        // Calculate the y_start value, which is the offset into the row of
        // the glyph where we start to the draw.
        if (draw_area.top < view->top)
        {
            map_offset.y = (GX_VALUE)(view->top - draw_area.top);
            draw_area.top = view->top;
        }

        if (draw_area.left < view->left)
        {
            map_offset.x = (GX_VALUE)(view->left - x_offset);
            draw_area.left = view->left;
        }

        if (draw_area.bottom > view->bottom)
        {
            draw_area.bottom = view->bottom;
        }

        if (draw_area.right > view->right)
        {
            draw_area.right = view->right;
        }

        draw_glyph(context, &draw_area, &map_offset, glyph);
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_kerning_glyphs_draw                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function prepares to draw text.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    font                                  Font used by the string       */
/*    draw_position                         Coord of draw position        */
/*    string                                Pointer to string which need  */
/*                                            to draw                     */
/*    length                                Number of string about to draw*/
/*    view                                  Pointer to view size          */
/*    draw_glyph                            Callback pointer to display   */
/*                                            driver text draw function   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_utf8_string_character_get                               */
/*                                    Get characters of this string       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_canvas_glyph_draw                                               */
/*                                                                        */
/**************************************************************************/
#if defined(GX_FONT_KERNING_SUPPORT)
static void _gx_canvas_kerning_glyphs_draw(GX_DRAW_CONTEXT* context,
                                           const GX_FONT* font,
                                           const GX_POINT* draw_position,
                                           GX_STRING string_copy,
                                           const GX_RECTANGLE* view,
                                           void (*draw_glyph)(GX_DRAW_CONTEXT*, GX_RECTANGLE*, GX_POINT*, const GX_GLYPH*))
{
    const GX_KERNING_GLYPH*    glyph;
    GX_CHAR_CODE               char_val;
    GX_VALUE                   xstart;
    GX_VALUE                   ystart;
    const GX_FONT*             font_link;

/* Used for kerning glyph. */
    GX_CHAR_CODE       pre_char_val = 0;
    GX_BYTE            kerning_offset = 0;
    const GX_UBYTE*    kerning_table;
    INT                kerning_counts;
    INT                index;
    GX_UBYTE*          left_glyph_ptr;

#if defined(GX_UTF8_SUPPORT)
    UINT ret;
#endif /* GX_UTF8_SUPPORT */

    /* Setup local variables.  */

    xstart = draw_position->x;
    ystart = draw_position->y;

    /* for each character in the string */
    do
    {
#ifdef GX_UTF8_SUPPORT
        ret = _gx_utility_utf8_string_character_get(&string_copy, &char_val, GX_NULL);

        if ((ret != GX_SUCCESS) || (char_val == 0))
#else
        char_val = (GX_CHAR_CODE)(*string_copy.ptr++);
        string_copy.length--;

        if (char_val == 0)
#endif /* GX_UTF8_SUPPORT */
        {
            break;
        }

        font_link = gx_font_find_(font, char_val);
        if (font_link)
        {
            char_val = (GX_CHAR_CODE)(char_val - font_link->first_glyph);

            glyph = &((const GX_KERNING_GLYPH*)font_link->glyphs.kerning)[char_val];
            kerning_table = ((GX_KERNING_GLYPH*)glyph)->kerning_table;
            if (kerning_table && (pre_char_val != 0))
            {
                /* Search the kerning table for the kerning value. */
                kerning_counts = *kerning_table;
                left_glyph_ptr = (GX_UBYTE*)(kerning_table + 1);

                for (index = 0; index < kerning_counts; index++)
                {
                    if ((*left_glyph_ptr) == (pre_char_val + font_link->first_glyph))
                    {
                        kerning_offset = (GX_CHAR)(*(left_glyph_ptr + 1));
                        break;
                    }
                    left_glyph_ptr += 2;
                }
            }

            if (glyph->map)
            {
                gx_canvas_glyph_map_draw_(context, xstart, ystart, view,
                                          font_link, (const GX_GLYPH*)glyph, draw_glyph, kerning_offset);
            }

            xstart = (GX_VALUE)(xstart + glyph->advance);
            xstart = (GX_VALUE)(xstart + kerning_offset);
        }

        pre_char_val = char_val;
        kerning_offset = 0;
    } while (string_copy.length > 0);
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_compressed_glyphs_draw                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function prepares to draw text.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    font                                  Font used by the string       */
/*    draw_position                         Coord of draw position        */
/*    string                                Pointer to string which need  */
/*                                            to draw                     */
/*    length                                Number of string about to draw*/
/*    view                                  Pointer to view size          */
/*    draw_glyph                            Callback pointer to display   */
/*                                            driver text draw function   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_utf8_string_character_get                               */
/*                                    Get characters of this string       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_canvas_glyph_draw                                               */
/*                                                                        */
/**************************************************************************/
static void _gx_canvas_compressed_glyphs_draw(GX_DRAW_CONTEXT* context,
                                              const GX_FONT* font,
                                              const GX_POINT* draw_position,
                                              GX_STRING string_copy,
                                              const GX_RECTANGLE* view,
                                              void (*draw_glyph)(GX_DRAW_CONTEXT*, GX_RECTANGLE*, GX_POINT*, const GX_GLYPH*))
{
    const GX_COMPRESSED_GLYPH*    glyph;
    GX_CHAR_CODE                  char_val;
    GX_VALUE                      xstart;
    GX_VALUE                      ystart;
    const GX_FONT*                font_link;

#ifdef GX_UTF8_SUPPORT
    UINT          ret = GX_SUCCESS;
#ifdef GX_THAI_GLYPH_SHAPING_SUPPORT
    GX_CHAR_CODE* code_list = GX_NULL;
    UINT          code_count = 0;
    UINT          index = 0;
    if (_gx_system_text_render_style & GX_TEXT_RENDER_THAI_GLYPH_SHAPING)
    {
        ret = _gx_utility_thai_glyph_shaping(&string_copy, &code_list, &code_count);
    }
#endif
#endif /* GX_UTF8_SUPPORT */

    /* Setup local variables.  */
    xstart = draw_position->x;
    ystart = draw_position->y;

    /* for each character in the string */
    do
    {
#ifdef GX_UTF8_SUPPORT
#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
        if (code_list)
        {
            if (index < code_count)
            {
                char_val = code_list[index++];
            }
            else
            {
                char_val = 0;
            }
        }
        else
        {
#endif
            ret = _gx_utility_utf8_string_character_get(&string_copy, &char_val, GX_NULL);
#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
        }
#endif
        if ((ret != GX_SUCCESS) || (char_val == 0))
#else
        char_val = (GX_CHAR_CODE)(*string_copy.ptr++);
        string_copy.length--;

        if (char_val == 0)
#endif /* GX_UTF8_SUPPORT */
        {
            break;
        }

        font_link = gx_font_find_(font, char_val);
        if (font_link)
        {
            char_val = (GX_CHAR_CODE)(char_val - font_link->first_glyph);
            glyph = &((const GX_COMPRESSED_GLYPH*)font_link->glyphs.compressed)[char_val];

            if (glyph->map)
            {
                gx_canvas_glyph_map_draw_(context, xstart, ystart, view,
                                          font_link, (const GX_GLYPH*)glyph, draw_glyph);
            }

            xstart = (GX_VALUE)(xstart + glyph->advance);
        }
    } while (string_copy.length > 0);

#ifdef GX_UTF8_SUPPORT
#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
    if (code_list)
    {
        _gx_system_memory_free((void*)code_list);
    }
#endif
#endif
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_generic_glyphs_draw                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function prepares to draw text.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    font                                  Font used by the string       */
/*    draw_position                         Coord of draw position        */
/*    string                                Pointer to string which need  */
/*                                            to draw                     */
/*    length                                Number of string about to draw*/
/*    view                                  Pointer to view size          */
/*    draw_glyph                            Callback pointer to display   */
/*                                            driver text draw function   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_utf8_string_character_get                               */
/*                                    Get characters of this string       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_canvas_glyph_draw                                               */
/*                                                                        */
/**************************************************************************/
static void _gx_canvas_generic_glyphs_draw(GX_DRAW_CONTEXT* context,
                                           const GX_FONT* font,
                                           const GX_POINT* draw_position,
                                           GX_STRING string_copy,
                                           const GX_RECTANGLE* view,
                                           void (*draw_glyph)(GX_DRAW_CONTEXT*, GX_RECTANGLE*, GX_POINT*, const GX_GLYPH*))
{
    const GX_GLYPH* glyph;
    GX_CHAR_CODE    char_val;
    GX_VALUE        xstart;
    GX_VALUE        ystart;
    const GX_FONT*  font_link;

#ifdef GX_UTF8_SUPPORT
    UINT          ret = GX_SUCCESS;
#ifdef GX_THAI_GLYPH_SHAPING_SUPPORT
    GX_CHAR_CODE* code_list = GX_NULL;
    UINT          code_count = 0;
    UINT          index = 0;
    if (_gx_system_text_render_style & GX_TEXT_RENDER_THAI_GLYPH_SHAPING)
    {
        ret = _gx_utility_thai_glyph_shaping(&string_copy, &code_list, &code_count);
    }
#endif
#endif /* GX_UTF8_SUPPORT */

    /* Setup local variables.  */
    xstart = draw_position->x;
    ystart = draw_position->y;

    /* for each character in the string */
    do
    {
#ifdef GX_UTF8_SUPPORT
#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
        if (code_list)
        {
            if (index < code_count)
            {
                char_val = code_list[index++];
            }
            else
            {
                char_val = 0;
            }
        }
        else
        {
#endif
            ret = _gx_utility_utf8_string_character_get(&string_copy, &char_val, GX_NULL);
#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
        }
#endif
        if ((ret != GX_SUCCESS) || (char_val == 0))
#else
        char_val = (GX_CHAR_CODE)(*string_copy.ptr++);
        string_copy.length--;

        if (char_val == 0)
#endif /* GX_UTF8_SUPPORT */
        {
            break;
        }

        font_link = gx_font_find_(font, char_val);
        if (font_link)
        {
            char_val = (GX_CHAR_CODE)(char_val - font_link->first_glyph);
            glyph = &font_link->glyphs.normal[char_val];

            if (glyph->map)
            {
                gx_canvas_glyph_map_draw_(context, xstart, ystart, view,
                                          font_link, glyph, draw_glyph);
            }

            xstart = (GX_VALUE)(xstart + glyph->advance);
        }
    } while (string_copy.length > 0);

#ifdef GX_UTF8_SUPPORT
#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
    if (code_list)
    {
        _gx_system_memory_free((void*)code_list);
    }
#endif
#endif
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_glyphs_draw                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function prepares to draw text.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    draw_position                         Coord of draw position        */
/*    string                                Pointer to string which need  */
/*                                            to draw                     */
/*    length                                Number of string about to draw*/
/*    view                                  Pointer to view size          */
/*    draw_glyph                            Callback pointer to display   */
/*                                            driver text draw function   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_utf8_string_character_get                               */
/*                                    Get characters of this string       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_canvas_compressed_glyphs_draw                                   */
/*    _gx_canvas_kerning_glyphs_draw                                      */
/*    _gx_canvas_generic_glyphs_draw                                      */
/*                                                                        */
/**************************************************************************/
void _gx_canvas_glyphs_draw(GX_DRAW_CONTEXT* context, const GX_POINT* draw_position,
                            const GX_STRING* string, const GX_RECTANGLE* view,
                            void (*draw_glyph)(GX_DRAW_CONTEXT*, GX_RECTANGLE*, GX_POINT*, const GX_GLYPH*))
{
    GX_FONT* font;
    GX_STRING string_copy;

    font = context->brush.font;

    if (!string || !string->ptr || !font)
    {
        return;
    }

    string_copy = *string;

    if (font->format & GX_FONT_FORMAT_COMPRESSED)
    {
        _gx_canvas_compressed_glyphs_draw(context, font, draw_position, string_copy, view, draw_glyph);
        return;
    }

#if defined(GX_FONT_KERNING_SUPPORT)
    if (font->format & GX_FONT_FORMAT_KERNING)
    {
        _gx_canvas_kerning_glyphs_draw(context, font, draw_position, string_copy, view, draw_glyph);
        return;
    }
#endif

    _gx_canvas_generic_glyphs_draw(context, font, draw_position, string_copy, view, draw_glyph);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_block_move.c                                             */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function copies a block of pixel from one reference position   */
/*    to another.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    block                                 Rectangle to move             */
/*    dx                                    Distance to move in x         */
/*                                            direction                   */
/*    dy                                    Distance to move in y         */
/*                                            direction                   */
/*    dirty                                 Flag of dirty block           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_inside_detect   Detect if a second rectangle  */
/*                                            is completely within the    */
/*                                            first one                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    _gx_window_client_scroll                                            */
/*                                                                        */
/**************************************************************************/
UINT _gx_canvas_block_move(GX_RECTANGLE* block, GX_VALUE dx, GX_VALUE dy, GX_RECTANGLE* dirty)
{
    GX_DRAW_CONTEXT* context = _gx_system_current_draw_context;
    if (!context)
    {
        return GX_FAILURE;
    }

    GX_DISPLAY* display = context->display;
    if (!display || !display->driver_block_move || (context->canvas->status & GX_CANVAS_PARTIAL_FRAME_BUFFER))
    {
        return GX_FAILURE;
    }

    UINT result = GX_FAILURE;

    for (GX_VIEW* view = context->view_head; view; view = view->next)
    {
        if (view->contain_rectangle_(*block))
        {
            context->clip = block;
            display->driver_block_move(context, block, dx, dy);
            result = GX_SUCCESS;
            break;
        }
    }

    if (result == GX_SUCCESS && dirty)
    {
        // we were able to aid drawing by doing a block move.
        //   Mark the remainder that was not updated as dirty
        //   so that the caller can redraw that portion

        *dirty = *block;

        if (dx > 0)
        {
            dirty->right = (GX_VALUE)(dirty->left + dx - 1);
        }
        else if (dx < 0)
        {
            dirty->left = (GX_VALUE)(dirty->right + dx + 1);
        }
        else
        {
            // no x move
        }

        if (dy > 0)
        {
            dirty->bottom = (GX_VALUE)(dirty->top + dy - 1);
        }
        else if (dy < 0)
        {
            dirty->top = (GX_VALUE)(dirty->bottom + dy + 1);
        }
        else
        {
            // no y move
        }
    }

    return result;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_canvas_composite_create                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks to see if we have a composite canvas created,  */
/*    and if we have an overlay canvas visible. If both are true, the     */
/*    function invokes display driver functions to build the composite    */
/*    canvas and returns a pointer to the resulting composite.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    return_composite                      Return pointer                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_canvas_copy]       copy canvas                   */
/*    [gx_display_driver_canvas_blend]      blend canvas                  */
/*    gx_utility_rectangle_shift            move a rectangle              */
/*    gx_utility_rectangle_combine          combine (add) two rectangles  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_system_canvas_refresh                                           */
/*                                                                        */
/**************************************************************************/
GX_CANVAS* _gx_canvas_composite_create(void)
{
    // look for a composite canvas and multiple visible managed canvases.
    // Sum the the invalid areas along the way

    GX_CANVAS* composite = GX_NULL;

    GX_CANVAS* last_canvas = GX_NULL;
    UINT canvas_count = 0;
    UINT dirty_canvas_count = 0;

    GX_RECTANGLE dirty_sum;
    dirty_sum.set_(GX_VALUE_MAX, GX_VALUE_MAX, -1, -1);

    GX_CANVAS* canvas = _gx_system_canvas_created_list;
    for (; canvas; canvas = canvas->created_next)
    {
        if (canvas->status & GX_CANVAS_COMPOSITE)
        {
            composite = canvas;
        }
        else if ((canvas->status & GX_CANVAS_MANAGED_VISIBLE) == GX_CANVAS_MANAGED_VISIBLE)
        {
            if (canvas->draw_count > 0)
            {
                GX_RECTANGLE dirty = canvas->dirty_area;
                dirty.move_(canvas->display_offset_x, canvas->display_offset_y);

                dirty_sum.combine_(dirty);

                ++dirty_canvas_count;
            }

            ++canvas_count;
        }

        last_canvas = canvas;
    }

    if (!composite || canvas_count < 2)
    {
        return GX_NULL;
    }

    // if we have found both a composite and multiple visible managed canvases,
    // then build the composite.

    if (dirty_canvas_count == 0) // nothing has changed
    {
        composite->draw_count = 0;

        return composite;
    }

    composite->clamp_(&dirty_sum);

    composite->dirty_area = dirty_sum;

    canvas_count = 0;

    // start at the last (lowest) canvas
    for (canvas = last_canvas; canvas; canvas = canvas->created_previous)
    {
        if ((canvas->status & (GX_CANVAS_MANAGED_VISIBLE | GX_CANVAS_COMPOSITE)) == GX_CANVAS_MANAGED_VISIBLE)
        {
            // if this canvas is first to draw into composite,
            // or if this canvas is not blended, copy it into the composite
            if (canvas_count == 0 || canvas->alpha == GX_ALPHA_VALUE_OPAQUE)
            {
                canvas->display->driver_canvas_copy(canvas, composite);
            }
            else
            {
                if (canvas->alpha != 0 && canvas->display->driver_canvas_blend)
                {
                    canvas->display->driver_canvas_blend(canvas, composite);
                }
            }

            ++canvas_count;
        }

        canvas->dirty_count = 0;
        canvas->draw_count = 0;
    }

    composite->draw_count = dirty_canvas_count;

    return composite;
}
