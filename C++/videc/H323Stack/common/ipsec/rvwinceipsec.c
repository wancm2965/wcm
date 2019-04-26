#include "rvtypes.h"
#include "rvaddress.h"
#include "rvalloc.h"
#include "rvlog.h"
#include "rvmutex.h"
#include "rvmemory.h"

#if (RV_OS_TYPE == RV_OS_TYPE_WINCE)

#if (RV_IMS_IPSEC_ENABLED == RV_YES)

#include "rvimsipsecprivate.h"

/* Include QuickSec interface */
#include "IpsecTypes.h"

/* Timeout for waiting response from QuickSec Policy Manager process for
   requested action. */
#define RV_QUICKSEC_RESPONSE_TIMEOUT (10000) /* msec */

/******************************************************************************
 * RvIpsecEngine
 *
 * This structure unites data, used for communication with QuickSec Policy
 * Manager.
 *****************************************************************************/
typedef struct
{
	RvLogMgr*     pLogMgr;
	HANDLE        hMsgQueueWrite; /* Common Core -> QuickSec action requests */
	HANDLE        hMsgQueueRead;  /* Common Core <- QuickSec action results */
	RvMutex       mutex;          /* QuickSec doesn't support multithreading.
	                                 Action Request / Action Result transaction
	                                 is locked by this object */
} RvIpsecEngine;

/* ----------------------------------------------------------------------------
 *      GLOBAL VARIABLES
 * ----------------------------------------------------------------------------
 */
RvIpsecEngine* g_pEngine = NULL;
LONG g_refCount = 0;


/* ----------------------------------------------------------------------------
 *      STATIC FUNCTION DECLARATIONS
 * ----------------------------------------------------------------------------
 */
static RvStatus MakeQuickSecAction(
                        IN RvIpsecEngine* pEngine,
						IN IpsecAction*   pAction);

/* ----------------------------------------------------------------------------
 *      FUNCTION IMPLEMENTATIONS
 * ----------------------------------------------------------------------------
 */
RvStatus rvAdIpsecInit()
{
    return RV_OK;
}

RvStatus rvAdIpsecEnd()
{
    return RV_OK;
}

/******************************************************************************
 * rvAdIpsecConstruct
 *
 * Allocate resources needed for communication with QuickSec Policy Manager.
 *
 * INPUT:
 *  logMgr - handle of the log manager for this instance
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 *******************************************************************************/
RvStatus rvAdIpsecConstruct(IN RvLogMgr* logMgr)
{
#define FUNCTION_NAME __FUNCTION__

    RvStatus           status = RV_OK;
	RvChar*            strError;
    MSGQUEUEOPTIONS    mqo;

	IMSIPSEC_LOG_ENTER_1("logMgr=%p", logMgr);

	/* Check reference counter */
	if (InterlockedIncrement(&g_refCount) > 1)
	{
	    IMSIPSEC_LOG_LEAVE_2("logMgr=%p, reference counter=%d", logMgr, g_refCount);
		return RV_OK;
	}

    /* Allocate memory for engine */
    status = RvMemoryAlloc(NULL,sizeof(RvIpsecEngine),logMgr,(void **)&g_pEngine);
    if ((status != RV_OK) || (g_pEngine == NULL))
	{
        IMSIPSEC_LOG_ERROR_1("logMgr=%p: failed to allocate engine", logMgr);
        return status;
    }
    memset(g_pEngine, 0, sizeof(RvIpsecEngine));
	g_pEngine->pLogMgr = logMgr;

	/* Construct mutex  */
    status = RvMutexConstruct(logMgr, &g_pEngine->mutex);
    if(status != RV_OK)
	{
        IMSIPSEC_LOG_ERROR_1("logMgr=%p: failed to construct engine mutex", logMgr);
		RvMemoryFree(g_pEngine, logMgr);
        return status;
    }

	/* Create message queues for communication with QuickSec Policy Manager */
    memset(&mqo, 0, sizeof(mqo));
    mqo.dwSize = sizeof(mqo);
    mqo.cbMaxMessage = sizeof(IpsecAction);
    mqo.bReadAccess = FALSE;

	/* Create RV -> QuickSec msg queue */
    g_pEngine->hMsgQueueWrite = CreateMsgQueue(MSGQUEUE_RV2QS_NAME, &mqo);
	if (g_pEngine->hMsgQueueWrite == NULL)
	{
        IMSIPSEC_LOG_ERROR_1("CreateMsgQueue RV2QS failed(err=%d)", GetLastError());
		strError = "failed to construct Message Queue";
		goto failure;
	}

	/* Create RV <- QuickSec msg queue */
    mqo.bReadAccess = TRUE;
    g_pEngine->hMsgQueueRead = CreateMsgQueue(MSGQUEUE_QS2RV_NAME, &mqo);
	if (g_pEngine->hMsgQueueRead == NULL)
	{
        IMSIPSEC_LOG_ERROR_1("CreateMsgQueue QS2RV failed(err=%d)", GetLastError());
		strError = "failed to construct Message Queue";
		goto failure;
	}

	IMSIPSEC_LOG_INFO_0("IPSec engine was constructed");
	IMSIPSEC_LOG_LEAVE;
	return RV_OK;

failure:
	IMSIPSEC_LOG_ERROR_2("status=%d, %s", status, strError);
	rvAdIpsecDestruct(logMgr);
	return status;
#undef FUNCTION_NAME
}

