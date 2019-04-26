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

/***********************************************************************
epSecurity.c

Support for H.235 security.
This module deals with all the security issues.
It is full of type definitions, allowing compilation with this module
and without it.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "h245.h"
#include "q931asn1.h"
#include "stkutils.h"

#include "lepUtils.h"
#include "lepSecurity.h"
#include "lepLdap.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifdef USE_SECURITY

#ifndef min
#define min(a,b) ((a)>(b) ? (b) : (a))
#endif


#ifdef USE_SECURITY_ALGORITHMS


/* We're using Open SSL for our security algorithms.
   These are not supplied as part of the test applications.
   You should supply your own algorithms to make this part work.
   Without these algorithms (i.e - USE_SECURITY_ALGORITHMS undefined), the security
   functions return the same input they receive, allowing you to test the basic
   functionality. */
#ifdef WIN32
#define OPENSSL_SYSNAME_WIN32
#endif
#define OPENSSL_THREADS
#define OPENSSL_NO_KRB5

#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/md5.h>
#include <openssl/des.h>

#else

/* No implementation of algorithms - we use an empty implementation */
#define RSA_free(_rsa)

#endif /* USE_SECURITY_ALGORITHMS */


/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/

#define CERTIFICATE_FILE "epPrvt.crt"




/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/
static int RVCALLCONV pickAPrime(
    IN     H235HKEY         keyHandle,
    IN OUT void             *buf,
    IN     int              bufLen,
    OUT    unsigned char    *digest,
    OUT    int              *len);
static void encryptVector(
    IN h235DHMode_t     dhMode,
    IN unsigned char    *vector,
    IN int              vecLen,
    IN char             *sharedSecret);
static void decryptVector(
    IN h235DHMode_t     dhMode,
    IN unsigned char    *vector,
    IN int              vecLen,
    IN char             *sharedSecret);




/*-----------------------------------------------------------------------*/
/*                           CALLBACK FUNCTIONS                          */
/*-----------------------------------------------------------------------*/


static h235AppFailReply_t RVCALLCONV  h235Failure(
    IN  HAPP        hApp,
    IN  H235HAPP    h235HApp,
    IN  H235HMSG    h235HMsg,
    IN  int         messageRoot,
    IN  entityId_t  *senderId,
    IN h235Reason_t reason)
{
    EpObj *ep;
    h235AppFailReply_t reply;
    cmProtocol prot;
    char * curCrtDn;

    RV_UNUSED_ARG(h235HApp);
    RV_UNUSED_ARG(h235HMsg);
    RV_UNUSED_ARG(senderId);

    cmGetHandle(hApp, (HAPPAPP *)&ep);

    if ((reason == h235ReasonEndToEndTokenSenderUnknown) && (epLdapIsConnected(ep, &curCrtDn)))
    {
        /* check if we aren't already busy */
        if (curCrtDn[0] == 0)
            return h235FailReplyFreezeMessage;
    }


    switch (reason)
    {
        case h235ReasonNoSecurityInfo:      reply = h235FailReplyForceOk;           break;
        case h235ReasonMessageError:        reply = h235FailReplyDeleteMessage;     break;
        case h235ReasonGeneralIDIncorrect:  reply = h235FailReplyForceOk;           break;
        case h235ReasonUnknownSender:       reply = h235FailReplyContinue;          break;
        case h235ReasonProcNotSupported:    reply = h235FailReplyContinue;          break;
        case h235ReasonTimeStampMissing:    reply = h235FailReplyContinue;          break;
        case h235ReasonTimeStampFail:       reply = h235FailReplyContinue;          break;
        case h235ReasonTimeRandomFail:      reply = h235FailReplyContinue;          break;
        case h235ReasonIncorrectAuth:       reply = h235FailReplyDeleteMessage;     break;
        case h235ReasonGeneralError:        reply = h235FailReplyDeleteMessage;     break;
        case h235ReasonEndToEndTokenSenderUnknown: reply = h235FailReplyForceOk;    break;
        default: reply = h235FailReplyContinue;
    }

    /* Reject messages if we might have security denial, therefore
       we will not fail them on security - we'd like the application to know about them */
    prot = cmProtocolGetProtocol(hApp, messageRoot);
    if (prot == cmProtocolRAS)
    {
        RvPstFieldId fieldId = -1;
        pvtGet(ep->hVal, pvtChild(ep->hVal, messageRoot), &fieldId, NULL, NULL, NULL);
        if ((fieldId == __q931(registrationReject)) ||
            (fieldId == __q931(admissionReject)) ||
            (fieldId == __q931(disengageReject)))
        {
            reply = h235FailReplyForceOk;
        }
    }

    /* Do some logging */
    {
        const RvChar *replyStr;
        char * messageName = cmGetProtocolMessageName(hApp, messageRoot);

        epUtilsLog(ep, NULL, "SEC: Incoming message '%s' failed security (%s)",
            cmGetProtocolMessageName(hApp, messageRoot), h235ReasonName(reason));

        if ((strncmp(messageName, "disengage", 9) != 0) && (strcmp(messageName, "releaseComplete") != 0))
        {
            epUtilsEvent(ep, "SEC", NULL, "h235Failure.%s.%s",
                cmGetProtocolMessageName(hApp, messageRoot), h235ReasonName(reason));
        }

        switch (reply)
        {
            case h235FailReplyContinue:         replyStr = "continue"; break;
            case h235FailReplyDeleteMessage:    replyStr = "delete incoming message"; break;
            case h235FailReplyForceOk:          replyStr = "ignore security failure"; break;
            default:                            replyStr = "bug"; /* this option is impossible */
        }
        epUtilsLog(ep, NULL, "SEC: application reply: %s", replyStr);
    }

    return reply;
}


static h235AppSuccessReply_t RVCALLCONV h235Success(
    IN  HAPP        hApp,
    IN  H235HAPP    h235HApp,
    IN  H235HMSG    h235HMsg,
    IN  RvPvtNodeId messageRoot,
    IN  entityId_t  *senderId)
{
    EpObj *ep;
    h235Mode_t secMode;
    RvChar msgText[128];

    RV_UNUSED_ARG(h235HApp);
    RV_UNUSED_ARG(senderId);

    cmGetHandle(hApp, (HAPPAPP *)&ep);

    h235GetModeByHMSG(h235HMsg, &secMode);

    msgText[0] = '\0';
    if (secMode & h235ModeProcedure1)
    {
        strcat(msgText, " Proc1");
    }
    if (secMode & h235ModeProcedure1a)
    {
        strcat(msgText, " Proc1A");
    }
    if (secMode & h235ModeProcedure2)
    {
        strcat(msgText, " Proc2");
    }
    if (secMode & h235ModeProcedure3)
    {
        strcat(msgText, " Proc3");
    }
    if (secMode & h235ModeProcedure4)
    {
        strcat(msgText, " Proc4");
    }

    epUtilsLog(ep, NULL, "SEC: Incoming message %s passed security by%s",
        cmGetProtocolMessageName(hApp, messageRoot), msgText);
    {
        char * messageName = cmGetProtocolMessageName(hApp, messageRoot);
        if ((strncmp(messageName, "disengage", 9) != 0) && (strcmp(messageName, "releaseComplete") != 0))
        {
            epUtilsEvent(ep, "SEC", NULL, "h235Success.%s.%x",
                cmGetProtocolMessageName(hApp, messageRoot), secMode);
        }
    }

    return h235SuccessReplyContinue;
}


