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

#ifndef __FWNAT_API_H
#define __FWNAT_API_H

#include "cm.h"

/*================================================
 * GENERAL DEFINITIONS
 ===============================================*/

RV_DECLARE_HANDLE(HH323FWNATMGR);

RV_DECLARE_HANDLE(HH323FWNATCONN);

#define RV_H323_FWNAT_SIGNALLING_TRAVERSAL_IDENTIFIER_VALUE 18
#define RV_H323_FWNAT_INCOMING_CALL_INDICATION_IDENTIFIER_VALUE 1
#define RV_H323_FWNAT_SIGNALLING_MEDIA_TRAVERSAL_IDENTIFIER_VALUE 19
#define RV_H323_FWNAT_SIGNALLING_SUPPORT_TRANSMIT_MULTIPLEXED_MEDIA_VALUE 1
#define RV_H323_FWNAT_SIGNALLING_MEDIA_TRAVERSAL_SEVER_IDENTIFIER_VALUE 2

/*=============================================
 * TYPEDEFS
 =============================================*/

typedef enum {
    RvH323FwNatH46017StateDisabled,
    RvH323FwNatH46017StateEnabled,
    RvH323FwNatH46017StateConnected,
    RvH323FwNatH46017StateError
} RvH323FwNatH46017State;

typedef enum {
    RvH323FwNatH46018StateNotSupported,
    RvH323FwNatH46018StateInitializedEP,
    RvH323FwNatH46018StateSupportedEP,
    RvH323FwNatH46018StateInitializedGK
} RvH323FwNatH46018State;

typedef enum {
    RvH323FwNatH46019StateDisabled,
    RvH323FwNatH46019StateClient,
    RvH323FwNatH46019StateServer
} RvH323FwNatH46019State;

/*multiplexing modes*/
#define RV_H323_FWNAT_H46019_MULTIPLEX_MODE_NONE        0x00 
#define RV_H323_FWNAT_H46019_MULTIPLEX_MODE_TRANSMIT    0x01  /*transmitting multiplexed media - supported*/
#define RV_H323_FWNAT_H46019_MULTIPLEX_MODE_RECEIVE     0x02  /*receiving multiplexed media - supported*/

typedef enum {
    RvH323FwNatH46017ConfigNotUsed,
    RvH323FwNatH46017ConfigEncapsulate
} RvH323FwNatH46017Config;

typedef enum {
    RvH323FwNatH46018ConfigNotUsed,
    RvH323FwNatH46018ConfigSupported
} RvH323FwNatH46018Config;

typedef enum {
    RvH323FwNatH46019ConfigNotUsed,
    RvH323FwNatH46019ConfigClient,
    RvH323FwNatH46019ConfigServer
} RvH323FwNatH46019Config;

typedef enum {
    RvH323FwNatH46019MediaMultiplexModeConfigNone,
    RvH323FwNatH46019MediaMultiplexModeConfigTransmit, /*only transmitting multiplexed media - supported*/
    RvH323FwNatH46019MediaMultiplexModeConfigReceive,  /*only receiving multiplexed media - supported*/
    RvH323FwNatH46019MediaMultiplexModeConfigFull     /*transmitting & receiving of multiplexed media - supported*/
} RvH323FwNatH46019MediaMultiplexModeConfig;



typedef struct {
    RvBool                                    bGatekeeper; /* true if this application should function as
                                                              a GK, i.e., interact with several entities. */
    RvH323FwNatH46017Config                   e17Config;                /* configuration of H.460.17 */
    RvH323FwNatH46018Config                   e18Config;                /* configuration of H.460.18 */
    RvH323FwNatH46019Config                   e19Config;                /* configuration of H.460.19 */
    RvH323FwNatH46019MediaMultiplexModeConfig e19MultiplexMediaConfig;  /* configuration of H.460.19 media 
                                                                           multiplexing capability */
} RvH323FwNatConfiguration;


