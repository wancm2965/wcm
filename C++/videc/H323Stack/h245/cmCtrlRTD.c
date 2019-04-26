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

#include "rvtime.h"
#include "rvtimestamp.h"
#include "rvstdio.h"
#include "cmictrl.h"
#include "cmConf.h"
#include "caputils.h"
#include "strutils.h"
#include "cmchan.h"
#include "h245.h"
#include "cmCtrlMSD.h"
#include "cmH245Object.h"

#ifdef __cplusplus
extern "C" {
#endif

static RvBool rtdTimeoutEventsHandler(void*hsCall);


/* round trip delay________________________________________________________________________________ */

RVAPI int RVCALLCONV
cmCallRoundTripDelay(
             /* Measure the round trip delay to the remote terminal */
             /* Note: maxDelay=0 --> delay taken from configuration */
             IN     HCALL       hsCall,
             IN     RvInt32     maxDelay /* maximum waiting delay in seconds */
             )
{
    H245Control*   ctrl=(H245Control*)cmiGetControl(hsCall);
    H245Object     *app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    int            ret=0;
    HPVT           hVal;
    int            message;
    RoundTripInfo* rtd;

    if ((hsCall == NULL) || (app == NULL))
        return RV_ERROR_NULLPTR;

    H245APIEnter((app->pAppObject, "cmCallRoundTripDelay: hsCall=%p, delay=%d", hsCall, maxDelay));

    if (emaLock((EMAElement)hsCall))
    {
        RvBool sentOnDummyHost = RV_FALSE;

        hVal = app->hVal;

        rtd=&ctrl->roundTrip;

        if (maxDelay == 0)
        {
            maxDelay = 10;
            pvtGetChildByFieldId(hVal,app->h245Conf,__h245(roundTripTimeout),&maxDelay,NULL);
        }

        rtd->seqNum++;
        rtd->seqNum%=256;

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        __pvtBuildByFieldIds(ret,hVal,message, {_h245(request) _h245(roundTripDelayRequest)
                                                _h245(sequenceNumber) LAST_TOKEN}, rtd->seqNum, NULL);
        if (ret >= 0)
            ret = sendMessageH245((HCONTROL)ctrl, message, RV_FALSE, &sentOnDummyHost);
        pvtDelete(hVal,message);

        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &rtd->timer);

        if (ret >= 0)
        {
            if (!sentOnDummyHost)
            {
                rtd->timer = app->evHandlers.pfnTimerStartEv((HCONTROL)ctrl,
                    rtdTimeoutEventsHandler, (void*)ctrl, maxDelay*1000);
            }
            rtd->timeStart = RvInt64ToRvInt32(RvInt64Div(RvTimestampGet(app->logMgr), RV_TIME64_NSECPERMSEC));
            rtd->waiting = RV_TRUE;
        }
        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallRoundTripDelay=%d", ret));
    return ret;
}


int roundTripDelayRequest(H245Control* ctrl, int message)
{
    HH245 hApp=(HH245)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    HPVT hVal;
    int rmessage, res;
    int sqNumber;

    hVal = ((H245Object *)hApp)->hVal;

    pvtGetChildByFieldId(hVal,message, __h245(sequenceNumber),&sqNumber,NULL);
    rmessage=pvtAddRoot(hVal,((H245Object*)hApp)->hSynProtH245,0,NULL);
    __pvtBuildByFieldIds(res,hVal,rmessage, {_h245(response) _h245(roundTripDelayResponse)
                                             _h245(sequenceNumber) LAST_TOKEN}, sqNumber, NULL);
    if (res >= 0)
        res = sendMessageH245((HCONTROL)ctrl, rmessage, RV_FALSE, NULL);
    pvtDelete(hVal,rmessage);
    return res;
}

int roundTripDelayResponse(H245Control* ctrl, int message)
{
    HH245          hApp=(HH245)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));
    H245Object*    app = (H245Object *)hApp;
    HPVT           hVal;
    RoundTripInfo* rtd=&ctrl->roundTrip;
    int            sqNumber;
    RvInt32        delay;

    hVal = app->hVal;

    pvtGetChildByFieldId(hVal,message, __h245(sequenceNumber),&sqNumber,NULL);

    app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &rtd->timer);

    if ((sqNumber == rtd->seqNum) && (app->cmMySessionEvent.cmEvCallRoundTripDelay != NULL))
    {
        int nesting;

        delay = RvInt64ToRvInt32(RvInt64Div(RvTimestampGet(app->logMgr), RV_TIME64_NSECPERMSEC)) - rtd->timeStart;
        H245CBEnter((app->pAppObject, "cmEvCallRoundTripDelay: haCall=%p, hsCall=%p, delay=%d.",(HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)), cmiGetByControl((HCONTROL)ctrl), delay));
        nesting=emaPrepareForCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl));
        app->cmMySessionEvent.cmEvCallRoundTripDelay(
            (HAPPCALL)emaGetApplicationHandle((EMAElement)cmiGetByControl((HCONTROL)ctrl)),
            (HCALL)cmiGetByControl((HCONTROL)ctrl),
            delay);
        emaReturnFromCallback((EMAElement)cmiGetByControl((HCONTROL)ctrl),nesting);
        H245CBExit((app->pAppObject, "cmEvCallRoundTripDelay."));
    }
    return RV_TRUE;
}


static RvBool rtdTimeoutEventsHandler(void*_ctrl)
{
    H245Control*   ctrl=(H245Control*)_ctrl;
    HCALL          hsCall;
    RoundTripInfo* rtd=&ctrl->roundTrip;

    hsCall = cmiGetByControl((HCONTROL)ctrl);

    if (emaLock((EMAElement)hsCall))
    {
        HH245       hApp;
        H245Object* app;
        int         nesting;
        hApp = (HH245)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall)));
        app = (H245Object *)hApp;
        app->evHandlers.pfnTimerClearEv((HCONTROL)ctrl, &rtd->timer);

        if (app->cmMySessionEvent.cmEvCallRoundTripDelay != NULL)
        {
            H245CBEnter((app->pAppObject, "cmEvCallRoundTripDelay: haCall=%p, hsCall=%p, delay=%d.",(HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall), hsCall, -1));
            nesting = emaPrepareForCallback((EMAElement)hsCall);
            app->cmMySessionEvent.cmEvCallRoundTripDelay(
                (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall), hsCall, -1);
            emaReturnFromCallback((EMAElement)hsCall, nesting);
            H245CBExit((app->pAppObject, "cmEvCallRoundTripDelay."));
        }

        emaUnlock((EMAElement)hsCall);
    }
    return RV_FALSE;
}

/************************************************************************
 * rtdInit
 * purpose: Initialize the round trip delay process on a control channel
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
int rtdInit(H245Control* ctrl)
{
    RoundTripInfo* rtd=&ctrl->roundTrip;
    rtd->seqNum=0;
    rtd->timer=NULL;
    return 0;
}

/************************************************************************
 * rtdEnd
 * purpose: Stop the round trip delay process on a control channel
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
void rtdEnd(IN H245Control* ctrl)
{
    RoundTripInfo* rtd=&ctrl->roundTrip;
    H245Object* app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));

    /* Get rid of the timer if one exists */
    app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &rtd->timer);
}




#ifdef __cplusplus
}
#endif

