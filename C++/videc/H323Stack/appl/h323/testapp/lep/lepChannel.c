/**********************************************************************
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

/***********************************************************************
epChannel.c

Channel handling of the endpoint.
Handles the channel procedures.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "stkutils.h"
#include "h245.h"

#include "lepRtp.h"
#include "lepHash.h"
#include "lepUtils.h"
#include "lepConversions.h"
#include "lepChannel.h"
#include "lepSecurity.h"
#include "lepFwNat.h"

#ifdef USE_PLAYER
#include "lepPlayMedia.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/





/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/
static EpChannelObj *channelCreate(
    IN EpObj        *ep,
    IN EpCallObj    *call,
    IN RvInt32      id,
    IN HCHAN        hsChan);
static RvPstFieldId getDataTypeFieldId(
    IN HPVT         hVal,
    IN RvPvtNodeId  dataTypeId);




/*-----------------------------------------------------------------------*/
/*                           CALLBACK FUNCTIONS                          */
/*-----------------------------------------------------------------------*/


int RVCALLCONV cmEvCallNewChannel(
    IN  HAPPCALL   haCall,
    IN  HCALL      hsCall,
    IN  HCHAN      hsChan,
    OUT LPHAPPCHAN lphaChan)
{
    EpChannelObj *chan;
    EpChannelObj *partnerChannel = NULL;
    EpCallObj *call = (EpCallObj *)haCall;
    RvUint16 port;
    RvBool   isMultiplexed;

    RV_UNUSED_ARG(hsCall);

    chan = channelCreate(call->ep, call, 0, hsChan);
    if (chan == NULL)
    {
        return -1;
    }

    /* if we already opened an RTP session for the opposite channel, include this one in it */
    cmChannelSameSession(hsChan, (HAPPCHAN *)&partnerChannel, NULL);
    if ((partnerChannel != NULL) && (partnerChannel->rtpSession >= 0))
    {
        /* we did, so we just connect this channel to it */
        chan->rtpSession = partnerChannel->rtpSession;
        chan->multiplexID = partnerChannel->multiplexID;
        chan->remoteMultiplexID = partnerChannel->remoteMultiplexID;
        epRtpSessionSetAction(call->ep->rtp, chan->rtpSession, call->action);
        epRtpSessionJoin(call->ep->rtp, chan->rtpSession);

        if ((call->isFastConnectChannels) && (call->isIncoming) && (!chan->isOutgoing))
        {
            /* this is an incoming FS channel: fill in rtp and rtcp */
            port = epRtpSessionGetLocalPort(call->ep->rtp, chan->rtpSession);
            cmChannelSetAddress(hsChan, 0, port);
            cmChannelSetRTCPAddress(hsChan, 0, (RvUint16)(port + 1));
        }
    }
    else
    {
        if (call->action == epRtpActionReplay)
        {
            RvUint32 muxId=(RvUint32)-1;
            RV_UNUSED_ARG(muxId);
            if (call->isFastConnectChannels)
            {
                /* if we are the destination of a Fast Start call, and are replaying RTP, we now connect
                   the RTP. if we are not the destination, we will do it when we know the channel type */
                if (call->isIncoming)
                {
                    cmCapDataType capData = cmCapEmpty;
                    int sessionIndex;

                    if ((cmChannelGetParam(hsChan, cmChannelParamCapDataType, (RvInt32*)&capData, NULL) == RV_OK) &&
                        ((capData == cmCapAudio) || (capData == cmCapVideo) || (capData == cmCapData)))
                    {
                        sessionIndex = capData - 1;

                        isMultiplexed = (epFwNatIsCallMultiplexed(call) && call->bRemoteMediaMultiplexEnabled);

                        /* we are, so we open a new RTP session */
                        if (!(isMultiplexed))
                        {
                            if (call->rtpSessions[sessionIndex] >= 0)
                            {
                                /*rtpSession already created*/
                                chan->rtpSession = call->rtpSessions[sessionIndex];
                                chan->multiplexID = (RvUint32)-1;
                                chan->remoteMultiplexID = call->remoteMultiplexID[sessionIndex];
                                call->rtpSessions[sessionIndex] = -1;
                                call->multiplexID[sessionIndex] = (RvUint32)-1;
                                call->remoteMultiplexID[sessionIndex] = (RvUint32)-1;
                            }
                            else
                                chan->rtpSession = epRtpSessionNew(call, RV_FALSE, &muxId);
                        }
                        else
                        {
                            if (call->muxedRtpSessions[sessionIndex] >= 0)
                            {
                                /*rtpSession already created*/
                                chan->rtpSession = call->muxedRtpSessions[sessionIndex];
                                chan->multiplexID = call->multiplexID[sessionIndex];
                                chan->remoteMultiplexID = call->remoteMultiplexID[sessionIndex];
                                call->muxedRtpSessions[sessionIndex] = -1;
                                call->multiplexID[sessionIndex] = (RvUint32)-1;
                                call->remoteMultiplexID[sessionIndex] = (RvUint32)-1;
                            }
                            else
                            {
                                epFwNatCreateChannelMuxedRtpSession(chan);
                            }
                        }

                        if (chan->rtpSession >= 0)
                        {
#ifdef USE_FWNAT
                            RvBool origin;
                            cmChannelGetOrigin(chan->hsChan, &origin);

                            /*send keepAlive from client*/
                            if ((chan->keepAliveTimer == HTI_NULL_TIMER) && 
                                (call->ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigClient) && 
                                ((!chan->isOutgoing) || (cmChannelIsDuplex(chan->hsChan) && origin == RV_TRUE)))
                            {
                                epFwNatSendKeepAlive(chan, RV_FALSE, chan->rtpSession);
                            }
#endif
                            /*get session address*/
                            port = epRtpSessionGetLocalPort(call->ep->rtp, chan->rtpSession);
                    
                            if (!chan->isOutgoing)
                            {
                                cmChannelSetAddress(hsChan, 0, port);
                            }
                            cmChannelSetRTCPAddress(hsChan, 0, (RvUint16)(port + 1));
                        }
                    }
                }
            }
            else
            {
                isMultiplexed = epFwNatIsCallMultiplexed(call);

                /* Normal call with a normal channel - just open the RTP session for it */
                if (!isMultiplexed)
                {
                    chan->rtpSession = epRtpSessionNew(call, RV_FALSE, &muxId);
                }
                else
                {
                    epFwNatCreateChannelMuxedRtpSession(chan);
                }
            }
        }
    }

    /* Set the application handle */
    *lphaChan = (HAPPCHAN)chan;

    if (chan->isIndependent)
    {
        epUtilsEvent(call->ep, "Script", call, "cb:cm:EvChannelNew %p %p", call, chan);
    }

#ifdef USE_PLAYER
    PlayerEvNewChannel (haCall, hsCall, hsChan, lphaChan);
#endif

    return 0;
}


