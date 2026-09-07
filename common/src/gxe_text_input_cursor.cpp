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
/**   Text Input Cursor Managment (Text Input Cursor)                     */
/**                                                                       */
/**************************************************************************/

#include "gx_text_input_cursor.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_text_input_cursor_blink_interval_set                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the text input cursor blink      */
/*    interval set function call.                                         */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    cursor_ptr                            Pointer to text input cursor  */
/*                                            control block               */
/*    blink_interval                        Cursor blink interval value   */
/*                                            to be set                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_input_cursor_blink_interval_set                            */
/*                                          Actual text input cursor      */
/*                                            blink interval set function */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_text_input_cursor_blink_interval_set(GX_TEXT_INPUT_CURSOR *cursor_ptr, GX_UBYTE blink_interval)
{
UINT status;

    if (!cursor_ptr)
    {
        return(GX_PTR_ERROR);
    }

    if (blink_interval == 0)
    {
        return(GX_INVALID_VALUE);
    }

    status = _gx_text_input_cursor_blink_interval_set(cursor_ptr, blink_interval);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_text_input_cursor_height_set                                   */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the text input cursor height     */
/*     set function call.                                                 */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    cursor_ptr                            Pointer to text input cursor  */
/*                                            control block               */
/*    height                                Cursor height value to be set */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_input_cursor_height_set                                    */
/*                                          Actual text input cursor      */
/*                                            height set function         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_text_input_cursor_height_set(GX_TEXT_INPUT_CURSOR *cursor_ptr, GX_UBYTE height)
{
UINT status;

    if (!cursor_ptr)
    {
        return(GX_PTR_ERROR);
    }

    if (height == 0)
    {
        return(GX_INVALID_VALUE);
    }

    status = _gx_text_input_cursor_height_set(cursor_ptr, height);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_text_input_cursor_width_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the text input cursor width      */
/*     set function call.                                                 */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    cursor_ptr                            Pointer to text input cursor  */
/*                                            control block               */
/*    width                                 Cursor width value to be set  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_text_input_cursor_width_set                                     */
/*                                          Actual text input cursor      */
/*                                            width set function          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_text_input_cursor_width_set(GX_TEXT_INPUT_CURSOR *cursor_ptr, GX_UBYTE width)
{
UINT status;

    if (!cursor_ptr)
    {
        return(GX_PTR_ERROR);
    }

    if (width == 0)
    {
        return(GX_INVALID_VALUE);
    }

    status = _gx_text_input_cursor_width_set(cursor_ptr, width);

    return(status);
}
