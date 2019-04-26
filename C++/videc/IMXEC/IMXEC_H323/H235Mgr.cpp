
#include <RVINC/h245.h>
#include <RVINC/q931asn1.h>
#include <RVINC/stkutils.h> 
#include "H235Mgr.h"
#include "H323CallMgr.h"
#include "stkHandler.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifndef min
#define min(a,b) ((a)>(b) ? (b) : (a))
#endif

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


#define CERTIFICATE_FILE "epPrvt.crt"

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


void* H235MemoryMalloc(RvSize_t   size,const RvChar  *filename,RvInt32  lineno)
{
	void* p=NULL;
	RV_UNUSED_ARG(filename);
	RV_UNUSED_ARG(lineno);

	p = malloc(size + sizeof(void*));
	if (p != NULL)
	{
		return ((void *)((char *)p + sizeof(void*)));
	}

	return NULL;
}

RvStatus H235MemoryFree(void  *ptr)
{
	int* myAlloc;
	int size;

	myAlloc = (int *)((char *)ptr - sizeof(void*));
	size = *myAlloc;
	free(myAlloc);

	return RV_OK;
}

//首先作generalID senderId 时间戳验证，如验证错误则作验证通过处理，不再验证密码，呼叫可以继续下去
//如果双方generalID senderId相同，但密码及消息验证不对则呼叫不再继续下去
//注，原是设置为此,如不让其验证通过可在h235Failure设置reason为h235FailReplyDeleteMessage

