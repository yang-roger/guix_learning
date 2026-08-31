#ifndef GX_CONSTS_H
#define GX_CONSTS_H

/* Define screen rotation types. */
#define GX_SCREEN_ROTATION_NONE               0
#define GX_SCREEN_ROTATION_CW                90
#define GX_SCREEN_ROTATION_CCW              270
#define GX_SCREEN_ROTATION_FLIP             180

/* API input parameters and general constants.  */
#define GX_TRUE                             1
#define GX_FALSE                            0
#define GX_NULL                             0
#define GX_ID_NONE                          0
#define GX_MAGIC_NUMBER                     0x4758U /* GX */
#define GX_RESOURCE_TYPE_BINRES_NORMAL      0x4758U /* GX */
#define GX_RESOURCE_TYPE_BINRES_STANDALONE  0x4753U /* GS */
#define GX_SEARCH_DEPTH_INFINITE            65535

/* API return values.  */
#define GX_SUCCESS                          0x00
#define GX_DELETED                          0x01
#define GX_INVALID_ID                       0x02
#define GX_NO_VIEWS                         0x03
#define GX_OUT_OF_TIMERS                    0x04
#define GX_DRAW_NESTING_EXCEEDED            0x05
#define GX_INVALID_CONTEXT                  0x06
#define GX_PTR_ERROR                        0x07
#define GX_NO_CHANGE                        0x08
#define GX_NOT_FOUND                        0x09
#define GX_FAILURE                          0x10
#define GX_CALLER_ERROR                     0x11
#define GX_INVALID_WIDGET                   0x12
#define GX_ALREADY_CREATED                  0x13
#define GX_INVALID_WIDGET_SIZE              0x14
#define GX_INVALID_COLOR                    0x15
#define GX_INVALID_FONT                     0x16
#define GX_INVALID_STRING                   0x17
#define GX_INVALID_STYLE                    0x18
#define GX_INVALID_SIZE                     0x19
#define GX_INVALID_ALPHA                    0x1A
#define GX_INVALID_TYPE                     0x1B
#define GX_INVALID_CANVAS_SIZE              0x1C
#define GX_INVALID_DISPLAY                  0x1D
#define GX_INVALID_WIDTH                    0x1E
#define GX_INVALID_HEIGHT                   0x1F
#define GX_INVALID_CANVAS                   0x20
#define GX_INVALID_COORDINATE               0x21
#define GX_INVALID_VALUE                    0x22
#define GX_INVALID_DISPLAY_SIZE             0x23
#define GX_INVALID_FORMAT                   0x24
#define GX_INVALID_APPEARANCE               0x25
#define GX_INVALID_STATUS                   0x26
#define GX_INPUT_CAPTURE_NESTING_EXCEEDED   0x27
#define GX_NOT_SUPPORTED                    0x28
#define GX_INVALID_MEMORY_SIZE              0x29
#define GX_SYSTEM_MEMORY_ERROR              0x30
#define GX_OUT_OF_ANIMATIONS                0x31
#define GX_INVALID_ANIMATION                0x32
#define GX_INVALID_RESOURCE_ID              0x33
#define GX_INVALID_STRING_LENGTH            0x34
#define GX_MATH_OVERFLOW                    0x35
#define GX_RICH_STYLE_NESTING_EXEEDED       0x36
#define GX_SYSTEM_ERROR                     0xFE


/* Define system error constants.  */
#define GX_SYSTEM_THREAD_CREATE_FAILED      1
#define GX_SYSTEM_QUEUE_CREATE_FAILED       2
#define GX_SYSTEM_MUTEX_CREATE_FAILED       3
#define GX_SYSTEM_EVENT_SIZE_MISMATCH       4
#define GX_SYSTEM_EVENT_RECEIVE_ERROR       5
#define GX_SYSTEM_PROTECTION_ERROR          6
#define GX_SYSTEM_DRIVER_SETUP_ERROR        7
#define GX_SYSTEM_OUT_OF_VIEWS              8

