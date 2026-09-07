/***************************************************************************
*  Copyright (c) 2024 Microsoft Corporation
*  Copyright (c) 2026 Eclipse ThreadX contributors
*
*  This program and the accompanying materials are made available under the
*  terms of the MIT License which is available at
*  https://opensource.org/licenses/MIT.
*
*  SPDX-License-Identifier: MIT
**** **********************************************************************/

#include "gx_studio_widget.h"

#include <string.h>

//--------------------------------------------------------------------------------------------------
// Display

static GX_DISPLAY* g_primary_display = GX_NULL;

UINT gx_studio_display_config(const GX_STUDIO_DISPLAY_INFO* display_info,
                              UINT (*driver)(GX_DISPLAY*),
                              GX_UBYTE language,
                              USHORT theme,
                              GX_WINDOW_ROOT** return_root)
{
    GX_DISPLAY* display = display_info->display;

    if (!g_primary_display)
    {
        g_primary_display = display;
    }

    gx_display_create(display,
                      display_info->name,
                      driver,
                      (GX_VALUE)display_info->x_resolution,
                      (GX_VALUE)display_info->y_resolution);

    if (display_info->theme_table)
    {
        const GX_THEME* theme_ptr = display_info->theme_table[theme];
        if (theme_ptr)
        {
            gx_display_theme_install(display, theme_ptr);
        }
    }

    if (display_info->language_table)
    {
        gx_display_language_table_set_ext(display,
                                          display_info->language_table,
                                          (GX_UBYTE)display_info->language_table_size,
                                          display_info->string_table_size);
        gx_display_active_language_set(display, language);
    }


    display->rotation_angle = display_info->rotation_angle;

    gx_canvas_create(display_info->canvas,
                     display_info->canvas_name,
                     display,
                     GX_CANVAS_MANAGED | GX_CANVAS_VISIBLE,
                     display_info->x_resolution,
                     display_info->y_resolution,
                     display_info->canvas_memory,
                     display_info->canvas_memory_size);

    GX_RECTANGLE size;
    gx_utility_rectangle_define(&size,
                                0, 0,
                                (GX_VALUE)(display_info->x_resolution - 1),
                                (GX_VALUE)(display_info->y_resolution - 1));

    gx_window_root_create(display_info->root_window,
                          display_info->name,
                          display_info->canvas, GX_STYLE_NONE, 0, &size);
    if (return_root)
    {
    *    return_root = display_info->root_window;
    }

    return GX_SUCCESS;
}

//--------------------------------------------------------------------------------------------------
// Action, Event Process

static GX_WIDGET* gx_studio_action_target_get(GX_WIDGET* current, const GX_STUDIO_ACTION* action)
{
    GX_WIDGET* parent = GX_NULL;
    GX_WIDGET* target = GX_NULL;

    if (action->flags & GX_ACTION_FLAG_DYNAMIC_TARGET)
    {
        // dynamically create the target widget

        INT search_depth;
        GX_STUDIO_WIDGET* widget_define = (GX_STUDIO_WIDGET*)action->target;
        if (action->flags & GX_ACTION_FLAG_DYNAMIC_PARENT)
        {
            gx_window_root_find(current, (GX_WINDOW_ROOT**)&parent);
            search_depth = GX_SEARCH_DEPTH_INFINITE;
        }
        else
        {
            parent = (GX_WIDGET*)action->parent;
            search_depth = 1;
        }

        gx_widget_find(parent, widget_define->widget_id, search_depth, &target);

        if (!target)
        {
            target = gx_studio_widget_create(GX_NULL, widget_define, GX_NULL);
        }

        if (target)
        {
            target->status |= GX_STATUS_STUDIO_CREATED;
        }
    }
    else
    {
        target = (GX_WIDGET*)action->target;
    }

    return target;
}

static GX_WIDGET* gx_studio_action_target_find(GX_WIDGET* current, const GX_STUDIO_ACTION* action)
{
    GX_WIDGET* parent = GX_NULL;
    GX_WIDGET* target = GX_NULL;

    if (action->flags & GX_ACTION_FLAG_DYNAMIC_TARGET)
    {
        // Find the dynamically created target

        GX_STUDIO_WIDGET* widget_define = (GX_STUDIO_WIDGET*)action->target;
        if (action->flags & GX_ACTION_FLAG_DYNAMIC_PARENT)
        {
            gx_window_root_find(current, (GX_WINDOW_ROOT**)&parent);
        }
        else
        {
            parent = (GX_WIDGET*)action->parent;
        }

        gx_widget_find(parent, widget_define->widget_id, GX_SEARCH_DEPTH_INFINITE, &target);
    }
    else
    {
        target = (GX_WIDGET*)action->target;
    }

    return target;
}

static GX_WIDGET* gx_studio_action_parent_find(GX_WIDGET* current, const GX_STUDIO_ACTION* action)
{
    GX_WIDGET* parent = GX_NULL;

    if (action->flags & GX_ACTION_FLAG_DYNAMIC_PARENT)
    {
        // Find the dynamically created parent

        GX_STUDIO_WIDGET* widget_define = (GX_STUDIO_WIDGET*)action->parent;
        gx_window_root_find(current, (GX_WINDOW_ROOT**)&parent);
        gx_widget_find(parent, widget_define->widget_id, GX_SEARCH_DEPTH_INFINITE, &parent);
    }
    else
    {
        parent = (GX_WIDGET*)action->parent;
    }

    return parent;
}

