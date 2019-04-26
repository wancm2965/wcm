/***********************************************************************************************
  Notice:
  This document contains information that is proprietary to RADVISION LTD..
  No part of this publication may be reproduced in any form whatsoever without
  written prior approval by RADVISION LTD..

    RADVISION LTD. reserves the right to revise this publication and make changes
    without obligation to notify any person of such revisions or changes.

*************************************************************************************************/
/********************************************************************************************
*                                h235_api.h
*
* This file contains all the API of the h235 add-on
*
*
*      Written by                        Version & Date                        Change
*     ------------                       ---------------                      --------
*      Dani Stettiner                     10-Jan-2001
*
********************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


#ifndef __FWNAT_H
#define __FWNAT_H

#include "fwnat_api.h"
#include "rvtimer.h"
#include "hash.h"
#include "ema.h"

/*================================================
 * GENERAL DEFINITIONS
 ===============================================*/


    
/*=============================================
 * TYPEDEFS
 =============================================*/

typedef enum {
    RvH323FwNatConnTypeRasEncapsulation,
    RvH323FwNatConnTypeReverseCallOutgoing,
    RvH323FwNatConnTypeReverseCallIncoming,
    RvH323FwNatConnTypeH245MultiHost
} RvH323FwNatConnType;

typedef struct {
    RvH323FwNatConnType connType; /* type (usage) of connection */
    HPROTCONN           protConn; /* handle to the stack's host */
    RvUint8             CID[16]; /* CID of the call */
    HCALL               hCall; /* handle to the call */
    RvTimer             *timer; /* termination timer */
    RvUint8             numRelatedCalls; /* # of related calls, used to issue a warning on close */
    HRAS                hsRas; /* the RAS message this connection was created for */
    void *              hashLoc;
} FwNatConn;

typedef struct {
    RvH323FwNatH46017State      e17State;           /* current H.460.17 status */
    RvH323FwNatH46018State      e18State;           /* current H.460.18 status */
    RvH323FwNatH46019State      e19State;           /* current H.460.19 status */
    RvUint8                     mediaMultiplexMode; /* multiplexing mode capability*/
    RvBool                      bGatekeeper;
    HAPP                        hApp; /* stack handle */
    HPST                        IncomingCallIndicationSyn; /*syntax of IncomingCallIndication*/
    HPST                        TraversalParamsSyn;         /*syntax of TraversalParameters*/
    FwNatConn *                 pGkConn; /* connection to the GK, if EP, NULL o.w. */
    HHASH                       hAddrHash; /* address for finding a connection from an address */
    cmTransportAddress          serverAddr; /* address of the TS server */
    HEMA                        hFwNatConns; /* EMA array containing the connection objects */
    RvH323FwNatCallbacks        callbacks;
    RvLogMgr*                   logMgr; /* Log manager used */
    RvLogSource                 log; /* Log to use for messages */
    RvPvtNodeId                 facilityMessageWithCID;  /*facility message for incoming call procedure*/
    RvPvtNodeId                 defEncapMsg; /* a basic facility message for encapsulation */
    RvSize_t                    encapSize; /* a size of the encapsulation buffer */
    RvUint8 *                   encapBuffer; /* a buffer to use for encapsulated messages */
    RvLock                      encapLock; /* a lock to protect the above buffer */
} FwNatMgr;


/*================================================
 * EVENT HANDLERS PROTOTYPES
 ===============================================*/


/*=============================================
 * Function prototypes
 ==============================================*/

/********************************************************************************
 * Type Name: RvH323FwNatDefaultSetProtConnHandleEv
 * Description: default callback, to use when the application does not set
 *              application handle for ProtConn elements,
 *
 * Input:   hApp         - stack application handle.
 *          hFwNat       - FwNat module handle
 *          hProtConn    - Stack's connection handle
 *          hFwNatConn   - The FwNat connection related to the connection
 *                         or NULL to unset.
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RVCALLCONV RvH323FwNatDefaultSetProtConnHandleEv(
    IN  HPROTCONN           hProtConn,
    IN  HH323FWNATCONN      hFwNatConn);


/********************************************************************************
 * Type Name: RvH323FwNatSetProtConnHandleEv_t (callback prototype)
 * Description: default callback, to use when the application does not set
 *              application handle for ProtConn elements,
 *
 * Input:   hApp         - stack application handle.
 *          hFwNat       - FwNat module handle
 *          hProtConn    - Stack's connection handle
 * Output:  hFwNatConn   - The FwNat connection related to the connection
 *                         or NULL to unset.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RVCALLCONV RvH323FwNatDefaultGetProtConnHandleEv(
    IN  HPROTCONN           hProtConn,
    OUT HH323FWNATCONN *    hFwNatConn);


/********************************************************************************
 * Function Name: RvH323FwNatBuildEncapsulationFacilityMessage
 * Description:  Create a default facility message to use for encapsulation.
 *
 * Input:   mgr          - FwNat module handle.
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RvH323FwNatBuildEncapsulationFacilityMessage(
    IN  FwNatMgr *      mgr);

/********************************************************************************
 * Function Name: RvH323FwNatEncapsulateMessage
 * Description:  Build a facility message encapsulating the given buffer.
 *
 * Input:   mgr          - FwNat module handle.
 *          rasMessage   - The message to be encapsulated.
 *          msgLen       - Message length.
 * Output:  facilityMessage - root on which the encapsulating message was built.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RvH323FwNatEncapsulateMessage(
    IN  FwNatMgr *      mgr,
    IN  const RvUint8 * rasMessage,
    IN  RvInt32         msgLen,
    OUT RvPvtNodeId     facilityMessage);

/********************************************************************************
 * Function Name: RvH323FwNatHasEncapsulatedMessage
 * Description:  Checks the generic data for encapsulated message identifiers.
 *
 * Input:   mgr          - FwNat module handle.
 *          msgNode      - Root of the message.
 * Output:  paramsNode   - node in which the encapsulation parameters are stored.
 * Return: RV_TRUE if there is an encapsulated message, RV_FALSE o.w.
 ********************************************************************************/
