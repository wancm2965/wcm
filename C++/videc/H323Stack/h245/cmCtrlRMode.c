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
#include "cmictrl.h"
#include "cmConf.h"
#include "caputils.h"
#include "strutils.h"
#include "cmchan.h"
#include "h245.h"
#include "cmCtrlMSD.h"
#include "cmH245Object.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif



/* request mode________________________________________________________________________________ */


int requestMode(H245Control* ctrl, int message)
{
    H245Object          *app;
    HPVT                hVal;
    EMAElement          callElem;
    InRequestModeInfo   *in_RM = &ctrl->inRequestMode;
    RvInt32             sq = 0;
    RvPvtNodeId         nodeId;
    int                 nesting;

    callElem = (EMAElement)cmiGetByControl((HCONTROL)ctrl);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance(callElem));

    if (app->cmMySessionEvent.cmEvCallRequestMode == NULL)
        return RV_TRUE;

    hVal = app->hVal;

    pvtGetChildByFieldId(hVal, message, __h245(sequenceNumber), &sq, NULL);
    in_RM->seqNum = (RvUint8)sq;

    nodeId = pvtGetChild(hVal, message, __h245(requestedModes), NULL);

    H245CBEnter((app->pAppObject, "cmEvCallRequestMode: haCall=%p, hsCall=%p, transfer, nodeId=%d.",
        (HAPPCALL)emaGetApplicationHandle(callElem), (HCALL)callElem, nodeId));
    nesting = emaPrepareForCallback(callElem);
    app->cmMySessionEvent.cmEvCallRequestMode((HAPPCALL)emaGetApplicationHandle(callElem), (HCALL)callElem, cmReqModeRequest, nodeId);
    emaReturnFromCallback(callElem, nesting);
    H245CBExit((app->pAppObject, "cmEvCallRequestMode."));

    return RV_TRUE;
}

int requestModeAck(H245Control* ctrl, int message)
{
    H245Object          *app;
    HPVT                hVal;
    EMAElement          callElem;
    OutRequestModeInfo  *out_RM = &ctrl->outRequestMode;
    RvPvtNodeId         nodeId;
    RvInt32             sq;
    int                 nesting;

    callElem = (EMAElement)cmiGetByControl((HCONTROL)ctrl);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance(callElem));

    if (app->cmMySessionEvent.cmEvCallRequestMode == NULL)
        return RV_TRUE;

    hVal = app->hVal;

    pvtGetChildByFieldId(hVal, message, __h245(sequenceNumber), &sq, NULL);

    if (out_RM->seqNum == (RvUint8)sq)
    {
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &out_RM->timer);
    }
    nodeId = pvtGetChild(hVal, message, __h245(response), NULL);

    H245CBEnter((app->pAppObject, "cmEvCallRequestMode: haCall=%p, hsCall=%p, confirm, nodeId=%d.",
        (HAPPCALL)emaGetApplicationHandle(callElem), (HCALL)callElem, nodeId));
    nesting = emaPrepareForCallback((EMAElement)callElem);
    app->cmMySessionEvent.cmEvCallRequestMode((HAPPCALL)emaGetApplicationHandle(callElem), (HCALL)callElem, cmReqModeAccept, nodeId);
    emaReturnFromCallback(callElem, nesting);
    H245CBExit((app->pAppObject, "cmEvCallRequestMode."));

    return RV_TRUE;
}

int requestModeReject(H245Control* ctrl, int message)
{
    H245Object          *app;
    HPVT                hVal;
    EMAElement          callElem;
    OutRequestModeInfo  *out_RM = &ctrl->outRequestMode;
    RvPvtNodeId         nodeId;
    RvInt32             sq;
    int                 nesting;

    callElem = (EMAElement)cmiGetByControl((HCONTROL)ctrl);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance(callElem));

    if (app->cmMySessionEvent.cmEvCallRequestMode == NULL)
        return RV_TRUE;

    hVal = app->hVal;

    pvtGetChildByFieldId(hVal,message,__h245(sequenceNumber),&sq,NULL);

    if (out_RM->seqNum == (RvUint8)sq)
    {
        app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &out_RM->timer);
    }
    nodeId = pvtGetChild(hVal, message, __h245(cause), NULL);

    H245CBEnter((app->pAppObject, "cmEvCallRequestMode: haCall=%p, hsCall=%p, reject, nodeId=%d.",
        (HAPPCALL)emaGetApplicationHandle(callElem), (HCALL)callElem, nodeId));
    nesting = emaPrepareForCallback(callElem);
    app->cmMySessionEvent.cmEvCallRequestMode((HAPPCALL)emaGetApplicationHandle(callElem), (HCALL)callElem, cmReqModeReject, nodeId);
    emaReturnFromCallback(callElem, nesting);
    H245CBExit((app->pAppObject, "cmEvCallRequestMode."));

    return RV_TRUE;
}

