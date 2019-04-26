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

#ifndef _RV_CMHOOK_H
#define _RV_CMHOOK_H

#ifdef __cplusplus
extern "C" {
#endif

#define CM_PROTOCOL_RAW_MESSAGE_HEADER_SIZE (sizeof(void*)+4)


typedef RvBool
    (RVCALLCONV *cmHookListenT)(/*Before listen*/
        IN      HPROTCONN           hConn,
        IN      int                 addr
        );


typedef int
    (RVCALLCONV *cmHookListeningT)(/*After listen*/
        IN      HPROTCONN           hConn,
        IN      int                 addr,
        IN      RvBool              error
        );

typedef int
    (RVCALLCONV*cmHookConnectingT)(/*Before connect*/
        IN      HPROTCONN           hConn,
        IN      int                 addr
        );

typedef int
    (RVCALLCONV*cmHookInConnT)(/*Incomming connect*/
        IN      HPROTCONN           hConn,
        IN      int                 addrFrom,
        IN      int                 addrTo
        );
typedef int
    (RVCALLCONV*cmHookOutConnT)(/*Outgoing connect*/
        IN      HPROTCONN           hConn,
        IN      int                 addrFrom,
        IN      int                 addrTo,
        IN      RvBool              error
        );
typedef RvBool
    (RVCALLCONV*cmHookSendT)(
        IN      HPROTCONN           hConn,
        IN      int                 nodeId,
        IN      RvBool              error
        );
typedef RvBool
    (RVCALLCONV*cmHookRecvT)(
        IN      HPROTCONN           hConn,
        IN      int                 nodeId,
        IN      RvBool              error
        );

typedef RvBool
    (RVCALLCONV*cmHookSendToT)(
        IN      HPROTCONN           hConn,
        IN      int                 nodeId,
        IN      int                 addrTo,
        IN      RvBool              error
        );
typedef RvBool
    (RVCALLCONV*cmHookRecvFromT)(
        IN      HPROTCONN           hConn,
        IN      int                 nodeId,
        IN      int                 addrFrom,
        IN      RvBool              multicast,
        IN      RvBool              error
        );

typedef void
    (RVCALLCONV*cmHookCloseT)(
        IN      HPROTCONN           hConn
        );
typedef RvBool
    (RVCALLCONV*cmHookSendRawT)(
        IN      HPROTCONN           hConn,
        IN      const RvUint8 *     rawMessage,
        IN      RvInt32             messageLen
        );
typedef RvBool
    (RVCALLCONV*cmHookSendRawToT)(
        IN      HPROTCONN           hConn,
        IN      const RvUint8 *     rawMessage,
        IN      RvInt32             messageLen,
        IN      cmTransportAddress* addrTo
        );


typedef  struct
{
   cmHookListenT      hookListen;
   cmHookListeningT   hookListening;
   cmHookConnectingT  hookConnecting;
   cmHookInConnT      hookInConn;
   cmHookOutConnT     hookOutConn;
   cmHookSendT        hookSend;
   cmHookRecvT        hookRecv;
   cmHookSendToT      hookSendTo;
   cmHookRecvFromT    hookRecvFrom;
   cmHookCloseT       hookClose;
   cmHookSendRawT     hookSendRaw;
   cmHookSendRawToT   hookSendRawTo;
} SCMPROTOCOLEVENT,*CMPROTOCOLEVENT;


typedef enum
{
    cmProtocolUnknown,
    cmProtocolRAS,
    cmProtocolQ931,
    cmProtocolH245
} cmProtocol;


typedef enum {
    cmProtConnParamType,
    cmProtConnParamIncoming,
    cmProtConnParamState,
    cmProtConnParamMultiplex,
    cmProtConnParamMaintain,
    cmProtConnParamNumCalls,
    cmProtConnParamTpkt,
} cmProtConnParam;


typedef enum {
    cmProtConnStateIdle,
    cmProtConnStateListening,
    cmProtConnStateConnecting,
    cmProtConnStateListeningConnecting,
    cmProtConnStateConnected,
    cmProtConnStateBusy,
    cmProtConnStateClosing,
    cmProtConnStateClosed
} cmProtConnState;


typedef void (*hostEventHandler)(void * tpkt, int event, int length, int error, void*context);


/************************************************************************
 * cmSetProtocolEventHandler
 * purpose: Installs the protocol event handler.
 * input  : hApp            - Stack handle for the application
 *          cmProtocolEvent - Pointer to array of pointers to general event callback functions
 *          size            - Size of *cmProtocolEvent in bytes
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmSetProtocolEventHandler(
        IN  HAPP            hApp,
        IN  CMPROTOCOLEVENT cmProtocolEvent,
        IN  int             size);

RVAPI
int RVCALLCONV cmSetProtocolEventHandlerReplacement(
        IN  HAPP            hApp,
        IN  CMPROTOCOLEVENT cmProtocolEventReplacement,
        IN  int             size);


/******************************************************************************
 * cmProtocolSendMessage
 * ----------------------------------------------------------------------------
 * General: Send a message through a given connection
 *          This function should be used for Q931 and H.245 messages.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp     - Stack handle for the application.
 *         hConn    - Connection to use.
 *         msg      - Node id of the message to send.
 *                    The message is not deleted by the call to this function.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolSendMessage(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvPvtNodeId msg);


/******************************************************************************
 * cmCallSendMessageExt
 * ----------------------------------------------------------------------------
 * General: Send a message for a call's connection
 *          This function should be used for Q931 and H.245 messages, and
 *          allows sending of tunnled messages.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hCall    - Stack handle for the call.
 *         msgType  - Type of message, Q.931 or H.245
 *         msgNode  - Node id of the message to send.
 *                    The message is not deleted by the call to this function.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmCallSendMessageExt(
        IN  HCALL                   hCall,
        IN  cmTpktChanHandleType    msgType,
        IN  RvPvtNodeId             msgNode);


/******************************************************************************
 * cmProtocolSendMessageTo
 * ----------------------------------------------------------------------------
 * General: Send a RAS message to a given remote address.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp     - Stack handle for the application.
 *         hConn    - Connection to use.
 *         msg      - Node id of the message to send.
 *                    The message is not deleted by the call to this function.
 *         addr     - Node id of TransportAddress type, indicating the
 *                    remote address to send the message to.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolSendMessageTo(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvPvtNodeId msg,
        IN  RvPvtNodeId addr);


/******************************************************************************
 * cmProtocolSendRawMessage
 * ----------------------------------------------------------------------------
 * General: Send a raw message through a given connection.
 *          This function should be used for Q931 and H.245 messages. It allows
 *          sending malformed messages to remote clients and is useful for
 *          testing tools development.
 *          Raw messages are packetized using TPKT as any other Q931 and H.245
 *          message.
 *          Note: The raw message must have a clean header of
 *          CM_PROTOCOL_RAW_MESSAGE_HEADER_SIZE bytes at its beginning for the
 *          internal use of the Stack. The bufferSize value must include this
 *          additional header size in its calculation.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp         - Stack handle for the application.
 *         hConn        - Connection to use.
 *         buffer       - Buffer to send as the message.
 *         bufferSize   - Size of buffer to send.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolSendRawMessage(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvUint8     *buffer,
        IN  RvSize_t    bufferSize);


/******************************************************************************
 * cmProtocolRecvRawMessage
 * ----------------------------------------------------------------------------
 * General: Receives a raw message through a given connection.
 *          This function should be used for Q931 messages only. It allows
 *          re-receiving of messages rejected by H.235.
 *          Raw messages are packetized using TPKT as any other Q931 and H.245
 *          message.
 *          Note: The raw message must have a clean header of
 *          CM_PROTOCOL_RAW_MESSAGE_HEADER_SIZE bytes at its beginning for the
 *          internal use of the Stack. The bufferSize value must include this
 *          additional header size in its calculation.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp         - Stack handle for the application.
 *         hConn        - Connection to use.
 *         buffer       - Buffer to send as the message.
 *         bufferSize   - Size of buffer to send.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolRecvRawMessage(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvUint8     *buffer,
        IN  RvSize_t    bufferSize);


/******************************************************************************
 * cmProtocolRecvRawMessageFrom
 * ----------------------------------------------------------------------------
 * General: Simulate receiving a raw message on a given RAS connection.
 *          This can be used for testing or for support of listening to several
 *          different RAS addresses (since the Stack does not support this
 *          feature).
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * See Also: cmGetUdpChanHandle, cmGetRASConnectionHandle.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp         - Stack handle for the application.
 *         hConn        - Connection to use (can be the unicast or the
 *                        multicast RAS connection handle).
 *         buffer       - Buffer of received message.
 *         bufferSize   - Size of buffer.
 *         fromAddress  - Address this message is "received" from.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolRecvRawMessageFrom(
        IN  HAPP                hApp,
        IN  HPROTCONN           hConn,
        IN  RvUint8             *buffer,
        IN  RvSize_t            bufferSize,
        IN  cmTransportAddress  *fromAddress);




/* Creates new message value tree*/
RVAPI int /* New message nodeId*/
RVCALLCONV cmProtocolCreateMessage(
                IN      HAPP                    hApp,
                IN      cmProtocol              protocol);

RVAPI/* returns protocol of the message tree*/
cmProtocol RVCALLCONV cmProtocolGetProtocol(
                IN      HAPP                    hApp,
                IN      int                     msg);/* message nodeId*/


RVAPI
char* RVCALLCONV cmProtocolGetProtocolName(
                IN      cmProtocol              protocol);


/************************************************************************
 * cmProtocolGetLocalAddress
 * purpose: Get the local address of a given connection of the stack.
 *          This function can be used on Q.931 or H.245 connections.
 * input  : hApp    - Stack handle for the application
 *          hConn   - Connection to use
 * output : address - The local address of this connection
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmProtocolGetLocalAddress(
        IN  HAPP                hApp,
        IN  HPROTCONN           hConn,
        OUT cmTransportAddress* address);


/************************************************************************
 * cmProtocolGetRemoteAddress
 * purpose: Get the remote address of a given connection of the stack.
 *          This function can be used on Q.931 or H.245 connections.
 * input  : hApp    - Stack handle for the application
 *          hConn   - Connection to use
 * output : address - The remote address of this connection
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmProtocolGetRemoteAddress(
        IN  HAPP                hApp,
        IN  HPROTCONN           hConn,
        OUT cmTransportAddress* address);


/************************************************************************
 * cmProtocolSetTypeOfService
 * purpose: Set the connection's DiffServ Code Point value. This is used
 *          for QoS.
 *          This function can be used on RAS, Q.931 or H.245 connections.
 * input  : hApp            - Stack handle for the application
 *          hConn           - Connection to use
 *          typeOfService   - Type of service value
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmProtocolSetTypeOfService(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvInt       typeOfService);


/************************************************************************
 * cmProtocolGetSocket
 * purpose: Get the underlying socket object of the operating system for
 *          a given connection object.
 * input  : hApp            - Stack handle for the application.
 *          hConn           - Connection to use.
 * output : socket          - Socket used for the connection.
 *                            The returned value should be type-casted to
 *                            the operating system specific socket type
 *                            (int for most operating systems, SOCKET
 *                            for Windows).
 * return : RV_OK on success, or a negative value on failure.
 ************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolGetSocket(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        OUT void        **socket);


/******************************************************************************
 * cmProtocolFindCallByMessage
 * ----------------------------------------------------------------------------
 * General: Get a call handle related to a given message.
 *          This function can be used from within hook functions to find out
 *          the call handle of a given message.
 *          This function will work only on Q931 messages.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         hConn            - Connection this message was received from (or
 *                            sent on).
 *         messageNodeId    - Q931 message to use.
 * Output: hsCall           - Call object related with the message.
 *                            If this function returns RV_OK, and this handle
 *                            is NULL, then the message is a general message
 *                            on the connection and is not related to any
 *                            specific call.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtocolFindCallByMessage(
    IN  HAPP                    hApp,
    IN  HPROTCONN               hConn,
    IN  RvPvtNodeId             messageNodeId,
    OUT HCALL                   *hsCall);


/******************************************************************************
 * cmProtConnNew
 * ----------------------------------------------------------------------------
 * General: Create a new TCP connection in the stack. the caller is responsible
 *          for the connection's termination.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         protocol         - Type of connection (Q.931 or H.245).
 *         bMultiServer     - should we listen as a multi-server?.
 *         eventHandler     - Connection event handler.
 *         context          - Event handler context.
 *         remoteAddr       - Address to connect to (NULL for listening).
 *         localAddr        - Local Address to use (set to zero for random).
 * Output: hConn            - Connection created.
 *         localAddr        - Local address used.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtConnNew(
    IN    HAPP                  hApp,
    IN    cmProtocol            protocol,
    IN    RvBool                bMultiServer,
    IN    hostEventHandler      eventHandler,
    IN    void *                context,
    IN    cmTransportAddress *  remoteAddr,
    INOUT cmTransportAddress *  localAddr,
    OUT   HPROTCONN *           hConn);


/******************************************************************************
 * cmProtConnGetParam
 * ----------------------------------------------------------------------------
 * General: Get a parameter from a connection.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         hConn            - Connection to get the parameter from.
 *         eParam           - Parameter to get.
 *         iValue           - The length of sValue.
 * Output: iValue           - The value or the length used of sValue.
 *         sValue           - Non-integer value.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtConnGetParam(
    IN    HAPP                  hApp,
    IN    HPROTCONN             hConn,
    IN    cmProtConnParam       eParam,
    INOUT RvInt32 *             iValue,
    OUT   RvUint8 *             sValue);


/******************************************************************************
 * cmProtConnSetParam
 * ----------------------------------------------------------------------------
 * General: Set a parameter for a connection.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         hConn            - Connection to set the parameter for.
 *         eParam           - Parameter to set.
 *         iValue           - The value to set or the length of sValue.
 *         sValue           - Non-integer value, or NULL.
 * Output: none.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtConnSetParam(
    IN    HAPP                  hApp,
    IN    HPROTCONN             hConn,
    IN    cmProtConnParam       eParam,
    IN    RvInt32               iValue,
    IN    RvUint8 *             sValue);


/******************************************************************************
 * cmProtConnRelinquish
 * ----------------------------------------------------------------------------
 * General: Relinquish a TCP connection to the default event handler of the stack.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         hConn            - Connection to relinquish.
 * Output: none.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtConnRelinquish(
    IN    HAPP                  hApp,
    IN    HPROTCONN             hConn);

    
/******************************************************************************
 * cmProtConnHandleEventByDefault
 * ----------------------------------------------------------------------------
 * General: Relayes an event to the default event handlers.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         hConn            - Connection to relinquish.
 *         event            - Event flag.
 * Output: none.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtConnHandleEventByDefault(
    IN    HAPP                  hApp,
    IN    HPROTCONN             hConn,
    IN    RvUint32              event);


/******************************************************************************
 * cmProtConnClose
 * ----------------------------------------------------------------------------
 * General: Destroy a TCP connection in the stack.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp             - Stack instance handle.
 *         hConn            - Connection created.
 * Output: none.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmProtConnClose(
    IN    HAPP                  hApp,
    IN    HPROTCONN             hConn);


RVAPI
HPROTCONN RVCALLCONV cmGetTpktChanHandle(
    IN HCALL                hsCall,
    IN cmTpktChanHandleType tpktChanHandleType);


RVAPI
HPROTCONN RVCALLCONV cmGetUdpChanHandle(IN HCALL hsCall,cmUdpChanHandleType  udpChanHandleType);


RVAPI
int RVCALLCONV cmSetUdpChanApplHandle(IN HPROTCONN hCon,HAPPCONN hAppConn);


RVAPI
int RVCALLCONV cmSetTpktChanApplHandle(
    IN HPROTCONN    hConn,
    IN HAPPCONN     hAppConn);


RVAPI
int RVCALLCONV cmGetUdpChanApplHandle(IN HPROTCONN hCon,HAPPCONN * hAppConn);


RVAPI
int RVCALLCONV cmGetTpktChanApplHandle(
    IN  HPROTCONN   hConn,
    OUT HAPPCONN*   hAppConn);


RVAPI
HCALL RVCALLCONV cmGetCallByH245Chan(
    IN HPROTCONN            hConn);



#ifdef __cplusplus
}
#endif

#endif  /* _RV_CMHOOK_H */


