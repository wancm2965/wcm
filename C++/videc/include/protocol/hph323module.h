#ifndef _HP_H323MODULE_H_
#define _HP_H323MODULE_H_

#include "hptype.h"
#include "hpmoduleobject.h"
#include "hpmodulecallstruct.h"


typedef BOOL32 (MODULECALLBACK* CBH323NewCallIncoming)( HMDLCALL hCall , THpModuleCallInfo* pcCallInfo );
typedef BOOL32 (MODULECALLBACK* CBH323CallDisconnected)( HMDLCALL hCall , HMDLAPPCALL hAppCall , emHpModuleDisconnectReason emReason );
typedef BOOL32 (MODULECALLBACK* CBH323CallConnected)( HMDLCALL hCall , HMDLAPPCALL hAppCall , THpModuleCallInfo* pcCallInfo );



typedef struct tagH323CallCallBack
{
	CBH323NewCallIncoming m_cbNewCallIncoming;
	CBH323CallDisconnected m_cbCallDisconnected;
	CBH323CallConnected m_cbCallConnected;

	tagH323CallCallBack()
	{
		m_cbNewCallIncoming = NULL;
		m_cbCallConnected = NULL;
		m_cbCallConnected = NULL;
	}
}TH323CallCallBack;


typedef BOOL32 (MODULECALLBACK* CBH323ChannelConnected)( HMDLCALL hCall, HMDLAPPCALL hAppCall, THpModuleChanConnectedInfo* atChanConnectInfo, u8 bySize );
typedef BOOL32 (MODULECALLBACK* CBH323ChannelDisconnected)( HMDLCALL hCall , HMDLAPPCALL hAppCall , HMDLAPPCHANNEL hAppChan , HMDLCHANNEL hChan , u8 byReason );
typedef BOOL32 (MODULECALLBACK* CBH323ChannelFlowCtrl)( HMDLCALL hCall , HMDLAPPCALL hAppCall , HMDLAPPCHANNEL hAppChan , HMDLCHANNEL hChan , u16 wBitrate );
typedef BOOL32 (MODULECALLBACK* CBH323ChannelFastUpdate)( HMDLCALL hCall , HMDLAPPCALL hAppCall , HMDLAPPCHANNEL hAppChan , HMDLCHANNEL hChan );
typedef BOOL32 (MODULECALLBACK* CBH323DualStreamIncoming)( HMDLCALL hCall, HMDLAPPCALL hAppCall, HMDLAPPCHANNEL hAppChan, HMDLCHANNEL hChan, THpModuleCallCap* ptDualCap );
typedef BOOL32 (MODULECALLBACK* CBH323DualStreamConnected)( HMDLCALL hCall, HMDLAPPCALL hAppCall, HMDLAPPCHANNEL hAppChan, HMDLCHANNEL hChan, THpModuleChanConnectedInfo* ptChanInfo );
typedef BOOL32 (MODULECALLBACK* CBH323DualStreamReject)( HMDLCALL hCall, HMDLAPPCALL hAppCall, HMDLAPPCHANNEL hAppChan, HMDLCHANNEL hChan );
typedef BOOL32 (MODULECALLBACK* CBH323DualStreamStop)( HMDLCALL hCall, HMDLAPPCALL hAppCall, HMDLAPPCHANNEL hAppChan, HMDLCHANNEL hChan );

typedef struct tagH323ChannelCallBack
{
	CBH323ChannelConnected m_cbChannelConnected;
	CBH323ChannelDisconnected m_cbChannelDisconnected;
	CBH323ChannelFlowCtrl m_cbChannelFlowCtrl;
	CBH323ChannelFastUpdate m_cbChannelFastUpdate;
	CBH323DualStreamIncoming m_cbDualStreamIncoming;
	CBH323DualStreamConnected m_cbDualStreamConnected;
	CBH323DualStreamReject m_cbDualStreamReject;
	CBH323DualStreamStop m_cbDualStreamStop;

	tagH323ChannelCallBack()
	{
		memset( this , 0 , sizeof(tagH323ChannelCallBack) );
	}
}TH323ChannelCallBack;


typedef BOOL (MODULECALLBACK* CBH323RegResult)( HMDLREG hReg , HMDLAPPREG hAppReg , u8* pbuf , u16 wLen );


