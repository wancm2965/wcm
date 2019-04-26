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
#include "hash.h"
#include "annexe.h"
#include "iannexe.h"
#include "cmutils.h"

#ifdef __cplusplus
extern "C" {
#endif

void free_resources(tAnnexE* pAnnexE);

void annexEEvent(
    IN RvSelectEngine*  selectEngine,
    IN RvSelectFd*      fd,
    IN RvSelectEvents   selectEvent,
    IN RvBool           error);


/* package init & done functions
 ****************************************************************************
 ***************************************************************************/

annexEStatus annexEInit(
    OUT HANNEXE*                hAnnexE,
    IN  HAPPANNEXE              hAppAnnexE,
    IN  int                     nMaxNodes,
    IN  int                     nMaxSupportedMessageSize,
    IN  int                     nMaxStoredPackets,
    IN  RvInt                   nMaxListeningAddresses,
    IN  RvH323TimerPoolHandle   timersPool,
    IN  RvSelectEngine*         selEngine,
    IN  RvPortRange*            portRange,
    IN  RvLogMgr*               logMgr)
{
    RvStatus    status;
    tAnnexE*    pAnnexE;
    int         i;
    RvSize_t    nPDUmemsize;

    if (hAnnexE == NULL)
        return annexEStatusBadParameter;

    *hAnnexE = NULL;

    /* allocate annex E context */
    if(RvMemoryAlloc(NULL, sizeof(tAnnexE), logMgr, (void**)&pAnnexE) != RV_OK)
    {
        return annexEStatusMemoryProblem;
    }
    memset(pAnnexE, 0, sizeof(tAnnexE));

    /* initialize tAnnexE */
    pAnnexE->logMgr = logMgr;
    pAnnexE->State = NOT_INITIALIZED;

    RvLogSourceConstruct(logMgr, &pAnnexE->log, "AnnexE", "H323 AnnexE module");

    RvLogInfo(&pAnnexE->log,
        (&pAnnexE->log, "annexEInit(MaxSes:%i, MaxMsgSize:%i, MaxPDU's:%i) Begin",
            nMaxNodes, nMaxSupportedMessageSize, nMaxStoredPackets));

    InitializeListHead(&pAnnexE->FreeNodesList);
    InitializeListHead(&pAnnexE->FreePDUList);
    InitializeListHead(&pAnnexE->WaitingForTransmissionList);
    InitializeListHead(&pAnnexE->ResBlockedNodeList);

    pAnnexE->t_R1   = ANNEXE_TR1;
    pAnnexE->t_R2   = ANNEXE_TR1;
    pAnnexE->n_R1   = ANNEXE_NR1;
    pAnnexE->t_IMA1 = ANNEXE_TIMA1;
    pAnnexE->n_IMA1 = ANNEXE_NIMA1;
    pAnnexE->t_DT   = ANNEXE_TR1 / 4;

    pAnnexE->ActiveNodesHash = hashConstruct(nMaxNodes + 1,
                                             nMaxNodes,
                                             NULL,
                                             NULL,
                                             sizeof_tNodeKey,
                                             sizeof(tNode*),
                                             "Annex E nodes header",
                                             logMgr);

    if (pAnnexE->ActiveNodesHash == NULL)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEInit() - MemoryProblem (ActiveNodes hash)!"));
        RvLogSourceDestruct(&pAnnexE->log);
        RvMemoryFree(pAnnexE, logMgr);
        return annexEStatusMemoryProblem;
    }

    pAnnexE->hAppAnnexE = hAppAnnexE;

    /* initialize listening addresses */
    pAnnexE->maxListening = nMaxListeningAddresses;
    status = RvMemoryAlloc(NULL, sizeof(tAnnexESocket)*nMaxListeningAddresses, logMgr,
        (void**)&pAnnexE->connection);
    if (status != RV_OK)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEInit() - MemoryProblem (addresses)!"));
        free_resources(pAnnexE);
        return annexEStatusMemoryProblem;
    }

    /* initialize tNode's */
    status = RvMemoryAlloc(NULL, sizeof(tNode)*nMaxNodes, logMgr, (void**)&pAnnexE->nodes);
    if (status != RV_OK)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEInit() - MemoryProblem (nodes)!"));
        free_resources(pAnnexE);
        return annexEStatusMemoryProblem;
    }
    memset(pAnnexE->nodes, 0, sizeof(tNode)*nMaxNodes);
    for (i = 0; i < nMaxNodes; i++)
    {
        pAnnexE->nodes[i].pAnnexE = pAnnexE;

        /* append tNode to FreeNodesList */
        InsertTailList(&pAnnexE->FreeNodesList, &pAnnexE->nodes[i].lNodeLink);
    }

    /* initialize PDU's */
    nPDUmemsize = RvRoundToSize(sizeof(tPDU) - sizeof_IAnnexEHeader + nMaxSupportedMessageSize, RV_ALIGN_SIZE);
    status = RvMemoryAlloc(NULL, nPDUmemsize*nMaxStoredPackets, logMgr, (void**)&pAnnexE->pdus);
    if (status != RV_OK)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEInit() - MemoryProblem (send PDU's)!"));
        free_resources(pAnnexE);
        return annexEStatusMemoryProblem;
    }
    memset(pAnnexE->pdus, 0, nPDUmemsize*nMaxStoredPackets);
    for (i = 0; i < nMaxStoredPackets; i++)
    {
        tPDU*       pPDU;
        pPDU = (tPDU*)(((RvChar*)pAnnexE->pdus) + i * nPDUmemsize);
        pPDU->nMaxSize = (RvSize_t)nMaxSupportedMessageSize;

        /* append tPDU to FreePDUList   */
        InsertTailList(&pAnnexE->FreePDUList, &pPDU->lPDULink);
    }

    /* initialize pRecvPDU! */
    if(RvMemoryAlloc(NULL, nPDUmemsize, logMgr, (void**)&pAnnexE->pRecvPDU) != RV_OK)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEInit() - MemoryProblem (recv PDU)!"));
        free_resources(pAnnexE);
        return annexEStatusMemoryProblem;
    }
    pAnnexE->pRecvPDU->nMaxSize = (RvSize_t)nMaxSupportedMessageSize /*MAX_UDP_FRAME*/;

    /* initialize Timers    */
    pAnnexE->hTimers = timersPool;

    if (pAnnexE->hTimers == NULL)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEInit() - MemoryProblem!"));
        free_resources(pAnnexE);
        return annexEStatusMemoryProblem;
    }

    pAnnexE->pSelEngine = selEngine;
    pAnnexE->pPortRange = portRange;

    pAnnexE->State = INITIALIZED;

    *hAnnexE = AsHANNEXE(pAnnexE);

    RvLogInfo(&pAnnexE->log, (&pAnnexE->log, "annexEInit() End! - successful!"));

    return annexEStatusNormal;
}

