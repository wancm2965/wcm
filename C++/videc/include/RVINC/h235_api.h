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

#ifndef __H235API_H
#define __H235API_H

#include "cm.h"

/*================================================
 * GENERAL DEFINITIONS
 ===============================================*/
RV_DECLARE_HANDLE(H235HAPP);       /* h235 application handle                                          */
RV_DECLARE_HANDLE(H235HKEY);       /* key handle                                                       */
RV_DECLARE_HANDLE(H235HMSG);       /* handle of the results of a received message security check*/
RV_DECLARE_HANDLE(H235HCALL);      /* handle of the H235 call struct*/
RV_DECLARE_HANDLE(H235HENTITY);    /* handle of the H235 entity struct*/

/*********************************************************************
 * The following string is the string of object id U.
 * It identifies the algorithm HMAC-SHA1-96 algorithm.
 * To be used as parameter for the h235SetEncodingCallback() function
 * (in this version, this is the only possible parameter)
 *********************************************************************/
#define OID_U_STRING "itu-t (0) recommendation (0) h (8) 235 version (0) 2 6"
#define OID_V_STRING "iso(1) member-body(2) us(840) rsadsi(113549) pkcs(1) pkcs-1(1) 4"
#define OID_W_STRING "iso(1) member-body(2) us(840) rsadsi(113549) pkcs(1) pkcs-1(1) 5"
#define OID_Y_STRING "iso(1) identified-organization(3) oiw(14) secsig(3) algorithm(2) desCBC(7)"
#define OID_Z_STRING "iso(1) identified-organization(3) oiw(14) secsig(3) algorithm(2) desEDE(17)"

#define RV_H235_GENERATE_PRIME_OID "prime generation function"


/*=============================================
 * TYPEDEFS
 =============================================*/

typedef enum {
    h235ModeNone         = 0x00000000,
    h235ModeProcedure1   = 0x00000001,
    h235ModeProcedure1a  = 0x00000002,
    h235ModeProcedure2   = 0x00000004,
    h235ModeProcedure3   = 0x00000008,
    h235ModeProcedure4   = 0x00000010,
    h235ModeProcedure4DH = 0x00000020,
    h235ModeDHgroup512   = 0x00001000,  /* generate a 56 bit shared secret */
    h235ModeDHgroup1024  = 0x00002000,  /* generate a 168 bit shared secret */
    h235ModeDHgroup1536  = 0x00004000,  /* generate a 168 bit shared secret */
    h235ModeDhInClear    = 0x80000000
} h235Mode_t;

typedef enum {
    h235ModeDiffieHellmanNone = 0x00, /* not using procedure */
    h235ModeDiffieHellman512  = 0x01, /* generate a 56 bit shared secret */
    h235ModeDiffieHellman1024 = 0x02, /* generate a 168 bit shared secret */
    h235ModeDiffieHellman1536 = 0x04  /* generate a 168 bit shared secret */
} h235DHMode_t;

#define RV_H235_MODE_TO_DH_MODE(_mode) ((h235DHMode_t)(((unsigned int)_mode)>>12))
#define RV_H235_DH_MODE_TO_MODE(_mode) ((h235Mode_t)(((unsigned int)_mode)<<12))
#define RV_H235_ALL_SEC_MODES ((h235Mode_t)(0x00000fff))
#define RV_H235_ALL_DH_MODES ((h235Mode_t)(0x000ff000))

typedef enum {
    h235StateUndefined,
    h235StateSecurityOK,
    h235StateSecurityFailed,
    h235StateSecurityIgnore
} h235State_t;

typedef enum {
    h235CertificateSHA1 = 0x00,
    h235CertificateMD5  = 0x01,
    h235CertificateURL  = 0x02
} h235CertificateType_t;


/*********************************************************************
 * reasons for success or fail of security check for incoming messages.
 *********************************************************************/

typedef enum {
    h235ReasonUndefined = 0,    /* no reason needed, message did not fail on sec check  */
    h235ReasonNoSecureRequired ,/* relevant when CheckIncomingMessages is RV_FALSE         */
    h235ReasonNoSecurityInfo,   /*the message does not contain any security information */
    h235ReasonMessageError,     /* some info required for security check is missing     */
    h235ReasonGeneralIDIncorrect,/*the generalID is missing or not equal our ID         */
    h235ReasonSendersIDMissing, /*the sendersID is missing                              */
    h235ReasonUnknownSender,    /*When this application is a gatekeeper:                */
                                /*    the sender entry was not returned by application  */
                                /*else: sender id is not the expected gkId              */
    h235ReasonProcNotSupported, /*incoming msg security mode is not supported           */
    h235ReasonTimeStampMissing, /*the timestamp in the message was invalid              */
    h235ReasonTimeStampFail,    /*the timestamp compare not close enougth               */
    h235ReasonTimeRandomFail,   /*random value is incorrect                             */
    h235ReasonIncorrectAuth,    /*the authenticator in the message was incorrect        */
    h235ReasonEncodeCBMissing,  /*the relevant encode callback not supplied by          */
                                /*the application (by h235SetEncodingCallback())        */
    h235ReasonAppForceFail,     /*the application forced failure                        */
    h235ReasonGeneralError,     /*general error                                         */
    h235ReasonEndToEndTokenSenderUnknown  /*received an end to end token, but no   */
                                /*certificate is known for the sender                   */
} h235Reason_t;

