/****************************************************************
*                        RTP Send.c                             *
*                                                               *
* The following program is a simple console application that    *
* demonstrates how to implement sending RTP packets             *
* using the RTP & RTCP protocols and the RADVISION H.323        *
* Protocol Stack.                                               *
* The program does the following:                               *
*                                                               *
* 1. Initializes the H.323 Stack.                               *
* 2. Installs the H.323 Stack call and channel callbacks.       *
* 3. Initializes the RTP and RTCP modules.                      *
* 4. Opens RTP and RTCP sessions.                               *
* 5. Prompts the user for the IP address of the receiving side. *
* 6. Creates a single call.                                     *
* 7. Waits for a response from the receiving side.              *
* 8. When the call is connected, opens one outgoing channel and *
*    sends the specified number of RTP packets to the receiving *
*    side.                                                      *
* 9. After the packets are sent, closes the channel and gives   *
*    the user the option to drop the call.                      *
****************************************************************/

/* Types of implementations we can use */
#define RTP_TYPE_NORMAL 0
#define RTP_TYPE_NEW 1

#if !defined(RTP_TYPE)
/* Default implementation is normal */
#define RTP_TYPE RTP_TYPE_NORMAL
#endif

#include "../osDependency.h"
#include <stdlib.h>
#include <seli.h>
#include <cm.h>
#include <rtp.h>
#include <rtcp.h>
#include <payload.h>
#include <mti.h>
#if (RTP_TYPE == RTP_TYPE_NEW)
#include "rvrtpnatfw.h"
#endif


/* The maximum RTP packets that are sent to the remote side (50
   in the example below). */
#define MAX_PACKETS 50


/* GLOBALS */


/* The application handle */
HAPP hApp;

/* The timer manager handle */
HSTIMER hTimers;

/* The Stack handle to the call */
HCALL hsCall;

/* The Stack handle to the outgoing channel. */
HCHAN hsOutChan;

/* The handle of the RTP session. */
#if (RTP_TYPE == RTP_TYPE_NEW)
RvRtpSession hRtp;
#else
HRTPSESSION hRtp;
#endif

/* The handle of the RTCP session. */
#if (RTP_TYPE == RTP_TYPE_NEW)
RvRtcpSession hRtcp;
#else
HRTCPSESSION hRtcp;
#endif

/* Stores the call state. */
RvUint32 callState;

/* Boolean indicator for terminating the application. */
static RvBool stop = RV_FALSE;

/* The number of milliseconds that elapse between the start of
   the system and the call connection. */
RvUint startTime;

/* Timer identifier (a value returned by the 'mtimerSet' function). */
HTI uIDEvent;

/* Pointer to the buffer containing the RTP packet. */
char buffer[600];

/* Stores the payload size in the buffer. */
RvUint payLoadSize = 480;

/* The number of RTP packets sent to the remote side. */
RvUint sentPackets = 0;



/* FUNCTIONS */

/* A callback function defined by the application that is
   notified when the time-out value elapses. A pointer to
   this callback function is transferred to setTimer. */
void RVCALLCONV TimerProc(IN void* context);

/* A function that initializes the buffer. */
void initBuffer(char *buf, RvInt32 sByte, int payLoadSize);




/* CALLBACK CODES */


/****************************************************************
 * inputHandler                                                 *
 * Purpose    : This is a callback function that is called      *
 *              whenever the user presses a key on the keyboard.*
 *              The function decides how to proceed according   *
 *              to the call state.                              *
 *              The key that is pressed is usually a response   *
 *              to a prompt for the user in the early stages of *
 *              the program.                                    *
 * Input      : fd       : File descriptor that you want to     *
 *                         register with seli.                  *
 *              seliEvent: The event that called the callback   *
 *                         function.                            *
 *              error    : If an error occurs, the value is 1.  *
 *                         If no error occurs, the value        *
 *                         remains 0.                           *
 ***************************************************************/
