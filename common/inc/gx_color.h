#ifndef GX_COLOR_H
#define GX_COLOR_H

#define REDVAL_16BPP(_c)      (GX_UBYTE)(((_c) >> 11) & 0x1f)
#define GREENVAL_16BPP(_c)    (GX_UBYTE)(((_c) >> 5) & 0x3f)
#define BLUEVAL_16BPP(_c)     (GX_UBYTE)(((_c)) & 0x1f)

/* Define macros for assembling a 16-bit r:g:b value from 3 components.  */

#define ASSEMBLECOLOR_16BPP(_r, _g, _b) \
    ((((_r) & 0x1f) << 11) |            \
     (((_g) & 0x3f) << 5) |             \
     (((_b) & 0x1f)))


#define REDVAL_24BPP(_c)      (GX_UBYTE)((_c) >> 16)
#define GREENVAL_24BPP(_c)    (GX_UBYTE)((_c) >> 8)
#define BLUEVAL_24BPP(_c)     (GX_UBYTE)(_c)

/* Define macros for assembling a 24-bit r:g:b value from 3 components.  */
#define ASSEMBLECOLOR_24BPP(_r, _g, _b) \
    (((_r) << 16) |                     \
     ((_g) << 8) |                      \
     (_b))


#define ALPHAVAL_32BPP(_c)    (GX_UBYTE)((_c) >> 24)
#define REDVAL_32BPP(_c)      (GX_UBYTE)((_c) >> 16)
#define GREENVAL_32BPP(_c)    (GX_UBYTE)((_c) >> 8)
#define BLUEVAL_32BPP(_c)     (GX_UBYTE)(_c)

/* Define macros for assembling a 32-bit r:g:b value from 3 components.  */

#define ASSEMBLECOLOR_32BPP(_r, _g, _b) \
    (((_r) << 16) |                     \
     ((_g) << 8) |                      \
     (_b))


#define ASSEMBLECOLOR_32ARGB(_a, _r, _g, _b) \
    (((_a) << 24) |                          \
     ((_r) << 16) |                          \
     ((_g) << 8) |                           \
     (_b))


#define GX_COLOR_24RGB_FROM_565RGB(c)   \
     (GX_COLOR)((((c) & 0xf800) << 8) | \
                (((c) & 0x07e0) << 5) | \
                (((c) & 0x001f) << 3))


#define GX_COLOR_24RGB_FROM_4444ARGB(c) \
     (GX_COLOR)((((c) & 0x0f00) << 12) | \
                (((c) & 0x00f0) << 8)  | \
                (((c) & 0x000f) << 4))

static inline GX_COLOR gx_color_24rgb_from_4444argb(USHORT pixel)
{
    GX_UBYTE r;
    GX_UBYTE g;
    GX_UBYTE b;

    r = (GX_UBYTE)((pixel & 0x0f00) >> 4);
    r = (GX_UBYTE)((r >> 4) | r);
    g = (GX_UBYTE)(pixel & 0x00f0);
    g = (GX_UBYTE)((g >> 4) | g);
    b = (GX_UBYTE)(pixel & 0x000f);
    b = (GX_UBYTE)((b << 4) | b);

    return (GX_COLOR)ASSEMBLECOLOR_32BPP(r, g, b);
}


#endif
