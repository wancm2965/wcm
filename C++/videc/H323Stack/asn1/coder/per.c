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

/*
    per.c

    *****************************************************
    *** Encoding Rules are ASN.1 PER BASIC - ALIGNED. ***
    *****************************************************


    Encoding/Decoding of the following ASN complex types:
    - Sequence
    - Sequence OF
    - Set OF
    - Choice

    Encoding: MSGV --> bitBuf
    Decoding: bitBuf --> MSGV

    Algorithm:
    ----------

    > Encoding(msgv, buf, tree)

    - Clear tmp and func lists.
    - Clear buf.
    - Add root node to func list.
    - while (not end_of_func_list) do
    - Clear tmp list.
    - tmp <- Encode head of func list.
    - func <- tmp + func
    - endwhile


    > EncodeFuncListNode(msgv, buf, tree, node)

    - switch (type of node)
    - Encode node by type:
    - Add encoding bits to buf.
    - Add child nodes to tmp list as needed.


    > Decoding(msgv, buf, tree)
    This is done much the same way aw the encoding procedure. Only the input is taken from
    the bit buffer and the value tree is generated.


  Notes:
  ------
  1. NULL object has no bit encoding and so there shall be no node of NULL type in
  the value tree.
  2. This is a non - reentrent code. Using tmp list as global variable for the encoding/
  decoding process.
  3. The output data structures shall have enough space to contain the encoding/decoding
  result.
  4. ALthough the process is recursive, it is implemented by a queue(func list) that
  holds an ordered list of the nodes to be processed.


  Parameters:
  -----------
  asnH: asn handler.
  deleted: number of bits deleted from bit buffer after last processing procedure.
  msgv: Message Values handler. The value tree.
  buf: bit buffer. Holds the message in ASN PER encoding(Decoding). Has sufficient space to
  hold the complete encoding(Encoding).
  tree: The syntax tree structure.
  type: Encode/Decode, determines which kind of operation to do.
  func: function node. The current node to process.
  parent: path to node in value tree.
  structEnum: enumeration of structure in the syntax tree.
  index: of child. Used for set - of nodes.
  path: path of structure in the syntax tree.
  vtPath: path of current node in value tree.
*/

#include "rvmemory.h"
#include "rvlog.h"
#include "rvthread.h"
#include "rvstdio.h"

#include "h32xLog.h"
#include "coderbitbuffer.h"
#include "copybits.h"
#include "pvaltreeStackApi.h"
#include "psyntreeStackApi.h"
#include "emanag.h"

#include "per.h"
#include "perseq.h"
#include "perseqof.h"
#include "perchoic.h"
#include "perchrst.h"
#include "peroctst.h"
#include "perBitString.h"
#include "peroid.h"
#include "perintr.h"