static void RVCALLCONV inputHandler(int fd, seliEvents sEvent, RvBool error)
{
    int answer;

    RV_UNUSED_ARG(fd);
    RV_UNUSED_ARG(sEvent);
    RV_UNUSED_ARG(error);

    /* Reads the user input. */
    answer = getchar();

    switch(callState)
    {
    case cmCallStateConnected:
        {
            /* The user opens a channel. The application first specifies the
               address that will be used as the RTCP address for an outgoing
               channel, then opens a channel. */
            if (answer == 'o')
            {
                cmTransportAddress tr;
                cmChannelNew(hsCall,NULL,&hsOutChan);
                cmGetLocalCallSignalAddress (hApp, &tr);

#if (RTP_TYPE == RTP_TYPE_NEW)
                cmChannelSetRTCPAddress(hsOutChan, 0, RvRtcpGetPort(hRtcp));
#else
                cmChannelSetRTCPAddress(hsOutChan, 0, rtcpGetPort(hRtcp));
#endif

                printf("Opening outgoing channel\n");
                cmChannelOpen(hsOutChan,(char *)"g711Alaw64k",NULL,NULL,0);
            }

            /* The user drops the call.*/
            if (answer == 'y')
            {
                printf("Dropping the call.\n");
                cmCallDrop(hsCall);
            }
        }
        break;

    /* The user closes the window and disconnects the Q.931 channel.
       When stop!=0, the loop in main function stops running. */
    case cmCallStateIdle:
        if(answer == 'y')
        {
            printf("Bye-bye\n");
            stop = RV_TRUE;
        }

        break;
    default:
        break;
    }

    if (stop)
        endKeyboardInput();
}



/****************************************************************
 * cmEvCallStateChanged                                         *
 * Purpose : This is the callback function used by the Stack to *
 *           report a change in the status of a call to the     *
 *           application.                                       *
 * Input   : haCall   :The application handle for the call.     *
 *           hsCall   :The Stack handle for the call.           *
 *           state    :The new state of the call.               *
 *           stateMode:New state mode. This parameter describes *
 *                     how or why the call enters a new state.  *
 * Return  : If an error occurs, the function returns a negative*
 *           value.                                             *
 *           If no error occurs, the function returns a         *
 *           non-negative value.                                *
 ***************************************************************/

static int RVCALLCONV cmEvCallStateChanged(IN   HAPPCALL           haCall,
                                           IN   HCALL              hsCall,
                                           IN   cmCallState_e      state,
                                           IN   cmCallStateMode_e  stateMode)
{
    RV_UNUSED_ARG(haCall);

    switch(state)
    {
    /* This callback function is called with the following state when
       the call is fully connected (both H.245 and Q.931 call
       establishment processes are complete). In this case, the
       application waits for a user response and opens an outgoing
       audio channel. After the channel is open, RTP packets will be
       sent. */
    case cmCallStateConnected:
        if (stateMode==cmCallStateModeConnectedCall)
        {
            callState = cmCallStateConnected;

            printf("Press 'o' to open an outgoing channel\n");
            printf("After the channel is opened %d RTP packets will be sent to the remote side.\n", MAX_PACKETS);
            printf("If you wish to drop the call, enter 'y' anytime\n");
        }
        break;


    /* The callback function is called with this state after the call is
       disconnected. The 'cmCallClose' function is called and then the
       user is prompted to close the application. */
    case cmCallStateIdle:
        callState = cmCallStateIdle;
        cmCallClose(hsCall);
        printf("The call is closed\n");
        printf("If you want to close the window, enter 'y'.\n");
        break;

    default:
        break;
    }
    return RV_OK;
}



/****************************************************************
 * cmEvChannelStateChanged                                      *
 * Purpose : This is the callback function used by the Stack to *
 *           report a change in channel status to the           *
 *           application.                                       *
 * Input   : haChan :Application handle for the channel.        *
 *           hsChan :Stack handle for the channel.              *
 *           state  :New state of the channel.                  *
 *           stateMode :New state mode. This parameter          *
 *                      describes how or why the channel        *
 *          enters a new state.                                 *
 * Return  : If an error occurs, the function returns a         *
 *           negative value.                                    *
 *           If no error occurs, the function returns a         *
 *           non-negative value.                                *
 ***************************************************************/
