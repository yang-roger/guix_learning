#ifndef GX_MACROS_H
#define GX_MACROS_H

/* Define macros used in GUIX.  */

#ifndef GX_PARAMETER_NOT_USED
#define GX_PARAMETER_NOT_USED(p) ((void) (p))
#endif /* GX_PARAMETER_NOT_USED */

#ifndef GX_MIN
#define GX_MIN(_a, _b)                      (((_a) < (_b)) ? (_a) : (_b))
#endif

#ifndef GX_MAX
#define GX_MAX(_a, _b)                      (((_a) > (_b)) ? (_a) : (_b))
#endif

#ifndef GX_ABS
#define GX_ABS(_a)                          (((_a) < 0) ? (-(_a)) : (_a))
#endif

#ifndef GX_MEMCPY
#define GX_MEMCPY(a, b, c)                  memcpy(a, b, c)
#endif

#ifndef GX_STRLEN
#define GX_STRLEN(a)                        strlen((char *)a)
#endif

#ifndef GX_STRCPY
#define GX_STRCPY(a, b)                     strcpy(a, b)
#endif

#ifndef GX_STRCAT
#define GX_STRCAT(a, b)                     strcat(a, b)
#endif

#define GX_SWAP_VALS(_a, _b)                {_a ^= _b; _b ^= _a; _a ^= _b; }

#endif
