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

#include "cmictrl.h"
#include "cmConf.h"
#include "caputils.h"
#include "strutils.h"
#include "cmchan.h"
#include "h245.h"
#include "cmCtrlMMode.h"
#include "cmH245Object.h"

#ifdef __cplusplus
extern "C" {
#endif





/* multi-point mode command on/off____________________________________________________________ */

int multipointModeCommand(H245Control* ctrl,int lcn)
{
    HCALL hsCall;
    int nesting;
    H245Object *app;

    hsCall = (EMAElement)cmiGetByControl((HCONTROL)ctrl);
    app = (H245Object *)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

    if (app == NULL)
        return RV_ERROR_UNKNOWN;

    RV_UNUSED_ARG(lcn);

    ctrl->multiPointMode = RV_TRUE;

    if (app->cmMySessionEvent.cmEvCallMiscStatus != NULL)
    {
        HAPPCALL haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall);
        H245CBEnter((app->pAppObject, "cmEvCallMiscStatus(haCall=%p,hsCall=%p,status=mpModeOn)", haCall, hsCall));
        nesting = emaPrepareForCallback((EMAElement)hsCall);
        app->cmMySessionEvent.cmEvCallMiscStatus(haCall, hsCall, cmMiscMpOn);
        emaReturnFromCallback((EMAElement)hsCall, nesting);
        H245CBExit((app->pAppObject, "cmEvCallMiscStatus"));
    }

    return RV_TRUE;
}

int cancelMultipointModeCommand(H245Control* ctrl,int lcn)
{
    HCALL hsCall;
    int nesting;
    H245Object *app;

    hsCall = (EMAElement)cmiGetByControl((HCONTROL)ctrl);
    app = (H245Object *)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

    if (app == NULL)
        return RV_ERROR_UNKNOWN;

    RV_UNUSED_ARG(lcn);

    ctrl->multiPointMode = RV_FALSE;

    if (app->cmMySessionEvent.cmEvCallMiscStatus != NULL)
    {
        HAPPCALL haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall);
        H245CBEnter((app->pAppObject, "cmEvCallMiscStatus(haCall=%p,hsCall=%p,status=mpModeOff)", haCall, hsCall));
        nesting = emaPrepareForCallback((EMAElement)hsCall);
        app->cmMySessionEvent.cmEvCallMiscStatus(haCall, hsCall, cmMiscMpOff);
        emaReturnFromCallback((EMAElement)hsCall, nesting);
        H245CBExit((app->pAppObject, "cmEvCallMiscStatus"));
    }

    return RV_TRUE;
}



RVAPI int RVCALLCONV
cmCallMultipointCommand(
          /* Send multipoint command (on or off) message */
          IN  HCALL hsCall,
          IN  RvBool isModeOn /* RV_TRUE: mp mode (on), RV_FALSE: cancel mp mode (off) */
          )
{
    RvPvtNodeId nodeId, message;
    int res = 0;

    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        HCONTROL ctrl=cmiGetControl(hsCall);
        H245Object *app = (H245Object*)hApp;
        HPVT hVal = app->hVal;

        H245APIEnter((app->pAppObject, "cmCallMultipointCommand: hsCall=%p, %s.", hsCall, (isModeOn)?"on":"off"));

        message = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);
        nodeId = pvtAddBranch2(hVal, message, __h245(command), __h245(miscellaneousCommand));
        pvtAdd(hVal, nodeId, __h245(logicalChannelNumber), 10000, NULL, NULL);
        pvtAddBranch2(hVal, nodeId, __h245(type),
            (isModeOn)?__h245(multipointModeCommand):__h245(cancelMultipointModeCommand));
        res = sendMessageH245(ctrl, message, RV_TRUE, NULL);

        H245APIExit((app->pAppObject, "cmCallMultipointCommand=%d", res));
        emaUnlock((EMAElement)cmiGetByControl((HCONTROL)ctrl));
    }

    return res;
}



RVAPI int RVCALLCONV
cmCallMultipointStatus(
          /* Get the multipoint mode status (on or off) */
          IN  HCALL hsCall,
          OUT RvBool *isModeOn /* RV_TRUE: mp mode (on), RV_FALSE: cancel mp mode (off) */
          )
{
    if ((hsCall == NULL) || (isModeOn == NULL))
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        H245Object *app = (H245Object*)hApp;
        H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);

		RV_UNUSED_ARG(app) /* used only when the log is on */
        H245APIEnter((app->pAppObject, "cmCallMultipointStatus: hsCall=%p, &isModeOn=%p", hsCall, isModeOn));

        *isModeOn = ctrl->multiPointMode;

        H245APIExit((app->pAppObject, "cmCallMultipointStatus=%s (%d)", (ctrl->multiPointMode)?"on":"off", ctrl->multiPointMode));
        emaUnlock((EMAElement)cmiGetByControl((HCONTROL)ctrl));
    }
    return RV_TRUE;
}

#ifdef __cplusplus
}
#endif