static int RVCALLCONV cmEvChannelStateChanged(IN  HAPPCHAN             haChannel,
                                              IN  HCHAN                hsChannel,
                                              IN  cmChannelState_e     state,
                                              IN  cmChannelStateMode_e stateMode)
{
    cmTransportAddress addr;
    RvInt32 size = sizeof(addr);
    RvStatus status;

    RV_UNUSED_ARG(haChannel);
    RV_UNUSED_ARG(stateMode);

    switch(state)
    {

    /* This callback function is called with the following state when
       the channel is connected. The buffer is then initialized while
       reserving the first few bytes for the header. The SetTimer
       function creates a timer with 60 ms time-out value so that
       the TimerProc callback function is notified every 60 ms. */

    case cmChannelStateConnected:
        printf("Channel is connected\n");

        status = cmChannelGetParam(hsChannel, cmChannelParamH225RemoteRtpAddress, &size, (RvUint8*)&addr);
        if (status == RV_OK)
        {
            printf("Setting up RTP address\n");
            /* The following function defines the RTP address of the remote
               peer or of the multicast group. */
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
#if (RTP_TYPE == RTP_TYPE_NEW)
            {
                RvNetAddress netAddr;
                RvNetIpv4 netV4;
                memset(&netAddr, 0, sizeof(netAddr));
                memset(&netV4, 0, sizeof(netV4));
                netV4.port = addr.port;
                netV4.ip = addr.addr.v4.ip;
                RvNetCreateIpv4(&netAddr, &netV4);
                RvRtpSetRemoteAddress(hRtp, &netAddr);
            }
#else
            rtpSetRemoteAddress(hRtp, addr.addr.v4.ip, addr.port);
#endif
#else
            rtpSetRemoteAddress(hRtp, addr.ip, addr.port);
#endif
        }

        status = cmChannelGetParam(hsChannel, cmChannelParamH225RemoteRtcpAddress, &size, (RvUint8*)&addr);
        if (status == RV_OK)
        {
            printf("Setting up RTCP address\n");
            /* The following function defines the RTP address of the remote
               peer or of the multicast group. */
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
#if (RTP_TYPE == RTP_TYPE_NEW)
            {
                RvNetAddress netAddr;
                RvNetIpv4 netV4;
                memset(&netAddr, 0, sizeof(netAddr));
                memset(&netV4, 0, sizeof(netV4));
                netV4.port = addr.port;
                netV4.ip = addr.addr.v4.ip;
                RvNetCreateIpv4(&netAddr, &netV4);
                RvRtcpSetRemoteAddress(hRtcp, &netAddr);
            }
#else
            rtcpSetRemoteAddress(hRtcp, addr.addr.v4.ip, addr.port);
#endif
#else
            rtcpSetRemoteAddress(hRtcp, addr.ip, addr.port);
#endif
        }

#if (RTP_TYPE == RTP_TYPE_NEW)
        initBuffer(buffer, RvRtpPCMUGetHeaderLength(), payLoadSize);
#else
        initBuffer(buffer, rtpPCMUGetHeaderLength(), payLoadSize);
#endif
        startTime = timerGetTimeInMilliseconds();
        uIDEvent = mtimerSet(hTimers, TimerProc, NULL, 60);
        break;

    /* This callback function is called with the following state
       after the channel is disconnected. The 'cmChannelClose'
       function is called and if the remote side has dropped the
       call, the timer is terminated.*/

    case cmChannelStateIdle:
        if (uIDEvent != HTI_NULL_TIMER)
        {
            mtimerReset(hTimers, uIDEvent);
            uIDEvent = HTI_NULL_TIMER;
        }

        printf("Closing outgoing channel\n");
        cmChannelClose(hsChannel);

        break;

    default:
        break;
    }

    return RV_OK;
}


