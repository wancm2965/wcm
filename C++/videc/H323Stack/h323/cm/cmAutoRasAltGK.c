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

/************************************************************************
 * cmAutoRasAltGK
 * --------------
 * This file provides functions to handle retrieval and selection of
 * Alternate Gatekeepers.
 ************************************************************************/

#include "rvstdio.h"
#include "q931asn1.h"
#include "cmAutoRasAltGK.h"

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 * AltGKSort
 * purpose: Sort function for qsort
 ************************************************************************/
#if (RV_OS_TYPE == RV_OS_TYPE_WINCE)
int __cdecl AltGKSort(const void * A, const void * B)
#else
int AltGKSort(const void * A, const void * B)
#endif
{
    AltGKwithPrio * GK_A = (AltGKwithPrio *) A;
    AltGKwithPrio * GK_B = (AltGKwithPrio *) B;

    if(GK_A->prio<GK_B->prio) return -1;
    if(GK_A->prio>GK_B->prio) return 1;
    return 0;
}



/************************************************************************
 * autoRasGetNextAltGkNode
 * purpose: Advances the EP's search index and returns the next GK.
 * input  : autoras - Automatic RAS instance of an EP
 *          autorasC - Automatic RAS instance of a call (may be NULL)
 * output : none
 * return : node ID of the new GK, Negative value if none.
 ************************************************************************/
int autoRasGetNextAltGkNode(autorasEndpoint * autoras, autorasCall * autorasC)
{
    int altGKInd;
    RvBool isTempRdrn = autorasC ? autorasC->isTempRdrn : autoras->isTempRdrn;

    if(autorasC) altGKInd = ++(autorasC->altGKInd);
    else altGKInd = ++(autoras->altGKInd);

    if((altGKInd >= ALT_GK_ARRAY_SIZE) || (altGKInd < 0))
        return RV_ERROR_UNKNOWN;
    /* Check which array we are working with */
    if(isTempRdrn)
        return autoras->tempAltGKs[altGKInd].nodeID;
    return autoras->permAltGKs[altGKInd].nodeID;
}

/************************************************************************
 * autoRasGetCurrentAltGkNode
 * purpose: returns the indexed GK.
 * input  : autoras - Automatic RAS instance of an EP
 *          autorasC - Automatic RAS instance of a call (may be NULL)
 * output : none
 * return : node ID of the new GK, Negative value if none.
 ************************************************************************/
int autoRasGetCurrentAltGkNode(autorasEndpoint * autoras, autorasCall * autorasC)
{
    int altGKInd = autorasC ? autorasC->altGKInd : autoras->altGKInd;
    RvBool isTempRdrn = autorasC ? autorasC->isTempRdrn : autoras->isTempRdrn;

    if((altGKInd >= ALT_GK_ARRAY_SIZE) || (altGKInd < 0))
        return RV_ERROR_UNKNOWN;
    /* Check which array we are working with */
    if(isTempRdrn)
        return autoras->tempAltGKs[altGKInd].nodeID;
    return autoras->permAltGKs[altGKInd].nodeID;
}

/************************************************************************
 * autoRasRemovePermList
 * purpose: Clears the Permanent Alt GKs array and frees all the nodes.
 * input  : autoras - Automatic RAS instance
 * output : none
 * return : none
 ************************************************************************/
void autoRasRemovePermList(autorasEndpoint * autoras)
{
    int i;
    cmiRASUpdateRegInfo(cmiGetRasHandle(autoras->hApp), RV_TRUE);
    for(i=0; i<ALT_GK_ARRAY_SIZE; i++)
        if(autoras->permAltGKs[i].nodeID != RV_ERROR_UNKNOWN)
        {
            pvtDelete(autoras->hVal, autoras->permAltGKs[i].nodeID);
            autoras->permAltGKs[i].nodeID = RV_PVT_INVALID_NODEID;
            autoras->permAltGKs[i].prio = RV_UINT8_MAX;
        }
        else break;
}

