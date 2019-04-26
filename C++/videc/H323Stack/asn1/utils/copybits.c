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
  copybits.c

  Sasha F.
  */

#ifdef __cplusplus
extern "C" {
#endif

#include "rvstdio.h"
#include "copybits.h"
#include <string.h>


/* copy bits from src to dest */
void memcpyb(
         OUT RvUint8 *dest,
         IN  RvUint32 destBitPos,
         IN  RvUint8 *src,
         IN  RvUint32 srcBitPos,
         IN  RvUint32 numOfBits)
{
    RvUint32 i;

    if ((srcBitPos&7) == 0 && (destBitPos&7) == 0 && numOfBits>=8)
    {
        for (i=numOfBits-(numOfBits&7); i<numOfBits; i++)
        {
            setBit(dest, destBitPos+i, getBit(src, srcBitPos+i));
        }
        memmove(dest + (destBitPos>>3), src + (srcBitPos>>3), (RvSize_t)(numOfBits>>3));
    }
    else
    {
        /* optimization: looping down to 0 is faster than looping up to
           numOfBits on most machines */
        for (i = numOfBits; i > 0; i--)
        {
            setBit(dest, destBitPos, getBit(src, srcBitPos));
            destBitPos++;
            srcBitPos++;
        }
    }
}



/* This static array is used by get1st0BitNumber().
   Each value in the array is the index of the first 0 bit in the index value of
   the current array position. */
static const RvUint8 the1st0InByte[]=
{
/*      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  */
/*0*/   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*1*/   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*2*/   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*3*/   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*4*/   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*5*/   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*6*/   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*7*/   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*8*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*9*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*A*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*B*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*C*/   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
/*D*/   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
/*E*/   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
/*F*/   4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 8,
};


/************************************************************************
 * get1st0BitNumber
 * purpose: Find the first bit set to 0 within an array of bits.
 *          This function is used by H.245 for handling the session IDs
 *          and the logical channel numbers of faststart channels.
 * input  : bitsBuffer  - Pointer to bits buffer
 *          startBit    - First bit to look from
 *          stopBit     - Last bit in the buffer
 * output : none
 * return : The index of the first bit set to 0 in the array of bits
 *          It will return stopBit+1 if no 0 bit was found
 ************************************************************************/
RVINTAPI RvUint32 RVCALLCONV
get1st0BitNumber(
    IN RvUint8* bitsBuffer,
    IN RvUint32 startBit,
    IN RvUint32 stopBit)
{
    RvUint32 i, finishLine;
    RvUint8 bit;
    RvUint8 ext = (RvUint8)(startBit&7);

    if (ext)
    {
        /* We're beginning the check from the middle of a byte - we should make sure the
           bits that we don't check are all marked as 1 */
        RvUint8 c = bitsBuffer[startBit >> 3]; /* The byte we're checking in - in a copied character */
        c |= ~((0x80 >> ((startBit & 7) - 1)) - 1); /* Mark all unchecked bits as 1 */
        if (c != 0xff)
        {
            /* We have a 0 bit here - get it by the pre-calculated array */
            bit = the1st0InByte[c];
            return (RvUint32)bit+(startBit-ext);
        }
        startBit += (8-ext);
    }

    /* Let's loop in a bytes array until we get to a byte with zero bits inside it */
    finishLine = (stopBit + 8) >> 3;
    for (i = startBit >> 3; i < finishLine; i++)
        if (bitsBuffer[i] != 0xff)
        {
            /* We have a 0 bit here - get it by the pre-calculated array */
            bit = the1st0InByte[bitsBuffer[i]];
            return (RvUint32)bit + (i << 3);
        }

    /* No 0 bit found... */
    return stopBit + 1;
}




#ifdef __cplusplus
}
#endif