/****************************************************************
* TimerProc                                                     *
* Purpose : This is a callback function that is notified when   *
*           the time-out value elapses.                         *
*           The setTimer function defines the time-out value    *
*           and receives a pointer to TimerProc.                *
* Input   : context - Timer context.                         *
* Return  : This function does not return a value.              *
****************************************************************/
void RVCALLCONV TimerProc(IN void* context)
{
    /* The structure that contains information needed for creating
       the RTP packet header. */
#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtpParam p;
#else
    rtpParam p;
#endif
    /* Stores the returned value from the rtpWrite function. */
    int error;

    RV_UNUSED_ARG(context);

#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtpParamConstruct(&p);
#endif

    /* RTP timestamp. */
    p.timestamp = abs(timerGetTimeInMilliseconds() - startTime);

    /* This field holds the offset of the first payload octet in the
       RTP packet buffer. 'rtpPCMUGetHeaderLength' determines maximum
       payload header size for payload PCMU. */
#if (RTP_TYPE == RTP_TYPE_NEW)
    p.sByte=RvRtpPCMUGetHeaderLength();
#else
    p.sByte=rtpPCMUGetHeaderLength();
#endif
#if (RTP_TYPE == RTP_TYPE_NEW)
    p.sByte += RvRtpNatMultiplexIdSize(); 
#endif

    /* The state of the marker bit. */
    p.marker = 0;

    /* Adds payload PCMU header to the RTP packet buffer. */
#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtpPCMAPack(buffer, payLoadSize + p.sByte, &p, NULL);
#else
    rtpPCMAPack(buffer, payLoadSize + p.sByte, &p, NULL);
#endif

    /* The following function sends an RTP packet. If no error occurs,
       the function returns a non-negative value. Otherwise it returns
       a negative value. */
#if (RTP_TYPE == RTP_TYPE_NEW)
    error = RvRtpWrite(hRtp, buffer, sizeof(buffer), &p);
#else
    error = rtpWrite(hRtp, buffer, sizeof(buffer), &p);
#endif


    if (error<0)
        printf("Error in rtpWrite. sent only %d packets\n", sentPackets);
    else
    {
        sentPackets++;
        printf("Packet %d was sent\n", sentPackets);
    }

}


/* FUNCTION CODE */

/****************************************************************
* initBuffer                                                    *
* Purpose : This function initializes the buffer to be sent by  *
*           rtpWrite.                                           *
* Input   : buf         : Buffer containing the RTP packet with *
*                         room before first payload byte for RTP*
*                         header.                               *
*           sByte       : Length of the RTP header.             *
*           payLoadSize : Number of bytes in the payLoad        *
*                                                               *
* Return  : This function does not return a value.              *
****************************************************************/

void initBuffer(char *buf, RvInt32 sByte, int payLoadSize)
{
    int i,j;

    for(i = sByte; i < payLoadSize+sByte; i += 48)
        for(j=0;j<48;j++)
            buf[sByte + i + j] = (char)(j*3);
}


