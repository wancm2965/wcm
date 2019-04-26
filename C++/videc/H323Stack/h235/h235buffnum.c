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
#include "h235buffnum.h"
#include "h235genrand.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 0
}
#endif



/* Res = A + B
There is no problem with Res being A or B or NULL.
The final carry from the addition is returned - in case it is not zero, there has been an overflow.
*/
RvUint16 rvh323BuffnumAdd(RvUint16 * Res, RvUint16 * A, RvUint16 * B, int size)
{
    int i;
    RvUint16 carry = 0;
    RvUint32 tempRes;

    for (i = 0; i < size; i++)
    {
        tempRes = (RvUint32) (((RvUint32)A[i]) + ((RvUint32)B[i]) + ((RvUint32)carry));
        if(Res) Res[i] = (RvUint16) tempRes;
        carry = (RvUint16) (tempRes >> 16);
    }

    return carry;
}

/* A = A + 2
The final carry from the addition is returned - in case it is not zero, there has been an overflow.
*/
RvUint16 rvh323BuffnumIncr2(RvUint16 * A, int size)
{
    int i;
    RvUint16 carry = 0;
    RvUint32 tempRes;

    tempRes = (RvUint32) (((RvUint32)A[0]) + 2);
    A[0] = (RvUint16) tempRes;
    carry = (RvUint16) (tempRes >> 16);

    for (i = 1; (i < size) && (carry != 0); i++)
    {
        tempRes = (RvUint32) (((RvUint32)A[i]) + ((RvUint32)carry));
        A[i] = (RvUint16) tempRes;
        carry = (RvUint16) (tempRes >> 16);
    }

    return carry;
}

/* Res = A - B
There is no problem with Res being A or B. It may also be NULL for use in < > functionality.
The final borrow from the substraction is returned - in case it is not zero, there has been an overflow.
Do try not to make that happen.
*/
RvUint16 rvh323BuffnumSubst(RvUint16 * Res, RvUint16 * A, RvUint16 * B, int size)
{
    int i;
    RvUint16 borrow = 0;
    RvInt32 tempRes;

    for (i = 0; i < size; i++)
    {
        tempRes = (RvInt32) (((RvInt32)A[i]) - ((RvInt32)B[i]) - borrow);
        if (tempRes < 0)
        {
            borrow = 1;
            if(Res) Res[i] = (RvUint16) (tempRes & 0x0000ffff);
        }
        else
        {
            borrow = 0;
            if(Res) Res[i] = (RvUint16) tempRes;
        }
    }

    return borrow;
}

/* Res = A * B
Res must not be be A nor B. size of Res must be twice that of A and B.
The final carry from the multiplication is returned - in case it is not zero, there has been an overflow.
Please try and make sure that the size is enough for the multiplication.
*/
RvUint16 rvh323BuffnumMult(RvUint16 * Res, RvUint16 * A, RvUint16 * B, int size)
{
    int i, j = 0, Imax, Jmax;
    RvUint16 carry = 0;
    RvUint32 tempRes;

    memset(Res, 0, (RvSize_t)size*sizeof(RvUint16));

    for(Imax=(size-1); Imax>=0; Imax--)
    {
        if (A[Imax])
            break;
    }
    for(Jmax=(size-1); Jmax>=0; Jmax--)
    {
        if (B[Jmax])
            break;
    }
    Jmax++;

    /* long multiplication:
                              123 *
                              456
                          --------
                              738
                             615  +
                            492   +
                          --------
                            560880
    */
    for (i = 0; (i < size) && (i<=Imax); i++)
    {
        carry = 0;

        for (j = 0; ((j+i) < size) && (j<=Jmax); j++)
        {
            tempRes = (RvUint32) ((((RvUint32)A[i]) * ((RvUint32)B[j])) + ((RvUint32)carry) + ((RvUint32) Res[i+j]));
            Res[i+j] = (RvUint16) tempRes;
            carry = (RvUint16) (tempRes >> 16);
        }
    }

    return carry;
}

