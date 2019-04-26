/****************************************************************
 *                           RTP Receive.c                      *
 *                                                              *
 * The following program is a simple console application that   *
 * demonstrates how to implement receiving RTP packets using    *
 * the RTP & RTCP protocols and the RADVISION H.323 Protocol    *
 * Stack.                                                       *
 * The program does the following:                              *
 *                                                              *
 * 1. Initializes the H.323 Stack.                              *
 * 2. Installs the H.323 Stack call and channel callbacks.      *
 * 3. Initializes the RTP and RTCP modules.                     *
 * 4. Opens RTP and RTCP sessions.                              *
 * 5. Gets the host IP address and displays it on the screen    *
 *    for the sending side.                                     *
 * 6. Waits for an incoming call and when it arrives, prompts   *
 *    the user for an answer.                                   *
 * 7. When the call is connected, accepts the remote side       *
 *    request to open an incoming channel.                      *
 * 8. When the channel is connected, receives the specified     *
 *    number of RTP packets from the remote side.               *
 * 9. After the packets are received, the remote side closes    *
 *    the channel and the user is given the option to drop the  *
 *    call.                                                     *
 ***************************************************************/


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
#include <mti.h>



/* GLOBALS */


/* The application handle. */
HAPP hApp;

/* The Stack handle to the call. */
HCALL hsReceivedCall;

/* Handle of the RTP session. */
#if (RTP_TYPE == RTP_TYPE_NEW)
RvRtpSession hRtp;
#else
HRTPSESSION hRtp;
#endif

/* Handle of the RTCP session. */
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

/* A callback function is called each time a new RTP packet
   arrives at the RTP session. */
#if (RTP_TYPE == RTP_TYPE_NEW)
void NewMessage(RvRtpSession hRTP, void* context);
#else
void NewMessage(HRTPSESSION hRTP, void* context);
#endif



/* CALLBACK CODES */