typedef struct {
    cmTransportAddress *    rtpMuxAddr; 
    cmTransportAddress *    rctpMuxAddr;
    RvUint32                multiplexID;
    cmTransportAddress *    keepAliveAddr;
    RvInt8                  keepAlivePayLoadType;
    RvInt16                 keepAliveInterval;
}RvH323FwNatH46019traversalParams;

/*================================================
 * EVENT HANDLERS PROTOTYPES
 ===============================================*/

/********************************************************************************
 * Type Name: RvH323FwNatSetByCallEv (callback prototype)
 * Description: This event gives the application a connection handle to keep for
 *              the FwNat module in the application's call database.
 *
 * Input:   hApp         - stack application handle.
 *          hFwNat       - FwNat module handle
 *          hsCall       - Stack's call handle
 *          haCall       - Application's call handle
 *          hFwNatConn   - The FwNat connection related to the call or NULL
 *                         to unset.
 *          index        - for future use (currently will be set to zero)
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
typedef RvStatus (RVCALLCONV *RvH323FwNatSetByCallEv)(
    IN  HAPP                hApp,
    IN  HH323FWNATMGR       hFwNat,
    IN  HCALL               hsCall,
    IN  HAPPCALL            haCall,
    IN  HH323FWNATCONN      hFwNatConn,
    IN  RvInt32             index);


/********************************************************************************
 * Type Name: RvH323FwNatGetByCallEv (callback prototype)
 * Description: This event gets the application a FwNat connection handle
 *              from the application's call database.
 *
 * Input:   hApp         - stack application handle.
 *          hFwNat       - FwNat module handle
 *          hsCall       - Stack's call handle
 *          haCall       - Application's call handle
 *          index        - for future use (currently will be set to zero)
 * Output:  hFwNatConn   - The FwNat connection related to the call or NULL
 *                         if not found.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
typedef RvStatus (RVCALLCONV *RvH323FwNatGetByCallEv)(
    IN  HAPP                hApp,
    IN  HH323FWNATMGR       hFwNat,
    IN  HCALL               hsCall,
    IN  HAPPCALL            haCall,
    IN  RvInt32             index,
    OUT HH323FWNATCONN *    hFwNatConn);


/********************************************************************************
 * Type Name: RvH323FwNatSetProtConnHandleEv (callback prototype)
 * Description: If the application sets application handle for ProtConn elements,
 *              it should implement this callback to allow the FANTAS module to
 *              associate ProtConn elements with FANTAS elements. Otherwise, the
 *              FANTAS module will set its own handles (default). Sets a handle
 *              to associate with a ProtConn element.
 *
 * Input:   hProtConn    - Stack's connection handle
 *          hFwNatConn   - The FwNat connection related to the connection
 *                         or NULL to unset.
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
typedef RvStatus (RVCALLCONV *RvH323FwNatSetProtConnHandleEv)(
    IN  HPROTCONN           hProtConn,
    IN  HH323FWNATCONN      hFwNatConn);


/********************************************************************************
 * Type Name: RvH323FwNatGetProtConnHandleEv (callback prototype)
 * Description: If the application sets application handle for ProtConn elements,
 *              it should implement this callback to allow the FANTAS module to
 *              associate ProtConn elements with FANTAS elements. Otherwise, the
 *              FANTAS module will set its own handles (default). Gets the handle
 *              associated with a ProtConn element.
 *
 * Input:   hProtConn    - Stack's connection handle
 * Output:  hFwNatConn   - The FwNat connection related to the connection
 *                         or NULL to unset.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
typedef RvStatus (RVCALLCONV *RvH323FwNatGetProtConnHandleEv)(
    IN  HPROTCONN           hProtConn,
    OUT HH323FWNATCONN *    hFwNatConn);


/********************************************************************************
 * Type Name: RvH323FwNatRasEncapStatusEv (callback prototype)
 * Description: Notifies the endpoint application as to the status of the
 *              H.460.17 RAS encapsulation over TPKT connection is.
 *
 * Input:   hApp            - stack application handle.
 *          hFwNat          - FwNat module handle.
 *          hProtConn       - Stack's connection handle.
 *          hFwNatConn      - FwNat connection handle.
 *          status          - Status of connection.
 *          hsRas           - Attempted RAS transaction handle.
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
typedef RvStatus (RVCALLCONV *RvH323FwNatRasEncapStatusEv)(
    IN  HAPP                    hApp,
    IN  HH323FWNATMGR           hFwNat,
    IN  HPROTCONN               hProtConn,
    IN  HH323FWNATCONN          hFwNatConn,
    IN  RvH323FwNatH46017State  status,
    IN  HRAS                    hsRas);


typedef struct {
    RvH323FwNatSetByCallEv            fwNatSetByCallEv;
    RvH323FwNatGetByCallEv            fwNatGetByCallEv;
    RvH323FwNatSetProtConnHandleEv    fwNatSetProtConnHandleEv;
    RvH323FwNatGetProtConnHandleEv    fwNatGetProtConnHandleEv;
    RvH323FwNatRasEncapStatusEv       fwNatRasEncapStatusEv;
} RvH323FwNatCallbacks;


/*=============================================
 * Function prototypes
 ==============================================*/

