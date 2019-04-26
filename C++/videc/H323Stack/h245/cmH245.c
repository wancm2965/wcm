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

#include "cmH245.h"
#include "cmictrl.h"
#include "cmH245Object.h"
#include "rvtimestamp.h"
#include "rvmemory.h"
#include "rvstdio.h"
#include "cmchan.h"
#include "h245.h"
#include "ema.h"
#include "oidutils.h"
#include "cmCtrlMSD.h"
#include "cmCtrlCap.h"
#include "cmCtrlMES.h"
#include "coder.h"

#ifdef __cplusplus
extern "C" {
#endif


/* H.245 Version used */
#define H245_PROTOCOL_IDENTIFIER "itu-t recommendation h(8) 245 0 13"




/******************************************************************************
 * H245Init
 * ----------------------------------------------------------------------------
 * General: Initialize an H.245 module.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  pH245Cfg     - Configuration parameters for the H245 module.
 *         hApp         - Application handle to associate with this module.
 * Output: pH245        - H245 module handle created.
 *****************************************************************************/
RvStatus H245Init(
    IN  H245Config  *pH245Cfg,
    IN  HAPP        hApp,
    OUT HH245       *pH245)
{
    H245Object *pH245Object;
    RvStatus status;

    /* H.245 object construction */
    status = RvMemoryAlloc(NULL, sizeof(H245Object), pH245Cfg->logMgr, (void**)&pH245Object);
    if (status != RV_OK)
        return status;
    memset(pH245Object, 0, sizeof(H245Object));

    /* Get all of the configuration parameters we will use */
    pH245Object->hVal = pH245Cfg->hVal;
    pH245Object->logMgr = pH245Cfg->logMgr;
    pH245Object->h245DataType = pH245Cfg->h245DataType;
#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
    pH245Object->h245TransCap = pH245Cfg->h245TransCap;
    pH245Object->h245RedEnc = pH245Cfg->h245RedEnc;
    pH245Object->hAddrSynH245 = pH245Cfg->hAddrSynH245;
    pH245Object->mibEvent = pH245Cfg->mibEvent;
    pH245Object->mibHandle = pH245Cfg->mibHandle;
#endif
    pH245Object->synOLC = pH245Cfg->synOLC;
    pH245Object->hSynProtH245 = pH245Cfg->hSynProtH245;
    pH245Object->h245Conf = pH245Cfg->h245Conf;
    pH245Object->bIsPropertyModeNotUsed = pH245Cfg->bIsPropertyModeNotUsed;
    pH245Object->evHandlers = pH245Cfg->evHandlers;
    pH245Object->pAppObject = hApp;
    pH245Object->h225CtrlOffset = pH245Cfg->h225CtrlOffset;
    pH245Object->h223CtrlOffset = pH245Cfg->h223CtrlOffset;
    pH245Object->encodeBufferSize = pH245Cfg->encodeBufferSize;
#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
    pH245Object->bSupportEFC = pH245Cfg->bSupportEFC;
#endif

    /* Create a random generator for our use */
    status = RvRandomGeneratorConstruct(&pH245Object->randomGenerator,
        (RvRandom)RvInt64ToRvUint32(RvInt64ShiftRight(RvTimestampGet(pH245Cfg->logMgr), 16)));
    if (status != RV_OK)
        return status;
    status = RvLockConstruct(pH245Object->logMgr, &pH245Object->lock);
    if (status != RV_OK)
    {
        RvRandomGeneratorDestruct(&pH245Object->randomGenerator);
        return status;
    }

    /* Initialize EMA channels */
    pH245Object->hChannels = H245InitChannels((HH245)pH245Object, hApp, pH245Cfg->watchdog, pH245Cfg->maxCalls, pH245Cfg->maxChannels);
    if (pH245Object->hChannels == NULL)
    {
        RvLockDestruct(&pH245Object->lock, pH245Object->logMgr);
        RvRandomGeneratorDestruct(&pH245Object->randomGenerator);
        return RV_ERROR_OUTOFRESOURCES;
    }

    RvLogSourceConstruct(pH245Object->logMgr, &pH245Object->log, "H245", "H.245 Module");

    *pH245 = (HH245)pH245Object;
    return RV_OK;
}


/******************************************************************************
 * H245End
 * ----------------------------------------------------------------------------
 * General: Destruct an H.245 module.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hH245        - Handle to the H.245 module.
 *****************************************************************************/
void H245End(IN HH245 hH245)
{
    H245Object *pH245Object = (H245Object*)hH245;

    RvRandomGeneratorDestruct(&pH245Object->randomGenerator);
    RvLockDestruct(&pH245Object->lock, pH245Object->logMgr);
    if (pH245Object->hChannels != NULL)
    {
        H245EndChannels(pH245Object->hChannels);
    }
    RvMemoryFree(pH245Object, pH245Object->logMgr);
}


/******************************************************************************
 * H245Start
 * ----------------------------------------------------------------------------
 * General: Start the execution of the H.245 module.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hH245        - Handle to the H.245 module.
 *****************************************************************************/
void H245Start(IN HH245 hH245)
{
    H245Object *hH245Object = (H245Object *)hH245;

    /* Encode the protocol identifier that we'll use in future TCS messages */
    hH245Object->h245protocolIDLen =
        oidEncodeOID(sizeof(hH245Object->h245protocolID), hH245Object->h245protocolID, H245_PROTOCOL_IDENTIFIER);
}


/******************************************************************************
 * H245Stop
 * ----------------------------------------------------------------------------
 * General: Stop the execution of the H.245 module.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hH245        - Handle to the H.245 module.
 *****************************************************************************/
void H245Stop(IN HH245 hH245)
{
    RV_UNUSED_ARG(hH245);
}



#if (RV_H245_USE_HIGH_AVAILABILITY == RV_YES)

/******************************************************************************
 * H245encodeIntoBuffer
 * ----------------------------------------------------------------------------
 * General: Encodes a nodeID into the buffer, adding to it's length.
 *
 * Return Value: None
 * ----------------------------------------------------------------------------
 * Arguments:
 * input  : hVal      - pvt thingie
 *          nodeId    - nodeId to encode
 *          bufferLen - Size of the buffer
 *          length    - pointer to the length done so far
 *          ret       - current status
 * output : buffer    - The buffer with the encoded nodeId
 *          length    - updated length done
 *****************************************************************************/
void H245encodeIntoBuffer(
    IN    HPVT          hVal,
    IN    int           nodeId,
    OUT   RvUint8       *pBuffer,
    IN    RvSize_t      bufferLen,
    INOUT RvSize_t      *length,
    INOUT RvStatus      *status)
{
    int encLen = 0;
    int len = bufferLen - *length - sizeof(int);

    if (!RV_PVT_NODEID_IS_VALID(nodeId) || *status < 0)
        return;

    if ((len > 0) &&
        (cmEmEncodeTolerant(hVal, nodeId, pBuffer+*length+sizeof(int), len, &encLen) >= 0) )
    {
        /* copy the length before the encoded buffer */
        memcpy((void*)(pBuffer+*length), (void*)&encLen, sizeof(int));
        *length += encLen+sizeof(int);
    }
    else
    {
        *length += encLen;
        if (len <= encLen)
            *status = RV_ERROR_INSUFFICIENT_BUFFER;
        else
            *status = RV_ERROR_UNKNOWN;
    }
}

/******************************************************************************
 * H245decodeFromBuffer
 * ----------------------------------------------------------------------------
 * General: Decodes the buffer into a node Id.
 *
 * Return Value: RV_OK on success error code (negative values) on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * input  : hVal      - pvt thingie
 *          hSyn      - syntax tree.
 *          nodeId    - pointer to nodeId to decode to
 *          length    - Size of the buffer
 *          lenDone   - pointer to the length done so far
 *          buffer    - The buffer with the encoded nodeId
 * output : lenDone   - updated length done
 *****************************************************************************/
RvStatus H245decodeFromBuffer(
    IN    HPVT              hVal,
    IN    HPST              hSyn,
    IN    RvPvtNodeId       *nodeId,
    IN    RvUint8           *buffer,
    IN    RvSize_t          length,
    INOUT RvSize_t          *lenDone,
    INOUT RvStatus          *status)
{
    int decLen = 0;
    int decoded = 0;

    /* if we already failed, or the node does not exist, exit */
    if ((*status != RV_OK) || !RV_PVT_NODEID_IS_VALID(*nodeId))
    {
        /* make sure we do not delete non-existant nodes */
        *nodeId = RV_PVT_INVALID_NODEID;
        return *status;
    }

    if ((length-*lenDone) >= sizeof(int))
        memcpy((void*)&decLen, (void*)(buffer+*lenDone), sizeof(int));
    else
    {
        *nodeId = -1;
        *status = RV_ERROR_UNKNOWN;
        return RV_ERROR_UNKNOWN;
    }
    *lenDone += sizeof(int);

    if (hSyn != NULL)
    {
        *nodeId = pvtAddRoot(hVal,hSyn,0,NULL);
    }

    if ((cmEmDecode(hVal, *nodeId, buffer+*lenDone, decLen, &decoded) < 0) ||
        (decLen != decoded))
    {
        pvtDelete(hVal, *nodeId);
        *nodeId = -1;
        *status = RV_ERROR_UNKNOWN;
        return RV_ERROR_UNKNOWN;
    }
    *lenDone += decLen;

    return RV_OK;
}

#endif /* (RV_H245_USE_HIGH_AVAILABILITY == RV_YES) */



#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
RvStatus H245SetH225CallEventHandler(
    IN HH245                        hH245,
    IN H245SupportH225CallEvent*    callEvents,
    IN RvSize_t                     size)
{
    H245Object * app = (H245Object*)hH245;
    memcpy(&(app->h225CallEvent), callEvents, RvMin(size, sizeof(H245SupportH225CallEvent)));
    return RV_OK;
}

/************************************************************************
 * cmiStopControlTimers
 * purpose: Stop the control timers after tunneling was rejected.
 * input  : hsCall     - handle to the call.
 * output : none
 * return : none
 ************************************************************************/
RvStatus cmiStopControlTimers(IN HCALL hsCall)
{
    H245Control* ctrl;
    H245Object* app;
    ctrl = (H245Control*)cmiGetControl(hsCall);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

    if (ctrl->eOutCapStatus == CapStatusSent)
    {
        TerminalCapabilitiesSetTimerStop(app, ctrl);
    }
    if (ctrl->msd.state == MsdStateOutgoingAwaitingResponse)
    {
        masterSlaveTimerStop(app, ctrl);
    }
    return RV_OK;
}

/************************************************************************
 * cmiStartControlTimers
 * purpose: Restart the control timers after a real H.245 connection was
 *          established.
 * input  : hsCall     - handle to the call.
 * output : none
 * return : none
 ************************************************************************/
RvStatus cmiStartControlTimers(IN HCALL hsCall)
{
    H245Control* ctrl;
    H245Object* app;
    ctrl = (H245Control*)cmiGetControl(hsCall);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

    if (ctrl->eOutCapStatus == CapStatusSent)
    {
        TerminalCapabilitiesSetTimerRestart(app, ctrl);
    }
    if (ctrl->msd.state == MsdStateOutgoingAwaitingResponse)
    {
        masterSlaveTimerRestart(app, ctrl);
    }
    return RV_OK;
}

#endif /* RV_H245_SUPPORT_H225_PARAMS == RV_YES */



/* Cross Reference functions */

/************************************************************************
 * cmiGetH245Handle
 * purpose: Get the H245 handle.
 *          note that we are counting on the fact that the H.245 handle is declared
 *          first in the cmElem object.
 * input  : hApp    - Stack's application handle
 * output : none
 * return : HH245 handle.
 ************************************************************************/
HH245 cmiGetH245Handle(IN HAPP hApp)
{
    HH245 hH245 = (HH245)hApp;
    H245Object ** ppH245Object;

    if (hApp == NULL)
        return NULL;

    ppH245Object = (H245Object**)hH245;
    return (HH245)*ppH245Object;
}

/************************************************************************
 * cmiGetByControl
 * purpose: get the call handle from the control handle.
 * input  : ctrl         - the control handle.
 * output : none
 * return : The call handle.
 ************************************************************************/
HCALL cmiGetByControl(HCONTROL ctrl)
{
    return (HCALL)((char*)(ctrl) - ((H245Control*)ctrl)->controlOffset);
}

/************************************************************************
 * cmiGetControl
 * purpose: get the control handle from the call handle.
 * input  : call         - the call handle.
 * output : none
 * return : The control handle.
 ************************************************************************/
HCONTROL cmiGetControl(HCALL call)
{
    H245Object *hH245Object = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)call));
    RvUint32   emaType = emaGetType((EMAElement)call);

    if (emaType == H225CALL)
    {
        return (HCONTROL)((char*)(call) + hH245Object->h225CtrlOffset);
    }
    else if (emaType == H223CALL)
    {
        return (HCONTROL)((char*)(call) + hH245Object->h223CtrlOffset);
    }
    return NULL;
}