#ifdef __cplusplus
extern "C" {
#endif

#define TMP_LOG H32xLogGet()

/********************** MACROS **********************/

#define nprn(s) ((s)?(s):"(null)")

#define PER_BB_SIZE(_octets) (sizeof(bbStruct) + _octets)



/*** static variables ***/
RvLogSource rvPerLogSource; /* PER log source */
RvLogSource rvPerErrLogSource; /* PERERR log source */

/* Maximum size of buffers for encoding/decoding */
static int         perMaxBufSize = 0;

/* todo: we should protect the counters in our program! */
static RvUint32   timesTlsInitialized = 0;
static RvUint32   rvPerTlsIndex;

static int perEncode(
    IN  emanagStruct    *emanag,
    IN  RvPvtNodeId     vNodeId,
    OUT int             *encoded);
static int perDecode(
    IN  emanagStruct    *emanag,
    IN  RvPvtNodeId     vNodeId,
    IN  RvInt32         fieldId,
    OUT int             *decoded);



/********************************************************************************************
 * coderFreeThreadBuffer
 * An exit callback of a specific thread for CODER.
 * This function frees the allocation of the encode/decode buffer
 * context  - The pointer to the buffer to free
 ********************************************************************************************/
static void coderFreeThreadBuffer(IN RvThread* thread, IN void* context, IN RvUint32 index)
{
    THREAD_CoderLocalStorage* coderTls = (THREAD_CoderLocalStorage *)context;
    RV_UNUSED_ARG(thread);
    RV_UNUSED_ARG(index);

    /* context is NULL - the "unauthorized" thread should not free any buffers */
    /* context is 1 - already deleted, it's only "marked" */
    if ((context == NULL) || ((RvInt)context == 1))
        return;

    if (coderTls->buffer != NULL)
        RvMemoryFree(coderTls->buffer, TMP_LOG);
    RvMemoryFree(coderTls, TMP_LOG);
}


/************************************************************************
 * perGetBuffer
 * purpose: Returns the buffer to use for PER encode/decode procedures
 * input  : none
 * output : none
 * return : Pointer to the buffer on success
 *          NULL on failure
 ************************************************************************/
static THREAD_CoderLocalStorage* perGetBuffer(void)
{
    THREAD_CoderLocalStorage*   coderTls;

    if ((timesTlsInitialized == 0) ||
        (RvThreadGetVar(rvPerTlsIndex, TMP_LOG, (void**)&coderTls) != RV_OK))
        return NULL;

    if ((coderTls == NULL) || ((RvInt)coderTls == 1))
    {
        RvMemoryAlloc(NULL, sizeof(THREAD_CoderLocalStorage), TMP_LOG, (void**)&coderTls);
        memset((void*)coderTls, 0, sizeof(THREAD_CoderLocalStorage));
        RvThreadSetVar(rvPerTlsIndex, (void*)coderTls, TMP_LOG);
    }

    if ((coderTls->buffer != NULL) && ((int)coderTls->bufferSize < perMaxBufSize))
    {
        /* Current allocation too small - make sure we fix this situation... */
        RvMemoryFree(coderTls->buffer, TMP_LOG);
        coderTls->buffer = NULL;
    }

    if (coderTls->buffer == NULL)
    {
        coderTls->bufferSize = (RvUint32)perMaxBufSize;
        if (RvMemoryAlloc(NULL, coderTls->bufferSize, TMP_LOG, (void**)&coderTls->buffer) != RV_OK)
            return NULL;
    }

    return coderTls;
}

/************************************************************************
 * perBufferInit
 * purpose: Create the local storage for the buffer to use for PER
 *          encode/decode procedures
 * input  : none
 * output : none
 * return : none
 ************************************************************************/
static void perBufferInit(void)
{
    if (timesTlsInitialized == 0)
    {
        timesTlsInitialized = 1;
        if (RvThreadCreateVar(coderFreeThreadBuffer, "RvAsnPER", TMP_LOG, &rvPerTlsIndex) != RV_OK)
            return;
        perGetBuffer();
    }
    else
        timesTlsInitialized++;
}

/************************************************************************
 * perBufferEnd
 * purpose: close down the local storage
 * input  : none
 * output : none
 * return : void
 ************************************************************************/
void perBufferEnd(void)
{
    THREAD_CoderLocalStorage*   coderTls;
    RvInt marking = 1; /* Mark the context as deleted */

    /* Calling the exit function of the variable while the thread is still active */
    if (RvThreadGetVar(rvPerTlsIndex, TMP_LOG, (void**)&coderTls) == RV_OK)
    {
        coderFreeThreadBuffer(NULL, (void*)coderTls, rvPerTlsIndex);
        RvThreadSetVar(rvPerTlsIndex, (void*)marking, TMP_LOG);
    }

    timesTlsInitialized--;
    if (timesTlsInitialized == 0)
        RvThreadDeleteVar(rvPerTlsIndex, TMP_LOG);
}

/*_____________________________________________________________________*/
/*_________________________________INFORMATION_________________________*/
/*_____________________________________________________________________*/

/*
Desc: put bits in head of dest buffer.
Returns: Number of bits extracted from buffer.
*/
RvInt32
bbGet2Left(IN  HPER     hPer,
           IN  RvUint32 fromBitPosition, /* in buffer */
           IN  RvUint32 numOfBitsToGet,
           OUT RvUint8* dest) /* destination buffer */
{
    perStruct *per =(perStruct *)hPer;
    bbStruct *bb =(bbStruct *)(per->hBB);

    if ((numOfBitsToGet > bb->bitsInUse - fromBitPosition) || (numOfBitsToGet > per->buf->bufferSize * 8))
    {
        per->encodingDecodingErrorBitMask = per->encodingDecodingErrorBitMask | encDecErrorsMessageIsInvalid;
        return RV_ERROR_UNKNOWN;
    }

    memcpyb(dest, 0, bb->octets, fromBitPosition, numOfBitsToGet);
    return numOfBitsToGet;
}


/*
Desc: put bits in tail of dest buffer.
Note: Length of dest buffer is the minimum number of bytes needed to hold
the numOfBitsToGet bits.
Returns: Number of bits extracted from buffer.
*/
RvInt32
bbGet2Right(HPER hPer,
            IN RvUint32 fromBitPosition, /* in buffer */
            IN RvUint32 numOfBitsToGet,
            OUT RvUint8 *dest) /* destination buffer */
{
    perStruct *per =(perStruct *)hPer;
    bbStruct *bb =(bbStruct *)(per->hBB);

    if ((numOfBitsToGet > bb->bitsInUse - fromBitPosition) || (numOfBitsToGet > MAX_INT_SIZE*8))
    {
        per->encodingDecodingErrorBitMask = per->encodingDecodingErrorBitMask | encDecErrorsMessageIsInvalid;
        return RV_ERROR_UNKNOWN;
    }

    memcpyb(dest, (8 - numOfBitsToGet%8)%8, bb->octets, fromBitPosition, numOfBitsToGet);
    return numOfBitsToGet;
}












/*______________________________open type____________________________________________*/


int
perEncodeOpenTypeBegin(
	IN  HPER hPer,
    IN  RvInt32 fieldId,
    OUT RvInt32 *offset)  /* beginning of open type encoding */
{
	RvUint8 ch = 0;
    perStruct *per =(perStruct *)hPer;

	RV_UNUSED_ARG(fieldId);

	/* Add an extra aligned byte - it's the least we will need. */
	if (bbAddTail(per->hBB, &ch, 8, RV_TRUE) >= 0)
	{
        RvLogDebug(&rvPerLogSource, (&rvPerLogSource,
            "Encoding OPENTYPE: reserved one byte for length."));
	}
	else
	{
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeOpenTypeBegin: reservation failed for '%s'.",
            pstGetFieldNamePtr(per->hSyn, fieldId)));
        return RV_ERROR_UNKNOWN;
	}

	*offset = bbBitsInUse(per->hBB);

	return RV_TRUE;
}