RvBool RvH323FwNatHasEncapsulatedMessage(
    IN  FwNatMgr *      mgr,
    IN  RvPvtNodeId     msgNode,
    OUT RvPvtNodeId *   paramsNode);

/********************************************************************************
 * Function Name: RvH323FwNatGetEncapsulatedMessage
 * Description:  Extracts the encapsulated message.
 *
 * Input:   mgr          - FwNat module handle.
 *          paramsNode   - Node in which the encapsulation parameters are stored.
 * Output:  message      - Pointer to a buffer containing the message
 *          msgLen       - Length of the message
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RvH323FwNatGetEncapsulatedMessage(
    IN  FwNatMgr *      mgr,
    IN  RvPvtNodeId     paramsNode,
    OUT RvUint8 * *     message,
    OUT RvInt32 *       msgLen);


/********************************************************************************
 * Function Name: RvH323FwNatConnectRasEncapsulation
 * Description:  Create RAS encapsulation connection.
 *
 * Input:   mgr          - FwNat module handle.
 *          remoteAddr   - Address to create a connection to.
 *          context      - Context to be raised with the connection event.
 * Output:  pFnConn      - Pointer to the created connection.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus RvH323FwNatConnectRasEncapsulation(
    IN  FwNatMgr *              mgr,
    IN  cmTransportAddress *    remoteAddr,
    IN  HRAS                    hsRas,
    OUT FwNatConn **            pFnConn);


/********************************************************************************
 * Function Name: RvH323FwNatGetEncapsulationConnection
 * Description:  Extracts the encapsulated message.
 *
 * Input:   mgr          - FwNat module handle.
 *          remoteAddr   - Address to check connection to.
 * Output:  none
 * Return: The encapsulation connection.
 ********************************************************************************/
FwNatConn * RvH323FwNatGetEncapsulationConnection(
    IN  FwNatMgr *              mgr,
    IN  cmTransportAddress *    remoteAddr);


/********************************************************************************
 * Function Name: RvH323FwNatGetEncapsulationConnection
 * Description:  Creates an encapsulation connection from an existing connection.
 *
 * Input:   mgr          - FwNat module handle.
 *          hConn        - Stack handle to the connection.
 * Output:  none
 * Return: The encapsulation connection.
 ********************************************************************************/
FwNatConn * RvH323FwNatSetEncapsulatedConnection(
    IN  FwNatMgr *              mgr,
    IN  HPROTCONN               hConn);
/********************************************************************************
 * Function Name: addIncomingCallIndication
 * Description:  Adds an IncomingCallIndication to a SCI message in a ras 
 *               transaction object.
 *
 * Input:   mgr          - FW/NAT manager.
 *          hsCall       - Handle of call in the stack.
 *          hsRas        - Handle of SCI ras transaction.
 * Output:  None.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
void addIncomingCallIndication(IN FwNatMgr *mgr, 
                               IN HCALL hsCall, 
                               IN HRAS hsRas);

/********************************************************************************
 * Function Name: getIncomingCallIndicationFromMessage
 * Description:  Gets the IncomingCallIndication from a SCI message 
 *
 * Input:   mgr             - FW/NAT manager.
 *          messageNodeId   - The serviceControlIndication node id.
 * Output:  IncomingCallIndication node.
 * Return:  An IncomingCallIndication node decoded.
 ********************************************************************************/
RvPvtNodeId getIncomingCallIndicationFromMessage(IN FwNatMgr *mgr, 
                                                 IN RvPvtNodeId messageNodeId, 
                                                 OUT RvPvtNodeId *incomingCallIndicationNode);