/****************************************************************************
 * Possible application replies when incoming message passed security check
 ***************************************************************************/
typedef enum {
    h235SuccessReplyForceFail = 100,     /*even though passed security, make it fail    */
    h235SuccessReplyDeleteMessage,       /*delete message and ignore it                 */
    h235SuccessReplyContinue             /*continue (message flow continues, the        */
                                         /*application can check results later)         */
} h235AppSuccessReply_t;

/****************************************************************************
 * Possible application replies when incoming message failed security check
 ***************************************************************************/
typedef enum {
    h235FailReplyForceOk = 200,          /* ignore the failure, continue with security checks */

    h235FailReplyDeleteMessage,          /* delete message and ignore it */

    h235FailReplyFreezeMessage,          /* freeze the message for now */

    h235FailReplyContinue                /* continue, the message will continue its flow as usual,
                                         but the application can later check if failed (use
                                         h235GetSecurityCheckResults..) */
} h235AppFailReply_t;


/****************************************
 * sendersId or GeneralId in BMP format
 ***************************************/
typedef struct {
  RvUint8 data[256];
  RvInt32 length;
} entityId_t;


/*================================================
 * EVENT HANDLERS PROTOTYPES
 ===============================================*/
/********************************************************************************
 * Type Name:  h235EvGetIDByRAS_t   (callback prototype)
 * Description : a callback to accept the ID of the target entity by RAS handle
 *               This callback is needed only when the application is a gatekeeper
 *               and for outgoing messages only.
 * Input:   hApp         - stack application handle.
 *          h235HApp     - h235 application handle
 *          hsRas        - rasHandle
 *          haRas        - application RAS handle
 * Output:  generalID    - the id of the other side, BMP format
 * Return: RV_TRUE if found, RV_FALSE if failed
 ********************************************************************************/
typedef RvBool (RVCALLCONV *h235EvGetIDByRAS_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  HRAS        hsRas,
                            IN  HAPPRAS     haRas,
                            OUT entityId_t  *generalID);


/********************************************************************************
 * Type Name:  h235EvGetIDByCall_t   (callback prototype)
 * Description : a callback to accept the ID of the target entity by Call handle
 *               This callback is needed when the application is a gatekeeper, for
 *               outgoing messages, and when an endpoint is using Procedure 3.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          hsCall      - callHandle    -
 *          haCall      - application call handle
 * Output:  generalID   - the id of the other side, BMP format
 * Return: RV_TRUE if found, RV_FALSE if failed
 ********************************************************************************/
typedef RvBool (RVCALLCONV *h235EvGetIDByCall_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  HCALL       hsCall,
                            IN  HAPPCALL    haCall,
                            OUT entityId_t  *generalID);


/********************************************************************************
 * Type Name:  h235EvGetIDByCall_t   (callback prototype)
 * Description : a callback to accept the ID of the target entity by Call handle
 *               This callback is needed when the application is a gatekeeper, for
 *               outgoing messages, and when an endpoint is using Procedure 3.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          hsCall      - callHandle    -
 *          haCall      - application call handle
 * Output:  senderID    - the id of the local side, BMP format
 *          generalID   - the id of the remote side, BMP format
 * Return: RV_TRUE if found, RV_FALSE if failed
 ********************************************************************************/
typedef RvBool (RVCALLCONV *h235EvGetIDsByCall_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  HCALL       hsCall,
                            IN  HAPPCALL    haCall,
                            OUT entityId_t  *senderID,
                            OUT entityId_t  *generalID);


/********************************************************************************
 * Type Name:  h235EvGetEntryByEndpointId_t   (callback prototype)
 * Description : This callback is needed only when the application is a gatekeeper.
 *               The gatekeeper application should store entries for h235, one entry
 *               per endpoint.  The structure of the entry is unknown to the gatekeeper.
 *               This callback retrieves the entry by id.
 *               If no such entry exist, the application should create a buffer
 *               (of size h235GetSizeofEntityEntry()) and return its pointer.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          endpointID  - The id by which the application search the entry (it is the id
 *                        which is sendersID in received messages, or generalID in sent messages)
 *
 * Output:  isNewEntry  - a flag says if the returned buffer this is a new allocation.
 *          ptr         - the buffer pointer
 * Return:
 ********************************************************************************/
typedef void (RVCALLCONV *h235EvGetEntryByEndpointId_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  entityId_t *endpointID,
                            OUT RvBool     *isNewEntry,
                            OUT H235HENTITY*ptr);


/********************************************************************************
 * Type Name:  h235EvChooseEndpointSecureMode_t   (callback prototype)
 * Description : This callback is needed only when the application is a gatekeeper.
 *               If an endpoint supports multiple security types, the gatekeeper
 *               should choose between them.  It may be called when the callback
 *               h235EvGetEntryByEndpointId_t creates a new entry.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          endpointID  - The id by which the application search the entry (it is the id
 *                        which is sendersID in received messages, or generalID in sent messages)
 *          secureMode  - Security modes supported by the EP
 *
 * Output:  secureMode  - Security modes chosen by the GK.
 *
 * Return:
 ********************************************************************************/
