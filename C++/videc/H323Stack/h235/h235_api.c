#ifdef __cplusplus
extern "C" {
#endif
/********************************************************************************
*                                                                               *
* NOTICE:                                                                       *
* This document contains information that is confidential and proprietary to    *
* RADVISION LTD.. No part of this publication may be reproduced in any form     *
* whatsoever without written prior approval by RADVISION LTD..                  *
*                                                                               *
* RADVISION LTD. reserves the right to revise this publication and make changes *
* without obligation to notify any person of such revisions or changes.         *
* Copyright RADVISION 1998.                                                     *
********************************************************************************/


#include "h323Version.h"
#include "pvaltreeStackApi.h"
#include "q931asn1.h"
#include "h32xLog.h"
#include "h235.h"
#include <string.h>

/* Indentification string of this version.
   Unix systems like to search for such a string in the binary file itself */
const char rvh235_ident[] = "@(#)RADVISION H.235 Security "RV_H323_STACK_VERSION;



#define MAX_TOKNS_ADDED 8



/**************************************************************************
 * A few headers                                                          *
 **************************************************************************/

void h235APIEnter(IN  h235Element_T* h235App);
void h235APIExit(IN  h235Element_T* h235App);

int RVCALLCONV h235cmEvRASNewRawMessage(
    IN  HAPP        hApp,
    IN  HPROTCONN   protConn,
    IN  int         nodeId,
    IN  RvUint8*    messageBuf,
    IN  int         messageLength,
    IN  cmRASTransport *srcAddress,
    OUT int*        bytesDecoded,
    OUT void**      hMsgContext);

int RVCALLCONV h235cmEvRASSendRawMessage(
    IN  HAPP        hApp,
    IN  HPROTCONN   protConn,
    IN  HRAS        hsRas,
    IN  HAPPRAS     haRas,
    IN  int         messageRoot,
    IN  int         messageLength,
    OUT RvUint8*    messageBuf,
    OUT int*        bytesEncoded);

int RVCALLCONV  h235cmEvRASReleaseMessageContext(
    IN  void*       hMsgContext);

int RVCALLCONV h235cmEvCallNewRawMessage(
    IN  HAPP        hApp,
    IN  HPROTCONN   protConn,
    IN  int         nodeId,
    IN  RvUint8*    messageBuf,
    IN  int         messageLength,
    OUT int*        bytesDecoded,
    OUT void**      hMsgContext);

int RVCALLCONV h235cmEvCallRecvRawMessage(
    IN  HAPP        hApp,
    IN  HCALL       hsCall,
    IN  int         nodeId,
    IN  void*       hMsgContext);

int RVCALLCONV h235cmEvCallSendRawMessage(
    IN  HAPP        hApp,
    IN  HPROTCONN   protConn,
    IN  HCALL       hsCall,
    IN  int         nodeId,
    IN  int         messageLength,
    OUT RvUint8*    messageBuf,
    OUT int*        bytesEncoded);

int RVCALLCONV h235cmEvCallReleaseMessageContext(
    IN  void*       hMsgContext);

static void checkMessageSecurity(
    IN  h235Element_T      *h235App,
    IN  HPROTCONN           protConn,
    IN  int                 messageRoot,
    IN  RvUint8            *messageBuf,
    IN  int                 messageLength,
    IN  cmRASTransport     *srcAddress,
    OUT h235ContextInfo_t  *contextInfo);

static int h235PrintTree(IN h235Element_T* h235App,IN int tree,IN const char *str);

RvStatus h235GetFirstCryptoNodeFromMessage(
    IN  HPVT             hVal,
    IN  const int        nodeId,
    IN  const cmProtocol protocol,
    OUT int              *firstCryptoNode);

#define _GET_STATE(msgContext) ((h235State_t)((msgContext) & 0X0000000F))
#define _GET_SECUREMODE(msgContext) ((h235Mode_t)((msgContext>>4) & 0X000FFFFF))
#define _GET_REASON(msgContext) ((h235Reason_t)((msgContext>>24) & 0X000000FF))

/*lint -save -e773  Expression-like macro not parenthesized */
#define _SET_STATE(msgContext,state)   msgContext   = (H235HMSG)(((RvUint32)msgContext & 0XFFFFFFF0) | (int)state)
#define _SET_SECUREMODE(msgContext,mode) msgContext = (H235HMSG)(((RvUint32)msgContext & 0XFF00000F) | ((int)mode << 4))
#define _SET_REASON(msgContext,reason) msgContext   = (H235HMSG)(((RvUint32)msgContext & 0X00FFFFFF) | ((int)reason << 24))
/*lint -restore */

RvStatus diffieHellmanTest(H235HAPP h235hApp);




RVAPI
RvStatus RVCALLCONV h235Init(IN  HAPP hApp,OUT H235HAPP *h235HApp)
{
    RvLogMgr* logMgr;
    h235Element_T* h235App;

    /* todo: get log manager from hApp and not from global one */
    logMgr = H32xLogGet();

    if (RvMemoryAlloc(NULL, sizeof(h235Element_T), logMgr, (void**)&h235App) != RV_OK)
        return -1;
    memset(h235App, 0, sizeof(h235Element_T));
    *h235HApp=(H235HAPP)h235App;
    h235App->hApp = hApp;
    h235App->hVal = cmGetValTree(hApp);
    h235App->logMgr = logMgr;
    cmSetPartnerHandle(h235App->hApp,(HAPPPARTNER)h235App);
    h235App->secureMode = h235ModeNone;
    h235App->isCheckIncoming = RV_FALSE;
    h235App->isGK = RV_TRUE;
    h235SetTimeSync(*h235HApp,RV_TRUE,3900); /* default value of 1:05 hours to overcome
                                             daylight time differences between platforms */
    RvMutexConstruct(logMgr, &h235App->mutex);

    /********************************/
    /* log register                 */
    /********************************/
    RvLogSourceConstruct(logMgr, &h235App->h235Chan, "SEC", "h235 Security add-on");

    /********************************/
    /* set cm callbacks             */
    /********************************/
    cmiRASSetMessageEventHandler(hApp,
                                /*(cmiEvRASNewRawMessageT)*/h235cmEvRASNewRawMessage,
                                /*(cmiEvRASSendRawMessageT)*/h235cmEvRASSendRawMessage,
                                /*(cmiEvRASReleaseMessageContextT)*/h235cmEvRASReleaseMessageContext);
    cmSetMessageEventHandler(hApp,
                                /*(cmEvCallNewRawMessageT)*/h235cmEvCallNewRawMessage,
                                /*(cmEvCallRecvRawMessageT)*/h235cmEvCallRecvRawMessage,
                                /*(cmEvCallSendRawMessageT)*/h235cmEvCallSendRawMessage,
                                /*(cmEvCallReleaseMessageContextT)*/h235cmEvCallReleaseMessageContext);

    /*initializing object id*/
    initOIDs();

    /* creating a syntax tree root for crypto-token and clear-token */
    h235App->cryptoTokenSyn = pstConstruct(cmEmGetQ931Syntax(),(char *)"CryptoH323Token");
    h235App->clearTokenSyn = pstConstruct(cmEmGetQ931Syntax(),(char *)"ClearToken");

    /*primeTime = (RvInt32)rvh323BuffnumTest(hApp);
    {
        RvInt64 time, accTime=0;
        int i;

        for(i=0; i<40; i++)
        {
            time = RvTimestampGet(NULL);
            if(diffieHellmanTest(*h235HApp) != RV_OK)
                break;
            accTime += RvTimestampGet(NULL) - time;
        }
        time = accTime/80;
        time /= 1000000;
        dhTime = (RvInt32) time;
    }*/
    RvLogInfo(&h235App->h235Chan, (&h235App->h235Chan, "H.235 Initialized"));
    return RV_OK;
}


RVAPI
RvStatus RVCALLCONV h235End(IN   H235HAPP h235HApp)
{
    h235Element_T* h235App = (h235Element_T*) h235HApp;
    pstDestruct(h235App->cryptoTokenSyn);
    pstDestruct(h235App->clearTokenSyn);
    cmSetPartnerHandle(h235App->hApp,0);
    RvMutexDestruct(&h235App->mutex, h235App->logMgr);
    RvLogSourceDestruct(&h235App->h235Chan);
    RvMemoryFree(h235App, h235App->logMgr);
    return RV_OK;
}


RVAPI
HAPP RVCALLCONV h235GetHAPP(
    IN  H235HAPP h235HApp)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    if (app)
        return app->hApp;
    return NULL;
}


RVAPI
RvStatus RVCALLCONV h235SetSecureMode(
    IN  H235HAPP        h235HApp,
    IN  h235Mode_t    secureMode)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    app->secureMode = secureMode;
    if (!app->isGK)
    {
        app->gkEntry.secureMode = (h235Mode_t)(secureMode & RV_H235_ALL_SEC_MODES);
        if (app->gkEntry.secureMode & h235ModeProcedure4)
        {
            /* make sure we did not already generate a P4 shared secret */
            if (!app->gkEntry.bGenerateSecret ||
                !app->gkEntry.inDHinf.isValid || !app->gkEntry.outDHinf.isValid)
            {
                /* no valid shared secret yet, initialize the parameters */
                app->gkEntry.bGenerateSecret = RV_TRUE;
                app->gkEntry.inDHinf.isValid = RV_FALSE;
                app->gkEntry.outDHinf.isValid = RV_FALSE;
            }
            else
            {
                /* already done with procedure 4, change to procedure 1 */
                app->gkEntry.secureMode &= ~h235ModeProcedure4;
                app->gkEntry.secureMode |= h235ModeProcedure1;
                app->secureMode &= ~h235ModeProcedure4;
                app->secureMode |= h235ModeProcedure1;
            }
        }
        else
            app->gkEntry.bGenerateSecret = RV_FALSE;
    }
    return RV_OK;
}


RVAPI
h235Mode_t RVCALLCONV h235GetSecureMode(
    IN  H235HAPP        h235HApp)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    return app->secureMode;
}


RVAPI
RvStatus RVCALLCONV h235SetCheckIncomingMessages(
    IN  H235HAPP        h235HApp,
    IN  RvBool          isCheck)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    app->isCheckIncoming = isCheck;
    return RV_OK;
}


RVAPI
RvBool RVCALLCONV h235GetCheckIncomingMessages(
    IN  H235HAPP        h235HApp)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    return app->isCheckIncoming ;
}


RVAPI
RvStatus RVCALLCONV h235SetSenderId(
    IN  H235HAPP        h235HApp,
    IN  entityId_t      *senderId)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    memcpy(&(app->senderId),senderId,sizeof(entityId_t));
    return RV_OK;
}


RVAPI
RvStatus RVCALLCONV h235GetSenderId(
    IN  H235HAPP        h235HApp,
    OUT entityId_t      *senderId)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    memcpy(senderId,&(app->senderId),sizeof(entityId_t));
    return RV_OK;
}


RVAPI
RvStatus RVCALLCONV h235SetGatekeeperId(
    IN  H235HAPP        h235HApp,
    IN  entityId_t      *gatkeeperId,
    IN  H235HKEY        keyHandle)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    app->isGK = RV_FALSE;
    memcpy(&(app->gkEntry.id),gatkeeperId,sizeof(entityId_t));
    app->gkEntry.keyHandle = keyHandle;
    app->gkEntry.secureMode = (h235Mode_t)(app->secureMode & RV_H235_ALL_SEC_MODES);
    /* initialize P4-DH parameters */
    app->gkEntry.inDHinf.isValid = RV_FALSE;
    app->gkEntry.outDHinf.isValid = RV_FALSE;
    return RV_OK;
}


