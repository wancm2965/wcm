#ifdef __cplusplus
extern "C"
{
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
    perSimple.c

    Encoding Rules are ASN.1 PER BASIC - ALIGNED.
    Encoding/Decoding of ASN simple types.
    Integer, length, null, boolean, octet string.

    Note: Open type fields are encoded into a separate bit buffer and then added
    to the complete encoding as an octet string with unconstrained length.

    Note: Bit order should be: Most Significant Bit(MSB) first.
    Byte order ...

    Encoding: Add to tail of buffer.
    Decoding: Decode from position in buffer. Returns the number of bits being decoded.
    Does not change the buffer.
    Uses user allocated memory for integers and strings(sra).


    Parameters:
    n: number.
    lb: lower bound. -1 if no constraint.
    ub: upper bound. -1 if no constraint.
    from: position in bit buffer from which to decode.
    decoded: number of bits successfully decoded from buffer.
    bbH: bit buffer handler.

*/

#include "rvstdio.h"
#include "perintr.h"
#include "persimpl.h"



#define K64    RvUint32Const(65536)      /* 64K */
#define BIT24  RvUint32Const(16777216)   /* 2^24 */
#define BIT32  RvUint32Const(4294967295) /* 2^32 - 1 */




static int
    perEncodeSemiConstInt(IN  RvUint32 n, /* the number */
          IN  RvInt32 lb,
          OUT HBB bbH);


static int
    perDecodeSemiConstInt(IN  RvUint32 *n,
          IN  RvInt32 lb,
          IN  RvInt32 length, /* in octets */
          IN  HPER hPer,
          IN  RvUint32 from,
          OUT RvUint32 *dec);


/* Description: Returns the exact number of bits for a constrained
   integer with defined range.
   Input: range- ub - lb + 1
   Returns: The number of bits for the integer. or -1 if range too
   large to conclude the size;
 */

static RvUint8
    perIntBits(RvUint32 range)
{
    static const RvUint8 lut[]=
    {
        0,
            0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
            5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
    };

    if (range <= 256)
        return lut[range];

    if (range <= K64)
        return 2*8;
    if (range <= BIT24)
        return 3*8;
    return 4*8;
}



/*___________________________________________________________________________*/
/*________________________Constraigned_whole_Integer_________________________*/
/*___________________________________________________________________________*/
/*
Desc: Encodes an integer and puts it in TAIL of bit buffer.
Returns: Failure if cannot encode n.
*/
int
    perEncodeInt(IN  RvUint32 n,
    IN  RvUint32 lb,
    IN  RvUint32 ub,
    IN  RvBool isFromAbsent,
    IN  RvBool isToAbsent,
    IN  RvBool isExtension,
    OUT HBB bbH) /* Should be initialized with sufficient size.
    It has to have at least 4 bytes size. */
{
    RvBool isExtended = RV_FALSE;
    RvUint32 num = n - lb;
    RvUint32 tmpNum;
    RvUint32 range = ub - lb + 1;
    unsigned char ch; /* for case 10.5.7.1 */
    unsigned char numOct[MAX_INT_SIZE]; /*num converted to array in BIG ENDIAN*/
    int bits;

    if (lb == 0 && ub == BIT32)
        range = (RvUint32)BIT32;

    if (!bbH || bbFreeBytes(bbH) < MAX_INT_SIZE)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "per:EncodeInt: Buffer allocation error"));
        bbSetOverflow(bbH);
        return RV_ERROR_UNKNOWN; /* Buffer allocation error */
    }

    if (isExtension)
    {
        if (((!isFromAbsent) && n<lb) ||((!isToAbsent) && ub < n))
        {
            isExtended = RV_TRUE;
            perEncodeBool(RV_TRUE, bbH); /* adding extension bit. Sergey M. */
        }
        else
            perEncodeBool(RV_FALSE, bbH);
    }



    /* Unconstrained or Semiconstrained INTEGER. Sergey M. */
    if (isToAbsent != RV_FALSE || isExtended)
    {
        if (n == 0)
            bits = 1;
        else
            bits = perIntBits(n + 1);
        if (isFromAbsent != RV_FALSE && (bits==8 || (bits==16 && (n&0x8000)) || (bits==24 && (n&0x800000))))
            bits++;
        ch = (unsigned char)bbSetByte((RvUint32)bits);

        perEncodeLen(perLenTypeUNCONSTRAINED, (RvUint32)ch, 0, 0, bbH);

        if (isFromAbsent != RV_FALSE || isExtended) /* Unconstrained INTEGER. Sergey M. */
        {
            /* Convert the number to 4 bytes in BIG ENDIAN order */
            tmpNum = n; /* 4 bytes integer */
            numOct[0] =(unsigned char)(tmpNum >> 24); /* n / 2^24 */
            tmpNum -= numOct[0] << 24;
            numOct[1] =(unsigned char)(tmpNum >> 16); /* n / 2^16 */
            tmpNum -= numOct[1] << 16;
            numOct[2] =(unsigned char)(tmpNum >> 8); /* n / 2^8 */
            tmpNum -= numOct[2] << 8;
            numOct[3] =(unsigned char)tmpNum;
            return bbAddTail(bbH, (RvUint8 *)(numOct + (MAX_INT_SIZE - ch)), ch*8, RV_TRUE);
        }
        else
        {
            if (n < lb)
            {
                RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                    "per:EncodeInt: Integer out of range [%d <= %d ", lb, n));
                return RV_ERROR_UNKNOWN; /* Range Error */
            }
            return perEncodeSemiConstInt(n, (RvInt32)lb, bbH);
        }
    } /* End of unconstrained or semiconstrained INTEGER. Sergey M. */

    if (ub == lb)
        return RV_TRUE; /* no encoding needed */



    /* Irina * changes for negative numbers */
    if (/*lb > ub || n < lb ||  n > ub */ n-lb > ub-lb)
    {
        /* Range Error */
        if ((int)ub>(int)lb)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "per:EncodeInt: Integer out of range [%d <= %d <= %d]", lb, n, ub));
        }
        else
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "per:EncodeInt: Integer out of range [%u <= %u <= %u]", lb, n, ub));
        }
        return RV_ERROR_UNKNOWN;
    }


    bits = perIntBits(range);

    /* range <= 255: 10.5.7.1 */
    if (range <= 255)
    {
        ch =(unsigned char)num;
        ch =(unsigned char)(ch <<(8 - bits));
        return bbAddTail(bbH, (RvUint8 *)&ch, (RvUint32)bits, RV_FALSE);
    }

    /* range = 256: 10.5.7.2 */
    if (range == 256)
    {
        ch =(unsigned char)num;
        return bbAddTail(bbH, (RvUint8 *)&ch, 8, RV_TRUE);
    }

    /* Convert the number to 4 bytes in BIG ENDIAN order */
    tmpNum = num; /* 4 bytes integer */
    numOct[0] =(unsigned char)(tmpNum >> 24); /* n / 2^24 */
    tmpNum -= numOct[0] << 24;
    numOct[1] =(unsigned char)(tmpNum >> 16); /* n / 2^16 */
    tmpNum -= numOct[1] << 16;
    numOct[2] =(unsigned char)(tmpNum >> 8); /* n / 2^8 */
    tmpNum -= numOct[2] << 8;
    numOct[3] =(unsigned char)tmpNum;

    /* range <=K64: 10.5.7.3; 2 octets aligned */
    if (range <= K64)
    {
        /* 256 < num <= 65536 */
        if (bits/8 < 0 || bits/8 > MAX_INT_SIZE)
            return RV_ERROR_UNKNOWN;
        return bbAddTail(bbH, (RvUint8 *)(numOct + (MAX_INT_SIZE - bits/8)), (RvUint32)bits, RV_TRUE);
    }

    /* range > K64: 10.5.7.4: indefinite length case */
    /* These cases require additional length field */
    /* The length shall be encoded according to clause 10.9.3.6 and be treated
    as an octet - aligned - bit - field. The length is <= 127.  */

    bits = perIntBits(num+1);
    ch = (unsigned char)bbSetByte((RvUint32)bits);
    if (ch == 0)
        ch = 1; /* at least 1 octet */

    /* length: 12.2.6a */
    perEncodeLen(perLenTypeCONSTRAINED, (RvUint32)ch, 1, bbSetByte(perIntBits(ub)), bbH);
    return bbAddTail(bbH, (RvUint8 *)(numOct + (MAX_INT_SIZE - ch)), ch*8, RV_TRUE); /* number */
}