typedef void (RVCALLCONV *h235EvChooseEndpointSecureMode_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  entityId_t  *endpointID,
                            INOUT h235Mode_t *secureMode);


/********************************************************************************
 * Type Name:  h235EvChooseCallSecureMode_t   (callback prototype)
 * Description : If an endpoint supports multiple security types, and more than
 *               one is offered it should choose between them.
 * Input:   hsCall      - stack call handle
 *          haCall      - application call handle
 *          h235Call    - handle to the H.235 call information
 *          secureMode  - Security modes supported by the EP
 *
 * Output:  secureMode  - Security modes chosen by the EP.
 *
 * Return:
 ********************************************************************************/
typedef void (RVCALLCONV *h235EvChooseCallSecureMode_t)(
                            IN  HCALL       hsCall,
                            IN  HAPPCALL    haCall,
                            IN  H235HCALL   hSecCall,
                            INOUT h235Mode_t *secureMode);


/********************************************************************************
 * Type Name:  h235EvGetH235Call_t   (callback prototype)
 * Description : a callback to get the H.235 call information handle, if it exists.
 * Input:   hsCall      - Stack call handle
 *          haCall      - application call handle
 * Output:  none.
 * Return:  handle to the H.235 call information or NULL
 ********************************************************************************/
typedef H235HCALL (RVCALLCONV *h235EvGetH235Call_t)(
                            IN  HCALL       hsCall,
                            IN  HAPPCALL    haCall);


/********************************************************************************
 * Type Name:  h235EvGetKeyHandleById_t   (callback prototype)
 * Description : This callback is needed only when the application is a gatekeeper.
 *               This callback retrieves application data about a new endpoint, it is
 *               called when the callback h235EvGetEntryByEndpointId_t creates a new entry.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          endpointID  - The id by which the application serach the entry (it is the id
 *                        which is sendersID in received messages, or generalID in sent messages)
 * Output:  keyHandle   - a handle to the key(password) defined for the endpoint
 * Return: RV_TRUE if the keyHandle was found, RV_FALSE if not
 ********************************************************************************/
typedef RvBool (RVCALLCONV *h235EvGetKeyHandleById_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  entityId_t* endpointID,
                            OUT H235HKEY*   keyHandle);


/********************************************************************************
 * Type Name:  h235EvSetKeyHandle_t   (callback prototype)
 * Description : When using procedure 4, this callback invokes updating of the
 *               shared secret for the entity.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          newKey      - new 20-Byte shared secret
 *          keyHandle   - pointer to the key-handle previously given
 * Output:  keyHandle   - pointer to the key-handle for the new password (may stay unchanged)
 * Return:  none
 ********************************************************************************/
typedef void (RVCALLCONV *h235EvSetKeyHandle_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  RvUint8     newKey[20],
                            INOUT H235HKEY* keyHandle);


/********************************************************************************
 * Type Name:  h235EvGetCrtHandleById_t   (callback prototype)
 * Description : This callback is needed either when the application is a gatekeeper:
 *               This callback retrieves certificate handle for a new endpoint, it is
 *               called when the callback h235EvGetEntryByEndpointId_t creates a new entry;
 *               Or when using Procedure 3, to retrieve the remote endpoint's certificate.
 * Input:   hApp         - stack application handle.
 *          h235HApp     - h235 application handle
 *          endpointID   - The id by which the application search the entry (it is the id
 *                        which is sendersID in received messages, or generalID in sent messages)
 * Output:  crtInHandle  - a handle to the certificate for incoming messages
 *          crtOutHandle - a handle to the certificate for outgoing messages (not used in P3)
 * Return:  RV_TRUE if the keyHandle was found, RV_FALSE if not
 * Remark:  the crtOutHandle should be the this EP/GK's certificate,
 *          the crtInHandle should be the handle for the certificate of the other side,
 *          GK/Proxy/GW/Other EP for the EP; EP/Proxy/GW/Other GK for the GK
 ********************************************************************************/
typedef RvBool (RVCALLCONV *h235EvGetCrtHandleById_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  entityId_t* endpointID,
                            OUT H235HKEY*   crtInHandle,
                            OUT H235HKEY*   crtOutHandle);


/********************************************************************************
 * Type Name:  h235EvGetCrtByHandle_t   (callback prototype)
 * Description : This callback retrieves the certificate information for a given handle.
 *               called when a message is about to be sent.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          crtHandle   - a handle to the certificate defined for the endpoint. may be NULL if
 *                        this application is an endpoint.
 * Output:  certificate - the certificate buffer, NULL if it should not be set in the message
 *          size        - the certificate buffer size, 0 if it should not be set in the message
 *          type        - algorithm to use for signature
 * Return: RV_TRUE if the keyHandle was found, RV_FALSE if not
 ********************************************************************************/
typedef RvBool (RVCALLCONV *h235EvGetCrtByHandle_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  H235HKEY    h235hCrt,
                            OUT RvUint8**   certificate,
                            OUT RvSize_t*   size,
                            OUT h235CertificateType_t* type);