int RVCALLCONV cmEvChannelStateChanged(
    IN      HAPPCHAN                haChan,
    IN      HCHAN                   hsChan,
    IN      cmChannelState_e        state,
    IN      cmChannelStateMode_e    stateMode)
{
    EpObj *ep;
    EpChannelObj *chan;
    EpCallObj *call;

    chan = (EpChannelObj *)haChan;
    if (chan == NULL)
    {
        if (state == cmChannelStateIdle)
            cmChannelClose(hsChan);
        return -1;
    }

    call = chan->call;
    ep = call->ep;

    epUtilsEvent(ep, "chanState", call, "%s %s %d",
        CMChannelState2String(state), CMChannelStateMode2String(stateMode), chan->id);

    lepLockCall(call);
    chan->state = state;
    lepUnlockCall(call);

    if (chan->isIndependent)
    {
        /* Advanced script handling */
        epUtilsEvent(ep, "Script", call, "cb:cm:EvChannelStateChanged %p %s %s",
                   chan, CMChannelState2String(state), CMChannelStateMode2String(stateMode));

        if (state == cmChannelStateIdle)
        {
            epChannelClose(ep, chan);
            if (call->numOfChannels >= 0)
                /* Redisplay the list of channels */
                epUtilsEvent(ep, "chanStateDisplay", call, "");
        }
        return 0;
    }

    switch (state)
    {
        case cmChannelStateOffering:
        {
            EpChannelObj    *sameSessionChannel = NULL;

            cmChannelSameSession(hsChan, (HAPPCHAN *)&sameSessionChannel, NULL);

            lepLockCall(call);
            /* Check to see if this channel has another of the same session, which was not already
              connected to it in cmEvCallNewChannel, and which has an RTP session */
            if ((sameSessionChannel != NULL) && (call->action == epRtpActionReplay) &&
                (chan->rtpSession != sameSessionChannel->rtpSession) &&
                (sameSessionChannel->rtpSession >= 0))
            {
                if (chan->rtpSession >= 0)
                {
                    RvBool isMultiplexed = RV_FALSE;
                    /* We have an open session already - add it as a second channel to that session.
                       This happens when we're the side that initiated the OLC, which never happens for
                       the test application with real media. */    
                    isMultiplexed = (chan->multiplexID != (RvUint32)-1);
                    epRtpSessionClose(ep->rtp, chan->rtpSession, isMultiplexed);
                }
                chan->rtpSession = sameSessionChannel->rtpSession;
                chan->multiplexID = sameSessionChannel->multiplexID;
                chan->remoteMultiplexID = sameSessionChannel->remoteMultiplexID;
                epRtpSessionSetAction(ep->rtp, chan->rtpSession, call->action);
                epRtpSessionJoin(ep->rtp, chan->rtpSession);
            }
            lepUnlockCall(call);

            /* Make sure we open the incoming channel window */
            if (ep->cfg.bAutoAcceptChannels)
            {
                /* Automatically accept the channel */
                lepLockCall(call);
                cmChannelSetAddress(chan->hsChan, 0, epRtpSessionGetLocalPort(ep->rtp, chan->rtpSession));
                cmChannelSetRTCPAddress(chan->hsChan, 0, (RvUint16)(epRtpSessionGetLocalPort(ep->rtp, chan->rtpSession) + 1));
                lepUnlockCall(call);
                cmChannelAnswer(chan->hsChan);

                /* Mimic channel opening if we have to */
                if (ep->cfg.bAutoMimicChannels)
                    epChannelMimic(chan, 0);
            }
            else
                ep->cb.epChannelNew(chan);
            break;
        }

        case cmChannelStateConnected:
            {
                ep->cb.epChannelStable(chan);

                chan->wasConnected = RV_TRUE;
#ifdef USE_SECURITY
                /* Remember: the secVector should be padded to a multiple of the block size */
                //RTP_TestSetSessionDESEncryption(chan->rtpSession, (RvUint8*)call->secVector);
#endif

#ifdef USE_RTP
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
                {
                    /* Enabling the use of IPv6 disabled cmEvChannelSetAddress and cmEvChannelSetRTCPAddress callbacks */
                    RvInt32 len = sizeof(cmTransportAddress);
                    cmTransportAddress remoteAddr;
                    if (cmChannelGetParam(hsChan, cmChannelParamH225RemoteRtpAddress, &len, (RvUint8*)&remoteAddr) == RV_OK)
                    { 
                        lepLockCall(call);
                        {  
#ifdef USE_FWNAT
                            /*remote address should be ignored by server*/
                            if (!(call->bUseH46019 && ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigServer))
#endif
                            {
                                RvBool bHasAddress = epRtpSessionHasRemoteRTPAddress(ep->rtp, chan->rtpSession);
                                if (!bHasAddress)
                                {
                                    epRtpSessionSetRemoteRTP(call, chan->rtpSession, &remoteAddr, RV_FALSE, (RvUint32)-1);
                                    epUtilsLog(ep, call, "remote address %d:%d set on session %d, remoteIsMultiplexed=%d, muxId=%d",
                                        remoteAddr.addr.v4.ip, remoteAddr.port, chan->rtpSession, RV_FALSE, (RvUint32)-1);
                                    
                                }
                            }
                        }
                        lepUnlockCall(call);
                    }
                    if (cmChannelGetParam(hsChan, cmChannelParamH225RemoteRtcpAddress, &len, (RvUint8*)&remoteAddr) == RV_OK)
                    {
                        lepLockCall(call);
#ifdef USE_FWNAT
                         /*remote address was already set if remote is multiplexed, 
                           should be ignored by server if FWNAT is used in the call*/
                        if (!(call->bUseH46019 && ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigServer))
#endif
                        {
                            RvBool bHasAddress = epRtpSessionHasRemoteRTCPAddress(ep->rtp, chan->rtpSession);
                            if (!bHasAddress)
                            {
                                epRtpSessionSetRemoteRTCP(call, chan->rtpSession, &remoteAddr, RV_FALSE, (RvUint32)-1);
                            }
                        }
                        lepUnlockCall(call);
                    }
                }
#endif
#endif

#if 0
                lepLockCall(call);
                if (!chan->isOutgoing)
                {
                    /* Set the handling of DTMF dynamic payloads of an incoming channel */
                    if ((chan->dynPayloadType != 0) &&
                        ((chan->dynPayloadType == GetDTMFTonePayload()) ||
                         (chan->dynPayloadType == GetDTMFEventPayload())))
                    {
                        RTP_TestSetPayloadCallback(
                            chan->rtpSession,
                            chan->dynPayloadType,
                            dtmfEventHandler,
                            chan);
                    }

                    /* For FECC channels, set the handling of the payload if it's an FECC channel */
                    if ((chan->dynPayloadType != 0) &&
                        ((int)chan->dynPayloadType==GetFECCPayload()))
                    {
                        RTP_TestSetPayloadCallback(
                            chan->rtpSession,
                            chan->dynPayloadType,
                            feccEventHandler,
                            chan);
                    }
                }
                lepUnlockCall(call);
#endif

                if (stateMode == cmChannelStateModeOff)
                {
                    /* mark channel as paused */
                    lepLockCall(call);
                    strcpy(chan->dataTypeName, "<PAUSED>");
                    lepUnlockCall(call);
                }

                if (stateMode == cmChannelStateModeReconfigured)
                {
                    HPVT hVal = chan->call->ep->hVal;
                    RvPvtNodeId dataTypeNode;
                    RvPstFieldId dataType;
                    HPST synConf;
                    /* check for new data type */
                    cmChannelGetParam(hsChan, cmChannelParamDataTypeNodeId, &dataTypeNode, NULL);
                    dataType = getDataTypeFieldId(hVal, dataTypeNode);
                    synConf = pvtGetSynTree(hVal, dataTypeNode);
                    lepLockCall(call);
                    pstGetFieldName(synConf, dataType, sizeof(chan->dataTypeName), chan->dataTypeName);
                    lepUnlockCall(call);
                }
                break;
            }

        case cmChannelStateDisconnected:
            {
                chan->wasConnected = RV_FALSE;
                break;
            }

        case cmChannelStateDialtone:
        case cmChannelStateRingBack:
            {
                epUtilsEvent(ep, "H245", call, "chanSid.%d.%d", chan->id, cmChannelSessionId(hsChan));
                break;
            }

        case cmChannelStateIdle:
        {
            ep->cb.epChannelStable(chan);

            /* free and close the channel */
            epChannelClose(ep, chan);
            break;
        }

        default:
        {
            break;
        }
    }

    /* Redisplay the list of channels */
    epUtilsEvent(ep, "chanStateDisplay", call, "");

    return 0;
}