/* RV_TRUE or negative value on failure */
RVINTAPI int RVCALLCONV
perDecodeInt(
             /* Decodes an integer from a bit buffer. */
             OUT RvUint32 *n, /* decoded number */
             IN  RvUint32 lb,
             IN  RvUint32 ub,
             IN  RvBool isFromAbsent,
             IN  RvBool isToAbsent,
             IN  RvBool isExtension,
             IN  HPER hPer,
             IN  RvUint32 from, /* position in buffer */
             OUT RvUint32 *decoded,
             IN  const char* desc /* short description(optional) */
             )
{
    RvBool isExtended = RV_FALSE;
    perStruct *per =(perStruct *)hPer;

    RvUint32 num;
    RvUint32 extract, bits, bytes;
    RvUint32 range = ub - lb + 1;
    RvUint8 numOct[MAX_INT_SIZE]; /*num converted to array in BIG ENDIAN*/
    RvUint32 myFrom = from;

    RV_UNUSED_ARG(desc);

    memset((char *)numOct, 0, MAX_INT_SIZE);

    if (lb == 0 && ub == BIT32)
        range = (RvUint32)BIT32;

    if (!n || !decoded)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeInt:%s: parameters (n,del) not allocated.", desc));
        return RV_ERROR_UNKNOWN;
    }
    *n = 0;
    *decoded = 0;

    if (!(per->hBB))
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeInt:%s: Buffer allocation error.", desc));
        return RV_ERROR_UNKNOWN; /* Buffer allocation error */
    }

    if (isExtension)
    {
        perDecodeBool(&isExtended, hPer, from, decoded); /* decoding extension bit. Sergey M. */
        myFrom += *decoded;
    }

    /* Unconstrained or Semi-constrained INTEGER. Sergey M. */
    if (isToAbsent !=  RV_FALSE || isExtended)
    {
        if (perDecodeLen(perLenTypeUNCONSTRAINED, (RvUint32 *)&bytes, 0, 0, hPer, myFrom, &extract) < 0)
            return RV_ERROR_UNKNOWN;
        bits = bytes*8; /* set # bits to extract */
        *decoded += extract;
        myFrom += extract;
        *decoded += bbAlignBits(per->hBB, (RvInt32)myFrom);
        myFrom += bbAlignBits(per->hBB, (RvInt32)myFrom); /* move to alignment */
        if (myFrom + bits > bbBitsInUse(per->hBB))
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeInt:%s: Integer too large to decode from buffer [len=%d]",
                desc, (int)bits));
            return RV_ERROR_UNKNOWN;
        }

        if (isFromAbsent !=  RV_FALSE || isExtended) /* Unconstrained INTEGER. Sergey M. */
        {
            if ((RvInt32)(extract = (RvUint32)bbGet2Right(hPer, myFrom, bits, (RvUint8 *)(numOct + (MAX_INT_SIZE - bytes)))) <0)
            {
                RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                    "perDecodeInt:%s: Extracting failed [%d]", desc, extract));
                return RV_ERROR_UNKNOWN;
            }

            num = numOct[3] +((RvUint32)numOct[2] << 8) +((RvUint32)numOct[1] << 16) +((RvUint32)numOct[0] << 24);
            /*num += lb; */

            *decoded += extract;

            *n = num;

            RvLogDebug(&rvPerLogSource, (&rvPerLogSource,
                " >> [%u..%u] (%u:%u) => %u \t \t %s.",
                lb, ub, from, *decoded, *n, desc));
            return RV_TRUE;
        }
        else /* Semi-constrained INTEGER. Sergey M. */
        {
            perDecodeSemiConstInt(n, (RvInt32)lb, (RvInt32)bytes, hPer, myFrom, &extract);
            *decoded += extract;
            RvLogDebug(&rvPerLogSource, (&rvPerLogSource,
                " >> [%u....] (%u:%u) => %u \t \t %s.",
                lb, from, *decoded, *n, desc));
            return RV_TRUE;
        }
    } /* end of Unconstrainted or Semi-constrained INTEGER. Sergey M. */


    if (ub == lb) /* no encoding needed */
    {
        *n = lb;
        return RV_TRUE;
    }


    bits = perIntBits(range);
    bytes = bbSetByte(bits);
    if (bytes > MAX_INT_SIZE)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeInt:%s: Number of bits in integer error [%u]", desc, bits));
        return RV_ERROR_UNKNOWN;
    }

    if (bits > 16)
    {
        /* Extract length */
        /* 12.2.6a */
        if (perDecodeLen(perLenTypeCONSTRAINED, (RvUint32 *)&bytes, 1, bbSetByte(perIntBits(ub)), hPer, myFrom, &extract) < 0)
            return RV_ERROR_UNKNOWN;
        if (bytes > MAX_INT_SIZE)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perDecodeInt:%s: Integer value too large to decode [%u].", desc, bytes));
            bbSetOverflow(per->hBB);
            return RV_ERROR_UNKNOWN;
        }
        bits = bytes*8; /* set # bits to extract */
        *decoded += extract;
        myFrom += extract;
    }

    if (range >= 256)
    {
		int alignment = bbAlignBits(per->hBB, (RvInt32)myFrom);
        *decoded += alignment;
        myFrom += alignment; /* move to alignment */
    }

    if (myFrom + bits > bbBitsInUse(per->hBB))
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeInt:%s: Integer too large to decode from buffer [len=%u]",
            desc, bits));
        bbSetOverflow(per->hBB);
        return RV_ERROR_UNKNOWN;
    }

    if ((RvInt32)(extract = (RvUint32)bbGet2Right(hPer, myFrom, bits, (RvUint8 *)(numOct + (MAX_INT_SIZE - bytes)))) <0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeInt:%s: Extracting failed [%u]", desc, extract));
        return RV_ERROR_UNKNOWN;
    }

    num = numOct[3] +((RvUint32)numOct[2] << 8) +((RvUint32)numOct[1] << 16) +((RvUint32)numOct[0] << 24);
    num += lb;

    if (num - lb > ub - lb)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeInt:%s: Decoded number out of range [%u]", desc, num));
        return RV_ERROR_UNKNOWN;
    }

    *decoded += extract;

    *n = num;

    RvLogDebug(&rvPerLogSource, (&rvPerLogSource,
        " >> [%u..%u] (%u:%u) => %u \t \t %s.",
        lb, ub, from, *decoded, *n, desc));
    return RV_TRUE;
}




