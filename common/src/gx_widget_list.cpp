#include "gx_widget_list.h"

#include "gx_system.h"
#include "gx_scroll_bar.h"

static UINT gx_widget_list_event_process_(GX_WIDGET_LIST* list, GX_EVENT* event_ptr)
{
    return list->event_process_(event_ptr);
}

static void gx_widget_list_scroll_info_get_(GX_WIDGET_LIST* list, ULONG style, GX_SCROLL_INFO* info)
{
    list->scroll_info_get_(style, info);
}

UINT GX_WIDGET_LIST::create_(USHORT type, const GX_CHAR* name, GX_WIDGET* parent, INT total,
                             void (*callback)(GX_WIDGET_LIST*, GX_WIDGET*, INT),
                             ULONG style, USHORT list_id, const GX_RECTANGLE* size)
{
    GX_WINDOW::create_(name, GX_NULL, style, list_id, size);

    this->type = type;

    this->is_vertical_list = (type == GX_TYPE_VERTICAL_LIST);

    this->status |= GX_STATUS_NAV_PARENT;

    this->total = total;
    this->first_visible_index = 0;
    this->pen_index = -1;
    this->child_max_extent = 1;
    this->selected = 0;
    this->visible_count = 0;
    this->child_count = 0;
    this->idle_child_list = GX_NULL;

    this->callback = callback;

    this->event_process_function = (UINT (*)(GX_WIDGET *, GX_EVENT *))gx_widget_list_event_process_;
    this->scroll_info_get = (void (*)(GX_WINDOW*, ULONG, GX_SCROLL_INFO*))gx_widget_list_scroll_info_get_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

GX_VALUE GX_WIDGET_LIST::client_extent_() const
{
    if (is_vertical_list)
    {
        return client.height_();
    }
    else // GX_TYPE_HORIZONTAL_LIST
    {
        return client.width_();
    }
}

GX_VALUE GX_WIDGET_LIST::client_children_extent_() const
{
    if (callback)
    {
        // If list callback is set, children within the list should share the same extent.
        return (GX_VALUE)(total * child_max_extent);
    }
    else
    {
        if (is_vertical_list)
        {
            return client_children_extent_v_();
        }
        else // GX_TYPE_HORIZONTAL_LIST
        {
            return client_children_extent_h_();
        }
    }
}

GX_VALUE GX_WIDGET_LIST::client_children_extent_v_() const
{
    GX_VALUE extent = 0;

    GX_WIDGET* child = first_client_child_get_();
    while (child)
    {
        extent += child->height_();
        child = child->next_client_child_get_();
    }

    return extent;
}

GX_VALUE GX_WIDGET_LIST::client_children_extent_h_() const
{
    GX_VALUE extent = 0;

    GX_WIDGET* child = first_client_child_get_();
    while (child)
    {
        extent += child->width_();
        child = child->next_client_child_get_();
    }

    return extent;
}

UINT GX_WIDGET_LIST::children_position_()
{
    GX_RECTANGLE child_size = client;
    INT index = first_visible_index;

    child_max_extent = 0;
    child_count = 0;

    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        if (child->is_client_())
        {
            ++child_count;

            if (!child->id)
            {
                child->id = (USHORT)(GX_LIST_CHILD_ID_START + child_count);
            }

            if (index == selected)
            {
                child->style |= GX_STYLE_DRAW_SELECTED;
            }
            else
            {
                child->style &= ~GX_STYLE_DRAW_SELECTED;
            }

            ++index;

            child->status_remove_(GX_STATUS_ACCEPTS_FOCUS);

            if (is_vertical_list)
            {
                GX_VALUE height = child->height_();

                if (child_max_extent < height)
                {
                    child_max_extent = height;
                }

                child_size.bottom = (GX_VALUE)(child_size.top + height - 1);
                child->resize_(&child_size);

                child_size.top = (GX_VALUE)(child_size.bottom + 1);
            }
            else // GX_TYPE_HORIZONTAL_LIST
            {
                GX_VALUE width = child->width_();

                if (child_max_extent < width)
                {
                    child_max_extent = width;
                }

                child_size.right = (GX_VALUE)(child_size.left + width - 1);
                child->resize_(&child_size);

                child_size.left = (GX_VALUE)(child_size.right + 1);
            }
        }
    }

    if (child_max_extent > 0)
    {
        visible_count = (GX_VALUE)((client_extent_() + child_max_extent - 1) / child_max_extent);
    }
    else
    {
        visible_count = 1;
    }

    return GX_SUCCESS;
}