static RvUint16 rvh323BuffnumMultByByte(RvUint16 * Res, RvUint16 * A, RvUint16 B, int size)
{
    int i, Imax;
    RvUint16 carry = 0;
    RvUint32 tempRes;

    memset(Res, 0, (RvSize_t)size*sizeof(RvUint16));

    for(Imax=(size-1); Imax>=0; Imax--)
    {
        if (A[Imax])
            break;
    }
    Imax++;
    carry = 0;
    for (i = 0; ((i) < size) && (i<=Imax); i++)
    {
        tempRes = (RvUint32) ((((RvUint32)A[i]) * ((RvUint32)B)) + ((RvUint32)carry) + ((RvUint32) Res[i]));
        Res[i] = (RvUint16) tempRes;
        carry = (RvUint16) (tempRes >> 16);
    }

    return carry;
}


static RvUint16 rvh323BuffnumGetNextByte(RvUint16 * A, int startByte, int startBit)
{
    RvUint16 ret;

    if (startByte >= 0)
        ret = (RvUint16)(A[startByte] >> startBit);
    else
        ret = 0;

    ret = (RvUint16)((A[startByte+1] << (16-startBit)) | ret);

    return ret;
}

static void rvh323BuffnumSetNextByte(RvUint16 * A, int startByte, int startBit, RvUint16 byteToSet)
{
    switch(startBit)
    {
    case(0):
        if(startByte >= 0) A[startByte]   = (RvUint16) byteToSet;
        break;
    case(1):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x0001) | (byteToSet << 1));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xfffe) | (byteToSet >> 15));
        break;
    case(2):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x0003) | (byteToSet << 2));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xfffc) | (byteToSet >> 14));
        break;
    case(3):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x0007) | (byteToSet << 3));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xfff8) | (byteToSet >> 13));
        break;
    case(4):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x000f) | (byteToSet << 4));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xfff0) | (byteToSet >> 12));
        break;
    case(5):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x001f) | (byteToSet << 5));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xffe0) | (byteToSet >> 11));
        break;
    case(6):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x003f) | (byteToSet << 6));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xffc0) | (byteToSet >> 10));
        break;
    case(7):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x007f) | (byteToSet << 7));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xff80) | (byteToSet >> 9));
        break;
    case(8):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x00ff) | (byteToSet << 8));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xff00) | (byteToSet >> 8));
        break;
    case(9):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x01ff) | (byteToSet << 9));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xfe00) | (byteToSet >> 7));
        break;
    case(10):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x03ff) | (byteToSet << 10));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xfc00) | (byteToSet >> 6));
        break;
    case(11):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x07ff) | (byteToSet << 11));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xf800) | (byteToSet >> 5));
        break;
    case(12):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x0fff) | (byteToSet << 12));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xf000) | (byteToSet >> 4));
        break;
    case(13):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x1fff) | (byteToSet << 13));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xe000) | (byteToSet >> 3));
        break;
    case(14):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x3fff) | (byteToSet << 14));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0xc000) | (byteToSet >> 2));
        break;
    case(15):
        if(startByte >= 0) A[startByte]   = (RvUint16) ((A[startByte]   & 0x7fff) | (byteToSet << 15));
        A[startByte+1] = (RvUint16) ((A[startByte+1] & 0x8000) | (byteToSet >> 1));
        break;
    default:
        break;
    }
    return;
}