/*___________________________________________________________________________*/
/*________________________Constraigned_non_negative_integer__________________*/
/*___________________________________________________________________________*/
/*
Desc: Encodes an integer into b bits and puts it in TAIL of bit buffer.
Returns: Failure if cannot encode n.
Note: n <=2^32
*/
int
perEncodeNumber(IN  RvUint32 n,
                IN  RvUint32 b,  /* number of bits to hold the encoding */
                OUT HBB bbH) /* Should be initialized with sufficient size. */
{
    RvUint32 tmpNum;
    unsigned char numOct[MAX_INT_SIZE]; /*num converted to array in BIG ENDIAN*/

    if (b>MAX_INT_SIZE*8)
    {
        RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeNumber: Encoding not supported for %d bits.", b));
        return RV_ERROR_UNKNOWN;
    }

    if (!bbH || bbFreeBits(bbH) < (RvInt32)b)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeNumber: Buffer allocation error. [%d bits]", b));
        return RV_ERROR_UNKNOWN; /* Buffer allocation error */
    }

    /* Convert the number to 4 bytes in BIG ENDIAN order */
    tmpNum = n; /* 4 bytes integer */
    numOct[0] =(unsigned char)(tmpNum >> 24); /* n / 2^24 */
    tmpNum -= numOct[0] << 24;
    numOct[1] =(unsigned char)(tmpNum >> 16); /* n / 2^16 */
    tmpNum -= numOct[1] << 16;
    numOct[2] =(unsigned char)(tmpNum >> 8); /* n / 2^8 */
    tmpNum -= numOct[2] << 8;
    numOct[3] =(unsigned char)(tmpNum);

    return bbAddTailFrom(bbH, (RvUint8 *)numOct, MAX_INT_SIZE*8 - b, b, RV_FALSE);
}



