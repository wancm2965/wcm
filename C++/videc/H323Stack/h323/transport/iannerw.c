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
/* needed to handle addresses */
#include "cmutils.h"

#ifdef __cplusplus
extern "C" {
#endif

/*lint -save -e416   Likely creation of out-of-bounds pointer */

RvUint32
get_next_seqn(tNode* pNode)
{
    RvUint32 res = pNode->nNext_SEQ++;
    if (pNode->nNext_SEQ >= 0x01000000U)
        pNode->nNext_SEQ = 0;
    return res;
}

RvBool
is_old_seqn(RvUint32 seqn, RvUint32 prev_seqn)
{
    if (prev_seqn == 0xffffffffU)
        return RV_FALSE;
    if (prev_seqn >= 0x7fffffU)
    {
        if ((seqn <= prev_seqn) && (seqn >= (prev_seqn - 0x7fffffU)))
            return RV_TRUE;
        return RV_FALSE;
    }
    else
    {
        if ((seqn <= prev_seqn) || (seqn >= (0x7fffffU + prev_seqn)))
            return RV_TRUE;
        return RV_FALSE;
    }
}

void
send_pdu(tNode* pNode, tPDU* pPDU)
{
    tAnnexE* pAnnexE = pNode->pAnnexE;

    if (pPDU->fPDUIsWaitingForSend)
        return;
    if ((!IsListEmpty(&pAnnexE->WaitingForTransmissionList)) || (!pAnnexE->fReadyToSend))
    {
        pPDU->fPDUIsWaitingForSend = RV_TRUE;
        InsertTailList(&pAnnexE->WaitingForTransmissionList, &pPDU->lPDULink);
        pPDU = NULL;
    }
    if (pAnnexE->fReadyToSend)
    {
        if(pAnnexE->fReadyToSend)
        {
            RvAddress remoteAddress;
            int res;

            if (pPDU == NULL)
            {
                RV_PLIST_ENTRY plink;
                if (IsListEmpty(&pAnnexE->WaitingForTransmissionList))
                    return;
                plink = RemoveHeadList(&pAnnexE->WaitingForTransmissionList);
                pPDU = RV_GET_STRUCT(tPDU, lPDULink, plink);
                pPDU->fPDUIsWaitingForSend = RV_FALSE;
            }

            /* Wrap this UDP and send it. Make sure the buffer can be disposed right after
               we send it. */
            RvH323CmToCoreAddress(&pPDU->ta, &remoteAddress);
            res = RvSocketSendBuffer(&pAnnexE->connection[pNode->localAddrIndex].socket, &pPDU->PDU[0], pPDU->nSize,
                &remoteAddress, pAnnexE->logMgr, NULL);
            RvAddressDestruct(&remoteAddress);

            if (res != RV_OK)
            {
                pPDU->fPDUIsWaitingForSend = RV_TRUE;
                InsertHeadList(&pAnnexE->WaitingForTransmissionList, &pPDU->lPDULink);
                pAnnexE->fReadyToSend = RV_FALSE;
                return;
            }
            if (AEHGet_A(pPDU->PDU) == 0)
                free_pdu(pAnnexE, pPDU);

            pPDU = NULL;
        }
    }
}

void
free_pdu(tAnnexE* pAnnexE, tPDU* pPDU) {
    if (pPDU->fPDUIsWaitingForSend) {
        RemoveEntryList(&pPDU->lPDULink);
        pPDU->fPDUIsWaitingForSend = RV_FALSE;
    }
    pPDU->nSize = 0;
    InsertTailList(&pAnnexE->FreePDUList, &pPDU->lPDULink);
    unblock_res_blocked_nodes(pAnnexE);
}

void
unblock_res_blocked_nodes(tAnnexE* pAnnexE) {
    RV_PLIST_ENTRY last;
    tNode*      lastPNode;
    RvBool      finished = RV_FALSE;

    if (pAnnexE->State != RUNNING)
        return;

    last = GetLastList(&pAnnexE->ResBlockedNodeList);
    lastPNode = RV_GET_STRUCT(tNode, lPendLink, last);

    while ((!IsListEmpty(&pAnnexE->FreePDUList)) &&
           (!IsListEmpty(&pAnnexE->ResBlockedNodeList)) &&
           (!finished)) {

        tNode*      pNode;

        RV_PLIST_ENTRY plink = RemoveHeadList(&pAnnexE->ResBlockedNodeList);
        pNode = RV_GET_STRUCT(tNode, lPendLink, plink);

        if (pAnnexE->events.AnnexEEvWriteable != NULL)
        {
            pNode->nRef++; /* protect node */
            pAnnexE->events.AnnexEEvWriteable(
                    AsHANNEXE(pNode->pAnnexE),
                    pNode->pAnnexE->hAppAnnexE,
                    &pNode->RemoteHost, pNode->localAddrIndex);
            if (pNode->nRef == 1)
            {
                del_node(pNode);
                continue;
            }
            pNode->nRef--; /* unprotect node */
        }

        if (pNode == lastPNode)
            finished = RV_TRUE;
    }
}

void
send_current_pdu(tNode* pNode)
{
    tPDU* pPDU;

#if defined(RV_ANNEXE_DEBUG)
    if (pNode->pCurrentPDU == NULL)
    {
        RvLogExcep(&pNode->pAnnexE->log, (&pNode->pAnnexE->log,
            "send_current_pdu: No current PDU..."));
        return;
    }
#endif

    if (AEHGet_A(pNode->pCurrentPDU->PDU) != 0)
    {
#if defined(RV_ANNEXE_DEBUG)
        if (pNode->pWaitingForAckPDU != NULL)
        {
            RvLogExcep(&pNode->pAnnexE->log, (&pNode->pAnnexE->log,
                "send_current_pdu: Already waiting for an ack."));
            return;
        }
#endif
        pNode->nRetry = 0;
        pNode->pWaitingForAckPDU = pNode->pCurrentPDU;
        start_retransmit_or_ima_timer(pNode);
    };
    pPDU = pNode->pCurrentPDU;
    pNode->pCurrentPDU = NULL;
    send_pdu(pNode, pPDU);
}

RvBool
send_payload(
        tNode*      pNode,
        void*       payload_header,
        int         payload_header_size,
        void*       payload_data,
        int         payload_data_size,
        void*       payload_exdata,
        int         payload_exdata_size,
        RvBool      AckFlag,
        RvBool      HFlag,
        RvBool      SendNow
       )
{
    tPDU*   pCurrentPDU;

    /* if can't send add to the list of pended nodes */
    if (AckFlag && (pNode->pWaitingForAckPDU != NULL)) {
        return RV_FALSE;
    }

    /* get free PDU if needed   */
    if (pNode->pCurrentPDU == NULL) {
        RV_PLIST_ENTRY plink;
        if (IsListEmpty(&pNode->pAnnexE->FreePDUList)) {
            InsertTailList(&pNode->pAnnexE->ResBlockedNodeList, &pNode->lPendLink);
            return RV_FALSE;
        }
        plink = RemoveHeadList(&pNode->pAnnexE->FreePDUList);
        pNode->pCurrentPDU = RV_GET_STRUCT(tPDU, lPDULink, plink);

        pCurrentPDU = pNode->pCurrentPDU;

        memcpy(&pCurrentPDU->ta, &pNode->RemoteHost, sizeof(pCurrentPDU->ta));

        pCurrentPDU->PDU[IAnnexEHeader_FLAGS] = 0;

        hton24(get_next_seqn(pNode), &pCurrentPDU->PDU[IAnnexEHeader_SEQN]);

        pCurrentPDU->nSize = sizeof_IAnnexEHeader;

        if (payload_header_size + payload_data_size + payload_exdata_size >= (int)(pCurrentPDU->nMaxSize - pCurrentPDU->nSize))
        {
            /* this is serious APPLICATION bug (payload is greather than PDU size)!!! */
            RvLogExcep(&pNode->pAnnexE->log, (&pNode->pAnnexE->log,
                "send_payload: Payload is greather than PDU size!"));
            return RV_FALSE;
        }
    }
    else
        pCurrentPDU = pNode->pCurrentPDU;

    /* check for free space in PDU */
    if (payload_header_size + payload_data_size + payload_exdata_size < (int)(pCurrentPDU->nMaxSize - pCurrentPDU->nSize)) {
        /* append payload to the PDU */
        if (payload_header_size > 0) {
            memcpy(&pCurrentPDU->PDU[pCurrentPDU->nSize], payload_header, (RvSize_t)payload_header_size);
            pCurrentPDU->nSize += payload_header_size;
        }
        if (payload_data_size > 0) {
            memcpy(&pCurrentPDU->PDU[pCurrentPDU->nSize], payload_data, (RvSize_t)payload_data_size);
            pCurrentPDU->nSize += payload_data_size;
        }
        if (payload_exdata_size > 0) {
            memcpy(&pCurrentPDU->PDU[pCurrentPDU->nSize], payload_exdata, (RvSize_t)payload_exdata_size);
            pCurrentPDU->nSize += payload_exdata_size;
        }

        AEHOr_A(pCurrentPDU->PDU, AckFlag);
        AEHOr_H(pCurrentPDU->PDU, HFlag);

        if ((SendNow) && (!pNode->fDontSend)) {
            send_current_pdu(pNode);
        }

        return RV_TRUE;
    }
    else {
        /* no place for that payload => send current PDU!   */
        send_current_pdu(pNode);

        if (AckFlag && (pNode->pWaitingForAckPDU != NULL))
            return RV_FALSE;
        else
            return send_payload(pNode, payload_header, payload_header_size, payload_data, payload_data_size, payload_exdata, payload_exdata_size, AckFlag, HFlag, SendNow);
    }
}

annexEStatus annexESendMessage(
    IN  HANNEXE                 hAnnexE,
    IN  cmTransportAddress*     ta,
    IN  cmTransportAddress*     locAddr,
    IN  RvUint16                CRV,
    IN  void*                   message,
    IN  int                     size,
    IN  RvBool                  fAckHint,
    IN  RvBool                  fReplyHint,
    IN  RvBool                  fSendHint
)
{
    tAnnexE *pAnnexE = AsAnnexE(hAnnexE);
    tNode   *pNode;
    RvUint8 addrInd = 0;

    if (!pAnnexE)
        return annexEStatusBadParameter;

    annexEGetLocalAddressIndex(hAnnexE, locAddr, &addrInd);

    pNode = get_node(pAnnexE, ta, addrInd);
    if (pNode == NULL)
    {
        pNode = add_node(pAnnexE, ta, addrInd);
    }
    if (pNode == NULL)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexESendMessage() - annexEStatusResourceProblem!"));
        return annexEStatusResourceProblem;
    }

    if (pNode->fLocalAddressUsed)
    {
        IAnnexET10PayloadSA pheader[sizeof_IAnnexET10PayloadSA_ND];

        pheader[IAnnexET10PayloadSA_Header + IAnnexET10Header_PFLAGS] = AEP_MASK_A | AEP_MASK_S | (AEPT_StaticMessage << AEP_SHFT_T);
        /*
        pheader.Header.A            = 1;
        pheader.Header.S            = 1;
        pheader.Header.RES          = 0;
        pheader.Header.T            = AEPT_StaticMessage;
        */
        pheader[IAnnexET10PayloadSA_Header + IAnnexET10Header_STATIC_TYPE]  = 0; /* h.225 messages */

        hton32(pNode->nLocalAddress, &pheader[IAnnexET10PayloadSA_ADDRESS]);
        hton16(CRV, &pheader[IAnnexET10PayloadSA_SESSION]);
        hton16((RvUint16)size, &pheader[IAnnexET10PayloadSA_DATA_LENGTH]);

        if (send_payload(pNode, pheader, sizeof_IAnnexET10PayloadSA_ND, message, size, NULL, 0, fAckHint, fReplyHint, fSendHint)) {
            return annexEStatusNormal;
        }
        else {
            return annexEStatusWouldBlock;
        }
    }
    else
    {
        IAnnexET10PayloadS  pheader[sizeof_IAnnexET10PayloadS_ND];

        pheader[IAnnexET10PayloadS_Header + IAnnexET10Header_PFLAGS] = AEP_MASK_S | (AEPT_StaticMessage << AEP_SHFT_T);
        /*
        pheader.Header.A            = 0;
        pheader.Header.S            = 1;
        pheader.Header.RES          = 0;
        pheader.Header.T            = AEPT_StaticMessage;
        */
        pheader[IAnnexET10PayloadS_Header + IAnnexET10Header_STATIC_TYPE]   = 0; /* h.225 messages  */
        hton16(CRV, &pheader[IAnnexET10PayloadS_SESSION]);
        hton16((RvUint16)size, &pheader[IAnnexET10PayloadS_DATA_LENGTH]);

        if (send_payload(pNode, pheader, sizeof_IAnnexET10PayloadS_ND, message, size, NULL, 0, fAckHint, fReplyHint, fSendHint))
        {
            return annexEStatusNormal;
        }
        else
        {
            return annexEStatusWouldBlock;
        }
    }
}

