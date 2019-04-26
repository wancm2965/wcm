#include "rverror.h"

/* To enable interface monitoring, set this flag to '1' in rvusrconfig.h */
#if (RV_OS_TYPE == RV_OS_TYPE_LINUX)&& RV_USES_IFMONITOR

#if defined(__cplusplus)
extern "C" {
#endif


/* the supported interface states */
typedef enum {
    RvInterfaceUp,          /* interface is UP & RUNNING */
    RvInterfaceDown,        /* interface is DOWN */
    RvInterfaceError        /* error on the interface */
} RvInterfaceState;

typedef void* RvInterfaceMonitorHandle;


/********************************************************************************************
 * RvInterfaceMonitorOnEvent - the application provided callback 
 *  Such function will be called by CommonCore interface monitor when
 *  the interface monitor changes.
 *
 * INPUT   : hIfMon         - the monitor handle created by RvInterfaceMonitorStart
 *           pUsrCtx        - the application context
 *           ifState        - the current interface state
 * OUTPUT  : None
 * RETURN  : None
 */
typedef RvStatus (*RvInterfaceMonitorOnEvent)(
    IN RvInterfaceMonitorHandle hIfMon,
    IN void* pUsrCtx, 
    IN RvInterfaceState ifState);


/********************************************************************************************
 * RvInterfaceMonitorStart - Starts to monitor the network interface
 *
 * Application registers the callback to be called when the network interface state
 * changes.
 * This function must be called in the thread where the toolkit (SIP,ARTP etc) was
 * constructed.
 *
 * INPUT   : addr           - the address of the interface
 *           ifname         - the name of the interface. ("eth0" for example)
 *           One of addr or ifname must be provided.
 *           onEvent        - the callback to be called when the interface status changes
 *           pUsrCtx        - the application context
 *           logMgr         - the log manager (may be NULL)
 * OUTPUT  : phIfMon        - the monitor handle. It will be needed later for RvInterfaceMonitorStop
 *           pIfState       - if not NULL the current state of the interface will be returned.
 * RETURN  : RV_OK on success, other on failure
 */
RvStatus RvInterfaceMonitorStart(
	IN RvAddress* addr, 
	IN RvChar* ifname, 
	IN RvInterfaceMonitorOnEvent onEvent,
	IN void* pUsrCtx,
	IN RvLogMgr *logMgr,
	OUT RvInterfaceMonitorHandle* phIfMon,
	OUT RvInterfaceState* pIfState);

/********************************************************************************************
 * RvInterfaceMonitorStop - Stops to monitor the network interface
 *
 *
 * INPUT   : hIfMon         - the monitor handle created by RvInterfaceMonitorStart
 * OUTPUT  : None
 * RETURN  : None
 */
void RvInterfaceMonitorStop(
	IN RvInterfaceMonitorHandle hIfMon);


#if defined(__cplusplus)
}
#endif


#endif /* (RV_OS_TYPE == RV_OS_TYPE_LINUX)&& RV_USES_IFMONITOR */