/*
Desc: Decodes an integer from a bit buffer.
Returns: RV_ERROR_UNKNOWN ot RV_TRUE.
*/
int
perDecodeNumber(OUT RvUint32 *n, /* decoded number */
                IN  RvUint32 b,  /* number of bits to hold the encoding */
                IN  HPER hPer,
                IN  RvUint32 from, /* position in buffer */
                OUT RvUint32 *decoded)
{
    perStruct *per =(perStruct *)hPer;

    RvUint8 numOct[MAX_INT_SIZE]; /*num converted to array in BIG ENDIAN*/
    unsigned int bytes;

    memset((char *)numOct, 0, MAX_INT_SIZE);

    if (!n || !decoded)
    {
        RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeNumber: parameters (n,dec) not allocated."));
        return RV_ERROR_UNKNOWN;
    }
    *n = 0;
    *decoded = 0;

    if (b>MAX_INT_SIZE*8)
    {
        RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeNumber: Encoding not supported for %d bits.", b));
        return RV_ERROR_UNKNOWN;
    }

    if (!(per->hBB))
    {
        RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeNumber: Buffer allocation error."));
        return RV_ERROR_UNKNOWN; /* Buffer allocation error */
    }

    if (from + b > bbBitsInUse(per->hBB))
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeNumber: Number too large to decode from buffer [len=%d]", b));
        return RV_ERROR_UNKNOWN;
    }

    bytes = bbSetByte(b);
    if ((RvUint32)bbGet2Right(hPer, from, b, (RvUint8 *)(numOct + (MAX_INT_SIZE - bytes))) !=b)
    {
        RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeInt: Extracting failed [%d]", b));
        return RV_ERROR_UNKNOWN;
    }

    *n = numOct[3] +(numOct[2] << 8) +(numOct[1] << 16) +(numOct[0] << 24);
    *decoded = b;

    RvLogDebug(&rvPerLogSource, (&rvPerLogSource,
        " >> [%u] (%u:%u) => %u. \t \t number", b, from, *decoded, *n));
    return RV_TRUE;
}