UINT GX_WIDGET_LIST::page_index_set_(INT index)
{
    if (total > 0 &&
        (index <= total - visible_count || (style & GX_STYLE_WRAP)))
    {
        GX_WIDGET* child = first_client_child_get_();
        if (child)
        {
            INT index_child_dist = (index - first_visible_index) * child_max_extent;

            INT scroll_value = is_vertical_list ?
                    (client.top - (child->top_() + index_child_dist)) :
                    (client.left - (child->left_() + index_child_dist));

            scroll_(scroll_value);

            scrollbar_reset_();

            return GX_SUCCESS;
        }
    }

    return GX_FAILURE;
}

INT GX_WIDGET_LIST::page_index_get_by_child_top_() const
{
    INT page_index = first_visible_index;

    GX_WIDGET* child = first_client_child_get_();
    while (child && child->top_() <= client.top)
    {
        ++page_index;
        child = child->next_client_child_get_();
    }

    return page_index;
}

INT GX_WIDGET_LIST::page_index_get_by_child_bottom_() const
{
    INT page_index = first_visible_index;

    GX_WIDGET* child = first_client_child_get_();
    while (child && child->bottom_() <= client.top)
    {
        ++page_index;
        child = child->next_client_child_get_();
    }

    return page_index;
}

INT GX_WIDGET_LIST::page_index_get_by_child_left_() const
{
    INT page_index = first_visible_index;

    GX_WIDGET* child = first_client_child_get_();
    while (child && child->left_() <= client.left)
    {
        ++page_index;
        child = child->next_client_child_get_();
    }

    return page_index;
}

INT GX_WIDGET_LIST::page_index_get_by_child_right_() const
{
    INT page_index = first_visible_index;

    GX_WIDGET* child = first_client_child_get_();
    while (child && child->right_() <= client.left)
    {
        ++page_index;
        child = child->next_client_child_get_();
    }

    return page_index;
}

UINT GX_WIDGET_LIST::total_set_(INT count)
{
    {
        GX_SYSTEM_AUTO_LOCK lock;

        total = count;

        if (selected < 0)
        {
            selected = 0;
        }

        if (selected > count - 1)
        {
            selected = count - 1;
        }

        INT page_index;

        GX_WIDGET* child = first_client_child_get_();

        if (is_vertical_list)
        {
            page_index = page_index_get_by_child_bottom_();
        }
        else // GX_TYPE_HORIZONTAL_LIST
        {
            page_index = page_index_get_by_child_right_();
        }

        // calculate new page index
        if (page_index + visible_count > count)
        {
            if (count > visible_count)
            {
                page_index = count - visible_count;
            }
            else
            {
                page_index = 0;
            }
        }

        // add idle children back to this list
        while (idle_child_list)
        {
            child = idle_child_list;
            idle_child_list = idle_child_list->next;

            child->attach_to_(this);
            ++child_count;
        }

        while (child_count > count)
        {
            child = last_client_child_get_();
            if (!child)
            {
                return GX_FAILURE;
            }

            child->detach_();

            child->next = idle_child_list;
            idle_child_list = child;

            --child_count;
        }

        first_visible_index = 0;

        INT index = 0;

        child = first_client_child_get_();
        while (child)
        {
            callback(this, child, index);
            ++index;

            child = child->next_client_child_get_();
        }

        children_position_();

        page_index_set_(page_index);

        scrollbar_reset_();
    }

    dirty_mark_if_visible_();

    return GX_SUCCESS;
}