/********************************************************************************
 * Type Name:  h235EvReceivedToken_t   (callback prototype)
 * Description : This callback notifies the application as to an incoming end-
 *               to-end token, a media token, or an unrecognized token.
 *               Meant for proxies or GKs.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          endpointID  - the related endpoint ID (may be NULL)
 *          hCall       - the related call handle (may be NULL)
 *          hSecCall    - the related H.235 call handle (may be NULL)
 *          tokenType   - the type of the token (P3, DH, etc. None if unrecognized.)
 *          tokenId     - token PVT node Id
 * Return: RV_TRUE if the H.235 should handle the token, RV_FALSE if not
 * Remark: a GK should catch the Proc3 token and move it to the other side here.
 *         diffie-hellman tokens should be caught too, if GK is not an MC.
 ********************************************************************************/
typedef RvBool (RVCALLCONV *h235EvReceivedToken_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  HCALL       hCall,
                            IN  H235HCALL   hSecCall,
                            IN  entityId_t* endpointID,
                            IN  int         tokenType,
                            IN  RvPvtNodeId tokenId);


/********************************************************************************
 * Type Name:  h235EvFailure_t   (callback prototype)
 * Description : a callback to notify the application about security failure of
 *               incoming message.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          h235HMsg    - handle of the message check details, can be used
 *                      - to get more information about the message security check
 *          nodeId      - root node of incoming message
 *          senderId    - the sender id (if exists) from the message
 *          reason      - the security failure reason
 * Return: h235AppFailReply_t   (see comments in h235AppFailReply_t decleration)
 ********************************************************************************/
typedef h235AppFailReply_t (RVCALLCONV *h235EvFailure_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  H235HMSG    h235HMsg,
                            IN  int         nodeId,
                            IN  entityId_t *senderId,
                            IN  h235Reason_t reason);

/********************************************************************************
 * Type Name:  h235EvSuccess_t   (callback prototype)
 * Description : a callback to notify the application about security success of
 *               incoming message.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          h235HMsg    - handle of the message check details, can be used
 *                      - to get more information about the message security check
 *          nodeId      - root node of incoming message
 *          senderId    - the sender id (if exists) from the message
 * Return: h235AppSuccessReply_t   (see comments in h235AppSuccessReply_t decleration)
 ********************************************************************************/
typedef h235AppSuccessReply_t (RVCALLCONV *h235EvSuccess_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  H235HMSG    h235HMsg,
                            IN  int         nodeId,
                            IN  entityId_t *senderId);

/********************************************************************************
 * Type Name:  h235EvFreeze_t   (callback prototype)
 * Description : a callback to enable the application to freeze a message. the
 *      application should copy (note: COPY, not keep the pointer to) the bufLen,
 *      buffer, srcAddress and feed them to cmProtocolRecvRawMessageFrom
 *      when the password/certificate is avaliable
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          h235HMsg    - handle of the message check details, can be used
 *                      - to get more information about the message security check
 *          protConn    - connection handle for the message
 *          nodeId      - root node of incoming message
 *          buffer      - the message to be frozen
 *          bufLen      - length of the message to be frozen
 *          senderId    - the sender id (if exists) from the message
 *          srcAddress  - pointer to the source transport address
 * Output:  deleteMsg   - set to RV_TRUE if user wants message to be deleted
 * Return: RV_OK on success, negative on fail
 ********************************************************************************/
typedef RvStatus (RVCALLCONV *h235EvFreeze_t)(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  H235HMSG    h235HMsg,
                            IN  HPROTCONN   protConn,
                            IN  int         nodeId,
                            IN  RvUint8*    buffer,
                            IN  int         bufLen,
                            IN  entityId_t *senderId,
                            IN  cmRASTransport *srcAddress,
                            OUT RvBool     *deleteMsg);


/********************************************************************************
 * Type Name:  h235EvEncode_t    (callback prototype)
 * Description : a callback to accept encode results from the Application
 *               for a specific message.
 *               for example: the results of the HMAC-SHA1-96 algorithm.
 * Input:
 *        keyHandle - that was accepted from the Application by h235EvGetKeyHandleById_t callback
 *        buf       - The message buffer
 *        bufLen    -
 * Output:digest    - the value calculated by the application encode function
 *        digestLen - length(in bytes) of digest
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
typedef int (RVCALLCONV *h235EvEncode_t)(
                        IN H235HKEY         keyHandle,
                        IN void *           Buf,
                        IN int bufLen,
                        OUT unsigned char * digest,
                        OUT int *           digestLen);


typedef struct
{
    /******************************************************/
    /* these callbacks are relevant for all applications  */
    /* (endpoints and gatekeepers)                        */
    /* If the callback pointer is NULL, or not set, h235  */
    /* will behave as if  replyContinue  was returned.    */
    /******************************************************/
    h235EvFailure_t         evFailure;      /* OPTIONAL, can set NULL */
    h235EvSuccess_t         evSuccess;      /* OPTIONAL, can set NULL */
    h235EvFreeze_t          evFreeze;       /* OPTIONAL, can set NULL */

    /********************************************************************/
    /* these callbacks are required from gatekeeper                     */
    /* applications only, not relevant for endpoint applications        */
    /* All of them are MANDATORY for gatekeeper apps, NULL for others   */
    /********************************************************************/
    h235EvGetIDByRAS_t                  evGetIDByRAS;
    h235EvGetIDByCall_t                 evGetIDByCall;
    h235EvGetH235Call_t                 evGetH235Call;
    h235EvGetEntryByEndpointId_t        evGetEntryByEndpointId;
    h235EvChooseEndpointSecureMode_t    evChooseEndpointSecureMode;
    h235EvChooseCallSecureMode_t        evChooseCallSecureMode;
    h235EvGetKeyHandleById_t            evGetKeyHandleById;
    h235EvSetKeyHandle_t                evSetKeyHandle;
    h235EvGetCrtHandleById_t            evGetCrtHandleById;
    h235EvGetCrtByHandle_t              evGetCrtByHandle;
    h235EvReceivedToken_t               evReceivedToken;
    h235EvGetIDsByCall_t                evGetIDsByCall;
} h235EventHandlers_T;