int
perEncodeOpenTypeEnd(
	IN  HPER hPer,
    IN  RvInt32 offset,  /* beginning of open type encoding */
    IN  RvInt32 fieldId)
{
    perStruct *per =(perStruct *)hPer;
    RvUint8 octets[PER_BB_SIZE(10)]; /* space for tmp encoding */
    RvInt32 encLen;
	RvUint32 bytesLen;
    HBB tmpBB; /* for length encoding */
    RvInt32 tmpBBlen;

    RV_UNUSED_ARG(fieldId);

	encLen = bbBitsInUse(per->hBB) - offset; /* encoding length */
	if (encLen > 0)
	{
		encLen = bbAlignTail(per->hBB) - offset;
	}
	else
	{
		/* Open type field must be at least one byte long. If it's not - force it to be */
		RvUint8 ch = 0;
		if (bbAddTail(per->hBB, &ch, 8, RV_FALSE) < 0)
		{
			RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
				"perEncodeOpenTypeEnd: single byte addition failed '%s'.",
				pstGetFieldNamePtr(per->hSyn, fieldId)));
			return RV_ERROR_UNKNOWN;
		}
		encLen = 8;
	}

	bytesLen = bbSetByte((RvUint32)encLen);
	if (bytesLen <= 127)
	{
		/* One byte only - we reserved enough room for it */
		RvUint8 ch = (RvUint8)bytesLen;
		bbSet(per->hBB, (RvUint32)offset - 8, 8, &ch);
		return RV_TRUE;
	}

    /* No easy way - we must calculate the size this time on our own */
	tmpBB = bbConstructFrom(10, (char *)octets, sizeof(octets));

    /* -- 10.2.2: length in octets */
    if (perEncodeLen(perLenTypeUNCONSTRAINED, bbSetByte((RvUint32)encLen), 0, 0, tmpBB) <0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeOpenType: length encoding failed for '%s' [%d].",
            pstGetFieldNamePtr(per->hSyn, fieldId), encLen));
        return RV_ERROR_UNKNOWN;
    }

    /* -- add length before open field encoding */
    /* move encoding tmpBBlen bits ahead */
    tmpBBlen = bbBitsInUse(tmpBB);
    if (bbMove(per->hBB, (RvUint32)offset, (RvUint32)encLen, (RvUint32)(offset + tmpBBlen - 8)) < 0)
        return RV_ERROR_UNKNOWN;
    if (bbSet(per->hBB, (RvUint32)offset - 8, (RvUint32)tmpBBlen, bbOctets(tmpBB)) >= 0)
    {
        RvLogDebug(&rvPerLogSource, (&rvPerLogSource,
            "Encoding OPENTYPE: inserted %d bits in [%d].", tmpBBlen, offset-8));
    }
    else
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeOpenType: set failed for '%s' [%d].",
            pstGetFieldNamePtr(per->hSyn, fieldId), encLen));
        return RV_ERROR_UNKNOWN;
    }

    return RV_TRUE;
}





