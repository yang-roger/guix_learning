#ifndef GX_ICON_BUTTON_H
#define GX_ICON_BUTTON_H

#include "gx_button.h"

/* Defing Icon Button type.  */
struct GX_ICON_BUTTON : public GX_BUTTON
{
    GX_RESOURCE_ID icon_id;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_RESOURCE_ID icon_id, ULONG style,
                 USHORT icon_button_id, const GX_RECTANGLE* size);

    UINT icon_set_(GX_RESOURCE_ID icon_id);

    void draw_();
};


UINT _gx_icon_button_create(GX_ICON_BUTTON* button, const GX_CHAR* name,
                            GX_WIDGET* parent, GX_RESOURCE_ID icon_id, ULONG style,
                            USHORT icon_button_id, const GX_RECTANGLE* size);
void _gx_icon_button_draw(GX_ICON_BUTTON* button);
UINT _gx_icon_button_pixelmap_set(GX_ICON_BUTTON* button, GX_RESOURCE_ID icon_id);


UINT _gxe_icon_button_create(GX_ICON_BUTTON* button, const GX_CHAR* name,
                             GX_WIDGET* parent, GX_RESOURCE_ID icon_id, ULONG style,
                             USHORT icon_button_id, const GX_RECTANGLE* size,
                             UINT button_control_block_size);
UINT _gxe_icon_button_pixelmap_set(GX_ICON_BUTTON* button, GX_RESOURCE_ID pixelmap_id);

#endif