/************************************************************************
 * cmiGetIsControl
 * purpose: Get the indication for H.245 control connection.
 * input  : hsCall - handle to the call.
 * output : none
 * return : RV_TRUE if the connection exists, otherwise RV_FALSE.
 ************************************************************************/
RvBool cmiGetIsControl(IN HCALL hsCall)
{
    return ((H245Control*)cmiGetControl(hsCall))->bIsControl;
}

/************************************************************************
 * cmiSetIsControl
 * purpose: Set the indication for H.245 control connection.
 * input  : hsCall     - handle to the call.
 *          bIsControl - RV_TRUE if the H.245 connection exists, otherwise RV_FALSE.
 * output : none
 * return : none
 ************************************************************************/
void cmiSetIsControl(IN HCALL hsCall, RvBool bIsControl)
{
    H245Control* ctrl;
    ctrl = (H245Control*)cmiGetControl(hsCall);
    ctrl->bIsControl = bIsControl;
}

#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)

/************************************************************************
 * cmiGetIsFastStartFinished
 * purpose: Get the indication that tells us whether the fast start process was finished.
 * input  : hsCall - handle to the call.
 * output : none
 * return : RV_TRUE if the fast start process was finished, otherwise RV_FALSE.
 ************************************************************************/
RvBool cmiGetIsFastStartFinished(IN HCALL hsCall)
{
    return ((H245Control*)cmiGetControl(hsCall))->bIsFastStartFinished;
}