UINT GX_WIDGET_LIST::selected_set_(INT index)
{
    if (index == selected)
    {
        if (!(style & GX_STYLE_REPEAT_SELECT))
        {
            return GX_SUCCESS;
        }
    }

    GX_WIDGET* child;

    if (index < 0)
    {
        child = selected_widget_get_();
        if (child)
        {
            child->style &= ~GX_STYLE_DRAW_SELECTED;
            child->dirty_mark_();
        }

        selected = index;

        return GX_SUCCESS;
    }

    INT diff;

    INT first_index = this->first_visible_index;
    INT last_index = first_index + child_count - 1;

    if (last_index >= total)
    {
        last_index -= total;

        if (index >= first_index)
        {
            diff = index - first_visible_index;
        }
        else if (index <= last_index)
        {
            diff = total + index - first_visible_index;
        }
        else
        {
            page_index_set_(index);
            diff = index - first_visible_index;
        }
    }
    else
    {
        if (index < first_index)
        {
            page_index_set_(index);
        }
        else if (index > last_index)
        {
            page_index_set_(index - visible_count + 1);
        }

        diff = index - first_visible_index;
    }

    if (diff < 0)
    {
        diff += total;
    }

    bool found = false;

    child = first_client_child_get_();
    while (child)
    {
        if (diff == 0)
        {
            found = true;

            selected = index;
            selected_visible_(child);
            child->style |= GX_STYLE_DRAW_SELECTED;
            child->dirty_mark_();

            signal_parent_(GX_EVENT_LIST_SELECT, selected);
        }
        else
        {
            if (child->style & GX_STYLE_DRAW_SELECTED)
            {
                child->style &= ~GX_STYLE_DRAW_SELECTED;
                child->dirty_mark_();
            }
        }

        --diff;
        child = child->next_client_child_get_();
    }

    return found ? GX_SUCCESS : GX_FAILURE;
}

GX_WIDGET* GX_WIDGET_LIST::selected_widget_get_() const
{
    INT diff = selected - first_visible_index;
    if (diff < 0)
    {
        diff += total;
    }

    GX_WIDGET* child = first_client_child_get_();
    while (child && diff > 0)
    {
        --diff;
        child = child->next_client_child_get_();
    }

    return child;
}

void GX_WIDGET_LIST::selected_visible_(GX_WIDGET* child)
{
    if (is_vertical_list)
    {
        selected_visible_v_(child);
    }
    else // GX_TYPE_HORIZONTAL_LIST
    {
        selected_visible_h_(child);
    }
}

void GX_WIDGET_LIST::selected_visible_v_(GX_WIDGET* child)
{
    if (child->top_() < client.top)
    {
        scroll_(client.top - child->top_());
        scrollbar_reset_();
    }
    else if (child->bottom_() > client.bottom)
    {
        scroll_(client.bottom - child->bottom_());
        scrollbar_reset_();
    }
}

void GX_WIDGET_LIST::selected_visible_h_(GX_WIDGET* child)
{
    if (child->left_() < client.left)
    {
        scroll_(client.left - child->left_());
        scrollbar_reset_();
    }
    else if (child->right_() > client.right)
    {
        scroll_(client.right - child->right_());
        scrollbar_reset_();
    }
}

void GX_WIDGET_LIST::scroll_(INT amount)
{
    if (amount == 0)
    {
        return;
    }

    bool reuse_child_widgets = false;

    if (callback &&
        visible_count < total &&
        (child_count < total || (style & GX_STYLE_WRAP)))
    {
        reuse_child_widgets = true;

        INT page_size = extent_();
        if (page_size == 0)
        {
            return;
        }

        INT num_pages;
        if (amount < 0)
        {
            num_pages = (amount + page_size) / page_size;
        }
        else
        {
            num_pages = (amount - page_size) / page_size;
        }

        if (num_pages)
        {
            amount -= num_pages * page_size;

            invisible_page_scroll_(num_pages);
        }
    }

    // scroll all client children
    for (GX_WIDGET* child = first_child; child; child = child->next)
    {
        if (child->is_client_())
        {
            if (is_vertical_list)
            {
                child->scroll_shift_(0, amount, GX_TRUE);
            }
            else // GX_TYPE_HORIZONTAL_LIST
            {
                child->scroll_shift_(amount, 0, GX_TRUE);
            }
        }
    }

    if (reuse_child_widgets)
    {
        if (amount < 0)
        {
            front_wrap_();
        }
        else
        {
            back_wrap_();
        }
    }

    scrollbar_reset_();

    if (is_visible_())
    {
        bool do_block_move = true;
        GX_RECTANGLE block = client;

        if (!(style & (GX_STYLE_WRAP | GX_STYLE_TRANSPARENT)) &&
            (style & GX_STYLE_BORDER_THIN))
        {
            GX_WIDGET* first = first_client_child_get_();
            if (first)
            {
                GX_WIDGET* last = last_client_child_get_();

                bool dirty = is_vertical_list ?
                    (first->top_() > block.top || last->bottom_() < block.bottom) :
                    (first->left_() > block.left || last->right_() < block.right);

                if (dirty)
                {
                    dirty_mark_();
                    do_block_move = false;

                    // If the widget has thin border, the round corder of the border
                    // will cover the client area, block move will cause trouble.
                }
            }
        }

        if (do_block_move)
        {
            if (is_vertical_list)
            {
                block_move_(&block, 0, amount);
            }
            else // GX_TYPE_HORIZONTAL_LIST
            {
                block_move_(&block, amount, 0);
            }
        }
    }
}

