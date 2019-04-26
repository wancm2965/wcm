/****************************************************************
 *                          Fast Start Send.c                   *
 *                                                              *
 * The following program is a simple console application that   *
 * demonstrates how to implement Fast Start call sending using  *
 * the RADVISION H.323 Protocol Stack.                          *
 * The program does the following:                              *
 *                                                              *
 * 1. Initializes the H.323 Stack.                              *
 * 2. Prompts the user for the IP address of the receiving      *
 *    side.                                                     *
 * 3. Installs the H.323 Stack call, channels and Fast Start    *
 *    callbacks.                                                *
 * 4. Makes a single Fast Start call.                           *
 * 5. Waits for a response from the receiving side.             *
 * 6. When both Fast Start call and channels are connected,     *
 *    gives the sending side the option to drop the call.       *
 *                                                              *
 ****************************************************************/

#include "../osDependency.h"
#include <seli.h>
#include <cm.h>

/* The application handle */
HAPP hApp;

/* Used for storing the station IP address. */
char dest[64];

/* Boolean indicator for terminating the application. */
static RvBool stop = RV_FALSE;

/* Stores the call state. */
RvUint32 newCallState;

/* The Stack handle to the call. */
HCALL hsNewCall;

/* The number of open channels. */
static int channels = 0;

/****************************************************************
 * inputHandler                                                 *
 * Purpose    : This is a callback function that is called      *
 *              whenever the user presses a key on the keyboard.*
 *              The function decides how to proceed according   *
 *              to the call state .                             *
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
        /* The user drops the call. */
        case cmCallStateConnected:
            {
                printf("Dropping the call.\n");
                cmCallDrop(hsNewCall);
            }
            break;

        case cmCallStateIdle:
            {
                /* The user closes the window and disconnects the Q.931 channel.
                   When stop!=0 the loop in the main function stops running */
                printf("Bye-bye\n");
                stop = RV_TRUE;
            }
            break;
        default:
            break;
        }
    }

    if (stop)
        endKeyboardInput();
}

/*****************************************************************
 * InitFastStart                                                 *
 * Purpose: Initializes the structure that stores a list of      *
 *          proposed Fast Start channels, RTP and RTCP           *
 *          addresses. The application opens Fast Start channels *
 * input  : hsCall :The Stack handle for the call.               *
 *****************************************************************/
void InitFastStart(HCALL hsCall)
{
    cmFastStartChannel          fsChannel;
    RvPvtNodeId                 fsChanNode;

    /* Initializes the structure */
    memset(&fsChannel, 0, sizeof(cmFastStartChannel));

    fsChannel.channelName = "g711Alaw64k";
    fsChannel.dataTypeHandle = -1;
    cmGetLocalCallSignalAddress(hApp, &fsChannel.rtp);
    cmGetLocalCallSignalAddress(hApp, &fsChannel.rtcp);
    
    fsChannel.rtp.port = 0;
    fsChannel.rtcp.port = 2327;
    fsChanNode = cmFastStartBuild(hsCall, 1, dirTransmit, &fsChannel);
    if (fsChanNode >= 0)
    {
        if (cmCallAddFastStartMessage(hsCall, fsChanNode) < 0)
        {
            pvtDelete(cmGetValTree(hApp), fsChanNode);
            printf("Unable to set faststart proposals");
            return;
        }
    }

    fsChannel.rtp.port = 2326;
    fsChanNode = cmFastStartBuild(hsCall, 1, dirReceive, &fsChannel);
    if (fsChanNode >= 0)
    {
        if (cmCallAddFastStartMessage(hsCall, fsChanNode) < 0)
        {
            pvtDelete(cmGetValTree(hApp), fsChanNode);
            printf("Unable to set faststart proposals");
            return;
        }
    }
}