annexEStatus annexECloseNode(
    IN  HANNEXE                 hAnnexE,
    IN  cmTransportAddress*     ta,
    IN  cmTransportAddress*     locAddr
) {
    tAnnexE *pAnnexE = AsAnnexE(hAnnexE);
    tNode   *pNode;
#if (RV_LOGMASK & (RV_LOGLEVEL_ERROR | RV_LOGLEVEL_INFO))
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif
    RvUint8 addrInd = 0;

    if (!pAnnexE)
        return annexEStatusBadParameter;

    if (ta == NULL)
        return annexEStatusNormal; /* Nothing to close, really */

    annexEGetLocalAddressIndex(hAnnexE, locAddr, &addrInd);

    pNode = get_node(pAnnexE, ta, addrInd);
    if (pNode == NULL)
    {
        RvLogDebug(&pAnnexE->log,
            (&pAnnexE->log, "annexECloseNode() - no such Remote Host(%s:%u)!", getIP(ta, buff), ta->port));
        return annexEStatusBadParameter;
    }
    else
    {
        RvLogInfo(&pNode->pAnnexE->log, (&pNode->pAnnexE->log, "Remote Host(%s:%u) closed! (nRef = %d)",
            getIP(&pNode->RemoteHost, buff), pNode->RemoteHost.port, pNode->nRef));
        /* we call del_node here because we WANT to decrease the nRef value. if the node is
           currently protected, it will be deleted later, when the nRef is decreased. otherwise
           it will be deleted here */
        del_node(pNode);
    }

    return annexEStatusNormal;
}