/* Define supported frame buffer color formats.  */
#define GX_COLOR_FORMAT_MONOCHROME          1                   /* 0 = black, 1 = white                     */
#define GX_COLOR_FORMAT_MONOCHROME_INVERTED 2                   /* 1 = black, 0 = white                     */
#define GX_COLOR_FORMAT_2BIT_GRAY           3                   /* 2 bit-per-pixel grayscale                */
#define GX_COLOR_FORMAT_2BIT_GRAY_INVERTED  4                   /* 2 bit-per-pixel grayscale inverted       */
#define GX_COLOR_FORMAT_4BIT_GRAY           5                   /* 4 bit-per-pixel grayscale                */
#define GX_COLOR_FORMAT_4BIT_GRAY_INVERTED  6
#define GX_COLOR_FORMAT_4BIT_VGA            7                   /* Obsolete VGA planar format, 16 colors    */
#define GX_COLOR_FORMAT_8BIT_GRAY           8                   /* 256 shades of gray                       */
#define GX_COLOR_FORMAT_8BIT_GRAY_INVERTED  9                   /* 256 shades of gray, 0 = white            */
#define GX_COLOR_FORMAT_8BIT_PALETTE        10                  /* 256 color palette mode                   */
#define GX_COLOR_FORMAT_8BIT_PACKED_PIXEL   11                  /* 3:3:2 color mode                         */
#define GX_COLOR_FORMAT_5551BGRX            12                  /* 15 bpp, b:g:r order                      */
#define GX_COLOR_FORMAT_1555XRGB            13                  /* 15 bpp, r:g:b order                      */
#define GX_COLOR_FORMAT_565RGB              14                  /* 16 bpp, r:g:b order                      */
#define GX_COLOR_FORMAT_4444ARGB            15                  /* 16 bpp, 4:4:4:4 argb order               */
#define GX_COLOR_FORMAT_4444BGRA            16                  /* 16 bpp, 4:4:4:4 bgra order               */
#define GX_COLOR_FORMAT_565BGR              17                  /* 16 bpp, b:g:r order                      */
#define GX_COLOR_FORMAT_24RGB               18                  /* 24 bpp, r:g:b order, packed              */
#define GX_COLOR_FORMAT_24BGR               19                  /* 24 bpp, b:g:r order, packed              */
#define GX_COLOR_FORMAT_24XRGB              20                  /* 24 bpp, r:g:b order, unpacked            */
#define GX_COLOR_FORMAT_24BGRX              21                  /* 24 bpp, b:g:r order, unpacked            */
#define GX_COLOR_FORMAT_32ARGB              22                  /* 32 bpp, a:r:g:b order                    */
#define GX_COLOR_FORMAT_32RGBA              23                  /* 32 bpp, r:g:b:a order                    */
#define GX_COLOR_FORMAT_32ABGR              24                  /* 32 BPP, a:b:g:r order                    */
#define GX_COLOR_FORMAT_32BGRA              25                  /* 32 BPP, b:g:r:a order                    */
#define GX_COLOR_FORMAT_8BIT_ALPHAMAP       26                  /* 8 bit alpha only                         */

/* Predefined color Resource IDs.  */
#define GX_COLOR_ID_CANVAS                  0
#define GX_COLOR_ID_WIDGET_FILL             1
#define GX_COLOR_ID_WINDOW_FILL             2
#define GX_COLOR_ID_DEFAULT_BORDER          3
#define GX_COLOR_ID_WINDOW_BORDER           4
#define GX_COLOR_ID_TEXT                    5
#define GX_COLOR_ID_SELECTED_TEXT           6
#define GX_COLOR_ID_SELECTED_FILL           7
#define GX_COLOR_ID_SHADOW                  8
#define GX_COLOR_ID_SHINE                   9

/* Button color Resource IDs.  */
#define GX_COLOR_ID_BTN_BORDER             10
#define GX_COLOR_ID_BUTTON_BORDER          GX_COLOR_ID_BTN_BORDER
#define GX_COLOR_ID_BTN_UPPER              11
#define GX_COLOR_ID_BUTTON_UPPER           GX_COLOR_ID_BTN_UPPER
#define GX_COLOR_ID_BTN_LOWER              12
#define GX_COLOR_ID_BUTTON_LOWER           GX_COLOR_ID_BTN_LOWER
#define GX_COLOR_ID_BTN_TEXT               13
#define GX_COLOR_ID_BUTTON_TEXT            GX_COLOR_ID_BTN_TEXT

/* Scrollbar color Resource IDs.   */
#define GX_COLOR_ID_SCROLL_FILL             14
#define GX_COLOR_ID_SCROLL_BUTTON           15

/* Text input color Resource IDs.  */
#define GX_COLOR_ID_TEXT_INPUT_TEXT         16
#define GX_COLOR_ID_TEXT_INPUT_FILL         17

/* slider color resource ids */
#define GX_COLOR_ID_SLIDER_TICK             18
#define GX_COLOR_ID_SLIDER_GROOVE_TOP       19      /* LIGHTGRAY */
#define GX_COLOR_ID_SLIDER_GROOVE_BOTTOM    20      /* WHITE */
#define GX_COLOR_ID_SLIDER_NEEDLE_OUTLINE   21      /* BLACK */
#define GX_COLOR_ID_SLIDER_NEEDLE_FILL      22      /* DARKGRAY */
#define GX_COLOR_ID_SLIDER_NEEDLE_LINE1     23      /* LIGHTGRAY */
#define GX_COLOR_ID_SLIDER_NEEDLE_LINE2     24      /* BTN_BORDER */