static h235AppFailReply_t RVCALLCONV  h235Failure(
	IN  HAPP        hApp,
	IN  H235HAPP    h235HApp,
	IN  H235HMSG    h235HMsg,
	IN  int         messageRoot,
	IN  entityId_t  *senderId,
	IN h235Reason_t reason)
{
	h235AppFailReply_t reply;
	cmProtocol prot;
	H235Mgr *ep=NULL;

	RV_UNUSED_ARG(h235HApp);
	RV_UNUSED_ARG(h235HMsg);
	RV_UNUSED_ARG(senderId);

	ep=&(H323CallMgr::Instance().m_h235Mgr);

	HPVT hVal;
	hVal = cmGetValTree(hApp);


	switch (reason)
	{
	case h235ReasonNoSecurityInfo:      reply = h235FailReplyForceOk;           break;
	case h235ReasonMessageError:        reply = h235FailReplyDeleteMessage;     break;
	case h235ReasonGeneralIDIncorrect:  reply = h235FailReplyDeleteMessage/*h235FailReplyForceOk*/;           break;
	case h235ReasonUnknownSender:       reply = h235FailReplyDeleteMessage/*h235FailReplyContinue*/;          break;
	case h235ReasonProcNotSupported:    reply = h235FailReplyContinue;          break;
	case h235ReasonTimeStampMissing:    reply = h235FailReplyContinue;          break;
	case h235ReasonTimeStampFail:       reply = h235FailReplyContinue;          break;
	case h235ReasonTimeRandomFail:      reply = h235FailReplyContinue;          break;
	case h235ReasonIncorrectAuth:       reply = h235FailReplyDeleteMessage;     break;
	case h235ReasonGeneralError:        reply = h235FailReplyDeleteMessage;     break;
	case h235ReasonEndToEndTokenSenderUnknown: reply = h235FailReplyForceOk;    break;
	default: reply = h235FailReplyContinue;
	}

	prot = cmProtocolGetProtocol(hApp, messageRoot);
	if (prot == cmProtocolRAS)
	{
		RvPstFieldId fieldId = -1;
		pvtGet(hVal, pvtChild(hVal, messageRoot), &fieldId, NULL, NULL, NULL);
		if ((fieldId == __q931(registrationReject)) ||
			(fieldId == __q931(admissionReject)) ||
			(fieldId == __q931(disengageReject)))
		{
			reply = h235FailReplyForceOk;
		}
	}

	RvChar *replyStr;
	switch (reply)
	{
	case h235FailReplyContinue:         replyStr = "continue"; break;
	case h235FailReplyDeleteMessage:    replyStr = "delete incoming message"; break;
	case h235FailReplyForceOk:          replyStr = "ignore security failure"; break;
	default:                            replyStr = "bug"; /* this option is impossible */
	}
	if(ep)
	{
		ep->m_nAuthCheckState=2;
		ep->m_nAuthFailureReason=reason;
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
	h235Mode_t secMode;
	RvChar msgText[128];
	H235Mgr *ep=NULL;

	RV_UNUSED_ARG(h235HApp);
	RV_UNUSED_ARG(senderId);

	ep=&(H323CallMgr::Instance().m_h235Mgr);;

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

	if(ep)
		ep->m_nAuthCheckState=1;

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
	H235Mgr *ep;
	ep=&(H323CallMgr::Instance().m_h235Mgr);;

	typedef struct
	{
		H235Mgr           *ep;
		HPROTCONN       protConn;
		cmRASTransport  srcAddress;
		H323Call       *call;
		int             msgLen;
		RvUint8         *message;
	} frozenMessage;

	RvUint8 info[1024]={0};

	frozenMessage msg;
	frozenMessage * pMsg=&msg;
	char senderDn[256]={0};
	RV_UNUSED_ARG(h235HApp);
	RV_UNUSED_ARG(h235HMsg);
	RV_UNUSED_ARG(nodeId);
	pMsg->message=info;
	memset(pMsg, 0, sizeof(frozenMessage));
	pMsg->protConn = protConn;
	pMsg->msgLen = bufLen;
	if (srcAddress)
		pMsg->srcAddress = *srcAddress;

	pMsg->message = info;
	if(bufLen<1024)
		memcpy(pMsg->message, buffer, bufLen);

	pMsg->ep = ep;
	utlBmp2Chr(senderDn, senderId->data, senderId->length);
	*deleteMsg = RV_TRUE;
	return RV_OK;
}


static void RVCALLCONV h235EvSetKeyHandle(
	IN  HAPP        hApp,
	IN  H235HAPP    h235HApp,
	IN  RvUint8     newKey[20],
	INOUT H235HKEY* keyHandle)
{
	H235Mgr * ep=(H235Mgr *)(*keyHandle);

	RvChar prnBuf[41];
	RvInt i;

	RV_UNUSED_ARG(hApp);
	RV_UNUSED_ARG(h235HApp);

	memcpy(ep->m_strKey, (void*)newKey, 20);

	for(i = 0; i < 20; i++)
	{
		sprintf(prnBuf+2*i, "%02X", newKey[i]);
	}
	prnBuf[2*i] = 0;

}


static H235HCALL RVCALLCONV h235EvGetH235Call(
	IN  HCALL       hsCall,
	IN  HAPPCALL    haCall)
{
	H323Call *call = (H323MediaCall *)haCall;

	RV_UNUSED_ARG(hsCall);

	if (call == NULL)
		return NULL;

	return call->m_hSecCall;
}


static RvBool RVCALLCONV h235EvGetIDByCall(
	IN  HAPP        hApp,
	IN  H235HAPP    h235HApp,
	IN  HCALL       hsCall,
	IN  HAPPCALL    haCall,
	OUT entityId_t  *generalID)
{
	H323Call *call = (H323MediaCall *)haCall;

	RV_UNUSED_ARG(hApp);
	RV_UNUSED_ARG(h235HApp);
	RV_UNUSED_ARG(hsCall);

	if (call == NULL)
		return RV_FALSE;

	*generalID = call->m_remoteEndpoint;

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
	H323Call *call = (H323MediaCall *)haCall;

	RV_UNUSED_ARG(hApp);
	RV_UNUSED_ARG(h235HApp);
	RV_UNUSED_ARG(hsCall);

	if (call == NULL)
		return RV_FALSE;

	H235Mgr *ep;
	ep=&(H323CallMgr::Instance().m_h235Mgr);;

	*senderID = ep->m_senderId;
	*generalID = call->m_remoteEndpoint;

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

	H235Mgr *ep;
	ep=&(H323CallMgr::Instance().m_h235Mgr);;
	*crtInHandle = (H235HKEY)&ep->m_tmpRSA;
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

	H235Mgr *ep;
	ep=&(H323CallMgr::Instance().m_h235Mgr);;

	if (h235hCrt == (H235HKEY)&ep->m_myRSA)
	{
		/* outgoing message, H.235 is getting my public key */
		*certificate = (RvUint8*)ep->m_strPublicKey;
		*size = ep->m_nPublicKeySize;
		*type = h235CertificateSHA1;
	}
	if (h235hCrt == (H235HKEY)&ep->m_gkRSA)
	{
		/* incoming message. get the public key if we don't have it */
		if (ep->m_gkRSA.rsa == NULL)
		{
			d2i_RSA_PUBKEY((RSA**)&ep->m_gkRSA.rsa, (const unsigned char **)certificate, *size);
		}
	}
	if (h235hCrt == (H235HKEY)&ep->m_tmpRSA)
	{
		ep->m_tmpRSA.rsa = (void*)RSA_new();
		d2i_RSA_PUBKEY((RSA**)&ep->m_tmpRSA.rsa, (const unsigned char **)certificate, *size);
	}
	return RV_TRUE;
}

static RvBool RVCALLCONV h235EvGetIDByRAS(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  HRAS        hsRas,
                            IN  HAPPRAS     haRas,
                            OUT entityId_t  *generalID)
{
	H235Mgr *ep;
	ep=&(H323CallMgr::Instance().m_h235Mgr);;
	*generalID = ep->m_generalId;
	return RV_TRUE;
}

static void RVCALLCONV h235EvGetEntryByEndpointId(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  entityId_t *endpointID,
                            OUT RvBool     *isNewEntry,
                            OUT H235HENTITY*ptr)
{
	isNewEntry =RV_FALSE;
	ptr =NULL;
}

static void RVCALLCONV h235EvChooseEndpointSecureMode(
                            IN  HAPP        hApp,
                            IN  H235HAPP    h235HApp,
                            IN  entityId_t  *endpointID,
                            INOUT h235Mode_t *secureMode)
{
	secureMode =NULL;
	H235Mgr *ep;
	ep=&(H323CallMgr::Instance().m_h235Mgr);
	*secureMode = ep->m_nSecurityMode;
}

static RvBool RVCALLCONV h235EvGetKeyHandleById(
	IN  HAPP        hApp,
	IN  H235HAPP    h235HApp,
	IN  entityId_t* endpointID,
	OUT H235HKEY*   keyHandle)
{
	H235Mgr *ep;
	ep=&(H323CallMgr::Instance().m_h235Mgr);;

	return RV_FALSE;
}

static RvBool RVCALLCONV h235EvReceivedToken(
									  IN  HAPP        hApp,
									  IN  H235HAPP    h235HApp,
									  IN  HCALL       hCall,
									  IN  H235HCALL   hSecCall,
									  IN  entityId_t* endpointID,
									  IN  int         tokenType,
									  IN  RvPvtNodeId tokenId)
{
	return RV_FALSE;
}


static int RVCALLCONV hash_SH1_96Callback(H235HKEY keyHandle,void *buf,int bufLen,unsigned char *digest,int *len)
{
	H235Mgr *ep = (H235Mgr *)keyHandle;

	HMAC(EVP_sha1(), (void*)ep->m_strKey, 20, (unsigned char*)buf, bufLen, digest, (unsigned int*)len);

	*len = 12;  /* 96 bits length in bytes */
	return 0;
}


static int RVCALLCONV sign_SH1_Callback(H235HKEY keyHandle,void *buf,int bufLen,unsigned char *digest,int *len)
{
	char hashed[SHA_DIGEST_LENGTH];
	SecurityCertificate *cert = (SecurityCertificate *)keyHandle;
	H235Mgr *ep;

	ep = cert->ep;

	SHA1((unsigned char*)buf, (unsigned long)bufLen, (unsigned char*)hashed);

	if (cert == &ep->m_myRSA)
	{
		/* we are asked to sign */
		memset(digest, 0, 128);
		*len = 0;

		if (RSA_sign(NID_sha1, (unsigned char*)hashed, SHA_DIGEST_LENGTH, digest, (unsigned int*)len, (RSA*)ep->m_myRSA.rsa))
		{
			*len = 128;  /* 1024 bits length in bytes */
			return RV_OK;
		}
	}
	if (cert == &ep->m_gkRSA)
	{
		/* we are asked to verify the GK's signature */
		if (RSA_verify(NID_sha1, (unsigned char*)hashed, SHA_DIGEST_LENGTH, digest, (unsigned int)*len, (RSA*)ep->m_gkRSA.rsa))
		{
			return RV_OK;
		}
	}
	if (cert == &ep->m_tmpRSA)
	{
		/* we are asked to verify the GK's signature */
		if (RSA_verify(NID_sha1, (unsigned char*)hashed, SHA_DIGEST_LENGTH, digest, (unsigned int)*len, (RSA*)ep->m_tmpRSA.rsa))
		{
			return RV_OK;
		}
	}
	return RV_ERROR_UNKNOWN;

}

static int RVCALLCONV sign_MD5_Callback(H235HKEY keyHandle,void *buf,int bufLen,unsigned char *digest,int *len)
{
	char hashed[MD5_DIGEST_LENGTH];
	SecurityCertificate *cert = (SecurityCertificate *)keyHandle;
	H235Mgr *ep;
	ep = cert->ep;

	MD5((unsigned char*)buf, (unsigned long)bufLen, (unsigned char*)hashed); /* or should we still use SHA1 hashing here? */

	if (cert == &ep->m_myRSA)
	{
		/* we are asked to sign */
		memset(digest, 0, 128);
		*len = 0;
		if (RSA_sign(NID_md5, (unsigned char*)hashed, MD5_DIGEST_LENGTH, digest, (unsigned int*)len, (RSA*)ep->m_myRSA.rsa))
		{
			*len = 128;  /* 1024 bits length in bytes */
			return RV_OK;
		}
	}
	if (cert == &ep->m_gkRSA)
	{
		/* we are asked to verify the GK's signature */
		if (RSA_verify(NID_md5, (unsigned char*)hashed, MD5_DIGEST_LENGTH, digest, (unsigned int)*len, (RSA*)ep->m_gkRSA.rsa))
		{
			return RV_OK;
		}
	}
	if (cert == &ep->m_tmpRSA)
	{
		/* we are asked to verify the GK's signature */
		if (RSA_verify(NID_md5, (unsigned char*)hashed, MD5_DIGEST_LENGTH, digest, (unsigned int)*len, (RSA*)ep->m_tmpRSA.rsa))
		{
			return RV_OK;
		}
	}
	return RV_ERROR_UNKNOWN;

}

int H235Mgr::H235SecurityChannelAddEncryptionSync(H323Call *call,H323Channel * pChannel,RvPvtNodeId  nodeId)
{
	RvStatus status;
	RvPvtNodeId encNodeId;
	RvPvtNodeId tmpNodeId;
	RvUint8 encoded[256];
	int encLen=0;
	HPVT hVal;

	unsigned char output[256]={0};
	unsigned char vector[128]={0};
	RvUint8 sharedSecret[64]={0};
	int nKeyLen=0;
	int nKeySecLen=0;
	int nCopyLen=m_nShareSecretLen;
	unsigned int nAlgorithmId=pChannel->AlgorithmId();

	if (call->m_hSecCall == NULL) return -1;
	hVal = cmGetValTree(m_hApp);

	if(nAlgorithmId == ALGORITHM_DES_CBC) { nKeyLen=7; nKeySecLen=8; }
	else if(nAlgorithmId == ALGORITHM_TRIPLE_DES_CBC ) { nKeyLen=21; nKeySecLen=24; }
	else if(nAlgorithmId == ALGORITHM_AES_CBC ) { nKeyLen=16; nKeySecLen=16; }
	else if(nAlgorithmId == ALGORITHM_AES256_CBC ) { nKeyLen=32; nKeySecLen=32; }
	else return -1;

	char * buf=pChannel->m_strSpecifiedSecretOLC;
	memset(vector,0,sizeof(vector));
	memcpy(vector,buf,nKeyLen);
	memcpy(pChannel->m_strSharedSecretOLC,buf,nKeyLen);
	//根据此通道用到的算法 取最后多少位
	memset(sharedSecret,0,sizeof(sharedSecret));
	memcpy(sharedSecret,call->m_strSharedSecret+nCopyLen-nKeyLen,nKeyLen);

	if(EncryptData(sharedSecret,nKeyLen,vector,nKeySecLen,output) <=0) return -1;

	unsigned char OID[128] = {0};
	int OIDLen=128;
	int len=H323CallMgr::Instance().GetEncDecAlgorithmOIDFromType((EncDecAlgorithmID)nAlgorithmId,OID,OIDLen) ;
	if(len<= 0) return -1;

	//create the key containing the encrypted
	encNodeId = pvtAddRoot(hVal, cmGetSynTreeByRootName(m_hApp, "H235Key"), 0, NULL);
	int err=0;
	err=pvtBuildByPath(hVal,encNodeId,"secureSharedSecret.paramS",0,NULL);
	err=pvtBuildByPath(hVal,encNodeId,"secureSharedSecret.algorithmOID",len, (const char *)OID);
	err=pvtBuildByPath(hVal,encNodeId,"secureSharedSecret.encryptedSessionKey",nKeySecLen,(const char *)output);

	//encode to buffer
	status = cmEmEncode(hVal, encNodeId, encoded, (int)sizeof(encoded), &encLen);

	//no need for the node anymore
	pvtDelete(hVal, encNodeId);
	if (status < 0)
	{
		return -1;
	}
	//create the message branch
	__pvtBuildByFieldIds(tmpNodeId, hVal, nodeId,
	{_anyField _anyField _h245(encryptionSync) LAST_TOKEN}, 0, NULL);

	int synchFlag=0;
	synchFlag=pChannel->PayloadType();
	//add the synch flag
	pvtAdd(hVal, tmpNodeId, __h245(synchFlag), synchFlag, NULL, NULL);
	//add the encrypted buffer to the h235 key field
	pvtAdd(hVal, tmpNodeId, __h245(h235Key), encLen, (char*)encoded, NULL);

	return 0;

}

int H235Mgr::H235SecurityChannelGetEncryptionSync(H323Call  *call,H323Channel * pChannel,RvPvtNodeId  nodeId)
{
	RvStatus status;
	RvPvtNodeId encNodeId;
	RvPvtNodeId tmpNodeId;
	RvUint8 encoded[256];
	int  encLen;
	HPVT hVal;
	int  lenData=-1;
	RvBool  bIsString=RV_FALSE;
	int nAlgoKeyLen=0;

	if (call->m_hSecCall == NULL) return -1;
	//if (!call->m_bSecureMedia) return -1;
	hVal = cmGetValTree(m_hApp);

	//get path value Algorithm Id
	EncDecAlgorithmID nAlgorithmId =(EncDecAlgorithmID) pChannel->AlgorithmId();
	char * msgName = cmGetProtocolMessageName(H323CallMgr::Instance().m_hApp, nodeId);
	if (strcmp(msgName, "openLogicalChannel") == 0)
	{
		bIsString=RV_FALSE;
		unsigned char strData[256]={0};
		int nodeId2=-1;
		int nRet=-1;
		nodeId2=pvtGetNodeIdByPath(hVal,nodeId,"request.openLogicalChannel.forwardLogicalChannelParameters.dataType");
		if(nodeId2 >=0)
		{
			nRet=pvtGetByPath(hVal,nodeId2,"h235Media.encryptionAuthenticationAndIntegrity.encryptionCapability.1.algorithm",NULL,&lenData, &bIsString );
		}
		if( nRet>=0 && bIsString && lenData>0 )
		{
			if( pvtGetString( hVal, nRet, lenData, (char*)strData ) > 0)
			{
				nRet=H323CallMgr::Instance().GetEncDecAlgorithmTypeFromOID(nAlgorithmId,strData,lenData);
			}
		}
		if(nRet <=0) 
		{
			//OutputDebugString("Get openLogicalChannel H235Media Algorithm Error!\n");
			return -1;
		}
	}

	if(nAlgorithmId == ALGORITHM_DES_CBC) nAlgoKeyLen=7;
	else if(nAlgorithmId == ALGORITHM_TRIPLE_DES_CBC ) nAlgoKeyLen=21;
	else if(nAlgorithmId == ALGORITHM_AES_CBC ) nAlgoKeyLen=16;
	else if(nAlgorithmId == ALGORITHM_AES256_CBC ) nAlgoKeyLen=32;
	else return -1;

	__pvtGetNodeIdByFieldIds(tmpNodeId, hVal, nodeId, {_anyField _anyField _h245(encryptionSync) LAST_TOKEN});
	if (!RV_PVT_NODEID_IS_VALID(tmpNodeId))
	{
		return -1;
	}
	int nodeData = pvtGetByPath( hVal, tmpNodeId, "synchFlag", NULL, &lenData, &bIsString );
	//encNodeId = pvtAddRoot(hVal, cmGetSynTreeByRootName(m_hApp, "KeySyncMaterial"), 0, NULL);

	//get the key, to decode the encrypted vector from
	encLen = pvtGetString(hVal, pvtGetChild(hVal, tmpNodeId, __h245(h235Key), NULL),(RvInt32)sizeof(encoded) -1, (RvChar*)encoded);
	if (encLen < 0 || encLen > 1024 ) return -1;
	encNodeId = pvtAddRoot(hVal, cmGetSynTreeByRootName(m_hApp, "H235Key"), 0, NULL);
	status = cmEmDecode(hVal, encNodeId, encoded, encLen, &encLen);
	if (status < 0)
	{
		std::string strContent="";
		OutputPVTNodeContent(strContent,tmpNodeId);
		pvtDelete(hVal, encNodeId);
		//OutputDebugString("Decode H235Key Error!\n");
		return -1;
	}

	unsigned int paramsIV =0;
	nodeData= pvtGetByPath( hVal, encNodeId, "secureSharedSecret.paramS", NULL, (int *)&paramsIV, &bIsString );
	nodeData = pvtGetByPath(hVal, encNodeId, "secureSharedSecret.encryptedSessionKey", NULL, &lenData, &bIsString );

	if( nodeData && bIsString && lenData>0 )
	{
		unsigned char encoded[256];
		encLen = pvtGetString( hVal, nodeData, lenData, (char*)encoded );
		if( encLen > 0)
		{
			unsigned char output[256]={0};
			RvUint8 sharedSecret[64]={0};
			int nKeyLen=0;
			int nKeySecLen=0;
			int nCopyLen=m_nShareSecretLen;
			nKeySecLen=encLen;

			if(nKeySecLen ==8 ) nKeyLen=7;
			else if(nKeySecLen ==24 ) nKeyLen=21;
			else if(nKeySecLen ==16 ) nKeyLen=16;
			else if(nKeySecLen ==32 ) nKeyLen=32;
			else return -1;

			if(nAlgoKeyLen !=nKeyLen) return -1;
			//根据此通道用到的算法 取最后多少位
			memset(sharedSecret,0,sizeof(sharedSecret));
			memcpy(sharedSecret,call->m_strSharedSecret+nCopyLen-nKeyLen,nKeyLen);

			if(DecryptData(sharedSecret,nKeyLen,encoded,nKeySecLen,output) <=0) return -1;

			//copy to Channel m_strSharedSecretOLC
			memcpy(pChannel->m_strSharedSecretOLC,output,nKeyLen);
		}
	}

	pvtDelete(hVal, encNodeId);
	return 0;
}

int H235Mgr::DecryptData(const unsigned char * sharedSecret,int nKeyLen,unsigned char * vector,int dataLen,unsigned char * output)
{
	int ret=1;
	unsigned char key[8];
	DES_key_schedule ks[3];

	if(nKeyLen==16 || nKeyLen==32) //aes
	{
		AES_KEY  aesKey;
		RvUint16 keyBitSize=nKeyLen * 8;
		unsigned char iv[16]={0};
		AES_set_decrypt_key(sharedSecret,keyBitSize, &aesKey);
		for(int i=0; i<16; ++i) { iv[i] = 0; } //the iv is set 0
		AES_cbc_encrypt(vector, output,dataLen, &aesKey,iv, (const int)AES_DECRYPT);
	}
	else if(nKeyLen==7) //des 56bit
	{
		unsigned char iv[8]={0};
		memset(iv,0,sizeof(iv)); //the iv is set 0
		memset(key, 0, sizeof(key));
		memcpy(key, sharedSecret, 7);
		DES_set_key_unchecked((const_DES_cblock*)key, &ks[0]);
		DES_ncbc_encrypt(vector, output, dataLen, &ks[0],(DES_cblock*)iv,RV_FALSE);
	}
	else if(nKeyLen==21) //3des 168bit
	{
		//TripleDES, with three keys
		unsigned char iv[24]={0};
		memset(iv,0,sizeof(iv));
		memset(key, 0, sizeof(key));
		memcpy(key, sharedSecret,7);
		DES_set_key_unchecked((const_DES_cblock*)key, &ks[0]);
		memcpy(key, sharedSecret+7,  7);
		DES_set_key_unchecked((const_DES_cblock*)key, &ks[1]);
		memcpy(key, sharedSecret+14, 7);
		DES_set_key_unchecked((const_DES_cblock*)key, &ks[2]);
		DES_ede3_cbc_encrypt(vector, output, dataLen, &ks[0], &ks[1], &ks[2],(DES_cblock*)iv, RV_FALSE);
	}
	else
	{
		return 0;
	}
	return ret;
}

int H235Mgr::EncryptData(const unsigned char * sharedSecret,int nKeyLen,unsigned char * vector,int dataLen,unsigned char * output)
{
	int ret=1;
	unsigned char key[8];
	DES_key_schedule ks[3];

	if(nKeyLen==16 || nKeyLen==32) //aes
	{
		AES_KEY  aesKey;
		RvUint16 keyBitSize=nKeyLen * 8;
		unsigned char iv[16]={0};
		for(int i=0; i<16; ++i) { iv[i] = 0; } //the iv is set 0
		int setrt=AES_set_encrypt_key(sharedSecret,keyBitSize, &aesKey);
		AES_cbc_encrypt(vector, output, dataLen, &aesKey,iv, (const int)AES_ENCRYPT);
	}
	else if(nKeyLen==7) //des 56
	{
		unsigned char iv[8]={0};
		memset(iv,0,sizeof(iv)); //the iv is set 0
		memset(key, 0, sizeof(key));
		memcpy(key, sharedSecret, 7);
		DES_set_key_unchecked((const_DES_cblock*)key, &ks[0]);
		DES_ncbc_encrypt(vector, output, dataLen, &ks[0], (DES_cblock*)iv, RV_TRUE);
	}
	else if(nKeyLen==21) //3des 168
	{
		//TripleDES, with three keys
		unsigned char iv[24]={0};
		memset(iv,0,sizeof(iv));
		memset(key, 0, sizeof(key));
		memcpy(key, sharedSecret,    7);
		DES_set_key_unchecked((const_DES_cblock*)key, &ks[0]);
		memcpy(key, sharedSecret+7,  7);
		DES_set_key_unchecked((const_DES_cblock*)key, &ks[1]);
		memcpy(key, sharedSecret+14, 7);
		DES_set_key_unchecked((const_DES_cblock*)key, &ks[2]);
		DES_ede3_cbc_encrypt(vector, output, dataLen, &ks[0], &ks[1], &ks[2], (DES_cblock*)iv, RV_TRUE);
	}
	else
	{
		return 0;
	}
	return ret;
}

H235Mgr::H235Mgr()
{
	m_nStarted=0;
	InitData();
}

H235Mgr::~H235Mgr()
{

}

void H235Mgr::InitData()
{
	m_hApp=NULL;
	m_h235App=NULL;

	m_strKey[0]=0;
	m_strPublicKey[0]=0;
	m_nPublicKeySize=0;
	m_nSecurityMode=h235ModeNone;
	memset(&m_myRSA,0,sizeof(SecurityCertificate));
	memset(&m_gkRSA,0,sizeof(SecurityCertificate));
	memset(&m_tmpRSA,0,sizeof(SecurityCertificate));

	m_bIsRegistered=false;  //reg to gk status
	m_bCheckIncoming=false; //checking authentication on incoming messages

	memset(m_strPassword ,0,sizeof(m_strPassword ) );
	memset(m_strSenderId ,0,sizeof( m_strSenderId) );
	memset( m_strGeneralId,0,sizeof(m_strGeneralId ) );
	memset( m_strRemoteId,0,sizeof(m_strRemoteId ) );

	memset(&m_senderId ,0,sizeof( entityId_t) );
	memset(&m_generalId,0,sizeof(entityId_t ) );
	memset(&m_remoteId,0,sizeof(entityId_t ) );

	m_bH235AnnexI=false;     //annex D I 消息完整性及密码认证
	m_bH235AnnexIA=false;    //annex D IA 仅密码认证
	m_bH235AnnexII=false;    //annex E II 采用公钥/私钥对的数字签名
	m_bH235AnnexIII=false;   //annex E III 端到端认证
	m_bH235AnnexIV=false;    //annex F 规程IV
	m_bUserPrimes=false;     //媒体流加密 使用素数参与生成共享密钥
	m_bH235MediaDH512=false; //媒体流加密 使用DH512生成7字节共享密钥
	m_bH235MediaDH1024=false;//媒体流加密 使用DH1024生成21字节共享密钥
	m_bH235MediaDH1536=false;//媒体流加密 使用DH1536生成21字节共享密钥
	m_nAuthCheckState=0;	 //1 成功 2 失败 3 H235媒体流共享密钥协商失败
	memset(m_strCheckFailureReason,0,sizeof(m_strCheckFailureReason) );
	m_nAuthFailureReason=h235ReasonUndefined;

	m_nShareSecretLen=0;
}

const char*  H235Mgr::H235SecurityGetAuthCheckState(int &nState,int &nLength)
{
	//m_nAuthCheckState与IMXEC_CALL_H235AUTH_STATE定义对应
	if(m_nAuthCheckState==1)  
		strcpy(m_strCheckFailureReason,"H235SecurityCheckSuccessPassed");
	else if(m_nAuthCheckState==2)
	{
		const char * p=h235ReasonName(m_nAuthFailureReason);
		if(strlen(p) < MAXREASONLEN)
			strcpy(m_strCheckFailureReason,p);
	}
	else if(m_nAuthCheckState==3)  
		strcpy(m_strCheckFailureReason,"H235MediaDiffie-HellmanSharedPasswordDiscussFailure");
	else  
		strcpy(m_strCheckFailureReason,"NotSetH235SecurityCheck");

	nState=m_nAuthCheckState;
	nLength=strlen(m_strCheckFailureReason);
	return m_strCheckFailureReason;
}

void H235Mgr::H235SecurityAnnexISetting(bool enable)
{
	m_bH235AnnexI=enable;
}
void H235Mgr::H235SecurityAnnexIASetting(bool enable)
{
	m_bH235AnnexIA=enable;
}
void H235Mgr::H235SecurityAnnexIISetting(bool enable)
{
	m_bH235AnnexII=enable;
}
void H235Mgr::H235SecurityAnnexIIISetting(bool enable)
{
	m_bH235AnnexIII=enable;
}
void H235Mgr::H235SecurityAnnexIVSetting(bool enable)
{
	m_bH235AnnexIV=enable;
}
void H235Mgr::H235SecurityMediaDH512Setting(bool enable)
{
	m_bH235MediaDH512=enable;
}
void H235Mgr::H235SecurityMediaDH1024Setting(bool enable)
{
	m_bH235MediaDH1024=enable;
}
void H235Mgr::H235SecurityMediaDH1536Setting(bool enable)
{
	m_bH235MediaDH1536=enable;
}

RvStatus H235Mgr::H235SecurityInit(HAPP hApp)
{
	RvStatus status;
	h235EventHandlers_T eh;
	RvUint8* ptr;
	if (m_nStarted!=0) return 0;
	InitData();
	m_hApp=hApp;

	status = h235Init(hApp, &m_h235App);
	if (status < 0)
		return status;

	//cmGetChannelEventHandler(m_hApp, &cmChannEvent, sizeof(SCMCHANEVENT));
	//cmSetChannelEventHandler(m_hApp, &cmChannEvent, sizeof(SCMCHANEVENT));

	m_myRSA.ep = this;
	m_gkRSA.ep = this;
	m_tmpRSA.ep = this;

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

	h235SetEventHandlers(m_h235App, &eh);

	//eh.evGetIDByRAS             = evGetIDByRAS;
	//eh.evGetEntryByEndpointId   = evGetEntryByEndpointId;
	//eh.evChooseEndpointSecureMode = evChooseEndpointSecureMode;
	//eh.evGetKeyHandleById         = evGetKeyHandleById;
	//eh.evReceivedToken            = evReceivedToken;



	h235SetEncodingCallback(m_h235App, hash_SH1_96Callback, OID_U_STRING);
	h235SetEncodingCallback(m_h235App, sign_MD5_Callback, OID_V_STRING);
	h235SetEncodingCallback(m_h235App, sign_SH1_Callback, OID_W_STRING);
	h235SetTimeSync(m_h235App, RV_TRUE, 86500); /*allow max 24 hours diff */

	bool bUseRSAGenerate =false; //注意 openssl RSA_generate_key 会有稍许内存泄漏
	if(bUseRSAGenerate)
	{
	RvChar rnd_seed[] = "string to make the random number generator think it has entropy";
	/*CRYPTO_malloc_debug_init();*/
	CRYPTO_dbg_set_options(V_CRYPTO_MDEBUG_ALL);
	CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
	ERR_load_crypto_strings();

	RAND_seed(rnd_seed, sizeof(rnd_seed)); /* or OAEP may fail */

	/* create my own public and private key (or prepare for reading one, same command) */
	m_myRSA.rsa = RSA_generate_key(1024, 3, NULL, NULL);
	/* check if we have a private certificate */
	//if (privateCrt != NULL)
	//{
	//    if (privateCrtSz > 500)
	//    {
	//        d2i_RSAPrivateKey((RSA**)&myRSA.rsa, (const unsigned char **)&privateCrt, privateCrtSz);
	//    }
	//    if ((privateCrtSz < 500) || (myRSA.rsa == NULL))
	//    {
	//    }
	//}

	ptr = m_strPublicKey;
	m_nPublicKeySize = (RvSize_t)i2d_RSA_PUBKEY((RSA*)m_myRSA.rsa, (RvUint8**)&ptr);
	m_gkRSA.rsa = NULL;
	m_tmpRSA.rsa = NULL;
	h235SetCertificateHandles(m_h235App, (H235HKEY)&m_gkRSA, (H235HKEY)&m_myRSA);
	}

	//cmSetHandle(hApp, (HAPPAPP)this);

	m_nStarted=1;
	return RV_OK;
}

RvStatus H235Mgr::H235SecurityEnd()
{
	if (m_nStarted==0)
	{
		return RV_OK;
	}
	m_nStarted=0;
	
	if (m_h235App == NULL)
		return RV_OK;

	H235HAPP h235App =m_h235App;
	m_h235App =NULL;
	h235End(h235App);

	ERR_free_strings();

	if (m_myRSA.rsa != NULL)
		RSA_free((RSA*)m_myRSA.rsa);
	if (m_gkRSA.rsa != NULL)
		RSA_free((RSA*)m_gkRSA.rsa);
	if (m_tmpRSA.rsa != NULL)
		RSA_free((RSA*)m_tmpRSA.rsa);

	return RV_OK;
}



void H235Mgr::H235SecurityVersion(char * buffer,int &bufLen)
{
	if (m_h235App != NULL)
	{
		if(bufLen>64)
		{
		sprintf(buffer, "%s, With OpenSSL", h235GetVersion());
		bufLen=strlen(buffer);
		}
		else bufLen=0;
	}
	else
		bufLen=0;
}



RvStatus H235Mgr::H235SecurityUserPrimeSetting(bool enable)
{
	m_bUserPrimes=enable;
	return RV_OK; 
}


RvStatus H235Mgr::H235SecurityCheckIncomingMsgSetting(bool check )
{
	m_bCheckIncoming=check;
	return RV_OK;
}

RvStatus H235Mgr::H235SecurityAuthSetting()
{
	return H235SecurityAuthInfoSetting(m_strSenderId,m_strGeneralId,m_strPassword);
}

RvStatus H235Mgr::H235SecurityAuthSetting(char * senderId,char * generalId,char * password)
{
	char * remoteId=generalId;

	if( (senderId ==NULL) || (generalId ==NULL) || (password ==NULL) )  return RV_OK;
	//if( (senderId[0]==0) || (generalId[0]==0) || (password[0]==0) ) return RV_OK;

	strcpy(m_strSenderId,senderId);
	strcpy(m_strGeneralId,generalId);
	strcpy(m_strPassword,password);
	strcpy(m_strRemoteId,remoteId);

	return RV_OK;
}

RvStatus H235Mgr::H235SecurityModeSetting()
{
	if (m_h235App == NULL) return RV_OK; /* Nothing to do here */

	/* set the security mode */
	bool m_bH235AnnexE=(m_bH235AnnexII || m_bH235AnnexIII) ? true : false ;

	//注意 AnnexD I或AnnexD IA 与AnnexF IV不能同时用
	if(m_bH235AnnexIV)
	{
		if ( (m_bH235AnnexI) || (m_bH235AnnexIA) ) m_bH235AnnexIV=false;
	}

	if (m_bH235AnnexI)
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) | (RvUint32)h235ModeProcedure1);
	else
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) & ~(RvUint32)h235ModeProcedure1);

	if (m_bH235AnnexIA)
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) | (RvUint32)h235ModeProcedure1a);
	else
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) & ~(RvUint32)h235ModeProcedure1a);

	if (m_bH235AnnexE )
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) | (RvUint32)h235ModeProcedure2 | (RvUint32)h235ModeProcedure3);
	else
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) & (~(RvUint32)h235ModeProcedure2 & ~(RvUint32)h235ModeProcedure3));

	if (m_bH235AnnexIV)
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) | (RvUint32)h235ModeProcedure4);
	else
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) & ~(RvUint32)h235ModeProcedure4);

	if (m_bH235MediaDH512)
	{
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) | (RvUint32)h235ModeDhInClear);
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) | (RvUint32)h235ModeDHgroup512);
		m_nShareSecretLen = 512 /8;
	}
	else
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) & ~(RvUint32)h235ModeDHgroup512);

	if (m_bH235MediaDH1024)
	{
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) | (RvUint32)h235ModeDhInClear);
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) | (RvUint32)h235ModeDHgroup1024);
		m_nShareSecretLen = 1024/8;
	}
	else
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) & ~(RvUint32)h235ModeDHgroup1024);

	if (m_bH235MediaDH1536)
	{
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) | (RvUint32)h235ModeDhInClear);
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) | (RvUint32)h235ModeDHgroup1536);
		m_nShareSecretLen = 1536 /8;
	}
	else
		m_nSecurityMode = (h235Mode_t)(((RvUint32)m_nSecurityMode) & ~(RvUint32)h235ModeDHgroup1536);

	h235SetSecureMode(m_h235App,m_nSecurityMode);

	return RV_OK;
}


