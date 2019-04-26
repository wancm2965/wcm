#ifndef _HP_SIPMODULE_H_
#define _HP_SIPMODULE_H_

#include "hptype.h"
#include "hpmoduleobject.h"
#include "hpmodulecallstruct.h"


typedef BOOL32 (MODULECALLBACK* CBSipCallIncoming)( HMDLCALL hCall, THpModuleCallInfo *ptCallParam, THpModuleCallCap *ptSndCap , THpModuleCallCap *ptRcvCap );
typedef BOOL32 (MODULECALLBACK* CBSipCallConnected)( HMDLCALL hCall, HMDLAPPCALL hAppCall, THpModuleCallInfo *ptCallParam,THpModuleCallCap *ptSndCap, THpModuleCallCap *ptRcvCap );
typedef BOOL32 (MODULECALLBACK* CBSipCallDisconnected)(HMDLCALL hCall, HMDLAPPCALL hAppCall, const emHpModuleDisconnectReason emReason ); 

typedef BOOL32 (MODULECALLBACK* CBSipChannelConnected)( HMDLCALL hCall, HMDLAPPCALL hAppCall, THpModuleChanConnectedInfo* pTConnectedInfo, s32 nSize );       
typedef BOOL32 (MODULECALLBACK* CBSipChannelDisconnected)( HMDLCALL hCall, HMDLAPPCALL hAppCall, THpModuleChanDisconnectedInfo *ptChanDisConnInfo, s32 nSize );
typedef BOOL32 (MODULECALLBACK* CBSipChannelFastUpdapte)( HMDLCALL hCall, HMDLAPPCALL hAppCall, HMDLCHANNEL hChan, HMDLAPPCHANNEL hAppChan );
typedef BOOL32 (MODULECALLBACK* CBSipChannelFlowControl)( HMDLCALL hCall, HMDLAPPCALL hAppCall, HMDLCHANNEL hChan, HMDLAPPCHANNEL hAppChan, u16 wBitRate );

typedef BOOL32 (MODULECALLBACK* CBSipDualIncoming)( HMDLCALL hCall, HMDLAPPCALL hAppCall, THpModuleDualCapList *tDualList );

typedef BOOL32 (MODULECALLBACK* CBSipRegister)( HMDLREG hReg, HMDLAPPREG hAppReg, emHpModuleRegResult regResult, const u8 *pBuf, const s32 nLen );
typedef BOOL32 (MODULECALLBACK* CBSipUnRegister)( HMDLAPPREG hAppReg, emHpModuleRegResult regResult, const u8 *pBuf, const s32 nLen );




struct TSipCallCBFunction
{
	CBSipCallIncoming         m_cbCallIncoming;
	CBSipCallConnected        m_cbCallConnected;
	CBSipCallDisconnected	  m_cbCallDisconnected;

	CBSipChannelConnected     m_cbChannelConnected;
	CBSipChannelDisconnected  m_cbChannelDisconnected;
	CBSipChannelFastUpdapte   m_cbChannelFastUpdate;
	CBSipChannelFlowControl   m_cbChannelFlowControl;

	CBSipDualIncoming		  m_cbDualIncoming;
	CBSipRegister             m_cbSipRegister;
	CBSipUnRegister           m_cbSipUnRegister;

	TSipCallCBFunction()
	{
		m_cbCallIncoming = NULL;
		m_cbCallConnected = NULL;
		m_cbCallDisconnected = NULL;
		m_cbChannelConnected = NULL;
		m_cbChannelDisconnected = NULL;
		m_cbChannelFastUpdate = NULL;
		m_cbChannelFlowControl = NULL;
		m_cbDualIncoming = NULL;
		m_cbSipRegister = NULL;
		m_cbSipUnRegister   = NULL;
	}
};


struct TSipCfg
{
	TSipCfg()
	{
		Clear();
	}
	void Clear()
	{
		m_emEndpointType = emHpModuleEndpointTypeBegin;
		m_dwMaxSendRecvBufferSize = 20480;
		m_wRegClientNum = 128;
		m_nVerIdSize = 0;
		m_nProductIdSize = 0;
		m_dwKeepAliveTime = 0;
		m_wReserved = 0;
		m_wIfProxy = FALSE;
		m_wCallingPort = 5060;
		memset( &m_tCallCBFuntion, 0, sizeof(m_tCallCBFuntion) );
		memset( m_abyProductId, 0, MAX_LEN_PRODUCT_ID );
		memset( m_abyVersionId, 0, MAX_LEN_VERSION_ID );
	}


