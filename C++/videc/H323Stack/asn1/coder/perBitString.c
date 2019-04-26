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
  ____________________________________________________________________________
  _____________________________BIT STRING_____________________________________
  ____________________________________________________________________________


  format:

  +----------+--------+---------+
  | ext. bit | length | string  |
  +----------+--------+---------+

  */

#include "rvstdio.h"
#include "perintr.h"
#include "perBitString.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
  Desc: Encodes a bit string into TAIL of buffer.
  clause '15'
  */
int perEncodeBitString(
             IN  HPER hPer,
             IN  int synParent,
             IN  int valParent, /* this is me */
             IN  RvInt32 fieldId)
{
    perStruct *per = (perStruct *)hPer;
    int length = per->buf->bufferSize;
    RvUint8 *data=per->buf->buffer;
    perLenType lenType;
    int from,to;
    
    RV_UNUSED_ARG(fieldId); /* Needed when compiling without logs */

    pstGetNodeRange(per->hSyn,synParent,&from,&to);

    pvtGetString(per->hVal, valParent, length, (char*)data);
    length = (RvUint32)pvtGetBitString(per->hVal, valParent, 0, NULL);

    if (bbFreeBytes(per->hBB) < (RvInt32)bbSetByte((RvUint32)length))
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeBitString: %s: buffer allocation error.",
            pstGetFieldNamePtr(per->hSyn, fieldId)));
        bbSetOverflow(per->hBB);
        return RV_ERROR_UNKNOWN;
    }

    /* -- extension: 15.5 */
    if (pstGetIsExtended(per->hSyn,synParent) == RV_TRUE)
    {
        if (length > to || length < from)
        { /* not within range */
            perEncodeBool(RV_TRUE, per->hBB);
            perEncodeLen(perLenTypeUNCONSTRAINED, (RvUint32)length, 0, 0, per->hBB);
            return bbAddTail(per->hBB, data, (RvUint32)length /* *8 */ , RV_TRUE);
        }
        else
            perEncodeBool(RV_FALSE, per->hBB);
    }


    /* -- length: 15.6 - 15.10 */
    if (from == to && from == 0)
    { /* no constraint */
        lenType = perLenTypeUNCONSTRAINED;
    }
    else
    {
        lenType = perLenTypeCONSTRAINED;

        /* not within range */
        if (length > to || length < from)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perEncodeBitString: %s length not within range: %d<=%d<=%d.",
                pstGetFieldNamePtr(per->hSyn, fieldId), from, length, to));

            return RV_ERROR_UNKNOWN;
        }

        if (to == 0) return RV_TRUE;  /* 15.7 */

        if (from == to)
        {  /* no length determinant */
            if (length <= 16)  /* 15.8 */
                return bbAddTail(per->hBB, data, (RvUint32)length/* *8 */, RV_FALSE); /* no alignment */
            else /* 15.9 */
                return bbAddTail(per->hBB, data, (RvUint32)length/* *8 */, RV_TRUE); /* with alignment */
        }
    }

    /* with length determinant: 15.10 */
    perEncodeLen(lenType, (RvUint32)length, (RvUint32)from, (RvUint32)to, per->hBB);

    return bbAddTail(per->hBB, data, (RvUint32)length /* *8 */, length > 0); /* Align only of length > 0: 10.9.3.3 note 2 */
}