void GX_WIDGET_LIST::invisible_page_scroll_(INT num_pages)
{
    if (!(style & GX_STYLE_WRAP))
    {
        // wrap style is not applied, need to calculate scroll limit.

        INT maximum_scroll_pages;

        if (num_pages < 0)
        {
            // calculate maximum up scroll num_pages

            maximum_scroll_pages = total - first_visible_index - child_count;
            maximum_scroll_pages /= child_count;

            // remain one page
            if (maximum_scroll_pages >= 1)
            {
                --maximum_scroll_pages;
            }

            if (maximum_scroll_pages < -num_pages)
            {
                num_pages = -maximum_scroll_pages;
            }
        }
        else
        {
            // calculate maximum down scroll num_pages

            maximum_scroll_pages = first_visible_index / child_count;

            // remain one page
            if (maximum_scroll_pages >= 1)
            {
                --maximum_scroll_pages;
            }

            if (maximum_scroll_pages < num_pages)
            {
                num_pages = maximum_scroll_pages;
            }
        }
    }

    if (num_pages)
    {
        first_visible_index -= num_pages * child_count;

        if (style & GX_STYLE_WRAP)
        {
            if (num_pages < 0)
            {
                while (first_visible_index >= total)
                {
                    first_visible_index -= total;
                }
            }
            else
            {
                while (first_visible_index < 0)
                {
                    first_visible_index += total;
                }
            }
        }
    }
}

void GX_WIDGET_LIST::front_wrap_()
{
    GX_WIDGET* test;
    GX_WIDGET* check;
    GX_RECTANGLE newpos;
    INT index;

    bool moving = true;
    while (moving)
    {
        moving = false;

        if (first_visible_index + child_count < total ||
            (style & GX_STYLE_WRAP))
        {
            test = first_client_child_get_();
            if (test)
            {
                bool need_moving = is_vertical_list ?
                        (test->bottom_() < client.top) :
                        (test->right_() < client.left);
                if (need_moving)
                {
                    moving = true;

                    check = last_client_child_get_();

                    test->detach_();

                    newpos = test->size;

                    if (is_vertical_list)
                    {
                        newpos.move_(0, (GX_VALUE)(check->bottom_() - test->top_() + 1));
                    }
                    else // GX_TYPE_HORIZONTAL_LIST
                    {
                        newpos.move_((GX_VALUE)(check->right_() - test->left_() + 1), 0);
                    }

                    test->resize_(&newpos);

                    index = first_visible_index + child_count;
                    if (index >= total)
                    {
                        index -= total;
                    }

                    callback(this, test, index);

                    if (index == selected)
                    {
                        test->style |=  GX_STYLE_DRAW_SELECTED;
                    }
                    else
                    {
                        test->style &= ~GX_STYLE_DRAW_SELECTED;
                    }

                    test->attach_to_(this);

                    ++first_visible_index;
                    if (first_visible_index >= total)
                    {
                        first_visible_index -= total;
                    }
                }
            }
        }
    }
}