static VOID gx_studio_animation_execute(GX_WIDGET* current, const GX_STUDIO_ACTION* action)
{
    GX_ANIMATION* animation;
    gx_system_animation_get(&animation);
    if (animation)
    {
        GX_WIDGET* parent = GX_NULL;
        GX_WIDGET* target = GX_NULL;

        GX_ANIMATION_INFO animation_info =* action->animation;

        if ((action->flags & GX_ACTION_FLAG_POP_TARGET) ||
            (action->flags & GX_ACTION_FLAG_POP_PARENT))
        {
            gx_system_screen_stack_get((GX_WIDGET**)&parent, &target);
        }

        if (action->flags & GX_ACTION_FLAG_POP_TARGET)
        {
            animation_info.target = target;
        }

        if (action->flags & GX_ACTION_FLAG_POP_PARENT)
        {
            animation_info.parent = (GX_WIDGET*)parent;
        }

        if ((!animation_info.target) &&
            (action->flags & GX_ACTION_FLAG_DYNAMIC_TARGET))
        {
            target = gx_studio_action_target_get(current, action);
            animation_info.target = target;
        }

        if (!animation_info.parent)
        {
            animation_info.parent = gx_studio_action_parent_find(current, action);
        }

        if (animation_info.target &&
            animation_info.parent)
        {
            gx_animation_start(animation, &animation_info);
        }
    }
}

UINT gx_studio_auto_event_handler(GX_WIDGET* widget, GX_EVENT* event_ptr, const GX_STUDIO_EVENT_PROCESS* record)
{
    UINT status = GX_SUCCESS;

    const GX_STUDIO_ACTION* action;
    const GX_WIDGET* parent = GX_NULL;
    GX_WIDGET* target = GX_NULL;

    const GX_STUDIO_EVENT_ENTRY* entry = record->event_table;
    while (entry->event_type)
    {
        if (entry->event_type == event_ptr->type)
        {
            if ((entry->event_type == GX_EVENT_ANIMATION_COMPLETE) &&
                (entry->event_sender != event_ptr->sender))
            {
                ++entry;
                continue;
            }

            action = entry->action_list;
            while (action->opcode)
            {
                switch (action->opcode)
                {
                case GX_ACTION_TYPE_ATTACH:
                    if ((action->flags & GX_ACTION_FLAG_POP_TARGET) ||
                        (action->flags & GX_ACTION_FLAG_POP_PARENT))
                    {
                        gx_system_screen_stack_get((GX_WIDGET**)&parent, &target);
                    }

                    if (!(action->flags & GX_ACTION_FLAG_POP_PARENT))
                    {
                        parent = (GX_WIDGET*)action->parent;
                    }
                    if (!(action->flags & GX_ACTION_FLAG_POP_TARGET))
                    {
                        target = gx_studio_action_target_get(widget, action);
                    }
                    if (parent && target)
                    {
                        gx_widget_attach(parent, target);
                    }
                    break;

                case GX_ACTION_TYPE_DETACH:
                    target = gx_studio_action_target_find(widget, action);
                    if (target)
                    {
                        gx_widget_detach(target);
                        if (target->status & GX_STATUS_STUDIO_CREATED)
                        {
                            if (widget == target)
                            {
                                widget = GX_NULL;
                            }

                            gx_widget_delete(target);
                        }
                    }
                    break;

                case GX_ACTION_TYPE_TOGGLE:
                    if (action->flags & GX_ACTION_FLAG_POP_TARGET)
                    {
                        gx_system_screen_stack_get(GX_NULL, &target);
                    }
                    else
                    {
                        target = gx_studio_action_target_get(widget, action);
                    }
                    parent = widget->parent;
                    if (parent)
                    {
                        gx_widget_detach(widget);
                        gx_widget_attach(parent, target);
                        if (widget->status & GX_STATUS_STUDIO_CREATED)
                        {
                            gx_widget_delete(widget);
                            widget = GX_NULL;
                        }
                    }
                    break;

                case GX_ACTION_TYPE_SHOW:
                    target = gx_studio_action_target_get(widget, action);
                    if (target)
                    {
                        gx_widget_show(target);
                    }
                    break;

                case GX_ACTION_TYPE_HIDE:
                    target = gx_studio_action_target_find(widget, action);
                    if (target)
                    {
                        gx_widget_hide(target);
                    }
                    break;

                case GX_ACTION_TYPE_ANIMATION:
                    gx_studio_animation_execute(widget, action);
                    break;

                case GX_ACTION_TYPE_WINDOW_EXECUTE:
                    if ((action->flags & GX_ACTION_FLAG_POP_TARGET) ||
                        (action->flags & GX_ACTION_FLAG_POP_PARENT))
                    {
                        gx_system_screen_stack_get((GX_WIDGET**)&parent, &target);
                    }

                    if (!(action->flags & GX_ACTION_FLAG_POP_PARENT))
                    {
                        parent = widget->parent;
                    }
                    if (!(action->flags & GX_ACTION_FLAG_POP_TARGET))
                    {
                        target = gx_studio_action_target_get(widget, action);
                    }
                    if (parent && target)
                    {
                        gx_widget_attach(parent, target);
                        gx_window_execute((GX_WINDOW*)target, GX_NULL);
                    }
                    break;

                case GX_ACTION_TYPE_WINDOW_EXECUTE_STOP:
                    return event_ptr->sender;

                case GX_ACTION_TYPE_SCREEN_STACK_PUSH:
                    target = gx_studio_action_target_get(widget, action);
                    if (target)
                    {
                        gx_system_screen_stack_push(target);
                    }
                    break;

                case GX_ACTION_TYPE_SCREEN_STACK_POP:
                    gx_system_screen_stack_pop();
                    break;

                case GX_ACTION_TYPE_SCREEN_STACK_RESET:
                    gx_system_screen_stack_reset();
                    break;

                default:
                    break;
                }

                ++action;
            }
        }

        ++entry;
    }

    if (widget && record->chain_event_handler)
    {
        status = record->chain_event_handler(widget, event_ptr);
    }

    return status;
}

//--------------------------------------------------------------------------------------------------
// Widget create functions

UINT gx_studio_button_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    return gx_button_create((GX_BUTTON*)control_block, info->widget_name, parent, info->style, info->widget_id, &info->size);
}

