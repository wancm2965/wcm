/***********************************************************************************************
  Notice:
  This document contains information that is proprietary to RADVISION LTD..
  No part of this publication may be reproduced in any form whatsoever without
  written prior approval by RADVISION LTD..

    RADVISION LTD. reserves the right to revise this publication and make changes
    without obligation to notify any person of such revisions or changes.

*************************************************************************************************/

/********************************************************************************************
*                                  lepFwNat.c
*
* This file contains all the functions which enable the use of the H.460.FANTAS/MA/17 module
*
*
*      Written by                        Version & Date                        Change
*     ------------                       ---------------                      --------
*       Ran Arad                           26-Jul-2005
*
********************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

#include "lepUtils.h"

#ifdef USE_FWNAT

#include "fwnat_api.h"
#include <assert.h>
#include "h245.h"
#include "q931asn1.h"
#include "lepRtp.h"
#include "lepFwNat.h"
#include "mti.h"
#include "lepUtils.h"


RvStatus RVCALLCONV epFwNatSetByCallEv(
    IN  HAPP                hApp,
    IN  HH323FWNATMGR       hFwNat,
    IN  HCALL               hsCall,
    IN  HAPPCALL            haCall,
    IN  HH323FWNATCONN      hFwNatConn,
    IN  RvInt32             index);
RvStatus RVCALLCONV epFwNatGetByCallEv(
    IN  HAPP                hApp,
    IN  HH323FWNATMGR       hFwNat,
    IN  HCALL               hsCall,
    IN  HAPPCALL            haCall,
    IN  RvInt32             index,
    OUT HH323FWNATCONN *    hFwNatConn);
RvStatus RVCALLCONV epFwNatSetProtConnHandleEv(
    IN  HPROTCONN           hProtConn,
    IN  HH323FWNATCONN      hFwNatConn);
RvStatus RVCALLCONV epFwNatGetProtConnHandleEv(
    IN  HPROTCONN           hProtConn,
    OUT HH323FWNATCONN *    hFwNatConn);
RvStatus RVCALLCONV epFwNatRasEncapStatusEv(
    IN  HAPP                hApp,
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hProtConn,
    IN  HH323FWNATCONN      hFwNatConn,
    IN  RvH323FwNatH46017State status,
    IN  HRAS                hsRas);

/*handler for the media keepAlives timer*/
static void RVCALLCONV keepAlive_handler(void * context);

static void getSessioIdFromOLCMessage(
    IN EpObj *      ep,
    IN RvPvtNodeId  message,
    IN RvBool       bIsFaststart,
    IN RvPstFieldId fieldId,
    OUT RvInt32 *   sessionId);

/******************************************************************************
 * epFwNatInit
 * ----------------------------------------------------------------------------
 * General: Initialize the H.460.FANTAS/MA/17 module.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep                   - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epFwNatInit(
    IN  EpObj           *ep)
{
    /* Initialize H.460.FANTAS/MA/17 (does nothing if not enabled) */
    RvH323FwNatCallbacks callbacks;
    RvStatus status;

    memset(&callbacks, 0, sizeof(callbacks));

    /* set callbacks and configurations */
    callbacks.fwNatGetByCallEv = epFwNatGetByCallEv;
    callbacks.fwNatSetByCallEv = epFwNatSetByCallEv;
    callbacks.fwNatGetProtConnHandleEv = epFwNatGetProtConnHandleEv;
    callbacks.fwNatSetProtConnHandleEv = epFwNatSetProtConnHandleEv;
    callbacks.fwNatRasEncapStatusEv = epFwNatRasEncapStatusEv;
    
    status = RvH323FwNatInitialize(ep->hApp, &(ep->cfg.fwNatConfig), &callbacks, ep->maxCalls+1, 2048, &ep->hFwNat);

    epUtilsEvent(ep, "TCL", NULL, "set app(h460,serverInd) -new-");

    return status;
}


/******************************************************************************
 * epFwNatConfigure
 * ----------------------------------------------------------------------------
 * General: Configures the H.460.FANTAS/MA/17 module.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep                   - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epFwNatConfigure(
    IN  EpObj           *ep)
{
    if (ep->hFwNat == NULL)
        return RV_ERROR_UNINITIALIZED;
    return RvH323FwNatReconfigure(ep->hFwNat, &(ep->cfg.fwNatConfig));
}


/******************************************************************************
 * epFwNatEnd
 * ----------------------------------------------------------------------------
 * General: End the H.460.FANTAS/MA/17 module.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epFwNatEnd(
    IN  EpObj * ep)
{
    if (ep->hFwNat != NULL)
    {
        RvH323FwNatEnd(ep->hFwNat);
        ep->hFwNat = NULL;
    }
    return RV_OK;
}


/******************************************************************************
 * epFwNatVersion
 * ----------------------------------------------------------------------------
 * General: Get the version string of the H.460.FANTAS/MA/17 module used.
 *
 * Return Value: Version string.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object.
 * Output: None.
 *****************************************************************************/
const RvChar *epFwNatVersion(
    IN EpObj    *ep)
{
    if (ep->hFwNat != NULL)
        return RvH323FwNatGetVersion();
    return "";
}


/******************************************************************************
 * epFwNatCallEnd
 * ----------------------------------------------------------------------------
 * General: Clean up FW/NAT related information.
 *
 * Return Value: none.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         call             - Call object to use.
 * Output: None.
 *****************************************************************************/
void epFwNatCallEnd(
    IN  EpObj * ep,
    IN  EpCallObj * call)
{
    if (call->hFwNatConn != NULL)
        RvH323FwNatConnCallTerminated(ep->hFwNat, call->hFwNatConn, call->hsCall);
}


/******************************************************************************
 * epFwNatSetCallH245Stage
 * ----------------------------------------------------------------------------
 * General: Set the H.245 stage so that an endpoint inside the FW/NAT will not
 *          offer an H.245 address.
 *
 * Return Value: none.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call             - Call object to use.
 *         cfg              - Call configuration.
 * Output: None.
 *****************************************************************************/
void epFwNatSetCallH245Stage(
    IN  EpCallObj * call,
    IN  EpCallConfig *cfg)
{
    EpObj * ep = call->ep;

    if (ep->hFwNat == NULL)
        return;
    if ((ep->cfg.fwNatConfig.e18Config != RvH323FwNatH46018ConfigSupported) &&
        (ep->cfg.fwNatConfig.e17Config != RvH323FwNatH46017ConfigEncapsulate))
        return;
    if ((cfg->h245Stage == cmTransH245Stage_facility) || (cfg->h245Stage == cmTransH245Stage_noH245))
        /* nothing to do */
        return;

    /* different cases for an incoming and an outgoing call */
    if (call->isIncoming)
    {
        cmTransportAddress addr;
        RvInt32 sz = sizeof(addr);
        if (cmCallGetParam(call->hsCall, cmParamSetupH245Address, 0, &sz, (char*)&addr) != RV_OK)
        {
            /* no address in setup for an incoming call - we must stop the endpoint from offering
            its own address */
            cmCallSetParam(call->hsCall, cmParamH245Stage, 0, cmTransH245Stage_facility, NULL);
        }
    }
    else
    {
        /* we must stop the endpoint from sending it's address in the setup message */
        if (cfg->h245Stage == cmTransH245Stage_setup)
        {
            cmCallSetParam(call->hsCall, cmParamH245Stage, 0, cmTransH245Stage_callProceeding, NULL);
        }
        else if (cfg->h245Stage == cmTransH245Stage_early)
        {
            cmCallSetParam(call->hsCall, cmParamH245Stage, 0, cmTransH245Stage_connect, NULL);
        }
    }
}

