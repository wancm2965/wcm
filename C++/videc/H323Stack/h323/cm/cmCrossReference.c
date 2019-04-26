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

#include "rvassert.h"
#include "ema.h"
#include "cmAutoRasIntr.h"
#include "cmCall.h"
#include "cmQ931.h"
#include "cmiQ931.h"
#include "cmControl.h"
#include "cmCrossReference.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************/
/*The structure of  */
/*the call element  */
/********************/
/*callElem          */
/*aRASElem          */
/*q931Elem          */
/*H245Control       */
/*NoPropertiesParams*/
/********************/


/************************************************************************
 *
 *                       Stack instance related
 *
 ************************************************************************/


/************************************************************************
 * cmiInitCalls
 * purpose: Constructs the EMA for the call elements
 * input  : hApp     - Stack's application handle
 *          maxCalls - number of calls to allocate
 * output : none
 * return : EMA-elements array pointer on success
 *          NULL on failure
 ************************************************************************/
HEMA cmiInitCalls(HAPP hApp,int maxCalls)
{
    cmElem *app = (cmElem *)hApp;
    int callStructSize = sizeof(callElem) + sizeof(q931Elem) + sizeof(H245Control);

    /* Make sure these structures are aligned - it's worthwhile to catch stupid bugs before we get
       a bus error */
    RvAssert((int)sizeof(callElem) == (int)RvAlignTo(sizeof(callElem), RV_ALIGN_PTRSIZE));
    RvAssert((int)sizeof(q931Elem) == (int)RvAlignTo(sizeof(q931Elem), RV_ALIGN_PTRSIZE));
    RvAssert((int)sizeof(H245Control) == (int)RvAlignTo(sizeof(H245Control), RV_ALIGN_PTRSIZE));
    RvAssert((int)sizeof(autorasCall) == (int)RvAlignTo(sizeof(autorasCall), RV_ALIGN_PTRSIZE));

    if (!app->manualRAS)
    {
        callStructSize += sizeof(autorasCall);
    }

    if (app->callPropertyMode == pmodeDoNotUseProperty)
    {
        callStructSize += sizeof(callNoPropertiesElem);
    }

    return emaConstruct(callStructSize, maxCalls,
        emaLockPointers, "CM Calls", H225CALL, NULL, (void*)hApp, ((cmElem*)hApp)->logMgr);
}

/************************************************************************
 * cmiEndCalls
 * purpose: Destructs the EMA of the call elements
 * input  : calls    - the EMA pointer created by cmiInitCalls()
 * output : none
 * return : none
 ************************************************************************/
void cmiEndCalls(HEMA calls)
{
    emaDestruct(calls);
}

/************************************************************************
 * cmiInitChannels
 * purpose: Constructs the EMA for the channel elements
 * input  : hApp        - Stack's application handle
 *          maxCalls    - number of calls to allocated
 *          maxChannels - number of channels to allocate per call
 * output : none
 * return : EMA-elements array pointer on success
 *          NULL on failure
 ************************************************************************/
HEMA cmiInitChannels(HAPP hApp,int maxCalls,int maxChannels)
{
    return emaConstruct(sizeof(H245Channel),
        maxCalls*maxChannels, emaLockPointers, "CM Channels",CHAN,NULL,
        (void*)hApp, ((cmElem*)hApp)->logMgr);
}

/************************************************************************
 * cmiEndChannels
 * purpose: Destructs the EMA of the channel elements
 * input  : channels    - the EMA pointer created by cmiInitCalls()
 * output : none
 * return : none
 ************************************************************************/
void cmiEndChannels(HEMA channels)
{
    emaDestruct(channels);
}


/************************************************************************
 * cmiGetWatchdogHandle
 * purpose: Get the watchdog object from the stack's instance
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Watchdog module pointer on success
 *          NULL on failure
 ************************************************************************/
RVAPI RvWatchdog* RVCALLCONV cmiGetWatchdogHandle(IN HAPP hApp)
{
    return &((cmElem*)hApp)->watchdog;
}


/************************************************************************
 * cmiGetWatchdogLogSource
 * purpose: Get the log source for watchdog printouts
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Pointer to watchdog log source on success
 *          NULL on failure
 ************************************************************************/
RvLogSource* cmiGetWatchdogLogSource(IN HAPP hApp)
{
    return &((cmElem*)hApp)->logWatchdog;
}


/************************************************************************
 * cmiGetRasHandle
 * purpose: Get the RAS module object from the stack's instance
 * input  : hApp    - Stack's application handle
 * output : none
 * return : RAS module handle on success
 *          NULL on failure
 ************************************************************************/

HRASMGR cmiGetRasHandle(IN HAPP hApp)
{
    return ((cmElem*)hApp)->rasManager;
}


/************************************************************************
 * cmiGetAutoRasHandle
 * purpose: Get the Automatic RAS module object from the stack's instance
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Automatic RAS module handle on success
 *          NULL on failure
 ************************************************************************/
HAUTORASMGR cmiGetAutoRasHandle(IN HAPP hApp)
{
    return ((cmElem*)hApp)->hAutoRas;
}


