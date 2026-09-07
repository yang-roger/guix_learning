#ifndef GX_TOUCH_H
#define GX_TOUCH_H

#include "gx_global.h"
#include "gx_point.h"

struct GX_MULTI_LINE_TEXT_VIEW;
struct GX_ICON;

/* define resistive touch screen calibration params */
struct TOUCH_CALIBRATION_MATRIX
{
    INT  An;     /* A = An/Divider */
    INT  Bn;     /* B = Bn/Divider */
    INT  Cn;     /* C = Cn/Divider */
    INT  Dn;     /* D = Dn/Divider */
    INT  En;     /* E = En/Divider */
    INT  Fn;     /* F = Fn/Divider */
    INT  Divider ;
};

/* touch driver info specified by application. This information is required
   to run the touch screen calibration */
struct GX_RESISTIVE_TOUCH_INFO
{
    GX_RESOURCE_ID string_id_no_touch;
    GX_RESOURCE_ID string_id_touch_1;
    GX_RESOURCE_ID string_id_touch_2;
    GX_RESOURCE_ID string_id_touch_3;
    GX_MULTI_LINE_TEXT_VIEW *text;
    GX_ICON *target;
};

/* touch driver control block */
struct GX_RESISTIVE_TOUCH
{
    GX_RESISTIVE_TOUCH_INFO   info;
    GX_BOOL                   calibrated;
    USHORT                    min_drag_delta;
    USHORT                    sample_size;
    USHORT                    max_retries;
    USHORT                    stability_limit;
    USHORT                    x_range;
    USHORT                    y_range;
    USHORT                    current_touch_state;
    USHORT                    last_touch_state;
    GX_POINT                 *sample_ram;
    GX_POINT                  current_touch_coord;
    GX_POINT                  last_touch_coord;
    GX_BOOL                 (*pen_down_detect)(void);
    USHORT                  (*sample_read)(GX_VALUE axis);
    TOUCH_CALIBRATION_MATRIX  calibration_matrix;
};

/* pen down or up status, returned to generic driver */
#define GX_TOUCH_STATE_TOUCHED  1
#define GX_TOUCH_STATE_RELEASED 2
#define GX_TOUCH_X_AXIS         1
#define GX_TOUCH_Y_AXIS         2

#endif