/********************************************************************************
 * Function Name: RvH323FwNatInitialize
 * Description:  Initialize the H.460.18/19/17 module.
 *
 * Input:   hApp         - Stack application handle.
 *          config       - Configuration to use.
 *          callbacks    - Callbacks to use.
 *          numHosts     - Number of hosts to allocate.
 *          encapMsgSize - Maximum size of an encapsulated message (or 0).
 * Output:  hFwNat       - FwNat module handle.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatInitialize(
    IN  HAPP                        hApp,
    IN  RvH323FwNatConfiguration *  config,
    IN  RvH323FwNatCallbacks *      callbacks,
    IN  RvUint32                    numHosts,
    IN  RvUint32                    encapMsgSize,
    OUT HH323FWNATMGR *             hFwNat);


/********************************************************************************
 * Function Name: RvH323FwNatGetVersion
 * Description:  Get the module version.
 *
 * Input:   none.
 * Output:  none.
 * Return: The version string.
 ********************************************************************************/
RVAPI const RvChar* RVCALLCONV RvH323FwNatGetVersion(void);


/********************************************************************************
 * Function Name: RvH323FwNatReconfigure
 * Description:  Change the H.460.18/19/17 configuration.
 *
 * Input:  hFwNat       - FwNat module handle.
 *         config       - Configuration to use.
 * Output: none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatReconfigure(
    IN  HH323FWNATMGR               hFwNat,
    IN  RvH323FwNatConfiguration *  config);


/********************************************************************************
 * Function Name: RvH323FwNatEnd
 * Description:  End the H.460.18/19/17 module.
 *
 * Input:   hFwNat       - FwNat module handle.
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEnd(
    IN  HH323FWNATMGR       hFwNat);


/********************************************************************************
 * Function Name: RvH323FwNatOpenReverseCall
 * Description:  Ask the FwNat module in the GK side to start a reverse call
 *               (a call into the NAT).
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsCall       - Stack's call handle
 *          epRasAddr    - RAS address of the EP to open a call to.
 *          haRas        - Application handle for the created SCI Tx.
 * Output:  haRas        - Stack handle for the created SCI Tx.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatOpenReverseCall(
    IN  HH323FWNATMGR       hFwNat,
    IN  HCALL               hsCall,
    IN  cmTransportAddress *epRasAddr,
    IN  HAPPRAS             haRas,
    OUT HRAS *              hRas);


/********************************************************************************
 * Function Name: RvH323FwNatOpenH245Server
 * Description:  Ask the FwNat module to create an H.245 multi-server
 *               connection. The application must keep the connection handle.
 *
 * Input:   hFwNat       - FwNat module handle
 *          addr         - Address to listen on, may be set to zero.
 * Output:  addr         - Address listening on.
 *          hFwNatConn   - The FwNat H.245 server.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatOpenH245Server(
    IN    HH323FWNATMGR         hFwNat,
    INOUT cmTransportAddress *  addr,
    OUT   HH323FWNATCONN *      hFwNatConn);


/********************************************************************************
 * Function Name: RvH323FwNatSetCallH245Server
 * Description:  Set an H.245 multi-server as the call's H.245 connection.
 *               This function must be called only after any call reversal
 *               procedures, dial-tone state being optimal.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hFwNatServ    - Handle of the server
 *          hsCall       - Stack's call handle
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatSetCallH245Server(
    IN  HH323FWNATMGR       hFwNat,
    IN  HH323FWNATCONN      hFwNatServ,
    IN  HCALL               hsCall);


/********************************************************************************
 * Function Name: RvH323FwNatConnCallTerminated
 * Description:  Notify the FW/NAT module that a the connection's call has been
 *               terminated.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hFwNatConn   - Handle of the connection
 *          hsCall       - Stack's call handle
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatConnCallTerminated(
    IN  HH323FWNATMGR       hFwNat,
    IN  HH323FWNATCONN      hFwNatServ,
    IN  HCALL               hsCall);


/********************************************************************************
 * Function Name: RvH323FwNatCloseH245Server
 * Description:  Close an H.245 multi-server connection. It is the application's
 *               responsibility to remove the server from the calls it was set to.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hFwNatServ    - Handle of the server
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatCloseH245Server(
    IN  HH323FWNATMGR       hFwNat,
    IN  HH323FWNATCONN      hFwNatServ);


/********************************************************************************
 * Function Name: RvH323FwNatRASIsEncapsulated
 * Description: Checks if an encapsulation connection exists to the given address,
 *              and if not, creates it. returns the existing or created connection.
 *
 * Input:   hFwNat       - FwNat module handle
 *          addrTo       - Transport address to check or connect to.
 * Output:  hConn        - The encapsulated connection
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatOpenEncapsulateConn(
    IN  HH323FWNATMGR       hFwNat,
    IN  cmTransportAddress *addrTo,
    OUT HPROTCONN *         hConn);


/********************************************************************************
 * Function Name: RvH323FwNatRASIsEncapsulated
 * Description: Checks if a transaction has an encapsulated connection according
 *              to a transport address or a RAS transaction handle.
 *
 * Input:   hFwNat       - FwNat module handle
 *          rasAddr      - RAS transport address to check.
 *          hsRas        - RAS transaction handle to check.
 *          hsCall       - Stack's RAs transaction handle
 * Output:  bEncapsulated - true if the connection is encapsulated.
 *          hConn        - The encapsulation connection
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatRASIsEncapsulated(
    IN  HH323FWNATMGR       hFwNat,
    IN  cmTransportAddress *rasAddr,
    IN  HRAS                hsRas,
    OUT RvBool *            bEncapsulated,
    OUT HPROTCONN *         hConn);

    
/********************************************************************************
 * Function Name: RvH323FwNatCloseEncapsulateConn
 * Description: Set a timer to close an encapsulated connection.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hConn        - The encapsulation connection.
 * Output:  None.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatCloseEncapsulateConn(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn);


/********************************************************************************
 * Function Name: RvH323FwNatAddChannelH46019Info
 * Description: This function should be called before sending OLC (proposal/response) 
 *              in case of fast connect call it may add FwNat information to the OLC 
 *              (proposal/response).
 * NOTE: The application should supply the correct rtpMuxAddr, rctpMuxAddr and
 *       multiplexID.
 *
 * Input:   hFwNat                      - FwNat module handle
 *          hsChan                      - Stack's handle for the channel.
 *          message                     - PVT node ID of the OLC/OLCA that is 
 *                                        about to be sent
 *          traversalParams             - The Traversal Parameters to add to OLC/OLCA. 
 *          bUseH46019                  - Indication if h46019 procedures are used
 *                                        in the call.
 *          bbRemoteMultiplexEnabled    - Indication if the remote side supports
 *                                        transmission of multiplexed media.
 *          bFastConnectCall            - RV_TRUE if call is using fast connect 
 *                                        procedures.
 *          channelDirection            - dirTransmit or dirReceive for FS calls.
 * Output:  bInfo        - RV_TRUE iff the message was added with any information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatAddChannelH46019Info(
    IN  HH323FWNATMGR                   hFwNat,
    IN  HCHAN                           hsChan,
    IN  RvPvtNodeId                     message,
    IN  RvH323FwNatH46019traversalParams * traversalParams,
    IN  RvBool                          bUseH46019,
    IN  RvBool                          bRemoteMultiplexEnabled,
    IN  RvBool                          bFastConnectCall,
    IN  cmChannelDirection              channelDirection,
    OUT RvBool *                        bInfo);


/********************************************************************************
 * Function Name: RvH323FwNatGetChannelH46019Info
 * Description: This function should be called for eack OLC/OLCA received in a 
 *              fast connect call.
 *              It extracts FwNat information from the OLC (proposal/response) messages
 *              This function should be called even if an h46019
 *              entity does not have multiplexing capabilities.
 *
 * Input:   hFwNat              - FwNat module handle
 *          hsChan              - Stack's handle for the channel.
 *          message             - PVT node ID of the message that is about to be sent
 *          bFastConnectCall    - RV_TRUE if call is using fast connect
 *                                procedures.
 *          channelDirection    - dirTransmit or dirReceive for FS calls.
 * Output:  traversalParams     - Traversal Parameters from OLC/OLCA
 *          bInfo               - RV_TRUE iff the message contained any relevant
 *                         information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatGetChannelH46019Info(
    IN  HH323FWNATMGR                   hFwNat,
    IN  HCHAN                           hsChan,
    IN  RvPvtNodeId                     message,
    IN  RvBool                          bFastConnectCall,
    IN  cmChannelDirection              channelDirection,
    OUT RvH323FwNatH46019traversalParams * traversalParams,
    OUT RvBool *                        bInfo);


/* Hook API:
 * The hook API functions should be called from the respective hook events. All hooks set a Boolean
 * return parameter to true if this message contained or was set with H.460.FANTAS/MA/17information
 * and false otherwise.
 **********************/


