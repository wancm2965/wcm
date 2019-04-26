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

#include "rvtypes.h"
#include "rverror.h"
#include "bitfield.h"
#include "rtp.h"
#include "rtputil.h"
#include "payload.h"

RVAPI
RvInt32 RVCALLCONV rtpPCMUPack(
                              IN    void*buf,
                              IN    RvInt32 len,
                              IN    rtpParam*p,
                              IN    void*param)
{
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(param);

    p->payload=PCMU;
    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpPCMUUnpack(
                                OUT void*buf,
                                IN  RvInt32 len,
                                OUT rtpParam*p,
                                OUT void*param)
{
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(param);
    RV_UNUSED_ARG(p);

    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpPCMUGetHeaderLength(void)
{
    return rtpGetHeaderLength();
}

RVAPI
RvInt32 RVCALLCONV rtpPCMAPack(
                              IN    void*buf,
                              IN    RvInt32 len,
                              IN    rtpParam*p,
                              IN    void*param)
{
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(param);

    p->payload=PCMA;
    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpPCMAUnpack(
                                OUT     void*buf,
                                IN  RvInt32 len,
                                OUT     rtpParam*p,
                                OUT void*param)
{
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(param);
    RV_UNUSED_ARG(p);

    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpPCMAGetHeaderLength(void)
{
    return rtpGetHeaderLength();
}

RVAPI
RvInt32 RVCALLCONV rtpG722Pack(
                              IN    void*buf,
                              IN    RvInt32 len,
                              IN    rtpParam*p,
                              IN    void*param)
{
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(param);

    p->payload=G722;
    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpG722Unpack(
                                OUT     void*buf,
                                IN  RvInt32 len,
                                OUT rtpParam*p,
                                OUT void*param)
{
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(param);
    RV_UNUSED_ARG(p);

    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpG722GetHeaderLength(void)
{
    return rtpGetHeaderLength();
}

RVAPI
RvInt32 RVCALLCONV rtpG728Pack(
                              IN    void*buf,
                              IN    RvInt32 len,
                              IN    rtpParam*p,
                              IN    void*param)
{
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(param);

    p->payload=G728;
    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpG728Unpack(
                                OUT void*buf,
                                IN  RvInt32 len,
                                OUT rtpParam*p,
                                OUT void*param)
{
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(param);
    RV_UNUSED_ARG(p);

    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpG728GetHeaderLength(void)
{
    return rtpGetHeaderLength();
}

RVAPI
RvInt32 RVCALLCONV rtpG729Pack(
                              IN    void*buf,
                              IN    RvInt32 len,
                              IN    rtpParam*p,
                              IN    void*param)
{
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(param);

    p->payload=G729;
    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpG729Unpack(
                                OUT void*buf,
                                IN  RvInt32 len,
                                OUT rtpParam*p,
                                OUT void*param)
{
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(param);
    RV_UNUSED_ARG(p);

    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpG729GetHeaderLength(void)
{
    return rtpGetHeaderLength();
}

RVAPI
RvInt32 RVCALLCONV rtpG7231Pack(
                              IN    void*buf,
                              IN    RvInt32 len,
                              IN    rtpParam*p,
                              IN    void*param)
{
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(param);

    p->payload=G7231;
    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpG7231Unpack(
                                OUT void*buf,
                                IN  RvInt32 len,
                                OUT rtpParam*p,
                                OUT void*param)
{
    RV_UNUSED_ARG(buf);
    RV_UNUSED_ARG(len);
    RV_UNUSED_ARG(param);
    RV_UNUSED_ARG(p);

    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpG7231GetHeaderLength(void)
{
    return rtpGetHeaderLength();
}


RVAPI
RvInt32 RVCALLCONV rtpH261Pack(
                              IN    void*buf,
                              IN    RvInt32 len,
                              IN    rtpParam*p,
                              IN    void*param)
{
    H261param*h261=(H261param*)param;
    RvUint32*hPtr;
    p->sByte-=4;
    hPtr=(RvUint32*)((RvUint8*)buf+p->sByte);
    hPtr[0]=bitfieldSet(0,h261->v,24,1);
    hPtr[0]=bitfieldSet(hPtr[0],h261->i,25,1);

    RV_UNUSED_ARG(len);

    if (h261->gobN)
    {
        hPtr[0]=bitfieldSet(hPtr[0],h261->vMvd,0,5);
        hPtr[0]=bitfieldSet(hPtr[0],h261->hMvd,5,5);
        hPtr[0]=bitfieldSet(hPtr[0],h261->quant,10,5);
        hPtr[0]=bitfieldSet(hPtr[0],h261->mbaP,15,5);
        hPtr[0]=bitfieldSet(hPtr[0],h261->gobN,20,4);
    }
    hPtr[0]=bitfieldSet(hPtr[0],h261->eBit,26,3);
    hPtr[0]=bitfieldSet(hPtr[0],h261->sBit,29,3);
    p->payload=H261;

    ConvertToNetwork(hPtr, 0, 1);

    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpH261Unpack(
                                OUT void*buf,
                                IN  RvInt32 len,
                                OUT rtpParam*p,
                                OUT void*param)
{
    H261param*h261=(H261param*)param;
    RvUint32*hPtr=(RvUint32*)((RvUint8*)buf+p->sByte);

    RV_UNUSED_ARG(len);

    p->sByte += 4;
    ConvertFromNetwork(hPtr, 0, 1);

    h261->vMvd=bitfieldGet(hPtr[0],0,5);
    h261->hMvd=bitfieldGet(hPtr[0],5,5);
    h261->quant=bitfieldGet(hPtr[0],10,5);
    h261->mbaP=bitfieldGet(hPtr[0],15,5);
    h261->gobN=bitfieldGet(hPtr[0],20,4);
    h261->v=bitfieldGet(hPtr[0],24,1);
    h261->i=bitfieldGet(hPtr[0],25,1);
    h261->eBit=bitfieldGet(hPtr[0],26,3);
    h261->sBit=bitfieldGet(hPtr[0],29,3);

    return RV_OK;
}

RVAPI
RvInt32 RVCALLCONV rtpH261GetHeaderLength()
{
    return rtpGetHeaderLength()+4;
}


RvInt32 RVCALLCONV rtpH263Pack(
                              IN    void*buf,
                              IN    RvInt32 len,
                              IN    rtpParam*p,
                              IN    void*param)
{
    H263param*h263=(H263param*)param;
    RvUint32*hPtr=NULL;
    int dwords=0;

    RV_UNUSED_ARG(len);

    if (h263->p)
    {
        dwords=1;
        p->sByte-=4;
        hPtr=(RvUint32*)((RvUint8*)buf+p->sByte);

        hPtr[0]=bitfieldSet(0,h263->dbq,11,2);
        hPtr[0]=bitfieldSet(hPtr[0],h263->trb,8,3);
        hPtr[0]=bitfieldSet(hPtr[0],h263->tr,0,8);
    }

    if (h263->f)
    {
        dwords+=2;
        p->sByte-=8;
        hPtr=(RvUint32*)((RvUint8*)buf+p->sByte);
        hPtr[0]=bitfieldSet(0,h263->mbaP,0,8);
        hPtr[0]=bitfieldSet(hPtr[0],h263->gobN,8,5);
        hPtr[0]=bitfieldSet(hPtr[0],h263->quant,16,5);


        hPtr[1]=bitfieldSet(      0,h263->vMv2, 0,8);
        hPtr[1]=bitfieldSet(hPtr[1],h263->hMv2, 8,8);
        hPtr[1]=bitfieldSet(hPtr[1],h263->vMv1,16,8);
        hPtr[1]=bitfieldSet(hPtr[1],h263->hMv1,24,8);
    }

    if (!h263->f && !h263->p)
    {
        dwords=1;
        p->sByte-=4;
        hPtr=(RvUint32*)((RvUint8*)buf+p->sByte);
        hPtr[0]=0;
    }

    hPtr[0]=bitfieldSet(hPtr[0],h263->f,31,1);
    hPtr[0]=bitfieldSet(hPtr[0],h263->p,30,1);
    hPtr[0]=bitfieldSet(hPtr[0],h263->sBit,27,3);
    hPtr[0]=bitfieldSet(hPtr[0],h263->eBit,24,3);
    hPtr[0]=bitfieldSet(hPtr[0],h263->src,21,3);
    hPtr[0]=bitfieldSet(hPtr[0],h263->i,15,1);
    hPtr[0]=bitfieldSet(hPtr[0],h263->a,14,1);
    hPtr[0]=bitfieldSet(hPtr[0],h263->s,13,1);
    p->payload=H263;

    ConvertToNetwork(hPtr, 0, dwords);

    return RV_OK;
}

RvInt32 RVCALLCONV rtpH263Unpack(
                                OUT     void*buf,
                                IN  RvInt32 len,
                                OUT     rtpParam*p,
                                OUT void*param)
{
    H263param*h263=(H263param*)param;
    RvUint32*hPtr=(RvUint32*)((RvUint8*)buf+p->sByte);

    RV_UNUSED_ARG(len);

    p->sByte+=4;

    ConvertFromNetwork(hPtr, 0, 1);

    h263->f=bitfieldGet(hPtr[0],31,1);
    h263->p=bitfieldGet(hPtr[0],30,1);

    h263->sBit=bitfieldGet(hPtr[0],27,3);
    h263->eBit=bitfieldGet(hPtr[0],24,3);
    h263->src=bitfieldGet(hPtr[0],21,3);
    h263->i=bitfieldGet(hPtr[0],15,1);
    h263->a=bitfieldGet(hPtr[0],14,1);
    h263->s=bitfieldGet(hPtr[0],13,1);

    if (h263->f)
    {
        int aDwords=h263->p+1;

        ConvertFromNetwork(hPtr, 1, aDwords);

        p->sByte+=4*aDwords;
        h263->mbaP=bitfieldGet(hPtr[0],0,8);
        h263->gobN=bitfieldGet(hPtr[0],8,5);
        h263->quant=bitfieldGet(hPtr[0],16,5);


        h263->vMv2=bitfieldGet(hPtr[1],0,8);
        h263->hMv2=bitfieldGet(hPtr[1],8,8);
        h263->vMv1=bitfieldGet(hPtr[1],16,8);
        h263->hMv1=bitfieldGet(hPtr[1],24,8);
        hPtr=(RvUint32*)((RvUint8*)buf+p->sByte-4);
    }

    if (h263->p)
    {

        h263->dbq=bitfieldGet(hPtr[0],11,2);
        h263->trb=bitfieldGet(hPtr[0],8,3);
        h263->tr=bitfieldGet(hPtr[0],0,8);
    }

    return RV_OK;
}

RvInt32 RVCALLCONV rtpH263GetHeaderLength()
{
    return rtpGetHeaderLength()+12;
}

RvInt32 RVCALLCONV rtpH263aPack(
                              IN    void*buf,
                              IN    RvInt32 len,
                              IN    rtpParam*p,
                              IN    void*param)
{
    H263aparam*h263a=(H263aparam*)param;
    RvUint32*hPtr=NULL;
    int dwords=0;

    RV_UNUSED_ARG(len);

    if (h263a->p)
    {
        dwords=1;
        p->sByte-=4;
        hPtr=(RvUint32*)((RvUint8*)buf+p->sByte);

        hPtr[0]=bitfieldSet(0,h263a->dbq,11,2);
        hPtr[0]=bitfieldSet(hPtr[0],h263a->trb,8,3);
        hPtr[0]=bitfieldSet(hPtr[0],h263a->tr,0,8);
    }

    if (h263a->f)
    {
        dwords+=2;
        p->sByte-=8;
        hPtr=(RvUint32*)((RvUint8*)buf+p->sByte);
        hPtr[0]=bitfieldSet(0,h263a->mbaP,2,9);
        hPtr[0]=bitfieldSet(hPtr[0],h263a->gobN,11,5);
        hPtr[0]=bitfieldSet(hPtr[0],h263a->quant,16,5);


        hPtr[1]=bitfieldSet(      0,h263a->vMv2, 0,7);
        hPtr[1]=bitfieldSet(hPtr[1],h263a->hMv2, 7,7);
        hPtr[1]=bitfieldSet(hPtr[1],h263a->vMv1,14,7);
        hPtr[1]=bitfieldSet(hPtr[1],h263a->hMv1,21,7);

        hPtr[1]=bitfieldSet(hPtr[1],h263a->a,28,1);
        hPtr[1]=bitfieldSet(hPtr[1],h263a->s,29,1);
        hPtr[1]=bitfieldSet(hPtr[1],h263a->u,30,1);
        hPtr[1]=bitfieldSet(hPtr[1],h263a->i,31,1);
    }

    if (!h263a->f && !h263a->p)
    {
        dwords=1;
        p->sByte-=4;
        hPtr=(RvUint32*)((RvUint8*)buf+p->sByte);
        hPtr[0]=bitfieldSet(      0,h263a->a,17,1);
        hPtr[0]=bitfieldSet(hPtr[0],h263a->s,18,1);
        hPtr[0]=bitfieldSet(hPtr[0],h263a->u,19,1);
        hPtr[0]=bitfieldSet(hPtr[0],h263a->i,20,1); ;
    }

    hPtr[0]=bitfieldSet(hPtr[0],h263a->f,31,1);
    hPtr[0]=bitfieldSet(hPtr[0],h263a->p,30,1);
    hPtr[0]=bitfieldSet(hPtr[0],h263a->sBit,27,3);
    hPtr[0]=bitfieldSet(hPtr[0],h263a->eBit,24,3);
    hPtr[0]=bitfieldSet(hPtr[0],h263a->src,21,3);

    p->payload=H263;

    ConvertToNetwork(hPtr, 0, dwords);

    return RV_OK;
}

RvInt32 RVCALLCONV rtpH263aUnpack(
                                OUT void*buf,
                                IN  RvInt32 len,
                                OUT rtpParam*p,
                                OUT void*param)
{
    H263aparam*h263a=(H263aparam*)param;
    RvUint32*hPtr=(RvUint32*)((RvUint8*)buf+p->sByte);

    RV_UNUSED_ARG(len);

    p->sByte+=4;

    ConvertFromNetwork(hPtr, 0, 1);

    h263a->f=bitfieldGet(hPtr[0],31,1);
    h263a->p=bitfieldGet(hPtr[0],30,1);

    h263a->sBit=bitfieldGet(hPtr[0],27,3);
    h263a->eBit=bitfieldGet(hPtr[0],24,3);
    h263a->src=bitfieldGet(hPtr[0],21,3);

    if (h263a->f)
    {
        int aDwords=h263a->p+1;
        ConvertFromNetwork(hPtr, 1, aDwords);

        p->sByte+=4*aDwords;
        h263a->mbaP=bitfieldGet(hPtr[0],2,9);
        h263a->gobN=bitfieldGet(hPtr[0],11,5);
        h263a->quant=bitfieldGet(hPtr[0],16,5);


        h263a->vMv2=bitfieldGet(hPtr[1],0,7);
        h263a->hMv2=bitfieldGet(hPtr[1],7,7);
        h263a->vMv1=bitfieldGet(hPtr[1],14,7);
        h263a->hMv1=bitfieldGet(hPtr[1],21,7);

        h263a->i=bitfieldGet(hPtr[1],31,1);
        h263a->u=bitfieldGet(hPtr[1],30,1);
        h263a->s=bitfieldGet(hPtr[1],29,1);
        h263a->a=bitfieldGet(hPtr[1],28,1);

        hPtr=(RvUint32*)((RvUint8*)buf+p->sByte-4);
    }

    if (h263a->p)
    {

        h263a->dbq=bitfieldGet(hPtr[0],11,2);
        h263a->trb=bitfieldGet(hPtr[0],8,3);
        h263a->tr=bitfieldGet(hPtr[0],0,8);
    }

    if (!h263a->f && !h263a->p)
    {
      h263a->i=bitfieldGet(hPtr[0],20,1);
      h263a->u=bitfieldGet(hPtr[0],19,1);
      h263a->s=bitfieldGet(hPtr[0],18,1);
      h263a->a=bitfieldGet(hPtr[0],17,1);

    }

    return RV_OK;
}

RvInt32 RVCALLCONV rtpH263aGetHeaderLength()
{
    return rtpGetHeaderLength()+12;
}




/* ==== DTMF inBand (via RTP payload) - RFC 2833 ==== */

/************************************************************************
 * rtpDtmfEventPack
 *
 * purpose: set the payload format, for sending DTMF events inband,
 *          as described in RFC2833, section 3.5.
 *
 * input  : buf - buffer pointer that will be sent
 *          len - length of the buffer.
 *          p - RTP header default parameters.
 *          param - a structure containing the required parameters for DTMF events.
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpDtmfEventPack(
        IN  void *      buf,
        IN  RvInt32     len,
        IN  rtpParam *  p,
        IN  void     *  param)
{
    rtpDtmfEventParams *dtmf = (rtpDtmfEventParams *)param;
    RvUint8*hPtr;

    RV_UNUSED_ARG(len);

    p->sByte -= 4;
    hPtr = (RvUint8*)buf + p->sByte;

    /* Due to some issues with the ordering of bytes that wasn't easy to fix using
       "conventional" means we deal with this as a bytes buffer... */
    /*                0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7
       Original:     |     event     |E|R| volume    |          duration             |
    */
    hPtr[0] = (RvUint8)(dtmf->event);
    hPtr[1] = (RvUint8)((RvUint8)((dtmf->end & 1) << 7) | (RvUint8)(dtmf->volume & 0x3f));
    hPtr[2] = (RvUint8)((dtmf->duration >> 8) & 0xff);
    hPtr[3] = (RvUint8)(dtmf->duration & 0xff);

    return RV_OK;
}


/************************************************************************
 * rtpDtmfEventUnpack
 *
 * purpose: evaluates the DTMF events from the received packed.
 *
 * input  : len - length of the buffer.
 * output : buf - the received buffer.
 *          p - RTP header parameters that were received.
 *          param - the received parameters for DTMF events.
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpDtmfEventUnpack(
        OUT  void *      buf,
        IN   RvInt32     len,
        OUT  rtpParam *  p,
        OUT  void *      param)
{
    rtpDtmfEventParams *dtmf = (rtpDtmfEventParams *)param;
    RvUint8*hPtr = (RvUint8*)buf + p->sByte;

    RV_UNUSED_ARG(len);

    p->sByte += 4;

    /* See Pack() function for explanation on ordering */
    dtmf->event = (rtpDtmfEvent)hPtr[0];
    dtmf->end = (RvBool)(hPtr[1] >> 7);
    dtmf->volume = (RvUint8)(hPtr[1] & 0x3f);
    dtmf->duration = ((RvUint32)hPtr[2] << 8) | (RvUint32)hPtr[3];

    return RV_OK;
}


/******************************************************************************
 * rtpDtmfEventGetHeaderLength
 * ----------------------------------------------------------------------------
 * General: Returns the length of a DTMF event payload.
 *          This length should be placed as the len parameter to rtpWrite().
 *
 * Return Value: Length of a DTMF event payload.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  none.
 * Output: none.
 *****************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpDtmfEventGetHeaderLength(void)
{
    return rtpGetHeaderLength() + 4;
}


/* Telephony Tones according to RFC2833 */

/************************************************************************
 * rtpDtmfTonesPack
 *
 * purpose: set the payload format, for sending telephony tones inband,
 *          as described in RFC2833, section 4.4.
 *
 * input  : buf - buffer pointer that will be sent
 *          len - length of the buffer.
 *          p - RTP header default parameters.
 *          param - a structure containing the required parameters for telephony tones.
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 *
 * Important: one of the members of the 'rtpDtmfTonesParams' struct(param) is a pointer
 * notes      to an integer array, that symbolizes the frequencies that form the tone.
 *            the array is not limited in size, since a single tone can contain any
 *            number of frequencies. this is the reason there is a member 'freqListLength'
 *            in the struct as well.
 ************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpDtmfTonesPack(
        IN  void *      buf,
        IN  RvInt32     len,
        IN  rtpParam *  p,
        IN  void     *  param)
{
    rtpDtmfTonesParams *dtmf = (rtpDtmfTonesParams *)param;
    RvUint8* hPtr;
    RvUint32 listLen = 0;

    RV_UNUSED_ARG(len);

    /* We place the bytes in "wrong order" - the ConvertToNetwork function
       is going to change the bytes position anyway... */
    /*                0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7
       Original:     |    modulation   |T|  volume   |          duration             |
    */

    p->sByte -= (4 + (((dtmf->freqListLength + 1) / 2) * 4));
    hPtr = (RvUint8*)buf + p->sByte;

    hPtr[0] = (RvUint8)((dtmf->modulation >> 1) & 0xff);
    hPtr[1] = (RvUint8)(((dtmf->modulation & 1) << 7) |
                        ((dtmf->T & 1)<< 6) |
                        (dtmf->volume & 0x3f));
    hPtr[2] = (RvUint8)((dtmf->duration >> 8) & 0xff);
    hPtr[3] = (RvUint8)(dtmf->duration & 0xff);

    hPtr += 4;

    while (listLen < dtmf->freqListLength)
    {
        /*                0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7
           Original:     |R R R R|       frequency       |R R R R|       frequency       |
        */
        hPtr[0] = (RvUint8)((dtmf->freqList[listLen] >> 8) & 0x0f);
        hPtr[1] = (RvUint8)(dtmf->freqList[listLen] & 0xff);
        hPtr += 2;
        listLen++;
    }

    return RV_OK;
}


/************************************************************************
 * rtpDtmfTonesUnpack
 *
 * purpose: evaluates the telephony tones from the received packed.
 *
 * input  : len - length of the buffer.
 * output : buf - the received buffer.
 *          p - RTP header parameters that were received.
 *          param - the received parameters for telephony tones.
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpDtmfTonesUnpack(
        IN   void *      buf,
        IN   RvInt32     len,
        OUT  rtpParam *  p,
        OUT  void *      param)
{
    rtpDtmfTonesParams *dtmf = (rtpDtmfTonesParams *)param;
    RvUint8*hPtr = (RvUint8*)buf + p->sByte;

    RV_UNUSED_ARG(len);

    /* set the offset right after the tone's first header,
       and before the frequencies list */
    p->sByte += 4;

    /* See Pack() function for explanation on ordering */
    dtmf->modulation = (RvUint16)((hPtr[1] >> 7) | ((RvUint16)hPtr[0] << 1));
    dtmf->T = (RvBool)((hPtr[2] & 0x40) != 0);
    dtmf->volume = (RvUint8)(hPtr[1] & 0x3f);
    dtmf->duration = ((RvUint32)hPtr[2] << 8) | (RvUint32)hPtr[3];

    p->len -= 16; /* removing RTP header and 4 first bytes of telephony tones header */
    p->len >>= 1; /* find out of many frequencies are there in the payload - divide by 2 */
    dtmf->freqListLength = p->len;

    return RV_OK;
}


/************************************************************************
 * rtpDtmfTonesGetByIndex
 *
 * purpose: find the requested frequency in the received message.
 *
 * input  : buf - the received buffer.
 *          index - index of the frequency inside the frequency list.
 *          p - RTP header parameters that were received.
 *          param - the received parameters for telephony tones.
 * output : frequency - The requested frequency
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
RvStatus RVCALLCONV rtpDtmfTonesGetByIndex(
        IN   void *      buf,
        IN   RvUint32    index,
        IN   rtpParam *  p,
        IN   void *      param,
        OUT  RvUint16 *  frequency)
{
    rtpDtmfTonesParams *dtmf = (rtpDtmfTonesParams *)param;
    RvUint8 *hPtr = (RvUint8*)buf + p->sByte;
    RvUint32 level;

    if (index > dtmf->freqListLength)
        return RV_ERROR_OUTOFRANGE;

    level = index << 1;

    /* Get the right frequency from inside the header */
    *frequency = (RvUint16)(((((RvUint16)hPtr[level]) << 8) | hPtr[level+1]) & 0x0fff);

    return RV_OK;
}



/* ==== Annex Q & H.281 - Far End Camera Control (FECC) ==== */

/************************************************************************
 * rtpAnnexQMessagePack
 *
 * purpose: Set the payload format, for sending Far end camera commands inband,
 *          as described in H.281.
 *          Several AnnexQ commands can be sent in the same packet. Using
 *          rtpAnnexQMessagePack on the same buffer for several such messages
 *          places them in reverse order.
 *
 * input  : buf - buffer pointer that will be sent
 *          len - length of the buffer.
 *          p - RTP header default parameters.
 *          param - a structure containing the required parameters for FECC.
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpAnnexQMessagePack(
        IN  void *      buf,
        IN  RvInt32     len,
        IN  rtpParam *  p,
        IN  void     *  params)
{
    rtpAnnexQParam *param = (rtpAnnexQParam *)params;
    RvUint8*hPtr;
    RvInt32 length = 2;

    RV_UNUSED_ARG(len);

    p->sByte-=2;
    if (param->procedure == rtpAnnexQProceduresStartAction)
        p->sByte-=1;

    hPtr=(RvUint8*)((RvUint8*)buf+p->sByte);
    hPtr[0]=(RvUint8)(param->procedure);

    switch (param->procedure)
    {
    case rtpAnnexQProceduresStartAction:
        length++;
        hPtr[2]=(RvUint8)(bitfieldSet(0, param->timeOut, 0, 4));    /* set timeout */
        /* No break - we continue as if it's a continue or a stop action */

    case rtpAnnexQProceduresContinueAction:
    case rtpAnnexQProceduresStopAction:
        hPtr[1]=(RvUint8)bitfieldSet(0, param->action.pan, 6, 2);        /* set pan   */
        hPtr[1]=(RvUint8)bitfieldSet(hPtr[1], param->action.tilt, 4, 2); /* set tilt  */
        hPtr[1]=(RvUint8)bitfieldSet(hPtr[1], param->action.zoom, 2, 2); /* set zoom  */
        hPtr[1]=(RvUint8)bitfieldSet(hPtr[1], param->action.focus, 0, 2);/* set focus */
        break;

    case rtpAnnexQProceduresSelectVideoSource:
    case rtpAnnexQProceduresVideoSourceSwitched:
        hPtr[1]=(RvUint8)bitfieldSet(0, param->videoSource, 4, 4);    /* set video source */
        hPtr[1]=(RvUint8)bitfieldSet(hPtr[1], param->mode.stillImage, 1, 1); /* set M1 */
        hPtr[1]=(RvUint8)bitfieldSet(hPtr[1], param->mode.doubleResolutionStillImage, 0, 1); /* set M0 */
        break;

    case rtpAnnexQProceduresStoreAsPreset:
    case rtpAnnexQProceduresActivatePreset:
        hPtr[1]=(RvUint8)bitfieldSet(0, param->preset, 4, 4);     /* set preset */
    }

    /*ConvertToNetwork(hPtr, 0, 1); Already in network format */
    return RV_OK;
}


/************************************************************************
 * rtpAnnexQMessageUnpack
 *
 * purpose: evaluates the FECC commands from the received packed.
 *
 * input  : len - length of the buffer.
 * output : buf - the received buffer.
 *          p - RTP header parameters that were received.
 *          param - the received parameters for FECC.
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
RvInt32 RVCALLCONV rtpAnnexQMessageUnpack(
        OUT  void *      buf,
        IN   RvInt32     len,
        OUT  rtpParam *  p,
        OUT  void *      params)
{
    rtpAnnexQParam *param = (rtpAnnexQParam *)params;
    RvUint8*hPtr=(RvUint8*)((RvUint8*)buf+p->sByte);

    RV_UNUSED_ARG(len);

    p->sByte+=2;
    /* ConvertFromNetwork(hPtr, 0, 1); Read directly from network format */

    param->procedure = (rtpAnnexQProcedures)hPtr[0];

    switch (param->procedure)
    {
    case rtpAnnexQProceduresStartAction:
        param->timeOut = (RvUint8)bitfieldGet(hPtr[2],0,4);  /* get timeout */
        p->sByte++;
        /* No break - we continue as if it's a continue or a stop action */

    case rtpAnnexQProceduresContinueAction:
    case rtpAnnexQProceduresStopAction:
        param->action.pan= (rtpAnnexQMoveCamera)bitfieldGet(hPtr[1],6,2);    /* get pan */
        param->action.tilt= (rtpAnnexQMoveCamera)bitfieldGet(hPtr[1],4,2);   /* get tilt */
        param->action.zoom= (rtpAnnexQMoveCamera)bitfieldGet(hPtr[1],2,2);   /* get zoom */
        param->action.focus= (rtpAnnexQMoveCamera)bitfieldGet(hPtr[1],0,2);  /* get focus */
        break;

    case rtpAnnexQProceduresSelectVideoSource:
    case rtpAnnexQProceduresVideoSourceSwitched:
        param->videoSource = (RvUint8)bitfieldGet(hPtr[1],4,4);  /* get video source */
        param->mode.stillImage = bitfieldGet(hPtr[1],1,1);  /* get M1 */
        param->mode.doubleResolutionStillImage = bitfieldGet(hPtr[1],0,1);  /* get M0 */
        break;

    case rtpAnnexQProceduresStoreAsPreset:
    case rtpAnnexQProceduresActivatePreset:
        param->preset = (RvUint8)bitfieldGet(hPtr[1],4,4);   /* get preset */
        break;
    }

    return RV_OK;
}

