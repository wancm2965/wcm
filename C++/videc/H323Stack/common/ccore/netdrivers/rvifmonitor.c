#include "rvtypes.h"
#include "rverror.h"
#include "rvnettypes.h"
#include "rvselect.h"

#include "rvlog.h"

#include "rvifmonitor.h"

/* To enable interface monitoring, set this flag to '1' in rvusrconfig.h */
#if (RV_OS_TYPE == RV_OS_TYPE_LINUX)&& RV_USES_IFMONITOR

#include <ifaddrs.h>
#include <errno.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>



#if (RV_LOGMASK != RV_LOGLEVEL_NONE)
#define LOG_ERR(f) if (logMgr) { RvLogError(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f));}
#define LOG_ERR_1(f,p1) if (logMgr) { RvLogError(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f, p1));}

#define LOG_DBG(f) if (logMgr) { RvLogDebug(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f));}
#define LOG_DBG_1(f,p1) if (logMgr) { RvLogDebug(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f, p1));}
#define LOG_DBG_2(f,p1,p2) if (logMgr) { RvLogDebug(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f, p1,p2));}
#define LOG_DBG_3(f,p1,p2,p3) if (logMgr) { RvLogDebug(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f, p1,p2,p3));}

#define LOG_ENTER(f) if (logMgr) { RvLogEnter(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f));}
#define LOG_ENTER_1(f,p1) if (logMgr) { RvLogEnter(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f, p1));}
#define LOG_ENTER_2(f,p1,p2) if (logMgr) { RvLogEnter(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f, p1,p2,));}
#define LOG_ENTER_3(f,p1,p2,p3) if (logMgr) { RvLogEnter(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f, p1,p2,p3));}

#define LOG_LEAVE(f) if (logMgr) { RvLogLeave(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f));}
#define LOG_LEAVE_1(f,p1) if (logMgr) { RvLogLeave(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f, p1));}
#define LOG_LEAVE_3(f,p1,p2,p3) if (logMgr) { RvLogLeave(&logMgr->socketSource, (&logMgr->socketSource, FUNCTION_NAME " " f, p1,p2,p3));}
#else
#define LOG_ERR(f) 
#define LOG_ERR_1(f,p1) 

#define LOG_DBG(f) 
#define LOG_DBG_1(f,p1) 
#define LOG_DBG_2(f,p1,p2) 
#define LOG_DBG_3(f,p1,p2,p3) 

#define LOG_ENTER(f) 
#define LOG_ENTER_1(f,p1) 
#define LOG_ENTER_2(f,p1,p2) 
#define LOG_ENTER_3(f,p1,p2,p3)

#define LOG_LEAVE(f) 
#define LOG_LEAVE_1(f,p1) 
#define LOG_LEAVE_3(f,p1,p2,p3) 
#endif


typedef struct {
    RvInt                       udpsock;
    RvInt                       nlsock;
    RvSelectFd                  selFd;
    RvInterfaceState            currState;
    RvInterfaceMonitorOnEvent   appOnEvent;
    void*                       pAppCtx;
    char                        ifname[IFNAMSIZ+1];
    RvLogMgr*                   logMgr;
    RvSelectEngine*             selEngine;
} RvIfmonitorData;



static RvInterfaceState getInterfaceStatus(int udps, char *ifname)
{
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name)-1);

    if (ioctl(udps, SIOCGIFFLAGS, &ifr) == -1) {
        return RvInterfaceError;
    }

    return ifr.ifr_flags & IFF_RUNNING ? RvInterfaceUp : RvInterfaceDown;

}

