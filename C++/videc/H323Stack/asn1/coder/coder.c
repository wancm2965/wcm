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

#include "emanag.h"
#include "q931.h"
#include "per.h"
#include "pvaltree.h"
#include "coder.h"


#ifdef __cplusplus
extern "C" {
#endif


static int coderTimersInitialized = 0;

RVAPI
int RVCALLCONV cmEmEncode(
            IN      HPVT            valH,
            IN      RvPvtNodeId     vNodeId,
            OUT     RvUint8*        buffer,
            IN      int             length,
            OUT     int*            encoded)
{
    if (RV_PVT_NODEID_IS_VALID(vNodeId) && (buffer != NULL))
    {
        emanagStruct emanag;

        emanag.hVal = valH;
        emanag.hSyn = pvtGetSynTree(valH, vNodeId);
        emanag.buffer = buffer;
        emanag.length = length * 8;
        emanag.isTolerant = RV_FALSE;

        return emEncode(&emanag, vNodeId, encoded);
    }

    return vNodeId;
}


RVAPI
int RVCALLCONV cmEmEncodeTolerant(
            IN      HPVT            valH,
            IN      RvPvtNodeId     vNodeId,
            OUT     RvUint8*        buffer,
            IN      int             length,
            OUT     int*            encoded)
{
    if (RV_PVT_NODEID_IS_VALID(vNodeId) && (buffer != NULL))
    {
        emanagStruct emanag;

        emanag.hVal = valH;
        emanag.hSyn = pvtGetSynTree(valH, vNodeId);
        emanag.buffer = buffer;
        emanag.length = length * 8;
        emanag.isTolerant = RV_TRUE;

        return emEncode(&emanag, vNodeId, encoded);
    }

    return vNodeId;
}


RVAPI
int RVCALLCONV cmEmDecode(
            IN      HPVT            valH,
            IN      RvPvtNodeId     vNodeId,
            IN      RvUint8*        buffer,
            IN      int             length,
            OUT     int*            decoded)
{
    if (RV_PVT_NODEID_IS_VALID(vNodeId) && (buffer != NULL))
    {
        emanagStruct emanag;

        emanag.hVal = valH;
        emanag.hSyn = pvtGetSynTree(valH, vNodeId);
        emanag.buffer = buffer;
        emanag.length = length * 8;
        emanag.isTolerant = RV_FALSE;

        return emDecode(&emanag, vNodeId, -1, decoded);
    }

    return vNodeId;
}



/************************************************************************
 * cmEmInstall
 * purpose: Initialize the encode/decode manager.
 *          This function should be called by applications that want to
 *          encode and decode ASN.1 messages, but don't want to initialize
 *          and use the CM for that purpose.
 * input  : maxBufSize  - Maximum size of buffer supported (messages larger
 *                        than this size in bytes cannot be decoded/encoded).
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmEmInstall(IN int maxBufSize)
{
    if (coderTimersInitialized == 0)
    {
#if (RV_ASN1_CODER_USE_Q931 == RV_YES)
        q931coderInit();
#endif /* (RV_ASN1_CODER_USE_Q931 == RV_YES) */
        perConstruct(maxBufSize);
    }
    coderTimersInitialized++;

    return 0;
}


/************************************************************************
 * cmEmEnd
 * purpose: Un-initialize the encode/decode manager.
 *          This function should be called by applications that called
 *          cmEmInstall() directly to clean up used memory.
 * input  : none
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmEmEnd(void)
{
    coderTimersInitialized--;
    if (coderTimersInitialized == 0)
    {
        perDestruct();
#if (RV_ASN1_CODER_USE_Q931 == RV_YES)
        q931coderEnd();
#endif /* (RV_ASN1_CODER_USE_Q931 == RV_YES) */
    }

    return 0;
}



#ifdef __cplusplus
}
#endif
