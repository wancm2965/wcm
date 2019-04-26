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

#ifndef _RV_EP_DEFS_H_
#define _RV_EP_DEFS_H_

/***********************************************************************
epDefs.h

Definitions required for running an endpoint application.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/

#include "cm.h"
#include "mti.h"

#ifdef USE_SECURITY
#include "h235_api.h"
#endif

#ifdef USE_H450
#include "sse.h"
#endif

#ifdef USE_RTP
#include "rtp.h"
#endif

#ifdef USE_LDAP
#include "rvh323ldap.h"
#define LDAP_H235_CONTEXT_SIZE sizeof(RvH323LdapH235Context)
#else
#define LDAP_H235_CONTEXT_SIZE 0
#endif

#ifdef USE_FWNAT
#include "fwnat_api.h"
#endif

#ifdef USE_PLAYER
#include "lepPlayMediaDefs.h"
#endif

/* The following are here for various operating systems */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* Declare the version of H.323 Protocol Stack we're using */
/* This will result the exact set of definitions we have declared */
#define RV_H323_VERSION_DEFINITIONS 500


#ifdef __cplusplus
extern "C" {
#endif



/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/


typedef struct EpObj_tag EpObj;
typedef struct EpCallObj_tag EpCallObj;
typedef struct EpChannelObj_tag EpChannelObj;


RV_DECLARE_HANDLE(HashHANDLE);
RV_DECLARE_HANDLE(RtpHANDLE);
RV_DECLARE_HANDLE(LockHANDLE);


typedef enum
{
    EpResourceCall,             /* Call object */
    EpResourceChannel,          /* Channel object */
    EpResourceInConnection,     /* Incoming connection */
    EpResourceOutConnection,    /* Outgoing connection */
    EpResourceCompletionService,/* H.450 completion service */
    EpResourceLast /* nuber or resources */
} EpResourceType;


/* Possible actions on media channels. */
typedef enum
{
    epRtpActionNone, /* Do nothing with channels */
    epRtpActionReplay /* Replay incoming channels back to sender */
} EpRtpAction;



/******************************************************************************
 * epMallocEv
 * ----------------------------------------------------------------------------
 * General: Dynamic allocation function to call (the equivalent of malloc()).
 *
 * Return Value: Pointer to allocated memory on success, NULL on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         size         - Size of allocation requested.
 *         filename     - Name of the file doing the alloc.
 *         lineno       - Line number doing the alloc.
 * Output: None.
 *****************************************************************************/
typedef void* (*epMallocEv)(
    IN EpObj                *ep,
    IN RvSize_t             size,
    IN const RvChar         *filename,
    IN RvInt32              lineno);


/******************************************************************************
 * epFreeEv
 * ----------------------------------------------------------------------------
 * General: Request user to allocate a resource identifier for an object.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         ptr          - Pointer of memory to free.
 * Output: None.
 *****************************************************************************/
typedef RvStatus (*epFreeEv)(
    IN EpObj                *ep,
    IN void                 *ptr);


/******************************************************************************
 * epAllocateResourceIdEv
 * ----------------------------------------------------------------------------
 * General: Request user to allocate a resource identifier for an object.
 *
 * Return Value: Allocated resource on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         resourceType - Type of resource we need id for.
 * Output: None.
 *****************************************************************************/
typedef RvInt32 (*epAllocateResourceIdEv)(
    IN EpObj                *ep,
    IN EpResourceType       resourceType);


/******************************************************************************
 * epRegistrationStatusEv
 * ----------------------------------------------------------------------------
 * General: Indication of a change in the registration status.
 *          This indication is received before the actual event from the Stack
 *          is handled by cmEvRegEvent().
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         newState     - The new registration state.
 * Output: None.
 *****************************************************************************/
typedef void (*epRegistrationStatusEv)(
    IN EpObj                *ep,
    IN cmRegState           newState);


/******************************************************************************
 * epCallNewEv
 * ----------------------------------------------------------------------------
 * General: Indication of a new call object that was created as is being used.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - The call object.
 *         isOutgoing   - Is this an incoming or an outgoing call?
 * Output: None.
 *****************************************************************************/
typedef void (*epCallNewEv)(
    IN EpCallObj            *call,
    IN RvBool               isOutgoing);


/******************************************************************************
 * epCallCloseEv
 * ----------------------------------------------------------------------------
 * General: Indication of a call closure.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - The call object.
 * Output: None.
 *****************************************************************************/
typedef void (*epCallCloseEv)(
    IN EpCallObj            *call);


/******************************************************************************
 * epChannelNewEv
 * ----------------------------------------------------------------------------
 * General: Indication of a new channel object that was created and should be
 *          displayed in the GUI.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  chan         - The channel object.
 * Output: None.
 *****************************************************************************/
typedef void (*epChannelNewEv)(
    IN EpChannelObj         *chan);


/******************************************************************************
 * epChannelStableEv
 * ----------------------------------------------------------------------------
 * General: Indication that a channel reached a stable state - either connected
 *          or idle.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  chan         - The channel object.
 * Output: None.
 *****************************************************************************/
typedef void (*epChannelStableEv)(
    IN EpChannelObj         *chan);


/******************************************************************************
 * epMessageEv
 * ----------------------------------------------------------------------------
 * General: Indication of a message being sent or received by the endpoint.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep       - Endpoint object to use.
 *         call     - Call this message belongs to (NULL if none).
 *         nodeId   - Message node id.
 *         name     - Message's name.
 *         isSend   - RV_TRUE for send, RV_FALSE for receive.
 * Output: None.
 *****************************************************************************/
typedef void (*epMessageEv)(
    IN EpObj                *ep,
    IN EpCallObj            *call,
    IN RvPvtNodeId          nodeId,
    IN const RvChar         *name,
    IN RvBool               isSend);


/******************************************************************************
 * epEventIndicationEv
 * ----------------------------------------------------------------------------
 * General: Indication of a message being sent or received by the endpoint.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         eventType        - Type of the event indicated.
 *         call             - Call this event belongs to (NULL if none).
 *         eventStr         - Event string.
 * Output: None.
 *****************************************************************************/
typedef void (*epEventIndicationEv)(
    IN EpObj                *ep,
    IN const RvChar         *eventType,
    IN EpCallObj            *call,
    IN const RvChar         *eventStr);


/******************************************************************************
 * epLogEv
 * ----------------------------------------------------------------------------
 * General: Indication of a message that can be logged somewhere.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         call             - Call this event belongs to (NULL if none).
 *         logMessage       - Log message.
 * Output: None.
 *****************************************************************************/
typedef void (*epLogEv)(
    IN EpObj                *ep,
    IN EpCallObj            *call,
    IN const RvChar         *logMessage);


/******************************************************************************
 * epInitLockEv
 * ----------------------------------------------------------------------------
 * General: Indicates a request to create an application lock.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: lock             - pointer to the lock object.
 *****************************************************************************/
typedef void (*epInitLockEv)(
    IN EpObj       *ep,
    OUT LockHANDLE *lock);


/******************************************************************************
 * epEndLockEv
 * ----------------------------------------------------------------------------
 * General: Indicates a request to destroy the application lock.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         lock             - pointer to the lock object.
 * Output: None.
 *****************************************************************************/
typedef void (*epEndLockEv)(
    IN EpObj      *ep,
    IN LockHANDLE lock);


/******************************************************************************
 * epLockEv
 * ----------------------------------------------------------------------------
 * General: Indicates a request to lock the application lock.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         lock             - pointer to the lock object.
 * Output: None.
 *****************************************************************************/
typedef void (*epLockEv)(
    IN EpObj      *ep,
    IN LockHANDLE lock);


/******************************************************************************
 * epUnlockEv
 * ----------------------------------------------------------------------------
 * General: Indicates a request to unlock the application lock.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         lock             - pointer to the lock object.
 * Output: None.
 *****************************************************************************/
typedef void (*epUnlockEv)(
    IN EpObj      *ep,
    IN LockHANDLE lock);

#define lepLockCall(_call) if (_call->ep->cb.epLock != NULL) _call->ep->cb.epLock(_call->ep, _call->lock)
#define lepUnlockCall(_call) if (_call->ep->cb.epLock != NULL) _call->ep->cb.epUnlock(_call->ep, _call->lock)
#define lepUnlockCallForAPI(_call, _api) if (_call->ep->cb.epLock != NULL) { _call->ep->cb.epUnlock(_call->ep, _call->lock); _api; _call->ep->cb.epLock(_call->ep, _call->lock); } else _api
#define lepLockEpForHash(_ep, _api) if (_ep->cb.epLock != NULL) { _ep->cb.epLock(_ep, _ep->lock); _api; _ep->cb.epUnlock(_ep, _ep->lock); } else _api



/* List of this module's callbacks */
typedef struct
{
    epMallocEv                  epMalloc;
    epFreeEv                    epFree;
    epAllocateResourceIdEv      epAllocateResourceId;
    epRegistrationStatusEv      epRegistrationStatus;
    epCallNewEv                 epCallNew;
    epCallCloseEv               epCallClose;
    epChannelNewEv              epChannelNew;
    epChannelStableEv           epChannelStable;
    epMessageEv                 epMessage;
    epEventIndicationEv         epEventIndication;
    epLogEv                     epLog;
    epInitLockEv                epInitLock;
    epLockEv                    epLock;
    epUnlockEv                  epUnlock;
    epEndLockEv                 epEndLock;
} EpCallbacks;



/* Call object */
struct EpCallObj_tag
{
    EpObj               *ep; /* Endpoint's object */
    RvInt32             id; /* Id given to the call */

    HCALL               hsCall; /* Call handle in the CM for this call */
    int                 numOfChannels; /* Number of channels opened in this call */
    EpChannelObj        *firstChannel; /* First channel opened (regarded as a linked-list) */
    EpChannelObj        *lastChannel; /* Last channel opened (regarded as a linked-list) */
    RvBool              sendComplete; /* Indicates if we have to set "sendingComplete"
                                         field inside SETUP and INFORMATION messages */
    RvBool              setCalledPartNumber; /* RV_TRUE if we already set the called party number */
    HPROTCONN           hCon; /* Connection used for this call. Used for coloring the
                                 connection to distinguish those calls that are multiplexed */
    cmCallState_e       state; /* Latest call state that was indicated */
    cmCallStateMode_e   stateMode; /* Latest state mode that was indicated */

    RvBool              isIndependent; /* RV_TRUE for independent calls, not handled too much by this library */
    RvBool              isIncoming; /* RV_TRUE if the call is an incoming call
                                       RV_FALSE if the call is an outgoing call */
    RvBool              endSessionSent; /* RV_TRUE if the call sent an end session command message
                                           RV_FALSE if the call did not send an end session command yet */
    RvBool              isFastConnectCall; /* RV_TRUE if this call is a fast-start call */
    RvBool              isFastConnectChannels; /* RV_TRUE currently raising events for FS channels */
    RvBool              initFsOnDialTone; /* RV_TRUE if init the FS on the dial-tone state (used after transfering) */
    EpRtpAction         action; /* RTP action to use on this call's channels */
    int                 rtpSessions[3]; /* List of offered RTP sessions for faststart use */
    int                 muxedRtpSessions[3]; /*List of offered muxed RTP sessions for faststart use */
    RvUint32            multiplexID[3];     /*List of multiplexIDs of sessions for faststart use*/
    RvUint32            remoteMultiplexID[3]; /*List of multiplexIDs of remote side for faststart use*/
    RvBool              bRemoteMediaMultiplexEnabled;   /*RV_TRUE if remote H46019 peer can
                                                          transmit multiplexed media*/
    RvBool              secureMedia; /* RV_TRUE if we're encrypting the media (RTP) */

    cmTransportAddress  remoteAddress;
    RvBool              separateH245;
    RvBool              wasConnected; /* Was this call ever connected? */
    RvBool              wasConnectedControl; /* Did this call have control? */

#ifdef USE_H450
    HSSECALL            hSSECall; /* used for H450: Call handle for SSE */
    RvInt32             h450outCall; /* counter of outgoing call if available */
    EpCallObj           *linkedCall; /* Linked call object. Used for some "juggling" of the application */
    RvBool              bParkNotifiedGroup; /* Was this call notified to parking group? */
#endif

#ifdef USE_SECURITY
    H235HCALL           hSecCall; /* Security information handle of this call */
    RvChar              sharedSecret[21]; /* Shared secret we got from Diffie-Hellman */
    RvChar              secVector[7]; /* Security vector to start with */
    RvBool              useDiffieHellman; /* RV_TRUE if we want to use the Diffie-Hellman
                                             procedure when connecting a call */
    entityId_t          remoteEndpoint;
#endif
    LockHANDLE          lock;
#ifdef USE_FWNAT
    HH323FWNATCONN      hFwNatConn;
    RvBool              bUseH46019;                     /*Indicates the use of H46019*/
    RvBool              bMultiplexParamsRcvd[3];       /*RV_TRUE if rtaversal parameters received 
                                                         for FS sessions*/
    cmTransportAddress  rtpSessionsAddr[3];            /*"Real" RTP address of client to be saved 
                                                         per server's rtp session in FS calls when first 
                                                         keepAlive is received before traversal
                                                         parameters are received for the session.*/
#endif

#ifdef USE_PLAYER
    PlayerCallParams    PlayerInfo;
    EpChannelObj        *RecChan[2];    /* Pointers to the 2 channel structures*/
    EpChannelObj        *in[10];
    EpChannelObj        *out[10];
#endif
};



/* Channel object */
struct EpChannelObj_tag
{
    EpCallObj               *call; /* Call object of this channel */
    RvInt32                 id; /* Id given to the channel */
    EpChannelObj            *nextChannel; /* Next channel that belongs to this call */    
    EpChannelObj            *prevChannel; /* Previous channel that belongs to this call */    

    cmChannelState_e        state; /* Channel's state */
    cmChannelStateMode_e    stateMode; /* Channel's state mode */
    RvBool                  mark; /* Used for printing the channels */
    RvChar                  dataTypeName[20]; /* Name of the data type used in this channel */
    RvInt32                 rtpSession; /* RTP session used for this channel */
    RvUint32                multiplexID;
    RvUint32                remoteMultiplexID; /*multiplex id of remote side*/
    HTI                     keepAliveTimer;    /*keepAlive timer for outgoing media*/
    RvBool                  keepAliveTimerSet; /*RV_TRUE if timer should be reset and set again*/
    RvInt8                  dynPayloadType; /* If other than 0, it indicates the dynamic payload
                                           type of an incoming channel */
    HCHAN                   hsChan; /* Channel handle in the CM for this channel */
    RvBool                  isOutgoing; /* RV_TRUE for outgoing channels */
    RvBool                  isIndependent; /* RV_TRUE for independent calls, not handled too much by this library */

    RvBool                  wasConnected; /* Was this channel connected? */

#ifdef USE_PLAYER
    PlayerChannelParams     ChPlayerInfo;
#endif
};



/* This struct holds the application's information about a TCP connection. */
typedef struct 
{
    RvInt32         id;     /* Id of this connection */
    RvBool          inConn; /* RV_TRUE for incoming TCP connections */
    int             color;  /* Color assigned for this connection host */
    RvBool          reported; /* RV_TRUE if an event was raised on this connection */
#ifdef USE_FWNAT
    HH323FWNATCONN  hFwNatConn;
#endif
} EpConnectionInfo;



/* Configuration that is specific to a call.
   Negative values for these parameters are ignored. */
typedef struct 
{
    RvInt32             manualRasCalls; /* RV_TRUE to force calls to work without a GK */
    RvInt32             multiplexing; /* RV_TRUE to support multiplexing */
    RvInt32             maintainConnection; /* RV_TRUE to support maintaining a multiplexed connection with no calls on it */
    RvInt32             canOverlapSend; /* RV_TRUE for OVSP support */
    RvInt32             setupSendingComplete; /* RV_TRUE to indicate dialing full aliases only */
    RvInt32             tunneling; /* RV_TRUE for tunneling support */
    RvInt32             parellel; /* RV_TRUE for parallel support */
    RvInt32             efc; /* RV_TRUE for parallel support */
    RvInt32             h245Stage; /* Stage of H.245 opening (of type cmH245Stage) */
    RvInt32             terminalType; /* Terminal type to use for the call */
    RvInt32             requestedBandwidth; /* Bandwidth to request for calls */
    RvInt32             useMultirate; /* RV_TRUE to use multi-rate */
    RvInt32             hopCount; /* Maximum hop count to set in messages */
} EpCallConfig;



#ifdef USE_SECURITY
typedef struct
{
    EpObj   *ep; /* Context to endpoint object */
    void    *rsa; /* RSA of open SSL object */
} SecurityCertificate;
#endif /* USE_SECURITY */



typedef struct
{
    RvBool              bIndependent; /* RV_TRUE if we want new calls to be configured as independent */
    RvBool              bAutoAnswer; /* RV_TRUE to automatically connect incoming calls */
    RvBool              bPreventRereg; /* RV_TRUE to prevent re-registration when we get unregistered */
    RvBool              bSupportFaststart; /* RV_TRUE for faststart support */
    RvBool              bAutomaticFaststart; /* RV_TRUE for automatic faststart support in the endpoint (no GUI interaction) */
    RvBool              bIgnoreMasterSlaveConflicts; /* RV_TRUE to ignore MS conflicts */
    RvBool              bAutoAcceptChannels; /* Automatically accept incoming channel requests? */
    RvBool              bAutoDropChannels; /* Automatically drop channels when requested? */
    RvBool              bAutoMimicChannels; /* Automatically mimic incoming channel requests? */
    RvBool              bSuppressReleaseComplete; /* RV_TRUE to disallow outgoing releaseComplete messages */
    RvBool              bSuppressEndSessionCommand; /* RV_TRUE to disallow outgoing endSessionCommand messages */
    RvBool              bSuppressQ931; /* RV_TRUE to disallow outgoing Q.931 messages */
    RvBool              bSuppressH245; /* RV_TRUE to disallow outgoing H.245 messages */
    RvBool              bSuppressRAS; /* RV_TRUE to disallow outgoing RAS messages */
    RvBool              bSuppressOutgoingMessages; /* RV_TRUE to suppress all outgoing messages */
    RvBool              bSuppressIncomingMessages; /* RV_TRUE to suppress all incoming messages */
    RvBool              bScanNodeIDs; /* RV_TRUE to run a scan on incoming node IDs */
    RvBool              bUseNatAddress; /* RV_TRUE if we want to pass through fw/nat */
    cmTransportAddress  natAddress; /* Nat address to set */
    RvChar              dialFromIp[64]; /* IP address to dial the call from */
    EpRtpAction         mediaAction; /* Media action to take */
    RvInt32             typeOfService; /* DiffServ TOS value to set */
    EpCallConfig        callConfig; /* Global configuration to use on new calls (mainly incoming) */
    RvPstFieldId        ignoreIncomingFid; /* Incoming message type to ignore */
    RvBool              bPopUp; /* RV_TRUE if we want pop-up windows */
    RvBool              bAdHocUse; /* RV_TRUE if we want to use an ad-hoc conference */
    RvChar              strAdHocCall[64]; /* String of the call to use with an ad-hoc conference */
    RvBool              bMultiplexUse; /* RV_TRUE if we want to use multiplexing */
    RvChar              strMultiplexCall[64]; /* String of the call whose multiplex connection we want to use */
    RvChar              strUserUser[64]; /* User-User information String */
    RvChar              strDisplay[64]; /* Display information String */
    RvBool              bUseNSD; /* RV_TRUE if we want to use Non Standard Data */
    RvChar              nsdString[64]; /* Contains the Non Standard Data string value */
#ifdef USE_SECURITY
    h235Mode_t          securityMode; /* Security mode used */
    RvChar              password[256]; /* Password to use for GK registrations */
    entityId_t          generalId; /* ID of the gatekeeper for authentication */
    entityId_t          senderId; /* ID of the sender for authentication */
    entityId_t          remoteId; /* ID of the sender for authentication */
    RvBool              bCheckIncoming; /* Are we checking authentication on incoming messages? */
    RvBool              bUserPrimes; /* Are we used pre-configured prime numbers? */
    RvUint8             *privateCrt; /* pointer to the private certificate used */
    RvUint32            privateCrtSz; /* size of the private certificate used */
#endif
#ifdef USE_H450
    ssAction            H450_responseType; /* What is the automatic response for incoming H.450 request? */
    RvInt32             H450_terminalType; /* Terminal type of outgoing calls initiated by SSE */
    RvBool              bH450_countPdu; /* Are we counting incoming H.450 PDUs? */
    RvBool              bH4504_disableWait; /* RV_TRUE if we want to disable the wait service */
    RvBool              bH4505_enablePark; /* Are we able to park a call? */
    RvBool              bH4505_alertingPrk; /* Deal with parking on alerting? */
    RvChar              *H4505_parkGroup; /* Group of endpoints used for parking */
    niNameChoice        H4508_niNameChoice; /* Type of name to use for calls */
    RvChar              H4508_name[64]; /* Name to use */
    RvInt32             H4509_foundService; /* Service to "automatically" choose on incoming completion messages */
    RvBool              bH4509_retainService; /* Are we retaining completion services? */
    RvBool              bH4509_retainConnection; /* Are we retaining connections? */
    RvBool              bH4509_isBusy; /* Are we considered busy? */
    RvBool              bH45011_isPossible; /* Is intrusion possible? */
    RvBool              bH45011_isIsolated; /* Is intrusion isolated? */
#endif
#ifdef USE_LDAP
    RvBool              bUseLdap; /* RV_TRUE if we to use the LDAP module */
#endif
#ifdef USE_FWNAT
    RvH323FwNatConfiguration fwNatConfig;
#endif
} EpConfig;


/* Endpoint instance object */
struct EpObj_tag
{
    EpCallbacks             cb; /* Callbacks implemented */

    EpConfig                cfg; /* Modifiable configuration parameters */

    HAPP                    hApp; /* Application instance of the stack */
    HPVT                    hVal; /* PVT used */

    HashHANDLE              calls; /* Calls table */
    HashHANDLE              channels; /* Channels table */

    RvInt32                 maxCalls; /* Maximum number of supported calls */

    RvBool                  bIsRegistered; /* Are we currently registered? */
    RvBool                  bIsManualRAS; /* Are we using manual RAS? */
    RvInt32                 portFrom; /* Used to change port range feature */
    RvInt32                 portTo; /* Used to change port range feature */
    RvPvtNodeId             partialUrqAliases; /* Used to set aliases in a partial URQ */

#ifdef USE_SECURITY
    H235HAPP                h235App; /* H.235 stack instance */
    RvChar                  key[20]; /* Key to use for GK registrations */
    RvUint8                 publicKey[256]; /* */
    RvSize_t                publicKeySize;
    SecurityCertificate     myRSA; /* RSA object from openSSL */
    SecurityCertificate     gkRSA; /* RSA certificate from openSSL of the GK */
    SecurityCertificate     tmpRSA; /* RSA certificate from temporary remote endpoints */
#endif
#ifdef USE_H450
    HSSEAPP                 hSSEApp; /* H.450 stack instance */
    HashHANDLE              completionServices; /* List of completion services used */
    RvPvtNodeId             parkedCallsNodeId; /* H.450.5 parked calls information */
    HSSECALL                pickupSseCall; /* a pointer to the picking-up call */
    RvInt32                 intrudedCallId; /* Id of the currently intruded call */
    RvPvtNodeId             cmnNodeId; /* H.450.12 CMN node id */
    RvInt32                 h450outCounter; /* Counter of initiated outgoing calls */
    HPST                    hH450PduPst; /* H.450 message syntax */
    HPST                    parkedCallsSyn;  /* H.450.5 parked calls information syntax */
#endif
    RtpHANDLE               rtp; /* RTP used */
#ifdef USE_LDAP
    // TODO: Ran, please add some comments on these parameters
    H323LDAP                hLdap;
    RvChar                  strDn[128];
    RvChar                  strSb[128];
    RvH323LdapH235Context   genContext;
    RvUint8                 lastCertificate[160];
    RvChar                  lastCertificateDn[128];
#endif

#ifdef USE_FWNAT
    HH323FWNATMGR           hFwNat;
#endif

    HSTIMER                 hTimers; /* Timers used internally */
    HTI                     dropAllTimer; /* Timer used when dropping all of the calls */
    cmReasonType            dropAllReason; /* Reason to use when dropping all calls */

    RvBool                  resetError; /* Do we need to reset the error string? */
    RvChar                  lastError[256]; /* Last error string we know of */
    RvBool                  bIgnoreRec; /* Do we need to ignore "Rec" events? */

    /* Resources */
    RvUint32                curAllocs;
    RvUint32                maxAllocs;
    RvUint32                curMemory;
    RvUint32                maxMemory;

    LockHANDLE              lock;
};



/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/







#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_DEFS_H_ */
