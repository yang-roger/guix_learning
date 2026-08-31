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
/**   Icon Management (Icon)                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_icon.h                                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX icon management component,               */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/**************************************************************************/

#ifndef GX_ICON_H
#define GX_ICON_H

#include "gx_widget.h"

/* Define Icon type.  */
struct GX_ICON : public GX_WIDGET
{
    GX_RESOURCE_ID normal_pixelmap;
    GX_RESOURCE_ID selected_pixelmap;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 GX_RESOURCE_ID pixelmap_id,
                 ULONG style, USHORT icon_id, GX_VALUE x, GX_VALUE y);

    UINT pixelmap_set_(GX_RESOURCE_ID normal_id, GX_RESOURCE_ID selected_id);

    void background_draw_();

    void draw_();
    UINT event_process_(GX_EVENT* event_ptr);

private:
    void resize_to_pixelmap_();
};


/* Define button management function prototypes.  */

void _gx_icon_background_draw(GX_ICON* icon);
UINT _gx_icon_create(GX_ICON* icon, const GX_CHAR* name, GX_WIDGET* parent,
                     GX_RESOURCE_ID pixelmap_id, ULONG style, USHORT icon_id,
                     GX_VALUE x, GX_VALUE y);
void _gx_icon_draw(GX_ICON* icon);
UINT _gx_icon_event_process(GX_ICON* icon, GX_EVENT* event_ptr);
UINT _gx_icon_pixelmap_set(GX_ICON* icon, GX_RESOURCE_ID normal_id, GX_RESOURCE_ID selected_id);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_icon_create(GX_ICON* icon, const GX_CHAR* name, GX_WIDGET* parent,
                      GX_RESOURCE_ID pixelmap_id, ULONG style, USHORT icon_id,
                      GX_VALUE x, GX_VALUE y,
                      UINT icon_control_block_size);
UINT _gxe_icon_event_process(GX_ICON* button, GX_EVENT* event_ptr);
UINT _gxe_icon_pixelmap_set(GX_ICON* icon, GX_RESOURCE_ID normal_id, GX_RESOURCE_ID selected_id);

#endif

