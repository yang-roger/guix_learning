
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
/**   Dispaly Management (Dispaly)                                        */
/**                                                                       */
/**************************************************************************/

#include "gx_display.h"

#if defined(GX_MOUSE_SUPPORT)
#if !defined(GX_HARDWARE_MOUSE_SUPPORT)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_generic_mouse_enable                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This service sets the mouse status for software mouse support.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    display                               Display control block         */
/*    enable                                Mouse status                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
void _gx_display_driver_generic_mouse_enable(GX_DISPLAY *display, GX_BOOL enable)
{
    if (enable)
    {
        if (!(display->mouse.status & GX_MOUSE_ENABLED))
        {
            display->mouse.status |= GX_MOUSE_ENABLED;

            /* reset to current position to force redraw */
            display->mouse_position_set(display, &display->mouse.position);
        }
    }
    else
    {
        if (display->mouse.status & GX_MOUSE_ENABLED)
        {
            display->mouse.status &= (GX_UBYTE)(~GX_MOUSE_ENABLED);

            if (display->mouse.status & GX_MOUSE_VISIBLE)
            {
                display->mouse_restore(display);
            }
        }
    }
}
#endif
#endif