RvStatus RVCALLCONV h235Freeze(
    IN  HAPP        hApp,
    IN  H235HAPP    h235HApp,
    IN  H235HMSG    h235HMsg,
    IN  HPROTCONN   protConn,
    IN  int         nodeId,
    IN  RvUint8*    buffer,
    IN  int         bufLen,
    IN  entityId_t *senderId,
    IN  cmRASTransport *srcAddress,
    OUT RvBool     *deleteMsg)
{
    EpObj *ep;
    frozenMessage * pMsg;
    char senderDn[256];

    RV_UNUSED_ARG(h235HApp);
    RV_UNUSED_ARG(h235HMsg);
    RV_UNUSED_ARG(nodeId);

    cmGetHandle(hApp, (HAPPAPP *)&ep);

    pMsg = ep->cb.epMalloc(ep, sizeof(frozenMessage)+bufLen, __FILE__, __LINE__);

    memset(pMsg, 0, sizeof(frozenMessage));
    pMsg->protConn = protConn;
    pMsg->msgLen = bufLen;
    if (srcAddress)
        pMsg->srcAddress = *srcAddress;
    pMsg->message = ((RvUint8*)pMsg)+sizeof(frozenMessage);
    memcpy(pMsg->message, buffer, bufLen);
    pMsg->ep = ep;

    utlBmp2Chr(senderDn, senderId->data, senderId->length);

    *deleteMsg = RV_TRUE;

    if (epLdapSendH235Quarry(ep, senderDn, pMsg) != RV_OK)
    {
        ep->cb.epFree(ep, pMsg);
        return RV_ERROR_UNKNOWN;
    }
    return RV_OK;
}


static void RVCALLCONV h235EvSetKeyHandle(
    IN  HAPP        hApp,
    IN  H235HAPP    h235HApp,
    IN  RvUint8     newKey[20],
    INOUT H235HKEY* keyHandle)
{
    EpObj *ep = (EpObj *)(*keyHandle);
    RvChar prnBuf[41];
    RvInt i;

    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(h235HApp);

    memcpy(ep->key, (void*)newKey, 20);

    for(i = 0; i < 20; i++)
    {
        sprintf(prnBuf+2*i, "%02X", newKey[i]);
    }
    prnBuf[2*i] = 0;

    epUtilsLog(ep, NULL, "SEC: Calculated Password is '%s'", prnBuf);
    epUtilsEvent(ep, "SEC", NULL, "h235EvSetKeyHandle");
}


static H235HCALL RVCALLCONV h235EvGetH235Call(
    IN  HCALL       hsCall,
    IN  HAPPCALL    haCall)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hsCall);

    if (call == NULL)
        return NULL;

    return call->hSecCall;
}


static RvBool RVCALLCONV h235EvGetIDByCall(
    IN  HAPP        hApp,
    IN  H235HAPP    h235HApp,
    IN  HCALL       hsCall,
    IN  HAPPCALL    haCall,
    OUT entityId_t  *generalID)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(h235HApp);
    RV_UNUSED_ARG(hsCall);

    if (call == NULL)
        return RV_FALSE;

    *generalID = call->remoteEndpoint;

    return RV_TRUE;
}


static RvBool RVCALLCONV h235EvGetIDsByCall(
    IN  HAPP        hApp,
    IN  H235HAPP    h235HApp,
    IN  HCALL       hsCall,
    IN  HAPPCALL    haCall,
    OUT entityId_t  *senderID,
    OUT entityId_t  *generalID)
{
    EpCallObj *call = (EpCallObj *)haCall;

    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(h235HApp);
    RV_UNUSED_ARG(hsCall);

    if (call == NULL)
        return RV_FALSE;

    *senderID = call->ep->cfg.senderId;
    *generalID = call->remoteEndpoint;

    return RV_TRUE;
}


static void RVCALLCONV h235EvChooseCallSecureMode(
    IN  HCALL       hsCall,
    IN  HAPPCALL    haCall,
    IN  H235HCALL   h235Call,
    INOUT h235Mode_t *secureMode)
{
    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(haCall);
    RV_UNUSED_ARG(h235Call);

    if ((RvUint32)*secureMode & (RvUint32)h235ModeDHgroup1536)
    {
        *secureMode = (h235Mode_t)((RvUint32)(*secureMode) & ~(RvUint32)h235ModeDHgroup1024);
        *secureMode = (h235Mode_t)((RvUint32)(*secureMode) & ~(RvUint32)h235ModeDHgroup512);
    }
    if ((RvUint32)*secureMode & (RvUint32)h235ModeDHgroup1024)
        *secureMode = (h235Mode_t)((RvUint32)(*secureMode) & ~(RvUint32)h235ModeDHgroup512);
}


static RvBool RVCALLCONV h235EvGetCrtHandleById(
    IN  HAPP        hApp,
    IN  H235HAPP    h235HApp,
    IN  entityId_t* endpointID,
    OUT H235HKEY*   crtInHandle,
    OUT H235HKEY*   crtOutHandle)
{
    RV_UNUSED_ARG(h235HApp);
    RV_UNUSED_ARG(endpointID);
    RV_UNUSED_ARG(crtInHandle);
    RV_UNUSED_ARG(crtOutHandle);

#ifdef USE_SECURITY_ALGORITHMS
    {
        EpObj *ep;
        char *curSenderDn;

        cmGetHandle(hApp, (HAPPAPP *)&ep);

        if (epLdapIsConnected(ep, &curSenderDn))
        {
            char senderDn[128];
            utlBmp2Chr(senderDn, endpointID->data, endpointID->length);
            if (strcmp(senderDn, curSenderDn) != 0)
            {
                /* no certificate waiting */
                return RV_FALSE;
            }
        }
        /* no LDAP or already got certificate */
        *crtInHandle = (H235HKEY)&ep->tmpRSA;
    }

#else /* USE_SECURITY_ALGORITHMS */
    RV_UNUSED_ARG(hApp);
#endif /* USE_SECURITY_ALGORITHMS */

    return RV_TRUE;
}