int AppMain(int argc, char* argv[])
{

    /* Boolean indicator for making a single call. */
    RvUint call= 0;

    /* RTP and RTCP port numbers. */
    RvUint16 rtpPort,rtcpPort;

    /* An array for storing the IP address input by the user. */
    char dest[50];

    /* Unique name representing the source of the RTP data. */
    char *name = "send";

    /* The structures that contain pointers to the callback functions. */
    SCMCALLEVENT cmCallEvent;
    SCMCHANEVENT cmChannelEvent;

    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    /* Initialize the structures that contain pointers to the callback functions. */
    memset(&cmCallEvent, 0, sizeof(SCMCALLEVENT));
    memset(&cmChannelEvent, 0, sizeof(SCMCHANEVENT));
    cmCallEvent.cmEvCallStateChanged = cmEvCallStateChanged;
    cmChannelEvent.cmEvChannelStateChanged = cmEvChannelStateChanged;

    /* Initializes the Stack according to the EPconfig.val file. */
    if (cmInitialize(GetFilename("EPConfig.val"),&hApp) < 0 )
        return RV_ERROR_UNKNOWN;

    /* Initializes the RTP & RTCP modules. */
#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtpInit();
    RvRtcpInit();
#else
    rtpInit();
    rtcpInit();
#endif

    /* Sets the calls event handler function. */
    cmSetCallEventHandler(hApp,&cmCallEvent,sizeof(cmCallEvent));

    /* Sets the channels event handler function. */
    cmSetChannelEventHandler(hApp,&cmChannelEvent,sizeof(cmChannelEvent));

    /* start the timer manager */
    hTimers = mtimerInit(1, NULL);

    /* The following instructions open RTP & RTCP sessions. An
       arbitrary port (even) number is used for the RTP session and
       the RTCP port (odd) number is the next in sequence. */
#if (RTP_TYPE == RTP_TYPE_NEW)
    {
        RvNetAddress        addr;
        RvNetIpv4           v4addr;
        cmTransportAddress  sourceAddr;

        memset(&sourceAddr, 0, sizeof(sourceAddr));
        memset(&v4addr, 0, sizeof(v4addr));
        memset(&addr, 0, sizeof(addr));

        /*get the ip address of the stack*/
        cmGetLocalCallSignalAddress(hApp, &sourceAddr);
        v4addr.ip = sourceAddr.addr.v4.ip;
        RvNetCreateIpv4(&addr, &v4addr);

        hRtp = RvRtpOpen(&addr, 116, 220);
    }
    rtpPort = RvRtpGetPort(hRtp);
    printf("rtp port is %d\n", rtpPort);

    {
        RvNetAddress netAddr;
        RvNetIpv4 netV4;
        memset(&netAddr, 0, sizeof(netAddr));
        memset(&netV4, 0, sizeof(netV4));
        netV4.port = (RvUint16)(rtpPort+1);
        RvNetCreateIpv4(&netAddr, &netV4);
        hRtcp = RvRtcpOpen(RvRtpGetSSRC(hRtp), &netAddr, name);
        rtcpPort = RvRtcpGetPort(hRtcp);
    }
#else
    hRtp = rtpOpen(0, 116, 220);
    rtpPort = rtpGetPort(hRtp);
    printf("rtp port is %d\n", rtpPort);

    hRtcp = rtcpOpen(rtpGetSSRC(hRtp), (RvUint16)(rtpPort+1), name);
    rtcpPort = rtcpGetPort(hRtcp);
#endif
    printf("rtcp port is %d\n",rtcpPort);

    /* Copies the IP address input by the user in 'dest'. */
    printf ("Enter address and port: (for example: TA:111.11.1.11:1111)\n");
    fgets(dest, sizeof(dest), stdin);
    /* remove the '\n' from the end */
    dest[strlen(dest)-1] = 0;

    /* Sets the inputHandler function as the callback function whenever an event occurs. */
    setKeyboardInput(inputHandler);

    /* This loop is responsible for activating the process that selects
       all registered file descriptors and calls the appropriate callback
       function when an event occurs.
       The application creates a single call to the IP in 'dest'.
       The loop stops when 'stop'=true. This is performed by the callback
       function after a call is connected and RTP packets are sent.
       The call is then disconnected and the user prompts the application
       to terminate.*/
    while(!stop)
    {
        if (!stop && !call)
        {
            call = 1;
            cmCallNew(hApp,NULL,&hsCall);
            printf("Making a call to %s\n", dest);
            cmCallMake(hsCall,64000,0,dest,NULL,dest,NULL,0);
        }

        /* When the outgoing channel is still connected, and max packets have
           been sent, the KillTimer function is called and the channel is
           dropped. The KillTimer function destroys the specified timer. */
        if (sentPackets == MAX_PACKETS)
        {
            sentPackets = 0;
            mtimerReset(hTimers, uIDEvent);
            uIDEvent = HTI_NULL_TIMER;

            printf("Dropping outgoing channel\n");
            cmChannelDrop(hsOutChan);
        }

        seliSelect();
    }

    /* Closes RTP & RTCP sessions. */
#if (RTP_TYPE == RTP_TYPE_NEW)
    if(hRtp)
        RvRtpClose(hRtp);
    if(hRtcp)
        RvRtcpClose(hRtcp);
#else
    if(hRtp)
        rtpClose(hRtp);
    if(hRtcp)
        rtcpClose(hRtcp);
#endif

    /* Shuts down RTP & RTCP modules. */
#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtpEnd();
    RvRtcpEnd();
#else
    rtpEnd();
    rtcpEnd();
#endif

    /* Shuts down the timer manager */
    mtimerEnd(hTimers);

    /* Shuts down the H.323 Stack. */
    cmEnd(hApp);
    return 0;
}