void
free_resources(tAnnexE* pAnnexE)
{
    /* free pRecvPDU    */
    if (pAnnexE->pRecvPDU)
        RvMemoryFree(pAnnexE->pRecvPDU, pAnnexE->logMgr);

    /* free PDUList */
    if (pAnnexE->pdus != NULL)
        RvMemoryFree(pAnnexE->pdus, pAnnexE->logMgr);

    /* free Nodes   */
    if (pAnnexE->nodes != NULL)
        RvMemoryFree(pAnnexE->nodes, pAnnexE->logMgr);

    if (pAnnexE->connection != NULL)
        RvMemoryFree(pAnnexE->connection, pAnnexE->logMgr);

    if (pAnnexE->ActiveNodesHash != NULL)
        hashDestruct(pAnnexE->ActiveNodesHash);

    RvLogSourceDestruct(&pAnnexE->log);

    /* free AnnexE  */
    RvMemoryFree(pAnnexE, pAnnexE->logMgr);
}

annexEStatus annexEEnd(IN  HANNEXE                 hAnnexE)
{

    tAnnexE*    pAnnexE = AsAnnexE(hAnnexE);

    if (pAnnexE->State == RUNNING)
        annexEStop(hAnnexE);

    pAnnexE->State = NOT_INITIALIZED;

    RvLogInfo(&pAnnexE->log, (&pAnnexE->log, "annexEEnd() - successful!"));

    free_resources(pAnnexE);

    return annexEStatusNormal;
}