UINT gx_studio_text_button_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_TEXT_BUTTON* button = (GX_TEXT_BUTTON*)control_block;
    GX_TEXT_BUTTON_PROPERTIES* props = (GX_TEXT_BUTTON_PROPERTIES*)info->properties;
    UINT status = gx_text_button_create(button, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_text_button_font_set(button, props->font_id);
        gx_text_button_text_color_set(button, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
    }
    return status;
}

UINT gx_studio_multi_line_text_button_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_MULTI_LINE_TEXT_BUTTON* button = (GX_MULTI_LINE_TEXT_BUTTON*)control_block;
    GX_ML_TEXT_BUTTON_PROPERTIES* props = (GX_ML_TEXT_BUTTON_PROPERTIES*)info->properties;
    UINT status = gx_multi_line_text_button_create(button, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_text_button_font_set((GX_TEXT_BUTTON*)button, props->font_id);
        gx_text_button_text_color_set((GX_TEXT_BUTTON*)button, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
    }
    return status;
}

UINT gx_studio_checkbox_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_CHECKBOX* button = (GX_CHECKBOX*)control_block;
    GX_TEXT_BUTTON* text_button = (GX_TEXT_BUTTON*)button;
    GX_CHECKBOX_PROPERTIES* props = (GX_CHECKBOX_PROPERTIES*)info->properties;
    UINT status = gx_checkbox_create(button, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_text_button_font_set(text_button, props->font_id);
        gx_text_button_text_color_set(text_button, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);

        if (props->unchecked_pixelmap_id ||
            props->checked_pixelmap_id ||
            props->unchecked_disabled_pixelmap_id ||
            props->checked_disabled_pixelmap_id)
        {
            gx_checkbox_pixelmap_set(button,
                                     props->unchecked_pixelmap_id,
                                     props->checked_pixelmap_id,
                                     props->unchecked_disabled_pixelmap_id,
                                     props->checked_disabled_pixelmap_id);
        }
    }
    return status;
}

UINT gx_studio_radio_button_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_RADIO_BUTTON* button = (GX_RADIO_BUTTON*)control_block;
    GX_TEXT_BUTTON* text_button = (GX_TEXT_BUTTON*)button;
    GX_RADIO_BUTTON_PROPERTIES* props = (GX_RADIO_BUTTON_PROPERTIES*)info->properties;
    UINT status = gx_radio_button_create(button, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_text_button_font_set(text_button, props->font_id);
        gx_text_button_text_color_set(text_button, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);

        if (props->off_pixelmap_id ||
            props->on_pixelmap_id ||
            props->off_disabled_pixelmap_id ||
            props->on_disabled_pixelmap_id)
        {
            gx_radio_button_pixelmap_set(button,
                                     props->off_pixelmap_id,
                                     props->on_pixelmap_id,
                                     props->off_disabled_pixelmap_id,
                                     props->on_disabled_pixelmap_id);
        }
    }
    return status;
}

UINT gx_studio_icon_button_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_ICON_BUTTON* button = (GX_ICON_BUTTON*)control_block;
    GX_ICON_BUTTON_PROPERTIES* props = (GX_ICON_BUTTON_PROPERTIES*)info->properties;
    return gx_icon_button_create(button, info->widget_name, parent, props->pixelmap_id, info->style, info->widget_id, &info->size);
}

UINT gx_studio_pixelmap_button_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_PIXELMAP_BUTTON* button = (GX_PIXELMAP_BUTTON*)control_block;
    GX_PIXELMAP_BUTTON_PROPERTIES* props = (GX_PIXELMAP_BUTTON_PROPERTIES*)info->properties;
    return gx_pixelmap_button_create(button, info->widget_name, parent,
               props->normal_pixelmap_id,
               props->selected_pixelmap_id,
               props->disabled_pixelmap_id,
               info->style, info->widget_id, &info->size);
}

UINT gx_studio_icon_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_ICON* icon = (GX_ICON*)control_block;
    GX_ICON_PROPERTIES* props = (GX_ICON_PROPERTIES*)info->properties;
    UINT status = gx_icon_create(icon, info->widget_name, parent, props->normal_pixelmap_id, info->style, info->widget_id, info->size.left, info->size.top);
    if (props->selected_pixelmap_id)
    {
        gx_icon_pixelmap_set(icon, props->normal_pixelmap_id, props->selected_pixelmap_id);
    }
    else
    {
        gx_widget_resize((GX_WIDGET*)icon, (GX_RECTANGLE*)&info->size);
    }
    return status;
}

UINT gx_studio_slider_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_SLIDER* slider = (GX_SLIDER*)control_block;
    GX_SLIDER_PROPERTIES* props = (GX_SLIDER_PROPERTIES*)info->properties;
    GX_SLIDER_INFO slider_info;
    slider_info.min_val = props->minval;
    slider_info.max_val = props->maxval;
    slider_info.current_val = props->current_val;
    slider_info.increment = props->increment;
    slider_info.min_travel = props->min_travel;
    slider_info.max_travel = props->max_travel;
    slider_info.needle_width = props->needle_width;
    slider_info.needle_height = props->needle_height;
    slider_info.needle_inset = props->needle_inset;
    slider_info.needle_hotspot_offset = props->needle_hotspot;
    return gx_slider_create(slider,
                    info->widget_name,
                    parent,
                    props->tickmark_count,
                    &slider_info,
                    info->style,
                    info->widget_id,
                    &info->size);
}