static RvBool RVCALLCONV h235EvGetCrtByHandle(
    IN  HAPP hApp,
    IN  H235HAPP h235HApp,
    IN  H235HKEY h235hCrt,
    OUT RvUint8** certificate,
    OUT RvSize_t* size,
    OUT h235CertificateType_t* type)
{
    RV_UNUSED_ARG(h235HApp);

#ifdef USE_SECURITY_ALGORITHMS
    {
        EpObj *ep;

        cmGetHandle(hApp, (HAPPAPP *)&ep);

        if (h235hCrt == (H235HKEY)&ep->myRSA)
        {
            /* outgoing message, H.235 is getting my public key */
            *certificate = (RvUint8*)ep->publicKey;
            *size = ep->publicKeySize;
            *type = h235CertificateSHA1;
        }
        if (h235hCrt == (H235HKEY)&ep->gkRSA)
        {
            /* incoming message. get the public key if we don't have it */
            if (ep->gkRSA.rsa == NULL)
            {
                d2i_RSA_PUBKEY((RSA**)&ep->gkRSA.rsa, (const unsigned char **)certificate, *size);
            }
        }
        if (h235hCrt == (H235HKEY)&ep->tmpRSA)
        {
#ifdef USE_LDAP
            char *curSenderDn;
            if (epLdapIsConnected(ep, &curSenderDn))
            {
                if (curSenderDn[0] != 0)
                {
                    /* use the certificate received */
                    RvUint8 *crt = ep->lastCertificate;
                    ep->tmpRSA.rsa = (void*)RSA_new();
                    d2i_RSA_PUBKEY((RSA**)&ep->tmpRSA.rsa, (const unsigned char **)&crt, 160);
                }
            }
            else
#endif
            {
                ep->tmpRSA.rsa = (void*)RSA_new();
                d2i_RSA_PUBKEY((RSA**)&ep->tmpRSA.rsa, (const unsigned char **)certificate, *size);
            }
        }
    }

#else /* USE_SECURITY_ALGORITHMS */
    RV_UNUSED_ARG(hApp);

    if (((int)h235hCrt) == 1)
    {
        *certificate = (RvUint8*)"private key";
        *size = strlen((RvChar*)*certificate);
        *type = h235CertificateSHA1;
    }
    else
    {
        *certificate = (RvUint8*)"public key";
        *size = strlen((RvChar*)*certificate);
        *type = h235CertificateSHA1;
    }
#endif /* USE_SECURITY_ALGORITHMS */

    return RV_TRUE;
}


static int RVCALLCONV hash_SH1_96Callback(
    IN     H235HKEY         keyHandle,
    IN OUT void             *buf,
    IN     int              bufLen,
    OUT    unsigned char    *digest,
    OUT    int              *len)
{
#if defined(USE_SECURITY_ALGORITHMS)
    EpObj *ep = (EpObj *)keyHandle;

    HMAC(EVP_sha1(), (void*)ep->key, 20, (unsigned char*)buf, bufLen, digest, (unsigned int*)len);

#else
    RV_UNUSED_ARG(keyHandle);
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(bufLen);
    memset(digest, 0, 12);
#endif /* USE_SECURITY_ALGORITHMS */

    *len = 12;  /* 96 bits length in bytes */
    return 0;
}


static int RVCALLCONV sign_SH1_Callback(
    IN     H235HKEY         keyHandle,
    IN OUT void             *buf,
    IN     int              bufLen,
    OUT    unsigned char    *digest,
    OUT    int              *len)
{
#ifdef USE_SECURITY_ALGORITHMS
    char hashed[SHA_DIGEST_LENGTH];
    SecurityCertificate *cert = (SecurityCertificate *)keyHandle;
    EpObj *ep;

    ep = cert->ep;

    SHA1((unsigned char*)buf, (unsigned long)bufLen, (unsigned char*)hashed);

    if (cert == &ep->myRSA)
    {
        /* we are asked to sign */
        memset(digest, 0, 128);
        *len = 0;

        /* if we are using LDAP, make sure the signature is synchronized */
        if (epLdapIsConnected(ep, NULL))
            epUtilsEvent(ep, "savePrivateCertificate", NULL, CERTIFICATE_FILE);

        if (RSA_sign(NID_sha1, (unsigned char*)hashed, SHA_DIGEST_LENGTH, digest, (unsigned int*)len, (RSA*)ep->myRSA.rsa))
        {
            *len = 128;  /* 1024 bits length in bytes */
            return RV_OK;
        }
    }
    if (cert == &ep->gkRSA)
    {
        /* we are asked to verify the GK's signature */
        if (RSA_verify(NID_sha1, (unsigned char*)hashed, SHA_DIGEST_LENGTH, digest, (unsigned int)*len, (RSA*)ep->gkRSA.rsa))
        {
            return RV_OK;
        }
    }
    if (cert == &ep->tmpRSA)
    {
        /* we are asked to verify the GK's signature */
        if (RSA_verify(NID_sha1, (unsigned char*)hashed, SHA_DIGEST_LENGTH, digest, (unsigned int)*len, (RSA*)ep->tmpRSA.rsa))
        {
            return RV_OK;
        }
    }
    return RV_ERROR_UNKNOWN;

#else /* USE_SECURITY_ALGORITHMS */
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(bufLen);
    if (((int)keyHandle) == 1)
    {
        memset(digest, 0, 128);
        *len = 128;
    }
    return RV_OK;
#endif /* USE_SECURITY_ALGORITHMS */
}


static int RVCALLCONV sign_MD5_Callback(
    IN     H235HKEY         keyHandle,
    IN OUT void             *buf,
    IN     int              bufLen,
    OUT    unsigned char    *digest,
    OUT    int              *len)
{
#ifdef USE_SECURITY_ALGORITHMS
    char hashed[MD5_DIGEST_LENGTH];
    SecurityCertificate *cert = (SecurityCertificate *)keyHandle;
    EpObj *ep;

    ep = cert->ep;

    MD5((unsigned char*)buf, (unsigned long)bufLen, (unsigned char*)hashed); /* or should we still use SHA1 hashing here? */

    if (cert == &ep->myRSA)
    {
        /* we are asked to sign */
        memset(digest, 0, 128);
        *len = 0;
        if (RSA_sign(NID_md5, (unsigned char*)hashed, MD5_DIGEST_LENGTH, digest, (unsigned int*)len, (RSA*)ep->myRSA.rsa))
        {
            *len = 128;  /* 1024 bits length in bytes */
            return RV_OK;
        }
        /* if we are using LDAP, make sure the signature is synchronized */
        if (epLdapIsConnected(ep, NULL))
            epUtilsEvent(ep, "savePrivateCertificate", NULL, CERTIFICATE_FILE);
    }
    if (cert == &ep->gkRSA)
    {
        /* we are asked to verify the GK's signature */
        if (RSA_verify(NID_md5, (unsigned char*)hashed, MD5_DIGEST_LENGTH, digest, (unsigned int)*len, (RSA*)ep->gkRSA.rsa))
        {
            return RV_OK;
        }
    }
    if (cert == &ep->tmpRSA)
    {
        /* we are asked to verify the GK's signature */
        if (RSA_verify(NID_md5, (unsigned char*)hashed, MD5_DIGEST_LENGTH, digest, (unsigned int)*len, (RSA*)ep->tmpRSA.rsa))
        {
            return RV_OK;
        }
    }
    return RV_ERROR_UNKNOWN;

#else /* USE_SECURITY_ALGORITHMS */
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(bufLen);
    if (((int)keyHandle) == 1)
    {
        memset(digest, 0, 128);
        *len = 128;
    }
    return RV_OK;
#endif /* USE_SECURITY_ALGORITHMS */
}