/* Text widget color resource ids */
#define GX_COLOR_ID_DISABLED_TEXT           25
#define GX_COLOR_ID_DISABLED_FILL           26
#define GX_COLOR_ID_READONLY_TEXT           27
#define GX_COLOR_ID_READONLY_FILL           28

#define GX_MAX_DEFAULT_COLORS               29
#define GX_FIRST_USER_COLOR                 GX_MAX_DEFAULT_COLORS

/* Default font Resource IDs.  */
#define GX_FONT_ID_DEFAULT                  0
#define GX_FONT_ID_SYSTEM                   GX_FONT_ID_DEFAULT
#define GX_FONT_ID_BUTTON                   1
#define GX_FONT_ID_PROMPT                   2
#define GX_FONT_ID_TEXT_INPUT               3
#define GX_DEFAULT_FONT_COUNT               4
#define GX_FIRST_USER_FONT                  GX_DEFAULT_FONT_COUNT

/* Default PIXELMAP Resource IDs. */
#define GX_PIXELMAP_NULL                    0
#define GX_PIXELMAP_RADIO_ON_ID             1
#define GX_PIXELMAP_ID_RADIO_ON             GX_PIXELMAP_RADIO_ON_ID
#define GX_PIXELMAP_RADIO_OFF_ID            2
#define GX_PIXELMAP_ID_RADIO_OFF            GX_PIXELMAP_RADIO_OFF_ID
#define GX_PIXELMAP_CHECKBOX_ON_ID          3
#define GX_PIXELMAP_ID_CHECKBOX_ON          GX_PIXELMAP_CHECKBOX_ON_ID
#define GX_PIXELMAP_CHECKBOX_OFF_ID         4
#define GX_PIXELMAP_ID_CHECKBOX_OFF         GX_PIXELMAP_CHECKBOX_OFF_ID
#define GX_DEFAULT_PIXELMAP_COUNT           5
#define GX_FIRST_USER_PIXELMAP              GX_DEFAULT_PIXELMAP_COUNT

/* Predefined a:r:g:b color values, these follow the standard 16 VGA color names.  */
#define GX_COLOR_BLACK                      0xff000000UL
#define GX_COLOR_RED                        0xffb80000UL
#define GX_COLOR_GREEN                      0xff00bc00UL
#define GX_COLOR_BROWN                      0xffb8bc00UL
#define GX_COLOR_BLUE                       0xff0000b8UL
#define GX_COLOR_MAGENTA                    0xffb800b8UL
#define GX_COLOR_CYAN                       0xff00bcb8UL
#define GX_COLOR_LIGHTGRAY                  0xffc0c0c0UL
#define GX_COLOR_DARKGRAY                   0xff808080UL
#define GX_COLOR_LIGHTRED                   0xffff0000UL
#define GX_COLOR_LIGHTGREEN                 0xff00ff00UL
#define GX_COLOR_YELLOW                     0xffffff00UL
#define GX_COLOR_LIGHTBLUE                  0xff0000ffUL
#define GX_COLOR_LIGHTMAGENTA               0xffff00ffUL
#define GX_COLOR_LIGHTCYAN                  0xff00ffffUL
#define GX_COLOR_WHITE                      0xffffffffUL

/* Define events. The the event ranges are:

   0x00000000 to 0x000000ff   256 pre-defined system event types
   0x00000100 to 0x00ffffff   Signals from child widgets
   0x01000000 to 0x3fffffff   Unused
   0x40000000 to 0xffffffff   Application defined

 */
#define GX_FIRST_SIGNAL                     0x100
#define GX_LAST_SIGNAL                      0x00ffffffUL
#define GX_SIGNAL_EVENT_MASK                0xff

#define GX_SIGNAL(_a, _b)                   (((_a) << 8) | (_b))
#define IS_SIGNAL(_a)                       (_a >= GX_FIRST_SIGNAL && _a <= GX_LAST_SIGNAL)

/* Define the pre-defined Widget event types.  */
#define GX_EVENT_TERMINATE                  1
#define GX_EVENT_REDRAW                     2
#define GX_EVENT_SHOW                       3
#define GX_EVENT_HIDE                       4
#define GX_EVENT_RESIZED                    5
#define GX_EVENT_SLIDE                      6
#define GX_EVENT_FOCUS_GAINED               7
#define GX_EVENT_FOCUS_LOST                 8
#define GX_EVENT_HORIZONTAL_SCROLL          9
#define GX_EVENT_VERTICAL_SCROLL            10
#define GX_EVENT_TIMER                      11
#define GX_EVENT_PEN_DOWN                   12
#define GX_EVENT_PEN_UP                     13
#define GX_EVENT_PEN_MOVE                   14
#define GX_EVENT_PEN_DRAG                   15
#define GX_EVENT_KEY_DOWN                   16
#define GX_EVENT_KEY_UP                     17
#define GX_EVENT_CLOSE                      18
#define GX_EVENT_DELETE                     19
#define GX_EVENT_SLIDER_VALUE               20
#define GX_EVENT_TOGGLE_ON                  21
#define GX_EVENT_TOGGLE_OFF                 22
#define GX_EVENT_RADIO_SELECT               23
#define GX_EVENT_RADIO_DESELECT             24
#define GX_EVENT_CLICKED                    25
#define GX_EVENT_LIST_SELECT                26
#define GX_EVENT_VERTICAL_FLICK             27
#define GX_EVENT_HORIZONTAL_FLICK           28

