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

#define GX_COLOR_32ARGB_FROM_565RGB(c)  \
    (GX_COLOR)(0xFF000000 | GX_COLOR_24RGB_FROM_565RGB(c))

#define GX_COLOR_32ARGB_FROM_4444ARGB(c) \
    (GX_COLOR)(0xFF000000 | GX_COLOR_24RGB_FROM_4444ARGB(c))

static inline GX_COLOR gx_color_32argb_from_4444argb(USHORT pixel)
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

    return (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, r, g, b);
}

static inline GX_COLOR gx_color_24xrgb_blend(GX_COLOR fcolor, GX_UBYTE falpha, GX_COLOR bcolor, GX_UBYTE balpha)
{
    // split foreground into red, green, and blue components
    GX_UBYTE fred = REDVAL_24BPP(fcolor);
    GX_UBYTE fgreen = GREENVAL_24BPP(fcolor);
    GX_UBYTE fblue = BLUEVAL_24BPP(fcolor);

    // split background color into red, green, and blue components
    GX_UBYTE bred = REDVAL_24BPP(bcolor);
    GX_UBYTE bgreen = GREENVAL_24BPP(bcolor);
    GX_UBYTE bblue = BLUEVAL_24BPP(bcolor);

    // blend foreground and background, each color channel
    fred = (GX_UBYTE)(((bred * balpha) + (fred * falpha)) >> 8);
    fgreen = (GX_UBYTE)(((bgreen * balpha) + (fgreen * falpha)) >> 8);
    fblue = (GX_UBYTE)(((bblue * balpha) + (fblue * falpha)) >> 8);

    // re-assemble into 32-bit color
    return (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, fred, fgreen, fblue);
}

static inline GX_COLOR gx_color_24xrgb_blend(GX_COLOR fcolor, GX_UBYTE alpha, GX_COLOR bcolor)
{
    if (alpha == 255)
    {
        return (GX_COLOR)(fcolor | 0xff000000);
    }

    // background alpha is inverse of foreground alpha
    GX_UBYTE balpha = (GX_UBYTE)(256 - alpha);

    return gx_color_24xrgb_blend(fcolor, alpha, bcolor, balpha);
}

static inline GX_COLOR gx_color_32argb_blend(GX_COLOR fcolor, GX_UBYTE falpha, GX_COLOR bcolor)
{
    // no need to blend if falpha value is 255.
    if (falpha == 255)
    {
        return fcolor;
    }

    // split foreground into red, green, and blue components
    GX_UBYTE fred = REDVAL_32BPP(fcolor);
    GX_UBYTE fgreen = GREENVAL_32BPP(fcolor);
    GX_UBYTE fblue = BLUEVAL_32BPP(fcolor);

    // split background color into alpha, red, green, and blue components
    GX_UBYTE balpha = ALPHAVAL_32BPP(bcolor);
    GX_UBYTE bred = REDVAL_32BPP(bcolor);
    GX_UBYTE bgreen = GREENVAL_32BPP(bcolor);
    GX_UBYTE bblue = BLUEVAL_32BPP(bcolor);

    // background alpha is inverse of foreground alpha
    INT combined_alpha = (falpha * balpha) / 255;

    // blend foreground and background, each color channel
    GX_UBYTE oalpha = (GX_UBYTE)(falpha + balpha - combined_alpha);
    fred = (GX_UBYTE)((fred * falpha + bred * balpha - bred * combined_alpha) / oalpha);
    fgreen = (GX_UBYTE)((fgreen * falpha + bgreen * balpha - bgreen * combined_alpha) / oalpha);
    fblue = (GX_UBYTE)((fblue * falpha + bblue * balpha - bblue * combined_alpha) / oalpha);

    // re-assemble into 32-bit color
    return (GX_COLOR)ASSEMBLECOLOR_32ARGB(oalpha, fred, fgreen, fblue);
}

#endif
