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

#include "rvstdio.h"
#include "copybits.h"
#include "coderbitbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif



/*
   Desc: Set correct number of bytes in length of bits.
   */
unsigned int
bbSetByte(RvUint32 bitLength)
{
    /*unsigned int bytes = (unsigned int)(bitLength / 8);*/
    unsigned int bytes = (unsigned int)((bitLength + 7) >> 3);

    return bytes;
}

/*_____________________________________________________________________*/
/*_________________________________MODULE______________________________*/
/*_____________________________________________________________________*/

RVINTAPI int RVCALLCONV
bbGetAllocationSize(int maxOctets)
{
    return sizeof(bbStruct) + maxOctets;
}


static bbStruct *
bbBuild(IN char *buffer,
    IN int maxOctets)
{
    bbStruct *bb;

    memset(buffer, 0, (RvSize_t)bbGetAllocationSize(maxOctets));
    bb = (bbStruct *)buffer;

    bb->maxOctets = (unsigned int)maxOctets;
    bb->bitsInUse = 0;
    bb->octets = (RvUint8 *)bb + sizeof(bbStruct);
    bb->isOverflowOfBuffer = RV_FALSE;

    return bb;
}


/*
  Desc: Construct bb from user specified memory location.
  Note: user memory size should be at least the size of bb structure!
  */
RVINTAPI HBB RVCALLCONV
bbConstructFrom(IN int maxOctets, /* size of buffer in octetes */
        IN char *buffer,
        IN int bufferSize)
{
    bbStruct *bb;

    if (!buffer || bufferSize < bbGetAllocationSize(maxOctets)) return NULL;
    bb = bbBuild(buffer, maxOctets);
    return (HBB)bb;
}


/*
   Desc: Set the octets to be octetBuffer.
   Usage: for user allocated buffer.
   */
RVINTAPI int RVCALLCONV
bbSetOctets(IN  HBB bbH,
        IN  int maxOctets, /* size of buffer in octets */
        IN  RvInt32 bitsInUse, /* number of bits already in use */
        IN  RvUint8 *octetBuffer) /* octet memory */
{
    bbStruct *bb = (bbStruct *)bbH;

    if (!octetBuffer) return RV_ERROR_UNKNOWN;
    bb->maxOctets = (unsigned int)maxOctets;
    bb->bitsInUse = (RvUint32)bitsInUse;
    bb->octets = octetBuffer;
    return RV_TRUE;
}




/*_____________________________________________________________________*/
/*_________________________________STATUS______________________________*/
/*_____________________________________________________________________*/

/* returns pointer to the octet array */
RvUint8 *
bbOctets(HBB bbH)
{
    bbStruct *bb = (bbStruct *)bbH;

    return bb->octets;
}


/* return number of alignment bits (modulu 8) */
int
bbAlignBits(HBB bbH,
        IN RvInt32 location)
{
    RV_UNUSED_ARG(bbH);

    /*return (8 - (int)location%8)%8;*/
    return (8 - ((int)location & 0x07)) & 0x07;
}


/* return number of bits in use */
int
bbAlignTail(HBB bbH)
{
    bbStruct *bb = (bbStruct *)bbH;
    RvInt32 bbPos, align;
    RvUint8 ch=0;

    /* we don't just need to align the tail, we need to pad with ZERO */
    /*bb->bitsInUse += (RvUint32)((8 - ((int)bb->bitsInUse & 0x07)) & 0x07);*/
    bbPos = bb->bitsInUse;
    align = bbAlignBits(bbH, bbPos);
    if (align > 0)
    {
        memcpyb(bb->octets, (RvUint32)bbPos, &ch, 0, (RvUint32)align);
        bb->bitsInUse += align;
    }

	return bb->bitsInUse;
}




/*
  Desc: Return number of BITS left free in buffer. Meaning you may add
  up to this number of bits to buffer.
*/
RvInt32
bbFreeBits(HBB bbH)
{
    bbStruct *bb = (bbStruct *)bbH;

    /*return (int)((bb->maxOctets * 8) - bb->bitsInUse);*/
    return (int)((bb->maxOctets << 3) - bb->bitsInUse);
}

/*
  Desc: Return number of BYTES left free in buffer. Meaning you may add
  up to this number of bytes to buffer. rounded up.
*/
int
bbFreeBytes(HBB bbH)
{
    bbStruct *bb = (bbStruct *)bbH;

    return (bb->maxOctets - bbSetByte(bb->bitsInUse));
}


/*
  Desc: Return number of BITS currently in buffer.
  */
RvUint32
bbBitsInUse(HBB bbH)
{
    bbStruct *bb = (bbStruct *)bbH;

    return bb->bitsInUse;
}