/*____________________________________________________________________________*/
/*___________________________NODE_____________________________________________*/
/*____________________________________________________________________________*/

/* Desc: encode a node in the static syntax tree.
Logic: switch between types.
*/
int
perEncNode(IN  HPER hPer,
           IN  int synParent,
           IN  int valParent,
           IN  RvInt32 fieldId,
           IN  RvBool wasTypeResolvedInRunTime)
{
    perStruct *per =(perStruct *)hPer;
    RvInt32 value=-1;
    int ret = RV_ERROR_UNKNOWN;
    RvInt32 offset=-1;
    pstNodeType type;

    if (((int)(type = pstGetNodeType(per->hSyn, synParent))) < 0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeNode: Illegal syntax node: [%s:%d].",
            pstGetFieldNamePtr(per->hSyn, fieldId), synParent));
        return RV_ERROR_UNKNOWN;
    }

    if (pvtGet(per->hVal, valParent, NULL, NULL, &value, NULL) <0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeNode: Illegal value node: [%s:%d].",
            pstGetFieldNamePtr(per->hSyn, fieldId), valParent));
        return RV_ERROR_UNKNOWN;
    }

    RvLogDebug(&rvPerLogSource, (&rvPerLogSource,
        "Encoding %s: %s [%d].",
        (fieldId >= 0) ? pstGetFieldNamePtr(per->hSyn, fieldId):"(null)",
        nprn(pstGetTokenName(type)),
        bbBitsInUse(per->hBB)));

    if (wasTypeResolvedInRunTime)
    {
        if (perEncodeOpenTypeBegin(hPer, fieldId, &offset) < 0)
            return RV_ERROR_UNKNOWN;
    }

    switch (type)
    {
        case pstNull:
            ret = RV_TRUE;
            break;
        case pstInteger:
            {
                int to, from;
                RvBool toAbsent, fromAbsent;

                pstGetNodeRangeExt(per->hSyn,synParent,&from,&to,&fromAbsent,&toAbsent);
                ret = perEncodeInt((RvUint32)value, (RvUint32)from, (RvUint32)to, fromAbsent, toAbsent,
                    (RvBool)pstGetIsExtended(per->hSyn,synParent), per->hBB);
            }
            break;
        case pstBoolean:
            ret = perEncodeBool(((value) ? (RV_TRUE):(RV_FALSE)), per->hBB);
            break;

        case pstUniversalString:
        case pstGeneralString:
        case pstBMPString:
        case pstIA5String:
        case pstNumericString:
        case pstPrintableString:
        case pstVisibleString:
            ret = perEncodeCharString(hPer, synParent, valParent, fieldId);
            break;
        case pstOctetString:
            ret = perEncodeOctetString(hPer, synParent, valParent, fieldId);
            break;

        case pstBitString:
            ret = perEncodeBitString(hPer, synParent, valParent, fieldId);
            break;

        case pstObjectIdentifier:
            ret = perEncodeOID(hPer, synParent, valParent, fieldId);
            break;

        case pstEnumeration:
        case pstChoice:
            ret = perEncodeChoice(hPer, synParent, valParent, fieldId);
            break;

        case pstSet:
        case pstSequence:
            ret = perEncodeSequece(hPer, synParent, valParent, fieldId);
            break;

        case pstSetOf:
        case pstSequenceOf:
            ret = perEncodeSequeceOF(hPer, synParent, valParent, fieldId);
            break;
        default:
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perEncodeNode: %s TYPE unrecognized: %s.",
                pstGetFieldNamePtr(per->hSyn, fieldId),
                nprn(pstGetTokenName(type))));
            return RV_ERROR_UNKNOWN;
    }

    if ((ret >= 0) && wasTypeResolvedInRunTime)
    {
        ret = perEncodeOpenTypeEnd(hPer, offset, fieldId);
    }

    if (ret < 0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeNode: '%s' %s encoding Error!.",
            pstGetFieldNamePtr(per->hSyn, fieldId),
            nprn(pstGetTokenName(type))));
        return RV_ERROR_UNKNOWN;
    }

    return RV_TRUE;
}



