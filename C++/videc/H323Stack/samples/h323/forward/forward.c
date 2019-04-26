/*****************************************************************
 *                             Forward.c                         *
 *                                                               *
 * The following program is a simple console application that    *
 * demonstrates how to implement Call Forward using the          *
 * RADVISION H.323 Protocol Stack.                               *
 * The program does the following :                              *
 * 1. Gets the host 'TEL' number and prints it on the screen for *
 *    the use of any calling EP.                                 *
 * 2. Initializes the H.323 Stack.                               *
 * 3. Installs the H.323 Stack call callbacks.                   *
 * 4. Prompts the user for a forwarding address and when forward *
 *    is activated, gives the user the option to deactivate it.  *
 * 5. When forward is deactivated, the application can either    *
 *    receive calls or be terminated by the user.                *
 *****************************************************************/

#include "../osDependency.h"
#include <cm.h>
#include <sse.h>
#include <ctype.h>


/*********************************************
 * A structure containing data for a call:   *
 * hsCall:    The Stack handle for the call. *
 * hSSECall:  The call object handle of SSE. *
 * state:     The state of the call.         *
 * stateMode: The state mode of the call.    *
 *********************************************/
typedef struct call {
    HCALL       hsCall;
    HSSECALL    hSSECall;
    int         state;
    int         stateMode;
} call;

/* The application handle. */
HAPP hApp;

/* Boolean indicator for terminating the application. */
static RvBool stop = RV_FALSE;
static RvBool registered = RV_FALSE;

/* Calls objects. */
call activate, deactivate, newCall;

/* The application object handle of SSE.*/
HSSEAPP hSSEApp;

/* Boolean indicator for activating forward. */
RvUint act = 0;

/* The entity address that causes the call forward, for
   example, a Gatekeeper. */
char gkAddrStr[100];

/****************************************************************
 * inputHandler                                                 *
 * Purpose    : This is a callback function that is called      *
 *              whenever the user presses a key on the keyboard.*
 *              The function decides how to proceed according   *
 *              to the call state.                              *
 *              The key that was pressed is usually a response  *
 *              to a prompt by the program in the early stages  *
 *              of the program.                                 *
 * Input      : fd       : File descriptor that you want to     *
 *                         register with seli.                  *
 *              seliEvent: The event that called the callback   *
 *                         function.                            *
 *              error    : If an error occurs, the value is 1.  *
 *                         If no error occurs, the value        *
 *                         remains 0.                           *
 ****************************************************************/
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

    /* If the user deactivates forward: */
    if((answer=='d') && act)
    {
        cmCallNew(hApp,(HAPPCALL)&deactivate,&(deactivate.hsCall));
        sseCreateCall(hSSEApp,&(deactivate.hSSECall),(HSSEAPPCALL)&deactivate,
            deactivate.hsCall);

        /* This function is used for an existing call object to stop a
           server from forwarding calls to a specified forwarding
           destination. */
        sseForwardDeactivate(deactivate.hSSECall,cfu_p,"","",gkAddrStr);

        printf("deactivated\n");
        act = 0;
    }

    /* If the answer is positive: */
    if (answer == 'y')
    {
        if(!newCall.hsCall)
        {
            printf("Bye-bye\n");
            stop = RV_TRUE;
        }

        else
        {

            switch(newCall.state)
            {
            case cmCallStateOffering:
                /* The user answers the call. */
                printf("Answering the call\n");
                cmCallAnswer(newCall.hsCall);
                break;

            case cmCallStateConnected:
                /* The user drops the call. */
                printf("Dropping the call.\n");
                cmCallDrop(newCall.hsCall);
                break;

            case cmCallStateIdle:
                /* The user closes the window and disconnects the Q.931 channel.
                   When stop!=0, the loop in the 'main' function stops running. */
                printf("Bye-bye\n");
                stop = RV_TRUE;
                break;

            default:
                break;
            }
        }
    }

    /* If the answer is negative: */
    if (answer == 'n')
    {
        if (newCall.state == cmCallStateOffering)
        {
            /* The user does not take the call and the application drops it. */
            printf("I don't want to take the call\n");
            cmCallDrop(newCall.hsCall);
        }
    }

    if (stop)
        endKeyboardInput();
}




