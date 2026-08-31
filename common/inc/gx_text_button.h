#ifndef GX_TEXT_BUTTON_H
#define GX_TEXT_BUTTON_H

#include "gx_button.h"
#include "gx_text.h"

/* Define Text Button type.  */
struct GX_TEXT_BUTTON : public GX_BUTTON
{
    GX_TEXT text_data;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_RESOURCE_ID text_id,
                 ULONG style, USHORT Id,
                 const GX_RECTANGLE* size);

    UINT text_get_ext_(GX_STRING* return_text)  { return text_data.get_and_resolve_bidi_text_(return_text); }
    UINT text_set_ext_(const GX_STRING* string) { return text_data.string_ext_set_(string); }
    void text_id_set_(GX_RESOURCE_ID text_id)   { text_data.text_id_set_(text_id); }

    GX_RESOURCE_ID font_id_() const { return text_data.font_id; }
    void font_set_(GX_RESOURCE_ID font_id) { text_data.font_set_(font_id); }

    GX_RESOURCE_ID& normal_text_color_()   { return text_data.normal_color;   }
    GX_RESOURCE_ID& selected_text_color_() { return text_data.selected_color; }
    GX_RESOURCE_ID& disabled_text_color_() { return text_data.disabled_color; }

    void text_color_set_(GX_RESOURCE_ID normal_text_color_id,
                         GX_RESOURCE_ID selected_text_color_id,
                         GX_RESOURCE_ID disabled_text_color_id)
    {
        text_data.color_set_(normal_text_color_id,
                             selected_text_color_id,
                             disabled_text_color_id);
    }

    GX_RESOURCE_ID text_draw_info_get_(INT* x_offset, INT* y_offset) const;

    void text_draw_();

    void text_draw_(const GX_STRING* text, GX_RESOURCE_ID color_id, INT x_offset, INT y_offset)
    {
        text_draw_ext_(color_id, font_id_(), text, x_offset, y_offset);
    }

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);
};


/* Define text button management function prototypes.  */

UINT _gx_text_button_create(GX_TEXT_BUTTON* button, const GX_CHAR* name, GX_WIDGET* parent,
                            GX_RESOURCE_ID text_id, ULONG style, USHORT Id, const GX_RECTANGLE* size);
void _gx_text_button_draw(GX_TEXT_BUTTON* button);
UINT _gx_text_button_event_process(GX_TEXT_BUTTON* button, GX_EVENT* event_ptr);
UINT _gx_text_button_font_set(GX_TEXT_BUTTON* button, GX_RESOURCE_ID font_id);
UINT _gx_text_button_text_color_set(GX_TEXT_BUTTON* button,
                                    GX_RESOURCE_ID normal_text_color_id,
                                    GX_RESOURCE_ID selected_text_color_id,
                                    GX_RESOURCE_ID disabled_text_color_id);
void _gx_text_button_text_draw(GX_TEXT_BUTTON* button);
UINT _gx_text_button_text_get_ext(GX_TEXT_BUTTON* button, GX_STRING* return_text);
UINT _gx_text_button_text_id_set(GX_TEXT_BUTTON* button, GX_RESOURCE_ID string_id);
UINT _gx_text_button_text_set_ext(GX_TEXT_BUTTON* button, const GX_STRING* text);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_text_button_create(GX_TEXT_BUTTON* button, const GX_CHAR* name, GX_WIDGET* parent,
                             GX_RESOURCE_ID text_id, ULONG style, USHORT Id, const GX_RECTANGLE* size,
                             UINT text_button_control_block_size);
UINT _gxe_text_button_event_process(GX_TEXT_BUTTON* button, GX_EVENT* event_ptr);
UINT _gxe_text_button_font_set(GX_TEXT_BUTTON* button, GX_RESOURCE_ID font_id);
UINT _gxe_text_button_text_color_set(GX_TEXT_BUTTON* button,
                                     GX_RESOURCE_ID normal_text_color_id,
                                     GX_RESOURCE_ID selected_text_color_id,
                                     GX_RESOURCE_ID disabled_text_color_id);

UINT _gxe_text_button_text_get_ext(GX_TEXT_BUTTON* button, GX_STRING* return_text);
UINT _gxe_text_button_text_id_set(GX_TEXT_BUTTON* button, GX_RESOURCE_ID string_id);
UINT _gxe_text_button_text_set_ext(GX_TEXT_BUTTON* button, const GX_STRING* text);

#endif
