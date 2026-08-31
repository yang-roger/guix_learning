#ifndef GX_WIDGET_LIST_H
#define GX_WIDGET_LIST_H

#include "gx_window.h"

struct GX_WIDGET_LIST : public GX_WINDOW
{
    void     (*callback)(GX_WIDGET_LIST* list, GX_WIDGET* child, INT index);
    INT        selected;
    INT        pen_index;
    INT        first_visible_index;
    INT        total;
    GX_VALUE   visible_count;
    GX_VALUE   child_count;
    GX_VALUE   child_max_extent;            // width for horizontal, height for vertical
    GX_VALUE   snap_back_distance;
    GX_WIDGET* idle_child_list;
    bool       is_vertical_list;

    void callback_run_(GX_WIDGET* child, INT index)
    {
        callback(this, child, index);
    }

    UINT create_(USHORT type, const GX_CHAR* name, GX_WIDGET* parent, INT total,
                 void (*callback)(GX_WIDGET_LIST*, GX_WIDGET*, INT),
                 ULONG style, USHORT list_id, const GX_RECTANGLE* size);

    bool is_valid_index_(INT index) const
    {
        return (0 <= index) && (index < total);
    }

    UINT children_position_();

    UINT page_index_set_(INT index);
    UINT total_set_(INT count);
    UINT selected_set_(INT index);

    GX_WIDGET* selected_widget_get_() const;

    void scroll_info_get_(ULONG style, GX_SCROLL_INFO* info);

    UINT event_process_(GX_EVENT* event_ptr);

private:
    INT page_index_get_by_child_top_() const;
    INT page_index_get_by_child_bottom_() const;
    INT page_index_get_by_child_left_() const;
    INT page_index_get_by_child_right_() const;

    void selected_visible_(GX_WIDGET* child);
    void selected_visible_v_(GX_WIDGET* child);
    void selected_visible_h_(GX_WIDGET* child);

    void scroll_(INT amount);
    void invisible_page_scroll_(INT num_pages);

    void front_wrap_();
    void back_wrap_();

    void slide_back_check_();

    void slide_back_distance_get_v_();
    void slide_back_distance_get_h_();

    void scrollbar_reset_();

    GX_WIDGET* pen_pos_child_find_(const GX_POINT& pen_pos) const;
    INT pen_index_get_(const GX_POINT& pen_pos) const;

    GX_VALUE extent_() const
    {
        return child_max_extent * child_count;
    }

    GX_VALUE client_extent_() const;

    GX_VALUE client_children_extent_() const;
    GX_VALUE client_children_extent_v_() const;
    GX_VALUE client_children_extent_h_() const;
};

#endif