UINT gx_studio_pixelmap_slider_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_PIXELMAP_SLIDER* slider = (GX_PIXELMAP_SLIDER*)control_block;
    GX_PIXELMAP_SLIDER_PROPERTIES* props = (GX_PIXELMAP_SLIDER_PROPERTIES*)info->properties;
    GX_PIXELMAP_SLIDER_INFO pixelmap_info;
    GX_SLIDER_INFO slider_info;
    slider_info.min_val = props->min_val;
    slider_info.max_val = props->max_val;
    slider_info.current_val = props->current_val;
    slider_info.increment = props->increment;
    slider_info.min_travel = props->min_travel;
    slider_info.max_travel = props->max_travel;
    slider_info.needle_width = props->needle_width;
    slider_info.needle_height = props->needle_height;
    slider_info.needle_inset = props->needle_inset;
    slider_info.needle_hotspot_offset = props->needle_hotspot;
    pixelmap_info.lower_background_pixelmap = props->lower_pixelmap;
    pixelmap_info.upper_background_pixelmap = props->upper_pixelmap;
    pixelmap_info.needle_pixelmap = props->needle_pixelmap;
    return gx_pixelmap_slider_create(slider,
                    info->widget_name,
                    parent,
                    &slider_info,
                    &pixelmap_info,
                    info->style,
                    info->widget_id,
                    &info->size);
}

UINT gx_studio_progress_bar_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_PROGRESS_BAR* bar = (GX_PROGRESS_BAR*)control_block;
    GX_PROGRESS_BAR_INFO* bar_info = (GX_PROGRESS_BAR_INFO*)info->properties;
    return gx_progress_bar_create(bar,
                    info->widget_name,
                    parent,
                    bar_info,
                    info->style,
                    info->widget_id,
                    &info->size);
}

UINT gx_studio_radial_progress_bar_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_RADIAL_PROGRESS_BAR* bar = (GX_RADIAL_PROGRESS_BAR*)control_block;
    GX_RADIAL_PROGRESS_BAR_INFO* bar_info = (GX_RADIAL_PROGRESS_BAR_INFO*)info->properties;
    return gx_radial_progress_bar_create(bar,
                    info->widget_name,
                    parent,
                    bar_info,
                    info->style,
                    info->widget_id);
}

UINT gx_studio_radial_slider_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_RADIAL_SLIDER* slider = (GX_RADIAL_SLIDER*)control_block;
    GX_RADIAL_SLIDER_INFO slider_info;
    GX_RADIAL_SLIDER_PROPERTIES* props = (GX_RADIAL_SLIDER_PROPERTIES*)info->properties;
    memset(&slider_info, 0, sizeof(GX_RADIAL_SLIDER_INFO));
    slider_info.xcenter = props->xcenter;
    slider_info.ycenter = props->ycenter;
    slider_info.radius = props->radius;
    slider_info.track_width = props->track_width;
    slider_info.needle_offset = props->needle_offset;
    slider_info.current_angle = props->current_angle;
    slider_info.min_angle = props->min_angle;
    slider_info.max_angle = props->max_angle;
    slider_info.background_pixelmap = props->background_pixelmap;
    slider_info.needle_pixelmap = props->needle_pixelmap;
    UINT status = gx_radial_slider_create(slider,
                    info->widget_name,
                    parent,
                    &slider_info,
                    info->style,
                    info->widget_id,
                    &info->size);
    gx_radial_slider_animation_set(slider, props->animation_total_steps, props->animation_delay, props->animation_style, props->animation_update_callback);
    return status;
}

UINT gx_studio_sprite_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_SPRITE* sprite = (GX_SPRITE*)control_block;
    GX_SPRITE_PROPERTIES* props = (GX_SPRITE_PROPERTIES*)info->properties;
    return gx_sprite_create(sprite, info->widget_name, parent,
               props->frame_list, props->frame_count,
               info->style, info->widget_id, &info->size);
}

UINT gx_studio_prompt_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_PROMPT* prompt = (GX_PROMPT*)control_block;
    GX_PROMPT_PROPERTIES* props = (GX_PROMPT_PROPERTIES*)info->properties;
    UINT status = gx_prompt_create(prompt, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_prompt_font_set(prompt, props->font_id);
        gx_prompt_text_color_set(prompt, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
    }
    return status;
}

UINT gx_studio_numeric_prompt_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_NUMERIC_PROMPT* prompt = (GX_NUMERIC_PROMPT*)control_block;
    GX_NUMERIC_PROMPT_PROPERTIES* props = (GX_NUMERIC_PROMPT_PROPERTIES*)info->properties;
    UINT status = gx_numeric_prompt_create(prompt, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_prompt_font_set((GX_PROMPT*)prompt, props->font_id);
        gx_prompt_text_color_set((GX_PROMPT*)prompt, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
        if (!props->string_id)
        {
            gx_numeric_prompt_value_set(prompt, props->numeric_prompt_value);
        }
        if (props->format_func)
        {
            gx_numeric_prompt_format_function_set(prompt, props->format_func);
        }
    }
    return status;
}

UINT gx_studio_pixelmap_prompt_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_PIXELMAP_PROMPT* pix_prompt = (GX_PIXELMAP_PROMPT*)control_block;
    GX_PROMPT* prompt = (GX_PROMPT*)pix_prompt;
    GX_PIXELMAP_PROMPT_PROPERTIES* props = (GX_PIXELMAP_PROMPT_PROPERTIES*)info->properties;
    UINT status = gx_pixelmap_prompt_create(pix_prompt, info->widget_name, parent,
               props->string_id,
               props->fill_map_id,
               info->style, info->widget_id, &info->size);

    if (status == GX_SUCCESS)
    {
        gx_pixelmap_prompt_pixelmap_set(pix_prompt,
                                        props->left_map_id,
                                        props->fill_map_id,
                                        props->right_map_id,
                                        props->selected_left_map_id,
                                        props->selected_fill_map_id,
                                        props->selected_right_map_id);
        gx_prompt_font_set(prompt, props->font_id);
        gx_prompt_text_color_set(prompt, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
    }
    return status;
}