/* event handler & protocol parameter functions
****************************************************************************
***************************************************************************/

annexEStatus annexESetProtocolParams(
    IN  HANNEXE                 hAnnexE,
    IN  int                     t_R1,
    IN  int                     t_R2,
    IN  int                     n_R1,
    IN  int                     t_IMA1,
    IN  int                     n_IMA1,
    IN  int                     t_DT)
{
    tAnnexE*    pAnnexE = AsAnnexE(hAnnexE);

    if (pAnnexE->State != INITIALIZED)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexESetProtocolParams() - annexEStatusMustBeStopped!"));
        return annexEStatusMustBeStopped;
    }

    pAnnexE->t_R1   = (t_R1 > 0) ? t_R1 : ANNEXE_TR1;
    pAnnexE->t_R2   = (t_R2 > 0) ? t_R2 : pAnnexE->t_R1;
    pAnnexE->n_R1   = (n_R1 > 0) ? n_R1 : ANNEXE_NR1;
    pAnnexE->t_IMA1 = (t_IMA1 > 0) ? t_IMA1 : ANNEXE_TIMA1;
    pAnnexE->n_IMA1 = (n_IMA1 > 0) ? n_IMA1 : ANNEXE_NIMA1;
    pAnnexE->t_DT   = (t_DT > 0) ? t_DT : (pAnnexE->t_R1 / 4);

    RvLogDebug(&pAnnexE->log, (&pAnnexE->log, "annexESetProtocolParams(tR1=%i, tR2=%i, nR1=%i, tIMA1=%i, nIMA1=%i, tDT=%i) - successful!",
            pAnnexE->t_R1, pAnnexE->t_R2, pAnnexE->n_R1, pAnnexE->t_IMA1, pAnnexE->n_IMA1, pAnnexE->t_DT));

    return annexEStatusNormal;
}

annexEStatus annexEAddLocalAddress(
    IN  HANNEXE                 hAnnexE,
    IN  cmTransportAddress*     pLocalAddress)
{
    tAnnexE*    pAnnexE = AsAnnexE(hAnnexE);

    if (pAnnexE->State != INITIALIZED)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEAddLocalAddress() - annexEStatusMustBeStopped!"));
        return annexEStatusMustBeStopped;
    }

    if (pAnnexE->curListening == pAnnexE->maxListening)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEAddLocalAddress() - too many listening addresses!"));
        return annexEStatusMustBeStopped;
    }


    if (pLocalAddress == NULL)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEAddLocalAddress() - annexEStatusBadParameter(pLocalAddress == NULL)!"));
        return annexEStatusBadParameter;
    }

    if ((pLocalAddress->distribution != cmDistributionUnicast) ||
        ((pLocalAddress->type != cmTransportTypeIP) && (pLocalAddress->type != cmTransportTypeIPv6)))
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEAddLocalAddress() - annexEStatusBadParameter(distribution or type error)!"));
        return annexEStatusBadParameter;
    }

    if (RvH323CmToCoreAddress(pLocalAddress, &pAnnexE->connection[pAnnexE->curListening].localAddr) != RV_OK)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEAddLocalAddress() - annexEStatusBadParameter(bad address)!"));
        return annexEStatusBadParameter;
    }