/************************************************************************
 * cmiGetRasHooks
 * purpose: Get the hook functions set by the application of the CM
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Pointer to the hook functions set by the application
 ************************************************************************/
CMPROTOCOLEVENT cmiGetRasHooks(IN HAPP hApp)
{
    return &((cmElem*)hApp)->cmMyProtocolEvent;
}




/************************************************************************
 *
 *                              Call related
 *
 ************************************************************************/

#define RV_Q931_OFFSET      (sizeof(callElem))
#define RV_RAS_OFFSET       (sizeof(callElem) + sizeof(q931Elem) + sizeof(H245Control))
#define RV_PROPS_OFFSET     (sizeof(callElem) + sizeof(q931Elem) + sizeof(H245Control) + sizeof(autorasCall))



/************************************************************************
 * cmiGetAutoRas
 * purpose: Get the Automatic RAS call object from the stack's call handle
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Automatic RAS call handle on success
 *          NULL on failure
 ************************************************************************/
HAUTORASCALL cmiGetAutoRas(IN HCALL call)
{
    return (HAUTORASCALL)((char*)(call) + RV_RAS_OFFSET);
}


/************************************************************************
 * cmiGetByAutoRas
 * purpose: Get the stack's call handle from the Automatic RAS call object
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Stack's call handle
 *          NULL on failure
 ************************************************************************/
HCALL cmiGetByAutoRas(IN HAUTORASCALL aras)
{
    return (HCALL)((char*)(aras) - RV_RAS_OFFSET);
}

/************************************************************************
 * cmiGetQ931
 * purpose: Get the Q.931 call handle from the stack's call handle
 * input  : hsCall  - Stack's call handle
 * output : none
 * return : Q.931 call handle on success
 ************************************************************************/
HQ931 cmiGetQ931(IN HCALL hsCall)
{
    return (HQ931)((char*)(hsCall) + RV_Q931_OFFSET);
}

/************************************************************************
 * cmiGetByQ931
 * purpose: Get the stack's call handle from the Q.931 call handle
 * input  : qCall   - Q.931's call handle
 * output : none
 * return : Stack call handle on success
 ************************************************************************/
HCALL cmiGetByQ931(HQ931 qCall)
{
    return (HCALL)((char*)(qCall) - RV_Q931_OFFSET);
}

/************************************************************************
 * cmiGetNoPropertiesParams
 * purpose: Get the No properties mode parameters of a call from a call
 *          handle.
 * input  : hApp    - Application's stack handle
 *          hsCall  - Stack's call handle
 * output : none
 * return : property mode struct pointer on success
 ************************************************************************/
void *cmiGetNoPropertiesParams(IN HAPP hApp, IN HCALL hsCall)
{
    cmElem *app = (cmElem *)hApp;

    if (app->manualRAS)
    {
        /* Properties come instead of the automatic RAS */
        return (void *)((char *)hsCall + RV_RAS_OFFSET);
    }

    /* Calculation should stay as is */
    return (void *)((char *)hsCall + RV_PROPS_OFFSET);
}

/************************************************************************
 * cmiGetChannelListForCtrl
 * purpose: Get the call's channel list from the control handle
 * input  : ctrl   - Call control handle
 * output : none
 * return : Handle of the first channel in the list, NULL if none
 ************************************************************************/
HCHAN cmiGetChannelListForCtrl(HCONTROL ctrl)
{
    return cmiGetChannelListForCall(cmiGetByControl(ctrl));
}

/************************************************************************
 * cmiGetChannelListForCall
 * purpose: Get the call's channel list from the stack call handle
 * input  : call   - Stack call handle
 * output : none
 * return : Handle of the first channel in the list, NULL if none
 ************************************************************************/
HCHAN cmiGetChannelListForCall(HCALL call)
{
    return ((callElem *)call)->firstChan;
}

/************************************************************************
 * cmiSetChannelListForCtrl
 * purpose: Set the call's channel list for the control handle
 * input  : ctrl   - Call control handle
 *          ch     - Handle of the first channel in the list
 * output : none
 * return : none
 ************************************************************************/
void cmiSetChannelListForCtrl(HCONTROL ctrl,HCHAN ch)
{
    cmiSetChannelListForCall(cmiGetByControl(ctrl),ch);
}

/************************************************************************
 * cmiSetChannelListForCall
 * purpose: Set the call's channel list for the stack call handle
 * input  : call   - Stack call handle
 *          ch     - Handle of the first channel in the list
 * output : none
 * return : none
 ************************************************************************/
void cmiSetChannelListForCall(HCALL call,HCHAN ch)
{
    ((callElem *)call)->firstChan = ch;
}

/************************************************************************
 * cmiGetCatForCall
 * purpose: Get the CAT call handle from the stack's call handle
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Cat call handle on success
 *          NULL on failure
 ************************************************************************/
RVHCATCALL cmiGetCatForCall(IN HCALL hsCall)
{
    return ((callElem*)hsCall)->hCatCall;
}





#ifdef __cplusplus
}
#endif
