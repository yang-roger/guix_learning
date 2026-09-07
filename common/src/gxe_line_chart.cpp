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
/**   Line Chart  (Chart)                                                 */
/**                                                                       */
/**************************************************************************/

#include "gx_line_chart.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_line_chart_create                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the line chart create function.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    chart                                 GX_LINE_CHART control block   */
/*    name                                  Logical name of icon widget   */
/*    parent                                Pointer to the parent widget  */
/*    info                                  chart drawing parameters      */
/*    style                                 chart style flags             */
/*    chart_id                              chart ID                      */
/*    size                                  chart size                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_line_chart_create                 Actual line chart create call */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_line_chart_create(GX_LINE_CHART *chart,
                             const GX_CHAR *name,
                             GX_WIDGET *parent,
                             const GX_LINE_CHART_INFO *info,
                             ULONG style,
                             USHORT chart_id,
                             const GX_RECTANGLE *size,
                             UINT control_block_size)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if ((chart == GX_NULL) || (size == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (control_block_size != sizeof(GX_LINE_CHART))
    {
        return(GX_INVALID_SIZE);
    }

    /* Check for widget already created.  */
    if (chart->type != 0)
    {
        return(GX_ALREADY_CREATED);
    }

    /* Call the actual icon button create function.  */
    status = _gx_line_chart_create(chart, name, parent, info, style, chart_id, size);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_line_chart_update                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the line chart draw.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    chart                                 GX_LINE_CHART control block   */
/*    data                                  Data array                    */
/*    data_count                            Data array size               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_line_chart_update                 Actual update call            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_line_chart_update(GX_LINE_CHART *chart, INT *data, INT data_count)
{
UINT status;

    /* Check for appropriate caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid input pointers.  */
    if (chart == GX_NULL)
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual icon button create function.  */
    status = _gx_line_chart_update(chart, data, data_count);

    /* Return completion status.  */
    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_line_chart_y_scale_calculate                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the line chart y scale calculate */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    chart                                 Line chart control block      */
/*    return_value                          location to pass return value */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_line_chart_y_scale_calculate      Actual line chart draw call   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_line_chart_y_scale_calculate(GX_LINE_CHART *chart, INT *return_value)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((chart == GX_NULL) || (return_value == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Call the actual icon button create function.  */
    status = _gx_line_chart_y_scale_calculate(chart, return_value);

    /* Return completion status.  */
    return status;
}
