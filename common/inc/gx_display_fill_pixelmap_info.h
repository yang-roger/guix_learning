#ifndef GX_DISPLAY_FILL_PIXELMAP_INFO_H
#define GX_DISPLAY_FILL_PIXELMAP_INFO_H

#include "gx_pixelmap.h"

/* Define fill map info type.  */
struct GX_FILL_PIXELMAP_INFO
{
    GX_PIXELMAP* pixelmap;
    GX_BOOL      draw;
    GX_UBYTE*    current_pixel_ptr;
    GX_UBYTE*    current_aux_ptr;
    GX_UBYTE     mask;
    INT          x_offset;
    INT          y_offset;

    // Data pointers goes to the end of full map, move it to the start again.
    void data_ptrs_reset_()
    {
        if (current_pixel_ptr >= pixelmap->data + pixelmap->data_size)
        {
            current_pixel_ptr = (GX_UBYTE*)pixelmap->data;
            current_aux_ptr = (GX_UBYTE*)pixelmap->aux_data;
        }
    }
};

#endif