int requestModeRelease(H245Control* ctrl, int message)
{
    H245Object          *app;
    HPVT                hVal;
    EMAElement          callElem;
    InRequestModeInfo   *in_RM = &ctrl->inRequestMode;
    RvInt32             sq = 0;
    RvPvtNodeId         nodeId;
    int                 nesting;

    callElem = (EMAElement)cmiGetByControl((HCONTROL)ctrl);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance(callElem));

    if (app->cmMySessionEvent.cmEvCallRequestMode == NULL)
        return RV_TRUE;

    hVal = app->hVal;

    pvtGetChildByFieldId(hVal, message, __h245(sequenceNumber), &sq, NULL);
    in_RM->seqNum = (RvUint8)sq;

    nodeId = pvtGetChildByFieldId(hVal, message, __h245(requestedModes), &sq, NULL);

    H245CBEnter((app->pAppObject, "cmEvCallRequestMode: haCall=%p, hsCall=%p, release, nodeId=%d.",
        (HAPPCALL)emaGetApplicationHandle(callElem), (HCALL)callElem, nodeId));
    nesting = emaPrepareForCallback(callElem);
    app->cmMySessionEvent.cmEvCallRequestMode((HAPPCALL)emaGetApplicationHandle(callElem), (HCALL)callElem, cmReqModeReleaseIn, nodeId);
    emaReturnFromCallback(callElem, nesting);
    H245CBExit((app->pAppObject, "cmEvCallRequestMode."));

    return RV_TRUE;
}

RvBool reqMode_TimeoutEventsHandler(void*_ctrl)
{
    H245Control*        ctrl=(H245Control*)_ctrl;
    H245Object          *app;
    HPVT                hVal;
    EMAElement          callElem;
    RvPvtNodeId         message;
    OutRequestModeInfo  *out_RM = &ctrl->outRequestMode;

    callElem = (EMAElement)cmiGetByControl((HCONTROL)ctrl);

    if (emaLock(callElem))
    {
        int nesting;

        app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance(callElem));
        hVal = app->hVal;

        app->evHandlers.pfnTimerClearEv((HCONTROL)ctrl, &out_RM->timer);

        if (app->cmMySessionEvent.cmEvCallRequestMode != NULL)
        {
            message = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);
            pvtAddBranch2(hVal, message, __h245(indication), __h245(requestModeRelease));
            sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, NULL);

            H245CBEnter((app->pAppObject, "cmEvCallRequestMode: haCall=%p, hsCall=%p, release.", (HAPPCALL)emaGetApplicationHandle(callElem), (HCALL)callElem));
            nesting = emaPrepareForCallback(callElem);
            app->cmMySessionEvent.cmEvCallRequestMode((HAPPCALL)emaGetApplicationHandle(callElem), (HCALL)callElem, cmReqModeReleaseOut, RV_PVT_INVALID_NODEID);
            emaReturnFromCallback(callElem, nesting);
            H245CBExit((app->pAppObject, "cmEvCallRequestMode."));
        }

        emaUnlock(callElem);
    }
    return RV_FALSE;
}



RVAPI int RVCALLCONV
cmCallRequestMode(
          /* send request mode msg */
          /* Note: id shall be deleted when function returns */
          IN  HCALL hsCall,
          IN  RvInt32 modeDescriptorsId /* (requestMode.requestedModes) */
          )
{
    H245Control*        ctrl;
    H245Object          *app;
    int                 nodeId;
    OutRequestModeInfo* out_RM;
    RvPvtNodeId         message;
    int                 res = RV_ERROR_UNKNOWN;
    int                 iSq;
    HPVT                hVal;

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;
#endif

    ctrl = (H245Control*)cmiGetControl(hsCall);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

    if (app == NULL)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmCallRequestMode: hsCall=%p, id=%d.",
        hsCall, modeDescriptorsId));

    if (emaLock((EMAElement)hsCall))
    {
        RvBool sentOnDummyHost;

        hVal = app->hVal;

        out_RM = &ctrl->outRequestMode;

        out_RM->seqNum++;
        out_RM->seqNum %= 255;

        message = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);
        nodeId = pvtAddBranch2(hVal, message, __h245(request), __h245(requestMode));
        iSq = out_RM->seqNum;
        pvtAdd(hVal, nodeId, __h245(sequenceNumber), iSq, NULL, NULL);
        nodeId = pvtAdd(hVal, nodeId, __h245(requestedModes), 0, NULL, NULL);
        pvtMoveTree(hVal, nodeId, modeDescriptorsId);

        res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, &sentOnDummyHost);

        if ((res >= 0) && !sentOnDummyHost)
        {
            int timeout = 10;
            pvtGetChildByFieldId(hVal, app->h245Conf, __h245(requestModeTimeout), &(timeout), NULL);
            app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &out_RM->timer);
            out_RM->timer = app->evHandlers.pfnTimerStartEv((HCONTROL)ctrl,
                reqMode_TimeoutEventsHandler, (void*)ctrl, timeout*1000);
        }

        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallRequestMode=%d", res));
    return res;
}



