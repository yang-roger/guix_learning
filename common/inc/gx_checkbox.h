#ifndef GX_CHECKBOX_H
#define GX_CHECKBOX_H

#include "gx_radio_button.h"

/* Define Checkbox type.  */
struct GX_CHECKBOX : public GX_RADIO_BUTTON
{
    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_RESOURCE_ID text_id, ULONG style, USHORT checkbox_id,
                 const GX_RECTANGLE* size);

    void select_();

    UINT event_process_(GX_EVENT* event_ptr);
};


UINT _gx_checkbox_create(GX_CHECKBOX* checkbox, const GX_CHAR* name, GX_WIDGET* parent,
                         GX_RESOURCE_ID text_id, ULONG style,
                         USHORT checkbox_id, const GX_RECTANGLE* size);
void _gx_checkbox_draw(GX_CHECKBOX* checkbox);
UINT _gx_checkbox_event_process(GX_CHECKBOX* checkbox, GX_EVENT* event_ptr);
UINT _gx_checkbox_pixelmap_set(GX_CHECKBOX* checkbox, GX_RESOURCE_ID unchecked_id, GX_RESOURCE_ID checked_id,
                               GX_RESOURCE_ID unchecked_disabled_id, GX_RESOURCE_ID checked_disabled_id);
UINT _gx_checkbox_select(GX_CHECKBOX* checkbox);


UINT _gxe_checkbox_create(GX_CHECKBOX* checkbox, const GX_CHAR* name, GX_WIDGET* parent,
                          GX_RESOURCE_ID text_id, ULONG style,
                          USHORT checkbox_id, const GX_RECTANGLE* size,
                          UINT checkbox_control_block_size);
UINT _gxe_checkbox_event_process(GX_CHECKBOX* checkbox, GX_EVENT* event_ptr);
UINT _gxe_checkbox_pixelmap_set(GX_CHECKBOX* checkbox, GX_RESOURCE_ID unchecked_id, GX_RESOURCE_ID checked_id,
                                GX_RESOURCE_ID unchecked_disabled_id, GX_RESOURCE_ID checked_disabled_id);
UINT _gxe_checkbox_select(GX_CHECKBOX* checkbox);

#endif