/************************************************************************
 * cmiSetIsFastStartFinished
 * purpose: Set the indication that tells us whether the fast start process was finished.
 * input  : hsCall               - handle to the call.
 *          bIsFastStartFinished - RV_TRUE if the fast start process was finished, otherwise RV_FALSE.
 * output : none
 * return : none
 ************************************************************************/
void cmiSetIsFastStartFinished(IN HCALL hsCall, RvBool bIsFastStartFinished)
{
    ((H245Control*)cmiGetControl(hsCall))->bIsFastStartFinished = bIsFastStartFinished;
}

/******************************************************************************
 * cmiGetExtendedFastConnectStatus
 * ----------------------------------------------------------------------------
 * General: Get the current status of the EFC.
 *
 * Return Value: The current status. If this is the calling side, the status
 * will be Undetermined until a response is received, at which time it will
 * change to Approved or No. If this is the receiving side, the status will be
 * No, Offered or Required, until the local support is set, in which time it
 * will change to Approved or No.
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsCall  - handle to the call
 *         bOrigin - RV_TRUE if we are the origin of the call
 * Output: None.
 *****************************************************************************/
cmExtendedFastConnectState_e cmiGetExtendedFastConnectStatus(IN HCALL  hsCall,
                                                            IN RvBool bOrigin)
{
    H245Control* ctrl = ((H245Control*)cmiGetControl(hsCall));
    if (bOrigin)
    {
        if ((ctrl->eExtendedFastConnectState == cmExtendedFastConnectApproved) ||
            (ctrl->eExtendedFastConnectState == cmExtendedFastConnectNo))
        {
            return ctrl->eExtendedFastConnectState;
        }
        else
        {
            return cmExtendedFastConnectUndetermined;
        }
    }
    else
    {
        return ctrl->eExtendedFastConnectState;
    }
}

/******************************************************************************
 * cmiSetExtendedFastConnectStatus
 * ----------------------------------------------------------------------------
 * General: Set the current side's support of EFC.
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsCall      - Handle to the call
 *         newEFCState - the current side's support of EFC. must be either No,
 *                       Supported or Required.
 * Output: None.
 *****************************************************************************/
RvStatus cmiSetExtendedFastConnectStatus(IN HCALL hsCall,
                                        IN cmExtendedFastConnectState_e newEFCState)
{
    H245Control* ctrl = ((H245Control*)cmiGetControl(hsCall));
    H245Object * app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
	RvStatus     res;

    H245APIEnter((app->pAppObject, "cmiSetExtendedFastConnectStatus: hsCall=%p, fastStartState=%d, efcState=%d", 
        hsCall, ctrl->fastStartState,  newEFCState));

    if (!app->bSupportEFC)
        res = RV_ERROR_NOTSUPPORTED;
	else 
	{
		if ((ctrl->eRemExtendedFastConnectState == cmExtendedFastConnectSupported) && 
			(newEFCState == cmExtendedFastConnectSupported))
		   ctrl->eExtendedFastConnectState = cmExtendedFastConnectApproved;
		else 
        {
            /* no need to change already approved state to "supported" or "required"*/
            if ((ctrl->eExtendedFastConnectState != cmExtendedFastConnectApproved) ||
    		((newEFCState != cmExtendedFastConnectSupported) && (newEFCState != cmExtendedFastConnectRequired))) 
            {
            	ctrl->eExtendedFastConnectState = newEFCState;	
            }
        }

		res = RV_OK;
	}
    
    H245APIExit((app->pAppObject, "cmiSetExtendedFastConnectStatus: efcState=%d res=%d", ctrl->eExtendedFastConnectState, res));

    return res;
}

/******************************************************************************
 * cmiSetRemoteExtendedFastConnectStatus
 * ----------------------------------------------------------------------------
 * General: Set the EFC state based on remote status
 *
 * Return Value: Non-negative value on success, Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * input : hsCall      - Handle to the call
 *		   bOrigin     - TRUE is call originator	
 *         remEFCState - the remote side's support of EFC. must be either No,
 *                       Supported or Required.
 * Output: None.
 *****************************************************************************/
RvStatus cmiSetRemoteExtendedFastConnectStatus(IN HCALL hsCall,
											   IN RvBool bOrigin,
                                               IN cmExtendedFastConnectState_e remEFCState)
{
    H245Control* ctrl = ((H245Control*)cmiGetControl(hsCall));
    H245Object *app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

	RV_UNUSED_ARG(bOrigin);
	

    H245APIEnter((app->pAppObject, "cmiSetRemoteExtendedFastConnectStatus: hsCall=%p, fastStartState=%d, efcState=%d remEFCState=%d ", 
        hsCall, ctrl->fastStartState,  ctrl->eExtendedFastConnectState, remEFCState));
    

	ctrl->eRemExtendedFastConnectState = remEFCState;

    /* if case we do not support or do not want EFC */
    if ((!app->bSupportEFC) || (ctrl->eExtendedFastConnectState == cmExtendedFastConnectNo))
    {
        /* if other side required EFC, we should return an error */
        if (remEFCState == cmExtendedFastConnectRequired)
        {
            H245APIExit((app->pAppObject, "cmiSetRemoteExtendedFastConnectStatus 1: hsCall=%p, fastStartState=%d, efcState=%d remEFCState=%d",
                hsCall, ctrl->fastStartState,  ctrl->eExtendedFastConnectState, ctrl->eRemExtendedFastConnectState));
            return RV_ERROR_NOTSUPPORTED;
        }

        /* cancel EFC, if it was supported */
        ctrl->eExtendedFastConnectState = cmExtendedFastConnectNo;
        H245APIExit((app->pAppObject, "cmiSetRemoteExtendedFastConnectStatus 2: hsCall=%p, fastStartState=%d, efcState=%d remEFCState=%d",
                hsCall, ctrl->fastStartState,  ctrl->eExtendedFastConnectState, ctrl->eRemExtendedFastConnectState));
        return RV_OK;
    }

    /* see if EFC was already approved */
    if (ctrl->eExtendedFastConnectState == cmExtendedFastConnectApproved)
    {
        /* check if it was now canceled */
        if (remEFCState == cmExtendedFastConnectNo)
        {
            /* EFC was canceled */
            ctrl->eExtendedFastConnectState = cmExtendedFastConnectNo; /* this COULD resolve bug 56062:  set state to cmExtendedFastConnectSupported */ 
        }

        H245APIExit((app->pAppObject, "cmiSetRemoteExtendedFastConnectStatus 3: hsCall=%p, fastStartState=%d, efcState=%d remEFCState=%d",
                hsCall, ctrl->fastStartState,  ctrl->eExtendedFastConnectState, ctrl->eRemExtendedFastConnectState));
        return RV_OK;
    }

    /* we support EFC, check if other side supports it as well */
    if ((remEFCState == cmExtendedFastConnectSupported) || 
        (remEFCState == cmExtendedFastConnectRequired))
    {
        ctrl->eExtendedFastConnectState = cmExtendedFastConnectApproved;
        H245APIExit((app->pAppObject, "cmiSetRemoteExtendedFastConnectStatus 4: hsCall=%p, fastStartState=%d, efcState=%d remEFCState=%d",
                hsCall, ctrl->fastStartState,  ctrl->eExtendedFastConnectState, ctrl->eRemExtendedFastConnectState));
        return RV_OK;
    }

    /* the other side does not support EFC. if we required it, we should return an error */
    if (ctrl->eExtendedFastConnectState == cmExtendedFastConnectRequired)
    {
        H245APIExit((app->pAppObject, "cmiSetRemoteExtendedFastConnectStatus 5: hsCall=%p, fastStartState=%d, efcState=%d remEFCState=%d",
                hsCall, ctrl->fastStartState,  ctrl->eExtendedFastConnectState, ctrl->eRemExtendedFastConnectState));
        return RV_ERROR_NOTSUPPORTED;
    }

    /* cancel support of EFC and return */
    H245APIExit((app->pAppObject, "cmiSetRemoteExtendedFastConnectStatus: hsCall=%p, fastStartState=%d, efcState=%d remEFCState=%d",
                hsCall, ctrl->fastStartState,  ctrl->eExtendedFastConnectState, ctrl->eRemExtendedFastConnectState));
    return RV_OK;
}

