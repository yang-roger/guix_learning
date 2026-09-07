#ifndef GX_GENERIC_SCROLL_WHEEL_H
#define GX_GENERIC_SCROLL_WHEEL_H

#include "gx_scroll_wheel.h"

/* Define GX_GENERIC_SCROLL_WHEEL control block type. */
struct GX_GENERIC_SCROLL_WHEEL : public GX_SCROLL_WHEEL
{
    void (*callback)(GX_GENERIC_SCROLL_WHEEL*, GX_WIDGET* widget, INT index);
    INT    top_index;
    INT    child_count;
    INT    visible_rows;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent, INT total_rows,
                 void (*callback)(GX_GENERIC_SCROLL_WHEEL*, GX_WIDGET*, INT),
                 ULONG style, USHORT Id, const GX_RECTANGLE* size);

    void children_position_();

    void total_rows_set_(INT count);
    void row_height_set_(GX_VALUE row_height);

    UINT scroll_(GX_VALUE shift);
    GX_BOOL is_wrap_style_() const;

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void up_wrap_();
    void down_wrap_();
};


UINT _gx_generic_scroll_wheel_children_position(GX_GENERIC_SCROLL_WHEEL* wheel);
UINT _gx_generic_scroll_wheel_create(GX_GENERIC_SCROLL_WHEEL* wheel,
                                     const GX_CHAR* name,
                                     GX_WIDGET* parent,
                                     INT total_rows,
                                     void (*callback)(GX_GENERIC_SCROLL_WHEEL*, GX_WIDGET*, INT),
                                     ULONG style,
                                     USHORT id,
                                     const GX_RECTANGLE* size);
void _gx_generic_scroll_wheel_draw(GX_GENERIC_SCROLL_WHEEL* wheel);
UINT _gx_generic_scroll_wheel_event_process(GX_GENERIC_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr);
UINT _gx_generic_scroll_wheel_row_height_set(GX_GENERIC_SCROLL_WHEEL* wheel, GX_VALUE row_height);
UINT _gx_generic_scroll_wheel_total_rows_set(GX_GENERIC_SCROLL_WHEEL* wheel, INT count);


UINT _gxe_generic_scroll_wheel_children_position(GX_GENERIC_SCROLL_WHEEL* wheel);
UINT _gxe_generic_scroll_wheel_create(GX_GENERIC_SCROLL_WHEEL* wheel,
                                      const GX_CHAR* name,
                                      GX_WIDGET* parent,
                                      INT total_rows,
                                      void (*callback)(GX_GENERIC_SCROLL_WHEEL*, GX_WIDGET*, INT),
                                      ULONG style,
                                      USHORT id,
                                      const GX_RECTANGLE* size,
                                      UINT control_block_size);
UINT _gxe_generic_scroll_wheel_event_process(GX_GENERIC_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr);
UINT _gxe_generic_scroll_wheel_row_height_set(GX_GENERIC_SCROLL_WHEEL* wheel, GX_VALUE row_height);
UINT _gxe_generic_scroll_wheel_total_rows_set(GX_GENERIC_SCROLL_WHEEL* wheel, INT count);

#endif
