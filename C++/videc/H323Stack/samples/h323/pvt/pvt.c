/****************************************************************
 *                       Pvt-Example2                           *
 * This program demonstrates working with PVT functions using   *
 * the RADVISION H.323 Protocol Stack.                          *
 * The program does the following:                              *
 *                                                              *
 * 1) Initializes the H.323 stack.                              *
 * 2) Creates 2 request messages:                               *
 *    a) Master/Slave Determination.                            *
 *    b) Terminal Capability Set.                               *
 * 3) Initializes the 2 PVT trees for these messages.           *
 * 4) Allows the user to navigate each tree and retrieve the    *
 *    field name and value of each node.                        *
 ***************************************************************/

#include "../osDependency.h"
#include <stdlib.h>
#include "cm.h"
#include "stkutils.h"

/* For storing a size for building the capability table. */
#define TABLE_SIZE  7


/* The structures for the capability table entry.
   A CapabilityTableEntry array will be used for building the
   table in the tree. */
typedef struct{
    char name[50];
    int value;
} CapabilityType;

typedef struct {
    int capabilityTableEntryNumber;
    int capability;
    CapabilityType type;
} CapabilityTableEntry;


/****************************************************************
 * buildCapabilityTableEntry:                                   *
 * This function converts the CapabilityTableEntry array it gets*
 * into a capability table in the PVT tree.                     *
 * Input: pvtTree - The handle returned by cmGetValTree().      *
 *    tableNodeId     - The capability table node ID in         *
 *                          pvtTree.                            *
 *    capabilityTable - An array of CapabilityTableEntry.       *
 *    size            - Size of this array.                     *
 ***************************************************************/

void buildCapabilityTableEntry(HPVT pvtTree,
                               int tableNodeId,
                               CapabilityTableEntry *capabilityTable,
                               int size)
{
    /* Used for building each entry in the capability table and
       placing it under the tableNodeId */
    int nodeId;
    int i;

    /* This loop searches the array, retrieves the data and builds
       each entry for the table as a child for tableNodeId. */
    for(i = 0; i<size; i++)
    {
        nodeId = pvtBuildByPath(pvtTree, tableNodeId, "0",0, NULL);
        pvtBuildByPath(pvtTree, nodeId, "capabilityTableEntryNumber",
                       capabilityTable[i].capabilityTableEntryNumber , NULL);
        nodeId = pvtBuildByPath(pvtTree, nodeId, "capability",
                                capabilityTable[i].capability , NULL);
        pvtBuildByPath(pvtTree, nodeId, capabilityTable[i].type.name,
                       capabilityTable[i].type.value , NULL);
    }
}