/*___________________________________________________________________________*/
/*________________________________normally small_____________________________*/
/*___________________________________________________________________________*/


/*
clause '10.7'
*/
static int
perEncodeSemiConstInt(IN  RvUint32 n, /* the number */
                      IN  RvInt32 lb,
                      OUT HBB bbH)
{
    RvUint32 num, tmpNum, len;
    unsigned char numOct[MAX_INT_SIZE]; /*num converted to array in BIG ENDIAN*/

    if ((RvUint32)lb >n)
        return RV_ERROR_UNKNOWN;
    num = n - lb; /* to be encoded */

    /* 10.3: octet - aligned - bit - field as non - negative - binary - integer. */

    /* Convert the number to 4 bytes in BIG ENDIAN order */
    tmpNum = num; /* 4 bytes integer */
    numOct[0] =(unsigned char)(tmpNum >> 24); /* n / 2^24 */
    tmpNum -= numOct[0] << 24;
    numOct[1] =(unsigned char)(tmpNum >> 16); /* n / 2^16 */
    tmpNum -= numOct[1] << 16;
    numOct[2] =(unsigned char)(tmpNum >> 8); /* n / 2^8 */
    tmpNum -= numOct[2] << 8;
    numOct[3] =(unsigned char)tmpNum;

    len = bbSetByte(perIntBits(num));
    if (len == 0)
        len = 1; /* at least 1 octet */

    return bbAddTail(bbH, (RvUint8 *)(numOct + (MAX_INT_SIZE - len)), len*8, RV_TRUE);
}