/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/
void generateCertificateFiles();


/******************************************************************************
 * lgkSecurityInit
 * ----------------------------------------------------------------------------
 * General: Initialize the security module of an endpoint object.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epSecurityInit(
    IN EpObj        *ep)
{
    RvStatus status;
    h235EventHandlers_T eh;
    RvUint8* ptr;

    status = h235Init(ep->hApp, &ep->h235App);
    if (status < 0)
        return status;

    ep->myRSA.ep = ep;
    ep->gkRSA.ep = ep;
    ep->tmpRSA.ep = ep;

    memset(&eh, 0, sizeof(h235EventHandlers_T));
    eh.evFailure                = h235Failure;
    eh.evSuccess                = h235Success;
    eh.evFreeze                 = h235Freeze;
    eh.evSetKeyHandle           = h235EvSetKeyHandle;
    eh.evGetH235Call            = h235EvGetH235Call;
    eh.evGetIDByCall            = h235EvGetIDByCall;
    eh.evChooseCallSecureMode   = h235EvChooseCallSecureMode;
    eh.evGetCrtHandleById       = h235EvGetCrtHandleById;
    eh.evGetCrtByHandle         = h235EvGetCrtByHandle;
    eh.evGetIDsByCall           = h235EvGetIDsByCall;
    h235SetEventHandlers(ep->h235App, &eh);

    h235SetEncodingCallback(ep->h235App, hash_SH1_96Callback, OID_U_STRING);
    h235SetEncodingCallback(ep->h235App, sign_MD5_Callback, OID_V_STRING);
    h235SetEncodingCallback(ep->h235App, sign_SH1_Callback, OID_W_STRING);
    h235SetTimeSync(ep->h235App, RV_TRUE, 86500); /*allow max 24 hours diff */

#if defined(USE_SECURITY_ALGORITHMS)
    {
        RvChar rnd_seed[] = "string to make the random number generator think it has entropy";

        /*CRYPTO_malloc_debug_init();*/
        CRYPTO_dbg_set_options(V_CRYPTO_MDEBUG_ALL);
        CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
        ERR_load_crypto_strings();

        RAND_seed(rnd_seed, sizeof(rnd_seed)); /* or OAEP may fail */
    }

    /* create my own public and private key (or prepare for reading one, same command) */
    ep->myRSA.rsa = RSA_generate_key(1024, 3, NULL, NULL);
    /* check if we have a private certificate */
    epUtilsEvent(ep, "getPrivateCertificate", NULL, CERTIFICATE_FILE);
    if (ep->cfg.privateCrt != NULL)
    {
        if (ep->cfg.privateCrtSz > 500)
        {
            d2i_RSAPrivateKey((RSA**)&ep->myRSA.rsa, (const unsigned char **)&ep->cfg.privateCrt, ep->cfg.privateCrtSz);
        }
        if ((ep->cfg.privateCrtSz < 500) || (ep->myRSA.rsa == NULL))
        {
            /* certificate too small (public?) or corrupt */
            epUtilsLog(ep, NULL, "Security Init: invalid %s", CERTIFICATE_FILE);
        }
    }
    ptr = ep->publicKey;
    ep->publicKeySize = (RvSize_t)i2d_RSA_PUBKEY((RSA*)ep->myRSA.rsa, (RvUint8**)&ptr);
    ep->gkRSA.rsa = NULL;
    ep->tmpRSA.rsa = NULL;
    h235SetCertificateHandles(ep->h235App, (H235HKEY)&ep->gkRSA, (H235HKEY)&ep->myRSA);
#else
    ptr = NULL;
    h235SetCertificateHandles(ep->h235App, (H235HKEY)0, (H235HKEY)1);
#endif /* USE_SECURITY_ALGORITHMS */

    /*generateCertificateFiles();*/
    return RV_OK;
}


/******************************************************************************
 * epSecurityEnd
 * ----------------------------------------------------------------------------
 * General: Destruct the security module of an endpoint object.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epSecurityEnd(
    IN EpObj        *ep)
{
    if (ep->h235App == NULL)
        return RV_OK;

    h235End(ep->h235App);

#if defined(USE_SECURITY_ALGORITHMS)
    ERR_free_strings();

    if (ep->myRSA.rsa != NULL)
        RSA_free((RSA*)ep->myRSA.rsa);
    if (ep->gkRSA.rsa != NULL)
        RSA_free((RSA*)ep->gkRSA.rsa);
    if (ep->tmpRSA.rsa != NULL)
        RSA_free((RSA*)ep->tmpRSA.rsa);
#endif /* USE_SECURITY_ALGORITHMS */

    return RV_OK;
}


/******************************************************************************
 * epSecurityVersion
 * ----------------------------------------------------------------------------
 * General: Get the version string of the H.235 module used.
 *
 * Return Value: Version string.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
const RvChar *epSecurityVersion(
    IN EpObj    *ep)
{

    if (ep->h235App != NULL)
    {
#ifdef USE_SECURITY_ALGORITHMS
        static RvChar h235versionStr[32];
        sprintf(h235versionStr, "%s, With OpenSSL", h235GetVersion());
        return h235versionStr;
#else
        return h235GetVersion();
#endif
    }
    else
        return "0";
}


/******************************************************************************
 * epSecurityUpdateSettings
 * ----------------------------------------------------------------------------
 * General: This function should be called whenever the default security
 *          settings are modified.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epSecurityUpdateSettings(
    IN EpObj    *ep)
{
    if (ep->h235App == NULL)
        return RV_OK; /* Nothing to do here */

    /* change GK related parameters only if we are not already registered */
    if (!ep->bIsRegistered)
    {
        /* get key and hash it as required by the standard */
        memset(ep->key, 0, sizeof(ep->key));
#ifdef USE_SECURITY_ALGORITHMS
        SHA1((unsigned char *)ep->cfg.password, strlen(ep->cfg.password), (unsigned char*)ep->key);
#else /* USE_SECURITY_ALGORITHMS */
        strncpy((char*)ep->key, (const char *)ep->cfg.password, min(sizeof(ep->key),strlen(ep->cfg.password)));
#endif /* USE_SECURITY_ALGORITHMS */

        h235SetGatekeeperId(ep->h235App, &ep->cfg.generalId, (H235HKEY)ep);
        h235SetSenderId(ep->h235App, &ep->cfg.senderId);
    }

    /* set the security mode */
    h235SetSecureMode(ep->h235App, ep->cfg.securityMode);
    h235SetCheckIncomingMessages(ep->h235App, ep->cfg.bCheckIncoming);

    if (ep->cfg.bUserPrimes)
    {
        /* Use our application specific prime numbers generator */
        h235SetEncodingCallback(ep->h235App, pickAPrime, RV_H235_GENERATE_PRIME_OID);
    }
    else
    {
        /* Use the H.235 built-in prime numbers generator */
        h235SetEncodingCallback(ep->h235App, NULL, RV_H235_GENERATE_PRIME_OID);
    }

    return RV_OK;
}