int RVCALLCONV cmEvChannelNewRate(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvUint32            rate)
{
    EpChannelObj *chan = (EpChannelObj *)haChan;

    RV_UNUSED_ARG(hsChan);

    epUtilsEvent(chan->call->ep, "H245", chan->call, "cmEvChannelNewRate.%d.%d", chan->id, rate);

    return 0;
}


int RVCALLCONV cmEvChannelMaxSkew(
    IN      HAPPCHAN            haChan1,
    IN      HCHAN               hsChan1,
    IN      HAPPCHAN            haChan2,
    IN      HCHAN               hsChan2,
    IN      RvUint32            skew)
{
    RV_UNUSED_ARG(haChan1);
    RV_UNUSED_ARG(hsChan1);
    RV_UNUSED_ARG(haChan2);
    RV_UNUSED_ARG(hsChan2);
    RV_UNUSED_ARG(skew);
    return 0;
}


#if (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY)
int RVCALLCONV cmEvChannelSetAddress(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvUint32            ip,
    IN      RvUint16            port)
{
#ifdef USE_RTP
    EpChannelObj        *chan = (EpChannelObj *)haChan;
    cmTransportAddress  addr;
    RvBool              remoteIsMultiplexed = RV_FALSE;
    RvUint32            invalidMuxId = (RvUint32)-1;
#endif
    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(ip);
    RV_UNUSED_ARG(port);

    if (haChan == NULL)
        return RV_ERROR_UNKNOWN;
#ifdef USE_RTP
    addr.type = cmTransportTypeIP;
    addr.ip = ip;
    addr.port = port;
    addr.distribution = cmDistributionUnicast;
    lepLockCall(chan->call);
#ifdef USE_FWNAT
    remoteIsMultiplexed = !(chan->remoteMultiplexID == invalidMuxId);
#endif
    /*remote address was already set if remote is multiplexed*/
    if (!remoteIsMultiplexed)
    {
        if (epRtpSessionHasRemoteRTPAddress(chan->call->ep->rtp, chan->rtpSession) == RV_FALSE)
        {
            epRtpSessionSetRemoteRTP(chan->call, chan->rtpSession, &addr, remoteIsMultiplexed, (RvUint32)-1);
            epUtilsLog(chan->call->ep, chan->call, "remote address %d:%d set on session %d, remoteIsMultiplexed=%d, muxId=%d",
                                    addr.ip, addr.port, chan->rtpSession, remoteIsMultiplexed, (RvUint32)-1);
        }
    }
    lepUnlockCall(chan->call);
#endif

#ifdef USE_PLAYER
    PlayerEvChannelSetAddress(haChan, hsChan, ip, port);
#endif

    return 0;
}


int RVCALLCONV cmEvChannelSetRTCPAddress(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvUint32            ip,
    IN      RvUint16            port)
{
#ifdef USE_RTP
    EpChannelObj        *chan = (EpChannelObj *)haChan;
    cmTransportAddress  addr;
    RvBool              remoteIsMultiplexed = RV_FALSE;
    RvUint32            invalidMuxId = (RvUint32)-1;
	RvBool				bEp46019ConfigServer = RV_FALSE;
#endif
    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(ip);
    RV_UNUSED_ARG(port);

    if (haChan == NULL)
        return RV_ERROR_UNKNOWN;
#ifdef USE_RTP
    addr.type = cmTransportTypeIP;
    addr.ip = ip;
    addr.port = port;
    addr.distribution = cmDistributionUnicast;
    lepLockCall(chan->call);
#ifdef USE_FWNAT
    remoteIsMultiplexed = !(chan->remoteMultiplexID == invalidMuxId);
	if (chan->call->bUseH46019 && (chan->call->ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigServer))
		bEp46019ConfigServer = RV_TRUE;
#endif
    /*remote address was already set if remote is multiplexed, 
    should be ignored by server if FWNAT is used in the call*/
    if (!remoteIsMultiplexed && !bEp46019ConfigServer)
        epRtpSessionSetRemoteRTCP(chan->call, chan->rtpSession, &addr, remoteIsMultiplexed, (RvUint32)-1);
    lepUnlockCall(chan->call);
#endif

	

#ifdef USE_PLAYER
    PlayerEvChannelSetRTCPAddress(haChan, hsChan, ip, port);
#endif

    return 0;
}
#endif /* (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY) */