/****************************************************************
 * printPath                                                    *
 * Purpose: This function does the following:                   *
 *                                                              *
 * 1) Prints the field name of the current node in the PVT tree.*
 * 2) Prints a list of children and for each child, its value   *
 *    and number of grandchildren.                              *
 * 3) Prompts the user to proceed to make a selection from the  *
 *    following for the function to proceed:                    *
 *    a) A child from the list.                                 *
 *    b) The parent of the current node .                       *
 * 4) The function stops when the user decides to.              *                                                       *
 *                                                              *
 * Input: pvtTree - The handle returned by cmGetValTree().      *
 *        rootId  - The Node ID with which to start.            *
 ***************************************************************/

 void printPath(HPVT pvtTree, int rootId)
{
    /* Stores a handle in pstTree for the structure (Syntax Tree)
       definition of a specified node.*/
    HPST pstTree = pvtGetSynTree(pvtTree, rootId);

    /* The returned field ID of the node.*/
    int fieldId;

    /* The field name of the current node. */
    char fieldName[50];

    /* The current child ID. */
    int childId;

    /* The value of the current node. */
    RvInt32 value;

    /* The number of children of the current node. */
    int children;

    /* The number of grandchildren of the current node. */
    int grandchildren;

    /* Stores user input. */
    char chosenNode[4];

    int i = 0,j = 1;


    while (j)
    {
    /* The following commands get the field ID and value of the root,
    and use the field ID to get the field name from the syntax
        tree by calling pstGetFieldName(). */
        pvtGet(pvtTree, rootId, &fieldId, NULL, &value, NULL);
        pstGetFieldName(pstTree, fieldId, 50, fieldName);

        /* Gets the root number of children. */
        children = pvtNumChilds(pvtTree, rootId);


        if (children > 0)
        {
            printf("%s has %d children:\n\n",fieldName, children);

            /* This 'for-loop' gets the data for each child of the root in its
            turn and displays it on the screen. The function calls
            pvtGetByIndex() with the index 'i' of the child (the leftmost
            child gets 1 and so on) to get the child Id. */
            for (i = 1; i <= children; i++)
            {
                pvtGetByIndex(pvtTree,rootId,i,&childId);
                pvtGet(pvtTree, childId, &fieldId, NULL, &value, NULL);
                grandchildren = pvtNumChilds(pvtTree, childId);
                pstGetFieldName(pstTree, fieldId, 50, fieldName);
                if (!strcmp(fieldName, "(error)"))
                    sprintf(fieldName, "(Entry no.%d)", i);
                printf("%2d) %-28.28s - value: %4d, grandchildren: %d\n",i,fieldName,value,grandchildren);
            }

            printf("-------------------------------------------------------\n");

            /* After printing data for each child of the current node, the
            function prompts the user for the serial number of the child
            with which to proceed.
            The function proceeds with the selected child as the new
            root (if selected) or goes up in the tree or stops. */
            printf("Select the number of the child you want to proceed with,\n");
            printf("'-1' for going back or '0' for stopping.\n");
            fgets(chosenNode, sizeof(chosenNode), stdin);
            /* remove the '\n' from the end */
            chosenNode[strlen(chosenNode)-1] = 0;
            j = atoi(chosenNode);
            printf("-------------------------------------------------------\n");

            /* Incorrect input terminates the function. */
            if (j>i||j<-1)
            {
                printf("The number you selected is incorrect.\n");
                break;
            }

            /* Calling the function with the correct Root ID. */
            if (j == -1)
            {
                /* Going up in the tree */
                rootId = pvtParent(pvtTree, rootId);

                /* The following message explains that if the current node Id is the root, there is no option to go up in the tree. */
                if (rootId < 0)
                {
                    printf("You have reached the top level of the tree.\n");
                    break;
                }
            }

            else
                rootId = pvtGetByIndex(pvtTree,rootId,j,&childId);
        }

        else  /* There are no children. */
        {
            printf("%s has no children.\n", fieldName);
            printf("Enter '-1'to go back, enter '0' to stop. \n\n");
            fgets(chosenNode, sizeof(chosenNode), stdin);
            /* remove the '\n' from the end */
            chosenNode[strlen(chosenNode)-1] = 0;
            j = atoi(chosenNode);
            printf("-------------------------------------------------------\n");
            if (j == -1)
                rootId = pvtParent(pvtTree, rootId);
        }
    }

 }


