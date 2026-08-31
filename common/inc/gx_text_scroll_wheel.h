#ifndef GX_TEXT_SCROLL_WHEEL_H
#define GX_TEXT_SCROLL_WHEEL_H

#include "gx_scroll_wheel.h"

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
#include "gx_bidi_text_info.h"
#endif

/* Define GX_TEXT_SCROLL_WHEEL_BASE control block type */
struct GX_TEXT_SCROLL_WHEEL : public GX_SCROLL_WHEEL
{
    GX_RESOURCE_ID      normal_font;
    GX_RESOURCE_ID      selected_font;
    GX_RESOURCE_ID      normal_text_color;
    GX_RESOURCE_ID      selected_text_color;
    GX_RESOURCE_ID      disabled_text_color;
    UINT              (*text_get)(GX_TEXT_SCROLL_WHEEL*, INT, GX_STRING*);
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    GX_BIDI_RESOLVED_TEXT_INFO** bidi_resolved_text_info;
#endif

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent, INT total_rows,
                 ULONG style, USHORT Id, const GX_RECTANGLE* size);

    void text_get_callback_set_ext_(UINT (*callback)(GX_TEXT_SCROLL_WHEEL*, INT, GX_STRING*));

    void font_set_(GX_RESOURCE_ID normal_font, GX_RESOURCE_ID selected_font);
    void text_color_set_(GX_RESOURCE_ID normal_text_color,
                         GX_RESOURCE_ID selected_text_color,
                         GX_RESOURCE_ID disabled_text_color);

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

protected:
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
    void dynamic_bidi_text_delete_();
#endif  // GX_DYNAMIC_BIDI_TEXT_SUPPORT

private:
    UINT text_get_(INT row, GX_STRING* string);

    bool trans_row_height_(INT dist, GX_VALUE client_height, INT* trans_height) const;

    void round_draw_();
    void flat_draw_();

    UINT row_draw_(const GX_RECTANGLE& selected_area, const GX_RECTANGLE& draw_area, const GX_STRING* string);

    UINT round_row_draw_(GX_RESOURCE_ID tColor, GX_RESOURCE_ID font_id,
                         const GX_STRING* string,
                         const GX_RECTANGLE& draw_area);

    UINT flat_row_draw_(GX_RESOURCE_ID tColor, GX_RESOURCE_ID font_id,
                        const GX_STRING* string,
                        const GX_RECTANGLE& draw_area);

    void draw_position_get_(GX_VALUE text_width, GX_VALUE text_height,
                            const GX_RECTANGLE& draw_area,
                            GX_VALUE* x_pos, GX_VALUE* y_pos);
};


UINT _gx_text_scroll_wheel_callback_set_ext(GX_TEXT_SCROLL_WHEEL* wheel, UINT (*callback)(GX_TEXT_SCROLL_WHEEL*, INT, GX_STRING*));
UINT _gx_text_scroll_wheel_create(GX_TEXT_SCROLL_WHEEL* wheel, const GX_CHAR* name, GX_WIDGET* parent, INT total_rows,
                                  ULONG style, USHORT Id, const GX_RECTANGLE* size);
void _gx_text_scroll_wheel_draw(GX_TEXT_SCROLL_WHEEL* wheel);
UINT _gx_text_scroll_wheel_event_process(GX_TEXT_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr);
UINT _gx_text_scroll_wheel_font_set(GX_TEXT_SCROLL_WHEEL* wheel, GX_RESOURCE_ID normal_font, GX_RESOURCE_ID selected_font);
UINT _gx_text_scroll_wheel_text_color_set(GX_TEXT_SCROLL_WHEEL* wheel,
                                          GX_RESOURCE_ID normal_text_color,
                                          GX_RESOURCE_ID selected_text_color,
                                          GX_RESOURCE_ID disabled_text_color);


UINT _gxe_text_scroll_wheel_callback_set_ext(GX_TEXT_SCROLL_WHEEL* wheel, UINT (*callback)(GX_TEXT_SCROLL_WHEEL*, INT, GX_STRING*));
UINT _gxe_text_scroll_wheel_create(GX_TEXT_SCROLL_WHEEL* wheel, const GX_CHAR* name, GX_WIDGET* parent, INT total_rows,
                                   ULONG style, USHORT Id, const GX_RECTANGLE* size,
                                   UINT control_block_size);
UINT _gxe_text_scroll_wheel_event_process(GX_TEXT_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr);
UINT _gxe_text_scroll_wheel_font_set(GX_TEXT_SCROLL_WHEEL* wheel, GX_RESOURCE_ID normal_font, GX_RESOURCE_ID selected_font);
UINT _gxe_text_scroll_wheel_text_color_set(GX_TEXT_SCROLL_WHEEL* wheel,
                                           GX_RESOURCE_ID normal_text_color,
                                           GX_RESOURCE_ID selected_text_color,
                                           GX_RESOURCE_ID disabled_text_color);

#endif
