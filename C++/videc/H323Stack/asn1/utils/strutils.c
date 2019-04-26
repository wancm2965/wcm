/***********************************************************************
        Copyright (c) 2003 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************

  strutils.c  --  String Utilities

  Module Authors: Oz Solomonovich, Sasha Ruditsky
  This Comment:   1-Jan-1997

  Abstract:       Various string manipulation utilities.

  Platforms:      All.

  Known Bugs:     None.

****************************************************************************/


#include "rvstdio.h"
#include <ctype.h>
#include "strutils.h"


RVINTAPI int RVCALLCONV
chrn2bmp(IN const RvChar *str, IN RvSize_t maxStrLen, OUT RvUint8 *bmpStr)
{
    RvSize_t i, i2;

    for (i = 0, i2 = 0; i < maxStrLen; i++, i2 += 2)
    {
        bmpStr[i2 + 0] = 0;
        bmpStr[i2 + 1] = (RvUint8)str[i];
    }

    return i2;
}

RVINTAPI int RVCALLCONV
chr2bmp(IN const RvChar *str, OUT RvUint8 *bmpStr)
{
    RvSize_t len = strlen(str);
    return chrn2bmp(str, len, bmpStr);
}

RVINTAPI int RVCALLCONV
bmp2chr(OUT RvChar *str, IN const RvUint8 *bmpStr, IN RvSize_t bmpLen)
{
    RvSize_t i, i2;

    for (i = 0, i2 = 0; i < bmpLen/2; i++, i2 += 2)
    {
        if (bmpStr[i2])
        {
            str[i] = 0;
            return RV_ERROR_UNKNOWN;
        }
        str[i] = bmpStr[i2 + 1];
    }

    str[i] = 0;
    return 0;
}


/* case insensitive strncmp */
int strncmp_ci(const RvChar *dst, const RvChar *src, RvSize_t count)
{
    int f, l;
    int result = 0;

    if (count)
    {
        do
        {
            f = tolower((int)*dst);
            l = tolower((int)*src);
            dst++;
            src++;
        } while (--count && f && l && f == l);

        result = f - l;
    }
    return(result);
}


#ifdef __cplusplus
}
#endif



