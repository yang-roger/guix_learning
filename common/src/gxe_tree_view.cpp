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
/**   Tree View Management (Tree View)                                    */
/**                                                                       */
/**************************************************************************/

#include "gx_tree_view.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_tree_view_create                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in tree view create function call.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to the tree view      */
/*                                            control block               */
/*    name                                  Name of the tree view         */
/*    parent                                Parent widget control block   */
/*    style                                 Style of the widget           */
/*    tree_id                               Application-defined ID of     */
/*                                          the tree view                 */
/*    size                                  Tree view size                */
/*    control_block_size                    Control block size            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_tree_view_create                  Actual tree view create       */
/*                                            function call               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_tree_view_create(GX_TREE_VIEW *tree, const GX_CHAR *name, GX_WIDGET *parent,
                            ULONG style, USHORT tree_id, const GX_RECTANGLE *size, UINT control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((!tree) || (!size))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for widget already created.  */
    if (tree->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid control block size. */
    if (control_block_size != sizeof(GX_TREE_VIEW))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for invalid parent widget. */
    if (parent && (parent->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual tree view create function.  */
    status = _gx_tree_view_create(tree, name, parent, style, tree_id, size);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_tree_view_event_process                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in tree view event process function */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to tree view control  */
/*                                            block                       */
/*    event_ptr                             Pointer to event to process   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gxe_tree_view_event_process          Actual tree view event process*/
/*                                            function call               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_tree_view_event_process(GX_TREE_VIEW *tree, GX_EVENT *event_ptr)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((!tree) || (!event_ptr))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (tree->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual tree view event process function.  */
    status = _gx_tree_view_event_process(tree, event_ptr);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_tree_view_indentation_set                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in tree view indentation set        */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to the tree view      */
/*                                            control block               */
/*    indentation                           Indentation to set            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_tree_view_indentation_set         Actual tree view indentation  */
/*                                            set function call           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_tree_view_indentation_set(GX_TREE_VIEW *tree, GX_VALUE indentation)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!tree)
    {
        return(GX_PTR_ERROR);
    }

    /* Check fo rinvalid widget. */
    if (tree->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual tree view indentation set function.  */
    status = _gx_tree_view_indentation_set(tree, indentation);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_tree_view_position                                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in tree view position function call.*/
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to tree view control  */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gxe_tree_view_position               Actual tree view position     */
/*                                            function call               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gxe_tree_view_position(GX_TREE_VIEW *tree)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!tree)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (tree->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual tree view position function.  */
    status = _gx_tree_view_position(tree);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_tree_view_root_line_color_set                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in tree view root line color set    */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to the tree view      */
/*                                            control block               */
/*    color                                 Root line color to set        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gxe_tree_view_root_line_color_set    Actual tree view root color   */
/*                                            set function call           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_tree_view_root_line_color_set(GX_TREE_VIEW *tree, GX_RESOURCE_ID color)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!tree)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (tree->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual tree view root pixelmap set function.  */
    status = _gx_tree_view_root_line_color_set(tree, color);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_tree_view_root_pixelmap_set                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in tree view root pixelmap set      */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to the tree view      */
/*                                            control block               */
/*    expand_map_id                         Resource id of expand         */
/*                                            pixelmap                    */
/*    collapse_map_id                       Reource id of collapse        */
/*                                            pixlemap                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gxe_tree_view_root_pixelmap_set      Actual tree view root         */
/*                                            pixelmap set function call  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_tree_view_root_pixelmap_set(GX_TREE_VIEW *tree, GX_RESOURCE_ID expand_map_id, GX_RESOURCE_ID collapse_map_id)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (!tree)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (tree->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual tree view root pixelmap set function.  */
    status = _gx_tree_view_root_pixelmap_set(tree, expand_map_id, collapse_map_id);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_tree_view_selected_get                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in tree view selected get function  */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to the tree view      */
/*                                            control block               */
/*    selected                              Selected widget to return     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_tree_view_selected_get            Actual tree view selected get */
/*                                            function call               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_tree_view_selected_get(GX_TREE_VIEW *tree, GX_WIDGET **selected)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((!tree) || (!selected))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (tree->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual tree view seleced get function.  */
    status = _gx_tree_view_selected_get(tree, selected);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_tree_view_selected_set                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in tree view selected set function  */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    tree                                  Pointer to the tree view      */
/*                                            control block               */
/*    selected                              New selected item             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_tree_view_selected_set            Actual tree view selected set */
/*                                            function call               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_tree_view_selected_set(GX_TREE_VIEW *tree, GX_WIDGET *selected)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((!tree) || (!selected))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (tree->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call actual tree view seleced set function.  */
    status = _gx_tree_view_selected_set(tree, selected);

    return(status);
}