/*=============================================
 * Function prototypes
 ==============================================*/
/********************************************************************************
 * Function Name:  h235SetEventHandlers
 * Description :
 *               Sets the event handlers (callbacks) the application should supply.
 *               For more information see h235EventHandlers_T definition.
 * Input: h235HApp          - h235 instance handle.
 *        eventHandlers     - the callbacks structure
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetEventHandlers(
    IN  H235HAPP h235HApp,
    IN  h235EventHandlers_T     *eventHandlers);


/********************************************************************************
 * Function Name:  h235Init
 * Description : Initiates h235 module instance
 *
 * Input: hApp      - stack application handle.
 * Output:h235HApp  - The new h235 handle
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235Init(IN  HAPP hApp,OUT H235HAPP *h235HApp);

/********************************************************************************
 * Function Name:  h235End
 * Description : close and delete h235 module instance
 *
 * input:       h235 application handle
 * Return:      0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235End(IN   H235HAPP h235HApp);


/************************************************************************
 * h235GetHAPP
 * purpose: Get the stack handle of a stack instance for a
 *          h235 application handle
 * input:       h235 application handle
 * output : none
 * return : The stack instance handle of the h235 instance handle on success
 *          NULL on failure
 ************************************************************************/
RVAPI
HAPP RVCALLCONV h235GetHAPP(
    IN  H235HAPP h235HApp);


/********************************************************************************
 * Function Name:  h235SetSecureMode
 * Description : Sets the secure mode for sent messages.
 *               This function can be called any time while exceuting
 *               to change the current mode.
 *               Until this function is called, the default mode is h235ModeNone.
 * Input: h235HApp          - h235 instance handle.
 *        secureMode        - The required secure mode
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetSecureMode(
    IN  H235HAPP        h235HApp,
    IN  h235Mode_t    secureMode);

/********************************************************************************
 * Function Name:  h235GetSecureMode
 * Description : Gets the secure mode for sent messages.
 * Input: h235HApp          - h235 instance handle.
 * Return: secureMode       - The  secure mode
 ********************************************************************************/
RVAPI
h235Mode_t RVCALLCONV h235GetSecureMode(
    IN  H235HAPP        h235HApp);

/********************************************************************************
 * Function Name:  h235SetCheckIncomingMessages
 * Description : Sets whether incoming messages should pass security check.
 *               This function can be called any time while exceuting.
 *               Until this function is called, the default value is RV_FALSE.
 * Input: h235HApp          - h235 instance handle.
 *        isCheck           - RV_TRUE: check, RV_FALSE: dont check
  * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetCheckIncomingMessages(
    IN  H235HAPP        h235HApp,
    IN  RvBool          isCheck);

/********************************************************************************
 * Function Name:  h235GetCheckIncomingMessages
 * Description : Gets whether incoming messages should pass security check.
 * Input: h235HApp          - h235 instance handle.
 *
 * Return: isCheck          - RV_TRUE: check, RV_FALSE: dont check
 ********************************************************************************/
RVAPI
RvBool RVCALLCONV h235GetCheckIncomingMessages(
    IN  H235HAPP        h235HApp);

