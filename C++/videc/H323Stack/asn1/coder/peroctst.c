/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
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
  ___________________________OCTET STRING_____________________________________
  ____________________________________________________________________________


  format:

  +----------+--------+---------+
  | ext. bit | length | string  |
  +----------+--------+---------+

  */


#include "rvstdio.h"
#include "perintr.h"
#include "peroctst.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
  Desc: Encodes an octet string into TAIL of buffer.
  clause '16'
  */
int
perEncodeOctetString(IN  HPER hPer,
             IN  int synParent,
             IN  int valParent, /* this is me */
             IN  RvInt32 fieldId)
{
    perStruct *per = (perStruct *)hPer;
    int length=per->buf->bufferSize;
    RvUint8 *data=per->buf->buffer;
    perLenType lenType;
    int to,from;

    RV_UNUSED_ARG(fieldId);

    pstGetNodeRange(per->hSyn, synParent,&from,&to);

    pvtGetString(per->hVal, valParent, (RvInt32)length, (char*)data);
    pvtGet(per->hVal, valParent, NULL, NULL, (RvInt32*)&length, NULL);
    if (bbFreeBytes(per->hBB) < (RvInt32)length)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeOctetString: %s: buffer allocation error.",
            pstGetFieldNamePtr(per->hSyn, fieldId)));
        bbSetOverflow(per->hBB);
        return RV_ERROR_UNKNOWN;
    }

    /* -- extension: 16.3 */
    if (pstGetIsExtended(per->hSyn, synParent) == RV_TRUE)
    {
        if (length > to || length < from)
        { /* not within range */
            perEncodeBool(RV_TRUE, per->hBB);
            perEncodeLen(perLenTypeUNCONSTRAINED, (RvUint32)length, 0, 0, per->hBB);
            return bbAddTail(per->hBB, data, (RvUint32)(length*8), RV_TRUE);
        }
        else
          perEncodeBool(RV_FALSE, per->hBB);
    }

    /* -- length: 16.5 - 16.8*/
    if (from == 0 && to == 0) /* no constraint */
        lenType = perLenTypeUNCONSTRAINED;
    else
    {
        lenType = perLenTypeCONSTRAINED;
        if (length > to || length < from)
        { /* not within range */
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perEncodeOctetString: %s length not within range: %d<=%d<=%d.",
                pstGetFieldNamePtr(per->hSyn, fieldId), from, length, to));
            return RV_ERROR_UNKNOWN;
        }

        if (to == 0) return RV_TRUE;  /* 16.5 */

        if (from == to)
        {  /* no length determinant */
            /* 16.6 - no alignment, 16.7 - with alignment */
            return bbAddTail(per->hBB, data, (RvUint32)(length*8), (length > 2));
        }
    }

    /* with length determinant: 16.8 */
    perEncodeLen(lenType, (RvUint32)length, (RvUint32)from, (RvUint32)to, per->hBB);

    return bbAddTail(per->hBB, data, (RvUint32)(length*8), length > 0); /* Align only if length > 0: 10.9.3.3 note 2 */
}