void
remote_host_is_dead(tNode* pNode)
{
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    RvLogInfo(&pNode->pAnnexE->log,
        (&pNode->pAnnexE->log, "Remote Host(%s:%u) is dead!", getIP(&pNode->RemoteHost, buff), pNode->RemoteHost.port));
    if (pNode->pAnnexE->events.AnnexEEvConnectionBroken != NULL)
    {
        pNode->pAnnexE->events.AnnexEEvConnectionBroken(AsHANNEXE(pNode->pAnnexE),
            pNode->pAnnexE->hAppAnnexE, &pNode->RemoteHost, pNode->localAddrIndex);
    }
    else
        del_node(pNode);
}

void
remote_host_is_alive(tNode* pNode)
{
    if ((pNode->pWaitingForAckPDU == NULL))
    {
        pNode->nRetry = 0;
        start_retransmit_or_ima_timer(pNode);
    }
}

void annexEEvent(
    IN RvSelectEngine*  selectEngine,
    IN RvSelectFd*      fd,
    IN RvSelectEvents   selectEvent,
    IN RvBool           error)
{
    tAnnexESocket * connection = RV_GET_STRUCT(tAnnexESocket, fd, fd);
    tAnnexE* pAnnexE;
    tNode* pNode;
    RvAddress remoteAddress;
    RvStatus ret;
    RvBool newNode = RV_FALSE;

    RV_UNUSED_ARG(selectEngine);

    if ((error) || (selectEvent))
    {
        /* ToDo: handle error and select event */
    }

    pAnnexE = connection->pAnnexE;

    ret = RvSocketReceiveBuffer(&connection->socket, &pAnnexE->pRecvPDU->PDU[0], pAnnexE->pRecvPDU->nMaxSize,
        pAnnexE->logMgr, &pAnnexE->pRecvPDU->nSize, &remoteAddress);

    if ((ret == RV_OK) && (pAnnexE->pRecvPDU->nSize > 0))
    {
        RvUint8 localAddrIndex;
        /* Get the IP/Port and put in the receive PDU */
        RvH323CoreToCmAddress(&remoteAddress, &pAnnexE->pRecvPDU->ta);

        /* check AnnexE PDU header! */
        if ((pAnnexE->pRecvPDU->nSize <= sizeof_IAnnexEHeader) ||
            (AEHGet_VER( pAnnexE->pRecvPDU->PDU ) != 0) ||
            (AEHGet_RES( pAnnexE->pRecvPDU->PDU ) != 0))
        {
            /* reject! */
            RvLogError(&pAnnexE->log,
                (&pAnnexE->log, "(incoming PDU) bad header, rejected!" ));
            return;
        }

        /* get the "listening address" */
        localAddrIndex = (RvUint8)(connection-pAnnexE->connection);
        /* search for node  */
        pNode = get_node(pAnnexE, &pAnnexE->pRecvPDU->ta, localAddrIndex);

        if (pNode == NULL)
        {
            pNode = add_node(pAnnexE, &pAnnexE->pRecvPDU->ta, localAddrIndex);

            if (pNode == NULL)
            {
                /* no free resources - reject! */
                RvLogError(&pAnnexE->log,
                    (&pAnnexE->log, "(incoming PDU) no free resources, rejected!" ));
                return;
            }
            newNode = RV_TRUE;
        }

        /* Set the node's "listening" address */
        pNode->localAddrIndex = localAddrIndex;

        pNode->nRef++; /* protect node  */
        ret = process_pdu(pNode, pAnnexE->pRecvPDU);
        if (pNode->nRef == 1)
        {
            del_node(pNode);
        }
        else
        {
            pNode->nRef--; /* unprotect node */
            if ((ret == RV_FALSE) && newNode)
            {
                del_node(pNode);
            }
        }
    }
}

void
send_ima(tNode* pNode, RvBool fSendNow) 
{
    IAnnexET00IAmAlive  ima[sizeof_IAnnexET00IAmAlive_ND];
    RvUint8             cookie[4];
    memset(ima, 0, sizeof(ima));

    ima[IAnnexET00IAmAlive_Header+IAnnexET00Header_PFLAGS]  = 0;
    AEPOr_T(ima[IAnnexET00IAmAlive_Header+IAnnexET00Header_PFLAGS], AEPT_TransportMessage);
    /*
    ima.Header.S    = 0;
    ima.Header.A    = 0;
    ima.Header.RES  = 0;
    */
    ima[IAnnexET00IAmAlive_Header+IAnnexET00Header_TRANSPORT_TYPE] = AEPT00_IAmAlive;

    IMASet_P(ima);

    hton16((RvUint16)get_ima_interval(pNode), &ima[IAnnexET00IAmAlive_VALIDITY]);

    IMASet_CookieLen(ima, sizeof(RvUint32));

    hton32(RvInt64ToRvUint32(RvInt64Div(RvTimestampGet(NULL), RV_TIME64_NSECPERMSEC)), &cookie[0]);

    send_payload(
            pNode,
            ima,
            sizeof_IAmAliveHeader(&ima),
            &cookie[0],
            4,
            NULL,
            0,
            RV_FALSE,
            RV_FALSE,
            fSendNow
       );
}

void
i_am_alive_request_received(tNode* pNode, IAnnexET00IAmAlive* pPayload)
{
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    RvLogInfo(&pNode->pAnnexE->log,
        (&pNode->pAnnexE->log, "IAmAlive request from Host(%s:%u)!", getIP(&pNode->RemoteHost, buff), pNode->RemoteHost.port));
    IMAClr_P(pPayload);
    send_payload(pNode, pPayload, sizeof_IAmAliveHeader(pPayload), &pPayload[IAnnexET00IAmAlive_COOKIE],
        sizeof_IAmAliveCookie(pPayload), NULL, 0, RV_FALSE, RV_FALSE, RV_TRUE);
}

