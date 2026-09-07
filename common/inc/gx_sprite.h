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
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_sprite.h                                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX sprite component, including all data     */
/*    types and external references.  It is assumed that gx_api.h and     */
/*    gx_port.h have already been included.                               */
/*                                                                        */
/**************************************************************************/

#ifndef GX_SPRITE_H
#define GX_SPRITE_H

#include "gx_widget.h"

struct GX_SPRITE_FRAME
{
    GX_RESOURCE_ID pixelmap;
    GX_VALUE       x_offset;
    GX_VALUE       y_offset;
    UINT           delay;
    UINT           background_operation;
    UCHAR          alpha;
};

/* Define Sprite type. */
struct GX_SPRITE : public GX_WIDGET
{
    GX_SPRITE_FRAME* frame_list;
    USHORT           frame_count;
    USHORT           current_frame;
    UCHAR            run_state;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_SPRITE_FRAME* frame_list, USHORT frame_count,
                 ULONG style, USHORT sprite_id, const GX_RECTANGLE* size);

    void frame_list_set_(GX_SPRITE_FRAME* frame_list, USHORT frame_count);
    void current_frame_set_(USHORT frame);

    UINT start_(USHORT start_frame);
    void stop_();

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void update_();

    UINT current_frame_start_();
    void current_frame_draw_();
};

#define GX_SPRITE_IDLE    0x0001
#define GX_SPRITE_RUNNING 0x0002


/* Define sprite management function prototypes.  */

UINT _gx_sprite_create(GX_SPRITE* sprite, const GX_CHAR* name, GX_WIDGET* parent,
                       GX_SPRITE_FRAME* frame_list, USHORT frame_count,
                       ULONG style, USHORT sprite_id, const GX_RECTANGLE* size);
UINT _gx_sprite_current_frame_set(GX_SPRITE* sprite, USHORT frame);
UINT _gx_sprite_frame_list_set(GX_SPRITE* sprite, GX_SPRITE_FRAME* frame_list, USHORT frame_count);
UINT _gx_sprite_start(GX_SPRITE* sprite, USHORT start_frame);
UINT _gx_sprite_stop(GX_SPRITE* sprite);


/* error checking versions of the animation API functions */

UINT _gxe_sprite_create(GX_SPRITE* sprite, const GX_CHAR* name, GX_WIDGET* parent,
                        GX_SPRITE_FRAME* frame_list, USHORT frame_count,
                        ULONG style, USHORT sprite_id, const GX_RECTANGLE* size, UINT sprite_control_block_size);
UINT _gxe_sprite_current_frame_set(GX_SPRITE* sprite, USHORT frame);
UINT _gxe_sprite_frame_list_set(GX_SPRITE* sprite, GX_SPRITE_FRAME* frame_list, USHORT frame_count);
UINT _gxe_sprite_start(GX_SPRITE* sprite, USHORT frame);
UINT _gxe_sprite_stop(GX_SPRITE* sprite);

#endif