UINT gx_studio_numeric_pixelmap_prompt_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_NUMERIC_PIXELMAP_PROMPT* pix_prompt = (GX_NUMERIC_PIXELMAP_PROMPT*)control_block;
    GX_PROMPT* prompt = (GX_PROMPT*)pix_prompt;
    GX_NUMERIC_PIXELMAP_PROMPT_PROPERTIES* props = (GX_NUMERIC_PIXELMAP_PROMPT_PROPERTIES*)info->properties;
    UINT status = gx_numeric_pixelmap_prompt_create(pix_prompt, info->widget_name, parent,
               props->string_id,
               props->fill_map_id,
               info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (!props->string_id)
        {
            gx_numeric_pixelmap_prompt_value_set(pix_prompt, props->numeric_prompt_value);
        }
        if (props->format_func)
        {
            gx_numeric_pixelmap_prompt_format_function_set(pix_prompt, props->format_func);
        }
        gx_pixelmap_prompt_pixelmap_set((GX_PIXELMAP_PROMPT*)pix_prompt,
                                        props->left_map_id,
                                        props->fill_map_id,
                                        props->right_map_id,
                                        props->selected_left_map_id,
                                        props->selected_fill_map_id,
                                        props->selected_right_map_id);
        gx_prompt_font_set(prompt, props->font_id);
        gx_prompt_text_color_set(prompt, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
    }
    return status;
}

UINT gx_studio_window_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_WINDOW* window = (GX_WINDOW*)control_block;
    GX_WINDOW_PROPERTIES* props = (GX_WINDOW_PROPERTIES*)info->properties;
    UINT status = gx_window_create(window, info->widget_name, parent, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set(window, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
    }
    return status;
}

UINT gx_studio_vertical_list_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_VERTICAL_LIST* list = (GX_VERTICAL_LIST*)control_block;
    GX_VERTICAL_LIST_PROPERTIES* props = (GX_VERTICAL_LIST_PROPERTIES*)info->properties;
    UINT status = gx_vertical_list_create(list, info->widget_name, parent, props->total_rows,
                                     props->callback, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set((GX_WINDOW*)list, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
    }
    return status;
}

UINT gx_studio_horizontal_list_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_HORIZONTAL_LIST* list = (GX_HORIZONTAL_LIST*)control_block;
    GX_HORIZONTAL_LIST_PROPERTIES* props = (GX_HORIZONTAL_LIST_PROPERTIES*)info->properties;
    UINT status = gx_horizontal_list_create(list, info->widget_name, parent, props->total_rows,
                                     props->callback, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set((GX_WINDOW*)list, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
    }
    return status;
}

UINT gx_studio_drop_list_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_DROP_LIST* list = (GX_DROP_LIST*)control_block;
    GX_DROP_LIST_PROPERTIES* props = (GX_DROP_LIST_PROPERTIES*)info->properties;
    UINT status = gx_drop_list_create(list, info->widget_name, parent,
                                 props->total_rows, props->open_height,
                                 props->callback, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->pixelmap_id)
        {
            gx_drop_list_pixelmap_set(list, props->pixelmap_id);
        }
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set((GX_WINDOW*)&list->popup, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
    }
    return status;
}

UINT gx_studio_scroll_wheel_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_SCROLL_WHEEL* wheel = (GX_SCROLL_WHEEL*)control_block;
    GX_SCROLL_WHEEL_PROPERTIES* props = (GX_SCROLL_WHEEL_PROPERTIES*)info->properties;
    UINT status = gx_scroll_wheel_create(wheel, info->widget_name, parent, props->total_rows,
                                    info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set((GX_WINDOW*)wheel, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
        if (props->selected_background)
        {
            gx_scroll_wheel_selected_background_set(wheel, props->selected_background);
        }
        gx_scroll_wheel_selected_set(wheel, props->selected_row);
        gx_scroll_wheel_gradient_alpha_set(wheel, props->start_alpha, props->end_alpha);
        gx_scroll_wheel_row_height_set(wheel, props->row_height);
    }
    return status;
}

UINT gx_studio_generic_scroll_wheel_create(GX_CONST GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_GENERIC_SCROLL_WHEEL* wheel = (GX_GENERIC_SCROLL_WHEEL*)control_block;
    GX_GENERIC_SCROLL_WHEEL_PROPERTIES* props = (GX_GENERIC_SCROLL_WHEEL_PROPERTIES*)info->properties;
    UINT status = gx_generic_scroll_wheel_create(wheel, info->widget_name, parent, props->total_rows, props->callback,
                                                 info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set((GX_WINDOW*)wheel, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
        if (props->selected_background)
        {
            gx_scroll_wheel_selected_background_set((GX_SCROLL_WHEEL*)wheel, props->selected_background);
        }
        gx_scroll_wheel_selected_set((GX_SCROLL_WHEEL*)wheel, props->selected_row);
        gx_scroll_wheel_gradient_alpha_set((GX_SCROLL_WHEEL*)wheel, props->start_alpha, props->end_alpha);
        gx_generic_scroll_wheel_row_height_set(wheel, props->row_height);
    }
    return status;
}

UINT gx_studio_text_scroll_wheel_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_TEXT_SCROLL_WHEEL* wheel = (GX_TEXT_SCROLL_WHEEL*)control_block;
    GX_TEXT_SCROLL_WHEEL_PROPERTIES* props = (GX_TEXT_SCROLL_WHEEL_PROPERTIES*)info->properties;
    UINT status = gx_text_scroll_wheel_create(wheel, info->widget_name, parent, props->total_rows,
                                              info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set((GX_WINDOW*)wheel, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
        if (props->selected_background)
        {
            gx_scroll_wheel_selected_background_set((GX_SCROLL_WHEEL*)wheel, props->selected_background);
        }
        gx_scroll_wheel_selected_set((GX_SCROLL_WHEEL*)wheel, props->selected_row);
        gx_scroll_wheel_gradient_alpha_set((GX_SCROLL_WHEEL*)wheel, props->start_alpha, props->end_alpha);
        gx_scroll_wheel_row_height_set((GX_SCROLL_WHEEL*)wheel, props->row_height);
        gx_text_scroll_wheel_font_set(wheel, props->normal_font, props->selected_font);;
        gx_text_scroll_wheel_text_color_set(wheel, props->normal_text_color, props->selected_text_color, props->disabled_text_color);
    }
    return status;
}

UINT gx_studio_string_scroll_wheel_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_STRING_SCROLL_WHEEL* wheel = (GX_STRING_SCROLL_WHEEL*)control_block;
    GX_STRING_SCROLL_WHEEL_PROPERTIES* props = (GX_STRING_SCROLL_WHEEL_PROPERTIES*)info->properties;
    UINT status = gx_string_scroll_wheel_create_ext(wheel, info->widget_name, parent, props->total_rows, GX_NULL,
                                               info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->string_id_list)
        {
            gx_string_scroll_wheel_string_id_list_set(wheel, props->string_id_list, props->total_rows);
        }
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set((GX_WINDOW*)wheel, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
        if (props->selected_background)
        {
            gx_scroll_wheel_selected_background_set((GX_SCROLL_WHEEL*)wheel, props->selected_background);
        }
        gx_scroll_wheel_selected_set((GX_SCROLL_WHEEL*)wheel, props->selected_row);
        gx_scroll_wheel_gradient_alpha_set((GX_SCROLL_WHEEL*)wheel, props->start_alpha, props->end_alpha);
        gx_scroll_wheel_row_height_set((GX_SCROLL_WHEEL*)wheel, props->row_height);
        gx_text_scroll_wheel_font_set((GX_TEXT_SCROLL_WHEEL*)wheel, props->normal_font, props->selected_font);
        gx_text_scroll_wheel_text_color_set((GX_TEXT_SCROLL_WHEEL*)wheel, props->normal_text_color, props->selected_text_color, props->disabled_text_color);
        if (props->callback)
        {
            gx_text_scroll_wheel_callback_set_ext((GX_TEXT_SCROLL_WHEEL*)wheel, (UINT (*)(GX_TEXT_SCROLL_WHEEL*, INT, GX_STRING*))props->callback);
        }
    }
    return status;
}