/********************************************************************************
 * Function Name:  h235SetSenderId
 * Description : Sets the sendersId.
 *               This value is used for purposes:
 *                  1. Set as the sendersId in sent messages.
 *                  2. Compared with generalId from received messages.
 * Input: h235HApp          - h235 instance handle.
 *        senderId          - the id of this application.
  * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetSenderId(
    IN  H235HAPP        h235HApp,
    IN  entityId_t      *senderId);

/********************************************************************************
 * Function Name:  h235GetSenderId
 * Description : Gets the sendersId.
 * Input: h235HApp          - h235 instance handle.
 * Output:senderId          - the id of this application.
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetSenderId(
    IN  H235HAPP        h235HApp,
    OUT entityId_t      *senderId);

/********************************************************************************
 * Function Name:  h235SetGatekeeperId
 * Description : This function must be called only by non-gatekeeper applications.
 *               By calling this function, you declare your application to be an
 *               endpoint application, if not called, h235 assumes it is a gatekeeper
 *               application.
 *               The gatkeeperId is used for purposes:
 *                  1. Set as the generalId in sent messages.
 *                  2. Compared with sendersId from received messages.
 * Input: h235HApp          - h235 instance handle.
 *        gatkeeperId       - the id
 *        keyHandle         - key handle to be used with messages sent to
 *                            the gatekeeper or received from it.
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetGatekeeperId(
    IN  H235HAPP        h235HApp,
    IN  entityId_t*     gatkeeperId,
    IN  H235HKEY        keyHandle);

/********************************************************************************
 * Function Name:  h235GetGatekeeperId
 * Description : Gets the GatekeeperId.
 * Input: h235HApp          - h235 instance handle.
 * Output:gatkeeperId       - the id
 *        keyHandle         - the key handle
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetGatekeeperId(
    IN  H235HAPP        h235HApp,
    OUT entityId_t      *gatkeeperId,
    OUT H235HKEY        *keyHandle);

/********************************************************************************
 * Function Name:  h235SetCertificateHandles
 * Description : This function must be called only by non-gatekeeper applications.
 *          It sets the certificate handles to be used.
 * Input:   h235HApp          - h235 instance handle.
 *          crtInHandle  - a handle to the certificate for incoming messages
 *          crtOutHandle - a handle to the certificate for outgoing messages
 * Return:  RV_OK - 0k , negative - failed
 * Remark:  the crtOutHandle should be this EP's certificate, the crtInHandle
 *          should be the handle for the certificate of the other side, GK/Proxy/GW.
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetCertificateHandles(
    IN  H235HAPP    h235HApp,
    IN  H235HKEY    crtInHandle,
    IN  H235HKEY    crtOutHandle);

/********************************************************************************
 * Function Name:  h235SetCertificateHandles
 * Description : This function can be called only by non-gatekeeper applications.
 *          It gets the certificate handles used.
 * Input:   h235HApp     - h235 instance handle.
 * Output:  crtInHandle  - the handle to the certificate for incoming messages
 *          crtOutHandle - the handle to the certificate for outgoing messages
 * Return:  RV_OK - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetCertificateHandles(
    IN  H235HAPP    h235HApp,
    OUT H235HKEY*   crtInHandle,
    OUT H235HKEY*   crtOutHandle);

/********************************************************************************
 * Function Name:  h235SetTimeSync
 * Description : Sets your time synchronization check policy.
 *               This is about the compare of time when the message was sent,
 *               and the time it was received by this application.
 * Input: h235HApp          - h235 instance handle.
 *        performTimeSync   - RV_TRUE: compare times, RV_FALSE: dont compare
 *        timeDiff          - The difference between time of creation and time
 *                            of receiving a message (in seconds), that you still
 *                            consider as close enough to pass the security check.
 *                            (this value is not defined in the standard, and is
 *                            for application decision.
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetTimeSync(
    IN  H235HAPP        h235HApp,
    IN  RvBool          performTimeSync,
    IN  RvUint32        timeDiff);

/********************************************************************************
 * Function Name:  h235GetTimeSync
 * Description :
 *               Gets the timediff settings
 * Input: h235HApp          - h235 instance handle.
 * Output:performTimeSync   - RV_TRUE: compare times, RV_FALSE: dont compare
 *        timeDiff          - The time difference
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetTimeSync(
    IN  H235HAPP        h235HApp,
    OUT RvBool         *performTimeSync,
    OUT RvUint32       *timeDiff);

/********************************************************************************
 * Function Name:  h235GetSizeofEntityEntry
 * Description :
 *               Returns the buffer size that the gatekeeper application should allocate
 *               when it returns a pointer via the h235EvGetEntryByEndpointId_t callback
 * Input: none
 * Return: the size
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetSizeofEntityEntry(void);

/********************************************************************************
 * Function Name:  h235GetSizeofCallInfo
 * Description :
 *               Returns the buffer size that the application should allocate
 *               for a H235 call object
 * Input: none
 * Return: the size
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetSizeofCallInfo(void);

/********************************************************************************
 * Function Name:  h235GetSizeofEntityEntryNoDH
 * Description :
 *               Returns the buffer size that the gatekeeper application should allocate
 *               when it returns a pointer via the h235EvGetEntryByEndpointId_t callback
 *               without the size of the Diffie-Hellman component used for Procedure 4.
 * Input: none
 * Return: the size
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetSizeofEntityEntryNoDH(void);

/********************************************************************************
 * Function Name:  h235GetSizeofCallInfoNoDH
 * Description :
 *               Returns the buffer size that the application should allocate
 *               for a H235 call object without the size of the Diffie-Hellman
 *               component used for Media Security
 * Input: none
 * Return: the size
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetSizeofCallInfoNoDH(void);

/********************************************************************************
 * Function Name:  h235SetEncodingCallback
 * Description : sets the encoding callback functions
 *
 * Input: h235HApp          - h235 instance handle.
 *        callback          - a pointer to the callback function
 *        algorithmOIDstr   - the relevant Object ID string that identifies
 *                            the algorithm applied by the callback.
 *                            You may use OID_U_STRING to identify the  HMAC-SHA1-96 algorithm
 *                            (this algorithm is required by procedure 1), other algorithms
 *                            are for future use.
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetEncodingCallback(
    IN  H235HAPP        h235HApp,
    IN  h235EvEncode_t  callback,
    IN  const char      *algorithmOIDstr);

/********************************************************************************
 * Function Name:  h235GetSecurityCheckResultsByRAS
 * Description : Gets the results of security check.
 *               This function allows you to check the results of the security check.
 * Input: h235HApp          - h235 instance handle.
 *        hsRas             - a RAS handle, this function will retreive the security check results
 *                            of the last received message related to this RAS handle.
 * Output:
 *        state             - security check results, can pass NULL value
 *        reason            - security fail or success reason, can pass NULL value
 *        h235HMsg          - handle of the security check results.
 *                            This handle can be used to get
 *                            more details about the results (e.g. h235GetModeByHMSG()).
 *                            Can pass NULL value
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetSecurityCheckResultsByRAS(
    IN  H235HAPP        h235HApp,
    IN  HRAS            hsRas,
    OUT h235State_t     *state,
    OUT h235Reason_t    *reason,
    OUT H235HMSG        *h235HMsg);

/********************************************************************************
 * Function Name:   h235GetSecurityCheckResultsByCall
 * Description :    Gets the results of security check.
 *                  This function allows you to check the results of the security check.
 * Input: h235HApp          - h235 instance handle.
 *        hsCall            - a call handle, this function will retreive the security check results
 *                            of the last received message related to this call.
 * Output:
 *        state             - security check results, can pass NULL value
 *        reason            - security fail or success reason, can pass NULL value
 *        h235HMsg          - handle of the security check results, can be used to get
 *                            more details about the results (e.g. h235GetModeByHMSG()).
 *                            Can pass NULL value
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetSecurityCheckResultsByCall(
    IN  H235HAPP        h235HApp,
    IN  HCALL           hsCall,
    OUT h235State_t     *state,
    OUT h235Reason_t    *reason,
    OUT H235HMSG        *h235HMsg);

/********************************************************************************
 * Function Name:   h235RecheckRasMessage
 * Description :    Starts the security check on a RAS message again.
 * Input: h235HApp          - h235 instance handle.
 *        protConn          - connection handle for the message
 *        messageBuf        - the message buffer
 *        messageLen        - length on the message in bytes
 *        srcAddress        - the source address of the message
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235RecheckRasMessage(
    IN  H235HAPP        h235HApp,
    IN  HPROTCONN       protConn,
    IN  RvUint8         *messageBuf,
    IN  int             messageLen,
    IN  cmRASTransport  *srcAddress);

/********************************************************************************
 * Function Name:   h235GetStateByHMSG
 * Description :    Gets the security state of an incoming message.
 * Input: h235HMsg          - handle of message security results
 * Output:state             - the state of security check (pass/fail)
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetStateByHMSG(
    IN  H235HMSG        h235HMsg,
    OUT h235State_t     *state);

/********************************************************************************
 * Function Name:   h235GetReasonByHMSG
 * Description :    Gets the security state reason of an incoming message.
 * Input: h235HMsg          - handle of message security results
 * Output:reason            -
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetReasonByHMSG(
    IN  H235HMSG        h235HMsg,
    OUT h235Reason_t    *reason);

/********************************************************************************
 * Function Name:   h235GetModeByHMSG
 * Description :    Gets the security mode of an incoming message.
 * Input: h235HMsg          - handle of message security results
 * Output:secureMode        - the mode of security which the message was secured with.
 *                            If the message has no security info, or the mode is unfamiliar
 *                            to RV h235, h235ModeNone will be returned.
  * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetModeByHMSG(
    IN  H235HMSG        h235HMsg,
    OUT h235Mode_t      *secureMode);


/********************************************************************************
 * Function Name:   h235ReasonName
 * Description :    translates reason code to a name
 * Input:   reason              - reason code
 * Return:  the name string
 ********************************************************************************/