int RVCALLCONV cmEvChannelParameters(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      char*               channelName,
    IN      HAPPCHAN            haChanSameSession,
    IN      HCHAN               hsChanSameSession,
    IN      HAPPCHAN            haChanAssociated,
    IN      HCHAN               hsChanAssociated,
    IN      RvUint32            rate)
{
    EpChannelObj *chan = (EpChannelObj *)haChan;
    EpCallObj *call;

    RV_UNUSED_ARG(haChanSameSession);
    RV_UNUSED_ARG(haChanAssociated);
    RV_UNUSED_ARG(hsChanAssociated);

    if (chan == NULL)
        return -1;

    call = chan->call;
    epUtilsEvent(call->ep, "H245", call, "cmEvChannelParameters.%d.%s.%d",
        chan->id, ((channelName != NULL) ? channelName : "(null)"), rate);

    /* there is implementation for this call back in a script */
    if (chan->isIndependent)
    {
        /* ToDo: fix TCL to correspond */
        epUtilsEvent(call->ep, "Script", call, "cb:cm:EvH245ChannelEstablishRequest %p %p {ChannelName : %s \n ChannelHandle : %p \n SameSessionChannelHandle : %p \n ChannelRate : %d}",
            call, chan, ((channelName != NULL) ? channelName : ""), hsChan, hsChanSameSession, rate);
    }

    lepLockCall(call);

    /* Make sure we remember this channel - it will help us to mimic it */
    if ((chan->dataTypeName[0] == 0) && (channelName != NULL))
    {
        strncpy(chan->dataTypeName, channelName, sizeof(chan->dataTypeName));
        chan->dataTypeName[sizeof(chan->dataTypeName)-1] = '\0';
    }

    /* if we are the origin of a Fast Start call, and we are replaying RTP, we now connect the RTP
      according to the RTP sessions we kept when we created the FS message*/
    if (!call->isIncoming && (call->action == epRtpActionReplay) && call->isFastConnectChannels && (chan->rtpSession < 0) &&
        ((call->rtpSessions[0] >= 0 || call->muxedRtpSessions[0] >= 0) || 
         (call->rtpSessions[1] >= 0 || call->muxedRtpSessions[1] >= 0) || 
         (call->rtpSessions[2] >= 0 || call->muxedRtpSessions[2] >= 0)))
    {
        int index = -1;
        /* we set the RTP session according to the type */
        if (chan->dataTypeName[0] == 'g')
            index = 0;
        else if (chan->dataTypeName[0] == 'h')
            index = 1;
        else if (chan->dataTypeName[0] == 't')
            index = 2;

        if (index >= 0)
        {
            if (call->rtpSessions[index] >= 0)
            {
                chan->rtpSession = call->rtpSessions[index];
                chan->remoteMultiplexID = call->remoteMultiplexID[index];
                call->rtpSessions[index] = -1;
                call->remoteMultiplexID[index] = (RvUint32)-1;
            }
            else if (call->muxedRtpSessions[index] >= 0)
            {
                chan->rtpSession = call->muxedRtpSessions[index];
                chan->multiplexID = call->multiplexID[index];
                chan->remoteMultiplexID = call->remoteMultiplexID[index];
                call->muxedRtpSessions[index] = -1;
                call->multiplexID[index] = (RvUint32)-1;
                call->remoteMultiplexID[index] = (RvUint32)-1;
            }
        }
    }

#ifdef USE_FWNAT
    {
        RvBool origin;
        cmChannelGetOrigin(chan->hsChan, &origin);

        /*if a client send keepAlive if we have to*/
        if ((chan->keepAliveTimer == HTI_NULL_TIMER) && 
            (call->ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigClient) && 
            ((!chan->isOutgoing) || (cmChannelIsDuplex(chan->hsChan) && origin == RV_TRUE)))
        {
            epFwNatSendKeepAlive(chan, RV_FALSE, chan->rtpSession);
        }
    }
#endif

    lepUnlockCall(call);
    return 0;
}


int RVCALLCONV cmEvChannelRTPDynamicPayloadType(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvInt8              dynamicPayloadType)
{
    EpChannelObj *chan = (EpChannelObj *)haChan;
    RV_UNUSED_ARG(hsChan);

    if (!haChan)
        return RV_ERROR_UNKNOWN;

    epUtilsEvent(chan->call->ep, "H245", chan->call, "cmEvChannelRTPDynamicPayloadType.%d.%d",
        chan->id, dynamicPayloadType);

    lepLockCall(chan->call);
    chan->dynPayloadType = dynamicPayloadType;
    lepUnlockCall(chan->call);

    return 0;
}


int RVCALLCONV cmEvChannelVideoFastUpdatePicture(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan)
{
    EpChannelObj *chan = (EpChannelObj *)haChan;

    RV_UNUSED_ARG(hsChan);

    epUtilsEvent(chan->call->ep, "H245", chan->call, "cmEvChannelVideoFastUpdatePicture.%d", chan->id);

    return 0;
}


int RVCALLCONV cmEvChannelVideoFastUpdateGOB(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      int                 firstGOB,
    IN      int                 numberOfGOBs)
{
    EpChannelObj *chan = (EpChannelObj *)haChan;

    RV_UNUSED_ARG(hsChan);

    epUtilsEvent(chan->call->ep, "H245", chan->call, "cmEvChannelVideoFastUpdateGOB.%d.%d.%d",
        chan->id, firstGOB, numberOfGOBs);

    return 0;
}


int RVCALLCONV cmEvChannelVideoFastUpdateMB(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      int                 firstGOB,
    IN      int                 firstMB,
    IN      int                 numberOfMBs)
{
    EpChannelObj *chan = (EpChannelObj *)haChan;

    RV_UNUSED_ARG(hsChan);

    epUtilsEvent(chan->call->ep, "H245", chan->call, "cmEvChannelVideoFastUpdateMB.%d.%d.%d.%d",
        chan->id, firstGOB, firstMB, numberOfMBs);

    return 0;
}


int RVCALLCONV cmEvChannelHandle(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      int                 dataTypeHandle,
    IN      cmCapDataType       dataType)
{
    EpChannelObj *chan = (EpChannelObj *)haChan;

    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(dataTypeHandle);

    if (chan == NULL)
        return -1;

    epUtilsEvent(chan->call->ep, "H245", chan->call, "cmEvChannelHandle.%d.%d", chan->id, dataType);

    return 0;
}


int RVCALLCONV cmEvChannelGetRTCPAddress(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvUint32*           ip,
    IN      RvUint16*           port)
{
    RV_UNUSED_ARG(haChan);
    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(ip);
    RV_UNUSED_ARG(port);
    return 0;
}


