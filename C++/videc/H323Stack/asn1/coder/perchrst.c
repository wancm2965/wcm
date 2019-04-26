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

  clause '26':
  Encoding the restricted character string type

  ____________________________________________________________________________
  ___________________________CHARACTER STRING_________________________________
  ____________________________________________________________________________


  format:

  +----------+--------+---------+
  | ext. bit | length | string  |
  +----------+--------+---------+

  */


#include "rvstdio.h"
#include "perintr.h"
#include "psyntree.h"
#include "intutils.h"
#include "perchrst.h"

#ifdef __cplusplus
extern "C" {
#endif


/* returns 2 to the power of n. n must be positive */
#define ipow2(n) (1<<(n))

#define nprn(s) ((s)?(s):"(null)")


static int /* number of bytes for a known-multiplier character according to type. */
perCharSize(pstNodeType type)
  /* 26.5.3 */
{
    switch (type)
    {
        case pstUniversalString:    return 4;
        case pstBMPString:          return 2;
        case pstIA5String:          return 1;
        case pstGeneralString:      return 1;
        case pstVisibleString:      return 1;
        case pstNumericString:      return 1;
        case pstPrintableString:    return 1;
        default: break;
    }

    return -1;
}


/*
   Returns number of bits needed to encode alphabet.
   26.5.2
   */
static int
perAlphabetBits(int N) /* number of characters in the effective permitted alphabet */
{
    if (N<=0)
    {
        RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perAlphabetBits: Illegal parameter N=%d.", N));
        return 0;
    }

    if (N <= 2)             return 1;
    else if (N <= 4)        return 2;
    else if (N <= 16)       return 4;
    else if (N <= 256)      return 8;
    else if (N <= 0x10000)  return 16;
    return 32;
}

static RvUint32
perAlphabetMaxValue(char *alphabet,
            int alphabetSize,
            int charSize)
{
  RvUint32 v=0;
  int i;

  if (charSize >4) return 0;

  for (i=0; i<charSize; i++)
  {
      v <<= 8;
      v += alphabet[(alphabetSize-1)*charSize + i];
  }
   /*  ((RvUint8 *)&v)[4-charSize+i] = alphabet[(alphabetSize-1)*charSize + i]; */
  return v;
}


/* return position of letter in alphabet. */
static int
perAlphabetIndex(char *alphabet,
         int alphabetSize,
         int charSize,
         RvUint8 *letter)
{
  int i, j;

  for (i=0; i<alphabetSize; i+=charSize)
  {
    for (j=0; j<charSize; j++)
      if ((RvUint8)alphabet[i+j] == letter[j]) break;
    if (j<charSize) return i/charSize;
  }
  return -1;
}




/*
  Desc: Encodes an character string into TAIL of buffer.
  clause '26'
  */
int
perEncodeCharString(IN  HPER hPer,
                    IN  int synParent,
                    IN  int valParent, /* this is me */
                    IN  RvInt32 fieldId)
{
    perStruct *per = (perStruct *)hPer;
    int length=per->buf->bufferSize;
    RvInt32 index;
    RvUint8 *data=per->buf->buffer;
    int numOfChars, charSize, b, i;
    RvInt32 ub; /* largest value in permitted alphabet */
    char *fromString=NULL;
    int fromStringSize=0;
    pstNodeType type;
    int to,from;

    RV_UNUSED_ARG(fieldId);

    type=pstGetNodeType(per->hSyn, synParent);
    pstGetNodeRange(per->hSyn, synParent,&from,&to);

    if (type != pstNumericString)
    {
        /* Get the "FROM" string from the syntax */
        fromString = pstGetFROMStringPtr(per->hSyn, synParent,&fromStringSize);
    }
    else
    {
        /* For NumericString types we know the FROM already... */
        fromString = (char*)" 0123456789";
        fromStringSize = 11;
    }

    pvtGetString(per->hVal, valParent, length, (char*)data);
    /* We will now call pvtGet to get the string length: in case of a NULL
       string, pvtGetString will return an error instead of the length, and we
       would like to encode NULL strings correctly (H.450 uses them). */
    pvtGet(per->hVal, valParent, NULL, NULL, (RvInt32*)&length, NULL);
    if ((bbFreeBytes(per->hBB) < (RvInt32)length) || (length > (RvInt32)per->buf->bufferSize))
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeCharString: %s: buffer allocation error.",
            pstGetFieldNamePtr(per->hSyn, fieldId)));
        bbSetOverflow(per->hBB);
        return RV_ERROR_UNKNOWN;
    }

    /* -- not known-multiplier strings: 26.6 */
    charSize = perCharSize(type);
    if (charSize < 0)
    { /* not known-multiplier type */
        perEncodeLen(perLenTypeUNCONSTRAINED, (RvUint32)length, 0, 0, per->hBB); /* length: 26.6.3 */
        return bbAddTail(per->hBB, data, (RvUint32)(length*8), RV_TRUE); /* BER: 26.6.2 */
    }

    /* -- extension: 26.4 */
    if (pstGetIsExtended(per->hSyn, synParent) == RV_TRUE)
    {
        if (length > to || length < from)
        { /* not within extension root range */
            perEncodeBool(RV_TRUE, per->hBB);
            from = 0;
            to = 0 ; /* no effective size constraint */
            fromStringSize = 0; /* no effective PermittedAlphabet constraint */
        }
        else
            perEncodeBool(RV_FALSE, per->hBB);
    }

    /* -- length: 26.5.6 - 26.5.7 */
    numOfChars = (int)(length / charSize);
    if (to > 0)
    {  /* size constraint exist */
        if (numOfChars > to || numOfChars < from)
        { /* not within extension root range */
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perEncodeCharString:%s length not within extension root range: %d<=%d<=%d.",
                pstGetFieldNamePtr(per->hSyn, fieldId), from, numOfChars, to));
            return RV_ERROR_UNKNOWN;
        }
    }

    /* if (length == 0) return RV_TRUE;*/  /* 26.5 */

    if (to == 0) /* 26.5.7: unconstrained length. aub unset. */
        perEncodeLen(perLenTypeUNCONSTRAINED, (RvUint32)numOfChars, 0, 0, per->hBB);
    if (from < to)  /* 26.5.7: length determinant. aub set */
        perEncodeLen(perLenTypeCONSTRAINED, (RvUint32)numOfChars, (RvUint32)from, (RvUint32)to, per->hBB);

    /* -- string encoding */
    if (fromStringSize<1)
        b = 8;
    else
        b = perAlphabetBits(fromStringSize); /* 26.5.2 */

    if ((numOfChars != 0) && /* 10.9.3.3 note 2 - no alignment if length of string is 0 */
        ((to*b > 16) || /* 26.5.6 alignment rule and part of 26.5.7 alignment rule */
        ((to*b == 16) && (from < to)))) /* 26.5.7 alignment rule */
    {
        /* Alignment is needed for this string value */
        bbAlignTail(per->hBB); /* align */
    }

    if ((b == charSize*8) || (fromString == NULL))
    {
        /* no FROM restriction, or the restriction is in the size of the number of bits we need */
        return bbAddTail(per->hBB, data, (RvUint32)(numOfChars*charSize*8), RV_FALSE);
    }

    /* -- calc from */
    ub = perAlphabetMaxValue(fromString, fromStringSize, charSize);

    if (ub <= ipow2(b))  /* 26.5.4.a */
        return bbAddTail(per->hBB, data, (RvUint32)(length*8), RV_FALSE);

    /* -- 26.5.4.b */
    for (i=0; i<numOfChars; i++)
    {
        index = perAlphabetIndex(fromString, fromStringSize, charSize, data+i*charSize);
        if (index <0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perEncodeCharString: String character out of alphabet: '%.100s':%d [%s].",
                nprn(((char*)data)), i, nprn(fromString)));
            return RV_ERROR_UNKNOWN;
        }
        perEncodeNumber((RvUint32)index, (RvUint32)b, per->hBB);
    }

    return RV_TRUE;
}