#if (RV_ASN1_CODER_USE_H450 == RV_YES)

static int perPushElemToArrayOfSpecialNodes(
                                     IN HPER hPer,
                                     IN int element
                                     )
{
    perStruct *per =(perStruct *)hPer;
    if (per->currentPositionInArrayOfSpecialNodes == MAX_SPECIAL_NODES)
        return RV_ERROR_UNKNOWN;
    per->arrayOfSpecialNodes[per->currentPositionInArrayOfSpecialNodes++] = element;
    return RV_TRUE;
}


static int perPopElemFromArrayOfSpecialNodes(
                                      IN HPER hPer
                                      )
{
    perStruct *per =(perStruct *)hPer;
    if (per->currentPositionInArrayOfSpecialNodes == 0)
        return RV_ERROR_UNKNOWN;
    per->currentPositionInArrayOfSpecialNodes--;
    return RV_TRUE;
}


static int perGetDependentTypeForDecoding(
                                   IN HPER hPer,
                                   IN int specialTypeNodeId, /* nodeid of "special" type */
                                   OUT int* dependentTypeNodeId /* nodeid of "real" type */
                                   )
{
    int i, objectId;
    pstTypeFromConstraint specialType;
    pstChild field;
    pstConstrainingField constrainingField;
    pstFieldOfObjectReference fieldOfObject;
    perStruct *per =(perStruct *)hPer;

    /* getting special type */
    pstGetTypeFromConstraint(per->hSyn, specialTypeNodeId, &specialType);

    /* getting constraining field */
    pstGetConstrainingField(per->hSyn, specialTypeNodeId, &constrainingField);

    /* getting field itself(node) */
    /* here very intimate knowledge of the internal database is used*/
    /* it is known that fields are placed at nodeId+fieldIndex position */

    pstGetChild(per->hSyn, 0, constrainingField.fieldId, &field);

    if (pvtFindObject(per->hVal,
           per->arrayOfSpecialNodes[per->currentPositionInArrayOfSpecialNodes - 1 -
                                    constrainingField.relativeIndex],
           per->hSyn, specialType.objectSetId, &objectId) == RV_ERROR_UNKNOWN)      /* finding object */
           /* object was not found */
           return (RV_PST_ERROR_UNDEFINED_TYPE);


    for (i = 1;; i++)                                                           /* finding field of object */
    {
        if (pstGetFieldOfObjectReference(per->hSyn, objectId, i, &fieldOfObject) < 0)
            return RV_ERROR_UNKNOWN;
        if (fieldOfObject.fieldInClassNumber == specialType.fieldInClassNumber)
            break;
    }

    if (dependentTypeNodeId)
        *dependentTypeNodeId = fieldOfObject.settingId;
    return fieldOfObject.settingId;
}

#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */



int perDecNode(
    IN  HPER         hPer,
    IN  pstChildExt  *synInfo,
    IN  int         valParent)
{
    perStruct *per =(perStruct *)hPer;
    RvUint32 value = 0;
    int ret = RV_ERROR_UNKNOWN;
    RvUint32 dec = 0;
    RvBool boola = RV_FALSE, wasTypeResolvedInRunTime = RV_FALSE;
    RvInt32 saveLocation=-1;
    RvUint32 fieldLen = 0;
    pstNodeType type = (pstNodeType)0;
    int  to=0, from=0;
    RvBool toAbsent=RV_TRUE, fromAbsent=RV_TRUE;
    int synParent = synInfo->nodeId;
    int fieldId = synInfo->fieldId;

    if (valParent < 0)
    {
        per->encodingDecodingErrorBitMask |= encDecErrorsMessageIsInvalid;
        return RV_ERROR_UNKNOWN;
    }

#if (RV_ASN1_CODER_USE_H450 == RV_YES)
    if (synInfo->speciality == pstDependingDependent || synInfo->speciality == pstDependent)
    {
        if ((synParent = perGetDependentTypeForDecoding(hPer, synParent, &synParent)) == RV_ERROR_UNKNOWN)
            return RV_ERROR_UNKNOWN;
        wasTypeResolvedInRunTime = RV_TRUE;

        if (synParent == RV_PST_ERROR_UNDEFINED_TYPE)
        {
            /* Seems like H.450 message contains parts we can't understand. */
            RvLogWarning(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeNode: Uknown dependent type: [%s:%d].",
                pstGetFieldNamePtr(per->hSyn, fieldId), synParent));
        }
    }