/* Res = A / B,  Mod = A % B
Res may be omitted.
*/
RvUint16 rvh323BuffnumDivMod(RvUint16 * Res, RvUint16 * Mod, RvUint16 * A, RvUint16 * B, int size)
{
    int topByteA = 0, topByteB = 0, topBitA = 0, topBitB = 0, lowByteA, lowBitA;
    RvBool bLoopBreak = RV_FALSE;
    RvBool bOneUp = RV_FALSE; /* used in the subst algorithem to signefy the we have somwhere to borrow from */

    /* Find the topmost byte and bit of A */
    for (topByteA = (size - 1); topByteA >= 0; topByteA--)
    {
        if (A[topByteA] != 0)
        {
            for (topBitA = 15; topBitA >= 0; topBitA--)
            {
                if ((A[topByteA] & (1 << topBitA)) != 0)
                {
                    bLoopBreak = RV_TRUE;
                    break;
                }
            }
            if(bLoopBreak)
            {
                bLoopBreak = RV_FALSE;
                break;
            }
        }
    }
    /* Find the topmost byte and bit of B */
    for (topByteB = (size - 1); topByteB >= 0; topByteB--)
    {
        if (B[topByteB] != 0)
        {
            for (topBitB = 15; topBitB >= 0; topBitB--)
            {
                if ((B[topByteB] & (1 << topBitB)) != 0)
                {
                    bLoopBreak = RV_TRUE;
                    break;
                }
            }
            if(bLoopBreak)
            {
                bLoopBreak = RV_FALSE;
                break;
            }
        }
    }

    /* Get the byte and bit of A's bit aligned to the last bit of B when B's top is aligned to A's top. */
    lowByteA = topByteA - topByteB;
    lowBitA = topBitA - topBitB;
    if(lowBitA < 0)
    {
        lowBitA += 16;
        lowByteA--;
    }
    if(lowByteA < 0)
    {
        /* B is larger than A */
        if(Mod) memcpy(Mod, A, (RvSize_t)size*sizeof(RvUint16));
        if(Res) memset(Res, 0, (RvSize_t)size*sizeof(RvUint16));
        return 0;
    }

    /* copy A to the mod, after substraction, the mod will be left */
    memcpy(Mod, A, (RvSize_t)size*sizeof(RvUint16));

    /* start long division */
    while(lowByteA >= 0)
    {
        int indBitA, indBitB, indByteA, indByteB;
        RvBool bSubst = RV_FALSE;

        if(bOneUp)
        {
            /* we have a 1 from the last calculation which was not substracted and will ensure that we can
            substaract now (and borrow from that 1 */
            bSubst = RV_TRUE;
        }
        else
        {
            indBitA = topBitA;
            indByteA = topByteA;
            indBitB = topBitB;
            indByteB = topByteB;

            /* mark it if the first bit in A is 1 */
            if ((Mod[indByteA] & (1 << indBitA)) != 0)
            {
                bOneUp = RV_TRUE;
            }
            /* check if this bit alignment can substract B from A */
            while(indByteB >= 0)
            {
                RvBool bitA = ((Mod[indByteA] & (1 << indBitA)) != 0);
                RvBool bitB = ((B[indByteB]   & (1 << indBitB)) != 0);
                if ( (bitA == bitB) )
                {
                    /* check next bit */
                    indBitA--;
                    if(indBitA < 0)
                    {
                        indBitA = 15;
                        indByteA--;
                    }
                    indBitB--;
                    if(indBitB < 0)
                    {
                        indBitB = 15;
                        indByteB--;
                    }
                }
                else
                {
                    /* if A is larger, go ahead and substract */
                    bSubst = (bitA > bitB);
                    /* in any case, move the alignment in a bit */
                    break;
                }
            }
            if(indByteB < 0)
            {
                /* full match - we can still substract */
                bSubst = RV_TRUE;
            }
        }

        if (bSubst)
        {
            /* substracting B from A (actually the Mod) */
            RvInt32 borrow = 0;

            /* set the lowest bits */
            indBitA = lowBitA;
            indByteA = lowByteA;
            indBitB = 0;
            indByteB = 0;

            /* set the bit in the result */
            if (Res)
            {
                Res[lowByteA] = (RvUint16) (Res[lowByteA] | (1 << lowBitA));
            }

            /* byte-wise substraction */
            while( (indByteA < topByteA) || ((indByteA == topByteA) && (indBitA <= topBitA)) )
            {
                RvInt32 byteA = (RvInt32) rvh323BuffnumGetNextByte(Mod, indByteA, indBitA);
                RvInt32 byteB = (RvInt32) rvh323BuffnumGetNextByte(B, indByteB, indBitB);
                RvInt32 res = (RvInt32) (byteA - byteB - borrow);

                rvh323BuffnumSetNextByte(Mod, indByteA, indBitA, (RvUint16) (res & 0x0000ffff));
                borrow = (RvInt32) (res < 0);

                indByteA++;
                indByteB++;
            }
            /* if we had a bit up, we must have borrowed from it */
            if(bOneUp)
            {
                /* set the bit up for the next loop */
                bOneUp = ((Mod[topByteA] & (1 << topBitA)) != 0);
                /* unset it in the result, if needed */
                if ((indByteA < size) && ((Mod[indByteA] & (1 << indBitA)) != 0))
                    Mod[indByteA] = (RvUint16) (Mod[indByteA] ^ (1 << indBitA));
            }
        }

        /* slide B along A */
        topBitA--;
        if(topBitA < 0)
        {
            topBitA = 15;
            topByteA--;
        }
        lowBitA--;
        if(lowBitA < 0)
        {
            lowBitA = 15;
            lowByteA--;
        }
    }
    return 0;
}