/********************************************************************************
 * Function Name: RvH323FwNatEvRASMessageSend
 * Description: This function should be called from cmEvRASMessageSend(). it
 *              may add FwNat information to the message.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - RAS transaction this message belongs to.
 *          rasTrStage   - Stage this message is sent for.
 *          messageNode  - PVT nodeId of the message being sent out.
 * Output:  bStop        - RV_TRUE iff the message should not be sent (to be
 *                         returned on exit from cmEvRASMessageSend).
 *          bInfo        - RV_TRUE iff the message was added with any relevant
 *                         information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvRASMessageSend(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas,
    IN  cmRASTrStage        rasTrStage,
    IN  RvPvtNodeId         messageNodeId,
    OUT RvBool *            bStop,
    OUT RvBool *            bInfo);

/********************************************************************************
 * Function Name: RvH323FwNatEvRASRequest
 * Description: This function should be called from cmEvRASRequest(). This API 
 *              is used in the EP inside the NAT to handle an incoming call.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - Stack's handle for the RAS transaction.
 * Output:  bIgnore      - RV_TRUE if the application should ignore the message.
 *          bInfo        - RV_TRUE if the message was modified with any relevant
 *                         information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvRASRequest(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas,
    OUT RvBool *            bIgnore,
    OUT RvBool *            bInfo);


/********************************************************************************
 * Function Name: RvH323FwNatEvRASConfirm
 * Description: This function should be called from cmEvRASConfirm().
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - Stack's handle for the RAS transaction
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvRASConfirm(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas);


/********************************************************************************
 * Function Name: RvH323FwNatEvRASReject
 * Description: This function should be called from cmEvRASReject(). it
 *              may extract FwNat information to the message.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - Stack's handle for the RAS transaction
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvRASReject(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas);


/********************************************************************************
 * Function Name: RvH323FwNatEvRASTimeout
 * Description: This function should be called from cmEvRASTimeout().
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - Stack's handle for the RAS transaction
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvRASTimeout(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas);


/********************************************************************************
 * Function Name: RvH323FwNatEvAutoRASRequest
 * Description: This function should be called from cmEvAutoRASRequest(). it
 *              may extract FwNat information from the message.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - Stack's handle for the RAS transaction
 * Output:  bInfo        - RV_TRUE iff the message contained any relevant
 *                         information.
 *          bIgnore      - RV_TRUE if message should be ignored.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvAutoRASRequest(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas,
    OUT RvBool *            bIgnore,
    OUT RvBool *            bInfo);

#if 0

/********************************************************************************
 * Function Name: RvH323FwNatEvAutoRASConfirm
 * Description: This function should be called from cmEvAutoRASConfirm().
 *
 * Input:   hFwNat       - FwNat module handle
 * input  : hsRas        - Stack's handle for the RAS transaction
 * Output:  None.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvAutoRASConfirm(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas);


/********************************************************************************
 * Function Name: RvH323FwNatEvAutoRASReject
 * Description: This function should be called from cmEvAutoRASReject().
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - Stack's handle for the RAS transaction
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvAutoRASReject(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas);


/********************************************************************************
 * Function Name: RvH323FwNatEvAutoRASTimeout
 * Description: This function should be called from cmEvAutoRASTimeout().
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsRas        - Stack's handle for the RAS transaction
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatEvAutoRASTimeout(
    IN  HH323FWNATMGR       hFwNat,
    IN  HRAS                hsRas);

#else
/* The above functions are place holders in case we'll need them in future
   releases of the firewall NAT add-on. for not, they are defined out */