/******************************************************************************
 * epFwNatCallSendMessage
 * ----------------------------------------------------------------------------
 * General: May add FwNat information to the message..
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         call             - Call object to use.
 *         message          - node id of sent message.
 * Output: bInfo            - RV_TRUE if FW/NAT related information was retrieved
 *                            from the message.
 *****************************************************************************/
RvStatus epFwNatCallSendMessage(
        IN  EpObj *        ep,
        IN  EpCallObj      *call,
        IN  RvPvtNodeId    message,
        OUT RvBool *       bInfo)
{
	if ((call == NULL) || (ep == NULL))
		return RV_ERROR_UNKNOWN;

	return RvH323FwNatCallSendMessage(ep->hFwNat, call->hsCall,  call->hFwNatConn, message, bInfo);
}


/******************************************************************************
 * epFwNatCallRecvMessage
 * ----------------------------------------------------------------------------
 * General: Retrieves FW/NAT related information from the message.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         call             - Call object to use.
 *         message          - node id of received message.
 * Output: bInfo            - RV_TRUE if FW/NAT related information was retrieved
 *                            from the message.
 *****************************************************************************/
RvStatus epFwNatCallRecvMessage(
        IN  EpObj *      ep,
        IN  EpCallObj *  call,
        IN  RvPvtNodeId  message, 
        OUT RvBool *     bInfo)
{
    RvStatus        status = RV_OK;
    RvBool          bUseH46019, bRemoteMediaMultiplexEnabled, ignoreH46019Params = RV_FALSE;
    RvPvtNodeId     msgNode = RV_PVT_INVALID_NODEID;
    RvPstFieldId    fieldId = -1;

	if ((call == NULL) || (ep == NULL))
		return RV_ERROR_UNKNOWN;

    if (ep->bIsRegistered)
    {
        /*find out the type of the message and ignore the bUseH46019 and bRemoteMediaMultiplexEnabled
        if it is callProceeding*/
        __pvtGetByFieldIds(msgNode, ep->hVal, message, { _q931(message) _anyField LAST_TOKEN }, &fieldId, NULL, NULL);
        if (fieldId == __q931(callProceeding))
            ignoreH46019Params = RV_TRUE;
    }
    status = RvH323FwNatCallRecvMessage(ep->hFwNat, 
                                        call->hsCall,
                                        call->hFwNatConn, 
                                        message, 
                                        bInfo,
                                        &bUseH46019,
                                        &bRemoteMediaMultiplexEnabled);

    if (*bInfo == RV_FALSE)
        return status;

    if (ignoreH46019Params == RV_FALSE)
    {
        call->bUseH46019 = bUseH46019;
        call->bRemoteMediaMultiplexEnabled = bRemoteMediaMultiplexEnabled;
    }

    if (ep->cfg.fwNatConfig.e19Config)
    {
        if (status == RV_OK && call->bUseH46019)
        {
            epUtilsEvent(ep, "H460", call, "Using H460-19 Mode");
            epUtilsLog(ep, call, "Using H460-19 Mode: Multiplexing Mode of Remote ep: %d", 
                       call->bRemoteMediaMultiplexEnabled);
        }
        else
        {
            epUtilsEvent(ep, "H460", call, "Not using H460-19 Mode");
            epUtilsLog(ep, call, "Not Using H460-19 Mode: Multiplexing Mode of Remote ep: %d", 
                        call->bRemoteMediaMultiplexEnabled);
        }
    }

    return status;
}

/********************************************************************************
 * Function Name: epFwNatEvRASMessageSend
 * Description: This function should be called from cmEvRASMessageSend(). it
 *              may add FwNat information to the message.
 *
 * Input:   ep           - Endpoint object to use.
 *          hsRas        - RAS transaction this message belongs to.
 *          rasTrStage   - Stage this message is sent for.
 *          messageNode  - PVT nodeId of the message being sent out.
 * Output:  bStop        - RV_TRUE if the message should not be sent (to be
 *                         returned on exit from cmEvRASMessageSend).
 *          bInfo        - RV_TRUE if the message was added with any information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus epFwNatEvRASMessageSend(
    IN  EpObj *             ep,
    IN  HRAS                hsRas,
    IN  cmRASTrStage        rasTrStage,
    IN  RvPvtNodeId         messageNodeId,
    OUT RvBool *            bStop,
    OUT RvBool *            bInfo)
{
	return RvH323FwNatEvRASMessageSend(ep->hFwNat, hsRas, rasTrStage, messageNodeId, bStop, bInfo);
}

/******************************************************************************
 * epFwNatAddH46019Info2Message
 * ----------------------------------------------------------------------------
 * General: Adds H.460.19 related information to OLC/OLCA message.
 *
 * Return Value: RV_OK on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call              - Call object to use.
 *         channel           - Channel object to use - can be NULL in FS calls.
 *         bKeepAliveNeeded  - Enforce addition of keepAlive parameters to the traversalParameters.
 *                             (if RV_FALSE keepAlive data will be added only in server's OLC)
 *         fsChannel         - FS channel object.
 *         message           - OLC/OLCA message node id.
 *         bIsFastStart      - RV_TRUE if FS procedure used.
 *         channelDirection  - dirTransmit or dirReceive in case of FS.
 *****************************************************************************/