/*
  Desc: Decodes an octet string from buffer (at position).
  Note: clause '16'
*/
int
perDecodeOctetString(IN  HPER hPer,
             IN  int synParent, /* parent in syntax tree */
             IN  int valParent, /* field parent in value tree */
             IN  RvInt32 fieldId)   /* enum of current field */
{
    perStruct *per =(perStruct *)hPer;

    int length = 0;
    RvBool isExtended = RV_FALSE;
    RvUint32 dec = 0; /* decoded bits */

    int vtPath;
    RvUint8 *data = per->buf->buffer;
    perLenType lenType;
    int to,from;

    pstGetNodeRange(per->hSyn, synParent,&from,&to);


    /* -- extension: 16.3 */
    if (pstGetIsExtended(per->hSyn, synParent) == RV_TRUE)
    {
        perDecodeBool(&isExtended, hPer, per->decodingPosition, &dec);
        per->decodingPosition += dec;
    }

    if (isExtended == RV_TRUE)
    {
        /* not within extension root range */
        if (perDecodeLen(perLenTypeUNCONSTRAINED, (RvUint32 *)&length, 0, 0, hPer, per->decodingPosition, &dec) < 0)
            return RV_ERROR_UNKNOWN;
        per->decodingPosition += dec;

        if ((RvUint32)per->decodingPosition + length*8 > bbBitsInUse(per->hBB))
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeOctetString1: Not enough space in buffer to decode string [%s:%d].",
                pstGetFieldNamePtr(per->hSyn, fieldId), length));
            length = 0;
            data[0] = '\0';
        }
        else
        {
            if (bbGet2Left(hPer, per->decodingPosition, (RvUint32)(length*8), data) < 0)
            {
                RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                    "perDecodeOctetString1: bbGet2Left failed [%s]",
                    pstGetFieldNamePtr(per->hSyn, fieldId)));
                length = 0;
                data[0] = '\0';
            }
        }
        vtPath=valParent;
        if (fieldId!=RV_ERROR_UNKNOWN)
        if ((vtPath=pvtAdd(per->hVal, valParent, fieldId, length, (char*)data, NULL)) < 0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeOctetString: cannot add string to value tree [%s:%d].",
                pstGetFieldNamePtr(per->hSyn, fieldId), length));
            return RV_ERROR_UNKNOWN;
        }

        per->decodingPosition +=(length*8);
        return length? vtPath : RV_ERROR_UNKNOWN;
    }

    /* -- not extended */

    /* -- length */
    if (from == to && from == 0) /* no constraint */
        lenType = perLenTypeUNCONSTRAINED;
    else
    {
        lenType = perLenTypeCONSTRAINED;

        if (from == to)
        {
            /* no length determinant */
            length = from;
/*            vtGetString(per->hVal, vtPath, length, (char*)data); */
            if (from > 2)
            {
                /* 16.7 */
                per->decodingPosition += bbAlignBits(per->hBB, (int)(per->decodingPosition)); /* alignment */

                if ((RvUint32)per->decodingPosition + length*8 > bbBitsInUse(per->hBB))
                {
                    RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                        "perDecodeOctetString2: Not enough space in buffer to decode string [%s:%d].",
                        pstGetFieldNamePtr(per->hSyn, fieldId), length));
                    data[0] = '\0';
                    length = 0;
                }
            }
            if (length)
            {
                if (bbGet2Left(hPer, per->decodingPosition, (RvUint32)(length*8), data) < 0) /* no alignment */
                {
                    RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                        "perDecodeOctetString2: bbGet2Left failed [%s]",
                        pstGetFieldNamePtr(per->hSyn, fieldId)));
                    data[0] = '\0';
                    length = 0;
                }
            }
            per->decodingPosition +=(length*8);
            vtPath=valParent;
            if (fieldId!=RV_ERROR_UNKNOWN)
            if ((vtPath = pvtAdd(per->hVal, valParent, fieldId, length, (char*)data, NULL)) <0)
            {
                RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                    "perDecodeOctetString2: cannot add string to value tree [%s:%d].",
                    pstGetFieldNamePtr(per->hSyn, fieldId), length));
                return RV_ERROR_UNKNOWN;
            }

            return length? vtPath : RV_ERROR_UNKNOWN;
        }
    }

    /* -- get length: 16.8 */
    if (perDecodeLen(lenType, (RvUint32 *)&length, (RvUint32)from, (RvUint32)to, hPer, per->decodingPosition, &dec) <0)
        return RV_ERROR_UNKNOWN;
    per->decodingPosition += dec;

    if (lenType == perLenTypeCONSTRAINED && (length > to || length < from))
    {
        /* not within extension root range */
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeOctetString: %s length not within extension root range: %d<=%d<=%d.",
            pstGetFieldNamePtr(per->hSyn, fieldId), from, length, to));
        return RV_ERROR_UNKNOWN;
    }


    /* -- add value node */
    if (length > 0)
    {
        /* Align only if the length > 0: 10.9.3.3 note 2 */
        per->decodingPosition += bbAlignBits(per->hBB, (RvInt32)(per->decodingPosition));
    }

    if ((RvUint32)per->decodingPosition + length*8 > bbBitsInUse(per->hBB))
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeOctetString3: Not enough space in buffer to decode string [%s:%d].",
            pstGetFieldNamePtr(per->hSyn, fieldId), length));
        length = 0;
        data[0] = '\0';
    }
    else
    {
        if (bbGet2Left(hPer, per->decodingPosition, (RvUint32)(length*8), data) < 0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeOctetString3: bbGet2Left failed [%s]",
                pstGetFieldNamePtr(per->hSyn, fieldId)));
            length = 0;
            data[0] = '\0';
        }
    }
    vtPath=valParent;
    if (fieldId!=RV_ERROR_UNKNOWN)
    if ((vtPath = pvtAdd(per->hVal, valParent, fieldId, length, (char*)data, NULL)) <0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeOctetString:1: cannot add string to value tree [%s:%d].",
            pstGetFieldNamePtr(per->hSyn, fieldId), length));
        return RV_ERROR_UNKNOWN;
    }

    per->decodingPosition +=(length*8);
    return RV_TRUE;
}

#ifdef __cplusplus
}
#endif



