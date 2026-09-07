#ifndef GX_NUMERIC_SCROLL_WHEEL_H
#define GX_NUMERIC_SCROLL_WHEEL_H

#include "gx_text_scroll_wheel.h"

/* Define GX_NUMERIC_SCROLL_WHEEL control block type */
struct GX_NUMERIC_SCROLL_WHEEL : public GX_TEXT_SCROLL_WHEEL
{
    INT     start_val;
    INT     end_val;
    GX_CHAR string_buffer[GX_NUMERIC_SCROLL_WHEEL_STRING_BUFFER_SIZE];

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 INT start_val, INT end_val,
                 ULONG style, USHORT Id, const GX_RECTANGLE* size);

    void range_set_(INT start_val, INT end_val);

    UINT text_get_(INT row, GX_STRING* string);
};


UINT _gx_numeric_scroll_wheel_create(GX_NUMERIC_SCROLL_WHEEL* wheel,
                                     const GX_CHAR* name, GX_WIDGET* parent,
                                     INT start_val, INT end_val,
                                     ULONG style, USHORT Id, const GX_RECTANGLE* size);
UINT _gx_numeric_scroll_wheel_range_set(GX_NUMERIC_SCROLL_WHEEL* wheel, INT start_val, INT end_val);


UINT _gxe_numeric_scroll_wheel_create(GX_NUMERIC_SCROLL_WHEEL* wheel, const GX_CHAR* name, GX_WIDGET* parent,
                                      INT start_val, INT end_val,
                                      ULONG style, USHORT Id, const GX_RECTANGLE* size, UINT control_block_size);
UINT _gxe_numeric_scroll_wheel_range_set(GX_NUMERIC_SCROLL_WHEEL* wheel, INT start_val, INT end_val);

#endif