#endif

/************************************************************************
 * cmiGetFirstChannel
 * purpose: Get the first channel in the channel list.
 * input  : hsCall - handle to the call.
 * output : none
 * return : handle of the first channel.
 ************************************************************************/
HCHAN cmiGetFirstChannel(IN HCALL hsCall)
{
    H245Control *ctrl = (H245Control*)cmiGetControl(hsCall);
    return ctrl->hFirstChannel;
}

/************************************************************************
 * cmiGetNextChannel
 * purpose: Get the next channel in the channel list.
 * input  : hsChan - handle to the channel.
 * output : none
 * return : handle of the next channel.
 ************************************************************************/
HCHAN cmiGetNextChannel(IN HCHAN hsChan)
{
    H245Channel *pChannel = (H245Channel*)hsChan;
    return (HCHAN)(pChannel->pNext);
}


/******************************************************************************
 * cmiGetSaveCallIndication
 * ----------------------------------------------------------------------------
 * General: return the value of bSaveCallIndication flag from the control.
 *
 * Return Value: bSaveCallIndication value
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:   hsCall          -   The call handle.
 *
 * Output:  None..
 *****************************************************************************/
RvBool cmiGetSaveCallIndication(IN HCALL hsCall)
{
    H245Control *ctrl;
    ctrl = (H245Control*)cmiGetControl(hsCall);
    return ctrl->bSaveCallIndication;
}

/************************************************************************
 * cmiSetSaveCallIndication
 * purpose: Sets the callSaveIndication if the call not using MPC. 
 * input  : hCtrl               - Stack handle for the H245 call control
 *          saveCallIndication  - Value of to set.
 * output : none
 * return : RV_OK on success, positive values if MPC call,
 *          RV_ERRORs on failure
 ************************************************************************/
RvStatus cmiSetSaveCallIndication(
    IN HCONTROL     hCtrl,
    IN RvBool       saveCallIndication)
{
    H245Control* ctrl = (H245Control*)hCtrl;
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
#if (RV_3G324M_USE_MONA == RV_YES)
        if (ctrl->bMPCinUse && saveCallIndication == RV_TRUE)
        {
            /* check that we don't have phantom channels */
            H245Channel     *curChan = (H245Channel *)ctrl->hFirstChannel;
            while (curChan != NULL)
            {
                if (curChan->data.h223Params.bIsMPCChannel == RV_TRUE)
                    /* in this case we don't set the flag of bSaveCallIndication */
                    return RV_TRUE;
                curChan = curChan->pNext;
            }
        }
#endif /* USE_MONA */
#endif
        ctrl->bSaveCallIndication = saveCallIndication;

        return RV_OK;
}

#if (RV_H245_SUPPORT_H223_PARAMS == 1)

/************************************************************************
 * cmiGetChannelDataType
 * purpose: Get the data type of the channel.
 * input  : hsChan - handle to the channel.
 * output : none
 * return : Data type.
 ************************************************************************/
cmChannelDataType cmiGetChannelDataType(IN HCHAN hsChan)
{
    H245Channel *pChannel = (H245Channel*)hsChan;
    return pChannel->data.h223Params.eDataType;
}


/************************************************************************
 * cmiGetControlReset
 * purpose: Get the indication for H.245 module that the user requested
 *          to perform a reset procedure.
 * input  : hsCall - handle to the call.
 * output : none
 * return : RV_TRUE if the user requested reset procedure, otherwise RV_FALSE.
 ************************************************************************/
RvBool cmiGetControlReset(IN HCALL hsCall)
{
    return ((H245Control*)cmiGetControl(hsCall))->bReset;
}

/************************************************************************
 * cmiGetControlResetInitiator
 * purpose: Get the indication for H.245 module that the user is the initiator
 *          of the Reset procedure.
 * input  : hsCall - handle to the call.
 * output : none
 * return : RV_TRUE if the user is the initiator of the reset procedure, otherwise RV_FALSE.
 ************************************************************************/
RvBool cmiGetControlResetInitiator(IN HCALL hsCall)
{
    return ((H245Control*)cmiGetControl(hsCall))->bResetInitiator;
}

/************************************************************************
 * cmiSetControlResetInitiator
 * purpose: Get the indication from H.245 module that the user is the initiator
 *          of the Reset procedure.
 * input  : hsCall     - handle to the call.
 *          bReset - RV_TRUE if to reset the call, otherwise RV_FALSE.
 * output : none
 * return : none
 ************************************************************************/
void cmiSetControlResetInitiator(IN HCALL hsCall, RvBool bResetInitiator)
{
    H245Control *ctrl;
    ctrl = (H245Control*)cmiGetControl(hsCall);
    ctrl->bResetInitiator = bResetInitiator;
}
#endif /* (RV_H245_SUPPORT_H223_PARAMS == 1) */

extern RvBool channelTimeoutEventsHandler(void* hsChan);


#if (RV_H245_USE_HIGH_AVAILABILITY == RV_YES)
/******************************************************************************
 * cmiStartChannelTimer
 * ----------------------------------------------------------------------------
 * General: start the channel timer.
 *
 * Return Value: None
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:   hVal          -   The h245 value tree
 *          hsChan        -   The channel handle
 * Output:  ppTimer       -   The channel's timer
 *****************************************************************************/
void cmiStartChannelTimer(IN  HPVT      hVal,
                          IN  HCHAN     hsChan,
                          OUT RvTimer   **ppTimer)
{

    int timeout = 29;
    H245Channel* pChannel= (H245Channel*)hsChan;
    H245Object* app = (H245Object *)emaGetUserData((EMAElement)hsChan);

    pvtGetChildByFieldId(hVal, app->h245Conf, __h245(channelsTimeout), &(timeout), NULL);
    *ppTimer = app->evHandlers.pfnTimerStartEv(pChannel->hCtrl,
    channelTimeoutEventsHandler, (void*)pChannel, timeout * 1000);
}



#if (RV_H245_SUPPORT_H223_PARAMS == 1)
/******************************************************************************
 * cmiCtrlSaveToBuffer
 * ----------------------------------------------------------------------------
 * General: Saves the control object and the channels into a buffer.
 *
 * Return Value: RV_OK on success, error value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hVal         - The value tree.
 *         ctrl         - The control object.
 *         chanList     - The call channel list.
 *         buffLen      - The size of the buffer.
 *         lenDone      - The used size before saving.
 *         pStatus      - status before saving.
 *
 * Output: pBuffer      - The buffer.
 *         lenDone      - The used/required size after saving.
 *         pStatus      - status after saving.
 *****************************************************************************/
