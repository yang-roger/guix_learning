#ifndef GX_RADIO_BUTTON_H
#define GX_RADIO_BUTTON_H

#include "gx_text_button.h"

/* Define Radio Button type.  */
struct GX_RADIO_BUTTON : public GX_TEXT_BUTTON
{
    GX_RESOURCE_ID off_pixelmap_id;
    GX_RESOURCE_ID on_pixelmap_id;
    GX_RESOURCE_ID off_disabled_pixelmap_id;
    GX_RESOURCE_ID on_disabled_pixelmap_id;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_RESOURCE_ID text_id, ULONG style, USHORT radio_button_id,
                 const GX_RECTANGLE* size);

    UINT pixelmap_set_(GX_RESOURCE_ID off_id, GX_RESOURCE_ID on_id,
                       GX_RESOURCE_ID off_disabled_id, GX_RESOURCE_ID on_disabled_id);

    void draw_();
};


UINT _gx_radio_button_create(GX_RADIO_BUTTON* button, const GX_CHAR* name, GX_WIDGET* parent, GX_RESOURCE_ID text_id,
                             ULONG style, USHORT radio_button_id, const GX_RECTANGLE* size);
void _gx_radio_button_draw(GX_RADIO_BUTTON* button);
UINT _gx_radio_button_pixelmap_set(GX_RADIO_BUTTON* button, GX_RESOURCE_ID off_id, GX_RESOURCE_ID on_id,
                                   GX_RESOURCE_ID off_disabled_id, GX_RESOURCE_ID on_disabled_id);


UINT _gxe_radio_button_create(GX_RADIO_BUTTON* button, const GX_CHAR* name, GX_WIDGET* parent, GX_RESOURCE_ID text_id,
                              ULONG style, USHORT radio_button_id, const GX_RECTANGLE* size,
                              UINT radio_button_control_block_size);
UINT _gxe_radio_button_pixelmap_set(GX_RADIO_BUTTON* button, GX_RESOURCE_ID off_id, GX_RESOURCE_ID on_id,
                                    GX_RESOURCE_ID off_disabled_id, GX_RESOURCE_ID on_disabled_id);

#endif
