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
/**   Utility (BIDI)                                                      */
/**                                                                       */
/**************************************************************************/

#include "gx_utility_bidi.h"

#include "gx_system.h"
#include "gx_utility_string.h"

/**************************************************************************/
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
#if defined(GX_DYNAMIC_ARABIC_SHAPING_SUPPORT)
/* Define joining types. */
#define JOINING_TYPE_NONE        0x01
#define JOINING_TYPE_RIGHT       0x02
#define JOINING_TYPE_LEFT        0x03
#define JOINING_TYPE_DUAL        0x04
#define JOINING_TYPE_CAUSING     0x05
#define JOINING_TYPE_TRANSPARENT 0x06

#define ARABIC_FORM_ISOLATED     0x01
#define ARABIC_FORM_INITIAL      0x02
#define ARABIC_FORM_FINAL        0x03
#define ARABIC_FORM_MEDIAL       0x04

struct ARABIC_SHAPING
{
    ULONG range_start;
    ULONG range_end;
    ULONG joining_type;
};

struct ARABIC_FORM
{
    ULONG code_point;
    ULONG isolated;
    ULONG final;
    ULONG initial;
    ULONG medial;
};

struct ARABIC_LIGATURE
{
    ULONG alef;
    ULONG lam;
    ULONG ligature;
};

struct ARABIC_UNIT
{
    ULONG code_point;
    ULONG joining_type;
    ULONG shape_form;
};

static ARABIC_SHAPING  arabic_shaping_table[] = {
    {0x0600, 0x060F, JOINING_TYPE_NONE},
    {0x0610, 0x061A, JOINING_TYPE_TRANSPARENT},
    {0x061B, 0x061B, JOINING_TYPE_NONE},
    {0x061C, 0x061C, JOINING_TYPE_TRANSPARENT},
    {0x061B, 0x061F, JOINING_TYPE_NONE},
    {0x0620, 0x0620, JOINING_TYPE_DUAL},
    {0x0621, 0x0621, JOINING_TYPE_NONE},
    {0x0622, 0x0625, JOINING_TYPE_RIGHT},
    {0x0626, 0x0626, JOINING_TYPE_DUAL},
    {0x0627, 0x0627, JOINING_TYPE_RIGHT},
    {0x0628, 0x0628, JOINING_TYPE_DUAL},
    {0x0629, 0x0629, JOINING_TYPE_RIGHT},
    {0x062A, 0x062E, JOINING_TYPE_DUAL},
    {0x062F, 0x0632, JOINING_TYPE_RIGHT},
    {0x0633, 0x063F, JOINING_TYPE_DUAL},
    {0x0640, 0x0640, JOINING_TYPE_CAUSING},
    {0x0641, 0x0647, JOINING_TYPE_DUAL},
    {0x0648, 0x0648, JOINING_TYPE_RIGHT},
    {0x0649, 0x064A, JOINING_TYPE_DUAL},
    {0x064B, 0x065F, JOINING_TYPE_TRANSPARENT},
    {0x0660, 0x066D, JOINING_TYPE_NONE},
    {0x066E, 0x066F, JOINING_TYPE_DUAL},
    {0x0670, 0x0670, JOINING_TYPE_TRANSPARENT},
    {0x0671, 0x0673, JOINING_TYPE_RIGHT},
    {0x0674, 0x0674, JOINING_TYPE_NONE},
    {0x0675, 0x0677, JOINING_TYPE_RIGHT},
    {0x0678, 0x0687, JOINING_TYPE_DUAL},
    {0x0688, 0x0699, JOINING_TYPE_RIGHT},
    {0x069A, 0x06BF, JOINING_TYPE_DUAL},
    {0x06C0, 0x06C0, JOINING_TYPE_RIGHT},
    {0x06C1, 0x06C2, JOINING_TYPE_DUAL},
    {0x06C3, 0x06CB, JOINING_TYPE_RIGHT},
    {0x06CC, 0x06CC, JOINING_TYPE_DUAL},
    {0x06CD, 0x06CD, JOINING_TYPE_RIGHT},
    {0x06CE, 0x06CE, JOINING_TYPE_DUAL},
    {0x06CF, 0x06CF, JOINING_TYPE_RIGHT},
    {0x06D0, 0x06D1, JOINING_TYPE_DUAL},
    {0x06D2, 0x06D3, JOINING_TYPE_RIGHT},
    {0x06D4, 0x06D4, JOINING_TYPE_NONE},
    {0x06D5, 0x06D5, JOINING_TYPE_RIGHT},
    {0x06D6, 0x06DC, JOINING_TYPE_TRANSPARENT},
    {0x06DD, 0x06DD, JOINING_TYPE_NONE},
    {0x06DF, 0x06E4, JOINING_TYPE_TRANSPARENT},
    {0x06E5, 0x06E6, JOINING_TYPE_NONE},
    {0x06E7, 0x06E8, JOINING_TYPE_TRANSPARENT},
    {0x06E9, 0x06E9, JOINING_TYPE_NONE},
    {0x06EA, 0x06ED, JOINING_TYPE_TRANSPARENT},
    {0x06EE, 0x06EF, JOINING_TYPE_RIGHT},
    {0x06FA, 0x06FC, JOINING_TYPE_DUAL},
    {0x06FF, 0x06FF, JOINING_TYPE_DUAL},
    {0, 0, 0}
};

static ARABIC_FORM     arabic_form_table_b[] = {
    /* code,  isolated, final, initial, medial */
    {0x0621, 0xFE80, 0xFE80, 0xFE80, 0xFE80},   /* HAMZA 0x0621*/
    {0x0622, 0xFE81, 0xFE82, 0xFE81, 0xFE82},   /* ALEF WITH MADDA ABOVE 0x622 */
    {0x0623, 0xFE83, 0xFE84, 0xFE83, 0xFE84},   /* ALEF WITH HAMZA ABOVE 0x0623*/
    {0x0624, 0xFE85, 0xFE86, 0xFE85, 0xFE86},   /* WAW WITH HAMZA ABOVE 0x0624 */
    {0x0625, 0xFE87, 0xFE88, 0xFE87, 0xFE88},   /* ALEF WITH HAMZA BELOW 0x0625*/
    {0x0626, 0xFE89, 0xFE8A, 0xFE8B, 0xFE8C},   /* YEH WITH HAMZA ABOVE 0x0626*/
    {0x0627, 0xFE8D, 0xFE8E, 0xFE8D, 0xFE8E},   /* ALEF 0x0627*/
    {0x0628, 0xFE8F, 0xFE90, 0xFE91, 0xFE92},   /* BEH 0x0628*/
    {0x0629, 0xFE93, 0xFE94, 0xFE93, 0xFE94},   /* TEH MARBUTA 0x0629*/
    {0x062A, 0xFE95, 0xFE96, 0xFE97, 0xFE98},   /* TEH 0x062A*/
    {0x062B, 0xFE99, 0xFE9A, 0xFE9B, 0xFE9C},   /* THEH 0x062B*/
    {0x062C, 0xFE9D, 0xFE9E, 0xFE9F, 0xFEA0},   /* JEEM 0x062C*/
    {0x062D, 0xFEA1, 0xFEA2, 0xFEA3, 0xFEA4},   /* HAH 0x062D*/
    {0x062E, 0xFEA5, 0xFEA6, 0xFEA7, 0xFEA8},   /* KHAH 0x062E*/
    {0x062F, 0xFEA9, 0xFEAA, 0xFEA9, 0xFEAA},   /* DAL 0x062F*/
    {0x0630, 0xFEAB, 0xFEAC, 0xFEAB, 0xFEAC},   /* THAL0x0630 */
    {0x0631, 0xFEAD, 0xFEAE, 0xFEAD, 0xFEAE},   /* RAA 0x0631*/
    {0x0632, 0xFEAF, 0xFEB0, 0xFEAF, 0xFEB0},   /* ZAIN 0x0632*/
    {0x0633, 0xFEB1, 0xFEB2, 0xFEB3, 0xFEB4},   /* SEEN 0x0633*/
    {0x0634, 0xFEB5, 0xFEB6, 0xFEB7, 0xFEB8},   /* SHEEN 0x0634*/
    {0x0635, 0xFEB9, 0xFEBA, 0xFEBB, 0xFEBC},   /* SAD 0x0635*/
    {0x0636, 0xFEBD, 0xFEBE, 0xFEBF, 0xFEC0},   /* DAD 0x0636*/
    {0x0637, 0xFEC1, 0xFEC2, 0xFEC3, 0xFEC4},   /* TAH 0x0637*/
    {0x0638, 0xFEC5, 0xFEC6, 0xFEC7, 0xFEC8},   /* ZAH 0x0638*/
    {0x0639, 0xFEC9, 0xFECA, 0xFECB, 0xFECC},   /* AIN 0x0639*/
    {0x063A, 0xFECD, 0xFECE, 0xFECF, 0xFED0},   /* GHAIN 0x063A*/
    {0x0641, 0xFED1, 0xFED2, 0xFED3, 0xFED4},   /* FAA 0x0641*/
    {0x0642, 0xFED5, 0xFED6, 0xFED7, 0xFED8},   /* QAF 0x0642*/
    {0x0643, 0xFED9, 0xFEDA, 0xFEDB, 0xFEDC},   /* KAF 0x0643*/
    {0x0644, 0xFEDD, 0xFEDE, 0xFEDF, 0xFEE0},   /* LAM 0x0644*/
    {0x0645, 0xFEE1, 0xFEE2, 0xFEE3, 0xFEE4},   /* MEEM 0x0645*/
    {0x0646, 0xFEE5, 0xFEE6, 0xFEE7, 0xFEE8},   /* NOON 0x0646*/
    {0x0647, 0xFEE9, 0xFEEA, 0xFEEB, 0xFEEC},   /* HEH 0x0647*/
    {0x0648, 0xFEED, 0xFEEE, 0xFEED, 0xFEEE},   /* WAW 0x0648*/
    {0x0649, 0xFEEF, 0xFEF0, 0xFBE8, 0xFBE9},   /* ALEF MAKSURA 0x0649*/
    {0x064A, 0xFEF1, 0xFEF2, 0xFEF3, 0xFEF4},   /* YEH 0x064A*/
    {0, 0, 0, 0, 0}
};

static ARABIC_FORM     arabic_form_table_a[] = {
    {0x0671, 0xFB50, 0xFB51, 0xFB50, 0xFB51}, /* Alef Wasla */
    {0x0679, 0xFB66, 0xFB67, 0xFB68, 0xFB89}, /* Tteh */
    {0x067A, 0xFB5E, 0xFB5F, 0xFB60, 0xFB61}, /* Tteheh */
    {0x067B, 0xFB52, 0xFB53, 0xFB54, 0xFB55}, /* Beeh */
    {0x067E, 0xFB56, 0xFB57, 0xFB58, 0xFB59}, /* Peh */
    {0x067F, 0xFB62, 0xFB63, 0xFB64, 0xFB65}, /* Teheh */
    {0x0680, 0xFB5A, 0xFB5B, 0xFB5C, 0xFB5D}, /* Beheh */
    {0x0683, 0xFB76, 0xFB77, 0xFB78, 0xFB79}, /* Nyeh */
    {0x0684, 0xFB72, 0xFB73, 0xFB74, 0xFB75}, /* Dyeh */
    {0x0686, 0xFB7A, 0xFB7B, 0xFB7C, 0xFB7D}, /* Tcheh */
    {0x0688, 0xFB88, 0xFB89, 0xFB88, 0xFB89}, /* Ddal */
    {0x0687, 0xFB7E, 0xFB7F, 0xFB80, 0xFB81}, /* Tcheheh */
    {0x068D, 0xFB82, 0xFB83, 0xFB82, 0xFB83}, /* Ddahal */
    {0x068E, 0xFB86, 0xFB87, 0xFB86, 0xFB87}, /* Dul */
    {0x0691, 0xFB8C, 0xFB8D, 0xFB8C, 0xFB8D}, /* Rreh */
    {0x0698, 0xFB8A, 0xFB8B, 0xFB8A, 0xFB8B}, /* Jeh */
    {0x06A4, 0xFB6A, 0xFB6B, 0xFB6C, 0xFB6D}, /* Veh */
    {0x06A6, 0xFB6E, 0xFB6F, 0xFB70, 0xFB71}, /* Peheh */
    {0x06A9, 0xFB8E, 0xFB8F, 0xFB90, 0xFB91}, /* Keheh */
    {0x06AD, 0xFBD3, 0xFBD4, 0xFBD5, 0xFBD6}, /* Ng */
    {0x06AF, 0xFB92, 0xFB93, 0xFB94, 0xFB95}, /* Gaf */
    {0x06B1, 0xFB9A, 0xFB9B, 0xFB9C, 0xFB9D}, /* Ngoeh */
    {0x06B3, 0xFB96, 0xFB97, 0xFB98, 0xFB99}, /* Gueh */
    {0x06BA, 0xFB9E, 0xFB9F, 0xFB9E, 0xFB9F}, /* Noon Ghunna */
    {0x06BB, 0xFBA0, 0xFBA1, 0xFBA2, 0xFBA3}, /* Rnoon */
    {0x06BE, 0xFBAA, 0xFBAB, 0xFBAC, 0xFBAD}, /* Heh Doachashmee */
    {0x06C1, 0xFBA6, 0xFBA7, 0xFBA8, 0xFBA9}, /* Heh Goal */
    {0x06C5, 0xFBE0, 0xFBE1, 0xFBE0, 0xFBE1}, /* Kirghiz Oe */
    {0x06C6, 0xFBD9, 0xFBDA, 0xFBD9, 0xFBDA}, /* Oe */
    {0x06C7, 0xFBD7, 0xFBD8, 0xFBD7, 0xFBD8}, /* U */
    {0x06C8, 0xFBDB, 0xFBDC, 0xFBDB, 0xFBDC}, /* Yu */
    {0x06C9, 0xFBE2, 0xFBE3, 0xFBE2, 0xFBE3}, /* Kirghiz Yu */
    {0x06CC, 0xFBFC, 0xFBFD, 0xFBFE, 0xFBFF}, /* Farsi Yeh */
    {0x06D0, 0xFBE4, 0xFBE5, 0xFBE6, 0xFBE7}, /* E */
    {0x06CB, 0xFBDE, 0xFBDF, 0xFBDE, 0xFBDF}, /* Ve */
    {0x06D2, 0xFBAE, 0xFBAF, 0xFBAE, 0xFBAF}, /* Yeh Barree */
    {0x06D3, 0xFBB0, 0xFBB1, 0xFBB0, 0xFBB1}  /* Yeh Barree with Hamza Above */
};

