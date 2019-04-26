/****************************************************************
 *                       Facility Send.c                        *
 *                                                              *
 * The following program is a simple console application that   *
 * demonstrates how to implement sending facility messages      *
 * using the RADVISION H.323 Protocol Stack.                    *
 * The program does the following :                             *
 * 1. Initializes the H.323 Stack.                              *
 * 2. Prompts the user for the IP address of the receiving side.*
 * 3. Installs the H.323 Stack call and control callbacks.      *
 * 4. Creates a single call.                                    *
 * 5. Once the call is established starts sending facility      *
 *    messages (the user is prompted for a string to put        *
 *    as a non standart data).                                  *
 * 6. When the user's string is "End the Call" , sends the last *
 *    message and drops the call.                               *
 * At the beginning the user is prompted for the APIs to be used*
 * to build the facility message.
 ****************************************************************/

#include "../osDependency.h"
#include <seli.h>
#include <cm.h>
#include <pvaltree.h>
#include <stkutils.h>
#include <q931asn1.h>

typedef enum{CREATE_NEW_BY_PATH , COPY_BY_PATH , CREATE_NEW_BY_FIELD
}app_method_t;


/* The application handle. */
HAPP hApp;

/* The Stack handle to the call. */
HCALL hsCall;

/* Boolean indicator for terminating the application. */
static RvBool stop = RV_FALSE;


/*Bolean indicator if facility message is currently being built*/
static RvBool facil_send_st = RV_FALSE;

/*Bolean indicator if end msg sent to remote*/
static RvBool end_msg_sent = RV_FALSE;

/*
	global variable for for choosing what API's the application uses
	by default , the application uses the build by fields macros.
*/
static app_method_t app_method = CREATE_NEW_BY_FIELD;  

/*
 *	a local function that builds the facility message from zero , using the pvtBuildByPath API's
 */
void buildFacilityMsg( HPVT pvTree , RvPvtNodeId* rootNodeId , RvChar* nonStandartData)
{
	RvPvtNodeId currNodeId = -1 , srcNodeId , tmpNodeId;
	char   oid[128];
    int    oidSize = sizeof(oid) , len = strlen(nonStandartData);
	cmNonStandardIdentifier  info;

	/*encode protocol identifier*/
	oidSize = utlEncodeOID(oidSize, oid, "itu-t recommendation h 2250 0 4");
	
	switch(app_method){
	case CREATE_NEW_BY_PATH:
		printf("Creating a New Message and Using the BuildByPath APIs.\n");
		/*set protocolDiscriminator of q931 msg*/
		pvtBuildByPath(pvTree, *rootNodeId, "protocolDiscriminator", 8, NULL);
		
		/*set userUser node*/
		currNodeId = pvtBuildByPath(pvTree, *rootNodeId , "message.facility.userUser" , 0 , NULL);

		/*set protocolDiscriminator of userUser*/
		pvtBuildByPath(pvTree, currNodeId, "protocolDiscriminator", 5, NULL);

		/*set user info node*/
		currNodeId = pvtBuildByPath(pvTree , currNodeId , "h323-UserInformation.h323-uu-pdu" , 0 , NULL);
		
		
		pvtBuildByPath(pvTree, currNodeId , "h323-message-body.facility.protocolIdentifier", oidSize, oid);

		currNodeId = pvtBuildByPath(pvTree, currNodeId , "nonStandardData", 0, NULL);
		break;
	case COPY_BY_PATH:
		printf("Copying a Message from the Stack and Using the BuildByPath APIs.\n");
		/*get application Q931 message node id*/
		srcNodeId = cmGetProperty((HPROTOCOL)hsCall);

		/*get Q931 facility message*/
		srcNodeId = pvtGetNodeIdByPath(pvTree , srcNodeId , "facility");

		/*copy facility message of stack into rootNodeId*/
		pvtSetTree(pvTree , *rootNodeId , pvTree , srcNodeId);
		
		currNodeId = pvtBuildByPath(pvTree , *rootNodeId , "message.facility.userUser.h323-UserInformation.h323-uu-pdu.nonStandardData" , 0 , NULL);
		break;
	case CREATE_NEW_BY_FIELD:
	default:
		 printf("Creating a New Message and Using the BuildByFeildIds APIs.\n");
		 pvtAdd(pvTree ,*rootNodeId ,__q931(protocolDiscriminator),8,NULL,NULL);
		
		__pvtBuildByFieldIds(currNodeId , pvTree, *rootNodeId ,
                                    { _q931(message)
									  _q931(facility)
                                      _q931(userUser)
                                     LAST_TOKEN
                                    },
                                 0, NULL);
        
		pvtAdd(pvTree ,currNodeId ,__q931(protocolDiscriminator),5,NULL,NULL);
		
	
		__pvtBuildByFieldIds(currNodeId , pvTree, currNodeId ,
                                    { 
									  
									  _q931(h323_UserInformation)
									  _q931(h323_uu_pdu)
                                     LAST_TOKEN
                                    },
                                 0, NULL);
		
								
		__pvtBuildByFieldIds(tmpNodeId, pvTree, currNodeId ,
                                    { 
									  
									  _q931(h323_message_body)
									  _q931(facility)
                                     LAST_TOKEN
                                    },
                                 0, NULL);
									  
		pvtAdd(pvTree ,tmpNodeId,__q931(protocolIdentifier),oidSize , oid , NULL);


		__pvtBuildByFieldIds(currNodeId , pvTree, currNodeId ,
                                    { 
									  _q931(nonStandardData)
                                     LAST_TOKEN
                                    },
                                 0, NULL);
		
		break;

	}
	

	info.objectLength=0;
	info.manufacturerCode = (RvUint16)1;
    info.t35CountryCode = (RvUint8)1;
    info.t35Extension = (RvUint8)0;

	cmNonStandardParameterCreate(pvTree, currNodeId, &info, nonStandartData, len);

}


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
 ****************************************************************/