#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */

    if (synParent != RV_PST_ERROR_UNDEFINED_TYPE)
    {
        pstGetNodeRangeExt(per->hSyn,synParent,&from,&to,&fromAbsent,&toAbsent);
        type = pstGetNodeType(per->hSyn, synParent);

        if (((int)type) < 0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeNode: Illegal syntax node: [%s:%d].",
                pstGetFieldNamePtr(per->hSyn, fieldId), synParent));
            return RV_ERROR_UNKNOWN;
        }

        RvLogDebug(&rvPerLogSource, (&rvPerLogSource,
            "Decoding %s: %s [%d].",
            pstGetFieldNamePtr(per->hSyn, fieldId), nprn(pstGetTokenName(type)),
            (int)per->decodingPosition));
    }

    if (wasTypeResolvedInRunTime)
    {
        if (perDecodeLen(perLenTypeUNCONSTRAINED, &fieldLen, 0, 0, hPer, per->decodingPosition, &dec) <0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecNode: open type field length missing [%s].",
                pstGetFieldNamePtr(per->hSyn, fieldId)));
            return RV_ERROR_UNKNOWN;
        }

        RvLogDebug(&rvPerLogSource, (&rvPerLogSource,
            "Decoding open type."));
        per->decodingPosition += dec;
        saveLocation = per->decodingPosition;
    }

    if (synParent != RV_PST_ERROR_UNDEFINED_TYPE)
    {
        switch (type)
        {
        case pstNull:
            if (fieldId!=RV_ERROR_UNKNOWN)
            ret = pvtAdd(per->hVal, valParent, fieldId, 0, NULL, NULL);
            break;
        case pstInteger:
            ret = perDecodeInt(&value, (RvUint32)from, (RvUint32)to, fromAbsent, toAbsent,
                                (RvBool)pstGetIsExtended(per->hSyn, synParent),
                                hPer, per->decodingPosition, &dec, "integer");
            if ((fieldId!=RV_ERROR_UNKNOWN) && (ret >= 0))
                ret = pvtAdd(per->hVal, valParent, fieldId, (RvInt32)value, NULL, NULL);
            per->decodingPosition += dec;
            break;
        case pstBoolean:
            ret = perDecodeBool(&boola, hPer, per->decodingPosition, &dec);
            if ((fieldId!=RV_ERROR_UNKNOWN) && (ret >= 0))
                ret = pvtAdd(per->hVal, valParent, fieldId, boola,  NULL, NULL);
            per->decodingPosition += dec;
            break;

        case pstUniversalString:
        case pstGeneralString:
        case pstBMPString:
        case pstIA5String:
        case pstNumericString:
        case pstPrintableString:
        case pstVisibleString:
            ret = perDecodeCharString(hPer, synParent, valParent, fieldId);
            break;

        case pstOctetString:
            ret = perDecodeOctetString(hPer, synParent, valParent, fieldId);
            break;

        case pstBitString:
            ret = perDecodeBitString(hPer, synParent, valParent, fieldId);
            break;
        case pstObjectIdentifier:
            ret = perDecodeOID(hPer, synParent, valParent, fieldId);
            break;
        case pstEnumeration:
        case pstChoice:
#if (RV_ASN1_CODER_USE_H450 == RV_YES)
            {
                int i, startListLength, finishListLength;
                startListLength = per->currentPositionInArrayOfSpecialNodes;
                ret = perDecodeChoice(hPer, synParent, valParent, fieldId);
                finishListLength = per->currentPositionInArrayOfSpecialNodes;
                for (i = 0; i < finishListLength - startListLength; i++)
                    perPopElemFromArrayOfSpecialNodes(hPer);
            }
#else
            ret = perDecodeChoice(hPer, synParent, valParent, fieldId);
#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */
            break;
        case pstSet:
        case pstSequence:
#if (RV_ASN1_CODER_USE_H450 == RV_YES)
            {
                int i, startListLength, finishListLength;
                startListLength = per->currentPositionInArrayOfSpecialNodes;
                ret = perDecodeSequece(hPer, synParent, valParent, fieldId);
                finishListLength = per->currentPositionInArrayOfSpecialNodes;
                for (i = 0; i < finishListLength - startListLength; i++)
                    perPopElemFromArrayOfSpecialNodes(hPer);
            }
#else
            ret = perDecodeSequece(hPer, synParent, valParent, fieldId);
#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */
            break;
        case pstSequenceOf:
        case pstSetOf:
            ret = perDecodeSequeceOF(hPer, synParent, valParent, fieldId);
            break;
        case pstNotSupported:
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeNode: %s TYPE not supported: %s.",
                pstGetFieldNamePtr(per->hSyn, fieldId),
                nprn(pstGetTokenName(type))));
            return RV_ERROR_NOTSUPPORTED;
        default:
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeNode: %s TYPE unrecognized: %s.",
                pstGetFieldNamePtr(per->hSyn, fieldId),
                nprn(pstGetTokenName(type))));
            return RV_ERROR_UNKNOWN;
        } /* end of switch */
    }
    /* synParent == RV_PST_ERROR_UNDEFINED_TYPE
    else
    {
        if (fieldId!=RV_ERROR_UNKNOWN)
        ret = pvtAdd(per->hVal, valParent, fieldId, 555, NULL, NULL);
    }
    */

    if (wasTypeResolvedInRunTime)
        per->decodingPosition = saveLocation + fieldLen*8; /* skip field */

    if (synParent != RV_PST_ERROR_UNDEFINED_TYPE && ret < 0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeNode: '%s' %s decoding Error!.",
            pstGetFieldNamePtr(per->hSyn, fieldId),
            nprn(pstGetTokenName(type))));

        switch (ret)
        {
            case RV_ERROR_UNKNOWN:
                per->encodingDecodingErrorBitMask |= encDecErrorsMessageIsInvalid;
                break;
            case RV_ERROR_OUTOFRESOURCES:
                per->encodingDecodingErrorBitMask |= encDecErrorsResourcesProblem;
                break;
            default:
                break;
        }

        return RV_ERROR_UNKNOWN;
    }

