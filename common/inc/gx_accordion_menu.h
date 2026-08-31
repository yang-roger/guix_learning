#ifndef GX_ACCORDION_MENU_H
#define GX_ACCORDION_MENU_H

#include "gx_widget.h"

struct GX_ACCORDION_MENU : public GX_WIDGET
{
    GX_WIDGET* collapse_item;
    GX_WIDGET* expand_item;
    GX_UBYTE   animation_status;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 ULONG style, USHORT accordion_menu_id, const GX_RECTANGLE* size);

    UINT position_();

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    UINT one_level_position_();

    void pen_down_event_process_(GX_EVENT* event_ptr);
    void pen_up_event_process_(GX_EVENT* event_ptr);
    void timer_event_process_(UINT timer_id);

    void open_animation_update_();
    void close_animation_update_();

    void shift_(GX_WIDGET* animation_target, INT shift);
    void shift_helper_(GX_WIDGET* animation_target, INT shift);
};


UINT _gx_accordion_menu_create(GX_ACCORDION_MENU* menu, const GX_CHAR* name, GX_WIDGET* parent,
                               ULONG style, USHORT accordion_menu_id, const GX_RECTANGLE* size);
void _gx_accordion_menu_draw(GX_ACCORDION_MENU* menu);
UINT _gx_accordion_menu_event_process(GX_ACCORDION_MENU* menu, GX_EVENT* event_ptr);
UINT _gx_accordion_menu_position(GX_ACCORDION_MENU* menu);


UINT _gxe_accordion_menu_create(GX_ACCORDION_MENU* menu, const GX_CHAR* name, GX_WIDGET* parent,
                                ULONG style, USHORT accordion_menu_id, const GX_RECTANGLE* size,
                                UINT control_block_size);
UINT _gxe_accordion_menu_event_process(GX_ACCORDION_MENU* menu, GX_EVENT* event_ptr);
UINT _gxe_accordion_menu_position(GX_ACCORDION_MENU* menu);

#endif