static void RVCALLCONV inputHandler(int fd, seliEvents sEvent, RvBool error)
{

/*non standart data string max len*/
#define ST_LEN 255

/*a message from the sender to drop the call*/
#define END_MSG "End The Call"
	
	/* The PVT handle. */
	HPVT pvTree = cmGetValTree(hApp);

	/*The PST handle*/
	HPST psTree = cmGetSynTreeByRootName(hApp , "q931");

    int answer ;

	/*string for user's non satandart data*/
	static RvChar nonStandartData[ST_LEN] = {0};

	/*q931 root node id*/
	static RvPvtNodeId rootNodeId = -1;
	
	/*buffer's index*/
	static int buf_ind = 0;

	RV_UNUSED_ARG(fd);
    RV_UNUSED_ARG(sEvent);
    RV_UNUSED_ARG(error);

	/*zero non standart data buffer*/
	if(buf_ind == 0)
		memset(nonStandartData , 0 , ST_LEN);

    
    /* Reads the user input. */
    answer = getchar();

	if(facil_send_st){
		if(answer == '\n' && buf_ind){
			facil_send_st = RV_FALSE;
		}else{
			if(answer != '\n'){
				nonStandartData[buf_ind] = (char)answer;
				++buf_ind;
			}
		}
		
	}else{
		switch (answer) {
		case '1':
			facil_send_st = RV_TRUE;
			printf("Non Standart Data -> ");
			break;
		case '2':
			strcpy(nonStandartData , END_MSG);
			end_msg_sent = RV_TRUE;
			break;
		default:
			break;
		}
		
	}
	
	if((facil_send_st == RV_FALSE && buf_ind) || ( end_msg_sent ==  RV_TRUE ) ){


		RvPvtNodeId rootNodeId = -1;

		
		/*get a q931message node*/
		if((rootNodeId) == -1){
			(rootNodeId) = pvtAddRoot(pvTree , psTree , 0 , NULL);
		}
		
		/*build message*/
		
		buildFacilityMsg(pvTree , &rootNodeId , nonStandartData);

		pvtPrintStd(pvTree , rootNodeId , -1);

		printf("Now I'm sending a Facility Message With User String: %s\n" , nonStandartData);
		
		/*send facility message*/
		cmCallFacility(hsCall , rootNodeId);

		facil_send_st = RV_FALSE;
		
		buf_ind = 0;
		
	}
	
	if (end_msg_sent){
        endKeyboardInput();
		pvtDelete(pvTree , rootNodeId);
	}
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
 ****************************************************************/
static int RVCALLCONV cmEvCallStateChanged(IN   HAPPCALL           haCall,
                                           IN   HCALL              hsCall,
                                           IN   cmCallState_e      state,
                                           IN   cmCallStateMode_e  stateMode)
{
    RV_UNUSED_ARG(haCall);

    switch(state)
    {

    /* 
		The callback function is called with this state when the call is
		connected (either after answering the call with state mode 
		cmCallStateModeConnectedCallSetup or when the call is fully connected after
		H.245 with state mode cmCallStateModeConnectedCall)
	*/

    case cmCallStateConnected:
		if (stateMode==cmCallStateModeConnectedCallSetup)
        {
			printf("Call Answered...\n");
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

int AppMain(int argc, char* argv[])
{

/*max time to block on select*/
#define MAX_TIME 10000

	/* An array for storing the IP address input by the user.*/
	char dest[50];
	
    /* Boolean indicator for making a single call. */
    int call= 0 , method = '3';

    /* The structures that contain pointers to the callback functions. */
    SCMCALLEVENT cmCallEvent;


    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

	printf("Choose Appliction APIs:\n1 -> Creating a new Facility Message and using the ByPath APIs\n"
		"2 -> Using a Facility Message Copied from the Stack and using the ByPath APIs\n"
		"3 -> Creating a new Facility Message and using the ByFieldId APIs\n");
	method = getchar();
	switch(method){
	case '1':
		app_method = CREATE_NEW_BY_PATH;
		break;
	case '2':
		app_method = COPY_BY_PATH;
		break;
	case '3':
	default:
		app_method = CREATE_NEW_BY_FIELD;
		break;
	}
	getchar();
	
	
    /* Initialize the structures that contain pointers to the callback functions. */
    memset(&cmCallEvent, 0, sizeof(SCMCALLEVENT));
    cmCallEvent.cmEvCallStateChanged = cmEvCallStateChanged;

    /* Initializes the Stack according to the EPconfig.val file. */
    if (cmInitialize(GetFilename("EPConfig.val"),&hApp) < 0 )
        return RV_ERROR_UNKNOWN;

    /* Copies the IP address the user printed into 'dest' */
    printf ("Enter address and port: (for example: TA:111.11.1.11:1111)\n");
    fgets(dest, sizeof(dest), stdin);
    /* remove the '\n' from the end */
    dest[strlen(dest)-1] = 0;

    /* Sets the inputHandler function as the callback function whenever an event occurs. */
    setKeyboardInput(inputHandler);

    /* Sets the calls event handler function. */
    cmSetCallEventHandler(hApp,&cmCallEvent,sizeof(cmCallEvent));

    /* This loop is responsible for activating the process that selects
       all registered file descriptors and calls the appropriate callback
       function when an event occurs. The application creates a single
       call to the IP in 'dest'.
       The loop stops when 'stop'=1. This is performed by the callback
       function after a call has been connected and then disconnected. The
       user then prompts the application to terminate. */

    while(!stop)
    {
		if(call == 1){	
			if(!end_msg_sent && !facil_send_st){
				printf(" 1. Send a Facility Message\n"
					" 2. Stop the Application\n");
			}
			call++;
		}	

        if (!stop && !call)
        {
            call = 1;
            cmCallNew(hApp,NULL,&hsCall);
            printf("Making a call to %s\n", dest);
            cmCallMake(hsCall,64000,0,dest,NULL,dest,NULL,0);
        }

		
		/*seliSelectUntil(MAX_TIME);*/
		seliSelect();
		

    }

    /* Shuts down the H.323 Stack.*/
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