UINT gx_studio_numeric_scroll_wheel_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_NUMERIC_SCROLL_WHEEL* wheel = (GX_NUMERIC_SCROLL_WHEEL*)control_block;
    GX_NUMERIC_SCROLL_WHEEL_PROPERTIES* props = (GX_NUMERIC_SCROLL_WHEEL_PROPERTIES*)info->properties;
    UINT status = gx_numeric_scroll_wheel_create(wheel, info->widget_name, parent, props->start_val, props->end_val,
                                    info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->wallpaper_id)
        {
            gx_window_wallpaper_set((GX_WINDOW*)wheel, props->wallpaper_id, info->style & GX_STYLE_TILE_WALLPAPER);
        }
        if (props->selected_background)
        {
            gx_scroll_wheel_selected_background_set((GX_SCROLL_WHEEL*)wheel, props->selected_background);
        }

        if (props->total_rows)
        {
            gx_scroll_wheel_total_rows_set((GX_SCROLL_WHEEL*)wheel, props->total_rows);
        }

        gx_scroll_wheel_selected_set((GX_SCROLL_WHEEL*)wheel, props->selected_row);
        gx_scroll_wheel_gradient_alpha_set((GX_SCROLL_WHEEL*)wheel, props->start_alpha, props->end_alpha);
        gx_scroll_wheel_row_height_set((GX_SCROLL_WHEEL*)wheel, props->row_height);
        gx_text_scroll_wheel_font_set((GX_TEXT_SCROLL_WHEEL*)wheel, props->normal_font, props->selected_font);
        gx_text_scroll_wheel_text_color_set((GX_TEXT_SCROLL_WHEEL*)wheel, props->normal_text_color, props->selected_text_color, props->disabled_text_color);
        if (props->callback)
        {
            gx_text_scroll_wheel_callback_set_ext((GX_TEXT_SCROLL_WHEEL*)wheel, (UINT (*)(GX_TEXT_SCROLL_WHEEL*, INT, GX_STRING*))props->callback);
        }
    }
    return status;
}

UINT gx_studio_text_input_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_STRING text;
    GX_SINGLE_LINE_TEXT_INPUT* input = (GX_SINGLE_LINE_TEXT_INPUT*)control_block;
    GX_PROMPT* prompt = (GX_PROMPT*)input;
    GX_SINGLE_LINE_TEXT_INPUT_PROPERTIES* props = (GX_SINGLE_LINE_TEXT_INPUT_PROPERTIES*)info->properties;
    UINT status = gx_single_line_text_input_create(input, info->widget_name, parent, props->buffer, props->buffer_size, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_prompt_font_set(prompt, props->font_id);
        gx_single_line_text_input_text_color_set(input, props->normal_text_color_id, props->selected_text_color_id,
                                                 props->disabled_text_color_id, props->readonly_text_color_id);
        gx_single_line_text_input_fill_color_set(input, input->normal_fill_color, input->selected_fill_color,
                                                 input->disabled_fill_color, props->readonly_fill_color_id);
        if (props->buffer && props->buffer_size > 0 && props->string_id)
        {
            if (g_primary_display)
            {
                gx_display_string_get_ext(g_primary_display, props->string_id, &text);
                if (text.ptr)
                {
                    gx_single_line_text_input_text_set_ext(input, &text);
                }
            }
        }
    }
    return status;
}

UINT gx_studio_multi_line_text_view_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_MULTI_LINE_TEXT_VIEW* view = (GX_MULTI_LINE_TEXT_VIEW*)control_block;
    GX_ML_TEXT_VIEW_PROPERTIES* props = (GX_ML_TEXT_VIEW_PROPERTIES*)info->properties;
    UINT status = gx_multi_line_text_view_create(view, info->widget_name, parent, props->string_id, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_multi_line_text_view_font_set(view, props->font_id);
        gx_multi_line_text_view_text_color_set(view, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
        gx_multi_line_text_view_whitespace_set(view, props->whitespace);
        gx_multi_line_text_view_line_space_set(view, props->line_space);
    }
    return status;
}