RVAPI
RvStatus RVCALLCONV h235GetGatekeeperId(
    IN  H235HAPP        h235HApp,
    OUT entityId_t      *gatkeeperId,
    OUT H235HKEY        *keyHandle)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    memcpy(gatkeeperId,&(app->gkEntry.id),sizeof(entityId_t));
    *keyHandle = app->gkEntry.keyHandle;
    return RV_OK;
}


RVAPI
RvStatus RVCALLCONV h235SetCertificateHandles(
    IN  H235HAPP    h235HApp,
    IN  H235HKEY    crtInHandle,
    IN  H235HKEY    crtOutHandle)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    app->gkEntry.crtInHandle = crtInHandle;
    app->gkEntry.crtOutHandle = crtOutHandle;
    return RV_OK;
}


RVAPI
RvStatus RVCALLCONV h235GetCertificateHandles(
    IN  H235HAPP    h235HApp,
    OUT H235HKEY*   crtInHandle,
    OUT H235HKEY*   crtOutHandle)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    RV_UNUSED_ARG(crtInHandle);
    *crtOutHandle = app->gkEntry.crtOutHandle;
    *crtOutHandle = app->gkEntry.crtOutHandle;
    return RV_OK;
}


RVAPI
RvStatus RVCALLCONV h235SetTimeSync(
    IN  H235HAPP        h235HApp,
    IN  RvBool          performTimeSync,
    IN  RvUint32        timeDiff)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    app->performTimeSync = performTimeSync;
    app->timeSyncDiff = timeDiff;
    return RV_OK;
}


RVAPI
RvStatus RVCALLCONV h235GetTimeSync(
    IN  H235HAPP        h235HApp,
    OUT RvBool         *performTimeSync,
    OUT RvUint32       *timeDiff)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    *performTimeSync = app->performTimeSync;
    *timeDiff = (RvUint32)app->timeSyncDiff;
    return RV_OK;
}


RVAPI
int RVCALLCONV h235GetSizeofEntityEntry(void)
{
    return sizeof(h235EntityEntry_t);
}


RVAPI
int RVCALLCONV h235GetSizeofCallInfo(void)
{
    return sizeof(h235CallInfo_t);
}

static RvSize_t DH_SIZE = (2*sizeof(h235DiffieHellmanInfo_t) + MAX_BUFFNUM_SIZE*sizeof(RvUint16));

RVAPI
int RVCALLCONV h235GetSizeofEntityEntryNoDH(void)
{
    return (sizeof(h235EntityEntry_t)-DH_SIZE);
}


RVAPI
int RVCALLCONV h235GetSizeofCallInfoNoDH(void)
{
    return (sizeof(h235CallInfo_t)-DH_SIZE);
}


RVAPI
RvStatus RVCALLCONV h235SetEventHandlers(
    IN  H235HAPP h235HApp,
    IN  h235EventHandlers_T     *eventHandlers)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    memcpy(&(app->eventHandlers),eventHandlers,sizeof(h235EventHandlers_T));
    return RV_OK;
}


RVAPI
RvStatus RVCALLCONV h235SetEncodingCallback(
    IN  H235HAPP        h235HApp,
    IN  h235EvEncode_t  callback,
    IN  const char      *algorithmOIDstr)
{
    h235Element_T* app = (h235Element_T*) h235HApp;
    char objectOID[128];
    RvSize_t objectOIDlength;
    objectOidType oid;
    RvStatus rv;

    if(!strcmp(algorithmOIDstr, RV_H235_GENERATE_PRIME_OID))
    {
        app->generatePrime = callback;
        return RV_OK;
    }

    objectOIDlength = (RvSize_t)utlEncodeOID(sizeof(objectOID), objectOID, algorithmOIDstr);
    rv = algorithmOidToOidType(objectOID, objectOIDlength, &oid);
    if (rv != RV_OK)
        return rv;

    switch (oid)
    {
    case(OID_U1):
    case(OID_U2):
        app->evSHA1_96 = callback; break;
    case(OID_V):
        app->evMD5_RSA_Sig = callback; break;
    case(OID_W):
        app->evSHA1_RSA_Sig = callback; break;
/*    case(OID_Y):
        app->evDES = callback; break;
    case(OID_Z):
        app->evTripleDES = callback; break;*/
    default:
        break;
    }
    return RV_OK;
}


/*******************************************/
/* Receiving RAS message                   */
/*******************************************/
int RVCALLCONV h235cmEvRASNewRawMessage(
    IN  HAPP            hApp,
    IN  HPROTCONN       protConn,
    IN  int             nodeId,
    IN  RvUint8         *messageBuf,
    IN  int             messageLength,
    IN  cmRASTransport  *srcAddress,
    OUT int             *bytesDecoded,
    OUT void            **hMsgContext)
{
    h235Element_T* h235App;
    int status;
    RvBool multicast = RV_TRUE;
    h235ContextInfo_t   contextInfo;
    H235HMSG msgContext = NULL;

    *hMsgContext = 0;

    contextInfo.state = h235StateUndefined;
    contextInfo.isMsgDeleted = RV_FALSE;
    contextInfo.reason = h235ReasonUndefined;
    contextInfo.secureMode = h235ModeNone;

    cmGetPartnerHandle(hApp, (HAPPPARTNER*) &h235App);
    if (h235App == NULL)
    {
        return cmEmDecode(cmGetValTree(hApp), nodeId, messageBuf, messageLength, bytesDecoded);
    }

    status = cmEmDecode(h235App->hVal, nodeId, messageBuf, messageLength, bytesDecoded);
    if (status < 0) {
        RvLogInfo(&h235App->h235Chan, (&h235App->h235Chan, "incoming message decode failed"));
        return status;
    }
    { /* todo: Ran, please fix this */
        char txt[128];
        RvSprintf(txt, "Received RAS message: %s", cmGetProtocolMessageName(hApp, nodeId));
        h235PrintTree(h235App, nodeId, txt);
    }

    /* set the GK assigned EP-ID even if we do not check incoming messages */
    if (!h235App->isGK)
    {
        RvPvtNodeId epid;

        __pvtGetNodeIdByFieldIds(epid, h235App->hVal, nodeId, {_q931(registrationConfirm) _q931(endpointIdentifier) LAST_TOKEN});
        if (RV_PVT_NODEID_IS_VALID(epid))
        {
            memset(&(h235App->senderId), 0, sizeof(h235App->senderId));
            h235App->senderId.length =
                pvtGetString(h235App->hVal, epid, sizeof(h235App->senderId.data), (char*)h235App->senderId.data);
        }
    }

    if (cmGetRASConnectionHandle(hApp) == protConn)
        multicast = RV_FALSE;
    if (multicast)
        return status;

    checkMessageSecurity(h235App, protConn, nodeId, messageBuf, messageLength, srcAddress, &contextInfo);
    if (contextInfo.isMsgDeleted == RV_TRUE)
        return RV_ERROR_UNKNOWN;

    _SET_STATE(msgContext,contextInfo.state);
    _SET_SECUREMODE(msgContext,contextInfo.secureMode);
    _SET_REASON(msgContext,contextInfo.reason);
    *hMsgContext = (void *)msgContext;
    return RV_OK;
}


/*******************************************/
/* sending RAS message                     */
/*******************************************/
int RVCALLCONV h235cmEvRASSendRawMessage(
    IN  HAPP        hApp,
    IN  HPROTCONN   protConn,
    IN  HRAS        hsRas,
    IN  HAPPRAS     haRas,
    IN  int         messageRoot,
    IN  int         messageLength,
    OUT RvUint8*    messageBuf,
    OUT int*        bytesEncoded)
{
    h235Element_T* h235App;
    entityId_t  entityId;
    RvStatus status;
    RvBool canSecure = RV_TRUE;
    RvPvtNodeId tokens[MAX_TOKNS_ADDED];
	h235Mode_t checkMode=h235ModeNone;//add

    RV_UNUSED_ARG(protConn);

    memset(tokens, RV_PVT_INVALID_NODEID, sizeof(tokens));
    cmGetPartnerHandle(hApp, (HAPPPARTNER*) &h235App);

#ifdef H235_DEBUG
    {
        char msgName[64];
        RvSprintf(msgName,"out:%s",cmGetProtocolMessageName(h235App->hApp,messageRoot));
    }
#endif

    if (h235App == NULL)
    {
        return cmEmEncode(cmGetValTree(hApp), messageRoot, messageBuf, messageLength, bytesEncoded);
    }

	//{{gaoshizhong 20140321 Add use to reg gk and use H235
	if ((h235App->secureMode & h235ModeDHgroup1536) != 0) checkMode = h235ModeDHgroup1536;
	else if ((h235App->secureMode & h235ModeDHgroup1024) != 0) checkMode = h235ModeDHgroup1024;
	else checkMode=h235ModeNone;

    //if (h235App->secureMode != h235ModeNone)
	//{
    if (checkMode != h235ModeDHgroup1536 && checkMode != h235ModeDHgroup1024 )
	{
	//}}gaoshizhong 20140321 Add use to reg gk and use H235

        h235EntityEntry_t* entityEntryPtr = NULL;
        RvPvtNodeId tmpNode;

        if (h235App->isGK)
        {
            /* get the ID of the EP we're sending this message to */
            LPHRAS  hsRasOut=0;
            cmRASGetHandle(hApp,haRas,hsRasOut);
			//{{gaoshizhong add if pointer
			if(h235App->eventHandlers.evGetIDByRAS)
            canSecure = h235App->eventHandlers.evGetIDByRAS(h235App->hApp,(H235HAPP)h235App,hsRas,haRas,
                            &entityId);
        }
        else
        {
            /* set the senderId as the EPID in RRQ (and cross fingers) */
            pvtGetChild(h235App->hVal, messageRoot, __q931(registrationRequest), &tmpNode);
            if ((RV_PVT_NODEID_IS_VALID(tmpNode)) && (h235App->senderId.length > 0))
            {
                pvtAdd(h235App->hVal, tmpNode, __q931(endpointIdentifier), h235App->senderId.length, (char*)h235App->senderId.data, NULL);
            }
        }
        if (canSecure)
        {
            if (getEntityEntry(h235App, &entityId, &entityEntryPtr, h235App->secureMode) != RV_OK)
                canSecure = RV_FALSE;
        }
        if ((h235App->isGK) && (canSecure))
        {
            /* update the senderId from the EPID in outgoing RCF */
            __pvtGetNodeIdByFieldIds(tmpNode, h235App->hVal, messageRoot, {_q931(registrationConfirm) _q931(endpointIdentifier) LAST_TOKEN});
            if (RV_PVT_NODEID_IS_VALID(tmpNode))
            {
                memset(&(entityEntryPtr->id), 0, sizeof(entityEntryPtr->id));
                entityEntryPtr->id.length =
                    pvtGetString(h235App->hVal, tmpNode, sizeof(entityEntryPtr->id.data), (char*)entityEntryPtr->id.data);
            }
        }
        if (canSecure)
            secureMessage(h235App, entityEntryPtr, NULL, messageRoot, tokens);
    }
    else canSecure = RV_FALSE;

    status = cmEmEncode(h235App->hVal, messageRoot, messageBuf, messageLength, bytesEncoded);
    if (status >= 0)
    {
//{{gaoshizhong 20140321 Add
       char txt[1024] ={0};//add 132 --> 1024
//}}gaoshizhong 20140321 Add
        RvSprintf(txt,"sending RAS message %s, bytesEncoded %d, messageLength size %d",
                cmGetProtocolMessageName(h235App->hApp,messageRoot),*bytesEncoded,messageLength);
        if (canSecure)
        {
            strcat(txt," (with security info)");
            h235PrintTree(h235App, messageRoot, txt);
        }
        else
        {
            if (h235App->secureMode != h235ModeNone)
                strcat(txt," (no security info, because target id is unknown)");
            RvLogInfo(&h235App->h235Chan, (&h235App->h235Chan, txt));
        }
    }

    {
        RvPvtNodeId parent, seq = RV_PVT_INVALID_NODEID;
        int i;
        /* delete the nodes we added */
        for (i=0; i<MAX_TOKNS_ADDED; i++)
        {
            if (RV_PVT_NODEID_IS_VALID(tokens[i]))
            {
                parent = pvtParent(h235App->hVal, pvtParent(h235App->hVal, pvtParent(h235App->hVal, tokens[i])));
                if (!RV_PVT_NODEID_IS_VALID(seq))
                {
                    seq = pvtParent(h235App->hVal, parent);
                }
                pvtDelete(h235App->hVal, parent);
            }
        }
        /* if no more children, delete */
        if (!RV_PVT_NODEID_IS_VALID(pvtChild(h235App->hVal, seq)))
            pvtDelete(h235App->hVal, seq);
    }

    return status;
}