/*****************************************************************
 * cmEvCallStateChanged                                          *
 * Purpose : This is the callback function used by the Stack to  *
 *           report a change in the status of a call to the      *
 *           application.                                        *
 * Input   : haCall   :The application handle for the call.      *
 *           hsCall   :The Stack handle for the call.            *
 *           state    :The new state of the call.                *
 *           stateMode:New state mode. This parameter describes  *
 *                     how or why the call enters a new state.   *
 * Return  : If an error occurs, the function returns a          *
 *           negative value.                                     *
 *           If no error occurs, the function returns a          *
 *           non-negative value.                                 *
 *****************************************************************/
static int RVCALLCONV cmEvCallStateChanged(IN  HAPPCALL           haCall,
                                           IN  HCALL              hsCall,
                                           IN  cmCallState_e      state,
                                           IN  cmCallStateMode_e  stateMode)
{
    RV_UNUSED_ARG(haCall);

    switch(state)
    {
    /* This callback function is called with the following state after
       calling cmCallMake. The application sends the remote side a 
       Fast Start proposal that includes all the channels it wants to open */
    case cmCallStateDialtone:
        newCallState = cmCallStateDialtone;
        printf("Init Fast Start\n");
        InitFastStart(hsCall);
        break;

    /* This callback function is called with the following state when
       the Fast Start call and all channels are connected (after the
       remote side has acknowledged all the Fast Start channels).
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
       the call is disconnected. The    function 'cmCallClose' is called.
       and the user is prompted to close the application. */
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
 * purpose : This is the callback function used by the Stack to  *
 *           report a change in channel status to the            *
 *           application.                                        *
 * input   : haChan :Application handle for the channel.         *
 *        hsChan :Stack handle for the channel.                  *
 *           state  :New state of the channel.                   *
 *           stateMode :New state mode. This parameter describes *
 *                      how or why the channel enters a new      *
 *                      state. See Remarks for an explanation of *
 *                      States and Mode.                         *
 * return  : If an error occurs, the function returns a negative *
 *           value.                                              *
 *           If no error occurs, the function returns a          *
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
       each channel is connected (after the remote side has
       acknowledged the channels and before the Fast Start call
       is connected). */
    case cmChannelStateConnected:
        channels++;
        if (channels == 2)
            printf("Incoming and outgoing Fast Start channels are connected\n");
        break;

    /* This callback function is called with the following state after
       each channel is disconnected. The 'cmChannelClose' function is then
       called. */
    case cmChannelStateIdle:
        cmChannelClose(hsChannel);
        channels--;
        if(!channels)
            printf("Fast Start channels are closed.\n");
        break;

    default:
        break;
    }
    return RV_OK;
}





int AppMain(int argc, char* argv[])
{
    /* Boolean indicator for making a call. */
    RvBool call = RV_FALSE;
    /* Structures that contain pointers to the callback functions. */
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

    /* Sets the inputHandler function as the callback function whenever an event occurs. */
    setKeyboardInput(inputHandler);

    /* Copies the IP address the user printed into 'dest'. */
    printf ("Enter address and port: (for example: \"TA:111.11.1.11:1111\")\n");
    fgets(dest, sizeof(dest), stdin);
    /* remove the '\n' from the end */
    dest[strlen(dest)-1] = 0;

    /* Sets the calls event handler function. */
    cmSetCallEventHandler(hApp,&cmCallEvent,sizeof(cmCallEvent));

    /* Sets the channels event handler function. */
    cmSetChannelEventHandler(hApp,&cmChannelEvent,sizeof(cmChannelEvent));

    /* This loop is responsible for activating the process that selects
       all registered file descriptors and calls the appropriate
       callback function when an event occurs. The application creates a
       single Fast Start call to the IP address in 'dest'.
       The loop stops when 'Stop'=1. This is performed by the callback
       function after a call is connected and then disconnected. The
       user then prompts the application to terminate. */

    while(!stop)
    {
        if (!stop && !call)
        {
            call = RV_TRUE;
            cmCallNew(hApp,NULL,&hsNewCall);
            printf("Making a Fast Start call to %s\n", dest);
            cmCallMake(hsNewCall,64000,0,dest,NULL,dest,NULL,0);
        }
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
