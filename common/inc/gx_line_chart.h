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
/**   Line Chart  (Charting)                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_line_chart.h                                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX line chart component, including all data */
/*    types and external references.  It is assumed that gx_api.h and     */
/*    gx_port.h have already been included.                               */
/*                                                                        */
/**************************************************************************/

#ifndef GX_LINE_CHART_H
#define GX_LINE_CHART_H

#include "gx_window.h"

struct GX_LINE_CHART_INFO
{
    INT            min_val;
    INT            max_val;
    INT*           data;
    GX_VALUE       left_margin;
    GX_VALUE       top_margin;
    GX_VALUE       right_margin;
    GX_VALUE       bottom_margin;
    GX_VALUE       max_data_count;
    GX_VALUE       active_data_count;
    GX_VALUE       axis_line_width;
    GX_VALUE       data_line_width;
    GX_RESOURCE_ID axis_color;
    GX_RESOURCE_ID line_color;
};

/* Define GX_LINE_CHART control block type */
struct GX_LINE_CHART : public GX_WINDOW
{
    GX_LINE_CHART_INFO info;

    UINT create_(const GX_CHAR* name, GX_WIDGET* parent,
                 const GX_LINE_CHART_INFO* info,
                 ULONG style, USHORT chart_id, const GX_RECTANGLE* size);

    GX_VALUE content_left_()   const { return left_()   + info.left_margin;   }
    GX_VALUE content_top_()    const { return top_()    + info.top_margin;    }
    GX_VALUE content_right_()  const { return right_()  - info.right_margin;  }
    GX_VALUE content_bottom_() const { return bottom_() - info.bottom_margin; }

    GX_VALUE content_width_() const
    {
        return width_() - info.left_margin - info.right_margin;
    }

    GX_VALUE content_height_() const
    {
        return height_() - info.top_margin - info.bottom_margin;
    }

    void content_rectangle_(GX_RECTANGLE* rect) const
    {
        rect->set_((GX_VALUE)(left_() + info.left_margin),
                   (GX_VALUE)(top_() + info.top_margin),
                   (GX_VALUE)(right_() - info.right_margin),
                   (GX_VALUE)(bottom_() - info.bottom_margin));
    }

    void update_(INT* data, INT data_count);

    INT y_scale_calculate_() const;

    void axis_draw_();
    void data_draw_();

    void draw_();

private:
    INT y_pos_for_value_(INT value, INT y_scale, GX_VALUE chart_bottom) const;
};


/* APIs defined for line chart widget */

void _gx_line_chart_axis_draw(GX_LINE_CHART* chart);
UINT _gx_line_chart_create(GX_LINE_CHART* chart,
                           const GX_CHAR* name,
                           GX_WIDGET* parent,
                           const GX_LINE_CHART_INFO* info,
                           ULONG style,
                           USHORT chart_id,
                           const GX_RECTANGLE* size);
void _gx_line_chart_data_draw(GX_LINE_CHART* chart);
void _gx_line_chart_draw(GX_LINE_CHART* chart);
UINT _gx_line_chart_update(GX_LINE_CHART* chart, INT* data, INT data_count);
UINT _gx_line_chart_y_scale_calculate(GX_LINE_CHART* chart, INT* return_val);


/* Define error checking shells for API services.  These are only referenced by the application.  */

UINT _gxe_line_chart_create(GX_LINE_CHART* chart,
                            const GX_CHAR* name,
                            GX_WIDGET* parent,
                            const GX_LINE_CHART_INFO* info,
                            ULONG style,
                            USHORT chart_id,
                            const GX_RECTANGLE* size,
                            UINT control_block_size);
UINT _gxe_line_chart_update(GX_LINE_CHART* chart, INT* data, INT data_count);
UINT _gxe_line_chart_y_scale_calculate(GX_LINE_CHART* chart, INT* return_val);

#endif