static int
perDecodeSemiConstInt(IN  RvUint32 *n,
                      IN  RvInt32 lb,
                      IN  RvInt32 length, /* in octets */
                      IN  HPER hPer,
                      IN  RvUint32 from,
                      OUT RvUint32 *dec)
{
    RvUint8 numOct[MAX_INT_SIZE]; /*num converted to array in BIG ENDIAN*/
    RvUint32 num;
    RvInt32 extract;

    if (length>MAX_INT_SIZE)
    {
        return RV_ERROR_UNKNOWN;
    }

    memset((char *)numOct, 0, MAX_INT_SIZE);

    if ((extract = bbGet2Right(hPer, from, (RvUint32)(length*8), (RvUint8 *)(numOct + (MAX_INT_SIZE - length)))) <0)
    {
        RvLogWarning(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeSemiConstInt: Extracting failed [%d]", extract));
        return RV_ERROR_UNKNOWN;
    }

    num = numOct[3] +(numOct[2] << 8) +(numOct[1] << 16) +(numOct[0] << 24);
    num += lb;
    *n = num;
    *dec = (RvUint32)extract;
    return RV_TRUE;
}




/*___________________________________________________________________________*/
/*________________________________normally small_____________________________*/
/*___________________________________________________________________________*/

/*
clause '10.6'
*/
RVINTAPI int RVCALLCONV
perEncodeNormallySmallInt(IN  RvUint32 n, /* the number */
                          OUT HBB bbH)
{
	RvUint8 len;

    if (n <= 63)
    {
        /* 10.6.1 */

        perEncodeBool(RV_FALSE, bbH);
        return perEncodeInt(n, 0, 63, RV_FALSE, RV_FALSE, RV_FALSE, bbH);
    }

    /* 10.6.2:  ... | 1 | length | n | */
    perEncodeBool(RV_TRUE, bbH);

	/* Now encode the length determinant aligned into a single byte. Do it "ugly, but working" */
    len = (RvUint8)bbSetByte(perIntBits(n));
    if (bbAddTail(bbH, &len, 8, RV_TRUE) < 0)
	{
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeNormallySmallInt: length encoding failed for '%d'.", len));
        return RV_ERROR_UNKNOWN;
	}

	/* Now add the integer */
    if (perEncodeSemiConstInt(n, 0, bbH) < 0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeNormallySmallInt: number encoding failed for '%d'.", n));
        return RV_ERROR_UNKNOWN;
    }

    return RV_TRUE;
}


RVINTAPI int RVCALLCONV
perDecodeNormallySmallInt(OUT RvUint32 *n, /* the number */
                          IN  HPER hPer,
                          IN  RvUint32 from,
                          OUT RvUint32 *dec)
{
    RvBool boola = RV_FALSE;
    RvUint32 myDec = RV_FALSE;
    RvUint32 length;
    int ret = RV_TRUE;

    perDecodeBool(&boola, hPer, from, &myDec);
    *dec = myDec;
    if (boola == RV_FALSE)
    {
        /* 10.6.1 */
        ret = perDecodeInt(n, 0, 63, RV_FALSE, RV_FALSE, RV_FALSE, hPer, from + myDec, &myDec, "normally small int");
        *dec += myDec;
        return ret;
    }

    /* 10.6.2 */
	ret = perDecodeInt(&length, 0, 255, RV_FALSE, RV_FALSE, RV_FALSE, hPer, from + (*dec), &myDec, "normally small int (length)");
	*dec += myDec;
	if ((ret >= 0) && (length > 0))
	{
        ret = perDecodeSemiConstInt(n, 0, (RvInt32)length, hPer, from + (*dec), &myDec);
        *dec += myDec;
	}

    return ret;
}




/*___________________________________________________________________________*/
/*________________________________Length_____________________________________*/
/*___________________________________________________________________________*/
/*
Desc: Encodes length and puts it in the TAIL of bit buffer.
Returns: Failure if cannot encode n.
Note: clause 10.9
as CONSTRAINED: ub <= 64K.
as UNCONSTRAINED: n < 16K.
*/
int
perEncodeLen(IN  perLenType type,  /* CONSTRAINED, NORMALLY_SMALL, UNCONSTRAINED */
             IN  RvUint32 n,  /* the length */
             IN  RvUint32 lb,  /* only for constrained type */
             IN  RvUint32 ub,  /* only for constrained type */
                             OUT HBB bbH) /* Should be initialized with sufficient size.
                             It has to have at least 4 bytes size. */
{
    if (bbFreeBytes(bbH) < MAX_INT_SIZE)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "per:EncodeLen: Buffer allocation error."));
        bbSetOverflow(bbH);
        return RV_ERROR_UNKNOWN; /* Buffer allocation error */
    }

    if (type == perLenTypeCONSTRAINED && ub < K64) /* 10.9.3.3 */
        return perEncodeInt(n, lb, ub, RV_FALSE, RV_FALSE, RV_FALSE, bbH);

    if (type == perLenTypeNORMALLY_SMALL) /* 10.9.3.4 extension addition bitmap length of set or sequence type */
    {
        if (n <= 64)
        {
            perEncodeBool(RV_FALSE, bbH);
            return perEncodeInt(n - 1, 0, 63, RV_FALSE, RV_FALSE, RV_FALSE, bbH);
        }
        else
            perEncodeBool(RV_TRUE, bbH);
    }

    /* unconstrained length 10.9.3.5 */
    lb = 0;

    if (n <= 127) /* 10.9.3.6 */
        return perEncodeInt(n, 0, 255, RV_FALSE, RV_FALSE, RV_FALSE, bbH); /* 1 octet aligned. 10.5.7.2 */
    if (n < 16384)  /* 10.9.3.7 */
        return perEncodeInt(n + 0x8000, 0, (RvUint32)(K64 - 1), RV_FALSE, RV_FALSE, RV_FALSE, bbH); /* 2 octet aligned */


    /* fragmentation procedure */
    /* if (n >= 16384) - always the case if we got till here... */
    {
        RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeLen: fragmentation procedure not available [%d]", n));
    }

    /* not reached */
    return RV_ERROR_UNKNOWN;
}