void cmiCtrlSaveToBuffer(IN     HPVT            hVal,
                         IN     H245Control*    ctrl,
                         IN     H245Channel *   chanList,
                         OUT    RvUint8*        pBuffer,
                         IN     RvSize_t        buffLen,
                         INOUT  RvSize_t        *lenDone,
                         INOUT  RvStatus        *pStatus)
{
    H245Channel *curChan = chanList;
    int i = 0;

     /* copy the control element to the buffer */
    if (*pStatus == RV_OK && ((*lenDone+sizeof(H245Control)) <= buffLen))
    {
        memcpy((void*)(pBuffer+*lenDone), (void*)ctrl, sizeof(H245Control));
    }
    else
        *pStatus = RV_ERROR_INSUFFICIENT_BUFFER;

    *lenDone += sizeof(H245Control);

    /*copy capabilities*/
    if (ctrl->eState != ctrlNotInitialized)
    {
        /* encode the capabilities into the buffer, if they exists */
        H245encodeIntoBuffer(hVal, ctrl->outCap.termNodeId, pBuffer, buffLen, lenDone, pStatus);
        H245encodeIntoBuffer(hVal, ctrl->inCap.termNodeId,  pBuffer, buffLen, lenDone, pStatus);
    }

    /* count the channels */
    while (curChan)
    {
        curChan = (H245Channel *)cmiGetNextChannel((HCHAN)curChan);
        i++;
    }

    if (*pStatus == RV_OK && (*lenDone+i*(sizeof(H245Channel) + sizeof(H245Channel*))+sizeof(int)) <= buffLen)
    {
        /*first copy number of channels*/
        memcpy((void*)(pBuffer+*lenDone), (void*)&i, sizeof(int));
    }
    else
        *pStatus = RV_ERROR_INSUFFICIENT_BUFFER;

    *lenDone += sizeof(int);

    /*copy the channels*/
    curChan = chanList;
    while (curChan != NULL)
    {
        if (*pStatus == RV_OK)
        {
            /*first copy the channel address*/
            memcpy((void*)(pBuffer+*lenDone), (void*)(&curChan), sizeof(H245Channel*));
            *lenDone += sizeof(H245Channel*);

            /*copy the channel object*/
            memcpy((void*)(pBuffer+*lenDone), (void*)(curChan), sizeof(H245Channel));
            *lenDone += sizeof(H245Channel);

            /*encode the dataType node Id*/
            H245encodeIntoBuffer(hVal, curChan->dataTypeID, pBuffer, buffLen, lenDone, pStatus);
            /*encode the reverseDataTypeID node Id, if not the same*/
            if (curChan->dataTypeID != curChan->reverseDataTypeID)
            {
                H245encodeIntoBuffer(hVal, curChan->reverseDataTypeID, pBuffer, buffLen, lenDone, pStatus);
            }
        }
        else
            *pStatus = RV_ERROR_INSUFFICIENT_BUFFER;
        curChan = (H245Channel *)cmiGetNextChannel((HCHAN)curChan);;
    }
}

/******************************************************************************
 * cmiCtrlClean
 * ----------------------------------------------------------------------------
 * General: Cleans the channels and capabilities of a call before
 *          update for HighAvailability.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hVal         - The value tree.
 *         ctrl         - The control object.
 *         chanList     - The call channel list.
 * Output: None.
 *****************************************************************************/
void cmiCtrlClean(IN HPVT           hVal,
                  IN H245Control    *ctrl,
                  IN H245Channel    *chanList)
{
    H245Channel *curChan = chanList, *nextChan;
    /*clean capabilities*/
    if (ctrl->eState != ctrlNotInitialized)
    {
        /*remove existing capabilities*/
        if (RV_PVT_NODEID_IS_VALID(ctrl->outCap.termNodeId))
        {
            pvtDelete(hVal, ctrl->outCap.termNodeId);
        }
        if (RV_PVT_NODEID_IS_VALID(ctrl->inCap.termNodeId))
        {
            pvtDelete(hVal, ctrl->inCap.termNodeId);
        }
    }
    /* remove the existing channels */
    while (curChan != NULL)
    {
        nextChan = (H245Channel *)cmiGetNextChannel((HCHAN)curChan);

        /*lock the channel - just for the sake of ema not shouting its head off*/
        emaLock((EMAElement)curChan); // TODO: Replace with Ran's new API

        if (curChan->dataTypeID != curChan->reverseDataTypeID)
        {
            pvtDelete(hVal, curChan->reverseDataTypeID);
        }
        pvtDelete(hVal, curChan->dataTypeID);
        emaDelete(curChan);
        emaUnlock((EMAElement)curChan);
        curChan = nextChan;
    }

}

/******************************************************************************
 * cmiCtrlGetFromBuffer
 * ----------------------------------------------------------------------------
 * General: Restores a control object, including channels and capabilities
 *          from buffer.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hVal         - The value tree.
 *         hSynProtH245 - H245 message syntax.
 *         h245DataType - Channel DataType syntax tree.
 *         newControl   - The new control object
 *         buffLen      - The size of the buffer.
 *         lenDone      - The used size before saving.
 *         pStatus      - status before saving.
 *
 * Output:
 *         pBuffer      - The buffer.
 *         lenDone      - The used/required size after saving.
 *         pStatus      - status after saving.
 *****************************************************************************/