RVAPI
const char* RVCALLCONV h235ReasonName(
    IN   h235Reason_t reason);

/********************************************************************************
 * Function Name:   h235ModeName
 * Description :    translates mode code to a name
 * Input:   mode            - mode code
 * Return:  the name string
 ********************************************************************************/
RVAPI
const char* RVCALLCONV h235ModeName(
    IN   h235Mode_t mode);

/********************************************************************************
 * Function Name:   h235StateName
 * Description :    translates state code to a name
 * Input:   state           - state code
 * Return:  the name string
 ********************************************************************************/
RVAPI
const char* RVCALLCONV h235StateName(
    IN   h235State_t    state);


/***********************************************************************************
 * Routine Name:  h235InitCallObject
 * Description : initialize the H.235 call structs
 * Input: h235HApp  - h235 instance handle.
 *        hSecCall  - handle to h235 call info.
 * Output: none.
 * Return: 0 - 0k , negative - failed
 ***********************************************************************************/
RVAPI
int RVCALLCONV h235InitCallObject(
    IN  H235HAPP      h235HApp,
    IN  H235HCALL     hSecCall);


/***********************************************************************************
 * Routine Name:  h235EndCallObject
 * Description : end the H235 call structs
 * Input: h235HApp  - h235 instance handle.
 *        hSecCall  - handle to h235 call info.
 * Output: none
 * Return: 0 - 0k , negative - failed
 ***********************************************************************************/
RVAPI
int RVCALLCONV h235EndCallObject(
    IN  H235HAPP      h235HApp,
    IN  H235HCALL     hSecCall);


