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
/**   Sprite Management (Sprite)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_sprite.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_sprite_create                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for error in the sprite create call.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    sprite                                Pointer to sprite widget      */
/*                                            control block               */
/*    name                                  Logical name of sprite        */
/*    parent                                Pointer to parent widget      */
/*                                            of sprite                   */
/*    frame_list                            The frame_list to be assigned */
/*    frame_count                           Number of frames in the frame */
/*    style                                 Sprite stuyle                 */
/*    sprite_id                             Application-defined ID of     */
/*                                             the sprite                 */
/*    size                                  Size of the sprite            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_sprite_create                     Actual sprite create call     */
/*    _gx_widget_link                       Link the sprite to its parent */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_sprite_create(GX_SPRITE *sprite, const GX_CHAR *name, GX_WIDGET *parent,
                         GX_SPRITE_FRAME *frame_list, USHORT frame_count,
                         ULONG style, USHORT sprite_id, const GX_RECTANGLE *size, UINT sprite_control_block_size)
{

UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((sprite == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget control block size.  */
    if (sprite_control_block_size != sizeof(GX_SPRITE))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (sprite->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Check for invalid parent widget. */
    if (parent && (parent->type == 0))
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual sprite create function.  */
    status = _gx_sprite_create(sprite, name, parent, frame_list, frame_count, style, sprite_id, size);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_sprite_current_frame_set                                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks for error in the sprite current frame set call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    sprite                                Pointer to sprite widget      */
/*                                            control block               */
/*    frame_index                           The frame index to display    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_sprite_current_frame_set          Actual sprite current frame   */
/*                                            set call.                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_sprite_current_frame_set(GX_SPRITE *sprite, USHORT frame_index)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if (sprite == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (sprite->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual sprite frame set function. */
    status = _gx_sprite_current_frame_set(sprite, frame_index);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_sprite_frame_list_set                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks error in the sprite frame_list set call.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    sprite                                Pointer to sprite widget      */
/*                                            control block               */
/*    frame_list                            The frame list to be assigned */
/*    frame_count                           Number of frames in the frame */
/*                                            list                        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_sprite_frame_list_set             Actual sprite frame list set  */
/*                                            call                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_sprite_frame_list_set(GX_SPRITE *sprite, GX_SPRITE_FRAME *frame_list, USHORT frame_count)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer. */
    if (sprite == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (sprite->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual sprite frame list set function. */
    status = _gx_sprite_frame_list_set(sprite, frame_list, frame_count);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_sprite_start                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks error in the sprite start call.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    sprite                                Pointer to sprite widget      */
/*                                           control block                */
/*    frame_number                          The frame number to start     */
/*                                            with                        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_sprite_start                      Actual sprite start call      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_sprite_start(GX_SPRITE *sprite, USHORT frame_number)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_THREADS_ONLY_CALLER_CHECKING

    /* Check for invalid pointer. */
    if (sprite == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (sprite->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual sprite start function.  */
    status = _gx_sprite_start(sprite, frame_number);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_sprite_stop                                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service checks for error in the sprite stop call.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    sprite                                Pointer to sprite widget      */
/*                                           control block                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_sprite_stop                       The actual sprite stop call   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_sprite_stop(GX_SPRITE *sprite)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_THREADS_ONLY_CALLER_CHECKING

    /* Check for the invalid input pointers.  */
    if (sprite == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    if (sprite->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Call the actual sprite stop function.  */
    status = _gx_sprite_stop(sprite);

    /* Return completion status.  */
    return(status);
}
