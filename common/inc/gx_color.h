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

#define GX_CLAMP_MAX(val, max_val)     if ((val) > (max_val)) { val = (max_val); }

#define GX_COLOR_VALUE_RAW_FROM_4COLORS(a_val, b_val, c_val, d_val,         \
                                        a_coeff, b_coeff, c_coeff, d_coeff) \
    ((GX_COLOR)((GX_COLOR)(a_val) * (a_coeff) +                             \
                (GX_COLOR)(b_val) * (b_coeff) +                             \
                (GX_COLOR)(c_val) * (c_coeff) +                             \
                (GX_COLOR)(d_val) * (d_coeff)) >> 16)

static inline void gx_color_32argb_raw_from_4colors(GX_COLOR a, GX_COLOR b, GX_COLOR c, GX_COLOR d, INT xdiff, INT ydiff,
                                                    INT& alpha, GX_COLOR& red, GX_COLOR& green, GX_COLOR& blue)
{
    INT a_coeff = (256 - xdiff) * (256 - ydiff);
    INT b_coeff = xdiff * (256 - ydiff);
    INT c_coeff = ydiff * (256 - xdiff);
    INT d_coeff = xdiff * ydiff;

    red = GX_COLOR_VALUE_RAW_FROM_4COLORS(REDVAL_24BPP(a), REDVAL_24BPP(b), REDVAL_24BPP(c), REDVAL_24BPP(d),
                                          a_coeff, b_coeff, c_coeff, d_coeff);

    green = GX_COLOR_VALUE_RAW_FROM_4COLORS(GREENVAL_24BPP(a), GREENVAL_24BPP(b), GREENVAL_24BPP(c), GREENVAL_24BPP(d),
                                            a_coeff, b_coeff, c_coeff, d_coeff);

    blue = GX_COLOR_VALUE_RAW_FROM_4COLORS(BLUEVAL_24BPP(a), BLUEVAL_24BPP(b), BLUEVAL_24BPP(c), BLUEVAL_24BPP(d),
                                           a_coeff, b_coeff, c_coeff, d_coeff);

    if ((alpha > 0) && (alpha < 0xff))
    {
        red = (red << 8) / (GX_COLOR)alpha;
        green = (green << 8) / (GX_COLOR)alpha;
        blue = (blue << 8) / (GX_COLOR)alpha;
    }

    GX_CLAMP_MAX(red, 255)
    GX_CLAMP_MAX(green, 255)
    GX_CLAMP_MAX(blue, 255)
    GX_CLAMP_MAX(alpha, 255)
}

static inline GX_COLOR gx_color_32argb_raw_from_4colors(GX_COLOR a, GX_COLOR b, GX_COLOR c, GX_COLOR d, INT xdiff, INT ydiff, INT alpha)
{
    GX_COLOR red;
    GX_COLOR green;
    GX_COLOR blue;

    gx_color_32argb_raw_from_4colors(a, b, c, d, xdiff, ydiff, alpha, red, green, blue);

    return (GX_COLOR)ASSEMBLECOLOR_32ARGB(alpha, red, green, blue);
}

// blends color and alpha after this function is called
static inline GX_COLOR gx_color_32argb_raw_from_4colors_2(GX_COLOR a, GX_COLOR b, GX_COLOR c, GX_COLOR d, INT xdiff, INT ydiff, INT& alpha)
{
    GX_COLOR red;
    GX_COLOR green;
    GX_COLOR blue;

    gx_color_32argb_raw_from_4colors(a, b, c, d, xdiff, ydiff, alpha, red, green, blue);

    return (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, red, green, blue);
}

#define GX_COLOR_VALUE_ALPHA_FROM_4COLORS(a_val, b_val, c_val, d_val,         \
                                          a_alpha, b_alpha, c_alpha, d_alpha, \
                                          a_coeff, b_coeff, c_coeff, d_coeff) \
    ((GX_COLOR)((GX_COLOR)(a_val) * (a_alpha) * (a_coeff) +                   \
                (GX_COLOR)(b_val) * (b_alpha) * (b_coeff) +                   \
                (GX_COLOR)(c_val) * (c_alpha) * (c_coeff) +                   \
                (GX_COLOR)(d_val) * (d_alpha) * (d_coeff)) >> 16)