/**********************************************/
/* release RAS message context                */
/* This function is for possible future use   */
/* If we ever return a pointer as hMsgContext */
/* (currently we return int), we would have to*/
/* release memory after message is done       */
/**********************************************/
int RVCALLCONV  h235cmEvRASReleaseMessageContext(
    IN  void*       hMsgContext)
{
    RV_UNUSED_ARG(hMsgContext);
    return 0;
}


/* Receiving Q931 message - this function now returns TRANSERR type */
int RVCALLCONV h235cmEvCallNewRawMessage(
    IN  HAPP        hApp,
    IN  HPROTCONN   protConn,
    IN  int         nodeId,
    IN  RvUint8*    messageBuf,
    IN  int         messageLength,
    OUT int*        bytesDecoded,
    OUT void**      hMsgContext)
{
    h235Element_T* h235App;
    int status;
    h235ContextInfo_t   contextInfo;
    H235HMSG msgContext = NULL;

    RV_UNUSED_ARG(protConn);
    *hMsgContext = 0;
    memset(&contextInfo,0,sizeof(h235ContextInfo_t));

    cmGetPartnerHandle(hApp, (HAPPPARTNER*) &h235App);

    status = cmEmDecode(cmGetValTree(hApp), nodeId, messageBuf, messageLength, bytesDecoded);
    if ((status < 0) || (h235App == NULL))
        return status;

    {
        char txt[128];
        RvSprintf(txt,"Received q931 message: %s",cmGetProtocolMessageName(hApp,nodeId));
        h235PrintTree(h235App,nodeId,txt);
    }

    checkMessageSecurity(h235App, protConn, nodeId, messageBuf, messageLength, NULL, &contextInfo);
    if (contextInfo.isMsgDeleted == RV_TRUE)
    {
        if (contextInfo.state == h235StateSecurityIgnore)
            return cmTransIgnoreMessage;
        return cmTransErr;
    }

    _SET_STATE(msgContext,contextInfo.state);
    _SET_SECUREMODE(msgContext,contextInfo.secureMode);
    _SET_REASON(msgContext,contextInfo.reason);
    *hMsgContext = (void *)msgContext;

    return cmTransOK;
}


/* Receiving Q931 message (after call association) */
int RVCALLCONV h235cmEvCallRecvRawMessage(
    IN  HAPP        hApp,
    IN  HCALL       hsCall,
    IN  int         nodeId,
    IN  void*       hMsgContext)
{
    h235Element_T* h235App;
    h235CallInfo_t* callInfoPtr = NULL;
    HAPPCALL haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall);
    h235Mode_t secMode, msgMode = _GET_SECUREMODE((RvUint32)hMsgContext);
    RvPvtNodeId dhNode512=-1, dhNode1024=-1, dhNode1536=-1, cryptoTokenNode, cryptoNode, clearTokenNode = RV_PVT_INVALID_NODEID;
    h235Mode_t tokenType;
    RvStatus status;
    RvBool dontHandleDH;
    RvPstFieldId fieldId = -1;
    HPVT hVal;

    cmGetPartnerHandle(hApp, (HAPPPARTNER*) &h235App);
    if (h235App == NULL)
        return RV_OK;

    hVal = h235App->hVal;

    __pvtGetNodeIdByFieldIds(clearTokenNode, hVal, nodeId,
         {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
          _q931(h323_uu_pdu) _q931(h323_message_body) _anyField _q931(tokens)
          _nul(1) LAST_TOKEN});

    if (h235App->isCheckIncoming && (msgMode == h235ModeNone))
    {
        /* we already checked this message, and didn't find anything. there may be DH tokens 
           which were passed as clear-tokens, we should check that out */
        if (!RV_PVT_NODEID_IS_VALID(clearTokenNode))
        {
            /* no clear tokens, and we already check the crypto-tokens and found nothing */
            return RV_OK;
        }
    }

    /* go over all the nodes and locate the relevant ones */
    status = h235GetFirstCryptoNodeFromMessage(hVal, nodeId, cmProtocolQ931, &cryptoTokenNode);
    if (status != RV_OK)
    {
        if (!RV_PVT_NODEID_IS_VALID(clearTokenNode))
            /* no security at all */
            return RV_OK;
    }
    else
    {
        /* real security - ignore clear-tokens */
        clearTokenNode = RV_PVT_INVALID_NODEID;
    }

    /* get the call pointer */
    if (h235App->eventHandlers.evGetH235Call != NULL)
    {
        callInfoPtr = (h235CallInfo_t*) h235App->eventHandlers.evGetH235Call(hsCall, haCall);
        if (callInfoPtr != NULL)
            callInfoPtr->hsCall = hsCall;
    }

    dontHandleDH = (h235App->isGK || (callInfoPtr == NULL) || !callInfoPtr->bSecureMedia);

    /* identify nodes */
    while (cryptoTokenNode >= 0)
    {
        tokenType = identifyNode(hVal, cryptoTokenNode, &cryptoNode);
        if ((dontHandleDH) && (h235App->eventHandlers.evReceivedToken != NULL))
        {
            switch (tokenType)
            {
            case h235ModeProcedure3:
                if (h235App->isGK)
                {
                    RvPvtNodeId generalIdNode;
                    entityId_t generalID;

                    /* get generalID and compare with my id */
                    __pvtGetNodeIdByFieldIds(generalIdNode, hVal, cryptoNode,
                        {_q931(token) _q931(toBeSigned) _q931(generalID) LAST_TOKEN});

                    if(!RV_PVT_NODEID_IS_VALID(generalIdNode))
                        break;

                    memset(&generalID, 0, sizeof(entityId_t));

                    /* get the general ID */
                    generalID.length = pvtGetString(hVal, generalIdNode,
                        sizeof(generalID.data), (char*)generalID.data);

                    /* check if general ID == out ID */
                    if ((generalID.length != h235App->senderId.length) ||
                        memcmp(generalID.data, h235App->senderId.data, (RvSize_t)generalID.length))
                    {
                        /* end to end token, and we are not the destination. raise to application */
                        h235App->eventHandlers.evReceivedToken(h235App->hApp, (H235HAPP)h235App,
                            hsCall, (H235HCALL)callInfoPtr, NULL, (int)tokenType, cryptoTokenNode);
                    }
                }
                break;
            case h235ModeDHgroup512:
                if (h235App->eventHandlers.evReceivedToken(h235App->hApp, (H235HAPP)h235App,
                    hsCall, (H235HCALL)callInfoPtr, NULL, (int)tokenType, cryptoTokenNode))
                {
                    dhNode512 = cryptoTokenNode;
                    msgMode |= tokenType;
                }
                break;
            case h235ModeDHgroup1024:
                if (h235App->eventHandlers.evReceivedToken(h235App->hApp, (H235HAPP)h235App,
                    hsCall, (H235HCALL)callInfoPtr, NULL, (int)tokenType, cryptoTokenNode))
                {
                    dhNode1024 = cryptoTokenNode;
                    msgMode |= tokenType;
                }
                break;
            case h235ModeDHgroup1536:
                if (h235App->eventHandlers.evReceivedToken(h235App->hApp, (H235HAPP)h235App,
                    hsCall, (H235HCALL)callInfoPtr, NULL, (int)tokenType, cryptoTokenNode))
                {
                    dhNode1536 = cryptoTokenNode;
                    msgMode |= tokenType;
                }
                break;
            default:
                break;
            }
        }
        else
        {
            if (tokenType == h235ModeDHgroup512)
                dhNode512 = cryptoTokenNode;
            if (tokenType == h235ModeDHgroup1024)
                dhNode1024 = cryptoTokenNode;
            if (tokenType == h235ModeDHgroup1536)
                dhNode1536 = cryptoTokenNode;
            msgMode |= tokenType;
        }
        cryptoTokenNode = pvtBrother(h235App->hVal, cryptoTokenNode);
    }
    if (!dontHandleDH)
    {
        while (clearTokenNode >= 0)
        {
            tokenType = identifyNode(hVal, clearTokenNode, &cryptoNode);
            if (tokenType == (h235ModeDHgroup512|h235ModeDhInClear))
                dhNode512 = clearTokenNode;
            if (tokenType == (h235ModeDHgroup1024|h235ModeDhInClear))
                dhNode1024 = clearTokenNode;
            if (tokenType == (h235ModeDHgroup1536|h235ModeDhInClear))
                dhNode1536 = clearTokenNode;
            msgMode |= tokenType;

            clearTokenNode = pvtBrother(h235App->hVal, clearTokenNode);
        }
    }

    if ((msgMode == h235ModeNone) || (callInfoPtr == NULL) || (!callInfoPtr->bSecureMedia))
    {
        /* no recognized security, or supported at application level */
        return RV_OK;
    }

    if (callInfoPtr)
        secMode = (h235Mode_t)(callInfoPtr->secureMode & msgMode & RV_H235_ALL_DH_MODES);
    else
        secMode = (h235Mode_t)(h235App->secureMode & msgMode & RV_H235_ALL_DH_MODES);

    if (secMode == h235ModeNone)
        /* we do not support DH types offered, so no need to continue */
        return RV_OK;

    secMode |= (msgMode & h235ModeDhInClear);
    pvtGet(hVal, pvtChild(hVal, pvtGetChild(hVal, nodeId, __q931(message), NULL)), &fieldId, NULL, NULL, NULL);

    /* check if this is the right kind of message */
    if (fieldId == __q931(setup))
    {
        int needToChoose = 0;

        /* see if we support more than one token type */
        if ((secMode & h235ModeDHgroup512) != h235ModeNone) needToChoose++;
        if ((secMode & h235ModeDHgroup1024) != h235ModeNone) needToChoose++;
        if ((secMode & h235ModeDHgroup1536) != h235ModeNone) needToChoose++;

        if (needToChoose > 1)
        {
            /* we must choose one. */
            h235App->eventHandlers.evChooseCallSecureMode(hsCall, haCall, (H235HCALL)callInfoPtr, &secMode);
        }

       /* we got a setup, get the Diffie-Hellman parameters and update our DH-mode to the one offered to us */
        if (secMode & h235ModeDHgroup1536)
        {
            if (secMode & h235ModeDhInClear)
            {
                status = getDHTokenNode(h235App->hVal, RV_PVT_INVALID_NODEID, dhNode1536, &callInfoPtr->inDHinf);
            }
            else
            {
                status = getDHTokenNode(h235App->hVal, dhNode1536, RV_PVT_INVALID_NODEID, &callInfoPtr->inDHinf);
            }
            if (status == RV_OK)
            {
                callInfoPtr->inDHinf.DHMode = h235ModeDiffieHellman1536;
                if (callInfoPtr != NULL)
                    callInfoPtr->secureMode = (h235Mode_t)(callInfoPtr->secureMode & (RV_H235_ALL_SEC_MODES | h235ModeDHgroup1536 | h235ModeDhInClear));
            }
        }
        else if (secMode & h235ModeDHgroup1024)
        {
            if (secMode & h235ModeDhInClear)
            {
                status = getDHTokenNode(h235App->hVal, RV_PVT_INVALID_NODEID, dhNode1024, &callInfoPtr->inDHinf);
            }
            else
            {
                status = getDHTokenNode(h235App->hVal, dhNode1024, RV_PVT_INVALID_NODEID, &callInfoPtr->inDHinf);
            }
            if (status == RV_OK)
            {
                callInfoPtr->inDHinf.DHMode = h235ModeDiffieHellman1024;
                if (callInfoPtr != NULL)
                    callInfoPtr->secureMode = (h235Mode_t)(callInfoPtr->secureMode & (RV_H235_ALL_SEC_MODES | h235ModeDHgroup1024));
            }
        }
        else if (secMode & h235ModeDHgroup512)
        {
            if (msgMode & h235ModeDhInClear)
            {
                status = getDHTokenNode(h235App->hVal, RV_PVT_INVALID_NODEID, dhNode512, &callInfoPtr->inDHinf);
            }
            else
            {
                status = getDHTokenNode(h235App->hVal, dhNode512, RV_PVT_INVALID_NODEID, &callInfoPtr->inDHinf);
            }
            if (status == RV_OK)
            {
                callInfoPtr->inDHinf.DHMode = h235ModeDiffieHellman512;

                if (memcmp(callInfoPtr->inDHinf.modSize, callInfoPtr->outDHinf.modSize, 256) != 0)
                    callInfoPtr->outDHinf.isValid = RV_FALSE;

                if (callInfoPtr != NULL)
                    callInfoPtr->secureMode = (h235Mode_t)(callInfoPtr->secureMode & (RV_H235_ALL_SEC_MODES | h235ModeDHgroup512));
            }
        }
        callInfoPtr->secureMode |= (secMode & h235ModeDhInClear);
    }
    else if ((fieldId == __q931(callProceeding)) ||
             (fieldId == __q931(alerting)) ||
             (fieldId == __q931(connect)))
    {
        h235DHMode_t replyDhMode = h235ModeDiffieHellmanNone;
        h235DHMode_t dhMode = RV_H235_MODE_TO_DH_MODE(secMode);
        /* we got a connect, get the Diffie-Hellman reply */
        if(h235GetIncomingDiffieHellmanHalfKey((H235HAPP)h235App, nodeId, (H235HCALL)callInfoPtr, &replyDhMode) >= 0)
        {
            /* make sure we got the same type we offered */
            if (replyDhMode & dhMode)
            {
                if (callInfoPtr != NULL)
                    callInfoPtr->secureMode = (h235Mode_t)(callInfoPtr->secureMode & (RV_H235_ALL_SEC_MODES | RV_H235_DH_MODE_TO_MODE(replyDhMode)));
            }
        }
    }
    /* there is no reason to return error from this function, even if Diffie-Hellman failed */
    return RV_OK;
}