int RVCALLCONV cmEvChannelRequestCloseStatus(
    IN      HAPPCHAN              haChan,
    IN      HCHAN                 hsChan,
    IN      cmRequestCloseStatus  status)
{
    EpChannelObj *chan = (EpChannelObj *)haChan;

    epUtilsEvent(chan->call->ep, "H245", chan->call, "cmEvChannelRequestCloseStatus.%d.%d", chan->id, status);
    epUtilsEvent(chan->call->ep, "chanReqClose", chan->call, "%d %d", chan->id, status);

    switch (status)
    {
    case cmRequestCloseRequest:
        {
            /* Received request to close channel */
            if (chan->call->ep->cfg.bAutoDropChannels == 1)
            {
                /* Automatically drop the channel */
                cmChannelAnswer(hsChan);
                cmChannelDrop(hsChan);
            }
            else
            {
                /* Manual drop - we should interact with the user */
                epUtilsEvent(chan->call->ep, "chanDropRequest", chan->call, "%p", chan);
            }
        }
        break;

    case cmRequestCloseReject:
        epUtilsEvent(chan->call->ep, "TCL", chan->call, "Window close .dropchan");
        if (chan->isOutgoing)
        {
            /* An incoming request to close an outgoing channel was probably ignored. Show an indication */
            epUtilsLog(chan->call->ep, chan->call, "Processing incoming request to release channelRequestClose");
        }
        break;

    case cmRequestCloseConfirm:
        break;
    }
    return 0;
}


int RVCALLCONV cmEvChannelTSTO(
    IN      HAPPCHAN              haChan,
    IN      HCHAN                 hsChan,
    IN      RvInt8                isCommand,
    IN      RvInt8                tradeoffValue)
{
    RV_UNUSED_ARG(haChan);
    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(isCommand);
    RV_UNUSED_ARG(tradeoffValue);
    return 0;
}


int RVCALLCONV cmEvChannelMediaLoopStatus(
    IN      HAPPCHAN              haChan,
    IN      HCHAN                 hsChan,
    IN      cmMediaLoopStatus     status)
{
    EpChannelObj *chan = (EpChannelObj *)haChan;

    epUtilsEvent(chan->call->ep, "H245", chan->call, "cmEvChannelMediaLoopStatus.%d.%d", chan->id, status);

    if (status == cmMediaLoopRequest)
        cmChannelMediaLoopConfirm(hsChan);

    return 0;
}


int RVCALLCONV cmEvChannelReplace(
     IN     HAPPCHAN              haChan,
     IN     HCHAN                 hsChan,
     IN     HAPPCHAN              haReplacedChannel,
     IN     HCHAN                 hsReplacedChannel)
{
    EpChannelObj *chan1 = (EpChannelObj *)haChan;
    EpChannelObj *chan2 = (EpChannelObj *)haReplacedChannel;

    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(hsReplacedChannel);

    epUtilsEvent(chan1->call->ep, "H245", chan1->call, "cmEvChannelReplace.%d.%d", chan1->id, chan2->id);

    return 0;
}


int RVCALLCONV cmEvChannelFlowControlToZero(
    IN      HAPPCHAN             haChan,
    IN      HCHAN                hsChan)
{
    RV_UNUSED_ARG(haChan);
    RV_UNUSED_ARG(hsChan);
    return 0;
}


int RVCALLCONV cmEvChannelMiscCommand(
     IN      HAPPCHAN             haChan,
     IN      HCHAN                hsChan,
     IN      cmMiscellaneousCommand miscCommand)
{
    EpChannelObj *chan = (EpChannelObj *)haChan;

    RV_UNUSED_ARG(hsChan);

    epUtilsEvent(chan->call->ep, "H245", chan->call, "cmEvChannelMiscCommand.%d.%d", chan->id, miscCommand);

    return 0;
}


int RVCALLCONV cmEvChannelTransportCapInd(
     IN      HAPPCHAN             haChan,
     IN      HCHAN                hsChan,
     IN      int                  nodeId)
{
    RV_UNUSED_ARG(haChan);
    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(nodeId);
    return 0;
}


int RVCALLCONV cmEvChannelSetNSAPAddress(
    IN      HAPPCHAN              haChan,
    IN      HCHAN                 hsChan,
    IN      char*                 address,
    IN      int                   length,
    IN      RvBool                multicast)
{
    RV_UNUSED_ARG(haChan);
    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(address);
    RV_UNUSED_ARG(length);
    RV_UNUSED_ARG(multicast);
    return 0;
}


int RVCALLCONV cmEvChannelSetATMVC(
    IN      HAPPCHAN              haChan,
    IN      HCHAN                 hsChan,
    IN      int                   portNumber)
{
    RV_UNUSED_ARG(haChan);
    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(portNumber);
    return 0;
}


int RVCALLCONV cmEvChannelRecvMessage(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvPvtNodeId         message)
{
    EpChannelObj *      chan = (EpChannelObj *)haChan;
    EpCallObj *         call = chan->call;
    EpObj *             ep   = call->ep;
    RvBool              bInfo = RV_FALSE;
#ifdef USE_FWNAT
    RvBool              bKeepAliveExpected = RV_FALSE;
#endif
    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(bInfo);
#ifdef USE_FWNAT
    lepLockCall(call);

    /*Retrieve h460.19 info from message*/
    if (ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigClient && cmChannelIsDuplex(chan->hsChan) == RV_TRUE)
    {
        
        RvBool        origin;
        cmChannelGetOrigin(chan->hsChan, &origin);
        if (origin)
            bKeepAliveExpected = RV_TRUE;
    }
    epFwNatGetH46019InfoFromMessage(call, chan, bKeepAliveExpected, message, NULL, -1, RV_FALSE, (cmChannelDirection)-1);

    lepUnlockCall(call);
#endif
    if ((chan != NULL) && ep->cfg.bScanNodeIDs)
        epScanPvtNode(ep, message);

    return 0;
}


int RVCALLCONV cmEvChannelSendMessage(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvPvtNodeId         message)
{
    EpChannelObj *chan = (EpChannelObj *)haChan;
    EpCallObj *   call = chan->call;
    EpObj *       ep = call->ep;
    RvBool        bInfo = RV_FALSE; 
    int           msdStatus;

#ifdef USE_FWNAT
    RvBool        bkeepAliveNeeded = RV_FALSE;
#endif
    RV_UNUSED_ARG(hsChan);
    RV_UNUSED_ARG(bInfo);

    lepLockCall(call);

    /* Add H.460.19 information to nodeId */

#ifdef USE_FWNAT
    if (ep->cfg.fwNatConfig.e19Config == RvH323FwNatH46019ConfigServer && cmChannelIsDuplex(chan->hsChan) == RV_TRUE)
    {
        
        RvBool        origin;
        cmChannelGetOrigin(chan->hsChan, &origin);
        if (!origin)
            bkeepAliveNeeded = RV_TRUE;
    }
#endif
    msdStatus = epFwNatAddH46019Info2Message(call, chan, bkeepAliveNeeded, NULL, message, RV_FALSE, (cmChannelDirection)-1);

    lepUnlockCallForAPI(call, msdStatus = cmCallMasterSlaveStatus(call->hsCall)); 

    if (msdStatus == 2)
    {
        char * msgName = cmGetProtocolMessageName(ep->hApp, message);
        if ((strcmp(msgName, "openLogicalChannel") == 0) ||
            (strcmp(msgName, "openLogicalChannelAck") == 0))
        {
            epSecurityChannelAddEncryptionSync(call, message);
        }
    }

    lepUnlockCall(call);

    if ((chan != NULL) && chan->call->ep->cfg.bScanNodeIDs)
        epScanPvtNode(chan->call->ep, message);

    return 0;
}


