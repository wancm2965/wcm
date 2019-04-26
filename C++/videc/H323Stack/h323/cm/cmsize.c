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

#include "rvinternal.h"
#include "cm.h"
#include "cmintr.h"
#include "rvh323watchdog.h"
#include "cmsize.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifdef RV_H323_COMPILE_WITH_DEAD_FUNCTIONS


RVAPI int RVCALLCONV cmSizeCurProtocols(HAPP hApp){ RV_UNUSED_ARG(hApp); return RV_ERROR_UNKNOWN; }
RVAPI int RVCALLCONV cmSizeMaxProtocols(HAPP hApp){ RV_UNUSED_ARG(hApp); return RV_ERROR_UNKNOWN; }
RVAPI int RVCALLCONV cmSizeCurProcs(HAPP hApp)    { RV_UNUSED_ARG(hApp); return RV_ERROR_UNKNOWN; }
RVAPI int RVCALLCONV cmSizeMaxProcs(HAPP hApp)    { RV_UNUSED_ARG(hApp); return RV_ERROR_UNKNOWN; }
RVAPI int RVCALLCONV cmSizeCurEvents(HAPP hApp)   { RV_UNUSED_ARG(hApp); return RV_ERROR_UNKNOWN; }
RVAPI int RVCALLCONV cmSizeMaxEvents(HAPP hApp)   { RV_UNUSED_ARG(hApp); return RV_ERROR_UNKNOWN; }
RVAPI int RVCALLCONV cmSizeCurUdpChans(HAPP hApp) { RV_UNUSED_ARG(hApp); return RV_ERROR_UNKNOWN; }
RVAPI int RVCALLCONV cmSizeMaxUdpChans(HAPP hApp) { RV_UNUSED_ARG(hApp); return RV_ERROR_UNKNOWN; }
RVAPI int RVCALLCONV cmSizeCurChannels(HAPP hApp) { RV_UNUSED_ARG(hApp); return RV_ERROR_UNKNOWN; }
RVAPI int RVCALLCONV cmSizeMaxChannels(HAPP hApp) { RV_UNUSED_ARG(hApp); return RV_ERROR_UNKNOWN; }
RVAPI int RVCALLCONV cmSizeCurChanDescs(HAPP hApp){ RV_UNUSED_ARG(hApp); return RV_ERROR_UNKNOWN; }
RVAPI int RVCALLCONV cmSizeMaxChanDescs(HAPP hApp){ RV_UNUSED_ARG(hApp); return RV_ERROR_UNKNOWN; }

RVAPI int RVCALLCONV cmSizeCurTimers(HAPP hApp)
{
    RvUint32 value;
    if (RvH323WatchdogGetResource(hApp, "CmTimers", RvH323WatchdogCurrent, &value, NULL) >= 0)
        return value;

    return RV_ERROR_UNKNOWN;
}

RVAPI int RVCALLCONV cmSizeMaxTimers(HAPP hApp)
{
    RvUint32 value;
    if (RvH323WatchdogGetResource(hApp, "CmTimers", RvH323WatchdogMaxVal, &value, NULL) >= 0)
        return value;

    return RV_ERROR_UNKNOWN;
}

RVAPI int RVCALLCONV cmSizeCurTpktChans(HAPP hApp)
{
    RvUint32 value;
    if (RvH323WatchdogGetResource(hApp, "TransportHosts", RvH323WatchdogCurrent, &value, NULL) >= 0)
        return value;

    return RV_ERROR_UNKNOWN;
}

RVAPI int RVCALLCONV cmSizeMaxTpktChans(HAPP hApp)
{
    RvUint32 value;
    if (RvH323WatchdogGetResource(hApp, "TransportHosts", RvH323WatchdogMaxVal, &value, NULL) >= 0)
        return value;

    return RV_ERROR_UNKNOWN;
}

RVAPI int RVCALLCONV cmSizeCurMessages(HAPP hApp)
{
    RvUint32 value;
    if (RvH323WatchdogGetResource(hApp, "MessagesPool", RvH323WatchdogCurrent, &value, NULL) >= 0)
        return value;

    return RV_ERROR_UNKNOWN;
}

RVAPI int RVCALLCONV cmSizeMaxMessages(HAPP hApp)
{
    RvUint32 value;
    if (RvH323WatchdogGetResource(hApp, "MessagesPool", RvH323WatchdogMaxVal, &value, NULL) >= 0)
        return value;

    return RV_ERROR_UNKNOWN;
}



#endif  /* RV_H323_COMPILE_WITH_DEAD_FUNCTIONS */


#ifdef __cplusplus
}
#endif