void GX_WIDGET_LIST::back_wrap_()
{
    GX_WIDGET* test;
    GX_WIDGET* check;
    GX_RECTANGLE newpos;

    bool moving = true;
    while (moving)
    {
        moving = false;

        if (first_visible_index > 0 ||
            (style & GX_STYLE_WRAP))
        {
            test = last_client_child_get_();
            if (test)
            {
                bool need_moving = is_vertical_list ?
                        (test->top_() > bottom_()) :
                        (test->left_() > right_());
                if (need_moving)
                {
                    moving = true;

                    --first_visible_index;
                    if (first_visible_index < 0)
                    {
                        first_visible_index = total - 1;
                    }

                    check = first_client_child_get_();
                    if (!check)
                    {
                        break;
                    }

                    test->detach_();

                    newpos = test->size;

                    if (is_vertical_list)
                    {
                        newpos.move_(0, (GX_VALUE)(-(test->bottom_() - check->top_() + 1)));
                    }
                    else // GX_TYPE_HORIZONTAL_LIST
                    {
                        newpos.move_((GX_VALUE)(-(test->right_() - check->left_() + 1)), 0);
                    }

                    test->resize_(&newpos);

                    callback(this, test, first_visible_index);

                    if (first_visible_index == selected)
                    {
                        test->style |=  GX_STYLE_DRAW_SELECTED;
                    }
                    else
                    {
                        test->style &= ~GX_STYLE_DRAW_SELECTED;
                    }

                    test->back_attach_to_(this);
                }
            }
        }
    }
}

void GX_WIDGET_LIST::slide_back_check_()
{
    if (child_count <= 0)
    {
        return;
    }

    if (is_vertical_list)
    {
        slide_back_distance_get_v_();
    }
    else
    {
        slide_back_distance_get_h_();
    }

    if (snap_back_distance != 0)
    {
        timer_start_(GX_SNAP_TIMER, 1, 1);
    }
}

void GX_WIDGET_LIST::slide_back_distance_get_v_()
{
    snap_back_distance = 0;

    INT page_index = page_index_get_by_child_top_();

    if (page_index == 0)
    {
        GX_WIDGET* first = first_client_child_get_();
        if (first && first->top_() > client.top)
        {
            snap_back_distance = (GX_VALUE)(client.top - first->top_());
        }
    }
    else if (first_visible_index + child_count >= total - 1)
    {
        GX_WIDGET* last = last_client_child_get_();
        if (last && last->bottom_() < client.bottom)
        {
            snap_back_distance = (GX_VALUE)(client.bottom - last->bottom_());
        }
    }
}

void GX_WIDGET_LIST::slide_back_distance_get_h_()
{
    snap_back_distance = 0;

    INT page_index = page_index_get_by_child_left_();

    if (page_index == 0)
    {
        GX_WIDGET* first = first_client_child_get_();
        if (first && first->left_() > client.left)
        {
            snap_back_distance = (GX_VALUE)(client.left - first->left_());
        }
    }
    else if (first_visible_index + child_count >= total - 1)
    {
        GX_WIDGET* last = last_client_child_get_();
        if (last && last->right_() < client.right)
        {
            snap_back_distance = (GX_VALUE)(client.right - last->right_());
        }
    }
}

void GX_WIDGET_LIST::scrollbar_reset_()
{
    USHORT scrollbar_type = is_vertical_list ?
                                GX_TYPE_VERTICAL_SCROLL : GX_TYPE_HORIZONTAL_SCROLL;

    GX_SCROLLBAR* scrollbar = scrollbar_find_(scrollbar_type);
    if (scrollbar)
    {
        scrollbar->reset_();
    }
}

void GX_WIDGET_LIST::scroll_info_get_(ULONG style, GX_SCROLL_INFO* info)
{
    GX_PARAMETER_NOT_USED(style);

    info->maximum = client_children_extent_();
    if (info->maximum > 0)
    {
        --info->maximum;
    }

    info->minimum = 0;
    info->visible = client_extent_();

    if (info->maximum <= info->visible)
    {
        info->value = 0;
        info->increment = 0;
        info->maximum = info->visible;

        return;
    }

    INT value = first_visible_index * child_max_extent;

    if (first_visible_index >= 0)
    {
        GX_WIDGET* child = first_client_child_get_();
        if (child)
        {
            if (is_vertical_list)
            {
                value += client.top - child->top_();
            }
            else // GX_TYPE_HORIZONTAL_LIST
            {
                value += client.left - child->left_();
            }
        }
    }

    if (value < info->minimum)
    {
        value = info->minimum;
    }
    else if (value > info->maximum - info->visible + 1)
    {
        value = info->maximum - info->visible + 1;
    }

    info->value = value;
    info->increment = child_max_extent / 2;
}

GX_WIDGET* GX_WIDGET_LIST::pen_pos_child_find_(const GX_POINT& pen_pos) const
{
    GX_WIDGET* child = top_child_in_point_(pen_pos, GX_STATUS_SELECTABLE);

    while (child && child->parent != this)
    {
        child = child->parent;
    }

    return child;
}