RVAPI int RVCALLCONV
cmCallRequestModeAck(
          /* Acknowledge the request */
          IN  HCALL hsCall,
          IN  const RvChar* responseName /* (requestModeAck.response) */
          )
{
    H245Control*       ctrl;
    H245Object         *app;
    int                nodeId;
    InRequestModeInfo* in_RM;
    RvPvtNodeId        message;
    int                res = RV_ERROR_UNKNOWN;
    HPVT               hVal;

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if ((hsCall == NULL) || (responseName == NULL))
        return RV_ERROR_NULLPTR;
#endif

    ctrl = (H245Control*)cmiGetControl(hsCall);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

    if (app == NULL)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmCallRequestModeAck: hsCall=%p, '%64s'.",
        hsCall, responseName));

    if (emaLock((EMAElement)hsCall))
    {
        int iSq;

        hVal = app->hVal;

        message = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);
        in_RM = &ctrl->inRequestMode;
        nodeId = pvtAddBranch2(hVal, message, __h245(response), __h245(requestModeAck));
        iSq = in_RM->seqNum;
        pvtAdd(hVal, nodeId, __h245(sequenceNumber), iSq, NULL, NULL);
        nodeId = pvtAdd(hVal, nodeId, __h245(response), 0, NULL, NULL);
        pvtBuildByPath(app->hVal, nodeId, responseName, 0, NULL);

        res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, NULL);

        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallRequestModeAck=%d", res));
    return res;
}



RVAPI int RVCALLCONV
cmCallRequestModeReject(
          /* Reject the request */
          IN  HCALL hsCall,
          IN  const RvChar* causeName /* requestModeReject.cause */
          )
{
    H245Control*       ctrl;
    H245Object         *app;
    int                nodeId;
    InRequestModeInfo* in_RM;
    RvPvtNodeId        message;
    int                res = RV_ERROR_UNKNOWN;
    HPVT               hVal;

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if ((hsCall == NULL) || (causeName == NULL))
        return RV_ERROR_NULLPTR;
#endif

    ctrl = (H245Control*)cmiGetControl(hsCall);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

    if (app == NULL)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmCallRequestModeReject: hsCall=%p, '%s'.", hsCall, causeName));

    if (emaLock((EMAElement)hsCall))
    {
        int iSq;

        hVal = app->hVal;

        in_RM = &ctrl->inRequestMode;
        message = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);
        nodeId = pvtAddBranch2(hVal, message, __h245(response), __h245(requestModeReject));
        iSq = in_RM->seqNum;
        pvtAdd(hVal, nodeId, __h245(sequenceNumber), iSq, NULL, NULL);
        nodeId = pvtAdd(hVal, nodeId, __h245(cause), 0, NULL, NULL);
        pvtBuildByPath(hVal, nodeId, causeName, 0, NULL);

        res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, NULL);

        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallRequestModeReject=%d", res));
    return res;
}