/******************************************************************************
 * rvAdIpsecDestruct
 *
 * Free resources, used for communication with QuickSec Policy Manager.
 *
 * INPUT:
 *  logMgr - handle of the log manager for this instance
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 *******************************************************************************/
RvStatus rvAdIpsecDestruct(IN RvLogMgr* logMgr)
{
#define FUNCTION_NAME __FUNCTION__

	RvStatus status = RV_OK;

	IMSIPSEC_LOG_ENTER_1("logMgr=%p", logMgr);

	/* Check reference counter */
	if (InterlockedDecrement(&g_refCount) > 0)
	{
	    IMSIPSEC_LOG_LEAVE_2("logMgr=%p, reference counter=%d", logMgr, g_refCount);
		return RV_OK;
	}

	/* Desruct mutex */
    RvMutexDestruct(&g_pEngine->mutex, logMgr);

	/* Free Message Queues */
    if (g_pEngine->hMsgQueueWrite != NULL)
	{
		CloseMsgQueue(g_pEngine->hMsgQueueWrite);
	}
    if (g_pEngine->hMsgQueueRead != NULL)
	{
		CloseMsgQueue(g_pEngine->hMsgQueueRead);
	}

	/* Free Engine itself */
	RvMemoryFree(g_pEngine, logMgr);

	IMSIPSEC_LOG_INFO_2("logMgr=%p,status=%d : IPsec engine was destructed", logMgr, status);

	IMSIPSEC_LOG_LEAVE_2("logMgr=%p,status=%d", logMgr, status);

	return status;
#undef FUNCTION_NAME
}

/******************************************************************************
 * rvAdSetOSSpecific
 *
 * Set OS specific data for SA/SP manipulations
 *
 * INPUT:
 *    sad       - contains data needed for SA/SP creation;
 *  logMgr      - handle of the log manager for this instance
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 *****************************************************************************/
RvStatus rvAdSetOSSpecific(
                    IN RvImsSAData* sad,
					IN RvLogMgr*    logMgr)
{
    RV_UNUSED_ARG(sad);
    RV_UNUSED_ARG(logMgr);
    return RV_OK;
}