int rvh323BuffnumDivModKnuthNoExt(RvUint16 * Res, RvUint16 * Mod, RvUint16 * A, RvUint16 * B, int size)
{
    int topByteA = 0, topByteB = 0, topBitA = 0, topBitB = 0, lowByteA, indByte, saveTopByteA;
    RvBool bLoopBreak = RV_FALSE;
    RvUint16 subst[MAX_BUFFNUM_SIZE];
    RvUint32 topB, top2B;

    /* Find the topmost byte and bit of A */
    for (topByteA = (size - 1); topByteA >= 0; topByteA--)
    {
        if (A[topByteA] != 0)
        {
            for (topBitA = 15; topBitA >= 0; topBitA--)
            {
                if ((A[topByteA] & (1 << topBitA)) != 0)
                {
                    bLoopBreak = RV_TRUE;
                    break;
                }
            }
            if(bLoopBreak)
            {
                bLoopBreak = RV_FALSE;
                break;
            }
        }
    }
    /* Find the topmost byte and bit of B */
    for (topByteB = (size - 1); topByteB >= 0; topByteB--)
    {
        if (B[topByteB] != 0)
        {
            for (topBitB = 15; topBitB >= 0; topBitB--)
            {
                if ((B[topByteB] & (1 << topBitB)) != 0)
                {
                    bLoopBreak = RV_TRUE;
                    break;
                }
            }
            if(bLoopBreak)
            {
                bLoopBreak = RV_FALSE;
                break;
            }
        }
    }

    /* first check if we're not dividing by zero */
    if (topByteB < 0)
        return -1;

    /* init Res to zero */
    if (Res)
        memset(Res, 0, (RvSize_t)size*sizeof(RvUint16));

    /* now see if maybe B is larger than A */
    if ((topByteB > topByteA) ||
        ((topByteB == topByteA) && (topBitB > topBitA)) ||
        ((topByteB == topByteA) && (topBitB == topBitA) && (rvh323BuffnumSubst(NULL, B, A, size) != 0)))
    {
        /* B is larger than A: set Res to zero, set Mod to A, and we're done */
        memcpy(Mod, A, (RvSize_t)size*sizeof(RvUint16));
        return 0;
    }

    saveTopByteA = topByteA;
    if (topBitB == 15)
    {
        topBitB = 0;
    }
    else
    {
        topBitB++;
        topByteB--;
    }

    /* see if we'll need shifting */
    if (topBitB != 0)
    {
        /* now, instead of getting the next byte all over the code, I will shift the numbers once, and that's it */
        for (indByte = -1; indByte <= topByteA; indByte++)
            Mod[indByte+1] = rvh323BuffnumGetNextByte(A, indByte, topBitB);
    }
    else
    {
        memcpy(Mod, A, (RvSize_t)size*sizeof(RvUint16));
    }

    lowByteA = topByteA - topByteB - (topBitB?1:0);
    topB  = (RvUint32) rvh323BuffnumGetNextByte(B, topByteB,   topBitB);
    top2B = (RvUint32) rvh323BuffnumGetNextByte(B, topByteB-1, topBitB);

    /* start long division */
    while (lowByteA >= 0)
    {
        RvUint32 topA;
        RvUint32 times, rmndr;

        topA = (RvUint32) (Mod[topByteA+1] << 16) | Mod[topByteA];

        times = topA/topB;
        rmndr = topA%topB;

        do
        {
            if ((times == 0x10000) || ((times*top2B) > ((rmndr << 16) + Mod[topByteA-1])))
            {
                times--;
                rmndr += topB;
            }
            else
                break;
        }
        while (rmndr < 0x10000);

        if(times > 0)
        {
            int indByteA, indByteSub;
            RvBool carry = 0;

            memset(subst, 0, sizeof(subst));
            rvh323BuffnumMultByByte(subst, B, (RvUint16)times, size);

            indByteSub = (topBitB?(-1):0);
            for(indByteA = lowByteA; indByteA <= (topByteA+1); indByteA++)
            {
                RvUint16 byteA = Mod[indByteA];
                RvUint16 byteB = rvh323BuffnumGetNextByte(subst, indByteSub, topBitB);
                RvInt32 tempRes = byteA - byteB - carry;
                carry = (tempRes < 0);
                Mod[indByteA] = (RvUint16)tempRes;
                indByteSub++;
            }
            if (carry)
            {
                carry = 0;
                times--;
                indByteSub = (topBitB?(-1):0);
                for(indByteA = lowByteA; indByteA <= (topByteA+1); indByteA++)
                {
                    RvUint16 byteA = Mod[indByteA];
                    RvUint16 byteB = rvh323BuffnumGetNextByte(B, indByteSub, topBitB);
                    RvInt32 tempRes = byteA + byteB + carry;
                    carry = ((tempRes >> 16) != 0);
                    Mod[indByteA] = (RvUint16)tempRes;
                    indByteSub++;
                }
            }
            if (Res)
            {
                Res[lowByteA] = (RvUint16)times;
            }
        }
        lowByteA--;
        topByteA--;
    }
    /* see if we need shifting */
    if (topBitB != 0)
    {
        /* now, shift back */
        topBitB = 16-topBitB;
        for (indByte = 0; indByte <= saveTopByteA; indByte++)
            Mod[indByte] = rvh323BuffnumGetNextByte(Mod, indByte, topBitB);
    }
    return 0;
}