RvStatus epFwNatAddH46019Info2Message(
        IN EpCallObj *           call,
        IN EpChannelObj *        channel,
        IN RvBool                bKeepAliveNeeded,
        IN cmFastStartChannel *  fsChannel,
        IN RvPvtNodeId           message,
        IN RvBool                bIsFastStart,
        IN cmChannelDirection    channelDirection)
{

    RvH323FwNatH46019traversalParams   traversalParams;
    RvH323FwNatH46019traversalParams * pTraversalParams = &traversalParams;
    cmTransportAddress      rtpMuxAddr, rctpMuxAddr, keepAliveAddr;
    EpObj *                 ep = call->ep;
    HPVT                    hVal = cmGetValTree(ep->hApp);
    RvStatus                status = RV_OK;
    RvBool                  bInfo = RV_FALSE;
    RvPstFieldId            messageTypeFieldId = RV_PVT_INVALID_NODEID;
    RvPvtNodeId             msgNode;
    RvPvtNodeId             dataTypeId = RV_PVT_INVALID_NODEID;
    RvChar *                name = NULL;
    RvInt32                 fsSessionIndex = -1;

	if (ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigNotUsed)
        return RV_OK;

    if (bIsFastStart && fsChannel != NULL)
    {
       dataTypeId = fsChannel->dataTypeHandle;
       name = fsChannel->channelName;

       if (dataTypeId != RV_PVT_INVALID_NODEID )
       {
           RvPstFieldId            fieldId=-1;
           RvPvtNodeId             dataId;
           
           dataId = pvtChild(hVal, dataTypeId);
           if (dataId < 0)
               return RV_ERROR_UNKNOWN;
           
           pvtGet(hVal, dataId, &fieldId, NULL, NULL, NULL);
           switch(fieldId)
           {
           case __h245(audioData):
               fsSessionIndex = 0;
               break;
           case __h245(videoData):
               fsSessionIndex = 1;
               break;
           case __h245(data):
               fsSessionIndex = 2;
               break;
           default:
               break;
           }
       }
       else if (name != NULL)
       {
           if (tolower(name[0]) == 'g')
               fsSessionIndex = 0;
           else if (tolower(name[0]) == 'h')
               fsSessionIndex = 1;
           else if (tolower(name[0]) == 't')
               fsSessionIndex = 2;
       }
       else
           return RV_ERROR_UNKNOWN;
    
       if (fsSessionIndex == -1)
           return RV_ERROR_UNKNOWN;
    }

    memset(&traversalParams, 0 , sizeof(traversalParams));
    memset(&rtpMuxAddr, 0 , sizeof(traversalParams));
    memset(&rctpMuxAddr, 0 , sizeof(traversalParams));
    memset(&keepAliveAddr, 0 , sizeof(traversalParams));

    if (bIsFastStart == RV_TRUE)
    {
        msgNode = message;
        if (channelDirection == dirTransmit)
            messageTypeFieldId = __h245(openLogicalChannel);
        else if (channelDirection == dirReceive)
                messageTypeFieldId = __h245(openLogicalChannelAck);
    }
    else
    {
        __pvtGetByFieldIds(msgNode, hVal, message, 
        {_anyField _anyField LAST_TOKEN}, &messageTypeFieldId, NULL, NULL);
    }

    /* get multiplexID*/
    if (channel != NULL)
        traversalParams.multiplexID = channel->multiplexID;
    else if (bIsFastStart == RV_TRUE)
    {
        /*If we are the called party, let's open a session if we still don't have one*/
        if ((call->isIncoming) && (call->rtpSessions[fsSessionIndex] < 0) && (call->muxedRtpSessions[fsSessionIndex] < 0))
        {
            if(((ep->cfg.fwNatConfig.e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigReceive) ||
                (ep->cfg.fwNatConfig.e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigFull)) &&
               (call->bRemoteMediaMultiplexEnabled == RV_TRUE))
            {
                /*We are demultiplexing media*/
                epRtpSessionNew(call, RV_TRUE, &call->multiplexID[fsSessionIndex]);
            }
            else
            {
                RvUint32 dummyMuxID;
                epRtpSessionNew(call, RV_FALSE, &dummyMuxID);
            }
        }
        traversalParams.multiplexID = call->multiplexID[fsSessionIndex];
    }
    

    /*if we don't de-multiplex - no need for multiplexID or rtpMuxAddr, or rctpMuxAddr*/
    if ((traversalParams.multiplexID == (RvUint32)-1) && (((ep->cfg.fwNatConfig.e19MultiplexMediaConfig != RvH323FwNatH46019MediaMultiplexModeConfigReceive ) &&
        (ep->cfg.fwNatConfig.e19MultiplexMediaConfig != RvH323FwNatH46019MediaMultiplexModeConfigFull)) ||
        (((!bIsFastStart) || (bIsFastStart && call->isIncoming)) && (!call->bRemoteMediaMultiplexEnabled))))
    {
        if (ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigServer)
        {
            /*we are a server and we do not demultiplex if it's an OLC or AddkeepAlive is RV_TRUE - 
              get rtpAddress from message for keepAliveChannel.
             This can be done because in our testApplication, rtpAddress, rtcpAddress and keepAliveAddress
            are always the same */
            
            if (messageTypeFieldId == __h245(openLogicalChannel) || bKeepAliveNeeded == RV_TRUE)
            {
                RvPvtNodeId addrNodeId;
                
                __pvtGetByFieldIds(addrNodeId, hVal, msgNode, 
                        {_h245(forwardLogicalChannelParameters) 
                         _h245(multiplexParameters) 
                         _h245(h2250LogicalChannelParameters) 
                         _h245(mediaControlChannel)
                         LAST_TOKEN}, NULL, NULL, NULL);
                if (addrNodeId == RV_PVT_INVALID_NODEID)
                {
                    __pvtGetByFieldIds(addrNodeId, hVal, msgNode, 
                    {_h245(forwardLogicalChannelParameters) 
                    _h245(multiplexParameters) 
                    _h245(h2250LogicalChannelParameters) 
                    _h245(mediaChannel)
                    LAST_TOKEN}, NULL, NULL, NULL);
                }
                if (RV_PVT_NODEID_IS_VALID(addrNodeId))
                {
                    cmVt2TransportAddress(hVal, addrNodeId, &keepAliveAddr);
                    traversalParams.keepAliveAddr = &keepAliveAddr;
                }
                else
                {
                    __pvtGetByFieldIds(addrNodeId, hVal, msgNode, 
                    {_h245(reverseLogicalChannelParameters) 
                    _h245(multiplexParameters) 
                    _h245(h2250LogicalChannelParameters) 
                    _h245(mediaControlChannel)
                    LAST_TOKEN}, NULL, NULL, NULL);
                    if (addrNodeId == RV_PVT_INVALID_NODEID)
                    {
                        __pvtGetByFieldIds(addrNodeId, hVal, msgNode, 
                        {_h245(reverseLogicalChannelParameters) 
                        _h245(multiplexParameters) 
                        _h245(h2250LogicalChannelParameters) 
                        _h245(mediaChannel)
                        LAST_TOKEN}, NULL, NULL, NULL);
                    }
                    if (RV_PVT_NODEID_IS_VALID(addrNodeId))
                    {
                        cmVt2TransportAddress(hVal, addrNodeId, &keepAliveAddr);
                        traversalParams.keepAliveAddr = &keepAliveAddr;
                    }
                    else
                    {
                        /*lets see if we can find an address in the separateStack field*/

                        traversalParams.keepAliveAddr = &keepAliveAddr;
                        
                         __pvtGetByFieldIds(addrNodeId, hVal, msgNode,
                             {_h245(separateStack) 
                              _h245(networkAddress) 
                              _h245(localAreaAddress) 
                              LAST_TOKEN}, NULL, NULL, NULL);

                        if (RV_PVT_NODEID_IS_VALID(addrNodeId))
                        {
                            cmVt2TransportAddress(hVal, addrNodeId, &keepAliveAddr);
                            traversalParams.keepAliveAddr = &keepAliveAddr;
                        }

                    }
                }

                if (traversalParams.keepAliveAddr != NULL)
                {
                    RvInt32 rtpSession = -1;

                    if (channel != NULL)
                        rtpSession = channel->rtpSession;
                    else if (bIsFastStart)
                        rtpSession = call->rtpSessions[fsSessionIndex];
                    if (rtpSession >= 0)
                        keepAliveAddr.port = epRtpSessionGetLocalPort(ep->rtp,rtpSession);
                }
            }
        }
    }
    else
    {
        /* get rtpMuxAddr & rctpMuxAddr from rtp stack */
        traversalParams.rtpMuxAddr = &rtpMuxAddr;
        traversalParams.rctpMuxAddr = &rctpMuxAddr;
        epRtpDemuxGetLocalRTP(ep->rtp, traversalParams.rtpMuxAddr);
        epRtpDemuxGetLocalRTCP(ep->rtp, traversalParams.rctpMuxAddr);
    }

    /*if we are a server and it's an OLC or bAddKeepAlive*/
    if ((ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigServer) &&
        ((messageTypeFieldId == __h245(openLogicalChannel)) ||( bKeepAliveNeeded == RV_TRUE)) )
    {
        traversalParams.keepAliveInterval = 30;
        /*PayloadType of keepAlive channel must be in the dynamic payLoadType range >96 <=127*/
        traversalParams.keepAlivePayLoadType = 97;
        /*copy keepAliveAddress from rtpMuxAddr if we have to*/
        if (traversalParams.rtpMuxAddr != NULL)
            memcpy(&keepAliveAddr, traversalParams.rtpMuxAddr, sizeof(cmTransportAddress));
        traversalParams.keepAliveAddr = &keepAliveAddr;
    }

    if (traversalParams.rctpMuxAddr == NULL && 
        traversalParams.rtpMuxAddr == NULL && 
        traversalParams.keepAliveAddr == NULL &&
        traversalParams.multiplexID == (RvUint32)-1)
    {
        /*nothing to add but call the RvH323FwNatAddChannelH46019Info anyway
        in FS calls to clean up the messages*/
        if (bIsFastStart)
            pTraversalParams = NULL;
        else
            return RV_OK; 
    }
    /*add info to message*/
    status = RvH323FwNatAddChannelH46019Info(ep->hFwNat,
                                             NULL,
                                             message,
                                             pTraversalParams,
                                             call->bUseH46019,
                                             call->bRemoteMediaMultiplexEnabled,
                                             bIsFastStart,
                                             channelDirection,
                                             &bInfo);

    if (bInfo == RV_TRUE && status == RV_OK)
    {
        epUtilsEvent(ep, "H460", call, "H460.19 Info Added to Message");
    }

    return status;
}

