/****************************************************************
 *                      Facility Receive.c                      *
 *                                                              *
 * The following program is a simple console application that   *
 * demonstrates how to implement facility messages receiving    *   
 * using the RADVISION H.323 Protocol Stack.                    *
 * The program does the following :                             *
 *                                                              *
 * 1. Gets the host IP address and displays it on the screen.   *
 * 2. Initializes the H.323 Stack.                              *
 * 3. Installs the H.323 Stack call and control callbacks.      *
 * 4. Waits for an incoming call and when it arrives, answer it.*
 * 5. When Ever a facility message arrives , prints the non     *
 *    standart data , if there is any.                          *
 * 6. When a facility message arrives with a string saying      *
 *    "End the Call" in its non standart data ,the program      *
 *    drops the call.                                           *
 ****************************************************************/

#include "../osDependency.h"
#include <seli.h>
#include <cm.h>

/*non standart data string max len*/
#define ST_LEN 255

/*a message from the sender to drop the call*/
#define END_MSG "End The Call"

/* The application handle. */
HAPP hApp;

/* The Stack handle to the call. */
HCALL hsCall;

/* Boolean indicator for terminating the application */
static RvBool stop = RV_FALSE;

/* The pvt handle. */
HPVT pvTree;

/* The remote terminal facility message PVT Node ID. */
RvPvtNodeId facil;



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
 ****************************************************************/
static int RVCALLCONV cmEvCallStateChanged(IN   HAPPCALL           haCall,
                                           IN   HCALL              hsCall,
                                           IN   cmCallState_e      state,
                                           IN   cmCallStateMode_e  stateMode)
{
    RV_UNUSED_ARG(haCall);

    switch(state)
    {

    /* This callback function is called with the following state when
       the Stack receives a call offering. The user is prompted to
       answer the call. The user response is read by the inputHandler
       function. 
	*/

	case cmCallStateOffering:
        printf("Received a call offering\nAnswering...\n");
		cmCallAnswer(hsCall);
        break;


    /* 
		The callback function is called with this state when the call is
		connected (either after answering the call with state mode 
		cmCallStateModeConnectedCallSetup or when the call is fully connected after
		H.245 with state mode cmCallStateModeConnectedCall)
	*/

    case cmCallStateConnected:
		if (stateMode==cmCallStateModeConnectedCallSetup)
        {
			printf("Call Answered ...\n");
		}
		if (stateMode==cmCallStateModeConnectedCall)
        {
			printf("Call Established...\n");
		}


			
        break;

    /* 
	   This callback function is called with the following state after
       the call is disconnected. The 'cmCallClose' function is called
    */

    case cmCallStateIdle:
        cmCallClose(hsCall);
        printf("The Call is Closed\n");
		printf("Bye-bye\n");
        stop = RV_TRUE;
        break;

    default:
        break;
    }

    return RV_OK;
}

/****************************************************************
 * cmEvCallFacility                                             *
 * Purpose : This is the callback function used by the Stack to *
 *           notify the application that a facility message has *
 *			 arrived.                                           *
 * Input   : haCall   :The application handle for the call.     *
 *           hsCall   :The Stack handle for the call.			*
 *           handle   :PVT Node ID of the Q931 Message sub-tree	*
 *			           of the FACILITY message that was received*
 *			 proceed  :If set to RV_TRUE , processed by the		*
 *					   stack , otherwise processed by the		*
 *					   application								*
 * Return  : If an error occurs, the function returns a negative*
 *           value.                                             *
 *           If no error occurs, the function returns a         *
 *           non-negative value.                                *
 ****************************************************************/
static int RVCALLCONV cmEvCallFacility( IN HAPPCALL haCall,  
										IN HCALL hsCall,
										IN int handle, 
										OUT IN RvBool *proceed)
{
	/*.facility.userUser.h323-UserInformation.h323-uu-pdu.nonStandardData.data*/


	RvChar nonStandartData[ST_LEN];
	RvPvtNodeId dataNodeId;

	RV_UNUSED_ARG(haCall);

	memset(nonStandartData , 0 , sizeof(nonStandartData));

	pvTree = cmGetValTree(hApp);

	/*get nodeId of user string*/
	dataNodeId = pvtGetNodeIdByPath(pvTree , handle , "message.facility.userUser.h323-UserInformation.h323-uu-pdu.nonStandardData.data");

	/*get user string*/
	pvtGetString(pvTree , dataNodeId , ( sizeof(nonStandartData) - 1 ) , nonStandartData);


	printf("Received a Facility Message with Non Standart Data : %s\n"  , nonStandartData);

	/*if it's "End the Call" message - drop the call*/
	if(!strcmp(nonStandartData , END_MSG)){
		/*drop the call*/
		printf("Droping the Call...\n");
		cmCallDrop(hsCall);
	}

	*proceed = RV_FALSE;

	return RV_OK;
}

int AppMain(int argc, char* argv[])
{
    RvChar addrStr[64];

    /* The structure which stores IP address and port.*/
    cmRASTransport tr;

    /* The structures that contain pointers to the callback functions. */
    SCMCALLEVENT cmCallEvent;

    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    /* Initialize the structure that contains pointers to the callback functions. */
    memset(&cmCallEvent, 0, sizeof(SCMCALLEVENT));
    cmCallEvent.cmEvCallStateChanged = cmEvCallStateChanged;
	cmCallEvent.cmEvCallFacility = cmEvCallFacility;

    /* Initializes the Stack according to the EPconfig.val file. */
    if (cmInitialize(GetFilename("EPConfig.val"),&hApp) < 0 )
        return RV_ERROR_UNKNOWN;

	/* Sets the calls event handler function. */
    cmSetCallEventHandler(hApp,&cmCallEvent,sizeof(cmCallEvent));

	 /* Gets the host IP address and displays it on the screen. */
    cmGetLocalCallSignalAddress(hApp,&tr);
    cmTransportAddress2String(&tr, sizeof(addrStr), addrStr);
    printf("My Address is: TA:%s\n", addrStr);

	/* 
		This loop is responsible for activating the process that selects
		all registered file descriptors and calls the appropriate
		callback function when an event occurs. The loop stops when
		'stop'=1. This is performed by the callback function after a call
		has been connected and then disconnected. The user then prompts the
		application to terminate. 
	*/
    stop = RV_FALSE;
    while(!stop)
       seliSelect();

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