/******************************************************************************
 * epSecurityGetSenderId
 * ----------------------------------------------------------------------------
 * General: This Function gets the sender id set by the GK.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 * Output: None.
 *****************************************************************************/
void epSecurityGetSenderId(
    IN EpObj    *ep)
{
    RvChar idStr[513];
    entityId_t entId;

    if (ep->h235App == NULL)
        return;

    h235GetSenderId(ep->h235App, &entId);
    utlBmp2Chr(idStr, entId.data, entId.length);

    epUtilsLog(ep, NULL, "SEC: Gatekeeper assigned EPID = '%s'", idStr);
    epUtilsEvent(ep, "SEC", NULL, "epSecurityGetSenderId");
}


/******************************************************************************
 * epSecurityCallInit
 * ----------------------------------------------------------------------------
 * General: Initialize security services for a call. Namely the Diffie-Hellman
 *          procedures.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epSecurityCallInit(
    IN EpCallObj    *call)
{
    EpObj *ep = call->ep;

    if (ep->h235App == NULL)
        return RV_OK;

    if ((RvUint32)ep->cfg.securityMode & ((RvUint32)RV_H235_ALL_DH_MODES | (RvUint32)h235ModeProcedure3))
    {
        h235Mode_t secureMode = ep->cfg.securityMode;
        RvInt i;

        /* we are using DH, init the H235 call struct */
        call->useDiffieHellman = RV_TRUE;
        call->hSecCall = (H235HCALL)ep->cb.epMalloc(ep, (RvSize_t)h235GetSizeofCallInfo(), __FILE__, __LINE__);
        h235InitCallObject(ep->h235App, call->hSecCall);

        /* Set the remote endpoint id */
        call->remoteEndpoint = ep->cfg.remoteId;

        /* choose ONE Diffie-Hellman mode */
        if ((secureMode & h235ModeDHgroup1536) != 0) secureMode = h235ModeDHgroup1536;
        else if ((secureMode & h235ModeDHgroup1024) != 0) secureMode = h235ModeDHgroup1024;

        /* We're calculating the DH half-key before connecting the TCP connection, otherwise, the
        other side will close the connection when the setup is delayed. */
        if ((RvUint32)secureMode & (RvUint32)RV_H235_ALL_DH_MODES)
        {
            h235GenerateDiffieHellman(ep->h235App, RV_H235_MODE_TO_DH_MODE(secureMode), call->hSecCall);
        }

        /* initialize the security vector randomly. while the standard dictates a different vector for each
           session, for testing it will be enough to use a single vector for all the channels in a call - the
           vector belonging master of the call is used, of course. The vector should be padded to a multiple
           of the block size, but we shall use 8 BYTEs. */
        for (i = 0; i < 7; i++)
        {
            call->secVector[i] = (char)rand();
        }
    }
    else
    {
        call->hSecCall = NULL;
    }

    return RV_OK;
}


/******************************************************************************
 * epSecurityCallEnd
 * ----------------------------------------------------------------------------
 * General: Finalize security services for a call. Namely the Diffie-Hellman
 *          procedures.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call object to use.
 * Output: None.
 *****************************************************************************/
RvStatus epSecurityCallEnd(
    IN EpCallObj    *call)
{
    EpObj *ep;
    if ((call == NULL) || (call->hSecCall == NULL))
        return RV_OK;

    ep = call->ep;

    /* end the H235 call struct */
    if (call->hSecCall != NULL)
    {
        h235EndCallObject(ep->h235App, call->hSecCall);
        ep->cb.epFree(ep, (void *)call->hSecCall);
    }

    call->hSecCall = NULL;
    return RV_OK;
}


/******************************************************************************
 * epSecurityCallGenerateSharedSecret
 * ----------------------------------------------------------------------------
 * General: Generate a DH shared secret. This function should be called after
 *          the entities exchanged DH half-keys.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call object to use.
 * Output: None.
 *****************************************************************************/
void epSecurityCallGenerateSharedSecret(
    IN EpCallObj    *call)
{
    h235Mode_t secMode;
    int secretLen;

    if (call->hSecCall == NULL)
        return;

    /* check if we want to use Diffie-Hellman */
    if (!call->useDiffieHellman)
        return;

    secMode = h235GetCallMode(call->hSecCall);
    if ((RvUint32)secMode & (RvUint32)h235ModeDHgroup512)
        secretLen = 7;
    else if ((RvUint32)secMode & ((RvUint32)h235ModeDHgroup1024 | (RvUint32)h235ModeDHgroup1536))
        secretLen = 21;
    else
        return;

    epUtilsLog(call->ep, call, "SEC: Calculating Diffie-Hellman Shared Secret, please wait...");
    epUtilsEvent(call->ep, "Catch", NULL, "update");

    /* init the shared secret and get it */
    memset(call->sharedSecret, 0, sizeof(call->sharedSecret));

    if (h235GenarateDiffieHellmanSharedSecret(call->hSecCall, RV_TRUE, call->sharedSecret) >= 0)
    {
        /* print out the shared secret */
        RvChar printBuf[64];
        RvInt i;

        for(i = 0; i < secretLen; i++)
        {
            sprintf(printBuf+2*i, "%02X", (unsigned char)call->sharedSecret[i]);
        }
        printBuf[2*i] = 0;
        epUtilsLog(call->ep, call, "SEC: Diffie-Hellman Shared Secret is:");
        epUtilsLog(call->ep, call, "SEC: %s", printBuf);
        epUtilsLog(call->ep, call, "SEC: ...but don't tell anyone! ;-)");
        epUtilsEvent(call->ep, "SEC", call, "h235GenarateDiffieHellmanSharedSecret.1");
        epUtilsEvent(call->ep, "Rec", call, "verifyDiffieHellman entry= %d mode= %d", call->id, RV_H235_MODE_TO_DH_MODE(secMode));
    }
    else
    {
        /* error */
        epUtilsLog(call->ep, call, "SEC: Problem calculating Diffie-Hellman Shared Secret");
        epUtilsEvent(call->ep, "SEC", NULL, "h235GenarateDiffieHellmanSharedSecret.0");
    }
}


/******************************************************************************
 * epSecurityChannelAddEncryptionSync
 * ----------------------------------------------------------------------------
 * General: Modify an outgoing OLC ack message to a secure channel.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call object to use.
 *         nodeId   - OLC acknowledge message to manipulate.
 * Output: None.
 *****************************************************************************/
