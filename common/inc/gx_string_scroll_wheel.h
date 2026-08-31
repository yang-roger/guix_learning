#ifndef GX_STRING_SCROLL_WHEEL_H
#define GX_STRING_SCROLL_WHEEL_H

#include "gx_text_scroll_wheel.h"

/* Define GX_STRING_SCROLL_WHEEL control block type */
struct GX_STRING_SCROLL_WHEEL : public GX_TEXT_SCROLL_WHEEL
{
    const GX_STRING* string_list;
    USHORT string_list_buffer_size;
    const GX_RESOURCE_ID* string_id_list;

    UINT create_ext_(const GX_CHAR* name, GX_WIDGET* parent,
                     INT total_rows, const GX_STRING* string_list,
                     ULONG style, USHORT Id, const GX_RECTANGLE* size);

    void string_id_list_set_(const GX_RESOURCE_ID* string_id_list, INT id_count);
    UINT string_list_set_ext_(const GX_STRING* string_list, INT string_count);

    UINT text_get_(INT row, GX_STRING* string);
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void string_list_delete_();
};


UINT _gx_string_scroll_wheel_create_ext(GX_STRING_SCROLL_WHEEL* wheel, const GX_CHAR* name,
                                        GX_WIDGET* parent, INT total_rows, const GX_STRING* string_list,
                                        ULONG style, USHORT Id, const GX_RECTANGLE* size);
UINT _gx_string_scroll_wheel_event_process(GX_STRING_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr);
UINT _gx_string_scroll_wheel_string_id_list_set(GX_STRING_SCROLL_WHEEL* wheel,
                                                const GX_RESOURCE_ID* string_id_list, INT id_count);
UINT _gx_string_scroll_wheel_string_list_set_ext(GX_STRING_SCROLL_WHEEL* wheel,
                                                 const GX_STRING* string_list, INT string_count);


UINT _gxe_string_scroll_wheel_create_ext(GX_STRING_SCROLL_WHEEL* wheel, const GX_CHAR* name,
                                         GX_WIDGET* parent, INT total_rows, const GX_STRING* string_list,
                                         ULONG style, USHORT Id, const GX_RECTANGLE* size, UINT control_block_size);
UINT _gxe_string_scroll_wheel_event_process(GX_STRING_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr);
UINT _gxe_string_scroll_wheel_string_id_list_set(GX_STRING_SCROLL_WHEEL* wheel,
                                                 const GX_RESOURCE_ID* string_id_list, INT id_count);
UINT _gxe_string_scroll_wheel_string_list_set_ext(GX_STRING_SCROLL_WHEEL* wheel,
                                                  const GX_STRING* string_list, INT string_count);

#endif