/* sending Q931 message */
int RVCALLCONV h235cmEvCallSendRawMessage(
    IN  HAPP        hApp,
    IN  HPROTCONN   protConn,
    IN  HCALL       hsCall,
    IN  int         messageRoot,
    IN  int         messageLength,
    OUT RvUint8*    messageBuf,
    OUT int*        bytesEncoded)
{
    h235Element_T* h235App;
    entityId_t  entityId;
    h235CallInfo_t* callInfoPtr = NULL;
    RvStatus status;
    RvBool canSecure = RV_TRUE;
    h235Mode_t secureMode;
    HAPPCALL haCall;
    RvPvtNodeId isEmptyFacility = RV_PVT_INVALID_NODEID;
    RvPvtNodeId tokens[MAX_TOKNS_ADDED];
    RvPstFieldId fieldId = -1;
    HPVT hVal;

    RV_UNUSED_ARG(protConn);

    memset(tokens, RV_PVT_INVALID_NODEID, sizeof(tokens));
    cmGetPartnerHandle(hApp, (HAPPPARTNER*) &h235App);
    if (h235App == NULL)
    {
        return cmEmEncode(cmGetValTree(hApp), messageRoot, messageBuf, messageLength, bytesEncoded);
    }
    haCall = emaGetApplicationHandle((EMAElement)hsCall);

    if (hsCall != NULL)
    {
        if (h235App->eventHandlers.evGetH235Call != NULL)
        {
            callInfoPtr = (h235CallInfo_t*) h235App->eventHandlers.evGetH235Call(hsCall, haCall);
            if (callInfoPtr != NULL)
                callInfoPtr->hsCall = hsCall;
        }

        if (callInfoPtr)
            secureMode = callInfoPtr->secureMode;
        else
            secureMode = h235App->secureMode;
    }
    else
        secureMode = h235ModeNone;

    hVal = h235App->hVal;
    isEmptyFacility = pvtGet(hVal, pvtChild(hVal, pvtGetChild(hVal, messageRoot, __q931(message), NULL)), &fieldId, NULL, NULL, NULL);

    /* don't try to add security information to empty facility messages */
    if (fieldId == __q931(facility))
    {
        __pvtGetNodeIdByFieldIds(isEmptyFacility, h235App->hVal, isEmptyFacility,
            { _q931(userUser) _q931(h323_UserInformation)  _q931(h323_uu_pdu) _q931(h323_message_body) _q931(empty) LAST_TOKEN });
        if (RV_PVT_NODEID_IS_VALID(isEmptyFacility))
        {
            secureMode = h235ModeNone;
        }
    }
    else
    {
        isEmptyFacility = RV_PVT_INVALID_NODEID;
    }

    if ((secureMode != h235ModeNone) ||
        ((callInfoPtr != NULL) && RV_PVT_NODEID_IS_VALID(callInfoPtr->sendCryptoToken[0])))
    {
        h235EntityEntry_t* entityEntryPtr = NULL;

        /* check if this is the right kind of message */
        if ( (callInfoPtr != NULL) && (callInfoPtr->bSecureMedia) &&
             ( fieldId == __q931(setup) ||
               (callInfoPtr->inDHinf.isValid && (fieldId == __q931(callProceeding) ||
                                                 fieldId == __q931(alerting) ||
                                                 fieldId == __q931(connect)) ) ) )
        {
            h235DHMode_t dhMode;
            h235DiffieHellmanInfo_t* dhInf;
            RvBool bOutValid = RV_FALSE;

            /* if we already received DH parameters, use that protocol */
            if (callInfoPtr->inDHinf.isValid)
                dhMode = callInfoPtr->inDHinf.DHMode;
            else
            {
                int needToChoose = 0;
                h235Mode_t secMode = secureMode;

                /* see if we support more than one token type */
                if ((secMode & h235ModeDHgroup512) != h235ModeNone) needToChoose++;
                if ((secMode & h235ModeDHgroup1024) != h235ModeNone) needToChoose++;
                if ((secMode & h235ModeDHgroup1536) != h235ModeNone) needToChoose++;

                if (needToChoose > 1)
                {
                    /* we must choose one. */
                    h235App->eventHandlers.evChooseCallSecureMode(hsCall, haCall, (H235HCALL)callInfoPtr, &secMode);
                }
                dhMode = RV_H235_MODE_TO_DH_MODE((secMode&(~h235ModeDhInClear)));
            }

            /* if we already generated DH parameters, use them */
            dhInf = &callInfoPtr->outDHinf;
            if(dhInf->isValid && (dhInf->DHMode == dhMode))
            {
                bOutValid = RV_TRUE;
            }

            if ((dhMode & (h235ModeDiffieHellman512|h235ModeDiffieHellman1024|h235ModeDiffieHellman1536)) &&
                (bOutValid || (h235GenerateDiffieHellman((H235HAPP)h235App, dhMode, (H235HCALL)callInfoPtr) == RV_OK)))
            {
                if ((callInfoPtr->secureMode & h235ModeDhInClear) == h235ModeNone)
                {
                    RvPvtNodeId cryptoToken = pvtAddRoot(h235App->hVal, h235App->cryptoTokenSyn, 0, NULL);
                    setDHP1TokenNode(h235App->hVal, dhMode, cryptoToken, dhInf);
                    h235CallSendCryptoTokenOnCall((H235HAPP)h235App, (H235HCALL)callInfoPtr, cryptoToken);
                }
                else
                {
                    if (!RV_PVT_NODEID_IS_VALID(callInfoPtr->sendClearToken))
                    {
                        callInfoPtr->sendClearToken = pvtAddRoot(h235App->hVal, h235App->clearTokenSyn, 0, NULL);
                    }
                    setDHClTokenNode(h235App->hVal, dhMode, callInfoPtr->sendClearToken, dhInf);
                }
            }
        }

        if (h235App->isGK)
        {
            canSecure = h235App->eventHandlers.evGetIDByCall(h235App->hApp,(H235HAPP)h235App,hsCall,haCall,
                            &entityId);
        }
        if (canSecure)
        {
            if (getEntityEntry(h235App, &entityId, &entityEntryPtr, h235App->secureMode) != RV_OK)
                canSecure = RV_FALSE;
        }
        if (canSecure)
            secureMessage(h235App, entityEntryPtr, callInfoPtr, messageRoot, tokens);
    }
    else canSecure = RV_FALSE;

    status = cmEmEncode(h235App->hVal, messageRoot, messageBuf, messageLength, bytesEncoded);
    if (status >= 0)
    {
        char txt[256];
        RvSprintf(txt,"sending Q931 message %s, bytesEncoded %d, messageLength size %d",
                cmGetProtocolMessageName(h235App->hApp, messageRoot), *bytesEncoded, messageLength);
        if (canSecure)
        {
            strcat(txt," (with security info)");
            h235PrintTree(h235App,messageRoot,txt);
        }
        else
        {
            if (secureMode != h235ModeNone)
                strcat(txt," (no security info, because target id is unknown)");
            if (RV_PVT_NODEID_IS_VALID(isEmptyFacility))
                strcat(txt," (no security info can be added to empty facility)");
            RvLogInfo(&h235App->h235Chan, (&h235App->h235Chan, txt));
        }
    }

    {
        RvPvtNodeId parent, seq = RV_PVT_INVALID_NODEID;
        int i;
        /* delete the nodes we added */
        for (i=0; i<MAX_TOKNS_ADDED; i++)
        {
            if (RV_PVT_NODEID_IS_VALID(tokens[i]))
            {
                parent = pvtParent(h235App->hVal, pvtParent(h235App->hVal, pvtParent(h235App->hVal, tokens[i])));
                if (!RV_PVT_NODEID_IS_VALID(seq))
                {
                    seq = pvtParent(h235App->hVal, parent);
                }
                pvtDelete(h235App->hVal, parent);
            }
        }
        /* if no more children, delete */
        if (!RV_PVT_NODEID_IS_VALID(pvtChild(h235App->hVal, seq)))
            pvtDelete(h235App->hVal, seq);
    }

    return status;
}


