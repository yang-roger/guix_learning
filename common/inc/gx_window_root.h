#ifndef GX_WINDOW_ROOT_H
#define GX_WINDOW_ROOT_H

#include "gx_window.h"

/* Define Root Window type.  */
struct GX_WINDOW_ROOT : public GX_WINDOW
{
    GX_CANVAS* canvas;
    GX_BOOL    views_changed;

    UINT create_(const GX_CHAR* name, GX_CANVAS* canvas,
                 ULONG style, USHORT Id, const GX_RECTANGLE* size);
    UINT delete_();

    void all_views_update_();

    UINT event_process_(GX_EVENT* event_ptr);

private:
    void root_view_add_(const GX_RECTANGLE& rect);
    void view_split_(const GX_RECTANGLE& ovesize, const GX_RECTANGLE& original);

    void all_views_free_();
};


UINT _gx_window_root_create(GX_WINDOW_ROOT* root_window, const GX_CHAR* name, GX_CANVAS* canvas,
                            ULONG style, USHORT Id, const GX_RECTANGLE* size);
UINT _gx_window_root_delete(GX_WINDOW_ROOT* root_window);
UINT _gx_window_root_event_process(GX_WINDOW_ROOT* root_window, GX_EVENT* event_ptr);
UINT _gx_window_root_find(GX_WIDGET* widget, GX_WINDOW_ROOT** return_root_window);


UINT _gxe_window_root_create(GX_WINDOW_ROOT* root_window, const GX_CHAR* name, GX_CANVAS* canvas,
                             ULONG style, USHORT Id, const GX_RECTANGLE* size, UINT root_window_control_block_size);
UINT _gxe_window_root_delete(GX_WINDOW_ROOT* root_window);
UINT _gxe_window_root_event_process(GX_WINDOW_ROOT* root_window, GX_EVENT* event_ptr);
UINT _gxe_window_root_find(GX_WIDGET* widget, GX_WINDOW_ROOT** return_root_window);

#endif