int rvh323BuffnumDivModKnuth(RvUint16 * Res, RvUint16 * Mod, RvUint16 * A, RvUint16 * B, int size)
{
    RvUint16 longMod[MAX_BUFFNUM_SIZE+1];
    memset(longMod, 0, sizeof(longMod));

    if(rvh323BuffnumDivModKnuthNoExt(Res, longMod, A, B, size) < 0)
        return -1;

    memcpy(Mod, longMod, (RvSize_t)size*sizeof(RvUint16));
    return 0;
}




/* Res = (A * B) % Mod */
void rvh323BuffnumMultMod(RvUint16 * Res, RvUint16 * A, RvUint16 * B, RvUint16 * Mod)
{
    RvUint16 tempRes[MAX_BUFFNUM_SIZE];

    rvh323BuffnumMult(tempRes, A, B, MAX_BUFFNUM_SIZE);
    rvh323BuffnumDivModKnuth(NULL, Res, tempRes, Mod, MAX_BUFFNUM_SIZE);
}


/* randomize a buffer */
void rvh323BuffnumRandomize(cmElem * app, RvUint16 * Res, int size)
{
    static RvRandom seed = 0;
    RvUint32 rndNum;
    int i, j;

    if(seed == 0)
    {
        RvRandomGeneratorGetValue(&app->randomGenerator, &seed);
        rvh323seedGenRand((RvUint32)seed);
    }

    for(i=(size-1); i>=0;)
    {
        rndNum = rvh323genRand();
        for(j=1; (i>=0) && (j>=0); i--, j--)
        {
            Res[i] = (RvUint16) (rndNum >> (j*16));
        }
    }
}

/* randomize a buffer but have it between A and B (A>B) */
void rvh323BuffnumRandomizeLimited(cmElem * app, RvUint16 * Res, RvUint16 * A, RvUint16 * B, int size)
{
    RvBool bOkBuffer = RV_FALSE;

    while(!bOkBuffer)
    {
        rvh323BuffnumRandomize(app, Res, size);
        if( ((B==NULL) || (rvh323BuffnumSubst(NULL, Res, B, size) == 0)) &&
            ((A==NULL) || (rvh323BuffnumSubst(NULL, A, Res, size) == 0)) )
        {
            bOkBuffer = RV_TRUE;
        }
    }
}