int AppMain(int argc, char* argv[])
{
    /* The handle to the pvt tree. */
    HPVT pvtTree;

    /* The handle to the syntax tree. */
    HPST pstTree;

    /* The application handle. */
    HAPP hApp;


    /* The 2 roots: one for the master slave determination and one
       for the terminal capability set. */
    int mstrSlvId, trCpStId;

    /* The current node ID. */
    int curNodeId;

    /* The temporary nodes ID that stores the nodes ID for further
       use. */
    int temp0, temp1;

    /* A pointer to the target OID buffer. */
    char oid[50];

    /* The length, in bytes, of the resulting target OID string. */
    int OIDsize;

    int tableNodeId;
    CapabilityTableEntry capabilityTable[TABLE_SIZE];

    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    /* Initializes the Stack according to the config.val file which
       includes no H.245 path. */
    if (cmInitialize(GetFilename("EPConfig.val"),&hApp) < 0 )
        return RV_ERROR_UNKNOWN;

    /* Stores a handle to the H.245 Syntax Tree in pstTree for
       performing operations with the PST API. */
    pstTree = cmGetSynTreeByRootName(hApp, "h245");

    /* Stores the handle in the pvt tree in pvtTree.*/
    pvtTree = cmGetValTree(hApp);

    /* Creates a new root for master slave determination,
       builds and updates the right nodes. */
    mstrSlvId = pvtAddRoot(pvtTree, pstTree, 0, NULL);
    curNodeId = pvtBuildByPath(pvtTree, mstrSlvId, "request.masterSlaveDetermination", 0, NULL);
    curNodeId = pvtBuildByPath(pvtTree, curNodeId, "terminalType", 50, NULL);
    pvtSetByPath(pvtTree, mstrSlvId, "request", 0, NULL);

    printf("Number of nodes in master slave determination is %d\n", pvtTreeSize(pvtTree, mstrSlvId));

    /* Creates a new root for the terminal capability set, builds
       and updates the right nodes */
    trCpStId = pvtAddRoot(pvtTree, pstTree, 0, NULL);
    curNodeId = pvtBuildByPath(pvtTree, trCpStId, "request.terminalCapabilitySet", 0, NULL);
    pvtSetByPath(pvtTree, trCpStId, "request", 0, NULL);
    pvtBuildByPath(pvtTree, curNodeId, "sequenceNumber", 0, NULL);

    /* Converts an OID string into a Stack internal representation
       and stores the length (bytes) in 'size', of the resulting
       target OID string. */
    OIDsize = utlEncodeOID(sizeof(oid), oid,"itu-t (0) recommendation (0) h (8) 245 version (0) 2");

    /* Updates the right node with a pointer to the target OID
       buffer. */
    pvtBuildByPath(pvtTree, curNodeId, "protocolIdentifier", OIDsize , oid);

    curNodeId = pvtBuildByPath(pvtTree, curNodeId, "multiplexCapability", 0 , NULL);

    /* Stores a temporary node ID in 'temp1' that is used later for
       building the other children. */
    temp1 = curNodeId = pvtBuildByPath(pvtTree, curNodeId, "h2250Capability", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "maximumAudioDelayJitter", 60 , NULL);

    /* Stores a temporary node ID in 'temp0' that is used later for
       copying a sub tree. */
    temp0 = pvtBuildByPath(pvtTree, curNodeId, "receiveMultipointCapability", 0 , NULL);
    pvtBuildByPath(pvtTree, temp0, "multicastCapability", 0 , NULL);
    pvtBuildByPath(pvtTree, temp0, "multiUniCastConference", 0 , NULL);

    /* Stores a SEQUENCE OF MediaDistributionCapability with one
       entry, in the right node. */
    curNodeId = pvtBuildByPath(pvtTree, temp0, "mediaDistributionCapability", 0 , NULL);
    curNodeId = pvtBuildByPath(pvtTree, curNodeId, "0",0, NULL);
    pvtBuildByPath(pvtTree, curNodeId, "centralizedControl", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "distributedControl", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "centralizedAudio", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "distributedAudio", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "centralizedVideo", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "distributedVideo", 0 , NULL);

    /* Builds a brother for "receiveMultipointCapability"
       (another son for "h2250Capability"). */
    curNodeId = pvtBuildByPath(pvtTree, temp1, "transmitMultipointCapability", 0 , NULL);

    /* Copys the sub-tree stored in 'temp0', excludes its root node
       and places it under
       transmitMultipointCapability. */
    pvtAddChilds(pvtTree, curNodeId, pvtTree, temp0);

    /* Builds another son for "h2250Capability". */
    curNodeId = pvtBuildByPath(pvtTree, temp1, "receiveAndTransmitMultipointCapability", 0 , NULL);

    /* Copys the sub-tree stored in 'temp0', excludes its root node
       and places it under receiveAndTransmitMultipointCapability. */
    pvtAddChilds(pvtTree, curNodeId, pvtTree, temp0);

    curNodeId = pvtBuildByPath(pvtTree, temp1, "mcCapability", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "centralizedConferenceMC", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "decentralizedConferenceMC", 0 , NULL);

    pvtBuildByPath(pvtTree, temp1, "rtcpVideoControlCapability", 0 , NULL);
    curNodeId = pvtBuildByPath(pvtTree, temp1, "mediaPacketizationCapability", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "h261aVideoPacketization", 0 , NULL);

    /* Sets 'tableNodeId' to be the node ID of the third level in the
       pvtTree and updates the capabilityTable, which is a SET SIZE
       (1..256) of CapabilityTableEntry and contains 7 entries. */
    tableNodeId = pvtBuildByPath(pvtTree, trCpStId, "request.terminalCapabilitySet.capabilityTable", 0, NULL);

    /* Initializes the array of data for the capability table in
       the tree. Later on the function buildCapabilityTableEntry
       will be called for building the table in the tree */
    capabilityTable[0].capabilityTableEntryNumber = 7111;
    capabilityTable[0].capability = 0;
    strcpy(capabilityTable[0].type.name, "receiveAudioCapability");
    capabilityTable[0].type.value = 0;

    capabilityTable[1].capabilityTableEntryNumber = 7110;
    capabilityTable[1].capability = 0;
    strcpy(capabilityTable[1].type.name, "receiveAudioCapability");
    capabilityTable[1].type.value = 0;

    capabilityTable[2].capabilityTableEntryNumber = 728;
    capabilityTable[2].capability = 0;
    strcpy(capabilityTable[2].type.name, "receiveAudioCapability");
    capabilityTable[2].type.value = 0;

    capabilityTable[3].capabilityTableEntryNumber = 261;
    capabilityTable[3].capability = 0;
    strcpy(capabilityTable[3].type.name, "receiveVideoCapability");
    capabilityTable[3].type.value = 0;

    capabilityTable[4].capabilityTableEntryNumber = 263;
    capabilityTable[4].capability = 0;
    strcpy(capabilityTable[4].type.name, "receiveVideoCapability");
    capabilityTable[4].type.value = 0;

    capabilityTable[5].capabilityTableEntryNumber = 7231;
    capabilityTable[5].capability = 0;
    strcpy(capabilityTable[5].type.name, "receiveAudioCapability");
    capabilityTable[5].type.value = 0;

    capabilityTable[6].capabilityTableEntryNumber = 120;
    capabilityTable[6].capability = 0;
    strcpy(capabilityTable[6].type.name, "receiveAndTransmitDataApplicationCapability");
    capabilityTable[6].type.value = 0;

    /* Builds the table with 7 entries. */
    buildCapabilityTableEntry(pvtTree, tableNodeId, capabilityTable, TABLE_SIZE);

    /* After building each table entry with 2 children, more children and paths
       can be added to a specific entry (in this case - the fifth one) */
    curNodeId = pvtGetNodeIdByPath(pvtTree, trCpStId, "request.terminalCapabilitySet.capabilityTable.5.capability.receiveVideoCapability");
    curNodeId = pvtBuildByPath(pvtTree, curNodeId, "h263VideoCapability", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "sqcifMPI", 1 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "qcifMPI", 1 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "cifMPI", 1 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "maxBitRate", 1000 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "unrestrictedVector", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "arithmeticCoding", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "advancedPrediction", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "pbFrames", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "temporalSpatialTradeOffCapability", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "errorCompensation", 0 , NULL);


    /* Sets the current node ID to the third level of the pvtTree and updates
       the capabilityDescriptors, which is a SET SIZE (1..256) OF CapabilityDescriptor
       with one entry */
    curNodeId = pvtBuildByPath(pvtTree, trCpStId, "request.terminalCapabilitySet.capabilityDescriptors", 0, NULL);
    temp0 = pvtBuildByPath(pvtTree, curNodeId, "0", 0 , NULL);
    pvtBuildByPath(pvtTree, temp0, "capabilityDescriptorNumber", 0 , NULL);

    /* simultaneousCapabilities is a SET SIZE (1..256) OF AlternativeCapabilitySet
       which contains 3 entries. 'temp1' stores its node ID. */
    temp1 = pvtBuildByPath(pvtTree, temp0, "simultaneousCapabilities", 0 , NULL);

    /* First entry. */
    curNodeId = pvtBuildByPath(pvtTree, temp1, "0", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "0", 7111 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "0", 7110 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "0", 7231 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "0", 728 , NULL);

    /* Second entry. */
    curNodeId = pvtBuildByPath(pvtTree, temp1, "0", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "0", 261 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "0", 263 , NULL);

    /* Third entry. */
    curNodeId = pvtBuildByPath(pvtTree, temp1, "0", 0 , NULL);
    pvtBuildByPath(pvtTree, curNodeId, "1", 120 , NULL);

    /* If everything is OK, the function prints the following line: */
    printf("Number of nodes in terminal capability set is %d\n", pvtTreeSize(pvtTree, trCpStId));
    printf("\n\n\n\n");

    /* The user can navigate each tree, and retrieve data from each node. */
    printf("Master slave determination:\n\n");
    printPath(pvtTree, pvtChild(pvtTree,mstrSlvId));
    printf("\n\n\n\n");
    printf("Terminal capability set:\n\n");
    printPath(pvtTree, pvtChild(pvtTree,trCpStId));

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
