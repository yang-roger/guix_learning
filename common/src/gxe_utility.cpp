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
/**   Utility (Utility)                                                   */
/**                                                                       */
/**************************************************************************/

#include "gx_utility.h"

#include "gx_pixelmap.h"
#include "gx_canvas.h"
#include "gx_system.h"


GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_bidi_resolved_text_info_delete                         */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility bidi paragraph       */
/*    reorder function call.                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input_info                            Pointer to bidi text info     */
/*    reordered_text                        Reordered text information    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                          Actual utility bidi paragraph */
/*                                            reorder function            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT _gxe_utility_bidi_paragraph_reorder(GX_BIDI_TEXT_INFO *input_info, GX_BIDI_RESOLVED_TEXT_INFO **resolved_info_head)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((input_info == GX_NULL) || (resolved_info_head == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual utility bidi paragraph reorder function.  */
    status = _gx_utility_bidi_paragraph_reorder(input_info, resolved_info_head);

    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_bidi_paragraph_reorder_ext                             */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Ting Zhu, Microsoft Corporation                                     */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility bidi paragraph       */
/*    reorder ext function call.                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input_info                            Pointer to bidi text info     */
/*    reordered_text                        Reordered text information    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder_ext                              */
/*                                          Actual utility bidi paragraph */
/*                                            reorder ext function        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT _gxe_utility_bidi_paragraph_reorder_ext(GX_BIDI_TEXT_INFO *input_info, GX_BIDI_RESOLVED_TEXT_INFO **resolved_info_head)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((input_info == GX_NULL) || (resolved_info_head == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual utility bidi paragraph reorder function.  */
    status = _gx_utility_bidi_paragraph_reorder_ext(input_info, resolved_info_head);

    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_bidi_resolved_text_info_delete                         */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility bidi resolved text   */