#if (RV_LOGMASK & RV_LOGLEVEL_DEBUG)
static void
i_am_alive_response_received(tNode* pNode, IAnnexET00IAmAlive* pPayload)
{
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];

    /* do nothing! remote_host_is_alive() is called from process_pdu() ;-)  */
    if (sizeof_IAmAliveCookie(pPayload) >= 4)
    {
        RvUint32 send_time = ntoh32(&pPayload[IAnnexET00IAmAlive_COOKIE]);
        RvLogDebug(&pNode->pAnnexE->log,
            (&pNode->pAnnexE->log, "IAmAlive response from Host(%s:%u), delay(%ims)",
            getIP(&pNode->RemoteHost, buff), pNode->RemoteHost.port,
                RvInt64ToRvUint32(RvInt64Div(RvTimestampGet(NULL), RV_TIME64_NSECPERMSEC)) - send_time));
    }
}
#else
#define i_am_alive_response_received(_pNode, _pPayload)
#endif

void
use_alternate_port(tNode* pNode, RvUint32 ip, RvUint16 port)
{
    cmTransportAddress      newAddress;
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    RvLogInfo(&pNode->pAnnexE->log,
        (&pNode->pAnnexE->log, "Use_Alternate_Port received from Host(%s:%u), New Host(%08x:%i)",
        getIP(&pNode->RemoteHost, buff), pNode->RemoteHost.port, ip, port));
    if (pNode->pAnnexE->events.AnnexEEvUseOtherAddress == NULL)
        return;
    newAddress.distribution = cmDistributionUnicast;
    newAddress.type         = cmTransportTypeIP;
    newAddress.port         = port;
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    newAddress.addr.v4.ip   = ip;
#else
    newAddress.ip           = ip;
#endif

    pNode->pAnnexE->events.AnnexEEvUseOtherAddress(
            AsHANNEXE(pNode->pAnnexE),
            pNode->pAnnexE->hAppAnnexE,
            &pNode->RemoteHost,
            &newAddress, pNode->localAddrIndex
   );
}

void
static_type0_is_not_supported(tNode* pNode)
{
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    RvLogInfo(&pNode->pAnnexE->log,
        (&pNode->pAnnexE->log, "Static Type=0 is not supported from remote Host(%s:%u)",
        getIP(&pNode->RemoteHost, buff), pNode->RemoteHost.port));
    if (pNode->pAnnexE->events.AnnexEEvNotSupported == NULL)
        return;

    pNode->pAnnexE->events.AnnexEEvNotSupported(
            AsHANNEXE(pNode->pAnnexE),
            pNode->pAnnexE->hAppAnnexE,
            &pNode->RemoteHost, pNode->localAddrIndex
   );
}

void
send_ack(tNode* pNode, RvUint32 seqn, RvBool fSendNow)
{
    IAnnexET00Ack ack[sizeof_IAnnexET00Ack_ND+sizeof_IAnnexEAckData];
    ack[IAnnexET00Ack_Header+IAnnexET00Header_PFLAGS] = 0;
    /*
    ack.Header.T = 0;
    ack.Header.A = 0;
    ack.Header.S = 0;
    ack.Header.RES = 0;
    */
    ack[IAnnexET00Ack_Header+IAnnexET00Header_TRANSPORT_TYPE] = AEPT00_Ack;
    hton16(1, &ack[IAnnexET00Ack_ACK_COUNT]);
    hton24(seqn, &ack[IAnnexET00Ack_ACK+IAnnexEAckData_SEQNUM]);
    ack[IAnnexET00Ack_ACK+IAnnexEAckData_RESERVED] = 0;

    send_payload(
            pNode,
            ack,
            sizeof_AckHeader(&ack),
            &ack[IAnnexET00Ack_ACK],
            sizeof_IAnnexEAckData,
            NULL,
            0,
            RV_FALSE,
            RV_FALSE,
            fSendNow
       );
}

void
send_nack3(tNode* pNode, RvUint32 seqn, int transport_type, RvBool fSendNow)
{
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    /* send NACK REASON_3:"Transport-payload not supported" */
    IAnnexET00NAckHeader header[sizeof_IAnnexET00NAckHeader];
    IAnnexENAckReason3   reason[sizeof_IAnnexENAckReason3];

    RvLogInfo(&pNode->pAnnexE->log,
        (&pNode->pAnnexE->log, "Send NAck3(Transport-payload-type=%i not supported, SEQN=%i) to Host(%s:%u)",
        transport_type, seqn, getIP(&pNode->RemoteHost, buff), pNode->RemoteHost.port));

    header[IAnnexET00NAckHeader_Header+IAnnexET00Header_PFLAGS] = 0;
    /*
    header.Header.T = 0;
    header.Header.A = 0;
    header.Header.S = 0;
    header.Header.RES = 0;
    */
    header[IAnnexET00NAckHeader_Header+IAnnexET00Header_TRANSPORT_TYPE] = AEPT00_NAck;
    hton16(1, &header[IAnnexET00NAckHeader_NACK_COUNT]);

    hton24(seqn, &reason[IAnnexENAckReason3_SEQNUM]);
    reason[IAnnexENAckReason3_DATA_LENGTH] = 1;
    hton16(3, &reason[IAnnexENAckReason3_REASON]);
    reason[IAnnexENAckReason3_TRANSPORT_TYPE] = (RvUint8)transport_type;

    send_payload(
            pNode,
            header,
            sizeof_IAnnexET00NAckHeader,
            reason,
            sizeof_IAnnexENAckReason3,
            NULL,
            0,
            RV_FALSE,
            RV_FALSE,
            fSendNow
       );
}

void
send_nack4(tNode* pNode, RvUint32 seqn, int static_type, RvBool fSendNow)
{
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    /* send NACK REASON_4:"Static-payload type not supported"   */
    IAnnexET00NAckHeader header[sizeof_IAnnexET00NAckHeader];
    IAnnexENAckReason4   reason[sizeof_IAnnexENAckReason4];

    RvLogInfo(&pNode->pAnnexE->log,
        (&pNode->pAnnexE->log, "Send NAck4(Static-payload-type=%i not supported, SEQN=%i) to Host(%s:%u)",
            static_type, seqn, getIP(&pNode->RemoteHost, buff), pNode->RemoteHost.port));

    header[IAnnexET00NAckHeader_Header+IAnnexET00Header_PFLAGS] = 0;
    /*
    header.Header.T = 0;
    header.Header.A = 0;
    header.Header.S = 0;
    header.Header.RES = 0;
    */
    header[IAnnexET00NAckHeader_Header+IAnnexET00Header_TRANSPORT_TYPE] = AEPT00_NAck;
    hton16(1, &header[IAnnexET00NAckHeader_NACK_COUNT]);

    hton24(seqn, &reason[IAnnexENAckReason4_SEQNUM]);
    reason[IAnnexENAckReason4_DATA_LENGTH] = 1;
    hton16(4, &reason[IAnnexENAckReason4_REASON]);
    reason[IAnnexENAckReason4_STATIC_TYPE] = (RvUint8)static_type;

    send_payload(
            pNode,
            header,
            sizeof_IAnnexET00NAckHeader,
            reason,
            sizeof_IAnnexENAckReason4,
            NULL,
            0,
            RV_FALSE,
            RV_FALSE,
            fSendNow
       );
}