RVAPI int RVCALLCONV  /* request mode node id or RV_ERROR_UNKNOWN */
cmRequestModeBuild(
           /* Build request mode msg */
           /* Note: entryId overrides name */
           IN  HAPP hApp,
           IN  cmReqModeEntry ** modes[] /* modes matrix in preference order. NULL terminated arrays */
           )
{
    int rootId, i, j, _descId, _entryId, id,ret;
    HPVT hVal;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp || !modes) return RV_ERROR_UNKNOWN;
    if (!modes[0]) return RV_ERROR_UNKNOWN; /* must contain at least one entry */
    H245APIEnter((app->pAppObject, "cmRequestModeBuild: hApp=%p",hApp));

    hVal = app->hVal;

    if ((rootId=pvtAddRootByPath(hVal, app->hSynProtH245,
                   "request.requestMode.requestedModes", 0, NULL)) <0)
    {
        H245APIExit((app->pAppObject,"cmRequestModeBuild=%d", rootId));
        return rootId;
    }
    for (i=0; modes[i]; i++)
    {
        _descId = pvtAdd(hVal, rootId, -444, 0, NULL, NULL);
        if (_descId<0)
        {
            H245APIExit((app->pAppObject,"cmRequestModeBuild=%d", _descId));
            return _descId;
        }
        for (j=0; modes[i][j]; j++)
        {
            if (modes[i][j]->entryId >=0)
            {
                _entryId = pvtAdd(hVal, _descId, -445, 0, NULL, NULL);
                if (_entryId<0)
                {
                    H245APIExit((app->pAppObject,"cmRequestModeBuild=%d", _entryId));
                    return _entryId;
                }

                pvtMoveTree(hVal, _entryId, modes[i][j]->entryId);
            }
            else
            {
                if (confGetModeEntry(hVal, app->h245Conf, modes[i][j]->name, &id)==RV_TRUE)
                {
                    ret =pvtAddTree(hVal, _descId, hVal, id);
                    if (ret<0)
                    {
                        H245APIExit((app->pAppObject,"cmRequestModeBuild=%d", ret));
                        return ret;
                    }
                }
            }
        }
    }

    H245APIExit((app->pAppObject,"cmRequestModeBuild: hApp=%p, root =%d", hApp, rootId));
    return rootId;
}




int /* RV_TRUE or RV_ERROR_UNKNOWN */
cmGetModeTypeName(
          /* Generate dataName using field name as in H.245 standard. */
          IN  HPVT hVal,
          IN  int dataTypeId, /* ModeElement node id */
          IN  int dataNameSize,
          OUT char *dataName, /* copied into user allocate space [128 chars] */
          OUT confDataType* type, /* of data */
          OUT RvInt32* typeId /* node id of media type */
          )
{
    HPST synConf = pvtGetSynTree(hVal, dataTypeId);
    int dataId = pvtChild(hVal, dataTypeId);
    int choice=-1;
    RvPstFieldId fieldId=-1;
    const char* dataNamePtr = NULL;

    if( (synConf == NULL) || (dataId < 0) )
        return RV_ERROR_UNKNOWN;

    pvtGet(hVal, dataId, &fieldId, NULL, NULL, NULL);
    switch(fieldId)
    {
    case __h245(nonStandard):
        choice = (int)confNonStandard;
        break;
    case __h245(videoMode):
        choice = (int)confVideoData;
        dataId = pvtChild(hVal, dataId);
        break;
    case __h245(audioMode):
        choice = (int)confAudioData;
        dataId = pvtChild(hVal, dataId);
        break;
    case __h245(dataMode):
        choice = (int)confData;
        dataId = pvtChild(hVal, pvtChild(hVal, dataId));
        break;

#if (RV_H245_LEAN_H223 == RV_NO)
    case __h245(EncryptionMode):
        choice = (int)confEncryptionData;
        dataNamePtr = "encryptionMode: Not supported";
        break;

    case __h245(h235Mode):
        choice = (int)confH235Control;
        dataNamePtr = "h235Mode: Not supported";
        break;
#endif

    case __h245(multiplexedStreamMode):
        choice = (int)confMultiplexedStream;
        dataNamePtr = "multiplexedStreamMode: Not supported";
        break;

#if (RV_H245_LEAN_H223 == RV_NO)
    case __h245(redundancyEncodingDTMode):
        choice = (int)confRedundancyEncoding;
        dataNamePtr = "redundancyEncodingDTMode: Not supported";
        break;

    case __h245(multiplePayloadStreamMode):
        choice = (int)confMultiplePayloadStream;
        dataNamePtr = "multiplePayloadStreamMode: Not supported";
        break;

    case __h245(fecMode):
        choice = (int)confFec;
        dataNamePtr = "fecMode: Not supported";
        break;
#endif

    default:
        break;
    }

    if (dataName != NULL)
    {
        if (dataNamePtr == NULL)
        {
            /* Found a supported data type */
            pvtGet(hVal, dataId, &fieldId, NULL, NULL, NULL);
            pstGetFieldName(synConf, fieldId, dataNameSize, dataName);
        }
        else
        {
            /* An unsupported one... */
            strncpy(dataName, dataNamePtr, (RvSize_t)dataNameSize);
        }
    }

    if (type) *type = (confDataType)choice;
    if (typeId) *typeId = dataId;

    return RV_TRUE;
}


