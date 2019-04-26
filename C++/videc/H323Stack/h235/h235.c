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


#include "rvinternal.h"
#include "rvstdio.h"
#include "rvmemory.h"
#include "rvclock.h"
#include "ci.h"
#include "cmiAddOns.h"
#include "cm.h"
#include "cmhook.h"
#include "cmiras.h"
#include "q931asn1.h"
#include "pvaltree.h"
#include "h235buffnum.h"
#include "h235.h"


#define PROCEDURE_1_PATTERN   "~*~!!!!!!~*~"
#define PROCEDURE_2_PATTERN   "prc2~*~@@@@@@~*~~*~@@@@@@~*~~*~@@@@@@~*~~*~@@@@@@~*~~*~@@@@@@~*~~*~@@@@@@~*~~*~@@@@@@~*~~*~@@@@@@~*~~*~@@@@@@~*~~*~@@@@@@~*~prc2"
#define PROCEDURE_4_PATTERN_1 "~4~$$$$$$~4~"
#define PROCEDURE_4_PATTERN_2 "prc4~*~$$$$$$~*~~*~$$$$$$~*~~*~$$$$$$~*~~*~$$$$$$~*~~*~$$$$$$~*~~*~$$$$$$~*~~*~$$$$$$~*~~*~$$$$$$~*~~*~$$$$$$~*~~*~$$$$$$~*~prc4"