UINT gx_studio_multi_line_text_input_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_STRING text;
    GX_MULTI_LINE_TEXT_INPUT* input = (GX_MULTI_LINE_TEXT_INPUT*)control_block;
    GX_MULTI_LINE_TEXT_INPUT_PROPERTIES* props = (GX_MULTI_LINE_TEXT_INPUT_PROPERTIES*)info->properties;
    UINT status = gx_multi_line_text_input_create(input, info->widget_name, parent, props->buffer, props->buffer_size, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_multi_line_text_view_font_set((GX_MULTI_LINE_TEXT_VIEW*)input, props->font_id);
        gx_multi_line_text_input_fill_color_set(input, info->normal_fill_color_id, info->selected_fill_color_id,
                                                info->disabled_fill_color_id, props->readonly_fill_color_id);
        gx_multi_line_text_input_text_color_set(input, props->normal_text_color_id, props->selected_text_color_id,
                                                props->disabled_text_color_id, props->readonly_text_color_id);
        gx_multi_line_text_view_whitespace_set((GX_MULTI_LINE_TEXT_VIEW*)input, props->whitespace);
        gx_multi_line_text_view_line_space_set((GX_MULTI_LINE_TEXT_VIEW*)input, props->line_space);
        if (props->buffer && (props->buffer_size > 0) && props->string_id)
        {
            if (g_primary_display)
            {
                gx_display_string_get_ext(g_primary_display, props->string_id, &text);
                if (text.ptr)
                {
                    gx_multi_line_text_input_text_set_ext(input, &text);
                }
            }
        }
    }
    return status;
}

UINT gx_studio_horizontal_scrollbar_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_SCROLLBAR* scroll = (GX_SCROLLBAR*)control_block;
    GX_SCROLLBAR_APPEARANCE* appearance = (GX_SCROLLBAR_APPEARANCE*)info->properties;
    return gx_horizontal_scrollbar_create(scroll, info->widget_name, parent, appearance, info->style);
}

UINT gx_studio_vertical_scrollbar_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_SCROLLBAR* scroll = (GX_SCROLLBAR*)control_block;
    GX_SCROLLBAR_APPEARANCE* appearance = (GX_SCROLLBAR_APPEARANCE*)info->properties;
    return gx_vertical_scrollbar_create(scroll, info->widget_name, parent, appearance, info->style);
}

UINT gx_studio_circular_gauge_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_CIRCULAR_GAUGE* gauge = (GX_CIRCULAR_GAUGE*)control_block;
    GX_CIRCULAR_GAUGE_PROPERTIES* props = (GX_CIRCULAR_GAUGE_PROPERTIES*)info->properties;
    GX_CIRCULAR_GAUGE_INFO gauge_info;
    gauge_info.needle_pixelmap = props->needle_pixelmap_id;
    gauge_info.needle_xpos = props->needle_xpos;
    gauge_info.needle_ypos = props->needle_ypos;
    gauge_info.needle_xcor = props->needle_xcor;
    gauge_info.needle_ycor = props->needle_ycor;
    gauge_info.animation_steps = props->animation_steps;
    gauge_info.animation_delay = props->animation_delay;
    gauge->normal_pixelmap = props->normal_pixelmap_id;
    gauge->selected_pixelmap = props->selected_pixelmap_id;
    return gx_circular_gauge_create(gauge,
                    info->widget_name,
                    parent,
                    &gauge_info,
                    props->normal_pixelmap_id,
                    info->style,
                    info->widget_id,
                    info->size.left,
                    info->size.top);
}

UINT gx_studio_line_chart_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_LINE_CHART* chart = (GX_LINE_CHART*)control_block;
    GX_LINE_CHART_INFO* chart_info = (GX_LINE_CHART_INFO*)info->properties;
    return gx_line_chart_create(chart, info->widget_name, parent, chart_info, info->style, info->widget_id, &info->size);
}

UINT gx_studio_template_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    UINT status = GX_SUCCESS;

    GX_TEMPLATE_PROPERTIES* props = (GX_TEMPLATE_PROPERTIES*)info->properties;

    GX_STUDIO_WIDGET derived =* props->base_info;

    derived.widget_id =   info->widget_id;
    derived.widget_name = info->widget_name;

    if (info->style & GX_STYLE_ENABLED)
    {
        derived.style |= GX_STYLE_ENABLED;
    }
    else
    {
        derived.style &= ~GX_STYLE_ENABLED;
    }

    if (info->status & GX_STATUS_ACCEPTS_FOCUS)
    {
        derived.status |= GX_STATUS_ACCEPTS_FOCUS;
    }
    else
    {
        derived.status &= ~GX_STATUS_ACCEPTS_FOCUS;
    }

#if defined(GX_WIDGET_USER_DATA)
    derived.user_data = info->user_data;
#endif

    control_block = gx_studio_widget_create((GX_BYTE*)control_block, &derived, parent);
    if (control_block)
    {
        if (info->style & GX_STYLE_DYNAMICALLY_ALLOCATED)
        {
            control_block->status |= GX_STATUS_DYNAMICALLY_ALLOCATED;
        }

        if (derived.size.left != info->size.left ||
            derived.size.top != info->size.top)
        {
            gx_widget_shift(control_block,
                info->size.left - derived.size.left,
                info->size.top - derived.size.top, GX_FALSE);
        }
    }
    else
    {
        status = GX_FAILURE;
    }

    return status;
}