static int
cmRequestModeBuildStructEntry(
                  /* Set the request mode structure by entry id */
                  IN  HPVT hVal,
                  IN  RvInt32 entryId,
                  cmReqModeEntry *entry
                  )
{
    entry->entryId = entryId;
    cmGetModeTypeName(hVal, pvtChild(hVal, entryId), sizeof(entry->name), entry->name, NULL, NULL);
    return RV_TRUE;
}


RVAPI int RVCALLCONV /* RV_TRUE or RV_ERROR_UNKNOWN */
cmRequestModeStructBuild(
             /* Build request mode matrix structure from msg */
             /* Note: entry name as in h.245 standard */
             IN  HAPP hApp,
             IN  RvInt32 descId, /* requestMode.requestedModes node id */
             IN  cmReqModeEntry **entries, /* user allocated entries */
             IN  int entriesSize, /* number of entries */
             IN  void **entryPtrs, /* pool of pointers to construct modes */
             IN  int ptrsSize, /* number of ptrs */

             /* modes matrix in preference order. NULL terminated arrays */
             OUT cmReqModeEntry ***modes[]
             )
{
  /*
     D: Descriptor
     E: Entry
     -: null

     ---------------------------------------------------------------------------
     | D(1) D(2) ... D - E(1,1) E2(1,2) ... E - E(2,1) - ... - E... E - - ===> |
     ---------------------------------------------------------------------------

     */


  /*
  cmReqModeEntry entries[CM_MAX_MODES];
  void *entryPtrs[CM_MAX_MODES_PTR];
  */

    int _descNum, _entryNum, _descId, _entryId, i, j;
    int _entryPos=0; /* vacant position in entries */
    int _entryPtrPos=0; /* next array position */
    HPVT hVal;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp || !modes || descId<0 || !entries || !entryPtrs) return RV_ERROR_UNKNOWN;
    H245APIEnter((app->pAppObject, "cmRequestModeStructBuild: hApp=%p",hApp));

    hVal = app->hVal;

    if ((_descNum = pvtNumChilds(hVal, descId)) > ptrsSize)
    {
        H245APIExit((app->pAppObject, "cmRequestModeStructBuild: hApp=%p: Overflow", hApp));
        return RV_ERROR_UNKNOWN; /* overflow */
    }
    entryPtrs[_descNum]=NULL; /* terminator */
    _entryPtrPos = _descNum+1;

    for (i=0; i<_descNum; i++)
    {
        _descId = pvtGetByIndex(hVal, descId, i+1, NULL);
        _entryNum = pvtNumChilds(hVal, _descId);
        entryPtrs[i]=&entryPtrs[_entryPtrPos];

        for (j=0; j<_entryNum; j++)
        {
            _entryId = pvtGetByIndex(hVal, _descId, j+1, NULL);
            if (_entryPtrPos+j >= ptrsSize || _entryPos >= entriesSize) /* overflow */
            {
                H245APIExit((app->pAppObject, "cmRequestModeStructBuild: RV_ERROR_UNKNOWN"));
                return RV_ERROR_UNKNOWN;
            }
            cmRequestModeBuildStructEntry(hVal, _entryId, entries[_entryPos]);
            entryPtrs[_entryPtrPos+j] = entries[_entryPos];
            _entryPos++;
        }

        entryPtrs[_entryPtrPos+j]=NULL;
        _entryPtrPos+=_entryNum+1;

    }

    *modes = (cmReqModeEntry***)entryPtrs;
    H245APIExit((app->pAppObject, "cmRequestModeStructBuild: hApp=%p",hApp));

    return RV_TRUE;
}


/************************************************************************
 * rmInit
 * purpose: Initialize the request mode process on a control channel
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
int rmInit(H245Control* ctrl)
{
    InRequestModeInfo* in_RM = &ctrl->inRequestMode;
    OutRequestModeInfo* out_RM = &ctrl->outRequestMode;

    in_RM->seqNum=0;
    out_RM->seqNum=0;
    out_RM->timer=NULL;

    return 0;
}

/************************************************************************
 * rmEnd
 * purpose: Stop the request mode process on a control channel
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
void rmEnd(IN H245Control* ctrl)
{
    OutRequestModeInfo* out_RM = &ctrl->outRequestMode;
    H245Object* app=(H245Object*)(cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl))));

    /* Get rid of the timer if one exists */
    app->evHandlers.pfnTimerCancelEv((HCONTROL)ctrl, &out_RM->timer);
}



#ifdef __cplusplus
}
#endif

