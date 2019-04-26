/**************************************************************
 *                        Fast Start Receive.c                *
 *                                                            *
 *                                                            *
 * The following program is a simple console application that *
 * demonstrates how to implement Fast Start call receiving    *
 * using the RADVISION H.323 Protocol Stack.                  *
 * The program does the following:                            *
 *                                                            *
 * 1. Initializes the H.323 Stack.                            *
 * 2. Gets the host IP address and prints it on the screen    *
 *    for the use of the sending side.                        *
 * 3. Installs the H.323 Stack call, channels and Fast Start  *
 *    callbacks.                                              *
 * 4. Waits for an incoming setup message that includes the   *
 *    Fast Start proposal, and when it arrives, acknowledges  *
 *    the channels.                                           *
 * 5. When channels are ready, the application prompts the    *
 *    user to answer the call.                                *
 * 6. When the Fast Start call is connected, gives the        *
 *    receiving side the option to drop the call.             *                                                     *
 **************************************************************/

#include "../osDependency.h"
#include <seli.h>
#include <cm.h>

/* The application handle */
HAPP hApp;

/* Used for storing the station IP address. */
RvChar my_ip[64];

/* Boolean indicator for terminating the application. */
static RvBool stop = RV_FALSE;

/* Stores the call state. */
cmCallState_e newCallState;

/* The Stack handle to the call. */
HCALL hsNewCall;

/* The number of open channels. */
static int channels = 0;

/***************************************************************
 * inputHandler                                                 *
 * Purpose    : This is a callback function that is called      *
 *              whenever the user presses a key on the keyboard.*
 *              The function decides how to proceed according   *
 *              to the call state.                              *
 *              The key that is pressed is usually a response   *
 *              to a prompt for the user in the early stages of *
 *              the program.                                    *
 * Input      : fd       : The file descriptor that you want    *
 *                         to register with seli.               *
 *              seliEvent: The event that called the callback   *
 *                         function.                            *
 *              error    : If an error occurs, the value is 1.  *
 *                         If no error occurs, the value        *
 *                         remains 0.                           *
 ****************************************************************/
static void RVCALLCONV inputHandler(int fd, seliEvents sEvent, RvBool error)
{
    int answer;

    RV_UNUSED_ARG(fd);
    RV_UNUSED_ARG(sEvent);
    RV_UNUSED_ARG(error);

    /* Reads the user input */
    answer = getchar();

    /* If the answer is positive: */
    if (answer == 'y')
    {
        switch(newCallState)
        {
        case cmCallStateOffering:
            /* The user answers the call. */
            printf("Answering the call\n");
            cmCallAnswer(hsNewCall);
            break;

        case cmCallStateConnected:
            /* The user drops the call. */
            printf("Dropping the call.\n");
            cmCallDrop(hsNewCall);
            break;

        case cmCallStateIdle:
            /* The user closes the window and disconnects the Q.931 channel.
               When stop == RV_TRUE, the loop in the main function stops running. */
            printf("Bye-bye\n");
            stop = RV_TRUE;
            break;
        default:
            break;
        }
    }
    /* If the answer is negative: */
    if (answer == 'n')
    {
        if (newCallState == cmCallStateOffering)
        {
            /* The user does not take the call and the application drops it. */
            printf("I do not want to take the call\n");
            cmCallDrop(hsNewCall);
        }
    }

    if (stop)
        endKeyboardInput();
}

/*******************************************************************
 * cmEvCallStateChanged                                            *
 * Purpose : This is the callback function used by the Stack to    *
 *           report a change in the status of a call to the        *
 *           application.                                          *
 * Input   : haCall   :The application handle for the call.        *
 *           hsCall   :The Stack handle for the call.              *
 *           state    :The new state of the call.                  *
 *           stateMode:New state mode. This parameter describes    *
 *                     how or why the call enters a new state.     *
 * Return  : If an error occurs, the function returns a negative   *
 *           value. If no error occurs, the function returns a     *
 *           non-negative value.                                   *
 *******************************************************************/
