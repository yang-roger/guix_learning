#ifndef GX_PIXELMAP_H
#define GX_PIXELMAP_H

#include "gx_global.h"

/* Define Pixelmap type.  */
struct GX_PIXELMAP
{
    GX_UBYTE           version_major;               /* Pixelmap major version number            */
    GX_UBYTE           version_minor;               /* Pixelmap minor version number            */
    GX_UBYTE           flags;                       /* Flags defined below                      */
    GX_UBYTE           format;                      /* Color format, defined below              */
    const GX_UBYTE*    data;                        /* Pointer to pixelmap data                 */
    ULONG              data_size;                   /* Size of the pixelmap data array          */
    const GX_UBYTE*    aux_data;                    /* Auxiliary data array.                    */
    ULONG              aux_data_size;               /* Size of the auxiliary data array         */
    ULONG              transparent_color;           /* Used for translated GIF images           */
    GX_VALUE           width;                       /* Width - in pixels                        */
    GX_VALUE           height;                      /* Height - in pixels                       */
};


/* Pixelmap flags that can be in GX_PIXELMAP.flags above.  */

#define GX_PIXELMAP_TRANSPARENT             0x01    /* Pixelmap has transparent pixels          */
#define GX_PIXELMAP_COMPRESSED              0x02    /* Pixelmap is compressed format            */
#define GX_PIXELMAP_ALPHA                   0x04    /* Pixelmap has alpha channel               */
#define GX_PIXELMAP_TARGA                   0x08    /* Pixelmap uses Targa format compresssion  */
#define GX_PIXELMAP_RAW_FORMAT              0x10    /* RAW JPG/PNG format                       */
#define GX_PIXELMAP_DYNAMICALLY_ALLOCATED   0x20    /* Pixelmap is dynamically allocated        */
#define GX_PIXELMAP_ROTATED_CW              0x40
#define GX_PIXELMAP_ROTATED_CCW             0x80

/* Deprecated definitions, provided only for backward compatibility */
#define GX_PIXELMAP_ROTATED_90  GX_PIXELMAP_ROTATED_CW
#define GX_PIXELMAP_ROTATED_270 GX_PIXELMAP_ROTATED_CCW

#define PIXELMAP_IS_TRANSPARENT(a) (a->flags & (GX_PIXELMAP_TRANSPARENT | GX_PIXELMAP_ALPHA))

#endif