typedef struct tagH323Cfg
{
	s32 m_nH323ServiceAppId;
	s32 m_nH323MsgInAppid;
	s32 m_nH323MsgOutAppid;

	u32 m_dwh323StackIp;
	u8  m_abyh323StackIp_ipv6[16];

	u16 m_wh225port;
	u16 m_dwh323StackPortStart;
	u16 m_dwh323StackPortEnd;

	u16 m_wTotalCallNum;

	TH323CallCallBack m_tCallCallBack;
	TH323ChannelCallBack m_tChannelCallBack;
	CBH323RegResult     m_cbRegResult;

	tagH323Cfg()
	{
		m_nH323ServiceAppId = -1;
		m_nH323MsgInAppid = -1;
		m_nH323MsgOutAppid = -1;

		m_wh225port = 1720;
		m_dwh323StackIp = 0;

		memset( m_abyh323StackIp_ipv6 , 0 , sizeof(m_abyh323StackIp_ipv6) );

		m_wTotalCallNum = 10;
		m_dwh323StackPortStart = 60000;
		m_dwh323StackPortEnd = 60000+4*10;

		m_cbRegResult = NULL;
	}

}TH323Cfg;

class CHpH323Module
{
public:

	static BOOL32 H323ModuleInit( TH323Cfg &tCfg );

	static BOOL32 H323ModuleStart();

	static BOOL32 H323ModuleStop();

	static BOOL32 H323ModuleExit();

	static BOOL32 H323ModuleMakeOutgoingCall( HMDLAPPCALL hAppCall, THpModuleCallInfo* ptModuleCallInfo, THpModuleCallCap* ptSendCapbility, THpModuleCallCap* ptRecvCapbility );

	static BOOL32 H323ModuleCallAnswer( HMDLCALL hCall, HMDLAPPCALL hAppCall, THpModuleCallInfo* ptModuleCallInfo, THpModuleCallCap* ptSendCapbility, THpModuleCallCap* ptRecvCapbility );

	static BOOL32 H323ModuleCallReject( HMDLCALL hCall, HMDLAPPCALL hAppCall, emHpModuleDisconnectReason emReason );

	static BOOL32 H323ModuleCallHangup( HMDLCALL hCall, HMDLAPPCALL hAppCall, emHpModuleDisconnectReason emReason );

	static BOOL32 H323MoudleCloseChannel( HMDLCALL hCall, HMDLAPPCALL hAppCall, HMDLCHANNEL hChan, HMDLAPPCHANNEL hAppChan );

	static BOOL32 H323MoudleChannelFlowCtrl( HMDLCALL hCall , HMDLAPPCALL hAppCall , HMDLCHANNEL hChan, HMDLAPPCHANNEL hAppChan , u16 wBitrate );

	static BOOL32 H323ModuleChannelFastUpdate( HMDLCALL hCall , HMDLAPPCALL hAppCall , HMDLCHANNEL hChan, HMDLAPPCHANNEL hAppChan  );

	static BOOL32 H323ModuleSendDual( HMDLCALL hCall , HMDLAPPCALL hAppCall , HMDLCHANNEL hChan, HMDLAPPCHANNEL hAppChan, THpModuleCallCap* ptDualCap );

	static BOOL32 H323ModuleAnswerDual( HMDLCALL hCall, HMDLAPPCALL hAppCall, HMDLCHANNEL hChan, HMDLAPPCHANNEL hAppChan, BOOL bIsSuc, THpModuleChanConnectedInfo* ptDualInfo );

	static BOOL32 H323ModuleStopDual( HMDLCALL hCall, HMDLAPPCALL hAppCall, HMDLCHANNEL hChan, HMDLAPPCHANNEL hAppChan );

	static BOOL32 H323ModuleRegister( HMDLREG hReg , HMDLAPPREG hAppReg , THpModuleRegistReq* ptRegInfo );
	static BOOL32 H323ModuleUnRegister( HMDLREG hReg , HMDLAPPREG hAppReg );
	static BOOL32 H323ModuleRegistUpdate( HMDLREG hReg, HMDLAPPREG hAppReg, THpModuleRegistReq* ptRegInfo );


	static u16 m_wH323serviceAppId;
	static TH323ChannelCallBack m_tChannelCallBack;
	static TH323CallCallBack m_tCallCallBack;
	static CBH323RegResult     m_CbRegResult;
};


API void h323help();
API void h323setlog( u8 byLevel );
API void h323showreg();
API void h323showcall( u8 i );
API void h323ver();


#endif