#define RvH323FwNatEvAutoRASConfirm (RV_OK)
#define RvH323FwNatEvAutoRASReject (RV_OK)
#define RvH323FwNatEvAutoRASTimeout (RV_OK)

#endif


/********************************************************************************
 * Function Name: RvH323FwNatCallSendMessage
 * Description: This function should be called from cmEvCallSendMessage(). it
 *              may add FwNat information to the message.
 *
 * Input:   hFwNat       - FwNat module handle
 *          hsCall       - Stack's handle for the call
 *          hFwNatConn    - The FwNat connection related to the call, if one
 *                         was set. NULL o.w.
 *          message      - PVT node ID of the message that is about to be sent
 * Output:  bInfo        - RV_TRUE iff the message was added with any information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatCallSendMessage(
    IN  HH323FWNATMGR       hFwNat,
    IN  HCALL               hsCall,
    IN  HH323FWNATCONN      hFwNatConn,
    IN  RvPvtNodeId         message,
    OUT RvBool *            bInfo);

/********************************************************************************
 * Function Name: RvH323FwNatCallRecvMessage
 * Description: This function should be called from cmEvCallRecvMessage(). It
 *              may retrieve FwNat information from the message and change
 *              the mode of use of H46019.
 *
 * Input:   hFwNat                          - FwNat module handle
 *          hsCall                          - Stack's handle for the call
 *          hFwNatConn                      - The FwNat connection related to the call, if one
 *                                            was set. NULL o.w.
 *          message                         - PVT node ID of the message that is about to be sent
 * Output:  bInfo                           - RV_TRUE if H46019 info received.
 *          bUseH46019                      - will be set to RV_TRUE if the procedures of h46019
 *                                            can be used in this call.
 *          bRemoteMediaMultiplexEnabled    - RV_TRUE if the remote peer supports 
 *                                            multiplexed media transmission.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatCallRecvMessage(
    IN  HH323FWNATMGR       hFwNat,
    IN  HCALL               hsCall,
    IN  HH323FWNATCONN      hFwNatConn,
    IN  RvPvtNodeId         message,
    OUT RvBool *            bInfo,
    OUT RvBool *            bUseH46019,
    OUT RvBool *            bRemoteMediaMultiplexEnabled);

/********************************************************************************
 * Function Name: RvH323FwNatChannelSendMessage
 * Description: This function should be called from cmEvChannelSendMessage(). it
 *              may add FwNat information to the message if it is an OLC or
 *              an OLCA.
 * NOTE: The application should supply the correct rtpMuxAddr, rctpMuxAddr and
 *       multiplexID.
 *
 * Input:   hFwNat                      - FwNat module handle
 *          hsChan                      - Stack's handle for the channel.
 *          message                     - PVT node ID of the message that is 
 *                                        about to be sent
 *          traversalParams             - Traversal Parameters to add to OLC/OLCA.
 *          bUseH46019                  - Indication if h46019 procedures are used
 *                                        in the call.
 *          bRemoteMultiplexEnabled    - Indication if the remote side supports
 *                                        transmission of multiplexed media. 
 * Output:  bInfo        - RV_TRUE iff the message was added with any information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatChannelSendMessage(
    IN  HH323FWNATMGR                   hFwNat,
    IN  HCHAN                           hsChan,
    IN  RvPvtNodeId                     message,
    IN  RvH323FwNatH46019traversalParams * traversalParams,
    IN  RvBool                          bUseH46019,
    IN  RvBool                          bRemoteMultiplexEnabled,
    OUT RvBool *                        bInfo);

/********************************************************************************
 * Function Name: RvH323FwNatChannelRecvMessage
 * Description: This function should be called from cmEvChannelRecvMessage(). it
 *              extracts FwNat information from the message if it is an OLC
 *              or an OLCA. This function should be called even if an h46019
 *              entity does not have multiplexing capabilities.
 *
 * Input:   hFwNat              - FwNat module handle
 *          hsChan              - Stack's handle for the channel.
 *          message             - PVT node ID of the message that is about to be sent
 *          bFastConnectCall    - RV_TRUE if call is using fast connect
 *                                procedures.
 *          channelDirection    - dirTransmit or dirReceive for FS calls.
 * Output:  traversalParams     - Traversal Parameters retrieved from OLC/OLCA
 *          bInfo               - RV_TRUE iff the message contained any relevant
 *                                information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatChannelRecvMessage(
    IN  HH323FWNATMGR                   hFwNat,
    IN  HCHAN                           hsChan,
    IN  RvPvtNodeId                     message,
    IN  RvBool                          bFastConnectCall,
    IN  cmChannelDirection              channelDirection,
    OUT RvH323FwNatH46019traversalParams * traversalParams,
    OUT RvBool *                        bInfo);


/********************************************************************************
 * Function Name: RvH323FwNatHookOutConn
 * Description: This function should be called from hookOutConn().
 *
 * Input:   hFwNat       - FwNat module handle
 *          hConn        - Stack's handle for the connection.
 *          addrFrom     - PVT node ID of the local address
 *          addrTo       - PVT node ID of the remote address
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookOutConn(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn,
    IN  RvPvtNodeId         addrFrom,
    IN  RvPvtNodeId         addrTo);


/********************************************************************************
 * Function Name: RvH323FwNatHookRecv
 * Description: This function should be called from hookRecv(). 
 *              It is used to extract encapsulated RAS messages.
 *
 * Input:   hFwNat          - FwNat module handle
 *          messageNodeId   - Stack's handle for the RAS transaction
 *          hConn           - Stack's handle for the connection.
 * Output:  bIgnore         - RV_TRUE if the application should ignore the message.
 *          bInfo           - RV_TRUE if the message contained any relevant
 *                            information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookRecv(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn,
    IN  RvPvtNodeId         messageNodeId,
    OUT RvBool              *bIgnore,
    OUT RvBool              *bInfo);


/********************************************************************************
 * Function Name: RvH323FwNatHookRecvFrom
 * Description: This function should be called from hookRecvFrom(). 
 *              in case of a ras request: it may cause the GK to replace 
 *              the rasAddress in the RAS message with the apparent source 
 *              transport address from which the request was sent.
 *              in case of a confirm: it may cause an endpoint to reset it's H.460.18
 *              state (RvH323FwNatH46018State) to h46018StateNotSupported ,
 *              if the GK does not support it.
 *
 * Input:   hFwNat          - FwNat module handle
 *          messageNodeId   - Stack's handle for the RAS transaction
 * Output:  bInfo           - RV_TRUE if the message was modified with any relevant
 *                            information.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookRecvFrom(
    IN  HH323FWNATMGR       hFwNat,
    IN  RvPvtNodeId         messageNodeId,
    IN  RvPvtNodeId         addrFromNodeId,
    OUT RvBool *            bInfo);


/********************************************************************************
 * Function Name: RvH323FwNatHookSendRaw
 * Description: This function should be called from hookSendRaw(). 
 *              Currently does nothing.
 *
 * Input:   hFwNat          - FwNat module handle.
 *          hConn           - Stack's handle connection.
 *          rawMessage      - Outgoing message buffer.
 *          messageLen      - Message length.
 * Output:  bStop           - RV_TRUE iff the message should not be sent (to be
 *                            returned on exit from hookSendRaw).
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookSendRaw(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn,
    IN  const RvUint8 *     rawMessage,
    IN  RvInt32             messageLen,
    OUT RvBool *            bStop);


/********************************************************************************
 * Function Name: RvH323FwNatHookSendRawTo
 * Description: This function should be called from hookSendRawTo(). 
 *              Encapsulates the RAS messages in facility messages.
 *
 * Input:   hFwNat          - FwNat module handle.
 *          hConn           - Stack's handle connection.
 *          rawMessage      - Outgoing message buffer.
 *          messageLen      - Message length.
 *          addrTo          - Address sent to.
 * Output:  bStop           - RV_TRUE iff the message should not be sent (to be
 *                            returned on exit from hookSendRawTo).
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookSendRawTo(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn,
    IN  const RvUint8 *     rawMessage,
    IN  RvInt32             messageLen,
    IN  cmTransportAddress* addrTo,
    OUT RvBool *            bStop);


/********************************************************************************
 * Function Name: RvH323FwNatHookClose
 * Description: This function should be called from hookClose(). 
 *              Allows the FW/NAT module to free related resources.
 *
 * Input:   hFwNat          - FwNat module handle.
 *          hConn           - Stack's handle connection.
 * Output:  none.
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookClose(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn);


/********************************************************************************
 * Function Name: RvH323FwNatHookListen
 * Description: This function should be called from hookListen(). 
 *              Allows the FW/NAT module prevent unnecessary listening.
 *
 * Input:   hFwNat          - FwNat module handle.
 *          hConn           - Stack's handle connection.
 *          addrNodeId      - Listening address node Id.
 * Output:  bStop           - RV_TRUE iff the message should not be sent (to be
 *                            returned on exit from hookListen).
 * Return: RV_OK on success, negative on failure
 ********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323FwNatHookListen(
    IN  HH323FWNATMGR       hFwNat,
    IN  HPROTCONN           hConn,
    IN  RvPvtNodeId         addrNodeId,
    OUT RvBool *            bStop);


#endif /*__FWNAT_API_H*/

#ifdef __cplusplus
}
#endif