UINT gx_studio_menu_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_MENU* menu = (GX_MENU*)control_block;
    GX_MENU_PROPERTIES* props = (GX_MENU_PROPERTIES*)info->properties;
    UINT status = gx_menu_create(menu, info->widget_name, parent,
               props->string_id,
               props->fill_map_id,
               info->style, info->widget_id, &info->size);

    if (status == GX_SUCCESS)
    {
        menu->list_total_count = props->list_total_count;
        gx_menu_text_offset_set(menu, props->text_x_offset, props->text_y_offset);
        gx_pixelmap_prompt_pixelmap_set((GX_PIXELMAP_PROMPT*)menu,
                                        props->left_map_id,
                                        props->fill_map_id,
                                        props->right_map_id,
                                        props->selected_left_map_id,
                                        props->selected_fill_map_id,
                                        props->selected_right_map_id);
        gx_prompt_font_set((GX_PROMPT*)menu, props->font_id);
        gx_prompt_text_color_set((GX_PROMPT*)menu, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
    }
    return status;
}

UINT gx_studio_accordion_menu_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_ACCORDION_MENU* accordion = (GX_ACCORDION_MENU*)control_block;
    return gx_accordion_menu_create(accordion, info->widget_name,
                                    parent, info->style, info->widget_id, &info->size);
}

UINT gx_studio_tree_view_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_TREE_VIEW* tree = (GX_TREE_VIEW*)control_block;
    GX_TREE_VIEW_PROPERTIES* props = (GX_TREE_VIEW_PROPERTIES*)info->properties;
    UINT status = gx_tree_view_create(tree, info->widget_name, parent,
                                      info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        if (props->expand_map_id || props->collapse_map_id)
        {
            gx_tree_view_root_pixelmap_set(tree, props->expand_map_id, props->collapse_map_id);
        }
        gx_tree_view_indentation_set(tree, props->indentation);
        gx_tree_view_root_line_color_set(tree, props->root_line_color_id);
    }
    return status;
}

UINT gx_studio_rich_text_view_create(const GX_STUDIO_WIDGET* info, GX_WIDGET* control_block, GX_WIDGET* parent)
{
    GX_RICH_TEXT_VIEW* view = (GX_RICH_TEXT_VIEW*)control_block;
    GX_RICH_TEXT_VIEW_PROPERTIES* props = (GX_RICH_TEXT_VIEW_PROPERTIES*)info->properties;
    UINT status = gx_rich_text_view_create(view, info->widget_name, parent, props->string_id, props->fonts, info->style, info->widget_id, &info->size);
    if (status == GX_SUCCESS)
    {
        gx_multi_line_text_view_text_color_set((GX_MULTI_LINE_TEXT_VIEW*)view, props->normal_text_color_id, props->selected_text_color_id, props->disabled_text_color_id);
        gx_multi_line_text_view_whitespace_set((GX_MULTI_LINE_TEXT_VIEW*)view, props->whitespace);
        gx_multi_line_text_view_line_space_set((GX_MULTI_LINE_TEXT_VIEW*)view, props->line_space);
    }
    return status;
}



static GX_WIDGET* gx_studio_nested_widget_create(GX_BYTE* control, const GX_STUDIO_WIDGET* definition, GX_WIDGET* parent)
{
    UINT status = GX_SUCCESS;
    GX_WIDGET* widget = GX_NULL;
    GX_VALUE list_count = 0;
    GX_VALUE list_total_count = 0;

    if (parent && (parent->type == GX_TYPE_MENU))
    {
        list_total_count = ((GX_MENU*)parent)->list_total_count;
    }

    while (definition && status == GX_SUCCESS)
    {
        if (definition->create_function)
        {
            if (definition->style & GX_STYLE_DYNAMICALLY_ALLOCATED)
            {
                status = gx_widget_allocate(&widget, definition->control_block_size);
                if (status != GX_SUCCESS)
                {
                    return GX_NULL;
                }
            }
            else
            {
                if (control == GX_NULL)
                {
                    return GX_NULL;
                }
                widget = (GX_WIDGET*)(control + definition->control_block_offset);
            }

            status = definition->create_function(definition, widget, parent);

            if (status == GX_SUCCESS)
            {
                if (list_count < list_total_count)
                {
                    gx_menu_insert((GX_MENU*)parent, widget);
                    ((GX_MENU*)parent)->list_total_count--;
                    list_count++;
                }

                if (definition->widget_type != GX_TYPE_TEMPLATE)
                {
                    gx_widget_fill_color_set(widget,
                                             definition->normal_fill_color_id,
                                             definition->selected_fill_color_id,
                                             definition->disabled_fill_color_id);
                }

                if (!(definition->status & GX_STATUS_ACCEPTS_FOCUS))
                {
                    gx_widget_status_remove(widget, GX_STATUS_ACCEPTS_FOCUS);
                }

                if (definition->draw_function)
                {
                    gx_widget_draw_set(widget, definition->draw_function);
                }

                if (definition->event_function)
                {
                    gx_widget_event_process_set(widget, definition->event_function);
                }

            #if defined(GX_WIDGET_USER_DATA)
                widget->user_data = definition->user_data;
            #endif

                if (definition->child_widget)
                {
                    gx_studio_nested_widget_create(control, definition->child_widget, widget);
                }
            }
        }

        definition = definition->next_widget;
    }

    return widget;
}

GX_WIDGET* gx_studio_widget_create(GX_BYTE* control, const GX_STUDIO_WIDGET* definition, GX_WIDGET* parent)
{
    GX_WIDGET* widget = gx_studio_nested_widget_create(control, definition, GX_NULL);

    if (parent && widget)
    {
        gx_widget_attach(parent, widget);
    }

    return widget;
}

UINT gx_studio_named_widget_create_from_table(const char* name, GX_WIDGET* parent, GX_WIDGET** new_widget,
                                              const GX_STUDIO_WIDGET_ENTRY* entry)
{
    UINT status = GX_FAILURE;
    GX_WIDGET* widget = GX_NULL;

    while (entry->widget_information)
    {
        if (!strcmp(name, entry->widget_information->widget_name))
        {
            widget = gx_studio_widget_create((GX_BYTE*)entry->widget, entry->widget_information, parent);
            if (widget)
            {
                status = GX_SUCCESS;
            }
            break;
        }

        ++entry;
    }

    if (new_widget)
    {
       * new_widget = widget;
    }

    return GX_NULL;
}