/*
Desc: Decodes length from the bit buffer.(at position).
Returns: RV_ERROR_UNKNOWN or RV_TRUE.
Note: clause 10.9
*/
RVINTAPI int RVCALLCONV
perDecodeLen(IN  perLenType type, /* CONSTRAINED, NORMALLY_SMALL, UNCONSTRAINED */
             OUT RvUint32 *n, /* the length */
             IN  RvUint32 lb,
             IN  RvUint32 ub,
             OUT HPER hPer,
             IN  RvUint32 from, /* position in buffer */
             OUT RvUint32 *decoded)
{
    RvInt32 num;
    int ret;
    RvBool flag = RV_FALSE;
    RvUint32 dec = 0;

    if (!n || !decoded)
    {
        RvLogExcep(&rvPerErrLogSource, (&rvPerErrLogSource,
            "per:DecodeLen: parameters (n,del) not allocated."));
        return RV_ERROR_UNKNOWN;
    }
    *n = 0;
    *decoded = 0;

    if (type == perLenTypeCONSTRAINED && ub < K64) /* 10.9.3.3 */
    {
        return perDecodeInt(n, lb, ub, RV_FALSE, RV_FALSE, RV_FALSE, hPer, from, decoded, "constrained length");
    }

    if (type == perLenTypeNORMALLY_SMALL)
    {
		if (perDecodeBool(&flag, hPer, from, &dec) < 0)
		{
			RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
				"per:DecodeLen: error decoding normally small length (1st bit)"));
			return RV_ERROR_UNKNOWN;
		}

        *decoded += dec;
        from += dec;

        if (!flag)
        {
            /* 10.9.3.4: n <= 64 */

            ret = perDecodeInt(n, 0, 63, RV_FALSE, RV_FALSE, RV_FALSE, hPer, from, &dec, "normally small length");

            *decoded += dec;
            from += dec;
            (*n)++;
            return ret;
        }
    }

    /* unconstrained length 10.9.3.5 */

    ret = perDecodeInt(&flag, 0, 255, RV_FALSE, RV_FALSE, RV_FALSE, hPer, from, &dec, "unconstrained length (1st byte)");  /* decode 1 aligned octet */

    *decoded += dec;
    from += dec;
    if (ret < 0)
        return ret;
    if (!(flag & 1 << 7))
    {
        /* 1st bit =0 ==> 1 octet aligned. 10.5.7.2 */
        *n = flag;
        return RV_TRUE;
    }
    if (!(flag & 1 << 6))
    {
        /* bits = 10 ==> 10.9.3.7: n < 16384 */
        num = flag & ~(1 << 7);
        num = num << 8;

        ret = perDecodeInt(&flag, 0, 255, RV_FALSE, RV_FALSE, RV_FALSE, hPer, from, &dec, "unconstrained length (2 bytes)");  /* decode 1 aligned octet */

        *decoded += dec;
        from += dec;
        *n = num + flag;
        return RV_TRUE;
    }

    RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
        "perDecodeLen: fragmentation procedure not available [%d]", *n));
    return RV_ERROR_UNKNOWN;
}

