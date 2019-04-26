
#include "rvinternal.h"
#include "rvstdio.h"
#include "cm.h"
#include "mib.h"
#include "cmmib.h"


#ifdef __cplusplus
extern "C" {
#endif



typedef struct
{
    RvUint8 key[CONNECTION_KEY_SIZE];/*23];*/
    HCALL hsCall;
}connectionElT;


void int2index(int entry,RvUint8 * key)
{
    int temp;
    temp = entry & 0xff;
    key[3] = (RvUint8)temp;

    entry = entry>>8;
    temp = entry & 0xff;
    key[2] = (RvUint8)temp;

    entry = entry>>8;
    temp = entry & 0xff;
    key[1] = (RvUint8)temp;

    entry = entry>>8;
    key[0] = (RvUint8)entry;

}

void    transportAddress2String(cmTransportAddress *tr,RvUint8 * key)
{
    RvUint16 temp;
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    if (tr->type == cmTransportTypeIP)
    {
        memcpy(&key[0],&(tr->addr.v4.ip),4);
    }
    if (tr->type == cmTransportTypeIPv6)
    {
        int i;
        for (i=0; i < (int)sizeof(tr->addr.v6.ip); i += 4)
        {
            key[0] = key[0] << 1 ^ tr->addr.v6.ip[i];
            key[1] = key[1] << 1 ^ tr->addr.v6.ip[i+1];
            key[2] = key[2] << 1 ^ tr->addr.v6.ip[i+2];
            key[3] = key[3] << 1 ^ tr->addr.v6.ip[i+3];
        }
    }
#else
    memcpy(&key[0],&(tr->ip),4);
#endif
    temp = (RvUint16)(tr->port & 0xff);
    key[4] = (RvUint8)temp;
    key[5] =(RvUint8)(tr->port>>8);

}

void transportAddress2Index(cmTransportAddress *tr,RvUint8 * key)
{
    RvUint16 temp;
    int  *TAddressTag=(int *)key;

    /* TAddress must be in network byte order */
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    if (tr->type == cmTransportTypeIP)
    {
        int i;
        *TAddressTag = IPV4;
        for (i=0; i < (int)sizeof(tr->addr.v6.ip); i += 4)
        {
            key[4] = key[4] << 1 ^ tr->addr.v6.ip[i];
            key[5] = key[5] << 1 ^ tr->addr.v6.ip[i+1];
            key[6] = key[6] << 1 ^ tr->addr.v6.ip[i+2];
            key[7] = key[7] << 1 ^ tr->addr.v6.ip[i+3];
        }
    }
    if (tr->type == cmTransportTypeIPv6)
    {
        *TAddressTag = IPV6;
        memcpy(&key[4],&(tr->addr.v6.ip),4);
    }
#else
    *TAddressTag = IPV4;
    memcpy(&key[4],&(tr->ip),4);
#endif
    temp = (RvUint16)(tr->port & 0xff);
    key[9] = (RvUint8)temp;
    key[8] =(RvUint8)(tr->port>>8);

}

/*
   tableGet
   purpose: get MIB data
   input  : data - element about which the data is requested
            name - actually is a number which represents requested parameter
            mibdata - storage for requested parameter
   output : 0 on success
            checkNext - for parameter that does not contain a data
            notImplemened - for parameter not implemented yet
            noSuchField   - for error request

 */
int tableGet(RAElement  data,h341ParameterName name,mibDataT *mibdata)
{
    int  res,length,temp;
    cmTransportAddress tr;
    cmAlias alias;

    HCALL hsCall;
    connectionElT *el = (connectionElT *)data;
    hsCall=el->hsCall;
    mibdata->type=asnError;
    switch(name)
    {

/*
    case connectionsSrcTransporTAddressTag:

        mibdata->valueSize=ipv4;
        mibdata->type=asnInt;
        return 0;

    case      connectionsSrcTransporTAddress:
        length = sizeof(tr);

        res =cmCallGetParam(hsCall,cmParamSrcCallSignalAddress,0,&length,(char *)&tr );
        if (res<0)
            return invalidData;
        memcpy(mibdata->value,(RvUint8 *)&tr.ip,4);
        temp = tr.port & 0xff;
        mibdata->value[4] = (RvUint8)temp;
        mibdata->value[5] =(RvUint8)tr.port>>8;
        mibdata->valueSize=6;
        mibdata->type=asnOctetString;
        return 0;

    case               connectionsCallIdentifier:
        mibdata->valueSize=16;
        res =cmCallGetParam(hsCall,cmParamCallID,0,&mibdata->valueSize,(char *)mibdata->value );
        if (res<0)
            return invalidData;

        mibdata->type=asnOctetString;
        return 0;
*/
    case connectionsSrcTransporTAddressTag:
    case      connectionsSrcTransporTAddress:
    case               connectionsCallIdentifier:
        return checkNext;

    case     connectionsRole :
        {
            RvBool myBool = RV_TRUE;
            res = cmCallGetOrigin(hsCall, &myBool);
            if (myBool)
                mibdata->valueSize = 1;
            else
                mibdata->valueSize = 2;
            mibdata->type = asnInt;
            return 0;
        }

    case connectionsDestTransporTAddressTag:
        mibdata->valueSize=IPV4;
        mibdata->type=asnInt;
        return 0;
    case connectionsDestTransporTAddress:
        length = sizeof(tr);
        mibdata->type=asnAddressString;
        mibdata->value[0] = 0;
        mibdata->valueSize=0;
        res =cmCallGetParam(hsCall,cmParamDestCallSignalAddress,0,&length,(char *)&tr );
        if (res>=0)
        {
            transportAddress2String(&tr,mibdata->value);
            mibdata->valueSize=6;
        }

        return 0;


    case connectionsConfId:

        mibdata->type=asnMibOctetString;
        mibdata->valueSize=0;
        mibdata->value[0] = 0;
        res =cmCallGetParam(hsCall,cmParamCID,0,&mibdata->valueSize,(char *)mibdata->value );
        if (res>=0)
            mibdata->valueSize=16;
        return 0;
    case connectionsCalledPartyNumber:
        mibdata->type=asnMibOctetString;

        alias.length=(RvUint16)mibdata->valueSize;
        alias.string=(char *)mibdata->value;
        alias.string[0]=0;
        res =cmCallGetParam(hsCall,cmParamCalledPartyNumber,0,&length,(char *)&alias );
        if (res>=0)
            mibdata->valueSize = alias.length;
        else
            mibdata->valueSize = 0;
        return 0;
    case connectionsDestXtraCallingNumber1:
        temp=1;
    case connectionsDestXtraCallingNumber2:
        temp=2;
    case connectionsDestXtraCallingNumber3:
        temp=3;
    case connectionsDestXtraCallingNumber4:
        temp=4;
    case connectionsDestXtraCallingNumber5:
        temp=5;

        {
        int ii=0,jj=1;
        mibdata->type=asnMibOctetString;

        alias.length=(RvUint16)mibdata->valueSize;
        alias.string=(char*)mibdata->value;
        alias.string[0]=0;

            while(cmCallGetParam(hsCall,cmParamDestinationAddress,ii++,0,(char *)&alias)>=0)
            {
                if (alias.type==cmAliasTypeE164)
                {
                    if (jj==temp)
                    {
                        mibdata->valueSize = alias.length;
                        return 0;
                    }
                    jj++;
                }
            }
            mibdata->valueSize=0;

        }
        return 0;
    case connectionsCanOverlapSend:

        if ( cmCallGetParam(hsCall,cmParamSetupCanOverlapSending,0,&temp,NULL) >=0)
        {
            mibdata->type=asnInt;
            if(temp)
                mibdata->valueSize=temp;
            else
                mibdata->valueSize=2;

        }
        return 0;
    case connectionsCRV:
        mibdata->type=asnInt;
        if (cmCallGetParam(hsCall,cmParamCRV,0,(RvInt32*)&mibdata->valueSize,NULL)<0)
            mibdata->valueSize=0;
        return 0;
    case connectionsCallType:
        mibdata->type=asnInt;
        if (cmCallGetParam(hsCall,cmParamCallType,0,(RvInt32*)&mibdata->valueSize,NULL)>=0)
            mibdata->valueSize++;
        else
            mibdata->valueSize=0;
        return 0;

    case connectionsSrcH245SigTransporTAddress:
        {
            cmTransportAddress trSrc,trDest;
            if (cmCallGetH245Address(hsCall,&trSrc,&trDest,&temp)>=0)
            {

                transportAddress2String(&trSrc,mibdata->value);
                mibdata->valueSize=6;
            }
            else
                mibdata->valueSize=0;
            mibdata->type=asnAddressString;

        }
        return 0;
    case connectionsDestH245SigTransporTAddressTag:
    case connectionsSrcH245SigTransporTAddressTag:
        mibdata->type=asnInt;
        mibdata->valueSize=IPV4;
        return 0;

    case connectionsDestH245SigTransporTAddress:
        {
            cmTransportAddress trSrc,trDest;
            if (cmCallGetH245Address(hsCall,&trSrc,&trDest,&temp)>=0)
            {
                transportAddress2String(&trDest,mibdata->value);
                mibdata->valueSize=6;
            }
            else
                mibdata->valueSize=0;
            mibdata->type=asnAddressString;

        }
        return 0;
    case connectionsState:
        mibdata->type=asnInt;
        if (mibGetCallParameters(hsCall,enumconnectionsState,&mibdata->valueSize,mibdata->value))
            mibdata->valueSize = 0  ;
        return 0;
    case connectionsFastCall:

    case connectionsH245Tunneling:

    case connectionsDestAliasTag:
    case connectionsDestAlias:
    case connectionsRemoteExtensionAddress:
    case connectionsExtraCRV1:
    case connectionsExtraCRV2:
    case connectionsSecurity:
    case connectionsConnectionStartTime:
    case connectionsEndpointType:
    case connectionsReleaseCompleteReason:
        return notImplemented;
    default :
              return noSuchField;
    }
}

RvUint8 * tableGetIndex(RAElement   data)
{
    return ((connectionElT *)data)->key;
}




tableT *  h225tableConstruct(int nElements)
{
    return tableConstruct(nElements,CONNECTION_KEY_SIZE,sizeof(connectionElT),tableGet,NULL,tableGetIndex);

}

h341ErrorT h341InstAddNewCall(h341InstanceHandle hSnmp,HCALL hsCall)
{
    int length ,res,entry;
    cmTransportAddress tr;

    connectionElT el;

    length = sizeof(tr);
    res =cmCallGetParam(hsCall,cmParamSrcCallSignalAddress,0,&length,(char *)&tr );
    if (res<0)
        return (h341ErrorT)invalidCall;
    length =16;
    res =cmCallGetParam(hsCall,cmParamCallID,0,&length,(char *)&el.key[10] );
    if (res<0)
        return (h341ErrorT)invalidCall;
    el.hsCall=hsCall;

    transportAddress2Index(&tr, el.key);

    res = tableAdd( ((h341InstanceHandleT *)hSnmp)->H225ConnectionsTable,(RAElement)&el,&entry);
    if (!res)
    {
        if (((h341InstanceHandleT *)hSnmp)->h341ChangeGlobals!=NULL)
            ((h341InstanceHandleT *)hSnmp)->h341ChangeGlobals(((h341InstanceHandleT *)hSnmp)->agentComHandle,
                    connectionsActiveConnections,1);

    }
    return (h341ErrorT)res;

}


h341ErrorT h341InstDeleteCall(h341InstanceHandle hSnmp, HCALL hsCall)
{
    int length ,res;
    RvUint16 temp;
    cmTransportAddress tr;

    connectionElT el;

    length = sizeof(tr);
    res =cmCallGetParam(hsCall,cmParamSrcCallSignalAddress,0,&length,(char *)&tr );
    if (res<0)
        return (h341ErrorT)invalidCall;
    length =16;
    res =cmCallGetParam(hsCall,cmParamCallID,0,&length,(char *)&el.key[10] );
    if (res<0)
        return (h341ErrorT)invalidCall;
    el.hsCall=hsCall;
    transportAddress2Index(&tr, el.key);

#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    if (tr.type == cmTransportTypeIP)
    {
        int i;
        for (i=0; i < (int)sizeof(tr.addr.v6.ip); i += 4)
        {
            el.key[4] = el.key[4] << 1 ^ tr.addr.v6.ip[i];
            el.key[5] = el.key[5] << 1 ^ tr.addr.v6.ip[i+1];
            el.key[6] = el.key[6] << 1 ^ tr.addr.v6.ip[i+2];
            el.key[7] = el.key[7] << 1 ^ tr.addr.v6.ip[i+3];
        }
    }
    if (tr.type == cmTransportTypeIPv6)
    {
        memcpy(&el.key[4],&(tr.addr.v6.ip),4);
    }
#else
    memcpy(&el.key[4],&(tr.ip),4);
#endif
    temp = (RvUint16)(tr.port & 0xff);
    el.key[9] = (RvUint8)temp;
    el.key[8] =(RvUint8)(tr.port>>8);
    res = tableDelete(((h341InstanceHandleT *)hSnmp)->H225ConnectionsTable,el.key,CONNECTION_KEY_SIZE);
    if (!res)
    {
        if (((h341InstanceHandleT *)hSnmp)->h341ChangeGlobals!=NULL)
            ((h341InstanceHandleT *)hSnmp)->h341ChangeGlobals(((h341InstanceHandleT *)hSnmp)->agentComHandle,
                    connectionsActiveConnections,-1);
    }
    return (h341ErrorT)0;
}

#ifdef __cplusplus
}
#endif