#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    {
        RvChar addrBuf[RV_TRANSPORT_ADDRESS_STRINGSIZE];
        addrBuf[0] = 0;
        RvAddressGetString(&pAnnexE->connection[pAnnexE->curListening].localAddr, sizeof(addrBuf), addrBuf);
        RvLogInfo(&pAnnexE->log, (&pAnnexE->log,
            "annexEAddLocalAddress(%s) - successful!", addrBuf));
    }
#endif

    pAnnexE->connection[pAnnexE->curListening].pAnnexE = pAnnexE;
    pAnnexE->curListening++;

    return annexEStatusNormal;
}

annexEStatus annexEGetLocalAddress(
    IN  HANNEXE                 hAnnexE,
    IN  RvInt                   index,
    OUT cmTransportAddress*     pLocalAddress)
{
    tAnnexE* pAnnexE = AsAnnexE(hAnnexE);

    if (pLocalAddress == NULL)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEGetLocalAddress() - annexEStatusBadParameter(pLocalAddress == NULL)!"));
        return annexEStatusBadParameter;
    }

    RvH323CoreToCmAddress(&pAnnexE->connection[index].localAddr, pLocalAddress);

    return annexEStatusNormal;
}

annexEStatus annexEGetLocalAddressIndex(
    IN  HANNEXE                 hAnnexE,
    IN  cmTransportAddress*     pLocalAddress,
    OUT RvUint8*                index)
{
    tAnnexE* pAnnexE = AsAnnexE(hAnnexE);
    RvAddress localAddr;
    cmTransportAddress lza;
    RvAddress localZeroAddr;
    int i;

    if (pLocalAddress == NULL)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEGetLocalAddressIndex() - annexEStatusBadParameter(pLocalAddress == NULL)!"));
        return annexEStatusBadParameter;
    }

    RvH323CmToCoreAddress(pLocalAddress, &localAddr);
    memset(&lza, 0, sizeof(lza));
    lza.distribution = pLocalAddress->distribution;
    lza.port = pLocalAddress->port;
    lza.type = pLocalAddress->type;
    RvH323CmToCoreAddress(&lza, &localZeroAddr);
    for (i=0; i<pAnnexE->curListening; i++)
    {
        if (RvAddressCompare(&localAddr, &pAnnexE->connection[i].localAddr, RV_ADDRESS_FULLADDRESS) ||
            RvAddressCompare(&localZeroAddr, &pAnnexE->connection[i].localAddr, RV_ADDRESS_FULLADDRESS))
        {
            break;
        }
    }
    RvAddressDestruct(localAddr);
    RvAddressDestruct(localZeroAddr);
    if (i == pAnnexE->curListening)
        return annexEStatusSocketProblem;

    *index = (RvUint8)i;
    return annexEStatusNormal;
}

annexEStatus annexESetEventHandler(
    IN  HANNEXE             hAnnexE,
    IN  annexEEvents*       pAnnexEEventHandler)
{
    tAnnexE*    pAnnexE = AsAnnexE(hAnnexE);

    if (pAnnexE->State != INITIALIZED) {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexESetEventHandler() - annexEStatusMustBeStopped!"));
        return annexEStatusMustBeStopped;
    }

    if (pAnnexEEventHandler == NULL) {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexESetEventHandler() - annexEStatusBadParameter(pAnnexEEventHandler == NULL)!"));
        return annexEStatusBadParameter;
    }

    pAnnexE->events = *pAnnexEEventHandler;

    return annexEStatusNormal;
}


/* start & stop functions
****************************************************************************
***************************************************************************/

