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
#include "cmintr.h"
#include "cmiras.h"
#include "q931asn1.h"
#include "cmutils.h"
#include "cmAutoRasEP.h"
#include "cmAutoRasCall.h"
#include "cmAutoRasAltGK.h"
#include "rasutils.h"
#include "cmAutoRas.h"

#ifdef __cplusplus
extern "C" {
#endif




/************************************************************************
 *
 *                          Private functions
 *
 ************************************************************************/






/************************************************************************
 *
 *                          Public functions
 *
 ************************************************************************/


/************************************************************************
 * cmiAutoRASInit
 * purpose: Initialize the RAS module and all the network related with
 *          RAS.
 * input  : hApp        - Application's stack handle
 *          logMgr      - Log manager used by the stack
 *          hTimers     - Timers pool used by the stack
 *          hVal        - Value tree handle
 *          rasConfNode - RAS configuration tree
 *          evFunc      - Event handler to set
 *          logMgr      - Log manager to use
 * output : none
 * return : Automatic RAS handle created on success
 *          NULL on failure
 ************************************************************************/
HAUTORASMGR cmiAutoRASInit(
    IN HAPP                     hApp,
    IN RvH323TimerPoolHandle    hTimers,
    IN HPVT                     hVal,
    IN int                      rasConfNode,
    IN cmiEvAutoRASEventT       evFunc,
    IN RvLogMgr*                logMgr)
{
    autorasEndpoint* autoras;

    if (RvMemoryAlloc(NULL, sizeof(autorasEndpoint), logMgr, (void**)&autoras) != RV_OK)
        return NULL;
    memset(autoras, 0, sizeof(autorasEndpoint));

    /* Set automatic RAS parameters */
    autoras->hApp = hApp;
    autoras->hTimers = hTimers;
    autoras->hVal = hVal;
    autoras->confNode = rasConfNode;
    autoras->event = evFunc;
    autoras->logMgr = logMgr;
    autoras->altGKNodeIdForDns = RV_PVT_INVALID_NODEID;

    autoras->state = cmIdle;
    autoras->regTimer = NULL;

    RvMutexConstruct(logMgr, &autoras->autorasLock);

    autoRasStartAltGKs(autoras);

    return (HAUTORASMGR)autoras;
}


/************************************************************************
 * cmiAutoRASStart
 * purpose: Start the RAS module and all the network related with
 *          RAS.
 * input  : hApp    - Application's stack handle
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiAutoRASStart(IN HAPP hApp)
{
    HAUTORASMGR hAutoRas = cmiGetAutoRasHandle(hApp);

    cmiAutoRASEPStart(hAutoRas);
    cmiAutoRASCallStart(hAutoRas);

    return 0;
}


/************************************************************************
 * cmiAutoRASEnd
 * purpose: End the automatic RAS module
 * input  : hApp        - Application's stack handle
 * output : none
 * return : none
 ************************************************************************/
void cmiAutoRASEnd(IN HAPP hApp)
{
    autorasEndpoint* autoras = (autorasEndpoint *)((cmElem*)hApp)->hAutoRas;

    if (autoras != NULL)
    {
        pvtDelete(((cmElem*)hApp)->hVal, autoras->altGKNodeIdForDns);
        RvMutexDestruct(&autoras->autorasLock, autoras->logMgr);
        autoRasStopAltGKs(autoras);
        RvMemoryFree(autoras, autoras->logMgr);
    }
}


/************************************************************************
 * cmiAutoRAS
 * purpose: Returns the RAS mode
 * input  : hApp    - Application's stack handle
 * output : none
 * return : RV_TRUE for automatic RAS
 *          RV_FALSE for manual RAS
 ************************************************************************/
RvBool cmiAutoRAS(IN HAPP hApp)
{
      return (((cmElem*)hApp)->hAutoRas != NULL);
}


/************************************************************************
 * setRasAlternateGatekeeper
 * purpose: Called after a confirmation message, sets the current GK and
 *          updates the list of AltGKs.
 * input  : autoras       - Automatic RAS EndPoint instance
 *        : autorasC      - Automatic RAS Call instance (may be NULL)
 *          hsRas         - transaction handle
 *          responseId    - node of response message
 *          bUpdate       - RV_TRUE if we should update the RAS module
 * output : none
 * return : none
 ************************************************************************/
void setRasAlternateGatekeeper(
    IN autorasEndpoint*    autoras,
    IN autorasCall*        autorasC,
    IN HRAS                hsRas,
    IN RvPvtNodeId         responseId,
    IN RvBool              bUpdate)
{
    if(autoRasIsInSearch(autoras, autorasC))
    {
        if((autoRasGotPermList(autoras)) && (autorasC ? !autorasC->isTempRdrn : !autoras->isTempRdrn))
        { /* we found the confirming GK through sending to PERMANENT AltGKs */
            /* Set this GK as our GK */
            int GKnode = autoRasGetCurrentAltGkNode(autoras, autorasC);
            if(GKnode < 0)
                /* SHOULD NEVER BE HERE! VERY BAD! */
                autoRasRemovePermList(autoras);
            else
            {
                RvPvtNodeId tmpNode;
                char gkId[256];
                int gkIdLen = 0;

                /* get the AltGK identifier (GKID) */
                tmpNode = pvtGetChild(autoras->hVal, GKnode, __q931(gatekeeperIdentifier), NULL);
                if (tmpNode >= 0)
                    gkIdLen = pvtGetString(autoras->hVal, tmpNode, sizeof(gkId), gkId);

                /* get the AltGK address node */
                tmpNode = pvtGetChild(autoras->hVal, GKnode, __q931(rasAddress), NULL);

                /* set the address and ID in the RAS module */
                rasUpdateGkRasAddress((rasModule *)emaGetUserData((EMAElement)hsRas), 
                    tmpNode, gkId, gkIdLen);

                /* report discover of new GK */
                autoRasChangeState(autoras, cmPermanentAlternateGatekeeperConfirmation, GKnode);
            }
        }
        autoRasResetSearch(autoras, autorasC);
        if(autorasC) autorasC->isTempRdrn = RV_FALSE;
        else autoras->isTempRdrn = RV_FALSE;
    }
    else
    {
        /* we weren't in any kind of search - that means updating the RAS module's GK address and
        ID is necessary if we got RCF/GCF */
        if (bUpdate)
        {
            RvPvtNodeId tmpNode;
            RvBool addedRoot = RV_FALSE;
            char gkId[256];
            int gkIdLen = 0;

            __pvtGetByFieldIds(tmpNode, autoras->hVal, responseId, {_anyField _q931(gatekeeperIdentifier) LAST_TOKEN}, NULL, NULL, NULL);
            if (tmpNode >= 0)
                gkIdLen = pvtGetString(autoras->hVal, tmpNode, sizeof(gkId), gkId);

            __pvtGetNodeIdByFieldIds(tmpNode, autoras->hVal, responseId, {_anyField _q931(rasAddress) LAST_TOKEN});
            if (!RV_PVT_NODEID_IS_VALID(tmpNode))
            {
                cmTransportAddress addr;
                cmRASGetParam(hsRas, cmRASTrStageRequest, cmRASParamDestinationIpAddress, 0, NULL, (char*)&addr);
                tmpNode = pvtAddRoot(autoras->hVal, ((cmElem*)(autoras->hApp))->hAddrSyn, 0, NULL);
                addedRoot = RV_TRUE;
                cmTAToVt(autoras->hVal, tmpNode, &addr);
            }

            rasUpdateGkRasAddress((rasModule *)emaGetUserData((EMAElement)hsRas), 
                tmpNode, gkId, gkIdLen);

            if(addedRoot)
                pvtDelete(autoras->hVal, tmpNode);
        }
    }

    /* getting the alternate gatekeepers from the message */
    if(responseId >= 0)
    {
        autoRasSetPermList(autoras, 
            pvtGetChild(autoras->hVal, pvtChild(autoras->hVal, responseId), __q931(alternateGatekeeper), NULL));
    }
}


#ifdef __cplusplus
}
#endif