/**********************************************/
/* release Q931 message context               */
/* This function is for possible future use   */
/* If we ever return a pointer as hMsgContext */
/* (currently we return int), we would have to*/
/* release memory after message is done       */
/**********************************************/
int RVCALLCONV h235cmEvCallReleaseMessageContext(
    IN  void*       hMsgContext)
{
    RV_UNUSED_ARG(hMsgContext);
    return 0;
}


RvStatus h235GetFirstCryptoNodeFromMessage(
    IN  HPVT             hVal,
    IN  const int        nodeId,
    IN  const cmProtocol protocol,
    OUT int              *firstCryptoNode)
{
    switch (protocol)
    {
        case cmProtocolRAS:
            __pvtGetNodeIdByFieldIds(*firstCryptoNode, hVal, nodeId,
                    {_anyField _q931(cryptoTokens) _nul(1) LAST_TOKEN});

            break;
        case cmProtocolQ931:
            __pvtGetNodeIdByFieldIds(*firstCryptoNode, hVal, nodeId,
                 {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
                 _q931(h323_uu_pdu) _q931(h323_message_body) _anyField _q931(cryptoTokens)
                 _nul(1) LAST_TOKEN});
            break;
        default:
                return RV_ERROR_UNKNOWN;
    }
    if(*firstCryptoNode < 0 )
    {
        return RV_ERROR_UNKNOWN;
    }
    return RV_OK;
}


RvStatus getEntityEntry(
    IN  h235Element_T*      h235App,
    IN  entityId_t*         entityId,
    IN  h235EntityEntry_t** entityEntryPtr,
    IN  h235Mode_t          secureMode)
{
    RvBool isNew = RV_TRUE;

    /* If this is endpoint, get the saved GK details */
    if (!h235App->isGK)
    {
        *entityEntryPtr = &(h235App->gkEntry);
        return RV_OK;
    }

    /* Get the entry structure that is saved in the application */
    h235APIEnter(h235App);
	//{{gaoshizhong add if pointer
	if(h235App->eventHandlers.evGetEntryByEndpointId)
    h235App->eventHandlers.evGetEntryByEndpointId(h235App->hApp,(H235HAPP)h235App,entityId,&isNew,(H235HENTITY*)entityEntryPtr);
    h235APIExit(h235App);
    if (*entityEntryPtr == NULL)
    {
        return RV_ERROR_UNKNOWN;
    }

    if (isNew)
    {
        int needToChoose = 0;

        /* new entity entry */
        secureMode &= h235App->secureMode;
        memcpy (&((*entityEntryPtr)->id), entityId, sizeof(entityId_t));
        (*entityEntryPtr)->secureMode = secureMode;
        (*entityEntryPtr)->lastRandom = -1;
        (*entityEntryPtr)->lastTime = 0;
        (*entityEntryPtr)->bGenerateSecret = RV_FALSE;
        (*entityEntryPtr)->keyHandle = NULL;
        (*entityEntryPtr)->crtInHandle = NULL;
        (*entityEntryPtr)->crtOutHandle = NULL;

        /* see if we support more than one token type */
        if ((secureMode & h235ModeProcedure1) != h235ModeNone) needToChoose++;
        if ((secureMode & h235ModeProcedure1a) != h235ModeNone) needToChoose++;
        if ((secureMode & (h235ModeProcedure2|h235ModeProcedure3)) != h235ModeNone) needToChoose++;
        if ((secureMode & h235ModeProcedure4) != h235ModeNone) needToChoose++;

        if (needToChoose > 1)
        {
			//{{gaoshizhong add if pointer
			if( h235App->eventHandlers.evChooseEndpointSecureMode)
            h235App->eventHandlers.evChooseEndpointSecureMode(h235App->hApp,(H235HAPP)h235App,entityId,
                &((*entityEntryPtr)->secureMode));
        }

        if ((*entityEntryPtr)->secureMode & (h235ModeProcedure1 | h235ModeProcedure1a | h235ModeProcedure4))
        {
            H235HKEY keyHandle;
			//{{gaoshizhong add if pointer
            if (h235App->eventHandlers.evGetKeyHandleById && h235App->eventHandlers.evGetKeyHandleById(h235App->hApp, (H235HAPP)h235App, entityId,
                &keyHandle))
            {
                (*entityEntryPtr)->keyHandle = keyHandle;
            }
            else
            {
                /* didn't get a key handle - remove support for these */
                (*entityEntryPtr)->secureMode &= (~h235ModeProcedure1 & ~h235ModeProcedure1a & ~h235ModeProcedure4);
            }
        }
        if ((*entityEntryPtr)->secureMode & (h235ModeProcedure2 | h235ModeProcedure3 | h235ModeProcedure4))
        {
            H235HKEY crtInHandle, crtOutHandle;
            if (h235App->eventHandlers.evGetCrtHandleById(h235App->hApp, (H235HAPP)h235App, entityId,
                &crtInHandle, &crtOutHandle))
            {
                (*entityEntryPtr)->crtInHandle = crtInHandle;
                (*entityEntryPtr)->crtOutHandle = crtOutHandle;
            }
            else
            {
                /* didn't get a key handle - remove support for these */
                (*entityEntryPtr)->secureMode &= (~h235ModeProcedure2 & ~h235ModeProcedure3 & ~h235ModeProcedure4);
            }
        }
        if ((*entityEntryPtr)->secureMode & h235ModeProcedure4)
        {
            memset(&(*entityEntryPtr)->inDHinf, 0, sizeof((*entityEntryPtr)->inDHinf));
            memset(&(*entityEntryPtr)->outDHinf, 0, sizeof((*entityEntryPtr)->outDHinf));
            memset((*entityEntryPtr)->myRandomNumber, 0, sizeof((*entityEntryPtr)->myRandomNumber));
            (*entityEntryPtr)->bGenerateSecret = RV_TRUE;
        }
    }
    return RV_OK;
}


static void h235SetContextInfo(
    OUT h235ContextInfo_t   *contextInfo,
    IN  h235State_t state,
    IN  RvBool      isMsgDeleted,
    IN  h235Reason_t reason)
{
    contextInfo->state = state;
    contextInfo->isMsgDeleted = isMsgDeleted;
    contextInfo->reason = reason;
}

h235AppFailReply_t failToApp(
    IN  h235Element_T* h235App,
    IN  int         messageRoot,
    IN  entityId_t  *senderId,
    IN  h235Reason_t reason,
    OUT h235ContextInfo_t   *contextInfo)
{
    h235AppFailReply_t appReply;

    H235HMSG H235HMsg = NULL;
    _SET_STATE(H235HMsg,contextInfo->state);
    _SET_SECUREMODE(H235HMsg,contextInfo->secureMode);
    _SET_REASON(H235HMsg,contextInfo->reason);

    if (h235App->eventHandlers.evFailure == NULL)
    {
        appReply = h235FailReplyContinue;

    }
    else appReply =  h235App->eventHandlers.evFailure(
                                h235App->hApp,(H235HAPP)h235App,H235HMsg,
                                messageRoot,senderId,reason);

    /****************************************/
    /* log                                  */
    /****************************************/
    {
        const RvChar *replyText;
        switch (appReply)
        {
            case h235FailReplyContinue:     replyText = "continue"; break;
            case h235FailReplyDeleteMessage:replyText = "delete incoming message"; break;
            case h235FailReplyFreezeMessage:replyText = "freeze incoming message"; break;
            case h235FailReplyForceOk:      replyText = "ignore security failure"; break;
            default:            replyText = "bug"; /* this option is impossible */
        }
        RvLogInfo(&h235App->h235Chan,
            (&h235App->h235Chan, "%s failed security check,reason: %s",
                cmGetProtocolMessageName(h235App->hApp, messageRoot), h235ReasonName(reason)));
        if (h235App->eventHandlers.evFailure != NULL)
        {
            RvLogInfo(&h235App->h235Chan, (&h235App->h235Chan, "application reply: %s", replyText));
        }
    }


    if (appReply == h235FailReplyForceOk)
    {
        h235SetContextInfo(contextInfo,h235StateSecurityOK,RV_FALSE,h235ReasonUndefined);
    }
    else if ((appReply == h235FailReplyDeleteMessage) || (appReply == h235FailReplyFreezeMessage))
    {
        h235SetContextInfo(contextInfo,h235StateSecurityFailed,RV_TRUE,reason);
    }
    else /* h235FailReplyContinue */
    {
        h235SetContextInfo(contextInfo,h235StateSecurityFailed,RV_FALSE,reason);
    }
    return appReply;
}


h235AppSuccessReply_t successToApp(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  entityId_t          *senderId,
    IN  h235Reason_t        reason,
    OUT h235ContextInfo_t   *contextInfo)
{
    h235AppSuccessReply_t appReply;

    H235HMSG H235HMsg = NULL;
    _SET_STATE(H235HMsg,contextInfo->state);
    _SET_SECUREMODE(H235HMsg,contextInfo->secureMode);
    _SET_REASON(H235HMsg,contextInfo->reason);

    if (h235App->eventHandlers.evSuccess == NULL)
    {
        appReply = h235SuccessReplyContinue;
    }
    else appReply =  h235App->eventHandlers.evSuccess(
                                h235App->hApp,(H235HAPP)h235App,H235HMsg,
                                messageRoot,senderId);

    /****************************************/
    /* log                                  */
    /****************************************/
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    {
        const RvChar *replyText;
        switch (appReply)
        {
            case h235SuccessReplyContinue:      replyText = "continue"; break;
            case h235SuccessReplyDeleteMessage: replyText = "delete incoming message"; break;
            case h235SuccessReplyForceFail:     replyText = "ignore security success"; break;
            default:                            replyText = "bug"; /* this option is impossible */
        }
        RvLogInfo(&h235App->h235Chan,
            (&h235App->h235Chan, "%s passed security check", cmGetProtocolMessageName(h235App->hApp, messageRoot)));
        if (h235App->eventHandlers.evSuccess)
        {
            RvLogInfo(&h235App->h235Chan, (&h235App->h235Chan, "application reply: %s", replyText));
        }
    }
#endif /* (RV_LOGMASK & RV_LOGLEVEL_INFO) */

    if (appReply == h235SuccessReplyForceFail)
    {
        h235SetContextInfo(contextInfo,h235StateSecurityFailed,RV_FALSE,h235ReasonAppForceFail);
    }
    else if (appReply == h235SuccessReplyDeleteMessage)
    {
        h235SetContextInfo(contextInfo,h235StateSecurityFailed,RV_TRUE,reason);
    }
    else /* h235SuccessReplyContinue */
    {
        h235SetContextInfo(contextInfo,h235StateSecurityOK,RV_FALSE,reason);
    }
    return appReply;
}


