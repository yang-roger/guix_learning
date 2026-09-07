#ifndef GX_PIXELMAP_SLIDER_H
#define GX_PIXELMAP_SLIDER_H

#include "gx_slider.h"

struct GX_PIXELMAP_SLIDER_INFO
{
    GX_RESOURCE_ID lower_background_pixelmap;
    GX_RESOURCE_ID upper_background_pixelmap;
    GX_RESOURCE_ID needle_pixelmap;
};

/* Define Pixelmap Slider type.  */
struct GX_PIXELMAP_SLIDER : public GX_SLIDER
{
    GX_PIXELMAP_SLIDER_INFO pixelmap_info;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 const GX_SLIDER_INFO* info,
                 const GX_PIXELMAP_SLIDER_INFO* pixelmap_info,
                 ULONG style, USHORT pixelmap_slider_id,
                 const GX_RECTANGLE* size);

    void pixelmap_set_(const GX_PIXELMAP_SLIDER_INFO* info);

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void pixelmap_update_();
};


UINT _gx_pixelmap_slider_create(GX_PIXELMAP_SLIDER* slider, const GX_CHAR* name, GX_WIDGET* parent,
                                GX_SLIDER_INFO* info, GX_PIXELMAP_SLIDER_INFO* pixelmap_info,
                                ULONG style, USHORT pixelmap_slider_id, const GX_RECTANGLE* size);
void _gx_pixelmap_slider_draw(GX_PIXELMAP_SLIDER* slider);
UINT _gx_pixelmap_slider_event_process(GX_PIXELMAP_SLIDER* slider, GX_EVENT* event_ptr);
UINT _gx_pixelmap_slider_pixelmap_set(GX_PIXELMAP_SLIDER* slider, GX_PIXELMAP_SLIDER_INFO* info);


UINT _gxe_pixelmap_slider_create(GX_PIXELMAP_SLIDER* slider, const GX_CHAR* name, GX_WIDGET* parent,
                                 GX_SLIDER_INFO* info, GX_PIXELMAP_SLIDER_INFO* pixelmap_info,
                                 ULONG style, USHORT pixelmap_slider_id, const GX_RECTANGLE* size,
                                 UINT pixelmap_slider_control_block_size);
UINT _gxe_pixelmap_slider_event_process(GX_PIXELMAP_SLIDER* slider, GX_EVENT* event_ptr);
UINT _gxe_pixelmap_slider_pixelmap_set(GX_PIXELMAP_SLIDER* slider, GX_PIXELMAP_SLIDER_INFO* info);

#endif