void cmiCtrlGetFromBuffer(
    IN    HPVT            hVal,
    IN    HPST            hSynProtH245,
    IN    HPST            h245DataType,
    OUT   H245Control     *newControl,
    IN    RvUint8         *pBuffer,
    IN    RvSize_t        bufLen,
    INOUT RvSize_t        *lenDone,
    INOUT RvStatus        *pStatus)
{
    RvSize_t    chanArraySize = 0;
    H245Control control;
    H245Channel *newChannelArray[20], *channelOldAddr[20];
    int i, j;

    /*get the control from the buffer*/
    if ((bufLen-*lenDone) >= sizeof(H245Control))
        memcpy((void*)&control, (void*)(pBuffer+*lenDone), sizeof(H245Control));
    else
    {
        *pStatus = RV_ERROR_UNKNOWN;
        return;
    }
    *lenDone += sizeof(H245Control);

    memset(newChannelArray, 0, sizeof(newChannelArray));
    memset(channelOldAddr, 0, sizeof(channelOldAddr));

     /* rebuild the capabilities, if needed */
    if (control.eState != ctrlNotInitialized)
    {
        if (RV_PVT_NODEID_IS_VALID(control.outCap.termNodeId))
        {
            control.outCap.termNodeId = pvtAddRootByPath(hVal, hSynProtH245,
                (char*)"request.terminalCapabilitySet", 0, NULL);

            if (H245decodeFromBuffer(hVal, NULL, &control.outCap.termNodeId, pBuffer, bufLen, lenDone, pStatus) < 0)
            {
                *pStatus = RV_ERROR_UNKNOWN;
                return;
            }
        }
        if (RV_PVT_NODEID_IS_VALID(control.inCap.termNodeId))
        {
            control.inCap.termNodeId = pvtAddRootByPath(hVal, hSynProtH245,
                (char*)"request.terminalCapabilitySet", 0, NULL);

            if (H245decodeFromBuffer(hVal, NULL, &control.inCap.termNodeId, pBuffer, bufLen, lenDone, pStatus) < 0)
            {
                pvtDelete(hVal, control.outCap.termNodeId);
                *pStatus = RV_ERROR_UNKNOWN;
                return;
            }
        }
    }
    else
    {
        control.outCap.termNodeId = -1;
        control.inCap.termNodeId = -1;
    }

    /* set the timers to NULL */
    control.outCap.timer = NULL;
    control.msd.timer = NULL;
    control.roundTrip.timer = NULL;
    control.outRequestMode.timer = NULL;
    control.outMES.timer = NULL;

    /*remove channel list from control*/
    control.hFirstChannel = NULL;

    /*get the control instance from the buffer*/
    memcpy((void*)newControl, (void*)&control, sizeof(H245Control));

    /*get the number of channels from the buffer*/
    if (*lenDone < bufLen)
    {
        memcpy((void*)&chanArraySize, (void*)(pBuffer+*lenDone), sizeof(int));
        *lenDone += sizeof(int);
    }

    if (bufLen-*lenDone < chanArraySize*sizeof(H245Channel))
    {
        pvtDelete(hVal, control.outCap.termNodeId);
        pvtDelete(hVal, control.inCap.termNodeId);
        *pStatus = RV_ERROR_UNKNOWN;
        return;
    }

    /*create the new channels and set them in newChannelArray*/
    for (i = 0; i<(int)chanArraySize; i++)
    {
        H245Channel *pPrev, *pNext;
        newChannelArray[i] = allocateChannel((HCONTROL)newControl);

        /*save the good pPrev & pNext*/
        pNext = newChannelArray[i]->pNext;
        pPrev = newChannelArray[i]->pPrev;

        if (newChannelArray[i])
        {
            RvPvtNodeId oldDataTypeId = RV_PVT_INVALID_NODEID;

            *pStatus = RV_OK;
            /*copy the old address of the channel to the channelOldAddr array*/
            memcpy((void*)&channelOldAddr[i], (void*)(pBuffer+*lenDone), sizeof(H245Channel*));
            *lenDone += sizeof(H245Channel*);
            /*copy the channel into chanArray*/
            memcpy((void*)newChannelArray[i], (void*)(pBuffer+*lenDone), sizeof(H245Channel));
            *lenDone += sizeof(H245Channel);

            /*decode the dataType and reverseDataType nodes from the buffer*/
            if (RV_PVT_NODEID_IS_VALID(newChannelArray[i]->dataTypeID))
            {
                oldDataTypeId = newChannelArray[i]->dataTypeID;
                H245decodeFromBuffer(hVal, h245DataType, &(newChannelArray[i]->dataTypeID),
                    pBuffer, bufLen, lenDone, pStatus);
            }
            if (RV_PVT_NODEID_IS_VALID(newChannelArray[i]->reverseDataTypeID))
            {
                if (newChannelArray[i]->reverseDataTypeID == oldDataTypeId)
                {
                    newChannelArray[i]->reverseDataTypeID = newChannelArray[i]->dataTypeID;
                }
                else
                {
                    H245decodeFromBuffer(hVal, h245DataType, &(newChannelArray[i]->reverseDataTypeID),
                        pBuffer, bufLen, lenDone, pStatus);
                }
            }

            if (*pStatus == RV_OK)
            {
                /*restore the control in the channel*/
                newChannelArray[i]->hCtrl = (HCONTROL)newControl;
                /*restore pNext & pPrev*/
                newChannelArray[i]->pNext = pNext;
                newChannelArray[i]->pPrev = pPrev;

                /*reset timers*/
                newChannelArray[i]->pRequestCloseTimer = NULL;
                newChannelArray[i]->pMediaLoopTimer = NULL;
                /* all ok, skip the deleting process */
                continue;
            }
            /* if we are here, we must delete this channel... */
            if (newChannelArray[i]->reverseDataTypeID != newChannelArray[i]->dataTypeID)
            {
                pvtDelete(hVal, newChannelArray[i]->reverseDataTypeID);
            }
            pvtDelete(hVal, newChannelArray[i]->dataTypeID);
            emaDelete((EMAElement)newChannelArray[i]);
        }

        /* ...and all those before it... */
        for(j=0; j < i; j++)
        {
            if (newChannelArray[i]->reverseDataTypeID != newChannelArray[i]->dataTypeID)
            {
                pvtDelete(hVal, newChannelArray[i]->reverseDataTypeID);
            }
            pvtDelete(hVal, newChannelArray[j]->dataTypeID);
            emaDelete((EMAElement)newChannelArray[j]);
        }
        /* ...and the ccapabilities */

        pvtDelete(hVal, control.inCap.termNodeId);
        pvtDelete(hVal, control.outCap.termNodeId);
        *pStatus = RV_ERROR_UNKNOWN;
        return;
    }

    /*set pReplacementCh & pBase in the channel if required*/
    for (i = 0 ; i < (int)chanArraySize ; i++)
    {
        if (newChannelArray[i]->pReplacementCh != 0)
        {
            /*find the channel*/
            for (j = 0 ; j < (int)chanArraySize ; j++)
            {
                if (channelOldAddr[j] == newChannelArray[i]->pReplacementCh)
                {
                    /*this is it*/
                    newChannelArray[i]->pReplacementCh = newChannelArray[j];
                }
            }
        }

        if (newChannelArray[i]->pBase != 0)
        {
            /*its the channel's lcn - find the channel*/
            for (j = 0 ; j < (int)chanArraySize ; j++)
            {
                if (channelOldAddr[j] == newChannelArray[i]->pBase)
                {
                    /*this is it*/
                    newChannelArray[i]->pBase = newChannelArray[j];
                }
            }
        }
    }

}

/******************************************************************************
 * cmiCtrlActivateH223Channel
 * ----------------------------------------------------------------------------
 * General: Calls the H324mH223LogicalChannelEv for each channel to open the h223
 *          objects of the channel.
 *
 * Return Value: RV_OK on success, negative values on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsChan           - The channel handle in the stack.
 * Output:None.
 *****************************************************************************/
RvStatus cmiCtrlActivateH223Channel(IN HCHAN    hsChan)
{
    H245Channel *pChannel = (H245Channel *)hsChan;
    HH245       hApp = (HH245)emaGetUserData((EMAElement)pChannel);
    H245Object  *pApp = (H245Object*)hApp;
    H245Control *pCtrl = (H245Control*)(pChannel->hCtrl);

    if (pApp->evHandlers.pfnH223LogicalChannelEv != NULL)
    {
        return pApp->evHandlers.pfnH223LogicalChannelEv(cmiGetByControl((HCONTROL)pCtrl),
                                                        hsChan,
                                                        RV_TRUE);
    }
    return RV_OK;
}
#endif /* (RV_H245_SUPPORT_H223_PARAMS == 1) */
#endif /* (RV_H245_USE_HIGH_AVAILABILITY == RV_YES) */

#if (RV_H245_SUPPORT_H223_PARAMS == 1)
/******************************************************************************
 * cmiResetH223ACPIgnoreDataTimer
 * ----------------------------------------------------------------------------
 * General: Calls the H324mResetH223ACPIgnoreDataEv. Called every time an
 *          incoming OLC is received and channel is not rejected, to avoid checking
 *          if a received data on an ACP channel should be ignored.
 *
 * Return Value: RV_OK on success, negative values on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsChan           - The channel handle in the stack.
 * Output:None.
 *****************************************************************************/
