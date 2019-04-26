#pragma once

#include "KGeneralFunc.h"
#include "KMONDefine.h"

/************************************************************************/
/* 打包，解包数据结构                                                   */
/* 打包时，可能使用 KCmdPacket，也可能使用 KCmdItem，所以用模板         */
/* 要针对不同的数据类型打包，所以要重载相应数据类型的模板               */
/************************************************************************/

/************************************************************************/
/* 打包，解包基本监控设备信息                                           */
/************************************************************************/
//------------------------------------------------------------------------
template<typename T>
inline void CMDPacket( T& t, PBASE_MON_DEV_INFO pBaseMonDevInfo)
{
    t.SetAttrib( MON_KEY_MCUID, pBaseMonDevInfo->strMcuID);
    t.SetAttrib( MON_KEY_MCUADDR, pBaseMonDevInfo->strMcuAddr);
    t.SetAttribUS( MON_KEY_MCUPORT, pBaseMonDevInfo->usMcuPort);
    t.SetAttrib( MON_KEY_MCUTYPE, pBaseMonDevInfo->strMcuType);

    t.SetAttrib( MON_KEY_FROM, pBaseMonDevInfo->strID);
    t.SetAttrib( MON_KEY_NODEID, pBaseMonDevInfo->strNodeID);
    t.SetAttrib( MON_KEY_NATADDR, pBaseMonDevInfo->strNatAddr);
    t.SetAttrib( MON_KEY_NATTYPE, pBaseMonDevInfo->strNatType);
    t.SetAttrib( MON_KEY_LOCALADDR, pBaseMonDevInfo->strLocalAddr);
    t.SetAttribUL( MON_KEY_LOCALPORT, pBaseMonDevInfo->usLocalPort);

    t.SetAttrib( MON_KEY_PASSWORD, pBaseMonDevInfo->strPassword);
    t.SetAttribUS( MON_KEY_CLIENTTYPE, pBaseMonDevInfo->eDevType);
    t.SetAttribUS( MON_KEY_TERMTYPE, pBaseMonDevInfo->usTermType);
    t.SetAttrib( MON_KEY_TERMNAME, pBaseMonDevInfo->strTermName);
    t.SetAttrib( MON_KEY_NAME, pBaseMonDevInfo->strName);
    t.SetAttrib( MON_KEY_SERIALNO, pBaseMonDevInfo->strSerialno);
    t.SetAttrib( MON_KEY_SYSSOFTVERSION, pBaseMonDevInfo->strSysSoftVersion);
	t.SetAttribUS(MON_KEY_FACTORY,pBaseMonDevInfo->usFactory);
	t.SetAttrib( MON_KEY_BINDMEMBERID, pBaseMonDevInfo->strBindMemberID);

	t.SetAttribUS( MON_KEY_CHSTATUS, pBaseMonDevInfo->usStatus);
	t.SetAttrib( MON_KEY_ORDERID, pBaseMonDevInfo->strOrderID);
    t.SetAttrib( MON_KEY_MGSID, pBaseMonDevInfo->strMGSID);
}

//------------------------------------------------------------------------
template<typename T>
inline void CMDUnPacket( T& t, PBASE_MON_DEV_INFO pBaseMonDevInfo)
{
    pBaseMonDevInfo->strMcuID = t.GetAttrib( MON_KEY_MCUID ).AsString();
    pBaseMonDevInfo->strMcuAddr = t.GetAttrib( MON_KEY_MCUADDR ).AsString();
    pBaseMonDevInfo->usMcuPort = t.GetAttrib( MON_KEY_MCUPORT ).AsUnsignedShort();
    pBaseMonDevInfo->strMcuType = t.GetAttrib( MON_KEY_MCUTYPE ).AsString();

    pBaseMonDevInfo->strID = t.GetAttrib( MON_KEY_FROM ).AsString();
    pBaseMonDevInfo->strNodeID = t.GetAttrib( MON_KEY_NODEID ).AsString();
    pBaseMonDevInfo->strNatAddr = t.GetAttrib( MON_KEY_NATADDR ).AsString();
    pBaseMonDevInfo->strNatType     = t.GetAttrib( MON_KEY_NATTYPE ).AsString();
    pBaseMonDevInfo->strLocalAddr = t.GetAttrib( MON_KEY_LOCALADDR ).AsString();
    pBaseMonDevInfo->usLocalPort = t.GetAttrib( MON_KEY_LOCALPORT ).AsUnsignedShort();

    pBaseMonDevInfo->strPassword    = t.GetAttrib( MON_KEY_PASSWORD ).AsString();
    pBaseMonDevInfo->eDevType       = (MON_DEVTYPE)t.GetAttrib( MON_KEY_CLIENTTYPE ).AsUnsignedShort();
    pBaseMonDevInfo->usTermType     = t.GetAttrib( MON_KEY_TERMTYPE ).AsUnsignedShort();
    pBaseMonDevInfo->strTermName    = t.GetAttrib( MON_KEY_TERMNAME ).AsString();
    pBaseMonDevInfo->strName        = t.GetAttrib( MON_KEY_NAME ).AsString();
    pBaseMonDevInfo->strSerialno    = t.GetAttrib( MON_KEY_SERIALNO ).AsString();
    pBaseMonDevInfo->strSysSoftVersion = t.GetAttrib( MON_KEY_SYSSOFTVERSION ).AsString();
	pBaseMonDevInfo->usFactory		= t.GetAttrib(MON_KEY_FACTORY).AsUnsignedShort();
	pBaseMonDevInfo->strBindMemberID = t.GetAttrib( MON_KEY_BINDMEMBERID ).AsString();

	pBaseMonDevInfo->usStatus		= t.GetAttrib(MON_KEY_CHSTATUS).AsUnsignedShort();
	pBaseMonDevInfo->strOrderID		= t.GetAttrib( MON_KEY_ORDERID ).AsString();
    pBaseMonDevInfo->strMGSID		= t.GetAttrib( MON_KEY_MGSID ).AsString();
}