void
send_nack5(tNode* pNode,RvUint32 seqn, int oid_length, RvUint8* poid, RvBool fSendNow)
{
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    /* send NACK REASON_5:"ObjectID-payload not supported!" */
    IAnnexET00NAckHeader header[sizeof_IAnnexET00NAckHeader];
    IAnnexENAckReason    reason[sizeof_IAnnexENAckReason_ND];

    RvLogInfo(&pNode->pAnnexE->log,
        (&pNode->pAnnexE->log, "Send NAck5(ObjectID-payload not supported, SEQN=%i) to Host(%s:%u)",
            seqn, getIP(&pNode->RemoteHost, buff), pNode->RemoteHost.port));

    header[IAnnexET00NAckHeader_Header+IAnnexET00Header_PFLAGS] = 0;
    /*
    header.Header.T = 0;
    header.Header.A = 0;
    header.Header.S = 0;
    header.Header.RES = 0;
    */
    header[IAnnexET00NAckHeader_Header+IAnnexET00Header_TRANSPORT_TYPE] = AEPT00_NAck;
    hton16(1, &header[IAnnexET00NAckHeader_NACK_COUNT]);

    hton24(seqn, &reason[IAnnexENAckReason_SEQNUM]);
    reason[IAnnexENAckReason_DATA_LENGTH] = (RvUint8)(oid_length & 0xff);
    hton16(5, &reason[IAnnexENAckReason_REASON]);

    send_payload(
            pNode,
            header,
            sizeof_IAnnexET00NAckHeader,
            reason,
            sizeof_IAnnexENAckReason_ND,
            poid,
            oid_length,
            RV_FALSE,
            RV_FALSE,
            fSendNow
       );
}

void
send_nack6(tNode* pNode, RvUint32 seqn, int nPayload, RvBool fSendNow)
{
    /* send NACK REASON_6:"Payload Corrupted"   */
    IAnnexET00NAckHeader header[sizeof_IAnnexET00NAckHeader];
    IAnnexENAckReason6   reason[sizeof_IAnnexENAckReason6];
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    RvLogInfo(&pNode->pAnnexE->log,
        (&pNode->pAnnexE->log, "Send NAck6(Payload corrupted, Id=%i, SEQN=%i) to Host(%s:%i)", nPayload,
        seqn, getIP(&pNode->RemoteHost, buff), pNode->RemoteHost.port));

    header[IAnnexET00NAckHeader_Header+IAnnexET00Header_PFLAGS] = 0;
    /*
    header.Header.T = 0;
    header.Header.A = 0;
    header.Header.S = 0;
    header.Header.RES = 0;
    */
    header[IAnnexET00NAckHeader_Header+IAnnexET00Header_TRANSPORT_TYPE] = AEPT00_NAck;
    hton16(1, &header[IAnnexET00NAckHeader_NACK_COUNT]);

    hton24(seqn, &reason[IAnnexENAckReason6_SEQNUM]);
    reason[IAnnexENAckReason6_DATA_LENGTH] = 1;
    hton16(6, &reason[IAnnexENAckReason6_REASON]);
    reason[IAnnexENAckReason6_PAYLOAD_NUMBER] = (RvUint8)nPayload;

    send_payload(
            pNode,
            header,
            sizeof_IAnnexET00NAckHeader,
            reason,
            sizeof_IAnnexENAckReason6,
            NULL,
            0,
            RV_FALSE,
            RV_FALSE,
            fSendNow
       );
}

void
ack_received(tNode* pNode) {
    tPDU* pPDU = pNode->pWaitingForAckPDU;
    pNode->pWaitingForAckPDU = NULL;

    /* notify user that the node is no longer waiting for ack */
    if (pNode->pAnnexE->events.AnnexEEvWriteable != NULL)
    {
        pNode->nRef++; /* protect node */
        pNode->pAnnexE->events.AnnexEEvWriteable(
                AsHANNEXE(pNode->pAnnexE),
                pNode->pAnnexE->hAppAnnexE,
                &pNode->RemoteHost, pNode->localAddrIndex);
        if (pNode->nRef == 1)
        {
            del_node(pNode);
        }
        else
            pNode->nRef--; /* unprotect node */
    }
    /* start IMA timer  */
    pNode->nRetry = 0;
    start_retransmit_or_ima_timer(pNode);
    /* free PDU! */
    free_pdu(pNode->pAnnexE, pPDU);
}

#define MoveToNextPayload(payload, payload_size) \
    ((IAnnexEPayloadHeader*)(((RvUint8*)(payload)) + (payload_size)))

#define MoveToNextReason(reason, reason_size) \
    ((IAnnexENAckReason*)(((RvUint8*)(reason)) + (reason_size)))