RvStatus cmiResetH223ACPIgnoreDataTimer(IN HCHAN    hsChan)
{
	H245Channel *pChannel = (H245Channel *)hsChan;
    HH245       hApp = (HH245)emaGetUserData((EMAElement)pChannel);
    H245Object  *pApp = (H245Object*)hApp;

    if (pApp->evHandlers.pfnResetH223ACPIgnoreData != NULL)
    {
        return pApp->evHandlers.pfnResetH223ACPIgnoreData(hsChan);
    }
    return RV_OK;
}

/******************************************************************************
 * cmiClearH223ACPChannel
 * ----------------------------------------------------------------------------
 * General: Calls the H324mClearH223ACPChannelEv. Called every time an
 *          incoming channel is rejected or droped.         
 *
 * Return Value: RV_OK on success, negative values on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsChan           - The channel handle in the stack.
 *         isVideo          - RV_TRUE if channel is a video channel - RV_FALSE
 *                            if it's an audio channel.
 * Output:None.
 *****************************************************************************/
RvStatus cmiClearH223ACPChannel(IN HCHAN    hsChan, 
                                IN RvBool   isVideo)
{
    H245Channel *pChannel = (H245Channel *)hsChan;
    HH245       hApp = (HH245)emaGetUserData((EMAElement)pChannel);
    H245Object  *pApp = (H245Object*)hApp;

    if (pApp->evHandlers.pfnClearH223ACPChannel != NULL)
    {
        return pApp->evHandlers.pfnClearH223ACPChannel(hsChan, isVideo);
    }
    return RV_OK;
}

/******************************************************************************
 * cmiSetH223ACPDefaultMuxTbl
 * ----------------------------------------------------------------------------
 * General: Sets default mux table for ACP .         
 *
 * Return Value: RV_OK on success, negative values on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall      - The call handle in the stack.
 * Output:None.
 *****************************************************************************/
RvStatus cmiSetH223ACPDefaultMuxTbl(IN HCALL    hsCall)
{

    H245Object  *pApp = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

    if (pApp->evHandlers.pfnSetH223ACPDefaultMuxTbl != NULL)
    {
        return pApp->evHandlers.pfnSetH223ACPDefaultMuxTbl(hsCall);
    }
    return RV_OK;
}

/******************************************************************************
 * cmiSetCtrlACP
 * ----------------------------------------------------------------------------
 * General: Sets the bACP boolean in the control object .         
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall      - The call handle in the stack.
 *         value       - The value to set.
 * Output:None.
 *****************************************************************************/
void cmiSetCtrlACP(IN HCALL    hsCall, RvBool value)
{
   H245Control *pCtrl = (H245Control *)cmiGetControl(hsCall);
   pCtrl->bACPinUse = value;
}

#if (RV_3G324M_USE_MONA == RV_YES)
/*****************************************************************************
 * cmiSetCtrlMPC
 * ----------------------------------------------------------------------------
 * General: Sets the bMPCinUse boolean in the control object .         
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall      - The call handle in the stack.
 *         value       - The value to set.
 * Output:None.
 *****************************************************************************/
void cmiSetCtrlMPC(IN HCALL hsCall, RvBool value)
{
   H245Control *pCtrl = (H245Control *)cmiGetControl(hsCall);
   pCtrl->bMPCinUse = value;
}
#endif /* USE_MONA */
#endif

#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
/* Util functions */

int cmVtToTA_H245(HPVT hVal,int nodeId, cmTransportAddress* ta)
{
    RvUint32 p;
    int vNodeId, addrNodeId;
    int len;
    int result = RV_OK;

    static const cmTransportType asn2typeU[]={
        cmTransportTypeIP,
        (cmTransportType)-1,
        cmTransportTypeIPv6,
        (cmTransportType)-1,
        cmTransportTypeIPStrictRoute,
        cmTransportTypeNSAP,
        (cmTransportType)-1};
    static const cmTransportType asn2typeM[]={
        cmTransportTypeIP,
        cmTransportTypeIPv6,
        cmTransportTypeNSAP,
        (cmTransportType)-1};

    memset(ta,0,sizeof(cmTransportAddress));
    if (nodeId<0)
        return RV_ERROR_UNKNOWN;
    addrNodeId=pvtChild(hVal,nodeId);

    ta->distribution=(cmDistribution)(pvtGetSyntaxIndex(hVal,addrNodeId)-1);
    addrNodeId=pvtChild(hVal,addrNodeId);
    ta->type=(ta->distribution==cmDistributionMulticast)?
        asn2typeM[pvtGetSyntaxIndex(hVal,addrNodeId)-1]:
        asn2typeU[pvtGetSyntaxIndex(hVal,addrNodeId)-1];
    switch(ta->type)
    {
        case cmTransportTypeIP:
            if (pvtGetChild(hVal, addrNodeId, __h245(network), &vNodeId)<0) return RV_ERROR_UNKNOWN;
            if (pvtGet(hVal, vNodeId,NULL,NULL,&len,NULL)<0) return RV_ERROR_UNKNOWN;
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
            if (len!=sizeof(RvUint32)) ta->addr.v4.ip=0;
            else pvtGetString(hVal,vNodeId,sizeof(RvUint32),(char*)&(ta->addr.v4.ip));

#else
            if (len!=sizeof(RvUint32)) ta->ip=0;
            else pvtGetString(hVal,vNodeId,sizeof(RvUint32),(char*)&(ta->ip));
#endif

            if (pvtGetChild(hVal, addrNodeId, __h245(tsapIdentifier), &vNodeId)<0) return RV_ERROR_UNKNOWN;

            if (pvtGet(hVal, vNodeId,NULL,NULL,(RvInt32*)&p,NULL)<0) return RV_ERROR_UNKNOWN;
            ta->port=(RvUint16)p;
        break;

#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
        case cmTransportTypeIPv6:
            if (pvtGetChild(hVal, addrNodeId, __h245(network), &vNodeId)<0) return RV_ERROR_UNKNOWN;
            if (pvtGet(hVal, vNodeId,NULL,NULL,&len,NULL)<0) return RV_ERROR_UNKNOWN;
            if (len!=sizeof(ta->addr.v6.ip)) memset(ta->addr.v6.ip, 0, sizeof(ta->addr.v6.ip));
            else pvtGetString(hVal, vNodeId, sizeof(ta->addr.v6.ip), (char*)ta->addr.v6.ip);

            if (pvtGetChild(hVal, addrNodeId, __h245(tsapIdentifier), &vNodeId)<0) return RV_ERROR_UNKNOWN;

            if (pvtGet(hVal, vNodeId,NULL,NULL,(RvInt32*)&p,NULL)<0) return RV_ERROR_UNKNOWN;
            ta->port=(RvUint16)p;
            break;
#endif

#if (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_IPV4_ONLY)
        case cmTransportTypeNSAP:
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
            ta->addr.v4.length=(RvUint16)pvtGetString(hVal,addrNodeId,20,(char*)ta->addr.v4.route);
#else
            ta->length=(RvUint16)pvtGetString(hVal,addrNodeId,20,(char*)ta->route);
#endif
        break;
#endif

        default:
            result = RV_ERROR_UNKNOWN;
        break;
    }
    return result;
}