/****************************************************************
* cmEvCallStateChanged                                          *
* Purpose : This is the callback function used by the Stack to  *
*           report a change in the status of a call to the      *
*           application.                                        *
* Input   : haCall   :The application handle for the call.      *
*           hsCall   :The Stack handle for the call.            *
*           state    :The new state of the call.                *
*           stateMode:New state mode. This parameter describes  *
*                     how or why the call enters a new state.   *
* Return  : If an error occurs, the function returns a negative *
*           value.                                              *
*           If no error occurs, the function returns a          *
*           non-negative value.                                 *
****************************************************************/
static int RVCALLCONV cmEvCallStateChanged(IN   HAPPCALL           haCall,
                                           IN   HCALL              hsCall,
                                           IN   cmCallState_e      state,
                                           IN   cmCallStateMode_e  stateMode)
{
    /* The application has to map the call handle of CM to the call
       handle of SSE in order to be able to call the function
       'sseCallStateChanged'. The remaining parameters must be passed
       transparently (unchanged); state and stateMode are the same in
       CM and SSE. */

    call *current = (call*)haCall;

    RV_UNUSED_ARG(hsCall);

    current->state = state;
    current->stateMode = stateMode;
    sseCallStateChanged(current->hSSECall,state,stateMode);

    switch(state)
    {
    /* This callback function is called with the following state when
       the Stack receives a call offering. The user is prompted to
       answer the call. The user response is read by the
       inputHandler function. */
    case cmCallStateOffering:
        {
            printf("Call state: offering\n");
            printf("Press 'y' to answer or another key to drop the call.\n");
        }
        break;

    /* This callback function is called with the following state when
       the call is fully connected (both H.245 and Q.931 call
       establishment processes are completed). */
    case cmCallStateConnected:
        if (stateMode==cmCallStateModeConnectedCall)
        {
            printf("Call state: connected\n");
            printf("If you want to drop the call, enter 'y' .\n");
        }
        break;

    /* This callback function is called with the following state
       after the call is disconnected. The 'cmCallClose' and
       'sseCallClose' functions are called and the user is prompted
       to close the application. */
    case cmCallStateIdle:
        {
            cmCallClose(current->hsCall);
            sseCallClose(current->hSSECall);
            current->hSSECall = NULL;
            current->hsCall = NULL;


            if ((current == &activate) && act)
                printf("to deactivate, enter 'd'\n");
            else
            {
                if(current == &newCall)
                    printf("call state: closed\n");
                printf("To close the window, enter 'y'.\n");
            }

        }
        break;

    default: break;
    }

    return RV_OK;
}


/*****************************************************************
 * cmEvCallH450SupplServ                                         *
 * Purpose : This is a callback function that notifies the       *
 *           application that a Q.931 message has arrived with a *
 *           new H.450 APDU .                                    *
 * Input   : haCall   :The application handle for the call.      *
 *           hsCall   :The Stack handle for the call.            *
 *           msgType  :Q.931 message type.                       *
 *           nodeId   :PVT node ID of the H.450 APDU Tree.       *
 *           size     :Size of the APDU in bytes.                *
 * Return  : If an error occurs, the function returns a negative *
 *           value.                                              *
 *           If no error occurs, the function returns a          *
 *           non-negative value.                                 *
 *****************************************************************/
int RVCALLCONV cmEvCallH450SupplServ(IN HAPPCALL haCall,
                                     IN HCALL hsCall,
                                     IN cmCallQ931MsgType msgType,
                                     IN int nodeId,
                                     IN int size)
{
    RV_UNUSED_ARG(hsCall);

    /* sseCallH450SupplServ takes the APDU and sends it, through the
       call defined in hSSECall, to a service. */
    sseCallH450SupplServ(((call*)haCall)->hSSECall,msgType,nodeId,size);
    return RV_OK;
}