/******************************************************************************
 * epFwNatGetH46019InfoFromMessage
 * ----------------------------------------------------------------------------
 * General: Gets the remote H.460.19 related information from OLC/OLCA messages
 *          and sets them in rtp.
 *
 * Return Value: RV_OK on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call                 - Call object to use.
 *         channel              - Channel object to use - can be null in FS calls.
 *         bKeepAliveExpected   - RV_TRUE if keepAliveAddress is expected even when the received message
 *                                is not an OLC. 
 *         message              - OLC/OLCA message node id.
 *         fsChannel            - FS channel object.
 *         sessionId            - Session id of channel in the stack - for fastConnect calls.
 *         bIsFastStart         - RV_TRUE if FS procedure used.
 *         chanDir              - Transmit, receive or both - for fastConnect calls.
 *****************************************************************************/
RvStatus epFwNatGetH46019InfoFromMessage(
        IN EpCallObj *          call,
        IN EpChannelObj *       channel,
        IN RvBool               bKeepAliveExpected,  
        IN RvPvtNodeId          message,
        IN cmFastStartChannel * fsChannel,
        IN int                  sessionId,
        IN RvBool               bIsFastStart,
        IN cmChannelDirection   chanDir)
{
    EpObj *                 ep = call->ep;
    HPVT                    hVal = cmGetValTree(ep->hApp);
    RvH323FwNatH46019traversalParams   traversalParams;
    cmTransportAddress      rtpMuxAddr, rctpMuxAddr, keepAliveAddr;
    RvStatus                status;
    EpChannelObj *          chan = NULL;
    int                     index = -1;
    RvBool                  origin = (chanDir == dirTransmit);
    RvBool                  bInfo = RV_FALSE;
    RvPvtNodeId             dataTypeId = RV_PVT_INVALID_NODEID;
    RvChar *                name = NULL;
    RvPstFieldId            messageTypeFieldId = RV_PVT_INVALID_NODEID;
    RvInt32 sid = -1;
    RvInt32                 rtpSession = -1;
    int                     rtpSessionIndex = -1;
    EpChannelObj *          sessionChannel = NULL;
    RvBool                  rtpSessionIsMuxed  = RV_FALSE;
    RvBool                  remoteIsMultiplexed;

    if (ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigNotUsed)
        return RV_OK; 

    /*find the type of message we're dealing with*/
    if ((bIsFastStart == RV_TRUE)  && (fsChannel != NULL))
    {
       dataTypeId = fsChannel->dataTypeHandle;
       name = fsChannel->channelName;

        if (chanDir == dirTransmit)
            messageTypeFieldId = __h245(openLogicalChannelAck);
        else if (chanDir == dirReceive)
            messageTypeFieldId = __h245(openLogicalChannel);
    }
    else
    {
        RvPvtNodeId msgNode;
        __pvtGetByFieldIds(msgNode, hVal, message,
            {_anyField _anyField LAST_TOKEN}, &messageTypeFieldId, NULL, NULL);
    }

    memset(&traversalParams, 0, sizeof(traversalParams));
    memset(&rtpMuxAddr, 0, sizeof(rtpMuxAddr));
    memset(&rctpMuxAddr, 0, sizeof(rctpMuxAddr));
    memset(&keepAliveAddr, 0, sizeof(keepAliveAddr));


    traversalParams.rtpMuxAddr = &rtpMuxAddr;
    traversalParams.rctpMuxAddr = &rctpMuxAddr;
    if (ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigClient)
    {
        if (messageTypeFieldId == __h245(openLogicalChannel) || bKeepAliveExpected == RV_TRUE)
            traversalParams.keepAliveAddr = &keepAliveAddr;
    }
    traversalParams.multiplexID = (RvUint32)-1;
    
    status = RvH323FwNatGetChannelH46019Info(ep->hFwNat,
                                             NULL,
                                             message,
                                             bIsFastStart,
                                             chanDir,
                                             &traversalParams,
                                             &bInfo);

    if (status == RV_OK)
    {
        remoteIsMultiplexed = (traversalParams.multiplexID != (RvUint32)-1 &&
                                      (ep->cfg.fwNatConfig.e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigTransmit ||
                                       ep->cfg.fwNatConfig.e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigFull));

         /*find the rtpSession*/
        if (channel != NULL)
        {
            rtpSession = channel->rtpSession;
            sessionChannel = channel;
            rtpSessionIsMuxed = !(channel->multiplexID == (RvUint32)-1);
            chan       = channel;
            if (remoteIsMultiplexed && (bInfo == RV_TRUE))
                channel->remoteMultiplexID = traversalParams.multiplexID;
        }
        else
        {
            if ((bIsFastStart == RV_TRUE) && (chan == NULL))
            {
                RvPstFieldId            fieldId=-1;
                RvPvtNodeId             dataId;
                HCHAN                   hsChan = NULL;

                /*see if we already have a channel in the stack for this fsChannel*/
                if ((sessionId != (RvUint8)-1) && (chanDir != (cmChannelDirection)-1))
                {
                    lepUnlockCallForAPI(call, cmCallGetChannelBySessionId(call->hsCall, sessionId, origin, &hsChan));
                    if (hsChan != NULL)
                        cmChannelGetHandle(hsChan, (HAPPCHAN *)&chan);
                }

                if ((chan == NULL) || (chan->rtpSession < 0))
                {        
                    if (dataTypeId != RV_PVT_INVALID_NODEID)
                    {
                        dataId = pvtChild(hVal, dataTypeId);
                        if (dataId < 0)
                            return RV_ERROR_UNKNOWN;
                        
                        pvtGet(hVal, dataId, &fieldId, NULL, NULL, NULL);
                        switch(fieldId)
                        {
                        case __h245(audioData):
                            index = 0;
                            break;
                        case __h245(videoData):
                            index = 1;
                            break;
                        case __h245(data):
                            index = 2;
                            break;
                        default:
                            break;
                        }
                    }
                    else if (name != NULL)
                    {
                        if (tolower(name[0]) == 'g')
                            index = 0;
                        if (tolower(name[0]) == 'h')
                            index = 1;
                        if (tolower(name[0]) == 't')
                            index = 2;
                    }
                    
                    if (index != -1)
                    {
                        /*set indication that traversal parameters received for the sessio*/
                        call->bMultiplexParamsRcvd[index] = RV_TRUE;
                        
                        /*set remote multiplexID*/
                        if (remoteIsMultiplexed && (bInfo == RV_TRUE))
                            call->remoteMultiplexID[index] = traversalParams.multiplexID;

                        /*decide what session to use*/
                        if(((ep->cfg.fwNatConfig.e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigReceive) ||
                            (ep->cfg.fwNatConfig.e19MultiplexMediaConfig == RvH323FwNatH46019MediaMultiplexModeConfigFull)) &&
                            (call->bRemoteMediaMultiplexEnabled == RV_TRUE))
                            
                        {
                            /*incoming stream is multiplexed close non multiplexed session*/
                            if (call->rtpSessions[index] >= 0)
                            {
                                epRtpSessionClose(ep->rtp, call->rtpSessions[index], RV_FALSE);
                            }
                            call->rtpSessions[index] = -1;
                            if (call->muxedRtpSessions[index] < 0)
                                /*first open rtpSession*/
                                call->muxedRtpSessions[index] = epRtpSessionNew(call, RV_TRUE, &call->multiplexID[index]);
                            rtpSession = call->muxedRtpSessions[index];
                            rtpSessionIndex = index;
                            rtpSessionIsMuxed = RV_TRUE;
                        }
                        else 
                        {
                            RvUint32 demyMuxID = (RvUint32)-1;
                            RV_UNUSED_ARG(demyMuxID);
                            /*incoming stream not multiplexed*/
                            if (call->muxedRtpSessions[index] >= 0)
                            {
                                epRtpSessionClose(ep->rtp, call->muxedRtpSessions[index], RV_TRUE);
                            }
                            call->muxedRtpSessions[index] = -1;
                            call->multiplexID[index] = (RvUint32)-1;
                            if (call->rtpSessions[index] < 0)
                                /*first open rtpSession*/
                                call->rtpSessions[index] = epRtpSessionNew(call, RV_FALSE, &demyMuxID);
                            rtpSession = call->rtpSessions[index];
                            rtpSessionIndex = index;
                            rtpSessionIsMuxed = RV_FALSE;
                        }
                    }
                    
                }
                else
                {
                    rtpSession = chan->rtpSession;
                    sessionChannel = chan;
                    rtpSessionIsMuxed = !(chan->multiplexID == (RvUint32)-1);
                    if (remoteIsMultiplexed && (bInfo == RV_TRUE))
                        chan->remoteMultiplexID = traversalParams.multiplexID;
                }
            }
        }

           
        if (rtpSession == -1)
            return RV_ERROR_UNKNOWN;

        /*see if we have same session already*/
        getSessioIdFromOLCMessage(ep, message, bIsFastStart, messageTypeFieldId, &sid);
        if (sid >= 0)
        {
            HCHAN           sameSessionhsChan = NULL;
            EpChannelObj *  sameSessionChan = NULL;

            cmCallGetChannelBySessionId(call->hsCall, sid, RV_TRUE, &sameSessionhsChan);
            if (sameSessionhsChan == NULL)
                sid = -1;
            else
            {
                 cmChannelGetHandle(sameSessionhsChan, (HAPPCHAN*)&sameSessionChan);
                 if (sameSessionChan == NULL)
                     sid = -1;
                 else
                 {
                     sid = sameSessionChan->rtpSession;
                 }
            }
        }

        if (sid >= 0)
        {
            /*close the current session*/
            epRtpSessionClose(ep->rtp, rtpSession, rtpSessionIsMuxed);

            rtpSession = sid;

            /*set the same session's parameters*/
            if (sessionChannel != NULL)
            {
                EpChannelObj * partnerChannel = call->firstChannel;
                sessionChannel->rtpSession = sid;
                /*find the partner channel in the application*/
                while (partnerChannel != NULL)
                {
                    if (partnerChannel == sessionChannel)
                        continue;
                    if (partnerChannel->rtpSession == sid)
                        break;
                    partnerChannel = partnerChannel->nextChannel;
                }
                if (partnerChannel != NULL)
                {
                    sessionChannel->multiplexID = partnerChannel->multiplexID;
                    sessionChannel->remoteMultiplexID = partnerChannel->remoteMultiplexID;
                    epRtpSessionSetAction(ep->rtp, sessionChannel->rtpSession, call->action);
                    epRtpSessionJoin(ep->rtp, sessionChannel->rtpSession);
                }
            }

            if (rtpSessionIndex >= 0)
            {
                if (rtpSessionIsMuxed)
                {
                    call->muxedRtpSessions[rtpSessionIndex] = sid;
                    call->multiplexID[rtpSessionIndex] = (RvUint32)-1;
                    call->remoteMultiplexID[rtpSessionIndex] = (RvUint32)-1;
                }
                else
                {
                    call->rtpSessions[rtpSessionIndex] = sid;
                    call->remoteMultiplexID[rtpSessionIndex] = (RvUint32)-1;
                }
            }
        }

        if (bInfo == RV_TRUE)
        {
            
            /*If a client - set rtp/rctp remote addresses (a server will ignore the addresses
            and will get it from first keepAlive*/
            if (ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigClient)
            {
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
#if (RTP_TYPE == RTP_TYPE_NEW)
                RvBool bHasAddr;
#endif
#endif
                if ((traversalParams.rtpMuxAddr != NULL && epTaHasIp(ep, traversalParams.rtpMuxAddr)) || 
                    (traversalParams.keepAliveAddr != NULL && epTaHasIp(ep, traversalParams.keepAliveAddr)))
                {
                    cmTransportAddress * addr = traversalParams.rtpMuxAddr;
                    if (!epTaHasIp(ep, traversalParams.rtpMuxAddr))
                        addr = traversalParams.keepAliveAddr;

#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
#if (RTP_TYPE == RTP_TYPE_NEW)
                    bHasAddr = epRtpSessionHasRemoteRTPAddress(ep->rtp, rtpSession);
                    if (!bHasAddr)
                    {
                        epRtpSessionSetRemoteRTP(call, rtpSession, addr, remoteIsMultiplexed, traversalParams.multiplexID);
                        epUtilsLog(ep, call, "remote address %d:%d set on session %d, remoteIsMultiplexed=%d, muxId=%d",
                            addr->addr.v4.ip, addr->port, rtpSession, remoteIsMultiplexed, traversalParams.multiplexID);
                    }
#endif
#endif
                    if (chan != NULL)
                    {
                        cmChannelGetOrigin(chan->hsChan, &origin);
                        if ((!bIsFastStart) && ((messageTypeFieldId == __h245(openLogicalChannel)) || (origin == RV_TRUE && cmChannelIsDuplex(chan->hsChan) == RV_TRUE)))
                        {
                            epFwNatSendKeepAlive(chan, origin, rtpSession);
                        }
                    }
                }
#if (RTP_TYPE == RTP_TYPE_NEW)
                if (epTaHasIp(ep, traversalParams.rctpMuxAddr))
                {
                    bHasAddr = epRtpSessionHasRemoteRTCPAddress(ep->rtp, rtpSession);
                    if (!bHasAddr)
                        epRtpSessionSetRemoteRTCP(call, rtpSession, traversalParams.rctpMuxAddr, remoteIsMultiplexed, traversalParams.multiplexID);
                }
#endif
            }
     
            epUtilsEvent(ep, "H460", call, "H460.19 Info Retrieved from Message");
        }
        
        if ((ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigServer) &&
            ((rtpSessionIndex > -1) || (sessionChannel != NULL)))
        {
            if (rtpSessionIndex < 0)
            {
                cmCapDataType capData = cmCapEmpty; 
                /*get channel type*/
                cmChannelGetParam(sessionChannel->hsChan, cmChannelParamCapDataType, (RvInt32*)&capData, NULL);
                rtpSessionIndex = capData - 1;
            }
            /*see if we have rtp address to set now*/
            if (epTaHasIp(ep, &call->rtpSessionsAddr[rtpSessionIndex]) && rtpSessionIndex >=0)
            {
                RvUint32 remoteMultiplexID = (RvUint32)-1;
                RvBool bHasAddr;

                if (sessionChannel != NULL)
                    remoteMultiplexID = sessionChannel->remoteMultiplexID;
                else
                {
                    remoteMultiplexID = call->remoteMultiplexID[rtpSessionIndex];
                }
                /*set remote rtp address*/
                bHasAddr = epRtpSessionHasRemoteRTPAddress(ep->rtp, rtpSession);
                if (!bHasAddr)
                {
                    epRtpSessionSetRemoteRTP(call, 
                        rtpSession, 
                        &call->rtpSessionsAddr[rtpSessionIndex], 
                        remoteIsMultiplexed, 
                        remoteMultiplexID);
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
                    epUtilsLog(ep, call, "remote address %d:%d set on session %d, remoteIsMultiplexed=%d, muxId=%d",
                        call->rtpSessionsAddr[rtpSessionIndex].addr.v4.ip, call->rtpSessionsAddr[rtpSessionIndex].port, rtpSession, remoteIsMultiplexed, remoteMultiplexID);
#endif
                }

                /*we might set the rtcp address as well asuming port number*/
                call->rtpSessionsAddr[rtpSessionIndex].port++;
                bHasAddr = epRtpSessionHasRemoteRTCPAddress(ep->rtp, rtpSession);
                if (!bHasAddr)
                    epRtpSessionSetRemoteRTCP(call, 
                    rtpSession, 
                    &call->rtpSessionsAddr[rtpSessionIndex], 
                    remoteIsMultiplexed, 
                    remoteMultiplexID);
                memset(&call->rtpSessionsAddr[rtpSessionIndex], 0, sizeof(cmTransportAddress));
            }
        }
    }
    return status;
}