/********************************************************************************
 * Function Name: openReverseCallTimeout
 * Description:  TimeOut callback for the GK after sending SCI. This callback
 *               will be invoked within 5 seconds after sending the SCI if the 
 *               facility message from the endpoint was not received yet.
 *
 * Input:   context     -   The FwNatConn on which the timeOut occurred.
 * Output:  None.
 * Return:  RV_FALSE.
 ********************************************************************************/
RvBool openReverseCallTimeout(IN void* context);

/********************************************************************************
 * Function Name: closeEncapsulationConnection
 * Description:  TimeOut callback for the GK after sending UCF. This callback
 *               will be invoked to close the encapsulation connection, to avoid
 *               closing it from within a callback.
 *
 * Input:   context     -   The FwNatConn on which the timeOut occurred.
 * Output:  None.
 * Return:  RV_FALSE.
 ********************************************************************************/
RvBool closeEncapsulationConnection(IN void* context);

/********************************************************************************
 * Function Name: outConn4incomingCallConnect
 * Description:  Handles the connect event of the tcp connection for an incoming call
 *               (after SCI with IncomingCallIndication from GK).
 * Input:   context     -   The connection handle in the FW/NAT module.
 * Output:  None.
 * Return:  None.
 ********************************************************************************/
void outConn4incomingCallConnect(void * tpkt, int event, int length, int error, void * context);

/********************************************************************************
 * Function Name: buildFacilityMessageWithCallID
 * Description:  Builds a facility message with the given callIdentifier with 
                 undefinedReason reason.
 * Input:   mgr                     -   The H460.18 manager.
 *          CID                     -   The callIdentifier.
 * Output:  facilityMessageNodeId   -   The node id of the facility message to send.
 * Return:  None.
 ********************************************************************************/
void buildFacilityMessageWithCallID(IN   FwNatMgr *      mgr,
                                    IN   RvUint8 *       CID, 
                                    OUT  RvPvtNodeId *   facilityMessageNodeId);

/********************************************************************************
 * Function Name: h245MultiServerAcceptHandler
 * Description:  handle the accept event on the H.245 Multi-Server connection.
 ********************************************************************************/
void h245MultiServerAcceptHandler(void * tpkt, int event, int length, int error, void*context);

/********************************************************************************
 * Function Name: buildGenericIndicationMessage
 * Description:  Builds a H.460.18 generic indication message with the given
 *               callIdentifier and answerCall values.
 * Input:   mgr         - The H460.18 manager.
 *          CID         - The callIdentifier.
 *          bIncoming   - is the call an incoming one?
 * Output:  None.
 * Return:  The node id of the generic indication message.
 ********************************************************************************/
RvPvtNodeId buildGenericIndicationMessage(IN  FwNatMgr *    mgr,
                                          IN  RvUint8 *     CID, 
                                          IN  RvBool        bIncoming);



/********************************************************************************
 * Function Name: getGenericIndicationMessage
 * Description:  Checks if this is a H.460.18 generic indication message and gets
 *               the call according to the callIdentifier and answerCall values.
 * Input:   mgr         - The H460.18 manager.
 *          message     - An H.245 message
 *          bIncoming   - is the call an incoming one?
 * Output:  hsCall      - The call referred to by the generic message, if found.
 * Return:  RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus getGenericIndicationMessage(IN  FwNatMgr *    mgr,
                                     IN  RvPvtNodeId   message, 
                                     OUT HCALL *       hsCall);


/********************************************************************************
 * Function Name: setH245ConnectionForCall
 * Description:  Sets the connections accepted from the H.245 multy-server as
 *               listening connection for a call.
 * Input:   mgr         - The H460.18 manager.
 *          hsCall      - Call to set the accepted connection for
 *          hConn       - The connection to set
 *          fnServ      - FW/NAT H.245 server connection
 * Output:  None.
 * Return:  RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus setH245ConnectionForCall(IN  FwNatMgr *    mgr,
                                  IN  HCALL         hsCall,
                                  IN  HPROTCONN     hConn,
                                  IN  FwNatConn *   fnServ);

/********************************************************************************
 * Function Name: addKeepAliveChannelInfo
 * Description:  Sets the keepAlive parameters in the TraversalParameters node.
 * Input:   hVal                    - The val tree handle of the stack.
 *          traversalParamsNodeId   - The TraversalParameters node id.
 *          keepAliveAddr           - The keepAliveChannel Address.
 *          keepAlivePayLoadType    - The keepAlivePayLoadType to be added to
 *                                    the TraversalParameters node. Will not
 *                                    be added if a negative value.
 *          keepAliveInterval       - The interval in seconds to send keepAlive messages.
 *                                    Will not be added to the TraversalParameters node
 *                                    if negative will be set to 5 if given value
 *                                    less than 5.
 * Output:  None.
 * Return:  RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus addKeepAliveChannelInfo(IN HPVT                    hVal, 
                                 IN RvPvtNodeId             traversalParamsNodeId, 
                                 IN cmTransportAddress *    keepAliveAddr, 
                                 IN RvInt8                  keepAlivePayLoadType, 
                                 IN RvInt16                 keepAliveInterval);

/********************************************************************************
 * Function Name: addMultiplexInfo
 * Description:  Add multiplex information in traversalParameters node.
 * Input:   hVal                    - The val tree handle of the stack.
 *          fieldId                 - fieldId of the message where multiplexed
 *                                    info is being added.
 *          traversalParamsNodeId   - The TraversalParameters node id.
 *          multiplexID             - The multiplexID for the channel.
 *          rtpMuxAddr              - The rtp Address to send media to.
 *          rctpMuxAddr             - rctp Address to send control media to.
 * Output:  None.
 * Return:  RV_OK on success, negative on failure
 ********************************************************************************/
