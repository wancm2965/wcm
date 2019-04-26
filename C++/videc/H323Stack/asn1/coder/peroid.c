#ifdef __cplusplus
extern "C" {
#endif



/*
***********************************************************************************

NOTICE:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

***********************************************************************************
*/

/*
  perOID.c

  Ron S.
  16 May 1996


  ____________________________________________________________________________
  ___________________________OBJECT IDENTIFIER________________________________
  ____________________________________________________________________________


  format:

  +--------+-----------+
  | length | BER - OID |
  +--------+-----------+

  */


#include "rvstdio.h"
#include "perintr.h"
#include "peroid.h"


/*___________________________________________________________________________*/
/*________________________________Object_Identifier__________________________*/
/*___________________________________________________________________________*/
/*
  Desc: Encodes an object identifier into TAIL of buffer.
  clause '21'

  */
int
perEncodeOID(IN  HPER hPer,
         IN  int synParent,
         IN  int valParent, /* this is me */
         IN  RvInt32 fieldId)
{
    perStruct *per = (perStruct *)hPer;
    RvInt32 length=per->buf->bufferSize;
    char *data=(char*)per->buf->buffer;

    RV_UNUSED_ARG(synParent);
    RV_UNUSED_ARG(fieldId);

    pvtGetString(per->hVal, valParent, length, data);
    if (pvtGet(per->hVal, valParent, NULL, NULL, &length, NULL) < 0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeOID: %s: buffer allocation error.",
            pstGetFieldNamePtr(per->hSyn, fieldId)));
        return RV_ERROR_UNKNOWN;
    }

    if (bbFreeBytes(per->hBB) < length)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeOID: %s: buffer allocation error.",
            pstGetFieldNamePtr(per->hSyn, fieldId)));
        bbSetOverflow(per->hBB);
        return RV_ERROR_UNKNOWN;
    }

    /* with length determinant: 16.8 */
    perEncodeLen(perLenTypeUNCONSTRAINED, (RvUint32)length, 0, 0, per->hBB);
    return bbAddTail(per->hBB, (RvUint8 *)data, (RvUint32)(length*8), RV_TRUE);
}


/*
  Desc: Decodes an object identifier from buffer (at position).
  Returns: RV_ERROR_UNKNOWN in case of fault or positive number.
  Note: Currently an octet string.
  */
int
perDecodeOID(IN  HPER hPer,
         IN  int synParent, /* parent in syntax tree */
         IN  int valParent, /* field parent in value tree */
         IN  RvInt32 fieldId)   /* enum of current field */
{
	perStruct *per = (perStruct *)hPer;
	RvUint32 length = 0;
	RvUint32 dec = 0; /* decoded bits */

	char *data = (char*)per->buf->buffer;

	RV_UNUSED_ARG(synParent);

	/* -- get length: 16.8 */
	if (perDecodeLen(perLenTypeUNCONSTRAINED, &length, 0, 0, hPer, per->decodingPosition, &dec) < 0)
		return RV_ERROR_UNKNOWN;
	per->decodingPosition += dec;
	
	if (length>0x1fffffff || per->decodingPosition+length*8 > (RvUint32)bbBitsInUse(per->hBB))
	{
		RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
			"per:DecodeOID: Object Identifier too large to decode from buffer [len=%d]",
			(int)(length*8)));
		return RV_ERROR_UNKNOWN;
	}
	
	/* -- get the data from the asn buffer */
	per->decodingPosition += bbAlignBits(per->hBB, (RvInt32)(per->decodingPosition)); /* alignment */
	if (bbGet2Left(hPer, per->decodingPosition, length*8, (RvUint8 *)data)<0)
		return RV_ERROR_UNKNOWN;
	
	/* -- add value node */
	if (fieldId != RV_ERROR_UNKNOWN)
	{
		if (pvtAdd(per->hVal, valParent, fieldId, (RvInt32)length, data, NULL) < 0)
		{
			RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
				"perDecodeOID: cannot add string to value tree [%s:%d].",
				pstGetFieldNamePtr(per->hSyn, fieldId), length));
			return RV_ERROR_UNKNOWN;
		}
	}
	
	per->decodingPosition += (length * 8);
	return RV_TRUE;
}

#ifdef __cplusplus
}
#endif



