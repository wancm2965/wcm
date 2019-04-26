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

#ifndef _RV_EP_FWNAT_H_
#define _RV_EP_FWNAT_H_

/***********************************************************************
epFwNat.h

FW/NAT traversal add-on module of the endpoint.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/




#ifdef __cplusplus
extern "C" {
#endif



/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/*                           CALLBACK HEADERS                            */
/*-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/

#ifdef USE_FWNAT

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
    IN  EpObj           *ep);


/******************************************************************************
 * epFwNatInit
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
    IN  EpObj           *ep);


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
RvStatus epFwNatEnd(IN  EpObj * ep);


/******************************************************************************
 * epFwNatVersion
 * ----------------------------------------------------------------------------
 * General: Get the version string of the H.460.FANTAS/MA/17 module used.
 *
 * Return Value: Version string.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
const RvChar *epFwNatVersion(IN  EpObj * ep);


/******************************************************************************
 * epFwNatCallEnd
 * ----------------------------------------------------------------------------
 * General: Clean up FW/NAT related information.
 *
 * Return Value: Non-negative value on success, Negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         call             - Call object to use.
 * Output: None.
 *****************************************************************************/
void epFwNatCallEnd(
    IN  EpObj * ep,
    IN  EpCallObj * call);

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
    IN  EpCallConfig *cfg);

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
        OUT RvBool *       bInfo);

/******************************************************************************
 * epFwNatCallEnd
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
      OUT RvBool *      bInfo);

/********************************************************************************
 * Function Name: epFwNatEvRASMessageSend
 * Description: May add FwNat information to the ras message.
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
    OUT RvBool *            bInfo);

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
        IN cmChannelDirection    channelDirection);

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
        IN cmChannelDirection   chanDir);

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
    IN EpCallObj *  call);

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
    IN EpChannelObj *  chan);

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
      IN EpCallObj * call);

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
        IN      RvBool              error);

/********************************************************************************
 * Name: epFwNatHookClose
 * Description:  This function should be called from hookClose().
 *              Allows the FW/NAT module to free related resources.
 * Input:   hConn        - Stack's handle for the connection.

 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
int RVCALLCONV epFwNatHookClose(
        IN      HPROTCONN           hConn);

/********************************************************************************
 * Function Name: epFwNatHookRecv
 * Description: This function should be called from hookRecv().
 *              It is used to extract FwNat information from the Q.931 messages.
 *
 * Input:   hFwNat          - FwNat module handle
 *          nodeId          - Stack's handle for the RAS transaction
 *          hConn           - Stack's handle for the connection.
 *          error           -
 * Return: RV_TRUE if message should be ignored by the stack 
 ********************************************************************************/
RvBool RVCALLCONV epFwNatHookRecv(
    IN      HPROTCONN           hConn,
    IN      int                 nodeId,
    IN      RvBool              error);

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
    IN      RvBool              error);

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
        IN      RvInt32             messageLen);

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
        IN      cmTransportAddress* addrTo);

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
        IN       HRAS    hsRas);

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
        IN       HRAS    hsRas);

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
    IN RvInt32           rtpSession);


#else


#define epFwNatInit(_ep) RV_OK
#define epFwNatConfigure(_ep) RV_ERROR_NOTSUPPORTED
#define epFwNatEnd(_ep) RV_OK
#define epFwNatVersion(_ep) "0"
#define epFwNatCallEnd(_ep, _call)
#define epFwNatSetCallH245Stage(_call, _cfg)
#define epFwNatCallSendMessage(_ep, _call, _message, _bInfo) RV_OK
#define epFwNatCallRecvMessage(_ep, _call, _message, _bInfo) RV_OK
#define epFwNatEvRASMessageSend(_ep, _hsRas, _rasTrStage, _messageNodeId, _bStop, _bInfo) RV_OK
#define epFwNatAddH46019Info2Message(_call, _channel, _bKeepAliveNeeded, _fsChan, _message, _bIsFastStart, _channelDirection) RV_OK
#define epFwNatGetH46019InfoFromMessage(_call, _channel, _bKeepAliveExpected, _message, fsChannel, _sessionId, _bIsFaststart, _chanDir) RV_OK
#define epFwNatCreateCallMuxedRtpSessions(_call)
#define epFwNatCreateChannelMuxedRtpSession(_channel)
#define epFwNatIsCallMultiplexed(_call) RV_FALSE
#define epFwNatHookOutConn(_hConn, _addrFrom, _addrTo, _error) RV_OK
#define epFwNatHookClose(_hConn) RV_OK
#define epFwNatHookRecv(_hConn, _nodeId, _error) RV_FALSE
#define epFwNatHookRecvFrom(_hConn, _nodeId,  _addrFrom, multicast, _error) RV_FALSE
#define epFwNatHookSendRaw(_hConn, _rawMessage, _messageLen) RV_FALSE
#define epFwNatHookSendRawTo(_hConn, _rawMessage, _messageLen, _addrTo) RV_FALSE
#define epFwNatEvRASRequest(_hsRas) RV_FALSE
#define epFwNatEvAutoRASRequest(_hsRas) RV_FALSE
#define epFwNatSendKeepAlive(_call, _origin, _rtpSession);

#endif /*USE_FWNAT*/


#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_FWNAT_H_ */