void epSecurityChannelAddEncryptionSync(
    IN EpCallObj    *call,
    IN RvPvtNodeId  nodeId)
{
    EpObj *ep;
    RvStatus status;
    h235Mode_t secMode;
    RvPvtNodeId encNodeId;
    RvPvtNodeId tmpNodeId;
    entityId_t gkId;
    H235HKEY keyHandle;
    RvUint8 encoded[256];
    int encLen;

    if (call->hSecCall == NULL)
        return;

    /* See if this call needs media encryption at all */
    if (!call->secureMedia)
        return;

    ep = call->ep;
    secMode = h235GetCallMode(call->hSecCall);

    /* create the key with the general ID and the encryption vector */
    encNodeId = pvtAddRoot(ep->hVal, cmGetSynTreeByRootName(ep->hApp, "KeySyncMaterial"), 0, NULL);

    /* add the security vector - 56 bits of a DES key*/
    pvtAdd(ep->hVal, encNodeId, __q931(keyMaterial), 8*7, call->secVector, NULL);

    /* add minimal authentication with general ID */
    h235GetGatekeeperId(ep->h235App, &gkId, &keyHandle);
    pvtAdd(ep->hVal, encNodeId, __q931(generalID), gkId.length, (RvChar*)gkId.data, NULL);

    /* encode to buffer */
    status = cmEmEncode(ep->hVal, encNodeId, encoded, (int)sizeof(encoded), &encLen);

    /* no need for the node anymore */
    pvtDelete(ep->hVal, encNodeId);
    if (status < 0)
    {
        /* could not encode or build */
        return;
    }

    /* encrypt the encoded buffer */
    encryptVector(RV_H235_MODE_TO_DH_MODE(secMode), encoded, encLen, call->sharedSecret);

    /* create the key containing the encrypted vector */
    encNodeId = pvtAddRoot(ep->hVal, cmGetSynTreeByRootName(ep->hApp, "H235Key"), 0, NULL);
    tmpNodeId = pvtAdd(ep->hVal, encNodeId, __q931(sharedSecret), 0, NULL, NULL);
    pvtAdd(ep->hVal, tmpNodeId, __q931(encryptedData), encLen, (char*)encoded, NULL);
    pvtAdd(ep->hVal, tmpNodeId, __q931(algorithmOID), 0, NULL, NULL);
    tmpNodeId = pvtAdd(ep->hVal, tmpNodeId, __q931(paramS), 0, NULL, NULL);

    /* encode to buffer */
    status = cmEmEncode(ep->hVal, encNodeId, encoded, (int)sizeof(encoded), &encLen);

    /* no need for the node anymore */
    pvtDelete(ep->hVal, encNodeId);
    if (status < 0)
    {
        /* could not encode or build */
        return;
    }

    /* create the message branch */
    __pvtBuildByFieldIds(tmpNodeId, ep->hVal, nodeId,
        {_anyField _anyField _h245(encryptionSync) LAST_TOKEN}, 0, NULL);

    /* add the synch flag */
    pvtAdd(ep->hVal, tmpNodeId, __h245(synchFlag), 0, NULL, NULL);

    /* add the encrypted buffer to the h235 key field */
    pvtAdd(ep->hVal, tmpNodeId, __h245(h235Key), encLen, (char*)encoded, NULL);
}


/******************************************************************************
 * epSecurityChannelGetEncryptionSync
 * ----------------------------------------------------------------------------
 * General: Open an incoming secure channel request.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call     - Call object to use.
 *         nodeId   - OLC acknowledge message to manipulate.
 * Output: None.
 *****************************************************************************/
void epSecurityChannelGetEncryptionSync(
    IN EpCallObj    *call,
    IN RvPvtNodeId  nodeId)
{
    EpObj *ep;
    RvStatus status;
    h235Mode_t secMode;
    RvPvtNodeId encNodeId;
    RvPvtNodeId tmpNodeId;
    RvUint8 encoded[256];
    int encLen;
    entityId_t gkId;
    H235HKEY keyHandle;

    if (call->hSecCall == NULL)
        return;

    /* See if this call needs media encryption at all */
    if (!call->secureMedia)
        return;

    ep = call->ep;

    secMode = h235GetCallMode(call->hSecCall);

    /* get the key node, and extract the synch flag */
    __pvtGetNodeIdByFieldIds(tmpNodeId, ep->hVal, nodeId, {_anyField _anyField _h245(encryptionSync) LAST_TOKEN});
    if (!RV_PVT_NODEID_IS_VALID(tmpNodeId))
    {
        /* no key node */
        return;
    }

    encNodeId = pvtAddRoot(ep->hVal, cmGetSynTreeByRootName(ep->hApp, "KeySyncMaterial"), 0, NULL);

    /* get the key, to decode the encrypted vector from */
    encLen = pvtGetString(ep->hVal, pvtGetChild(ep->hVal, tmpNodeId, __h245(h235Key), NULL),
        (RvInt32)sizeof(encoded), (RvChar*)encoded);

    encNodeId = pvtAddRoot(ep->hVal, cmGetSynTreeByRootName(ep->hApp, "H235Key"), 0, NULL);
    status = cmEmDecode(ep->hVal, encNodeId, encoded, encLen, &encLen);

    /* get the key, decrypt and decode it */
    encLen = pvtGetString(ep->hVal, pvtGetChild(ep->hVal, encNodeId, __h245(sharedSecret), NULL),
        (RvInt32)sizeof(encoded), (RvChar*)encoded);

    decryptVector(RV_H235_MODE_TO_DH_MODE(secMode), encoded, encLen, call->sharedSecret);
    status = cmEmDecode(ep->hVal, encNodeId, encoded, encLen, &encLen);
    if (status < 0)
    {
        /* was not able to decode */
        pvtDelete(ep->hVal, encNodeId);
        return;
    }

    /* check minimal authentication - that the general ID is the same */
    encLen = pvtGetString(ep->hVal, pvtGetChild(ep->hVal, encNodeId, __q931(generalID), NULL),
        sizeof(encoded), (RvChar*)encoded);
    h235GetGatekeeperId(ep->h235App, &gkId, &keyHandle);
    if ((encLen != gkId.length) || (memcmp(encoded, gkId.data, (RvSize_t)encLen) != 0))
    {
        /* general ID is wrong. error. */
        pvtDelete(ep->hVal, encNodeId);
    }

    /* now get the vector */
    pvtGetBitString(ep->hVal, pvtGetChild(ep->hVal, encNodeId, __q931(keyMaterial), NULL),
        (RvInt32)sizeof(call->secVector), call->secVector);
    pvtDelete(ep->hVal, encNodeId);
}





/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