/********************************************************************************
 * epFwNatCreateCallMuxedRtpSessions
 * ----------------------------------------------------------------------------
 * General: Init the rtpSessions in the call.
 *
 * Return: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:   call            - Call object to use. 
 * Output:  None.
 ********************************************************************************/
void epFwNatCreateCallMuxedRtpSessions(
    IN EpCallObj *  call)
{
    if (call->muxedRtpSessions[0] < 0)
        call->muxedRtpSessions[0] = epRtpSessionNew(call, RV_TRUE, &call->multiplexID[0]);
    if (call->muxedRtpSessions[1] < 0)
        call->muxedRtpSessions[1] = epRtpSessionNew(call, RV_TRUE, &call->multiplexID[1]);
    if (call->muxedRtpSessions[2] < 0)
        call->muxedRtpSessions[2] = epRtpSessionNew(call, RV_TRUE, &call->multiplexID[2]);

}

/********************************************************************************
 * epFwNatCreateChannelMuxedRtpSession
 * ----------------------------------------------------------------------------
 * General: Init the rtpSession in the channel.
 *
 * Return: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:   chan            - Channel object to use. 
 * None.
 ********************************************************************************/
void epFwNatCreateChannelMuxedRtpSession(
    IN EpChannelObj *  chan)
{
    EpCallObj * call = chan->call;
    RV_UNUSED_ARG(call);

    chan->rtpSession = epRtpSessionNew(call, RV_TRUE, &chan->multiplexID);
}