/*
  Desc: Decodes an character string from buffer (at position).
  Note: clause '26'
*/
int
perDecodeCharString(IN  HPER hPer,
                    IN  int synParent, /* parent in syntax tree */
                    IN  int valParent, /* field parent in value tree */
                    IN  RvInt32 fieldId)   /* enum of current field */
{
    perStruct *per = (perStruct *)hPer;

    RvInt32 ub;
    int length, numOfChars=0;
    RvBool isExtended = RV_FALSE;
    RvUint32 dec = 0; /* decoded bits */
    int vtPath;
    RvUint8 *data=per->buf->buffer;
    int charSize, b, pos, j;
    int i;
    RvUint32 index;
    char *fromString=NULL;
    int fromStringSize=0;
    pstNodeType type;
    int to,from;
    int extension;

    RV_UNUSED_ARG(fieldId);

    type=pstGetNodeType(per->hSyn, synParent);
    pstGetNodeRange(per->hSyn, synParent,&from,&to);

    if (type != pstNumericString)
    {
        /* Get the "FROM" string from the syntax */
        fromString = pstGetFROMStringPtr(per->hSyn, synParent,&fromStringSize);
    }
    else
    {
        /* For NumericString types we know the FROM already... */
        fromString = (char*)" 0123456789";
        fromStringSize = 11;
    }

    charSize = perCharSize(type);

    extension=pstGetIsExtended(per->hSyn,synParent);

    /* -- not known-multiplier strings: 26.6 */
    if (charSize < 0)
    {   /* not known-multiplier type */
        /* -- length: 26.6.3 */
        if (perDecodeLen(perLenTypeUNCONSTRAINED, (RvUint32 *)&numOfChars, 0, 0, hPer, per->decodingPosition, &dec) < 0)
            return RV_ERROR_UNKNOWN;
        per->decodingPosition += dec;
        length = numOfChars;

        if (bbGet2Left(hPer, per->decodingPosition, (RvUint32)(length*8), data)<0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeCharString: bbGet2Left failed [%s]",
                pstGetFieldNamePtr(per->hSyn, fieldId)));
            return RV_ERROR_UNKNOWN;
        }
        /* -- data in BER: 26.6.2 */
        vtPath=valParent;
        if (fieldId!=RV_ERROR_UNKNOWN)
        {
            if ((vtPath=pvtAdd(per->hVal, valParent, fieldId, length, (char*)data, NULL)) <0)
            {
                RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                    "perDecodeCharString: cannot add string to value tree [%s:%d].",
                    pstGetFieldNamePtr(per->hSyn, fieldId), length));
                return RV_ERROR_UNKNOWN;
            }
        }
        per->decodingPosition += (length*8);
        return vtPath;
    }

    /* -- extension: 26.3 */
    if (extension == RV_TRUE)
    {
        perDecodeBool(&isExtended, hPer, per->decodingPosition, &dec);
    }
    per->decodingPosition += dec;

    if (isExtended == RV_TRUE)
    {   /* not within extension root range */
        from = 0;
        to = 0; /* no effective size constraint */
        fromStringSize = 0; /* no effective PermittedAlphabet constraint */
    }

    /* -- length */
    if ((from == to) && (from > 0)) /* no length determinant */
    {
        numOfChars = from;
    }
    else
    { /* there is length */
        if (to == 0) /* unconstrained */
        {
            if (perDecodeLen(perLenTypeUNCONSTRAINED, (RvUint32 *)&numOfChars, 0, 0, hPer, per->decodingPosition, &dec) < 0)
                return RV_ERROR_UNKNOWN;
        }
        else
        {
            if (perDecodeLen(perLenTypeCONSTRAINED, (RvUint32 *)&numOfChars,
                (RvUint32)from, (RvUint32)to, hPer, per->decodingPosition, &dec) < 0)
                return RV_ERROR_UNKNOWN;
        }

        per->decodingPosition+= dec;
    }

    if (to>0 && (numOfChars > to|| numOfChars < from))
    { /* not within extension root range */
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeCharString: %s length not within extension root range: %d<=%d<=%d.",
            pstGetFieldNamePtr(per->hSyn, fieldId), from, numOfChars, to));
        return RV_ERROR_UNKNOWN;
    }


    /* -- string */
    if (fromStringSize<1)   b = 8;
    else                    b = perAlphabetBits(fromStringSize); /* 26.5.2 */

    if ((numOfChars != 0) && /* 10.9.3.3 note 2 - no alignment if length of string is 0 */
        ((to*b > 16) || /* 26.5.6 alignment rule and part of 26.5.7 alignment rule */
        ((to*b == 16) && (from < to)))) /* 26.5.7 alignment rule */
        per->decodingPosition += bbAlignBits(per->hBB, (RvInt32)(per->decodingPosition)); /* alignment */

    /* -- add value node */
    length = numOfChars*charSize;
    if ((b == charSize*8) || (fromString == NULL))
    {
        /* no FROM restriction, or the restriction is in the size of the number of bits we need */
        int bitsToDecode = length * 8;
        if (bbGet2Left(hPer, per->decodingPosition, (RvUint32)bitsToDecode, data) < 0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeCharString2: bbGet2Left failed [%s]",
                pstGetFieldNamePtr(per->hSyn, fieldId)));
            return RV_ERROR_UNKNOWN;
        }
        per->decodingPosition += bitsToDecode;
    }
    else
    {
        /* -- calc. from */
        ub = perAlphabetMaxValue(fromString, fromStringSize, charSize);

        if (ub <= ipow2(b))
        { /* 26.5.4.a */
            int bitsToDecode = length * 8;
            if (bbGet2Left(hPer, per->decodingPosition, (RvUint32)bitsToDecode, data) < 0)
            {
                RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                    "perDecodeCharString3: bbGet2Left failed [%s]",
                    pstGetFieldNamePtr(per->hSyn, fieldId)));
                return RV_ERROR_UNKNOWN;
            }
            per->decodingPosition += bitsToDecode;
        }
        else
        {
            /* -- 26.5.4.b */
            pos=0;
            for (i=0; i<numOfChars; i++)
            {
                if (perDecodeNumber(&index, (RvUint32)b, hPer, per->decodingPosition, &dec) <0)
                    return RV_ERROR_UNKNOWN;
                per->decodingPosition+=dec;
                for (j=0; j<charSize; j++)   /* get real character */
                    data[pos++] = (RvUint8)fromString[index*charSize+j];
            }
        }
    }
    vtPath=valParent;
    if (fieldId!=RV_ERROR_UNKNOWN)
    {
        if ((vtPath= pvtAdd(per->hVal, valParent, fieldId, length, (char*)data, NULL)) <0)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeCharString:1: cannot add string to value tree [%s:%d].",
                pstGetFieldNamePtr(per->hSyn, fieldId), length));
            return RV_ERROR_UNKNOWN;
        }
    }
    return vtPath;
}

#ifdef __cplusplus
}
#endif