/************************************************************************
 * autoRasRemoveTempList
 * purpose: Clears the Temporary Alt GKs array and frees all the nodes.
 * input  : autoras - Automatic RAS instance
 * output : none
 * return : none
 ************************************************************************/
void autoRasRemoveTempList(autorasEndpoint * autoras)
{
    int i;
    for(i=0; i<ALT_GK_ARRAY_SIZE; i++)
        if(autoras->tempAltGKs[i].nodeID != RV_ERROR_UNKNOWN)
        {
            pvtDelete(autoras->hVal, autoras->tempAltGKs[i].nodeID);
            autoras->tempAltGKs[i].nodeID = RV_PVT_INVALID_NODEID;
            autoras->tempAltGKs[i].prio = RV_UINT8_MAX;
        }
        else break;
}

/************************************************************************
 * autoRasIsInSearch
 * purpose: Checks if the EP is in a state of Alt GK search
 * input  : autoras - Automatic RAS instance of an EP
 *          autorasC - Automatic RAS instance of a call
 * output : none
 * remark : one of the instances may be NULL
 * return : true iff in search
 ************************************************************************/
RvBool autoRasIsInSearch(autorasEndpoint * autoras, autorasCall * autorasC)
{
    if(autorasC) return(autorasC->altGKInd != -1);
    return(autoras->altGKInd != -1);
}

/************************************************************************
 * autoRasResetSearch
 * purpose: Resets the status of a search to No Search
 * input  : autoras - Automatic RAS instance of an EP
 *          autorasC - Automatic RAS instance of a call
 * output : none
 * remark : one of the instances may be NULL
 * return : none
 ************************************************************************/
void autoRasResetSearch(autorasEndpoint * autoras, autorasCall * autorasC)
{
    if(autorasC) autorasC->altGKInd = -1;
    else autoras->altGKInd = -1;
}

/************************************************************************
 * autoRasSetPermList
 * purpose: Set the array Permanent Alt GKs according to the AltGKs node,
 *          and sorts by priority.
 * input  : autoras - Automatic RAS instance of an EP
 *          nodeId - Alternate Gatekeeper node ID
 * output : none
 * return : none
 ************************************************************************/
void autoRasSetPermList(autorasEndpoint * autoras, int nodeId)
{
    int i, GKnode;
    RvBool stop = RV_FALSE;

    if (!RV_PVT_NODEID_IS_VALID(nodeId))
        return;

    for(i=0; i<ALT_GK_ARRAY_SIZE; i++)
    {
        if(autoras->permAltGKs[i].nodeID >= 0)
            pvtDelete(autoras->hVal, autoras->permAltGKs[i].nodeID);
        else
            stop = RV_TRUE;
        if (pvtGetByIndex(autoras->hVal, nodeId, (RvInt32)(i+1), &GKnode) >= 0)
        {
            pvtGetChildByFieldId(autoras->hVal, GKnode, __q931(priority), (RvInt32 *)&(autoras->permAltGKs[i].prio), NULL);
            autoras->permAltGKs[i].nodeID = pvtAddRoot(autoras->hVal, ((cmElem*)autoras->hApp)->synConfRAS, 0, NULL);
            pvtSetTree(autoras->hVal, autoras->permAltGKs[i].nodeID, autoras->hVal, GKnode);
        }
        else
        {
            autoras->permAltGKs[i].nodeID = RV_PVT_INVALID_NODEID;
            autoras->permAltGKs[i].prio = RV_UINT8_MAX;
            if(stop) break;
        }
    }
    RvQsort((void *)autoras->permAltGKs, ALT_GK_ARRAY_SIZE, sizeof(AltGKwithPrio), AltGKSort);
}

/************************************************************************
 * autoRasSetTempList
 * purpose: Set the array Temporary Alt GKs according to the AltGKs node,
 *          and sorts by priority.
 * input  : autoras - Automatic RAS instance of an EP
 *          nodeId - Alternate Gateeker node ID
 * output : none
 * return : none
 ************************************************************************/