static int RVCALLCONV pickAPrime(
    IN     H235HKEY         keyHandle,
    IN OUT void             *buf,
    IN     int              bufLen,
    OUT    unsigned char    *digest,
    OUT    int              *len)
{
    /* This function is an example of how to set the callback for prime numbers selection.
       What we actually do here is preset 10 prime numbers in advance so we won't have
       to find a prime number during runtime and then we just choose randomly one of these
       primes when it's needed.
       You can replace this implementation with others that might be faster than the
       internal prime numbers generator of the H.235 add-on module. */
    /* Note that this sample implementation makes the assumption that bufLen==64. */
    static unsigned char userPrime[10][64] =
    {
        {
            0xD0,0xF5,0x3A,0xBB,0x3A,0x69,0xFC,0xC0,0xEE,0x1A,0x0F,0xE3,0xFB,0xB6,0x32,0x62,
            0x20,0xD1,0xA9,0x87,0x1C,0x46,0xB6,0x67,0x70,0x0D,0xAE,0x14,0xFC,0x10,0x22,0x10,
            0x3A,0x21,0x6A,0x2E,0xA3,0x10,0x84,0xFA,0xCB,0x67,0xF7,0x4E,0x8A,0xA9,0xBA,0xFE,
            0xA3,0x66,0xD8,0x9B,0xE1,0xD6,0x13,0xCF,0x3A,0xAC,0x9B,0xDA,0x6E,0x9E,0x18,0x7F
        },
        {
            0xCC,0x40,0x0A,0x4D,0x50,0xC5,0xBF,0x6C,0x84,0x2B,0xDE,0x74,0x73,0x0A,0x13,0x58,
            0xBA,0xD6,0x3A,0x0B,0x63,0x3E,0x93,0x39,0x89,0xC3,0xE6,0x82,0xC2,0x5F,0x91,0xC0,
            0x7D,0x92,0x7E,0xA4,0xF5,0x5D,0x2B,0x63,0xB5,0x3C,0x4A,0xCF,0x02,0xBA,0x8E,0xE3,
            0x7E,0x46,0x0C,0x25,0xFB,0x4B,0x4E,0xE7,0x89,0xD4,0xB7,0x1E,0xEB,0x7B,0xC2,0xD3
        },
        {
            0xB0,0xCA,0x99,0x80,0xF3,0x51,0xD7,0x1D,0x7E,0xEB,0x0E,0x47,0x3A,0x41,0xAA,0x15,
            0x4A,0xA5,0xDD,0xC4,0xA6,0xEA,0x19,0xDC,0x02,0xD2,0x32,0xF9,0x37,0x03,0x80,0xF3,
            0x87,0x77,0xCA,0xBE,0xF5,0x2F,0xB2,0x5D,0xC8,0x52,0x8F,0xC7,0xB6,0x78,0x24,0x86,
            0x4E,0xE6,0xA7,0x54,0x21,0x6B,0xE3,0xCD,0x21,0xF6,0xE2,0x76,0xE0,0x83,0x05,0x3D
        },
        {
            0x87,0x95,0x70,0x0A,0xEC,0x35,0x44,0x1A,0xBE,0x7A,0x70,0xF7,0x55,0xDE,0x41,0x36,
            0xFB,0xAB,0x70,0xD5,0x23,0xAA,0xCA,0x18,0x5B,0x7A,0x32,0x32,0xAC,0x16,0x33,0x1C,
            0xE0,0xF3,0x36,0x35,0x7A,0xB3,0x77,0x10,0xE5,0x6A,0xDC,0xE6,0x99,0xC9,0xF2,0xE2,
            0x1D,0xC4,0x0B,0xEF,0x9F,0x88,0xD7,0x4D,0x7D,0x11,0xC3,0xF0,0xE5,0x50,0xAB,0xDF
        },
        {
            0xCE,0x2D,0xA9,0xDF,0xB5,0x16,0x49,0x54,0x1B,0xF3,0x80,0xBC,0xB2,0xB4,0x04,0xBA,
            0xD0,0x5A,0xFD,0x1F,0xAC,0x21,0x7B,0xA7,0xF0,0x30,0x08,0xE2,0x34,0xFD,0xA2,0x72,
            0x31,0xEE,0x74,0x1F,0x9D,0x34,0x18,0xEE,0x6D,0x05,0x94,0x9E,0xF4,0xB9,0x9C,0x63,
            0xD9,0xAF,0x0C,0x83,0x03,0xF3,0x75,0xF2,0x0B,0x18,0x80,0xBF,0xFF,0x12,0x1F,0xA7
        },
        {
            0xB9,0x23,0x38,0x71,0xAF,0x8B,0xB1,0x93,0x15,0x40,0xC0,0xDB,0xC0,0x85,0xAD,0x7C,
            0xCF,0x57,0x29,0x29,0xFF,0x32,0x3F,0xA3,0xB0,0xC3,0x63,0xB4,0xEB,0xEC,0xF6,0x6D,
            0x18,0x45,0xC8,0xA0,0xB0,0xE8,0xC2,0x91,0x87,0x3E,0x94,0x00,0xFD,0x2F,0xAB,0x10,
            0xBD,0xA0,0x17,0x72,0x08,0x63,0x33,0x3E,0x8E,0x22,0x79,0x85,0x50,0x62,0x14,0xD9
        },
        {
            0x8B,0xC3,0x19,0xE2,0x99,0x43,0xCF,0x58,0x25,0x2F,0x53,0xF6,0x00,0x32,0xCD,0x5E,
            0x30,0x7F,0xAE,0xCA,0x7E,0xAD,0xF0,0xFB,0xE1,0xFA,0x97,0x2A,0xEA,0x3B,0x1C,0x1D,
            0x69,0x98,0x1B,0xD1,0x7B,0x83,0xD5,0xBC,0x8E,0xD3,0x46,0x10,0x00,0x99,0xBE,0x53,
            0x3E,0x66,0xC8,0x34,0xA0,0x4E,0x97,0xD1,0xEF,0x38,0xAD,0xBD,0xE5,0xC1,0x5D,0xBD
        },
        {
            0xC6,0x70,0x6B,0x6B,0xA4,0x40,0x6C,0xBA,0xD2,0x78,0x93,0xE2,0x0E,0x7C,0x24,0x6C,
            0xE4,0x5B,0x1A,0x01,0xDB,0x5D,0x78,0x80,0x4B,0xBB,0x62,0xDC,0xDD,0xDE,0x9B,0xEA,
            0x36,0x35,0x29,0xB0,0x22,0x18,0x87,0x79,0xB5,0xAA,0x3F,0x40,0x2B,0xFD,0x6E,0x0B,
            0x85,0xA1,0x10,0x9E,0xA0,0xD0,0x0B,0x54,0x20,0x06,0x63,0xE9,0x0D,0x7D,0xE7,0x75
        },
        {
            0xBD,0x72,0x6F,0xB6,0x28,0x3F,0x3F,0xA2,0x6B,0x30,0xDC,0x37,0x74,0xBF,0xD4,0xC1,
            0xD7,0x2C,0xFF,0x21,0x72,0x7D,0x07,0x6D,0x69,0x34,0x78,0xB6,0x22,0xA6,0xF4,0x7D,
            0x7B,0x1A,0x9A,0xB3,0xF1,0xC5,0xDD,0xA7,0x0F,0xB5,0xB4,0x64,0xE8,0xDE,0x8F,0x4D,
            0xFC,0x1E,0x40,0xFE,0x2D,0xE5,0xCE,0x08,0x61,0x1E,0x7C,0x56,0xEF,0x9B,0xAC,0x29
        },
        {
            0xA0,0x42,0x40,0x5F,0xBE,0x06,0x32,0x1D,0x25,0xFE,0x9F,0xF3,0x71,0x8B,0x85,0x6C,
            0x34,0x72,0x7C,0x49,0x10,0x3E,0x39,0xD7,0xB9,0xE9,0x75,0xC2,0x07,0x32,0xE5,0x63,
            0x7B,0x2E,0xE6,0xCB,0x78,0x60,0x61,0x51,0x22,0x9D,0x1D,0x63,0x9B,0x59,0xB3,0x0E,
            0x94,0xD0,0x41,0x5B,0x3B,0x7E,0x85,0x0E,0x36,0x56,0xBF,0xEB,0x7C,0xB4,0x10,0x6B
        },
    };

    /* Choose one of the prime numbers randomly */
    int i = rand()%10;

    /* Copy it to the selected place */
    memcpy(digest, userPrime[i], sizeof(userPrime[i]));

    RV_UNUSED_ARG(keyHandle);
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(bufLen);
    RV_UNUSED_ARG(len);

    return 0;
}