/******************************************************************************
 * rvAdIPSecAddSASP
 *
 * Requests the QuickSec Policy Manager to set up policy rules,
 * that will protect specific IMS tunnel.
 * The IMS tunnel to be protected is identified by the set of parameters,
 * specified in RvImsIpsecTunnel structure.
 *
 *
 * INPUT:
 *    sad  - array of SA data
 * sapdNum - not in use
 *  logMgr - handle of the log manager for this instance
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 *****************************************************************************/
RvStatus rvAdIPSecAddSASP(
                    IN RvImsSAData* sad,
                    IN int          sapdNum,
                    IN RvLogMgr*    logMgr)
{
#define FUNCTION_NAME __FUNCTION__

	RvStatus        status;
	IpsecAction     action;
	IpsecImsTunnel* pImsTunnel;

	IMSIPSEC_LOG_ENTER_2("logMgr=%p,sad=%p", logMgr, sad);

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if (sad == NULL || g_pEngine == NULL)
    {
        IMSIPSEC_LOG_ERROR_3("logMgr=%p, sad=%p, g_pEngine=%p", logMgr, sad, g_pEngine);
        return RV_ERROR_NULLPTR;
    }
#endif

    if (sapdNum != 4)
    {
        IMSIPSEC_LOG_ERROR_0("4 SAPD was expected");
        return RV_ERROR_BADPARAM;
    }

    action.eType = ACTION_IMSTUNNEL_ADD;

    /* Prepare IMS Tunnel configuration data */

	pImsTunnel = &action.data.tunnel;

    /* Set IPs */
    RvAddressGetString((const RvAddress*)&sad->iLclAddress,
		               IP_MAX_LENGTH, pImsTunnel->strLocalIP);
    RvAddressGetString((const RvAddress*)&sad->iPeerAddress,
		               IP_MAX_LENGTH, pImsTunnel->strRemoteIP);

    /* Set transport protocol */
    pImsTunnel->protocol[0] = '\0';
    if (sad->iProto != RvImsProtoAny)
    {
        strcpy(pImsTunnel->protocol,
               (sad->iProto == RvImsProtoUdp ? "udp" : "tcp"));
    }

    /* Set algorithms */
    pImsTunnel->algorithms = 0;
    switch (sad->iEncrAlg)
    {
        case RvImsEncr3Des:
            pImsTunnel->algorithms |= ALG_3DES;
            break;
        case RvImsEncrAes:
            pImsTunnel->algorithms |= ALG_AES;
            break;
        case RvImsEncrNull:
            break;
        default:
            IMSIPSEC_LOG_ERROR_1("Bad encryption algorithm %d", sad->iEncrAlg);
            return RV_ERROR_BADPARAM;
    }
    switch (sad->iAuthAlg)
    {
        case RvImsAuthMd5:
            pImsTunnel->algorithms |= ALG_MD5;
            break;
        case RvImsAuthSha1:
            pImsTunnel->algorithms |= ALG_SHA1;
            break;
        default:
            IMSIPSEC_LOG_ERROR_1("Bad authentication algorithm %d", sad->iAuthAlg);
            return RV_ERROR_BADPARAM;
    }

    /* Set keys */
    memcpy(pImsTunnel->key_encr,  sad->iEncrKey.iKeyValue, sad->iEncrKey.iKeyBits / 8);
    pImsTunnel->keyLen_encr = sad->iEncrKey.iKeyBits / 8;
    memcpy(pImsTunnel->key_auth,  sad->iAuthKey.iKeyValue, sad->iAuthKey.iKeyBits / 8);
    pImsTunnel->keyLen_auth = sad->iAuthKey.iKeyBits / 8;

    /* Set SPIs */
    pImsTunnel->localSpiC  = sad->iSpiInClnt;
    pImsTunnel->localSpiS  = sad->iSpiInSrv;
    pImsTunnel->remoteSpiC = sad->iSpiOutSrv;
    pImsTunnel->remoteSpiS = sad->iSpiOutClnt;

    /* Set ports */
    pImsTunnel->localPortC  = sad->iLclPrtClnt;
    pImsTunnel->localPortS  = sad->iLclPrtSrv;
    pImsTunnel->remotePortC = sad->iPeerPrtClnt;
    pImsTunnel->remotePortS = sad->iPeerPrtSrv;

	status = MakeQuickSecAction(g_pEngine, &action);

    if (status == RV_OK)
    {
        /* On successful return from  MakeQuickSecAction the action should
           contain answer of QuickSec Policy Manager with secure rule
           identifiers, that should be provided later on security removing. */
        IpsecRuleIds* pIdentifiers = &action.data.identifiers;
        sad->iIpSecSpecific.iSAP[0].iSPDIndex = pIdentifiers->id[0];
        sad->iIpSecSpecific.iSAP[0].iSADIndex = pIdentifiers->id[1];
        sad->iIpSecSpecific.iSAP[1].iSPDIndex = pIdentifiers->id[2];
        sad->iIpSecSpecific.iSAP[1].iSADIndex = pIdentifiers->id[3];
        sad->iIpSecSpecific.iSAP[2].iSPDIndex = pIdentifiers->id[4];
        sad->iIpSecSpecific.iSAP[2].iSADIndex = pIdentifiers->id[5];
        sad->iIpSecSpecific.iSAP[3].iSPDIndex = pIdentifiers->id[6];
        sad->iIpSecSpecific.iSAP[3].iSADIndex = pIdentifiers->id[7];
	}

	IMSIPSEC_LOG_LEAVE_3("logMgr=%p,sad=%p,status=%d", logMgr, sad, status);
    return status;
#undef FUNCTION_NAME
}

