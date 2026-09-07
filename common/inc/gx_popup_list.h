#ifndef GX_POPUP_LIST_H
#define GX_POPUP_LIST_H

#include "gx_vertical_list.h"

struct GX_POPUP_LIST : public GX_VERTICAL_LIST
{
    GX_WIDGET* owner;

    void create_(GX_WIDGET* owner, INT total_rows,
                 void (*callback)(GX_VERTICAL_LIST*, GX_WIDGET*, INT));

    UINT event_process_(GX_EVENT* event_ptr);
};

UINT _gx_popup_list_event_process(GX_POPUP_LIST* popup_list, GX_EVENT* event_ptr);

#endif
