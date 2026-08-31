#ifndef GX_SCROLL_THUMB_H
#define GX_SCROLL_THUMB_H

#include "gx_button.h"

struct GX_SCROLLBAR;

/* Define Scroll Thumb type.  */
struct GX_SCROLL_THUMB : public GX_BUTTON
{
    GX_RESOURCE_ID pixelmap;
    GX_RESOURCE_ID border_color;
    GX_BOOL        drag_mode;
    INT            start_drag;

    UINT create_(GX_SCROLLBAR* parent, ULONG style);

    bool is_vertical_()   const { return (style & GX_SCROLLBAR_VERTICAL) != 0; }
    bool is_horizontal_() const { return (style & GX_SCROLLBAR_HORIZONTAL) != 0; }

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    INT shift_clamp_(INT shift) const;
};


/* Define scroll thumb management function prototypes.  */

UINT _gx_scroll_thumb_create(GX_SCROLL_THUMB* scroll_thumb, GX_SCROLLBAR* parent, ULONG style);
void _gx_scroll_thumb_draw(GX_SCROLL_THUMB* scroll_thumb);
UINT _gx_scroll_thumb_event_process(GX_SCROLL_THUMB* scroll_thumb, GX_EVENT* event_ptr);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_scroll_thumb_create(GX_SCROLL_THUMB* scroll_thumb, GX_SCROLLBAR* parent, ULONG style,
                              UINT scroll_thumb_control_block_size);
UINT _gxe_scroll_thumb_event_process(GX_SCROLL_THUMB* scroll_thumb, GX_EVENT* event_ptr);


#endif