int RVCALLCONV cmEvChannelMasterSlaveConflict(
    IN      HAPPCHAN                    haChan,
    IN      HCHAN                       hsChan,
    IN      cmH245ChannelConflictType   conflictType,
    IN      HCHAN                      *confChans,
    IN      int                         numConfChans,
    IN      RvPvtNodeId                 message)
{
    EpChannelObj *chan = (EpChannelObj *)haChan;
    HPVT hVal;
    RvPvtNodeId dataTypeId;
    RvPstFieldId incomingType, outgoingType;
    int i;

    hVal = chan->call->ep->hVal;

    if (chan->call->ep->cfg.bScanNodeIDs)
        epScanPvtNode(chan->call->ep, message);

    epUtilsEvent(chan->call->ep, "H245", chan->call, "cmEvChannelMasterSlaveConflict.%d.%d.%d",
        chan->id, conflictType, numConfChans);

    if (conflictType != cmH245ConflictUnansweredProposals)
        return RV_ERROR_UNKNOWN;

    if (!chan->call->ep->cfg.bIgnoreMasterSlaveConflicts)
        return RV_ERROR_UNKNOWN; /* Conflicts are not ignored... */

    /* get the data type of the channel */
    cmChannelGetParam(hsChan, cmChannelParamDataTypeNodeId, &dataTypeId, NULL);
    incomingType = getDataTypeFieldId(hVal, dataTypeId);

    if (incomingType < 0)
        return 0;

    /* we try to find an outgoing channel of the same data type */
    for (i = 0; i < numConfChans; i++)
    {
        cmChannelGetParam(confChans[i], cmChannelParamDataTypeNodeId, &dataTypeId, NULL);
        outgoingType = getDataTypeFieldId(hVal, dataTypeId);
        if (outgoingType == incomingType)
        {
            HCHAN hsChan2 = NULL;

            cmChannelSameSession(confChans[i], NULL, &hsChan2);
            if (hsChan2 == NULL)
                return cmChannelSessionId(confChans[i]);
        }
    }

    /* found nothing. just use default answer */
    return 0;
}






/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * epChannelCreate
 * ----------------------------------------------------------------------------
 * General: Create a new channel object to use later on.
 *
 * Return Value: Channel object created.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep                   - Endpoint object to use.
 *         call                 - Call this channel belongs to.
 *         id                   - Id to give to the new channel.
 * Output: None.
 *****************************************************************************/
EpChannelObj *epChannelCreate(
    IN EpObj        *ep,
    IN EpCallObj    *call,
    IN RvInt32      id)
{
    return channelCreate(ep, call, id, NULL);
}


/******************************************************************************
 * epChannelOpen
 * ----------------------------------------------------------------------------
 * General: Open a new outgoing channel.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  channel              - Channel object to open.
 *         dataTypeName         - Name of data type to use for this channel.
 *         sameSessionChannel   - Incoming channel with the same session Id.
 *                                Can be NULL.
 *         replacementChannel   - Outgoing channel this one replaces.
 *                                Can be NULL.
 *         rate                 - Rate of the channel.
 * Output: None.
 *****************************************************************************/
RvStatus epChannelOpen(
    IN EpChannelObj *channel,
    IN const RvChar *dataTypeName,
    IN EpChannelObj *sameSessionChannel,
    IN EpChannelObj *replacementChannel,
    IN RvUint32     rate)
{
    EpCallObj *call;
    EpObj *ep;
    RvStatus status;
    HCHAN hsSameSessionChannel = NULL;

    call = channel->call;
    ep = call->ep;

    if (replacementChannel != NULL)
        cmChannelReplace(channel->hsChan, replacementChannel->hsChan);
    if (sameSessionChannel != NULL)
    {
        hsSameSessionChannel = sameSessionChannel->hsChan;

        if (dataTypeName == NULL)
        {
            /* Channel is mimiced */
            lepLockCall(call);
            channel->rtpSession = sameSessionChannel->rtpSession;
            channel->multiplexID = sameSessionChannel->multiplexID;
            channel->remoteMultiplexID = sameSessionChannel->remoteMultiplexID;
            epRtpSessionSetAction(ep->rtp, channel->rtpSession, call->action);
            epRtpSessionJoin(ep->rtp, channel->rtpSession);
            lepUnlockCall(call);
        }
    }

    /* Open an RTP session for this channel */
    lepLockCall(call);
    if ((call->action == epRtpActionReplay) && (channel->rtpSession < 0))
    {
        RvBool isMultiplexed;
        RvUint32 muxId=(RvUint32)-1;
        RV_UNUSED_ARG(muxId);
        
        if (!(isMultiplexed = epFwNatIsCallMultiplexed(call)))
        {
            channel->rtpSession = epRtpSessionNew(call, RV_FALSE, &muxId);
        }
        else
        {
            epFwNatCreateChannelMuxedRtpSession(channel);
        }
    }
    lepUnlockCall(call);

    /* Set RTCP address  and open the channel */
    if ((dataTypeName != NULL) && (dataTypeName[0] == 't'))
    {
        /* Data channel (Duplex) */
        cmTransportAddress ta;
        cmGetLocalCallSignalAddress(ep->hApp, &ta);
        epChannelSetDuplex(channel, &ta);
    }
    else
        cmChannelSetRTCPAddress(channel->hsChan, 0, (RvUint16)(epRtpSessionGetLocalPort(ep->rtp, channel->rtpSession) + 1));

    if (dataTypeName != NULL)
    {
        /* set data type */
        strncpy(channel->dataTypeName, dataTypeName, sizeof(channel->dataTypeName));
        channel->dataTypeName[sizeof(channel->dataTypeName)-1] = '\0';

        status = cmChannelOpen(channel->hsChan, dataTypeName, hsSameSessionChannel, NULL, rate);
    }
    else
    {
        RvPvtNodeId dataTypeId = RV_PVT_INVALID_NODEID;

        strcpy(channel->dataTypeName, sameSessionChannel->dataTypeName);

        lepLockCall(call);
        if (sameSessionChannel->dynPayloadType != 0)
            cmChannelSetDynamicRTPPayloadType(channel->hsChan, sameSessionChannel->dynPayloadType);
        lepUnlockCall(call);

        cmChannelGetParam(hsSameSessionChannel, cmChannelParamDataTypeNodeId, (RvInt32 *)&dataTypeId, NULL);
        dataTypeId = pvtChild(ep->hVal, dataTypeId);
        status = cmChannelOpenDynamic(channel->hsChan, dataTypeId, hsSameSessionChannel, NULL, RV_FALSE);
    }

    epUtilsLog(ep, call, "ChannelOpen (%s). result=%s", channel->dataTypeName, Status2String(status));

    if (status > 0)
        status = RV_OK;

    return status;
}