/* return values:
RV_TRUE - a real message was received
RV_FALSE - no real message was received
negative - an error has occured */
int process_pdu(IN tNode* pNode, IN tpPDU pPDU)
{
    IAnnexEPayloadHeader*   pPayload;
    RvBool                  fError = RV_FALSE;
    int                     nCurPayload = 0;
    int                     nBytesAvail = pPDU->nSize;
    int                     i, cnt, nSz;
    RvBool                  fResendForced = RV_FALSE;
    RvUint32                seqn;
    RvBool                  realMessage = RV_FALSE;

    remote_host_is_alive(pNode);

    seqn = ntoh24(&pPDU->PDU[IAnnexEHeader_SEQN]);
    /* Check for duplicated PDU's. That check is possible only for PDU's with set A-flag!*/
    if (AEHGet_A(pPDU->PDU) != 0)
    {
        if (is_old_seqn(seqn, pNode->nLast_Ack_SEQ))
        {
            /* duplicate!
             An Ack is sent immediately in order to unblock remote annexe module */
            RvLogInfo(&pNode->pAnnexE->log, (&pNode->pAnnexE->log, "Duplicate PDU received! send Ack immediately."));
            send_ack(pNode, seqn, RV_TRUE);
            return RV_TRUE;
        }
        if (check_pdu(pNode, pPDU))
        {
            pNode->nLast_Ack_SEQ = seqn;

            if (pNode->pWaitingForAckPDU != NULL)
                send_ack(pNode, seqn, RV_TRUE);
            else
                send_ack(pNode, seqn, RV_FALSE);
        }
    }

    /* get first payload!   */
    if (AEHGet_L(pPDU->PDU) != 0)
    {
        /* this is valid only for Annex E over TCP, but it is possible that
            the host implementation of Annex E to not understand it.*/
        pPayload = (IAnnexEPayloadHeader*)(pPDU->PDU + 8);
    }
    else
    {
        pPayload = (IAnnexEPayloadHeader*)(pPDU->PDU + 4);
        nBytesAvail -= 4;
    }

    /* a cycle to walk through all payloads in the PDU  */
    while ((void*)pPayload < (void*)(&pPDU->PDU[pPDU->nSize]))
    {
        switch (AEPGet_T(*pPayload))
        {
        case AEPT_TransportMessage: {
            /***************************************************************/
            /* T == 00 * Annex E Transport Messages                        */
            /***************************************************************/
            IAnnexET00Header* pTrPayload = (IAnnexET00Header*)(pPayload);

            /* check 'Source/Dest' and 'Session' flags in payload header */
            if ((AEPGet_A(*pPayload) != 0) || (AEPGet_S(*pPayload) != 0))
            {
                /* this is not a valid Annex E Transport message!
                   send NACK REASON_6:"Payload Corrupted" */
                send_nack6(pNode, seqn, nCurPayload, RV_TRUE);

                /* break the processing of the rest payloads as there is no
                   guarantee that the size of packet may be found correctly. */
                fError = RV_TRUE;
                break;
            }

            switch (pTrPayload[IAnnexET00Header_TRANSPORT_TYPE])
            {
            case AEPT00_IAmAlive:
                /* check size of payload! */
                nSz = sizeof_IAmAlive(pTrPayload);
                nBytesAvail -= nSz;
                if (nBytesAvail < 0) {
                    /* incorrect payload size! payload corrupted! */
                    send_nack6(pNode, seqn, nCurPayload, RV_TRUE);
                    fError = RV_TRUE;
                    break;
                }
                if (IMAGet_P(AsIAmAlive(pTrPayload)))
                {
                    /* I_Am_Alive message from remote AnnexE module, return
                       the same payload, with 'P' flag cleared!*/
                    i_am_alive_request_received(pNode, AsIAmAlive(pTrPayload));
                }
                else
                {
                    /* answer to our I-Am-Alive payload!    */
                    i_am_alive_response_received(pNode, AsIAmAlive(pTrPayload));
                }
                pPayload = MoveToNextPayload(pPayload, sizeof_IAmAlive(pTrPayload));
                break;
            case AEPT00_Ack:
                /*  check size of payload! */
                nSz = sizeof_Ack(pTrPayload);
                nBytesAvail -= nSz;
                if (nBytesAvail < 0)
                {
                    /*  incorrect payload size! payload corrupted! */
                    send_nack6(pNode, seqn, nCurPayload, RV_TRUE);
                    fError = RV_TRUE;
                    break;
                }
                /* as the current implementation of Annex E fulfills only the
                 'Serial' model, we are interested only of ACK.SEQN equal to
                 that waiting for acknowledge.*/
                if (pNode->pWaitingForAckPDU != NULL)
                {
                    RvUint32 waiting_seqn = ntoh24(&pNode->pWaitingForAckPDU->PDU[IAnnexEHeader_SEQN]);
                    cnt = countof_AckData(pTrPayload);
                    for (i = 0; i < cnt; i++)
                    {
                        RvUint32 ack_seqn = ntoh24(&(AsAck(pTrPayload)[IAnnexET00Ack_ACK+
                                                                        i*sizeof_IAnnexEAckData+
                                                                        IAnnexEAckData_SEQNUM]));
                        if (ack_seqn == waiting_seqn)
                        {
                            ack_received(pNode);
                            break;
                        }
                    }
                }
                pPayload = MoveToNextPayload(pPayload, nSz);
                break;
            case AEPT00_NAck:
                {
                    IAnnexENAckReason*  pReason = AsNAckReason(GetNAckReasonPtr(pTrPayload));
                    /*  check size of payload! */
                    nSz = sizeof_IAnnexET00NAckHeader;
                    nBytesAvail -= nSz;
                    if (nBytesAvail < 0)
                    {
                        /* incorrect payload size! payload corrupted! */
                        send_nack6(pNode, seqn, nCurPayload, RV_TRUE);
                        fError = RV_TRUE;
                        break;
                    }
                    cnt = countof_NAckReasons(pTrPayload);
                    for (i = 0; i < cnt; i++)
                    {
                        RvUint16    nReason;
                        RvUint32    reason_seqn;
                        /* check size of reason */
                        nSz = sizeof_NAckReason(pReason);
                        nBytesAvail -= nSz;
                        if (nBytesAvail < 0)
                        {
                            /* incorrect payload size! payload corrupted! */
                            send_nack6(pNode, seqn, nCurPayload, RV_TRUE);
                            fError = RV_TRUE;
                            break;
                        }

                        /*  process pReason */
                        nReason = ntoh16(&pReason[IAnnexENAckReason_REASON]);
                        reason_seqn = ntoh24(&pReason[IAnnexENAckReason_SEQNUM]);
                        switch (nReason)
                        {
                        case 0:
                            /* NAck.REASON = Non-Standart Reason
                             not supported by this implementation of Annex E! Skipped!*/
                            break;
                        case 1:
                            /* NAck.REASON = Request the sender to use an alternate port
                             for the specified static payload type. That Nack is of
                             interest only if static type == 0 => (H.225) payload!*/
                            if (AsNAckReason1(pReason)[IAnnexENAckReason1_STATIC_TYPE] == 0)
                            {
                                use_alternate_port(pNode,
                                                    ntoh32(&AsNAckReason1(pReason)[IAnnexENAckReason1_ALTERNATE_IP]),
                                                    ntoh16(&AsNAckReason1(pReason)[IAnnexENAckReason1_ALTERNATE_PORT]));
                            }
                            break;
                        case 2:
                            /* NAck.REASON = Request the sender to use an alternate port
                             for the specified ObjectID payload type

                             not supported in that implementation of Annex E => it is not possible
                             for us to have sent such payload. Possible line error.
                             No reaction currently. It is possible that the next payload is
                             corrupted and the if the Nack is for the PDU with set A flag it
                             should be retransmitted.*/
                            break;
                        case 3:
                            /* NAck.REASON = Transport-payload not supported
                             Supporting only types to 3 including ('Restart Message'), their
                             implementation is necessary. Do not pay attention to that Nack!*/
                            break;
                        case 4:
                            /* NAck.REASON = Static-payload type not supported
                             the NAck is of interest only if it is a static type == 0 => (H.225) payload!*/
                            if (AsNAckReason4(pReason)[IAnnexENAckReason4_STATIC_TYPE] == 0)
                            {
                                static_type0_is_not_supported(pNode);
                            }
                            break;
                        case 5:
                            /* NAck.REASON = OID-payload not supported
                               Do not pay attention to that Nack!*/
                            break;
                        case 6:
                            /* NAck.REASON = Payload Corrupted
                             The NAck is of interest only if it is about a PDU waiting for pWaitingForAckPDU!*/
                            if (!fResendForced && (pNode->pWaitingForAckPDU != NULL) && (reason_seqn == ntoh24(&pNode->pWaitingForAckPDU->PDU[IAnnexEHeader_SEQN])))
                            {
                                fResendForced = RV_TRUE;
                                RvH323TimerCancel(pNode->pAnnexE->hTimers, &pNode->hResendAndIMATimer);
                                pNode->nRetry--;
                                retransmit_or_ima_timer_event(pNode);
                            }
                            break;
                        default:
                            /* reserved for future use => skipped! */
                            break;
                        }

                        /* goto next reason */
                        pReason = MoveToNextReason(pReason, nSz);
                    }
                    pPayload = (IAnnexEPayloadHeader*)pReason;
                }
                break;
            case AEPT00_Restart:
                /*  check size of payload! */
                nSz = sizeof_IAnnexET00Restart;
                nBytesAvail -= nSz;
                if (nBytesAvail < 0)
                {
                    /* incorrect payload size! payload corrupted! */
                    send_nack6(pNode, seqn, nCurPayload, RV_TRUE);
                    fError = RV_TRUE;
                    break;
                }
                /* notify application!*/
                if (pNode->pAnnexE->events.AnnexEEvRestart != NULL)
                {
                    pNode->pAnnexE->events.AnnexEEvRestart(
                            AsHANNEXE(pNode->pAnnexE),
                            pNode->pAnnexE->hAppAnnexE,
                            &pNode->RemoteHost
                       );
                }
                else
                {
                    RvLogInfo(&pNode->pAnnexE->log,
                        (&pNode->pAnnexE->log, "Restart command arrived - no CB to treat it was given"));

                }
                pPayload = MoveToNextPayload(pPayload, nSz);
                break;
            default:
                {
                    int transType = pTrPayload[IAnnexET00Header_TRANSPORT_TYPE];
                    /* reserved for future use.... (not supported by this version of AnnexE)*/
                    send_nack3(pNode, seqn, transType, RV_TRUE);

                    /* break the processing of the rest payloads in the PDU as there
                    is no guarantee to find the exact size of the payload.*/
                    fError = RV_TRUE;
                }
                break;
            }
        };
        break;
        case AEPT_OIDTypedMessage: {
            /***************************************************************/
            /* T == 01 * OBJECT IDENTIFIER typed messages                  */
            /***************************************************************/
            int iLength;

            /*  check size of payload! */
            if ((int)sizeof_OIDHeader(pPayload) >= nBytesAvail)
            {
                /*  incorrect payload size! payload corrupted! */
                send_nack6(pNode, seqn, nCurPayload, RV_TRUE);
                fError = RV_TRUE;
                break;
            }
            nSz = sizeof_OID(pPayload);
            nBytesAvail -= nSz;
            if (nBytesAvail < 0)
            {
                /* incorrect payload size! payload corrupted! */
                send_nack6(pNode, seqn, nCurPayload, RV_TRUE);
                fError = RV_TRUE;
                break;
            }

            /* that type of messages is not supported in the current implementation of
            the Annex E => return corresponding NAck (REASON_5: "Object-ID payload
            not supported)! As it is possible to find the exact size of the payload
            the PDU parsing is not broken, but the current payload is skipped!*/
            iLength = AsOIDHeader(pPayload)[IAnnexET01Header_OID_LENGTH];
            send_nack5(pNode,
                        seqn,
                        iLength,
                        &AsOIDHeader(pPayload)[IAnnexET01Header_OID],
                        RV_FALSE);

            pPayload = MoveToNextPayload(pPayload, sizeof_OID(pPayload));
        };
        break;
        case AEPT_StaticMessage: {
            /***************************************************************/
            /* T == 10 * Static-payload typed messages                     */
            /***************************************************************/

            /* typecast to the Annex E Static-typed message payload */
            IAnnexET10Payload*  pStPayload = (IAnnexET10Payload*)pPayload;

            RvBool              fSessionField = pStPayload[IAnnexET10Payload_Header+IAnnexET10Header_PFLAGS] & AEP_MASK_S;
            RvBool              fAddressField = pStPayload[IAnnexET10Payload_Header+IAnnexET10Header_PFLAGS] & AEP_MASK_A;
            /*RvUint16            nSessionID;*/
            RvUint8*            pData;
            RvUint16            nDataSize;
            int                 staticType;
            int                 iMsgSize;

            /*  check size of payload! */
            nSz = sizeof_Static(pPayload);
            nBytesAvail -= nSz;
            if (nBytesAvail < 0)
            {
                /*  incorrect payload size! payload corrupted! */
                send_nack6(pNode, seqn, nCurPayload, RV_TRUE);
                fError = RV_TRUE;
                break;
            }

            if (pStPayload[IAnnexET10Payload_Header+IAnnexET10Header_STATIC_TYPE] != 0)
            {
                /* this implementation of the Annex E supports only H.225 messages,
                 all the rest are unknown currently.*/
                staticType = pStPayload[IAnnexET10Payload_Header+IAnnexET10Header_STATIC_TYPE];
                send_nack4(pNode, seqn, staticType, RV_FALSE);

                /* skip payload*/
                pPayload = MoveToNextPayload(pPayload, sizeof_Static(pStPayload));
                break;
            }

            if (fSessionField)
            {
                if (fAddressField)
                {
                    /*nSessionID = ntoh16(&AsStaticSA(pStPayload)[IAnnexET10PayloadSA_SESSION]);*/
                    pData = &AsStaticSA(pStPayload)[IAnnexET10PayloadSA_DATA];
                    nDataSize = ntoh16(&AsStaticSA(pStPayload)[IAnnexET10PayloadSA_DATA_LENGTH]);
                }
                else
                {
                    /*nSessionID = ntoh16(&AsStaticS(pStPayload)[IAnnexET10PayloadS_SESSION]);*/
                    pData = &AsStaticS(pStPayload)[IAnnexET10PayloadS_DATA];
                    nDataSize = ntoh16(&AsStaticS(pStPayload)[IAnnexET10PayloadS_DATA_LENGTH]);
                }

                /* Turn the last bit of the session Id for incoming msgs */
                /*nSessionID ^= 0x8000U;*/
            }
            else
            {
                /* this h.225 message has no session fields defined. From AnnexE API those
                messages are only session oriented => we suppose the payload is
                broken while transferring.*/
                send_nack6(pNode, seqn, nCurPayload, RV_FALSE);

                /* Do not break the processing of the PDU as that mistake may be in
                result of a bug in the remote Annex E module implementation. It is
                possible to follow valid payloads in the PDU!

                skip payload*/
                pPayload = MoveToNextPayload(pPayload, sizeof_Static(pStPayload));
                break;
            }

            /* everything is alright and we may notify the user for the received payload*/
            if (pNode->pAnnexE->events.AnnexEEvNewMessage != NULL)
            {
                annexEStatus status;
                pNode->fDontSend = RV_TRUE; /* block sends until all the treatment is done by the user */
                pNode->nRef++; /* protect the node*/
                iMsgSize = nDataSize;
                status = pNode->pAnnexE->events.AnnexEEvNewMessage(
                        AsHANNEXE(pNode->pAnnexE),
                        pNode->pAnnexE->hAppAnnexE,
                        pNode->localAddrIndex,
                        &pNode->RemoteHost,
                        pData,
                        iMsgSize);
                if (pNode->nRef == 1)
                {
                    /* node is closed by the application ?!?!*/
                    del_node(pNode);
                }
                else
                {
                    pNode->nRef--; /* unprotect the node */
                    pNode->fDontSend = RV_FALSE; /* allow sends again */

                    /* send any messages that the user has sent during the callback */
                    if (pNode->pCurrentPDU)
                        send_current_pdu(pNode);
                }
                realMessage = (status != annexEStatusBadMessage);
            }

            /* static-payload processing is done! now skip to the next one!*/
            pPayload = MoveToNextPayload(pPayload, nSz);
        };
        break;
        case AEPT_ReservedForFutureUse: {
            /***************************************************************/
            /* T == 11 * Reserved for future use                           */
            /***************************************************************/

            /* this is not a valid Annex E Transport message!*/
            send_nack6(pNode, seqn, nCurPayload, RV_TRUE);

            /* break the processing of the rest payloads in the PDU as there is no
             guarantee to find the exact size of the payload.*/
            fError = RV_TRUE;
        };
        break;
        default: break;
        }

        if (fError)
        {
            /* during the parsing of the current payload in the PDU there
            was a serious error! => break further processing!*/
            break;
        }

        nCurPayload++;
    }

    if (fError)
        return RV_ERROR_UNKNOWN;
    if ((AEHGet_A(pPDU->PDU) != 0) && (AEHGet_H(pPDU->PDU) == 0) && (pNode->pCurrentPDU != NULL)) {
        /* send current pdu forced */
        send_current_pdu(pNode);
    }
    return realMessage;
}