	s32						  m_nsipServiceAppId;
	s32						  m_nsipMsgInAppId;
	s32						  m_nsipMsgOutAppId;

	u16						  m_wTotalCallNum;
	TSipCallCBFunction		  m_tCallCBFuntion;

	//adapter ²ÎÊý
	emHpModuleEndpointType	  m_emEndpointType;
	u16						  m_wCallingPort;
	BOOL32				      m_wIfProxy;
	u16						  m_wReserved;
	u32					      m_dwKeepAliveTime;
	u16						  m_wRegClientNum;
	u32						  m_dwMaxSendRecvBufferSize;
	s32						  m_nProductIdSize;
	u8						  m_abyProductId[MAX_LEN_PRODUCT_ID];
	s32						  m_nVerIdSize;
	u8						  m_abyVersionId[MAX_LEN_VERSION_ID];
};


class CHpSipModule
{
public:
	static BOOL32 SipModuleInit( TSipCfg &tCfg );   
	static BOOL32 SipModuleExit();

	static BOOL32 SipModuleMakeCall( HMDLAPPCALL hAppCall, THpModuleCallInfo *ptCallParam, THpModuleCallCap *ptSndCap, THpModuleCallCap *ptRcvCap );  
	static BOOL32 SipModuleCallAnswer( HMDLCALL hCall, HMDLAPPCALL hAppCall, THpModuleCallInfo *ptCallParam, THpModuleCallCap *ptSndCap, THpModuleCallCap *ptRcvCap );
	static BOOL32 SipModuleCallReject( HMDLCALL hCall, HMDLAPPCALL hAppCall, emHpModuleDisconnectReason emReason );
	static BOOL32 SipModuleCallHangup( HMDLCALL hCall, HMDLAPPCALL hAppCall, emHpModuleDisconnectReason emReason );
	static BOOL32 SipModuleCallAck( HMDLCALL hCall, HMDLAPPCALL hAppCall, THpModuleCallCap *tSndCap = NULL , THpModuleCallCap *tRcvCap = NULL );
	static BOOL32 SipModuleCallReInvite( HMDLCALL hCall, HMDLAPPCALL hAppCall, THpModuleChanConnectedInfo* ptConnectInfo, u8 bySize );

	static BOOL32 SipModuleChannelFlowCtrl(HMDLCALL hCall , HMDLAPPCALL hAppCall , HMDLCHANNEL hChan, HMDLAPPCHANNEL hAppChan, u16 wBitrate);
	static BOOL32 SipModuleChannelFastUpdate(HMDLCALL hCall , HMDLAPPCALL hAppCall , HMDLCHANNEL hChan, HMDLAPPCHANNEL hAppChan);
	static BOOL32 SipModuleChannelFastUpdateWithoutID(HMDLCALL hCall , HMDLAPPCALL hAppCall);

	static BOOL32 SipModuleRegister( HMDLREG hReg, HMDLAPPREG hAppReg, THpModuleRegistReq* tRegInfo );
	static BOOL32 SipModuleUnregister(HMDLREG hReg, HMDLAPPREG hAppReg);

	static BOOL32 SipModuleSendDual( HMDLCALL hCall , HMDLAPPCALL hAppCall, THpModuleDualCapList *ptDual = NULL );    
	static BOOL32 SipModuleAnswerDual( HMDLCALL hCall , HMDLAPPCALL hAppCall, BOOL32 bSuc, THpModuleDualCapList *ptDual = NULL );  
	static BOOL32 SipModuleStopDual( HMDLCALL hCall , HMDLAPPCALL hAppCall ); 
private:
	static BOOL32 PostToService( CHpModuleMsg &cMsg );
	static u16 m_wSipServiceAppId; 
public:
	static TSipCallCBFunction m_tSipCallCBFunction;
};

#endif