void addMultiplexInfo(IN HPVT                    hVal,
                      IN RvPstFieldId            fieldId,
                      IN RvPvtNodeId             traversalParamsNodeId, 
                      IN RvUint32                multiplexID, 
                      IN cmTransportAddress *    rtpMuxAddr, 
                      IN cmTransportAddress *    rctpMuxAddr);

/********************************************************************************
 * Function Name: encodeAndAddTraversalParams
 * Description:  Encode traversalParameters and add to OLC/OLCA.
 * Input:   hVal                    - The val tree handle of the stack.
 *          olcMsg                  - The OLC/OLCA node id.
 *          traversalParamsNodeId   - The TraversalParameters node id.
 * Output:  None.
 * Return:  RV_OK on success, negative on failure
 ********************************************************************************/
RvStatus encodeAndAddTraversalParams(IN HPVT            hVal, 
                                     IN RvPvtNodeId     message, 
                                     IN RvPvtNodeId     traversalParamsNodeId);

/********************************************************************************
 * Function Name: decodeAndGetTraversalParams
 * Description:  Retrieves traversalParameters node from OLC/OLCA.
 * Input:   hVal                    - The val tree handle of the stack.
 *          olcMsg                  - The OLC/OLCA node id.
 * Output:  traversalParamsNodeId   - The TraversalParameters node.
 * Return:  None.
 ********************************************************************************/
void decodeAndGetTraversalParams(IN     HPVT          hVal, 
                                 IN     RvPvtNodeId   olcMsg, 
                                 OUT    RvPvtNodeId * traversalParamsNodeId);

/********************************************************************************
 * Function Name: getKeepAliveAddr
 * Description:  Retrieves keepAlive address from traverslParameters.
 * Input:   hVal                    - The val tree handle of the stack.
 *          traversalParamsNodeId   - The traversalParameters node id.
 * Output:  addr                    - The keepAlive address.
 *          keepAlivePayLoadType    - The keepAlivePayLoadType value.
 *          keepAliveInterval       - The keepAliveInterval value.
 * Return:  None.
 ********************************************************************************/
void getKeepAliveChannelInfo(IN    HPVT                 hVal, 
                             IN    RvPvtNodeId          traversalParamsNodeId, 
                             OUT   cmTransportAddress * addr,
                             OUT   RvInt8 *             keepAlivePayLoadType, 
                             OUT   RvInt16 *            keepAliveInterval);

/********************************************************************************
 * Function Name: getMultiplexInfo
 * Description:  Retrieves multiplex information from traversalParameters.
 * Input:   hVal                    - The val tree handle of the stack.
 *          traversalParamsNodeId   - The traversalParameters node id.
 * Output:  multiplexId             - the multiplexID.  
 *          rtpMuxAddr              - The multiplexedMediaChannel.
 *          rctpMuxAddr             - The multiplexedMediaControlChannel.
 * Return:  None.
 ********************************************************************************/
void getMultiplexInfo(IN    HPVT                 hVal,
                      IN    RvPvtNodeId          traversalParamsNodeId,
                      OUT   RvUint32 *           multiplexId,
                      OUT   cmTransportAddress * rtpMuxAddr,
                      OUT   cmTransportAddress * rctpMuxAddr);

/********************************************************************************
 * Function Name: removeTraversalParams
 * Description:  Remove traversal parameters from OLC/OLCA message.
 * Input:   hVal                    - The val tree handle of the stack.
 *          message                 - The OLC/OLCA node id.
 * Output:  None.
 * Return:  None.
 ********************************************************************************/
void removeTraversalParams(
           IN HPVT            hVal, 
           IN RvPvtNodeId     message);

#endif /*__FWNAT_H*/

#ifdef __cplusplus
}
#endif

