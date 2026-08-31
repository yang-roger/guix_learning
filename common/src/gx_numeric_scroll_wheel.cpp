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
/**   Numeric Scroll Wheel Management (Scroll Wheel)                      */
/**                                                                       */
/**************************************************************************/

#include "gx_numeric_scroll_wheel.h"

#include "gx_utility.h"

static inline INT gx_total_rows_(INT start_val, INT end_val)
{
    return GX_ABS(end_val - start_val) + 1;
}

static UINT gx_numeric_scroll_wheel_text_get_(GX_NUMERIC_SCROLL_WHEEL* wheel, INT row, GX_STRING* string)
{
    return wheel->text_get_(row, string);
}

UINT GX_NUMERIC_SCROLL_WHEEL::create_(const GX_CHAR* name, GX_WIDGET* parent,
                                      INT start_val, INT end_val,
                                      ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    GX_TEXT_SCROLL_WHEEL::create_(name, GX_NULL, gx_total_rows_(start_val, end_val), style, Id, size);

    this->type = GX_TYPE_NUMERIC_SCROLL_WHEEL;

    this->start_val = start_val;
    this->end_val = end_val;

    this->text_get = (UINT (*)(GX_TEXT_SCROLL_WHEEL*, INT, GX_STRING*))gx_numeric_scroll_wheel_text_get_;

    if (parent)
    {
        link_to_(parent);
    }

    return GX_SUCCESS;
}

void GX_NUMERIC_SCROLL_WHEEL::range_set_(INT start_val, INT end_val)
{
#ifdef GX_DYNAMIC_BIDI_TEXT_SUPPORT
    dynamic_bidi_text_delete_();
#endif  // GX_DYNAMIC_BIDI_TEXT_SUPPORT

    this->start_val = start_val;
    this->end_val = end_val;

    total_rows_set_(gx_total_rows_(start_val, end_val));
}

UINT GX_NUMERIC_SCROLL_WHEEL::text_get_(INT row, GX_STRING* string)
{
    if (row < total_rows)
    {
        INT step = end_val - start_val;
        if (step == (total_rows - 1))
        {
            step = 1;
        }
        else if (step == (1 - total_rows))
        {
            step = -1;
        }
        else if (total_rows > 1)
        {
            step /= (total_rows - 1);
        }

        INT val = start_val + step * row;
        _gx_utility_ltoa(val, string_buffer, GX_NUMERIC_SCROLL_WHEEL_STRING_BUFFER_SIZE);

        _gx_utility_string_length_check(string_buffer, &string->length, GX_NUMERIC_SCROLL_WHEEL_STRING_BUFFER_SIZE - 1);
        string->ptr = string_buffer;
    }
    else
    {
        string->ptr = GX_NULL;
        string->length = 0;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_numeric_scroll_wheel_create                                     */
/*                                                           6.1.6        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a numeric scroll wheel selector widget.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    name                                  Name of widget                */
/*    parent                                Parent widget control block   */
/*    start_val                             Start value of numeric range  */
/*    end_val                               End value of numeric range    */
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
UINT _gx_numeric_scroll_wheel_create(GX_NUMERIC_SCROLL_WHEEL* wheel,
                                     const GX_CHAR* name, GX_WIDGET* parent,
                                     INT start_val, INT end_val,
                                     ULONG style, USHORT Id, const GX_RECTANGLE* size)
{
    return wheel->create_(name, parent, start_val, end_val, style, Id, size);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_numeric_scroll_wheel_range_set                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets range values for numeric scroll wheel widget.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    wheel                                 Scroll wheel control block    */
/*    start_val                             Start value of numeric range  */
/*    end_val                               End value of numeric range    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_scroll_wheel_total_rows_set       Reset scroll wheel total rows */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_numeric_scroll_wheel_range_set(GX_NUMERIC_SCROLL_WHEEL* wheel,
                                        INT start_val, INT end_val)
{
    wheel->range_set_(start_val, end_val);

    return GX_SUCCESS;
}