/******************************************************************************
 * encryptVector
 * ----------------------------------------------------------------------------
 * General: Encrypt a buffer. Used for secured logical channels.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  dhMode       - DH mode used.
 *         vector       - Vector to encrypt.
 *         vecLen       - Length of vector.
 *         sharedSecret - Key to use.
 * Output: None.
 *****************************************************************************/
static void encryptVector(
    IN h235DHMode_t     dhMode,
    IN unsigned char    *vector,
    IN int              vecLen,
    IN char             *sharedSecret)
{
#ifdef USE_SECURITY_ALGORITHMS
    unsigned char key[8];
    DES_key_schedule ks[3];

    memset(key, 0, sizeof(key));

    /* check which Diffie-Hellman we are using */
    switch (dhMode)
    {
        case h235ModeDiffieHellman512:
            /* just DES */
            memcpy(key, sharedSecret, 7);
            DES_set_key_unchecked((const_DES_cblock*)key, &ks[0]);
            DES_ncbc_encrypt(vector, vector, vecLen, &ks[0], (DES_cblock*)key, RV_TRUE);
            break;

        case h235ModeDiffieHellman1024:
        case h235ModeDiffieHellman1536:
            /* use TripleDES, with three keys */
            memcpy(key, sharedSecret,    7);
            DES_set_key_unchecked((const_DES_cblock*)key, &ks[0]);
            memcpy(key, sharedSecret+7,  7);
            DES_set_key_unchecked((const_DES_cblock*)key, &ks[1]);
            memcpy(key, sharedSecret+14, 7);
            DES_set_key_unchecked((const_DES_cblock*)key, &ks[2]);
            DES_ede3_cbc_encrypt(vector, vector, vecLen, &ks[0], &ks[1], &ks[3], NULL, RV_TRUE);
            break;

        case h235ModeDiffieHellmanNone:
            break;
    }

#else /* USE_SECURITY_ALGORITHMS */
    RV_UNUSED_ARG(dhMode);
    RV_UNUSED_ARG(vector);
    RV_UNUSED_ARG(vecLen);
    RV_UNUSED_ARG(sharedSecret);
#endif /* USE_SECURITY_ALGORITHMS */
}


/******************************************************************************
 * decryptVector
 * ----------------------------------------------------------------------------
 * General: Decrypt a buffer. Used for secured logical channels.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  dhMode       - DH mode used.
 *         vector       - Vector to decrypt.
 *         vecLen       - Length of vector.
 *         sharedSecret - Key to use.
 * Output: None.
 *****************************************************************************/
static void decryptVector(
    IN h235DHMode_t     dhMode,
    IN unsigned char    *vector,
    IN int              vecLen,
    IN char             *sharedSecret)
{
#ifdef USE_SECURITY_ALGORITHMS
    unsigned char key[8];
    DES_key_schedule ks[3];

    memset(key, 0, sizeof(key));

    /* check which Diffie-Hellman we are using */
    switch (dhMode)
    {
        case h235ModeDiffieHellman512:
            /* just DES */
            memcpy(key, sharedSecret, 7);
            DES_set_key_unchecked((const_DES_cblock*)key, &ks[0]);
            DES_ncbc_encrypt(vector, vector, vecLen, &ks[0], NULL, RV_FALSE);
            break;

        case h235ModeDiffieHellman1024:
        case h235ModeDiffieHellman1536:
            /* use TripleDES, with three keys */
            memcpy(key, sharedSecret,    7);
            DES_set_key_unchecked((const_DES_cblock*)key, &ks[0]);
            memcpy(key, sharedSecret+7,  7);
            DES_set_key_unchecked((const_DES_cblock*)key, &ks[1]);
            memcpy(key, sharedSecret+14, 7);
            DES_set_key_unchecked((const_DES_cblock*)key, &ks[2]);
            DES_ede3_cbc_encrypt(vector, vector, vecLen, &ks[0], &ks[1], &ks[3], NULL, RV_FALSE);
            break;

        case h235ModeDiffieHellmanNone:
            break;
    }

#else /* USE_SECURITY_ALGORITHMS */
    RV_UNUSED_ARG(dhMode);
    RV_UNUSED_ARG(vector);
    RV_UNUSED_ARG(vecLen);
    RV_UNUSED_ARG(sharedSecret);
#endif /* USE_SECURITY_ALGORITHMS */
}


#ifdef USE_SECURITY_ALGORITHMS

void generateCertificateFiles()
{
    RSA * rsa = NULL;
    int i, j, length;
    RvUint8 buffer[1024];
    RvUint8 * pBuff;
    char filename[32];
    FILE * crtFile;

    for (i=0; i<10; i++)
    {
        rsa = RSA_generate_key(1024, 3, NULL, NULL);
        pBuff = buffer;
        length = i2d_RSA_PUBKEY(rsa, &pBuff);
        sprintf(filename, "1%d_public.crt", i);
        crtFile = fopen(filename, "wb");
        for (j=0; j<length; j++)
            fputc((int)buffer[j], crtFile);
        fclose(crtFile);
        pBuff = buffer;
        length = i2d_RSAPrivateKey(rsa, &pBuff);
        sprintf(filename, "1%d_private.crt", i);
        crtFile = fopen(filename, "wb");
        for (j=0; j<length; j++)
            fputc((int)buffer[j], crtFile);
        fclose(crtFile);



/*        crtFile = fopen(filename, "rb");
        for (j=0; j<length; j++)
        {
            RvUint8 c = (RvUint8)fgetc(crtFile);
            if (buffer[j] != c)
                return;
        }
        fclose(crtFile);
        pBuff = buffer;
        {
            RSA * rsa2 = RSA_generate_key(1024, 3, NULL, NULL);
            d2i_RSAPrivateKey((RSA**)&rsa2, &pBuff, length);
        }*/
    }
}

#endif /* USE_SECURITY_ALGORITHMS */


#endif /* USE_SECURITY */


#ifdef __cplusplus
}
#endif