/*****************************************************************
 * cmEvNewCall                                                   *
 * Purpose : This is the callback function used by the Stack to  *
 *           report an incoming call to the application.         *
 * Input   : hApp     :Stack handle for the application.         *
 *           hsCall   :The Stack handle for the call.            *
 *           lphaCall :Pointer to the application handle for the *
 *                     new call.                                 *
 * Return  : If an error occurs, the function returns a negative *
 *           value.                                              *
 *           If no error occurs, the function returns a          *
 *           non-negative value.                                 *
 *****************************************************************/
static int RVCALLCONV cmEvNewCall(IN  HAPP         hApp,
                                  IN  HCALL        hsCall,
                                  IN  LPHAPPCALL   lphaCall)
{
    RV_UNUSED_ARG(hApp);

    printf("Receiving an incoming call.\n");
    newCall.hsCall=hsCall;
    *lphaCall=(HAPPCALL)&newCall;

    /* The following functions create a new call object and pass the
       appropriate call handle when preparing the calls that
       participate in the Forward service. */
    sseCreateCall(hSSEApp,&(newCall.hSSECall),(HSSEAPPCALL)&newCall,
        newCall.hsCall);

    sseCallImplementForward(newCall.hSSECall);

    return RV_OK;
}


/*****************************************************************
 * cmEvRegEvent                                                  *
 * Purpose : This is the callback function used by the Stack to  *
 *           report changes in the registration status.          *
 * input   : hAPP   - The application handle to the EPM module.  *
 *           state  - Registration status                        *
 *           regEvent  - Event that occured                      *
 *           regEventHandle - Node ID of message that caused     *
 *                            the event                          *
 * output  : None.                                               *
 * return  : None.                                               *
 *****************************************************************/
static int RVCALLCONV cmEvRegEvent(IN  HAPP         hApp,
                                   IN  cmRegState   regState,
                                   IN  cmRegEvent   regEvent,
                                   IN  RvPvtNodeId  regEventHandle)
{
    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(regEvent);
    RV_UNUSED_ARG(regEventHandle);

    if (regState == cmRegistered)
    {
        printf("Registered.\n");
        registered = RV_TRUE;
    }
    return RV_OK;
}