#define GX_EVENT_PARENT_SIZED               30
#define GX_EVENT_CLOSE_POPUP                31
#define GX_EVENT_ZOOM_IN                    32
#define GX_EVENT_ZOOM_OUT                   33
#define GX_EVENT_LANGUAGE_CHANGE            34
#define GX_EVENT_RESOURCE_CHANGE            35
#define GX_EVENT_ANIMATION_COMPLETE         36
#define GX_EVENT_SPRITE_COMPLETE            37
#define GX_EVENT_TEXT_EDITED                40
#define GX_EVENT_ANIMATION_START            41
#define GX_EVENT_FOCUS_NEXT                 42
#define GX_EVENT_FOCUS_PREVIOUS             43
#define GX_EVENT_FOCUS_GAIN_NOTIFY          44
#define GX_EVENT_SELECT                     45
#define GX_EVENT_DESELECT                   46
#define GX_EVENT_PROGRESS_VALUE             47
#define GX_EVENT_TOUCH_CALIBRATION_COMPLETE 48
#define GX_EVENT_INPUT_RELEASE              49
#define GX_EVENT_TREE_SELECT                50
#define GX_EVENT_STYLE_CHANGED              51
#define GX_EVENT_CLIENT_UPDATED             52
#define GX_EVENT_CUT                        53
#define GX_EVENT_COPY                       54
#define GX_EVENT_PASTE                      55
#define GX_EVENT_MARK_NEXT                  56
#define GX_EVENT_MARK_PREVIOUS              57
#define GX_EVENT_MARK_UP                    58
#define GX_EVENT_MARK_DOWN                  59
#define GX_EVENT_MARK_END                   60
#define GX_EVENT_MARK_HOME                  61
#define GX_EVENT_DYNAMIC_BIDI_TEXT_ENABLE   62
#define GX_EVENT_DYNAMIC_BIDI_TEXT_DISABLE  63

/* Define the value of the first application defined event type.  */
#define GX_FIRST_USER_EVENT                 0x40000000UL

/* The following name has been deprecated, please use GX_FIRST_USER_EVENT */
#define GX_FIRST_APP_EVENT                  GX_FIRST_USER_EVENT


/* Define logical key values.  */
#define GX_KEY_PAGE_UP                      0x1b01U
#define GX_KEY_PAGE_DOWN                    0x1b02U
#define GX_KEY_END                          0x1b03U
#define GX_KEY_HOME                         0x1b04U
#define GX_KEY_LEFT_ARROW                   0x1b05U
#define GX_KEY_UP_ARROW                     0x1b06U
#define GX_KEY_RIGHT_ARROW                  0x1b07U
#define GX_KEY_DOWN_ARROW                   0x1b08U
#define GX_KEY_NEXT                         0x1b09U
#define GX_KEY_PREVIOUS                     0x1b0aU
#define GX_KEY_CAPS_LOCK                    0x1b10U
#define GX_KEY_SHIFT                        0x1b11U
#define GX_KEY_CONTROL                      0x1b12U
#define GX_KEY_NUMLOCK                      0x1b13U
#define GX_KEY_SEPARATOR                    0x1b14U

#define GX_KEY_BACKSPACE                    0x08
#define GX_KEY_SPACE                        0x20
#define GX_KEY_DELETE                       0x7f
#define GX_KEY_CARRIAGE_RETURN              0x0d
#define GX_KEY_LINE_FEED                    0x0a
#define GX_KEY_SELECT                       GX_KEY_CARRIAGE_RETURN


/* Define the Widget types.  */
#define GX_TYPE_WIDGET                      1
#define GX_TYPE_BUTTON                      2
#define GX_TYPE_TEXT_BUTTON                 3
#define GX_TYPE_MULTI_LINE_TEXT_BUTTON      4
#define GX_TYPE_RADIO_BUTTON                5
#define GX_TYPE_CHECKBOX                    6
#define GX_TYPE_PIXELMAP_BUTTON             7
#define GX_TYPE_SHADOW_BUTTON               8
#define GX_TYPE_ICON_BUTTON                 9
#define GX_TYPE_SPIN_BUTTON                 10
#define GX_TYPE_ICON                        11
#define GX_TYPE_SPRITE                      12
#define GX_TYPE_CIRCULAR_GAUGE              13