RvStatus H235Mgr::H235SecuritySettingSubmit()
{
	if (m_nStarted==0) return RV_OK;

	if (m_h235App == NULL) return RV_OK; /* Nothing to do here */
	
	//if( (m_strSenderId[0]==0) || (m_strGeneralId[0]==0) || (m_strPassword[0]==0) ) return RV_OK;

	H235SecurityAuthInfoSetting(m_strSenderId,m_strGeneralId,m_strPassword);

	H235SecurityModeSetting();

	if ((m_bCheckIncoming) && ((m_nSecurityMode & RV_H235_ALL_SEC_MODES) == h235ModeNone) )
		m_bCheckIncoming=false;
	h235SetCheckIncomingMessages(m_h235App, m_bCheckIncoming);
	
	if (m_bUserPrimes)
	{
		/* Use our application specific prime numbers generator */
		h235SetEncodingCallback(m_h235App, pickAPrime, RV_H235_GENERATE_PRIME_OID);
	}
	else
	{
		/* Use the H.235 built-in prime numbers generator */
		h235SetEncodingCallback(m_h235App, NULL, RV_H235_GENERATE_PRIME_OID);
	}

	return RV_OK;
}


RvStatus H235Mgr::H235SecurityAuthInfoSetting(char * senderId,char * generalId,char * password)
{
	//char msg[128]={0};
	//sprintf(msg,"sendid:%s,generalid:%s,password:%s\n",senderId,generalId,password);
	//OutputDebugString(msg);

	if (m_h235App == NULL) return RV_OK; /* Nothing to do here */

	int length=0;
	unsigned char strBuf[256]={0};
	char *tmpStr = NULL;

	//notice remoteId = generalId
	char * remoteId=generalId;

	/* change GK related parameters only if we are not already registered */
	if (!m_bIsRegistered)
	{
		/* get key and hash it as required by the standard */
		memset(m_strKey, 0, sizeof(m_strKey));

		if(strlen(password) >0 )
		SHA1((unsigned char *)password, strlen(password), (unsigned char*)m_strKey);

		if(strlen(generalId) >0 )
		{
		tmpStr=generalId;
		m_generalId.length = utlChrn2Bmp(tmpStr, RvMin(strlen(tmpStr), 128), m_generalId.data);
		//OutputDebugString("h235SetGatekeeperId\n");
		if(m_h235App) h235SetGatekeeperId(m_h235App, &m_generalId, (H235HKEY)this);
		}

		if(strlen(senderId) >0 )
		{
		tmpStr=senderId;
		m_senderId.length = utlChrn2Bmp(tmpStr, RvMin(strlen(tmpStr), 128), m_senderId.data);
		if(m_h235App) h235SetSenderId(m_h235App, &m_senderId);
		}

		if(strlen(remoteId) >0 )
		{
		tmpStr=remoteId;
		m_remoteId.length = utlChrn2Bmp(tmpStr, RvMin(strlen(tmpStr), 128), m_remoteId.data);
		}

	}
	return RV_OK;
}