/******************************************************************************
 * epChannelMimic
 * ----------------------------------------------------------------------------
 * General: Mimic an incoming channel.
 *
 * Return Value: Channel object created.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  incomingChannel      - Incoming channel to mimic.
 *         id                   - Id to assign to the new outgoing channel.
 *                                0 means it will be allocated by the usual
 *                                resource id callback.
 * Output: None.
 *****************************************************************************/
EpChannelObj *epChannelMimic(
    IN EpChannelObj *incomingChannel,
    IN RvInt32      id)
{
    EpChannelObj *channel;
    EpCallObj *call;
    HCHAN hsSameSession = NULL;

    if (cmChannelIsDuplex(incomingChannel->hsChan))
        return NULL;

    /* Make sure we're not in an endless loop of mimicing channels */
    cmChannelSameSession(incomingChannel->hsChan, NULL, &hsSameSession);
    if (hsSameSession != NULL)
        return NULL;

    call = incomingChannel->call;

    channel = epChannelCreate(call->ep, call, id);
    if (channel == NULL)
        return NULL;

    if (epChannelOpen(channel, NULL, incomingChannel, NULL, 0) == RV_OK)
        return channel;

    return NULL;
}


/******************************************************************************
 * epChannelRespond
 * ----------------------------------------------------------------------------
 * General: Send a response for an incoming open logical channel
 *          or to an outgoing channel's requestChannelClose message.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  channel  - Incoming or outgoing channel to respond to.
 *         confirm  - RV_TRUE to confirm, RV_FALSE to reject.
 *         reason   - Reason to use for rejection.
 * Output: None.
 *****************************************************************************/