/*
  Desc: Return number of BYTES currently in buffer.
  */
unsigned
bbBytesInUse(HBB bbH)
{
    bbStruct *bb = (bbStruct *)bbH;

    return bbSetByte(bb->bitsInUse);
}


/*_____________________________________________________________________*/
/*_________________________________UPDATE______________________________*/
/*_____________________________________________________________________*/

/* concatenate src to buffer */
int
bbAddTail(HBB bbH,
      IN RvUint8 *src,
      IN RvUint32 srcBitsLength,
      IN RvBool isAligned) /* true if src is aligned */
{
    bbStruct *bb = (bbStruct *)bbH;
    RvInt32 bbPos, align;
    RvUint8 ch=0;

    if (!src && srcBitsLength!=0)
        return RV_ERROR_UNKNOWN;

    bbPos = bb->bitsInUse;
    if (isAligned)
    {  /* align to octet boundary. 0 padding */
        align = bbAlignBits(bbH, bbPos);
        memcpyb(bb->octets, (RvUint32)bbPos, &ch, 0, (RvUint32)align);
        bbPos += align;
    }

    if (bbPos + srcBitsLength > bb->maxOctets * 8)
    {
        bb->isOverflowOfBuffer = RV_TRUE;
        return RV_ERROR_UNKNOWN; /* no place */
    }
    memcpyb(bb->octets, (RvUint32)bbPos, src, 0, srcBitsLength);
    bb->bitsInUse = bbPos + srcBitsLength;
    return RV_TRUE;
}



/* concatenate src to buffer */
int
bbAddTailFrom(HBB bbH,
          IN  RvUint8 *src,
          IN  RvUint32 srcFrom, /* offset for beginning of data in src, in bits */
          IN  RvUint32 srcBitsLength,
          IN  RvBool isAligned) /* true if src is aligned */
{
    bbStruct *bb = (bbStruct *)bbH;
    RvInt32 bbPos, align;
    RvUint8 ch=0;

    if (!src) return RV_ERROR_UNKNOWN;

    bbPos = bb->bitsInUse;
    if (isAligned)
    {  /* align to octet boundary. 0 padding */
        align = bbAlignBits(bbH, bbPos);
        memcpyb(bb->octets, (RvUint32)bbPos, &ch, 0, (RvUint32)align);
        bbPos += align;
    }

    if (bbPos + srcBitsLength > bb->maxOctets * 8)
    {
        bb->isOverflowOfBuffer=RV_TRUE;
        return RV_ERROR_UNKNOWN; /* no place */
    }
    memcpyb(bb->octets, (RvUint32)bbPos, src, srcFrom, srcBitsLength);
    bb->bitsInUse = bbPos + srcBitsLength;
    return RV_TRUE;
}



/* move bits within buffer
   bitLength bits starting at fromOffset shall be moved to position starting at toOffset */
int
bbMove(HBB bbH,
       RvUint32 fromOffset,
       RvUint32 bitLength,
       RvUint32 toOffset)
{
    bbStruct *bb = (bbStruct *)bbH;

    if (fromOffset + bitLength > bbBitsInUse(bbH) ||
        toOffset + bitLength > bb->maxOctets * 8)
    {  /* no place to hold bits */
        bb->isOverflowOfBuffer=RV_TRUE;
        return RV_ERROR_UNKNOWN;
    }
    memcpyb(bb->octets, toOffset, bb->octets, fromOffset, bitLength);

    /* update current size */
    bb->bitsInUse = RvMax(bb->bitsInUse, toOffset+bitLength);

    return RV_TRUE;
}




/*
   Desc: Set bits within buffer.
   */
int
bbSet(HBB bbH,
      IN  RvUint32 fromOffset,
      IN  RvUint32 bitLength,
      IN  RvUint8 *src)
{
    bbStruct *bb = (bbStruct *)bbH;

    if (!src) return RV_ERROR_UNKNOWN;
    if (fromOffset + bitLength > bb->maxOctets * 8)
    {  /* no place to hold bits */
        bb->isOverflowOfBuffer=RV_TRUE;
        return RV_ERROR_UNKNOWN;
    }
    memcpyb(bb->octets, fromOffset, src, 0, bitLength);

    /* update current size */
    bb->bitsInUse = RvMax(bb->bitsInUse, fromOffset+bitLength);

    return RV_TRUE;
}


void bbSetOverflow(HBB bbH)
{
    bbStruct *bb = (bbStruct *)bbH;

    bb->isOverflowOfBuffer = RV_TRUE;
}





#ifdef __cplusplus
}
#endif



