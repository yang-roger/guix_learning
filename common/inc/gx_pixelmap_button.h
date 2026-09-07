#ifndef GX_PIXELMAP_BUTTON_H
#define GX_PIXELMAP_BUTTON_H

#include "gx_button.h"

/* Define Pixelmap Button type.  */
struct GX_PIXELMAP_BUTTON : public GX_BUTTON
{
    GX_RESOURCE_ID normal_id;
    GX_RESOURCE_ID selected_id;
    GX_RESOURCE_ID disabled_id;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_RESOURCE_ID normal_id, GX_RESOURCE_ID selected_id, GX_RESOURCE_ID disabled_id,
                 ULONG style, USHORT pixelmap_button_id,
                 const GX_RECTANGLE* size);

    void pixelmap_set_(GX_RESOURCE_ID normal_id, GX_RESOURCE_ID selected_id, GX_RESOURCE_ID disabled_id);

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void transparent_detect_();
};


UINT _gx_pixelmap_button_create(GX_PIXELMAP_BUTTON* button, const GX_CHAR* name, GX_WIDGET* parent,
                                GX_RESOURCE_ID normal_id, GX_RESOURCE_ID selected_id, GX_RESOURCE_ID disabled_id,
                                ULONG style, USHORT pixelmap_button_id, const GX_RECTANGLE* size);
void _gx_pixelmap_button_draw(GX_PIXELMAP_BUTTON* button);
UINT _gx_pixelmap_button_event_process(GX_PIXELMAP_BUTTON* button, GX_EVENT* event_ptr);
UINT _gx_pixelmap_button_pixelmap_set(GX_PIXELMAP_BUTTON* button,
                                      GX_RESOURCE_ID normal_id,
                                      GX_RESOURCE_ID selected_id,
                                      GX_RESOURCE_ID disabled_id);


UINT _gxe_pixelmap_button_create(GX_PIXELMAP_BUTTON* button, const GX_CHAR* name, GX_WIDGET* parent,
                                 GX_RESOURCE_ID normal_id, GX_RESOURCE_ID selected_id, GX_RESOURCE_ID disabled_id,
                                 ULONG style, USHORT pixelmap_button_id, const GX_RECTANGLE* size,
                                 UINT button_control_block_size);
UINT _gxe_pixelmap_button_event_process(GX_PIXELMAP_BUTTON* button, GX_EVENT* event_ptr);
UINT _gxe_pixelmap_button_pixelmap_set(GX_PIXELMAP_BUTTON* button,
                                       GX_RESOURCE_ID normal_id,
                                       GX_RESOURCE_ID selected_id,
                                       GX_RESOURCE_ID disabled_id);

#endif