void autoRasSetTempList(autorasEndpoint * autoras, int nodeId)
{
    int i, GKnode;
    RvBool stop = RV_FALSE;

    /* first, see if this is an empty list, and if so, IGNORE IT */
    if(pvtGetByIndex(autoras->hVal, nodeId, (RvInt32)1, NULL) < 0)
        return;

    /* not an empty list - set it in the array */
    for(i=0; i<ALT_GK_ARRAY_SIZE; i++)
    {
        if(autoras->tempAltGKs[i].nodeID >= 0)
            pvtDelete(autoras->hVal, autoras->tempAltGKs[i].nodeID);
        else stop = RV_TRUE;
        if(pvtGetByIndex(autoras->hVal, nodeId, (RvInt32)(i+1), &GKnode)>=0)
        {
            pvtGetChildByFieldId(autoras->hVal, GKnode, __q931(priority), (RvInt32 *)&(autoras->tempAltGKs[i].prio), NULL);
            autoras->tempAltGKs[i].nodeID = pvtAddRoot(autoras->hVal, ((cmElem*)autoras->hApp)->synConfRAS, 0, NULL);
            pvtSetTree(autoras->hVal, autoras->tempAltGKs[i].nodeID, autoras->hVal, GKnode);
        }
        else
        {
            autoras->tempAltGKs[i].nodeID = RV_PVT_INVALID_NODEID;
            autoras->tempAltGKs[i].prio = RV_UINT8_MAX;
            if(stop) break;
        }
    }
    RvQsort((void *)autoras->tempAltGKs, ALT_GK_ARRAY_SIZE, sizeof(AltGKwithPrio), AltGKSort);
}

/************************************************************************
 * autoRasStartAltGKs
 * purpose: Set both arrays of Alt GKs to default (-1) values
 * input  : autoras - Automatic RAS instance of an EP
 * output : none
 * return : none
 ************************************************************************/
void autoRasStartAltGKs(autorasEndpoint * autoras)
{
    int i;
    for(i=0; i<ALT_GK_ARRAY_SIZE; i++)
    {
        autoras->tempAltGKs[i].nodeID = RV_PVT_INVALID_NODEID;
        autoras->tempAltGKs[i].prio = RV_UINT8_MAX;
        autoras->permAltGKs[i].nodeID = RV_PVT_INVALID_NODEID;
        autoras->permAltGKs[i].prio = RV_UINT8_MAX;
    }
    autoras->altGKInd = -1;
    autoras->isTempRdrn = RV_FALSE;
}

/************************************************************************
 * autoRasStopAltGKs
 * purpose: Deletes all the copied nodes in both AltGK arays
 * input  : autoras - Automatic RAS instance of an EP
 * output : none
 * return : none
 ************************************************************************/
void autoRasStopAltGKs(autorasEndpoint * autoras)
{
    int i;
    for(i=0; i<ALT_GK_ARRAY_SIZE; i++)
    {
        if(autoras->permAltGKs[i].nodeID != RV_ERROR_UNKNOWN)
            pvtDelete(autoras->hVal, autoras->permAltGKs[i].nodeID);
        if(autoras->tempAltGKs[i].nodeID != RV_ERROR_UNKNOWN)
            pvtDelete(autoras->hVal, autoras->tempAltGKs[i].nodeID);
    }
}

/************************************************************************
 * autoRasGotPermList
 * purpose: Checks if there is a list of Permanent Alt GK
 * input  : autoras - Automatic RAS instance of an EP
 * output : none
 * return : true iff the PermAltGKs list contains GKs
 ************************************************************************/
RvBool autoRasGotPermList(autorasEndpoint * autoras)
{
    return(autoras->permAltGKs[0].nodeID != RV_ERROR_UNKNOWN);
}

/************************************************************************
 * autoRasGotTempList
 * purpose: Checks if there is a list of Temporary Alt GK
 * input  : autoras - Automatic RAS instance of an EP
 * output : none
 * return : true iff the TempAltGKs list contains GKs
 ************************************************************************/
RvBool autoRasGotTempList(autorasEndpoint * autoras)
{
    return(autoras->tempAltGKs[0].nodeID != RV_ERROR_UNKNOWN);
}


#ifdef __cplusplus
}
#endif