void H235Mgr::H235SecurityGetSenderId()
{
	RvChar idStr[513];
	entityId_t entId;

	if (m_h235App == NULL) return;

	h235GetSenderId(m_h235App, &entId);
	utlBmp2Chr(idStr, entId.data, entId.length);
}


int H235Mgr::H235SecurityCallInit(H323Call * call)
{
	H235Mgr *ep = this;

	if (m_h235App == NULL) return RV_OK;

	if ((RvUint32)ep->m_nSecurityMode & ((RvUint32)RV_H235_ALL_DH_MODES | (RvUint32)h235ModeProcedure3))
	{
		h235Mode_t secureMode =(h235Mode_t)ep->m_nSecurityMode;

		/* we are using DH, init the H235 call struct */
		call->m_bUseDiffieHellman = RV_TRUE; 
		call->m_hSecCall = (H235HCALL)H235MemoryMalloc( (RvSize_t)h235GetSizeofCallInfo(), __FILE__, __LINE__);
		h235InitCallObject(m_h235App, call->m_hSecCall);

		/* Set the remote endpoint id */
		call->m_remoteEndpoint = m_remoteId;

		/* choose ONE Diffie-Hellman mode */
		if ((secureMode & h235ModeDHgroup1536) != 0) secureMode = h235ModeDHgroup1536;
		else if ((secureMode & h235ModeDHgroup1024) != 0) secureMode = h235ModeDHgroup1024;

		if ((RvUint32)secureMode & (RvUint32)RV_H235_ALL_DH_MODES)
		{
			h235GenerateDiffieHellman(m_h235App, RV_H235_MODE_TO_DH_MODE(secureMode), call->m_hSecCall);
		}

		call->m_nSecVector=7;
		if( (secureMode == h235ModeDHgroup1024) || (secureMode == h235ModeDHgroup1536 ) )
		{
			call->m_nSecVector=21;
		}
		for (int i = 0; i < call->m_nSecVector; i++)
		{
			call->m_strSecVector[i] = (char)rand();
		}
	}
	else
	{
		call->m_hSecCall = NULL;
	}

	return RV_OK;
}