static inline void gx_color_32argb_alpha_from_4colors(GX_COLOR a, GX_COLOR b, GX_COLOR c, GX_COLOR d, INT xdiff, INT ydiff,
                                                      INT& alpha, GX_COLOR& red, GX_COLOR& green, GX_COLOR& blue)
{
    INT a_alpha = ALPHAVAL_32BPP(a);
    INT b_alpha = ALPHAVAL_32BPP(b);
    INT c_alpha = ALPHAVAL_32BPP(c);
    INT d_alpha = ALPHAVAL_32BPP(d);

    INT a_coeff = (256 - xdiff) * (256 - ydiff);
    INT b_coeff = xdiff * (256 - ydiff);
    INT c_coeff = ydiff * (256 - xdiff);
    INT d_coeff = xdiff * ydiff;

    red = GX_COLOR_VALUE_ALPHA_FROM_4COLORS(REDVAL_24BPP(a), REDVAL_24BPP(b), REDVAL_24BPP(c), REDVAL_24BPP(d),
                                            a_alpha, b_alpha, c_alpha, d_alpha,
                                            a_coeff, b_coeff, c_coeff, d_coeff);

    green = GX_COLOR_VALUE_ALPHA_FROM_4COLORS(GREENVAL_24BPP(a), GREENVAL_24BPP(b), GREENVAL_24BPP(c), GREENVAL_24BPP(d),
                                              a_alpha, b_alpha, c_alpha, d_alpha,
                                              a_coeff, b_coeff, c_coeff, d_coeff);

    blue = GX_COLOR_VALUE_ALPHA_FROM_4COLORS(BLUEVAL_24BPP(a), BLUEVAL_24BPP(b), BLUEVAL_24BPP(c), BLUEVAL_24BPP(d),
                                             a_alpha, b_alpha, c_alpha, d_alpha,
                                             a_coeff, b_coeff, c_coeff, d_coeff);

    alpha = (INT)((GX_COLOR)((a_alpha) * (a_coeff) +
                             (b_alpha) * (b_coeff) +
                             (c_alpha) * (c_coeff) +
                             (d_alpha) * (d_coeff)) >> 16);

    if (alpha)
    {
        red /= (UINT)alpha;
        green /= (UINT)alpha;
        blue /= (UINT)alpha;
    }

    GX_CLAMP_MAX(red, 255)
    GX_CLAMP_MAX(green, 255)
    GX_CLAMP_MAX(blue, 255)
    GX_CLAMP_MAX(alpha, 255)
}

static inline GX_COLOR gx_color_32argb_alpha_from_4colors(GX_COLOR a, GX_COLOR b, GX_COLOR c, GX_COLOR d, INT xdiff, INT ydiff)
{
    INT alpha;
    GX_COLOR red;
    GX_COLOR green;
    GX_COLOR blue;

    gx_color_32argb_alpha_from_4colors(a, b, c, d, xdiff, ydiff, alpha, red, green, blue);

    return (GX_COLOR)ASSEMBLECOLOR_32ARGB(alpha, red, green, blue);
}

// blends color and alpha after this function is called
static inline GX_COLOR gx_color_32argb_alpha_from_4colors_2(GX_COLOR a, GX_COLOR b, GX_COLOR c, GX_COLOR d, INT xdiff, INT ydiff, INT& alpha)
{
    GX_COLOR red;
    GX_COLOR green;
    GX_COLOR blue;

    gx_color_32argb_alpha_from_4colors(a, b, c, d, xdiff, ydiff, alpha, red, green, blue);

    return (GX_COLOR)ASSEMBLECOLOR_32ARGB(0xff, red, green, blue);
}

#endif
