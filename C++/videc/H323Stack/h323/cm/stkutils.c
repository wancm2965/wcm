/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

/***************************************************************************

  stkutils.c  --  Exported Stack Utilities

  Abstract:       This is an wrapper module for the various stack utilities
                  that we export.

***************************************************************************/


#include "stkutils.h"
#include "oidutils.h"
#include "strutils.h"

#ifdef __cplusplus
extern "C" {
#endif


                        /* == OID Manipulation == */

RVAPI
int RVCALLCONV utlEncodeOID(
            IN    int           oidSize,
            OUT   char *        oid,
            IN    const char *  buff)
{
    return oidEncodeOID(oidSize, oid, buff);
}

RVAPI
int RVCALLCONV utlDecodeOID(
        IN   int            oidSize,
        IN   const char *   oid,
        IN   int            buffSize,
        OUT  char *         buff,
        IN   OID_form       f)
{
    return oidDecodeOID(oidSize, (char *)oid, buffSize, buff, (form)f);
}


                     /* == BMP String Manipulation == */

/* ascii->bmp string - returns bytes written to target buffer */
RVAPI
int RVCALLCONV utlChr2Bmp(
            IN   const char *   str,
            OUT  RvUint8 *      bmpStr)
{
    return chr2bmp((char *)str, bmpStr);
}

/* same as chr2bmp, with maximum length */
RVAPI
int RVCALLCONV utlChrn2Bmp(
            IN   const char *   str,
            IN   int            maxStrLen,
            OUT  RvUint8 *      bmpStr)
{
    return chrn2bmp((char *)str, (RvSize_t)maxStrLen, bmpStr);
}

/* bmp->ascii string - returns RV_ERROR_UNKNOWN if conversion was unsuccessful */
RVAPI
int RVCALLCONV utlBmp2Chr(
            OUT  char *         str,
            IN   const RvUint8* bmpStr,
            IN   int            bmpLen)
{
    return bmp2chr(str, (RvUint8 *)bmpStr, (RvSize_t)bmpLen);
}



#ifdef __cplusplus
}
#endif