static void nlSockCb(
    IN RvSelectEngine*  selectEngine,
    IN RvSelectFd*      fd,
    IN RvSelectEvents   selectEvent,
    IN RvBool           error)
{
#define FUNCTION_NAME "nlSockCb"    
    RvIfmonitorData *monData;
    RvLogMgr* logMgr;
    RvInt ret;
    RvChar buff[4096];
    RvInterfaceState istate;

    RV_UNUSED_ARG(selectEngine);


    monData = (RvIfmonitorData*)((RvChar*)fd-RV_OFFSETOF(RvIfmonitorData,selFd));
    logMgr = monData->logMgr;

    if (error)
    {
        LOG_ERR("Select report error");
        return;
    }

    if (selectEvent != RvSelectRead)
    {
        LOG_ERR_1("Unexpected event (%d) reported",selectEvent);
        return;
    }

    ret = recv(monData->nlsock,buff,sizeof(buff),0);
    if (ret < 0)
    {
        LOG_ERR_1("recv failed for NL socket with error %d",errno);
        return;
    }

    istate = getInterfaceStatus(monData->udpsock,monData->ifname);
    if (istate == RvInterfaceError)
    {
        LOG_ERR_1("interface %s reports error",monData->ifname);
        return;
    }

    if (istate == monData->currState)
        return;

    LOG_DBG_3("interface %s reports state change %d->%d",monData->ifname,monData->currState,istate);
    monData->currState = istate;

    (monData->appOnEvent)((RvInterfaceMonitorHandle)monData,monData->pAppCtx,istate);

#undef FUNCTION_NAME
}

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
	OUT RvInterfaceState* pIfState)
{
#define FUNCTION_NAME "RvInterfaceMonitorStart"
#if (RV_LOGMASK != RV_LOGLEVEL_NONE)
    RvChar addrTxt[256];
#endif
    RvSelectEngine* pSelEngine;
    RvChar ifnameStr[IFNAMSIZ+1], *i;
    RvInt udps, nlsock;
    RvInterfaceState iState;
    RvIfmonitorData* monData;
    RvStatus retv;

    LOG_ENTER("");

    if (RvSelectGetThreadEngine(logMgr,&pSelEngine) != RV_OK || pSelEngine == NULL)
    {
        LOG_ERR("RvSelectGetThreadEngine failed");
        return RV_ERROR_UNKNOWN;
    }

    if (!logMgr)
        logMgr = pSelEngine->logMgr;

    LOG_DBG_2("Called addr %s  ifname %s",
        (addr)?RvAddressGetString(addr,sizeof(addrTxt),addrTxt):"none",
        (ifname)?ifname:"");

    if (addr == NULL && (ifname == NULL || *ifname == 0))
    {
        LOG_ERR("No input data was provided");
        return RV_ERROR_UNKNOWN;
    }

    /* if no interface name was provided, get it */
    if (!ifname || !*ifname)
    {        
        struct ifaddrs *ifap, *ifa;
        union {
            struct sockaddr_in dummy;
            RvUint8 sockdata[RV_SOCKET_SOCKADDR_SIZE];
        } sockAddr;
        int sockAddrLen,len,offs;

        i = NULL;

        RvSocketAddressToSockAddr(addr,&sockAddr.sockdata,&sockAddrLen);

        if (getifaddrs(&ifap) != 0)
        {
            LOG_ERR_1("getifaddrs fails with error %d",errno);
            return RV_ERROR_UNKNOWN;
        }    

        len = (sockAddr.dummy.sin_family == AF_INET) ? 4:16;
        offs = (sockAddr.dummy.sin_family == AF_INET) ? 4:8;

        for (ifa = ifap; ifa; ifa = ifa->ifa_next) 
        {    
            if (sockAddr.dummy.sin_family == ifa->ifa_addr->sa_family &&
                memcmp((char*)&sockAddr+offs,(char*)ifa->ifa_addr+offs,len) == 0)
            {
                strcpy(ifnameStr,ifa->ifa_name);
                i = ifnameStr;
                break;
            }


        }
        freeifaddrs(ifap);

        if (i == NULL)
        {
            LOG_ERR_1("Could not find interface for address %s",RvAddressGetString(addr,sizeof(addrTxt),addrTxt));
            return RV_ERROR_UNKNOWN;
        }
        
        LOG_DBG_1("Could find the interface %s",i);
    }
    else
        i = ifname;


    /* test the  interface status */
    udps = socket(AF_INET,SOCK_DGRAM, 0);
    if (udps < 0)
    {
        LOG_ERR_1("failed to create the UDP socket, error is %d",errno);
        return RV_ERROR_UNKNOWN;
    }

    iState = getInterfaceStatus(udps,i);
    if (pIfState)    
        *pIfState = iState;


    LOG_DBG_2("The current state of intreface %s is %d",i,iState);

    /* now create the NETLINK socket */
    {
        struct sockaddr_nl nladdr;
    
        if ((nlsock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0) 
        {
            LOG_ERR_1("failed to create the NETLINK socket, error is %d",errno);
            close(udps);
            return RV_ERROR_UNKNOWN;
        }
    
        memset(&nladdr, 0, sizeof(nladdr));
        nladdr.nl_family = AF_NETLINK;
        nladdr.nl_groups = RTMGRP_LINK;
        nladdr.nl_pid = getpid();
    
        if (bind(nlsock, (struct sockaddr *) &nladdr, sizeof(nladdr)) < 0) {
            LOG_ERR_1("failed to bind the NETLINK socket, error is %d",errno);
            close(nlsock);
            close(udps);
            return RV_ERROR_UNKNOWN;
        }

        LOG_DBG_1("Constructed bound NETLINK socket %d",nlsock);
    }

    /* allocate the memory */
    if (RvMemoryAlloc(NULL,sizeof(RvIfmonitorData),logMgr,(void**)&monData) != RV_OK)
    {
        LOG_ERR_1("RvMemoryAlloc failed",errno);
        return RV_ERROR_UNKNOWN;
    }
    memset(monData,0,sizeof(RvIfmonitorData));

    monData->currState = iState;
    monData->udpsock = udps;
    monData->nlsock = nlsock;  
    RvFdConstruct(&monData->selFd,&monData->nlsock,logMgr);
    monData->appOnEvent = onEvent;
    monData->pAppCtx = pUsrCtx;
    monData->logMgr = logMgr;
    monData->selEngine = pSelEngine;
    strcpy(monData->ifname,i);

    retv = RvSelectAdd(pSelEngine,&monData->selFd,RvSelectRead,nlSockCb);
    if (retv != RV_OK)
    {
        close(monData->nlsock);
        close(monData->udpsock);
        RvMemoryFree(monData,logMgr);
        LOG_ERR_1("RvSelectAdd failed for nl socket %d",monData->nlsock);
        return RV_ERROR_UNKNOWN;
    }

    *phIfMon = (RvInterfaceMonitorHandle) monData;

    LOG_LEAVE("");

    return RV_OK;
#undef FUNCTION_NAME
}

/********************************************************************************************
 * RvInterfaceMonitorStop - Stops to monitor the network interface
 *
 *
 * INPUT   : hIfMon         - the monitor handle created by RvInterfaceMonitorStart
 * OUTPUT  : None
 * RETURN  : None
 */
void RvInterfaceMonitorStop(
	IN RvInterfaceMonitorHandle hIfMon)
{
    RvIfmonitorData* monData = (RvIfmonitorData*) hIfMon;
    RvLogMgr* logMgr;

#define FUNCTION_NAME "RvInterfaceMonitorStop"

    LOG_ENTER_1("Called for %s",monData->ifname);

    RvSelectRemoveEx(monData->selEngine,&monData->selFd,RV_TRUE,NULL);

    close(monData->udpsock);
    close(monData->nlsock);

    RvFdDestruct(&monData->selFd);

    RvMemoryFree((void*)monData,logMgr);

    LOG_LEAVE("");

#undef FUNCTION_NAME
}

#endif /*(RV_OS_TYPE == RV_OS_TYPE_LINUX)&& RV_USES_IFMONITOR */