/*___________________________________________________________________________*/
/*________________________________Boolean____________________________________*/
/*___________________________________________________________________________*/
/*
Desc: Encodes boolean and puts it in the TAIL of bit buffer.
Returns: Failure if cannot encode n.
Note: clause 11
*/
int
perEncodeBool(IN  RvBool n,
OUT HBB bbH) /* Should be initialized with sufficient size.
It has to have at least 1 byte size. */
{
    int res;
    static RvUint8 boolValues[2] = {0, 128};

    res = bbAddTail(bbH, boolValues + (n == RV_TRUE), 1, RV_FALSE);
    if (res < 0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource, "per:EncodeBool: Buffer allocation error."));
    }

    return res;
}


/*
Desc: Decodes boolean from buffer(at position).
Returns: RV_ERROR_UNKNOWN or positive number.
Note: clause 10.9
Note: clause 11
*/
RVINTAPI int RVCALLCONV
perDecodeBool(OUT RvBool *n,
              IN  HPER hPer,
              IN  RvUint32 from,
              OUT RvUint32 *decoded)
{
    RvUint8 value = 0;
    int res;
    
    res = bbGet2Right(hPer, from, 1, &value);
    if (res < 0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource, "per:DecodeBool: Extracting failed"));
        return res;
    }

    *decoded += res;
    *n = (value != 0);

    return RV_TRUE;
}



#ifdef __cplusplus
}
#endif