const char* myValFile0[] = /* SOURCE = ./EPConfig.val */ {"EPConfig.val",
"# RADVISION H.323 Stack Configuration File",
"#",
"# Value encodings:",
"#    ''   -  String (and asciiz is not appended)",
"#    \"\"   -  BMP string of ASCII charactes",
"#    []   -  Hex octet string",
"#    <>   -  IP",
"#    {}   -  Object ID",
"#    %   -  Bit string",
"#   Other -  Integer",
"",
"",
"",
"1 h450system = 0",
"+  maxCalls = 10",
"   maxServices = 100",
"",
"",
"1 system = 0",
"+  maxCalls = 2",
"   maxChannels = 4",
"",
"",
"1 RAS = 0",
"+  responseTimeOut = 4",
"   manualRAS = 1",
"   maxFail = 1",
"   allowCallsWhenNonReg = 0",
"   manualRegistration = 0",
"   manualDiscovery = 0",
"+   defaultGatekeeper = 0",
"+    ipAddress = 0",
"+     ip = [7f000001]",
"      port = 1719",
"",
"2  registrationInfo = 0",
"+   terminalType = 0",
"+    vendor = 0",
"+     vendor = 0",
"+      t35CountryCode = 11",
"       t35Extension = 11",
"       manufacturerCode = 11",
"-     productId = 'Test application'",
"      versionId = 'RADVISION'",
"-    terminal = 0",
"     mc = 0",
"     undefinedNode = 0",
"-   terminalAlias = 0",
"+    * = 0",
"+     h323-ID = \"TEST\"",
"",
"2  rasMulticastAddress = 0",
"+   ipAddress = 0",
"+    ip = [e0000129]",
"     port = 1718",
"2  rasPort = 0",
"   compare15bitRasCrv = 1",
"   maxMulticastTTL = 10",
"",
"",
"1 Q931 = 0",
"+  responseTimeOut = 10",
"   connectTimeOut = 200",
"   callSignalingPort = 0",
"   maxCalls = 2",
"   manualCallProceeding = 0",
"   manualAccept = 0",
"",
"1 h245 = 0",
"+  masterSlave = 0",
"+   terminalType = 50",
"    timeout = 100",
"-  capabilities = 0",
"+   terminalCapabilitySet = 0",
"+    sequenceNumber = 0",
"     protocolIdentifier = [00]",
"     multiplexCapability = 0",
"+     h2250Capability = 0",
"+      maximumAudioDelayJitter = 60",
"       receiveMultipointCapability = 0",
"+       multicastCapability = 0",
"        multiUniCastConference = 0",
"        mediaDistributionCapability = 0",
"+        * = 0",
"+         centralizedControl = 0",
"          distributedControl = 0",
"          centralizedAudio = 0",
"          distributedAudio = 0",
"          centralizedVideo = 0",
"          distributedVideo = 0",
"",
"6      transmitMultipointCapability = 0",
"+       multicastCapability = 0",
"        multiUniCastConference = 0",
"        mediaDistributionCapability = 0",
"+        * = 0",
"+         centralizedControl = 0",
"          distributedControl = 0",
"          centralizedAudio = 0",
"          distributedAudio = 0",
"          centralizedVideo = 0",
"          distributedVideo = 0",
"",
"6      receiveAndTransmitMultipointCapability = 0",
"+       multicastCapability = 0",
"        multiUniCastConference = 0",
"        mediaDistributionCapability = 0",
"+        * = 0",
"+         centralizedControl = 0",
"          distributedControl = 0",
"          centralizedAudio = 0",
"          distributedAudio = 0",
"          centralizedVideo = 0",
"          distributedVideo = 0",
"",
"6      mcCapability = 0",
"+       centralizedConferenceMC = 0",
"        decentralizedConferenceMC = 0",
"-      rtcpVideoControlCapability = 0",
"       mediaPacketizationCapability = 0",
"+       h261aVideoPacketization = 0",
"",
"4    capabilityTable = 0",
"+     * = 0",
"+      capabilityTableEntryNumber = 7111",
"       capability = 0",
"+       receiveAudioCapability = 0",
"+        g711Ulaw64k = 60",
"",
"5     * = 0",
"+      capabilityTableEntryNumber = 7110",
"       capability = 0",
"+       receiveAudioCapability = 0",
"+        g711Alaw64k = 60",
"",
"5     * = 0",
"+      capabilityTableEntryNumber = 728",
"       capability = 0",
"+       receiveAudioCapability = 0",
"+        g728 = 60",
"",
"5     * = 0",
"+      capabilityTableEntryNumber = 261",
"       capability = 0",
"+       receiveVideoCapability = 0",
"+        h261VideoCapability = 0",
"+         qcifMPI = 1",
"          cifMPI = 1",
"          temporalSpatialTradeOffCapability = 0",
"          maxBitRate = 600",
"          stillImageTransmission = 0",
"",
"5     * = 0",
"+      capabilityTableEntryNumber = 263",
"       capability = 0",
"+       receiveVideoCapability = 0",
"+        h263VideoCapability = 0",
"+         sqcifMPI = 1",
"          qcifMPI = 1",
"          cifMPI = 1",
"          maxBitRate = 1000",
"          unrestrictedVector = 0",
"          arithmeticCoding = 0",
"          advancedPrediction = 0",
"          pbFrames = 0",
"          temporalSpatialTradeOffCapability = 0",
"          errorCompensation = 0",
"",
"5     * = 0",
"+      capabilityTableEntryNumber = 120",
"       capability = 0",
"+       receiveAndTransmitDataApplicationCapability = 0",
"+        application = 0",
"+         t120 = 0",
"+          separateLANStack = 0",
"8        maxBitRate = 1000",
"",
"4    capabilityDescriptors = 0",
"+     * = 0",
"+      capabilityDescriptorNumber = 0",
"       simultaneousCapabilities = 0",
"+       * = 0",
"+        * = 7111",
"         * = 7110",
"         * = 728",
"-       * = 0",
"+        * = 261",
"         * = 263",
"-       * = 0",
"+        * = 120",
"",
"3   timeout = 100",
"-  channels = 0",
"+   * = 0",
"+    name = 'g711Alaw64k'",
"     dataType = 0",
"+     audioData = 0",
"+      g711Alaw64k = 60",
"",
"3   * = 0",
"+    name = 'g711Ulaw64k'",
"     dataType = 0",
"+     audioData = 0",
"+      g711Ulaw64k = 60",
"",
"3   * = 0",
"+    name = 'g728'",
"     dataType = 0",
"+     audioData = 0",
"+      g728 = 60",
"",
"3   * = 0",
"+    name = 'h261VideoCapability'",
"     dataType = 0",
"+     videoData = 0",
"+      h261VideoCapability = 0",
"+       qcifMPI = 1",
"        cifMPI = 1",
"        temporalSpatialTradeOffCapability = 0",
"        maxBitRate = 600",
"        stillImageTransmission = 0",
"",
"3   * = 0",
"+    name = 'h263VideoCapability'",
"     dataType = 0",
"+     videoData = 0",
"+      h263VideoCapability = 0",
"+       sqcifMPI = 1",
"        qcifMPI = 1",
"        cifMPI = 1",
"        maxBitRate = 1000",
"        unrestrictedVector = 0",
"        arithmeticCoding = 0",
"        advancedPrediction = 0",
"        pbFrames = 0",
"        temporalSpatialTradeOffCapability = 0",
"        errorCompensation = 0",
"",
"3   * = 0",
"+    name = 't120'",
"     dataType = 0",
"+     data = 0",
"+      application = 0",
"+       t120 = 0",
"+        separateLANStack = 0",
"6      maxBitRate = 1000",
"",
NULL};
