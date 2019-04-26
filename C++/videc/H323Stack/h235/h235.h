#ifdef __cplusplus
extern "C" {
#endif

#ifndef __H235_H
#define __H235_H


#include "h235_api.h"
#include "h235buffnum.h"
#include "rvmutex.h"
#include "stkutils.h"

/*=============================================
 * TYPEDEFS
 ==============================================*/


typedef struct {
    RvBool       isValid;
    h235DHMode_t DHMode;
    RvUint8      halfkey[256];       /* contains the random public key of one party */
    RvUint8      modSize[256];       /* contains the DH-prime */
    RvUint8      generator[256];     /* contains the DH-group */
} h235DiffieHellmanInfo_t;


typedef struct {
    entityId_t                  id;
    H235HKEY                    keyHandle;
    H235HKEY                    crtInHandle;
    H235HKEY                    crtOutHandle;
    RvUint32                    lastTime;
    RvInt32                     lastRandom;
    h235Mode_t                  secureMode;
    RvBool                      bGenerateSecret;
    h235DiffieHellmanInfo_t     inDHinf;
    h235DiffieHellmanInfo_t     outDHinf;
    RvUint16                    myRandomNumber[MAX_BUFFNUM_SIZE];
} h235EntityEntry_t;


typedef struct
{
    HAPP                hApp;
    h235EventHandlers_T eventHandlers;
    h235EvEncode_t      evSHA1_96;
    h235EvEncode_t      evMD5_RSA_Sig;
    h235EvEncode_t      evSHA1_RSA_Sig;
/*    h235EvEncode_t      evDES;
    h235EvEncode_t      evTripleDES;*/
    h235EvEncode_t      generatePrime;

    entityId_t          senderId;
    h235Mode_t          secureMode; /* for outgoing messages */
    RvBool              isCheckIncoming;
    RvBool              performTimeSync;
    RvInt32             timeSyncDiff;   /* in seconds */

    RvMutex             mutex;
    RvBool              isGK;

    /* relevant only when isGK is RV_FALSE */
    h235EntityEntry_t   gkEntry;
    HPST                cryptoTokenSyn;
    HPST                clearTokenSyn;
    HPVT                hVal; /* Value tree of hApp. Here for faster access */

    /* Log parameters */
    RvLogMgr*           logMgr; /* Log manager to use */
    RvLogSource         h235Chan;
} h235Element_T;


typedef struct {
    h235State_t       state;
    RvBool            isMsgDeleted;
    h235Reason_t      reason;
    h235Mode_t        secureMode; /* the mode which incoming message was secured */
} h235ContextInfo_t;


#define MAX_CALL_CRYPTO_TOKENS 4 /* maximum number of call related crypto tokens (2*DH+2*P3) */

typedef struct {
    HCALL                       hsCall;
    RvPvtNodeId                 sendCryptoToken[MAX_CALL_CRYPTO_TOKENS];
    RvPvtNodeId                 sendClearToken;
    h235Mode_t                  secureMode;
    RvBool                      bSecureMedia;
    h235DiffieHellmanInfo_t     inDHinf;
    h235DiffieHellmanInfo_t     outDHinf;
    RvUint16                    myRandomNumber[MAX_BUFFNUM_SIZE];
} h235CallInfo_t;


typedef struct {
    RvUint32 time;   /*the timestamp from the message*/
    RvInt32 random; /*the randon from the message*/
} h235TimeInfo_t;

/*=============================================
 * HEADERS
 ==============================================*/

h235AppFailReply_t failToApp(
    IN  h235Element_T* h235App,
    IN  int         messageRoot,
    IN  entityId_t  *senderId,
    IN  h235Reason_t reason,
    OUT h235ContextInfo_t   *contextInfo);

h235AppSuccessReply_t successToApp(
    IN  h235Element_T* h235App,
    IN  int         messageRoot,
    IN  entityId_t  *senderId,
    IN  h235Reason_t reason,
    OUT h235ContextInfo_t   *contextInfo);

RvStatus freezeRasMessage(
    IN  h235Element_T*      h235App,
    IN  HPROTCONN           protConn,
    IN  int                 messageRoot,
    IN  entityId_t          *senderId,
    IN  int                 messageLen,
    IN  RvUint8             *messageBuf,
    IN  cmRASTransport      *srcAddress,
    IN  h235ContextInfo_t   *contextInfo);

RvStatus verifyTimeSync(
    IN  h235Element_T* h235App,
    IN  int         messageRoot,
    IN  entityId_t  *sendersID,
    IN  int         firstCryptoNode,
    IN  h235EntityEntry_t *entityEntry,
    OUT h235TimeInfo_t *timeInfo,
    OUT h235ContextInfo_t   *contextInfo);

RvStatus checkAuthentication(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  RvUint8            *messageBuf,
    IN  int                 messageLength,
    IN  int                 cryptoTokenNode,
    IN  HPROTCONN           protConn,
    IN  cmRASTransport     *srcAddress,
    OUT h235ContextInfo_t  *contextInfo);

RvStatus getEntityEntry(
    IN  h235Element_T*      h235App,
    IN  entityId_t*         entityId,
    IN  h235EntityEntry_t** entityEntryPtr,
    IN  h235Mode_t          secureMode);

void secureMessage(
    IN  h235Element_T*      h235App,
    IN  h235EntityEntry_t*  entityEntryPtr,
    IN  h235CallInfo_t*     callInfoPtr,
    IN  RvPvtNodeId         messageRoot,
    OUT RvPvtNodeId         tokens[5]);

RvStatus generateDiffieHellman(
    IN  h235Element_T*  h235App,
    IN  h235DHMode_t    DHMode,
    IN  h235DiffieHellmanInfo_t* inDHinf,
    IN  h235DiffieHellmanInfo_t* outDHinf,
    IN  RvUint16 myRandomNumber[MAX_BUFFNUM_SIZE]);

RvStatus genarateDHSharedSecret(
    IN  h235DiffieHellmanInfo_t* dhInfo,
    IN  RvBool        leastSignificantFirst,
    IN  RvUint16      randomNumber[MAX_BUFFNUM_SIZE],
    OUT char *        sharedSecret);

RvStatus setDHP1TokenNode(
    IN  HPVT                        hVal,
    IN  h235DHMode_t                DHMode,
    IN  RvPvtNodeId                 nodeId,
    IN  h235DiffieHellmanInfo_t*    dhInf);

RvStatus setDHClTokenNode(
    IN  HPVT                        hVal,
    IN  h235DHMode_t                dhMode,
    IN  RvPvtNodeId                 nodeId,
    IN  h235DiffieHellmanInfo_t*    dhInf);

RvStatus getDHTokenNode(
    IN    HPVT                      hVal,
    IN    RvPvtNodeId               cryptoTokenNode,
    IN    RvPvtNodeId               clearTokenNode,
    OUT   h235DiffieHellmanInfo_t*  dhInf);


/*=============================================
 * OID STUFF
 ==============================================*/

typedef enum {
    OID_ZERO,/*  0 zero OID, used for certificate calculation */
    OID_A1,  /*  1 p1 or p2 CryptoToken indicating that the signature includes all fields in the RAS/H.225.0 message (authentication and integrity) */
    OID_A2,  /*  2 p1 or p2 CryptoToken indicating that the signature includes all fields in the RAS/H.225.0 message (authentication and integrity) */
    OID_A_1, /*  3 Used as replacement for OID "A" in p2 CryptoToken indicating that the signature includes all fields in the RAS/H.225.0 message (authentication and integrity). */
    OID_B1,  /*  4 p2 CryptoToken indicating that the signature includes a subset of fields in the RAS/H.225.0 message (ClearToken) for authentication-only terminals without integrity */
    OID_B2,  /*  5 p2 CryptoToken indicating that the signature includes a subset of fields in the RAS/H.225.0 message (ClearToken) for authentication-only terminals without integrity */
    OID_E1,  /*  6 End-to-end ClearToken carrying sendersID for verification at the recipient side */
    OID_E2,  /*  7 End-to-end ClearToken carrying sendersID for verification at the recipient side */
    OID_P1,  /*  8 p2 or p3 to indicate that certificate carries a URL */
    OID_P2,  /*  9 p2 or p3 to indicate that certificate carries a URL */
    OID_Q,   /* 10 Used in p4 indicating that the ClearToken on the hop-by-hop link carries an Diffie-Hellman token */
    OID_R1,  /* 11 p2 ClearToken indicating that the ClearToken is being used for end-to-end authentication/integrity */
    OID_R2,  /* 12 p2 ClearToken indicating that the ClearToken is being used for end-to-end authentication/integrity */
    OID_S1,  /* 13 p2 this token OID indicates message authentication, integrity and non-repudiation */
    OID_S2,  /* 14 p2 this token OID indicates message authentication, integrity and non-repudiation */
    OID_S_1, /* 15 Used as replacement for OID "S" in p2 ClearToken indicating  message authentication and integrity. This OID in the end-to-end CryptoToken implicitly indicates also use of DH during fast start */
    OID_T1,  /* 16 p1 ClearToken is used for message authentication and integrity */
    OID_T2,  /* 17 p1 ClearToken is used for message authentication and integrity */
    OID_U1,  /* 18 p1 c-sha1-96 algorithm, used only to compare with incoming messages */
    OID_U2,  /* 19 p1 c-sha1-96 algorithm, used only to compare with incoming messages */
    OID_V,   /* 20 MD5 RSA digital signature algorithmOID */
    OID_W,   /* 21 p2 and p4 SHA1 RSA digital signature algorithmOID */
    OID_W_1, /* 22 p4 SHA1 RSA digital signature algorithmOID */
    OID_X,   /* 23 Voice encryption using RC2-compatible (56 bit) or RC2-compatible in CBC mode and 512-bit DH-group */
    OID_X1,  /* 24 Voice encryption using RC2-compatible (56 bit) or RC2-compatible in CBC mode and 512-bit DH-group */
    OID_Y,   /* 25 Voice encryption using DES (56 bit) in CBC mode and 512-bit DH-group */
    OID_Y1,  /* 26 Voice encryption using DES (56 bit) in CBC mode and 512-bit DH-group */
    OID_Z,   /* 27 Voice encryption using Triple-DES (168-bit) in outer-CBC mode and 1024-bit DH-group */
    OID_Z1,  /* 28 Voice encryption using Triple-DES (168-bit) in outer-EOFB mode and 1024-bit DH-group with 64-bit feedback */
    OID_Z2,  /* 29 Voice encryption using AES (128-bit) in EOFB mode and 1024-bit DH-group */
    OID_Z3,  /* 30 Voice encryption using AES (128-bit) in CBC mode and 1024-bit DH-group */
    OID_DH_512_1, /* 31 Non-standard or 512-bit DH-group explicitly provided */
    OID_DH_512_2, /* 32 Non-standard or 512-bit DH-group explicitly provided */
    OID_DH_1024_1, /* 33 1024-bit DH group */
    OID_DH_1024_2, /* 34 1024-bit DH group */
    OID_DH_1536, /* 35 1536-bit DH group */
    OID_V3, /* Version 3 capability indicator in ClearToken during call signaling */
    OID_TEMP, /* temp */
    numOfOIds
} objectOidType;

typedef struct {
    char OID[numOfOIds][16];
    RvSize_t length[numOfOIds];
} objectsOidArray;

RvStatus  tokenOidToOidType(IN char *objectOID,IN RvSize_t objectOID_length,OUT objectOidType *oid);
RvStatus  algorithmOidToOidType(IN char *objectOID,IN RvSize_t objectOID_length,OUT objectOidType *oid);
void initOIDs(void);
h235Mode_t identifyNode(
    HPVT hVal,
    RvPvtNodeId cryptoTokenNode,
    RvPvtNodeId* cryptoNode);


#endif

#ifdef __cplusplus
}
#endif