#define GX_TYPE_SLIDER                      20
#define GX_TYPE_PIXELMAP_SLIDER             21
#define GX_TYPE_VERTICAL_SCROLL             22
#define GX_TYPE_HORIZONTAL_SCROLL           23
#define GX_TYPE_PROGRESS_BAR                24
#define GX_TYPE_RADIAL_PROGRESS_BAR         25
#define GX_TYPE_RADIAL_SLIDER               26

#define GX_TYPE_PROMPT                      30
#define GX_TYPE_NUMERIC_PROMPT              31
#define GX_TYPE_PIXELMAP_PROMPT             32
#define GX_TYPE_NUMERIC_PIXELMAP_PROMPT     33

#define GX_TYPE_SINGLE_LINE_TEXT_INPUT      64
#define GX_TYPE_PIXELMAP_TEXT_INPUT         65
#define GX_TYPE_DROP_LIST                   70

#define GX_TYPE_MENU_LIST                   75
#define GX_TYPE_MENU                        76
#define GX_TYPE_ACCORDION_MENU              77

/* Define the Window derived types. These types must always be >= GX_TYPE_WINDOW
   to insure type-safe casting in the GUIX internal code.  */
#define GX_TYPE_WINDOW                      128
#define GX_TYPE_ROOT_WINDOW                 129
#define GX_TYPE_VERTICAL_LIST               131
#define GX_TYPE_HORIZONTAL_LIST             132
#define GX_TYPE_POPUP_LIST                  133
#define GX_TYPE_MULTI_LINE_TEXT_VIEW        134
#define GX_TYPE_MULTI_LINE_TEXT_INPUT       135
#define GX_TYPE_LINE_CHART                  136
#define GX_TYPE_DIALOG                      137
#define GX_TYPE_KEYBOARD                    138
#define GX_TYPE_SCROLL_WHEEL                139
#define GX_TYPE_TEXT_SCROLL_WHEEL           140
#define GX_TYPE_STRING_SCROLL_WHEEL         141
#define GX_TYPE_NUMERIC_SCROLL_WHEEL        142
#define GX_TYPE_TREE_VIEW                   143
#define GX_TYPE_RICH_TEXT_VIEW              144
#define GX_TYPE_GENERIC_SCROLL_WHEEL        145

/* A pseudo-type, used by Studio code generator */
#define GX_TYPE_TEMPLATE                    200

/* Applications can define their own window types */
#define GX_FIRST_USER_WINDOW_TYPE           256


/* Define Widget status flags.  */
#define GX_STATUS_VISIBLE                   0x00000001UL
#define GX_STATUS_SELECTABLE                0x00000002UL
#define GX_STATUS_ACCEPTS_INPUT             0x00000004UL
#define GX_STATUS_HIDDEN                    0x00000008UL

#define GX_STATUS_DEFAULT_FOCUS             0x00000010UL
#define GX_STATUS_ACCEPTS_FOCUS             0x00000020UL
#define GX_STATUS_HAS_FOCUS                 0x00000040UL
#define GX_STATUS_NOTIFY_ON_GAIN_FOCUS      0x00000080UL

#define GX_STATUS_NAV_STOP                  0x00000100UL
#define GX_STATUS_NAV_PARENT                0x00000200UL

#define GX_STATUS_MOVABLE                   0x00001000UL
#define GX_STATUS_BUTTON_DERIVED            0x00002000UL
#define GX_STATUS_TOGGLE_UNLOCK             0x00004000UL
#define GX_STATUS_DIRTY                     0x00008000UL

/* define cursor status flags */
#define GX_STATUS_CURSOR_SHOW               0x00010000UL
#define GX_STATUS_CURSOR_DRAW               0x00020000UL
#define GX_STATUS_MARK_PREVIOUS             0x00040000UL
#define GX_STATUS_MARK_NEXT                 0x00080000UL

/* Define radial slider status flags */
#define GX_STATUS_ANIMATION_NONE            0x00010000UL
#define GX_STATUS_TRACKING_PEN              0x00020000UL

#define GX_STATUS_MODAL                     0x00100000UL

#define GX_STATUS_DYNAMIC_BUFFER            0x01000000UL
#define GX_STATUS_LINE_BREAK_PROCESSED      0x02000000UL
#define GX_STATUS_RESIZE_NOTIFY             0x04000000UL
#define GX_STATUS_STUDIO_CREATED            0x08000000UL

#define GX_STATUS_TRANSPARENT               0x10000000UL
#define GX_STATUS_NONCLIENT                 0x20000000UL
#define GX_STATUS_OWNS_INPUT                0x40000000UL
#define GX_STATUS_DYNAMICALLY_ALLOCATED     0x80000000UL