static RvUint16 g_1024Prime[MAX_BUFFNUM_SIZE] =
{
    0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x5381,0xECE6,0x6651,0x4928,0x1FE6,0x7C4B,0x2411,0xAE9F,0x9FA5,0x5A89,0x6BFB,0xEE38,
    0xB7ED,0xF406,0x5CB6,0x0BFF,0xED6B,0xA637,0x42E9,0xF44C,0x7EC6,0x625E,0xB576,0xE485,0xC245,0x6D51,0x356D,0x4FE1,
    0x1437,0xF25F,0x0A6D,0x302B,0x431B,0xCD3A,0x19B3,0xEF95,0x04DD,0x8E34,0x0879,0x514A,0x9B22,0x3B13,0xBEA6,0x020B,
    0xCC74,0x8A67,0x4E08,0x2902,0x1CD1,0x80DC,0x628B,0xC4C6,0xC234,0x2168,0xDAA2,0xC90F,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

static RvUint16 g_1536Prime[MAX_BUFFNUM_SIZE] =
{
    0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x7327,0xCA23,0x6C08,0xF174,0x9804,0x4ABC,0x354E,0x670C,0x966D,0x7096,0x2907,0x9ED5,
    0x52BB,0x2085,0xF356,0x1C62,0xAD96,0xDCA3,0x5D23,0x8365,0xCF5F,0xFD24,0x3FA8,0x6916,0xD39A,0x1C55,0x4836,0x98DA,
    0xBF05,0xA163,0x7CB8,0xC200,0x5B3D,0xECE4,0x6651,0x4928,0x1FE6,0x7C4B,0x2411,0xAE9F,0x9FA5,0x5A89,0x6BFB,0xEE38,
    0xB7ED,0xF406,0x5CB6,0x0BFF,0xED6B,0xA637,0x42E9,0xF44C,0x7EC6,0x625E,0xB576,0xE485,0xC245,0x6D51,0x356D,0x4FE1,
    0x1437,0xF25F,0x0A6D,0x302B,0x431B,0xCD3A,0x19B3,0xEF95,0x04DD,0x8E34,0x0879,0x514A,0x9B22,0x3B13,0xBEA6,0x020B,
    0xCC74,0x8A67,0x4E08,0x2902,0x1CD1,0x80DC,0x628B,0xC4C6,0xC234,0x2168,0xDAA2,0xC90F,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};


/************************************************************
 * The handle that carries the security check results
 * (H235HMSG ), is a collection of bits described by the
 * following macros. If in future there will be a need to
 * save more information about the security check results,
 * I recommend that this handle will contain a pointer to
 * h235ContextInfo_t, but than we have to take care of
 * memory management (see h235cmEvRASReleaseMessageContext()
 * and h235cmEvCallReleaseMessageContext())
 * Dani Stettiner 19 Feb 2001
 *************************************************************/


static RvPvtNodeId secureByAuthAndIntegrityProcedure1(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  h235EntityEntry_t*  entityEntry,
    IN  h235CallInfo_t*     callInfo,
    IN  int                 cryptoTokensNode);

static RvPvtNodeId secureByAuthProcedure1a(
    IN  h235Element_T*     h235App,
    IN  int                messageRoot,
    IN  h235EntityEntry_t* entityEntry,
    IN  h235CallInfo_t*    callInfo,
    IN  int                cryptoTokensNode,
    OUT RvUint8*           buffer);

static RvPvtNodeId secureByAuthAndIntegrityProcedure2(
    IN  h235Element_T*          h235App,
    IN  int                     messageRoot,
    IN  h235EntityEntry_t*      entityEntry,
    IN  h235CallInfo_t*         callInfo,
    IN  int                     cryptoTokensNode,
    IN  h235CertificateType_t*  type);

static RvPvtNodeId secureByAuthAndIntegrityProcedure3(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  h235EntityEntry_t*  entityEntry,
    IN  h235CallInfo_t*     callInfo,
    IN  int                 cryptoTokensNode,
    OUT RvUint8*           buffer);

static RvPvtNodeId secureByAuthAndIntegrityProcedure4(
    IN  h235Element_T*          h235App,
    IN  int                     messageRoot,
    IN  h235EntityEntry_t*      entityEntry,
    IN  h235CallInfo_t*         callInfo,
    IN  int                     cryptoTokensNode,
    OUT h235CertificateType_t*  type,
    OUT RvPvtNodeId*            dhToken);

static RvStatus calcP1HashValue(
    IN  h235Element_T*  h235App,
    IN  char*           buffer,
    IN  int             buffLen,
    IN  H235HKEY        keyHandle,
    OUT unsigned char*  digest,
    OUT int*            digLen);

static RvStatus calcP2Signature(
    IN  h235Element_T*          h235App,
    IN  char*                   buffer,
    IN  int                     buffLen,
    IN  H235HKEY                crtHandle,
    OUT unsigned char*          digest,
    OUT int*                    digLen,
    IN  h235CertificateType_t   type);

RvStatus checkAuthenticationProc1(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  RvUint8            *messageBuf,
    IN  int                 messageLength,
    IN  int                 cryptoHashedTokenNode,
    IN  RvUint8             hashedVal[12],
    OUT h235ContextInfo_t  *contextInfo,
    OUT entityId_t         *sendersID);

RvStatus checkAuthenticationProc1a(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  int                 cryptoHashedTokenNode,
    OUT h235ContextInfo_t  *contextInfo,
    OUT entityId_t         *sendersID);

RvStatus checkAuthenticationProc2(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  RvUint8            *messageBuf,
    IN  int                 messageLength,
    IN  int                 cryptoSignedTokenNode,
    IN  RvUint8             signedVal[128],
    OUT h235ContextInfo_t  *contextInfo,
    OUT entityId_t         *sendersID);

RvStatus checkAuthenticationProc3(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  int                 cryptoSignedTokenNode,
    OUT h235ContextInfo_t  *contextInfo,
    OUT entityId_t         *sendersID);

RvStatus checkAuthenticationProc4(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  RvUint8            *messageBuf,
    IN  int                 messageLength,
    IN  int                 cryptoTokenNode,
    IN  RvUint8             signedVal[128],
    IN  RvUint8             hashedVal[12],
    OUT h235ContextInfo_t  *contextInfo,
    OUT entityId_t         *sendersID);

RvStatus checkByProcedure1(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  RvUint8            *messageBuf,
    IN  int                 messageLength,
    IN  int                 cryptoHashedTokenNode,
    IN  h235EntityEntry_t  *entityEntry,
    IN  RvUint8             hashedVal[12],
    OUT h235ContextInfo_t  *contextInfo);

RvStatus checkByProcedure1a(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  int                 cryptoHashedTokenNode,
    IN  h235EntityEntry_t*  entityEntry,
    OUT h235ContextInfo_t*  contextInfo);

RvStatus checkByProcedure2(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  RvUint8*            messageBuf,
    IN  int                 messageLength,
    IN  int                 cryptoSignedTokenNode,
    IN  h235EntityEntry_t*  entityEntry,
    IN  RvUint8             signedVal[128],
    OUT h235ContextInfo_t*  contextInfo);

RvStatus checkByProcedure3(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  int                 cryptoSignedTokenNode,
    IN  entityId_t*         entityId,
    IN  H235HKEY            crtInHandle,
    OUT h235ContextInfo_t*  contextInfo);

RvStatus checkSenderId(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  RvPvtNodeId         senderIdNode,
    IN  h235ContextInfo_t*  contextInfo,
    OUT entityId_t*         sendersID,
    OUT h235EntityEntry_t** entityEntryPtr,
    IN  RvBool              bEndToEndToken);


static int findSubstring(unsigned char *str, int strLen, int strIdx,
                         unsigned char *subStr, int subLen);

RVAPI
RvStatus RVCALLCONV h235GenarateDiffieHellmanSharedSecretEP(
    IN  H235HENTITY   hEntity,
    IN  RvBool        leastSignificantFirst,
    OUT char *        sharedSecret);

RVAPI
RvStatus RVCALLCONV h235GenerateDiffieHellmanEP(
    IN  H235HAPP       h235HApp,
    IN  h235DHMode_t   DHMode,
    IN  H235HENTITY    hEntity);

/**************************************************************************/


static const char * OIDstrArray[] = {
    "itu-t (0) recommendation (0)", /*ZERO*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 1 1", /*A1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 1", /*A2*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 20", /*A_1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 1 2", /*B1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 2", /*B2*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 1 9", /*E1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 9", /*E2*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 1 4", /*P1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 4", /*P2*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 22", /*Q*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 1 3", /*R1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 3", /*R2*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 1 7", /*S1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 7", /*S2*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 21", /*S_1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 1 5", /*T1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 5", /*T2*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 1 6", /*U1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 6", /*U2*/
    "iso(1) member-body(2) us(840) rsadsi(113549) pkcs(1) pkcs-1(1) 4", /*V*/
    "iso(1) member-body(2) us(840) rsadsi(113549) pkcs(1) pkcs-1(1) 5", /*W*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 23", /*W_1*/
    "iso(1) member-body(2) us(840) rsadsi(113549) encryptionAlgorithm(3) 2", /*X*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 3 27", /*X1*/
    "iso(1) identified-organization(3) oiw(14) secsig(3) algorithm(2) desCBC(7)", /*Y*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 3 28", /*Y1*/
    "iso(1) identified-organization(3) oiw(14) secsig(3) algorithm(2) desEDE(17)", /*Z*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 3 29", /*Z1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 3 30", /*Z2*/
    "joint-iso-itu-t(2) country(16) us(840) organization(1) gov(101) 3  nistAlgorithm(4)  aes(1) cbc(2)", /*Z3*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 40", /*DH_512_1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 3 40", /*DH_512_2*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 2 43", /*DH_1024_1*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 3 43", /*DH_1024_2*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 3 44", /*DH_1536*/

    "itu-t (0) recommendation (0) h (8) 235 version (0) 3 24", /*V3*/
    "itu-t (0) recommendation (0) h (8) 235 version (0) 1 5"/* temp */
};

static objectsOidArray objects; /*contains all object Id's and their length*/

void initOIDs()
{
    int i;
    for(i = (int)OID_ZERO; i < (int)numOfOIds; i++)
    {
        objects.length[i] = (RvSize_t)utlEncodeOID(sizeof(objects.OID[i]), objects.OID[i], OIDstrArray[i]);
    }
}

static RvStatus getTokenOID(HPVT hVal,int nodeId,OUT objectOidType *oid)
{
    /********************************************/
    /* get the tokenOID                         */
    /********************************************/
    int objectNode;
    char objectOID[128];
    RvSize_t objectOID_length;

    objectNode = pvtGetChild(hVal, nodeId, __q931(tokenOID), NULL);
    if (objectNode < 0)
        return RV_ERROR_UNKNOWN;
    objectOID_length = (RvSize_t)pvtGetString(hVal, objectNode, sizeof(objectOID), objectOID);

    /************************************************************************/
    /* get the tokenOID as "A" or "B" or "T" or "R" ("R" for future imp)    */
    /* note, "A" is just a shortage used in the standard                    */
    /* the real value is a string                                           */
    /************************************************************************/
    return (tokenOidToOidType(objectOID, objectOID_length, oid));
}


RvStatus tokenOidToOidType(IN char *objectOID,IN RvSize_t objectOID_length,OUT objectOidType *oid)
{
    /************************************************************************/
    /* get the tokenOID as "A" or "B" or "T" or "R" ("R" for future imp)    */
    /* note, "A" is just a shortage used in the standard                    */
    /* the real value is a string                                           */
    /************************************************************************/
    if ((objectOID_length == objects.length[OID_A1]) &&
        (memcmp(objects.OID[OID_A1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_A1;
    }
    else if((objectOID_length == objects.length[OID_A2]) &&
        (memcmp(objects.OID[OID_A2],objectOID,objectOID_length) == 0))
    {
        *oid = OID_A2;
    }
    else if((objectOID_length == objects.length[OID_A_1]) &&
        (memcmp(objects.OID[OID_A_1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_A_1;
    }
    else if((objectOID_length == objects.length[OID_B1]) &&
        (memcmp(objects.OID[OID_B1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_B1;
    }
    else if((objectOID_length == objects.length[OID_B2]) &&
        (memcmp(objects.OID[OID_B2],objectOID,objectOID_length) == 0))
    {
        *oid = OID_B2;
    }
    else if((objectOID_length == objects.length[OID_E1]) &&
        (memcmp(objects.OID[OID_E1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_E1;
    }
    else if((objectOID_length == objects.length[OID_E2]) &&
        (memcmp(objects.OID[OID_E2],objectOID,objectOID_length) == 0))
    {
        *oid = OID_E2;
    }
    else if((objectOID_length == objects.length[OID_P1]) &&
        (memcmp(objects.OID[OID_P1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_P1;
    }
    else if((objectOID_length == objects.length[OID_P2]) &&
        (memcmp(objects.OID[OID_P2],objectOID,objectOID_length) == 0))
    {
        *oid = OID_P2;
    }
    else if((objectOID_length == objects.length[OID_Q]) &&
        (memcmp(objects.OID[OID_Q],objectOID,objectOID_length) == 0))
    {
        *oid = OID_Q;
    }
    else if((objectOID_length == objects.length[OID_R1]) &&
        (memcmp(objects.OID[OID_R1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_R1;
    }
    else if((objectOID_length == objects.length[OID_R2]) &&
        (memcmp(objects.OID[OID_R2],objectOID,objectOID_length) == 0))
    {
        *oid = OID_R2;
    }
    else if((objectOID_length == objects.length[OID_S1]) &&
        (memcmp(objects.OID[OID_S1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_S1;
    }
    else if((objectOID_length == objects.length[OID_S2]) &&
        (memcmp(objects.OID[OID_S2],objectOID,objectOID_length) == 0))
    {
        *oid = OID_S2;
    }
    else if((objectOID_length == objects.length[OID_S_1]) &&
        (memcmp(objects.OID[OID_S_1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_S_1;
    }
    else if((objectOID_length == objects.length[OID_T1]) &&
        (memcmp(objects.OID[OID_T1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_T1;
    }
    else if((objectOID_length == objects.length[OID_T2]) &&
        (memcmp(objects.OID[OID_T2],objectOID,objectOID_length) == 0))
    {
        *oid = OID_T2;
    }
    else if((objectOID_length == objects.length[OID_DH_512_1]) &&
        (memcmp(objects.OID[OID_DH_512_1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_DH_512_1;
    }
    else if((objectOID_length == objects.length[OID_DH_512_2]) &&
        (memcmp(objects.OID[OID_DH_512_2],objectOID,objectOID_length) == 0))
    {
        *oid = OID_DH_512_2;
    }
    else if((objectOID_length == objects.length[OID_DH_1024_1]) &&
        (memcmp(objects.OID[OID_DH_1024_1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_DH_1024_1;
    }
    else if((objectOID_length == objects.length[OID_DH_1024_2]) &&
        (memcmp(objects.OID[OID_DH_1024_2],objectOID,objectOID_length) == 0))
    {
        *oid = OID_DH_1024_2;
    }
    else if((objectOID_length == objects.length[OID_DH_1536]) &&
        (memcmp(objects.OID[OID_DH_1536],objectOID,objectOID_length) == 0))
    {
        *oid = OID_DH_1536;
    }
    else {
        *oid = (objectOidType) -1;
        return RV_ERROR_UNKNOWN;
    }
    return RV_OK;
}


static RvStatus getAlgorithmOID(HPVT hVal,int nodeId,OUT objectOidType *oid)
{
    /********************************************/
    /* get   the algorithmOID                   */
    /********************************************/
    int objectNode;
    char objectOID[128];
    RvSize_t objectOID_length;

    objectNode = pvtGetChild(hVal, nodeId, __q931(algorithmOID), NULL);
    if (objectNode < 0)
        return RV_ERROR_UNKNOWN;
    objectOID_length = (RvSize_t)pvtGetString(hVal, objectNode, sizeof(objectOID), objectOID);

    /*******************************************************/
    /* get the algorithmOID as objectOidType               */
    /* note, objectOidType is just an enumeration          */
    /* the real value is a string                          */
    /*******************************************************/
    return (algorithmOidToOidType(objectOID, objectOID_length, oid));
}


RvStatus algorithmOidToOidType(IN char *objectOID,IN RvSize_t objectOID_length,OUT objectOidType *oid)
{
    /*******************************************************/
    /* get the algorithmOID as "U" or "V" or "W"           */
    /* note, "U" is just a shortage used in the standard   */
    /* the real value is a string                          */
    /*******************************************************/
    if ((objectOID_length == objects.length[OID_U1]) &&
        (memcmp(objects.OID[OID_U1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_U1;
    }
    else if((objectOID_length == objects.length[OID_U2]) &&
        (memcmp(objects.OID[OID_U2],objectOID,objectOID_length) == 0))
    {
        *oid = OID_U2;
    }
    else if((objectOID_length == objects.length[OID_V]) &&
        (memcmp(objects.OID[OID_V],objectOID,objectOID_length) == 0))
    {
        *oid = OID_V;
    }
    else if((objectOID_length == objects.length[OID_W]) &&
        (memcmp(objects.OID[OID_W],objectOID,objectOID_length) == 0))
    {
        *oid = OID_W;
    }
    else if((objectOID_length == objects.length[OID_W_1]) &&
        (memcmp(objects.OID[OID_W_1],objectOID,objectOID_length) == 0))
    {
        *oid = OID_W_1;
    }
    else if((objectOID_length == objects.length[OID_X]) &&
        (memcmp(objects.OID[OID_X],objectOID,objectOID_length) == 0))
    {
        *oid = OID_X;
    }
    else if((objectOID_length == objects.length[OID_Y]) &&
        (memcmp(objects.OID[OID_Y],objectOID,objectOID_length) == 0))
    {
        *oid = OID_Y;
    }
    else if((objectOID_length == objects.length[OID_Z]) &&
        (memcmp(objects.OID[OID_Z],objectOID,objectOID_length) == 0))
    {
        *oid = OID_Z;
    }
    else {
        *oid = (objectOidType) -1;
        return RV_ERROR_UNKNOWN;
    }
    return RV_OK;
}


h235DHMode_t identifyTokenDH(
    IN    HPVT                      hVal,
    IN    RvPvtNodeId               dhkeyNode,
    OUT   RvUint8                   halfkey[256],
    OUT   RvUint8                   modSize[256])
{
    RvBool bNonZero = RV_FALSE;
    int i, msLen;

    msLen = pvtGetBitString(hVal, pvtGetChild(hVal, dhkeyNode, __q931(modSize),   NULL), 256, (char*)modSize);

    for (i=0; (msLen>4) && (i<(msLen/4)); i+=4)
    {
        if (*((int*)(modSize+i)) != 0)
        {
            bNonZero = RV_TRUE;
            break;
        }
    }

    if (bNonZero)
    {
        /* there's something in there */
        int hkLen;
        hkLen = pvtGetBitString(hVal, pvtGetChild(hVal, dhkeyNode, __q931(halfkey),   NULL), 256, (char*)halfkey);

        /* see if it's something even remotely understandable */
        if (hkLen==1536 && msLen==1536)
            return h235ModeDiffieHellman1536;
        else if (hkLen==1024 && msLen==1024)
            return h235ModeDiffieHellman1024;
        else
            return h235ModeDiffieHellman512;
    }
    return h235ModeDiffieHellmanNone;
}


h235Mode_t identifyNode(
    HPVT hVal,
    RvPvtNodeId cryptoTokenNode,
    RvPvtNodeId* cryptoNode)
{
    __pvtGetNodeIdByFieldIds(*cryptoNode, hVal, cryptoTokenNode,
        {_q931(nestedcryptoToken) _q931(cryptoHashedToken) LAST_TOKEN});
    /* find by which procedure this message's security information was created */
    if (*cryptoNode >= 0)
    {
        /* may be procedure 1 */
        RvStatus rv0,rv1,rv2;
        objectOidType oid0,oid1,oid2;
        int hashedValsNode,tokenNode;

        pvtGetChild(hVal, *cryptoNode, __q931(hashedVals), &hashedValsNode);
        pvtGetChild(hVal, *cryptoNode, __q931(token), &tokenNode);

        rv0 = getTokenOID(hVal,*cryptoNode,&oid0);
        rv1 = getTokenOID(hVal,hashedValsNode,&oid1);
        rv2 = getAlgorithmOID(hVal,tokenNode,&oid2);

        if (rv0 == RV_OK && (oid0==OID_A1 || oid0==OID_A2 || oid0==OID_B1 || oid0==OID_B2) &&
            rv1 == RV_OK && (oid1==OID_T1 || oid1==OID_T2) &&
            rv2 == RV_OK && (oid2==OID_U1 || oid2==OID_U2))
        {
            RvUint8 buf[256];
            h235DHMode_t dhMode = h235ModeDiffieHellmanNone;
            RvPvtNodeId dhKeyNode = pvtGetChild(hVal, hashedValsNode, __q931(dhkey), NULL);

            if (RV_PVT_NODEID_IS_VALID(dhKeyNode))
                dhMode = identifyTokenDH(hVal, dhKeyNode, buf, buf);

            if (oid0==OID_A1 || oid0==OID_A2)
            {
                return (h235ModeProcedure1 | RV_H235_DH_MODE_TO_MODE(dhMode));
            }
            else
            {
                return (h235ModeProcedure1a | RV_H235_DH_MODE_TO_MODE(dhMode));
            }
        }
        else if (rv0 == RV_OK && (oid0==OID_DH_512_1 || oid0==OID_DH_512_2))
        {
            return h235ModeDHgroup512;
        }
        else if (rv0 == RV_OK && (oid0==OID_DH_1024_1 || oid0==OID_DH_1024_2))
        {
            return h235ModeDHgroup1024;
        }
        else if (rv0 == RV_OK && (oid0==OID_DH_1536 || oid0==OID_DH_1536))
        {
            return h235ModeDHgroup1536;
        }
        else if (rv0 == RV_OK && oid0==OID_Q)
        {
            return h235ModeProcedure4DH;
        }
        return h235ModeNone;
    }
    __pvtGetNodeIdByFieldIds(*cryptoNode, hVal, cryptoTokenNode,
        {_q931(nestedcryptoToken) _q931(cryptoSignedToken) LAST_TOKEN})
    if (*cryptoNode >= 0)
    {
        /* may be procedure 2 or 3 */
        RvStatus rv0,rv1,rv2;
        objectOidType oid0,oid1,oid2;
        int toBeSignedNode,tokenNode;

        pvtGetChild(hVal, *cryptoNode, __q931(token), &tokenNode);
        pvtGetChild(hVal, tokenNode, __q931(toBeSigned), &toBeSignedNode);

        rv0 = getTokenOID(hVal,*cryptoNode,&oid0);
        rv1 = getTokenOID(hVal,toBeSignedNode,&oid1);
        rv2 = getAlgorithmOID(hVal,tokenNode,&oid2);

        if (rv0 == RV_OK && (oid0==OID_A1 || oid0==OID_A2 || oid0==OID_B1 || oid0==OID_B2) &&
            rv1 == RV_OK && (oid1==OID_R1 || oid1==OID_R2 || oid1==OID_S1 || oid1==OID_S2) &&
            rv2 == RV_OK && (oid2==OID_V  || oid2==OID_W))
        {
            h235Mode_t mode;
            RvUint8 buf[256];
            h235DHMode_t dhMode = h235ModeDiffieHellmanNone;
            RvPvtNodeId dhKeyNode = pvtGetChild(hVal, toBeSignedNode, __q931(dhkey), NULL);

            if (oid0==OID_A1 || oid0==OID_A2)
            {
                mode = h235ModeProcedure2;
            }
            else
            {
                mode = h235ModeProcedure3;
            }

            if (RV_PVT_NODEID_IS_VALID(dhKeyNode))
                dhMode = identifyTokenDH(hVal, dhKeyNode, buf, buf);

            return (mode | RV_H235_DH_MODE_TO_MODE(dhMode));
        }
        else if (rv0 == RV_OK && (oid0==OID_A_1) &&
                 rv1 == RV_OK && (oid1==OID_S_1) &&
                 rv2 == RV_OK && (oid2==OID_W_1))
        {
            RvUint8 buf[256];
            h235DHMode_t dhMode = h235ModeDiffieHellmanNone;
            RvPvtNodeId dhKeyNode = pvtGetChild(hVal, toBeSignedNode, __q931(dhkey), NULL);

            if (RV_PVT_NODEID_IS_VALID(dhKeyNode))
                dhMode = identifyTokenDH(hVal, dhKeyNode, buf, buf);

            return (h235ModeProcedure4 | RV_H235_DH_MODE_TO_MODE(dhMode));
        }
        else if (rv0 == RV_OK && (oid0==OID_DH_512_1 || oid0==OID_DH_512_2))
        {
            return h235ModeDHgroup512;
        }
        else if (rv0 == RV_OK && (oid0==OID_DH_1024_1 || oid0==OID_DH_1024_2))
        {
            return h235ModeDHgroup1024;
        }
        else if (rv0 == RV_OK && (oid0==OID_DH_1536 || oid0==OID_DH_1536))
        {
            return h235ModeDHgroup1536;
        }
        else if (rv0 == RV_OK && oid0==OID_Q)
        {
            RvUint8 buf[256];
            h235DHMode_t dhMode = h235ModeDiffieHellmanNone;
            RvPvtNodeId dhKeyNode = pvtGetChild(hVal, toBeSignedNode, __q931(dhkey), NULL);

            if (RV_PVT_NODEID_IS_VALID(dhKeyNode))
                dhMode = identifyTokenDH(hVal, dhKeyNode, buf, buf);
            return (h235ModeProcedure4DH | RV_H235_DH_MODE_TO_MODE(dhMode));
        }
        return h235ModeNone;
    }
    __pvtGetNodeIdByFieldIds(*cryptoNode, hVal, cryptoTokenNode,
        {_q931(dhkey) LAST_TOKEN})
    if (*cryptoNode >= 0)
    {
        /* might be a clear-token */
        RvStatus rv0;
        objectOidType oid0;
/*        RvInt len = 0;

        pvtGetChildByFieldId(hVal, *cryptoNode, __q931(modSize), &len, NULL);
        if (len == 128)
        {
            *cryptoNode = cryptoTokenNode;
            return h235ModeDHgroup1024|h235ModeDhInClear;
        }*/

        *cryptoNode = cryptoTokenNode;
        rv0 = getTokenOID(hVal, *cryptoNode, &oid0);

        if (rv0 == RV_OK && (oid0==OID_DH_512_1 || oid0==OID_DH_512_2))
        {
            return h235ModeDHgroup512|h235ModeDhInClear;
        }
        if (rv0 == RV_OK && (oid0==OID_DH_1024_1 || oid0==OID_DH_1024_2))
        {
            return h235ModeDHgroup1024|h235ModeDhInClear;
        }
        if (rv0 == RV_OK && (oid0==OID_DH_1536 || oid0==OID_DH_1536))
        {
            return h235ModeDHgroup1536|h235ModeDhInClear;
        }
        return h235ModeNone;
    }
    return h235ModeNone;
}


RvStatus checkAuthentication(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  RvUint8            *messageBuf,
    IN  int                 messageLength,
    IN  int                 cryptoTokenNode,
    IN  HPROTCONN           protConn,
    IN  cmRASTransport     *srcAddress,
    OUT h235ContextInfo_t  *contextInfo)
{
    HPVT hVal = h235App->hVal;
    RvStatus status = RV_OK;
    h235Mode_t tokenType;
    RvUint8 hashedVal[12], signedVal[128], hashedVal4[12], signedVal4[128];
    h235DHMode_t p4dhMode = h235ModeDiffieHellmanNone;
    h235Mode_t toDoList = h235ModeNone;
    entityId_t lastSendersID;

    RvPvtNodeId cryptoNode=-1, p1Node=-1, p1aNode[4]={-1,-1,-1,-1}, p2Node=-1, p4Node=-1, p4dhNode=-1, p3Node[4]={-1,-1,-1,-1};
    int p1aIndex=0, p3Index=0;
    int hashedInd=-1, signInd=-1, hashedInd4=-1, signInd4=-1;

    contextInfo->secureMode = h235ModeNone;
    while (cryptoTokenNode >= 0)
    {
        tokenType = identifyNode(hVal, cryptoTokenNode, &cryptoNode);

        if (tokenType == h235ModeNone)
        {
            /* we did not recognize this node. let's hope the application knows how to handle it */
            if (h235App->eventHandlers.evReceivedToken != NULL)
            {
                h235App->eventHandlers.evReceivedToken(h235App->hApp, (H235HAPP)h235App,
                    NULL, NULL, NULL, (int)tokenType, cryptoTokenNode);
            }
        }

        if (tokenType & h235ModeProcedure1)
        {
            RvPvtNodeId hashNode;

            /* get the hashed value, and blank it in the message */
            __pvtGetNodeIdByFieldIds(hashNode, hVal, cryptoNode, {_q931(token) _q931(hash) LAST_TOKEN});
            pvtGetString(hVal, hashNode, sizeof(hashedVal), (char*)hashedVal);

            /* find the hash value in the encoded message and replace it with zeros */
            hashedInd = findSubstring((unsigned char*)messageBuf, messageLength, 0, hashedVal, 12);
            if (hashedInd < 0)
                return RV_ERROR_UNKNOWN;
            memset(messageBuf+hashedInd, 0, 12);

            /* save the node */
            p1Node = cryptoNode;
        }
        if (tokenType & h235ModeProcedure1a)
        {
            if (p1aIndex < 4)
            {
                p1aNode[p1aIndex] = cryptoNode;
                p1aIndex++;
            }
        }
        if (tokenType & h235ModeProcedure2)
        {
            RvPvtNodeId signNode;

            /* get the signature value, and blank it in the message */
            __pvtGetNodeIdByFieldIds(signNode, hVal, cryptoNode, {_q931(token) _q931(signature) LAST_TOKEN});
            pvtGetString(hVal, signNode, sizeof(signedVal), (char*)signedVal);

            /* find the hash value in the encoded message and replace it with zeros */
            signInd = findSubstring((unsigned char*)messageBuf, messageLength, 0, signedVal, 128);
            if (signInd < 0)
                return RV_ERROR_UNKNOWN;
            memset(messageBuf+signInd, 0, 128);

            /* save the node */
            p2Node = cryptoNode;
        }
        if (tokenType & h235ModeProcedure3)
        {
            if (p3Index < 4)
            {
                p3Node[p3Index] = cryptoNode;
                p3Index++;
            }
        }
        if (tokenType & h235ModeProcedure4)
        {
            RvPvtNodeId signNode;
            RvPvtNodeId hashNode;

            /* get the signature value, and blank it in the message */
            __pvtGetNodeIdByFieldIds(signNode, hVal, cryptoNode, {_q931(token) _q931(signature) LAST_TOKEN});
            if (RV_PVT_NODEID_IS_VALID(signNode))
            {
                pvtGetString(hVal, signNode, sizeof(signedVal4), (char*)signedVal4);

                /* find the hash value in the encoded message and replace it with zeros */
                signInd4 = findSubstring((unsigned char*)messageBuf, messageLength, 0, signedVal4, 128);
                if (signInd4 < 0)
                    return RV_ERROR_UNKNOWN;
                memset(messageBuf+signInd4, 0, 128);
            }

            /* get the hashed value, and blank it in the message */
            __pvtGetNodeIdByFieldIds(hashNode, hVal, cryptoNode, {_q931(token) _q931(hash) LAST_TOKEN});
            if (RV_PVT_NODEID_IS_VALID(hashNode))
            {
                pvtGetString(hVal, hashNode, sizeof(hashedVal4), (char*)hashedVal4);

                /* find the hash value in the encoded message and replace it with zeros */
                hashedInd4 = findSubstring((unsigned char*)messageBuf, messageLength, 0, hashedVal4, 12);
                if (hashedInd4 < 0)
                    return RV_ERROR_UNKNOWN;
                memset(messageBuf+hashedInd4, 0, 12);
            }

            /* save the node */
            p4Node = cryptoNode;
        }
        if (tokenType & h235ModeProcedure4DH)
        {
            p4dhNode = cryptoNode;
            p4dhMode = RV_H235_MODE_TO_DH_MODE(tokenType);
            /* do not add this token to the secure mode */
            tokenType = h235ModeNone;
        }

        contextInfo->secureMode |= tokenType;

        cryptoNode = -1;
        cryptoTokenNode = pvtBrother(hVal, cryptoTokenNode);
    }
    toDoList = contextInfo->secureMode;

    /* first, we want to handle the Procedure 4 Diffie-hellman, it might move us into procedure 1 */
    if (RV_PVT_NODEID_IS_VALID(p4dhNode))
    {
        h235EntityEntry_t* entityEntryPtr;
        RvPvtNodeId clearTokenNode, sendersIdNode;
        entityId_t sendersID;

        /* must get the DH halfkey before checking the authentication, because the
        authentication may be already using the shared secret */
        /* must first get the endpoint Id */
        __pvtGetNodeIdByFieldIds(clearTokenNode, hVal, p4dhNode, {_q931(token) _q931(toBeSigned) LAST_TOKEN});
        if (!RV_PVT_NODEID_IS_VALID(clearTokenNode))
            clearTokenNode = pvtGetChild(hVal, p4dhNode, __q931(hashedVals), NULL);

        sendersIdNode = pvtGetChild(hVal, clearTokenNode, __q931(sendersID), NULL);
        status = checkSenderId(h235App, messageRoot, sendersIdNode, contextInfo, &sendersID, &entityEntryPtr, RV_FALSE);

        if (status == RV_OK)
        {
            getDHTokenNode(hVal, RV_PVT_INVALID_NODEID, clearTokenNode, &entityEntryPtr->inDHinf);
            entityEntryPtr->inDHinf.DHMode = p4dhMode;

            if (entityEntryPtr->inDHinf.isValid)
            {
                /* ToDo: inform the application that it should generate the shared secret at it's leasure */
                /* ForNow: calculate the DH right here */
                if (entityEntryPtr->outDHinf.isValid ||
                    (h235GenerateDiffieHellmanEP((H235HAPP)h235App, h235ModeDiffieHellman1024,
                        (H235HENTITY)entityEntryPtr) == RV_OK))
                {
					//注意 变更20111205: newKey 20改为21 否则Annex F IV使用时会出问题
                    RvUint8 newKey[21];

                    status = h235GenarateDiffieHellmanSharedSecretEP((H235HENTITY)entityEntryPtr,
                        RV_TRUE, (char *)newKey);

                    if ((status == RV_OK) && (h235App->eventHandlers.evSetKeyHandle != NULL))
                    {
                        h235App->eventHandlers.evSetKeyHandle(h235App->hApp, (H235HAPP)h235App,
                            newKey, &entityEntryPtr->keyHandle);
                        if (!h235App->isGK)
                        {
                            h235App->secureMode = (h235Mode_t)((int)h235App->secureMode & ~(int)h235ModeProcedure4);
                            h235App->secureMode = (h235Mode_t)((int)h235App->secureMode | (int)h235ModeProcedure1);
                        }
                    }
                }
            }
        }
        /* ignore this status, for no real reason */
        status = RV_OK;
    }
    /* now we must choose which one we want to use */
    if ((status == RV_OK) && (contextInfo->secureMode & h235ModeProcedure1))
    {
        status = checkAuthenticationProc1(h235App, messageRoot, messageBuf, messageLength,
            p1Node, hashedVal, contextInfo, &lastSendersID);
    }
    while ((status == RV_OK) && (contextInfo->secureMode & h235ModeProcedure1a) && (p1aIndex > 0))
    {
        p1aIndex--;
        status = checkAuthenticationProc1a(h235App, messageRoot,
            p1aNode[p1aIndex], contextInfo, &lastSendersID);
    }
    if ((status == RV_OK) && (contextInfo->secureMode & h235ModeProcedure2))
    {
        status = checkAuthenticationProc2(h235App, messageRoot, messageBuf, messageLength,
            p2Node, signedVal, contextInfo, &lastSendersID);
    }
    while ((status == RV_OK) && (contextInfo->secureMode & h235ModeProcedure3) && (p3Index > 0))
    {
        p3Index--;
        status = checkAuthenticationProc3(h235App, messageRoot,
            p3Node[p3Index], contextInfo, &lastSendersID);
    }
    if ((status == RV_OK) && (contextInfo->secureMode & h235ModeProcedure4))
    {
        status = checkAuthenticationProc4(h235App, messageRoot, messageBuf, messageLength,
            p4Node, signedVal4, hashedVal4, contextInfo, &lastSendersID);
    }

    /* BUFFER ROLLBACK - I must undo all the changes I made to the buffer above */
    if (toDoList & h235ModeProcedure1)
    {
        memcpy(messageBuf+hashedInd, hashedVal, 12);
    }
    if (toDoList & h235ModeProcedure2)
    {
        memcpy(messageBuf+signInd, signedVal, 128);
    }
    if (toDoList & h235ModeProcedure4)
    {
        if (hashedInd4 > 0)
            memcpy(messageBuf+hashedInd4, hashedVal4, 12);
        if (signInd4 > 0)
            memcpy(messageBuf+signInd4, signedVal4, 128);
    }

    /* see if we should now freeze the message */
    if (status == RV_ERROR_UNINITIALIZED)
    {
        /* it's time to freeze the message */
        return freezeRasMessage(h235App, protConn, messageRoot, &lastSendersID, messageLength, messageBuf,
            srcAddress, contextInfo);
    }

    /* went through all the crypto-nodes, and asked the application about the ones we recognized.
    make sure the application actually supported one of the ones we recognized */
    if ((status == RV_OK) && ((contextInfo->secureMode & RV_H235_ALL_SEC_MODES) == h235ModeNone))
    {
        h235AppFailReply_t appReply = failToApp(h235App, messageRoot, NULL,
            h235ReasonProcNotSupported, contextInfo);

        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else return RV_ERROR_UNKNOWN;
    }

    return status;
}


RvStatus checkGeneralId(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  RvPvtNodeId         generalIdNode,
    IN  h235ContextInfo_t*  contextInfo,
    IN  RvBool              bEndToEndToken)
{
    entityId_t generalID;
    h235AppFailReply_t appReply;

    if (!RV_PVT_NODEID_IS_VALID(generalIdNode))
    {
        appReply = failToApp(h235App, messageRoot, NULL, h235ReasonGeneralIDIncorrect, contextInfo);

        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else
            return RV_ERROR_UNKNOWN;
    }

    memset(&generalID,0,sizeof(entityId_t));

    /* get the general ID */
    generalID.length = pvtGetString(h235App->hVal, generalIdNode,  sizeof(generalID.data), (char*)generalID.data);

    /* check if general ID == out ID */
    if ((generalID.length != h235App->senderId.length) ||
        memcmp(generalID.data, h235App->senderId.data, (RvSize_t)generalID.length))
    {
        if (bEndToEndToken && h235App->isGK)
            /* we are probably not the last stop, return */
            return RV_ERROR_NOTSUPPORTED;

        /* notify application that general ID is incorrect */
        appReply = failToApp(h235App, messageRoot, NULL, h235ReasonGeneralIDIncorrect, contextInfo);
        if (appReply != h235FailReplyForceOk)
            return RV_ERROR_UNKNOWN;
    }

    return RV_OK;
}


RvStatus checkSenderId(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  RvPvtNodeId         senderIdNode,
    IN  h235ContextInfo_t*  contextInfo,
    OUT entityId_t*         sendersID,
    OUT h235EntityEntry_t** entityEntryPtr,
    IN  RvBool              bEndToEndToken)
{
    h235AppFailReply_t appReply;

    /* get the sender id from the message */
    memset(sendersID, 0, sizeof(entityId_t));

    if (!RV_PVT_NODEID_IS_VALID(senderIdNode))
    {
        appReply = failToApp(h235App, messageRoot, NULL, h235ReasonSendersIDMissing, contextInfo);

        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else
            return RV_ERROR_UNKNOWN;
    }

    sendersID->length = pvtGetString(h235App->hVal, senderIdNode, sizeof(sendersID->data), (char*)sendersID->data);

    /* If this is endpoint, compare saved GK id with sendersId from the message */
    if (!h235App->isGK)
    {
        if ((sendersID->length != h235App->gkEntry.id.length) ||
            memcmp(sendersID->data, h235App->gkEntry.id.data, (size_t)sendersID->length))
        {
            if (bEndToEndToken)
                return RV_ERROR_NOTSUPPORTED;

            appReply = failToApp(h235App, messageRoot, NULL, h235ReasonUnknownSender,contextInfo);

            if (appReply != h235FailReplyForceOk)
                return RV_ERROR_UNKNOWN;
        }
    }

    /* Get the entry structure */
    if (getEntityEntry(h235App, sendersID, entityEntryPtr, contextInfo->secureMode) != RV_OK)
    {
        /* this can happen only when this application is a gatekeeper */
        appReply = failToApp(h235App, messageRoot, NULL, h235ReasonUnknownSender, contextInfo);

        if ((appReply == h235FailReplyFreezeMessage) && h235App->isGK)
            return RV_ERROR_UNINITIALIZED;

        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else
            return RV_ERROR_UNKNOWN;
    }

    if (!h235App->isGK)
    {
        (*entityEntryPtr)->secureMode = (contextInfo->secureMode & h235App->secureMode);
    }

    return RV_OK;
}


RvStatus checkAuthenticationProc1(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  RvUint8            *messageBuf,
    IN  int                 messageLength,
    IN  int                 cryptoHashedTokenNode,
    IN  RvUint8             hashedVal[12],
    OUT h235ContextInfo_t  *contextInfo,
    OUT entityId_t         *sendersID)
{
    HPVT hVal = h235App->hVal;
    h235AppSuccessReply_t appReply;
    h235EntityEntry_t *entityEntryPtr = NULL;
    h235TimeInfo_t timeInfo = {0,0};
    RvPvtNodeId hashedValsNode, generalIdNode, sendersIdNode;
    RvStatus rv;

    /* get generalID and compare with my id */
    hashedValsNode = pvtGetChild(hVal, cryptoHashedTokenNode, __q931(hashedVals), NULL);
    generalIdNode = pvtGetChild(hVal, hashedValsNode, __q931(generalID), NULL);

    rv= checkGeneralId(h235App, messageRoot, generalIdNode, contextInfo, RV_FALSE);
    if(rv != RV_OK)
        return rv;

    /* get senderID and entity entry pointer */
    sendersIdNode = pvtGetChild(hVal, hashedValsNode, __q931(sendersID), NULL);

    rv= checkSenderId(h235App, messageRoot, sendersIdNode, contextInfo, sendersID, &entityEntryPtr, RV_FALSE);
    if (rv != RV_OK)
        return rv;

    /* see if we want to check this node */
    contextInfo->secureMode = entityEntryPtr->secureMode;
    if (!(entityEntryPtr->secureMode & h235ModeProcedure1))
    {
        /* application decided not to support this procedure for this EP */
        return RV_OK;
    }

    /* check for time synchronization if the performTimeSync is true */
    if (h235App->performTimeSync)
    {
        rv= verifyTimeSync(h235App, messageRoot, sendersID, hashedValsNode, entityEntryPtr,
            &timeInfo, contextInfo);
        if(rv != RV_OK)
            return rv;
    }

    /* Call the authentication Procedure */
    rv = checkByProcedure1(h235App, messageRoot, messageBuf, messageLength, cryptoHashedTokenNode,
        entityEntryPtr, hashedVal, contextInfo);
    if(rv != RV_OK)
        return rv;

    /* update lastRandom and last time only if message passed security */
    if (timeInfo.random)
    {
        entityEntryPtr->lastRandom = timeInfo.random;
        entityEntryPtr->lastTime   = timeInfo.time;
    }

    /* Notify the application about authentication success */
    {
        h235Mode_t tmpMode = contextInfo->secureMode;
        contextInfo->secureMode = h235ModeProcedure1;
        appReply = successToApp(h235App, messageRoot, sendersID, h235ReasonUndefined, contextInfo);
        contextInfo->secureMode = tmpMode;
        if (appReply == h235SuccessReplyContinue)
            return RV_OK;
    }
    return RV_ERROR_UNKNOWN;
}


RvStatus checkAuthenticationProc1a(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  int                 cryptoHashedTokenNode,
    OUT h235ContextInfo_t  *contextInfo,
    OUT entityId_t         *sendersID)
{
    HPVT hVal = h235App->hVal;
    h235AppSuccessReply_t appReply;
    h235EntityEntry_t *entityEntryPtr = NULL;
    h235TimeInfo_t timeInfo = {0,0};
    RvPvtNodeId hashedValsNode, generalIdNode, sendersIdNode;
    RvStatus rv;

    /* get generalID and compare with my id */
    hashedValsNode = pvtGetChild(hVal, cryptoHashedTokenNode, __q931(hashedVals), NULL);
    generalIdNode = pvtGetChild(hVal, hashedValsNode, __q931(generalID), NULL);

    rv= checkGeneralId(h235App, messageRoot, generalIdNode, contextInfo, RV_FALSE);
    if(rv != RV_OK)
        return rv;

    /* get senderID and entity entry pointer */
    sendersIdNode = pvtGetChild(hVal, hashedValsNode, __q931(sendersID), NULL);

    rv= checkSenderId(h235App, messageRoot, sendersIdNode, contextInfo, sendersID, &entityEntryPtr, RV_FALSE);
    if (rv != RV_OK)
        return rv;

    /* see if we want to check this node */
    contextInfo->secureMode = entityEntryPtr->secureMode;
    if (!(entityEntryPtr->secureMode & h235ModeProcedure1a))
    {
        /* application decided not to support this procedure for this EP */
        return RV_OK;
    }

    /* check for time synchronization if the performTimeSync is true */
    if (h235App->performTimeSync)
    {
        rv= verifyTimeSync(h235App, messageRoot, sendersID, hashedValsNode, entityEntryPtr,
            &timeInfo, contextInfo);
        if(rv != RV_OK)
            return rv;
    }

    /* Call the authentication Procedure */
    rv = checkByProcedure1a(h235App, messageRoot, cryptoHashedTokenNode,
        entityEntryPtr, contextInfo);
    if(rv != RV_OK)
        return rv;

    /* update lastRandom and last time only if message passed security */
    if (timeInfo.random)
    {
        entityEntryPtr->lastRandom = timeInfo.random;
        entityEntryPtr->lastTime   = timeInfo.time;
    }

    /* Notify the application about authentication success */
    {
        h235Mode_t tmpMode = contextInfo->secureMode;
        contextInfo->secureMode = h235ModeProcedure1a;
        appReply = successToApp(h235App, messageRoot, sendersID, h235ReasonUndefined, contextInfo);
        contextInfo->secureMode = tmpMode;
        if (appReply == h235SuccessReplyContinue)
            return RV_OK;
    }
    return RV_ERROR_UNKNOWN;
}


RvStatus checkAuthenticationProc2(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  RvUint8            *messageBuf,
    IN  int                 messageLength,
    IN  int                 cryptoSignedTokenNode,
    IN  RvUint8             signedVal[128],
    OUT h235ContextInfo_t  *contextInfo,
    OUT entityId_t         *sendersID)
{
    HPVT hVal = h235App->hVal;
    h235AppSuccessReply_t appReply;
    h235EntityEntry_t *entityEntryPtr = NULL;
    h235TimeInfo_t timeInfo = {0,0};
    RvPvtNodeId toBeSignedNode, generalIdNode, sendersIdNode;
    RvStatus rv;

    /* get generalID and compare with my id */
    __pvtGetNodeIdByFieldIds(toBeSignedNode, hVal, cryptoSignedTokenNode,
        {_q931(token) _q931(toBeSigned) LAST_TOKEN});
    generalIdNode = pvtGetChild(hVal, toBeSignedNode, __q931(generalID), NULL);

    rv= checkGeneralId(h235App, messageRoot, generalIdNode, contextInfo, RV_FALSE);
    if(rv != RV_OK)
        return rv;

    /* get senderID and entity entry pointer */
    sendersIdNode = pvtGetChild(hVal, toBeSignedNode, __q931(sendersID), NULL);

    rv= checkSenderId(h235App, messageRoot, sendersIdNode, contextInfo, sendersID, &entityEntryPtr, RV_FALSE);
    if (rv != RV_OK)
        return rv;

    /* see if we want to check this node */
    contextInfo->secureMode = entityEntryPtr->secureMode;
    if (!(entityEntryPtr->secureMode & h235ModeProcedure2))
    {
        /* application decided not to support this procedure for this EP */
        return RV_OK;
    }

    /* check for time synchronization if the performTimeSync is true */
    if (h235App->performTimeSync)
    {
        rv= verifyTimeSync(h235App, messageRoot, sendersID, toBeSignedNode, entityEntryPtr,
            &timeInfo, contextInfo);
        if(rv != RV_OK)
            return rv;
    }

    /* Call the authentication Procedure */
    rv = checkByProcedure2(h235App, messageRoot, messageBuf, messageLength, cryptoSignedTokenNode,
        entityEntryPtr, signedVal, contextInfo);
    if(rv != RV_OK)
        return rv;

    /* update lastRandom and last time only if message passed security */
    if (timeInfo.random)
    {
        entityEntryPtr->lastRandom = timeInfo.random;
        entityEntryPtr->lastTime   = timeInfo.time;
    }

    /* Notify the application about authentication success */
    {
        h235Mode_t tmpMode = contextInfo->secureMode;
        contextInfo->secureMode = h235ModeProcedure2;
        appReply = successToApp(h235App, messageRoot, sendersID, h235ReasonUndefined, contextInfo);
        contextInfo->secureMode = tmpMode;
        if (appReply == h235SuccessReplyContinue)
            return RV_OK;
    }
    return RV_ERROR_UNKNOWN;
}


RvStatus checkAuthenticationProc3(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  int                 cryptoSignedTokenNode,
    OUT h235ContextInfo_t  *contextInfo,
    OUT entityId_t         *sendersID)
{
    HPVT hVal = h235App->hVal;
    h235EntityEntry_t *entityEntryPtr = NULL;
    h235TimeInfo_t timeInfo = {0,0};
    RvPvtNodeId toBeSignedNode, generalIdNode, sendersIdNode;
    H235HKEY crtInHandle = NULL;
    RvStatus rv;

    /* get generalID and compare with my id */
    __pvtGetNodeIdByFieldIds(toBeSignedNode, hVal, cryptoSignedTokenNode,
        {_q931(token) _q931(toBeSigned) LAST_TOKEN});
    generalIdNode = pvtGetChild(hVal, toBeSignedNode, __q931(generalID), NULL);

    rv= checkGeneralId(h235App, messageRoot, generalIdNode, contextInfo, RV_TRUE);
    if (rv == RV_ERROR_NOTSUPPORTED)
        /* end to end token, but not for us */
        return RV_OK;
    if (rv != RV_OK)
        return rv;

    /* get senderID and entity entry pointer */
    sendersIdNode = pvtGetChild(hVal, toBeSignedNode, __q931(sendersID), NULL);

    rv= checkSenderId(h235App, messageRoot, sendersIdNode, contextInfo, sendersID, &entityEntryPtr, RV_TRUE);
    if ((rv == RV_ERROR_NOTSUPPORTED) && (h235App->eventHandlers.evGetCrtHandleById != NULL))
    {
        /* check if we can get this from the application */
        if (h235App->eventHandlers.evGetCrtHandleById(h235App->hApp, (H235HAPP)h235App,
            sendersID, &crtInHandle, NULL))
        {
            rv = RV_OK;
        }
        else
        {
            h235AppFailReply_t failReply;
            /* application does not have this certificate. maybe we could freeze the message */
            failReply = failToApp(h235App, messageRoot, NULL, h235ReasonEndToEndTokenSenderUnknown, contextInfo);

            if (failReply == h235FailReplyFreezeMessage)
                return RV_ERROR_UNINITIALIZED;
            if (failReply == h235FailReplyForceOk)
                return RV_OK;
        }
    }
    else if (rv != RV_OK)
        return rv;

    /* see if we want to check this node */
    if (entityEntryPtr)
    {
        contextInfo->secureMode = entityEntryPtr->secureMode;
        if (!(entityEntryPtr->secureMode & h235ModeProcedure3))
        {
            /* application decided not to support this procedure for this EP */
            return RV_OK;
        }
        crtInHandle = entityEntryPtr->crtInHandle;
    }
    else
    {
        if (!(h235App->secureMode & h235ModeProcedure3))
        {
            /* application decided not to support this procedure for this EP */
            return RV_OK;
        }
    }

    /* check for time synchronization if the performTimeSync is true */
    if (h235App->performTimeSync)
    {
        rv= verifyTimeSync(h235App, messageRoot, sendersID, toBeSignedNode, entityEntryPtr,
            &timeInfo, contextInfo);
        if(rv != RV_OK)
            return rv;
    }

    /* Call the authentication Procedure */
    rv = checkByProcedure3(h235App, messageRoot, cryptoSignedTokenNode, sendersID, crtInHandle, contextInfo);
    if(rv != RV_OK)
        return rv;

    /* update lastRandom and last time only if message passed security */
    if ((entityEntryPtr != NULL) && (timeInfo.random))
    {
        entityEntryPtr->lastRandom = timeInfo.random;
        entityEntryPtr->lastTime   = timeInfo.time;
    }

    /* Notify the application about authentication success */
    {
        h235AppSuccessReply_t appReply;
        h235Mode_t tmpMode = contextInfo->secureMode;
        contextInfo->secureMode = h235ModeProcedure3;
        appReply = successToApp(h235App, messageRoot, sendersID, h235ReasonUndefined, contextInfo);
        contextInfo->secureMode = tmpMode;
        if (appReply == h235SuccessReplyContinue)
            return RV_OK;
    }
    return RV_ERROR_UNKNOWN;
}


RvStatus checkAuthenticationProc4(
    IN  h235Element_T      *h235App,
    IN  int                 messageRoot,
    IN  RvUint8            *messageBuf,
    IN  int                 messageLength,
    IN  int                 cryptoTokenNode,
    IN  RvUint8             signedVal[128],
    IN  RvUint8             hashedVal[12],
    OUT h235ContextInfo_t  *contextInfo,
    OUT entityId_t         *sendersID)
{
    HPVT hVal = h235App->hVal;
    h235AppSuccessReply_t appReply;
    h235EntityEntry_t *entityEntryPtr = NULL;
    h235TimeInfo_t timeInfo = {0,0};
    RvPvtNodeId clearTokenNode, generalIdNode, sendersIdNode;
    RvStatus rv;
    RvBool bAfterDH = RV_FALSE;

    /* get generalID and compare with my id */
    __pvtGetNodeIdByFieldIds(clearTokenNode, hVal, cryptoTokenNode, {_q931(token) _q931(toBeSigned) LAST_TOKEN});
    if (!RV_PVT_NODEID_IS_VALID(clearTokenNode))
    {
        clearTokenNode = pvtGetChild(hVal, cryptoTokenNode, __q931(hashedVals), NULL);
        bAfterDH = RV_TRUE;
    }

    generalIdNode = pvtGetChild(hVal, clearTokenNode, __q931(generalID), NULL);
    rv= checkGeneralId(h235App, messageRoot, generalIdNode, contextInfo, RV_FALSE);
    if(rv != RV_OK)
        return rv;

    /* get senderID and entity entry pointer */
    sendersIdNode = pvtGetChild(hVal, clearTokenNode, __q931(sendersID), NULL);

    rv= checkSenderId(h235App, messageRoot, sendersIdNode, contextInfo, sendersID, &entityEntryPtr, RV_FALSE);
    if (rv != RV_OK)
        return rv;

    /* see if we want to check this node */
    contextInfo->secureMode = entityEntryPtr->secureMode;
    if (!(entityEntryPtr->secureMode & h235ModeProcedure4))
    {
        /* application decided not to support this procedure for this EP */
        return RV_OK;
    }

    /* check for time synchronization if the performTimeSync is true */
    if (h235App->performTimeSync)
    {
        rv= verifyTimeSync(h235App, messageRoot, sendersID, clearTokenNode, entityEntryPtr,
            &timeInfo, contextInfo);
        if(rv != RV_OK)
            return rv;
    }

    /* Call the authentication Procedure */
    if (bAfterDH)
    {
        rv = checkByProcedure1(h235App, messageRoot, messageBuf, messageLength, cryptoTokenNode,
            entityEntryPtr, hashedVal, contextInfo);
    }
    else
    {
        rv = checkByProcedure2(h235App, messageRoot, messageBuf, messageLength, cryptoTokenNode,
            entityEntryPtr, signedVal, contextInfo);
    }
    if(rv != RV_OK)
        return rv;

    /* update lastRandom and last time only if message passed security */
    if (timeInfo.random)
    {
        entityEntryPtr->lastRandom = timeInfo.random;
        entityEntryPtr->lastTime   = timeInfo.time;
    }

    /* Notify the application about authentication success */
    {
        h235Mode_t tmpMode = contextInfo->secureMode;
        contextInfo->secureMode = h235ModeProcedure4;
        appReply = successToApp(h235App, messageRoot, sendersID, h235ReasonUndefined, contextInfo);
        contextInfo->secureMode = tmpMode;
        if (appReply == h235SuccessReplyContinue)
            return RV_OK;
    }
    return RV_ERROR_UNKNOWN;
}

RvStatus verifyTimeSync(
    IN  h235Element_T* h235App,
    IN  int         messageRoot,
    IN  entityId_t  *sendersID,
    IN  int         tokenNode,
    IN  h235EntityEntry_t *entityEntry,
    OUT h235TimeInfo_t *timeInfo,
    OUT h235ContextInfo_t   *contextInfo)
{
    HPVT hVal = h235App->hVal;
    RvPvtNodeId timeStampNode,randomNode;
    int  timeDiff;
    RvUint32 myTime = (RvUint32)RvClockGet(NULL, NULL);
    h235AppFailReply_t appReply;

    /*getting the time stamp and random*/
    timeStampNode = pvtGetChildByFieldId(hVal, tokenNode, __q931(timeStamp), (RvInt32*)&(timeInfo->time), NULL);
    randomNode = pvtGetChildByFieldId(hVal, tokenNode, __q931(random), &(timeInfo->random), NULL);

    if((timeStampNode < 0) || (randomNode < 0))
    {
        appReply = failToApp(h235App, messageRoot, sendersID, h235ReasonTimeStampMissing, contextInfo);

        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else
            return RV_ERROR_UNKNOWN;
    }

    timeDiff = myTime - timeInfo->time;
    if (timeDiff < 0)
        timeDiff = -timeDiff;
    if(timeDiff > h235App->timeSyncDiff)
    {
        appReply = failToApp(h235App, messageRoot, sendersID, h235ReasonTimeStampFail, contextInfo);
        if (appReply != h235FailReplyForceOk)
                return RV_ERROR_UNKNOWN;

    }

    /* When time received is equal to time received in previous message, check random to see it
    is higher than previous random - 4 (-4 because messages arrival order might be different from
    arrival order, but it must not be the same as previous random. This is to encounter replay
    attacks */
    if (entityEntry && (timeInfo->time == entityEntry->lastTime))
    {
        if (timeInfo->random < (entityEntry->lastRandom-4) || timeInfo->random == entityEntry->lastRandom)
        {
            appReply = failToApp(h235App, messageRoot, sendersID, h235ReasonTimeRandomFail, contextInfo);
            if (appReply == h235FailReplyForceOk)
                return RV_OK;
            else
                return RV_ERROR_UNKNOWN;
        }
    }
    return RV_OK;
}


RvStatus checkByProcedure1(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  RvUint8*            messageBuf,
    IN  int                 messageLength,
    IN  int                 cryptoHashedTokenNode,
    IN  h235EntityEntry_t*  entityEntry,
    IN  RvUint8             hashedVal[12],
    OUT h235ContextInfo_t*  contextInfo)
{
    unsigned char calculatedHash[20];
    h235AppFailReply_t appReply;
    int len;

    RV_UNUSED_ARG(cryptoHashedTokenNode);
    /* calculating the hash value on the fixed encoded message */
    if (h235App->evSHA1_96 == NULL)
    {
        appReply = failToApp(h235App, messageRoot, &entityEntry->id, h235ReasonEncodeCBMissing, contextInfo);
        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else return RV_ERROR_UNKNOWN;
    }
    h235App->evSHA1_96(entityEntry->keyHandle, (char*)messageBuf, messageLength, calculatedHash, &len);

    /* comparing the original hash value with the calculated hash value */
    if (memcmp(calculatedHash, hashedVal, 12))
    {
        appReply = failToApp(h235App, messageRoot, &entityEntry->id, h235ReasonIncorrectAuth, contextInfo);
        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else return RV_ERROR_UNKNOWN;
    }
    return RV_OK;
}


RvStatus checkByProcedure1a(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  int                 cryptoHashedTokenNode,
    IN  h235EntityEntry_t*  entityEntry,
    OUT h235ContextInfo_t*  contextInfo)
{
    HPVT hVal = h235App->hVal;
    unsigned char calculatedHash[20], hashedVal[20], tokenBuf[512];
    RvPvtNodeId hashedValsNode, tmpNode;
    h235AppFailReply_t appReply;
    int len=0, bufLen=0;
    RvStatus res;

    RV_UNUSED_ARG(cryptoHashedTokenNode);
    /* calculating the hash value on the fixed encoded message */
    if (h235App->evSHA1_96 == NULL)
    {
        appReply = failToApp(h235App, messageRoot, &entityEntry->id, h235ReasonEncodeCBMissing, contextInfo);
        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else return RV_ERROR_UNKNOWN;
    }

    hashedValsNode = pvtGetChild(hVal, cryptoHashedTokenNode, __q931(hashedVals), NULL);
    res = cmEmEncode(hVal, hashedValsNode, (unsigned char *)tokenBuf, 512, &bufLen);
    if (res < 0)
        return res;

    h235App->evSHA1_96(entityEntry->keyHandle, (char*)tokenBuf, bufLen, calculatedHash, &len);

    __pvtGetNodeIdByFieldIds(tmpNode, hVal, cryptoHashedTokenNode, {_q931(token) _q931(hash) LAST_TOKEN});
    pvtGetString(hVal, tmpNode, sizeof(hashedVal), (char*)hashedVal);

    /* comparing the original hash value with the calculated hash value */
    if (memcmp(calculatedHash, hashedVal, 12))
    {
        appReply = failToApp(h235App, messageRoot, &entityEntry->id, h235ReasonIncorrectAuth, contextInfo);
        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else return RV_ERROR_UNKNOWN;
    }
    return RV_OK;
}


RvStatus checkByProcedure2(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  RvUint8*            buf,
    IN  int                 bufLen,
    IN  int                 cryptoSignedTokenNode,
    IN  h235EntityEntry_t*  entityEntry,
    IN  RvUint8             signedVal[128],
    OUT h235ContextInfo_t*  contextInfo)
{
    HPVT hVal = h235App->hVal;
    RvPvtNodeId tokenNode, certificateNode;
    int len=128, oidLen;
    RvInt32 size = 0;
    unsigned char oidStr[128], certificate[512];
    RvUint8 * crtPtr;
    h235CertificateType_t type = (h235CertificateType_t)0;
    objectOidType oid;
    h235AppFailReply_t appReply;
    RvStatus res;
    RvBool foundCrt;

    /* get the certificate type */
    tokenNode = pvtGetChild(hVal, cryptoSignedTokenNode, __q931(token), NULL);
    getAlgorithmOID(hVal, tokenNode, &oid);
    if (oid == OID_V)
        type = h235CertificateMD5;
    else if ((oid == OID_W) || (oid == OID_W_1))
        type = h235CertificateSHA1;

    /* check if the certificate is a URL */
    __pvtGetNodeIdByFieldIds(certificateNode, hVal, tokenNode,
        {_q931(toBeSigned) _q931(certificate) LAST_TOKEN});
    oidLen = pvtGetString(hVal, pvtGetChild(hVal, certificateNode, __q931(type), NULL), sizeof(oidStr), (char*)oidStr);
    tokenOidToOidType((char*)oidStr, (RvSize_t)oidLen, &oid);
    if ((oid == OID_P1) || (oid == OID_P2))
        type |= h235CertificateURL;

    /* get the certificate */
    size = pvtGetString(hVal, pvtGetChild(hVal, certificateNode, __q931(certificate), NULL), sizeof(certificate), (char*)certificate);
    if (size < 0)
    {
        size = 0;
        crtPtr = NULL;
    }
    else
        crtPtr = (RvUint8*)certificate;

    foundCrt = h235App->eventHandlers.evGetCrtByHandle(h235App->hApp, (H235HAPP) h235App,
        entityEntry->crtInHandle, &crtPtr, (RvSize_t*)&size, &type);
    if (!foundCrt)
        return RV_ERROR_UNINITIALIZED;

    /* verifying the hash value on the fixed encoded message */
    res=calcP2Signature(h235App, (char*)buf, bufLen, entityEntry->crtInHandle, signedVal, &len, type);
    if (res == RV_ERROR_NOTSUPPORTED)
    {
        /* CB not implemented */
        appReply = failToApp(h235App, messageRoot, &entityEntry->id, h235ReasonEncodeCBMissing, contextInfo);
        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else return RV_ERROR_UNKNOWN;
    }
    if (res != RV_OK)
    {
        /* CB implemented, but verification failed */
        appReply = failToApp(h235App, messageRoot, &entityEntry->id, h235ReasonIncorrectAuth, contextInfo);
        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else return RV_ERROR_UNKNOWN;
    }
    return RV_OK;
}


RvStatus checkByProcedure3(
    IN  h235Element_T*      h235App,
    IN  int                 messageRoot,
    IN  int                 cryptoSignedTokenNode,
    IN  entityId_t*         entityId,
    IN  H235HKEY            crtInHandle,
    OUT h235ContextInfo_t*  contextInfo)
{
    HPVT hVal = h235App->hVal;
    char tokenBuf[2048], sigValue[128], oidStr[128], certificate[512];
    int sigLen, bufLen, oidLen, res;
    RvInt32 len;
    RvSize_t crtLen;
    RvUint8 * crtPtr;
    RvBool foundCrt;

    h235AppFailReply_t appReply;
    RvPvtNodeId tokenNode, sigNode, certificateNode, toBeSignedNode;

    h235CertificateType_t type = (h235CertificateType_t)0;
    objectOidType oid;

    /* get the certificate type */
    tokenNode = pvtGetChild(hVal, cryptoSignedTokenNode, __q931(token), NULL);
    getAlgorithmOID(hVal, tokenNode, &oid);
    if (oid == OID_V)
        type = h235CertificateMD5;
    else if (oid == OID_W)
        type = h235CertificateSHA1;

    /* check if the certificate is a URL */
    __pvtGetNodeIdByFieldIds(certificateNode, hVal, tokenNode,
        {_q931(toBeSigned) _q931(certificate) LAST_TOKEN});
    oidLen = pvtGetString(hVal, pvtGetChild(hVal, certificateNode, __q931(type), NULL), sizeof(oidStr), oidStr);
    tokenOidToOidType(oidStr, (RvSize_t)oidLen, &oid);
    if ((oid == OID_P1) || (oid == OID_P2))
        type |= h235CertificateURL;

    /* get the certificate */
    len = pvtGetString(hVal, pvtGetChild(hVal, certificateNode, __q931(certificate), NULL), sizeof(certificate), certificate);
    if (len < 0)
    {
        crtLen = 0;
        crtPtr = NULL;
    }
    else
    {
        crtLen = (RvSize_t)len;
        crtPtr = (RvUint8*)certificate;
    }

    foundCrt = h235App->eventHandlers.evGetCrtByHandle(h235App->hApp, (H235HAPP)h235App, crtInHandle,
        &crtPtr, &crtLen, &type);
    if (!foundCrt)
        return RV_ERROR_UNINITIALIZED;

    /* getting the signature value */
    sigNode = pvtGetChild(hVal, tokenNode, __q931(signature), NULL);
    sigLen = pvtGetString(hVal, sigNode, sizeof(sigValue), (char*)sigValue);

    /* encode the token */
    toBeSignedNode = pvtGetChild(hVal, tokenNode, __q931(toBeSigned), NULL);

    /* set zero in the token OID in order to calculate the signature */
    pvtAdd(hVal, toBeSignedNode, __q931(tokenOID), (RvInt32)objects.length[OID_ZERO], objects.OID[OID_ZERO], NULL);

    res = cmEmEncode(hVal, toBeSignedNode, (unsigned char *)tokenBuf, 2048, &bufLen);
    if (res < 0)
        return res;

    /* verifying the hash value on the fixed encoded message */
    res = calcP2Signature(h235App, tokenBuf, bufLen, crtInHandle, (RvUint8*)sigValue, &sigLen, type);
    if (res == RV_ERROR_NOTSUPPORTED)
    {
        /* CB not implemented */
        appReply = failToApp(h235App, messageRoot, entityId, h235ReasonEncodeCBMissing, contextInfo);
        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else return RV_ERROR_UNKNOWN;
    }
    if (res != RV_OK)
    {
        /* CB implemented, but verification failed */
        appReply = failToApp(h235App, messageRoot, entityId, h235ReasonIncorrectAuth, contextInfo);
        if (appReply == h235FailReplyForceOk)
            return RV_OK;
        else return RV_ERROR_UNKNOWN;
    }
    return RV_OK;
}


/*==============================================================================*/
/*==============================================================================*/
/*==============================================================================*/
/*==============================================================================*/


/********************************************************************************
 * Routine Name: secureMessage
 * Description : add security tokens to the message
 * Input:  h235App          - H.235 information pointer
 *         entityEntryPtr   - receiving entity information pointer
 *         callInfoPtr      - call information pointer
 *         messageRoot      - the root of the message
 * Output: (-)
 * Return: (-)
 ********************************************************************************/
void secureMessage(
    IN  h235Element_T*      h235App,
    IN  h235EntityEntry_t*  entityEntryPtr,
    IN  h235CallInfo_t*     callInfoPtr,
    IN  int                 messageRoot,
    OUT RvPvtNodeId         tokens[5])
{
    HPVT hVal = h235App->hVal;
    cmProtocol protocol = cmProtocolGetProtocol(h235App->hApp,messageRoot);
    RvPvtNodeId cryptoTokensNode=RV_PVT_INVALID_NODEID, clearTokenNode=RV_PVT_INVALID_NODEID, tmpNode;
    h235Mode_t secureMode;
    h235CertificateType_t type = (h235CertificateType_t)0;
    RvUint8 buffer[5000]; /* ToDo: how to handle this buffer allocation? */
    RvUint8 digest[128];
    int result, encodedLen, patternId1=RV_ERROR_UNKNOWN, patternId2=RV_ERROR_UNKNOWN, patternId4=RV_ERROR_UNKNOWN, len;
    h235Mode_t modeSupportCrtP3 = h235ModeNone;

    /* determine the secure mode */
    secureMode = entityEntryPtr->secureMode;
    if (secureMode & (h235ModeProcedure2 | h235ModeProcedure3))
        modeSupportCrtP3 = h235ModeProcedure3; /* only add P3 if we have the endpoint's certificates */
    if (callInfoPtr)
        secureMode = (h235Mode_t)(secureMode | (callInfoPtr->secureMode & (modeSupportCrtP3 | RV_H235_ALL_DH_MODES)));

    switch(protocol)
    {
    case cmProtocolRAS:
        if (((callInfoPtr != NULL) && (RV_PVT_NODEID_IS_VALID(callInfoPtr->sendCryptoToken[0]))) ||
            (secureMode & RV_H235_ALL_SEC_MODES))
        {
            __pvtBuildByFieldIds(cryptoTokensNode, hVal, messageRoot, {_anyField _q931(cryptoTokens) LAST_TOKEN}, 0, NULL);
        }
        __pvtBuildByFieldIds(clearTokenNode, hVal, messageRoot, {_anyField _q931(tokens) LAST_TOKEN}, 0, NULL);
        break;
    case cmProtocolQ931:
        if (((callInfoPtr != NULL) && (RV_PVT_NODEID_IS_VALID(callInfoPtr->sendCryptoToken[0]))) ||
            (secureMode & RV_H235_ALL_SEC_MODES))
        {
            __pvtBuildByFieldIds(cryptoTokensNode, hVal, messageRoot,
                {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu)
                 _q931(h323_message_body) _anyField  _q931(cryptoTokens) LAST_TOKEN}, 0, NULL);
        }
        __pvtBuildByFieldIds(clearTokenNode, hVal, messageRoot,
            {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu)
             _q931(h323_message_body) _anyField  _q931(tokens) LAST_TOKEN}, 0, NULL);
        break;
    default:
        return;
    }

    /* set V3 support token in case we do not have it*/
 	{
 		RvBool     addTokenOID = RV_TRUE;
 		RvBool     isString;
 		RvInt32	   size;
 		RvInt32    tokenOIDNode;

 		__pvtGetByFieldIds(tokenOIDNode, hVal, clearTokenNode, {_anyField _q931(tokenOID) LAST_TOKEN}, NULL, &size, &isString);
 		if (RV_PVT_NODEID_IS_VALID(tokenOIDNode) && isString)
 		{
 			char arrayOID[256];

 			pvtGetString(hVal, tokenOIDNode, sizeof(arrayOID), arrayOID);
 			if (memcmp(arrayOID, objects.OID[OID_V3], size) == 0)
 			{
 				/* the tokenOID is already set */
 				addTokenOID = RV_FALSE;
 			}
 		}

 		if (addTokenOID)
 		{
 			/* no tokenOID node */
 			tmpNode = pvtAdd(hVal, clearTokenNode, __nul(0),0, NULL, NULL);
 			pvtAdd(hVal, tmpNode, __q931(tokenOID), (RvInt32)objects.length[OID_V3], objects.OID[OID_V3], NULL);
 		}

 	}

    /* adding procedure 1 token */
    if ((secureMode & h235ModeProcedure1) ||
        (entityEntryPtr->bGenerateSecret && (entityEntryPtr->secureMode & h235ModeProcedure1)))
        tokens[0] = secureByAuthAndIntegrityProcedure1(h235App, messageRoot, entityEntryPtr, callInfoPtr, cryptoTokensNode);
    if (secureMode & h235ModeProcedure1a)
        tokens[1] = secureByAuthProcedure1a(h235App, messageRoot, entityEntryPtr, callInfoPtr, cryptoTokensNode, buffer);
    /* adding procedure 2 token */
    if (secureMode & h235ModeProcedure2)
        tokens[2] = secureByAuthAndIntegrityProcedure2(h235App, messageRoot, entityEntryPtr, callInfoPtr, cryptoTokensNode, &type);
    /* adding procedure 3 token */
    if (secureMode & h235ModeProcedure3)
        tokens[3] = secureByAuthAndIntegrityProcedure3(h235App, messageRoot, entityEntryPtr, callInfoPtr, cryptoTokensNode, buffer);
    /* adding procedure 4 token */
    if ((secureMode & h235ModeProcedure4) && (entityEntryPtr->secureMode & h235ModeProcedure4))
        tokens[4] = secureByAuthAndIntegrityProcedure4(h235App, messageRoot, entityEntryPtr, callInfoPtr, cryptoTokensNode, &type, &tokens[4]);

    /* ToDo: add the rest of the tokens */
    if (callInfoPtr != NULL)
    {
        int i;
        for (i=0; (i < MAX_CALL_CRYPTO_TOKENS) && (callInfoPtr->sendCryptoToken[i] >= 0); i++)
        {
            tmpNode = pvtAdd(hVal, cryptoTokensNode, __nul(0),0, NULL, NULL);
            pvtMoveTree(hVal, tmpNode, callInfoPtr->sendCryptoToken[i]);
            callInfoPtr->sendCryptoToken[i] = RV_PVT_INVALID_NODEID;
        }
        if ((protocol == cmProtocolQ931) && (RV_PVT_NODEID_IS_VALID(callInfoPtr->sendClearToken)))
        {
            tmpNode = pvtAdd(hVal, clearTokenNode, __nul(0),0, NULL, NULL);
            pvtMoveTree(hVal, tmpNode, callInfoPtr->sendClearToken);
            callInfoPtr->sendClearToken = RV_PVT_INVALID_NODEID;
        }
    }

    /* encode the message */
	memset(buffer, 0, sizeof(buffer));
    result = cmEmEncode(hVal, messageRoot, buffer, sizeof(buffer), &encodedLen);
    if (result < 0)
        return;

    if (tokens[0] >= 0)
    {
        /* turn the P1 pattern to zeroes */
        patternId1 = findSubstring((unsigned char*)buffer, encodedLen, 0, (unsigned char*)PROCEDURE_1_PATTERN, 12);
        if (patternId1 >= 0)
            memset(buffer+patternId1, 0, 12);
    }
    if (tokens[2] >= 0)
    {
        /* turn the P2 pattern to zeroes */
        patternId2 = findSubstring((unsigned char*)buffer, encodedLen, 0, (unsigned char*)PROCEDURE_2_PATTERN, 128);
        if (patternId2 >= 0)
            memset(buffer+patternId2, 0, 128);
    }
    if (tokens[4] >= 0)
    {
        /* turn the P2 pattern to zeroes */
        patternId4 = findSubstring((unsigned char*)buffer, encodedLen, 0, (unsigned char*)PROCEDURE_4_PATTERN_2, 128);
        if (patternId4 >= 0)
            memset(buffer+patternId4, 0, 128);
        else
        {
            patternId4 = findSubstring((unsigned char*)buffer, encodedLen, 0, (unsigned char*)PROCEDURE_4_PATTERN_1, 12);
            if (patternId4 >= 0)
                memset(buffer+patternId4, 0, 12);
        }
    }

    if (patternId1 >= 0)
    {
        /* secure by password hashing */
        len = 0;
        calcP1HashValue(h235App, (char*)buffer, encodedLen, entityEntryPtr->keyHandle, digest, &len);
        pvtAdd(hVal, tokens[0], __q931(hash), 96, (const char *)digest, NULL);
    }

    if (patternId2 >= 0)
    {
        /* secure by certificate hashing */
        calcP2Signature(h235App, (char*)buffer, encodedLen, entityEntryPtr->crtOutHandle, digest, &len, type);
        pvtAdd(hVal, tokens[2], __q931(signature), 1024, (const char *)digest, NULL);
    }

    if (patternId4 >= 0)
    {
        /* secure by certificate hashing */
        if (entityEntryPtr->outDHinf.isValid && entityEntryPtr->inDHinf.isValid)
        {
            calcP1HashValue(h235App, (char*)buffer, encodedLen, entityEntryPtr->keyHandle,digest, &len);
            pvtAdd(hVal, tokens[4], __q931(hash), 96, (const char *)digest, NULL);
        }
        else
        {
            calcP2Signature(h235App, (char*)buffer, encodedLen, entityEntryPtr->crtOutHandle, digest, &len, h235CertificateSHA1);
            pvtAdd(hVal, tokens[4], __q931(signature), 1024, (const char *)digest, NULL);
        }
    }

    return;
}


static RvStatus addClearTokenParams(
    IN  h235Element_T*     h235App,
    IN  entityId_t*        senderId,
    IN  entityId_t*        generalId,
    IN  int                clearTokenRoot,
    IN  h235Mode_t         secureMode)
{
    HPVT hVal = h235App->hVal;
    RvInt32 timeStamp = RvClockGet(NULL, NULL);
    objectOidType oidType;
    static int random = 0;

    switch (secureMode)
    {
    case h235ModeProcedure1:
    case h235ModeProcedure1a:   oidType = OID_T2; break;
    case h235ModeProcedure2:    oidType = OID_S2; break;
    case h235ModeProcedure3:    oidType = OID_ZERO; break; /* add a zero OID, just for the signature calculation */
    case h235ModeProcedure4:    oidType = OID_S_1; break;
    default:
        return RV_ERROR_BADPARAM;
    }

    pvtAdd(hVal, clearTokenRoot, __q931(tokenOID), (RvInt32)objects.length[oidType], objects.OID[oidType], NULL);
    pvtAdd(hVal, clearTokenRoot, __q931(timeStamp), timeStamp, NULL,NULL);
    pvtAdd(hVal, clearTokenRoot, __q931(random), random, NULL,NULL);

    if (generalId->length)
        pvtAdd(hVal,clearTokenRoot, __q931(generalID), generalId->length, (const char*)generalId->data, NULL);

    if (senderId != NULL)
        pvtAdd(hVal,clearTokenRoot, __q931(sendersID), senderId->length, (const char*)senderId->data, NULL);
    else if (h235App->senderId.length)
        pvtAdd(hVal,clearTokenRoot,__q931(sendersID),h235App->senderId.length,(const char*)h235App->senderId.data,NULL);

    random = (random + 1)%65530;
    return RV_OK;
}


/***********************************************************************************
 * Routine Name:  secureByAuthAndIntegrityProcedure1
 * Description :  add authentication information and authenticator to the message
 *                according the H.235 version2 - ANNEX D - procedure I.
 * Input: h235App          - H235 application handle.
 *        messageRoot      - the root of the message
 *        entityEntry      - information about the destination entity
 *        cryptoTokensNode - node of the crypto-tokens
 * Output: (-)
 * Return: RvPvtNodeId on success, -1 on fail
 ***********************************************************************************/
static RvPvtNodeId secureByAuthAndIntegrityProcedure1(
    IN  h235Element_T*     h235App,
    IN  int                messageRoot,
    IN  h235EntityEntry_t* entityEntry,
    IN  h235CallInfo_t*    callInfo,
    IN  int                cryptoTokensNode)
{
    HPVT hVal = h235App->hVal;
    int cryptoNode, tokenNode, hashValsNode;

    RV_UNUSED_ARG(messageRoot);
    RV_UNUSED_ARG(callInfo);

    /* check if there already is a first token */
    cryptoNode = pvtGetChild(hVal, cryptoTokensNode, __nul(1), NULL);
    if (RV_PVT_NODEID_IS_VALID(cryptoNode))
    {
        /* just make sure it contains something, and not saved for us */
        if (RV_PVT_NODEID_IS_VALID(pvtChild(hVal, cryptoNode)))
        {
            /* ok, we have to move this */
            RvPvtNodeId moveTo = pvtAdd(hVal, cryptoTokensNode, __anyField, 0, NULL, NULL);
            pvtMoveTree(hVal, moveTo, cryptoNode);
        }
    }

    /* build first branch */
    __pvtBuildByFieldIds(cryptoNode, hVal, cryptoTokensNode, {_nul(1) _q931(nestedcryptoToken)
        _q931(cryptoHashedToken)  LAST_TOKEN}, 0, NULL);

    /*set the tokenOID to A - all message is Authenticated*/
    pvtAdd(hVal,cryptoNode,__q931(tokenOID), (RvInt32)objects.length[OID_A2], objects.OID[OID_A2], NULL);

    /*set the hashVals fields. The hashVals is a clearToken structure*/
    hashValsNode = pvtAdd(hVal,cryptoNode,__q931(hashedVals),0,NULL, NULL);
    addClearTokenParams(h235App, NULL, &entityEntry->id, hashValsNode, h235ModeProcedure1);

    /*set the token fields. the token is a HASHED structure*/
    tokenNode = pvtAdd(hVal,cryptoNode,__q931(token),0,NULL, NULL);

    pvtAdd(hVal, tokenNode, __q931(algorithmOID), (RvInt32)objects.length[OID_U2], objects.OID[OID_U2], NULL);

    pvtAdd(hVal, tokenNode, __q931(paramS), 0, NULL, NULL);

    pvtAdd(hVal, tokenNode, __q931(hash), 96, PROCEDURE_1_PATTERN, NULL);

    return tokenNode;
}


/***********************************************************************************
 * Routine Name:  secureByAuthProcedure1a
 * Description :  add authentication information and authenticator to the message
 *                according the H.235 version3 - ANNEX D - procedure IA.
 * Input: h235App          - H235 application handle.
 *        messageRoot      - the root of the message
 *        entityEntry      - information about the destination entity
 *        cryptoTokensNode - node of the crypto-tokens
 * Output: (-)
 * Return: RvPvtNodeId on success, -1 on fail
 ***********************************************************************************/
static RvPvtNodeId secureByAuthProcedure1a(
    IN  h235Element_T*     h235App,
    IN  int                messageRoot,
    IN  h235EntityEntry_t* entityEntry,
    IN  h235CallInfo_t*    callInfo,
    IN  int                cryptoTokensNode,
    OUT RvUint8*           buffer)
{
    HPVT hVal = h235App->hVal;
    int cryptoNode, tokenNode, hashValsNode;
    int len = 0, encodedLen=0;
    RvStatus result;
    RvUint8 digest[32];

    RV_UNUSED_ARG(messageRoot);
    RV_UNUSED_ARG(callInfo);

    __pvtBuildByFieldIds(cryptoNode, hVal, cryptoTokensNode, {_nul(0) _q931(nestedcryptoToken)
        _q931(cryptoHashedToken)  LAST_TOKEN}, 0, NULL);

    /*set the tokenOID to B - only this token is secured*/
    pvtAdd(hVal,cryptoNode,__q931(tokenOID), (RvInt32)objects.length[OID_B2], objects.OID[OID_B2], NULL);

    /*set the hashVals fields. The hashVals is a clearToken structure*/
    hashValsNode = pvtAdd(hVal,cryptoNode,__q931(hashedVals),0,NULL, NULL);
    addClearTokenParams(h235App, NULL, &entityEntry->id, hashValsNode, h235ModeProcedure1);

    /*set the token fields. the token is a HASHED structure*/
    tokenNode = pvtAdd(hVal,cryptoNode,__q931(token),0,NULL, NULL);

    pvtAdd(hVal, tokenNode, __q931(algorithmOID), (RvInt32)objects.length[OID_U2], objects.OID[OID_U2], NULL);

    pvtAdd(hVal, tokenNode, __q931(paramS), 0, NULL, NULL);

    pvtAdd(hVal, tokenNode, __q931(hash), 96, PROCEDURE_1_PATTERN, NULL);

    /* and now, the signature (ToDo: check) */
    result = cmEmEncode(hVal, hashValsNode, buffer, 5000, &encodedLen);
    if (result < 0)
        return result;

    calcP1HashValue(h235App, (char*)buffer, encodedLen, entityEntry->keyHandle, digest, &len);
    pvtAdd(hVal, tokenNode, __q931(hash), 96, (const char *)digest, NULL);

    return tokenNode;
}


/***********************************************************************************
 * Routine Name:  secureByAuthAndIntegrityProcedure2
 * Description :  add authentication information and authenticator to the message
 *                according the H.235 version2 - ANNEX E - procedure II.
 * Input: h235App          - H235 application handle.
 *        messageRoot      - the root of the message
 *        entityEntry      - information about the destination entity
 *        cryptoTokensNode - node of the crypto-tokens
 * Output: (-)
 * Return: RvPvtNodeId on success, -1 on fail
 ***********************************************************************************/
static RvPvtNodeId secureByAuthAndIntegrityProcedure2(
    IN  h235Element_T*          h235App,
    IN  int                     messageRoot,
    IN  h235EntityEntry_t*      entityEntry,
    IN  h235CallInfo_t*         callInfo,
    IN  int                     cryptoTokensNode,
    OUT h235CertificateType_t*  type)
{
    HPVT hVal = h235App->hVal;
    RvPvtNodeId cryptoNode, tokenNode, toBeSignedNode, crtNode;
    RvUint8* certificate = NULL;
    RvSize_t size = 0;
    RvBool foundCrt;
    *type = (h235CertificateType_t)0;

    RV_UNUSED_ARG(messageRoot);
    RV_UNUSED_ARG(callInfo);

    if ((h235App->evSHA1_RSA_Sig == NULL) && (h235App->evMD5_RSA_Sig == NULL))
        return RV_ERROR_NOTSUPPORTED;

    __pvtBuildByFieldIds(cryptoNode, hVal, cryptoTokensNode, {_nul(0) _q931(nestedcryptoToken)
        _q931(cryptoSignedToken)  LAST_TOKEN}, 0, NULL);

    /* set the tokenOID to A - all message is Authenticated */
    pvtAdd(hVal,cryptoNode,__q931(tokenOID), (RvInt32)objects.length[OID_A2], objects.OID[OID_A2], NULL);

    /* set the token fields */
    tokenNode = pvtAdd(hVal,cryptoNode,__q931(token),0,NULL,NULL);

    /* set the toBeSigned fields. The toBeSigned is a clearToken structure */
    toBeSignedNode = pvtAdd(hVal,tokenNode,__q931(toBeSigned),0,NULL,NULL);
    /* setting tokenOID, timestamp, random, generalId and senderId */
    addClearTokenParams(h235App, NULL, &entityEntry->id, toBeSignedNode, h235ModeProcedure2);
    /* ToDo: add certificate inside the clearToken */

    /* NULL paramS */
    pvtAdd(hVal, tokenNode, __q931(paramS), 0, NULL, NULL);
    /* certificate */
    crtNode = pvtAdd(hVal,toBeSignedNode,__q931(certificate),0,NULL,NULL);

    /* get the certificate */
    foundCrt = h235App->eventHandlers.evGetCrtByHandle(h235App->hApp, (H235HAPP) h235App,
        entityEntry->crtOutHandle, &certificate, &size, type);
    if (!foundCrt)
        return RV_ERROR_UNINITIALIZED;

    if (*type & h235CertificateMD5)
    {
        /* we're using MD5 Signature */
        pvtAdd(hVal,tokenNode,__q931(algorithmOID),(RvInt32)objects.length[OID_V],objects.OID[OID_V],NULL);
        pvtAdd(hVal,crtNode,__q931(type),(RvInt32)objects.length[OID_V],objects.OID[OID_V],NULL);
    }
    else
    {
        /* we're using SHA1 Signature */
        pvtAdd(hVal,tokenNode,__q931(algorithmOID),(RvInt32)objects.length[OID_W],objects.OID[OID_W],NULL);
        pvtAdd(hVal,crtNode,__q931(type),(RvInt32)objects.length[OID_W],objects.OID[OID_W],NULL);
    }
    if (*type & h235CertificateURL)
        pvtAdd(hVal,crtNode,__q931(type),(RvInt32)objects.length[OID_P1],objects.OID[OID_P1],NULL);

    if (certificate && size)
        pvtAdd(hVal,crtNode,__q931(certificate),(RvInt32)size,(const char*)certificate,NULL);
    else
        pvtAdd(hVal,crtNode,__q931(certificate),(RvInt32)0,(const char*)"",NULL);

    /* and now, the signature (ToDo: check) */
    pvtAdd(hVal, tokenNode, __q931(signature), 1024, (const char*)PROCEDURE_2_PATTERN, NULL);

    return tokenNode;
}


/***********************************************************************************
 * Routine Name:  secureByAuthAndIntegrityProcedure3
 * Description :  add authentication information and authenticator to the message
 *                according the H.235 version2 - ANNEX E - procedure III.
 * Input: h235App          - H235 application handle.
 *        messageRoot      - the root of the message
 *        entityEntry      - information about the destination entity
 *        cryptoTokensNode - node of the crypto-tokens
 * Output: (-)
 * Return: RvPvtNodeId on success, -1 on fail
 ***********************************************************************************/
static RvPvtNodeId secureByAuthAndIntegrityProcedure3(
    IN  h235Element_T*     h235App,
    IN  int                messageRoot,
    IN  h235EntityEntry_t* entityEntry,
    IN  h235CallInfo_t*    callInfo,
    IN  int                cryptoTokensNode,
    OUT RvUint8*           buffer)
{
    HPVT hVal = h235App->hVal;
    RvPvtNodeId cryptoNode,tokenNode, toBeSignedNode, crtNode;
    RvUint8* certificate = NULL;
    RvSize_t size = 0;
    h235CertificateType_t type = (h235CertificateType_t)0;
    RvUint8 digest[128];
    RvStatus result;
    int encodedLen, len;
    entityId_t dstEntId, srcEntId;
    entityId_t *realDst = &entityEntry->id, *realSrc = NULL;
    RvBool foundCrt;

    RV_UNUSED_ARG(messageRoot);
    RV_UNUSED_ARG(callInfo);

    if ((h235App->evSHA1_RSA_Sig == NULL) && (h235App->evMD5_RSA_Sig == NULL))
        return RV_ERROR_NOTSUPPORTED;

    __pvtBuildByFieldIds(cryptoNode, hVal, cryptoTokensNode, {_nul(0) _q931(nestedcryptoToken)
        _q931(cryptoSignedToken)  LAST_TOKEN}, 0, NULL);

    /* set the tokenOID to A - all message is Authenticated */
    pvtAdd(hVal,cryptoNode,__q931(tokenOID), (RvInt32)objects.length[OID_B2], objects.OID[OID_B2], NULL);

    /* set the token fields */
    tokenNode = pvtAdd(hVal,cryptoNode,__q931(token),0,NULL,NULL);

    /* see if we should get the general ID from the application */
    if ((!h235App->isGK) && (callInfo != NULL) && (callInfo->hsCall != NULL))
    {
        if (h235App->eventHandlers.evGetIDsByCall != NULL)
        {
            if (h235App->eventHandlers.evGetIDsByCall(h235App->hApp, (H235HAPP)h235App,
                callInfo->hsCall, emaGetApplicationHandle((EMAElement)callInfo->hsCall), &srcEntId, &dstEntId))
            {
                realDst = &dstEntId;
                if (srcEntId.length > 0)
                    realSrc = &srcEntId;
            }
        }
        else if (h235App->eventHandlers.evGetIDByCall != NULL)
        {
            if (h235App->eventHandlers.evGetIDByCall(h235App->hApp, (H235HAPP)h235App,
                callInfo->hsCall, emaGetApplicationHandle((EMAElement)callInfo->hsCall), &dstEntId))
            {
                realDst = &dstEntId;
            }
        }
    }

    /* set the toBeSigned fields. The toBeSigned is a clearToken structure */
    toBeSignedNode = pvtAdd(hVal,tokenNode,__q931(toBeSigned),0,NULL,NULL);
    /* setting tokenOID, timestamp, random, generalId and senderId */
    addClearTokenParams(h235App, realSrc, realDst, toBeSignedNode, h235ModeProcedure3);

    /* NULL paramS */
    pvtAdd(hVal, tokenNode, __q931(paramS), 0, NULL, NULL);
    /* certificate */
    crtNode = pvtAdd(hVal,toBeSignedNode,__q931(certificate),0,NULL,NULL);

    /* get the certificate */
    foundCrt = h235App->eventHandlers.evGetCrtByHandle(h235App->hApp, (H235HAPP) h235App,
        entityEntry->crtOutHandle, &certificate, &size, &type);
    if (!foundCrt)
        return RV_ERROR_UNINITIALIZED;

    if (type & h235CertificateMD5)
    {
        /* we're using MD5 Signature */
        pvtAdd(hVal,tokenNode,__q931(algorithmOID),(RvInt32)objects.length[OID_V],objects.OID[OID_V],NULL);
        pvtAdd(hVal,crtNode,__q931(type),(RvInt32)objects.length[OID_V],objects.OID[OID_V],NULL);
    }
    else
    {
        /* we're using SHA1 Signature */
        pvtAdd(hVal,tokenNode,__q931(algorithmOID),(RvInt32)objects.length[OID_W],objects.OID[OID_W],NULL);
        pvtAdd(hVal,crtNode,__q931(type),(RvInt32)objects.length[OID_W],objects.OID[OID_W],NULL);
    }
    if (type & h235CertificateURL)
        pvtAdd(hVal,crtNode,__q931(type),(RvInt32)objects.length[OID_P1],objects.OID[OID_P1],NULL);

    if (certificate && size)
        pvtAdd(hVal,crtNode,__q931(certificate),(RvInt32)size,(const char*)certificate,NULL);
    else
        pvtAdd(hVal,crtNode,__q931(certificate),(RvInt32)0,(const char*)"",NULL);

    /* and now, the signature (ToDo: check) */
    result = cmEmEncode(hVal, toBeSignedNode, buffer, 5000, &encodedLen);
    if (result < 0)
        return result;

    calcP2Signature(h235App, (char*)buffer, encodedLen, entityEntry->crtOutHandle, digest, &len, type);
    pvtAdd(hVal, tokenNode, __q931(signature), 1024, (const char *)digest, NULL);
    pvtAdd(hVal, toBeSignedNode, __q931(tokenOID), (RvInt32)objects.length[OID_R2], objects.OID[OID_R2], NULL);

    return tokenNode;
}


/***********************************************************************************
 * Routine Name:  secureByAuthAndIntegrityProcedure4
 * Description :  add authentication information and authenticator to the message
 *                according the H.235 version2 - ANNEX E - procedure IV.
 * Input: h235App          - H235 application handle.
 *        messageRoot      - the root of the message
 *        entityEntry      - information about the destination entity
 *        cryptoTokensNode - node of the crypto-tokens
 * Output: type - type of certificate to use
 *         dhToken - the Diffie Hellman token, if used
 * Return: RvPvtNodeId on success, -1 on fail
 ***********************************************************************************/
static RvPvtNodeId secureByAuthAndIntegrityProcedure4(
    IN  h235Element_T*          h235App,
    IN  int                     messageRoot,
    IN  h235EntityEntry_t*      entityEntry,
    IN  h235CallInfo_t*         callInfo,
    IN  int                     cryptoTokensNode,
    OUT h235CertificateType_t*  type,
    OUT RvPvtNodeId*            dhToken)
{
    HPVT hVal = h235App->hVal;
    RvPvtNodeId cryptoNode, tokenNode;

    RV_UNUSED_ARG(messageRoot);
    RV_UNUSED_ARG(callInfo);

    if (entityEntry->inDHinf.isValid && entityEntry->outDHinf.isValid)
    {
        /* we're just now moving into the procedure I mode of operation */
        RvPvtNodeId hashValsNode;

        __pvtBuildByFieldIds(cryptoNode, hVal, cryptoTokensNode, {_nul(0) _q931(nestedcryptoToken)
            _q931(cryptoHashedToken)  LAST_TOKEN}, 0, NULL);

        /*set the tokenOID to A - all message is Authenticated*/
        pvtAdd(hVal,cryptoNode,__q931(tokenOID), (RvInt32)objects.length[OID_A1], objects.OID[OID_A1], NULL);

        /*set the hashVals fields. The hashVals is a clearToken structure*/
        hashValsNode = pvtAdd(hVal,cryptoNode,__q931(hashedVals),0,NULL, NULL);
        addClearTokenParams(h235App, NULL, &entityEntry->id, hashValsNode, h235ModeProcedure1);

        /*set the token fields. the token is a HASHED structure*/
        tokenNode = pvtAdd(hVal,cryptoNode,__q931(token),0,NULL, NULL);
        pvtAdd(hVal, tokenNode, __q931(algorithmOID), (RvInt32)objects.length[OID_U1], objects.OID[OID_U1], NULL);
        pvtAdd(hVal, tokenNode, __q931(paramS), 0, NULL, NULL);
        pvtAdd(hVal, tokenNode, __q931(hash), 96, PROCEDURE_4_PATTERN_1, NULL);

        /* from now on, we're procedure 1 */
        entityEntry->secureMode = (h235Mode_t)(entityEntry->secureMode & RV_H235_ALL_DH_MODES);
        entityEntry->secureMode = (h235Mode_t)(entityEntry->secureMode | h235ModeProcedure1);
    }
    else
    {
        /* we're in the procedure II mode of operation */
        RvPvtNodeId toBeSignedNode, crtNode;
        RvUint8* certificate = NULL;
        RvSize_t size = 0;
        RvBool foundCrt;
        *type = (h235CertificateType_t)0;

        if (h235App->evSHA1_RSA_Sig == NULL)
            return RV_ERROR_NOTSUPPORTED;

        /*** First Token, the Procedure-2-like token */
        __pvtBuildByFieldIds(cryptoNode, hVal, cryptoTokensNode, {_nul(0) _q931(nestedcryptoToken)
            _q931(cryptoSignedToken)  LAST_TOKEN}, 0, NULL);

        /* set the tokenOID to A - all message is Authenticated */
        pvtAdd(hVal,cryptoNode,__q931(tokenOID), (RvInt32)objects.length[OID_A_1], objects.OID[OID_A_1], NULL);

        /* set the token fields */
        tokenNode = pvtAdd(hVal,cryptoNode,__q931(token),0,NULL,NULL);

        /* set the toBeSigned fields. The toBeSigned is a clearToken structure */
        toBeSignedNode = pvtAdd(hVal,tokenNode,__q931(toBeSigned),0,NULL,NULL);
        /* setting tokenOID, timestamp, random, generalId and senderId */
        addClearTokenParams(h235App, NULL, &entityEntry->id, toBeSignedNode, h235ModeProcedure4);
        /* ToDo: add certificate inside the clearToken */

        /* NULL paramS */
        pvtAdd(hVal, tokenNode, __q931(paramS), 0, NULL, NULL);
        /* certificate */
        crtNode = pvtAdd(hVal,toBeSignedNode,__q931(certificate),0,NULL,NULL);

        /* get the certificate */
        foundCrt = h235App->eventHandlers.evGetCrtByHandle(h235App->hApp, (H235HAPP) h235App,
            entityEntry->crtOutHandle, &certificate, &size, type);
        if (!foundCrt)
            return RV_ERROR_UNINITIALIZED;

        /* we're using SHA1 Signature */
        pvtAdd(hVal,tokenNode,__q931(algorithmOID),(RvInt32)objects.length[OID_W_1],objects.OID[OID_W_1],NULL);
        pvtAdd(hVal,crtNode,__q931(type),(RvInt32)objects.length[OID_W_1],objects.OID[OID_W_1],NULL);

        if (*type & h235CertificateURL)
            pvtAdd(hVal,crtNode,__q931(type),(RvInt32)objects.length[OID_P1],objects.OID[OID_P1],NULL);

        if (certificate && size)
            pvtAdd(hVal,crtNode,__q931(certificate),(RvInt32)size,(const char*)certificate,NULL);
        else
            pvtAdd(hVal,crtNode,__q931(certificate),(RvInt32)0,(const char*)"",NULL);

        /* and now, the signature (ToDo: check) */
        pvtAdd(hVal, tokenNode, __q931(signature), 1024, (const char*)PROCEDURE_4_PATTERN_2, NULL);
    }

    /*** Second Token, the Diffie-Hellman-like token */
    if ((entityEntry->outDHinf.isValid) ||
        (h235GenerateDiffieHellmanEP((H235HAPP)h235App, h235ModeDiffieHellman1024, (H235HENTITY)entityEntry) == RV_OK))
    {
        RvPvtNodeId tempNode;

        __pvtBuildByFieldIds(tempNode, hVal, cryptoTokensNode, {_nul(0) _q931(nestedcryptoToken)
            _q931(cryptoSignedToken)  LAST_TOKEN}, 0, NULL);

        /* add the appropriate token OID */
        pvtAdd(hVal, tempNode, __q931(tokenOID), (RvInt32)objects.length[OID_Q], objects.OID[OID_Q], NULL);

        /* build an empty token */
        tempNode = pvtAdd(hVal, tempNode, __q931(token), 0, NULL, NULL);
        pvtAdd(hVal, tempNode, __q931(algorithmOID), 0, NULL, NULL);
        pvtAdd(hVal, tempNode, __q931(paramS), 0, NULL, NULL);
        pvtAdd(hVal, tempNode, __q931(signature), 1, "\0", NULL);

        *dhToken = tempNode;

        /* go to hashed values */
        tempNode = pvtAddBranch(hVal, tempNode, __q931(toBeSigned));
        pvtAdd(hVal, tempNode, __q931(tokenOID), (RvInt32)objects.length[OID_Q], objects.OID[OID_Q], NULL);
        pvtAdd(hVal,tempNode,__q931(sendersID), h235App->senderId.length, (const char*)h235App->senderId.data, NULL);

        /* insert DH key into message */
        tempNode = pvtAddBranch(hVal, tempNode, __q931(dhkey));
        pvtAdd(hVal, tempNode, __q931(halfkey),   1024, (char*)entityEntry->outDHinf.halfkey+128,   NULL);
        pvtAdd(hVal, tempNode, __q931(modSize),   1024, (char*)entityEntry->outDHinf.modSize+128,   NULL);
        pvtAdd(hVal, tempNode, __q931(generator), 8, (char*)entityEntry->outDHinf.generator+255, NULL);
    }

    return tokenNode;
}


/***********************************************************************************
 * Routine Name:  generateDiffieHellman
 * Description : generate the Diffie-Hellman HalfKey
 * Input:  h235App    - pointer to h235 structure.
 *         DHMode     - structure containing a Diffie-Hellman HalfKey.
 *         call       - pointer to h235 call info structure.
 * Output: none.
 * Return: 0 - 0k , -1 - failed
 ***********************************************************************************/
RvStatus generateDiffieHellman(
    IN  h235Element_T*  h235App,
    IN  h235DHMode_t    DHMode,
    IN  h235DiffieHellmanInfo_t* inDHinf,
    IN  h235DiffieHellmanInfo_t* outDHinf,
    IN  RvUint16 myRandomNumber[MAX_BUFFNUM_SIZE])
{
    RvUint16 prime[MAX_BUFFNUM_SIZE];
    RvUint16 PublicValue[MAX_BUFFNUM_SIZE];
    RvUint16 * usedPrime = NULL;
    int primeLength = 0, i, j;

    if (inDHinf->isValid && (inDHinf->DHMode != DHMode))
        return RV_ERROR_UNKNOWN;

    /* set the length and prime according to protocol needed */
    if (DHMode == h235ModeDiffieHellman512)
    {
        primeLength = 32; /* two-byte, = 512 bit */
        outDHinf->DHMode = h235ModeDiffieHellman512;
    }
    else if (DHMode == h235ModeDiffieHellman1024)
    {
        usedPrime = g_1024Prime;
        primeLength = 64; /* two-byte, = 1024 bit */
        outDHinf->DHMode = h235ModeDiffieHellman1024;
    }
    else if (DHMode == h235ModeDiffieHellman1536)
    {
        usedPrime = g_1536Prime;
        primeLength = 96; /* two-byte, = 1536 bit */
        outDHinf->DHMode = h235ModeDiffieHellman1536;
    }
    else return RV_ERROR_UNKNOWN;

    /* clear values */
    memset(myRandomNumber, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
    memset(prime, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
    memset(PublicValue, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));

    /* calculate Public Value: */

    /* get the prime number */
    if(usedPrime == NULL)
    {
        /* if we got a prime from the other side, use it */
        if (inDHinf->isValid)
        {
            for(i=0, j=255; i<MAX_BUFFNUM_SIZE && j>=0; i++, j-=2)
            {
                prime[i] = (RvUint16) ((inDHinf->modSize[j-1] << 8) | inDHinf->modSize[j]);
            }
        }
        else
        {
            /* we'll need a prime */
            if(h235App->generatePrime != NULL)
            {
                unsigned char userPrime[64];
                memset(userPrime, 0, sizeof(userPrime));

                /* get prime from user - we expect MSB first*/
                h235App->generatePrime(NULL, NULL, primeLength*2, userPrime, NULL);

                /* arrange it by our order */
                for(i=0, j=((primeLength*2)-1); i<MAX_BUFFNUM_SIZE && j>=0; i++, j-=2)
                {
                    prime[i] = (RvUint16) ((userPrime[j] << 8) | userPrime[j-1]);
                }
            }
            else
                rvh323BuffnumGeneratePrime((cmElem *)h235App->hApp, prime, primeLength);
        }
        usedPrime = prime;
    }
    /* get a private value */
    rvh323BuffnumRandomizeLimited((cmElem *)h235App->hApp, myRandomNumber, usedPrime, NULL, primeLength);

    /* check if we received a generator number from the other side */
    if (inDHinf->isValid)
    {
        RvUint16 generator[MAX_BUFFNUM_SIZE];
        /* use the incoming generator */
        memset(generator, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
        memcpy(outDHinf->generator, inDHinf->generator, sizeof(outDHinf->generator));
        for(i=0, j=255; i<MAX_BUFFNUM_SIZE && j>=0; i++, j-=2)
        {
            generator[i] = (RvUint16) ((outDHinf->generator[j-1] << 8) | outDHinf->generator[j]);
        }
        /* PublicValue = Generator^(PrivateValue) % Prime */
        rvh323BuffnumBaseExpMod(PublicValue, generator, myRandomNumber, usedPrime);
    }
    else
    {
        /* set the generator to 2 (first octet has the most significance) */
        memset(outDHinf->generator, 0, 256);
        outDHinf->generator[255] = 0x02;
        /* PublicValue = 2^(PrivateValue) % Prime */
        rvh323BuffnumTwoExpMod(PublicValue, myRandomNumber, usedPrime);
    }

    /* set the halfkey to the public key (first octet has the most significance) */
    memset(outDHinf->halfkey, 0, 256);
    for(i=255, j=0; i>=0 && j<MAX_BUFFNUM_SIZE; i-=2, j++)
    {
        outDHinf->halfkey[i] = (RvUint8) PublicValue[j];
        outDHinf->halfkey[i-1] = (RvUint8) (PublicValue[j] >> 8);
    }

    /* set the modSize to the prime used (first octet has the most significance) */
    memset(outDHinf->modSize, 0, 256);
    for(i=255, j=0; i>=0 && j<MAX_BUFFNUM_SIZE; i-=2, j++)
    {
        outDHinf->modSize[i] = (RvUint8) usedPrime[j];
        outDHinf->modSize[i-1] = (RvUint8) (usedPrime[j] >> 8);
    }

    outDHinf->isValid = RV_TRUE;
    return RV_OK;
}

RvStatus genarateDHSharedSecret(
    IN  h235DiffieHellmanInfo_t* dhInfo,
    IN  RvBool        leastSignificantFirst,
    IN  RvUint16      randomNumber[MAX_BUFFNUM_SIZE],
    OUT char *        sharedSecret)
{
    RvUint16 inPV[MAX_BUFFNUM_SIZE];
    RvUint16 prime[MAX_BUFFNUM_SIZE];
    RvUint16 SS[MAX_BUFFNUM_SIZE];
    RvUint16 * usedPrime = NULL;
    int i, j, ssLen;

	//{{gaoshizhong 20131125 Add
	int nCopyLen=1024 / 8;   //SharedSecret generate length
	//}}gaoshizhong 20131125 Add

    memset(inPV,  0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
    memset(SS,    0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
    memset(prime, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));

    /* get the incoming Public Value */
    for(i=0, j=255; i<MAX_BUFFNUM_SIZE && j>=0; i++, j-=2)
    {
        inPV[i] = (RvUint16) (dhInfo->halfkey[j] | (dhInfo->halfkey[j-1] << 8));
    }

    /* get the prime used */
    if (dhInfo->DHMode == h235ModeDiffieHellman1024)
    {
        usedPrime = g_1024Prime;
        ssLen = 21;
		nCopyLen=1024 / 8;
    }
    else if (dhInfo->DHMode == h235ModeDiffieHellman1536)
    {
        usedPrime = g_1536Prime;
        ssLen = 21;
		nCopyLen=1536 / 8;
    }
    else
    {
        for(i=0, j=255; i<MAX_BUFFNUM_SIZE && j>=0; i++, j-=2)
        {
            prime[i] = (RvUint16) (dhInfo->modSize[j] | (dhInfo->modSize[j-1] << 8));
        }
        usedPrime = prime;
        ssLen = 7;
		nCopyLen=512 / 8;
    }

    /* now, for the shared secret calculation
    SharedSecret = inPV^myNumber % prime */
    rvh323BuffnumBaseExpMod(SS, inPV, randomNumber, usedPrime);

	//{{gaoshizhong 20131125 Add

    //if(leastSignificantFirst)
    //{
    //    for(i=0, j=0; (i<2*MAX_BUFFNUM_SIZE) && (j<ssLen); i++, j++)
    //    {
    //        if (!(i%2))
    //            sharedSecret[j] = (char) SS[i/2];
    //        else
    //            sharedSecret[j] = (char) (SS[i/2] >> 8);
    //    }
    //}
    //else
    //{
    //    for(i=0, j=(ssLen-1); (i<2*MAX_BUFFNUM_SIZE) && (j>=0); i++, j--)
    //    {
    //        if (i%2)
    //            sharedSecret[j] = (char) SS[i/2];
    //        else
    //            sharedSecret[j] = (char) (SS[i/2] >> 8);
    //    }
    //}

	{
		unsigned char shareKey[MAX_BUFFNUM_SIZE*2+8]={0};
		ConvertUint16ToChar(shareKey,SS,MAX_BUFFNUM_SIZE);
		memcpy(sharedSecret,shareKey+ 256 - nCopyLen,nCopyLen);
	}
	//}}gaoshizhong 20131125 Add

    return RV_OK;
}


RvStatus setDHP1TokenNode(
    IN  HPVT                        hVal,
    IN  h235DHMode_t                dhMode,
    IN  RvPvtNodeId                 nodeId,
    IN  h235DiffieHellmanInfo_t*    dhInf)
{
    RvPvtNodeId tempNode, tokenNode;
    objectOidType OID;

    if(!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_ERROR_UNKNOWN;

    if (dhMode & h235ModeDiffieHellman512)
        OID = OID_DH_512_1;
    else if (dhMode & h235ModeDiffieHellman1024)
        OID = OID_DH_1024_2;
    else if (dhMode & h235ModeDiffieHellman1536)
        OID = OID_DH_1536;
    else return RV_ERROR_UNKNOWN;

    __pvtBuildByFieldIds(tempNode, hVal, nodeId,
        {_q931(nestedcryptoToken) _q931(cryptoHashedToken) LAST_TOKEN}, 0, NULL);

    /* add the appropriate token OID */
    pvtAdd(hVal, tempNode, __q931(tokenOID), (RvInt32)objects.length[OID], objects.OID[OID], NULL);

    /* build an empty token */
    tokenNode = pvtAdd(hVal, tempNode, __q931(token), 0, NULL, NULL);
    pvtAdd(hVal, tokenNode, __q931(algorithmOID), 0, NULL, NULL);
    pvtAdd(hVal, tokenNode, __q931(paramS), 0, NULL, NULL);
    pvtAdd(hVal, tokenNode, __q931(hash), 1, "\0", NULL);

    /* go to hashed values */
    tempNode = pvtAddBranch(hVal, tempNode, __q931(hashedVals));
    pvtAdd(hVal, tempNode, __q931(tokenOID), (RvInt32)objects.length[OID], objects.OID[OID], NULL);

    /* insert DH key into message */
    tempNode = pvtAddBranch(hVal, tempNode, __q931(dhkey));
    if (dhMode & h235ModeDiffieHellman512)
    {
        pvtAdd(hVal, tempNode, __q931(halfkey),   512, (char*)dhInf->halfkey+192,   NULL);
        pvtAdd(hVal, tempNode, __q931(modSize),   512, (char*)dhInf->modSize+192,   NULL);
        pvtAdd(hVal, tempNode, __q931(generator), 8, (char*)dhInf->generator+255, NULL);
    }
    else if (dhMode & h235ModeDiffieHellman1024)
    {
        pvtAdd(hVal, tempNode, __q931(halfkey),   1024, (char*)dhInf->halfkey+128,   NULL);
        pvtAdd(hVal, tempNode, __q931(modSize),   1024, (char*)dhInf->modSize+128,   NULL);
        pvtAdd(hVal, tempNode, __q931(generator), 8, (char*)dhInf->generator+255, NULL);
    }
    else if (dhMode & h235ModeDiffieHellman1536)
    {
        pvtAdd(hVal, tempNode, __q931(halfkey),   1536, (char*)dhInf->halfkey+64,   NULL);
        pvtAdd(hVal, tempNode, __q931(modSize),   1536, (char*)dhInf->modSize+64,   NULL);
        pvtAdd(hVal, tempNode, __q931(generator), 8, (char*)dhInf->generator+255, NULL);
    }
    else return RV_ERROR_UNKNOWN;

    /* all done */
    return RV_OK;
}


RvStatus setDHClTokenNode(
    IN  HPVT                        hVal,
    IN  h235DHMode_t                dhMode,
    IN  RvPvtNodeId                 nodeId,
    IN  h235DiffieHellmanInfo_t*    dhInf)
{
    RvPvtNodeId tempNode;
    objectOidType OID;

    if(!RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_ERROR_UNKNOWN;

    if (dhMode & h235ModeDiffieHellman512)
        OID = OID_DH_512_1;
    else if (dhMode & h235ModeDiffieHellman1024)
        OID = OID_DH_1024_2;//        OID = OID_TEMP;
    else if (dhMode & h235ModeDiffieHellman1536)
        OID = OID_DH_1536;
    else return RV_ERROR_UNKNOWN;

    pvtAdd(hVal, nodeId, __q931(tokenOID), (RvInt32)objects.length[OID], objects.OID[OID], NULL);

    /* insert DH key into message */
    tempNode = pvtAddBranch(hVal, nodeId, __q931(dhkey));
    if (dhMode & h235ModeDiffieHellman512)
    {
        pvtAdd(hVal, tempNode, __q931(halfkey),   512, (char*)dhInf->halfkey+192,   NULL);
        pvtAdd(hVal, tempNode, __q931(modSize),   512, (char*)dhInf->modSize+192,   NULL);
        pvtAdd(hVal, tempNode, __q931(generator), 8, (char*)dhInf->generator+255, NULL);
    }
    else if (dhMode & h235ModeDiffieHellman1024)
    {
        pvtAdd(hVal, tempNode, __q931(halfkey),   1024, (char*)dhInf->halfkey+128,   NULL);
        pvtAdd(hVal, tempNode, __q931(modSize),   1024, (char*)dhInf->modSize+128,   NULL);
        pvtAdd(hVal, tempNode, __q931(generator), 8, (char*)dhInf->generator+255, NULL);
    }
    else if (dhMode & h235ModeDiffieHellman1536)
    {
        pvtAdd(hVal, tempNode, __q931(halfkey),   1536, (char*)dhInf->halfkey+64,   NULL);
        pvtAdd(hVal, tempNode, __q931(modSize),   1536, (char*)dhInf->modSize+64,   NULL);
        pvtAdd(hVal, tempNode, __q931(generator), 8, (char*)dhInf->generator+255, NULL);
    }
    else return RV_ERROR_UNKNOWN;

    /* all done */
    return RV_OK;
}

RvStatus getDHTokenNode(
    IN    HPVT                      hVal,
    IN    RvPvtNodeId               cryptoTokenNode,
    IN    RvPvtNodeId               clearTokenNode,
    OUT   h235DiffieHellmanInfo_t*  dhInf)
{
    RvPvtNodeId dhkeyNode, child;
    RvInt32 len = 0;

    if (!RV_PVT_NODEID_IS_VALID(clearTokenNode))
    {
        /* try the Annex D way */
        __pvtGetNodeIdByFieldIds(clearTokenNode, hVal, cryptoTokenNode,
            {_q931(nestedcryptoToken) _q931(cryptoHashedToken) _q931(hashedVals) LAST_TOKEN});

        if (!RV_PVT_NODEID_IS_VALID(clearTokenNode))
        {
            /* ok, try the Annex E way */
            __pvtGetNodeIdByFieldIds(clearTokenNode, hVal, cryptoTokenNode,
                {_q931(nestedcryptoToken) _q931(cryptoSignedToken) _q931(token) _q931(toBeSigned) LAST_TOKEN});
            if (!RV_PVT_NODEID_IS_VALID(clearTokenNode))
                return RV_ERROR_UNKNOWN;
        }
    }
    dhkeyNode = pvtGetChild(hVal, clearTokenNode, __q931(dhkey), NULL);

    /* get the values */
    memset(dhInf,  0, sizeof(*dhInf));

    child = pvtGetChildByFieldId(hVal, dhkeyNode, __q931(halfkey), &len, NULL);
    pvtGetBitString(hVal, child, len, (char*)dhInf->halfkey+256-len);

    child = pvtGetChildByFieldId(hVal, dhkeyNode, __q931(modSize), &len, NULL);
    pvtGetBitString(hVal, child, len, (char*)dhInf->modSize+256-len);

    child = pvtGetChildByFieldId(hVal, dhkeyNode, __q931(generator), &len, NULL);
    pvtGetBitString(hVal, child, len, (char*)dhInf->generator+256-len);

    dhInf->isValid = RV_TRUE;
    return RV_OK;
}


/***********************************************************************************
 * Routine Name:  calcP1HashValue
 * Description : calculate the HMAC-SH1-96 over the entire message.
 * Input: h235App   - pointer to the H.235 instance
 *        buffer    - the encoded message
 *        keyHandle - handle for the symmetric key
 * Output: digest - the result hash value with length of 96 bit.
 * Return: RvStatus
 ***********************************************************************************/
static RvStatus calcP1HashValue(
    IN  h235Element_T*  h235App,
    IN  char*           buffer,
    IN  int             buffLen,
    IN  H235HKEY        keyHandle,
    OUT unsigned char*  digest,
    OUT int*            digLen)
{
    if (h235App->evSHA1_96 == NULL)
        memset(digest, 0, 12);
    else
        h235App->evSHA1_96(keyHandle, buffer, buffLen, digest, digLen);
    return RV_OK;
}


/***********************************************************************************
 * Routine Name:  calcP2Signature
 * Description : calculate the SH1-RSA or MD5 over the entire message.
 * Input: h235App   - pointer to the H.235 instance
 *        buffer    - the encoded message
 *        buffLen   - length of the encoded message
 *        crtHandle - handle for the certificate
 *        type      - type of signature to used (MD5 or SH1-RSA)
 * Output: digest - the result signature with length of 1024 bit
 *         digLen - length the result signature (just to be safe)
 * Return: RvStatus
 ***********************************************************************************/
static RvStatus calcP2Signature(
    IN  h235Element_T*          h235App,
    IN  char*                   buffer,
    IN  int                     buffLen,
    IN  H235HKEY                crtHandle,
    OUT unsigned char*          digest,
    OUT int*                    digLen,
    IN  h235CertificateType_t   type)
{
    if (type & h235CertificateMD5)
    {
        if (h235App->evMD5_RSA_Sig == NULL)
        {
            memset(digest, 0, 128);
            return RV_ERROR_NOTSUPPORTED;
        }
        else
        {
            return h235App->evMD5_RSA_Sig(crtHandle, buffer, buffLen, digest, digLen);
        }
    }
    else
    {
        if (h235App->evSHA1_RSA_Sig == NULL)
        {
            memset(digest, 0, 128);
            return RV_ERROR_NOTSUPPORTED;
        }
        else
        {
            return h235App->evSHA1_RSA_Sig(crtHandle, buffer, buffLen, digest, digLen);
        }
    }
}


/***********************************************************************************
 * Routine Name:  findSubstring
 * Description : finds a substring in a string
 * Input: str -  the string
 *        strLen - string length
 *        strIdx - index to match from
 *        subStr -  the sub string
 *        subLen - sub string length
 *        subIdx - index to match from
 * Output: (-)
 * Return: The last index of the substring in the string or -1 if the substring was
 *         not found.
 ***********************************************************************************/
static int findSubstring(unsigned char *str, int strLen, int strIdx,
                         unsigned char *subStr, int subLen)
{
    /* sanity check */
    if (strIdx > (strLen-subLen))
    {
        return -1;
    }

    for (; (strIdx <= (strLen-subLen)); strIdx++)
    {
        if (str[strIdx] == subStr[0])
        {
            if (memcmp(&str[strIdx], subStr, (RvSize_t)subLen) == 0)
                return strIdx;
        }
    }

    return -1;
}



#ifdef __cplusplus
}
#endif