/************************************************************************/
/* 打包，解包监控网关信息                                               */
/************************************************************************/
//------------------------------------------------------------------------
template<typename T>
inline void CMDPacket( T& t, PMON_GATEWAY_INFO pMonGatewayInfo)
{
    CMDPacket( t, static_cast<PBASE_MON_DEV_INFO>(pMonGatewayInfo));
}

//------------------------------------------------------------------------
template<typename T>
inline void CMDUnPacket( T& t, PMON_GATEWAY_INFO pMonGatewayInfo)
{
    CMDUnPacket( t, static_cast<PBASE_MON_DEV_INFO>(pMonGatewayInfo));
}

/************************************************************************/
/* 打包，解包监控设备信息                                               */
/************************************************************************/
//------------------------------------------------------------------------
template<typename T>
inline void CMDPacket( T& t, PMON_DEV_INFO pMonDevInfo)
{
    CMDPacket( t, static_cast<PBASE_MON_DEV_INFO>(pMonDevInfo));
}

//------------------------------------------------------------------------
template<typename T>
inline void CMDUnPacket( T& t, PMON_DEV_INFO pMonDevInfo)
{
    CMDUnPacket( t, static_cast<PBASE_MON_DEV_INFO>(pMonDevInfo));
}

/************************************************************************/
/* 打包，解包通道信息                                                   */
/************************************************************************/
template<typename T>
inline void CMDPacket( T& t, PMON_CHANNEL_INFO pMonChannelInfo)
{
    CMDPacket( t, static_cast<PBASE_MON_DEV_INFO>(pMonChannelInfo));

    t.SetAttrib( MON_KEY_CHANNELID, pMonChannelInfo->strChannelID);
    t.SetAttribUS( MON_KEY_CHNUM, pMonChannelInfo->usChannelNum);
    t.SetAttrib( MON_KEY_CHNAME, pMonChannelInfo->strChannelName);
    t.SetAttrib( MON_KEY_DEVID, pMonChannelInfo->strDevID);
    t.SetAttrib( MON_KEY_DEVNAME, pMonChannelInfo->strDevName);
    t.SetAttribUS( MON_KEY_SDPDATA, pMonChannelInfo->usSpType);
    t.SetAttribUS( MON_KEY_CHSTATUS, pMonChannelInfo->usStatus);
    t.SetAttribUL( MON_KEY_VIDEOCID, pMonChannelInfo->ulVideocID);
    t.SetAttribUL( MON_KEY_AUDIOCID, pMonChannelInfo->ulAudiocID);
    t.SetAttrib( MON_KEY_DEVINFO, pMonChannelInfo->strDevInfo);
	t.SetAttrib( MON_KEY_NODEID, pMonChannelInfo->strNodeID);
	t.SetAttribUN( MON_KEY_CONTROLFLAG, pMonChannelInfo->nControlFlag);
}

//------------------------------------------------------------------------
template<typename T>
inline void CMDUnPacket( T& t, PMON_CHANNEL_INFO pMonChannelInfo)
{
    CMDUnPacket( t, static_cast<PBASE_MON_DEV_INFO>(pMonChannelInfo));
    pMonChannelInfo->strChannelID = t.GetAttrib( MON_KEY_CHANNELID ).AsString();
    pMonChannelInfo->usChannelNum = t.GetAttrib( MON_KEY_CHNUM ).AsUnsignedShort();
    pMonChannelInfo->strChannelName = t.GetAttrib( MON_KEY_CHNAME ).AsString();
    pMonChannelInfo->strDevID = t.GetAttrib( MON_KEY_DEVID ).AsString();
    pMonChannelInfo->strDevName = t.GetAttrib( MON_KEY_DEVNAME ).AsString();
    pMonChannelInfo->usSpType = t.GetAttrib( MON_KEY_SDPDATA ).AsUnsignedShort();
    pMonChannelInfo->usStatus = t.GetAttrib( MON_KEY_CHSTATUS ).AsUnsignedShort();
    pMonChannelInfo->ulVideocID = t.GetAttrib( MON_KEY_VIDEOCID ).AsUnsignedLong();
    pMonChannelInfo->ulAudiocID = t.GetAttrib( MON_KEY_AUDIOCID ).AsUnsignedLong();
    pMonChannelInfo->strDevInfo = t.GetAttrib( MON_KEY_DEVINFO ).AsString();
	pMonChannelInfo->strNodeID = t.GetAttrib( MON_KEY_NODEID ).AsString();
	pMonChannelInfo->nControlFlag = t.GetAttrib( MON_KEY_CONTROLFLAG ).AsUnsignedInt();
}