/********************************************************************************
 * epFwNatIsCallMultiplexed
 * ----------------------------------------------------------------------------
 * General: Checks of a call is using multiplex mode.
 *
 * Return: RV_TRUE if call is using multiplex mode.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:   call         - Call object.
 * Output:  none.
 ********************************************************************************/
RvBool epFwNatIsCallMultiplexed(
      IN EpCallObj * call)
{
    EpObj * ep = call->ep;
    
    if (ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigNotUsed ||
        (ep->cfg.fwNatConfig.e19MultiplexMediaConfig != RvH323FwNatH46019MediaMultiplexModeConfigReceive &&
        ep->cfg.fwNatConfig.e19MultiplexMediaConfig != RvH323FwNatH46019MediaMultiplexModeConfigFull))
        return RV_FALSE;

    if (call->isFastConnectCall)
        return RV_TRUE;

    if (call->bRemoteMediaMultiplexEnabled)
        return RV_TRUE;

    return RV_FALSE;
}

/********************************************************************************
 * epFwNatSetByCallEv
 * ----------------------------------------------------------------------------
 * General: Keep a connection handle for the FwNat module in the
 *          application's call database.
 *
 * Return: RV_OK on success, negative on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:   hApp         - stack application handle.
 *          hFwNat       - FwNat module handle
 *          hsCall       - Stack's call handle
 *          haCall       - Application's call handle
 *          hFwNatConn   - The FwNat connection related to the call or NULL
 *                         to unset.
 *          index        - for future use (currently will be set to zero)
 * Output:  none.
 ********************************************************************************/
RvStatus RVCALLCONV epFwNatSetByCallEv(
    IN  HAPP                hApp,
    IN  HH323FWNATMGR       hFwNat,
    IN  HCALL               hsCall,
    IN  HAPPCALL            haCall,
    IN  HH323FWNATCONN      hFwNatConn,
    IN  RvInt32             index)
{
    EpCallObj * call = (EpCallObj *)haCall;
    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(hFwNat);
    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(index);
    assert (call != NULL);
    call->hFwNatConn = hFwNatConn;
    return RV_OK;
}


/********************************************************************************
 * epFwNatSetByCallEv
 * ----------------------------------------------------------------------------
 * General: get FwNat connection handle from the application's call database.
 *                         if not found.
 *
 * Return: RV_OK on success, negative on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:   hApp         - stack application handle.
 *          hFwNat  - FwNat module handle
 *          hsCall       - Stack's call handle
 *          haCall       - Application's call handle
 *          index        - for future use (currently will be set to zero)
 * Output:  hFwNatConn    - The FwNat connection related to the call or NULL
 ********************************************************************************/
RvStatus RVCALLCONV epFwNatGetByCallEv(
    IN  HAPP                hApp,
    IN  HH323FWNATMGR       hFwNat,
    IN  HCALL               hsCall,
    IN  HAPPCALL            haCall,
    IN  RvInt32             index,
    OUT HH323FWNATCONN *    hFwNatConn)
{
    EpCallObj * call = (EpCallObj *)haCall;
    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(hFwNat);
    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(index);
    assert (call != NULL);
    *hFwNatConn = call->hFwNatConn;
    return RV_OK;
}


/********************************************************************************
 * epFwNatSetProtConnHandleEv
 * ----------------------------------------------------------------------------
 * General: Set a FANTAS element handle for a ProtConn element.
 *
 * Return: RV_OK on success, negative on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:   hApp         - stack application handle.
 *          hFwNat       - FwNat module handle
 *          hProtConn    - Stack's connection handle
 *          hFwNatConn   - The FwNat connection related to the connection
 *                         or NULL to unset.
 * Output:  none.
 ********************************************************************************/
RvStatus RVCALLCONV epFwNatSetProtConnHandleEv(
    IN  HPROTCONN           hProtConn,
    IN  HH323FWNATCONN      hFwNatConn)
{
    EpConnectionInfo    *appCon = NULL;
    if ((cmGetTpktChanApplHandle(hProtConn, (HAPPCONN*)&appCon) >= 0) && (appCon != NULL))
        appCon->hFwNatConn = hFwNatConn;
    return RV_OK;
}


