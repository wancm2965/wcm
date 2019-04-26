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

#include "rvstdio.h"
#include "emanag.h"

#define MAX_E_SYSTEMS (2)
static emTypeOfEncoding eSystems[MAX_E_SYSTEMS];


int emSetEncoding(
    IN  int                 privateTag,
    IN  emTypeOfEncoding*   encoding)
{
    eSystems[privateTag] = *encoding;
    return 0;
}


int emEncode(
    IN  emanagStruct    *emanag,
    IN  RvPvtNodeId     vNodeId,
    OUT int             *encoded)
{
    int eSystem = (int)emPER;

#if (RV_ASN1_CODER_USE_Q931 == RV_YES)
    int nodeId = -1;
    int tag;
    pstTagClass tagClass;

    pvtGet(emanag->hVal, vNodeId, NULL, &nodeId, NULL, NULL);
    if (nodeId < 0)
        return nodeId;
    tag = pstGetTag(emanag->hSyn, nodeId, &tagClass);

    eSystem = (int)(tagClass==pstTagPrivate) ? tag : (int)emPER;
#endif

    if (eSystems[eSystem].Encode != NULL)
    {
        /* Encode it */
        return eSystems[eSystem].Encode(emanag, vNodeId, encoded);
    }
    return 0;
}


int emDecode(
    IN  emanagStruct    *emanag,
    IN  RvPvtNodeId     vNodeId,
    IN  RvInt32         fieldId,
    OUT int             *decoded)
{
    int eSystem = (int)emPER;

    if (emanag->hSyn == NULL)
    {
        /* No syntax information in the given node id! */
        return RV_ERROR_UNKNOWN;
    }

#if (RV_ASN1_CODER_USE_Q931 == RV_YES)
    {
        int nodeId,tag;
        pstTagClass tagClass;

        /* Find out which decoding system we have to use */
        pvtGet(emanag->hVal, vNodeId, NULL, &nodeId, NULL, NULL);
        tag = pstGetTag(emanag->hSyn, nodeId, &tagClass);

        eSystem = (int)(tagClass==pstTagPrivate) ? tag : (int)emPER;
    }
#endif

    if (eSystems[eSystem].Decode != NULL)
    {
        /* Decode it */
        return eSystems[eSystem].Decode(emanag, vNodeId, fieldId, decoded);
    }
    return RV_ERROR_NOTSUPPORTED;
}



#ifdef __cplusplus
}
#endif