annexEStatus annexEStart(IN  HANNEXE                 hAnnexE)
{
    RvStatus    status;
    tAnnexE*    pAnnexE = AsAnnexE(hAnnexE);
    RvInt       i;

    if (pAnnexE->State != INITIALIZED)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEStart() - annexEStatusMustBeStopped!"));
        return annexEStatusMustBeStopped;
    }

    for (i = 0; i < pAnnexE->curListening; i++)
    {
        status = RvSocketConstruct(pAnnexE->connection[i].localAddr.addrtype, RvSocketProtocolUdp, pAnnexE->logMgr, &pAnnexE->connection[i].socket);
        if (status != RV_OK)
        {
            RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEStart() - Error opening socket!"));
            return annexEStatusSocketProblem;
        }

        /* We're always working with non-blocking sockets */
        status = RvSocketSetBlocking(&pAnnexE->connection[i].socket, RV_FALSE, pAnnexE->logMgr);
        if (status == RV_OK)
            status = RvSocketBind(&pAnnexE->connection[i].socket, &pAnnexE->connection[i].localAddr, pAnnexE->pPortRange, pAnnexE->logMgr);
        if (status != RV_OK)
        {
            RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEStart() - Error binding socket!"));
            RvSocketDestruct(&pAnnexE->connection[i].socket, RV_FALSE, NULL, pAnnexE->logMgr);
            return annexEStatusSocketProblem;
        }

        status = RvSocketGetLocalAddress(&pAnnexE->connection[i].socket, pAnnexE->logMgr, &pAnnexE->connection[i].localAddr);

        if (status == RV_OK)
        {
            RvFdConstruct(&pAnnexE->connection[i].fd, &pAnnexE->connection[i].socket, pAnnexE->logMgr);
            status = RvSelectAdd(pAnnexE->pSelEngine, &pAnnexE->connection[i].fd, RvSelectRead, annexEEvent);
        }

        if (status != RV_OK)
        {
            RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEStart() - Error handling listening socket!"));
            RvSocketDestruct(&pAnnexE->connection[i].socket, RV_FALSE, pAnnexE->pPortRange, pAnnexE->logMgr);
            return annexEStatusSocketProblem;
        }

#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
        {
            RvChar addrBuf[RV_TRANSPORT_ADDRESS_STRINGSIZE];
            addrBuf[0] = 0;

            RvAddressGetString(&pAnnexE->connection[i].localAddr, sizeof(addrBuf), addrBuf);

            RvLogInfo(&pAnnexE->log, (&pAnnexE->log,
                "annexEStart() - successfuly bind on %s", addrBuf));
        }
#endif
    }

    pAnnexE->State = RUNNING;
    pAnnexE->fReadyToSend = RV_TRUE;

    return annexEStatusNormal;
}

annexEStatus annexEStop(IN  HANNEXE                 hAnnexE)
{
    tAnnexE*    pAnnexE = AsAnnexE(hAnnexE);
    RvInt       i;

    if (pAnnexE->State != RUNNING)
    {
        RvLogError(&pAnnexE->log, (&pAnnexE->log, "annexEStop() - annexEStatusMustBeStarted!"));
        return annexEStatusMustBeStarted;
    }

    for (i = 0; i < pAnnexE->curListening; i++)
    {
        RvSelectRemove(pAnnexE->pSelEngine, &pAnnexE->connection[i].fd);
        RvFdDestruct(&pAnnexE->connection[i].fd);
        RvSocketDestruct(&pAnnexE->connection[i].socket, RV_FALSE, pAnnexE->pPortRange, pAnnexE->logMgr);
    }

    pAnnexE->curListening = 0;

    del_all_nodes(pAnnexE);

    /* clear WaitingForTransmissionList! */
    {
        RV_PLIST_ENTRY plink;
        tPDU*       pPDU;
        while (!IsListEmpty(&pAnnexE->WaitingForTransmissionList))
        {
            plink = RemoveHeadList(&pAnnexE->WaitingForTransmissionList);
            pPDU = RV_GET_STRUCT(tPDU, lPDULink, plink);
            pPDU->fPDUIsWaitingForSend = RV_FALSE;
            pPDU->nSize = 0;
            InsertTailList(&pAnnexE->FreePDUList, &pPDU->lPDULink);
        }
    }

    pAnnexE->State = INITIALIZED;

    RvLogInfo(&pAnnexE->log, (&pAnnexE->log, "annexEStop() - successful!"));

    return annexEStatusNormal;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