/********************************************************************************
 * Type Name: RvH323FwNatSetProtConnHandleEv_t (callback prototype)
 * ----------------------------------------------------------------------------
 * General: Get the FANTAS element handle associated with a ProtConn element.
 *
 * Return: RV_OK on success, negative on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:   hApp         - stack application handle.
 *          hFwNat       - FwNat module handle
 *          hProtConn    - Stack's connection handle
 * Output:  hFwNatConn   - The FwNat connection related to the connection
 *                         or NULL to unset.
 ********************************************************************************/
RvStatus RVCALLCONV epFwNatGetProtConnHandleEv(
    IN  HPROTCONN           hProtConn,
    OUT HH323FWNATCONN *    hFwNatConn)
{
    EpConnectionInfo    *appCon = NULL;
    *hFwNatConn = NULL;
    if ((cmGetTpktChanApplHandle(hProtConn, (HAPPCONN*)&appCon) >= 0) && (appCon != NULL))
        *hFwNatConn = appCon->hFwNatConn;
    return RV_OK;
}


/********************************************************************************
 * Name: epFwNatRasEncapStatusEv
 * Description: Notifies the application as to the status of the H.460.17 RAS
 *              encapsulation over TPKT connection is.
 *
 * Input:   hApp            - stack application handle.
 *          hFwNat          - FwNat module handle.
 *          hProtConn       - Stack's connection handle.
 *          hFwNatConn      - FwNat connection handle.
 *          status          - Status of connection (if not GK, status of the module).
 *          hsRas           - Attempted RAS transaction handle.
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RVCALLCONV epFwNatRasEncapStatusEv(
    IN  HAPP                    hApp,
    IN  HH323FWNATMGR           hFwNat,
    IN  HPROTCONN               hProtConn,
    IN  HH323FWNATCONN          hFwNatConn,
    IN  RvH323FwNatH46017State  status,
    IN  HRAS                    hsRas)
{
    EpObj       *ep;

    RV_UNUSED_ARG(hFwNat);
    RV_UNUSED_ARG(hProtConn);
    RV_UNUSED_ARG(hFwNatConn);
    RV_UNUSED_ARG(hsRas);

    cmGetHandle(hApp, (HAPPAPP *)&ep);

    epUtilsEvent(ep, "H460", NULL, "rasEncapStatusEv.%d", status);

    switch (status)
    {
    case RvH323FwNatH46017StateDisabled:
        epUtilsLog(ep, NULL, "RAS encapsulation connection disabled");
        break;
    case RvH323FwNatH46017StateEnabled:
        epUtilsLog(ep, NULL, "RAS encapsulation connection enabled");
        break;
    case RvH323FwNatH46017StateConnected:
        epUtilsLog(ep, NULL, "RAS encapsulation connection connected");
        ep->cfg.callConfig.multiplexing = 1;
        ep->cfg.callConfig.maintainConnection = 1;
        break;
    case RvH323FwNatH46017StateError:
        epUtilsLog(ep, NULL, "RAS encapsulation connection error");
        break;
    default:
        break;
    }
    return RV_OK;
}

/********************************************************************************
 * Name: epFwNatHookOutConn
 * Description:  This function should be called from cmHookOutConn().
 * Input:   hConn        - Stack's handle for the connection.
 *          addrFrom     - PVT node ID of the local address
 *          addrTo       - PVT node ID of the remote address
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
int RVCALLCONV epFwNatHookOutConn(
        IN      HPROTCONN           hConn,
        IN      int                 addrFrom,
        IN      int                 addrTo,
        IN      RvBool              error)
{
    EpObj *ep;
    RV_UNUSED_ARG(error);
    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

    return RvH323FwNatHookOutConn(ep->hFwNat, hConn, addrFrom, addrTo);
}

/********************************************************************************
 * Name: epFwNatHookClose
 * Description:  This function should be called from hookClose().
 *              Allows the FW/NAT module to free related resources.
 * Input:   hConn        - Stack's handle for the connection.

 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
int RVCALLCONV epFwNatHookClose(
        IN      HPROTCONN           hConn)
{
    EpObj *ep;
    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

    return RvH323FwNatHookClose(ep->hFwNat, hConn);
}

/********************************************************************************
 * Function Name: epFwNatHookRecv
 * Description: This function should be called from hookRecv().
 *              It is used to extract FwNat information from the Q.931 messages.
 *
 * Input:   hFwNat          - FwNat module handle
 *          nodeId          - Stack's handle for the RAS transaction
 *          hConn           - Stack's handle for the connection.
 *          error           -
 * Output:  none.
 * Return: RV_TRUE if message should be ignored by the stack 
 ********************************************************************************/
RvBool RVCALLCONV epFwNatHookRecv(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      RvBool              error)
{
    EpObj *ep;
    RvBool bInfo = RV_FALSE, bIgnore = RV_FALSE;

    RV_UNUSED_ARG(error);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

    /*Handle a facility after SCI/SCR if that's what we have here*/
    RvH323FwNatHookRecv(ep->hFwNat, hConn, nodeId, &bIgnore, &bInfo);
    return bIgnore;
}

/********************************************************************************
 * Function Name: epFwNatHookRecvFrom
 * Description: This function should be called from hookRecvFrom().
 *              It may modify the message.
 *
 * Input:   nodeId   - Stack's handle for the RAS transaction
 * Output:  none.
 *                            information.
 * Return: RV_TRUE if message should be ignored by the stack 
 ********************************************************************************/
RvBool RVCALLCONV epFwNatHookRecvFrom(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      int                 addrFrom,
    IN      RvBool              multicast,
    IN      RvBool              error)
{
    EpObj      *ep;
    RvBool      bInfo = RV_FALSE, bIgnore = RV_FALSE;
    RvStatus    status;

    RV_UNUSED_ARG(addrFrom);
    RV_UNUSED_ARG(multicast);
    RV_UNUSED_ARG(error);

    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);

    /* Modify rasAddress in the message if we have to */
    status = RvH323FwNatHookRecvFrom(ep->hFwNat, nodeId, addrFrom, &bInfo);

    if (bInfo)
    {
        RvBool bSupported, bNeeded, bDesired;
        RvPvtNodeId supportedNodeId, neededNodeId, desiredNodeId;
        cmIsFoundFeature(ep->hApp, nodeId, RV_H323_FWNAT_SIGNALLING_TRAVERSAL_IDENTIFIER_VALUE,
            &bSupported, &bNeeded, &bDesired, &supportedNodeId, &neededNodeId, &desiredNodeId);
        if (bSupported)
            epUtilsEvent(ep, "H460", NULL, "usingFeature.H460.18");
    }

    return bIgnore;
}

/********************************************************************************
 * Function Name: epFwNatHookSendRaw
 * Description: This function should be called from hookSendRaw().
 * Input:   hConn           - Stack's handle connection.
 *          rawMessage      - Outgoing message buffer.
 *          messageLen      - Message length.
 * Output:  none.
 * Return: RV_TRUE iff the message should not be sent (to be
 *                            returned on exit from hookSendRawTo).
 ********************************************************************************/
RvBool RVCALLCONV epFwNatHookSendRaw(
        IN      HPROTCONN           hConn,
        IN      const RvUint8 *     rawMessage,
        IN      RvInt32             messageLen)
{
    EpObj * ep;
    RvBool bStop = RV_FALSE;
    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);
    RvH323FwNatHookSendRaw(ep->hFwNat, hConn, rawMessage, messageLen, &bStop);
    return bStop;
}