/* Define Widget style flags.  */
#define GX_STYLE_NONE                       0x00000000UL
#define GX_STYLE_BORDER_NONE                0x00000000UL
#define GX_STYLE_BORDER_RAISED              0x00000001UL
#define GX_STYLE_BORDER_RECESSED            0x00000002UL
#define GX_STYLE_BORDER_THIN                0x00000004UL
#define GX_STYLE_BORDER_THICK               0x00000008UL
#define GX_STYLE_BORDER_MASK                0x0000000fUL

/* Define global style flags.  */
#define GX_STYLE_TRANSPARENT                0x10000000UL
#define GX_STYLE_DRAW_SELECTED              0x20000000UL
#define GX_STYLE_ENABLED                    0x40000000UL
#define GX_STYLE_DYNAMICALLY_ALLOCATED      0x80000000UL
#define GX_STYLE_USE_LOCAL_ALPHA            0x01000000UL

/* Define Button style flags.  */
#define GX_STYLE_BUTTON_PUSHED              0x00000010UL
#define GX_STYLE_BUTTON_TOGGLE              0x00000020UL
#define GX_STYLE_BUTTON_RADIO               0x00000040UL
#define GX_STYLE_BUTTON_EVENT_ON_PUSH       0x00000080UL
#define GX_STYLE_BUTTON_REPEAT              0x00000100UL

/* Define Sprite style flags */
#define GX_STYLE_SPRITE_AUTO                0x00000010UL
#define GX_STYLE_SPRITE_LOOP                0x00000020UL

/* Define List style flags.  */
#define GX_STYLE_CENTER_SELECTED            0x00000010UL
#define GX_STYLE_WRAP                       0x00000020UL
#define GX_STYLE_FLICKABLE                  0x00000040UL
#define GX_STYLE_REPEAT_SELECT              0x00000080UL


/* Define Icon, Pixelmap button and Icon button alignment flags.  */
#define GX_STYLE_PIXELMAP_HALIGN_CENTER     0x00010000UL
#define GX_STYLE_PIXELMAP_HALIGN_LEFT       0x00020000UL
#define GX_STYLE_PIXELMAP_HALIGN_RIGHT      0x00040000UL
#define GX_STYLE_PIXELMAP_VALIGN_CENTER     0x00080000UL
#define GX_STYLE_PIXELMAP_VALIGN_TOP        0x00100000UL
#define GX_STYLE_PIXELMAP_VALIGN_BOTTOM     0x00200000UL

#define GX_STYLE_PIXELMAP_HALIGN_MASK       0x00070000UL
#define GX_STYLE_PIXELMAP_VALIGN_MASK       0x00380000UL

#define GX_STYLE_HALIGN_CENTER              GX_STYLE_PIXELMAP_HALIGN_CENTER
#define GX_STYLE_HALIGN_LEFT                GX_STYLE_PIXELMAP_HALIGN_LEFT
#define GX_STYLE_HALIGN_RIGHT               GX_STYLE_PIXELMAP_HALIGN_RIGHT
#define GX_STYLE_VALIGN_CENTER              GX_STYLE_PIXELMAP_VALIGN_CENTER
#define GX_STYLE_VALIGN_TOP                 GX_STYLE_PIXELMAP_VALIGN_TOP
#define GX_STYLE_VALIGN_BOTTOM              GX_STYLE_PIXELMAP_VALIGN_BOTTOM

#define GX_PIXELMAP_HALIGN_MASK             GX_STYLE_PIXELMAP_HALIGN_MASK
#define GX_PIXELMAP_VALIGN_MASK             GX_STYLE_PIXELMAP_VALIGN_MASK

/* Define Slider style flags.  */
#define GX_STYLE_SHOW_NEEDLE                0x00000200UL
#define GX_STYLE_SHOW_TICKMARKS             0x00000400UL
#define GX_STYLE_SLIDER_VERTICAL            0x00000800UL

/* Define Pixelmap Slider flags.  */
#define GX_STYLE_TILE_BACKGROUND            0x00001000UL

/* Define Progress Bar style flags.  */
#define GX_STYLE_PROGRESS_PERCENT           0x00000010UL
#define GX_STYLE_PROGRESS_TEXT_DRAW         0x00000020UL
#define GX_STYLE_PROGRESS_VERTICAL          0x00000040UL

#define GX_STYLE_PROGRESS_SEGMENTED_FILL    0x00000100UL

/* Define Radial Progress Bar style flags.  */
#define GX_STYLE_RADIAL_PROGRESS_ALIAS      0x00000200UL
#define GX_STYLE_RADIAL_PROGRESS_ROUND      0x00000400UL
#define GX_STYLE_RADIAL_PROGRESS_NO_BACKTRACK 0x00000800UL

/* Define Text alignment styles.  */
#define GX_STYLE_TEXT_LEFT                  0x00001000UL
#define GX_STYLE_TEXT_RIGHT                 0x00002000UL
#define GX_STYLE_TEXT_CENTER                0x00004000UL
#define GX_STYLE_TEXT_ALIGNMENT_MASK        0x00007000UL
#define GX_STYLE_TEXT_COPY                  0x00008000UL