RvStatus rvh323BuffnumExpMod(RvUint16 * res, RvUint16 baseByFactor[3][MAX_BUFFNUM_SIZE], RvUint16 * exp, RvUint16 * mod)
{
    RvUint16 mulRes[MAX_BUFFNUM_SIZE];
    int indByte, indBit;

    memset(mulRes, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
    memset(res, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));

    /* set mod to be 1 */
    res[0] = 0x0001;

    /* find the first two most significat digits in the prime number */
    indByte = MAX_BUFFNUM_SIZE - 1;
    indBit = 14;
    while((indByte >= 0) && (exp[indByte] == 0x0000))
        indByte--;
    if (indByte < 0) return RV_ERROR_UNKNOWN;
    while((indBit >= 0) && ((exp[indByte] & (3 << indBit)) == 0))
        indBit -= 2;

    /* now go over the number */
    while (indByte >= 0)
    {
        /* get first two digits */
        int twoDig = ( (exp[indByte] >> indBit) & 3 );

        rvh323BuffnumMult(mulRes, res, res, MAX_BUFFNUM_SIZE);
        rvh323BuffnumDivModKnuth(NULL, res, mulRes, mod, MAX_BUFFNUM_SIZE);
        rvh323BuffnumMult(mulRes, res, res, MAX_BUFFNUM_SIZE);
        rvh323BuffnumDivModKnuth(NULL, res, mulRes, mod, MAX_BUFFNUM_SIZE);
        if(twoDig != 0)
        {
            rvh323BuffnumMult(mulRes, res, baseByFactor[twoDig-1], MAX_BUFFNUM_SIZE);
            rvh323BuffnumDivModKnuth(NULL, res, mulRes, mod, MAX_BUFFNUM_SIZE);
        }

        indBit -= 2;
        if (indBit < 0)
        {
            indBit = 14;
            indByte--;
        }
    }
    return RV_OK;
}

/* sets res to ((base^num) % mod) */
RvStatus rvh323BuffnumBaseExpMod(RvUint16 * res, RvUint16 * base, RvUint16 * exp, RvUint16 * mod)
{
    RvUint16 baseByFactor[3][MAX_BUFFNUM_SIZE];
    memset(baseByFactor, 0, 3*MAX_BUFFNUM_SIZE*sizeof(RvUint16));

    /* set base, base^2, ans base^3 % mod */
    rvh323BuffnumDivModKnuth(NULL, baseByFactor[0], base, mod, MAX_BUFFNUM_SIZE);
    rvh323BuffnumMultMod(baseByFactor[1], baseByFactor[0], baseByFactor[0], mod);
    rvh323BuffnumMultMod(baseByFactor[2], baseByFactor[1], baseByFactor[0], mod);

    return rvh323BuffnumExpMod(res, baseByFactor, exp, mod);
}

/* sets res to ((2^num) % mod) */
RvStatus rvh323BuffnumTwoExpMod(RvUint16 * res, RvUint16 * exp, RvUint16 * mod)
{
    RvUint16 twoByFactor[3][MAX_BUFFNUM_SIZE];
    memset(twoByFactor, 0, 3*MAX_BUFFNUM_SIZE*sizeof(RvUint16));

    /* set 2, 2^2 and 2^3 */
    twoByFactor[0][0] = 0x0002;
    twoByFactor[1][0] = 0x0004;
    twoByFactor[2][0] = 0x0008;

    return rvh323BuffnumExpMod(res, twoByFactor, exp, mod);
}

static RvUint16 ONE_BYTE_PRIMES[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251 };

