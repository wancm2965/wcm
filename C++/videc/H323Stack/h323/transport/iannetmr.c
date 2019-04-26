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

#include "rvtime.h"
#include "rvinternal.h"
#include "annexe.h"
#include "iannexe.h"

#ifdef __cplusplus
extern "C" {
#endif

char * getIP(IN cmTransportAddress* cmAddress, OUT RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE]);


RvUint32 get_delay_interval( tNode* pNode ) {
    return (RvUint32)pNode->pAnnexE->t_DT;
}

RvUint32 get_retransmit_interval( tNode* pNode ) {
    tAnnexE* pAnnexE = pNode->pAnnexE;
    return (RvUint32)(pAnnexE->t_R1 + (pAnnexE->t_R2 - pAnnexE->t_R1) * pNode->nRetry);
}

RvUint32 get_ima_interval( tNode* pNode ) {
    return (RvUint32)pNode->pAnnexE->t_IMA1;
}

RvBool start_retransmit_or_ima_timer( IN tNode* pNode )
{
    RvUint32 timeout;
    RvH323TimerCancel(pNode->pAnnexE->hTimers, &pNode->hResendAndIMATimer);

    if (pNode->pWaitingForAckPDU != NULL)
        timeout = get_retransmit_interval(pNode);
    else
        timeout = get_ima_interval(pNode);

    pNode->hResendAndIMATimer = RvH323TimerStart(pNode->pAnnexE->hTimers, retransmit_or_ima_timer_event, pNode, timeout);
    return RV_FALSE;
}

void stop_retransmit_or_ima_timer( tNode* pNode ) {
    RvH323TimerCancel(pNode->pAnnexE->hTimers, &pNode->hResendAndIMATimer);
}

void RVCALLCONV delay_timer_event(void* context) {
    tNode* pNode = (tNode*)context;
    if (pNode->pCurrentPDU == NULL)
    {
        RvLogExcep(&pNode->pAnnexE->log, (&pNode->pAnnexE->log,
            "delay_timer_event: No current PDU..."));
        return;
    }
    send_current_pdu( pNode );
}

RvBool retransmit_or_ima_timer_event(IN void* context)
{
    tNode* pNode = (tNode*)context;
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    RvH323TimerClear(pNode->pAnnexE->hTimers, &pNode->hResendAndIMATimer);

    if (pNode->pWaitingForAckPDU != NULL)
    {
        /* retransmit timer event! */
        if (pNode->nRetry++ == pNode->pAnnexE->n_R1)
        {
            remote_host_is_dead( pNode );
            return RV_FALSE;
        }
        RvLogInfo(&pNode->pAnnexE->log,
            (&pNode->pAnnexE->log, "Retransmit PDU to Host(%s:%u), retry=%i",
                getIP(&pNode->RemoteHost, buff), pNode->RemoteHost.port, pNode->nRetry-1));
        send_pdu( pNode, pNode->pWaitingForAckPDU );
    }
    else {
        /* ima timer event! */
        if (pNode->nRetry++ == pNode->pAnnexE->n_IMA1)
        {
            remote_host_is_dead( pNode );
            return RV_FALSE;
        }
        RvLogInfo(&pNode->pAnnexE->log,
            (&pNode->pAnnexE->log, "IAmAlive timer event - Send IMA to Host(%s:%u), retry=%i",
                getIP(&pNode->RemoteHost, buff), pNode->RemoteHost.port, pNode->nRetry-1));

        /* send ima payload*/
        send_ima(pNode, RV_TRUE);
    }
    return start_retransmit_or_ima_timer(pNode);
}

#ifdef __cplusplus
}
#endif /* __cplusplus*/