/***********************************************************************************
 * Routine Name:  h235SetCallMode
 * Description : set the H.235 call security mode
 * Input: hSecCall   - handle to h235 call info.
 *        secureMode - security mode to use.
 * Output: none.
 * Return: none
 ***********************************************************************************/
RVAPI
void RVCALLCONV h235SetCallMode(
    IN  H235HCALL     hSecCall,
    IN  h235Mode_t    secureMode);


/***********************************************************************************
 * Routine Name:  h235GetCallMode
 * Description : get the H.235 call security mode
 * Input: hSecCall   - handle to h235 call info.
 * Output: none.
 * Return: security mode used.
 ***********************************************************************************/
RVAPI
h235Mode_t RVCALLCONV h235GetCallMode(
    IN  H235HCALL     hSecCall);


/***********************************************************************************
 * Routine Name:  h235CallSendCryptoTokenOnCall
 * Description : set a token to be send with the next outgoing message
 * Input: h235HApp    - h235 instance handle.
 *        hSecCall    - handle to h235 call info.
 *        cryptoToken - PVT node to send
 * Output: none.
 * Return: RV_OK - 0k , negative - failed
 ***********************************************************************************/
RVAPI
RvStatus RVCALLCONV h235CallSendCryptoTokenOnCall(
    IN  H235HAPP      h235HApp,
    IN  H235HCALL     hSecCall,
    IN  RvPvtNodeId   cryptoToken);


/***********************************************************************************
 * Routine Name:  h235GenerateDiffieHellman
 * Description : generate the Diffie Hellman HalfKey
 * Input:  h235HApp    - h235 instance handle.
 *         DHMode      - Diffie-Hellman mode needed.
 *         hSecCall    - handle to h235 call info.
 * Output: none.
 * Return: 0 - 0k , negative - failed
 ***********************************************************************************/
RVAPI
int RVCALLCONV h235GenerateDiffieHellman(
    IN  H235HAPP       h235HApp,
    IN  h235DHMode_t   DHMode,
    IN  H235HCALL      hSecCall);


/***********************************************************************************
 * Routine Name:  h235AddDiffieHellman
 * Description : add the Diffie Hellman HalfKey to messageRoot
 * Input:  h235HApp    - h235 instance handle.
 *         DHMode      - mode of DH to use. irrelevent if replying
 *         messageRoot - the message node Id
 *         hsCall      - stack call handle.
 *         hSecCall    - handle to h235 call info.
 * Output: none
 * Return: 0 - 0k , negative - failed
 ***********************************************************************************/
RVAPI
int RVCALLCONV h235AddDiffieHellman(
    IN  H235HAPP      h235HApp,
    IN  h235DHMode_t  DHMode,
    IN  RvPvtNodeId   messageRoot,
    IN  HCALL         hsCall,
    IN  H235HCALL     hSecCall);


/********************************************************************************
 * Function Name:   h235GetIncomingDiffieHellmanHalfKey
 * Description :    Gets the Diffie Hleman HalfKey from an incoming message
 * Input: h235HApp    - h235 instance handle.
 *        messageNode - the incoming message from which to get the DH HalfKey
 *        hSecCall       - handle to h235 call info.
 *        dhMode      - type of incoming Diffie-Hellmanto look for, h235ModeNone
 *                      for any node.
 * Output: dhMode     - mode of the incoming Diffie-Hellman
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetIncomingDiffieHellmanHalfKey(
    IN  H235HAPP      h235HApp,
    IN  RvPvtNodeId   messageNode,
    IN  H235HCALL     hSecCall,
    INOUT h235DHMode_t *dhMode);


/********************************************************************************
 * Function Name:   h235GenarateDiffieHellmanSharedSecret
 * Description :    Gets the Diffie Hleman HalfKey from an incoming message
 * Input:  hSecCall              - handle to h235 call info.
 *         leastSignificantFirst - true iff the least significant should be the first byte.
 * Output: SharedSecret          - buffer containing the generated Shared Secret,
 *                        it is expected to be of enough size for the protocol
 * Return: 0 - 0k , negative - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GenarateDiffieHellmanSharedSecret(
    IN  H235HCALL     hSecCall,
    IN  RvBool        leastSignificantFirst,
    OUT char *        sharedSecret);

/********************************************************************************
 * Function Name:   h235GetVersion
 * Description :    returnes the H.235 version
 ********************************************************************************/
RVAPI
const char* RVCALLCONV h235GetVersion(void);


//{{gaoshizhong 20131125 Add
RVAPI
void RVCALLCONV ConvertUint16ToChar(unsigned char * destBuf,RvUint16 * srcBuf,int convertsrcLen);

RVAPI
void RVCALLCONV ConvertCharToUint16(RvUint16 * destBuf,unsigned char * srcBuf,int convertsrcLen);

RVAPI
void RVCALLCONV ConvertCharToHexChar(unsigned char * destBuf,unsigned char * srcBuf,int convertsrcLen);

RVAPI
void RVCALLCONV ConvertHexCharToChar(unsigned char * destBuf,unsigned char * srcBuf,int convertsrcLen);
//}}gaoshizhong 20131125 Add




#endif

#ifdef __cplusplus
}
#endif