RvStatus freezeRasMessage(
    IN  h235Element_T*      h235App,
    IN  HPROTCONN           protConn,
    IN  int                 messageRoot,
    IN  entityId_t          *senderId,
    IN  int                 messageLen,
    IN  RvUint8             *messageBuf,
    IN  cmRASTransport      *srcAddress,
    IN  h235ContextInfo_t   *contextInfo)
{
    RvStatus ret;
    RvBool deleteMsg = RV_FALSE;

    H235HMSG H235HMsg = NULL;
    _SET_STATE(H235HMsg,contextInfo->state);
    _SET_SECUREMODE(H235HMsg,contextInfo->secureMode);
    _SET_REASON(H235HMsg,contextInfo->reason);

    if (h235App->eventHandlers.evFreeze == NULL)
    {
        ret = RV_ERROR_UNKNOWN;
    }
    else ret = h235App->eventHandlers.evFreeze(
        h235App->hApp, (H235HAPP)h235App, H235HMsg, protConn, messageRoot, messageBuf, messageLen, senderId,
        srcAddress, &deleteMsg);

    if (ret == RV_OK)
        h235SetContextInfo(contextInfo, h235StateSecurityIgnore, deleteMsg, h235ReasonUnknownSender);
    else
        h235SetContextInfo(contextInfo, h235StateSecurityFailed, RV_TRUE, h235ReasonUnknownSender);

    /****************************************/
    /* log                                  */
    /****************************************/
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
    {
        if (h235App->eventHandlers.evSuccess)
        {
            RvLogInfo(&h235App->h235Chan,
                (&h235App->h235Chan, "freeze message result:%d",ret));
        }
        else
        {
            RvLogInfo(&h235App->h235Chan,
                (&h235App->h235Chan, "freeze message failed: no callback set"));
        }
    }
#endif /* (RV_LOGMASK & RV_LOGLEVEL_INFO) */

    return ret;
}


/********************************************************************************
 * Routine Name:  checkMessageSecurity
 * Description : check an incoming message
 * Input: aApp - stack application handle.
 *        nodeId - the root of the message
 *        error - true if the message contains an error.
 * Output: (-)
 * Return: (-)
 ********************************************************************************/
static void checkMessageSecurity(
    IN  h235Element_T      *h235App,
    IN  HPROTCONN           protConn,
    IN  int                 messageRoot,
    IN  RvUint8            *messageBuf,
    IN  int                 messageLength,
    IN  cmRASTransport     *srcAddress,
    OUT h235ContextInfo_t  *contextInfo)
{
    cmProtocol protocol;
    RvStatus rv;
    int firstCryptoNode = -1;
    char msgName[64];

    if (!h235App->isCheckIncoming)
    {
        h235SetContextInfo(contextInfo, h235StateSecurityOK, RV_FALSE, h235ReasonNoSecureRequired);
        return;
    }

    protocol = cmProtocolGetProtocol(h235App->hApp,messageRoot);
    RvSprintf(msgName,"in:%s", cmGetProtocolMessageName(h235App->hApp,messageRoot));

    rv = h235GetFirstCryptoNodeFromMessage(h235App->hVal, messageRoot, protocol, &firstCryptoNode);
    if(rv != 0 || firstCryptoNode < 0)
    {
        failToApp(h235App,messageRoot,NULL,h235ReasonNoSecurityInfo,contextInfo);
        return;
    }
    checkAuthentication(h235App, messageRoot, messageBuf, messageLength, firstCryptoNode,
        protConn, srcAddress, contextInfo);
}


/*-------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------*/


/***********************************************************************************
 * Routine Name:  h235InitCallObject
 * Description : initialize the H.235 call structs
 * Input: h235HApp  - h235 instance handle.
 *        hCall     - handle to h235 call info.
 * Output: none.
 * Return: 0 - 0k , -1 - failed
 ***********************************************************************************/
RVAPI
int RVCALLCONV h235InitCallObject(
    IN  H235HAPP      h235HApp,
    IN  H235HCALL     hCall)
{
    h235Element_T* h235App = (h235Element_T*) h235HApp;
    h235CallInfo_t* call = (h235CallInfo_t*) hCall;

    RV_UNUSED_ARG(h235HApp);

    memset((void*)call, 0, sizeof(h235CallInfo_t));
    call->secureMode = h235App->secureMode;
    call->bSecureMedia = RV_TRUE;
    memset((void*)call->sendCryptoToken, -1, sizeof(call->sendCryptoToken));
    call->sendClearToken = RV_PVT_INVALID_NODEID;

    return RV_OK;
}


/***********************************************************************************
 * Routine Name:  h235InitCallObjectNoDH
 * Description : initialize the H.235 call structs without the Diffie-Hellman
 *               components
 * Input: h235HApp  - h235 instance handle.
 *        hCall     - handle to h235 call info.
 * Output: none.
 * Return: 0 - 0k , -1 - failed
 ***********************************************************************************/
RVAPI
int RVCALLCONV h235InitCallObjectNoDH(
    IN  H235HAPP      h235HApp,
    IN  H235HCALL     hCall)
{
    h235Element_T* h235App = (h235Element_T*) h235HApp;
    h235CallInfo_t* call = (h235CallInfo_t*) hCall;

    RV_UNUSED_ARG(h235HApp);

    memset((void*)call, 0, sizeof(h235CallInfo_t)-DH_SIZE);
    call->secureMode = h235App->secureMode;
    memset((void*)call->sendCryptoToken, -1, sizeof(call->sendCryptoToken));
    call->sendClearToken = RV_PVT_INVALID_NODEID;

    return RV_OK;
}


/***********************************************************************************
 * Routine Name:  h235EndCallObject
 * Description : end the H.235 call structs
 * Input: h235HApp  - h235 instance handle.
 *        hCall     - handle to h235 call info.
 * Output: none
 * Return: 0 - 0k , -1 - failed
 ***********************************************************************************/
RVAPI
int RVCALLCONV h235EndCallObject(
    IN  H235HAPP      h235HApp,
    IN  H235HCALL     hCall)
{
    h235Element_T* h235App = (h235Element_T*) h235HApp;
    h235CallInfo_t *call = (h235CallInfo_t*) hCall;
    int i;

    if (call->bSecureMedia)
        call->inDHinf.isValid = call->outDHinf.isValid = RV_FALSE;

    for (i = 0; i < MAX_CALL_CRYPTO_TOKENS; i++)
    {
        pvtDelete(h235App->hVal, call->sendCryptoToken[i]);
        call->sendCryptoToken[i] = RV_PVT_INVALID_NODEID;
    }
    call->sendClearToken = RV_PVT_INVALID_NODEID;

    return RV_OK;
}


/***********************************************************************************
 * Routine Name:  h235SetCallMode
 * Description : set the H.235 call security mode
 * Input: hCall      - handle to h235 call info.
 *        secureMode - security mode to use.
 * Output: none.
 * Return: none
 ***********************************************************************************/
RVAPI
void RVCALLCONV h235SetCallMode(
    IN  H235HCALL     hCall,
    IN  h235Mode_t    secureMode)
{
    h235CallInfo_t* call = (h235CallInfo_t*) hCall;
    call->secureMode = secureMode;
}


/***********************************************************************************
 * Routine Name:  h235GetCallMode
 * Description : get the H.235 call security mode
 * Input: hCall      - handle to h235 call info.
 * Output: none.
 * Return: security mode to used.
 ***********************************************************************************/
RVAPI
h235Mode_t RVCALLCONV h235GetCallMode(
    IN  H235HCALL     hCall)
{
    h235CallInfo_t* call = (h235CallInfo_t*) hCall;
    return call->secureMode;
}


/***********************************************************************************
 * Routine Name:  h235CallSendCryptoTokenOnCall
 * Description : add a crypto token to be sent in the next message of a call
 * Input: h235HApp    - handle to h235 call info.
 *        hCall       - handle to h235 call info.
 *        cryptoToken - node Id of the token to add.
 * Output: none.
 * Return: none
 ***********************************************************************************/
RVAPI
RvStatus RVCALLCONV h235CallSendCryptoTokenOnCall(
    IN  H235HAPP      h235HApp,
    IN  H235HCALL     hCall,
    IN  RvPvtNodeId   cryptoToken)
{
    h235CallInfo_t* callInfoPtr = (h235CallInfo_t*)hCall;
    int i;
    RV_UNUSED_ARG(h235HApp);
    for (i = 0; i < MAX_CALL_CRYPTO_TOKENS; i++)
    {
        if (!RV_PVT_NODEID_IS_VALID(callInfoPtr->sendCryptoToken[i]))
        {
            callInfoPtr->sendCryptoToken[i] = cryptoToken;
            return RV_OK;
        }
    }
    return RV_ERROR_OUTOFRESOURCES;
}


static int h235PrintTree(IN h235Element_T* h235App,IN int tree,IN const char *str)
{
    RV_UNUSED_ARG(str);

    RvLogInfo(&h235App->h235Chan, (&h235App->h235Chan, str));
#ifdef H235_DEBUG
    pvtPrintInternal(hVal ,tree, &h235App->h235Chan, 3);
#else
    /* print only the security part of the message */
    {
        RvStatus rv;
        RvPvtNodeId cryptoTokenNode = -1, cryptoNode;
        cmProtocol protocol;
        h235Mode_t mode;

        protocol = cmProtocolGetProtocol(h235App->hApp,tree);
        rv = h235GetFirstCryptoNodeFromMessage(h235App->hVal, tree, protocol, &cryptoTokenNode);
        if(rv == 0 && cryptoTokenNode >= 0)
        {
            RvLogDebug(&h235App->h235Chan, (&h235App->h235Chan,"security part of the message:"));

            while (cryptoTokenNode >= 0)
            {
                mode = identifyNode(h235App->hVal, cryptoTokenNode, &cryptoNode);
                RvLogDebug(&h235App->h235Chan, (&h235App->h235Chan, h235ModeName(mode)));
                pvtPrintInternal(h235App->hVal ,cryptoTokenNode, &h235App->h235Chan, 3);
                cryptoTokenNode = pvtBrother(h235App->hVal, cryptoTokenNode);
            }
        }
    }
#endif
    return 0;

}


RVAPI
int RVCALLCONV h235GetSecurityCheckResultsByRAS(
    IN  H235HAPP        h235HApp,
    IN  HRAS            hsRas,
    OUT h235State_t     *state,
    OUT h235Reason_t    *reason,
    OUT H235HMSG        *h235HMsg)
{
    int err;
    RvUint32 msgContext;

    RV_UNUSED_ARG(h235HApp);

    err = cmiRASGetMessageContext(hsRas, (void **)&msgContext);

    if (err < 0)
        return -1;
    else
    {
        if (state)
        {
            *state = _GET_STATE(msgContext);
        }
        if (reason)
            *reason = _GET_REASON(msgContext);
        if (h235HMsg)
            *h235HMsg = (H235HMSG)msgContext;
    }
    return 0;
}