int AppMain(int argc, char* argv[])
{
    HPVT pvtTree;
    int rasRootId;
    int nodeId;
    char my_tel[50];
    /* The structure which stores IP address and port. */
    cmRASTransport tr;
    /* Used for storing the endpoint IP address. */
    char myIP[64];
    /* Used for storing the forwarding address. */
    char str[128], str1[128];
    /* Structures that contain pointers to the callback functions. */
    SCMCALLEVENT cmCallEvent;
    SCMEVENT cmEvent;
    /* a place to store the gk's address */
    cmTransportAddress gkAddr;

    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    /* Initializes the Stack according to the EPconfig.val file. */
    printf("Initializing CM\n");
    if (cmInitialize(GetFilename("EPConfig.val"),&hApp) < 0)
        return RV_ERROR_UNKNOWN;

    /* Initializes the SSE instance and exchanges application object handles. */
    printf("Initializing SSE\n");
    if (sseInit(GetFilename("EPConfig.val"),&hSSEApp,NULL,hApp,100) < 0)
        return RV_ERROR_UNKNOWN;
    if (sseWatchdogInit(hSSEApp,hApp) < 0)
        return RV_ERROR_UNKNOWN;

    /* Initialize the structures that contain pointers to the callback functions. */
    memset((void*)&cmCallEvent, 0, sizeof(SCMCALLEVENT));
    memset((void*)&cmEvent, 0, sizeof(SCMEVENT));
    cmCallEvent.cmEvCallStateChanged = cmEvCallStateChanged;
    cmCallEvent.cmEvCallH450SupplServ = cmEvCallH450SupplServ;
    cmEvent.cmEvNewCall = cmEvNewCall;
    cmEvent.cmEvRegEvent  = cmEvRegEvent;

    /* Set the call event handler. */
    cmSetCallEventHandler(hApp,&cmCallEvent,sizeof(cmCallEvent));

    /* Set the general event handler. */
    cmSetGenEventHandler(hApp,&cmEvent,sizeof(cmEvent));

    /* Gets the host IP address. */
    cmGetLocalCallSignalAddress(hApp,&tr);
    cmTransportAddress2String(&tr, sizeof(myIP), myIP);

    /* This function sets the default address of the entity. */
    sseSetDefaultAddress(hSSEApp, myIP);

    /* wait to be registered */
    printf("Waiting for RCF..");
    while(!registered)
    {
        printf(".");
        seliSelect();
    }

    /* Gets a PVT handle. */
    pvtTree = cmGetValTree(hApp);

    /* Provides access to the RAS tree in the Stack configuration. */
    rasRootId = cmGetRASConfigurationHandle(hApp);
    nodeId = pvtGetNodeIdByPath(pvtTree, rasRootId, "registrationInfo.terminalAlias.1.e164");
    pvtGetString(pvtTree,nodeId, sizeof(my_tel), my_tel);

    printf("My TEL is: %s\n", my_tel);

    printf("Enter a call forwarding address/alias:\n");

    /* get the forwarding address */
    fgets(str1, sizeof(str1), stdin);
    /* remove the '\n' from the end */
    str1[strlen(str1)-1] = 0;

    /* see if it needs any aditing */
    if(isdigit((int)str1[0]))
    {
        int a, b, c, d, e;
        /* check if it's an IP v4 address */
        if (sscanf(str1, "%d.%d.%d.%d:%d", &a, &b, &c, &d, &e) == 5)
            sprintf(str, "TA:%s", str1);
        else /* so it's a tel-number */
            sprintf(str, "TEL:%s", str1);
    }
    else if (str1[0] == '[') /* maybe it's an IP v6 address */
        sprintf(str, "TA:%s", str1);
    else /* so it's just some other alias */
        strcpy(str, str1);

    /* get the GK address, we assume it's also the forwarding server */
    cmGetGKCallSignalAddress(hApp,&gkAddr);
    sprintf(gkAddrStr, "TA:");
    cmTransportAddress2String(&gkAddr, sizeof(gkAddrStr)-3, gkAddrStr+3);

    /* start the forwarding service */
    cmCallNew(hApp,(HAPPCALL)&activate,&(activate.hsCall));
    sseCreateCall(hSSEApp,&(activate.hSSECall),(HSSEAPPCALL)&activate,activate.hsCall);

    /* This function is used for an existing call object to request a server to forward
       calls to a specified forwarding destination. */
    if(sseForwardActivate(activate.hSSECall,str,cfu_p,"","",gkAddrStr)<0)
        return RV_ERROR_UNKNOWN;
    else
        act = 1;

    /* Sets the inputHandler function as the callback function whenever an event occurs. */
    setKeyboardInput(inputHandler);

    /* This loop is responsible for activating the process that
       selects all registered file descriptors and calls the
       appropriate callback function when an event occurs.
       The loop stops when 'stop' = true. This is performed by the
       callback function after a forward is activated and then
       deactivated. The user then prompts the application
       to terminate. */
    while(!stop)
        seliSelectUntil(1000);

    /* Shuts down SSE and invalidates the application handle. */
    sseEnd(hSSEApp);

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
"   maxFail = 1",
"   allowCallsWhenNonReg = 0",
"   manualDiscovery = 0",
"+   defaultGatekeeper = 0",
"+    ipAddress = 0",
"+     ip = <127.0.0.1>",
"      port = 1719",
"",
"2  registrationInfo = 0",
"+   terminalType = 0",
"+    vendor = 0",
"+     vendor = 0",
"+      t35CountryCode = 11",
"       t35Extension = 11",
"       manufacturerCode = 11",
"-     productId = 'Sample Code Forward'",
"      versionId = 'RADVISION'",
"-    terminal = 0",
"     mc = 0",
"     undefinedNode = 0",
"-   terminalAlias = 0",
"+    * = 0",
"+     e164 = '323'",
"-    * = 0",
"+     h323-ID = \"SampleCode\"",
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
