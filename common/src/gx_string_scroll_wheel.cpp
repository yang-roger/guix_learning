/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** GUIX Component                                                        */
/**                                                                       */
/**   String Scroll Wheel Management (Scroll Wheel)                       */
/**                                                                       */
/**************************************************************************/

#include "gx_string_scroll_wheel.h"

#include "gx_utility.h"
#include "gx_system.h"

static UINT gx_string_scroll_wheel_event_process_(GX_STRING_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr)
{
    return wheel->event_process_(event_ptr);
}

static UINT gx_string_scroll_wheel_text_get_(GX_STRING_SCROLL_WHEEL* wheel, INT row, GX_STRING* string)
{
    return wheel->text_get_(row, string);
}

UINT GX_STRING_SCROLL_WHEEL::create_ext_(const GX_CHAR* name, GX_WIDGET* parent,
                                         INT total_rows, const GX_STRING* string_list,
                                         ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    GX_TEXT_SCROLL_WHEEL::create_(name, GX_NULL, total_rows, style, Id, size);

    this->type = GX_TYPE_STRING_SCROLL_WHEEL;

    this->string_list = string_list;
    this->string_list_buffer_size = 0;
    this->string_id_list = GX_NULL;

    this->event_process_function = (UINT(*)(GX_WIDGET*, GX_EVENT*))gx_string_scroll_wheel_event_process_;
    this->text_get = (UINT (*)(GX_TEXT_SCROLL_WHEEL*, INT, GX_STRING*))gx_string_scroll_wheel_text_get_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_STRING_SCROLL_WHEEL::string_list_delete_()
{
    if (style & GX_STYLE_TEXT_COPY)
    {
        if (this->string_list)
        {
            if (_gx_system_memory_free)
            {
                _gx_system_memory_free((void*)this->string_list);
            }
        }
    }

    this->string_list = GX_NULL;
    this->string_list_buffer_size = 0;

#ifdef GX_DYNAMIC_BIDI_TEXT_SUPPORT
    dynamic_bidi_text_delete_();
#endif  // GX_DYNAMIC_BIDI_TEXT_SUPPORT

}

void GX_STRING_SCROLL_WHEEL::string_id_list_set_(const GX_RESOURCE_ID* string_id_list, INT id_count)
{
    string_list_delete_();

    this->string_id_list = string_id_list;

    total_rows_set_(id_count);
}

UINT GX_STRING_SCROLL_WHEEL::string_list_set_ext_(const GX_STRING* string_list, INT string_count)
{
    UINT result = GX_SUCCESS;

    if (style & GX_STYLE_TEXT_COPY)
    {
        result = _gx_system_private_string_list_copy_ext((GX_STRING**)&this->string_list, &this->string_list_buffer_size,
                                                         string_list, string_count);
    }
    else
    {
        this->string_list = string_list;
    }

#ifdef GX_DYNAMIC_BIDI_TEXT_SUPPORT
    dynamic_bidi_text_delete_();
#endif  // GX_DYNAMIC_BIDI_TEXT_SUPPORT

    if (result == GX_SUCCESS)
    {
        this->string_id_list = GX_NULL;

        total_rows_set_(string_count);
    }

    return result;
}

UINT GX_STRING_SCROLL_WHEEL::text_get_(INT row, GX_STRING* string)
{
    string->ptr = GX_NULL;
    string->length = 0;

    if ((this->string_list == GX_NULL && this->string_id_list == GX_NULL) ||
        row >= this->total_rows)
    {
        return GX_SUCCESS;
    }

    if (this->string_id_list)
    {
        string_get_ext_(this->string_id_list[row], string);
    }
    else
    {
        *string = this->string_list[row];
    }

    return GX_SUCCESS;
}

UINT GX_STRING_SCROLL_WHEEL::event_process_(GX_EVENT* event_ptr)
{
    switch (event_ptr->type)
    {
    case GX_EVENT_DELETE:
        string_list_delete_();
        break;

    default:
        // do nothing
        break;
    }

    return GX_TEXT_SCROLL_WHEEL::event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_string_scroll_wheel_create_ext                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a string scroll wheel selector widget.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    name                                  Name of widget                */
/*    parent                                Parent widget control block   */
/*    total_rows                            Total rows of the scroll wheel*/
/*    string_list                           String list for display       */
/*    callback                              Callback for retrieved row    */
/*                                            string                      */
/*    style                                 Style of widget               */
/*    Id                                    Application-defined ID of the */
/*                                            the widget                  */
/*    size                                  Widget size                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_scroll_wheel_create          Create a text scroll wheel    */
/*    _gx_widget_link                       Link a widget to parent       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_string_scroll_wheel_create_ext(GX_STRING_SCROLL_WHEEL* wheel, const GX_CHAR* name,
                                        GX_WIDGET* parent, INT total_rows,
                                        const GX_STRING* string_list,
                                        ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    return wheel->create_ext_(name, parent, total_rows, string_list, style, Id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_string_scroll_wheel_event_process                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes the comming events for a string scroll      */
/*    wheel widget.                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Text scroll wheel control     */
/*                                            block                       */
/*    event_ptr                             Event to be processed         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_free                Memory free function          */
/*    _gx_scroll_wheel_event_process        Default event process         */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_string_scroll_wheel_event_process(GX_STRING_SCROLL_WHEEL* wheel, GX_EVENT* event_ptr)
{
    return wheel->event_process_(event_ptr);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_string_scroll_wheel_string_id_list_set                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns a string id list for the string scroll wheel. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    string_id_list                        String id list to be set      */
/*    id_count                              The number of string ids      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_memory_free                Application defined memory    */
/*                                            free function               */
/*    _gx_scroll_wheel_total_rows_set       Set scroll wheel total rows   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_string_scroll_wheel_string_id_list_set(GX_STRING_SCROLL_WHEEL* wheel,
                                                const GX_RESOURCE_ID* string_id_list,
                                                INT id_count)
{
    wheel->string_id_list_set_(string_id_list, id_count);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_string_scroll_wheel_string_list_set_ext                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function assigns a string list for the string scroll wheel.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    string_list                           String list to be set         */
/*    string_count                          The number of assigned strings*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_system_private_string_list_copy   Makde a private copy of string*/
/*                                            list                        */
/*    _gx_scroll_wheel_total_rows_set       Set scroll wheel total rows   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_string_scroll_wheel_string_list_set_ext(GX_STRING_SCROLL_WHEEL* wheel,
                                                 const GX_STRING* string_list,
                                                 INT string_count)
{
    return wheel->string_list_set_ext_(string_list, string_count);
}
