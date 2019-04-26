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


#include "perintr.h"
#include "rasdecoder.h"

#ifdef __cplusplus
extern "C" {
#endif



/************************************************************************
 * rasDecoderInit
 * purpose: Initialize the decoder. Checks out the information inside
 *          the PST tree to allow faster runtime decoding of the first
 *          2 fields.
 * input  : ras             - RAS instance to use
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasDecoderInit(IN  rasModule*  ras)
{
    pstChild synMsg, synField;
    RvUint32 i;
    int rootId, j;

    ras->decoder.firstExtField = 26;

    /* Find out the number of messages */
    rootId = pstGetNodeIdByPath(ras->synMessage, NULL);
    ras->decoder.numOfMessages = (RvUint32)pstGetNumberOfChildren(ras->synMessage, rootId);

    /* Allocate a place for the number of OPTIONAL fields per message */
    RvMemoryAlloc(NULL, sizeof(RvUint32)*ras->decoder.numOfMessages, ras->logMgr,
        (void**)&ras->decoder.numOfOptFields);

    for (i = 0; i < ras->decoder.numOfMessages; i++)
    {
        ras->decoder.numOfOptFields[i] = 0;
        pstGetChild(ras->synMessage, rootId, (int)(i + 1), &synMsg);

        /* Find out the number of OPTINAL fields for this specific message */
        for (j = 0; j < pstGetNumberOfChildren(ras->synMessage, synMsg.nodeId); j++)
        {
            /* We count only fields that are not after ... that are OPTIONAL */
            pstGetChild(ras->synMessage, synMsg.nodeId, j + 1, &synField);
            if ((pstChildIsExtended(ras->synMessage, synMsg.nodeId, j + 1) == RV_FALSE) &&
                (synField.isOptional == RV_TRUE))
                ras->decoder.numOfOptFields[i]++;
        }
    }

    return 0;
}


/************************************************************************
 * rasDecoderEnd
 * purpose: Deinitialize the decoder.
 * input  : ras             - RAS instance to use
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasDecoderEnd(IN  rasModule*  ras)
{
    if (ras->decoder.numOfOptFields != NULL)
        RvMemoryFree(ras->decoder.numOfOptFields, ras->logMgr);
    return 0;
}



/************************************************************************
 * rasDecodePart
 * purpose: Decode the first 2 fields of an incoming message. This way
 *          We know the message's type and its sequence number.
 * input  : ras             - RAS instance to use
 *          messageBuffer   - The encoded message to check
 *          messageSize     - The size of the message in bytes
 * output : index           - Index of the RAS message
 *          requestSeqNum   - Request sequence number of the message
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasDecodePart(
    IN  rasModule*  ras,
    IN  RvUint8*    messageBuffer,
    IN  RvUint32    messageSize,
    OUT RvUint32*   index,
    OUT RvUint32*   requestSeqNum)
{
    RvUint8     octets[64]; /* Octets we need for the bit-block allocation (bbH) */
    HBB         bbH; /* Handle to the bit-block we use while decoding the message */
    RvUint32    from = 0, dec = 0; /* Positions inside PER buffer */
    perStruct   per; /* We need this struct to decode... */
    HPER        hPer = (HPER)&per;

    RvUint32    fieldLen; /* Length of an open type field - just because we must */
    RvBool      isExtended = RV_FALSE; /* Indication if the CHOICE value is after ... */


    /* Allocate a bit-block for the decoder for the first 2 fields.
     * We will create this decoder and then reset its buffer to point at the buffer we
     * have instead of the empty one we just created with 10 bytes
     */
    if (bbGetAllocationSize(10) > (int)sizeof(octets))
    {
        RvLogExcep(&ras->log,
            (&ras->log, "rasDecodePart: Allocation space for first 2 fields in RAS is not enough"));
        return RV_ERROR_UNKNOWN;
    }
    bbH = bbConstructFrom(10, (char *)octets, sizeof(octets));
    bbSetOctets(bbH, (int)messageSize, (RvInt32)messageSize * 8, messageBuffer);

    /* Set-up PER for decoding the first fields */
    memset((void *)&per, 0, sizeof(per));
    per.hBB = bbH;

    /* Decode first boolean field. This is the extension mark for the RAS Message CHOICE */
    if (perDecodeBool(&isExtended, hPer, from, &dec) < 0)
        return RV_ERROR_UNKNOWN;
    from += dec; /* Advance with the decoder's position */

    /* Find out the type of message */
    if (!isExtended)
    {
        /* Field before extension mark (...) - get the type of message */
        if (perDecodeInt(index, 0, ras->decoder.firstExtField - 2, RV_FALSE, RV_FALSE, RV_FALSE,
                         hPer, from, &dec, (char*)"choice") < 0)
            return RV_ERROR_UNKNOWN;
    }
    else
    {
        /* Field after extension mark (...) - get the type of message */
        if (perDecodeNormallySmallInt(index, hPer, from, &dec) < 0)
            return RV_ERROR_UNKNOWN;
        (*index) += ras->decoder.firstExtField - 1;
    }
    from += dec; /* Advance with the decoder's position */
    per.decodingPosition = from;

    if (isExtended)
    {
        /* We're after ... - this kind of field is an OPEN TYPE, os we should decode
           the length first */
        if (perDecodeLen(perLenTypeUNCONSTRAINED, &fieldLen, 0, 0, hPer, from, &dec) < 0)
            return RV_ERROR_UNKNOWN;
        from += dec;
    }

    /* Skip the extension indication field on the SEQUENCE */
    from++;

    /* Fix the positioning. We're now inside a SEQUENCE and we want to skip the bit-field
       that holds all the OPTINAL field indications */
    from += ras->decoder.numOfOptFields[*index];
    per.decodingPosition = from;

    /* Decode the requestSeqNum - it's just an INTEGER */
    if (perDecodeInt(requestSeqNum, 1, 65535, RV_FALSE, RV_FALSE, RV_FALSE, hPer,
                     from, &dec, (char*)"reqSeqNum") < 0)
        return RV_ERROR_UNKNOWN;

    RvLogDebug(&ras->log,
        (&ras->log, "rasDecodePart: index=%d, seqNum=%d", *index, *requestSeqNum));

    return RV_OK;
}





#ifdef __cplusplus
}
#endif


