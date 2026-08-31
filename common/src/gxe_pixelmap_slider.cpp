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
/**   Slider Management (Slider)                                          */
/**                                                                       */
/**************************************************************************/

#include "gx_pixelmap_slider.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_pixelmap_slider_create                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for error in pixelmap slider create call.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider control block          */
/*    name                                  Name of prompt                */
/*    parent                                Parent widget control block   */
/*    info                                  Pointer to a GX_SLIDER_INFO   */
/*    pixelmap_info                         Pointer to the pixelmap       */
/*                                            info block                  */
/*    style                                 Style of the slider           */
/*    pixelmap_slider_id                    Application-defined ID of     */
/*                                            pixelmap slider             */
/*    size                                  Dimensions of pixelmap prompt */
/*    pixelmap_slider_control_block_size    Size of the pixelmap slider   */
/*                                            control block               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_pixelmap_slider_create            The actual function           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_pixelmap_slider_create(GX_PIXELMAP_SLIDER *slider,
                                  const GX_CHAR *name,
                                  GX_WIDGET *parent,
                                  GX_SLIDER_INFO *info,
                                  GX_PIXELMAP_SLIDER_INFO *pixelmap_info,
                                  ULONG style,
                                  USHORT pixelmap_slider_id,
                                  const GX_RECTANGLE *size,
                                  UINT pixelmap_slider_control_block_size)
{

UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer. */
    if ((slider == GX_NULL) || (size == GX_NULL) ||
        (info == GX_NULL) || (pixelmap_info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget control block size. */
    if (pixelmap_slider_control_block_size != sizeof(GX_PIXELMAP_SLIDER))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (slider->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    status = _gx_pixelmap_slider_create(slider, name, parent, info, pixelmap_info, style, pixelmap_slider_id, size);

    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_pixelmap_slider_event_process                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in the pixelmap slider event process    */
/*    function.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider's widget control block */
/*    event_ptr                             Incoming event to process     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_widget_event_process              Default widget event process  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_pixelmap_slider_event_process(GX_PIXELMAP_SLIDER *slider, GX_EVENT *event_ptr)
{

UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check error for valid pointer.  */
    if ((slider == GX_NULL) || (event_ptr == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget. */
    if (slider->type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    status = _gx_pixelmap_slider_event_process(slider, event_ptr);

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_pixelmap_slider_pixelmap_set                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the pixelmap slider pixelmap     */
/*    set function call.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider control block          */
/*    pixinfo                               Slider infomration block      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_pixelmap_slidre_pixelmap_set      Actual pixelmap slider        */
/*                                            pixelmap set function       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_pixelmap_slider_pixelmap_set(GX_PIXELMAP_SLIDER *slider,
                                        GX_PIXELMAP_SLIDER_INFO *pixinfo)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    if ((!slider) || (!pixinfo))
    {
        return(GX_PTR_ERROR);
    }

    status = _gx_pixelmap_slider_pixelmap_set(slider, pixinfo);

    return(status);
}