#if (RV_ASN1_CODER_USE_H450 == RV_YES)
    if (synInfo->speciality == pstDependingDependent || synInfo->speciality == pstDepending)
        perPushElemToArrayOfSpecialNodes(hPer, ret);
#endif /* (RV_ASN1_CODER_USE_H450 == RV_YES) */

    if (synParent == RV_PST_ERROR_UNDEFINED_TYPE)
        per->encodingDecodingErrorBitMask |= encDecErrorsObjectWasNotFound;

    return RV_TRUE;
}





/************************************************************************
 * perConstruct
 * purpose: Construct PER information needed by the encode/decode manager.
 * input  : maxBufSize  - Maximum size of buffer supported (messages larger
 *                        than this size in bytes cannot be decoded/encoded).
 * output : none
 * return : none
 ************************************************************************/
void perConstruct(IN int maxBufSize)
{
    emTypeOfEncoding eSys;

    eSys.Encode = perEncode;
    eSys.Decode = perDecode;

    /* First time we call perConstruct() - make sure we register our log sources */
    RvLogSourceConstruct(TMP_LOG, &rvPerLogSource, "PER", "PER Encoder/Decoder");
    RvLogSourceConstruct(TMP_LOG, &rvPerErrLogSource, "PERERR", "PER Error Messages");

    emSetEncoding((int)emPER, &eSys);

    /* Make sure we create a buffer with sufficient size for this thread */
    if (perMaxBufSize < maxBufSize)
        perMaxBufSize = maxBufSize;
    perBufferInit();
}


/*
Desc: Free per allocations.
Free func list and structure.
*/
RVINTAPI void RVCALLCONV
perDestruct(void)
{
    RvLogSourceDestruct(&rvPerLogSource);
    RvLogSourceDestruct(&rvPerErrLogSource);

    perBufferEnd();
}