int  H235Mgr::H235SecurityCallEnd(H323Call    *call)
{
	if ( (m_h235App == NULL) || (call == NULL) || (call->m_hSecCall == NULL))
		return RV_OK;

	// end the H235 call struct
	if (call->m_hSecCall != NULL)
	{
		h235EndCallObject(m_h235App, call->m_hSecCall);
		H235MemoryFree( (void *)call->m_hSecCall);
	}

	call->m_hSecCall = NULL;
	return RV_OK;
}

void H235Mgr::H235SecurityCallGenerateSharedSecret(H323Call    *call)
{
	h235Mode_t secMode;
	if (call->m_hSecCall == NULL) return;

	/* check if we want to use Diffie-Hellman */
	if (!call->m_bUseDiffieHellman)
		return;

	secMode = h235GetCallMode(call->m_hSecCall);

	if ((RvUint32)secMode & (RvUint32)h235ModeDHgroup1536)
		m_nShareSecretLen = 1536 /8;
	else if ((RvUint32)secMode & (RvUint32)h235ModeDHgroup1024)
		m_nShareSecretLen = 1024 /8;
	else if ((RvUint32)secMode & (RvUint32)h235ModeDHgroup512)
		m_nShareSecretLen = 512 /8;

	/* init the shared secret and get it */
	memset(call->m_strSharedSecret, 0, sizeof(call->m_strSharedSecret));

	if (h235GenarateDiffieHellmanSharedSecret(call->m_hSecCall, RV_TRUE, call->m_strSharedSecret) >= 0)
	{
		//OutputDebugString("\nCall ShareSecret: \n");
		char lastkey[512]={0};
		int nCopyLen=m_nShareSecretLen;
		ConvertCharToHexChar((unsigned char *)lastkey,(unsigned char *)call->m_strSharedSecret,nCopyLen);
		//OutputDebugString(lastkey);
		//OutputDebugString("\n");
		call->m_bShareSecret=true;
	}
	else
	{
		m_nAuthCheckState=3;
		call->m_bShareSecret=false;
	}
}


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

static void encryptVector(
						  IN h235DHMode_t     dhMode,
						  IN unsigned char    *vector,
						  IN int              vecLen,
						  IN char             *sharedSecret)
{
	unsigned char key[8];
	DES_key_schedule ks[3];

	unsigned char keyTemp[32];
	memset(keyTemp, 0, sizeof(keyTemp));
	
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
		//notice alter test ...
		//DES_ede3_cbc_encrypt(vector, vector, vecLen, &ks[0], &ks[1], &ks[3], NULL, RV_TRUE);
		memcpy(keyTemp,sharedSecret,21);
		DES_ede3_cbc_encrypt(vector, vector, vecLen, &ks[0], &ks[1], &ks[3], (DES_cblock*)keyTemp, RV_TRUE);
		break;

	case h235ModeDiffieHellmanNone:
		break;
	}
}

static void decryptVector(
						  IN h235DHMode_t     dhMode,
						  IN unsigned char    *vector,
						  IN int              vecLen,
						  IN char             *sharedSecret)
{
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
}


#ifdef __cplusplus
}
#endif
