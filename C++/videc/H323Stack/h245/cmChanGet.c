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

#include "cmH245.h"
#include "cmChanGet.h"

#ifdef __cplusplus
extern "C" {
#endif


/* find an incoming channel with LCN == lcn */
H245Channel* getInChanByLCN(HCONTROL ctrl, int lcn)
{
    /* get the pointer to the first channel from EMA call "struct" */
    H245Channel * channel=(H245Channel*)cmH245GetChannelList(ctrl);
    /* go through the channel list to the end (NULL) */
    while(channel)
    {
        /* match LCN for incoming channels */
        if ((channel->bOrigin == RV_FALSE) && ((int)channel->myLogicalChannelNum == lcn))
        {
            /* return match */
            return channel;
        }

        /* We would like to check bidirectional channels as well */
        if (channel->bIsDuplex && (channel->bOrigin == RV_TRUE) &&
            (channel->reverseLogicalChannelNum == lcn))
        {
            /* return a match */
            return channel;
        }

        /* next channel on the list */
        channel=channel->pNext;
    }
    /* not found */
    return NULL;
}

/* find an outgoing channel with LCN == lcn */
H245Channel* getOutChanByLCN(HCONTROL ctrl, int lcn)
{
    /* get the pointer to the first channel from EMA call "struct" */
    H245Channel * channel=(H245Channel*)cmH245GetChannelList(ctrl);
    /* go through the channel list to the end (NULL) */
    while(channel)
    {
        /* match LCN for outgoing channels */
        if ((channel->bOrigin == RV_TRUE) && ((int)channel->myLogicalChannelNum == lcn))
        {
            /* return match */
            return channel;
        }

        /* We would like to check bidirectional channels as well */
        if (channel->bIsDuplex && (channel->bOrigin == RV_FALSE) &&
            (channel->reverseLogicalChannelNum == lcn))
        {
            /* return a match */
            return channel;
        }

        /* next channel on the list */
        channel=channel->pNext;
    }
    /* not found */
    return NULL;
}

/* find an outgoing channel with LCN == lcn,  or reverseLCN == lcn for incoming ones */
H245Channel* getOutSubChanByLCN(HCONTROL ctrl, int lcn)
{
    /* get the pointer to the first channel from EMA call "struct" */
    H245Channel * channel=(H245Channel*)cmH245GetChannelList(ctrl);
    /* go through the channel list to the end (NULL) */
    while(channel)
    {
        /* match LCN for outgoing channels and reverse LCN for incoming ones */
        if ((channel->bOrigin && (int)channel->myLogicalChannelNum==lcn) ||
            (!channel->bOrigin && (int)channel->reverseLogicalChannelNum==lcn))
            /* return match */
            return channel;
        /* next channel on the list */
        channel=channel->pNext;
    }
    /* not found */
    return NULL;
}

#if 0
/* find an incoming channel with LCN == lcn,  or reverseLCN == lcn for outgoing ones */
H245Channel* getInSubChanByLCN(HCONTROL ctrl, int lcn)
{
    /* get the pointer to the first channel from EMA call "struct" */
    H245Channel * channel=(H245Channel*)cmH245GetChannelList(ctrl);
    /* go through the channel list to the end (NULL) */
    while(channel)
    {
        /* match LCN for incoming channels and reverse LCN for outgoing ones */
        if ((!channel->bOrigin && (int)channel->myLogicalChannelNum==lcn) ||
            (channel->bOrigin && (int)channel->reverseLogicalChannelNum==lcn))
            /* return match */
            return channel;
        /* next channel on the list */
        channel=channel->pNext;
    }
    /* not found */
    return NULL;
}
#endif

/* find an outgoing channel whose base is channel, starting from currentChannel */
H245Channel* getNextOutChanByBase(HCONTROL ctrl,H245Channel* channel,void** currentChannel)
{
    H245Channel * itr_channel;

    if (*currentChannel)
        /* continue from last position */
        itr_channel=((H245Channel*)*currentChannel)->pNext;
    else
        /* get the pointer to the first channel from EMA call "struct" */
        itr_channel=(H245Channel *)cmH245GetChannelList(ctrl);

    /* go through the channel list to the end (NULL) */
    while (itr_channel)
    {
        /* find an outgoing channel whose base is this channel */
        if (itr_channel->bOrigin && (itr_channel->pBase==channel))
        {
            /* save position */
            *currentChannel=(void*)itr_channel;
            /* return match */
            return itr_channel;
        }
        /* next channel on the list */
        itr_channel=itr_channel->pNext;
    }
    /* not found */
    return NULL;
}

/* get next outgoing channel, starting from currentChannel */
H245Channel* getNextOutChan(HCONTROL ctrl, void** currentChannel)
{
    H245Channel * itr_channel;

    if (*currentChannel)
        /* continue from last position */
        itr_channel=((H245Channel*)*currentChannel)->pNext;
    else
        /* get the pointer to the first channel from EMA call "struct" */
        itr_channel=(H245Channel *)cmH245GetChannelList(ctrl);

    /* go through the channel list to the end (NULL) */
    while (itr_channel)
    {
        /* get the next outgoing channel */
        if (itr_channel->bOrigin)
        {
            /* save position */
            *currentChannel=(void*)itr_channel;
            /* return match */
            return itr_channel;
        }
        /* next channel on the list */
        itr_channel=itr_channel->pNext;
    }
    /* not found */
    return NULL;
}

/* get next incoming channel, starting from currentChannel */
H245Channel* getNextInChan(HCONTROL ctrl, void** currentChannel)
{
    H245Channel * itr_channel;

    if (*currentChannel)
        /* continue from last position */
        itr_channel=((H245Channel*)*currentChannel)->pNext;
    else
        /* get the pointer to the first channel from EMA call "struct" */
        itr_channel=(H245Channel *)cmH245GetChannelList(ctrl);

    /* go through the channel list to the end (NULL) */
    while (itr_channel)
    {
        /* get the next incoming channel */
        if (!itr_channel->bOrigin)
        {
            /* save position */
            *currentChannel=(void*)itr_channel;
            /* return match */
            return itr_channel;
        }
        /* next channel on the list */
        itr_channel=itr_channel->pNext;
    }
    /* not found */
    return NULL;
}

/************************************************************************
 * getNextChan
 * purpose: Get the next channel for a given control object.
 *          This function can be used to perform a single task on all
 *          the channels.
 * input  : ctrl            - Control object
 *          currentChannel  - Current channel we have.
 *                            If the contents of this pointer is NULL, then the
 *                            first channel will be returned
 * output : currentChannel  - Next channel in list
 * return : Next channel in list on success
 *          NULL when there are no more channels
 ************************************************************************/
H245Channel* getNextChan(IN HCONTROL ctrl, INOUT void** currentChannel)
{
    H245Channel * itr_channel;

    if (*currentChannel)
        /* continue from last position */
        itr_channel=((H245Channel*)*currentChannel)->pNext;
    else
        /* get the pointer to the first channel from EMA call "struct" */
        itr_channel=(H245Channel *)cmH245GetChannelList(ctrl);

    /* if there is another channel in the list */
    if(itr_channel)
    {
        /* save position */
        *currentChannel=(void*)itr_channel;
        /* return it */
        return itr_channel;
    }
    return NULL;
}


#if (RV_H245_SUPPORT_H223_PARAMS == 1)
/************************************************************************
 * getInChanByDataType
 * purpose: Get incoming channel for a given DataType.
 *          
 * input  : ctrl            - Control object
 *          dataType  - required data type
 * output : None
 * return : The required channel if exist
 *          NULL when there is no such channel
 ************************************************************************/
H245Channel* getInChanByDataType(IN HCONTROL ctrl, IN cmChannelDataType dataType)
{
    /* get the pointer to the first channel from EMA call "struct" */
    H245Channel *channel = (H245Channel*)cmH245GetChannelList(ctrl);

    /*get first in channel*/
    if (channel->bOrigin) /*if its an outgoing channel get the first in*/
        channel = getNextInChan(ctrl, (void**)&channel);
   
    /* go through the channel list to the end (NULL) */
    while (channel)
    {
        /* match DataType for incoming channels */
        if ((channel->bOrigin == RV_FALSE) && 
            (dataType == channel->data.h223Params.eDataType))
        {
            /* return match */
            return channel;
        }
        channel = getNextInChan(ctrl, (void**)&channel);
    }

    /* not found */
    return NULL;
}

/************************************************************************
 * getOutChanByDataType
 * purpose: Get outgoing channel for a given DataType.
 *          
 * input  : ctrl            - Control object
 *          dataType  - DataType of channel
 * output : None
 * return : The required channel if exist
 *          NULL when there is no such channel
 ************************************************************************/
H245Channel* getOutChanByDataType(IN HCONTROL ctrl, IN cmChannelDataType dataType)
{
     /* get the pointer to the first channel from EMA call "struct" */
    H245Channel *channel = (H245Channel*)cmH245GetChannelList(ctrl);
   
    if (!(channel->bOrigin)) /*if its an incoming channel get the first out*/
        channel = getNextOutChan(ctrl, (void**)&channel);
   
    /* go through the channel list to the end (NULL) */
    while (channel)
    {
        /* match DataType for incoming channels */
        if ((channel->bOrigin == RV_TRUE) && 
            (dataType == channel->data.h223Params.eDataType))
        {
            /* return match */
            return channel;
        }
        channel = getNextOutChan(ctrl, (void**)&channel);
    }

    /* not found */
    return NULL;
}
#endif

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
/************************************************************************
 *
 *                Functions that deal with session IDs
 *
 ************************************************************************/


/* find an outgoing channel with SID == sid */
H245Channel* getOutChanBySID(HCONTROL ctrl, RvUint8 sid)
{
    /* get the pointer to the first channel from EMA call "struct" */
    H245Channel * channel=(H245Channel*)cmH245GetChannelList(ctrl);
    /* go through the channel list to the end (NULL) */
    while(channel)
    {
        /* match SID for outgoing channels */
        if (channel->bOrigin && channel->data.h225Params.sessionId==sid &&
            channel->eState != ChannelStateReleased && channel->eState != ChannelStateAwaitingRelease)
        {
            /* return match */
            return channel;
        }
        /* next channel on the list */
        channel=channel->pNext;
    }
    /* not found */
    return NULL;
}

/* find an incoming channel with SID == sid */
H245Channel* getInChanBySID(HCONTROL ctrl, RvUint8 sid)
{
    /* get the pointer to the first channel from EMA call "struct" */
    H245Channel * channel=(H245Channel*)cmH245GetChannelList(ctrl);
    /* go through the channel list to the end (NULL) */
    while(channel)
    {
        /* match SID for incoming channels */
        if (!channel->bOrigin && channel->data.h225Params.sessionId==sid)
            /* return match */
            return channel;
        /* next channel on the list */
        channel=channel->pNext;
    }
    /* not found */
    return NULL;
}

/* Tests that there is no channel in the same direction with the same non-zero SID */
RvBool checkChanSIDConsistency(
    IN HCONTROL         ctrl,
    IN H245Channel*     test_channel,
    IN RvUint8          sessionId,
    IN RvBool           isOrigin,
    IN RvBool           isDuplex)
{
    /* get the pointer to the first channel from EMA call "struct" */
    H245Channel * channel = (H245Channel *)cmH245GetChannelList(ctrl);

    if (sessionId == 0)
        return RV_FALSE;

    /* go through the channel list to the end (NULL) */
    while (channel)
    {
        /* see if there is a different channel of the same direction with the same SID */
        if ((channel != test_channel) &&
            ((channel->bOrigin == isOrigin) || channel->bIsDuplex || isDuplex) &&
            (channel->data.h225Params.sessionId == sessionId) &&
            (channel->eState != ChannelStateReleased))
            return RV_TRUE;
        /* next channel on the list */
        channel = channel->pNext;
    }
    /* none found */
    return RV_FALSE;
}

RvUint8 getFreeSID(HCONTROL ctrl, RvBool isOrigin, RvBool isDuplex)
{
    H245Control* ctrlE = (H245Control*)ctrl;
    RvBool found = RV_FALSE;
    RvInt cnt = 224;

    /* We need to loop until we find a vacant session id... */
    while (!found)
    {
        ctrlE->nextFreeSID++; /* This wraps around (8-bit) */
        if (ctrlE->nextFreeSID < 0x20)
            ctrlE->nextFreeSID = 0x20;
        found = !checkChanSIDConsistency(ctrl, NULL, ctrlE->nextFreeSID, isOrigin, isDuplex);
        cnt--;
        if (cnt == 0)
            return 0; /* Make sure we don't loop... */
    }
    return ctrlE->nextFreeSID;
}

#endif /* (RV_H245_SUPPORT_H225_PARAMS == 1) */

#ifdef __cplusplus
}
#endif
