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

#ifndef _RV_EP_CHANNEL_H_
#define _RV_EP_CHANNEL_H_

/***********************************************************************
epChannel.h

Channel handling of the endpoint.
Handles the channel procedures.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "lepDefs.h"




#ifdef __cplusplus
extern "C" {
#endif



/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/*                           CALLBACK HEADERS                            */
/*-----------------------------------------------------------------------*/
int RVCALLCONV cmEvCallNewChannel(
    IN  HAPPCALL   haCall,
    IN  HCALL      hsCall,
    IN  HCHAN      hsChan,
    OUT LPHAPPCHAN lphaChan);
int RVCALLCONV cmEvChannelStateChanged(
    IN      HAPPCHAN                haChan,
    IN      HCHAN                   hsChan,
    IN      cmChannelState_e        state,
    IN      cmChannelStateMode_e    stateMode);
int RVCALLCONV cmEvChannelNewRate(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvUint32            rate);
int RVCALLCONV cmEvChannelMaxSkew(
    IN      HAPPCHAN            haChan1,
    IN      HCHAN               hsChan1,
    IN      HAPPCHAN            haChan2,
    IN      HCHAN               hsChan2,
    IN      RvUint32            skew);
int RVCALLCONV cmEvChannelSetAddress(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvUint32            ip,
    IN      RvUint16            port);
int RVCALLCONV cmEvChannelSetRTCPAddress(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvUint32            ip,
    IN      RvUint16            port);
int RVCALLCONV cmEvChannelParameters(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      char*               channelName,
    IN      HAPPCHAN            haChanSameSession,
    IN      HCHAN               hsChanSameSession,
    IN      HAPPCHAN            haChanAssociated,
    IN      HCHAN               hsChanAssociated,
    IN      RvUint32            rate);
int RVCALLCONV cmEvChannelRTPDynamicPayloadType(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvInt8              dynamicPayloadType);
int RVCALLCONV cmEvChannelVideoFastUpdatePicture(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan);
int RVCALLCONV cmEvChannelVideoFastUpdateGOB(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      int                 firstGOB,
    IN      int                 numberOfGOBs);
int RVCALLCONV cmEvChannelVideoFastUpdateMB(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      int                 firstGOB,
    IN      int                 firstMB,
    IN      int                 numberOfMBs);
int RVCALLCONV cmEvChannelHandle(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      int                 dataTypeHandle,
    IN      cmCapDataType       dataType);
int RVCALLCONV cmEvChannelGetRTCPAddress(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvUint32*           ip,
    IN      RvUint16*           port);
int RVCALLCONV cmEvChannelRequestCloseStatus(
    IN      HAPPCHAN              haChan,
    IN      HCHAN                 hsChan,
    IN      cmRequestCloseStatus  status);
int RVCALLCONV cmEvChannelTSTO(
    IN      HAPPCHAN              haChan,
    IN      HCHAN                 hsChan,
    IN      RvInt8                isCommand,
    IN      RvInt8                tradeoffValue);
int RVCALLCONV cmEvChannelMediaLoopStatus(
    IN      HAPPCHAN              haChan,
    IN      HCHAN                 hsChan,
    IN      cmMediaLoopStatus     status);
int RVCALLCONV cmEvChannelReplace(
     IN     HAPPCHAN              haChan,
     IN     HCHAN                 hsChan,
     IN     HAPPCHAN              haReplacedChannel,
     IN     HCHAN                 hsReplacedChannel);
int RVCALLCONV cmEvChannelFlowControlToZero(
    IN      HAPPCHAN             haChan,
    IN      HCHAN                hsChan);
int RVCALLCONV cmEvChannelMiscCommand(
     IN      HAPPCHAN             haChan,
     IN      HCHAN                hsChan,
     IN      cmMiscellaneousCommand miscCommand);
int RVCALLCONV cmEvChannelTransportCapInd(
     IN      HAPPCHAN             haChan,
     IN      HCHAN                hsChan,
     IN      int                  nodeId);
int RVCALLCONV cmEvChannelSetNSAPAddress(
    IN      HAPPCHAN              haChan,
    IN      HCHAN                 hsChan,
    IN      char*                 address,
    IN      int                   length,
    IN      RvBool                multicast);
int RVCALLCONV cmEvChannelSetATMVC(
    IN      HAPPCHAN              haChan,
    IN      HCHAN                 hsChan,
    IN      int                   portNumber);
int RVCALLCONV cmEvChannelRecvMessage(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvPvtNodeId         message);
int RVCALLCONV cmEvChannelSendMessage(
    IN      HAPPCHAN            haChan,
    IN      HCHAN               hsChan,
    IN      RvPvtNodeId         message);
int RVCALLCONV cmEvChannelMasterSlaveConflict(
    IN      HAPPCHAN                    haChan,
    IN      HCHAN                       hsChan,
    IN      cmH245ChannelConflictType   conflictType,
    IN      HCHAN                      *confChans,
    IN      int                         numConfChans,
    IN      RvPvtNodeId                 message);





/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
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
    IN RvInt32      id);


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
    IN RvUint32     rate);


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
    IN RvInt32      id);


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
    IN cmRejectLcnReason    reason);


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
    IN cmCloseLcnReason dropReason);


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
    IN EpChannelObj     *chan);


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
    IN cmTransportAddress   *addr);


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
    IN EpChannelObj         *channel);


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
    IN RvUint32             rate);


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
    IN EpChannelObj         *channel);


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
    IN RvInt32  id);







#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_CHANNEL_H_ */