/******************************************************************************
 * rvAdIPSecDeleteSASP
 *
 * Requests the QuickSec Policy Manager to remove policy rules,
 * used for protection of IMS tunnel.
 *
 * INPUT:
 *    sad   - array of SA data
 * sapdNum  - not in use
 *  logMgr  - handle of the log manager for this instance
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 *****************************************************************************/
RvStatus rvAdIPSecDeleteSASP(
                    IN RvImsSAData* sad,
                    IN int          sapdNum,
                    IN RvLogMgr*    logMgr)
{
#define FUNCTION_NAME __FUNCTION__

	RvStatus      status;
	IpsecAction   action;
    IpsecRuleIds* pIdentifiers;

    RV_UNUSED_ARG(sapdNum);

	IMSIPSEC_LOG_ENTER_2("logMgr=%p,sad=%p", logMgr, sad);

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if (sad == NULL || g_pEngine == NULL)
    {
        IMSIPSEC_LOG_ERROR_3("logMgr=%p, sad=%p, g_pEngine=%p", logMgr, sad, g_pEngine);
        return RV_ERROR_NULLPTR;
    }
#endif

    action.eType = ACTION_IMSTUNNEL_REMOVE;
    pIdentifiers = &action.data.identifiers;
    pIdentifiers->id[0] = sad->iIpSecSpecific.iSAP[0].iSPDIndex;
    pIdentifiers->id[1] = sad->iIpSecSpecific.iSAP[0].iSADIndex;
    pIdentifiers->id[2] = sad->iIpSecSpecific.iSAP[1].iSPDIndex;
    pIdentifiers->id[3] = sad->iIpSecSpecific.iSAP[1].iSADIndex;
    pIdentifiers->id[4] = sad->iIpSecSpecific.iSAP[2].iSPDIndex;
    pIdentifiers->id[5] = sad->iIpSecSpecific.iSAP[2].iSADIndex;
    pIdentifiers->id[6] = sad->iIpSecSpecific.iSAP[3].iSPDIndex;
    pIdentifiers->id[7] = sad->iIpSecSpecific.iSAP[3].iSADIndex;

	status = MakeQuickSecAction(g_pEngine, &action);

	IMSIPSEC_LOG_LEAVE_3("logMgr=%p,sad=%p,status=%d",logMgr, sad, status);
    return status;
#undef FUNCTION_NAME
}


/******************************************************************************
 * rvAdIPSecCleanAll
 *
 * Removes all SA/SP set on this host.
 *
 * INPUT:
 *  logMgr      - handle of the log manager for this instance
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 *****************************************************************************/
RvStatus rvAdIPSecCleanAll(IN RvLogMgr *logMgr)
{
#define FUNCTION_NAME __FUNCTION__
    IMSIPSEC_LOG_ERROR_1("logMgr=%p - operation is not supported", logMgr);
    return RV_ERROR_NOTSUPPORTED;
#undef FUNCTION_NAME
}