RvStatus epChannelRespond(
    IN EpChannelObj         *channel,
    IN RvBool               confirm,
    IN cmRejectLcnReason    reason)
{
    RvStatus status;

    if (channel->isOutgoing)
    {
        if (confirm)
        {
            /* After this, we need to call cmChannelDrop() to drop it... */
            status = cmChannelAnswer(channel->hsChan);
        }
        else
        {
            status = cmChannelRequestCloseReject(channel->hsChan);
        }
    }
    else
    {
        if (confirm)
        {
            EpObj *ep = channel->call->ep;
            RvUint16 port;
            RV_UNUSED_ARG(ep);

            lepLockCall(channel->call);
            port = (RvUint16)epRtpSessionGetLocalPort(ep->rtp, channel->rtpSession);
            lepUnlockCall(channel->call);

            cmChannelSetAddress(channel->hsChan, 0, port);
            cmChannelSetRTCPAddress(channel->hsChan, 0, (RvUint16)(port + 1));

            status = cmChannelAnswer(channel->hsChan);
        }
        else
        {
            status = cmChannelRejectReason(channel->hsChan, reason);
        }
    }

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epChannelDrop
 * ----------------------------------------------------------------------------
 * General: Drop a channel. This supports both incoming and outgoing channels.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  chan                 - Channel to drop.
 *         dropReason           - Reason for channel dropping.
 * Output: None.
 *****************************************************************************/
RvStatus epChannelDrop(
    IN EpChannelObj     *chan,
    IN cmCloseLcnReason dropReason)
{
    RvStatus status;
    const RvChar *dirStr;

    if (chan->isOutgoing)
    {
        status = cmChannelDropReason(chan->hsChan, dropReason);
        dirStr = "Outgoing";
    }
    else
    {
        status = cmChannelRequestClose(chan->hsChan, dropReason, NULL);
        dirStr = "Incoming";
    }

    if (status < 0)
        epUtilsLog(chan->call->ep, chan->call, "%s channel drop failed", dirStr);

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epChannelClose
 * ----------------------------------------------------------------------------
 * General: Close an idle channel and dispose all of its used memory.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep                   - Endpoint object used.
 *         chan                 - Channel to close.
 * Output: None.
 *****************************************************************************/
RvStatus epChannelClose(
    IN EpObj            *ep,
    IN EpChannelObj     *chan)
{
    EpCallObj *call;
    EpChannelObj *prev;
    EpChannelObj *next;
    int status;
    RvBool isMultiplexed = RV_FALSE;

    call = chan->call;
    lepLockCall(call);

    prev = chan->prevChannel;
    next = chan->nextChannel;

    if (chan->keepAliveTimer != HTI_NULL_TIMER)
        status = mtimerReset(ep->hTimers, chan->keepAliveTimer);

    lepLockEpForHash(ep, epHashRemove(ep->channels, chan->id));

    if (call != NULL)
    {
        if (call->numOfChannels >= 0)
            call->numOfChannels--;

        /* Fix call's channels list */
        if (call->firstChannel == chan)
            call->firstChannel = next;
        if (call->lastChannel == chan)
            call->lastChannel = prev;

        /* Fix linkage between neighbor channels */
        if (prev != NULL)
            prev->nextChannel = next;
        if (next != NULL)
            next->prevChannel = prev;
    }
    chan->call = NULL;
    isMultiplexed = (chan->multiplexID != (RvUint32)-1);
    /* Close the RTP session */
    epRtpSessionClose(ep->rtp, chan->rtpSession, isMultiplexed);
    lepUnlockCall(call);

    /* Close the channel in the CM */
    if (chan->hsChan != NULL)
    {
        status = cmChannelClose(chan->hsChan);
        chan->hsChan = NULL;
    }
    else
        status = 0;

    if (status > 0)
        status = RV_OK;
    return status;
}


/******************************************************************************
 * epChannelSetDuplex
 * ----------------------------------------------------------------------------
 * General: Set a channel as duplex.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  channel  - Channel object to use.
 *         address  - Optional address to set for the duplex channel.
 * Output: None.
 *****************************************************************************/
RvStatus epChannelSetDuplex(
    IN EpChannelObj         *channel,
    IN cmTransportAddress   *addr)
{
    RvStatus status;

    status = cmChannelDuplex(channel->hsChan);
    if (status > 0)
        status = RV_OK;

    if ((status == RV_OK) && (addr != NULL))
    {
        status = cmChannelSetDuplexAddress(channel->hsChan, *addr, 1, "1", RV_FALSE);
        if (status > 0)
            status = RV_OK;
    }

    return status;
}


/******************************************************************************
 * epChannelMediaLoopRequest
 * ----------------------------------------------------------------------------
 * General: Request a media loop on a channel.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  channel  - Channel object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epChannelMediaLoopRequest(
    IN EpChannelObj         *channel)
{
    RvStatus status;

    epUtilsEvent(channel->call->ep, "Rec", NULL, "mediaLoopRequest chanEntry= %d", channel->id);

    status = cmChannelMediaLoopRequest(channel->hsChan);
    if (status > 0)
        status = RV_OK;

    return status;
}


/******************************************************************************
 * epChannelSetRate
 * ----------------------------------------------------------------------------
 * General: Set the rate of a channel.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  channel  - Channel object to use.
 *         rate     - Rate to use.
 * Output: None.
 *****************************************************************************/
RvStatus epChannelSetRate(
    IN EpChannelObj         *channel,
    IN RvUint32             rate)
{
    RvStatus status;

    epUtilsEvent(channel->call->ep, "Rec", NULL, "setRate chanEntry= %d rate= %d", channel->id, rate);

    status = cmChannelFlowControl(channel->hsChan, rate);
    if (status > 0)
        status = RV_OK;

    return status;
}


/******************************************************************************
 * epChannelVideoFastUpdate
 * ----------------------------------------------------------------------------
 * General: Request for a fast update of the video channel.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  channel  - Channel object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epChannelVideoFastUpdate(
    IN EpChannelObj         *channel)
{
    RvStatus status;

    epUtilsEvent(channel->call->ep, "Rec", NULL, "videoFastUpdatePicture chanEntry= %d", channel->id);

    status = cmChannelVideoFastUpdatePicture(channel->hsChan);
    if (status > 0)
        status = RV_OK;

    return status;
}


/******************************************************************************
 * epChannelGetById
 * ----------------------------------------------------------------------------
 * General: Find a channel object by its id.
 *
 * Return Value: The channel object if found, NULL otherwise.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep       - Endpoint object to use.
 *         id       - Id of the channel to look for.
 * Output: None.
 *****************************************************************************/
EpChannelObj *epChannelGetById(
    IN EpObj    *ep,
    IN RvInt32  id)
{
    EpChannelObj *chan;

    lepLockEpForHash(ep, chan = (EpChannelObj *)epHashGet(ep->channels, id));

    return chan;
}







/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * channelCreate
 * ----------------------------------------------------------------------------
 * General: Create a complete channel object.
 *
 * Return Value: Channel object created.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep       - Endpoint object to use.
 *         call     - Call this channel belongs to.
 *         id       - Id to give to the new channel. If 0, will be allocated by
 *                    the allocation resource callback.
 *         hsChan   - Channel handle to use, or NULL for newly created outgoing
 *                    channels.
 * Output: None.
 *****************************************************************************/
static EpChannelObj *channelCreate(
    IN EpObj        *ep,
    IN EpCallObj    *call,
    IN RvInt32      id,
    IN HCHAN        hsChan)
{
    RvStatus status;
    EpChannelObj *lastChannel;
    EpChannelObj *chan;

    /* Ask for an ID for this call */
    if (id <= 0)
    {
        lepLockEpForHash(ep, id = ep->cb.epAllocateResourceId(ep, EpResourceChannel));
        if (id <= 0)
        {
            /* Channel was rejected ??? */
            return NULL;
        }
    }

    /* Allocate a call object */
    lepLockEpForHash(ep, status = epHashAdd(ep->channels, id, (void**)&chan));
    if (status != RV_OK)
    {
        /* No internal resources ??? */
        return NULL;
    }

    if (hsChan == NULL)
    {
        status = cmChannelNew(call->hsCall, (HAPPCHAN)chan, &hsChan);
        if (status < 0)
        {
            lepLockEpForHash(ep, epHashRemove(ep->channels, id));
            return NULL;
        }
    }

    memset(chan, 0, sizeof(*chan));
    chan->call = call;
    chan->id = id;
    chan->hsChan = hsChan;
    chan->isIndependent = call->isIndependent;
    chan->rtpSession = -1;
    chan->multiplexID = (RvUint32)-1;
    chan->remoteMultiplexID = (RvUint32)-1;
    chan->keepAliveTimer = HTI_NULL_TIMER;
    chan->state = (cmChannelState_e)-1;
    chan->stateMode = (cmChannelStateMode_e)-1;

    /* Connect this channel to the list of current channels for the call */
    lepLockCall(call);
    lastChannel = call->lastChannel;
    call->numOfChannels++;
    if (lastChannel == NULL)
    {
        call->firstChannel = chan;
        call->lastChannel = chan;
    }
    else
    {
        lastChannel->nextChannel = chan;
        chan->prevChannel = lastChannel;
        call->lastChannel = chan;
    }
    lepUnlockCall(call);

    cmChannelGetOrigin(chan->hsChan, &chan->isOutgoing);

    chan->mark = call->firstChannel->mark;

    return chan;
}


/******************************************************************************
 * getDataTypeFieldId
 * ----------------------------------------------------------------------------
 * General: Get the field id of the given data type we're dealing with.
 *
 * Return Value: Field ID of the data type.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hVal         - PVT handle used.
 *         dataTypeId   - DataType as known by the channel.
 * Output: None.
 *****************************************************************************/
static RvPstFieldId getDataTypeFieldId(
    IN HPVT         hVal,
    IN RvPvtNodeId  dataTypeId)
{
    RvPstFieldId fieldId=-1;
    RvPvtNodeId dataId;

    dataId = pvtChild(hVal, dataTypeId);
    if (dataId < 0)
        return RV_ERROR_UNKNOWN;

    pvtGet(hVal, dataId, &fieldId, NULL, NULL, NULL);
    switch(fieldId)
    {
    case __h245(nonStandard):
    case __h245(nullData):
        break;
    case __h245(videoData):
    case __h245(audioData):
        dataId = pvtChild(hVal, dataId);
        break;
    case __h245(data):
        dataId = pvtChild(hVal, pvtChild(hVal, dataId));
        break;
    default:
        return RV_ERROR_UNKNOWN;
    }

    /* Found a supported data type */
    pvtGet(hVal, dataId, &fieldId, NULL, NULL, NULL);
    return fieldId;
}







#ifdef __cplusplus
}
#endif
