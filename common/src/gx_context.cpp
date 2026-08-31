#include "gx_context.h"

#include "gx_display.h"
#include "gx_system.h"

GX_DRAW_CONTEXT* gx_context_current_()
{
    return _gx_system_current_draw_context;
}

void GX_DRAW_CONTEXT::brush_default_init_()
{
    brush.default_init_();
}

UINT GX_DRAW_CONTEXT::brush_set_(GX_RESOURCE_ID line_color_id, GX_RESOURCE_ID fill_color_id, UINT style)
{
    GX_COLOR line_color = 0;
    UINT result = color_get_(line_color_id, &line_color);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    GX_COLOR fill_color = 0;
    result = color_get_(fill_color_id, &fill_color);
    if (result != GX_SUCCESS)
    {
        return result;
    }

    brush.set_(line_color, fill_color, style);

    return GX_SUCCESS;
}

void GX_DRAW_CONTEXT::brush_pattern_set_(ULONG pattern)
{
    brush.line_pattern = pattern;
    brush.pattern_mask = 0x80000000;
}

UINT GX_DRAW_CONTEXT::color_get_(GX_RESOURCE_ID color_id, GX_COLOR* color)
{
    if (!display)
    {
        *color = 0;
        return GX_INVALID_DISPLAY;
    }

    return display->color_get_(color_id, color);
}

UINT GX_DRAW_CONTEXT::line_color_set_(GX_RESOURCE_ID line_color_id)
{
    GX_COLOR line_color = 0;
    UINT result = color_get_(line_color_id, &line_color);
    if (result == GX_SUCCESS)
    {
        brush.line_color = line_color;
    }

    return result;
}

UINT GX_DRAW_CONTEXT::fill_color_set_(GX_RESOURCE_ID fill_color_id)
{
    GX_COLOR fill_color = 0;
    UINT result = color_get_(fill_color_id, &fill_color);
    if (result == GX_SUCCESS)
    {
        brush.fill_color = fill_color;
    }

    return result;
}

UINT GX_DRAW_CONTEXT::font_get_(GX_RESOURCE_ID font_id, GX_FONT** return_font)
{
    if (!display)
    {
        *return_font = GX_NULL;
        return GX_INVALID_DISPLAY;
    }

    return display->font_get_(font_id, return_font);
}

UINT GX_DRAW_CONTEXT::font_set_(GX_RESOURCE_ID font_id)
{
    GX_FONT* font = GX_NULL;
    UINT result = font_get_(font_id, &font);
    if (result == GX_SUCCESS)
    {
        brush.font = font;
    }

    return result;
}

UINT GX_DRAW_CONTEXT::pixelmap_get_(GX_RESOURCE_ID pixelmap_id, GX_PIXELMAP** return_pixelmap)
{
    if (!display)
    {
        *return_pixelmap = GX_NULL;
        return GX_INVALID_DISPLAY;
    }

    return display->pixelmap_get_(pixelmap_id, return_pixelmap);
}

UINT GX_DRAW_CONTEXT::pixelmap_set_(GX_RESOURCE_ID pixelmap_id)
{
    GX_PIXELMAP* pixelmap = GX_NULL;
    UINT result = pixelmap_get_(pixelmap_id, &pixelmap);
    if (result == GX_SUCCESS)
    {
        brush.pixelmap = pixelmap;
    }

    return result;
}

UINT GX_DRAW_CONTEXT::string_get_ext_(GX_RESOURCE_ID string_id, GX_STRING* return_string)
{
    if (!display)
    {
        return_string->ptr = GX_NULL;
        return_string->length = 0;

        return GX_INVALID_DISPLAY;
    }

    return display->string_get_ext_(string_id, return_string);
}

UINT GX_DRAW_CONTEXT::raw_brush_define_(GX_COLOR line_color, GX_COLOR fill_color, UINT style)
{
    if (!display)
    {
        return GX_INVALID_DISPLAY;
    }

    brush.set_(display->driver_native_color_get(display, line_color),
               display->driver_native_color_get(display, fill_color),
               style);

    return GX_SUCCESS;
}