/******************************************************************************
 * rvAdIpsecPickSpiValue
 *
 * Requests the QuickSec Policy Manager to allocate value for inbound SPI.
 *
 * INPUT:
 *  logMgr     - handle of the log manager
 *  rangeStart - start of range, where from the value should be chosen
 *  rangeStart - end of range, where from the value should be chosen
 * OUTPUT:
 *  pVal       - chosen value
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 *****************************************************************************/
RvStatus rvAdIpsecPickSpiValue(IN RvLogMgr* logMgr, IN RvUint32 rangeStart, IN RvUint32 rangeEnd, OUT RvUint32* pVal)
{
#define FUNCTION_NAME __FUNCTION__

	RvStatus     status;
	IpsecAction  action;

	IMSIPSEC_LOG_ENTER_1("logMgr=%p", logMgr);

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if (g_pEngine == NULL ||  pVal == NULL)
    {
        IMSIPSEC_LOG_ERROR_3("logMgr=%p, g_pEngine=%p, pVal=%p", logMgr, g_pEngine, pVal);
        return RV_ERROR_NULLPTR;
    }
#endif

    action.eType = ACTION_ALLOC_SPI;
	status = MakeQuickSecAction(g_pEngine, &action);

	if (status == RV_OK)
	{
		/* If allocated SPI value does not fit the requested range,
		   free it and report an error */
		if (action.data.spi < rangeStart || rangeEnd < action.data.spi)
		{
            IMSIPSEC_LOG_ERROR_3("allocated by QuickSec SPI value %d doesn't fit the range [%d-%d]",
				action.data.spi, rangeStart, rangeEnd);
            IMSIPSEC_LOG_WARN_0("consider change of SSH_ENGINE_INBOUND_SPI_MAX_MANUAL in QuickSec");
			action.eType = ACTION_FREE_SPI;
			MakeQuickSecAction(g_pEngine, &action);
			return RV_ERROR_UNKNOWN;
		}

		*pVal = action.data.spi;
	}

	IMSIPSEC_LOG_LEAVE_2("logMgr=%p,spi=%d", logMgr, *pVal);
    return status;
#undef FUNCTION_NAME
}

/******************************************************************************
 * rvAdIpsecFreeSpiValue
 *
 * Causes QuickSec Policy Manager to free the specified SPI value.
 *
 * INPUT:
 *  logMgr - handle of the log manager
 *  val    - value to be freed
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 *****************************************************************************/
RvStatus rvAdIpsecFreeSpiValue(IN RvLogMgr* logMgr, IN RvUint32 val)
{
#define FUNCTION_NAME __FUNCTION__

	RvStatus     status;
	IpsecAction  action;

	IMSIPSEC_LOG_ENTER_2("logMgr=%p,val=%d", logMgr, val);

#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if (g_pEngine == NULL)
    {
        IMSIPSEC_LOG_ERROR_2("logMgr=%p, g_pEngine=%p", logMgr, g_pEngine);
        return RV_ERROR_NULLPTR;
    }
#endif

    action.eType = ACTION_FREE_SPI;
	action.data.spi = val;

	status = MakeQuickSecAction(g_pEngine, &action);

	IMSIPSEC_LOG_LEAVE_3("logMgr=%p,spi=%d,status=%d", logMgr, val, status);
    return status;
#undef FUNCTION_NAME
}

/* ----------------------------------------------------------------------------
 *      STATIC FUNCTION IMPLEMENTATIONS
 * ----------------------------------------------------------------------------
 */