RVAPI
int RVCALLCONV h235GetSecurityCheckResultsByCall(
    IN  H235HAPP        h235HApp,
    IN  HCALL           hsCall,
    OUT h235State_t     *state,
    OUT h235Reason_t    *reason,
    OUT H235HMSG        *h235HMsg)
{
    int err;
    RvUint32 msgContext;
    RV_UNUSED_ARG(h235HApp);
    err = cmCallGetMessageContext(hsCall,(void **)&msgContext);
    if (err < 0)
        return -1;
    else
    {
        if (state)
        {
            *state = _GET_STATE(msgContext);
        }
        if (reason)
            *reason = _GET_REASON(msgContext);
        if (h235HMsg)
            *h235HMsg = (H235HMSG)msgContext;
    }
    return 0;
}


/* h235DefrostRasMessage = cmProtocolRecvRawMessageFrom (for RAS) or cmProtocolReceiveRawMessage (for Q.931) */


RVAPI
int RVCALLCONV h235RecheckRasMessage(
    IN  H235HAPP        h235HApp,
    IN  HPROTCONN       protConn,
    IN  RvUint8         *messageBuf,
    IN  int             messageLen,
    IN  cmRASTransport  *srcAddress)
{
    h235Element_T *h235App = (h235Element_T*)h235HApp;
    RvPvtNodeId msgRoot;
    RvStatus status;
    h235ContextInfo_t contextInfo;

    contextInfo.state = h235StateUndefined;
    contextInfo.isMsgDeleted = RV_FALSE;
    contextInfo.reason = h235ReasonUndefined;
    contextInfo.secureMode = h235ModeNone;

    msgRoot = pvtAddRoot(h235App->hVal, NULL, 0, NULL);

    status = cmEmDecode(h235App->hVal, msgRoot, messageBuf, messageLen, NULL);
    if (status < 0)
        return status;

    checkMessageSecurity(h235App, protConn, msgRoot, messageBuf, messageLen, srcAddress,  &contextInfo);
    if (contextInfo.isMsgDeleted == RV_TRUE)
        return RV_ERROR_UNKNOWN;
    return RV_OK;
}


RVAPI
int RVCALLCONV h235GetStateByHMSG(
    IN  H235HMSG        h235HMsg,
    OUT h235State_t     *state)
{
    *state = _GET_STATE((RvUint32)h235HMsg);
    return 0;
}


RVAPI
int RVCALLCONV h235GetReasonByHMSG(
    IN  H235HMSG        h235HMsg,
    OUT h235Reason_t    *reason)
{
    *reason = _GET_REASON((RvUint32)h235HMsg);
    return 0;
}


RVAPI
int RVCALLCONV h235GetModeByHMSG(
    IN  H235HMSG        h235HMsg,
    OUT h235Mode_t      *secureMode)
{
    *secureMode = _GET_SECUREMODE((RvUint32)h235HMsg);
    return 0;
}


RVAPI
const char* RVCALLCONV h235ReasonName(
    IN   h235Reason_t reason)
{
    switch (reason)
    {
    case h235ReasonUndefined:          return "Undefined";
    case h235ReasonNoSecureRequired:   return "NoSecureRequired";
    case h235ReasonNoSecurityInfo:     return "NoSecurityInfo";
    case h235ReasonMessageError:       return "MessageError";
    case h235ReasonSendersIDMissing:   return "SendersIDMissing";
    case h235ReasonGeneralIDIncorrect: return "GeneralIDIncorrect";
    case h235ReasonUnknownSender:      return "UnknownSender";
    case h235ReasonProcNotSupported:   return "ProcNotSupported";
    case h235ReasonTimeStampMissing:   return "TimeStampMissing";
    case h235ReasonTimeStampFail:      return "TimeStampFail";
    case h235ReasonTimeRandomFail:     return "TimeRandomFail";
    case h235ReasonIncorrectAuth:      return "IncorrectAuth";
    case h235ReasonEncodeCBMissing:    return "EncodeCBMissing";
    case h235ReasonAppForceFail:       return "AppForceFail";
    case h235ReasonGeneralError:       return "GeneralError";
    case h235ReasonEndToEndTokenSenderUnknown: return "EndToEndTokenSenderUnknown";
    default:                           return "reason unknown";
    }
}


RVAPI
const char* RVCALLCONV h235ModeName(
    IN   h235Mode_t secureMode)
{
    static char ret[64];
    ret[0] = 0;

    if (secureMode == h235ModeNone)
        strcpy(ret, "None");
    if (secureMode & h235ModeProcedure1)
        strcpy(ret, "Procedure1");
    if (secureMode & h235ModeProcedure1a)
    {
        if (ret[0])
            strcat(ret, "+1a");
        else
            strcpy(ret, "Procedure1a");
    }
    if (secureMode & h235ModeProcedure2)
    {
        if (ret[0])
            strcat(ret, "+2");
        else
            strcpy(ret, "Procedure2");
    }
    if (secureMode & h235ModeProcedure3)
    {
        if (ret[0])
            strcat(ret, "+3");
        else
            strcpy(ret, "Procedure3");
    }
    if (secureMode & h235ModeProcedure4)
    {
        if (ret[0])
            strcat(ret, "+4");
        else
            strcpy(ret, "Procedure4");
    }
    if (secureMode & h235ModeProcedure4DH)
    {
        if (ret[0])
            strcat(ret, "+4DH");
        else
            strcpy(ret, "Procedure4DH");
    }
    if (secureMode & h235ModeDHgroup512)
    {
        if (ret[0])
            strcat(ret, "+DH-512");
        else
            strcpy(ret, "Diffie-Hellman 512");
    }
    if (secureMode & h235ModeDHgroup1024)
    {
        if (ret[0])
            strcat(ret, "+DH-1024");
        else
            strcpy(ret, "Diffie-Hellman 1024");
    }
    if (secureMode & h235ModeDHgroup1536)
    {
        if (ret[0])
            strcat(ret, "+DH-1536");
        else
            strcpy(ret, "Diffie-Hellman 1536");
    }
    return ret;
}


RVAPI
const char* RVCALLCONV h235StateName(
    IN   h235State_t    state)
{
    switch (state)
    {
    case h235StateUndefined:       return "Undefined";
    case h235StateSecurityOK:      return "SecurityOK";
    case h235StateSecurityFailed:  return "SecurityFailed";
    default:                       return "state unknown";
    }
}


RVAPI
const char* RVCALLCONV h235GetVersion(void)
{
    return RV_H323_STACK_VERSION;
}


void h235APIEnter(IN  h235Element_T* h235App)
{
    if (h235App->isGK)
        RvMutexLock(&h235App->mutex, h235App->logMgr);
}


void h235APIExit(IN  h235Element_T* h235App)
{
    if (h235App->isGK)
        RvMutexUnlock(&h235App->mutex, h235App->logMgr);
}


/*-------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------*/


/***********************************************************************************
 * Routine Name:  h235GenerateDiffieHellman
 * Description : generate the Diffie-Hellman HalfKey
 * Input:  h235HApp    - h235 instance handle.
 *         DHMode      - mode of the DH half-key to be generated.
 *         hCall       - handle to h235 call info.
 * Output: none.
 * Return: 0 - 0k , -1 - failed
 ***********************************************************************************/
RVAPI
RvStatus RVCALLCONV h235GenerateDiffieHellman(
    IN  H235HAPP       h235HApp,
    IN  h235DHMode_t   DHMode,
    IN  H235HCALL      hCall)
{
    h235Element_T *h235App = (h235Element_T*)h235HApp;
    h235CallInfo_t *call = (h235CallInfo_t*) hCall;
    RV_UNUSED_ARG(hCall);
    if (call->bSecureMedia)
        return generateDiffieHellman(h235App, DHMode, &call->inDHinf, &call->outDHinf, call->myRandomNumber);
    return RV_ERROR_UNINITIALIZED;
}


/***********************************************************************************
 * Routine Name:  h235GenerateDiffieHellman
 * Description : generate the Diffie Hellman HalfKey
 * Input:  h235HApp    - h235 instance handle.
 *         DHMode      - ignored.
 *         hEntity     - handle to h235 entity info.
 * Output: none.
 * Return: 0 - 0k , -1 - failed
 ***********************************************************************************/
RVAPI
RvStatus RVCALLCONV h235GenerateDiffieHellmanEP(
    IN  H235HAPP       h235HApp,
    IN  h235DHMode_t   DHMode,
    IN  H235HENTITY    hEntity)
{
    h235Element_T *h235App = (h235Element_T*)h235HApp;
    h235EntityEntry_t *entity = (h235EntityEntry_t*) hEntity;
    if (entity->bGenerateSecret)
        return generateDiffieHellman(h235App, DHMode, &entity->inDHinf, &entity->outDHinf, entity->myRandomNumber);
    return RV_ERROR_UNINITIALIZED;
}


/***********************************************************************************
 * Routine Name:  h235AddDiffieHellman
 * Description : add the Diffie Hellman HalfKey to messageRoot
 * Input: DHMode      - mode of DH to use. irrelevant if replying
 *        messageRoot - the message node Id
 *        hCall       - handle to h235 call info.
 * Output: none
 * Return: 0 - 0k , -1 - failed
 ***********************************************************************************/
RVAPI
RvStatus RVCALLCONV h235AddDiffieHellman(
    IN  H235HAPP      h235HApp,
    IN  h235DHMode_t  DHMode,
    IN  RvPvtNodeId   messageRoot,
    IN  HCALL         hsCall,
    IN  H235HCALL     hSecCall)
{
    h235CallInfo_t *call = (h235CallInfo_t*)hSecCall;
    h235Element_T *h235App = (h235Element_T*)h235HApp;
    RvBool bOutValid = RV_FALSE;
    h235DiffieHellmanInfo_t *dhInf;

    RV_UNUSED_ARG(hsCall);

    /* we use this function only with procedure 1 */
    if (!call->bSecureMedia)
        return RV_ERROR_UNKNOWN;

    /* if we already received DH parameters, use that protocol */
    if(call->inDHinf.isValid)
        DHMode = call->inDHinf.DHMode;
    /* if we already generated DH parameters, use them */
    dhInf = &call->outDHinf;
    if(dhInf->isValid && (dhInf->DHMode == DHMode))
        bOutValid = RV_TRUE;

    /* make sure we got ONE of the supported types */
    if ((DHMode != h235ModeDiffieHellman512) &&
        (DHMode != h235ModeDiffieHellman1024) &&
        (DHMode != h235ModeDiffieHellman1536))
        return RV_ERROR_UNKNOWN;

    if(bOutValid || (h235GenerateDiffieHellman(h235HApp, DHMode, hSecCall) == RV_OK))
    {
        RvPvtNodeId tempNode;

        /* Create a branch, we're adding the DH params at the bottom -
        procedure 1 may later mode this branch downwards. */
        __pvtBuildByFieldIds(tempNode, h235App->hVal, messageRoot,
            {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu)
             _q931(h323_message_body) _anyField _q931(cryptoTokens) _nul(0) LAST_TOKEN}, 0, NULL);

        return setDHP1TokenNode(h235App->hVal, DHMode, tempNode, dhInf);
    }

    return RV_ERROR_UNKNOWN;
}