/*
Desc: Decodes a bit string from buffer (at position).
Note: clause '15'
*/
int perDecodeBitString(
                   IN  HPER hPer,
                   IN  int synParent, /* parent in syntax tree */
                   IN  int valParent, /* field parent in value tree */
                   IN  RvInt32 fieldId)   /* enum of current field */
{
    perStruct *per = (perStruct *)hPer;

    int length=0,ret=RV_ERROR_UNKNOWN;
    RvBool isExtended = RV_FALSE;
    RvUint32 dec = 0; /* decoded bits */
    int vtPath;
    RvUint8 *data=per->buf->buffer;
    perLenType lenType;
    int from,to;


    pstGetNodeRange(per->hSyn,synParent,&from,&to);


    /* -- extension: 16.3 */
    if (pstGetIsExtended(per->hSyn,synParent) == RV_TRUE)
    {
        perDecodeBool(&isExtended, hPer, per->decodingPosition, &dec);
    }
    per->decodingPosition += dec;

    if (isExtended == RV_TRUE)
    {   /* not within extension root range */
        if (perDecodeLen(perLenTypeUNCONSTRAINED, (RvUint32 *)&length, 0, 0, hPer, per->decodingPosition, &dec) < 0)
            return RV_ERROR_UNKNOWN;
        per->decodingPosition += dec;
        vtPath=valParent;
        if (fieldId != RV_ERROR_UNKNOWN)
        {
            if ((ret=vtPath=pvtAdd(per->hVal, valParent, fieldId, length, (char*)"", NULL)) <0)
            {
                RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                    "perDecodeBitString1: Cannot add string to value tree [%s:%d].",
                    pstGetFieldNamePtr(per->hSyn, fieldId), length));
                return RV_ERROR_UNKNOWN;
            }
        }

        if ((RvUint32)per->decodingPosition + length > bbBitsInUse(per->hBB))
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeBitString1: Not enough space in buffer to decode string [%s:%d].",
                pstGetFieldNamePtr(per->hSyn, fieldId), length));
                return RV_ERROR_UNKNOWN;
        }
        bbGet2Left(hPer, per->decodingPosition, (RvUint32)length/* *8 */, data);

        if (pvtSet(per->hVal, vtPath, -1, length, (char*)data) <0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeBitString2: Cannot add string to value tree [%s:%d].",
                pstGetFieldNamePtr(per->hSyn, fieldId), length));
                return RV_ERROR_UNKNOWN;
        }


        per->decodingPosition += (length/* *8 */);
        return ret;
    }

    /* -- not extended */

    /* -- length */
    if (from == to && from == 0) /* no constraint */
        lenType = perLenTypeUNCONSTRAINED;
    else
    {
        lenType = perLenTypeCONSTRAINED;

        if (from == to)
        {  /* no length determinant */
            length = from;
            vtPath = valParent;
            if (fieldId != RV_ERROR_UNKNOWN)
            {
                if ((ret=vtPath=pvtAdd(per->hVal, valParent, fieldId, length, (char*)"", NULL)) <0)
                {
                    RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                        "perDecodeBitString3: cannot add string to value tree [%s:%d].",
                        pstGetFieldNamePtr(per->hSyn, fieldId), length));
                    return RV_ERROR_UNKNOWN;
                }
            }

            if (from > 16)  /* 15.9 */
            {
                per->decodingPosition += bbAlignBits(per->hBB, (RvInt32)(per->decodingPosition)); /* alignment */

                if ((RvUint32)per->decodingPosition + length > bbBitsInUse(per->hBB))
                {
                    RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                        "perDecodeBitString2: Not enough space in buffer to decode string [%s:%d].",
                        pstGetFieldNamePtr(per->hSyn, fieldId), length));
                    return RV_ERROR_UNKNOWN;
                }
            }

            /* 15.8 *//* 15.9 */
            bbGet2Left(hPer, per->decodingPosition, (RvUint32)length/* *8 */, data); /* no alignment */
            if (pvtSet(per->hVal, vtPath, -1, length, (char*)data) <0)
            {
                RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                    "perDecodeBitString4: Cannot add string to value tree [%s:%d].",
                    pstGetFieldNamePtr(per->hSyn, fieldId), length));
                return RV_ERROR_UNKNOWN;
            }
            per->decodingPosition += (length/* *8 */);
            return ret;
        }
    }

    /* -- get length: 15.10 */
    if (perDecodeLen(lenType, (RvUint32 *)&length, (RvUint32)from, (RvUint32)to,
        hPer, per->decodingPosition, &dec) <0)
        return RV_ERROR_UNKNOWN;
    per->decodingPosition+=dec;

    if (lenType == perLenTypeCONSTRAINED &&
        (length > to || length < from))
    { /* not within extension root range */
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeBitString: %s length not within extension root range: %d<=%d<=%d.",
            pstGetFieldNamePtr(per->hSyn, fieldId), from, length, to));
        return RV_ERROR_UNKNOWN;
    }


    /* -- add value node */
    vtPath=valParent;
    if (fieldId != RV_ERROR_UNKNOWN)
    {
        if ((ret=vtPath=pvtAdd(per->hVal, valParent, fieldId, length, (char*)"", NULL)) <0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeBitString5: Cannot add string to value tree [%s:%d].",
                pstGetFieldNamePtr(per->hSyn, fieldId), length));
            return RV_ERROR_UNKNOWN;
        }
    }

    if (length > 0)
    {
        /* Align only if length > 0: 10.9.3.3 note 2 */
        per->decodingPosition += bbAlignBits(per->hBB, (RvInt32)(per->decodingPosition));
    }

    if ((RvUint32)per->decodingPosition + length > bbBitsInUse(per->hBB))
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeBitString3: Not enough space in buffer to decode string [%s:%d].",
            pstGetFieldNamePtr(per->hSyn, fieldId), length));
        return RV_ERROR_UNKNOWN;
    }

    bbGet2Left(hPer, per->decodingPosition, (RvUint32)length/* *8 */, data);
    if (pvtSet(per->hVal, vtPath, -1, length, (char*)data) <0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeBitString6: Cannot add string to value tree [%s:%d].",
            pstGetFieldNamePtr(per->hSyn, fieldId), length));
        return RV_ERROR_UNKNOWN;
    }
    per->decodingPosition += (length/* *8 */);
    return RV_TRUE;
}


#ifdef __cplusplus
}
#endif