static ARABIC_LIGATURE arabic_ligature_table[] =
{
    /*alef, lam, (lam-alef) */
    {0xFE82, 0xFEDF, 0xFEF5},   /* Arabic Ligature Lam with Alef with Madda Above Isolated Form */
    {0xFE82, 0xFEE0, 0xFEF6},   /* Arabic Ligature Lam with Alef with Madda Above Final Form */
    {0xFE84, 0xFEDF, 0xFEF7},   /* Arabic Ligature Lam with Alef with Hamza Above Isolated Form */
    {0xFE84, 0xFEE0, 0xFEF8},   /* Arabic Ligature Lam with Alef with Hamza Above Final Form */
    {0xFE88, 0xFEDF, 0xFEF9},   /* Arabic Ligature Lam with Alef with Hamza Below Isolated Form */
    {0xFE88, 0xFEE0, 0xFEFA},   /* Arabic Ligature Lam with Alef with Hamza Below Final Form */
    {0xFE8E, 0xFEDF, 0xFEFB},   /* Arabic Ligature Lam with Alef Isolated Form */
    {0xFE8E, 0xFEE0, 0xFEFC},   /* Arabic Ligature Lam with Alef Final Form */
    {0, 0, 0}
};

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_arabic_joining_type_get                            */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns join type of sepecified unicode.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    code_point                            Code point                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    joining type                                                        */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_arabic_shaping                                     */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_arabic_joining_type_get(ULONG code_point)
{
ARABIC_SHAPING *shaping = arabic_shaping_table;

    if (code_point >= 0x0600 && code_point <= 0x06FF)
    {
        /* Arabic. */
        while (shaping->range_start)
        {
            if ((code_point >= shaping->range_start) &&
                (code_point <= shaping->range_end))
            {
                return shaping->joining_type;
            }
            shaping++;
        }
    }

    return JOINING_TYPE_NONE;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_arabic_form_get                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function gets Arabic shape form.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    code_point                            Code point                    */
/*    form_type                             Joining type                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_arabic_shaping                                     */
/*                                                                        */
/**************************************************************************/
static ULONG _gx_utility_bidi_arabic_form_get(ULONG code_point, UINT form_type)
{
ARABIC_FORM *entry = GX_NULL;

    if ((code_point) >= 0x0621 && (code_point <= 0x064A))
    {
        entry = arabic_form_table_b;
    }
    else if (code_point >= 0x0671 && code_point <= 0x06D3)
    {
        entry = arabic_form_table_a;
    }

    if (entry)
    {
        while (entry->code_point)
        {
            if (entry->code_point == code_point)
            {
                switch (form_type)
                {
                case ARABIC_FORM_ISOLATED:
                    return entry->isolated;

                case ARABIC_FORM_INITIAL:
                    return entry->initial;

                case ARABIC_FORM_FINAL:
                    return entry->final;

                case ARABIC_FORM_MEDIAL:
                    return entry->medial;
                }
            }

            entry++;
        }
    }

    return code_point;
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_arabic_ligature_get                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves Arabic ligature.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    alef                                  Code of Arabic letter alef    */
/*    lam                                   Code of Arabic letter lam     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_arabic_shaping                                     */
/*                                                                        */
/**************************************************************************/
static ULONG _gx_utility_bidi_arabic_ligature_get(ULONG alef, ULONG lam)
{
ARABIC_LIGATURE *entry = arabic_ligature_table;

    while (entry->ligature)
    {
        if (entry->alef == alef && entry->lam == lam)
        {
            return entry->ligature;
        }
        entry++;
    }

    return 0;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_right_joining_causing_test                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function tests if a join type is right joining causing.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    joining_type                          Joining type                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_arabic_shaping                                     */
/*                                                                        */
/**************************************************************************/
static GX_BOOL _gx_utility_bidi_right_joining_causing_test(UINT joining_type)
{
    switch (joining_type)
    {
    case JOINING_TYPE_DUAL:
    case JOINING_TYPE_LEFT:
    case JOINING_TYPE_CAUSING:
        return GX_TRUE;
    }

    return GX_FALSE;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_left_joining_causing_test                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function tests if a joing type is left joining causing.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    joinint_type                          Joining type                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_arabic_shaping                                     */
/*                                                                        */
/**************************************************************************/
static GX_BOOL _gx_utility_bidi_left_joining_causing_test(UINT joining_type)
{
    switch (joining_type)
    {
    case JOINING_TYPE_DUAL:
    case JOINING_TYPE_RIGHT:
    case JOINING_TYPE_CAUSING:
        return GX_TRUE;
    }

    return GX_FALSE;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_arabic_test                                        */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function tests is the specified strings contains Arabic.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_arabic_shaping                                     */
/*                                                                        */
/**************************************************************************/
static GX_BOOL _gx_utility_bidi_arabic_test(GX_BIDI_CONTEXT *context)
{
GX_BIDI_UNIT *unit = context->unit_list;
INT           index;

    for (index = 0; index < context->unit_count; index++)
    {
        if ((unit->code >= 0x0600) && (unit->code <= 0x06FF))
        {
            return GX_TRUE;
        }

        unit++;
    }

    return GX_FALSE;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_arabic_shaping                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function shapes Arabic glyphs according to logical order.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_bidi_arabic_shaping(GX_BIDI_CONTEXT *context)
{
ARABIC_UNIT  *arabic_unit;
ARABIC_UNIT  *pre_unit;
ARABIC_UNIT  *unit;
ARABIC_UNIT  *follow_unit;
ULONG         ligature;
INT           index;
GX_BIDI_UNIT *bidi_unit;
GX_BIDI_UNIT *pre_bidi_unit;
GX_UBYTE      utf8[6];
UINT          glyph_len;
INT           increment;
UINT          form_type = 0;
INT           buffer_size;

    /* Test Arabic glyph. */
    if (_gx_utility_bidi_arabic_test(context) == GX_FALSE)
    {
        /* Nothing to change. */
        return GX_SUCCESS;
    }

    buffer_size = (context->unit_count + 1) * (INT)sizeof(ARABIC_UNIT);

    /* Allocate memory for Arabic shaping. */
    arabic_unit = (ARABIC_UNIT *)_gx_system_memory_allocator((UINT)buffer_size);

    if (arabic_unit == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    /* Initiate Arabic shaping information list. */
    for (index = 0; index < context->unit_count; index++)
    {
        arabic_unit[index].code_point = context->unit_list[index].code;
        arabic_unit[index].shape_form = arabic_unit[index].code_point;
        arabic_unit[index].joining_type = _gx_utility_bidi_arabic_joining_type_get(arabic_unit[index].code_point);
    }

    arabic_unit[index].code_point = 0;
    arabic_unit[index].joining_type = 0;

    pre_unit = GX_NULL;
    unit = arabic_unit;
    bidi_unit = context->unit_list;

    /* Start shaping. */
    while (unit->code_point)
    {
        increment = 1;

        if (unit->joining_type == JOINING_TYPE_TRANSPARENT)
        {
            /* Rule 1: Transparent characters do not affect the joining behavior of base characters. */
            unit++;
        }
        else
        {
            follow_unit = unit + 1;

            /* Find next non-transparent character. */
            while (follow_unit->code_point && follow_unit->joining_type == JOINING_TYPE_TRANSPARENT)
            {
                increment++;
                follow_unit++;
            }

            if (unit->joining_type == JOINING_TYPE_RIGHT && pre_unit && _gx_utility_bidi_right_joining_causing_test(pre_unit->joining_type))
            {
                /* Rule 2: a right-joining character x that has a right joining-causing character on the right(preceding) will
                   adopt the form x-right. */
                form_type = ARABIC_FORM_FINAL;
            }
            else if (unit->joining_type == JOINING_TYPE_LEFT && _gx_utility_bidi_left_joining_causing_test(follow_unit->joining_type))
            {
                /* Rule 3: a left-joining character x that has a left joining-causing character on the left(following) will
                   adopt the form x-left. */
                form_type = ARABIC_FORM_INITIAL;
            }
            else if (unit->joining_type == JOINING_TYPE_DUAL &&
                     pre_unit && _gx_utility_bidi_right_joining_causing_test(pre_unit->joining_type) &&
                     _gx_utility_bidi_left_joining_causing_test(follow_unit->joining_type))
            {
                /* Rule 4: a dual-joining character x that has a right joining-causing character on the right(preceding) and
                   a left join-causing character on the left(following) will adopt the form x-medial. */
                form_type = ARABIC_FORM_MEDIAL;
            }
            else if (unit->joining_type == JOINING_TYPE_DUAL &&
                     pre_unit && _gx_utility_bidi_right_joining_causing_test(pre_unit->joining_type) &&
                     !_gx_utility_bidi_left_joining_causing_test(follow_unit->joining_type))
            {
                /* Rule 5: a dual-joining character x that has a right join-causing character on the right(preceding) and
                   no left join-causing character on the left(following) will adopt the form x-right. */
                form_type = ARABIC_FORM_FINAL;
            }
            else if (unit->joining_type == JOINING_TYPE_DUAL &&
                     _gx_utility_bidi_left_joining_causing_test(follow_unit->joining_type) &&
                     ((pre_unit == GX_NULL) || !_gx_utility_bidi_right_joining_causing_test(pre_unit->joining_type)))
            {
                /* Rule 6: a dual-joining character x that has left joining-causing character on the left(following) and
                   no right join-causing character on the right(preceding) will adopt the form x-left. */
                form_type = ARABIC_FORM_INITIAL;
            }

            if (form_type)
            {
                unit->shape_form = _gx_utility_bidi_arabic_form_get(unit->code_point, form_type);
                form_type = 0;

                if (pre_unit)
                {
                    /* Any sequence with alef-right on the left(following) and lam-medial on the right(preceding) will form
                       the ligature (lam-alef)-right. */
                    ligature = _gx_utility_bidi_arabic_ligature_get(unit->shape_form, pre_unit->shape_form);
                    if (ligature)
                    {
                        pre_unit->shape_form = ligature;
                        pre_bidi_unit->code = ligature;
                        unit->shape_form = 0;
                    }
                }

                bidi_unit->code = unit->shape_form;
            }

            pre_bidi_unit = bidi_unit;
            pre_unit = unit;
            unit = follow_unit;
        }

        bidi_unit += increment;
    }

    /* Calculate total utf8 size for shaped glyphs. */
    context->reordered_utf8_size = 0;
    bidi_unit = context->unit_list;
    for (index = 0; index < context->unit_count; index++)
    {
        if (bidi_unit->code)
        {
            _gx_utility_unicode_to_utf8(bidi_unit->code, utf8, &glyph_len);
            context->reordered_utf8_size += glyph_len;
        }

        bidi_unit++;
    }

    _gx_system_memory_free(arabic_unit);

    return GX_SUCCESS;
}
#endif
#endif

/**************************************************************************/

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
static const GX_BIDI_BRACKET_PAIR _gx_bidi_bracket_pair[] =
{
    {0x0028, 0x0029},    /* PARENTHESIS */
    {0x005B, 0x005D},    /* SQUARE BRACKET*/
    {0x007B, 0x007D},    /* CURLY BRACKET*/
    {0x0F3A, 0x0F3B},    /* TIBETAN MARK GUG RTAGS GYON*/
    {0x0F3C, 0x0F3D},    /* TIBETAN MARK ANG KHANG GYON*/
    {0x169B, 0x169C},    /* OGHAM FEATHER MARK*/
    {0x2045, 0x2046},    /* SQUARE BRACKET WITH QUILL*/
    {0x207D, 0x207E},    /* SUPERSCRIPT PARENTHESIS*/
    {0x208D, 0x208E},    /* SUBSCRIPT PARENTHESIS*/
    {0x2308, 0x2309},    /* CEILING*/
    {0x230A, 0x230B},    /* FLOOR*/
    {0x2329, 0x232A},    /* POINTING ANGLE BRACKET*/
    {0x2768, 0x2769},    /* MEDIUM PARENTHESIS ORNAMENT*/
    {0x276A, 0x276B},    /* MEDIUM FLATTENED PARENTHESIS ORNAMENT*/
    {0x276C, 0x276D},    /* MEDIUM - POINTING ANGLE BRACKET ORNAMENT*/
    {0x276E, 0x276F},    /* HEAVY - POINTING ANGLE QUOTATION MARK ORNAMENT*/
    {0x2770, 0x2771},    /* HEAVY - POINTING ANGLE BRACKET ORNAMENT*/
    {0x2772, 0x2773},    /* LIGHT TORTOISE SHELL BRACKET ORNAMENT*/
    {0x2774, 0x2775},    /* MEDIUM CURLY BRACKET ORNAMENT*/
    {0x27C5, 0x27C6},    /* LEFT S - SHAPED BAG DELIMITER*/
    {0x27E6, 0x27E7},    /* MATHEMATICAL WHITE SQUARE BRACKET*/
    {0x27E8, 0x27E9},    /* MATHEMATICAL ANGLE BRACKET*/
    {0x27EA, 0x27EB},    /* MATHEMATICAL DOUBLE ANGLE BRACKET*/
    {0x27EC, 0x27ED},    /* MATHEMATICAL WHITE TORTOISE SHELL BRACKET*/
    {0x27EE, 0x27EF},    /* MATHEMATICAL FLATTENED PARENTHESIS*/
    {0x2983, 0x2984},    /* WHITE CURLY BRACKET*/
    {0x2985, 0x2986},    /* WHITE PARENTHESIS*/
    {0x2987, 0x2988},    /* Z NOTATION IMAGE BRACKET*/
    {0x2989, 0x298A},    /* Z NOTATION BINDING BRACKET*/
    {0x298B, 0x298C},    /* SQUARE BRACKET WITH UNDERBAR*/
    {0x298D, 0x2990},    /* SQUARE BRACKET WITH TICK IN TOP CORNER*/
    {0x298F, 0x298E},    /* SQUARE BRACKET WITH TICK IN BOTTOM CORNER*/
    {0x2991, 0x2992},    /* ANGLE BRACKET WITH DOT*/
    {0x2993, 0x2994},    /* ARC LESS - THAN BRACKET*/
    {0x2995, 0x2996},    /* DOUBLE ARC GREATER - THAN BRACKET*/
    {0x2997, 0x2998},    /* BLACK TORTOISE SHELL BRACKET*/
    {0x29D8, 0x29D9},    /* WIGGLY FENCE*/
    {0x29DA, 0x29DB},    /* DOUBLE WIGGLY FENCE*/
    {0x29FC, 0x29FD},    /* POINTING CURVED ANGLE BRACKET*/
    {0x2E22, 0x2E23},    /* TOP HALF BRACKET*/
    {0x2E24, 0x2E25},    /* BOTTOM HALF BRACKET*/
    {0x2E26, 0x2E27},    /* SIDEWAYS U BRACKET*/
    {0x2E28, 0x2E29},    /* DOUBLE PARENTHESIS*/
    {0x3008, 0x3009},    /* ANGLE BRACKET*/
    {0x300A, 0x300B},    /* DOUBLE ANGLE BRACKET*/
    {0x300C, 0x300D},    /* CORNER BRACKET*/
    {0x300E, 0x300F},    /* WHITE CORNER BRACKET*/
    {0x3010, 0x3011},    /* BLACK LENTICULAR BRACKET*/
    {0x3014, 0x3015},    /* TORTOISE SHELL BRACKET*/
    {0x3016, 0x3017},    /* WHITE LENTICULAR BRACKET*/
    {0x3018, 0x3019},    /* WHITE TORTOISE SHELL BRACKET*/
    {0x301A, 0x301B},    /* WHITE SQUARE BRACKET*/
    {0xFE59, 0xFE5A},    /* SMALL PARENTHESIS*/
    {0xFE5B, 0xFE5C},    /* SMALL CURLY BRACKET*/
    {0xFE5D, 0xFE5E},    /* SMALL TORTOISE SHELL BRACKET*/
    {0xFF08, 0xFF09},    /* FULLWIDTH PARENTHESIS*/
    {0xFF3B, 0xFF3D},    /* FULLWIDTH SQUARE BRACKET*/
    {0xFF5B, 0xFF5D},    /* FULLWIDTH CURLY BRACKET*/
    {0xFF5F, 0xFF60},    /* FULLWIDTH WHITE PARENTHESIS*/
    {0xFF62, 0xFF63},    /* HALFWIDTH CORNER BRACKET*/
    {0, 0}
};

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_bracket_pair_get                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function finds the bracked pair that contain the specified     */
/*    code point.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    code                                  Code point whose bracket pair */
/*                                            is retrieved                */
/*    bracket_pair                          Retrieved bracket pair        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_bidi_bracket_pair_get(ULONG code, GX_BIDI_BRACKET_PAIR *bracket_pair)
{
const GX_BIDI_BRACKET_PAIR *entry = _gx_bidi_bracket_pair;

    memset(bracket_pair, 0, sizeof(GX_BIDI_BRACKET_PAIR));

    while (entry->open)
    {
        if (code == entry->open ||
            code == entry->close)
        {
            *bracket_pair = *entry;
            break;
        }
        entry++;
    }

    return GX_SUCCESS;
}
#endif

/**************************************************************************/

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
static const GX_BIDI_CHARACTER_INFO _gx_bidi_character_types[] =
{
    {0x0041, 0x005A, GX_BIDI_CHARACTER_TYPE_L},
    {0x0061, 0x007A, GX_BIDI_CHARACTER_TYPE_L},
    {0x00AA, 0x00AA, GX_BIDI_CHARACTER_TYPE_L},
    {0x00B5, 0x00B5, GX_BIDI_CHARACTER_TYPE_L},
    {0x00BA, 0x00BA, GX_BIDI_CHARACTER_TYPE_L},
    {0x00C0, 0x00D6, GX_BIDI_CHARACTER_TYPE_L},
    {0x00D8, 0x00F6, GX_BIDI_CHARACTER_TYPE_L},
    {0x00F8, 0x01BA, GX_BIDI_CHARACTER_TYPE_L},
    {0x01BB, 0x01BB, GX_BIDI_CHARACTER_TYPE_L},
    {0x01BC, 0x01BF, GX_BIDI_CHARACTER_TYPE_L},
    {0x01C0, 0x01C3, GX_BIDI_CHARACTER_TYPE_L},
    {0x01C4, 0x0293, GX_BIDI_CHARACTER_TYPE_L},
    {0x0294, 0x0294, GX_BIDI_CHARACTER_TYPE_L},
    {0x0295, 0x02AF, GX_BIDI_CHARACTER_TYPE_L},
    {0x02B0, 0x02B8, GX_BIDI_CHARACTER_TYPE_L},
    {0x02BB, 0x02C1, GX_BIDI_CHARACTER_TYPE_L},
    {0x02D0, 0x02D1, GX_BIDI_CHARACTER_TYPE_L},
    {0x02E0, 0x02E4, GX_BIDI_CHARACTER_TYPE_L},
    {0x02EE, 0x02EE, GX_BIDI_CHARACTER_TYPE_L},
    {0x0370, 0x0373, GX_BIDI_CHARACTER_TYPE_L},
    {0x0376, 0x0377, GX_BIDI_CHARACTER_TYPE_L},
    {0x037A, 0x037A, GX_BIDI_CHARACTER_TYPE_L},
    {0x037B, 0x037D, GX_BIDI_CHARACTER_TYPE_L},
    {0x0386, 0x0386, GX_BIDI_CHARACTER_TYPE_L},
    {0x0388, 0x038A, GX_BIDI_CHARACTER_TYPE_L},
    {0x038C, 0x038C, GX_BIDI_CHARACTER_TYPE_L},
    {0x038E, 0x03A1, GX_BIDI_CHARACTER_TYPE_L},
    {0x03A3, 0x03F5, GX_BIDI_CHARACTER_TYPE_L},
    {0x03F7, 0x0481, GX_BIDI_CHARACTER_TYPE_L},
    {0x0482, 0x0482, GX_BIDI_CHARACTER_TYPE_L},
    {0x048A, 0x0523, GX_BIDI_CHARACTER_TYPE_L},
    {0x0531, 0x0556, GX_BIDI_CHARACTER_TYPE_L},
    {0x0559, 0x0559, GX_BIDI_CHARACTER_TYPE_L},
    {0x055A, 0x055F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0561, 0x0587, GX_BIDI_CHARACTER_TYPE_L},
    {0x0589, 0x0589, GX_BIDI_CHARACTER_TYPE_L},
    {0x0903, 0x0903, GX_BIDI_CHARACTER_TYPE_L},
    {0x0904, 0x0939, GX_BIDI_CHARACTER_TYPE_L},
    {0x093D, 0x093D, GX_BIDI_CHARACTER_TYPE_L},
    {0x093E, 0x0940, GX_BIDI_CHARACTER_TYPE_L},
    {0x0949, 0x094C, GX_BIDI_CHARACTER_TYPE_L},
    {0x0950, 0x0950, GX_BIDI_CHARACTER_TYPE_L},
    {0x0958, 0x0961, GX_BIDI_CHARACTER_TYPE_L},
    {0x0964, 0x0965, GX_BIDI_CHARACTER_TYPE_L},
    {0x0966, 0x096F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0970, 0x0970, GX_BIDI_CHARACTER_TYPE_L},
    {0x0971, 0x0971, GX_BIDI_CHARACTER_TYPE_L},
    {0x0972, 0x0972, GX_BIDI_CHARACTER_TYPE_L},
    {0x097B, 0x097F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0982, 0x0983, GX_BIDI_CHARACTER_TYPE_L},
    {0x0985, 0x098C, GX_BIDI_CHARACTER_TYPE_L},
    {0x098F, 0x0990, GX_BIDI_CHARACTER_TYPE_L},
    {0x0993, 0x09A8, GX_BIDI_CHARACTER_TYPE_L},
    {0x09AA, 0x09B0, GX_BIDI_CHARACTER_TYPE_L},
    {0x09B2, 0x09B2, GX_BIDI_CHARACTER_TYPE_L},
    {0x09B6, 0x09B9, GX_BIDI_CHARACTER_TYPE_L},
    {0x09BD, 0x09BD, GX_BIDI_CHARACTER_TYPE_L},
    {0x09BE, 0x09C0, GX_BIDI_CHARACTER_TYPE_L},
    {0x09C7, 0x09C8, GX_BIDI_CHARACTER_TYPE_L},
    {0x09CB, 0x09CC, GX_BIDI_CHARACTER_TYPE_L},
    {0x09CE, 0x09CE, GX_BIDI_CHARACTER_TYPE_L},
    {0x09D7, 0x09D7, GX_BIDI_CHARACTER_TYPE_L},
    {0x09DC, 0x09DD, GX_BIDI_CHARACTER_TYPE_L},
    {0x09DF, 0x09E1, GX_BIDI_CHARACTER_TYPE_L},
    {0x09E6, 0x09EF, GX_BIDI_CHARACTER_TYPE_L},
    {0x09F0, 0x09F1, GX_BIDI_CHARACTER_TYPE_L},
    {0x09F4, 0x09F9, GX_BIDI_CHARACTER_TYPE_L},
    {0x09FA, 0x09FA, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A03, 0x0A03, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A05, 0x0A0A, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A0F, 0x0A10, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A13, 0x0A28, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A2A, 0x0A30, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A32, 0x0A33, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A35, 0x0A36, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A38, 0x0A39, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A3E, 0x0A40, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A59, 0x0A5C, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A5E, 0x0A5E, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A66, 0x0A6F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A72, 0x0A74, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A83, 0x0A83, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A85, 0x0A8D, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A8F, 0x0A91, GX_BIDI_CHARACTER_TYPE_L},
    {0x0A93, 0x0AA8, GX_BIDI_CHARACTER_TYPE_L},
    {0x0AAA, 0x0AB0, GX_BIDI_CHARACTER_TYPE_L},
    {0x0AB2, 0x0AB3, GX_BIDI_CHARACTER_TYPE_L},
    {0x0AB5, 0x0AB9, GX_BIDI_CHARACTER_TYPE_L},
    {0x0ABD, 0x0ABD, GX_BIDI_CHARACTER_TYPE_L},
    {0x0ABE, 0x0AC0, GX_BIDI_CHARACTER_TYPE_L},
    {0x0AC9, 0x0AC9, GX_BIDI_CHARACTER_TYPE_L},
    {0x0ACB, 0x0ACC, GX_BIDI_CHARACTER_TYPE_L},
    {0x0AD0, 0x0AD0, GX_BIDI_CHARACTER_TYPE_L},
    {0x0AE0, 0x0AE1, GX_BIDI_CHARACTER_TYPE_L},
    {0x0AE6, 0x0AEF, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B02, 0x0B03, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B05, 0x0B0C, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B0F, 0x0B10, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B13, 0x0B28, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B2A, 0x0B30, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B32, 0x0B33, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B35, 0x0B39, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B3D, 0x0B3D, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B3E, 0x0B3E, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B40, 0x0B40, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B47, 0x0B48, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B4B, 0x0B4C, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B57, 0x0B57, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B5C, 0x0B5D, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B5F, 0x0B61, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B66, 0x0B6F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B70, 0x0B70, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B71, 0x0B71, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B83, 0x0B83, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B85, 0x0B8A, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B8E, 0x0B90, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B92, 0x0B95, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B99, 0x0B9A, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B9C, 0x0B9C, GX_BIDI_CHARACTER_TYPE_L},
    {0x0B9E, 0x0B9F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0BA3, 0x0BA4, GX_BIDI_CHARACTER_TYPE_L},
    {0x0BA8, 0x0BAA, GX_BIDI_CHARACTER_TYPE_L},
    {0x0BAE, 0x0BB9, GX_BIDI_CHARACTER_TYPE_L},
    {0x0BBE, 0x0BBF, GX_BIDI_CHARACTER_TYPE_L},
    {0x0BC1, 0x0BC2, GX_BIDI_CHARACTER_TYPE_L},
    {0x0BC6, 0x0BC8, GX_BIDI_CHARACTER_TYPE_L},
    {0x0BCA, 0x0BCC, GX_BIDI_CHARACTER_TYPE_L},
    {0x0BD0, 0x0BD0, GX_BIDI_CHARACTER_TYPE_L},
    {0x0BD7, 0x0BD7, GX_BIDI_CHARACTER_TYPE_L},
    {0x0BE6, 0x0BEF, GX_BIDI_CHARACTER_TYPE_L},
    {0x0BF0, 0x0BF2, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C01, 0x0C03, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C05, 0x0C0C, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C0E, 0x0C10, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C12, 0x0C28, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C2A, 0x0C33, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C35, 0x0C39, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C3D, 0x0C3D, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C41, 0x0C44, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C58, 0x0C59, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C60, 0x0C61, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C66, 0x0C6F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C7F, 0x0C7F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C82, 0x0C83, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C85, 0x0C8C, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C8E, 0x0C90, GX_BIDI_CHARACTER_TYPE_L},
    {0x0C92, 0x0CA8, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CAA, 0x0CB3, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CB5, 0x0CB9, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CBD, 0x0CBD, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CBE, 0x0CBE, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CBF, 0x0CBF, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CC0, 0x0CC4, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CC6, 0x0CC6, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CC7, 0x0CC8, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CCA, 0x0CCB, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CD5, 0x0CD6, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CDE, 0x0CDE, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CE0, 0x0CE1, GX_BIDI_CHARACTER_TYPE_L},
    {0x0CE6, 0x0CEF, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D02, 0x0D03, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D05, 0x0D0C, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D0E, 0x0D10, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D12, 0x0D28, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D2A, 0x0D39, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D3D, 0x0D3D, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D3E, 0x0D40, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D46, 0x0D48, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D4A, 0x0D4C, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D57, 0x0D57, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D60, 0x0D61, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D66, 0x0D6F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D70, 0x0D75, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D79, 0x0D79, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D7A, 0x0D7F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D82, 0x0D83, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D85, 0x0D96, GX_BIDI_CHARACTER_TYPE_L},
    {0x0D9A, 0x0DB1, GX_BIDI_CHARACTER_TYPE_L},
    {0x0DB3, 0x0DBB, GX_BIDI_CHARACTER_TYPE_L},
    {0x0DBD, 0x0DBD, GX_BIDI_CHARACTER_TYPE_L},
    {0x0DC0, 0x0DC6, GX_BIDI_CHARACTER_TYPE_L},
    {0x0DCF, 0x0DD1, GX_BIDI_CHARACTER_TYPE_L},
    {0x0DD8, 0x0DDF, GX_BIDI_CHARACTER_TYPE_L},
    {0x0DF2, 0x0DF3, GX_BIDI_CHARACTER_TYPE_L},
    {0x0DF4, 0x0DF4, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E01, 0x0E30, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E32, 0x0E33, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E40, 0x0E45, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E46, 0x0E46, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E4F, 0x0E4F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E50, 0x0E59, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E5A, 0x0E5B, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E81, 0x0E82, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E84, 0x0E84, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E87, 0x0E88, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E8A, 0x0E8A, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E8D, 0x0E8D, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E94, 0x0E97, GX_BIDI_CHARACTER_TYPE_L},
    {0x0E99, 0x0E9F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0EA1, 0x0EA3, GX_BIDI_CHARACTER_TYPE_L},
    {0x0EA5, 0x0EA5, GX_BIDI_CHARACTER_TYPE_L},
    {0x0EA7, 0x0EA7, GX_BIDI_CHARACTER_TYPE_L},
    {0x0EAA, 0x0EAB, GX_BIDI_CHARACTER_TYPE_L},
    {0x0EAD, 0x0EB0, GX_BIDI_CHARACTER_TYPE_L},
    {0x0EB2, 0x0EB3, GX_BIDI_CHARACTER_TYPE_L},
    {0x0EBD, 0x0EBD, GX_BIDI_CHARACTER_TYPE_L},
    {0x0EC0, 0x0EC4, GX_BIDI_CHARACTER_TYPE_L},
    {0x0EC6, 0x0EC6, GX_BIDI_CHARACTER_TYPE_L},
    {0x0ED0, 0x0ED9, GX_BIDI_CHARACTER_TYPE_L},
    {0x0EDC, 0x0EDD, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F00, 0x0F00, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F01, 0x0F03, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F04, 0x0F12, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F13, 0x0F17, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F1A, 0x0F1F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F20, 0x0F29, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F2A, 0x0F33, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F34, 0x0F34, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F36, 0x0F36, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F38, 0x0F38, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F3E, 0x0F3F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F40, 0x0F47, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F49, 0x0F6C, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F7F, 0x0F7F, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F85, 0x0F85, GX_BIDI_CHARACTER_TYPE_L},
    {0x0F88, 0x0F8B, GX_BIDI_CHARACTER_TYPE_L},
    {0x0FBE, 0x0FC5, GX_BIDI_CHARACTER_TYPE_L},
    {0x0FC7, 0x0FCC, GX_BIDI_CHARACTER_TYPE_L},
    {0x0FCE, 0x0FCF, GX_BIDI_CHARACTER_TYPE_L},
    {0x0FD0, 0x0FD4, GX_BIDI_CHARACTER_TYPE_L},
    {0x1000, 0x102A, GX_BIDI_CHARACTER_TYPE_L},
    {0x102B, 0x102C, GX_BIDI_CHARACTER_TYPE_L},
    {0x1031, 0x1031, GX_BIDI_CHARACTER_TYPE_L},
    {0x1038, 0x1038, GX_BIDI_CHARACTER_TYPE_L},
    {0x103B, 0x103C, GX_BIDI_CHARACTER_TYPE_L},
    {0x103F, 0x103F, GX_BIDI_CHARACTER_TYPE_L},
    {0x1040, 0x1049, GX_BIDI_CHARACTER_TYPE_L},
    {0x104A, 0x104F, GX_BIDI_CHARACTER_TYPE_L},
    {0x1050, 0x1055, GX_BIDI_CHARACTER_TYPE_L},
    {0x1056, 0x1057, GX_BIDI_CHARACTER_TYPE_L},
    {0x105A, 0x105D, GX_BIDI_CHARACTER_TYPE_L},
    {0x1061, 0x1061, GX_BIDI_CHARACTER_TYPE_L},
    {0x1062, 0x1064, GX_BIDI_CHARACTER_TYPE_L},
    {0x1065, 0x1066, GX_BIDI_CHARACTER_TYPE_L},
    {0x1067, 0x106D, GX_BIDI_CHARACTER_TYPE_L},
    {0x106E, 0x1070, GX_BIDI_CHARACTER_TYPE_L},
    {0x1075, 0x1081, GX_BIDI_CHARACTER_TYPE_L},
    {0x1083, 0x1084, GX_BIDI_CHARACTER_TYPE_L},
    {0x1087, 0x108C, GX_BIDI_CHARACTER_TYPE_L},
    {0x108E, 0x108E, GX_BIDI_CHARACTER_TYPE_L},
    {0x108F, 0x108F, GX_BIDI_CHARACTER_TYPE_L},
    {0x1090, 0x1099, GX_BIDI_CHARACTER_TYPE_L},
    {0x109E, 0x109F, GX_BIDI_CHARACTER_TYPE_L},
    {0x10A0, 0x10C5, GX_BIDI_CHARACTER_TYPE_L},
    {0x10D0, 0x10FA, GX_BIDI_CHARACTER_TYPE_L},
    {0x10FB, 0x10FB, GX_BIDI_CHARACTER_TYPE_L},
    {0x10FC, 0x10FC, GX_BIDI_CHARACTER_TYPE_L},
    {0x1100, 0x1159, GX_BIDI_CHARACTER_TYPE_L},
    {0x115F, 0x11A2, GX_BIDI_CHARACTER_TYPE_L},
    {0x11A8, 0x11F9, GX_BIDI_CHARACTER_TYPE_L},
    {0x1200, 0x1248, GX_BIDI_CHARACTER_TYPE_L},
    {0x124A, 0x124D, GX_BIDI_CHARACTER_TYPE_L},
    {0x1250, 0x1256, GX_BIDI_CHARACTER_TYPE_L},
    {0x1258, 0x1258, GX_BIDI_CHARACTER_TYPE_L},
    {0x125A, 0x125D, GX_BIDI_CHARACTER_TYPE_L},
    {0x1260, 0x1288, GX_BIDI_CHARACTER_TYPE_L},
    {0x128A, 0x128D, GX_BIDI_CHARACTER_TYPE_L},
    {0x1290, 0x12B0, GX_BIDI_CHARACTER_TYPE_L},
    {0x12B2, 0x12B5, GX_BIDI_CHARACTER_TYPE_L},
    {0x12B8, 0x12BE, GX_BIDI_CHARACTER_TYPE_L},
    {0x12C0, 0x12C0, GX_BIDI_CHARACTER_TYPE_L},
    {0x12C2, 0x12C5, GX_BIDI_CHARACTER_TYPE_L},
    {0x12C8, 0x12D6, GX_BIDI_CHARACTER_TYPE_L},
    {0x12D8, 0x1310, GX_BIDI_CHARACTER_TYPE_L},
    {0x1312, 0x1315, GX_BIDI_CHARACTER_TYPE_L},
    {0x1318, 0x135A, GX_BIDI_CHARACTER_TYPE_L},
    {0x1360, 0x1360, GX_BIDI_CHARACTER_TYPE_L},
    {0x1361, 0x1368, GX_BIDI_CHARACTER_TYPE_L},
    {0x1369, 0x137C, GX_BIDI_CHARACTER_TYPE_L},
    {0x1380, 0x138F, GX_BIDI_CHARACTER_TYPE_L},
    {0x13A0, 0x13F4, GX_BIDI_CHARACTER_TYPE_L},
    {0x1401, 0x166C, GX_BIDI_CHARACTER_TYPE_L},
    {0x166D, 0x166E, GX_BIDI_CHARACTER_TYPE_L},
    {0x166F, 0x1676, GX_BIDI_CHARACTER_TYPE_L},
    {0x1681, 0x169A, GX_BIDI_CHARACTER_TYPE_L},
    {0x16A0, 0x16EA, GX_BIDI_CHARACTER_TYPE_L},
    {0x16EB, 0x16ED, GX_BIDI_CHARACTER_TYPE_L},
    {0x16EE, 0x16F0, GX_BIDI_CHARACTER_TYPE_L},
    {0x1700, 0x170C, GX_BIDI_CHARACTER_TYPE_L},
    {0x170E, 0x1711, GX_BIDI_CHARACTER_TYPE_L},
    {0x1720, 0x1731, GX_BIDI_CHARACTER_TYPE_L},
    {0x1735, 0x1736, GX_BIDI_CHARACTER_TYPE_L},
    {0x1740, 0x1751, GX_BIDI_CHARACTER_TYPE_L},
    {0x1760, 0x176C, GX_BIDI_CHARACTER_TYPE_L},
    {0x176E, 0x1770, GX_BIDI_CHARACTER_TYPE_L},
    {0x1780, 0x17B3, GX_BIDI_CHARACTER_TYPE_L},
    {0x17B4, 0x17B5, GX_BIDI_CHARACTER_TYPE_L},
    {0x17B6, 0x17B6, GX_BIDI_CHARACTER_TYPE_L},
    {0x17BE, 0x17C5, GX_BIDI_CHARACTER_TYPE_L},
    {0x17C7, 0x17C8, GX_BIDI_CHARACTER_TYPE_L},
    {0x17D4, 0x17D6, GX_BIDI_CHARACTER_TYPE_L},
    {0x17D7, 0x17D7, GX_BIDI_CHARACTER_TYPE_L},
    {0x17D8, 0x17DA, GX_BIDI_CHARACTER_TYPE_L},
    {0x17DC, 0x17DC, GX_BIDI_CHARACTER_TYPE_L},
    {0x17E0, 0x17E9, GX_BIDI_CHARACTER_TYPE_L},
    {0x1810, 0x1819, GX_BIDI_CHARACTER_TYPE_L},
    {0x1820, 0x1842, GX_BIDI_CHARACTER_TYPE_L},
    {0x1843, 0x1843, GX_BIDI_CHARACTER_TYPE_L},
    {0x1844, 0x1877, GX_BIDI_CHARACTER_TYPE_L},
    {0x1880, 0x18A8, GX_BIDI_CHARACTER_TYPE_L},
    {0x18AA, 0x18AA, GX_BIDI_CHARACTER_TYPE_L},
    {0x1900, 0x191C, GX_BIDI_CHARACTER_TYPE_L},
    {0x1923, 0x1926, GX_BIDI_CHARACTER_TYPE_L},
    {0x1929, 0x192B, GX_BIDI_CHARACTER_TYPE_L},
    {0x1930, 0x1931, GX_BIDI_CHARACTER_TYPE_L},
    {0x1933, 0x1938, GX_BIDI_CHARACTER_TYPE_L},
    {0x1946, 0x194F, GX_BIDI_CHARACTER_TYPE_L},
    {0x1950, 0x196D, GX_BIDI_CHARACTER_TYPE_L},
    {0x1970, 0x1974, GX_BIDI_CHARACTER_TYPE_L},
    {0x1980, 0x19A9, GX_BIDI_CHARACTER_TYPE_L},
    {0x19B0, 0x19C0, GX_BIDI_CHARACTER_TYPE_L},
    {0x19C1, 0x19C7, GX_BIDI_CHARACTER_TYPE_L},
    {0x19C8, 0x19C9, GX_BIDI_CHARACTER_TYPE_L},
    {0x19D0, 0x19D9, GX_BIDI_CHARACTER_TYPE_L},
    {0x1A00, 0x1A16, GX_BIDI_CHARACTER_TYPE_L},
    {0x1A19, 0x1A1B, GX_BIDI_CHARACTER_TYPE_L},
    {0x1A1E, 0x1A1F, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B04, 0x1B04, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B05, 0x1B33, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B35, 0x1B35, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B3B, 0x1B3B, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B3D, 0x1B41, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B43, 0x1B44, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B45, 0x1B4B, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B50, 0x1B59, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B5A, 0x1B60, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B61, 0x1B6A, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B74, 0x1B7C, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B82, 0x1B82, GX_BIDI_CHARACTER_TYPE_L},
    {0x1B83, 0x1BA0, GX_BIDI_CHARACTER_TYPE_L},
    {0x1BA1, 0x1BA1, GX_BIDI_CHARACTER_TYPE_L},
    {0x1BA6, 0x1BA7, GX_BIDI_CHARACTER_TYPE_L},
    {0x1BAA, 0x1BAA, GX_BIDI_CHARACTER_TYPE_L},
    {0x1BAE, 0x1BAF, GX_BIDI_CHARACTER_TYPE_L},
    {0x1BB0, 0x1BB9, GX_BIDI_CHARACTER_TYPE_L},
    {0x1C00, 0x1C23, GX_BIDI_CHARACTER_TYPE_L},
    {0x1C24, 0x1C2B, GX_BIDI_CHARACTER_TYPE_L},
    {0x1C34, 0x1C35, GX_BIDI_CHARACTER_TYPE_L},
    {0x1C3B, 0x1C3F, GX_BIDI_CHARACTER_TYPE_L},
    {0x1C40, 0x1C49, GX_BIDI_CHARACTER_TYPE_L},
    {0x1C4D, 0x1C4F, GX_BIDI_CHARACTER_TYPE_L},
    {0x1C50, 0x1C59, GX_BIDI_CHARACTER_TYPE_L},
    {0x1C5A, 0x1C77, GX_BIDI_CHARACTER_TYPE_L},
    {0x1C78, 0x1C7D, GX_BIDI_CHARACTER_TYPE_L},
    {0x1C7E, 0x1C7F, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D00, 0x1D2B, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D2C, 0x1D61, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D62, 0x1D77, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D78, 0x1D78, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D79, 0x1D9A, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D9B, 0x1DBF, GX_BIDI_CHARACTER_TYPE_L},
    {0x1E00, 0x1F15, GX_BIDI_CHARACTER_TYPE_L},
    {0x1F18, 0x1F1D, GX_BIDI_CHARACTER_TYPE_L},
    {0x1F20, 0x1F45, GX_BIDI_CHARACTER_TYPE_L},
    {0x1F48, 0x1F4D, GX_BIDI_CHARACTER_TYPE_L},
    {0x1F50, 0x1F57, GX_BIDI_CHARACTER_TYPE_L},
    {0x1F59, 0x1F59, GX_BIDI_CHARACTER_TYPE_L},
    {0x1F5B, 0x1F5B, GX_BIDI_CHARACTER_TYPE_L},
    {0x1F5D, 0x1F5D, GX_BIDI_CHARACTER_TYPE_L},
    {0x1F5F, 0x1F7D, GX_BIDI_CHARACTER_TYPE_L},
    {0x1F80, 0x1FB4, GX_BIDI_CHARACTER_TYPE_L},
    {0x1FB6, 0x1FBC, GX_BIDI_CHARACTER_TYPE_L},
    {0x1FBE, 0x1FBE, GX_BIDI_CHARACTER_TYPE_L},
    {0x1FC2, 0x1FC4, GX_BIDI_CHARACTER_TYPE_L},
    {0x1FC6, 0x1FCC, GX_BIDI_CHARACTER_TYPE_L},
    {0x1FD0, 0x1FD3, GX_BIDI_CHARACTER_TYPE_L},
    {0x1FD6, 0x1FDB, GX_BIDI_CHARACTER_TYPE_L},
    {0x1FE0, 0x1FEC, GX_BIDI_CHARACTER_TYPE_L},
    {0x1FF2, 0x1FF4, GX_BIDI_CHARACTER_TYPE_L},
    {0x1FF6, 0x1FFC, GX_BIDI_CHARACTER_TYPE_L},
    {0x200E, 0x200E, GX_BIDI_CHARACTER_TYPE_L},
    {0x2071, 0x2071, GX_BIDI_CHARACTER_TYPE_L},
    {0x207F, 0x207F, GX_BIDI_CHARACTER_TYPE_L},
    {0x2090, 0x2094, GX_BIDI_CHARACTER_TYPE_L},
    {0x2102, 0x2102, GX_BIDI_CHARACTER_TYPE_L},
    {0x2107, 0x2107, GX_BIDI_CHARACTER_TYPE_L},
    {0x210A, 0x2113, GX_BIDI_CHARACTER_TYPE_L},
    {0x2115, 0x2115, GX_BIDI_CHARACTER_TYPE_L},
    {0x2119, 0x211D, GX_BIDI_CHARACTER_TYPE_L},
    {0x2124, 0x2124, GX_BIDI_CHARACTER_TYPE_L},
    {0x2126, 0x2126, GX_BIDI_CHARACTER_TYPE_L},
    {0x2128, 0x2128, GX_BIDI_CHARACTER_TYPE_L},
    {0x212A, 0x212D, GX_BIDI_CHARACTER_TYPE_L},
    {0x212F, 0x2134, GX_BIDI_CHARACTER_TYPE_L},
    {0x2135, 0x2138, GX_BIDI_CHARACTER_TYPE_L},
    {0x2139, 0x2139, GX_BIDI_CHARACTER_TYPE_L},
    {0x213C, 0x213F, GX_BIDI_CHARACTER_TYPE_L},
    {0x2145, 0x2149, GX_BIDI_CHARACTER_TYPE_L},
    {0x214E, 0x214E, GX_BIDI_CHARACTER_TYPE_L},
    {0x214F, 0x214F, GX_BIDI_CHARACTER_TYPE_L},
    {0x2160, 0x2182, GX_BIDI_CHARACTER_TYPE_L},
    {0x2183, 0x2184, GX_BIDI_CHARACTER_TYPE_L},
    {0x2185, 0x2188, GX_BIDI_CHARACTER_TYPE_L},
    {0x2336, 0x237A, GX_BIDI_CHARACTER_TYPE_L},
    {0x2395, 0x2395, GX_BIDI_CHARACTER_TYPE_L},
    {0x249C, 0x24E9, GX_BIDI_CHARACTER_TYPE_L},
    {0x26AC, 0x26AC, GX_BIDI_CHARACTER_TYPE_L},
    {0x2800, 0x28FF, GX_BIDI_CHARACTER_TYPE_L},
    {0x2C00, 0x2C2E, GX_BIDI_CHARACTER_TYPE_L},
    {0x2C30, 0x2C5E, GX_BIDI_CHARACTER_TYPE_L},
    {0x2C60, 0x2C6F, GX_BIDI_CHARACTER_TYPE_L},
    {0x2C71, 0x2C7C, GX_BIDI_CHARACTER_TYPE_L},
    {0x2C7D, 0x2C7D, GX_BIDI_CHARACTER_TYPE_L},
    {0x2C80, 0x2CE4, GX_BIDI_CHARACTER_TYPE_L},
    {0x2D00, 0x2D25, GX_BIDI_CHARACTER_TYPE_L},
    {0x2D30, 0x2D65, GX_BIDI_CHARACTER_TYPE_L},
    {0x2D6F, 0x2D6F, GX_BIDI_CHARACTER_TYPE_L},
    {0x2D80, 0x2D96, GX_BIDI_CHARACTER_TYPE_L},
    {0x2DA0, 0x2DA6, GX_BIDI_CHARACTER_TYPE_L},
    {0x2DA8, 0x2DAE, GX_BIDI_CHARACTER_TYPE_L},
    {0x2DB0, 0x2DB6, GX_BIDI_CHARACTER_TYPE_L},
    {0x2DB8, 0x2DBE, GX_BIDI_CHARACTER_TYPE_L},
    {0x2DC0, 0x2DC6, GX_BIDI_CHARACTER_TYPE_L},
    {0x2DC8, 0x2DCE, GX_BIDI_CHARACTER_TYPE_L},
    {0x2DD0, 0x2DD6, GX_BIDI_CHARACTER_TYPE_L},
    {0x2DD8, 0x2DDE, GX_BIDI_CHARACTER_TYPE_L},
    {0x3005, 0x3005, GX_BIDI_CHARACTER_TYPE_L},
    {0x3006, 0x3006, GX_BIDI_CHARACTER_TYPE_L},
    {0x3007, 0x3007, GX_BIDI_CHARACTER_TYPE_L},
    {0x3021, 0x3029, GX_BIDI_CHARACTER_TYPE_L},
    {0x3031, 0x3035, GX_BIDI_CHARACTER_TYPE_L},
    {0x3038, 0x303A, GX_BIDI_CHARACTER_TYPE_L},
    {0x303B, 0x303B, GX_BIDI_CHARACTER_TYPE_L},
    {0x303C, 0x303C, GX_BIDI_CHARACTER_TYPE_L},
    {0x3041, 0x3096, GX_BIDI_CHARACTER_TYPE_L},
    {0x309D, 0x309E, GX_BIDI_CHARACTER_TYPE_L},
    {0x309F, 0x309F, GX_BIDI_CHARACTER_TYPE_L},
    {0x30A1, 0x30FA, GX_BIDI_CHARACTER_TYPE_L},
    {0x30FC, 0x30FE, GX_BIDI_CHARACTER_TYPE_L},
    {0x30FF, 0x30FF, GX_BIDI_CHARACTER_TYPE_L},
    {0x3105, 0x312D, GX_BIDI_CHARACTER_TYPE_L},
    {0x3131, 0x318E, GX_BIDI_CHARACTER_TYPE_L},
    {0x3190, 0x3191, GX_BIDI_CHARACTER_TYPE_L},
    {0x3192, 0x3195, GX_BIDI_CHARACTER_TYPE_L},
    {0x3196, 0x319F, GX_BIDI_CHARACTER_TYPE_L},
    {0x31A0, 0x31B7, GX_BIDI_CHARACTER_TYPE_L},
    {0x31F0, 0x31FF, GX_BIDI_CHARACTER_TYPE_L},
    {0x3200, 0x321C, GX_BIDI_CHARACTER_TYPE_L},
    {0x3220, 0x3229, GX_BIDI_CHARACTER_TYPE_L},
    {0x322A, 0x3243, GX_BIDI_CHARACTER_TYPE_L},
    {0x3260, 0x327B, GX_BIDI_CHARACTER_TYPE_L},
    {0x327F, 0x327F, GX_BIDI_CHARACTER_TYPE_L},
    {0x3280, 0x3289, GX_BIDI_CHARACTER_TYPE_L},
    {0x328A, 0x32B0, GX_BIDI_CHARACTER_TYPE_L},
    {0x32C0, 0x32CB, GX_BIDI_CHARACTER_TYPE_L},
    {0x32D0, 0x32FE, GX_BIDI_CHARACTER_TYPE_L},
    {0x3300, 0x3376, GX_BIDI_CHARACTER_TYPE_L},
    {0x337B, 0x33DD, GX_BIDI_CHARACTER_TYPE_L},
    {0x33E0, 0x33FE, GX_BIDI_CHARACTER_TYPE_L},
    {0x3400, 0x4DB5, GX_BIDI_CHARACTER_TYPE_L},
    {0x4E00, 0x9FC3, GX_BIDI_CHARACTER_TYPE_L},
    {0xA000, 0xA014, GX_BIDI_CHARACTER_TYPE_L},
    {0xA015, 0xA015, GX_BIDI_CHARACTER_TYPE_L},
    {0xA016, 0xA48C, GX_BIDI_CHARACTER_TYPE_L},
    {0xA500, 0xA60B, GX_BIDI_CHARACTER_TYPE_L},
    {0xA60C, 0xA60C, GX_BIDI_CHARACTER_TYPE_L},
    {0xA610, 0xA61F, GX_BIDI_CHARACTER_TYPE_L},
    {0xA620, 0xA629, GX_BIDI_CHARACTER_TYPE_L},
    {0xA62A, 0xA62B, GX_BIDI_CHARACTER_TYPE_L},
    {0xA640, 0xA65F, GX_BIDI_CHARACTER_TYPE_L},
    {0xA662, 0xA66D, GX_BIDI_CHARACTER_TYPE_L},
    {0xA66E, 0xA66E, GX_BIDI_CHARACTER_TYPE_L},
    {0xA680, 0xA697, GX_BIDI_CHARACTER_TYPE_L},
    {0xA722, 0xA76F, GX_BIDI_CHARACTER_TYPE_L},
    {0xA770, 0xA770, GX_BIDI_CHARACTER_TYPE_L},
    {0xA771, 0xA787, GX_BIDI_CHARACTER_TYPE_L},
    {0xA789, 0xA78A, GX_BIDI_CHARACTER_TYPE_L},
    {0xA78B, 0xA78C, GX_BIDI_CHARACTER_TYPE_L},
    {0xA7FB, 0xA801, GX_BIDI_CHARACTER_TYPE_L},
    {0xA803, 0xA805, GX_BIDI_CHARACTER_TYPE_L},
    {0xA807, 0xA80A, GX_BIDI_CHARACTER_TYPE_L},
    {0xA80C, 0xA822, GX_BIDI_CHARACTER_TYPE_L},
    {0xA823, 0xA824, GX_BIDI_CHARACTER_TYPE_L},
    {0xA827, 0xA827, GX_BIDI_CHARACTER_TYPE_L},
    {0xA840, 0xA873, GX_BIDI_CHARACTER_TYPE_L},
    {0xA880, 0xA881, GX_BIDI_CHARACTER_TYPE_L},
    {0xA882, 0xA8B3, GX_BIDI_CHARACTER_TYPE_L},
    {0xA8B4, 0xA8C3, GX_BIDI_CHARACTER_TYPE_L},
    {0xA8CE, 0xA8CF, GX_BIDI_CHARACTER_TYPE_L},
    {0xA8D0, 0xA8D9, GX_BIDI_CHARACTER_TYPE_L},
    {0xA900, 0xA909, GX_BIDI_CHARACTER_TYPE_L},
    {0xA90A, 0xA925, GX_BIDI_CHARACTER_TYPE_L},
    {0xA92E, 0xA92F, GX_BIDI_CHARACTER_TYPE_L},
    {0xA930, 0xA946, GX_BIDI_CHARACTER_TYPE_L},
    {0xA952, 0xA953, GX_BIDI_CHARACTER_TYPE_L},
    {0xA95F, 0xA95F, GX_BIDI_CHARACTER_TYPE_L},
    {0xAA00, 0xAA28, GX_BIDI_CHARACTER_TYPE_L},
    {0xAA2F, 0xAA30, GX_BIDI_CHARACTER_TYPE_L},
    {0xAA33, 0xAA34, GX_BIDI_CHARACTER_TYPE_L},
    {0xAA40, 0xAA42, GX_BIDI_CHARACTER_TYPE_L},
    {0xAA44, 0xAA4B, GX_BIDI_CHARACTER_TYPE_L},
    {0xAA4D, 0xAA4D, GX_BIDI_CHARACTER_TYPE_L},
    {0xAA50, 0xAA59, GX_BIDI_CHARACTER_TYPE_L},
    {0xAA5C, 0xAA5F, GX_BIDI_CHARACTER_TYPE_L},
    {0xAC00, 0xD7A3, GX_BIDI_CHARACTER_TYPE_L},
    {0xE000, 0xF8FF, GX_BIDI_CHARACTER_TYPE_L},
    {0xF900, 0xFA2D, GX_BIDI_CHARACTER_TYPE_L},
    {0xFA30, 0xFA6A, GX_BIDI_CHARACTER_TYPE_L},
    {0xFA70, 0xFAD9, GX_BIDI_CHARACTER_TYPE_L},
    {0xFB00, 0xFB06, GX_BIDI_CHARACTER_TYPE_L},
    {0xFB13, 0xFB17, GX_BIDI_CHARACTER_TYPE_L},
    {0xFF21, 0xFF3A, GX_BIDI_CHARACTER_TYPE_L},
    {0xFF41, 0xFF5A, GX_BIDI_CHARACTER_TYPE_L},
    {0xFF66, 0xFF6F, GX_BIDI_CHARACTER_TYPE_L},
    {0xFF70, 0xFF70, GX_BIDI_CHARACTER_TYPE_L},
    {0xFF71, 0xFF9D, GX_BIDI_CHARACTER_TYPE_L},
    {0xFF9E, 0xFF9F, GX_BIDI_CHARACTER_TYPE_L},
    {0xFFA0, 0xFFBE, GX_BIDI_CHARACTER_TYPE_L},
    {0xFFC2, 0xFFC7, GX_BIDI_CHARACTER_TYPE_L},
    {0xFFCA, 0xFFCF, GX_BIDI_CHARACTER_TYPE_L},
    {0xFFD2, 0xFFD7, GX_BIDI_CHARACTER_TYPE_L},
    {0xFFDA, 0xFFDC, GX_BIDI_CHARACTER_TYPE_L},
    {0x10000, 0x1000B, GX_BIDI_CHARACTER_TYPE_L},
    {0x1000D, 0x10026, GX_BIDI_CHARACTER_TYPE_L},
    {0x10028, 0x1003A, GX_BIDI_CHARACTER_TYPE_L},
    {0x1003C, 0x1003D, GX_BIDI_CHARACTER_TYPE_L},
    {0x1003F, 0x1004D, GX_BIDI_CHARACTER_TYPE_L},
    {0x10050, 0x1005D, GX_BIDI_CHARACTER_TYPE_L},
    {0x10080, 0x100FA, GX_BIDI_CHARACTER_TYPE_L},
    {0x10100, 0x10100, GX_BIDI_CHARACTER_TYPE_L},
    {0x10102, 0x10102, GX_BIDI_CHARACTER_TYPE_L},
    {0x10107, 0x10133, GX_BIDI_CHARACTER_TYPE_L},
    {0x10137, 0x1013F, GX_BIDI_CHARACTER_TYPE_L},
    {0x101D0, 0x101FC, GX_BIDI_CHARACTER_TYPE_L},
    {0x10280, 0x1029C, GX_BIDI_CHARACTER_TYPE_L},
    {0x102A0, 0x102D0, GX_BIDI_CHARACTER_TYPE_L},
    {0x10300, 0x1031E, GX_BIDI_CHARACTER_TYPE_L},
    {0x10320, 0x10323, GX_BIDI_CHARACTER_TYPE_L},
    {0x10330, 0x10340, GX_BIDI_CHARACTER_TYPE_L},
    {0x10341, 0x10341, GX_BIDI_CHARACTER_TYPE_L},
    {0x10342, 0x10349, GX_BIDI_CHARACTER_TYPE_L},
    {0x1034A, 0x1034A, GX_BIDI_CHARACTER_TYPE_L},
    {0x10380, 0x1039D, GX_BIDI_CHARACTER_TYPE_L},
    {0x1039F, 0x1039F, GX_BIDI_CHARACTER_TYPE_L},
    {0x103A0, 0x103C3, GX_BIDI_CHARACTER_TYPE_L},
    {0x103C8, 0x103CF, GX_BIDI_CHARACTER_TYPE_L},
    {0x103D0, 0x103D0, GX_BIDI_CHARACTER_TYPE_L},
    {0x103D1, 0x103D5, GX_BIDI_CHARACTER_TYPE_L},
    {0x10400, 0x1044F, GX_BIDI_CHARACTER_TYPE_L},
    {0x10450, 0x1049D, GX_BIDI_CHARACTER_TYPE_L},
    {0x104A0, 0x104A9, GX_BIDI_CHARACTER_TYPE_L},
    {0x12000, 0x1236E, GX_BIDI_CHARACTER_TYPE_L},
    {0x12400, 0x12462, GX_BIDI_CHARACTER_TYPE_L},
    {0x12470, 0x12473, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D000, 0x1D0F5, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D100, 0x1D126, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D129, 0x1D164, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D165, 0x1D166, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D16A, 0x1D16C, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D16D, 0x1D172, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D183, 0x1D184, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D18C, 0x1D1A9, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D1AE, 0x1D1DD, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D360, 0x1D371, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D400, 0x1D454, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D456, 0x1D49C, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D49E, 0x1D49F, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D4A2, 0x1D4A2, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D4A5, 0x1D4A6, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D4A9, 0x1D4AC, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D4AE, 0x1D4B9, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D4BB, 0x1D4BB, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D4BD, 0x1D4C3, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D4C5, 0x1D505, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D507, 0x1D50A, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D50D, 0x1D514, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D516, 0x1D51C, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D51E, 0x1D539, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D53B, 0x1D53E, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D540, 0x1D544, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D546, 0x1D546, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D54A, 0x1D550, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D552, 0x1D6A5, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D6A8, 0x1D6C0, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D6C1, 0x1D6C1, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D6C2, 0x1D6DA, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D6DB, 0x1D6DB, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D6DC, 0x1D6FA, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D6FB, 0x1D6FB, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D6FC, 0x1D714, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D715, 0x1D715, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D716, 0x1D734, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D735, 0x1D735, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D736, 0x1D74E, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D74F, 0x1D74F, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D750, 0x1D76E, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D76F, 0x1D76F, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D770, 0x1D788, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D789, 0x1D789, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D78A, 0x1D7A8, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D7A9, 0x1D7A9, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D7AA, 0x1D7C2, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D7C3, 0x1D7C3, GX_BIDI_CHARACTER_TYPE_L},
    {0x1D7C4, 0x1D7CB, GX_BIDI_CHARACTER_TYPE_L},
    {0x20000, 0x2A6D6, GX_BIDI_CHARACTER_TYPE_L},
    {0x2F800, 0x2FA1D, GX_BIDI_CHARACTER_TYPE_L},
    {0xF0000, 0xFFFFD, GX_BIDI_CHARACTER_TYPE_L},
    {0x100000, 0x10FFFD, GX_BIDI_CHARACTER_TYPE_L},
    {0x0590, 0x0590, GX_BIDI_CHARACTER_TYPE_R},
    {0x05BE, 0x05BE, GX_BIDI_CHARACTER_TYPE_R},
    {0x05C0, 0x05C0, GX_BIDI_CHARACTER_TYPE_R},
    {0x05C3, 0x05C3, GX_BIDI_CHARACTER_TYPE_R},
    {0x05C6, 0x05C6, GX_BIDI_CHARACTER_TYPE_R},
    {0x05C8, 0x05CF, GX_BIDI_CHARACTER_TYPE_R},
    {0x05D0, 0x05EA, GX_BIDI_CHARACTER_TYPE_R},
    {0x05EB, 0x05EF, GX_BIDI_CHARACTER_TYPE_R},
    {0x05F0, 0x05F2, GX_BIDI_CHARACTER_TYPE_R},
    {0x05F3, 0x05F4, GX_BIDI_CHARACTER_TYPE_R},
    {0x05F5, 0x05FF, GX_BIDI_CHARACTER_TYPE_R},
    {0x07C0, 0x07C9, GX_BIDI_CHARACTER_TYPE_R},
    {0x07CA, 0x07EA, GX_BIDI_CHARACTER_TYPE_R},
    {0x07F4, 0x07F5, GX_BIDI_CHARACTER_TYPE_R},
    {0x07FA, 0x07FA, GX_BIDI_CHARACTER_TYPE_R},
    {0x07FB, 0x08FF, GX_BIDI_CHARACTER_TYPE_R},
    {0x200F, 0x200F, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB1D, 0xFB1D, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB1F, 0xFB28, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB2A, 0xFB36, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB37, 0xFB37, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB38, 0xFB3C, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB3D, 0xFB3D, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB3E, 0xFB3E, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB3F, 0xFB3F, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB40, 0xFB41, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB42, 0xFB42, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB43, 0xFB44, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB45, 0xFB45, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB46, 0xFB4F, GX_BIDI_CHARACTER_TYPE_R},
    {0x10800, 0x10805, GX_BIDI_CHARACTER_TYPE_R},
    {0x10806, 0x10807, GX_BIDI_CHARACTER_TYPE_R},
    {0x10808, 0x10808, GX_BIDI_CHARACTER_TYPE_R},
    {0x10809, 0x10809, GX_BIDI_CHARACTER_TYPE_R},
    {0x1080A, 0x10835, GX_BIDI_CHARACTER_TYPE_R},
    {0x10836, 0x10836, GX_BIDI_CHARACTER_TYPE_R},
    {0x10837, 0x10838, GX_BIDI_CHARACTER_TYPE_R},
    {0x10839, 0x1083B, GX_BIDI_CHARACTER_TYPE_R},
    {0x1083C, 0x1083C, GX_BIDI_CHARACTER_TYPE_R},
    {0x1083D, 0x1083E, GX_BIDI_CHARACTER_TYPE_R},
    {0x1083F, 0x1083F, GX_BIDI_CHARACTER_TYPE_R},
    {0x10840, 0x108FF, GX_BIDI_CHARACTER_TYPE_R},
    {0x10900, 0x10915, GX_BIDI_CHARACTER_TYPE_R},
    {0x10916, 0x10919, GX_BIDI_CHARACTER_TYPE_R},
    {0x1091A, 0x1091E, GX_BIDI_CHARACTER_TYPE_R},
    {0x10920, 0x10939, GX_BIDI_CHARACTER_TYPE_R},
    {0x1093A, 0x1093E, GX_BIDI_CHARACTER_TYPE_R},
    {0x1093F, 0x1093F, GX_BIDI_CHARACTER_TYPE_R},
    {0x10940, 0x109FF, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A00, 0x10A00, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A04, 0x10A04, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A07, 0x10A0B, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A10, 0x10A13, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A14, 0x10A14, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A15, 0x10A17, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A18, 0x10A18, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A19, 0x10A33, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A34, 0x10A37, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A3B, 0x10A3E, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A40, 0x10A47, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A48, 0x10A4F, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A50, 0x10A58, GX_BIDI_CHARACTER_TYPE_R},
    {0x10A59, 0x10FFF, GX_BIDI_CHARACTER_TYPE_R},
    {0x0030, 0x0039, GX_BIDI_CHARACTER_TYPE_EN},
    {0x00B2, 0x00B3, GX_BIDI_CHARACTER_TYPE_EN},
    {0x00B9, 0x00B9, GX_BIDI_CHARACTER_TYPE_EN},
    {0x06F0, 0x06F9, GX_BIDI_CHARACTER_TYPE_EN},
    {0x2070, 0x2070, GX_BIDI_CHARACTER_TYPE_EN},
    {0x2074, 0x2079, GX_BIDI_CHARACTER_TYPE_EN},
    {0x2080, 0x2089, GX_BIDI_CHARACTER_TYPE_EN},
    {0x2488, 0x249B, GX_BIDI_CHARACTER_TYPE_EN},
    {0xFF10, 0xFF19, GX_BIDI_CHARACTER_TYPE_EN},
    {0x1D7CE, 0x1D7FF, GX_BIDI_CHARACTER_TYPE_EN},
    {0x002B, 0x002B, GX_BIDI_CHARACTER_TYPE_ES},
    {0x002D, 0x002D, GX_BIDI_CHARACTER_TYPE_ES},
    {0x207A, 0x207B, GX_BIDI_CHARACTER_TYPE_ES},
    {0x208A, 0x208B, GX_BIDI_CHARACTER_TYPE_ES},
    {0x2212, 0x2212, GX_BIDI_CHARACTER_TYPE_ES},
    {0xFB29, 0xFB29, GX_BIDI_CHARACTER_TYPE_ES},
    {0xFE62, 0xFE62, GX_BIDI_CHARACTER_TYPE_ES},
    {0xFE63, 0xFE63, GX_BIDI_CHARACTER_TYPE_ES},
    {0xFF0B, 0xFF0B, GX_BIDI_CHARACTER_TYPE_ES},
    {0xFF0D, 0xFF0D, GX_BIDI_CHARACTER_TYPE_ES},
    {0x0023, 0x0023, GX_BIDI_CHARACTER_TYPE_ET},
    {0x0024, 0x0024, GX_BIDI_CHARACTER_TYPE_ET},
    {0x0025, 0x0025, GX_BIDI_CHARACTER_TYPE_ET},
    {0x00A2, 0x00A5, GX_BIDI_CHARACTER_TYPE_ET},
    {0x00B0, 0x00B0, GX_BIDI_CHARACTER_TYPE_ET},
    {0x00B1, 0x00B1, GX_BIDI_CHARACTER_TYPE_ET},
    {0x0609, 0x060A, GX_BIDI_CHARACTER_TYPE_ET},
    {0x066A, 0x066A, GX_BIDI_CHARACTER_TYPE_ET},
    {0x09F2, 0x09F3, GX_BIDI_CHARACTER_TYPE_ET},
    {0x0AF1, 0x0AF1, GX_BIDI_CHARACTER_TYPE_ET},
    {0x0BF9, 0x0BF9, GX_BIDI_CHARACTER_TYPE_ET},
    {0x0E3F, 0x0E3F, GX_BIDI_CHARACTER_TYPE_ET},
    {0x17DB, 0x17DB, GX_BIDI_CHARACTER_TYPE_ET},
    {0x2030, 0x2034, GX_BIDI_CHARACTER_TYPE_ET},
    {0x20A0, 0x20B5, GX_BIDI_CHARACTER_TYPE_ET},
    {0x212E, 0x212E, GX_BIDI_CHARACTER_TYPE_ET},
    {0x2213, 0x2213, GX_BIDI_CHARACTER_TYPE_ET},
    {0xFE5F, 0xFE5F, GX_BIDI_CHARACTER_TYPE_ET},
    {0xFE69, 0xFE69, GX_BIDI_CHARACTER_TYPE_ET},
    {0xFE6A, 0xFE6A, GX_BIDI_CHARACTER_TYPE_ET},
    {0xFF03, 0xFF03, GX_BIDI_CHARACTER_TYPE_ET},
    {0xFF04, 0xFF04, GX_BIDI_CHARACTER_TYPE_ET},
    {0xFF05, 0xFF05, GX_BIDI_CHARACTER_TYPE_ET},
    {0xFFE0, 0xFFE1, GX_BIDI_CHARACTER_TYPE_ET},
    {0xFFE5, 0xFFE6, GX_BIDI_CHARACTER_TYPE_ET},
    {0x0600, 0x0603, GX_BIDI_CHARACTER_TYPE_AN},
    {0x0660, 0x0669, GX_BIDI_CHARACTER_TYPE_AN},
    {0x066B, 0x066C, GX_BIDI_CHARACTER_TYPE_AN},
    {0x06DD, 0x06DD, GX_BIDI_CHARACTER_TYPE_AN},
    {0x002C, 0x002C, GX_BIDI_CHARACTER_TYPE_CS},
    {0x002E, 0x002F, GX_BIDI_CHARACTER_TYPE_CS},
    {0x003A, 0x003A, GX_BIDI_CHARACTER_TYPE_CS},
    {0x00A0, 0x00A0, GX_BIDI_CHARACTER_TYPE_CS},
    {0x060C, 0x060C, GX_BIDI_CHARACTER_TYPE_CS},
    {0x202F, 0x202F, GX_BIDI_CHARACTER_TYPE_CS},
    {0x2044, 0x2044, GX_BIDI_CHARACTER_TYPE_CS},
    {0xFE50, 0xFE50, GX_BIDI_CHARACTER_TYPE_CS},
    {0xFE52, 0xFE52, GX_BIDI_CHARACTER_TYPE_CS},
    {0xFE55, 0xFE55, GX_BIDI_CHARACTER_TYPE_CS},
    {0xFF0C, 0xFF0C, GX_BIDI_CHARACTER_TYPE_CS},
    {0xFF0E, 0xFF0F, GX_BIDI_CHARACTER_TYPE_CS},
    {0xFF1A, 0xFF1A, GX_BIDI_CHARACTER_TYPE_CS},
    {0x000A, 0x000A, GX_BIDI_CHARACTER_TYPE_B},
    {0x000D, 0x000D, GX_BIDI_CHARACTER_TYPE_B},
    {0x001C, 0x001E, GX_BIDI_CHARACTER_TYPE_B},
    {0x0085, 0x0085, GX_BIDI_CHARACTER_TYPE_B},
    {0x2029, 0x2029, GX_BIDI_CHARACTER_TYPE_B},
    {0x0009, 0x0009, GX_BIDI_CHARACTER_TYPE_S},
    {0x000B, 0x000B, GX_BIDI_CHARACTER_TYPE_S},
    {0x001F, 0x001F, GX_BIDI_CHARACTER_TYPE_S},
    {0x000C, 0x000C, GX_BIDI_CHARACTER_TYPE_WS},
    {0x0020, 0x0020, GX_BIDI_CHARACTER_TYPE_WS},
    {0x1680, 0x1680, GX_BIDI_CHARACTER_TYPE_WS},
    {0x180E, 0x180E, GX_BIDI_CHARACTER_TYPE_WS},
    {0x2000, 0x200A, GX_BIDI_CHARACTER_TYPE_WS},
    {0x2028, 0x2028, GX_BIDI_CHARACTER_TYPE_WS},
    {0x205F, 0x205F, GX_BIDI_CHARACTER_TYPE_WS},
    {0x3000, 0x3000, GX_BIDI_CHARACTER_TYPE_WS},
    {0x0021, 0x0022, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0026, 0x0027, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0028, 0x0028, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0029, 0x0029, GX_BIDI_CHARACTER_TYPE_ON},
    {0x002A, 0x002A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x003B, 0x003B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x003C, 0x003E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x003F, 0x0040, GX_BIDI_CHARACTER_TYPE_ON},
    {0x005B, 0x005B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x005C, 0x005C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x005D, 0x005D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x005E, 0x005E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x005F, 0x005F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0060, 0x0060, GX_BIDI_CHARACTER_TYPE_ON},
    {0x007B, 0x007B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x007C, 0x007C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x007D, 0x007D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x007E, 0x007E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00A1, 0x00A1, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00A6, 0x00A7, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00A8, 0x00A8, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00A9, 0x00A9, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00AB, 0x00AB, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00AC, 0x00AC, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00AE, 0x00AE, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00AF, 0x00AF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00B4, 0x00B4, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00B6, 0x00B6, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00B7, 0x00B7, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00B8, 0x00B8, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00BB, 0x00BB, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00BC, 0x00BE, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00BF, 0x00BF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00D7, 0x00D7, GX_BIDI_CHARACTER_TYPE_ON},
    {0x00F7, 0x00F7, GX_BIDI_CHARACTER_TYPE_ON},
    {0x02B9, 0x02BA, GX_BIDI_CHARACTER_TYPE_ON},
    {0x02C2, 0x02C5, GX_BIDI_CHARACTER_TYPE_ON},
    {0x02C6, 0x02CF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x02D2, 0x02DF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x02E5, 0x02EB, GX_BIDI_CHARACTER_TYPE_ON},
    {0x02EC, 0x02EC, GX_BIDI_CHARACTER_TYPE_ON},
    {0x02ED, 0x02ED, GX_BIDI_CHARACTER_TYPE_ON},
    {0x02EF, 0x02FF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0374, 0x0374, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0375, 0x0375, GX_BIDI_CHARACTER_TYPE_ON},
    {0x037E, 0x037E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0384, 0x0385, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0387, 0x0387, GX_BIDI_CHARACTER_TYPE_ON},
    {0x03F6, 0x03F6, GX_BIDI_CHARACTER_TYPE_ON},
    {0x058A, 0x058A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0606, 0x0607, GX_BIDI_CHARACTER_TYPE_ON},
    {0x060E, 0x060F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x06E9, 0x06E9, GX_BIDI_CHARACTER_TYPE_ON},
    {0x07F6, 0x07F6, GX_BIDI_CHARACTER_TYPE_ON},
    {0x07F7, 0x07F9, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0BF3, 0x0BF8, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0BFA, 0x0BFA, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0C78, 0x0C7E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0CF1, 0x0CF2, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0F3A, 0x0F3A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0F3B, 0x0F3B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0F3C, 0x0F3C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0F3D, 0x0F3D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1390, 0x1399, GX_BIDI_CHARACTER_TYPE_ON},
    {0x169B, 0x169B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x169C, 0x169C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x17F0, 0x17F9, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1800, 0x1805, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1806, 0x1806, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1807, 0x180A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1940, 0x1940, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1944, 0x1945, GX_BIDI_CHARACTER_TYPE_ON},
    {0x19DE, 0x19DF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x19E0, 0x19FF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1FBD, 0x1FBD, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1FBF, 0x1FC1, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1FCD, 0x1FCF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1FDD, 0x1FDF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1FED, 0x1FEF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1FFD, 0x1FFE, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2010, 0x2015, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2016, 0x2017, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2018, 0x2018, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2019, 0x2019, GX_BIDI_CHARACTER_TYPE_ON},
    {0x201A, 0x201A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x201B, 0x201C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x201D, 0x201D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x201E, 0x201E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x201F, 0x201F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2020, 0x2027, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2035, 0x2038, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2039, 0x2039, GX_BIDI_CHARACTER_TYPE_ON},
    {0x203A, 0x203A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x203B, 0x203E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x203F, 0x2040, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2041, 0x2043, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2045, 0x2045, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2046, 0x2046, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2047, 0x2051, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2052, 0x2052, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2053, 0x2053, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2054, 0x2054, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2055, 0x205E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x207C, 0x207C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x207D, 0x207D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x207E, 0x207E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x208C, 0x208C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x208D, 0x208D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x208E, 0x208E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2100, 0x2101, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2103, 0x2106, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2108, 0x2109, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2114, 0x2114, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2116, 0x2118, GX_BIDI_CHARACTER_TYPE_ON},
    {0x211E, 0x2123, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2125, 0x2125, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2127, 0x2127, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2129, 0x2129, GX_BIDI_CHARACTER_TYPE_ON},
    {0x213A, 0x213B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2140, 0x2144, GX_BIDI_CHARACTER_TYPE_ON},
    {0x214A, 0x214A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x214B, 0x214B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x214C, 0x214D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2153, 0x215F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2190, 0x2194, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2195, 0x2199, GX_BIDI_CHARACTER_TYPE_ON},
    {0x219A, 0x219B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x219C, 0x219F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21A0, 0x21A0, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21A1, 0x21A2, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21A3, 0x21A3, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21A4, 0x21A5, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21A6, 0x21A6, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21A7, 0x21AD, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21AE, 0x21AE, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21AF, 0x21CD, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21CE, 0x21CF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21D0, 0x21D1, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21D2, 0x21D2, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21D3, 0x21D3, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21D4, 0x21D4, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21D5, 0x21F3, GX_BIDI_CHARACTER_TYPE_ON},
    {0x21F4, 0x2211, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2214, 0x22FF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2300, 0x2307, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2308, 0x230B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x230C, 0x231F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2320, 0x2321, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2322, 0x2328, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2329, 0x2329, GX_BIDI_CHARACTER_TYPE_ON},
    {0x232A, 0x232A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x232B, 0x2335, GX_BIDI_CHARACTER_TYPE_ON},
    {0x237B, 0x237B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x237C, 0x237C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x237D, 0x2394, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2396, 0x239A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x239B, 0x23B3, GX_BIDI_CHARACTER_TYPE_ON},
    {0x23B4, 0x23DB, GX_BIDI_CHARACTER_TYPE_ON},
    {0x23DC, 0x23E1, GX_BIDI_CHARACTER_TYPE_ON},
    {0x23E2, 0x23E7, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2400, 0x2426, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2440, 0x244A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2460, 0x2487, GX_BIDI_CHARACTER_TYPE_ON},
    {0x24EA, 0x24FF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2500, 0x25B6, GX_BIDI_CHARACTER_TYPE_ON},
    {0x25B7, 0x25B7, GX_BIDI_CHARACTER_TYPE_ON},
    {0x25B8, 0x25C0, GX_BIDI_CHARACTER_TYPE_ON},
    {0x25C1, 0x25C1, GX_BIDI_CHARACTER_TYPE_ON},
    {0x25C2, 0x25F7, GX_BIDI_CHARACTER_TYPE_ON},
    {0x25F8, 0x25FF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2600, 0x266E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x266F, 0x266F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2670, 0x269D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x26A0, 0x26AB, GX_BIDI_CHARACTER_TYPE_ON},
    {0x26AD, 0x26BC, GX_BIDI_CHARACTER_TYPE_ON},
    {0x26C0, 0x26C3, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2701, 0x2704, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2706, 0x2709, GX_BIDI_CHARACTER_TYPE_ON},
    {0x270C, 0x2727, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2729, 0x274B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x274D, 0x274D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x274F, 0x2752, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2756, 0x2756, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2758, 0x275E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2761, 0x2767, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2768, 0x2768, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2769, 0x2769, GX_BIDI_CHARACTER_TYPE_ON},
    {0x276A, 0x276A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x276B, 0x276B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x276C, 0x276C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x276D, 0x276D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x276E, 0x276E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x276F, 0x276F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2770, 0x2770, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2771, 0x2771, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2772, 0x2772, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2773, 0x2773, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2774, 0x2774, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2775, 0x2775, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2776, 0x2793, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2794, 0x2794, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2798, 0x27AF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27B1, 0x27BE, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27C0, 0x27C4, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27C5, 0x27C5, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27C6, 0x27C6, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27C7, 0x27CA, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27CC, 0x27CC, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27D0, 0x27E5, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27E6, 0x27E6, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27E7, 0x27E7, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27E8, 0x27E8, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27E9, 0x27E9, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27EA, 0x27EA, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27EB, 0x27EB, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27EC, 0x27EC, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27ED, 0x27ED, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27EE, 0x27EE, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27EF, 0x27EF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x27F0, 0x27FF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2900, 0x2982, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2983, 0x2983, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2984, 0x2984, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2985, 0x2985, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2986, 0x2986, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2987, 0x2987, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2988, 0x2988, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2989, 0x2989, GX_BIDI_CHARACTER_TYPE_ON},
    {0x298A, 0x298A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x298B, 0x298B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x298C, 0x298C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x298D, 0x298D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x298E, 0x298E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x298F, 0x298F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2990, 0x2990, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2991, 0x2991, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2992, 0x2992, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2993, 0x2993, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2994, 0x2994, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2995, 0x2995, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2996, 0x2996, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2997, 0x2997, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2998, 0x2998, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2999, 0x29D7, GX_BIDI_CHARACTER_TYPE_ON},
    {0x29D8, 0x29D8, GX_BIDI_CHARACTER_TYPE_ON},
    {0x29D9, 0x29D9, GX_BIDI_CHARACTER_TYPE_ON},
    {0x29DA, 0x29DA, GX_BIDI_CHARACTER_TYPE_ON},
    {0x29DB, 0x29DB, GX_BIDI_CHARACTER_TYPE_ON},
    {0x29DC, 0x29FB, GX_BIDI_CHARACTER_TYPE_ON},
    {0x29FC, 0x29FC, GX_BIDI_CHARACTER_TYPE_ON},
    {0x29FD, 0x29FD, GX_BIDI_CHARACTER_TYPE_ON},
    {0x29FE, 0x2AFF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2B00, 0x2B2F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2B30, 0x2B44, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2B45, 0x2B46, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2B47, 0x2B4C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2B50, 0x2B54, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2CE5, 0x2CEA, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2CF9, 0x2CFC, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2CFD, 0x2CFD, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2CFE, 0x2CFF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E00, 0x2E01, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E02, 0x2E02, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E03, 0x2E03, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E04, 0x2E04, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E05, 0x2E05, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E06, 0x2E08, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E09, 0x2E09, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E0A, 0x2E0A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E0B, 0x2E0B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E0C, 0x2E0C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E0D, 0x2E0D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E0E, 0x2E16, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E17, 0x2E17, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E18, 0x2E19, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E1A, 0x2E1A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E1B, 0x2E1B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E1C, 0x2E1C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E1D, 0x2E1D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E1E, 0x2E1F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E20, 0x2E20, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E21, 0x2E21, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E22, 0x2E22, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E23, 0x2E23, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E24, 0x2E24, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E25, 0x2E25, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E26, 0x2E26, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E27, 0x2E27, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E28, 0x2E28, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E29, 0x2E29, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E2A, 0x2E2E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E2F, 0x2E2F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E30, 0x2E30, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E80, 0x2E99, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2E9B, 0x2EF3, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2F00, 0x2FD5, GX_BIDI_CHARACTER_TYPE_ON},
    {0x2FF0, 0x2FFB, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3001, 0x3003, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3004, 0x3004, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3008, 0x3008, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3009, 0x3009, GX_BIDI_CHARACTER_TYPE_ON},
    {0x300A, 0x300A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x300B, 0x300B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x300C, 0x300C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x300D, 0x300D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x300E, 0x300E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x300F, 0x300F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3010, 0x3010, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3011, 0x3011, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3012, 0x3013, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3014, 0x3014, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3015, 0x3015, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3016, 0x3016, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3017, 0x3017, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3018, 0x3018, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3019, 0x3019, GX_BIDI_CHARACTER_TYPE_ON},
    {0x301A, 0x301A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x301B, 0x301B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x301C, 0x301C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x301D, 0x301D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x301E, 0x301F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3020, 0x3020, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3030, 0x3030, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3036, 0x3037, GX_BIDI_CHARACTER_TYPE_ON},
    {0x303D, 0x303D, GX_BIDI_CHARACTER_TYPE_ON},
    {0x303E, 0x303F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x309B, 0x309C, GX_BIDI_CHARACTER_TYPE_ON},
    {0x30A0, 0x30A0, GX_BIDI_CHARACTER_TYPE_ON},
    {0x30FB, 0x30FB, GX_BIDI_CHARACTER_TYPE_ON},
    {0x31C0, 0x31E3, GX_BIDI_CHARACTER_TYPE_ON},
    {0x321D, 0x321E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3250, 0x3250, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3251, 0x325F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x327C, 0x327E, GX_BIDI_CHARACTER_TYPE_ON},
    {0x32B1, 0x32BF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x32CC, 0x32CF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x3377, 0x337A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x33DE, 0x33DF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x33FF, 0x33FF, GX_BIDI_CHARACTER_TYPE_ON},
    {0x4DC0, 0x4DFF, GX_BIDI_CHARACTER_TYPE_ON},
    {0xA490, 0xA4C6, GX_BIDI_CHARACTER_TYPE_ON},
    {0xA60D, 0xA60F, GX_BIDI_CHARACTER_TYPE_ON},
    {0xA673, 0xA673, GX_BIDI_CHARACTER_TYPE_ON},
    {0xA67E, 0xA67E, GX_BIDI_CHARACTER_TYPE_ON},
    {0xA67F, 0xA67F, GX_BIDI_CHARACTER_TYPE_ON},
    {0xA700, 0xA716, GX_BIDI_CHARACTER_TYPE_ON},
    {0xA717, 0xA71F, GX_BIDI_CHARACTER_TYPE_ON},
    {0xA720, 0xA721, GX_BIDI_CHARACTER_TYPE_ON},
    {0xA788, 0xA788, GX_BIDI_CHARACTER_TYPE_ON},
    {0xA828, 0xA82B, GX_BIDI_CHARACTER_TYPE_ON},
    {0xA874, 0xA877, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFD3E, 0xFD3E, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFD3F, 0xFD3F, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFDFD, 0xFDFD, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE10, 0xFE16, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE17, 0xFE17, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE18, 0xFE18, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE19, 0xFE19, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE30, 0xFE30, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE31, 0xFE32, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE33, 0xFE34, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE35, 0xFE35, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE36, 0xFE36, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE37, 0xFE37, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE38, 0xFE38, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE39, 0xFE39, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE3A, 0xFE3A, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE3B, 0xFE3B, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE3C, 0xFE3C, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE3D, 0xFE3D, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE3E, 0xFE3E, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE3F, 0xFE3F, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE40, 0xFE40, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE41, 0xFE41, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE42, 0xFE42, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE43, 0xFE43, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE44, 0xFE44, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE45, 0xFE46, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE47, 0xFE47, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE48, 0xFE48, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE49, 0xFE4C, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE4D, 0xFE4F, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE51, 0xFE51, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE54, 0xFE54, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE56, 0xFE57, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE58, 0xFE58, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE59, 0xFE59, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE5A, 0xFE5A, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE5B, 0xFE5B, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE5C, 0xFE5C, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE5D, 0xFE5D, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE5E, 0xFE5E, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE60, 0xFE61, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE64, 0xFE66, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE68, 0xFE68, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFE6B, 0xFE6B, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF01, 0xFF02, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF06, 0xFF07, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF08, 0xFF08, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF09, 0xFF09, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF0A, 0xFF0A, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF1B, 0xFF1B, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF1C, 0xFF1E, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF1F, 0xFF20, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF3B, 0xFF3B, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF3C, 0xFF3C, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF3D, 0xFF3D, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF3E, 0xFF3E, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF3F, 0xFF3F, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF40, 0xFF40, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF5B, 0xFF5B, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF5C, 0xFF5C, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF5D, 0xFF5D, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF5E, 0xFF5E, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF5F, 0xFF5F, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF60, 0xFF60, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF61, 0xFF61, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF62, 0xFF62, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF63, 0xFF63, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFF64, 0xFF65, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFFE2, 0xFFE2, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFFE3, 0xFFE3, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFFE4, 0xFFE4, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFFE8, 0xFFE8, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFFE9, 0xFFEC, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFFED, 0xFFEE, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFFF9, 0xFFFB, GX_BIDI_CHARACTER_TYPE_ON},
    {0xFFFC, 0xFFFD, GX_BIDI_CHARACTER_TYPE_ON},
    {0x10101, 0x10101, GX_BIDI_CHARACTER_TYPE_ON},
    {0x10140, 0x10174, GX_BIDI_CHARACTER_TYPE_ON},
    {0x10175, 0x10178, GX_BIDI_CHARACTER_TYPE_ON},
    {0x10179, 0x10189, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1018A, 0x1018A, GX_BIDI_CHARACTER_TYPE_ON},
    {0x10190, 0x1019B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1091F, 0x1091F, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1D200, 0x1D241, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1D245, 0x1D245, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1D300, 0x1D356, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1F000, 0x1F02B, GX_BIDI_CHARACTER_TYPE_ON},
    {0x1F030, 0x1F093, GX_BIDI_CHARACTER_TYPE_ON},
    {0x0000, 0x0008, GX_BIDI_CHARACTER_TYPE_BN},
    {0x000E, 0x001B, GX_BIDI_CHARACTER_TYPE_BN},
    {0x007F, 0x0084, GX_BIDI_CHARACTER_TYPE_BN},
    {0x0086, 0x009F, GX_BIDI_CHARACTER_TYPE_BN},
    {0x00AD, 0x00AD, GX_BIDI_CHARACTER_TYPE_BN},
    {0x070F, 0x070F, GX_BIDI_CHARACTER_TYPE_BN},
    {0x200B, 0x200D, GX_BIDI_CHARACTER_TYPE_BN},
    {0x2060, 0x2064, GX_BIDI_CHARACTER_TYPE_BN},
    {0x2065, 0x2069, GX_BIDI_CHARACTER_TYPE_BN},
    {0x206A, 0x206F, GX_BIDI_CHARACTER_TYPE_BN},
    {0xFDD0, 0xFDEF, GX_BIDI_CHARACTER_TYPE_BN},
    {0xFEFF, 0xFEFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0xFFF0, 0xFFF8, GX_BIDI_CHARACTER_TYPE_BN},
    {0xFFFE, 0xFFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0x1D173, 0x1D17A, GX_BIDI_CHARACTER_TYPE_BN},
    {0x1FFFE, 0x1FFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0x2FFFE, 0x2FFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0x3FFFE, 0x3FFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0x4FFFE, 0x4FFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0x5FFFE, 0x5FFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0x6FFFE, 0x6FFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0x7FFFE, 0x7FFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0x8FFFE, 0x8FFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0x9FFFE, 0x9FFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0xAFFFE, 0xAFFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0xBFFFE, 0xBFFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0xCFFFE, 0xCFFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0xDFFFE, 0xE0000, GX_BIDI_CHARACTER_TYPE_BN},
    {0xE0001, 0xE0001, GX_BIDI_CHARACTER_TYPE_BN},
    {0xE0002, 0xE001F, GX_BIDI_CHARACTER_TYPE_BN},
    {0xE0020, 0xE007F, GX_BIDI_CHARACTER_TYPE_BN},
    {0xE0080, 0xE00FF, GX_BIDI_CHARACTER_TYPE_BN},
    {0xE01F0, 0xE0FFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0xEFFFE, 0xEFFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0xFFFFE, 0xFFFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0x10FFFE, 0x10FFFF, GX_BIDI_CHARACTER_TYPE_BN},
    {0x0300, 0x036F, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0483, 0x0487, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0488, 0x0489, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0591, 0x05BD, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x05BF, 0x05BF, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x05C1, 0x05C2, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x05C4, 0x05C5, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x05C7, 0x05C7, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0610, 0x061A, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x064B, 0x065E, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0670, 0x0670, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x06D6, 0x06DC, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x06DE, 0x06DE, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x06DF, 0x06E4, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x06E7, 0x06E8, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x06EA, 0x06ED, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0711, 0x0711, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0730, 0x074A, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x07A6, 0x07B0, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x07EB, 0x07F3, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0901, 0x0902, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x093C, 0x093C, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0941, 0x0948, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x094D, 0x094D, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0951, 0x0954, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0962, 0x0963, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0981, 0x0981, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x09BC, 0x09BC, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x09C1, 0x09C4, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x09CD, 0x09CD, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x09E2, 0x09E3, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0A01, 0x0A02, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0A3C, 0x0A3C, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0A41, 0x0A42, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0A47, 0x0A48, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0A4B, 0x0A4D, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0A51, 0x0A51, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0A70, 0x0A71, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0A75, 0x0A75, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0A81, 0x0A82, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0ABC, 0x0ABC, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0AC1, 0x0AC5, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0AC7, 0x0AC8, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0ACD, 0x0ACD, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0AE2, 0x0AE3, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0B01, 0x0B01, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0B3C, 0x0B3C, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0B3F, 0x0B3F, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0B41, 0x0B44, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0B4D, 0x0B4D, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0B56, 0x0B56, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0B62, 0x0B63, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0B82, 0x0B82, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0BC0, 0x0BC0, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0BCD, 0x0BCD, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0C3E, 0x0C40, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0C46, 0x0C48, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0C4A, 0x0C4D, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0C55, 0x0C56, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0C62, 0x0C63, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0CBC, 0x0CBC, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0CCC, 0x0CCD, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0CE2, 0x0CE3, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0D41, 0x0D44, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0D4D, 0x0D4D, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0D62, 0x0D63, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0DCA, 0x0DCA, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0DD2, 0x0DD4, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0DD6, 0x0DD6, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0E31, 0x0E31, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0E34, 0x0E3A, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0E47, 0x0E4E, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0EB1, 0x0EB1, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0EB4, 0x0EB9, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0EBB, 0x0EBC, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0EC8, 0x0ECD, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0F18, 0x0F19, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0F35, 0x0F35, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0F37, 0x0F37, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0F39, 0x0F39, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0F71, 0x0F7E, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0F80, 0x0F84, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0F86, 0x0F87, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0F90, 0x0F97, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0F99, 0x0FBC, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0FC6, 0x0FC6, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x102D, 0x1030, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1032, 0x1037, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1039, 0x103A, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x103D, 0x103E, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1058, 0x1059, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x105E, 0x1060, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1071, 0x1074, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1082, 0x1082, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1085, 0x1086, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x108D, 0x108D, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x135F, 0x135F, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1712, 0x1714, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1732, 0x1734, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1752, 0x1753, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1772, 0x1773, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x17B7, 0x17BD, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x17C6, 0x17C6, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x17C9, 0x17D3, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x17DD, 0x17DD, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x180B, 0x180D, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x18A9, 0x18A9, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1920, 0x1922, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1927, 0x1928, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1932, 0x1932, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1939, 0x193B, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1A17, 0x1A18, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1B00, 0x1B03, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1B34, 0x1B34, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1B36, 0x1B3A, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1B3C, 0x1B3C, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1B42, 0x1B42, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1B6B, 0x1B73, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1B80, 0x1B81, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1BA2, 0x1BA5, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1BA8, 0x1BA9, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1C2C, 0x1C33, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1C36, 0x1C37, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1DC0, 0x1DE6, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1DFE, 0x1DFF, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x20D0, 0x20DC, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x20DD, 0x20E0, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x20E1, 0x20E1, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x20E2, 0x20E4, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x20E5, 0x20F0, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x2DE0, 0x2DFF, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x302A, 0x302F, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x3099, 0x309A, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xA66F, 0xA66F, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xA670, 0xA672, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xA67C, 0xA67D, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xA802, 0xA802, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xA806, 0xA806, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xA80B, 0xA80B, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xA825, 0xA826, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xA8C4, 0xA8C4, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xA926, 0xA92D, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xA947, 0xA951, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xAA29, 0xAA2E, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xAA31, 0xAA32, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xAA35, 0xAA36, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xAA43, 0xAA43, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xAA4C, 0xAA4C, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xFB1E, 0xFB1E, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xFE00, 0xFE0F, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xFE20, 0xFE26, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x101FD, 0x101FD, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x10A01, 0x10A03, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x10A05, 0x10A06, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x10A0C, 0x10A0F, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x10A38, 0x10A3A, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x10A3F, 0x10A3F, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1D167, 0x1D169, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1D17B, 0x1D182, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1D185, 0x1D18B, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1D1AA, 0x1D1AD, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x1D242, 0x1D244, GX_BIDI_CHARACTER_TYPE_NSM},
    {0xE0100, 0xE01EF, GX_BIDI_CHARACTER_TYPE_NSM},
    {0x0604, 0x0605, GX_BIDI_CHARACTER_TYPE_AL},
    {0x0608, 0x0608, GX_BIDI_CHARACTER_TYPE_AL},
    {0x060B, 0x060B, GX_BIDI_CHARACTER_TYPE_AL},
    {0x060D, 0x060D, GX_BIDI_CHARACTER_TYPE_AL},
    {0x061B, 0x061B, GX_BIDI_CHARACTER_TYPE_AL},
    {0x061C, 0x061D, GX_BIDI_CHARACTER_TYPE_AL},
    {0x061E, 0x061F, GX_BIDI_CHARACTER_TYPE_AL},
    {0x0620, 0x0620, GX_BIDI_CHARACTER_TYPE_AL},
    {0x0621, 0x063F, GX_BIDI_CHARACTER_TYPE_AL},
    {0x0640, 0x0640, GX_BIDI_CHARACTER_TYPE_AL},
    {0x0641, 0x064A, GX_BIDI_CHARACTER_TYPE_AL},
    {0x065F, 0x065F, GX_BIDI_CHARACTER_TYPE_AL},
    {0x066D, 0x066D, GX_BIDI_CHARACTER_TYPE_AL},
    {0x066E, 0x066F, GX_BIDI_CHARACTER_TYPE_AL},
    {0x0671, 0x06D3, GX_BIDI_CHARACTER_TYPE_AL},
    {0x06D4, 0x06D4, GX_BIDI_CHARACTER_TYPE_AL},
    {0x06D5, 0x06D5, GX_BIDI_CHARACTER_TYPE_AL},
    {0x06E5, 0x06E6, GX_BIDI_CHARACTER_TYPE_AL},
    {0x06EE, 0x06EF, GX_BIDI_CHARACTER_TYPE_AL},
    {0x06FA, 0x06FC, GX_BIDI_CHARACTER_TYPE_AL},
    {0x06FD, 0x06FE, GX_BIDI_CHARACTER_TYPE_AL},
    {0x06FF, 0x06FF, GX_BIDI_CHARACTER_TYPE_AL},
    {0x0700, 0x070D, GX_BIDI_CHARACTER_TYPE_AL},
    {0x070E, 0x070E, GX_BIDI_CHARACTER_TYPE_AL},
    {0x0710, 0x0710, GX_BIDI_CHARACTER_TYPE_AL},
    {0x0712, 0x072F, GX_BIDI_CHARACTER_TYPE_AL},
    {0x074B, 0x074C, GX_BIDI_CHARACTER_TYPE_AL},
    {0x074D, 0x07A5, GX_BIDI_CHARACTER_TYPE_AL},
    {0x07B1, 0x07B1, GX_BIDI_CHARACTER_TYPE_AL},
    {0x07B2, 0x07BF, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFB50, 0xFBB1, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFBB2, 0xFBD2, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFBD3, 0xFD3D, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFD40, 0xFD4F, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFD50, 0xFD8F, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFD90, 0xFD91, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFD92, 0xFDC7, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFDC8, 0xFDCF, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFDF0, 0xFDFB, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFDFC, 0xFDFC, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFDFE, 0xFDFF, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFE70, 0xFE74, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFE75, 0xFE75, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFE76, 0xFEFC, GX_BIDI_CHARACTER_TYPE_AL},
    {0xFEFD, 0xFEFE, GX_BIDI_CHARACTER_TYPE_AL},
    {0x0600, 0x06FF, GX_BIDI_CHARACTER_TYPE_AL},        /* Arabic */
    {0x0700, 0x074F, GX_BIDI_CHARACTER_TYPE_AL},        /* Syriac */
    {0x0750, 0x077F, GX_BIDI_CHARACTER_TYPE_AL},        /* Arabic_Supplement */
    {0x0780, 0x07BF, GX_BIDI_CHARACTER_TYPE_AL},        /* Thaana */
    {0xFB50, 0xFDFF, GX_BIDI_CHARACTER_TYPE_AL},        /* Arabic_Presentation_Formas_A */
    {0xFE70, 0xFEFF, GX_BIDI_CHARACTER_TYPE_AL},        /* Arabic_Presentation_formas_B */
    {0x0590, 0x05FF, GX_BIDI_CHARACTER_TYPE_R},         /* Hebrew */
    {0x07C0, 0x07FF, GX_BIDI_CHARACTER_TYPE_R},         /* NKo*/
    {0x00010800, 0x0001083F, GX_BIDI_CHARACTER_TYPE_R}, /* Cypriot_Syllabary */
    {0x00010900, 0x0001091F, GX_BIDI_CHARACTER_TYPE_R}, /* Phoenician */
    {0x00010920, 0x0001093F, GX_BIDI_CHARACTER_TYPE_R}, /* Lydian */
    {0x00010A00, 0x00010A5F, GX_BIDI_CHARACTER_TYPE_R}, /* Kharoshthi */
    {0x0800, 0x08FF, GX_BIDI_CHARACTER_TYPE_R},
    {0xFB1D, 0xFB4F, GX_BIDI_CHARACTER_TYPE_R},
    {0x00010840, 0x00010FFF, GX_BIDI_CHARACTER_TYPE_R},
    {0, 0, 0}
};

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_character_type_get                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function gets the bidi type of a unicode.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    code                                  Code point whose bidi type is */
/*                                            retrieved                   */
/*    type                                  Retrieved bidi type.          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_bidi_character_type_get(ULONG code, GX_UBYTE *type)
{
GX_UBYTE                         get_type = GX_BIDI_CHARACTER_TYPE_L;
const GX_BIDI_CHARACTER_INFO *entry = _gx_bidi_character_types;

    switch (code)
    {
    case GX_BIDI_DIRECTION_FORMAT_LRE:
        get_type = GX_BIDI_CHARACTER_TYPE_LRE;
        break;
    case GX_BIDI_DIRECTION_FORMAT_RLE:
        get_type = GX_BIDI_CHARACTER_TYPE_RLE;
        break;
    case GX_BIDI_DIRECTION_FORMAT_LRO:
        get_type = GX_BIDI_CHARACTER_TYPE_LRO;
        break;
    case GX_BIDI_DIRECTION_FORMAT_RLO:
        get_type = GX_BIDI_CHARACTER_TYPE_RLO;
        break;
    case GX_BIDI_DIRECTION_FORMAT_PDF:
        get_type = GX_BIDI_CHARACTER_TYPE_PDF;
        break;
    case GX_BIDI_DIRECTION_FORMAT_LRI:
        get_type = GX_BIDI_CHARACTER_TYPE_LRI;
        break;
    case GX_BIDI_DIRECTION_FORMAT_RLI:
        get_type = GX_BIDI_CHARACTER_TYPE_RLI;
        break;
    case GX_BIDI_DIRECTION_FORMAT_FSI:
        get_type = GX_BIDI_CHARACTER_TYPE_FSI;
        break;
    case GX_BIDI_DIRECTION_FORMAT_PDI:
        get_type = GX_BIDI_CHARACTER_TYPE_PDI;
        break;

    default:
        while (entry->type)
        {
            if ((code >= entry->range_start) &&
                (code <= entry->range_end))
            {
                get_type = entry->type;
                break;
            }

            entry++;
        }
        break;
    }

    if (type)
    {
        *type = get_type;
    }

    return GX_SUCCESS;
}
#endif

/**************************************************************************/

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
struct GX_BIDI_MIRROR
{
    USHORT code;
    USHORT mirror_code;
};

static const GX_BIDI_MIRROR _gx_bidi_mirroring[] =
{
    {0x0028, 0x0029}, /* LEFT PARENTHESIS */
    {0x0029, 0x0028}, /*RIGHT PARENTHESIS */
    {0x003C, 0x003E}, /*LESS-THAN SIGN */
    {0x003E, 0x003C}, /*GREATER-THAN SIGN */
    {0x005B, 0x005D}, /*LEFT SQUARE BRACKET */
    {0x005D, 0x005B}, /*RIGHT SQUARE BRACKET */
    {0x007B, 0x007D}, /*LEFT CURLY BRACKET */
    {0x007D, 0x007B}, /*RIGHT CURLY BRACKET */
    {0x00AB, 0x00BB}, /*LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
    {0x00BB, 0x00AB}, /*RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
    {0x0F3A, 0x0F3B}, /*TIBETAN MARK GUG RTAGS GYON */
    {0x0F3B, 0x0F3A}, /*TIBETAN MARK GUG RTAGS GYAS */
    {0x0F3C, 0x0F3D}, /*TIBETAN MARK ANG KHANG GYON */
    {0x0F3D, 0x0F3C}, /*TIBETAN MARK ANG KHANG GYAS */
    {0x169B, 0x169C}, /*OGHAM FEATHER MARK */
    {0x169C, 0x169B}, /*OGHAM REVERSED FEATHER MARK */
    {0x2039, 0x203A}, /*SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
    {0x203A, 0x2039}, /*SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
    {0x2045, 0x2046}, /*LEFT SQUARE BRACKET WITH QUILL */
    {0x2046, 0x2045}, /*RIGHT SQUARE BRACKET WITH QUILL */
    {0x207D, 0x207E}, /*SUPERSCRIPT LEFT PARENTHESIS */
    {0x207E, 0x207D}, /*SUPERSCRIPT RIGHT PARENTHESIS */
    {0x208D, 0x208E}, /*SUBSCRIPT LEFT PARENTHESIS */
    {0x208E, 0x208D}, /*SUBSCRIPT RIGHT PARENTHESIS */
    {0x2208, 0x220B}, /*ELEMENT OF */
    {0x2209, 0x220C}, /*NOT AN ELEMENT OF */
    {0x220A, 0x220D}, /*SMALL ELEMENT OF */
    {0x220B, 0x2208}, /*CONTAINS AS MEMBER */
    {0x220C, 0x2209}, /*DOES NOT CONTAIN AS MEMBER */
    {0x220D, 0x220A}, /*SMALL CONTAINS AS MEMBER */
    {0x2215, 0x29F5}, /*DIVISION SLASH */
    {0x223C, 0x223D}, /*TILDE OPERATOR */
    {0x223D, 0x223C}, /*REVERSED TILDE */
    {0x2243, 0x22CD}, /*ASYMPTOTICALLY EQUAL TO */
    {0x2252, 0x2253}, /*APPROXIMATELY EQUAL TO OR THE IMAGE OF */
    {0x2253, 0x2252}, /*IMAGE OF OR APPROXIMATELY EQUAL TO */
    {0x2254, 0x2255}, /*COLON EQUALS */
    {0x2255, 0x2254}, /*EQUALS COLON */
    {0x2264, 0x2265}, /*LESS-THAN OR EQUAL TO */
    {0x2265, 0x2264}, /*GREATER-THAN OR EQUAL TO */
    {0x2266, 0x2267}, /*LESS-THAN OVER EQUAL TO */
    {0x2267, 0x2266}, /*GREATER-THAN OVER EQUAL TO */
    {0x2268, 0x2269}, /*[BEST FIT] LESS-THAN BUT NOT EQUAL TO */
    {0x2269, 0x2268}, /*[BEST FIT] GREATER-THAN BUT NOT EQUAL TO */
    {0x226A, 0x226B}, /*MUCH LESS-THAN */
    {0x226B, 0x226A}, /*MUCH GREATER-THAN */
    {0x226E, 0x226F}, /*[BEST FIT] NOT LESS-THAN */
    {0x226F, 0x226E}, /*[BEST FIT] NOT GREATER-THAN */
    {0x2270, 0x2271}, /*[BEST FIT] NEITHER LESS-THAN NOR EQUAL TO */
    {0x2271, 0x2270}, /*[BEST FIT] NEITHER GREATER-THAN NOR EQUAL TO */
    {0x2272, 0x2273}, /*[BEST FIT] LESS-THAN OR EQUIVALENT TO */
    {0x2273, 0x2272}, /*[BEST FIT] GREATER-THAN OR EQUIVALENT TO */
    {0x2274, 0x2275}, /*[BEST FIT] NEITHER LESS-THAN NOR EQUIVALENT TO */
    {0x2275, 0x2274}, /*[BEST FIT] NEITHER GREATER-THAN NOR EQUIVALENT TO */
    {0x2276, 0x2277}, /*LESS-THAN OR GREATER-THAN */
    {0x2277, 0x2276}, /*GREATER-THAN OR LESS-THAN */
    {0x2278, 0x2279}, /*[BEST FIT] NEITHER LESS-THAN NOR GREATER-THAN */
    {0x2279, 0x2278}, /*[BEST FIT] NEITHER GREATER-THAN NOR LESS-THAN */
    {0x227A, 0x227B}, /*PRECEDES */
    {0x227B, 0x227A}, /*SUCCEEDS */
    {0x227C, 0x227D}, /*PRECEDES OR EQUAL TO */
    {0x227D, 0x227C}, /*SUCCEEDS OR EQUAL TO */
    {0x227E, 0x227F}, /*[BEST FIT] PRECEDES OR EQUIVALENT TO */
    {0x227F, 0x227E}, /*[BEST FIT] SUCCEEDS OR EQUIVALENT TO */
    {0x2280, 0x2281}, /*[BEST FIT] DOES NOT PRECEDE */
    {0x2281, 0x2280}, /*[BEST FIT] DOES NOT SUCCEED */
    {0x2282, 0x2283}, /*SUBSET OF */
    {0x2283, 0x2282}, /*SUPERSET OF */
    {0x2284, 0x2285}, /*[BEST FIT] NOT A SUBSET OF */
    {0x2285, 0x2284}, /*[BEST FIT] NOT A SUPERSET OF */
    {0x2286, 0x2287}, /*SUBSET OF OR EQUAL TO */
    {0x2287, 0x2286}, /*SUPERSET OF OR EQUAL TO */
    {0x2288, 0x2289}, /*[BEST FIT] NEITHER A SUBSET OF NOR EQUAL TO */
    {0x2289, 0x2288}, /*[BEST FIT] NEITHER A SUPERSET OF NOR EQUAL TO */
    {0x228A, 0x228B}, /*[BEST FIT] SUBSET OF WITH NOT EQUAL TO */
    {0x228B, 0x228A}, /*[BEST FIT] SUPERSET OF WITH NOT EQUAL TO */
    {0x228F, 0x2290}, /*SQUARE IMAGE OF */
    {0x2290, 0x228F}, /*SQUARE ORIGINAL OF */
    {0x2291, 0x2292}, /*SQUARE IMAGE OF OR EQUAL TO */
    {0x2292, 0x2291}, /*SQUARE ORIGINAL OF OR EQUAL TO */
    {0x2298, 0x29B8}, /*CIRCLED DIVISION SLASH */
    {0x22A2, 0x22A3}, /*RIGHT TACK */
    {0x22A3, 0x22A2}, /*LEFT TACK */
    {0x22A6, 0x2ADE}, /*ASSERTION */
    {0x22A8, 0x2AE4}, /*TRUE */
    {0x22A9, 0x2AE3}, /*FORCES */
    {0x22AB, 0x2AE5}, /*DOUBLE VERTICAL BAR DOUBLE RIGHT TURNSTILE */
    {0x22B0, 0x22B1}, /*PRECEDES UNDER RELATION */
    {0x22B1, 0x22B0}, /*SUCCEEDS UNDER RELATION */
    {0x22B2, 0x22B3}, /*NORMAL SUBGROUP OF */
    {0x22B3, 0x22B2}, /*CONTAINS AS NORMAL SUBGROUP */
    {0x22B4, 0x22B5}, /*NORMAL SUBGROUP OF OR EQUAL TO */
    {0x22B5, 0x22B4}, /*CONTAINS AS NORMAL SUBGROUP OR EQUAL TO */
    {0x22B6, 0x22B7}, /*ORIGINAL OF */
    {0x22B7, 0x22B6}, /*IMAGE OF */
    {0x22C9, 0x22CA}, /*LEFT NORMAL FACTOR SEMIDIRECT PRODUCT */
    {0x22CA, 0x22C9}, /*RIGHT NORMAL FACTOR SEMIDIRECT PRODUCT */
    {0x22CB, 0x22CC}, /*LEFT SEMIDIRECT PRODUCT */
    {0x22CC, 0x22CB}, /*RIGHT SEMIDIRECT PRODUCT */
    {0x22CD, 0x2243}, /*REVERSED TILDE EQUALS */
    {0x22D0, 0x22D1}, /*DOUBLE SUBSET */
    {0x22D1, 0x22D0}, /*DOUBLE SUPERSET */
    {0x22D6, 0x22D7}, /*LESS-THAN WITH DOT */
    {0x22D7, 0x22D6}, /*GREATER-THAN WITH DOT */
    {0x22D8, 0x22D9}, /*VERY MUCH LESS-THAN */
    {0x22D9, 0x22D8}, /*VERY MUCH GREATER-THAN */
    {0x22DA, 0x22DB}, /*LESS-THAN EQUAL TO OR GREATER-THAN */
    {0x22DB, 0x22DA}, /*GREATER-THAN EQUAL TO OR LESS-THAN */
    {0x22DC, 0x22DD}, /*EQUAL TO OR LESS-THAN */
    {0x22DD, 0x22DC}, /*EQUAL TO OR GREATER-THAN */
    {0x22DE, 0x22DF}, /*EQUAL TO OR PRECEDES */
    {0x22DF, 0x22DE}, /*EQUAL TO OR SUCCEEDS */
    {0x22E0, 0x22E1}, /*[BEST FIT] DOES NOT PRECEDE OR EQUAL */
    {0x22E1, 0x22E0}, /*[BEST FIT] DOES NOT SUCCEED OR EQUAL */
    {0x22E2, 0x22E3}, /*[BEST FIT] NOT SQUARE IMAGE OF OR EQUAL TO */
    {0x22E3, 0x22E2}, /*[BEST FIT] NOT SQUARE ORIGINAL OF OR EQUAL TO */
    {0x22E4, 0x22E5}, /*[BEST FIT] SQUARE IMAGE OF OR NOT EQUAL TO */
    {0x22E5, 0x22E4}, /*[BEST FIT] SQUARE ORIGINAL OF OR NOT EQUAL TO */
    {0x22E6, 0x22E7}, /*[BEST FIT] LESS-THAN BUT NOT EQUIVALENT TO */
    {0x22E7, 0x22E6}, /*[BEST FIT] GREATER-THAN BUT NOT EQUIVALENT TO */
    {0x22E8, 0x22E9}, /*[BEST FIT] PRECEDES BUT NOT EQUIVALENT TO */
    {0x22E9, 0x22E8}, /*[BEST FIT] SUCCEEDS BUT NOT EQUIVALENT TO */
    {0x22EA, 0x22EB}, /*[BEST FIT] NOT NORMAL SUBGROUP OF */
    {0x22EB, 0x22EA}, /*[BEST FIT] DOES NOT CONTAIN AS NORMAL SUBGROUP */
    {0x22EC, 0x22ED}, /*[BEST FIT] NOT NORMAL SUBGROUP OF OR EQUAL TO */
    {0x22ED, 0x22EC}, /*[BEST FIT] DOES NOT CONTAIN AS NORMAL SUBGROUP OR EQUAL */
    {0x22F0, 0x22F1}, /*UP RIGHT DIAGONAL ELLIPSIS */
    {0x22F1, 0x22F0}, /*DOWN RIGHT DIAGONAL ELLIPSIS */
    {0x22F2, 0x22FA}, /*ELEMENT OF WITH LONG HORIZONTAL STROKE */
    {0x22F3, 0x22FB}, /*ELEMENT OF WITH VERTICAL BAR AT END OF HORIZONTAL STROKE */
    {0x22F4, 0x22FC}, /*SMALL ELEMENT OF WITH VERTICAL BAR AT END OF HORIZONTAL STROKE */
    {0x22F6, 0x22FD}, /*ELEMENT OF WITH OVERBAR */
    {0x22F7, 0x22FE}, /*SMALL ELEMENT OF WITH OVERBAR */
    {0x22FA, 0x22F2}, /*CONTAINS WITH LONG HORIZONTAL STROKE */
    {0x22FB, 0x22F3}, /*CONTAINS WITH VERTICAL BAR AT END OF HORIZONTAL STROKE */
    {0x22FC, 0x22F4}, /*SMALL CONTAINS WITH VERTICAL BAR AT END OF HORIZONTAL STROKE */
    {0x22FD, 0x22F6}, /*CONTAINS WITH OVERBAR */
    {0x22FE, 0x22F7}, /*SMALL CONTAINS WITH OVERBAR */
    {0x2308, 0x2309}, /*LEFT CEILING */
    {0x2309, 0x2308}, /*RIGHT CEILING */
    {0x230A, 0x230B}, /*LEFT FLOOR */
    {0x230B, 0x230A}, /*RIGHT FLOOR */
    {0x2329, 0x232A}, /*LEFT-POINTING ANGLE BRACKET */
    {0x232A, 0x2329}, /*RIGHT-POINTING ANGLE BRACKET */
    {0x2768, 0x2769}, /*MEDIUM LEFT PARENTHESIS ORNAMENT */
    {0x2769, 0x2768}, /*MEDIUM RIGHT PARENTHESIS ORNAMENT */
    {0x276A, 0x276B}, /*MEDIUM FLATTENED LEFT PARENTHESIS ORNAMENT */
    {0x276B, 0x276A}, /*MEDIUM FLATTENED RIGHT PARENTHESIS ORNAMENT */
    {0x276C, 0x276D}, /*MEDIUM LEFT-POINTING ANGLE BRACKET ORNAMENT */
    {0x276D, 0x276C}, /*MEDIUM RIGHT-POINTING ANGLE BRACKET ORNAMENT */
    {0x276E, 0x276F}, /*HEAVY LEFT-POINTING ANGLE QUOTATION MARK ORNAMENT */
    {0x276F, 0x276E}, /*HEAVY RIGHT-POINTING ANGLE QUOTATION MARK ORNAMENT */
    {0x2770, 0x2771}, /*HEAVY LEFT-POINTING ANGLE BRACKET ORNAMENT */
    {0x2771, 0x2770}, /*HEAVY RIGHT-POINTING ANGLE BRACKET ORNAMENT */
    {0x2772, 0x2773}, /*LIGHT LEFT TORTOISE SHELL BRACKET ORNAMENT */
    {0x2773, 0x2772}, /*LIGHT RIGHT TORTOISE SHELL BRACKET ORNAMENT */
    {0x2774, 0x2775}, /*MEDIUM LEFT CURLY BRACKET ORNAMENT */
    {0x2775, 0x2774}, /*MEDIUM RIGHT CURLY BRACKET ORNAMENT */
    {0x27C3, 0x27C4}, /*OPEN SUBSET */
    {0x27C4, 0x27C3}, /*OPEN SUPERSET */
    {0x27C5, 0x27C6}, /*LEFT S-SHAPED BAG DELIMITER */
    {0x27C6, 0x27C5}, /*RIGHT S-SHAPED BAG DELIMITER */
    {0x27C8, 0x27C9}, /*REVERSE SOLIDUS PRECEDING SUBSET */
    {0x27C9, 0x27C8}, /*SUPERSET PRECEDING SOLIDUS */
    {0x27CB, 0x27CD}, /*MATHEMATICAL RISING DIAGONAL */
    {0x27CD, 0x27CB}, /*MATHEMATICAL FALLING DIAGONAL */
    {0x27D5, 0x27D6}, /*LEFT OUTER JOIN */
    {0x27D6, 0x27D5}, /*RIGHT OUTER JOIN */
    {0x27DD, 0x27DE}, /*LONG RIGHT TACK */
    {0x27DE, 0x27DD}, /*LONG LEFT TACK */
    {0x27E2, 0x27E3}, /*WHITE CONCAVE-SIDED DIAMOND WITH LEFTWARDS TICK */
    {0x27E3, 0x27E2}, /*WHITE CONCAVE-SIDED DIAMOND WITH RIGHTWARDS TICK */
    {0x27E4, 0x27E5}, /*WHITE SQUARE WITH LEFTWARDS TICK */
    {0x27E5, 0x27E4}, /*WHITE SQUARE WITH RIGHTWARDS TICK */
    {0x27E6, 0x27E7}, /*MATHEMATICAL LEFT WHITE SQUARE BRACKET */
    {0x27E7, 0x27E6}, /*MATHEMATICAL RIGHT WHITE SQUARE BRACKET */
    {0x27E8, 0x27E9}, /*MATHEMATICAL LEFT ANGLE BRACKET */
    {0x27E9, 0x27E8}, /*MATHEMATICAL RIGHT ANGLE BRACKET */
    {0x27EA, 0x27EB}, /*MATHEMATICAL LEFT DOUBLE ANGLE BRACKET */
    {0x27EB, 0x27EA}, /*MATHEMATICAL RIGHT DOUBLE ANGLE BRACKET */
    {0x27EC, 0x27ED}, /*MATHEMATICAL LEFT WHITE TORTOISE SHELL BRACKET */
    {0x27ED, 0x27EC}, /*MATHEMATICAL RIGHT WHITE TORTOISE SHELL BRACKET */
    {0x27EE, 0x27EF}, /*MATHEMATICAL LEFT FLATTENED PARENTHESIS */
    {0x27EF, 0x27EE}, /*MATHEMATICAL RIGHT FLATTENED PARENTHESIS */
    {0x2983, 0x2984}, /*LEFT WHITE CURLY BRACKET */
    {0x2984, 0x2983}, /*RIGHT WHITE CURLY BRACKET */
    {0x2985, 0x2986}, /*LEFT WHITE PARENTHESIS */
    {0x2986, 0x2985}, /*RIGHT WHITE PARENTHESIS */
    {0x2987, 0x2988}, /*Z NOTATION LEFT IMAGE BRACKET */
    {0x2988, 0x2987}, /*Z NOTATION RIGHT IMAGE BRACKET */
    {0x2989, 0x298A}, /*Z NOTATION LEFT BINDING BRACKET */
    {0x298A, 0x2989}, /*Z NOTATION RIGHT BINDING BRACKET */
    {0x298B, 0x298C}, /*LEFT SQUARE BRACKET WITH UNDERBAR */
    {0x298C, 0x298B}, /*RIGHT SQUARE BRACKET WITH UNDERBAR */
    {0x298D, 0x2990}, /*LEFT SQUARE BRACKET WITH TICK IN TOP CORNER */
    {0x298E, 0x298F}, /*RIGHT SQUARE BRACKET WITH TICK IN BOTTOM CORNER */
    {0x298F, 0x298E}, /*LEFT SQUARE BRACKET WITH TICK IN BOTTOM CORNER */
    {0x2990, 0x298D}, /*RIGHT SQUARE BRACKET WITH TICK IN TOP CORNER */
    {0x2991, 0x2992}, /*LEFT ANGLE BRACKET WITH DOT */
    {0x2992, 0x2991}, /*RIGHT ANGLE BRACKET WITH DOT */
    {0x2993, 0x2994}, /*LEFT ARC LESS-THAN BRACKET */
    {0x2994, 0x2993}, /*RIGHT ARC GREATER-THAN BRACKET */
    {0x2995, 0x2996}, /*DOUBLE LEFT ARC GREATER-THAN BRACKET */
    {0x2996, 0x2995}, /*DOUBLE RIGHT ARC LESS-THAN BRACKET */
    {0x2997, 0x2998}, /*LEFT BLACK TORTOISE SHELL BRACKET */
    {0x2998, 0x2997}, /*RIGHT BLACK TORTOISE SHELL BRACKET */
    {0x29B8, 0x2298}, /*CIRCLED REVERSE SOLIDUS */
    {0x29C0, 0x29C1}, /*CIRCLED LESS-THAN */
    {0x29C1, 0x29C0}, /*CIRCLED GREATER-THAN */
    {0x29C4, 0x29C5}, /*SQUARED RISING DIAGONAL SLASH */
    {0x29C5, 0x29C4}, /*SQUARED FALLING DIAGONAL SLASH */
    {0x29CF, 0x29D0}, /*LEFT TRIANGLE BESIDE VERTICAL BAR */
    {0x29D0, 0x29CF}, /*VERTICAL BAR BESIDE RIGHT TRIANGLE */
    {0x29D1, 0x29D2}, /*BOWTIE WITH LEFT HALF BLACK */
    {0x29D2, 0x29D1}, /*BOWTIE WITH RIGHT HALF BLACK */
    {0x29D4, 0x29D5}, /*TIMES WITH LEFT HALF BLACK */
    {0x29D5, 0x29D4}, /*TIMES WITH RIGHT HALF BLACK */
    {0x29D8, 0x29D9}, /*LEFT WIGGLY FENCE */
    {0x29D9, 0x29D8}, /*RIGHT WIGGLY FENCE */
    {0x29DA, 0x29DB}, /*LEFT DOUBLE WIGGLY FENCE */
    {0x29DB, 0x29DA}, /*RIGHT DOUBLE WIGGLY FENCE */
    {0x29F5, 0x2215}, /*REVERSE SOLIDUS OPERATOR */
    {0x29F8, 0x29F9}, /*BIG SOLIDUS */
    {0x29F9, 0x29F8}, /*BIG REVERSE SOLIDUS */
    {0x29FC, 0x29FD}, /*LEFT-POINTING CURVED ANGLE BRACKET */
    {0x29FD, 0x29FC}, /*RIGHT-POINTING CURVED ANGLE BRACKET */
    {0x2A2B, 0x2A2C}, /*MINUS SIGN WITH FALLING DOTS */
    {0x2A2C, 0x2A2B}, /*MINUS SIGN WITH RISING DOTS */
    {0x2A2D, 0x2A2E}, /*PLUS SIGN IN LEFT HALF CIRCLE */
    {0x2A2E, 0x2A2D}, /*PLUS SIGN IN RIGHT HALF CIRCLE */
    {0x2A34, 0x2A35}, /*MULTIPLICATION SIGN IN LEFT HALF CIRCLE */
    {0x2A35, 0x2A34}, /*MULTIPLICATION SIGN IN RIGHT HALF CIRCLE */
    {0x2A3C, 0x2A3D}, /*INTERIOR PRODUCT */
    {0x2A3D, 0x2A3C}, /*RIGHTHAND INTERIOR PRODUCT */
    {0x2A64, 0x2A65}, /*Z NOTATION DOMAIN ANTIRESTRICTION */
    {0x2A65, 0x2A64}, /*Z NOTATION RANGE ANTIRESTRICTION */
    {0x2A79, 0x2A7A}, /*LESS-THAN WITH CIRCLE INSIDE */
    {0x2A7A, 0x2A79}, /*GREATER-THAN WITH CIRCLE INSIDE */
    {0x2A7D, 0x2A7E}, /*LESS-THAN OR SLANTED EQUAL TO */
    {0x2A7E, 0x2A7D}, /*GREATER-THAN OR SLANTED EQUAL TO */
    {0x2A7F, 0x2A80}, /*LESS-THAN OR SLANTED EQUAL TO WITH DOT INSIDE */
    {0x2A80, 0x2A7F}, /*GREATER-THAN OR SLANTED EQUAL TO WITH DOT INSIDE */
    {0x2A81, 0x2A82}, /*LESS-THAN OR SLANTED EQUAL TO WITH DOT ABOVE */
    {0x2A82, 0x2A81}, /*GREATER-THAN OR SLANTED EQUAL TO WITH DOT ABOVE */
    {0x2A83, 0x2A84}, /*LESS-THAN OR SLANTED EQUAL TO WITH DOT ABOVE RIGHT */
    {0x2A84, 0x2A83}, /*GREATER-THAN OR SLANTED EQUAL TO WITH DOT ABOVE LEFT */
    {0x2A8B, 0x2A8C}, /*LESS-THAN ABOVE DOUBLE-LINE EQUAL ABOVE GREATER-THAN */
    {0x2A8C, 0x2A8B}, /*GREATER-THAN ABOVE DOUBLE-LINE EQUAL ABOVE LESS-THAN */
    {0x2A91, 0x2A92}, /*LESS-THAN ABOVE GREATER-THAN ABOVE DOUBLE-LINE EQUAL */
    {0x2A92, 0x2A91}, /*GREATER-THAN ABOVE LESS-THAN ABOVE DOUBLE-LINE EQUAL */
    {0x2A93, 0x2A94}, /*LESS-THAN ABOVE SLANTED EQUAL ABOVE GREATER-THAN ABOVE SLANTED EQUAL */
    {0x2A94, 0x2A93}, /*GREATER-THAN ABOVE SLANTED EQUAL ABOVE LESS-THAN ABOVE SLANTED EQUAL */
    {0x2A95, 0x2A96}, /*SLANTED EQUAL TO OR LESS-THAN */
    {0x2A96, 0x2A95}, /*SLANTED EQUAL TO OR GREATER-THAN */
    {0x2A97, 0x2A98}, /*SLANTED EQUAL TO OR LESS-THAN WITH DOT INSIDE */
    {0x2A98, 0x2A97}, /*SLANTED EQUAL TO OR GREATER-THAN WITH DOT INSIDE */
    {0x2A99, 0x2A9A}, /*DOUBLE-LINE EQUAL TO OR LESS-THAN */
    {0x2A9A, 0x2A99}, /*DOUBLE-LINE EQUAL TO OR GREATER-THAN */
    {0x2A9B, 0x2A9C}, /*DOUBLE-LINE SLANTED EQUAL TO OR LESS-THAN */
    {0x2A9C, 0x2A9B}, /*DOUBLE-LINE SLANTED EQUAL TO OR GREATER-THAN */
    {0x2AA1, 0x2AA2}, /*DOUBLE NESTED LESS-THAN */
    {0x2AA2, 0x2AA1}, /*DOUBLE NESTED GREATER-THAN */
    {0x2AA6, 0x2AA7}, /*LESS-THAN CLOSED BY CURVE */
    {0x2AA7, 0x2AA6}, /*GREATER-THAN CLOSED BY CURVE */
    {0x2AA8, 0x2AA9}, /*LESS-THAN CLOSED BY CURVE ABOVE SLANTED EQUAL */
    {0x2AA9, 0x2AA8}, /*GREATER-THAN CLOSED BY CURVE ABOVE SLANTED EQUAL */
    {0x2AAA, 0x2AAB}, /*SMALLER THAN */
    {0x2AAB, 0x2AAA}, /*LARGER THAN */
    {0x2AAC, 0x2AAD}, /*SMALLER THAN OR EQUAL TO */
    {0x2AAD, 0x2AAC}, /*LARGER THAN OR EQUAL TO */
    {0x2AAF, 0x2AB0}, /*PRECEDES ABOVE SINGLE-LINE EQUALS SIGN */
    {0x2AB0, 0x2AAF}, /*SUCCEEDS ABOVE SINGLE-LINE EQUALS SIGN */
    {0x2AB3, 0x2AB4}, /*PRECEDES ABOVE EQUALS SIGN */
    {0x2AB4, 0x2AB3}, /*SUCCEEDS ABOVE EQUALS SIGN */
    {0x2ABB, 0x2ABC}, /*DOUBLE PRECEDES */
    {0x2ABC, 0x2ABB}, /*DOUBLE SUCCEEDS */
    {0x2ABD, 0x2ABE}, /*SUBSET WITH DOT */
    {0x2ABE, 0x2ABD}, /*SUPERSET WITH DOT */
    {0x2ABF, 0x2AC0}, /*SUBSET WITH PLUS SIGN BELOW */
    {0x2AC0, 0x2ABF}, /*SUPERSET WITH PLUS SIGN BELOW */
    {0x2AC1, 0x2AC2}, /*SUBSET WITH MULTIPLICATION SIGN BELOW */
    {0x2AC2, 0x2AC1}, /*SUPERSET WITH MULTIPLICATION SIGN BELOW */
    {0x2AC3, 0x2AC4}, /*SUBSET OF OR EQUAL TO WITH DOT ABOVE */
    {0x2AC4, 0x2AC3}, /*SUPERSET OF OR EQUAL TO WITH DOT ABOVE */
    {0x2AC5, 0x2AC6}, /*SUBSET OF ABOVE EQUALS SIGN */
    {0x2AC6, 0x2AC5}, /*SUPERSET OF ABOVE EQUALS SIGN */
    {0x2ACD, 0x2ACE}, /*SQUARE LEFT OPEN BOX OPERATOR */
    {0x2ACE, 0x2ACD}, /*SQUARE RIGHT OPEN BOX OPERATOR */
    {0x2ACF, 0x2AD0}, /*CLOSED SUBSET */
    {0x2AD0, 0x2ACF}, /*CLOSED SUPERSET */
    {0x2AD1, 0x2AD2}, /*CLOSED SUBSET OR EQUAL TO */
    {0x2AD2, 0x2AD1}, /*CLOSED SUPERSET OR EQUAL TO */
    {0x2AD3, 0x2AD4}, /*SUBSET ABOVE SUPERSET */
    {0x2AD4, 0x2AD3}, /*SUPERSET ABOVE SUBSET */
    {0x2AD5, 0x2AD6}, /*SUBSET ABOVE SUBSET */
    {0x2AD6, 0x2AD5}, /*SUPERSET ABOVE SUPERSET */
    {0x2ADE, 0x22A6}, /*SHORT LEFT TACK */
    {0x2AE3, 0x22A9}, /*DOUBLE VERTICAL BAR LEFT TURNSTILE */
    {0x2AE4, 0x22A8}, /*VERTICAL BAR DOUBLE LEFT TURNSTILE */
    {0x2AE5, 0x22AB}, /*DOUBLE VERTICAL BAR DOUBLE LEFT TURNSTILE */
    {0x2AEC, 0x2AED}, /*DOUBLE STROKE NOT SIGN */
    {0x2AED, 0x2AEC}, /*REVERSED DOUBLE STROKE NOT SIGN */
    {0x2AF7, 0x2AF8}, /*TRIPLE NESTED LESS-THAN */
    {0x2AF8, 0x2AF7}, /*TRIPLE NESTED GREATER-THAN */
    {0x2AF9, 0x2AFA}, /*DOUBLE-LINE SLANTED LESS-THAN OR EQUAL TO */
    {0x2AFA, 0x2AF9}, /*DOUBLE-LINE SLANTED GREATER-THAN OR EQUAL TO */
    {0x2E02, 0x2E03}, /*LEFT SUBSTITUTION BRACKET */
    {0x2E03, 0x2E02}, /*RIGHT SUBSTITUTION BRACKET */
    {0x2E04, 0x2E05}, /*LEFT DOTTED SUBSTITUTION BRACKET */
    {0x2E05, 0x2E04}, /*RIGHT DOTTED SUBSTITUTION BRACKET */
    {0x2E09, 0x2E0A}, /*LEFT TRANSPOSITION BRACKET */
    {0x2E0A, 0x2E09}, /*RIGHT TRANSPOSITION BRACKET */
    {0x2E0C, 0x2E0D}, /*LEFT RAISED OMISSION BRACKET */
    {0x2E0D, 0x2E0C}, /*RIGHT RAISED OMISSION BRACKET */
    {0x2E1C, 0x2E1D}, /*LEFT LOW PARAPHRASE BRACKET */
    {0x2E1D, 0x2E1C}, /*RIGHT LOW PARAPHRASE BRACKET */
    {0x2E20, 0x2E21}, /*LEFT VERTICAL BAR WITH QUILL */
    {0x2E21, 0x2E20}, /*RIGHT VERTICAL BAR WITH QUILL */
    {0x2E22, 0x2E23}, /*TOP LEFT HALF BRACKET */
    {0x2E23, 0x2E22}, /*TOP RIGHT HALF BRACKET */
    {0x2E24, 0x2E25}, /*BOTTOM LEFT HALF BRACKET */
    {0x2E25, 0x2E24}, /*BOTTOM RIGHT HALF BRACKET */
    {0x2E26, 0x2E27}, /*LEFT SIDEWAYS U BRACKET */
    {0x2E27, 0x2E26}, /*RIGHT SIDEWAYS U BRACKET */
    {0x2E28, 0x2E29}, /*LEFT DOUBLE PARENTHESIS */
    {0x2E29, 0x2E28}, /*RIGHT DOUBLE PARENTHESIS */
    {0x3008, 0x3009}, /*LEFT ANGLE BRACKET */
    {0x3009, 0x3008}, /*RIGHT ANGLE BRACKET */
    {0x300A, 0x300B}, /*LEFT DOUBLE ANGLE BRACKET */
    {0x300B, 0x300A}, /*RIGHT DOUBLE ANGLE BRACKET */
    {0x300C, 0x300D}, /*[BEST FIT] LEFT CORNER BRACKET */
    {0x300D, 0x300C}, /*[BEST FIT] RIGHT CORNER BRACKET */
    {0x300E, 0x300F}, /*[BEST FIT] LEFT WHITE CORNER BRACKET */
    {0x300F, 0x300E}, /*[BEST FIT] RIGHT WHITE CORNER BRACKET */
    {0x3010, 0x3011}, /*LEFT BLACK LENTICULAR BRACKET */
    {0x3011, 0x3010}, /*RIGHT BLACK LENTICULAR BRACKET */
    {0x3014, 0x3015}, /*LEFT TORTOISE SHELL BRACKET */
    {0x3015, 0x3014}, /*RIGHT TORTOISE SHELL BRACKET */
    {0x3016, 0x3017}, /*LEFT WHITE LENTICULAR BRACKET */
    {0x3017, 0x3016}, /*RIGHT WHITE LENTICULAR BRACKET */
    {0x3018, 0x3019}, /*LEFT WHITE TORTOISE SHELL BRACKET */
    {0x3019, 0x3018}, /*RIGHT WHITE TORTOISE SHELL BRACKET */
    {0x301A, 0x301B}, /*LEFT WHITE SQUARE BRACKET */
    {0x301B, 0x301A}, /*RIGHT WHITE SQUARE BRACKET */
    {0xFE59, 0xFE5A}, /*SMALL LEFT PARENTHESIS */
    {0xFE5A, 0xFE59}, /*SMALL RIGHT PARENTHESIS */
    {0xFE5B, 0xFE5C}, /*SMALL LEFT CURLY BRACKET */
    {0xFE5C, 0xFE5B}, /*SMALL RIGHT CURLY BRACKET */
    {0xFE5D, 0xFE5E}, /*SMALL LEFT TORTOISE SHELL BRACKET */
    {0xFE5E, 0xFE5D}, /*SMALL RIGHT TORTOISE SHELL BRACKET */
    {0xFE64, 0xFE65}, /*SMALL LESS-THAN SIGN */
    {0xFE65, 0xFE64}, /*SMALL GREATER-THAN SIGN */
    {0xFF08, 0xFF09}, /*FULLWIDTH LEFT PARENTHESIS */
    {0xFF09, 0xFF08}, /*FULLWIDTH RIGHT PARENTHESIS */
    {0xFF1C, 0xFF1E}, /*FULLWIDTH LESS-THAN SIGN */
    {0xFF1E, 0xFF1C}, /*FULLWIDTH GREATER-THAN SIGN */
    {0xFF3B, 0xFF3D}, /*FULLWIDTH LEFT SQUARE BRACKET */
    {0xFF3D, 0xFF3B}, /*FULLWIDTH RIGHT SQUARE BRACKET */
    {0xFF5B, 0xFF5D}, /*FULLWIDTH LEFT CURLY BRACKET */
    {0xFF5D, 0xFF5B}, /*FULLWIDTH RIGHT CURLY BRACKET */
    {0xFF5F, 0xFF60}, /*FULLWIDTH LEFT WHITE PARENTHESIS */
    {0xFF60, 0xFF5F}, /*FULLWIDTH RIGHT WHITE PARENTHESIS */
    {0xFF62, 0xFF63}, /*[BEST FIT] HALFWIDTH LEFT CORNER BRACKET */
    {0xFF63, 0xFF62}, /*[BEST FIT] HALFWIDTH RIGHT CORNER BRACKET*/
    {0, 0}
};

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_mirroring_get                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function gets the mirror code of a unicode.                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    code                                  Code point whose mirror code  */
/*                                            is retrieved                */
/*    mirror                                Retrieved mirror.             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_bidi_mirroring_get(USHORT code, USHORT *mirror)
{
const GX_BIDI_MIRROR *entry = _gx_bidi_mirroring;

    *mirror = 0;

    while (entry->code)
    {
        if (entry->code == code)
        {
            *mirror = entry->mirror_code;
            break;
        }
        entry++;
    }

    return GX_SUCCESS;
}

#endif

/**************************************************************************/

#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)

#define GX_LINK_RESOLVED_BIDI_TEXT_INFO                         \
    if (pre)                                                    \
    {                                                           \
        pre->next = resolved_info;                              \
    }                                                           \
    pre = resolved_info;                                        \
    if (!head)                                                  \
    {                                                           \
        head = pre;                                             \
    }

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_buffer_allocate                                    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to calculate character count to be handled */
/*    and allocate a buffer for following processing.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_buffer_allocate(GX_BIDI_CONTEXT *context)
{
INT                  character_count = 0;
GX_STRING            text;
ULONG                unicode;
INT                  max_depth = 0;
INT                  depth = 0;
INT                  level_run_count = 1;
INT                  bracket_pair_count = 0;
INT                  max_bracket_pair = 0;
INT                  run_stack_size;
INT                  explicite_stack_size;
GX_BIDI_BRACKET_PAIR bracket_pair;
GX_BIDI_TEXT_INFO   *input_info = context->input_info;

    text = input_info->text;

    while (text.length > 0)
    {
        if ((text.ptr[0] == GX_KEY_CARRIAGE_RETURN) ||
            (text.ptr[0] == GX_KEY_LINE_FEED))
        {
            break;
        }

        /* Convert utf8 to unicode. */
        unicode = 0;
        _gx_utility_utf8_string_character_get(&text, (GX_CHAR_CODE *)&unicode, GX_NULL);

        switch (unicode)
        {
        case GX_BIDI_DIRECTION_FORMAT_RLE:
        case GX_BIDI_DIRECTION_FORMAT_LRE:
        case GX_BIDI_DIRECTION_FORMAT_RLO:
        case GX_BIDI_DIRECTION_FORMAT_LRO:
        case GX_BIDI_DIRECTION_FORMAT_RLI:
        case GX_BIDI_DIRECTION_FORMAT_LRI:
        case GX_BIDI_DIRECTION_FORMAT_FSI:
            depth++;
            level_run_count++;
            break;

        case GX_BIDI_DIRECTION_FORMAT_PDF:
        case GX_BIDI_DIRECTION_FORMAT_PDI:
            depth--;
            level_run_count++;
            break;
        default:
            if (max_bracket_pair < GX_BIDI_MAX_BRACKET_DEPTH)
            {

                _gx_utility_bidi_bracket_pair_get(unicode, &bracket_pair);
                if (bracket_pair.open)
                {
                    if (bracket_pair.open == unicode)
                    {
                        bracket_pair_count++;
                        if (bracket_pair_count > max_bracket_pair)
                        {
                            max_bracket_pair = bracket_pair_count;
                        }
                    }
                }
            }
            break;
        }

        if (depth > max_depth)
        {
            max_depth = depth;
        }

        character_count++;
    }

    if (max_depth > GX_BIDI_MAX_EXPLICIT_DEPTH + 1)
    {
        max_depth = GX_BIDI_MAX_EXPLICIT_DEPTH + 1;
    }

    context->processced_size = input_info->text.length - text.length;
    context->reordered_utf8_size = context->processced_size;
    context->unit_count = character_count;

    context->buffer_size = sizeof(GX_BIDI_UNIT) * (UINT)(character_count);         /* unit list size. */

    if (input_info->font && input_info->display_width > 0)
    {
        context->buffer_size += sizeof(ULONG) * (UINT)character_count; /* line index cache size. */
    }

    /* Size of buffer used for loading explicit level data
       Will be released before level run buffer is used. */
    explicite_stack_size = (INT)sizeof(GX_BIDI_EXPLICIT_ENTRY) * (max_depth + 1);

    /* Size of buffer used for loading level run data. */
    run_stack_size = (INT)sizeof(GX_BIDI_LEVEL_RUN) * character_count;

    /* Size of buffer used for loading isolate run data. */
    run_stack_size += (INT)sizeof(GX_BIDI_ISOLATE_RUN) * level_run_count + (INT)sizeof(ULONG) * character_count;

    /* Size of buffer used for loading bracket pair data. */
    context->bracket_pair_size = sizeof(ULONG) * (UINT)(max_bracket_pair * 2);
    run_stack_size += (INT)context->bracket_pair_size;
    run_stack_size += (INT)sizeof(ULONG) * (INT)max_bracket_pair;

    if (run_stack_size < explicite_stack_size)
    {
        run_stack_size = explicite_stack_size;
    }

    context->buffer_size += (UINT)run_stack_size;

    context->buffer = (GX_UBYTE *)_gx_system_memory_allocator(context->buffer_size);
    if (context->buffer == GX_NULL)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    memset(context->buffer, 0, (size_t)context->buffer_size);

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_initiate                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to convert utf8 text to unicode, initiate  */
/*    initiate character types and calculate the total lines.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    font                                  Text font, if not set, the    */
/*                                            text is handled as a single */
/*                                            line                        */
/*    display_width                         The max width of text in one  */
/*                                            line , if not set, the text */
/*                                            is handled as a single line */
/*    total_lines                           Total lines of handled text   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_initiate(GX_BIDI_CONTEXT *context)
{
GX_BIDI_TEXT_INFO *input_info = context->input_info;
INT                index = 0;
GX_STRING          string;
GX_CHAR_CODE       code;
GX_BIDI_UNIT      *unit;

    context->unit_list = (GX_BIDI_UNIT *)context->buffer;
    context->buffer_index += sizeof(GX_BIDI_UNIT) * (UINT)context->unit_count;

    string = input_info->text;

    while (index < context->unit_count)
    {
        unit = &context->unit_list[index];

        /* Convert utf8 to unicode. */
        _gx_utility_utf8_string_character_get(&string, &code, GX_NULL);
        unit->code = code;

        _gx_utility_bidi_character_type_get(unit->code, &unit->type);
        unit->org_type = unit->type;

        index++;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_block_level_determine                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to calculate the level of the specified    */
/*    text block                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    start_index                           The start index of the text   */
/*                                            block                       */
/*    end_index                             The end index of the text     */
/*                                            block                       */
/*    level                                 Buffer for level value        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*    _gx_utility_bidi_explicit_levels_determine                          */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_block_level_determine(GX_BIDI_CONTEXT *context, UINT start_index, UINT end_index, GX_UBYTE *level)
{
INT           isolate_count = 0;
GX_BIDI_UNIT *unit;

    unit = &context->unit_list[start_index];

    while (start_index <= end_index)
    {
        /* Find the first character of type L, AL, or R
           while skipping over any hcaracters between an isolate initiator and its matching PDI. */
        switch (unit->type)
        {
        case GX_BIDI_CHARACTER_TYPE_AL:
        case GX_BIDI_CHARACTER_TYPE_R:
            if (isolate_count == 0)
            {
                *level = 1;
                return GX_SUCCESS;
            }
            break;

        case GX_BIDI_CHARACTER_TYPE_L:
            if (isolate_count == 0)
            {
                *level = 0;
                return GX_SUCCESS;
            }
            break;

        case GX_BIDI_CHARACTER_TYPE_RLI:
        case GX_BIDI_CHARACTER_TYPE_LRI:
            isolate_count++;
            break;

        case GX_BIDI_CHARACTER_TYPE_PDI:
            if (isolate_count)
            {
                isolate_count--;
            }
            break;
        }

        start_index++;
        unit++;
    }

    /* Otherwise, set paragraph embedding level to 0. */
    *level = 0;
    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_explicit_levels_determine                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to calculate explicite level of every      */
/*    character.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*                                            format.                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_explicit_levels_determine(GX_BIDI_CONTEXT *context)
{
GX_BIDI_EXPLICIT_ENTRY *stack;
GX_BIDI_EXPLICIT_ENTRY *entry;
GX_BIDI_EXPLICIT_ENTRY *last_entry;
INT                     stack_top = 0;
INT                     overflow_isolate_count = 0;
INT                     overflow_embedding_count = 0;
INT                     valid_isolate_count = 0;
INT                     index = 0;
INT                     end_index;
GX_UBYTE                level;
ULONG                   type;
GX_BIDI_UNIT           *unit;

    stack = (GX_BIDI_EXPLICIT_ENTRY *)(context->buffer + context->buffer_index);

    if (context->buffer_index > context->buffer_size)
    {
        return GX_INVALID_MEMORY_SIZE;
    }
    entry = stack;

    /* Push onto the stack an entry consisting of the paragraph embedding level,
       a neutral directional override status, and a false directional isolate status. */
    entry->level = context->base_level;
    entry->override_status = GX_BIDI_OVERRIDE_STATUS_N;
    entry->isolate_status = GX_FALSE;
    last_entry = entry;
    entry++;
    stack_top++;

    unit = &context->unit_list[index];

    while (index < context->unit_count)
    {
        type = unit->type;

        switch (type)
        {
        case GX_BIDI_CHARACTER_TYPE_RLE:
        case GX_BIDI_CHARACTER_TYPE_LRE:
        case GX_BIDI_CHARACTER_TYPE_RLO:
        case GX_BIDI_CHARACTER_TYPE_LRO:
            /* Handle explicit embeddings/overrides. */
            entry->level = (GX_UBYTE)(last_entry->level + 1);
            unit->type = GX_BIDI_CHARACTER_TYPE_BN;

            if (type == GX_BIDI_CHARACTER_TYPE_RLE ||
                type == GX_BIDI_CHARACTER_TYPE_RLO)
            {
                /* Compute the least odd embedding level greater than the embedding level of the last entry. */
                if ((entry->level & 0x01) == 0)
                {
                    entry->level = (GX_UBYTE)(entry->level + 1);
                }
            }
            else
            {
                /* Compute the least even embedding level greater than the embedding level of the last entry. */
                if ((entry->level & 0x01) != 0)
                {
                    entry->level = (GX_UBYTE)(entry->level + 1);
                }
            }

            if ((entry->level <= GX_BIDI_MAX_EXPLICIT_DEPTH) &&
                (overflow_isolate_count == 0) &&
                (overflow_embedding_count == 0))
            {
                /* Push an entry to stack. */

                if (type == GX_BIDI_CHARACTER_TYPE_RLE ||
                    type == GX_BIDI_CHARACTER_TYPE_LRE)
                {
                    entry->override_status = GX_BIDI_OVERRIDE_STATUS_N;
                }
                else if (type == GX_BIDI_CHARACTER_TYPE_RLO)
                {
                    entry->override_status = GX_BIDI_OVERRIDE_STATUS_R;
                }
                else
                {
                    entry->override_status = GX_BIDI_OVERRIDE_STATUS_L;
                }

                entry->isolate_status = GX_FALSE;
                last_entry = entry;
                entry++;
                stack_top++;
            }
            else if (overflow_isolate_count == 0)
            {
                overflow_embedding_count++;
            }
            break;

        case GX_BIDI_CHARACTER_TYPE_RLI:
        case GX_BIDI_CHARACTER_TYPE_LRI:
        case GX_BIDI_CHARACTER_TYPE_FSI:
            if (type == GX_BIDI_CHARACTER_TYPE_FSI)
            {
                for (end_index = index + 1; end_index < context->unit_count; end_index++)
                {
                    if (context->unit_list[end_index].type == GX_BIDI_CHARACTER_TYPE_PDI)
                    {
                        break;
                    }
                }

                _gx_utility_bidi_block_level_determine(context, (UINT)index, (UINT)end_index, &level);

                if (level == 1)
                {
                    type = GX_BIDI_CHARACTER_TYPE_RLI;
                }
                else
                {
                    type = GX_BIDI_CHARACTER_TYPE_LRI;
                }
            }

            /* Set the RLI's embedding level to the embedding level of the last entry. */
            unit->level = last_entry->level;
            entry->level = (GX_UBYTE)(last_entry->level + 1);

            if (last_entry->override_status == GX_BIDI_OVERRIDE_STATUS_L)
            {
                /* Reset the current character type from RLI to L if last override status is left to right. */
                unit->type = GX_BIDI_CHARACTER_TYPE_L;
            }
            else if (last_entry->override_status == GX_BIDI_OVERRIDE_STATUS_R)
            {
                /* Reset the current character type from RLI to R if last overried status is right to left. */
                unit->type = GX_BIDI_CHARACTER_TYPE_R;
            }

            if ((type == GX_BIDI_CHARACTER_TYPE_RLI && (entry->level & 0x01) == 0) ||
                (type == GX_BIDI_CHARACTER_TYPE_LRI && (entry->level & 0x01) != 0))
            {
                entry->level = (GX_UBYTE)(entry->level + 1);
            }

            if (entry->level <= GX_BIDI_MAX_EXPLICIT_DEPTH &&
                overflow_isolate_count == 0 &&
                overflow_embedding_count == 0)
            {
                valid_isolate_count++;
                entry->override_status = GX_BIDI_OVERRIDE_STATUS_N;
                entry->isolate_status = GX_TRUE;
                last_entry = entry;
                entry++;
                stack_top++;
            }
            else
            {
                /* This is an overflow RLI/LRI. */
                overflow_isolate_count++;
            }
            break;

        case GX_BIDI_CHARACTER_TYPE_PDF:
            unit->type = GX_BIDI_CHARACTER_TYPE_BN;
            if (overflow_isolate_count == 0)
            {
                if (overflow_embedding_count)
                {
                    overflow_embedding_count--;
                }
                else if (last_entry->isolate_status == GX_FALSE &&
                         stack_top >= 2)
                {
                    last_entry--;
                    entry--;
                    stack_top--;
                }
            }
            break;

        case GX_BIDI_CHARACTER_TYPE_PDI:
            if (overflow_isolate_count > 0)
            {
                /* This PDI matches an overflow isolate initiator. */
                overflow_isolate_count--;
            }
            else if (valid_isolate_count != 0)
            {
                overflow_embedding_count = 0;

                while ((stack_top > 1) &&
                       (last_entry->isolate_status == GX_FALSE))
                {
                    /* Pop the last entry. */
                    last_entry--;
                    entry--;
                    stack_top--;
                }

                last_entry--;
                entry--;
                stack_top--;
                valid_isolate_count--;
            }

            unit->level = last_entry->level;

            if (last_entry->override_status == GX_BIDI_OVERRIDE_STATUS_L)
            {
                unit->type = GX_BIDI_CHARACTER_TYPE_L;
            }
            else if (last_entry->override_status == GX_BIDI_OVERRIDE_STATUS_R)
            {
                unit->type = GX_BIDI_CHARACTER_TYPE_R;
            }
            break;

        case GX_BIDI_CHARACTER_TYPE_BN:
            break;

        default:
            /* Set current character's embedding level to the embedding level of the last entry. */
            unit->level = last_entry->level;

            if (last_entry->override_status == GX_BIDI_OVERRIDE_STATUS_L)
            {
                unit->type = GX_BIDI_CHARACTER_TYPE_L;
            }
            else if (last_entry->override_status == GX_BIDI_OVERRIDE_STATUS_R)
            {
                unit->type = GX_BIDI_CHARACTER_TYPE_R;
            }
            break;
        }

        index++;
        unit++;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_level_runs_compute                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to calculate the level runs of the         */
/*    specified text block                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    start_index                           The start index of the text   */
/*                                            block                       */
/*    end_index                             The end index of the text     */
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_level_runs_compute(GX_BIDI_CONTEXT *context, INT start_index, INT end_index)
{
INT                run_start_index = -1;
INT                run_end_index = -1;
INT                index = start_index;
GX_BIDI_LEVEL_RUN *entry;
GX_BIDI_LEVEL_RUN *last_entry = GX_NULL;
GX_UBYTE           run_level = 0;
GX_BIDI_UNIT      *unit;

    if (context->level_runs == GX_NULL)
    {
        context->level_runs = (GX_BIDI_LEVEL_RUN *)(context->buffer + context->buffer_index);
        context->buffer_index += sizeof(GX_BIDI_LEVEL_RUN) * (UINT)context->unit_count;

        if (context->buffer_index > context->buffer_size)
        {
            return GX_INVALID_MEMORY_SIZE;
        }
    }

    entry = context->level_runs;

    unit = &context->unit_list[index];
    while (index <= end_index)
    {
        switch (unit->type)
        {
        case GX_BIDI_CHARACTER_TYPE_BN:
            break;

        default:
            if (run_start_index == -1)
            {
                run_start_index = index;
                run_level = unit->level;
            }
            else if (run_level != unit->level)
            {
                entry->start_index = run_start_index;
                entry->end_index = run_end_index;
                entry->level = run_level;
                entry->next = GX_NULL;

                if (last_entry)
                {
                    last_entry->next = entry;
                }
                last_entry = entry;
                entry++;
                run_start_index = index;
                run_level = unit->level;
            }
            run_end_index = index;
            break;
        }

        index++;
        unit++;
    }

    entry->start_index = run_start_index;
    entry->end_index = run_end_index;
    entry->level = run_level;
    entry->next = GX_NULL;

    if (last_entry)
    {
        last_entry->next = entry;
    }
    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utiltiy_isolate_run_sequence_append                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to append a sequence to the specified      */
/*    isolate run sequence.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    start_index                           Start position of the append  */
/*                                            sequence                    */
/*    end_index                             End position of the append    */
/*                                            sequence                    */
/*    siolate_run                           Pointer to isolate run        */
/*                                            structure                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_one_isolate_run_sequences_get                      */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utiltiy_isolate_run_sequence_append(GX_BIDI_CONTEXT *context, INT start_index, INT end_index,
                                                    GX_BIDI_ISOLATE_RUN *isolate_run)
{
INT           index;
GX_BIDI_UNIT *unit;

    unit = &context->unit_list[start_index];
    for (index = start_index; index <= end_index; index++)
    {
        if (unit->type == GX_BIDI_CHARACTER_TYPE_BN)
        {
            unit++;
            continue;
        }

        isolate_run->index_list[isolate_run->index_count++] = index;
        context->buffer_index += sizeof(ULONG);

        unit++;
    }

    if (context->buffer_index > context->buffer_size)
    {
        return GX_INVALID_MEMORY_SIZE;
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_one_isolate_run_sequence_get                       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to calculate one isolate run sequence      */
/*    start from the specified level run.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    pre_level                             Embedding level of the        */
/*                                            previous level run          */
/*    level_run                             Pointer to level run structure*/
/*    isolate_run                           Buffer to load an isolate run */
/*                                            sequence                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utiltiy_isolate_run_sequence_append                             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_isolate_run_sequence_get                           */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_one_isolate_run_sequence_get(GX_BIDI_CONTEXT *context, GX_UBYTE  pre_level,
                                                          GX_BIDI_LEVEL_RUN *level_run, GX_BIDI_ISOLATE_RUN *isolate_run)
{
UINT          status;
GX_UBYTE      next_level;
GX_UBYTE      run_level;
GX_UBYTE      higher_level;
INT           start_index;
INT           end_index;
UINT          isolate_initiator_count;
GX_BIDI_UNIT *unit;
ULONG         last_character;

    run_level = level_run->level;
    start_index = level_run->start_index;
    end_index = level_run->end_index;

    isolate_run->index_count = 0;

    status = _gx_utiltiy_isolate_run_sequence_append(context, start_index, end_index, isolate_run);

    if (status != GX_SUCCESS)
    {
        return status;
    }

    isolate_run->next = GX_NULL;

    last_character = context->unit_list[end_index].code;

    level_run = level_run->next;
    isolate_initiator_count = 1;

    next_level = (level_run == GX_NULL) ? context->base_level : level_run->level;

    while (level_run)
    {
        /* while the level run last in the sequence ends with an isolate initiator that has a matching PDI,
           append the level run containing the matching PDI to the isolate run sequence. */
        if ((last_character != GX_BIDI_DIRECTION_FORMAT_RLI) &&
            (last_character != GX_BIDI_DIRECTION_FORMAT_LRI) &&
            (last_character != GX_BIDI_DIRECTION_FORMAT_FSI))
        {
            break;
        }

        start_index = level_run->start_index;
        end_index = level_run->end_index;

        unit = &context->unit_list[start_index];
        if (unit->code == GX_BIDI_DIRECTION_FORMAT_PDI)
        {
            isolate_initiator_count--;
            if (isolate_initiator_count == 0)
            {
                /* This is a matching PDI. */
                status = _gx_utiltiy_isolate_run_sequence_append(context, start_index, end_index, isolate_run);

                if (status != GX_SUCCESS)
                {
                    return status;
                }

                last_character = context->unit_list[end_index].code;

                if (level_run->next)
                {
                    next_level = level_run->next->level;
                }
                else
                {
                    next_level = context->base_level;
                }
            }
        }

        unit = &context->unit_list[end_index];

        if (unit->code == GX_BIDI_DIRECTION_FORMAT_LRI ||
            unit->code == GX_BIDI_DIRECTION_FORMAT_RLI ||
            unit->code == GX_BIDI_DIRECTION_FORMAT_FSI)
        {
            isolate_initiator_count++;
        }

        level_run = level_run->next;
    }

    /* Determine sos. */
    higher_level = (pre_level > run_level) ? pre_level : run_level;
    isolate_run->sos = ((higher_level & 0x01) == 1) ? GX_BIDI_CHARACTER_TYPE_R : GX_BIDI_CHARACTER_TYPE_L;

    /* Determine eos. */
    higher_level = (next_level > run_level) ? next_level : run_level;
    isolate_run->eos = ((higher_level & 0x01) == 1) ? GX_BIDI_CHARACTER_TYPE_R : GX_BIDI_CHARACTER_TYPE_L;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_isolate_run_sequences_get                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to calculate the isolate run sequences.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_isolate_run_sequences_get(GX_BIDI_CONTEXT *context)
{
UINT                 status;
INT                  level_run_start_index;
INT                  level_run_end_index;
INT                  isolate_initiator_count = 0;
GX_BIDI_LEVEL_RUN   *level_run_entry;
GX_BIDI_ISOLATE_RUN *isolate_run_entry;
GX_BIDI_ISOLATE_RUN *last_isolate_run_entry = GX_NULL;
GX_UBYTE             pre_level = context->base_level;
GX_BIDI_UNIT        *unit;

    /* Compute level runs. */
    status = _gx_utility_bidi_level_runs_compute(context, 0, context->unit_count - 1);

    if (status != GX_SUCCESS)
    {
        return status;
    }

    /* Compute isolate runs. */
    context->isolate_runs = (GX_BIDI_ISOLATE_RUN *)(context->buffer + context->buffer_index);

    level_run_entry = context->level_runs;
    while (level_run_entry)
    {
        /* For each level run in the paragraph whose first characer is not a PDI,
           or is a PDI that does not match any isolate initiator,
           create a new isolate run sequence and initiate it to the level run. */
        level_run_start_index = level_run_entry->start_index;
        level_run_end_index = level_run_entry->end_index;

        unit = &context->unit_list[level_run_start_index];

        if ((unit->code != GX_BIDI_DIRECTION_FORMAT_PDI) ||
            (isolate_initiator_count == 0))
        {
            isolate_run_entry = (GX_BIDI_ISOLATE_RUN *)(context->buffer + context->buffer_index);
            context->buffer_index += (INT)(sizeof(GX_BIDI_ISOLATE_RUN));
            isolate_run_entry->index_list = (INT *)(context->buffer + context->buffer_index);

            if (context->buffer_index > context->buffer_size)
            {
                return GX_INVALID_MEMORY_SIZE;
            }

            /* Get one isolate run sequence. */
            _gx_utility_bidi_one_isolate_run_sequence_get(context, pre_level, level_run_entry, isolate_run_entry);

            if (last_isolate_run_entry)
            {
                last_isolate_run_entry->next = isolate_run_entry;
            }

            last_isolate_run_entry = isolate_run_entry;
        }

        if (unit->code == GX_BIDI_DIRECTION_FORMAT_PDI)
        {
            isolate_initiator_count--;
        }

        unit = &context->unit_list[level_run_end_index];

        if (unit->code == GX_BIDI_DIRECTION_FORMAT_LRI ||
            unit->code == GX_BIDI_DIRECTION_FORMAT_RLI ||
            unit->code == GX_BIDI_DIRECTION_FORMAT_FSI)
        {
            isolate_initiator_count++;
        }

        pre_level = level_run_entry->level;
        level_run_entry = level_run_entry->next;
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_weak_type_resolve_1                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to apply rule 1 to weak characters in an   */
/*    isolate run sequence.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    entry                                 Pointer to an isolate run     */
/*                                            sequence                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_weak_type_resolve_1(GX_BIDI_CONTEXT *context, GX_BIDI_ISOLATE_RUN *entry)
{
INT           run_index;
INT           text_index;
GX_UBYTE      pre_type = 0;
GX_BIDI_UNIT *unit;

    for (run_index = 0; run_index < entry->index_count; run_index++)
    {
        text_index = entry->index_list[run_index];
        unit = &context->unit_list[text_index];

        if (unit->type == GX_BIDI_CHARACTER_TYPE_NSM)
        {
            if (pre_type == 0)
            {
                /* If the NSM is at the start of the isolating run sequence, it will get the type of sos. */
                unit->type = entry->sos;
            }
            else
            {
                switch (pre_type)
                {
                case GX_BIDI_CHARACTER_TYPE_RLI:
                case GX_BIDI_CHARACTER_TYPE_LRI:
                case GX_BIDI_CHARACTER_TYPE_PDI:
                    /* If the previous character type is an isolate initiator or PDI, change type to ON. */
                    unit->type = GX_BIDI_CHARACTER_TYPE_ON;
                    break;

                default:
                    /* Otherwise, change type to the type of previous character. */
                    unit->type = pre_type;
                    break;
                }
            }
        }

        pre_type = unit->type;
    }
    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_weak_type_resolve_2_3                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to apply rule 2 and 3 to weak characters in*/
/*    an isolate run sequence.                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    entry                                 Pointer to an isolate run     */
/*                                            sequence                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_weak_type_resolve_2_3(GX_BIDI_CONTEXT *context, GX_BIDI_ISOLATE_RUN *entry)
{
INT           run_index = 0;
INT           text_index;
GX_BOOL       do_change = GX_FALSE;
GX_BIDI_UNIT *unit;

    for (run_index = 0; run_index < entry->index_count; run_index++)
    {
        text_index = entry->index_list[run_index];
        unit = &context->unit_list[text_index];

        /* Search backward from each instance of a EN until the first strong type is found,
           If AL is found, replacing following EN to AN. */

        switch (unit->type)
        {
        case GX_BIDI_CHARACTER_TYPE_R:
        case GX_BIDI_CHARACTER_TYPE_L:
            do_change = GX_FALSE;
            break;

        case GX_BIDI_CHARACTER_TYPE_AL:
            do_change = GX_TRUE;

            /* Change all ALs to R. */
            unit->type = GX_BIDI_CHARACTER_TYPE_R;
            break;

        case GX_BIDI_CHARACTER_TYPE_EN:
            if (do_change)
            {
                unit->type = GX_BIDI_CHARACTER_TYPE_AN;
            }
            break;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_weak_type_resolve_4                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to apply rule 4 to weak characters in an   */
/*    isolate run sequence.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    entry                                 Pointer to an isolate run     */
/*                                            sequence                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_weak_type_resolve_4(GX_BIDI_CONTEXT *context, GX_BIDI_ISOLATE_RUN *entry)
{
INT           run_index;
INT           text_index;
GX_UBYTE      pre_type = 0;
GX_UBYTE      follow_type = 0;
GX_BIDI_UNIT *unit;

    /* A single ES between two EN, change to EN,
       a single CS between two EN, change to EN,
       a single CS between two AN, change to AN. */

    for (run_index = 0; run_index < entry->index_count; run_index++)
    {
        if (run_index + 1 < entry->index_count)
        {
            text_index = entry->index_list[run_index + 1];
            follow_type = context->unit_list[text_index].type;
        }

        text_index = entry->index_list[run_index];
        unit = &context->unit_list[text_index];

        if (pre_type == follow_type)
        {
            if (unit->type == GX_BIDI_CHARACTER_TYPE_ES &&
                pre_type == GX_BIDI_CHARACTER_TYPE_EN)
            {
                unit->type = GX_BIDI_CHARACTER_TYPE_EN;
            }
            else if ((unit->type == GX_BIDI_CHARACTER_TYPE_CS) &&
                     (pre_type == GX_BIDI_CHARACTER_TYPE_EN || pre_type == GX_BIDI_CHARACTER_TYPE_AN))
            {
                unit->type = pre_type;
            }
            run_index++;
        }
        else
        {
            pre_type = unit->type;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_weak_type_resolve_5                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to apply rule 5 to weak characters in an   */
/*    isolate run sequence.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    entry                                 Pointer to an isolate run     */
/*                                            sequence                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_weak_type_resolve_5(GX_BIDI_CONTEXT *context, GX_BIDI_ISOLATE_RUN *entry)
{

INT           run_index;
INT           pre_run_index;
INT           text_index;
GX_BOOL       do_change = GX_FALSE;
GX_BIDI_UNIT *unit;

    /* A sequence of ETs adjacent to EN, change to all ENs. */
    for (run_index = 0; run_index < entry->index_count; run_index++)
    {
        text_index = entry->index_list[run_index];
        unit = &context->unit_list[text_index];

        if (unit->type == GX_BIDI_CHARACTER_TYPE_EN)
        {
            do_change = GX_TRUE;

            pre_run_index = run_index - 1;

            while (pre_run_index >= 0)
            {
                text_index = entry->index_list[pre_run_index--];
                unit = &context->unit_list[text_index];

                if (unit->type == GX_BIDI_CHARACTER_TYPE_ET)
                {
                    unit->type = GX_BIDI_CHARACTER_TYPE_EN;
                }
                else
                {
                    break;
                }
            }
        }
        else if (do_change && unit->type == GX_BIDI_CHARACTER_TYPE_ET)
        {
            unit->type = GX_BIDI_CHARACTER_TYPE_EN;
        }
        else
        {
            do_change = GX_FALSE;
        }
    }
    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_weak_type_resolve_6                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to apply rule 6 to weak characters in an   */
/*    isolate run sequence.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    entry                                 Pointer to an isolate run     */
/*                                            sequence                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_weak_type_resolve_6(GX_BIDI_CONTEXT *context, GX_BIDI_ISOLATE_RUN *entry)
{
INT           run_index;
INT           text_index;
GX_BIDI_UNIT *unit;

    /* ES, CS, ET change to ON. */
    for (run_index = 0; run_index < entry->index_count; run_index++)
    {
        text_index = entry->index_list[run_index];
        unit = &context->unit_list[text_index];

        switch (unit->type)
        {
        case GX_BIDI_CHARACTER_TYPE_ES:
        case GX_BIDI_CHARACTER_TYPE_CS:
        case GX_BIDI_CHARACTER_TYPE_ET:
            unit->type = GX_BIDI_CHARACTER_TYPE_ON;
            break;
        }
    }
    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_weak_type_resolve_7                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to apply rule 7 to weak characters in an   */
/*    isolate run sequence.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    entry                                 Pointer to an isolate run     */
/*                                            sequence                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_weak_type_resolve_7(GX_BIDI_CONTEXT *context, GX_BIDI_ISOLATE_RUN *entry)
{
INT           run_index;
INT           text_index;
GX_BOOL       do_change = GX_FALSE;
GX_BIDI_UNIT *unit;

    /* Search backward from each EN until first strong type (R, L, or SOS) is found, if
       L is found, change type to L.*/
    if (entry->sos == GX_BIDI_CHARACTER_TYPE_L)
    {
        do_change = GX_TRUE;
    }
    else
    {
        do_change = GX_FALSE;
    }

    for (run_index = 0; run_index < entry->index_count; run_index++)
    {
        text_index = entry->index_list[run_index];
        unit = &context->unit_list[text_index];

        switch (unit->type)
        {
        case GX_BIDI_CHARACTER_TYPE_R:
            do_change = GX_FALSE;
            break;

        case GX_BIDI_CHARACTER_TYPE_L:
            do_change = GX_TRUE;
            break;

        default:
            if (do_change && unit->type == GX_BIDI_CHARACTER_TYPE_EN)
            {
                unit->type = GX_BIDI_CHARACTER_TYPE_L;
            }
            break;
        }
    }
    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_bracket_pair_search                                */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to calculate bracket pairs in an isolate   */
/*    run sequence.                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    entry                                 Pointer to an isolate run     */
/*                                            sequence                    */
/*    return_bracket_pair                   Buffer to load bracket pair   */
/*                                            list                        */
/*    return_pair_count                     Buffer to load the number of  */
/*                                            bracket pair characters     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_neutral_type_resolve_0                             */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_bracket_pair_search(GX_BIDI_CONTEXT *context, GX_BIDI_ISOLATE_RUN *entry, INT **return_bracket_pair, INT *return_pair_count)
{
ULONG               *stack;
ULONG               *bracket_entry;
ULONG               *last_bracket_entry = GX_NULL;
INT                  top = 0;
INT                  run_index;
INT                  text_index;
ULONG                character;
INT                 *bracket_pair = *return_bracket_pair;
INT                  bracket_pair_count = 0;
GX_BIDI_BRACKET_PAIR pair;
UINT                 buffer_index;
INT                  depth;
INT                  temp = 0;
GX_BIDI_UNIT        *unit;

    stack = (ULONG *)(context->buffer + context->buffer_index);
    buffer_index = context->buffer_index;
    bracket_entry = stack;

    for (run_index = 0; run_index < (INT)entry->index_count; run_index++)
    {
        text_index = entry->index_list[run_index];
        unit = &context->unit_list[text_index];

        if (unit->type != GX_BIDI_CHARACTER_TYPE_ON)
        {
            continue;
        }

        character = unit->code;
        _gx_utility_bidi_bracket_pair_get(character, &pair);

        if (character == pair.open)
        {
            if (top >= 63)
            {
                /* Stop processing for the remainder to the isolating run sequence. */
                break;
            }

            last_bracket_entry = bracket_entry;
            *bracket_entry = (ULONG)run_index;
            bracket_entry++;
            top++;
            buffer_index += (INT)(sizeof(ULONG));
            if (buffer_index > context->buffer_size)
            {
                return GX_INVALID_MEMORY_SIZE;
            }
        }
        else if (last_bracket_entry && (character == pair.close))
        {
            depth = 0;

            while (top - depth > 0)
            {
                text_index = entry->index_list[*(last_bracket_entry - depth)];
                unit = &context->unit_list[text_index];

                if (unit->code == pair.open ||
                    (unit->code == 0x2329 && pair.open == 0x3008) ||
                    (unit->code == 0x3008 && pair.open == 0x2329))
                {
                    bracket_pair[bracket_pair_count++] = (INT)(*(last_bracket_entry - depth));
                    bracket_pair[bracket_pair_count++] = run_index;
                    depth++;

                    last_bracket_entry -= depth;
                    bracket_entry -= depth;
                    top -= depth;
                    buffer_index -= sizeof(ULONG) * (UINT)depth;
                    break;
                }
                depth++;
            }
        }
    }

    /* Sort list of pairs of text position in ascending order
       based on the text position of the openging paired bracket. */
    for (run_index = 0; run_index < bracket_pair_count - 2; run_index += 2)
    {
        for (text_index = run_index + 2; text_index < bracket_pair_count; text_index += 2)
        {
            if (bracket_pair[run_index] > bracket_pair[text_index])
            {
                temp = bracket_pair[run_index];
                bracket_pair[run_index] = bracket_pair[text_index];
                bracket_pair[text_index] = temp;

                temp = bracket_pair[run_index + 1];
                bracket_pair[run_index + 1] = bracket_pair[text_index + 1];
                bracket_pair[text_index + 1] = temp;
            }
        }
    }

    *return_pair_count = bracket_pair_count;
    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_neutral_type_resolve_0                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to apply rule 0 to neutral characters in an*/
/*    isolate run sequence.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    entry                                 Pointer to an isolate run     */
/*                                            sequence                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_neutral_type_resolve_0(GX_BIDI_CONTEXT *context, GX_BIDI_ISOLATE_RUN *entry)
{
UINT          status;
INT           index = 0;
INT           start_index;
INT           end_index;
INT           text_index;
INT          *bracket_pair;
INT           bracket_pair_count = 0;
GX_UBYTE      embedding_type = GX_BIDI_CHARACTER_TYPE_L;
GX_UBYTE      oppsite_type = GX_BIDI_CHARACTER_TYPE_R;
GX_UBYTE      type = GX_BIDI_CHARACTER_TYPE_L;
GX_BOOL       bracket_type_changed;
GX_BOOL       strong_type_found;
GX_BIDI_UNIT *unit;

    if (!context->bracket_pair_size)
    {
        return GX_SUCCESS;
    }

    bracket_pair = (INT *)(context->buffer + context->buffer_index);
    context->buffer_index += context->bracket_pair_size;

    if (context->buffer_index > context->buffer_size)
    {
        return GX_INVALID_MEMORY_SIZE;
    }

    status = _gx_utility_bidi_bracket_pair_search(context, entry, &bracket_pair, &bracket_pair_count);

    if (status != GX_SUCCESS)
    {
        return GX_SUCCESS;
    }

    if (bracket_pair_count > 0)
    {
        text_index = entry->index_list[bracket_pair[0]];
        unit = &context->unit_list[text_index];

        if ((unit->level & 0x01) == 0)
        {
            embedding_type = GX_BIDI_CHARACTER_TYPE_L;
            oppsite_type = GX_BIDI_CHARACTER_TYPE_R;
        }
        else
        {
            embedding_type = GX_BIDI_CHARACTER_TYPE_R;
            oppsite_type = GX_BIDI_CHARACTER_TYPE_L;
        }
    }

    for (index = 0; index < bracket_pair_count; index += 2)
    {
        strong_type_found = GX_FALSE;
        bracket_type_changed = GX_FALSE;
        for (start_index = bracket_pair[index] + 1; start_index < bracket_pair[index + 1]; start_index++)
        {
            text_index = entry->index_list[start_index];
            unit = &context->unit_list[text_index];

            type = unit->type;

            if (type == GX_BIDI_CHARACTER_TYPE_EN ||
                type == GX_BIDI_CHARACTER_TYPE_AN)
            {
                /* Within this scope, bidirectional types EN and AN are treated as R. */
                type = GX_BIDI_CHARACTER_TYPE_R;
            }

            if (type == GX_BIDI_CHARACTER_TYPE_L ||
                type == GX_BIDI_CHARACTER_TYPE_R)
            {
                /* If any strong type (either L or R) matching the embedding direction is found,
                   set the type for both brackets in the pair to match the embedding direction. */
                strong_type_found = GX_TRUE;

                if (type == embedding_type)
                {
                    bracket_type_changed = GX_TRUE;

                    text_index = entry->index_list[bracket_pair[index]];
                    unit = &context->unit_list[text_index];
                    unit->type = embedding_type;

                    text_index = entry->index_list[bracket_pair[index + 1]];
                    unit = &context->unit_list[text_index];
                    unit->type = embedding_type;
                    break;
                }
            }
        }

        if (!bracket_type_changed)
        {
            if (strong_type_found)
            {
                bracket_type_changed = GX_TRUE;

                end_index = bracket_pair[index] - 1;
                strong_type_found = GX_FALSE;
                while (end_index >= 0)
                {
                    text_index = entry->index_list[end_index];
                    unit = &context->unit_list[text_index];

                    /* Search baackward until first strong type is found. */
                    type = unit->type;

                    if (type == GX_BIDI_CHARACTER_TYPE_EN ||
                        type == GX_BIDI_CHARACTER_TYPE_AN)
                    {
                        /* Within this scope, bidirectional types EN and AN are treated as R. */
                        type = GX_BIDI_CHARACTER_TYPE_R;
                    }

                    if (type == GX_BIDI_CHARACTER_TYPE_R ||
                        type == GX_BIDI_CHARACTER_TYPE_L)
                    {
                        strong_type_found = GX_TRUE;
                        break;
                    }

                    end_index--;
                }

                if (!strong_type_found)
                {
                    type = entry->sos;
                }

                if (type != oppsite_type)
                {
                    type = embedding_type;
                }

                text_index = entry->index_list[bracket_pair[index]];
                unit = &context->unit_list[text_index];
                unit->type = type;

                text_index = entry->index_list[bracket_pair[index + 1]];
                unit = &context->unit_list[text_index];
                unit->type = type;
            }
        }

        if (bracket_type_changed)
        {
            /* Any number of characters that had original bidirectional character type NSM that immediately follow a
               paired bracket which changed to L or R under N0 should change to match the type of their preceding bracket.*/
            start_index = bracket_pair[index] + 1;
            while (start_index < bracket_pair[index + 1])
            {
                end_index = entry->index_list[start_index];
                unit = &context->unit_list[end_index];

                if (unit->org_type == GX_BIDI_CHARACTER_TYPE_NSM)
                {
                    unit->type = type;
                }
                else
                {
                    break;
                }
                start_index++;
            }

            start_index = bracket_pair[index + 1] + 1;
            while (start_index < (INT)entry->index_count)
            {
                text_index = entry->index_list[start_index];
                unit = &context->unit_list[text_index];

                if (unit->org_type == GX_BIDI_CHARACTER_TYPE_NSM)
                {
                    unit->type = type;
                }
                else
                {
                    break;
                }
                start_index++;
            }
        }
    }

    context->buffer_index -= context->bracket_pair_size;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_neutral_type_resolve_1                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to apply rule 1 to neutral characters in an*/
/*    isolate run sequence.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    entry                                 Pointer to an isolate run     */
/*                                            sequence                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_neutral_type_resolve_1(GX_BIDI_CONTEXT *context, GX_BIDI_ISOLATE_RUN *entry)
{
INT      run_index;
INT      start_index = 0;
INT      text_index;
GX_UBYTE pre_type = entry->sos;
GX_UBYTE follow_type;
GX_UBYTE type;

    /* A sequence of NIs takes the direction of the surrounding strong text if the text
       on both sides has the same direction. */
    for (run_index = 0; run_index <= (INT)entry->index_count; run_index++)
    {
        if (run_index < (INT)entry->index_count)
        {
            text_index = entry->index_list[run_index];
            type = context->unit_list[text_index].type;
        }
        else
        {
            type = entry->eos;
        }

        switch (type)
        {
        case GX_BIDI_CHARACTER_TYPE_B:
        case GX_BIDI_CHARACTER_TYPE_S:
        case GX_BIDI_CHARACTER_TYPE_WS:
        case GX_BIDI_CHARACTER_TYPE_ON:
        case GX_BIDI_CHARACTER_TYPE_FSI:
        case GX_BIDI_CHARACTER_TYPE_LRI:
        case GX_BIDI_CHARACTER_TYPE_RLI:
        case GX_BIDI_CHARACTER_TYPE_PDI:
            break;

        default:
            follow_type = type;

            if (follow_type == GX_BIDI_CHARACTER_TYPE_EN ||
                follow_type == GX_BIDI_CHARACTER_TYPE_AN)
            {
                follow_type = GX_BIDI_CHARACTER_TYPE_R;
            }

            if (pre_type == follow_type)
            {
                while (start_index < run_index)
                {
                    text_index = entry->index_list[start_index];

                    context->unit_list[text_index].type = pre_type;
                    start_index++;
                }
            }
            pre_type = follow_type;
            start_index = run_index + 1;
            break;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_neutral_type_resolve_2                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to apply rule 2 to neutral characters in an*/
/*    isolate run sequence.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    entry                                 Pointer to an isolate run     */
/*                                            sequence                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_neutral_type_resolve_2(GX_BIDI_CONTEXT *context, GX_BIDI_ISOLATE_RUN *entry)
{
INT           run_index = 0;
INT           text_index;
GX_BIDI_UNIT *unit;

    /* Any remaining NIs take the embedding direction. */
    for (run_index = 0; run_index < entry->index_count; run_index++)
    {
        text_index = entry->index_list[run_index];
        unit = &context->unit_list[text_index];

        switch (unit->type)
        {
        case GX_BIDI_CHARACTER_TYPE_B:
        case GX_BIDI_CHARACTER_TYPE_S:
        case GX_BIDI_CHARACTER_TYPE_WS:
        case GX_BIDI_CHARACTER_TYPE_ON:
        case GX_BIDI_CHARACTER_TYPE_FSI:
        case GX_BIDI_CHARACTER_TYPE_LRI:
        case GX_BIDI_CHARACTER_TYPE_RLI:
        case GX_BIDI_CHARACTER_TYPE_PDI:
            if ((unit->level & 0x01) == 0)
            {
                unit->type = GX_BIDI_CHARACTER_TYPE_L;
            }
            else
            {
                unit->type = GX_BIDI_CHARACTER_TYPE_R;
            }
            break;
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_implicit_level_resolve                             */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to increase the embedding level of text    */
/*    based on the resolved character type.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    entry                                 Pointer to an isolate run     */
/*                                            sequence                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_implicit_level_resolve(GX_BIDI_CONTEXT *context, GX_BIDI_ISOLATE_RUN *entry)
{
INT           run_index;
INT           text_index;
GX_BIDI_UNIT *unit;

    /* Any remaining NIs take the embedding direction. */
    for (run_index = 0; run_index < entry->index_count; run_index++)
    {
        text_index = entry->index_list[run_index];
        unit = &context->unit_list[text_index];

        if ((unit->level & 0x01) == 0)
        {
            /* For all characers with an even embedding level,
                those of type R go up ove level and those of type AN or EN go up two levels. */
            switch (unit->type)
            {
            case GX_BIDI_CHARACTER_TYPE_R:
                unit->level++;
                break;

            case GX_BIDI_CHARACTER_TYPE_AN:
            case GX_BIDI_CHARACTER_TYPE_EN:
                unit->level = (GX_UBYTE)(unit->level + 2);
                break;
            }
        }

        if ((unit->level & 0x01) == 1)
        {
            /* For all characers with an odd embedding level,
                those of type L, EN and AN go up ove level. */
            switch (unit->type)
            {
            case GX_BIDI_CHARACTER_TYPE_L:
            case GX_BIDI_CHARACTER_TYPE_EN:
            case GX_BIDI_CHARACTER_TYPE_AN:
                unit->level++;
                break;
            }
        }
    }

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_isolate_run_sequences_resolve                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to determine the embedding levels of the   */
/*    text.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_isolate_run_sequences_resolve(GX_BIDI_CONTEXT *context)
{
UINT                 status = GX_SUCCESS;

GX_BIDI_ISOLATE_RUN *entry = context->isolate_runs;

    while (entry)
    {
        status = _gx_utility_bidi_weak_type_resolve_1(context, entry);

        if (status != GX_SUCCESS)
        {
            break;
        }

        status = _gx_utility_bidi_weak_type_resolve_2_3(context, entry);

        if (status != GX_SUCCESS)
        {
            break;
        }
        status = _gx_utility_bidi_weak_type_resolve_4(context, entry);

        if (status != GX_SUCCESS)
        {
            break;
        }
        status = _gx_utility_bidi_weak_type_resolve_5(context, entry);

        if (status != GX_SUCCESS)
        {
            break;
        }
        status = _gx_utility_bidi_weak_type_resolve_6(context, entry);

        if (status != GX_SUCCESS)
        {
            break;
        }

        status = _gx_utility_bidi_weak_type_resolve_7(context, entry);

        if (status != GX_SUCCESS)
        {
            break;
        }

        /* Process bracket pairs. */
        status = _gx_utility_bidi_neutral_type_resolve_0(context, entry);

        if (status != GX_SUCCESS)
        {
            break;
        }

        status = _gx_utility_bidi_neutral_type_resolve_1(context, entry);

        if (status != GX_SUCCESS)
        {
            break;
        }

        status = _gx_utility_bidi_neutral_type_resolve_2(context, entry);

        if (status != GX_SUCCESS)
        {
            break;
        }

        status = _gx_utility_bidi_implicit_level_resolve(context, entry);

        if (status != GX_SUCCESS)
        {
            break;
        }

        entry = entry->next;
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_reordering_resolve_1                               */
/*                                                           6.1.9        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to reset the embedding level of some       */
/*    characters in specified text block.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*    start_index                           Start index of the text block */
/*    end_index                             End index of the text block   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_reordering_resolve_1(GX_BIDI_CONTEXT *context, INT start_index, INT end_index)
{
INT           index;
INT           pre_index;
GX_BOOL       check_before;
GX_BIDI_UNIT *unit;
GX_BIDI_UNIT *pre_unit;
USHORT        mirror;

    for (index = start_index; index <= end_index; index++)
    {
        check_before = GX_FALSE;
        unit = &context->unit_list[index];

        switch (unit->org_type)
        {
        case GX_BIDI_CHARACTER_TYPE_S:
        case GX_BIDI_CHARACTER_TYPE_B:
            /* Reset segment separators and paragraph separators to the paragraph embedding level. */
            unit->level = context->base_level;
            check_before = GX_TRUE;
            break;

        default:
            if (index == context->unit_count - 1)
            {
                index = context->unit_count;
                check_before = GX_TRUE;
            }
            break;
        }

        if (check_before)
        {
            /* Any sequence of whitespace or isolate formatting characters preceding a
               segment/paragraph separator or at the end of the text, reset to paragraph level. */
            pre_index = index - 1;
            pre_unit = &context->unit_list[pre_index];
            while (pre_index >= 0)
            {
                if (pre_unit->type == GX_BIDI_CHARACTER_TYPE_BN)
                {
                    pre_index--;
                    pre_unit--;
                    continue;
                }

                switch (pre_unit->org_type)
                {
                case GX_BIDI_CHARACTER_TYPE_WS:
                case GX_BIDI_CHARACTER_TYPE_FSI:
                case GX_BIDI_CHARACTER_TYPE_LRI:
                case GX_BIDI_CHARACTER_TYPE_RLI:
                case GX_BIDI_CHARACTER_TYPE_PDI:
                    pre_unit->level = context->base_level;
                    break;

                default:
                    pre_index = -1;
                    break;
                }
                pre_index--;
                pre_unit--;
            }
        }

        if (unit->type == GX_BIDI_CHARACTER_TYPE_R &&
            unit->code < 0xffff)
        {
            /* Mirror */
            _gx_utility_bidi_mirroring_get((USHORT)unit->code, &mirror);

            if (mirror)
            {
                unit->code = mirror;
            }
        }
    }
    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_reordering_resolve_2                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to reverse text for the specified text     */
/*    block.                                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*    start_index                           Start index of the text block */
/*    end_index                             End index of the text block   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_reordering_resolve_2(GX_BIDI_CONTEXT *context, INT start_index, INT end_index)
{
INT                max_level = 0;
INT                min_level = 999;
INT                level;
INT                index;
INT                run_start_index;
INT                run_end_index;
GX_BIDI_LEVEL_RUN *entry;
GX_BIDI_LEVEL_RUN *next_entry;
GX_BIDI_UNIT      *unit;
GX_BIDI_UNIT      *tail_unit;
GX_BIDI_UNIT       temp;
INT                count;

    _gx_utility_bidi_level_runs_compute(context, start_index, end_index);

    /* Find max level. */
    entry = context->level_runs;
    if (entry)
    {
        unit = &context->unit_list[entry->start_index];
        max_level = unit->level;
        min_level = max_level;
        entry = entry->next;
    }

    while (entry)
    {
        unit = &context->unit_list[entry->start_index];

        if (unit->level > max_level)
        {
            max_level = unit->level;
        }
        else if (unit->level < min_level)
        {
            min_level = unit->level;
        }
        entry = entry->next;
    }

    if ((min_level & 0x01) == 0)
    {
        min_level += 1;
    }

    /* From highest level, reverse any contiguous sequence of characters that are at that level or higher. */
    for (level = max_level; level >= min_level; level--)
    {
        entry = context->level_runs;

        while (entry)
        {
            run_start_index = entry->start_index;
            run_end_index = entry->end_index;

            next_entry = entry->next;
            unit = &context->unit_list[run_start_index];

            if (unit->level >= level)
            {
                while (next_entry &&
                       context->unit_list[next_entry->start_index].level >= level)
                {
                    run_end_index = next_entry->end_index;
                    next_entry = next_entry->next;
                }

                count = (run_end_index - run_start_index + 1) >> 1;

                /* Reverse characters. */
                unit = &context->unit_list[run_start_index];
                tail_unit = &context->unit_list[run_end_index];

                for (index = 0; index < count; index++)
                {
                    temp = *unit;
                    *unit = *tail_unit;
                    *tail_unit = temp;

                    unit++;
                    tail_unit--;
                }
            }
            entry = next_entry;
        }
    }

    context->buffer_index -= sizeof(GX_BIDI_LEVEL_RUN) * (UINT)context->unit_count;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_line_break                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to break text into lines.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_line_break(GX_BIDI_CONTEXT *context)
{
GX_BIDI_TEXT_INFO *input_info = context->input_info;
INT                index = 0;
GX_STRING          ch;
UINT               glyph_len;
GX_VALUE           glyph_width;
UINT               line = 0;
GX_BIDI_UNIT      *unit;
GX_UBYTE           utf8[6];
INT                display_number = 0;
INT                display_width = 0;
INT                line_break_display_number = 0;
INT                line_break_display_width = 0;
INT                line_index = -1;

    unit = context->unit_list;

    if ((!input_info->font) || (input_info->display_width <= 0))
    {
        context->total_lines = 1;
        return GX_SUCCESS;
    }

    context->line_index_cache = (INT *)(context->buffer + context->buffer_index);
    context->line_index_cache[0] = 0;

    for (index = 0; index < context->unit_count; index++)
    {
        _gx_utility_unicode_to_utf8(unit->code, utf8, &glyph_len);

        ch.ptr = (GX_CHAR *)utf8;
        ch.length = glyph_len;
        _gx_system_string_width_get_ext(input_info->font, &ch, &glyph_width);

        if ((display_width + glyph_width > input_info->display_width) &&
            (display_number > 0) &&
            (ch.ptr[0] != ' '))
        {
            /* Breadk line. */
            if (line_break_display_number)
            {
                line_index += line_break_display_number;
                display_number -= line_break_display_number;
                display_width -= line_break_display_width;
            }
            else
            {
                line_index += display_number;
                display_number = 0;
                display_width = 0;
            }

            context->line_index_cache[line++] = line_index;
            line_break_display_number = 0;
            line_break_display_width = 0;
        }

        display_width += glyph_width;
        display_number++;

        if ((ch.ptr[0] == ' ') ||
            (ch.ptr[0] == ',') ||
            (ch.ptr[0] == ';'))
        {
            line_break_display_number = display_number;
            line_break_display_width = display_width;
        }

        unit++;
    }

    context->line_index_cache[line++] = context->unit_count - 1;
    context->total_lines = line;

    context->buffer_index += sizeof(INT *) * line;

    return GX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_reordering_resolve                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function to reorder text based on resolved embedding*/
/*    levels or each line.                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Bidi information control block*/
/*    total_lines                           Total lines of the text       */
/*    returned_utf8_text                    Pointer to the utf8 string of */
/*                                            the first line, following   */
/*                                            lines are linked one after  */
/*                                            another.                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_reordering_resolve(GX_BIDI_CONTEXT *context, GX_BIDI_RESOLVED_TEXT_INFO **resolved_info)
{
UINT                        status = GX_SUCCESS;
INT                         start_index = 0;
INT                         end_index;
UINT                        line_index;
GX_STRING                  *out_text_list = GX_NULL;
GX_CHAR                    *line_text;
ULONG                       byte_size;
INT                         index;
UINT                        glyph_count;
GX_BIDI_UNIT               *unit;
GX_BIDI_RESOLVED_TEXT_INFO *bidi_text;

    byte_size = context->total_lines * (sizeof(GX_BIDI_RESOLVED_TEXT_INFO) + sizeof(GX_STRING));
    byte_size += context->reordered_utf8_size + context->total_lines;

    bidi_text = (GX_BIDI_RESOLVED_TEXT_INFO *)_gx_system_memory_allocator(byte_size);

    if (!bidi_text)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    memset(bidi_text, 0, (size_t)byte_size);

    bidi_text->text = (GX_STRING *)(bidi_text + 1);
    bidi_text->total_lines = context->total_lines;

    *resolved_info = bidi_text;

    out_text_list = bidi_text->text;
    line_text = (GX_CHAR *)(out_text_list + context->total_lines);


    for (line_index = 0; line_index < context->total_lines; line_index++)
    {
        if (context->total_lines == 1)
        {
            end_index = context->unit_count - 1;
        }
        else
        {
            end_index = context->line_index_cache[line_index];
        }

        status = _gx_utility_bidi_reordering_resolve_1(context, start_index, end_index);

        if (status == GX_SUCCESS)
        {
            status = _gx_utility_bidi_reordering_resolve_2(context, start_index, end_index);
        }

        if (status == GX_SUCCESS)
        {
            unit = &context->unit_list[start_index];

            out_text_list->ptr = line_text;

            /* Convert unicode to utf8 text. */
            for (index = start_index; index <= end_index; index++)
            {
                switch (unit->type)
                {
                case GX_BIDI_CHARACTER_TYPE_BN:
                    /* Skip above characters. */
                    break;

                default:
                    if (unit->code)
                    {
                        if (unit->code < 0x80)
                        {
                            *(GX_UBYTE *)(line_text) = (GX_UBYTE)(unit->code);
                            glyph_count = 1;
                        }
                        else
                        {

                            _gx_utility_unicode_to_utf8(unit->code, (GX_UBYTE *)line_text, &glyph_count);
                        }
                        line_text += glyph_count;
                        out_text_list->length += glyph_count;
                    }
                    break;
                }

                unit++;
            }

            line_text++;
            out_text_list++;
        }

        start_index = end_index + 1;
    }

    return status;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_one_paragraph_reorder                              */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reorders a bidi text for displaying.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    text_info                             Pointer to bidi text          */
/*    reordered_text                        Reordered text, each line is  */
/*                                            ended a with string         */
/*                                            terminator '\0', multi line */
/*                                            strings are linked one after*/
/*                                            another                     */
/*    processed_count                       The processed text size in    */
/*                                            byte                        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                                        */
/**************************************************************************/
static UINT _gx_utility_bidi_one_paragraph_reorder(GX_BIDI_TEXT_INFO *input_info, GX_BIDI_RESOLVED_TEXT_INFO **resolved_info, UINT *processed_size)
{
UINT            status;
GX_BIDI_CONTEXT context;

    memset(&context, 0, sizeof(GX_BIDI_CONTEXT));
    context.input_info = input_info;

    /* Allocate buffer needed for bidi text reordering. */
    status = _gx_utility_bidi_buffer_allocate(&context);

    if (status == GX_SUCCESS)
    {
        status = _gx_utility_bidi_initiate(&context);
    }

    if (status == GX_SUCCESS)
    {
        switch (input_info->direction)
        {
        case GX_LANGUAGE_DIRECTION_LTR:
            context.base_level = 0;
            break;

        case GX_LANGUAGE_DIRECTION_RTL:
            context.base_level = 1;
            break;

        default:
            /* Compute paragraph embedding_level. */
            status = _gx_utility_bidi_block_level_determine(&context, 0,
                                                            (UINT)(context.unit_count - 1),
                                                            &context.base_level);
            break;
        }
    }

    if (status == GX_SUCCESS)
    {
        /* Compute explicie level for every character. */
        status = _gx_utility_bidi_explicit_levels_determine(&context);
    }

    if (status == GX_SUCCESS)
    {
        /* Compute isolate run sequences. */
        status = _gx_utility_bidi_isolate_run_sequences_get(&context);
    }

    if (status == GX_SUCCESS)
    {
        /* Apply rules to each isolate run sequence. */
        status = _gx_utility_bidi_isolate_run_sequences_resolve(&context);
    }

#if defined(GX_DYNAMIC_ARABIC_SHAPING_SUPPORT)
    if (status == GX_SUCCESS)
    {
        status = _gx_utility_bidi_arabic_shaping(&context);
    }
#endif

    if (status == GX_SUCCESS)
    {
        /* Broke paragraph text into lines. */
        status = _gx_utility_bidi_line_break(&context);
    }

    if (status == GX_SUCCESS)
    {
        /* Reorder text of each line for display. */
        status = _gx_utility_bidi_reordering_resolve(&context, resolved_info);
    }

    if (status == GX_SUCCESS)
    {
        if (processed_size)
        {
            *processed_size = context.processced_size;
        }

        _gx_system_memory_free(context.buffer);
    }

    return status;
}



/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder                                  */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reorders a bidi text for displaying.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input_info                            Pointer to bidi text info     */
/*    reordered_text                        Reordered text information    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_bidi_paragraph_reorder(GX_BIDI_TEXT_INFO *input_info, GX_BIDI_RESOLVED_TEXT_INFO **resolved_info_head)
{
    input_info->direction = -1;

    return _gx_utility_bidi_paragraph_reorder_ext(input_info, resolved_info_head);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_paragraph_reorder_ext                              */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reorders a bidi text for displaying.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    input_info                            Pointer to bidi text info     */
/*    reordered_text                        Reordered text information    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _gx_utility_bidi_paragraph_reorder_ext(GX_BIDI_TEXT_INFO *input_info, GX_BIDI_RESOLVED_TEXT_INFO **resolved_info_head)
{
UINT                        status = GX_SUCCESS;
GX_BIDI_TEXT_INFO           text_info;
GX_BIDI_RESOLVED_TEXT_INFO *resolved_info;
GX_BIDI_RESOLVED_TEXT_INFO *head = GX_NULL;
GX_BIDI_RESOLVED_TEXT_INFO *pre = GX_NULL;
UINT                        line_size;
UINT                        line_break_counts = 0;
GX_STRING                   string = input_info->text;

    if (!_gx_system_memory_allocator)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    text_info = *input_info;

    while (string.length > 0)
    {
        if (string.ptr[0] == GX_KEY_CARRIAGE_RETURN)
        {
            if ((string.length > 1) && (string.ptr[1] == GX_KEY_LINE_FEED))
            {
                line_size = 2;
            }
            else
            {
                line_size = 1;
            }
            line_break_counts++;
        }
        else if (string.ptr[0] == GX_KEY_LINE_FEED)
        {
            line_size = 1;
            line_break_counts++;
        }
        else
        {
            line_size = 0;
        }

        if (((line_break_counts > 1) && (!line_size)) ||
            ((line_break_counts > 0) && (line_size == string.length)))
        {
            /* Collect blank lines as one bidi resolved text info instance. */
            resolved_info = (GX_BIDI_RESOLVED_TEXT_INFO *)_gx_system_memory_allocator(sizeof(GX_BIDI_RESOLVED_TEXT_INFO));
            if (!resolved_info)
            {
                status = GX_SYSTEM_MEMORY_ERROR;
                break;
            }

            memset(resolved_info, 0, sizeof(GX_BIDI_RESOLVED_TEXT_INFO));

            if (!line_size)
            {
                resolved_info->total_lines = (UINT)(line_break_counts - 1);
            }
            else
            {
                resolved_info->total_lines = line_break_counts;
            }

            GX_LINK_RESOLVED_BIDI_TEXT_INFO
        }

        if (!line_size)
        {
            /* Start bidi text reorderding for one paragraph. */
            text_info.text = string;
            status = _gx_utility_bidi_one_paragraph_reorder(&text_info, &resolved_info, &line_size);

            if (status != GX_SUCCESS)
            {
                break;
            }

            GX_LINK_RESOLVED_BIDI_TEXT_INFO

            line_break_counts = 0;
        }

        /* Increment string pointer and decrement string length. */
        string.ptr += line_size;
        string.length -= line_size;
    }

    if (status == GX_SUCCESS)
    {
        *resolved_info_head = head;
    }
    else if (head)
    {
        /* Clean generated resolved bidi text link. */
        _gx_utility_bidi_resolved_text_info_delete(&head);
    }

    return status;
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_utility_bidi_resolved_text_info_delete                          */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deletes a reordered bidi text information link.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    reordered_text                        The head pointer of reordered */
/*                                            bidi text information link  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_DYNAMIC_BIDI_TEXT_SUPPORT)
UINT _gx_utility_bidi_resolved_text_info_delete(GX_BIDI_RESOLVED_TEXT_INFO **resolved_info_head)
{
GX_BIDI_RESOLVED_TEXT_INFO *info;
GX_BIDI_RESOLVED_TEXT_INFO *next;

    if (!_gx_system_memory_free)
    {
        return GX_SYSTEM_MEMORY_ERROR;
    }

    info = *resolved_info_head;

    while (info)
    {
        next = info->next;
        _gx_system_memory_free(info);
        info = next;
    }

    *resolved_info_head = GX_NULL;

    return GX_SUCCESS;
}
#endif