int cmTAToVt_H245(HPVT hVal,int nodeId, cmTransportAddress* ta)
{
    int addrNodeId = RV_PVT_INVALID_NODEID;
    int iPort;
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    if (ta->type == cmTransportTypeIP)
    {
        addrNodeId= pvtAddBranch(hVal,nodeId,(ta->distribution)?__h245(multicastAddress):__h245(unicastAddress));
        addrNodeId= pvtAddBranch(hVal, addrNodeId, __h245(iPAddress));
        pvtAdd(hVal, addrNodeId, __h245(network),sizeof(RvUint32),(ta->addr.v4.ip)?((char*)&(ta->addr.v4.ip)):(char*)"\0\0\0", NULL);
    }
    if (ta->type == cmTransportTypeIPv6)
    {
        addrNodeId= pvtAddBranch(hVal,nodeId,(ta->distribution)?__h245(multicastAddress):__h245(unicastAddress));
        addrNodeId= pvtAddBranch(hVal, addrNodeId, __h245(iP6Address));
        pvtAdd(hVal, addrNodeId, __h245(network),sizeof(ta->addr.v6.ip),(char*)ta->addr.v6.ip, NULL);
    }
#else
    addrNodeId= pvtAddBranch(hVal,nodeId,(ta->distribution)?__h245(multicastAddress):__h245(unicastAddress));
    addrNodeId= pvtAddBranch(hVal, addrNodeId, __h245(iPAddress));
    pvtAdd(hVal, addrNodeId, __h245(network),sizeof(RvUint32),(ta->ip)?((char*)&(ta->ip)):(char*)"\0\0\0", NULL);
#endif
    iPort = ta->port;
    return pvtAdd(hVal, addrNodeId,__h245(tsapIdentifier),iPort,NULL,NULL);
}


#endif  /* (RV_H245_SUPPORT_H225_PARAMS == RV_YES) */



/* non standard parameter________________________________________________________________________________ */




/******************************************************************************
 * cmNonStandardParameterCreate
 * ----------------------------------------------------------------------------
 * General: Creates a NonStandardParameter PVT node id.
 *
 * See also: cmNonStandardParam, cmNonStandardIdentifier
 * Return Value: The root PVT node id - if successful.
 *               Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hVal       - The PVT handle.
 *         rootID     - The root node id provided by application (nonStandardData) to which the nonStandardParameter is going to
 *                      be built.
 *         identifier - The cmNonStandardIdentifier structure which is included in nonStandardParameter.
 *         data       - The data string which is included in nonStandardParameter.
 *         dataLength - The length in bytes of the string provided in the data parameter.
 *****************************************************************************/
RVAPI int RVCALLCONV cmNonStandardParameterCreate(
                 IN  HPVT                     hVal,
                 IN  int                      rootId,
                 IN  cmNonStandardIdentifier  *identifier,
                 IN  const RvChar             *data,
                 IN  int                      dataLength
                 )
{
    int ret = -1;

    if (!hVal || rootId<0 || !data || dataLength<0 || !identifier) return RV_ERROR_UNKNOWN;

    if (identifier->objectLength >0) {
        char buff[128];
        int buffLen;

        strcpy(buff, "");

        buffLen = oidEncodeOID(sizeof(buff), buff, identifier->object);
        if (buffLen>0)
        {
            /* The GetByFieldId functions cannot be used in this function since both Q931 and H.245
               ASN.1 definitions use this function! */
            pvtBuildByPath(hVal, rootId, "data", dataLength, data);
            ret=pvtBuildByPath(hVal, rootId, "nonStandardIdentifier.object", buffLen, buff);
        }
    }
    else {
        int h221Id = pvtBuildByPath(hVal, rootId, "nonStandardIdentifier.h221NonStandard", 0, NULL);
        int iCountryCode = identifier->t35CountryCode;
        int iExtension = identifier->t35Extension;
        int iManufacturerCode = identifier->manufacturerCode;
        pvtBuildByPath(hVal, rootId, "data", dataLength, data);
        pvtBuildByPath(hVal, h221Id, "t35CountryCode", iCountryCode, NULL);
        pvtBuildByPath(hVal, h221Id, "t35Extension", iExtension, NULL);
        ret=pvtBuildByPath(hVal, h221Id, "manufacturerCode", iManufacturerCode, NULL);
    }

    return ret;
}


/******************************************************************************
 * cmNonStandardParameterGet
 * ----------------------------------------------------------------------------
 * General: Converts nonStandardParameter PVT node it into structures in C.
 *
 * See also: cmNonStandardParam, cmNonStandardIdentifier
 * Return Value: The root PVT node id - if successful.
 *               Negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:    hVal       - The PVT handle.
 *           rootID     - The root node id (nonStandardData) which includes the nonStandardParameter.
 * Output:   identifier - The cmNonStandardIdentifier structure which is included in nonStandardParameter.
 *           data       - The data string which is included in nonStandardParameter.
 *           dataLength - The length in bytes of the string provided in the data parameter. The application should set this
 *                        parameter to the allocated length of the data parameter but when the function returns, the actual
 *                        size will be set to this parameter.
 *****************************************************************************/
RVAPI int RVCALLCONV cmNonStandardParameterGet (
                 IN    HPVT                     hVal,
                 IN    int                      rootId,
                 OUT   cmNonStandardIdentifier  *identifier,
                 OUT   RvChar                   *data,
                 INOUT RvInt32                  *dataLength
                 )
{
    RvPvtNodeId id;
    RvInt32 len;

    if (!hVal ||
        !data || !dataLength || *dataLength<0 ||
        !identifier) return RV_ERROR_UNKNOWN;
    if (rootId<0)
    {
        *dataLength=RV_ERROR_UNKNOWN;
        return RV_ERROR_UNKNOWN;
    }

    /* The GetByFieldId functions cannot be used in this function since both Q931 and H.245
       ASN.1 definitions use this function! */
    id = pvtGetByPath(hVal, rootId, "data", NULL, &len, NULL);
    pvtGetString(hVal, id, *dataLength, data);
    *dataLength = len;

    /* -- object id */
    identifier->object[0]=0;
    identifier->objectLength=0;
    id = pvtGetNodeIdByPath(hVal, rootId, "nonStandardIdentifier.object");
    if (RV_PVT_NODEID_IS_VALID(id))
    {
        char buff[128];
        int buffLen;

        strcpy(buff, "");

        buffLen=pvtGetString(hVal, id, sizeof(buff), buff);
        oidDecodeOID(buffLen, buff, sizeof(identifier->object), identifier->object, numberForm);
        identifier->objectLength=strlen(identifier->object);
    }

    /* -- h221 id */
    identifier->t35CountryCode=0;
    identifier->t35Extension=0;
    identifier->manufacturerCode=0;
    id = pvtGetNodeIdByPath(hVal, rootId, "nonStandardIdentifier.h221NonStandard");
    if (RV_PVT_NODEID_IS_VALID(id))
    {
        RvInt32 value;

        pvtGetByPath(hVal, id, "t35CountryCode", NULL, &value, NULL);
        identifier->t35CountryCode=(RvUint8)value;
        pvtGetByPath(hVal, id, "t35Extension", NULL, &value, NULL);
        identifier->t35Extension=(RvUint8)value;
        pvtGetByPath(hVal, id, "manufacturerCode", NULL, &value, NULL);
        identifier->manufacturerCode=(RvUint16)value;
    }

    return RV_TRUE;
}



#ifdef __cplusplus
}
#endif





