UINT GX_DRAW_CONTEXT::raw_line_color_set_(GX_COLOR line_color)
{
    if (!display)
    {
        return GX_INVALID_DISPLAY;
    }

    brush.line_color = display->driver_native_color_get(display, line_color);

    return GX_SUCCESS;
}

UINT GX_DRAW_CONTEXT::raw_fill_color_set_(GX_COLOR fill_color)
{
    if (!display)
    {
        return GX_INVALID_DISPLAY;
    }

    brush.fill_color = display->driver_native_color_get(display, fill_color);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_brush_default                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Set the brush of the current display context.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Pointer to context            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_brush_default                     Sets the default brush        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_canvas_drawing_initiate                                         */
/*                                                                        */
/**************************************************************************/

UINT _gx_context_brush_default(GX_DRAW_CONTEXT* context)
{
    context->brush_default_init_();

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_brush_define                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Set the brush of the current display context.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    line_color_id                         Resource ID of line color     */
/*    fill_color_id                         Resource ID of fill color     */
/*    style                                 Style of brush                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_color_get                 Retrieve context color        */
/*    _gx_brush_define                      Define the brush              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_scrollbar_draw                                                  */
/*    _gx_slider_needle_draw                                              */
/*    _gx_widget_border_draw                                              */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_brush_define(GX_RESOURCE_ID line_color_id, GX_RESOURCE_ID fill_color_id, UINT style)
{
    return _gx_system_current_draw_context->brush_set_(line_color_id, fill_color_id, style);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_brush_get                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Get the brush of the current drawing context.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    return_brush                          Pointer to destination for    */
/*                                            brush                       */
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
/*    _gx_single_line_text_input_cursor_dirty_mark                        */
/*    _gx_single_line_text_input_draw                                     */
/*    _gx_widget_text_draw                                                */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_brush_get(GX_BRUSH** return_brush)
{
    *return_brush = &_gx_system_current_draw_context->brush;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_brush_set                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Set the brush of the current screen context.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    brush                                 Pointer to brush              */
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
UINT _gx_context_brush_set(GX_BRUSH* brush)
{
    _gx_system_current_draw_context->brush = *brush;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_brush_pattern_set                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Set the brush patterh of the current screen context.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pattern                               Brush pattern of current      */
/*                                            context                     */
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
UINT _gx_context_brush_pattern_set(ULONG pattern)
{
    _gx_system_current_draw_context->brush_pattern_set_(pattern);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_brush_style_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Set the brush of the current screen context.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    style                                 Brush style of current context*/
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
UINT _gx_context_brush_style_set(UINT style)
{
    _gx_system_current_draw_context->brush.style = style;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_brush_width_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Set the with of the brush of the current screen context.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    width                                 Brush width in pixels of      */
/*                                            current context             */
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
UINT _gx_context_brush_width_set(UINT width)
{
    _gx_system_current_draw_context->brush.width = (GX_VALUE)width;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_color_get                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the color associated with the supplied            */
/*    resource ID from the system color table.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    color_id                              Resource ID of color          */
/*    return_color                          Pointer to destination for    */
/*                                            color                       */
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
/*    _gx_context_brush_define                                            */
/*    _gx_context_fill_color_set                                          */
/*    _gx_context_line_color_set                                          */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_color_get(GX_RESOURCE_ID color_id, GX_COLOR* return_color)
{
    return _gx_system_current_draw_context->color_get_(color_id, return_color);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_line_color_set                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the line color of the current display context.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    line_color_id                         Line color of current context */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_color_get                 Retrieve context color        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_multi_line_text_view_draw                                       */
/*    _gx_single_line_text_input_draw                                     */
/*    _gx_slider_needle_draw                                              */
/*    _gx_slider_tickmarks_draw                                           */
/*    _gx_widget_border_draw                                              */
/*    _gx_widget_text_draw                                                */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_line_color_set(GX_RESOURCE_ID line_color_id)
{
    _gx_system_current_draw_context->line_color_set_(line_color_id);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_fill_color_set                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Set the brush of the current display context.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    fill_color_id                         Resource ID of fill color     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_color_get                 Retrieve context color        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_widget_border_draw                                              */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_fill_color_set(GX_RESOURCE_ID fill_color_id)
{
    return _gx_system_current_draw_context->fill_color_set_(fill_color_id);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_font_get                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets font associated with the specified resource ID.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    resource_id                           Resource ID of font           */
/*    return_font                           Pointer to destination for    */
/*                                            font pointer                */
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
/*    _gx_context_font_set                                                */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_font_get(GX_RESOURCE_ID resource_id, GX_FONT** return_font)
{
    return _gx_system_current_draw_context->font_get_(resource_id, return_font);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_font_set                                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Set the brush of the current screen context.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    font_id                               Font resource ID of current   */
/*                                          context                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_font_get                  Retrieve the font based on    */
/*                                            its resource ID.            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_multi_line_text_view_draw                                       */
/*    _gx_single_line_text_input_draw                                     */
/*    _gx_widget_text_draw                                                */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_font_set(GX_RESOURCE_ID font_id)
{
    return _gx_system_current_draw_context->font_set_(font_id);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_pixelmap_get                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the pixelmap associated with the supplied         */
/*      resource ID.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pixelmap_id                           Pixelmap resource ID          */
/*    return_pixelmap                       Pointer to pixelmap           */
/*                                            destination pointer         */
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
/*    _gx_checkbox_draw                                                   */
/*    _gx_context_pixelmap_set                                            */
/*    _gx_icon_button_draw                                                */
/*    _gx_icon_draw                                                       */
/*    _gx_pixelmap_button_draw                                            */
/*    _gx_pixelmap_prompt_draw                                            */
/*    _gx_pixelmap_slider_draw                                            */
/*    _gx_radio_button_draw                                               */
/*    _gx_scroll_thumb_draw                                               */
/*    _gx_scrollbar_draw                                                  */
/*    _gx_scrollbar_thumb_position_calculate                              */
/*    _gx_window_draw                                                     */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_pixelmap_get(GX_RESOURCE_ID resource_id, GX_PIXELMAP** return_pixelmap)
{
    return _gx_system_current_draw_context->pixelmap_get_(resource_id, return_pixelmap);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_pixelmap_set                                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Set the pixelmap of the current screen context.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pixelmap_id                           Pixelmap resource ID          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_context_pixelmap_get              Retrieve pixelmap image       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_pixelmap_set(GX_RESOURCE_ID pixelmap_id)
{
    return _gx_system_current_draw_context->pixelmap_set_(pixelmap_id);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_raw_brush_define                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Set the brush of the current dispaly context.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    line_color                            Color of line                 */
/*    fill_color                            Color of fill                 */
/*    style                                 Style of brush                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_brush_define                      Define the brush              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_raw_brush_define(GX_COLOR line_color, GX_COLOR fill_color, UINT style)
{
    return _gx_system_current_draw_context->raw_brush_define_(line_color, fill_color, style);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_raw_line_color_set                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the raw line color of the current display context.*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    line_color                            Line color of current context */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_native_color_get]  Display driver color get      */
/*                                            routine                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_raw_line_color_set(GX_COLOR line_color)
{
    return _gx_system_current_draw_context->raw_line_color_set_(line_color);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_raw_fill_color_set                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the raw fill color of the current screen context. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    fill_color                            Color of fill                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_native_color_get]                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_raw_fill_color_set(GX_COLOR fill_color)
{
    return _gx_system_current_draw_context->raw_fill_color_set_(fill_color);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_context_string_get_ext                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service gets the string associated with the supplied           */
/*      resource ID.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    string_id                             String resource ID            */
/*    return_string                         Pointer to string             */
/*                                            destination pointer         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_string_get_ext                                          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_widget_text_id_draw                                             */
/*                                                                        */
/**************************************************************************/
UINT _gx_context_string_get_ext(GX_RESOURCE_ID string_id, GX_STRING* return_string)
{
    return _gx_system_current_draw_context->string_get_ext_(string_id, return_string);
}