INT GX_WIDGET_LIST::pen_index_get_(const GX_POINT& pen_pos) const
{
    INT index = -1;

    GX_WIDGET* child = pen_pos_child_find_(pen_pos);
    if (child)
    {
        index = first_visible_index + client_child_index_get_(child);
        if (index >= total)
        {
            index -= total;
        }
    }

    return index;
}

UINT GX_WIDGET_LIST::event_process_(GX_EVENT* event_ptr)
{
    UINT result = GX_SUCCESS;

    INT pos_diff;
    INT new_pen_index;
    UINT timer_id;

    switch (event_ptr->type)
    {
    case GX_EVENT_SHOW:
        // show the children before attempting to position them,
        // because child widgets often do not know their size until shown

        result = GX_WINDOW::event_process_(event_ptr);

        if (child_count == 0)
        {
            children_position_();
        }

        scrollbar_reset_();
        break;

    case GX_EVENT_FOCUS_NEXT:
        if (selected < total - 1)
        {
            selected_set_(selected + 1);
        }
        break;

    case GX_EVENT_FOCUS_PREVIOUS:
        if (selected > 0)
        {
            selected_set_(selected - 1);
        }
        break;

    case GX_EVENT_PEN_DOWN:
        _gx_system_input_capture(this);

        move_start = event_ptr->payload.pointdata;
        pen_index = pen_index_get_(event_ptr->payload.pointdata);
        break;

    case GX_EVENT_PEN_UP:
        if (this->status & GX_STATUS_OWNS_INPUT)
        {
            _gx_system_input_release(this);

            if (extent_() > client_extent_())
            {
                slide_back_check_();
            }

            if (pen_index >= 0 && snap_back_distance == 0)
            {
                new_pen_index = pen_index_get_(event_ptr->payload.pointdata);
                if (new_pen_index == pen_index)
                {
                    selected_set_(pen_index);
                }
            }
        }
        else
        {
            parent_event_process_run_(event_ptr);
        }
        break;

    case GX_EVENT_PEN_DRAG:
        pos_diff = is_vertical_list ?
            (event_ptr->payload.pointdata.y - move_start.y) :
            (event_ptr->payload.pointdata.x - move_start.x);

        if ((this->status & GX_STATUS_OWNS_INPUT) && pos_diff != 0)
        {
            if (extent_() > client_extent_())
            {
                // Start sliding, remove other widgets from input capture stack.
                others_input_release_();

                scroll_(pos_diff);
                scrollbar_reset_();

                move_start = event_ptr->payload.pointdata;
                pen_index = -1;
            }
        }
        else
        {
            parent_event_process_run_(event_ptr);
        }
        break;

    case GX_EVENT_VERTICAL_FLICK:
    case GX_EVENT_HORIZONTAL_FLICK:
        if (extent_() > client_extent_())
        {
            snap_back_distance = (GX_VALUE)(GX_FIXED_VAL_TO_INT(event_ptr->payload.intdata[0]) * 8);
            timer_start_(GX_FLICK_TIMER, 1, 1);
        }
        pen_index = -1;
        break;

    case GX_EVENT_VERTICAL_SCROLL:
    case GX_EVENT_HORIZONTAL_SCROLL:
        scroll_(event_ptr->payload.intdata[1] - event_ptr->payload.intdata[0]);
        return 0;

    case GX_EVENT_TIMER:
        timer_id = event_ptr->payload.timer_id;
        if (timer_id == GX_FLICK_TIMER || timer_id == GX_SNAP_TIMER)
        {
            if (GX_ABS(snap_back_distance) < GX_ABS(child_max_extent) / 3)
            {
                timer_stop_(timer_id);

                scroll_(snap_back_distance);

                if (event_ptr->payload.timer_id == GX_FLICK_TIMER)
                {
                    slide_back_check_();
                }
            }
            else
            {
                INT snap_dist = snap_back_distance / 5;
                snap_back_distance = (GX_VALUE)(snap_back_distance - snap_dist);

                scroll_(snap_dist);
            }

            scrollbar_reset_();
        }
        else
        {
            result = GX_WINDOW::event_process_(event_ptr);
        }
        break;

    default:
        result = GX_WINDOW::event_process_(event_ptr);
        break;
    }

    return result;
}