RvBool check_pdu(tNode* pNode, IN tpPDU pPDU)
{
    IAnnexEPayloadHeader*   pPayload;
    int                     nBytesAvail = pPDU->nSize;
    int                     nSz, i, cnt;
    
    RV_UNUSED_ARG(pNode);

    /* get first payload!*/
    if (AEHGet_L(pPDU->PDU) != 0)
    {
        pPayload = (IAnnexEPayloadHeader*)(pPDU->PDU + 8);
        nBytesAvail -= 4 + 4;
    }
    else
    {
        pPayload = (IAnnexEPayloadHeader*)(pPDU->PDU + 4);
        nBytesAvail -= 4;
    }

    if (nBytesAvail <= 0)
        return RV_FALSE;

    while ((void*)pPayload < (void*)(&pPDU->PDU[pPDU->nSize]))
    {
        switch (AEPGet_T(*pPayload))
        {
        case AEPT_TransportMessage: {
            IAnnexET00Header* pTrPayload = (IAnnexET00Header*)(pPayload);

            /* check 'Source/Dest' and 'Session' flags in payload header*/
            if ((AEPGet_A(*pPayload) != 0) || (AEPGet_S(*pPayload) != 0))
            {
                /* this is not a valid Annex E Transport message!*/
                return RV_FALSE;
            }

            switch (pTrPayload[IAnnexET00Header_TRANSPORT_TYPE])
            {
            case AEPT00_IAmAlive:
                nSz = sizeof_IAmAlive(pTrPayload);
                nBytesAvail -= nSz;
                if (nBytesAvail < 0)
                    return RV_FALSE;
                pPayload = MoveToNextPayload(pPayload, nSz);
                break;
            case AEPT00_Ack:
                nSz = sizeof_Ack(pTrPayload);
                nBytesAvail -= nSz;
                if (nBytesAvail < 0)
                    return RV_FALSE;
                pPayload = MoveToNextPayload(pPayload, nSz);
                break;
            case AEPT00_NAck:
                {
                    IAnnexENAckReason*  pReason = AsNAckReason(GetNAckReasonPtr(pTrPayload));
                    nSz = sizeof_IAnnexET00NAckHeader;
                    nBytesAvail -= nSz;
                    if (nBytesAvail < 0)
                        return RV_FALSE;
                    cnt = countof_NAckReasons(pTrPayload);
                    for (i = 0; i < cnt; i++)
                    {
                        /* goto next reason*/
                        nSz = sizeof_NAckReason(pReason);
                        nBytesAvail -= nSz;
                        if (nBytesAvail < 0)
                            return RV_FALSE;
                        pReason = MoveToNextReason(pReason, nSz);
                    }
                    pPayload = (IAnnexEPayloadHeader*)pReason;
                }
                break;
            case AEPT00_Restart:
                nSz = sizeof_IAnnexET00Restart;
                nBytesAvail -= nSz;
                if (nBytesAvail < 0)
                    return RV_FALSE;
                pPayload = MoveToNextPayload(pPayload, nSz);
                break;
            default:
                return RV_FALSE;
            }
        }
        break;
        case AEPT_OIDTypedMessage:
        {
            if ((int)sizeof_OIDHeader(pPayload) >= nBytesAvail)
                return RV_FALSE;
            nSz = sizeof_OID(pPayload);
            nBytesAvail -= nSz;
            if (nBytesAvail < 0)
                return RV_FALSE;
            pPayload = MoveToNextPayload(pPayload, nSz);
        }
        break;
        case AEPT_StaticMessage:
        {
            nSz = sizeof_Static(pPayload);
            nBytesAvail -= nSz;
            if (nBytesAvail < 0)
                return RV_FALSE;
            pPayload = MoveToNextPayload(pPayload, nSz);
        }
        break;
        case AEPT_ReservedForFutureUse:
        {
            return RV_FALSE;
        }
        default: break;
        }
    }

#if defined(RV_ANNEXE_DEBUG)
    if (nBytesAvail != 0)
    {
        RvLogExcep(&pNode->pAnnexE->log, (&pNode->pAnnexE->log,
                "check_pdu: nBytesAvail=%d", nBytesAvail));
    }
#endif

    return RV_TRUE;
}

/*lint -restore */

#ifdef __cplusplus
}
#endif /* __cplusplus*/