static int RVCALLCONV cmEvCallStateChanged(IN   HAPPCALL           haCall,
                                           IN   HCALL              hsCall,
                                           IN   cmCallState_e      state,
                                           IN   cmCallStateMode_e  stateMode)
{
    RV_UNUSED_ARG(haCall);

    /* The global variable for the Stack handle. */
    hsNewCall = hsCall;

    switch(state)
    {
    /* This callback function is called with the following state when
       the Stack receives a call offering. The user is prompted to
       answer the call and the response is read by the inputHandler
       function. */
    case cmCallStateOffering:
        newCallState = cmCallStateOffering;
        break;

   /* This callback function is called with the following state
      when the Fast Start call and all channels are connected.
      The user is then prompted to drop the call. */
    case cmCallStateConnected:
        newCallState = cmCallStateConnected;
        if (stateMode==cmCallStateModeCallSetupConnected)
        {
            printf("The Fast Start call is connected\n");
            printf("If you want to drop the call, enter 'y'\n");
        }
        break;

    /* This callback function is called with the following state after
       the call is disconnected. The 'cmCallClose' function is called and then the user is prompted
       to close the application */
    case cmCallStateIdle:
        newCallState = cmCallStateIdle;
        cmCallClose(hsCall);
        printf("The Fast Start call is closed\n");
        printf("If you want to close the window, enter 'y'\n");
        break;

    default:
        break;
    }

    return RV_OK;
}

/*****************************************************************
 * cmEvChannelStateChanged                                       *
 * Purpose : This is the callback function used by the Stack to  *
 *           report a change in channel status to the            *
 *           application.                                        *
 * Input   : haChan :Application handle for the channel.         *
 *        hsChan :Stack handle for the channel.                  *
 *           state  :New state of the channel.                   *
 *           stateMode :New state mode. This parameter describes *
 *                      how or why the channel enters a new      *
 *                      state. See Remarks for an explanation    *
 *                      of States and Mode.                      *
 * Return  : If an error occurs, the function returns a negative *
 *           value. If no error occurs, the function returns a   *
 *           non-negative value.                                 *
 *****************************************************************/
static int RVCALLCONV cmEvChannelStateChanged(IN  HAPPCHAN             haChannel,
                                              IN  HCHAN                hsChannel,
                                              IN  cmChannelState_e     state,
                                              IN  cmChannelStateMode_e stateMode)
{
    RV_UNUSED_ARG(stateMode);
    RV_UNUSED_ARG(haChannel);

    switch(state)
    {
    /* This callback function is called with the following state when
       each channel is connected (after the call offering and before
       the Fast Start call is connected). The application counts the
       open channels and when there are 2 (outgoing and incoming), it
       prompts the user to accept the call. */
    case cmChannelStateConnected:
        channels++;
        if (channels == 2)
        {
            printf("Incoming and outgoing Fast Start channels are connected\n");
            printf("If you want to answer the call, enter 'y'\n");
        }
        break;

    /* This callback function is called with the following state after
       each channel is disconnected. The 'cmChannelClose' function is
       then called. */
    case cmChannelStateIdle:
        cmChannelClose(hsChannel);
        channels--;
        if(!channels)
            printf("Fast Start channels are closed\n");
        break;

    default:
        break;
    }

    return RV_OK;
}


/*****************************************************************
 * cmEvCallFastStartSetup                                        *
 * Purpose : This is the callback function used by the Stack to  *
 *           report Fast Start channel proposals to the          *
 *           application .                                       *
 * Input   : haCall: Application handle for the call.            *
 *           hsCall: Stack handle for the call.                  *
 *           fsMessage: A list of channels proposed by the       *
 *                      remote party.                            *
 * Return  : If an error occurs, the function should return a    *
 *           negative value. If no error occurs, the function    *
 *           should return a non-negative value.                 *
 *****************************************************************/
