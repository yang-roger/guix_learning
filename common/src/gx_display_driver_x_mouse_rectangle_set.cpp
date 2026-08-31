#include "gx_display.h"

#include "gx_canvas.h"
#include "gx_pixelmap.h"

GX_RECTANGLE* _gx_display_driver_mouse_rectangle_set(GX_DISPLAY* display, INT* mouse_rect_width, INT* mouse_rect_height)
{
    GX_RECTANGLE* mouse_rect = &display->mouse.rect;

    GX_RESOURCE_ID image_id = display->mouse.cursor_info->cursor_image_id;
    if (image_id && image_id < display->pixelmap_table_size)
    {
        GX_PIXELMAP* map = display->pixelmap_table[image_id];

        mouse_rect->left = display->mouse.position.x;
        mouse_rect->top = display->mouse.position.y;
        mouse_rect->left = (GX_VALUE)(mouse_rect->left - display->mouse.cursor_info->cursor_hotspot_x);
        mouse_rect->top = (GX_VALUE)(mouse_rect->top - display->mouse.cursor_info->cursor_hotspot_y);

        mouse_rect->right = (GX_VALUE)(mouse_rect->left + map->width - 1);
        mouse_rect->bottom = (GX_VALUE)(mouse_rect->top + map->height - 1);

        GX_CANVAS* canvas = display->mouse.canvas;

        canvas->clamp_(mouse_rect);

        *mouse_rect_width = mouse_rect->width_();
        *mouse_rect_height = mouse_rect->height_();

        return mouse_rect;
    }
    else
    {
        mouse_rect->left = 0;
        mouse_rect->top = 0;
        mouse_rect->right = -1;
        mouse_rect->bottom = -1;

        return GX_NULL;
    }
}

