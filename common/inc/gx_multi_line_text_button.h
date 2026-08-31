#ifndef GX_MULTI_LINE_TEXT_BUTTON_H
#define GX_MULTI_LINE_TEXT_BUTTON_H

#include "gx_text_button.h"

/* Define Multi-line Text Button type.  */
struct GX_MULTI_LINE_TEXT_BUTTON : public GX_TEXT_BUTTON
{
    INT       line_count;
    GX_STRING lines[GX_MULTI_LINE_TEXT_BUTTON_MAX_LINES];

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent, GX_RESOURCE_ID text_id,
                 ULONG style, USHORT Id, const GX_RECTANGLE* size);

    UINT text_set_ext_(const GX_STRING* string);
    void text_id_set_(GX_RESOURCE_ID text_id);

    void text_draw_();

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void lines_clear_();
    void lines_set_();

    void lines_add_(const GX_CHAR* text);
};


UINT _gx_multi_line_text_button_create(GX_MULTI_LINE_TEXT_BUTTON* button, const GX_CHAR* name,
                                       GX_WIDGET* parent, GX_RESOURCE_ID text_id,
                                       ULONG style, USHORT Id, const GX_RECTANGLE* size);
void _gx_multi_line_text_button_draw(GX_MULTI_LINE_TEXT_BUTTON* button);
UINT _gx_multi_line_text_button_event_process(GX_MULTI_LINE_TEXT_BUTTON* button, GX_EVENT* event_ptr);
void _gx_multi_line_text_button_text_draw(GX_MULTI_LINE_TEXT_BUTTON* button);
UINT _gx_multi_line_text_button_text_set_ext(GX_MULTI_LINE_TEXT_BUTTON* button, const GX_STRING* string);
UINT _gx_multi_line_text_button_text_id_set(GX_MULTI_LINE_TEXT_BUTTON* button, GX_RESOURCE_ID text_id);


UINT _gxe_multi_line_text_button_create(GX_MULTI_LINE_TEXT_BUTTON* button, const GX_CHAR* name,
                                        GX_WIDGET* parent, GX_RESOURCE_ID text_id, ULONG style, USHORT Id,
                                        const GX_RECTANGLE* size,
                                        UINT text_button_control_block_size);
UINT _gxe_multi_line_text_button_event_process(GX_MULTI_LINE_TEXT_BUTTON* button, GX_EVENT* event_ptr);

UINT _gxe_multi_line_text_button_text_set_ext(GX_MULTI_LINE_TEXT_BUTTON* button, const GX_STRING* text);
UINT _gxe_multi_line_text_button_text_id_set(GX_MULTI_LINE_TEXT_BUTTON* button, GX_RESOURCE_ID string_id);

#endif