/********************************************************************************
 * Function Name:   h235GetIncomingDiffieHellmanHalfKey
 * Description :    Gets the Diffie Hleman HalfKey from an incoming message
 * Input: h235HApp    - h235 instance handle.
 *        messageNode - the incoming message from which to get the DH HalfKey
 *        hCall       - handle to h235 call info.
 *        dhMode      - type of incoming Diffie-Hellman to look for, h235ModeNone
 *                      for any node.
 * Output: dhMode     - mode of the incoming Diffie-Hellman
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetIncomingDiffieHellmanHalfKey(
    IN  H235HAPP      h235HApp,
    IN  RvPvtNodeId   messageNode,
    IN  H235HCALL     hCall,
    INOUT h235DHMode_t *dhMode)
{
    h235CallInfo_t *call = (h235CallInfo_t*) hCall;
    h235Element_T *h235App = (h235Element_T*)h235HApp;
    RvPvtNodeId curCryptoTokenNode, cryptoNode;

    if (!call->bSecureMedia)
        return RV_ERROR_UNINITIALIZED;

    if (call->outDHinf.isValid)
    {
        if (call->outDHinf.DHMode == h235ModeDiffieHellmanNone)
            return RV_ERROR_UNKNOWN;
        *dhMode = call->outDHinf.DHMode;
    }

    __pvtGetNodeIdByFieldIds(curCryptoTokenNode, h235App->hVal, messageNode,
        {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body)
        _anyField _q931(cryptoTokens) _nul(1) LAST_TOKEN});

    if (RV_PVT_NODEID_IS_VALID(curCryptoTokenNode))
    {
        while (RV_PVT_NODEID_IS_VALID(curCryptoTokenNode))
        {
            h235Mode_t secMode = identifyNode(h235App->hVal, curCryptoTokenNode, &cryptoNode);

            /* see if this node has DH information in it */
            if ((secMode & RV_H235_ALL_DH_MODES) != h235ModeNone)
            {
                /* if we want any node, return the type of node we have */
                if (*dhMode == h235ModeDiffieHellmanNone)
                {
                    *dhMode = RV_H235_MODE_TO_DH_MODE(secMode);
                }
                /* if we want a specific one, see if this is it */
                if ((*dhMode & RV_H235_MODE_TO_DH_MODE(secMode)) != h235ModeDiffieHellmanNone)
                {
                    if (getDHTokenNode(h235App->hVal, curCryptoTokenNode, RV_PVT_INVALID_NODEID, &call->inDHinf) == RV_OK)
                    {
                        call->inDHinf.DHMode = RV_H235_MODE_TO_DH_MODE(secMode);
                        return RV_OK;
                    }
                }
            }
            curCryptoTokenNode = pvtBrother(h235App->hVal, curCryptoTokenNode);
        }
    }
    else
    {
        RvPvtNodeId curClearTokenNode;

        __pvtGetNodeIdByFieldIds(curClearTokenNode, h235App->hVal, messageNode,
            {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body)
            _anyField _q931(tokens) _nul(1) LAST_TOKEN});

        while (RV_PVT_NODEID_IS_VALID(curClearTokenNode))
        {
            h235Mode_t secMode = identifyNode(h235App->hVal, curClearTokenNode, &cryptoNode);
            secMode &= ~h235ModeDhInClear;

            /* see if this node has DH information in it */
            if ((secMode & RV_H235_ALL_DH_MODES) != h235ModeNone)
            {
                /* if we want any node, return the type of node we have */
                if (*dhMode == h235ModeDiffieHellmanNone)
                {
                    *dhMode = RV_H235_MODE_TO_DH_MODE(secMode);
                }
                /* if we want a specific one, see if this is it */
                if ((*dhMode & RV_H235_MODE_TO_DH_MODE(secMode)) != h235ModeDiffieHellmanNone)
                {
                    if (getDHTokenNode(h235App->hVal, RV_PVT_INVALID_NODEID, curClearTokenNode, &call->inDHinf) == RV_OK)
                    {
                        call->inDHinf.DHMode = RV_H235_MODE_TO_DH_MODE(secMode);
                        return RV_OK;
                    }
                }
            }
            curClearTokenNode = pvtBrother(h235App->hVal, curClearTokenNode);
        }
    }
    return RV_ERROR_UNKNOWN;
}


/********************************************************************************
 * Function Name:   h235GenarateDiffieHellmanSharedSecret
 * Description :    Generates a shared secret of the desired length, according to
 *                 the protocol used. Should be called after generating a halfkey
 *                 and receiving a halfkey.
 * Input:  h235HApp     - h235 instance handle.
 *         hCall        - handle to h235 call info.
 *         leastSignificantFirst - true iff the least significant should be the first byte.
 * Output: SharedSecret - buffer containing the generated Shared Secret,
 *                        it is expected to be of enough size for the protocol
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
RvStatus RVCALLCONV h235GenarateDiffieHellmanSharedSecret(
    IN  H235HCALL     hCall,
    IN  RvBool        leastSignificantFirst,
    OUT char *        sharedSecret)
{
    h235CallInfo_t *call = (h235CallInfo_t*) hCall;

    if (!call->bSecureMedia)
        return RV_ERROR_UNINITIALIZED;

    /* sanity checks - make sure Diffie-Hellman structs are valid */
    if (!call->inDHinf.isValid || !call->outDHinf.isValid)
        return RV_ERROR_UNKNOWN;
    /* sanity checks - make sure the protocols are the same */
    if (call->inDHinf.DHMode != call->outDHinf.DHMode)
        return RV_ERROR_UNKNOWN;
    /* sanity checks - make sure the generators are the same */
    if (memcmp(call->inDHinf.generator, call->outDHinf.generator, 256) != 0)
        return RV_ERROR_UNKNOWN;
    /* sanity checks - make sure the primes are the same */
    if (memcmp(call->inDHinf.modSize, call->outDHinf.modSize, 256) != 0)
        return RV_ERROR_UNKNOWN;

    /* OK, we should be sane now. */
    return genarateDHSharedSecret(&call->inDHinf, leastSignificantFirst, call->myRandomNumber, sharedSecret);
}


/********************************************************************************
 * Function Name:   h235GenarateDiffieHellmanSharedSecret
 * Description :    Generates a shared secret of the desired length, according to
 *                 the protocol used. Should be called after generating a halfkey
 *                 and receiving a halfkey.
 * Input:  h235HApp     - h235 instance handle.
 *         hCall        - handle to h235 call info.
 *         leastSignificantFirst - true iff the least significant should be the first byte.
 * Output: SharedSecret - buffer containing the generated Shared Secret,
 *                        it is expected to be of enough size for the protocol
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
RvStatus RVCALLCONV h235GenarateDiffieHellmanSharedSecretEP(
    IN  H235HENTITY   hEntity,
    IN  RvBool        leastSignificantFirst,
    OUT char *        sharedSecret)
{
    h235EntityEntry_t *entity = (h235EntityEntry_t*) hEntity;

    if (!entity->bGenerateSecret)
        return RV_ERROR_UNINITIALIZED;

    /* sanity checks - make sure Diffie-Hellman structs are valid */
    if (!entity->inDHinf.isValid || !entity->outDHinf.isValid)
        return RV_ERROR_UNKNOWN;
    /* sanity checks - make sure the protocols are the same */
    if (entity->inDHinf.DHMode != entity->outDHinf.DHMode)
        return RV_ERROR_UNKNOWN;
    /* sanity checks - make sure the generators are the same */
    if (memcmp(entity->inDHinf.generator, entity->outDHinf.generator, 256) != 0)
        return RV_ERROR_UNKNOWN;
    /* sanity checks - make sure the primes are the same */
    if (memcmp(entity->inDHinf.modSize, entity->outDHinf.modSize, 256) != 0)
        return RV_ERROR_UNKNOWN;

    /* OK, we should be sane now. */
    return genarateDHSharedSecret(&entity->inDHinf, leastSignificantFirst, entity->myRandomNumber, sharedSecret);
}


RvStatus diffieHellmanTest(H235HAPP h235hApp)
{
    h235CallInfo_t call1;
    h235CallInfo_t call2;
    char secret1[21] = "0";
    char secret2[21] = "1";

    memset(&call1, 0, sizeof(call1));
    memset(&call2, 0, sizeof(call2));
    memset(secret1, 0, sizeof(secret1));
    memset(secret2, 0, sizeof(secret2));

    call1.bSecureMedia = call2.bSecureMedia = RV_TRUE;

    h235GenerateDiffieHellman(h235hApp, h235ModeDiffieHellman1024, (H235HCALL) &call1);
    memcpy(&call2.inDHinf, &call1.outDHinf, sizeof(call1.outDHinf));
    h235GenerateDiffieHellman(h235hApp, h235ModeDiffieHellman1024, (H235HCALL) &call2);
    memcpy(&call1.inDHinf, &call2.outDHinf, sizeof(call2.outDHinf));

    h235GenarateDiffieHellmanSharedSecret((H235HCALL) &call1, RV_TRUE, secret1);
    h235GenarateDiffieHellmanSharedSecret((H235HCALL) &call1, RV_TRUE, secret2);

    if(memcmp(secret1, secret2, 21) != 0)
        return RV_ERROR_UNKNOWN;

    return RV_OK;
}

//{{gaoshizhong 20131125 Add
RVAPI void RVCALLCONV ConvertUint16ToChar(unsigned char * destBuf,RvUint16 * srcBuf,int convertsrcLen)
{
	int i=0;
	int j=0;
	for(i=255, j=0; i>=0 && j<convertsrcLen; i-=2, j++)
	{
		destBuf[i] = (RvUint8) srcBuf[j];
		destBuf[i-1] = (RvUint8) (srcBuf[j] >> 8);
	}
}

RVAPI void RVCALLCONV ConvertCharToUint16(RvUint16 * destBuf,unsigned char * srcBuf,int convertsrcLen)
{
	int i=0;
	int j=0;
	for(i=255, j=0; i>=0 && j<convertsrcLen; i-=2, j++)
	{
		destBuf[j]=(srcBuf[i-1] << 8) + srcBuf[i];
	}
}

RVAPI void RVCALLCONV ConvertCharToHexChar(unsigned char * destBuf,unsigned char * srcBuf,int convertsrcLen)
{
	char value[]={"0123456789ABCDEF"};
	char * printBuf=(char *)destBuf;
	unsigned char heigh=0;
	unsigned char avlow=0;
	unsigned char avheigh=0;
	int i=0;
	int j=0;
	int av=0;

	for(i = 0; i < convertsrcLen; i++)
	{
		heigh=srcBuf[i];
		avheigh=(heigh & 0xf0)>>4;
		avlow=(heigh & 0x0f);
		av=avheigh;
		avheigh=value[av];
		av=avlow;
		avlow=value[av];

		printBuf[2*i]=avheigh;
		printBuf[2*i+1]=avlow;
		//sprintf(printBuf+2*i, "%02X",srcBuf[i]);
	}
}


RVAPI void RVCALLCONV ConvertHexCharToChar(unsigned char * destBuf,unsigned char * srcBuf,int convertsrcLen)
{
	char value[]={"0123456789ABCDEF"};
	unsigned char avlow=0;
	unsigned char avheigh=0;
	int i=0;
	int j=0;
	int k=0;
	for(i = 0 ,j=0; i < convertsrcLen; i+=2,j++)
	{
		for(k=0;k<16;k++)
		{
			if(srcBuf[i] == value[k]) 
			{
				avheigh=k;
				break;
			}
		}
		for(k=0;k<16;k++)
		{
			if(srcBuf[i+1] == value[k]) 
			{
				avlow=k;
				break;
			}
		}
		destBuf[j]=(avheigh << 4) + avlow;
	}
}
//}}gaoshizhong 20131125 Add


#ifdef __cplusplus
}
#endif