/****************************************************************
 * inputHandler                                                 *
 * Purpose    : This is a callback function that is called      *
 *              whenever the user presses a key on the keyboard.*
 *              The function decides how to proceed according   *
 *              to the call state.                              *
 *              The key that was pressed is usually a response  *
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
static void RVCALLCONV inputHandler(int        fd,
                                    seliEvents sEvent,
                                    RvBool     error)
{
    int answer;

    RV_UNUSED_ARG(fd);
    RV_UNUSED_ARG(sEvent);
    RV_UNUSED_ARG(error);

    /* Reads the user input. */
    answer = getchar();

    /* If the answer is positive: */
    if (answer == 'y')
    {
        switch(callState)
        {
        case cmCallStateOffering:
            /* The user answers the call. */
            printf("Answering the call\n");
            cmCallAnswer(hsReceivedCall);
            break;

        case cmCallStateConnected:
            /* The user drops the call. */
            printf("Dropping the call.\n");
            cmCallDrop(hsReceivedCall);
            break;

        case cmCallStateIdle:
            /* The user closes the window and disconnects the Q.931 channel.
                When stop!=0, the loop in the main function stops running.*/
            printf("Bye-bye\n");
            stop = RV_TRUE;
            break;

        default:
            break;
        }
    }

    /* If the answer is not positive: */
    if (answer == 'n')
    {
        /* The user does not take the call and the application drops it. */
        if (callState == cmCallStateOffering)
        {
            printf("I do not want to take the call\n");
            cmCallDrop(hsReceivedCall);
        }
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

    /* The global variable for the Stack handle. */
    hsReceivedCall = hsCall;

    switch(state)
    {

    /* This callback function is called with the following state when
       the Stack receives a call offering. The user is prompted to
       answer the call. The user response is read by the
       'inputHandler' function. */
    case cmCallStateOffering:
        callState = cmCallStateOffering;
        printf("Recieving a call offering\n");
        printf("If you want to answer, enter 'y'.\n");
        break;

    /* This callback function is called with the following state when
       the call is fully connected (both H.245 and Q.931 call
       establishment processes are completed). In this case the remote
       side opens an audio channel. */
    case cmCallStateConnected:
        if (stateMode==cmCallStateModeConnectedCall)
        {
            callState = cmCallStateConnected;
            printf("The call is connected.\n");
            printf("For disconnecting enter 'y' anytime\n");

        }
        break;

     /* This callback function is called with the following state
        after the call is disconnected. The 'cmCallClose' function is
        called and the user is prompted to close the application. */
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
    RV_UNUSED_ARG(haChannel);
    RV_UNUSED_ARG(stateMode);

    switch(state)
    {
        /* This callback function is called with the following state when
           the Stack receives a channel offering.
           The application specifies the address to be used as the RTP or
           data address for the incoming stream.
           The application also specifies the address to be used as the RTCP
           address for an incoming channel and then answers the channel. */
        case cmChannelStateOffering:
        {
            cmTransportAddress tr;

            printf("Receiving a channel offering.\n");
            cmGetLocalCallSignalAddress (hApp, &tr);

#if (RTP_TYPE == RTP_TYPE_NEW)
            cmChannelSetAddress(hsChannel, 0, RvRtpGetPort(hRtp));
            cmChannelSetRTCPAddress(hsChannel, 0, RvRtcpGetPort(hRtcp));
#else
            cmChannelSetAddress(hsChannel, 0, rtpGetPort(hRtp));
            cmChannelSetRTCPAddress(hsChannel, 0, rtcpGetPort(hRtcp));
#endif

            printf("Answering an incoming channel.\n");
            cmChannelAnswer(hsChannel);
        }
        break;

        /* This callback function is called with the following state when
           the channel is connected. The application then installs an
           event handler for the RTP session.*/
        case cmChannelStateConnected:
        {
            printf("Channel is connected\n");

            /* The number of milliseconds that have elapsed since the system
               started. */
            startTime = timerGetTimeInMilliseconds();

#if (RTP_TYPE == RTP_TYPE_NEW)
            RvRtpSetEventHandler(hRtp, NewMessage, NULL);
#else
            rtpSetEventHandler(hRtp, NewMessage, NULL);
#endif
        }
        break;

        /* This callback function is called with the following state
           after the channel is disconnected. The 'cmChannelClose' function
           is then called. */
        case cmChannelStateIdle:
        {
            printf("Closing incoming channel\n");
            cmChannelClose(hsChannel);

        }
        break;

        default:
            break;
    }
    return RV_OK;
}


/****************************************************************
 * cmEvChannelSetAddress                                        *
 * Purpose : This is the callback function used by the Stack to *
 *           specify the address to which RTP packets should be *
 *           sent.                                              *
 * Input   : haChan :Application handle for the channel.        *
 *           hsChan :Stack handle for the channel.              *
 *           ip     :IP address to which RTP packets should be  *
 *                   sent.                                      *
 *           port   :Port number to which RTP packets should be *
 *                   sent.                                      *
 *                                                              *
 * Return  : If an error occurs, the function returns a negative*
 *           value.                                             *
 *           If no error occurs, the function returns a         *
 *           non-negative value.                                *
 ***************************************************************/
static int RVCALLCONV cmEvChannelSetAddress(IN  HAPPCHAN   haChan,
                                            IN  HCHAN      hsChan,
                                            IN  RvUint32   ip,
                                            IN  RvUint16   port)
{
    RV_UNUSED_ARG(haChan);
    RV_UNUSED_ARG(hsChan);

    printf("setting up RTP address.\n");
#if (RTP_TYPE == RTP_TYPE_NEW)
    {
        RvNetAddress netAddr;
        RvNetIpv4 netV4;
        memset(&netAddr, 0, sizeof(netAddr));
        memset(&netV4, 0, sizeof(netV4));
        netV4.port = port;
        netV4.ip = ip;
        RvNetCreateIpv4(&netAddr, &netV4);
        RvRtpSetRemoteAddress(hRtp, &netAddr);
    }
#else
    rtpSetRemoteAddress(hRtp, ip, port);
#endif

    return 0;
}


/****************************************************************
 * cmEvChannelSetRTCPAddress                                    *
 * Purpose : This is the callback function used by the Stack to *
 *           specify the address to which RTCP packets should   *
 *           be sent.                                           *
 * Input   : haChan :Application handle for the channel.        *
 *           hsChan :Stack handle for the channel.              *
 *           ip     :IP address to which RTCP packets should be *
 *                   sent.                                      *
 *           port   :Port number to which RTCP packets should   *
 *                   be sent.                                   *
 * Return  : If an error occurs, the function returns a         *
 *           negative value.                                    *
 *           If no error occurs, the function returns a         *
 *           non-negative value.                                *
 ***************************************************************/
static int RVCALLCONV cmEvChannelSetRTCPAddress(IN  HAPPCHAN   haChan,
                                                IN  HCHAN      hsChan,
                                                IN  RvUint32   ip,
                                                IN  RvUint16   port)
{
    RV_UNUSED_ARG(haChan);
    RV_UNUSED_ARG(hsChan);

    printf("Setting up RTCP address.\n");
#if (RTP_TYPE == RTP_TYPE_NEW)
    {
        RvNetAddress netAddr;
        RvNetIpv4 netV4;
        memset(&netAddr, 0, sizeof(netAddr));
        memset(&netV4, 0, sizeof(netV4));
        netV4.port = port;
        netV4.ip = ip;
        RvNetCreateIpv4(&netAddr, &netV4);
        RvRtcpSetRemoteAddress(hRtcp, &netAddr);
    }
#else
    rtcpSetRemoteAddress(hRtcp, ip, port);
#endif

    return 0;
}


/****************************************************************
 * NewMessage                                                   *
 * purpose : This is the callback function that will be called  *
 *           each time a new RTP packet arrives at the RTP      *
 *           session.                                           *
 * input   : hRTP    :Handle of the RTP session.                *
 *       context :Identical to the context parameter of the     *
 *                    rtpSetEventHandler() function (in this    *
 *                    case 'NULL').                             *
 * return  : If an error occurs, the function returns a         *
 *           negative value.                                    *
 *           If no error occurs, the function returns a         *
 *           non-negative value.                                *
 ***************************************************************/
#if (RTP_TYPE == RTP_TYPE_NEW)
void NewMessage(RvRtpSession hRTP, void* context)
#else
void NewMessage(HRTPSESSION hRTP, void* context)
#endif
{
    /* A structure that contains information obtained from the
       decoded RTP packet header. */
#if (RTP_TYPE == RTP_TYPE_NEW)
    RvRtpParam p;
#else
    rtpParam p;
#endif
    /* Stores the returned value from rtpReadEx. */
    RvInt32 error;
    /* Number of RTP packets received by the remote side. */
    static RvUint receivedPackets = 0;
    /* A pointer to a buffer that contains the RTP received packet. */
    char buffer[600];
    /* The local RTP timestamp for the received packet. */
    RvUint32 timestamp = abs(timerGetTimeInMilliseconds() - startTime);

    RV_UNUSED_ARG(context);

    /* This function receives the RTP packet and updates the
       corresponding RTCP session. */
#if (RTP_TYPE == RTP_TYPE_NEW)
    error = RvRtpReadEx(hRTP, buffer, sizeof(buffer), timestamp, &p);
#else
    error = rtpReadEx(hRTP, buffer, sizeof(buffer), timestamp, &p);
#endif
    if (error<0)
        printf("Error in reading\n");
    else
        printf("Read %d bytes from packet #%d\n", p.len, ++receivedPackets);
}


int AppMain(int argc, char* argv[])
{
    /* The structure which stores IP address and port.*/
    cmRASTransport tr;

    /* Rtp and Rtcp port numbers */
    RvUint16 rtpPort,rtcpPort;

    /* Used for storing the station IP address */
    RvChar myIP[64];

    /* Unique name representing the source of the RTP data. */
    RvChar *name = "receive";

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
    cmChannelEvent.cmEvChannelSetAddress = cmEvChannelSetAddress;
    cmChannelEvent.cmEvChannelSetRTCPAddress = cmEvChannelSetRTCPAddress;

    /* Initializes the Stack according to the EPconfig.val file. */
    if (cmInitialize(GetFilename("EPConfig.val"),&hApp) < 0 )
        return RV_ERROR_UNKNOWN;

    /* Sets the inputHandler function as the callback function whenever an event occurs. */
    setKeyboardInput(inputHandler);

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

    /* The following instructions open RTP & RTCP sessions. An
       arbitrary port (even) number is used for the RTP session
       and the RTCP port (odd) number is the following one. */

    printf("Opening rtp rtcp channels...\n");
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
    printf("Rtp port is %d.\n", rtpPort);

#if (RTP_TYPE == RTP_TYPE_NEW)
    {
        RvNetAddress netAddr;
        RvNetIpv4 netV4;
        memset(&netAddr, 0, sizeof(netAddr));
        memset(&netV4, 0, sizeof(netV4));
        netV4.port = (RvUint16)(rtpPort+1);
        RvNetCreateIpv4(&netAddr, &netV4);
        hRtcp = RvRtcpOpen(RvRtpGetSSRC(hRtp), &netAddr, name);
    }
#else
    hRtcp = RvRtcpOpen(RvRtpGetSSRC(hRtp), (RvUint16)(rtpPort+1), name);
#endif
    rtcpPort = RvRtcpGetPort(hRtcp);
#else
    hRtp = rtpOpen(0, 116, 220);
    rtpPort = rtpGetPort(hRtp);
    printf("Rtp port is %d.\n", rtpPort);

    hRtcp = rtcpOpen(rtpGetSSRC(hRtp), (RvUint16)(rtpPort+1), name);
    rtcpPort = rtcpGetPort(hRtcp);
#endif
    printf("Rtcp port is %d.\n",rtcpPort);

    /* Gets the host IP address and displays it on the screen. */
    cmGetLocalCallSignalAddress(hApp,&tr);
    cmTransportAddress2String(&tr, sizeof(myIP), myIP);
    printf("My address is: TA:%s\n", myIP);



    /* This loop is responsible for activating the process that
       selects all registered file descriptors and calls the
       appropriate callback function when an event occurs.
       The loop stops when 'stop' = true. This is performed by the
       callback function after a call is connected, RTP packets are
       received and then the call is disconnected. The user
       then prompts the application to terminate. */

    while(!stop)
       seliSelect();

    /* Closes RTP & RTCP sessions. */
#if (RTP_TYPE == RTP_TYPE_NEW)
    if(hRtp)
        RvRtpClose(hRtp);
    if(hRtcp)
        RvRtcpClose(hRtcp);

    /* Shuts down RTP & RTCP modules. */
    RvRtpEnd();
    RvRtcpEnd();
#else
    if(hRtp)
        rtpClose(hRtp);
    if(hRtcp)
        rtcpClose(hRtcp);

    /* Shuts down RTP & RTCP modules. */
    rtpEnd();
    rtcpEnd();
#endif

    /*Shuts down the H.323 Stack. */
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