/*
Desc: Encode a message from root.
*/
static int perEncode(
    IN  emanagStruct    *emanag,
    IN  RvPvtNodeId     vNodeId,
    OUT int             *encoded)
{
    perStruct per;
    RvUint8 octets[PER_BB_SIZE(0)];
    HBB bbH;
    int synNodeId;
    int ret;

    bbH = bbConstructFrom(0, (char *)octets, sizeof(octets));
    bbSetOctets(bbH, emanag->length / 8, 0, emanag->buffer);

    if (bbH == NULL)
        return RV_ERROR_UNKNOWN;

    memset((void *)&per, 0, sizeof(per));
    per.hSyn = emanag->hSyn;
    per.hVal = emanag->hVal;
    per.isTolerant = emanag->isTolerant;
    per.hBB = bbH;
    per.encodingDecodingErrorBitMask = 0x0;
    per.buf = perGetBuffer();
    if (per.buf == NULL)
    {
        RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncode: PER scratchpad cannot be obtained."));
        return RV_ERROR_OUTOFRESOURCES;
    }

    if (pvtGet(per.hVal, vNodeId, NULL, &synNodeId, NULL, NULL) <0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncode: value tree root id is illegal [%d].", vNodeId));
        return RV_ERROR_UNKNOWN;
    }

    ret = perEncNode((HPER)&per, synNodeId, vNodeId, -1, RV_FALSE);
    if (((bbStruct*)(per.hBB))->isOverflowOfBuffer == RV_TRUE)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "Encoding error: Overflow of buffer."));
        if (ret >= 0)
            ret = RV_ERROR_OUTOFRANGE;
    }

	/* Make sure result is byte-aligned */
	bbAlignTail(per.hBB);

    if (encoded)
        *encoded = bbBytesInUse(per.hBB);

    return ret;
}



/*
Desc: Decode a message from root.
*/
static int perDecode(
    IN  emanagStruct    *emanag,
    IN  RvPvtNodeId     vNodeId,
    IN  RvInt32         fieldId,
    OUT int             *decoded)
{
    pstChildExt info;
    perStruct per;
    RvUint8 octets[64];
    HBB bbH;
    pstChildExt child;
    int index;

    bbH = bbConstructFrom(10, (char *)octets, sizeof(octets));
    bbSetOctets(bbH, emanag->length / 8, emanag->length, emanag->buffer);

    if (bbH == NULL)
        return RV_ERROR_OUTOFRANGE;

    memset((void *)&per, 0, sizeof(per));
    per.hSyn = emanag->hSyn;
    per.hVal = emanag->hVal;
    per.isTolerant = emanag->isTolerant;
    per.hBB = bbH;
    per.encodingDecodingErrorBitMask = 0x0;
    per.buf = perGetBuffer();
    if (per.buf == NULL)
    {
        RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecode: PER scratchpad cannot be obtained."));
        return RV_ERROR_OUTOFRESOURCES;
    }

    pvtGet(per.hVal, vNodeId, NULL, &info.nodeId, NULL, NULL);
    index = pstGetFieldExt(per.hSyn, info.nodeId, fieldId, &child);
    if (index >= 0)
    {
#if (RV_ASN1_CODER_USE_H450 == RV_YES)
        info.speciality = child.speciality;
#endif
        info.nodeId = child.nodeId;
    }
#if (RV_ASN1_CODER_USE_H450 == RV_YES)
    else
        info.speciality = pstNotSpecial;
#endif

    {
        info.fieldId = fieldId;
        if (perDecNode((HPER)&per, &info, vNodeId) < 0  &&
            ((bbStruct*)(per.hBB))->isOverflowOfBuffer == RV_TRUE)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "Decoding error: Overflow of buffer."));
        }
    }

    if (decoded)
        *decoded = bbSetByte(per.decodingPosition);

    if (per.encodingDecodingErrorBitMask)
    {
        if (per.encodingDecodingErrorBitMask &  encDecErrorsResourcesProblem)
            return RV_ERROR_OUTOFRESOURCES;

        if (per.encodingDecodingErrorBitMask &  encDecErrorsMessageIsInvalid)
            return RV_ERROR_UNKNOWN;

        /* We might have an H.450 message with parts we can't understand. In such a case,
           we should still think of the message as decodable without any real errors
        if (per.encodingDecodingErrorBitMask &  encDecErrorsObjectWasNotFound)
            return RV_ERROR_BADPARAM;*/
    }

    return RV_TRUE;
}



#ifdef __cplusplus
}
#endif



