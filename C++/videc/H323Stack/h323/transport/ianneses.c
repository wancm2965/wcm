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

#include "rvinternal.h"
#include "rvtimestamp.h"
#include "annexe.h"
#include "iannexe.h"
#include "hash.h"

#ifdef __cplusplus
extern "C" {
#endif

RvUint32 condenseIpTo32bit(IN cmTransportAddress* cmAddress);
RvBool isSameAddress(IN cmTransportAddress* addr1, IN cmTransportAddress* addr2);



tNode * get_node(tAnnexE* pAnnexE, cmTransportAddress * ta, RvUint8 localAddrIndex)
{
    tNodeKey    NodeKey;
    tNode*      pNode = NULL;
    void*       loc;

    NodeKey.ipNum          = condenseIpTo32bit(ta);
    NodeKey.nPort          = ta->port;
    NodeKey.localAddrIndex = localAddrIndex;

    for (loc = hashFind(pAnnexE->ActiveNodesHash,(void *)&NodeKey);
         loc != NULL;
         loc = hashFindNext(pAnnexE->ActiveNodesHash,(void *)&NodeKey, loc))
        {
            pNode = *(tNode **)hashGetElement(pAnnexE->ActiveNodesHash, loc);
            if (isSameAddress(ta, &pNode->RemoteHost))
                break;
            else
                pNode = NULL;
        }

    return pNode;
}

tNode * add_node(tAnnexE* pAnnexE, cmTransportAddress * ta, RvUint8 localAddrIndex)
{

    tNodeKey    NodeKey;
    tNode*      pNode = NULL;

#if defined(RV_ANNEXE_DEBUG)
    if (get_node(pAnnexE, ta, localAddrIndex) != NULL)
    {
        RvLogExcep(&pAnnexE->log, (&pAnnexE->log,
            "add_node called when such a node is already in the database!"));
        return NULL;
    }
#endif

    NodeKey.ipNum          = condenseIpTo32bit(ta);
    NodeKey.nPort          = ta->port;
    NodeKey.localAddrIndex = localAddrIndex;

    if (!IsListEmpty( &pAnnexE->FreeNodesList ))
    {
        RV_PLIST_ENTRY plink;

        plink   = RemoveHeadList( &pAnnexE->FreeNodesList );
        pNode   = RV_GET_STRUCT(tNode, lNodeLink, plink);

        pNode->nRef             = 1;
        memcpy(&pNode->RemoteHost, ta, sizeof(pNode->RemoteHost));
        pNode->nLast_Ack_SEQ    = 0xffffffff;
        pNode->nNext_SEQ        = RvUint64ToRvUint32(RvInt64Div(RvTimestampGet(NULL), RV_TIME64_NSECPERMSEC)) & 0xffffff;
        pNode->nRetry           = 0;
        pNode->fDontSend        = RV_FALSE;
        pNode->localAddrIndex   = localAddrIndex;

        if (hashAdd(pAnnexE->ActiveNodesHash, &NodeKey, &pNode, RV_FALSE) == NULL)
        {
            /* Can't add to list? */
            InsertTailList( &pAnnexE->FreeNodesList, &pNode->lNodeLink );

            RvLogExcep(&pAnnexE->log, (&pAnnexE->log,
                "add_node failed in adding element to the hash table"));
            return NULL;
        }
    };

    return pNode;
}

void
del_node( tNode* pNode )
{
    tNodeKey    NodeKey;
    tAnnexE*    pAnnexE = pNode->pAnnexE;
    void*       loc;

#if defined(RV_ANNEXE_DEBUG)
    if ( pNode != get_node(pNode->pAnnexE, &pNode->RemoteHost, (RvUint8)pNode->localAddrIndex) )
    {
        RvLogExcep(&pAnnexE->log, (&pAnnexE->log,
            "del_node: bad node in database"));
        return;
    }

    if (pNode->nRef <= 0)
    {
        RvLogExcep(&pAnnexE->log, (&pAnnexE->log,
            "del_node: reference count=%d", pNode->nRef));
        return;
    }
#endif

    pNode->nRef--;

    if (pNode->nRef > 0)
        return;

    RvH323TimerCancel(pNode->pAnnexE->hTimers, &pNode->hResendAndIMATimer);

    if (pNode->pCurrentPDU != NULL) {
        AEHClr_A( pNode->pCurrentPDU->PDU ); /* !!!! */
        send_current_pdu( pNode );
    }

    if (pNode->pWaitingForAckPDU != NULL)
    {
        tPDU* pPDU = pNode->pWaitingForAckPDU;
        pNode->pWaitingForAckPDU = NULL;
        pNode->nRef++; /* protect node */
        free_pdu( pAnnexE, pPDU );
        pNode->nRef--; /* unprotect node */
    }

#if defined(RV_ANNEXE_DEBUG)
    if ((pNode->pCurrentPDU != NULL) || (pNode->pWaitingForAckPDU != NULL))
    {
        RvLogExcep(&pAnnexE->log, (&pAnnexE->log,
            "del_node: pNode element has a current PDU in process or is waiting for ack"));
        return;
    }
#endif
    NodeKey.ipNum = condenseIpTo32bit(&pNode->RemoteHost);
    NodeKey.nPort = pNode->RemoteHost.port;
    NodeKey.localAddrIndex = (RvUint8)pNode->localAddrIndex;

    for (loc = hashFind(pAnnexE->ActiveNodesHash,(void *)&NodeKey);
         loc != NULL;
         loc = hashFindNext(pAnnexE->ActiveNodesHash,(void *)&NodeKey, loc))
        {
            tNode * tmpNode = *(tNode **)hashGetElement(pAnnexE->ActiveNodesHash, loc);
            if (isSameAddress(&tmpNode->RemoteHost, &pNode->RemoteHost))
                break;
        }

    if (loc)
        hashDelete(pAnnexE->ActiveNodesHash, loc);

    InsertTailList(&pNode->pAnnexE->FreeNodesList, &pNode->lNodeLink );
}

void* remove_one_node(HHASH hHash, void* elem, void *param) {
    tNode* pNode;
    pNode = *(tNode**)hashGetElement(hHash, elem);

    if ( (pNode) && (pNode->pAnnexE->events.AnnexEEvConnectionBroken != NULL)) {
        pNode->pAnnexE->events.AnnexEEvConnectionBroken(
                AsHANNEXE( pNode->pAnnexE ),
                pNode->pAnnexE->hAppAnnexE,
                &pNode->RemoteHost, pNode->localAddrIndex
            );
    }
    else
        del_node( pNode );

    return param;
}

void
del_all_nodes( tAnnexE* pAnnexE ) {
    hashDoAll(pAnnexE->ActiveNodesHash, remove_one_node, (void*)1);
}

#ifdef __cplusplus
}
#endif /* __cplusplus*/