/******************************************************************************
 * MakeQuickSecAction
 *
 * Sends the Action Request to the QuickSec Policy Manager.
 * This function is blocked on waiting for Action Result,
 * that should be sent by the QuickSec Policy Manager in response to the Action
 * Request.
 * May return on timeout.
 *
 * INPUT:
 *  logMgr  - handle of the log manager
 *  pAction - action request to be sent
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 *****************************************************************************/
static RvStatus MakeQuickSecAction(
                            IN RvIpsecEngine* pEngine,
                            IN IpsecAction*   pAction)
{
#define FUNCTION_NAME __FUNCTION__

	RvLogMgr* logMgr = pEngine->pLogMgr;
	RvStatus  status;
	BOOL	  bResult;
    DWORD     sizeOfResponse, flags;
	char*     strActionType;

#if (RV_CHECK_MASK & RV_CHECK_NULL)
	if (pEngine->hMsgQueueWrite == NULL)
	{
		IMSIPSEC_LOG_ERROR_1("pEngine=%p : hMsgQueueWrite is NULL", pEngine);
		return RV_ERROR_NULLPTR;
	}
	if (pEngine->hMsgQueueRead == NULL)
	{
		IMSIPSEC_LOG_ERROR_1("pEngine=%p : hMsgQueueRead is NULL", pEngine);
		return RV_ERROR_NULLPTR;
	}
#endif

	/* QuickSec Policy Manager enables only one concurrent action on Policy
       database (ssh_pm_commit() can't be called once again before
       SshPmStatusCB callback is called).
       To prevent simultaneous action requests from different threads lock
       the action requisting */
    status = RvMutexLock(&pEngine->mutex, pEngine->pLogMgr);
    if (status != RV_OK)
    {
		IMSIPSEC_LOG_ERROR_1("pEngine=%p : failed to lock engine", pEngine);
		return status;
    }

	/* Send action request to the QuickSec Policy Manager process */
	IpsecConvertActionType2Str(pAction->eType, &strActionType);
    IMSIPSEC_LOG_DEBUG_2("pEngine=%p: request action %s ...", pEngine, strActionType);
    bResult = WriteMsgQueue(pEngine->hMsgQueueWrite, pAction,
		                    sizeof(IpsecAction), 0, 0);
	if (bResult == 0)
	{
        IMSIPSEC_LOG_ERROR_2("failed to send message(pEngine=%p, err=%d)",
                             pEngine, GetLastError());
        RvMutexUnlock(&pEngine->mutex, pEngine->pLogMgr);
        return RV_ERROR_UNKNOWN;
    }

	/* Wait for response from QuickSec Policy Manager */
    IMSIPSEC_LOG_DEBUG_1("pEngine=%p: waiting for action completion...", pEngine);

    bResult = ReadMsgQueue(pEngine->hMsgQueueRead, pAction,
		                   sizeof(IpsecAction), &sizeOfResponse,
						   RV_QUICKSEC_RESPONSE_TIMEOUT, &flags);
	if (bResult == 0)
	{
        IMSIPSEC_LOG_ERROR_2("failed to read message(pEngine=%p, err=%d)",
                             pEngine, GetLastError());
        RvMutexUnlock(&pEngine->mutex, pEngine->pLogMgr);
        return RV_ERROR_UNKNOWN;
    }

    /* RESULT_IDS is returned in response to IMSTUNNEL_ADD action on success;
	   RESULT_SPI is returned in response to ALLOC_SPI action on success*/
    if (pAction->eType==ACTION_RESULT_IDS ||
		pAction->eType==ACTION_RESULT_SPI)
    {
        status = RV_OK;
    }
    else if (pAction->eType==ACTION_RESULT)
    {
        switch(pAction->data.eResult)
        {
            case ACTION_RESULT_OK:
                status = RV_OK;
                break;
            default:
                status = RV_ERROR_UNKNOWN;
                break;
        }
    }
    else
    {
        status = RV_ERROR_UNKNOWN;
    }

	IMSIPSEC_LOG_INFO_2("pEngine=%p: action was completed:result=%d",
		                pEngine, status);

    RvMutexUnlock(&pEngine->mutex, pEngine->pLogMgr);

    return status;
#undef FUNCTION_NAME
}

#endif /*(RV_IMS_IPSEC_ENABLED == RV_YES) */

#else
int prevent_warning_of_ranlib_has_no_symbols_rvwinceipsec=0;
#endif /*(RV_OS_TYPE == RV_OS_TYPE_LINUX && RV_OS_VERSION & RV_OS_LINUX_REDHAT)*/