/*    info delete function call.                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    reordered_text                        The head pointer of reordered */
/*                                            bidi text information link  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_bidi_resolved_text_info_delete                          */
/*                                          Actual utility bidi resolved  */
/*                                            text into delete function   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT _gxe_utility_bidi_resolved_text_info_delete(GX_BIDI_RESOLVED_TEXT_INFO **resolved_info_head)
{
UINT status;

    /* Check for invalid input pointers.  */
    if (resolved_info_head == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual utility pixelmap resize function.  */
    status = _gx_utility_bidi_resolved_text_info_delete(resolved_info_head);

    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_canvas_to_bmp                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Covert canvas memory data to bmp file.                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    canvas                                Canvas control block          */
/*    rectangle                             Rectangle specification       */
/*    write_data                            Write data callback function  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_canvas_to_bmp             Actual function which do this */
/*                                            convert.                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/**************************************************************************/
UINT _gxe_utility_canvas_to_bmp(GX_CANVAS *canvas, GX_RECTANGLE *rect, UINT (*write_data)(GX_UBYTE *byte_data, UINT data_count))
{

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if ((canvas == GX_NULL) ||
        (canvas->display == GX_NULL) ||
        (canvas->memory == GX_NULL) ||
        (rect == GX_NULL) ||
        (write_data == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    return _gx_utility_canvas_to_bmp(canvas, rect, write_data);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_circle_point_get                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility circle point get     */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    xcenter                               x-coord of center of circle   */
/*    ycenter                               y-coord of center of circle   */
/*    r                                     Radius of circle              */
/*    angle                                 The angle where the point is  */
/*    point                                 Return value of the point at  */
/*                                            the angle                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_circle_point_get                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gxe_utility_circle_point_get(INT xcenter, INT ycenter, UINT r, INT angle, GX_POINT *point)
{
UINT status;

    /* Check for invalid input pointers.  */
    if (point == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (r == 0)
    {
        return GX_INVALID_VALUE;
    }

    /* Call the actual utility circle point get.  */
    status = _gx_utility_circle_point_get(xcenter, ycenter, r, angle, point);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_ltoa                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility ltoa function call.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    value                                 Long integer value to convert */
/*    return_buffer                         Destination buffer for ASCII  */
/*                                            string                      */
/*    return_buffer_size                    Size of destination buffer    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_ltoa                                                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_utility_ltoa(LONG value, GX_CHAR *return_buffer, UINT return_buffer_size)
{
    /* Check for invalid input pointers.  */
    if (return_buffer == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    if (return_buffer_size == 0)
    {
        return GX_INVALID_SIZE;
    }

    /* Call actual utility ltoa function.  */
    return(_gx_utility_ltoa(value, return_buffer, return_buffer_size));
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_pixelmap_resize                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility pixelmap resize      */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The source pixelmap           */
/*    destination                           The resized pixelmap to be    */
/*                                            returned                    */
/*    width                                 New width                     */
/*    height                                New height                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_pixelmap_resize           Actual utility pixelmap       */
/*                                            resize function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_utility_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((src == GX_NULL) || (destination == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for valid value.  */
    if ((width <= 0) && (height <= 0))
    {
        return GX_INVALID_VALUE;
    }

    /* Check for pixelmap flags.  */
    if (src->flags & GX_PIXELMAP_COMPRESSED)
    {
        return GX_NOT_SUPPORTED;
    }

    if (!_gx_system_memory_allocator || !_gx_system_memory_free)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    /* Call the actual utility pixelmap resize function.  */
    status = _gx_utility_pixelmap_resize(src, destination, width, height);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_pixelmap_rotation                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility pixelmap rotate call.*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The pixelmap to be rotated    */
/*    angle                                 The angle to be rotated       */
/*    destination                           The rotated bitmap to be      */
/*                                            returned                    */
/*    rot_cx                                X coordinate of rotation      */
/*                                            center                      */
/*    rot_cy                                Y coordinate of rotation      */
/*                                            center                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_pixelmap_rotation                                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gxe_utility_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((src == GX_NULL) || (destination == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual utility rectangle center function.  */
    status = _gx_utility_pixelmap_rotate(src, angle, destination, rot_cx, rot_cy);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_pixelmap_simple_rotate                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility pixelmap rotate call.*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    src                                   The pixelmap to be rotated    */
/*    angle                                 The angle to be rotated       */
/*    destination                           The rotated bitmap to be      */
/*                                            returned                    */
/*    rot_cx                                X coordinate of rotation      */
/*                                            center                      */
/*    rot_cy                                Y coordinate of rotation      */
/*                                            center                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_pixelmap_simple_rotate                                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/**************************************************************************/
UINT _gxe_utility_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((src == GX_NULL) || (destination == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual utility pixelmap simple rotate function.  */
    status = _gx_utility_pixelmap_simple_rotate(src, angle, destination, rot_cx, rot_cy);

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_rectangle_center                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility rectangle center     */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    rectangle                             Rectangle to center           */
/*    within                                Rectangle to center within    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_center          Actual utility rectangle      */
/*                                            center function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_utility_rectangle_center(GX_RECTANGLE *rectangle, GX_RECTANGLE *within)
{
    /* Check for invalid input pointers.  */
    if ((rectangle == GX_NULL) || (within == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for valid rectangle.  */
    if ((rectangle->left > rectangle->right) ||
        (rectangle->top > rectangle->bottom) ||
        (within->left > within->right) ||
        (within->top > within->bottom))
    {
        return GX_INVALID_SIZE;
    }

    /* Call the actual utility rectangle center function.  */
    return(_gx_utility_rectangle_center(rectangle, within));
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_rectangle_center_find                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility rectangle center     */
/*    find function call.                                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    rectangle                             Rectangle                     */
/*    return_center                         Pointer to destination to     */
/*                                            store found center point    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_center_find     Actual utility rectangle      */
/*                                          center find function          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_utility_rectangle_center_find(GX_RECTANGLE *rectangle,
                                         GX_POINT *return_center)
{
    /* Check for invalid input pointers.  */
    if ((rectangle == GX_NULL) || (return_center == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Check for valid rectangle.  */
    if ((rectangle->left > rectangle->right) ||
        (rectangle->top > rectangle->bottom))
    {
        return GX_INVALID_SIZE;
    }

    /* Call the actual utility rectangle center find function.  */
    return(_gx_utility_rectangle_center_find(rectangle, return_center));
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_rectangle_combine                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility rectangle combine    */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    first_rectangle                             First rectangle and     */
/*                                                combined rectangle      */
/*    second_rectangle                            Second rectangle        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_combine         Actual utility rectangle      */
/*                                          combine function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_utility_rectangle_combine(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle)
{
    /* Check for invalid input pointers.  */
    if ((first_rectangle == GX_NULL) || (second_rectangle == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual utility rectangle combine function.  */
    return(_gx_utility_rectangle_combine(first_rectangle, second_rectangle));
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_rectangle_compare                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility rectangle compare    */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    first_rectangle                       First rectangle               */
/*    second_rectangle                      Second rectangle              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    [GX_TRUE | GX_FALSE]                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_compare         Actual utility rectangle      */
/*                                          compare function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
GX_BOOL  _gxe_utility_rectangle_compare(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle)
{
    /* Check for invalid input pointers.  */
    if ((first_rectangle == GX_NULL) || (second_rectangle == GX_NULL))
    {
        return GX_FALSE;
    }

    /* Check for valid rectangle.  */
    if ((first_rectangle->left > first_rectangle->right) ||
        (first_rectangle->top > first_rectangle->bottom) ||
        (second_rectangle->left > second_rectangle->right) ||
        (second_rectangle->top > second_rectangle->bottom))
    {
        return GX_FALSE;
    }

    /* Call the actual utility rectangle compare function.  */
    return(_gx_utility_rectangle_compare(first_rectangle, second_rectangle));
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_rectangle_define                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility rectangle define     */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    rectangle                             Rectangle to define           */
/*    left                                  Left most coordinate          */
/*    top                                   Top most coordinate           */
/*    right                                 Right most coordinate         */
/*    bottom                                Bottom most coordinate        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_define          Actual utility rectangle      */
/*                                            define function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_utility_rectangle_define(GX_RECTANGLE *rectangle, GX_VALUE left,
                                   GX_VALUE top, GX_VALUE right, GX_VALUE bottom)
{

    /* Check for invalid input pointers.  */
    if (rectangle == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual utility rectangle define function.  */
    return (_gx_utility_rectangle_define(rectangle, left, top, right, bottom));
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_rectangle_overlap_detect                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility rectangle overlap    */
/*    detect function call.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    first_rectangle                       First rectangle               */
/*    second_rectangle                      Second rectangle              */
/*    return_overlap_area                   Overlapping rectangle area    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    [GX_TRUE | GX_FALSE]                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_overlap_detect  Actual utility rectangle      */
/*                                          overlap detect function       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
GX_BOOL  _gxe_utility_rectangle_overlap_detect(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle,
                                               GX_RECTANGLE *return_overlap_area)
{
GX_BOOL status;

    /* Check for invalid input pointers.  */
    if ((first_rectangle == GX_NULL) || (second_rectangle == GX_NULL))
    {
        return GX_FALSE;
    }

    /* Check for valid rectangle.  */
    if ((first_rectangle->left > first_rectangle->right) ||
        (first_rectangle->top > first_rectangle->bottom) ||
        (second_rectangle->left > second_rectangle->right) ||
        (second_rectangle->top > second_rectangle->bottom))
    {
        return GX_FALSE;
    }

    /* Call the actual utility rectangle overlap detect function.  */
    status = _gx_utility_rectangle_overlap_detect(first_rectangle, second_rectangle, return_overlap_area);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_rectangle_point_detect                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility rectangle point      */
/*    detect function call.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    rectangle                             Rectangle                     */
/*    point                                 Point                         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    [GX_TRUE | GX_FALSE]                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_point_detect    Actual utility rectangle      */
/*                                          point detect function         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
GX_BOOL  _gxe_utility_rectangle_point_detect(GX_RECTANGLE *rectangle, GX_POINT point)
{
GX_BOOL status;

    /* Check for invalid input pointers.  */
    if (rectangle == GX_NULL)
    {
        return(GX_FALSE);
    }

    /* Check for valid rectangle.  */
    if ((rectangle->left > rectangle->right) ||
        (rectangle->top > rectangle->bottom))
    {
        return(GX_FALSE);
    }

    /* Call the actual utility rectangle point detect function.  */
    status = _gx_utility_rectangle_point_detect(rectangle, point);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_rectangle_resize                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility rectangle resize     */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    rectangle                             Rectangle                     */
/*    adjust                                Amount to adjust              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_resize          Actual utility rectangle      */
/*                                            resize function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_utility_rectangle_resize(GX_RECTANGLE *rectangle, GX_VALUE adjust)
{
    /* Check for invalid input pointers.  */
    if (rectangle == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual utility rectangle resize function.  */
    return(_gx_utility_rectangle_resize(rectangle, adjust));
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_rectangle_shift                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the utility rectangle shift      */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    rectangle                             Rectangle to shift            */
/*    x_shift                               Number of pixels to shift     */
/*                                            on the x-axis               */
/*    y_shift                               Number of pixels to shift     */
/*                                            on the y-axis               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_shift           Actual utility rectangle      */
/*                                            shift function              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_utility_rectangle_shift(GX_RECTANGLE *rectangle, GX_VALUE x_shift, GX_VALUE y_shift)
{
    /* Check for invalid input pointers.  */
    if (rectangle == GX_NULL)
    {
        return GX_PTR_ERROR;
    }

    /* Call the actual utility rectangle shift function.  */
    return(_gx_utility_rectangle_shift(rectangle, x_shift, y_shift));
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_utility_string_to_alphamap_ext                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks error in utility string to alphamap function.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text                                  pointer to string             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_string_to_alphamap_ext    The actual utility string to  */
/*                                            alphamap ext function       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT    _gxe_utility_string_to_alphamap_ext(const GX_STRING *text, const GX_FONT *font, GX_PIXELMAP *textmap)
{
UINT status;
UINT text_length = 0;

    if ((text == GX_NULL) ||
        (text->ptr == GX_NULL) ||
        (font == GX_NULL) ||
        (textmap == GX_NULL))
    {
        return GX_PTR_ERROR;
    }

    if (!_gx_system_memory_allocator || !_gx_system_memory_free)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    status = _gx_utility_string_length_check(text->ptr, &text_length, text->length);

    if (status != GX_SUCCESS)
    {
        return status;
    }

    if (text_length != text->length)
    {
        return GX_INVALID_STRING_LENGTH;
    }

    status = _gx_utility_string_to_alphamap_ext(text, font, textmap);

    /* Return completion status code. */
    return(status);
}