/* define cursor style flags */
#define GX_STYLE_CURSOR_BLINK               0x00000040UL
#define GX_STYLE_CURSOR_ALWAYS              0x00000080UL

/* text input style */
#define GX_STYLE_TEXT_INPUT_NOTIFY_ALL      0x00000100UL
#define GX_STYLE_TEXT_INPUT_READONLY        0x00000200UL

/* Define Window styles.  */
#define GX_STYLE_TILE_WALLPAPER             0x00040000UL

/* Define circular gauge styles.  */
#define GX_STYLE_CIRCULAR_GAUGE_CCW         0x00000010UL

/* Define menu styles. */
#define GX_STYLE_MENU_EXPANDED              0x00000010UL

/* Define tree view styles. */
#define GX_STYLE_TREE_VIEW_SHOW_ROOT_LINES  0x00000010UL

/* Define Scroll Bar styles.  */
#define GX_SCROLLBAR_RELATIVE_THUMB         0x00020000UL
#define GX_SCROLLBAR_END_BUTTONS            0x00040000UL
#define GX_SCROLLBAR_VERTICAL               0x01000000UL
#define GX_SCROLLBAR_HORIZONTAL             0x02000000UL

/* Define text scroll wheel styles*/
#define GX_STYLE_TEXT_SCROLL_WHEEL_ROUND    0x00000100UL

/* Define animation status.  */
#define GX_ANIMATION_IDLE                   0x01U
#define GX_ANIMATION_ACTIVE                 0x02U
#define GX_ANIMATION_SLIDE_TRACKING         0x03U
#define GX_ANIMATION_SLIDE_LANDING          0x04U

/* Define pre-defined animation type flags.  */
#define GX_ANIMATION_NONE                   0x0000U
#define GX_ANIMATION_TRANSLATE              0x0001U
#define GX_ANIMATION_SCREEN_DRAG            0x0002U
#define GX_ANIMATION_BLOCK_MOVE             0x0004U

/* flags that can be used in combination with screen drag animation */
#define GX_ANIMATION_WRAP                   0x0100U
#define GX_ANIMATION_HORIZONTAL             0x0200U
#define GX_ANIMATION_VERTICAL               0x0400U

/* flag that can be used in combination with translate animation */
#define GX_ANIMATION_PUSH_STACK             0x4000U
#define GX_ANIMATION_DETACH                 0x8000U
#define GX_ANIMATION_EASING_FUNC_MASK       0x10f0U
#define GX_ANIMATION_BACK_EASE_IN           0x0010U
#define GX_ANIMATION_BACK_EASE_OUT          0x0020U
#define GX_ANIMATION_BACK_EASE_IN_OUT       0x0030U
#define GX_ANIMATION_BOUNCE_EASE_IN         0x0040U
#define GX_ANIMATION_BOUNCE_EASE_OUT        0x0050U
#define GX_ANIMATION_BOUNCE_EASE_IN_OUT     0x0060U
#define GX_ANIMATION_CIRC_EASE_IN           0x0070U
#define GX_ANIMATION_CIRC_EASE_OUT          0x0080U
#define GX_ANIMATION_CIRC_EASE_IN_OUT       0x0090U
#define GX_ANIMATION_CUBIC_EASE_IN          0x00a0U
#define GX_ANIMATION_CUBIC_EASE_OUT         0x00b0U
#define GX_ANIMATION_CUBIC_EASE_IN_OUT      0x00c0U
#define GX_ANIMATION_ELASTIC_EASE_IN        0x00d0U
#define GX_ANIMATION_ELASTIC_EASE_OUT       0x00e0U
#define GX_ANIMATION_ELASTIC_EASE_IN_OUT    0x00f0U
#define GX_ANIMATION_EXPO_EASE_IN           0x1000U
#define GX_ANIMATION_EXPO_EASE_OUT          0x1010U
#define GX_ANIMATION_EXPO_EASE_IN_OUT       0x1020U
#define GX_ANIMATION_QUAD_EASE_IN           0x1030U
#define GX_ANIMATION_QUAD_EASE_OUT          0x1040U
#define GX_ANIMATION_QUAD_EASE_IN_OUT       0x1050U
#define GX_ANIMATION_QUART_EASE_IN          0x1060U
#define GX_ANIMATION_QUART_EASE_OUT         0x1070U
#define GX_ANIMATION_QUART_EASE_IN_OUT      0x1080U
#define GX_ANIMATION_QUINT_EASE_IN          0x1090U
#define GX_ANIMATION_QUINT_EASE_OUT         0x10a0U
#define GX_ANIMATION_QUINT_EASE_IN_OUT      0x10b0U
#define GX_ANIMATION_SINE_EASE_IN           0x10c0U
#define GX_ANIMATION_SINE_EASE_OUT          0x10d0U
#define GX_ANIMATION_SINE_EASE_IN_OUT       0x10e0U