int RVCALLCONV cmEvCallFastStart(HAPPCALL  haCall,
                                 HCALL     hsCall)
{
    int                         i;
    cmTransportAddress          ownCSAddr;
    int                         channelId = 0;
    int                         sessionId;
    cmChannelDirection          chanDir;
    cmFastStartChannel          fsChan;
    /* this array will help us aser just one channel of each session Id */
    RvBool                      channels[512];

    RV_UNUSED_ARG(haCall);
    memset(channels, 0, sizeof(channels));

    printf("Receiving a Fast Start call\n");

    cmGetLocalCallSignalAddress(hApp, &ownCSAddr);

    for ( i=0; channelId >= 0; i++)
    {
        RvPvtNodeId ackChan;

        /* get a proposed channel */
        channelId = cmFastStartGetByIndex(hsCall, i);
        if(channelId < 0) break;
        /* get its parameters */
        cmFastStartGet(hsCall, channelId, &sessionId, &chanDir, &fsChan);

        /* check is we answered an alternate channel of this session ID already */
        if (channels[sessionId + ((chanDir==dirTransmit)?256:0)] == RV_FALSE)
        {
            /* set own IP or zeros as the RTP and RTCP addresses. we will
               set the real addresses later, in cmEvchannelSetAddress*/
            memcpy(&fsChan.rtcp, &ownCSAddr, sizeof(fsChan.rtcp));
            fsChan.rtcp.port = 2327;
            if (chanDir == dirReceive)
            {
                memcpy(&fsChan.rtp, &ownCSAddr, sizeof(fsChan.rtp));
                fsChan.rtp.port = 2326;
            }
            else
                memset(&fsChan.rtp, 0, sizeof(fsChan.rtp));

            ackChan = cmFastStartChannelsAckIndex(hsCall, i, &fsChan.rtcp, &fsChan.rtp);
            printf("Fast Start channel %d acknowledged\n", i);

            /* mark this SID as answered, so we do not answer alternate channels */
            channels[sessionId + ((chanDir==dirTransmit)?256:0)] = RV_TRUE;
            if (chanDir == dirBoth) channels[sessionId + 256] = RV_TRUE;
        }
    }

    /* The application calls this function when it finishes acknowledging
       all the channels it requires. */
    printf("Fast Start channels are ready\n");
    cmFastStartChannelsReady(hsCall);

    return RV_OK;
}


int AppMain(int argc, char* argv[])
{
    /* The structure which stores IP address and port number.*/
    cmTransportAddress tr;
    /* The structures that contain pointers to the callback functions. */
    SCMCALLEVENT cmCallEvent;
    SCMCHANEVENT cmChannelEvent;

    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    /* Initialize the structures that contain pointers to the callback functions. */
    memset(&cmCallEvent, 0, sizeof(SCMCALLEVENT));
    memset(&cmChannelEvent, 0, sizeof(SCMCHANEVENT));
    cmCallEvent.cmEvCallStateChanged = cmEvCallStateChanged;
    cmCallEvent.cmEvCallFastStart = cmEvCallFastStart;
    cmChannelEvent.cmEvChannelStateChanged = cmEvChannelStateChanged;

    /* Initializes the Stack according to the EPconfig.val file. */
    if (cmInitialize(GetFilename("EPConfig.val"),&hApp) < 0 )
        return RV_ERROR_UNKNOWN;

    /* Sets the inputHandler function as the callback function whenever an event occurs. */
    setKeyboardInput(inputHandler);

    /* Set the calls event handler functions. */
    cmSetCallEventHandler(hApp,&cmCallEvent,sizeof(cmCallEvent));

    /* Set the channels event handler functions. */
    cmSetChannelEventHandler(hApp,&cmChannelEvent,sizeof(cmChannelEvent));

    /* Gets the host IP address and prints it on the screen. */
    cmGetLocalCallSignalAddress(hApp,&tr);
    cmTransportAddress2String(&tr, sizeof(my_ip), my_ip);

    printf("My address is: TA:%s\n", my_ip);

    /* This loop is responsible for activating the process that
       selects all registered file descriptors and calls the
       appropriate callback function when an event occurs. The loop
       stops when 'Stop'=1. This is performed by the callback
       function after a call has been connected and then disconnected. The
       user then prompts the application to terminate. */
    while(!stop)
    {
        seliSelect();
    }

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