/********************************************************************************
 * Function Name: epFwNatHookSendRawTo
 * Description: This function should be called from hookSendRawTo().
 *              Encapsulates the RAS messages in facility messages.
 *
 * Input:   hConn           - Stack's handle connection.
 *          rawMessage      - Outgoing message buffer.
 *          messageLen      - Message length.
 *          addrTo          - Address sent to.
 * Output:  none.
 * Return: RV_TRUE iff the message should not be sent (to be
 *                            returned on exit from hookSendRawTo).
 ********************************************************************************/
RvBool RVCALLCONV epFwNatHookSendRawTo(
        IN      HPROTCONN           hConn,
        IN      const RvUint8 *     rawMessage,
        IN      RvInt32             messageLen,
        IN      cmTransportAddress* addrTo)
{
    EpObj * ep;
    RvBool bStop = RV_FALSE;
    cmGetHandle(cmGetAHandle((HPROTOCOL)hConn), (HAPPAPP *)&ep);
    RvH323FwNatHookSendRawTo(ep->hFwNat, hConn, rawMessage, messageLen, addrTo, &bStop);
    return bStop;
}

/********************************************************************************
 * Function Name: epFwNatEvRASRequest
 * Description: This function should be called from cmEvRASRequest(). This API
 *              is used in the EP inside the NAT to handle an incoming call (SCI message).
 *
 * Input:   hsRas        - Stack's handle for the RAS transaction
 * Output:  none.
 * Return: RV_TRUE if the application should ignore the message.
 ********************************************************************************/
RvBool RVCALLCONV epFwNatEvRASRequest(
        IN       HRAS    hsRas)
{
    EpObj *     ep;
    RvBool bInfo = RV_FALSE, bIgnore = RV_FALSE;

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsRas), (HAPPAPP *)&ep);

    RvH323FwNatEvRASRequest(ep->hFwNat, hsRas, &bIgnore, &bInfo);

    return bIgnore;
}

/********************************************************************************
 * Function Name: epFwNatEvAutoRASRequest
 * Description: This function should be called from cmEvAutoRASRequest(). This API
 *              is used in the EP inside the NAT to handle an incoming call (SCI message).
 *
 * Input:   hsRas        - Stack's handle for the RAS transaction
 * Output:  none.
 * Return: RV_TRUE if the application should ignore the message.
 ********************************************************************************/
RvBool RVCALLCONV epFwNatEvAutoRASRequest(
        IN       HRAS    hsRas)
{
    EpObj *     ep;
    RvBool bInfo = RV_FALSE, bIgnore = RV_FALSE;

    cmGetHandle(cmGetAHandle((HPROTOCOL)hsRas), (HAPPAPP *)&ep);

    RvH323FwNatEvAutoRASRequest(ep->hFwNat, hsRas, &bIgnore, &bInfo);

    return bIgnore;
}

/********************************************************************************
 * Name: epFwNatSendKeepAlive
 * Description: Send keepAlive packet on each request/response and starts timers 
 *              on outgoing channels if channels created.
 *
 * Input:   channel        - Channel object to use - can be NULL.
 *          origin      - RV_TRUE for outgoing channels.
 *          rtpSession  - The rtp session.
 * Output:  none.
 * Return:  None.
 ********************************************************************************/
void epFwNatSendKeepAlive(
    IN EpChannelObj *    channel, 
    IN RvBool            origin, 
    IN RvInt32           rtpSession)
{
    EpObj *     ep;
    EpCallObj * call;
    RvStatus    status = RV_OK;

    RV_UNUSED_ARG(rtpSession);

    if (channel == NULL)
        return;

    call = channel->call;

    if (!call->bUseH46019)
        return;

    ep = channel->call->ep;

    if (ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigClient && 
        origin == RV_TRUE &&
        cmChannelIsDuplex(channel->hsChan) == RV_TRUE)
        origin = RV_FALSE;

    if (ep->cfg.fwNatConfig.e19Config != RvH323FwNatH46019ConfigClient || origin)
        return;

    /*send keepAlive on the session if incoming channel*/
    if (channel->keepAliveTimer != HTI_NULL_TIMER /*&& times > 0*/)
    {
        status = epRtpSendKeepAlive(ep->rtp, rtpSession);
    }

    /*start timers*/
    if (status == RV_OK && channel != NULL && channel->keepAliveTimer == HTI_NULL_TIMER)
    {
        channel->keepAliveTimerSet = RV_TRUE;
        channel->keepAliveTimer = mtimerSet(ep->hTimers, keepAlive_handler, (void *)channel, 100);
        epUtilsLog(ep, call, "keepAliveTimer on channel %p created", channel);
    }
    else
    {
        
        if (channel->keepAliveTimerSet == RV_TRUE)
        {
            mtimerReset(ep->hTimers, channel->keepAliveTimer);
            channel->keepAliveTimer = mtimerSet(ep->hTimers, keepAlive_handler, (void *)channel, 30*1000);
            channel->keepAliveTimerSet = RV_FALSE;
        }

    }
}

/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/

/********************************************************************************
 * Name: getSessioIdFromOLCMessage
 * Description: Retrieves the sessionId from an OLC message.
 *
 * Input:   ep              - endpoint object to use.
 *          message         - OLC message root nodeId.
 *          bIsFaststart    - RV_TRUE if a faststart process.
 *          fieldId         - fieldId of message.
 * Output:  sessionId       - sessionId in OLC/OLCA.
 * Return:  None.
 ********************************************************************************/
static void getSessioIdFromOLCMessage(
    IN EpObj *      ep,
    IN RvPvtNodeId  message,
    IN RvBool       bIsFaststart,
    IN RvPstFieldId fieldId,
    OUT RvInt32 *   sessionId)
{
    RvPvtNodeId  olcNodeId, sessionNodeId;
    HPVT         hVal = cmGetValTree(ep->hApp);

    *sessionId = -1;

    if (!bIsFaststart)
    {
        RvPstFieldId oldFieldId = -1;
        /*find the type of message we're dealing with*/
        __pvtGetByFieldIds(olcNodeId, hVal, message,
        {_anyField _anyField LAST_TOKEN}, &oldFieldId, NULL, NULL);
        
        if (oldFieldId != __h245(openLogicalChannel))
            return;
    }
    else
        olcNodeId = message;

    if (fieldId == __h245(openLogicalChannel))
        __pvtGetByFieldIds(sessionNodeId, hVal, olcNodeId, 
                        {_h245(forwardLogicalChannelParameters) 
                         _h245(multiplexParameters) 
                         _h245(h2250LogicalChannelParameters)
                         _h245(sessionID)
                         LAST_TOKEN}, 
                         NULL, 
                         sessionId,
                         NULL);

    if (fieldId == __h245(openLogicalChannelAck))
         __pvtGetByFieldIds(sessionNodeId, hVal, olcNodeId, 
                        {_h245(reverseLogicalChannelParameters) 
                         _h245(multiplexParameters) 
                         _h245(h2250LogicalChannelParameters)
                         _h245(sessionID)
                         LAST_TOKEN}, 
                         NULL, 
                         sessionId,
                         NULL);
}

/*event handler of the keepAlive timer*/
static void RVCALLCONV keepAlive_handler(void * context)
{
    EpChannelObj * channel = (EpChannelObj *)context;
    RvBool         origin;

    cmChannelGetOrigin(channel->hsChan, &origin);

    epFwNatSendKeepAlive(channel, origin, channel->rtpSession);
}



#endif

#if defined(__cplusplus)
}
#endif