/* check if number is Prime
the buffer sizes must be MAX_BUFFNUM_SIZE, but the number must be half that, */
RvBool rvh323BuffnumCheckPrime(RvUint16 * num)
{
    RvUint16 smallPrime[MAX_BUFFNUM_SIZE];
    RvUint16 result[MAX_BUFFNUM_SIZE];
    int numOfPrimes = (sizeof(ONE_BYTE_PRIMES) / sizeof(RvUint16));
    int i;

    memset(smallPrime, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
    memset(result, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));

    /* check if any of the small primes divides the number */
    for (i = 0; i < numOfPrimes; i++)
    {
        smallPrime[0] = ONE_BYTE_PRIMES[i];
        rvh323BuffnumDivModKnuth(NULL, result, num, smallPrime, MAX_BUFFNUM_SIZE);
        smallPrime[0] = 0x0000;

        /* compare to zero to see if it devided */
        if (!memcmp(result, smallPrime, MAX_BUFFNUM_SIZE*sizeof(RvUint16)))
        {
            return RV_FALSE;
        }
        memset(result, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
    }

    /* now check if 2^(num-1) = 1 (% num) (i.e. Fermat) */
    /* in our case, check if 2^num = 2 (%num) */
    if(rvh323BuffnumTwoExpMod(result, num, num) != RV_OK)
        return RV_FALSE;
    smallPrime[0] = 0x0002;
    if(memcmp(result, smallPrime, MAX_BUFFNUM_SIZE*sizeof(RvUint16)) == 0)
        return RV_TRUE;

    return RV_FALSE;
}


/* generate Prime of length 'length' in bytes. size must be at most MAX_BUFFNUM_SIZE. */
RvStatus rvh323BuffnumGeneratePrime(cmElem * app, RvUint16 * prime, int length)
{
    RvUint16 primeNum[MAX_BUFFNUM_SIZE];
    RvBool bFound = RV_FALSE;
    RvBool bCont = RV_TRUE;

    while(!bFound)
    {
        memset(primeNum, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));

        /* generate random number */
        rvh323BuffnumRandomize(app, primeNum, length);
        primeNum[length-1] = (RvUint16) (primeNum[length-1] | 0x8000);   /* make sure it's really up to size */
        primeNum[0]        = (RvUint16) (primeNum[0]        | 0x0001);   /* make sure it's odd */

        /* now search for a prime in steps of incr */
        while(bCont)
        {
            if(rvh323BuffnumCheckPrime(primeNum))
            {
                bFound = RV_TRUE;
                break;
            }
            rvh323BuffnumRandomize(app, primeNum, length);
            primeNum[length-1] = (RvUint16) (primeNum[length-1] | 0x8000);   /* make sure it's really up to size */
            primeNum[0]        = (RvUint16) (primeNum[0]        | 0x0001);   /* make sure it's odd */
        }
    }

    /* WE FOUND A PRIME !! - we always do :^) */
    memcpy(prime, primeNum, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
    return RV_OK;
}

RvStatus rvh323BuffnumTest(HAPP hApp)
{
    cmElem * app = (cmElem *) hApp;
    int i;
    int res = 0;

    RvUint16 A[MAX_BUFFNUM_SIZE];
    RvUint16 B[MAX_BUFFNUM_SIZE];
    RvUint16 C[MAX_BUFFNUM_SIZE];
    RvUint16 D[MAX_BUFFNUM_SIZE];
    RvUint16 E[MAX_BUFFNUM_SIZE];
    RvUint16 F[MAX_BUFFNUM_SIZE];

    /* simple math */
    for (i=0; i<100000; i++)
    {
        memset(A, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
        memset(B, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
        memset(C, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
        memset(D, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
        memset(E, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
        memset(F, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint16));

        rvh323BuffnumRandomize(app, A, MAX_BUFFNUM_SIZE/2);
        rvh323BuffnumRandomize(app, B, MAX_BUFFNUM_SIZE/4);

        if (rvh323BuffnumDivModKnuth(C, D, A, B, MAX_BUFFNUM_SIZE) < 0)
        {
            /* divide by zero */
            continue;
        }
        /*rvh323BuffnumDivMod(E, F, A, B, MAX_BUFFNUM_SIZE);*/
        rvh323BuffnumMult(E, C, B, MAX_BUFFNUM_SIZE);
        rvh323BuffnumAdd(E, E, D, MAX_BUFFNUM_SIZE);

        res = memcmp(E, A, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
        if (res != 0)
            return RV_ERROR_UNKNOWN;

        rvh323BuffnumAdd(E, C, A, MAX_BUFFNUM_SIZE);
        rvh323BuffnumSubst(E, E, A, MAX_BUFFNUM_SIZE);

        res = memcmp(E, C, MAX_BUFFNUM_SIZE*sizeof(RvUint16));
        if (res != 0)
            return RV_ERROR_UNKNOWN;
    }

    /* generating and checking primes *
    for(i=0; i < 20; i++)
    {
        memset(A, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint8));
        memset(B, 0, MAX_BUFFNUM_SIZE*sizeof(RvUint8));
        B[0] = 2;
        res = 0;

        rvh323BuffnumGeneratePrime(app, A, 32);

        for(j = 0; j<8; j++)
        {
            res += (RvUint64) ((RvUint64) A[j] << j*16);
        }

        for(j=3; j<0x10000; j+=2)
        {
            if ((res%j) == 0)
            {
                return RV_ERROR_UNKNOWN;
            }
        }
    }*/

    return RV_OK;
}






#if 0
{
#endif

#ifdef __cplusplus
}
#endif