/* Sprite background operations */
#define GX_SPRITE_BACKGROUND_NO_ACTION      0x00     /* do nothing between frames            */
#define GX_SPRITE_BACKGROUND_SOLID_FILL     0x01     /* draw background with solid color     */
#define GX_SPRITE_BACKGROUND_RESTORE        0x02     /* draw background with first image     */


/* Define Font information.  */
#define GX_FONT_ALIASED                     0x01

/* Define Brush attributes.  */
#define GX_BRUSH_OUTLINE                    0x0000U            /* for rectangle, text, polygon              */
#define GX_BRUSH_SOLID_FILL                 0x0001U            /* For rectangle, text, polygon              */
#define GX_BRUSH_PIXELMAP_FILL              0x0002U            /* For rectangle, polygon                    */
#define GX_BRUSH_ALIAS                      0x0004U            /* For lines                                 */
#define GX_BRUSH_UNDERLINE                  0x0008U            /* Underlined text                           */
#define GX_BRUSH_ROUND                      0x0010U            /* For lines                                 */


/* Define Canvas information.  */
#define GX_CANVAS_SIMPLE                    0x01                /* Canvas is memory-only canvas (no display) */
#define GX_CANVAS_MANAGED                   0x02                /* canvas is composited to frame buffer      */
#define GX_CANVAS_VISIBLE                   0x04                /* Canvas is visible                         */
#define GX_CANVAS_MODIFIED                  0x08
#define GX_CANVAS_COMPOSITE                 0x20                /* Canvas is a buffer for creating composite*/
#define GX_CANVAS_PARTIAL_FRAME_BUFFER      0x40

/* Define common combinations.  */
#define GX_CANVAS_MANAGED_VISIBLE           (GX_CANVAS_MANAGED | GX_CANVAS_VISIBLE)


#define GX_ALPHA_VALUE_OPAQUE               255


/* Define pre-defined Widget Resource IDs.  */
#define GX_ID_VERTICAL_SCROLL               0xff00U
#define GX_ID_HORIZONTAL_SCROLL             0xff01U
#define GX_ID_SCROLL_UP_LEFT                0xff02U
#define GX_ID_SCROLL_DOWN_RIGHT             0xff03U
#define GX_ID_SCROLL_THUMB                  0xff04U
#define GX_ID_DROP_BUTTON                   0xff05U

#define GX_ID_DROP_LIST_BUTTON              0xffffU


/* pre-defined timer IDs */
#define GX_BUTTON_TIMER                     0xff00U
#define GX_CIRCULAR_GAUGE_TIMER             0xff01U
#define GX_RADIAL_SLIDER_TIMER              0xff02U

/* Define the echo mode of Single-line Text input.  */
#define GX_STAR_ECHO                        0x01
#define GX_ECHO                             0x02

/* Define image type.  */
#define GX_IMAGE_TYPE_BMP                   0x01
#define GX_IMAGE_TYPE_GIF                   0x02
#define GX_IMAGE_TYPE_PNG                   0x03
#define GX_IMAGE_TYPE_JPG                   0x04

/* Define action types for Studio generated screen flow */
#define GX_ACTION_TYPE_ATTACH               1
#define GX_ACTION_TYPE_DETACH               2
#define GX_ACTION_TYPE_TOGGLE               3
#define GX_ACTION_TYPE_SHOW                 4
#define GX_ACTION_TYPE_HIDE                 5
#define GX_ACTION_TYPE_ANIMATION            6
#define GX_ACTION_TYPE_WINDOW_EXECUTE       7
#define GX_ACTION_TYPE_WINDOW_EXECUTE_STOP  8
#define GX_ACTION_TYPE_SCREEN_STACK_PUSH    9
#define GX_ACTION_TYPE_SCREEN_STACK_POP     10
#define GX_ACTION_TYPE_SCREEN_STACK_RESET   11

/* Gradient Styles */
#define GX_GRADIENT_TYPE_VERTICAL           0x01
#define GX_GRADIENT_TYPE_ALPHA              0x02
#define GX_GRADIENT_TYPE_MIRROR             0x04

/* Mouse status flags */
#define GX_MOUSE_ENABLED                    0x01
#define GX_MOUSE_VISIBLE                    0x02

/* Text input cursor flags */
#define GX_CURSOR_USE_CUSTOM_HEIGHT         0x01

/* Text render stlyles */
#if defined(GX_THAI_GLYPH_SHAPING_SUPPORT)
#define GX_TEXT_RENDER_THAI_GLYPH_SHAPING   0x01
#endif

/* Language direction flags. */
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
#define GX_LANGUAGE_DIRECTION_LTR           0x01
#define GX_LANGUAGE_DIRECTION_RTL           0x02
#endif

#endif
