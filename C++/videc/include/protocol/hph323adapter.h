#ifndef _HP_H323ADAPTER_H_
#define _HP_H323ADAPTER_H_

#include "ipc.h"
#include "cm.h"
#include <time.h>
#include <math.h>

#ifndef CALLBACK
#define CALLBACK RVCALLCONV
#endif


#ifndef IN
#define IN
#endif//!IN
#ifndef OUT
#define OUT
#endif//!OUT

#ifdef _LINUX_
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#endif//!_LINUX_

#define LEN_ALIAS			(u16)64	
#define LEN_256				(u16)256
#define LEN_1				(u16)1
#define LEN_16				(u16)16
#define LEN_128				(u16)128
#define LEN_64				(u16)64

#define LEN_OID				 LEN_16					//OID长度
#define LEN_GUID			 LEN_16					//GIUD长度
#define LEN_NETADDR			sizeof(TH323NetAddr)	//网路地址长度
#define LEN_ALIAS			(u16)64				    //最大别名长度h323-id

#define LEN_DHKEY_MAX		(u16)256                //dhkey的最大长度
#define LEN_DH_X			(u16)8                  //dh中的随机数长度
#define LEN_DHKEY			(u16)64                 //dhkey的长度
#define LEN_H235KEY_MAX		(u16)64					//(1-65535)  
#define LEN_DES_KEY         (u8)8                   //DES的密钥长度
#define LEN_AES_KEY         (u8)16                  //AES的密钥长度

#define ANNEXQ_DYNAMIC_PT	(u16)100				//h.281动态载荷

#define MAX_CALL_NUM		(u16)256
#define MIN_CALL_NUM        (u16)10
#define MIN_CHANNEL_NUM		(u16)12
#define MAX_CHANNEL_NUM     (u16)20
#define LEN_USERINFO_MAX    (u8)131
#define LEN_USERINFO_MIN	(u16)2

#define H323CALLPORT		(u16)1720
#define H323RASPORT			(u16)1719

#define LEN_NONSTANDARD_MAX (u16)2048

#define LEN_DISPLAY_MAX		(u16)82
#define LEN_DISPLAY_MIN		(u16)2



#define MAX_CAPTABLE_SIZE	(u16)64
#define MAX_ALTCAP_NUM      MAX_CAPTABLE_SIZE
#define MAX_SIMUCAP_NUM     (u16)16
#define MAX_DESCAP_NUM      (u16)5
#define MAX_H264CAP_NUM		(u16)10 

#define LEN_MTNUM_PERCONF	(u16)193

#define PRODUCTSTRING_LESS_SIZE        (u16)32      //产品信息的长度,用于获取对方的产品信息
#define VERSIONSTRING_LESS_SIZE        (u16)32      //版本信息的长度,用于获取对方的版本信息

#define MAXBITRATE_H261     (u16)19200				//单位100bps
#define MAXBITRATE_H263     (u32)192400				//单位100bps


#define   MEDIA_TYPE_MP4	              (u8)97  /*MPEG-4*/

enum
{
	RV_PEXP,
	RV_PINFO,
	RV_PDEBUG,
	RV_ALL
};

enum callBackResult 
{
	cb_err = -1,
	cb_ok
};

enum emH225CallMsgType 
{
	h_h323_call_invalidInterface = 0,
    h_h323_call_make,
	h_h323_call_incoming,	  	  	
	h_h323_call_answer,
	h_h323_call_drop,
	h_h323_call_connected,
	h_h323_call_disconnected,
	h_h323_call_facility,
};

//RAS消息
enum H225RasMsgType 
{
	h_h323_ras_invalidInterface = 0,
	h_h323_ras_GRQ,						//reserve
	h_h323_ras_GCF,						//reserve
	h_h323_ras_GRJ,						//reserve
	h_h323_ras_RRQ,                      //[incoming]&[outgoing]:[TH323RRQInfo]
	h_h323_ras_RCF,						//[incoming]&[outgoing]:[TRCFINFO]    
	h_h323_ras_RRJ,                      //[incoming]&[outgoing]:[int:reason]
	h_h323_ras_LWRRQ,                    //[incoming]&[outgoing]:[TH323RRQInfo]
	h_h323_ras_ARQ,						//[incoming]&[outgoing]:[HCALL][TARQINFO]
	h_h323_ras_ACF,                      //[incoming]&[outgoing]:[HCALL][TACFINFO]
	h_h323_ras_ARJ,                      //[incoming]&[outgoing]:[HCALL][int:reason]
	h_h323_ras_DRQ,                      //[incoming]&[outgoing]:[HCALL][TDRQINFO]
	h_h323_ras_DCF,						//[incoming]&[outgoing]:[HCALL][TDCFINFO]
	h_h323_ras_DRJ,						//[incoming]&[outgoing]:[HCALL][int:reason]
	h_h323_ras_URQ,						//[incoming]&[outgoing]:[TURQINFO]
	h_h323_ras_UCF,						//[incoming]&[outgoing]:[TUCFINFO]
	h_h323_ras_URJ,						//[incoming]&[outgoing]:[int:reason]
	h_h323_ras_IRQ,                      //[incoming]&[outgoing]:[HCALL][TIRQINFO]
	h_h323_ras_IRQTimeOut,               //[incoming]:           [HCALL]
	h_h323_ras_IRR,						//[incoming]&[outgoing]:[HCALL][TIRRINFO]
	h_h323_ras_BRQ,						//[incoming]&[outgoing]:[HCALL][TBRQINFO] 
	h_h323_ras_BCF,                      //[incoming]&[outgoing]:[HCALL][TBCFINFO]
	h_h323_ras_BRJ,						//[incoming]&[outgoing]:[HCALL][int:reason]
	h_h323_ras_LRQ,						//[incoming]&[outgoing]:[TLRQINFO]
	h_h323_ras_LCF,                      //[incoming]&[outgoing]:[TLCFINFO]
	h_h323_ras_LRJ,                      //[incoming]&[outgoing]:[int:reason]
	h_h323_ras_NSM,
	h_h323_ras_SCR,

	h_h323_ras_registration,			//自动RAS修改注册信息重新向GK注册	[outgoing]:TH323RRQInfo
	h_h323_ras_unregistration,			//自动RAS向GK注销				    [outgoing]:NULL
	h_h323_ras_online,					//自动RAS在GK注册成功			    [incoming]:NULL
	h_h323_ras_offline,					//自动RAS未在GK注册			        [incoming]:NULL
};

//逻辑通道消息
enum emH245ChanMsgType 
{
	h_h323_chan_invalidInterface = 0,
    h_h323_chan_open,
	h_h323_chan_openAck,
	h_h323_chan_openReject,
	h_h323_chan_connected,
	h_h323_chan_close,

	h_h323_chan_parameter,
	h_h323_chan_remoteRtpAddress,
	h_h323_chan_remoteRtcpAddress,
	h_h323_chan_dynamicpayloadtype,
	h_h323_chan_videofastupdatePIC,
	h_h323_chan_flowControl,

	h_h323_chan_on,
	h_h323_chan_off,


    h_h323_chan_flowControlIndication,
	h_h323_chan_newchan,
    
};


//会议控制消息
enum emH245CtrlMsgType 
{
	h_h323ctrl_invlaidInterface = 0,
	h_h323_ctrl_h245TransportConnected,
	h_h323_ctrl_h245Established,
	h_h323_ctrl_masterSlave,
	h_h323_ctrl_masterSlaveDetermine,
	h_h323_ctrl_masterSlaveResponse,             
	h_h323_ctrl_capNotifyExt,
	h_h323_ctrl_capResponse,
	h_h323_ctrl_roundTripDelay,
	h_h323_ctrl_roundTripDelay_request,
	h_h323_ctrl_H239Message,
	h_h323_ctrl_customFecc,
    h_h323_ctrl_end,
};

enum emActResult
{
	act_err = -1,
	act_ok,
};

enum emCallMakeErrorReason 
{
	err_unknow				= -1,	//未知原因
	err_full_call			= -2,	//呼叫已满
	err_set_param			= -3,	//适配层设置参数出错(包含地址)
	err_null_ptr			= -4,	//指针为NULL或指向的buffer大小不匹配	
	err_set_address			= -5,	//呼叫时协议栈内部判断地址设置有误	
	err_already_setup		= -6,	//已经呼叫过	
	err_autoRas_PrepareARQ	= -7,	//发送ARQ失败
	err_autoRas_Register	= -8,   //注册失败
	err_Q931_Connect		= -9,	//Q931呼叫失败
	err_iptype_notmatch		= -10,	//呼叫源地址和目的地址类型不一致(多宿IP下)
};

enum emMsdResult
{
	msd_master,
	msd_slave,
	msd_err = 0xff    
};

enum emTransportAddrType
{
	addr_unicast,
	addr_multiplecast,
	addr_err = 0xff,
};

enum emCallAddrType
{
	cat_err = -1,
	cat_ip = 1,
	cat_alias
};

//呼叫断开原因
enum emCallDisconnectReason 
{
	reason_h323_busy = 1,
	reason_h323_normal,
	reason_h323_rejected,
	reason_h323_unreachable,
	reason_h323_local,
	reason_h323_unknown,

	reason_h323_nobandwidth,                  //ARQ拒绝或没有带宽
	reason_h323_nopermission,                 //被叫网守拒绝
	reason_h323_unreachablegatekeeper,         //网守不可达for arq
	reason_h323_mcuoccupy,
	reason_h323reconnect,
	reason_h323confholding,
	reason_h323hascascaded                 
};

typedef enum
{
	emLogChanRejectUnspecified,
	emLogChanRejectUnsuitableReverseParameters,
	emLogChanRejectDataTypeNotSupported,
	emLogChanRejectDataTypeNotAvailable,
	emLogChanRejectUnknownDataType,
	emLogChanRejectDataTypeALCombinationNotSupported,
	emLogChanRejectMulticastChannelNotAllowed,
	emLogChanRejectInsuffientBandwdith,
	emLogChanRejectSeparateStackEstablishmentFailed,
	emLogChanRejectInvalidSessionID,
	emLogChanRejectMasterSlaveConflict,
	emLogChanRejectWaitForCommunicationMode,
	emLogChanRejectInvalidDependentChannel,
	emLogChanRejectReplacementForRejected
}emlogChannelRejectCause;

enum emCapExchangeResult 
{
	cese_accept,		//对端接受本端能力
	cese_reject,		//对端拒绝本端能力
	cese_err = 0xff
};

//别名类型
enum emAliasType
{
	alias_type_aliastype,
	alias_type_e164 = 0x1,
	alias_type_h323ID,
	alias_type_email,
	alias_type_epID,
	alias_type_GKID,
	alias_type_others
};

enum  emDataType 
{
	data_type_none = 0,
	data_type_audio,
	data_type_video,
	data_type_data,
};


enum  PayloadType 
{
	emPayloadTypeBegin = 0,
	//音频格式
	a_typeBegin,
	a_g711a,
	a_g711u,
	a_g722,
	a_g7231,
	a_g728,
	a_g729,
	a_g723,
	a_g719,
	a_mpegaaclc,
	a_mpegaacld,
	a_typeEnd = 49,

	//视频格式
	v_typeBegin = 50,	
	v_h261,
	v_h262,
	v_h263,
	v_h263plus,
	v_h264,
	v_mpeg4,
	v_extended,
	v_typeEnd = 99,

	//数据格式
	d_typeBegin = 100,	
	d_t120,
	d_h224,
	d_typeEnd = 149,

	emPayloadTypeEnd = 255
};


inline BOOL IsAudio( u16 payload )
{
	if( a_typeBegin < payload && payload < a_typeEnd  )
		return TRUE;
	else
		return FALSE;
}

inline BOOL IsVideo( u16 payload )
{
	if(v_typeBegin < payload && payload < v_typeEnd )
		return TRUE;
	else
		return FALSE;
}


typedef enum
{
	emResolutionBegin,
	emSQCIF,
	emQCIF,
	emCIF,
	emCIF2,
	emCIF4,    // 576SD: stands for D1: 720*576
	emCIF16,

	em352240,
	em704480,  // 480SD

	em640480,
	em800600,
	em1024768,

	emW4CIF,   // Wide4CIF 16:9 1024*576
	emHD720,        // 720   1280*720
	emSXGA,         // SXGA  1280*1024
	emUXGA,         // UXGA  1600*1200
	emHD1080,       // 1080  1920*1080	

	em1280x800,
	em1280x768,

	emAUTO,
	emResolutionEnd
}emResolution;


//终端类型
typedef enum
{
	emEndpointTypeUnknown = 0,
	emEndpointTypeMT	  = 0x01,
	emEndpointTypeMCU	  = 0x02,
	emEndpointTypeGK      = 0x04
}emEndpointType;

//终端类型值
typedef enum
{
	emMsMT			= 50,
	emMsMT_MC		= 70,
	emMsMCU			= 190,
	emMsActiveMC	= 240,
}emMsTerminalType;


//设备厂商标识信息
enum emVendorProductId
{
	vendor_MICROSOFT_NETMEETING = 1,
	vendor_AVCON,
	vendor_KEDA,
	vendor_HUAWEI,
	vendor_POLYCOM,
	vendor_POLYCOM_MCU,
	vendor_TANDBERG,
	vendor_RADVISION,
	vendor_VCON,
	vendor_SONY,
	vendor_VTEL,
	vendor_ZTE,
	vendor_ZTE_MCU,
	vendor_CODIAN,
	vendor_AETHRA,
	vendor_CHAORAN,
	vendor_UNKNOWN,
	vendor_END
};


//H323 Annex Q
/*   
 *   h.281 encoded in host bit order:
 *   +-------+---------+--------+---------+--------+---------+--------+---------+
 *   +   P   | R=1/L=0 |    T   | U=1/D=0 |    Z   | I=1/O=0 |    F   | I=1/O=0 |
 *   +-------+---------+--------+---------+--------+---------+--------+---------+
 */
enum feccAction
{
	actionInvalid				= 0,  
	actionStart					= 0x01,  //开始
	actionContinue				= 0x02,  //继续
	actionStop					= 0x03,  //停止
	actionSelectVideoSource		= 0x04,  //选择视频源
	actionVideoSourceSwitched	= 0x05,  //切换视频源
	actionStoreAsPreset			= 0x06,  //存储预置位
	actionActivatePreset		= 0x07,  //恢复到预置位
	actionAutoFocus				= 0x11,  //自动调焦
};

//PTFZ codes, parameters for action Start, continue and stop.
enum feccPTFZRequest
{
	requestInvalid				= 0,
	requestPanLeft				= 0x80,	//向左移动
	requestPanRight				= 0xc0,	//向右移动
	requestTiltUp				= 0x30,	//向上移动
	requestTiltDown				= 0x20,	//向下移动
	requestFocusIn				= 0x03,	//近焦
	requestFocusOut				= 0x02,	//远焦
	
	requestZoomIn				= 0x0c,	//视野变小
	requestZoomOut				= 0x08,	//视野变大

	requestBrightnessUp			= 0x01,	//亮度加(nonstandard)
	requestBrightnessDown		= 0x04  //亮度减(nonstandard)
};

typedef enum
{
	annexQNotInited				= -20,
	annexQBindError				= -15,
	annexQParamError			= -14,
	annexQLockError				= -12,	
	annexQCodingError			= -10,
	annexQSendError				= -9,	
	annexQGeneralError			= -5,
	annexQOk					= 1
}annexQResult;

//for tanderg
enum feccVideoSourceNumber
{
    VideoSourceDefault               = 0,    //当前视频源
    VideoSourceMainCamera            = 1,    //主视频源
    VideoSourceAuxCamera             = 2,    //辅助视频源
    VideoSourceDocCamera             = 3,    //文件展示台
    VideoSourceAuxDocCamera          = 4,    //辅助文件展示台，对于泰德为vcr
    VideoSourceVideoPlaybackSource   = 5,    //vcr，对于泰德为PC
};


//H239
typedef enum
{
	emH239InfoTypeInvalid		        = 0,

	emH239FlowControlReleaseRequest     = 1,
	emH239FlowControlReleaseResponse    = 2,
	emH239PresentationTokenRequest      = 3,
	emH239PresentationTokenResponse     = 4,
	emH239PresentationTokenRelease      = 5,
	emH239PresentationTokenIndicateOwner= 6,

	emH239InfoTypeEnd
}emH239InfoType;


typedef enum
{
	emChanUnkown = 0,
	emChanVideo1 = 1,
	emChanAudio	 = 2,
	emChanVideo2 = 3
}emChanID;

//H323 Annex Q
typedef struct
{
	u8 m_byAction;			  //start, continue, stop, video source, preset

	union 
	{
		u8 m_byRequest;		  //for PTFZ operations
		u8 m_bySourceNo;      //for VIDEO SOURCE operations
		u8 m_byPresetNo;      //for PRESET operations
	}arguments;

	u8 m_byTimeout;			  //for start action only ,unit 50ms, 0表示800ms	
}TFeccStruct, *PTFeccStruct;

typedef struct tagH323TerminalLabel 
{
	u8 m_byMcuNo;
	u8 m_byTerNo;

	tagH323TerminalLabel()
	{
		Clear();		
	}

	void Clear()
	{
		m_byMcuNo = 0xff;
		m_byTerNo = 0xff;
	}

	bool operator ==(tagH323TerminalLabel& tLabel)
	{
		if (m_byMcuNo == tLabel.m_byMcuNo && m_byTerNo == tLabel.m_byTerNo)
			return true;
		else
			return false;
	}

	bool operator !=(tagH323TerminalLabel& tLabel)
	{
		if (m_byMcuNo != tLabel.m_byMcuNo || m_byTerNo != tLabel.m_byTerNo)
			return true;
		else
			return false;
	}

	bool IsValid()
	{
		return ( m_byMcuNo < LEN_MTNUM_PERCONF && m_byTerNo < LEN_MTNUM_PERCONF);
	}

	BOOL SetTerminalLabel( u8 byMcuNo, u8 byTerNo )
	{
		if( byMcuNo < LEN_MTNUM_PERCONF && byTerNo < LEN_MTNUM_PERCONF )
		{
			m_byMcuNo = byMcuNo;
			m_byTerNo = byTerNo;
			return TRUE;
		}
		return FALSE;
	}

	void GetTerminalLabel( u8 *pMcuNo, u8 *pTerNo )
	{
		*pMcuNo = m_byMcuNo;
		*pTerNo = m_byTerNo;
	}

	u8 GetMcuNo()
	{
		return m_byMcuNo;
	}

	u8 GetTerNo()
	{
		return  m_byTerNo;
	}
}TH323TerLabel;

typedef struct 
{
	TFeccStruct m_tFeccStruct;
	TH323TerLabel   m_tSrcTer;
	TH323TerLabel   m_tDstTer;
}TH245Fecc;

//别名地址
typedef struct tagH323AliasAddr
{
	emAliasType  m_emType;
	s8			 m_achAlias[LEN_ALIAS+2];

	tagH323AliasAddr()
	{
		Clear();
	}

	void Clear()
	{
		m_emType = alias_type_aliastype;
		memset( m_achAlias, 0, sizeof(m_achAlias) );
	}

	BOOL SetAliasAddr( emAliasType aliasType, const char* pAlias )
	{
		if ( aliasType <= alias_type_aliastype || aliasType >= alias_type_others || pAlias == NULL ) 
			return FALSE;

		memset( m_achAlias, 0, sizeof(m_achAlias) );

		m_emType = aliasType;

		u16 nLen = min( strlen(pAlias), LEN_ALIAS );
		strncpy( m_achAlias, pAlias, nLen );
		m_achAlias[nLen] = '\0';
		return TRUE;
	}	

	emAliasType GetAliasType()
	{
		return m_emType;
	}

	s8* GetAliasName()  
	{
		return m_achAlias;	
	}
}TH323AliasAddr,*PTH323AliasAddr;


typedef struct tagH323StackConfigs
{
	u32				m_dwSystem_maxCalls;				    //最大呼叫数
	u32				m_dwSystem_maxChannels;					//每个呼叫打开的逻辑通道数

	emEndpointType  m_emTerminalType;						//终端类型如(emMt emMcu)
	BOOL			m_bRas_manualRAS;						//是否手动RAS操作
	BOOL			m_bRas_manualRegistration;				//是否手动RAS注册
	s8				m_achRas_defaultGkIp[4];				//缺省gk地址
	u16				m_wRas_defaultGkPort;					//缺省gk呼叫端口
	u16				m_wRas_defaultlocalPort;				//缺省本地RAS端口
	s8				m_ach_UserName[LEN_ALIAS];			    //注册帐号(E164)
	s8				m_ach_Password[LEN_ALIAS];				//注册密钥

	s32				m_nProductIdSize;
	u8				m_abyProductId[LEN_256];				//产品号
	s32				m_nVerIdSize;
	u8				m_abyVersionId[LEN_256];				//版本号
	TH323AliasAddr	m_atAlias[alias_type_others];			//别名数组

	u16				m_wQ931_CallingPort;					//h225呼叫端口
	emMsTerminalType  m_emMsTerminalType;					//主从决定终端类型   
	BOOL			m_bH245_masterSlaveManualOperation;		//是否手工进行主从确定
	BOOL			m_bH245_capabilitiesManualOperation;	//是否进行手工能力交换

	s32             m_nPortFrom;                            //端口范围
	s32             m_nPortTo;                              

	u32				m_dwStackIp;							//协议栈运行IP，网络序;

	tagH323StackConfigs()
	{
		Clear();
	}

	void Clear()
	{
		m_dwSystem_maxCalls = MAX_CALL_NUM;
		m_dwSystem_maxChannels = MIN_CHANNEL_NUM;
		m_bRas_manualRAS = TRUE;
		m_bRas_manualRegistration = TRUE;
		m_wRas_defaultGkPort = H323RASPORT;
		m_wRas_defaultlocalPort = H323RASPORT;
		m_wQ931_CallingPort = H323CALLPORT;
		m_bH245_masterSlaveManualOperation = TRUE;
		m_bH245_capabilitiesManualOperation = TRUE;

		memset(m_atAlias, 0, sizeof(m_atAlias));
		memset(m_ach_UserName, 0, sizeof(m_ach_UserName));
		memset(m_ach_Password, 0, sizeof(m_ach_Password));
		m_emTerminalType  = emEndpointTypeMT;
		m_emMsTerminalType = emMsMT;
		m_nProductIdSize  = 0;
		m_nVerIdSize      = 0;
		memset(m_abyProductId, 0, sizeof(m_abyProductId));
		memset(m_abyVersionId, 0, sizeof(m_abyVersionId));
		m_achRas_defaultGkIp[0] = 127;
		m_achRas_defaultGkIp[1] = 0;
		m_achRas_defaultGkIp[2] = 0;
		m_achRas_defaultGkIp[3] = 1;

		m_nPortFrom = 0;
		m_nPortTo   = 0;

		m_dwStackIp = 0;
	};

	void SetMaxNum( u32 dwMaxCalls, u32 dwMaxChannels )
	{
		m_dwSystem_maxCalls = (dwMaxCalls>MIN_CALL_NUM && dwMaxCalls<MAX_CALL_NUM)?dwMaxCalls:MAX_CALL_NUM;
		m_dwSystem_maxChannels = (dwMaxChannels>MIN_CHANNEL_NUM && dwMaxChannels<MAX_CHANNEL_NUM)?dwMaxChannels:MIN_CHANNEL_NUM;
	}

	u32 GetMaxCalls()
	{
		return m_dwSystem_maxCalls;
	}

	u32 GetMaxChannels()
	{
		return m_dwSystem_maxChannels;
	}

	void SetMannualRAS( BOOL bManualRAS, BOOL bManualReg )
	{
		m_bRas_manualRAS = bManualRAS;
		m_bRas_manualRegistration = bManualReg;
	}

	BOOL IsManualRAS()
	{
		return m_bRas_manualRAS;
	}

	BOOL IsManualReg()
	{
		return m_bRas_manualRegistration;
	}

	void SetTerminalType( emEndpointType emEpType, emMsTerminalType emMSTType )
	{
		m_emTerminalType = emEpType;
		m_emMsTerminalType = emMSTType;
	}

	emEndpointType GetEndpointType()
	{
		return m_emTerminalType;
	}

	emMsTerminalType GetMsTerminalType()
	{
		return m_emMsTerminalType;
	}

	void SetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyProductId, 0, sizeof(m_abyProductId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nProductIdSize = nReal;
		if(nLen >= LEN_1)
		{
			memcpy(m_abyProductId, pbyId, nReal);
		}
	}

	s32 GetProductIdSize()
	{
		return m_nProductIdSize;
	}

	s32 GetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nProductIdSize); 
		memcpy(pbyId, m_abyProductId, nReal);
		return nReal;
	}

	void SetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyVersionId, 0, sizeof(m_abyVersionId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nVerIdSize = nReal;
		if(nLen >= LEN_1)
		{
			memcpy(m_abyVersionId, pbyId, nReal);
		}
	}

	s32 GetVersionIdSize()
	{
		return m_nVerIdSize;
	}

	s32 GetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nVerIdSize); 
		memcpy(pbyId, m_abyVersionId, nReal);
		return nReal;
	}

	void SetUserInfo(s8* pszUserName, s8* pszPassword)
	{
		if (pszUserName == NULL)
		{
			return;
		}
		memset(m_ach_UserName, 0, sizeof(m_ach_UserName));
		u16 nameLen = min( strlen(pszUserName), LEN_ALIAS);
		strncpy(m_ach_UserName, pszUserName, nameLen);
		m_ach_UserName[nameLen] = '\0';

		if (pszPassword)
		{
			memset(m_ach_Password, 0, sizeof(m_ach_Password));
			u16 pwdLen = min(strlen(pszPassword), LEN_ALIAS);
			strncpy(m_ach_Password, pszPassword, pwdLen);
			m_ach_Password[pwdLen] = '\0';
		}

	}

	s8* GetUserName()
	{
		return m_ach_UserName;
	}

	s8* GetPassword()
	{
		return m_ach_Password;
	}


	void SetDefGKIP(u32 dwGkIP, u16  wGkPort = H323RASPORT)
	{
		s8 *p = (s8 *)(&dwGkIP);
		memset(m_achRas_defaultGkIp, 0, sizeof(m_achRas_defaultGkIp));
		for(s32 nIndex=0; nIndex<4; nIndex++)
		{
			m_achRas_defaultGkIp[nIndex] = *(p+nIndex);			
		}
		m_wRas_defaultGkPort = wGkPort;
	}

	u32 GetDefGKIP()
	{
		u32 dwGkIP = 0;
		s8 *p = (s8 *)(&dwGkIP);
		for(s32 nIndex=0; nIndex<4; nIndex++)
		{
			*(p+nIndex) = m_achRas_defaultGkIp[nIndex];
		}
		return dwGkIP;
	}

	u16 GetDefGKPort()
	{
		return m_wRas_defaultGkPort;
	}


	void SetAliasAddr( TH323AliasAddr &tAliasAddr, u32 dwAliasNo )
	{
		if ( dwAliasNo >= alias_type_others )
		{
			return;
		}
		m_atAlias[dwAliasNo] = tAliasAddr;
	}

	TH323AliasAddr* GetAliasAddr( u32 dwAliasNo )
	{
		if ( dwAliasNo >= alias_type_others )
		{
			return NULL;
		}
		return &(m_atAlias[dwAliasNo]);
	}


	void SetH225CallingPort( u16 wCallPort = H323CALLPORT )
	{
		m_wQ931_CallingPort = wCallPort;
	}

	u16 GetH225CallingPort()
	{
		return m_wQ931_CallingPort;
	}

	void SetLocalRASPort( u16 wRASPort = H323RASPORT )
	{
		m_wRas_defaultlocalPort = wRASPort;
	}

	u16 GetLocalRASPort()
	{
		return m_wRas_defaultlocalPort;
	}

	void SetH245IsManual( BOOL bMsdManual, BOOL bCseManual )
	{
		m_bH245_masterSlaveManualOperation  = bMsdManual;
		m_bH245_capabilitiesManualOperation = bCseManual;
	}

	BOOL IsMasterSlaveManual()
	{
		return m_bH245_masterSlaveManualOperation;
	}

	BOOL IsCapabilitiesManual()
	{
		return m_bH245_capabilitiesManualOperation;
	}

	void SetPortRange(s32 portFrom, s32 portTo)
	{
		m_nPortFrom = portFrom;
		m_nPortTo   = portTo;
	}

	s32 GetFirstPort()
	{
		return m_nPortFrom;
	}

	s32 GetLastPort()
	{
		return m_nPortTo;
	}

	void SetStackIP(u32 dwIP)
	{
		m_dwStackIp = dwIP;
	}
	u32	GetStackIP()
	{
		return m_dwStackIp;
	}
}TH323StackCfg,*PTH323StackCfg;


//transport地址
typedef struct tagH323NetAddr
{
	u32			m_dwIP;
	u16			m_wPort;
	u8			m_byType;

	tagH323NetAddr()
	{
		Clear();
	}

	void Clear()
	{
		m_dwIP	 = 0;
		m_wPort  = 0;
		m_byType = addr_unicast;
	}

	void host2net(BOOL bhton)
	{
		if (bhton)
		{
			m_dwIP  = htonl(m_dwIP);
			m_wPort = htons(m_wPort);
		}
		else
		{
			m_dwIP  = ntohl(m_dwIP);
			m_wPort = ntohs(m_wPort);
		}
	}

	void SetNetAddress( u32 dwIPAddr, u16 wIPPort, u8 byAddrType = addr_unicast )
	{
		m_dwIP   = dwIPAddr;
		m_wPort  = wIPPort;
		m_byType = byAddrType;
	}

	u32 GetIPAddr()
	{
		return m_dwIP;
	}

	u16 GetIPPort()
	{
		return m_wPort;
	}

	u8 GetNetAddrType()
	{
		return m_byType;
	}
}TH323NetAddr, *PTH323NetAddr;


//呼叫地址
typedef struct tagH323CallAddr
{
private:
	emCallAddrType		m_eType;
	TH323NetAddr		m_tIPAddr;					
	TH323AliasAddr		m_tAliasAddr;
public:
	tagH323CallAddr()
	{
		Clear();
	}

	void Clear()
	{
		m_tIPAddr.Clear();
		m_tAliasAddr.Clear();
	}

	void SetCallAddrType( emCallAddrType emAddrType )
	{
		m_eType = emAddrType;
	}

	emCallAddrType GetCallAddrType()
	{
		return m_eType;
	}

	void SetIPAddr(u32 dwIp,u16 wPort = 0)
	{
		m_tIPAddr.m_dwIP  = dwIp;
		m_tIPAddr.m_wPort = htons(wPort);
	}	

	u32 GetIPAddr()  
	{	
		return m_tIPAddr.m_dwIP;	
	}

	u16 GetIPPort()  
	{	
		return ntohs( m_tIPAddr.m_wPort );	
	}

	void SetAliasAddr( emAliasType emAliasType, const s8* sAlias )
	{
		memset( &m_tAliasAddr, 0, sizeof(m_tAliasAddr) );
		m_tAliasAddr.m_emType = emAliasType;

		if ( sAlias != NULL ) 
		{
			u16 wLen = min( strlen(sAlias), LEN_ALIAS );
			strncpy( m_tAliasAddr.m_achAlias, sAlias, wLen );
			m_tAliasAddr.m_achAlias[wLen] = '\0';
		}
	}	

	emAliasType GetAliasType()
	{
		return m_tAliasAddr.m_emType;
	}

	s8* GetAliasName()  
	{
		return m_tAliasAddr.m_achAlias;	
	}	
}TH323CallAddr;



typedef struct tagH323CallingAliasAddr
{
	emCallAddrType		m_emType;		
	TH323NetAddr		m_tNetAddr;			
	TH323AliasAddr		m_tAlias;

	tagH323CallingAliasAddr()
	{
		m_emType = cat_err;
	}

	void Clear()
	{
		m_emType = cat_err;
		m_tNetAddr.Clear();
		m_tAlias.Clear();
	}


	void SetIPCallAddr( TH323NetAddr* pTNetAddr )
	{
		if ( pTNetAddr != NULL )
		{
			m_emType = cat_ip;
			memcpy( &m_tNetAddr, pTNetAddr, sizeof(m_tNetAddr) );
		}		
	}

	u32 GetIPAddr()
	{
		if (m_emType == cat_ip) 
		{
			return m_tNetAddr.GetIPAddr();
		}
		return 0;
	}

	u16 GetIPPort()
	{
		if (m_emType == cat_ip) 
		{
			return m_tNetAddr.GetIPPort();
		}
		return 0;
	}

	void SetIPCallAddr( TH323AliasAddr *pAliasAddr )
	{
		if ( pAliasAddr != NULL )
		{
			m_emType = cat_alias;
			memcpy( &m_tAlias, pAliasAddr, sizeof(m_tAlias) );
		}		
	}

	emAliasType GetAliasType()
	{
		if (m_emType == cat_alias)
		{
			return m_tAlias.GetAliasType();
		}
		return alias_type_aliastype;
	}

	s8* GetAliasName()
	{
		if (m_emType == cat_alias)
		{
			return m_tAlias.GetAliasName();
		}
		return NULL;
	}

	emCallAddrType GetCallAddrType()
	{
		return m_emType;
	}
}TH323CallingAliasAddr;


typedef struct tagH323NodeInfo 
{
	emEndpointType			m_emEndpointType;   
	s32						m_nVenderid;					
	s8						m_chName[LEN_DISPLAY_MAX+1];	
	BOOL					m_bIsEncrypt;

	s32						m_nProductIdSize;
	u8						m_abyProductId[LEN_256];
	s32						m_nVerIdSize;
	u8						m_abyVersionId[LEN_256];

	u32						m_dwCallRate;

	TH323CallingAliasAddr	m_atCallingAliasAddr[alias_type_others+1]; 
	TH323CallingAliasAddr	m_atCalledAliasAddr[alias_type_others+1];

	tagH323NodeInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_emEndpointType = emEndpointTypeMT;
		m_nVenderid = vendor_AVCON;
		memset( m_chName, 0, sizeof(m_chName) ); 		

		m_bIsEncrypt = FALSE;

		m_nProductIdSize = 0;
		m_nVerIdSize = 0;
		memset(m_abyProductId, 0, sizeof(m_abyProductId));
		memset(m_abyVersionId, 0, sizeof(m_abyVersionId));
	}


	void SetEndpointType(emEndpointType emEpType)
	{
		m_emEndpointType = emEpType;
	}

	emEndpointType GetEndpointType()
	{
		return m_emEndpointType;
	}

	void SetVenderId( s32 nVenderId)
	{
		m_nVenderid = nVenderId;
	}

	s32 GetVenderId()
	{
		return m_nVenderid;
	}

	void SetName(s8 *pchName, s32 nLen)
	{
		if (pchName == NULL || nLen <= 0) 
			return;
		s32 realLen = min(nLen,LEN_DISPLAY_MAX);
		strncpy( m_chName, pchName, realLen );
		m_chName[realLen] = '\0';
	}

	s8* GetName()
	{
		return m_chName;
	}

	void SetIsEncrypt( BOOL bIsEncrypt )
	{
		m_bIsEncrypt = bIsEncrypt;
	}

	BOOL IsEncrypt()
	{
		return m_bIsEncrypt;
	}


	void SetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyProductId, 0, sizeof(m_abyProductId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nProductIdSize = nReal;
		if(nLen >= LEN_1)
		{
			memcpy(m_abyProductId, pbyId, nReal);
		}
	}

	s32 GetProductIdSize()
	{
		return m_nProductIdSize;
	}

	s32 GetProductId(u8 *pbyId, s32 nLen)
	{
		if( pbyId == NULL || nLen <= 0 || m_nProductIdSize <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nProductIdSize); 
		memcpy(pbyId, m_abyProductId, nReal);
		return nReal;
	}

	void SetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyVersionId, 0, sizeof(m_abyVersionId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nVerIdSize = nReal;
		if(nLen >= LEN_1)
		{
			memcpy(m_abyVersionId, pbyId, nReal);
		}
	}

	s32 GetVersionIdSize()
	{
		return m_nVerIdSize;
	}

	s32 GetVersionId(u8 *pbyId, s32 nLen)
	{
		if( pbyId == NULL || nLen <= 0 || m_nVerIdSize <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nVerIdSize); 
		memcpy(pbyId, m_abyVersionId, nReal);
		return nReal;
	}


	void SetCallingAliasaddr( TH323CallingAliasAddr &tAliasAddr, u8 addrNo )
	{
		if ( addrNo < alias_type_others + 1  )
		{
			m_atCallingAliasAddr[addrNo] = tAliasAddr;
		}
	}

	TH323CallingAliasAddr* GetCallingAliasaddr( u8 addrNo )
	{
		if ( addrNo < alias_type_others + 1  )
		{
			return &(m_atCallingAliasAddr[addrNo]);
		}
		return NULL;
	}

	void SetCalledAliasAddr( TH323CallingAliasAddr &tAliasAddr, u8 byAddrNo )
	{
		if ( byAddrNo < alias_type_others + 1  )
		{
			m_atCalledAliasAddr[byAddrNo] = tAliasAddr;
		}		
	}

	TH323CallingAliasAddr* GetCalledAliasAddr( u8 byAddrNo )
	{
		if ( byAddrNo < alias_type_others + 1  )
		{
			return &(m_atCalledAliasAddr[byAddrNo]);
		}
		return NULL;
	}

}TH323NODEINFO,*PTH323NODEINFO;


//视频能力集
/*********************关于帧率的设置******************
*1表示29.97, 2表示29.97/2, 3表示29.97/3,.....
*H261：cif(1-4) qcif(1-4)
*H263: sqcif,qcif,cif,4cif,16cif  (1-32)
*MP2/MP4: cif, 2cif, 4cif 1-30
*****************************************************/
typedef struct tagH323VideoCap
{
	cmCapDirection  m_emCapDirection;
	u16             m_wMaxBitRate;
	u8              m_abyResolution[emResolutionEnd];	
public:
	tagH323VideoCap()
	{
		Clear();
	}
	
	void Clear()
	{
		m_emCapDirection = cmCapReceive;
		m_wMaxBitRate = 0;
		memset(m_abyResolution, 0, sizeof(m_abyResolution));        
	}

    void host2net(BOOL bhton)
    {
        s32 nCapDirection = m_emCapDirection;
        if (bhton)
        {
            m_emCapDirection = (cmCapDirection)htonl(nCapDirection);
            m_wMaxBitRate    = htons(m_wMaxBitRate);
        }
        else
        {
            m_emCapDirection = (cmCapDirection)ntohl(nCapDirection);
            m_wMaxBitRate    = ntohs(m_wMaxBitRate);
        }
    }

	void SetBitRate(u16 wBitRate)
	{
		m_wMaxBitRate = wBitRate;		
	}
	u16 GetBitRate() const
	{
        return m_wMaxBitRate;
	}

	void SetCapDirection(cmCapDirection emDirect)
	{
		m_emCapDirection = emDirect;
	}
	cmCapDirection GetCapDirection() const
	{
		return m_emCapDirection;
	}

	BOOL SetResolution(emResolution emRes, u8 byMPI)
	{
		if( emRes <= emResolutionBegin || emRes >= emResolutionEnd )
		{
			return FALSE;
		}
		m_abyResolution[emRes] = byMPI;
	    return TRUE;
	}

	u8 GetResolution(emResolution emRes) const
	{
		if(emRes <= emResolutionBegin || emRes >= emResolutionEnd)
		{
			return 0;
		}
		return m_abyResolution[emRes];
	}
	
	void MergeVideoCap( const tagH323VideoCap &tVideoCap )
	{
		if ( m_wMaxBitRate != 0 )
		{
			m_wMaxBitRate = min( m_wMaxBitRate, tVideoCap.m_wMaxBitRate );
		}
		else
		{
			m_wMaxBitRate = tVideoCap.m_wMaxBitRate;
		}
		m_emCapDirection = tVideoCap.m_emCapDirection;
		for ( int nIndex = emResolutionBegin +1; nIndex < emResolutionEnd; ++nIndex )
		{
			if ( m_abyResolution[nIndex] == 0 )
			{
				m_abyResolution[nIndex] = tVideoCap.m_abyResolution[nIndex];
			}
		}
	}
}TH323VideoCap;


typedef struct tagH323H264VideoCap
{
public:
    enum  emH264ProfileMask
    {
        emProfileBaseline = 64,
        emProfileMain = 32,
        emProfileExtend = 16,
        emProfileHigh = 8
    };

    tagH323H264VideoCap()
    {
        Clear();
    }
    
    void Clear()
    {
        memset(this, 0, sizeof(tagH323H264VideoCap));
        m_emCapDirection = cmCapReceive;
        m_wMaxBitRate = 0;
        SetSupportProfile(emProfileBaseline);
        m_byLevel = 36;
		m_nParaFlag = 0;
        m_nMaxBRandCPB = 0;
    }
    
    void host2net(BOOL bhton)
    {
        s32 nCapDirection = m_emCapDirection;
        if (bhton)
        {
            m_emCapDirection = (cmCapDirection)htonl(nCapDirection);
            m_wMaxBitRate    = htons(m_wMaxBitRate);
            m_nMaxMBPS       = htonl(m_nMaxMBPS);
            m_nMaxFS         = htonl(m_nMaxFS);
            m_nMaxDPB        = htonl(m_nMaxDPB);
            m_nMaxBRandCPB   = htonl(m_nMaxBRandCPB);
            m_nMaxStaticMBPS = htonl(m_nMaxStaticMBPS);
            m_nParaFlag      = htonl(m_nParaFlag);
        }
        else
        {
            m_emCapDirection = (cmCapDirection)ntohl(nCapDirection);
            m_wMaxBitRate    = ntohs(m_wMaxBitRate);
            m_nMaxMBPS       = ntohl(m_nMaxMBPS);
            m_nMaxFS         = ntohl(m_nMaxFS);
            m_nMaxDPB        = ntohl(m_nMaxDPB);
            m_nMaxBRandCPB   = ntohl(m_nMaxBRandCPB);
            m_nMaxStaticMBPS = ntohl(m_nMaxStaticMBPS);
            m_nParaFlag      = ntohl(m_nParaFlag);
        }
    }        

    void SetCapDirection(cmCapDirection emDirect)
    {
        m_emCapDirection = emDirect;
    }
    cmCapDirection GetCapDirection() const
    {
        return m_emCapDirection;
	}

    void SetBitRate(u16 wBitrate)
    {
        m_wMaxBitRate = wBitrate;
    }
    
    u16  GetBitRate() const
    {
        return m_wMaxBitRate;
    }

    void SetSupportProfile(u8 byProfile)
    {
        m_byProfile = byProfile;
    }

    BOOL IsSupportProfile(u8 byReqProfile) const
    {
        return ((m_byProfile&byReqProfile) == byReqProfile);
    }

    u8   GetProfileValue() const
    {
        return m_byProfile;
    }
    
    inline u8   GetResolution(u8 byRes) const;
    inline u8   GetResolution(u16 wWidth, u16 wHeight) const;

    inline BOOL IsSupport(u8 byRes, u8 byFps) const;
    inline BOOL IsSupport(u16 wWidth, u16 wHeight, u8 byFps) const;

    inline void  SetResolution(u8 byRes, u8 byFps) ;
    inline void  SetResolution(u16 wWidth, u16 wHeight, u8 byFps) ;

    void  SetMaxMBPS(s32 nMax)
    {
        m_nMaxMBPS = nMax;
    }
    s32   GetMaxMBPS() const
    {
        return m_nMaxMBPS;
    }
    void  SetMaxFS(s32 nMax)
    {
        m_nMaxFS = nMax;
    }
    s32   GetMaxFS() const
    {
        return m_nMaxFS;
    }
	void  SetMaxBRandCPB(s32 nMaxBRandCPB)
	{
		m_nMaxBRandCPB = nMaxBRandCPB;
	}
	s32   GetMaxBRandCPB() const
	{
		return m_nMaxBRandCPB;
	}

    void SetLevel(u8 byLevel)
    {
        m_byLevel = byLevel;
    }
    u8 GetLevel() const
    {
        return m_byLevel;
    }

	s32 GetParaFlag()
	{
		return m_nParaFlag;
	}
    void SetParaFlag(s32 nParaFlag)
    {
        m_nParaFlag = nParaFlag;
    }

	void SetMaxStaticMBPs( s32 nStaticMBPs )
	{
		m_nMaxStaticMBPS = nStaticMBPs;
	}
	s32 GetMaxStaticMBPs() const
	{
		return m_nMaxStaticMBPS;
	}

    inline void  Res2WxH(u8 byRes, u16 &wWidth, u16 &wHeight) const;

	BOOL operator <= (const tagH323H264VideoCap& tH264VideoCap)
	{
		if((m_nMaxMBPS <= tH264VideoCap.GetMaxMBPS()) && (m_nMaxFS <= tH264VideoCap.GetMaxFS()) && ( m_byProfile | tH264VideoCap.GetProfileValue() ) == tH264VideoCap.GetProfileValue() )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void operator = (const tagH323H264VideoCap& tH264VideoCap)
	{
		memcpy(this, (void*)&tH264VideoCap, sizeof(tH264VideoCap));
	}

	BOOL operator == (const tagH323H264VideoCap& tH264VideoCap)
	{
        if(memcmp(this, (void*)&tH264VideoCap, sizeof(tH264VideoCap)) == 0)
		    	return TRUE;

		return FALSE;
	}

protected:
    cmCapDirection  m_emCapDirection;
    u16             m_wMaxBitRate;

    u8              m_byProfile;
    u8              m_byLevel;

    s32             m_nMaxMBPS;
    s32             m_nMaxFS;

    s32             m_nMaxDPB;
    s32             m_nMaxBRandCPB;
    s32             m_nMaxStaticMBPS;

	s32             m_nParaFlag;
}TH323H264VideoCap;

inline void TH323H264VideoCap::Res2WxH(u8 byRes, u16 &wWidth, u16 &wHeight) const
{
    switch (byRes)
    {
    case emCIF:
        wWidth = 352;   wHeight = 288;   break;
    case emCIF2:
        wWidth = 576;   wHeight = 352;   break;
    case emCIF4:
        wWidth = 704;   wHeight = 576;   break;
    case em352240:
        wWidth = 352;   wHeight = 240;   break;
    case em704480:
        wWidth = 704;   wHeight = 480;   break;
    case em640480:
        wWidth = 640;   wHeight = 480;   break;
    case em800600:
        wWidth = 800;   wHeight = 600;   break;
    case em1024768:
        wWidth = 1024;  wHeight = 768;   break;
    case emW4CIF:
        wWidth = 1024;  wHeight = 576;   break;
    case emHD720:
        wWidth = 1280;  wHeight = 720;   break;
    case emSXGA:
        wWidth = 1280;  wHeight = 1024;  break;
    case emUXGA:
        wWidth = 1600;  wHeight = 1200;  break;
    case emHD1080:
        wWidth = 1920;  wHeight = 1080;  break;
	case em1280x768:
		wWidth = 1280;  wHeight = 768;   break;
	case em1280x800:
		wWidth = 1280;  wHeight = 800;   break;
    default:
        wWidth = 352;   wHeight = 288;   break;
    }

    return;
}
inline u8 TH323H264VideoCap::GetResolution(u8 byRes) const
{
    u16 wWidth = 0;
    u16 wHeight = 0;
    Res2WxH(byRes, wWidth, wHeight);

    return GetResolution(wWidth, wHeight);
}

#define BYTE_CUTTER(n)  ( n>0xFF ? 0xFF : ( (n==11 || n==12 || n==14 || n==24 || n==29 || n==59) ? n+1 : n )  ) 
inline u8 TH323H264VideoCap::GetResolution(u16 wWidth, u16 wHeight) const
{    
    if (wWidth*wHeight > (m_nMaxFS)<<8)
    {
        return 0;
    }

	int nFps = 0;
	s32 nMaxFs = (wWidth*wHeight)>>8;;
	if ( ((nMaxFs>>8)<<8) < nMaxFs )
	{
		nMaxFs = (nMaxFs>>8) + 1;
		nMaxFs = nMaxFs << 8;
    }
	double dabs = (nMaxFs) - (m_nMaxFS);
	if ( m_nMaxStaticMBPS != 0 && fabs( dabs ) <= 2.0 )
	{
		double dMaxMBPS = m_nMaxMBPS;
		double dMaxFS = m_nMaxFS;
		double dMaxStaticMBPS = m_nMaxStaticMBPS;
		s32 nNewMaxMBPS = (s32)(1.0 / ( ( ( 4.0 / dMaxFS ) / dMaxMBPS ) + ( ( ( dMaxFS - 4.0 ) / dMaxFS ) / dMaxStaticMBPS ) ));
		nFps = (nNewMaxMBPS<<8) / (wWidth*wHeight);
	} 
	else
	{
		nFps = (m_nMaxMBPS<<8) / (wWidth*wHeight);
	}

    return BYTE_CUTTER(nFps);
}
#undef BYTE_CUTTER

inline BOOL TH323H264VideoCap::IsSupport(u8 byRes, u8 byFps) const
{
    return GetResolution(byRes) >= byFps;
}

inline BOOL TH323H264VideoCap::IsSupport(u16 wWidth, u16 wHeight, u8 byFps) const
{
    return GetResolution(wWidth, wHeight) >= byFps;
}

inline void TH323H264VideoCap::SetResolution(u8 byRes, u8 byFps)
{
    u16 wWidth = 0;
    u16 wHeight = 0;
    Res2WxH(byRes, wWidth, wHeight);
    
    SetResolution(wWidth, wHeight, byFps);
}
inline void TH323H264VideoCap::SetResolution(u16 wWidth, u16 wHeight, u8 byFps)
{
	//                       1  1.1 1.2 1.3 2.1 2.2 3   3.1 3.2  4  4.2   5   5.1
	const u8 abyLevel[13] = {15, 22, 29, 36, 50, 57, 64, 71, 78, 85, 99, 106, 113};

    const s32 MaxMBPSAndMaxFrame[13][2] = {{1485,99},{3000,396},{6000,396},{11880,396},{19800,792},{20250,1620},{40500,1620},
											{108000,3600},{216000,5120},{245760,8192},{522240,8704},{589824,22080},{983040,36864}};

    s32 nNewMaxFs = (wWidth*wHeight)>>8;
    if (nNewMaxFs > m_nMaxFS)
    {
        m_nMaxFS = nNewMaxFs;
        if ( ((m_nMaxFS>>8)<<8) < m_nMaxFS )
        {
            m_nMaxFS = (m_nMaxFS>>8) + 1;
			m_nMaxFS = m_nMaxFS << 8;
        }
    }
    s32 nNewMaxMBPS = nNewMaxFs*byFps;
    if (nNewMaxMBPS > m_nMaxMBPS)
    {
        m_nMaxMBPS = nNewMaxMBPS;
        if ( (m_nMaxMBPS/500*500) < m_nMaxMBPS )
        {
            m_nMaxMBPS = m_nMaxMBPS/500 + 1;
			m_nMaxMBPS = m_nMaxMBPS * 500;
        }
    }

	for(s32 i=0; i<13; i++)
	{
		if(m_nMaxMBPS >= MaxMBPSAndMaxFrame[i][0] && m_nMaxFS >= MaxMBPSAndMaxFrame[i][1])
		{
			m_byLevel = abyLevel[i];
			m_nParaFlag = 0;
			if(m_nMaxMBPS > MaxMBPSAndMaxFrame[i][0])
				m_nParaFlag = 1;
			if(m_nMaxFS > MaxMBPSAndMaxFrame[i][1])
				m_nParaFlag = m_nParaFlag | 2;
		}
		else
		{
			break;
		}
	}
}

//音频能力集，不包括G.7231
typedef struct tagH323AudioCap
{
	cmCapDirection  m_emCapDirection; //transmit or receive
	u8              m_byPackTime;     //每一RTP包的最大时长,单位ms(1-256); 
public:
	tagH323AudioCap()
	{
		Clear();
	}

	void Clear()
	{
		m_emCapDirection = cmCapReceive;
		m_byPackTime = 60;
	};

    void host2net(BOOL bhton)
    {
        s32 nCapDirection = m_emCapDirection;
        if (bhton)
        {
            m_emCapDirection = (cmCapDirection)htonl(nCapDirection);
        }
        else
        {
            m_emCapDirection = (cmCapDirection)ntohl(nCapDirection);
        }
    }

	void SetCapDirection(cmCapDirection emDirect)
	{
		m_emCapDirection = emDirect;
	}
	cmCapDirection GetCapDirection()
	{
		return m_emCapDirection;
	}
    //设置打包时间
	void SetPackTime(u8 byPackTime)
	{
		m_byPackTime = byPackTime;
	}
    //获取打包时间
	u8  GetPackTime()
	{
		return m_byPackTime;
	}	
}TH323AudioCap;

//G7231的能力集
typedef struct tagH323G7231Cap
{
	cmCapDirection  m_emCapDirection;		//transmit or receive
	u8				m_byMaxFrameNum;		//单包最大的帧数
	BOOL			m_bSilenceSuppression;	//是否静音抑制
public:
	tagH323G7231Cap()
	{
		Clear();
	}

	void Clear()
	{
		m_emCapDirection = cmCapReceive;
		m_byMaxFrameNum  = 1;			    //目前我们只支持单帧
		m_bSilenceSuppression = TRUE;	    //我们默认支持静音抑制
	}

    void host2net(BOOL bhton)
    {
        s32 nCapDirection = m_emCapDirection;
        if (bhton)
        {
            m_emCapDirection        = (cmCapDirection)htonl(nCapDirection);
            m_bSilenceSuppression   = htonl(m_bSilenceSuppression);
        }
        else
        {
            m_emCapDirection        = (cmCapDirection)ntohl(nCapDirection);
            m_bSilenceSuppression   = ntohl(m_bSilenceSuppression);
        }
    }

	void SetCapDirection(cmCapDirection emDirect)
	{
		m_emCapDirection = emDirect;
	}
	cmCapDirection GetCapDirection()
	{
		return m_emCapDirection;
	}

    //设置是否支持静音
    void SetIsSilenceSuppression(BOOL bSilenceSuppression)
    {
        m_bSilenceSuppression = bSilenceSuppression;
    }
    //是否支持静音
	BOOL IsSilenceSuppression()
	{
		return m_bSilenceSuppression;
	}
    
    //设置每包的最大帧数
	void SetMaxFrameNumPerPack(u8 byMaxFrameNum)
	{
		m_byMaxFrameNum = byMaxFrameNum;
	}
    //获取每包的最大帧数
	u8 GetMaxFrameNumPerPack()
	{
		return m_byMaxFrameNum;
	}
}TH323G7231Cap;

//MPEG AAC能力集
typedef struct tagH323AACCap
{
    enum emAACSampleFreq                    //sampling frequency
    {
        emFs96000 = 0,
        emFs88200 = 1,
        emFs64000 = 2,
        emFs48000 = 3,
        emFs44100 = 4,
        emFs32000 = 5,
        emFs24000 = 6,
        emFs22050 = 7,
        emFs16000 = 8,
        emFs12000 = 9,
        emFs11025 = 10,
        emFs8000  = 11
    };
    
    enum emAACChnlCfg                       //channel config
    {
        emChnlCust   = 0,
        emChnl1      = 1,                   //单声道
        emChnl2      = 2,                   //双声道
        emChnl3      = 4,
        emChnl4      = 8,
        emChnl5      = 16,
        emChnl5dot1  = 32,                   //5.1声道
        emChnl7dot1  = 64                    //7.1声道
    };

	cmCapDirection  m_emCapDirection;		//transmit or receive    
    emAACSampleFreq m_emSampleFreq;
    emAACChnlCfg    m_emChnlCfg;
    u16             m_wBitrate;
    u8              m_byMaxFrameNum;

public:
    tagH323AACCap()
    {
        Clear();
    }
    
    void Clear()
    {
        m_emCapDirection = cmCapReceive;
        m_emSampleFreq = emFs32000;
        m_emChnlCfg = emChnlCust;
        m_byMaxFrameNum  = 1;			    //目前我们只支持单帧
        m_wBitrate = 96;                    //kbps
    }
    
    void host2net(BOOL bhton)
    {
        s32 nCapDirection = m_emCapDirection;
        if (bhton)
        {
            m_emCapDirection        = (cmCapDirection)htonl(nCapDirection);
            m_emSampleFreq          = (emAACSampleFreq)htonl(m_emSampleFreq);
            m_emChnlCfg             = (emAACChnlCfg)htonl(m_emChnlCfg);
            m_wBitrate              = htons(m_wBitrate);
        }
        else
        {
            m_emCapDirection        = (cmCapDirection)ntohl(nCapDirection);
            m_emSampleFreq          = (emAACSampleFreq)ntohl(m_emSampleFreq);
            m_emChnlCfg             = (emAACChnlCfg)ntohl(m_emChnlCfg);
            m_wBitrate              = ntohs(m_wBitrate);
        }
    }
    
    void SetCapDirection(cmCapDirection emDirect)
    {
        m_emCapDirection = emDirect;
    }
    cmCapDirection GetCapDirection() const
    {
        return m_emCapDirection;
	}

    void SetSampleFreq(emAACSampleFreq emFs)
    {
        m_emSampleFreq= emFs;
    }

    emAACSampleFreq GetSampleFreq() const
    {
        return m_emSampleFreq;
    }

    void SetChnl(emAACChnlCfg emChnl)
    {
        m_emChnlCfg = emChnl;
    }

    emAACChnlCfg GetChnl() const
    {
        return m_emChnlCfg;
    }

    void SetBitrate(u16 wBitrate) 
    {
        m_wBitrate = wBitrate;
    }

    u16 GetBitrate() const
    {
        return m_wBitrate;
    }
}TH323AACCap;

//数据能力集
typedef struct tagH323DataCap
{
	cmCapDirection  m_emCapDirection;				//transmit or receive
	u32             m_dwMaxBitRate;					//单位kbps; 
public:
	tagH323DataCap()
	{
		Clear();
	}

	void Clear()
	{
		m_dwMaxBitRate = 64;
		m_emCapDirection = cmCapReceiveAndTransmit;//数据信道
	}

	void host2net(BOOL bhton)
	{
		s32 nCapDirection = m_emCapDirection;
		if (bhton)
		{
			m_emCapDirection = (cmCapDirection)htonl(nCapDirection);
			m_dwMaxBitRate   = htonl(m_dwMaxBitRate);
		}
		else
		{
			m_emCapDirection = (cmCapDirection)ntohl(nCapDirection);
			m_dwMaxBitRate   = ntohl(m_dwMaxBitRate);
		}
	}

	void SetCapDirection(cmCapDirection emDirect)
	{
		m_emCapDirection = emDirect;
	}
	cmCapDirection GetCapDirection()
	{
		return m_emCapDirection;
	}

	//单位kbps
	void SetBitRate(u32 dwBitRate)
	{
		m_dwMaxBitRate = dwBitRate;		
	}
	u32 GetBitRate()
	{
		return m_dwMaxBitRate;
	}		
}TH323DataCap;

typedef struct
{
	unsigned char m_abyMacAddress[6];
	unsigned char m_abyTime[8];
	BOOL m_bClock_seq_change;
} uCID;

typedef struct
{
	u32		m_dwStartTime;
	u32		m_dwStartMilliTime;
	u32		m_dwOldMilliTime;
	u8		m_abyLastTime[8];
	BOOL	m_bNotStart;
	BOOL	m_bGenerated;
	uCID	m_tUCID;
	u8  	m_abyCid[16];
	u8  	m_abyOldCid[16];
	u16     m_wClock_seq;
} cidLocalStorage;

u8* GetH323GUID(void);

typedef struct tagH323CallParam
{
	cmConferenceGoalType  m_emConfGoalType;  				  //会议的目标类型
	cmCallType            m_emCallType;						  //呼叫类型 
	emEndpointType        m_emEndpointType;					  //终端类型  表示呼入终端的类型
	u32					  m_dwCallRate;						  //呼叫速率	(kbps)
	TH323CallingAliasAddr m_tCalledAddr;					  //被叫地址。 发送时,可设置为别名地址，接收时为被叫信令地址
	TH323CallingAliasAddr m_tCallingAddr;					  //主叫地址   主叫信令地址
	TH323CallingAliasAddr m_atCallingAliasAddr[alias_type_others+1];//主叫别名地址(别名地址+IP) ，缺省采用配置文件的设置
	TH323CallingAliasAddr m_atCalledAliasAddr[alias_type_others+1]; //被叫别名地址(别名地址+IP)
	u8					  m_abyConfId[LEN_16];		      //会议号,必须16 octet
	u8					  m_abyCallId[LEN_16];  
	s8					  m_achDisplay[LEN_DISPLAY_MAX+1];	  //主叫信息

	//UserInfo字段暂不使用
	s32					  m_nUUSize; 
	u8					  m_abyUserInfo[LEN_USERINFO_MAX];	  //UserInfo消息长度：2-131字节

	//代替原来的UserInfo字段
	s32					  m_nNonStandLen;
	u8				      m_abyNonStandData[LEN_NONSTANDARD_MAX];


	//2007.05.30 
	s32				      m_nProductIdSize;
	u8				      m_abyProductId[LEN_256];				//产品号
	s32				      m_nVerIdSize;
	u8				      m_abyVersionId[LEN_256];				//版本号

	tagH323CallParam()
	{
		Clear();	
		CreateConfId();
	}

	void Clear()
	{
		m_emConfGoalType = cmCreate;
		m_emCallType     = cmCallTypeP2P; 
		m_emEndpointType = emEndpointTypeUnknown;
		m_dwCallRate = 0;
		memset(m_atCallingAliasAddr,0,sizeof(m_atCallingAliasAddr));
		memset(m_atCalledAliasAddr,0,sizeof(m_atCalledAliasAddr));
		memset(&m_tCalledAddr, 0, sizeof(m_tCalledAddr));
		memset(&m_tCallingAddr, 0, sizeof(m_tCallingAddr));
		m_tCalledAddr.m_tNetAddr.m_wPort = H323CALLPORT;
		memset( m_abyConfId, 0, sizeof(m_abyConfId));		
		memset( m_abyCallId, 0, sizeof(m_abyCallId));
		memset( m_achDisplay,0, sizeof(m_achDisplay));
		memset( m_abyUserInfo,0,sizeof(m_abyUserInfo));
		m_nUUSize = 0;
		m_nProductIdSize = 0;
		m_nVerIdSize = 0;
		memset(m_abyProductId, 0, sizeof(m_abyProductId));
		memset(m_abyVersionId, 0, sizeof(m_abyVersionId));

		m_nNonStandLen = 0;
		memset( m_abyNonStandData, 0, LEN_NONSTANDARD_MAX );
	}

	void SetEndpointType(emEndpointType emType)
	{
		m_emEndpointType = emType;
	}

	emEndpointType GetEndpointType()
	{
		return m_emEndpointType;
	}

	void SetCallRate( u32 dwCallRate )
	{
		m_dwCallRate = dwCallRate;
	}

	u32 GetCallRate()
	{
		return m_dwCallRate;
	}

	void SetCalledAddr(u32 dwIp,u16 wPort = H323CALLPORT)
	{
		TH323NetAddr addr;
		addr.SetNetAddress( dwIp, wPort==0 ? H323CALLPORT : wPort );
		m_tCalledAddr.SetIPCallAddr( &addr );
	}

	u32 GetCalledIp()
	{
		return m_tCalledAddr.GetIPAddr(); 
	}	

	u16 GetCalledPort() 
	{ 
		return m_tCalledAddr.GetIPPort(); 
	}

	void SetCalledAddr( emAliasType eType,const s8* sAlias)
	{
		TH323AliasAddr aliasAddr;
		aliasAddr.SetAliasAddr( eType, sAlias );
		m_tCalledAddr.SetIPCallAddr( &aliasAddr );
	}

	TH323CallingAliasAddr& GetCalledAddr()
	{
		return m_tCalledAddr;
	}

	void SetCallingAddr( u32 dwIp,u16 wPort = 0 )
	{
		TH323NetAddr addr;
		addr.SetNetAddress( dwIp, wPort == H323CALLPORT ? 0 : wPort );
		m_tCallingAddr.SetIPCallAddr( &addr );
	}

	u32 GetCallingIp()
	{
		return m_tCallingAddr.GetIPAddr();	
	}	

	u16 GetCallingPort()
	{
		return m_tCallingAddr.GetIPPort();	
	}

	void SetCallingAddr( emAliasType eType,const char* sAlias )
	{
		TH323AliasAddr aliasAddr;
		aliasAddr.SetAliasAddr( eType, sAlias );
		m_tCallingAddr.SetIPCallAddr( &aliasAddr );
	}

	TH323CallingAliasAddr& GetCallingAddr()
	{
		return m_tCallingAddr;
	}   


	void SetCallingAliasaddr( TH323CallingAliasAddr &tAliasAddr, u8 addrNo )
	{
		if ( addrNo < alias_type_others + 1  )
		{
			m_atCallingAliasAddr[addrNo] = tAliasAddr;
		}
	}

	TH323CallingAliasAddr* GetCallingAliasaddr( u8 addrNo )
	{
		if ( addrNo < alias_type_others + 1  )
		{
			return &(m_atCallingAliasAddr[addrNo]);
		}
		return NULL;
	}

	void SetCalledAliasAddr( TH323CallingAliasAddr &tAliasAddr, u8 byAddrNo )
	{
		if ( byAddrNo < alias_type_others + 1  )
		{
			m_atCalledAliasAddr[byAddrNo] = tAliasAddr;
		}		
	}

	TH323CallingAliasAddr* GetCalledAliasAddr( u8 byAddrNo )
	{
		if ( byAddrNo < alias_type_others + 1 )
		{
			return &(m_atCalledAliasAddr[byAddrNo]);
		}
		return NULL;
	}

	void SetConfId(const u8* pchConfId,u8 byLen)
	{
		if( pchConfId == NULL || byLen <= 0 )
		{
			return;
		}

		s32 nReal = min( max( LEN_1, byLen ), LEN_16 );
		memcpy( m_abyConfId, pchConfId, nReal );
	}

	s32 GetConfIdSize()
	{
		return LEN_16;
	};

	s32 GetConfId(u8* pbyConfId, s32 nLen)
	{
		if( pbyConfId == NULL || nLen <= 0 )
		{
			return 0;
		}

		s32 nReal = min(max(LEN_1, nLen), LEN_16 );
		memcpy( pbyConfId, m_abyConfId, nReal );
		return nReal;
	}


	void CreateConfId()
	{
		memcpy( m_abyConfId, GetH323GUID(), LEN_16 );
	}


	void CreateCallId()
	{
		memcpy(m_abyCallId, GetH323GUID(), LEN_16 );
	}

	void SetCallId(const u8* pbyCallId, u8 byLen)
	{
		if( pbyCallId == NULL || byLen <= 0 )
		{
			return;
		}
		CreateCallId();
		s32 nReal = min( max(LEN_1, byLen), LEN_16 );
		memcpy(m_abyCallId, pbyCallId, nReal);
	}

	s32 GetCallIdSize()
	{
		return LEN_16;
	};

	s32 GetCallId(u8* pbyCallId, s32 nLen)
	{
		if(pbyCallId == NULL|| nLen <= 0 )
		{
			return 0;
		}

		s32 nReal = min(max(LEN_1, nLen), LEN_16);
		memcpy(pbyCallId, m_abyCallId, nReal);
		return nReal;
	}

	void SetDisplayInfo(const s8* pchInfo)
	{
		if (pchInfo == NULL|| strlen(pchInfo) < LEN_DISPLAY_MIN)
		{
			return ;
		}
		strncpy( m_achDisplay, pchInfo, LEN_DISPLAY_MAX );
		m_achDisplay[LEN_DISPLAY_MAX] = 0;
	}

	s8* GetDisplayInfo()
	{
		return m_achDisplay;
	}


	void SetUserInfo(const u8* pchUserInfo, u8 byLen)
	{
		if( pchUserInfo == NULL || byLen < LEN_USERINFO_MIN )
		{
			return ;
		}
		u8 length = min(LEN_USERINFO_MAX, byLen);
		memcpy(m_abyUserInfo,pchUserInfo,length);
		m_nUUSize = length;
	}	

	s32 GetUserInfoSize()
	{
		return m_nUUSize;
	}

	s32 GetUserInfo(u8* pbyUserInfo, u8 byLen)
	{
		if(pbyUserInfo == NULL || byLen < 1)
		{
			return 0;
		}
		s32 nReal = min(byLen, m_nUUSize);
		memcpy(pbyUserInfo, m_abyUserInfo, nReal);
		return nReal;
	}	

	BOOL SetNonStandData(u8 *pBuf, s32 nLen)
	{
		if(nLen < 0 ||nLen > LEN_NONSTANDARD_MAX)
		{
			return  FALSE;
		}
		memcpy( m_abyNonStandData, pBuf, nLen );
		m_nNonStandLen = nLen;
		return TRUE;
	}

	BOOL GetNonStandData(u8 **ppBuf, s32 *pnLen)
	{
		if(ppBuf == NULL|| pnLen == NULL)
		{
			return FALSE;
		}
		*ppBuf = m_abyNonStandData;
		*pnLen = m_nNonStandLen;
		return TRUE;
	}


	void SetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyProductId, 0, sizeof(m_abyProductId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nProductIdSize = nReal;
		if(nLen >= LEN_1)
		{
			memcpy(m_abyProductId, pbyId, nReal);
		}
	}

	s32 GetProductIdSize()
	{
		return m_nProductIdSize;
	}

	s32 GetProductId(u8 *pbyId, s32 nLen)
	{
		if( pbyId == NULL || nLen <= 0 || m_nProductIdSize <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nProductIdSize); 
		memcpy(pbyId, m_abyProductId, nReal);
		return nReal;
	}

	void SetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyVersionId, 0, sizeof(m_abyVersionId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nVerIdSize = nReal;
		if(nLen >= LEN_1)
		{
			memcpy(m_abyVersionId, pbyId, nReal);
		}
	}

	s32 GetVersionIdSize()
	{
		return m_nVerIdSize;
	}

	s32 GetVersionId(u8 *pbyId, s32 nLen)
	{
		if( pbyId == NULL || nLen <= 0 || m_nVerIdSize <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nVerIdSize); 
		memcpy(pbyId, m_abyVersionId, nReal);
		return nReal;
	}

}TH323CALLPARAM,*PTH323CALLPARAM;


//加密同步结构  
typedef struct tagEncryptSync
{
	s32			  m_nSynchFlag;			         //shall be the Dynamic PayloadType for H.323
	u8			  m_abyH235Key[LEN_H235KEY_MAX]; //H.235 encoded value(1-65535)
	s32           m_nH235KeyLen;

	tagEncryptSync()
	{
		Reset();
	}

	void Reset()
	{
		m_nSynchFlag  = 0;
		m_nH235KeyLen = 0;
	}

	void host2net(BOOL bhton)
	{
		if (bhton)
		{
			m_nSynchFlag  = htonl(m_nSynchFlag);
			m_nH235KeyLen = htonl(m_nH235KeyLen);
		}
		else
		{
			m_nSynchFlag  = ntohl(m_nSynchFlag);
			m_nH235KeyLen = ntohl(m_nH235KeyLen);
		}
	}

	//设置SynchFlag
	void SetSynchFlag(s32 nFlag)
	{ 
		m_nSynchFlag = nFlag; 
	}
	//获取SynchFlag
	s32  GetSynchFlag() 
	{
		return m_nSynchFlag; 
	}

	//设置H235Key
	s32 SetH235Key(u8 *pBuf, s32 nLen)
	{
		m_nH235KeyLen = min(max(0, nLen), LEN_H235KEY_MAX);
		memcpy(m_abyH235Key, pBuf, m_nH235KeyLen);
		return m_nH235KeyLen;
	}	
	//获取H235Key的长度
	s32 GetH235KeyLen() 
	{
		return m_nH235KeyLen; 
	}
	//获取H235Key
	u8* GetH235Key()
	{
		return m_abyH235Key;
	}
}TEncryptSync;


//打开信道参数
typedef struct tagH323ChannelParam 
{
	TH323NetAddr	  m_tRtp;				//rtp地址 ,incoming chan使用
	TH323NetAddr	  m_tRtcp;				//rtcp地址,incoming and outgoing chan使用
	u16				  m_wPayloadType;		//媒体通道类型	

	TH323VideoCap	  m_tVideoCap;
	TH323AudioCap	  m_tAudioCap;
	TH323G7231Cap	  m_tG7231Cap;

	u8				  m_byDynamicPT;		//动态载荷类型值(96~127)      
	BOOL			  m_bIsH239DStream;     //在打开视频通道时指明是否H239协议的双流

	TH323H264VideoCap m_tH264Cap;           //H.264能力视频能力，与m_tVideoCap互斥，只需填写一个即可
	TH323AACCap       m_tAacCap;            //AAC LC/LD 音频能力

	void host2net(BOOL bhton)
	{
		m_tRtp.host2net(bhton);
		m_tRtcp.host2net(bhton);
		m_wPayloadType = htons(m_wPayloadType);
		m_tVideoCap.host2net(bhton);
		m_tH264Cap.host2net(bhton);
		m_tAudioCap.host2net(bhton);
		m_tG7231Cap.host2net(bhton);
		m_tAacCap.host2net(bhton);
		m_bIsH239DStream = htonl(m_bIsH239DStream);
	}

public:
	tagH323ChannelParam()
	{
		Reset();
	};

	void Reset()
	{
		m_tRtp.Clear();
		m_tRtcp.Clear();

		m_wPayloadType = emPayloadTypeBegin;
		m_byDynamicPT   = 0;  

		m_bIsH239DStream  = FALSE;
	}

	void SetRtp(TH323NetAddr& rtp) 
	{ 
		m_tRtp = rtp; 
	}

	TH323NetAddr& GetRtp() 
	{
		return m_tRtp; 
	};

	void SetRtcp(TH323NetAddr& rtcp) 
	{
		m_tRtcp = rtcp; 
	}

	TH323NetAddr& GetRtcp() 
	{
		return m_tRtcp; 
	}

	BOOL SetDynamicPayloadParam( u8 byDynamicPT )
	{
		if ( byDynamicPT>=96 && byDynamicPT<=127 ) 
		{
			m_byDynamicPT = byDynamicPT;
			return TRUE;
		}
		return FALSE;
	}
	u8 GetDynamicPayloadParam()
	{
		return m_byDynamicPT;
	} 

	BOOL IsDynamicPayload()	
	{ 
		return (m_byDynamicPT >= 96)&&(m_byDynamicPT <= 127); 
	}

	u16 GetPayloadType()
	{
		return m_wPayloadType; 
	}

	BOOL SetPayloadParam(u16 wPayloadType, TH323VideoCap &tVideoCap)
	{
		if ( !IsVideo(wPayloadType) || wPayloadType == v_h264 )
		{
			return FALSE;
		}
		m_tVideoCap = tVideoCap;
		m_wPayloadType = wPayloadType;
		return TRUE;
	}

	BOOL GetPayloadParam(u16 wPayloadType, TH323VideoCap *ptVideoCap)
	{
		if( !IsVideo(wPayloadType) || wPayloadType == v_h264 || ptVideoCap == NULL )
		{
			return FALSE;
		}
		*ptVideoCap = m_tVideoCap;
		return TRUE;
	}

	BOOL SetPayloadParam(u16 wPayloadType, TH323H264VideoCap &tVideoCap)
	{
		if ( wPayloadType != v_h264 )
		{
			return FALSE;
		}
		m_tH264Cap = tVideoCap;
		m_wPayloadType = wPayloadType;
		return TRUE;
	}

	BOOL GetPayloadParam(u16 wPayloadType, TH323H264VideoCap *ptVideoCap)
	{        
		if( wPayloadType != v_h264 || ptVideoCap == NULL )
		{
			return FALSE;
		}
		*ptVideoCap = m_tH264Cap;
		return TRUE;
	}

	BOOL SetPayloadParam(u16 wPayloadType, TH323AudioCap &tAudioCap)
	{
		if ( !IsAudio(wPayloadType) || 
			wPayloadType == a_g7231 ||
			wPayloadType == a_mpegaaclc || 
			wPayloadType == a_mpegaacld )
		{
			return FALSE;
		}
		m_tAudioCap = tAudioCap;
		m_wPayloadType = wPayloadType;
		return TRUE;
	}

	BOOL GetPayloadParam(u16 wPayloadType, TH323AudioCap *ptAudioCap)
	{
		if ( !IsAudio(wPayloadType) || 
			wPayloadType == a_g7231 || 
			wPayloadType == a_mpegaaclc || 
			wPayloadType == a_mpegaacld || 
			ptAudioCap == NULL ) 
		{
			return FALSE;
		}
		*ptAudioCap = m_tAudioCap;
		return TRUE;
	}

	BOOL SetPayloadParam(u16 wPayloadType, TH323G7231Cap &tG7231Cap)
	{
		if(wPayloadType != a_g7231)
		{
			return FALSE;
		}
		m_tG7231Cap = tG7231Cap;
		m_wPayloadType = wPayloadType;
		return TRUE;
	}

	BOOL GetPayloadParam(u16 wPayloadType, TH323G7231Cap *ptG7231Cap)
	{
		if(wPayloadType  != a_g7231|| ptG7231Cap == NULL )
		{
			return FALSE;
		}
		*ptG7231Cap = m_tG7231Cap;
		return TRUE;
	}

	BOOL SetPayloadParam(u16 wPayloadType, TH323AACCap &tAacCap)
	{
		if (wPayloadType != a_mpegaaclc && wPayloadType  != a_mpegaacld)
		{
			return FALSE;
		}
		m_tAacCap = tAacCap;
		m_wPayloadType = wPayloadType;
		return TRUE;
	}

	BOOL GetPayloadParam(u16 wPayloadType, TH323AACCap *ptAacCap)
	{
		if((wPayloadType != a_mpegaaclc && wPayloadType  != a_mpegaacld) || ptAacCap == NULL )
		{
			IPCPrintf(1, 0, "[H323Adp][TCHANPARAM::GetPayloadParam] Payload param(%d) error! TAACCap\n", wPayloadType);
			return FALSE;
		}
		*ptAacCap = m_tAacCap;
		return TRUE;
	}
}TH323ChannelParam, *PTH323ChannelParam;


typedef enum
{
	emEncryptTypeNone = 1,
	emEncryptTypeDES  = 2,
	emEncryptTypeAES  = 4,
	emEncryptTypeH239 = 8,
}emEncryptType;

inline u16 GetEType( IN u16 wType, IN u8 byEBits) 
{
	u16 wTmp = byEBits;
	wTmp = ((wTmp<<8)&0xff00);
	wTmp |= wType;
	return wTmp;
};

inline u16 GetH239Type( IN u16 wType ) 
{
	return GetEType(wType, emEncryptTypeH239);
};

inline BOOL IsH239Type( IN u16 wType )
{  
	return ((wType>>8) & emEncryptTypeH239);
}


typedef struct tagH323CapSet
{
private:
	u16 m_awCapTable[MAX_CAPTABLE_SIZE]; //PayloadType array
	u16 m_aaawCapDesc[MAX_DESCAP_NUM][MAX_SIMUCAP_NUM][MAX_ALTCAP_NUM]; //PayloadType array
	
	TH323VideoCap m_tH261Cap;
	TH323VideoCap m_tH263Cap;
	TH323VideoCap m_tH263plusCap;
	TH323VideoCap m_tMpeg4Cap;
    TH323VideoCap m_tH262Cap;    

	TH323AudioCap m_tG711u;   
	TH323AudioCap m_tG711a;   
	TH323AudioCap m_tG722;    
	TH323AudioCap m_tG728;	   
	TH323AudioCap m_tG729;
	TH323AudioCap m_tMP3;
	TH323AudioCap m_tG719;
    TH323AACCap   m_tAaclc;
    TH323AACCap   m_tAacld;		
	TH323G7231Cap m_tG7231;

    BOOL      m_bIsSupportH239;
	BOOL      m_bSupportActiveH263Plus;
    TH323H264VideoCap m_atH264Cap[MAX_H264CAP_NUM];
	u8            m_awH264ToSubId[MAX_CAPTABLE_SIZE];
	u8            m_bySubId;

public:
	tagH323CapSet()
	{
		Clear();
	}

	void Clear()
	{
		memset( m_awCapTable, 0, sizeof(m_awCapTable) ); 
		memset( m_aaawCapDesc, 0xFF, sizeof(m_aaawCapDesc) ); 

		memset(m_awH264ToSubId, 0xFF, sizeof(m_awH264ToSubId));

		m_tH261Cap.Clear();
		m_tH263Cap.Clear();
		m_tH263plusCap.Clear();
		for (u8 byLoop = 0; byLoop < MAX_H264CAP_NUM; byLoop++)
        {
            m_atH264Cap[byLoop].Clear();
        }
		m_tMpeg4Cap.Clear();
        m_tH262Cap.Clear();
	
		m_tG711u.Clear();   
		m_tG711a.Clear();   
		m_tG722.Clear();    
		m_tG728.Clear();	   
		m_tG729.Clear();
		m_tMP3.Clear();
		m_tG719.Clear();
        m_tAaclc.Clear();
        m_tAacld.Clear();
		m_tG7231.Clear();

        m_bIsSupportH239 = FALSE;
		m_bSupportActiveH263Plus = FALSE;
		m_bySubId = 0;
	}

    void host2net(BOOL bhton)
    {
        for(s32 nIndex = 0; nIndex<MAX_CAPTABLE_SIZE; nIndex++)
        {
            if (m_awCapTable[nIndex] == 0)
                break;
            
            if (bhton) 
            {
                m_awCapTable[nIndex] = htons(m_awCapTable[nIndex]);
            }
            else
            {
                m_awCapTable[nIndex] = ntohs(m_awCapTable[nIndex]);
            }
        }
        
        s32 nDesCap = 0;
        s32 nSimuCap = 0;
        s32 nAltCap = 0;
        for(nDesCap = 0; nDesCap < MAX_DESCAP_NUM; nDesCap++)
        {
            for(nSimuCap = 0; nSimuCap < MAX_SIMUCAP_NUM; nSimuCap++)
            {
                for(nAltCap = 0; nAltCap < MAX_ALTCAP_NUM; nAltCap++)
                {
                    if (bhton)
                    {
                        m_aaawCapDesc[nDesCap][nSimuCap][nAltCap] 
                            = htons(m_aaawCapDesc[nDesCap][nSimuCap][nAltCap]);
                    }
                    else
                    {
                        m_aaawCapDesc[nDesCap][nSimuCap][nAltCap] 
                            = ntohs(m_aaawCapDesc[nDesCap][nSimuCap][nAltCap]);
                    }
                }
            }
        }
        
        
        m_tH261Cap.host2net(bhton);
        m_tH263Cap.host2net(bhton);
        m_tH263plusCap.host2net(bhton);
        for (u8 byLoop = 0; byLoop < MAX_H264CAP_NUM; byLoop++)
        {
            m_atH264Cap[byLoop].host2net(bhton);
        }
        
        m_tMpeg4Cap.host2net(bhton);
        m_tH262Cap.host2net(bhton);
        
        m_tG711u.host2net(bhton);
        m_tG711a.host2net(bhton);
        m_tG722.host2net(bhton);
        m_tG728.host2net(bhton);
        m_tG729.host2net(bhton);
        m_tMP3.host2net(bhton); 
		m_tG719.host2net(bhton);
        m_tG7231.host2net(bhton);
        m_tAaclc.host2net(bhton);
        m_tAacld.host2net(bhton);
        
        m_bIsSupportH239 = htonl(m_bIsSupportH239);
    }


    void SetIsSupportActiveH263(BOOL bSupport)
    {
        m_bSupportActiveH263Plus = bSupport;
    }
    BOOL GetIsSupportActiveH263()
    {
        return m_bSupportActiveH263Plus;
    }

    void SetSupportH239(BOOL bSupporth239)
    {
        m_bIsSupportH239 = bSupporth239;
    }
    BOOL IsSupportH239()
    {
        return m_bIsSupportH239;
    }
	
	BOOL SetCapDesc(u8 byCapIndex, u8 bySimuIndex, u8 byAltIndex, u16 wPayload, u8 byH264SubId = 0)
	{
		u16 wRealPt = wPayload&0xff;

		if( byCapIndex >= MAX_DESCAP_NUM || bySimuIndex >= MAX_SIMUCAP_NUM
			|| byAltIndex >= MAX_ALTCAP_NUM )
		{
			return FALSE;
		}

        if ( wRealPt == v_h264 && byH264SubId >= MAX_H264CAP_NUM )
        {
            return FALSE;
        }

		u16 byCapTableId = AddCapToTable(wPayload, byH264SubId);
		m_aaawCapDesc[byCapIndex][bySimuIndex][byAltIndex] = byCapTableId;
		if((wPayload&0xff) == v_h264)
		{
			m_awH264ToSubId[byCapTableId] = byH264SubId;
		}
		
		return TRUE;
	}

	u16 GetCapDescId(u8 byCapIndex, u8 bySimuIndex, u8 byAltIndex)
	{
		if(byCapIndex >= MAX_DESCAP_NUM || bySimuIndex >= MAX_SIMUCAP_NUM
			|| byAltIndex >= MAX_ALTCAP_NUM )
		{
			return 0xFFFF;
		}
		return m_aaawCapDesc[byCapIndex][bySimuIndex][byAltIndex];		
	}
    
	BOOL IsAllSupport(u16 *pwPayload, s32 nNum, TH323H264VideoCap* ptH264Cap=NULL, TH323AACCap *ptAacCap = NULL )
	{
		TH323H264VideoCap tH264;
		s32 nH264Num=0;
		s32 anH264Input[MAX_H264CAP_NUM];
		if(ptH264Cap != NULL)
		{
			memset(anH264Input, 0xFF, MAX_H264CAP_NUM);
			s32 i;
			for(i=0; i<nNum; i++)
			{
				if((pwPayload[i]&0xff) == v_h264)
				{
					nH264Num++;
				}
			}
		}
		

        u16 m_awInterPayload[MAX_SIMUCAP_NUM]; //保存外部载荷类型的数组
        for(s32 nInterIndex = 0; nInterIndex < nNum; nInterIndex++)
        {
            m_awInterPayload[nInterIndex] = pwPayload[nInterIndex];
        }

        BOOL bRes = FALSE;
        for(s32 nH263Num = 0; nH263Num < 3; nH263Num++)
        {
            bRes = IsInterAllSupport(m_awInterPayload, nNum, ptH264Cap, nH264Num, ptAacCap );

			if ( bRes == TRUE )
            {
				break;
            }
            
            //如果判断h263能力集失败，再判断h263plus能力集
            s32 nIndex = 0;
            for(nIndex = 0; nIndex < nNum; nIndex++) 
            {
                if(m_awInterPayload[nIndex] == v_h263)
                {
                    m_awInterPayload[nIndex] = v_h263plus;
                    break;
                }
                else if(m_awInterPayload[nIndex] == GetH239Type(v_h263))
                {
                    m_awInterPayload[nIndex] = GetH239Type(v_h263plus);
                    break;
                }
            }

            if (nIndex == nNum) //没有H263能力
                return bRes;
        }
        return bRes;
	}
	

	u16 GetCapFromTable(u8 byCapIndex)
	{
		if(byCapIndex >= MAX_CAPTABLE_SIZE)
			return 0;
		return m_awCapTable[byCapIndex];
	};
	void SetCapToTable(u8 byCapIndex, u16 wPayload)
	{
		if(byCapIndex >= MAX_CAPTABLE_SIZE)
			return;
		m_awCapTable[byCapIndex] = wPayload;
	}

	BOOL SetH323VideoCap(TH323VideoCap &tVideoCap, u16 wPayload)
	{
		switch(wPayload)
		{
		case v_h261:	 m_tH261Cap		= tVideoCap; break;
		case v_h263:	 m_tH263Cap		= tVideoCap; break;
		case v_h263plus: m_tH263plusCap = tVideoCap; break;		
		case v_mpeg4:	 m_tMpeg4Cap	= tVideoCap; break;
        case v_h262:	 m_tH262Cap	    = tVideoCap; break;
            
        case v_h264:
		default: 
            return FALSE;
		}
		return TRUE;
	}

    TH323VideoCap* GetH323VideoCap(u16 wPayload)
    {
        TH323VideoCap *ptVideoCap = NULL;
        switch(wPayload)
        {
        case v_h261:	 ptVideoCap = &m_tH261Cap;		break;
        case v_h263:	 ptVideoCap = &m_tH263Cap;		break;
        case v_h263plus: ptVideoCap = &m_tH263plusCap;  break;        
        case v_mpeg4:	 ptVideoCap = &m_tMpeg4Cap;		break;
        case v_h262:	 ptVideoCap = &m_tH262Cap;		break;
        
        case v_h264:
        default:break;
        }
        return ptVideoCap;
    }

    u8 SetH264VideoCap(TH323H264VideoCap &tVideoCap)
    {       
        if (m_bySubId >= MAX_H264CAP_NUM)
        {
            return FALSE;
        }
		u8 byResult = m_bySubId;
		if(FindH264VideoCap(tVideoCap, byResult) == FALSE)
		{
			m_atH264Cap[m_bySubId++]= tVideoCap;
		}

		return byResult;
    }

    TH323H264VideoCap* GetH264VideoCap(u8 bySubId)
    {
        if (bySubId >= MAX_H264CAP_NUM)
        {
            return NULL;
        }
        return &(m_atH264Cap[bySubId]);
    }

	u8 GetH264Num()
	{
		return m_bySubId;
	}


	TH323AudioCap* GetH323AudioCap(u16 wPayload)
	{
		TH323AudioCap *ptAudioCap = NULL;
		switch(wPayload)
		{
		case a_g711u: ptAudioCap = &m_tG711u; break;
		case a_g711a: ptAudioCap = &m_tG711a; break;
		case a_g722:  ptAudioCap = &m_tG722;  break;
		case a_g728:  ptAudioCap = &m_tG728;  break;
		case a_g729:  ptAudioCap = &m_tG729;  break;
		case a_g719:  ptAudioCap = &m_tG719;  break;
		default:break;
		}
		return ptAudioCap;
	}

	BOOL SetH323AudioCap(TH323AudioCap &tAudioCap, u16 wPayload)
	{
		switch(wPayload)
		{
		case a_g711u: m_tG711u = tAudioCap; break;
		case a_g711a: m_tG711a = tAudioCap; break;
		case a_g722:  m_tG722  = tAudioCap; break;
		case a_g728:  m_tG728  = tAudioCap; break;
		case a_g729:  m_tG729  = tAudioCap; break;
		case a_g719:  m_tG719  = tAudioCap; break;
		default: return FALSE; break;
		}
		return TRUE;
	}

	TH323G7231Cap* GetG7231Cap()
	{	
		return &m_tG7231;
	}

	BOOL SetG7231Cap(TH323G7231Cap &tG7231Cap)
	{
		m_tG7231 = tG7231Cap;
		return TRUE;
	}

    TH323AACCap* GetH323AACCap(u16 wPayload)
    {
        TH323AACCap *ptAudioCap = NULL;
        switch(wPayload)
        {
        case a_mpegaaclc: ptAudioCap = &m_tAaclc;break;
        case a_mpegaacld: ptAudioCap = &m_tAacld;break;
        default:break;
        }
        return ptAudioCap;
    }

    BOOL SetH323AACCap(TH323AACCap &tAudioCap, u16 wPayload)
    {
        switch(wPayload)
        {
        case a_mpegaaclc: 
			{
				m_tAaclc.SetCapDirection( tAudioCap.GetCapDirection() );
				m_tAacld.SetChnl( static_cast<tagH323AACCap::emAACChnlCfg>(m_tAaclc.GetChnl() | tAudioCap.GetChnl()) );
				break;
			}
        case a_mpegaacld: 
			{
				m_tAacld.SetCapDirection( tAudioCap.GetCapDirection() );
				m_tAacld.SetChnl( static_cast<tagH323AACCap::emAACChnlCfg>(m_tAacld.GetChnl() | tAudioCap.GetChnl()) );
				break;
			}
            
        default: return FALSE; break;
        }
        return TRUE;
	}


	u8 GetSubId(u8 byCapIdex)
	{
		return m_awH264ToSubId[byCapIdex];
	}

private:
	u16 AddCapToTable(u16 wPayload, u8 byH264SubId)
	{
		for(s32 i = 0; i < MAX_CAPTABLE_SIZE; i++)
		{
            u8 byH264Count = 0;
            // 检查是否重复
			if(m_awCapTable[i] == wPayload)
            {
                if ((wPayload&0xff) != v_h264) 
                {
				    return i;
                }
				if(byH264SubId == m_awH264ToSubId[i])
				{
					return i;
				}
                byH264Count ++;
                if (byH264Count >= MAX_H264CAP_NUM)
                {
                    return 0xFFFF;
                }
            }
			
			if(m_awCapTable[i] == 0)
			{
				m_awCapTable[i] = wPayload;
				return i;
			}
		}
		return 0xFFFF;
	}

    BOOL IsInterAllSupport( u16 *pwPayload, s32 nNum, TH323H264VideoCap* ptH264Cap=NULL, s32 nH264Num=0, TH323AACCap* ptAacCap = NULL )
    {
		BOOL bUsedFlag[MAX_DESCAP_NUM][MAX_SIMUCAP_NUM];

		BOOL bH264Flag[MAX_H264CAP_NUM];
		
		u8   anH264Index[MAX_H264CAP_NUM];

		BOOL ab239Flag[MAX_DESCAP_NUM][MAX_SIMUCAP_NUM];
        s32  nCount = nNum;//用于计数
		s32 nH264Count = 0;
        s32 nCapIndex=0, nSimuIndex=0, nAltIndex=0;
        for(nCapIndex = 0; nCapIndex < MAX_DESCAP_NUM; nCapIndex++)
        {
            nCount = 0; //恢复初始值
			memset(anH264Index, 0xFF, MAX_H264CAP_NUM);
			memset(bH264Flag, 0, sizeof(bH264Flag));
			memset(bUsedFlag, 0, sizeof(bUsedFlag));
			memset(ab239Flag, 0, sizeof(ab239Flag));
			nH264Count = 0;
            for(nSimuIndex=0; nSimuIndex < MAX_SIMUCAP_NUM; nSimuIndex++)
            {
                for(nAltIndex = 0; nAltIndex < MAX_ALTCAP_NUM; nAltIndex++)
                {
                    u16 wCapTableId = GetCapDescId(nCapIndex,nSimuIndex,nAltIndex);
                    if ( wCapTableId == 0xFFFF )
                        break;
					u16 wPayloadType = m_awCapTable[wCapTableId];
                    if(wPayloadType == 0) 
                        break;
                    
                    if ( IsH239Type( wPayloadType ) != IsH239Type( pwPayload[nCount] ) )
                        continue;

                    if ( (((~(wPayloadType&0xff00))&(pwPayload[nCount]&0xff00)) == 0)
                        && ((wPayloadType&0xff) == (pwPayload[nCount]&0xff))
                        && bUsedFlag[nCapIndex][nSimuIndex] == FALSE )
                    {
						if((wPayloadType&0xff) == v_h264 && ptH264Cap!=NULL)
						{
                            u8 bySubId=m_awH264ToSubId[wCapTableId];
							if(bySubId == 0xFF)
								 break;

							if(nH264Count < nH264Num && bH264Flag[nH264Count] == FALSE && ptH264Cap[nH264Count] <= m_atH264Cap[bySubId])
							{
								bH264Flag[nH264Count] = TRUE;
								++nCount;
								bUsedFlag[nCapIndex][nSimuIndex] = TRUE;
								nSimuIndex = -1;//重新搜索
								
								anH264Index[nH264Count] = bySubId;
								++nH264Count;
								break;
							}
						}
						else
						{
							if( ( (wPayloadType&0xff) == a_mpegaaclc || (wPayloadType&0xff) == a_mpegaacld )  )
							{
								TH323AACCap *ptLocAacCap = GetH323AACCap( (wPayloadType&0xff) );
								if( ((wPayloadType&0xff) == a_mpegaaclc) && (ptAacCap != NULL) )
								{
									if( ( (~(ptLocAacCap->GetChnl()&0x7f)) & (ptAacCap->GetChnl()&0x7f) ) != 0 )
										break;
								}
								else if( ((wPayloadType&0xff) == a_mpegaacld) && (ptAacCap != NULL) )
								{
									if( ( (~(ptLocAacCap->GetChnl()&0x7f)) & (ptAacCap->GetChnl()&0x7f) ) != 0 )
										break;
								}
							}

							++nCount;
							bUsedFlag[nCapIndex][nSimuIndex] = TRUE;
							nSimuIndex = -1;//重新搜索
							break;
						}
                    }
                    
                }
                if(nCount == nNum)
                {
					for(s32 i=0; i<nH264Num; i++)
					{
						ptH264Cap[i] = m_atH264Cap[anH264Index[i]];
					}
                    return TRUE;
                }
            }
        }

        return FALSE;
    }

    BOOL FindH264VideoCap(TH323H264VideoCap& tH264VideoCap, u8& bySubId)
	{
		for(u8 i=0; i<m_bySubId; i++)
		{
			if(m_atH264Cap[i] == tH264VideoCap)
			{
				bySubId = i;
				return TRUE;
			}
		}
		return FALSE;
	}
}TH323CapSet;

//H221非标参数
typedef struct tagH323H221NonStandard
{
	u8		m_byT35CountryCode;
	u8		m_byT35Extension;
	u16		m_wManufacturerCode;

	tagH323H221NonStandard()
	{
		Clear();
	}

	void Clear()
	{
		m_byT35CountryCode	 = 0;
		m_byT35Extension	 = 0;
		m_wManufacturerCode  = 0;
	}

	void SetH221NonStandard( u8 byCountryCode, u8 byExtension, u16 wManufacturerCode)
	{
		m_byT35CountryCode	= byCountryCode;
		m_byT35Extension	= byExtension;
		m_wManufacturerCode = wManufacturerCode;
	}
	u8 GetT35CountryCode()
	{
		return m_byT35CountryCode;
	}
	u8 GetT35Extension()
	{
		return m_byT35Extension;
	}
	u16 GetManufacturerCode()
	{
		return m_wManufacturerCode;
	}

}TH323H221Nonstandard;

//厂商标识信息
typedef struct tagH323VendorInfo
{
	TH323H221Nonstandard	m_tVendor;
	s8						m_achProductId[LEN_256+1];
	s8						m_achVersionId[LEN_256+1];

	tagH323VendorInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_tVendor.Clear();
		memset( m_achProductId, 0, sizeof(m_achProductId) );
		memset( m_achVersionId, 0, sizeof(m_achVersionId) );
	}

	void SetH221NonStandard( TH323H221Nonstandard &tVendor)
	{
		m_tVendor = tVendor;
	}
	TH323H221Nonstandard& GetH221NonStandard()
	{
		return m_tVendor;
	}

	void SetVendorID( s8 *pProductId, s8 *pVersionId )
	{
		strncpy( m_achProductId, pProductId, min(strlen(pProductId), LEN_256));
		strncpy( m_achVersionId, pVersionId, min(strlen(pVersionId), LEN_256));		
	}
	s8* GetProductID()
	{
		return m_achProductId;
	}
	s8* GetVersionID()
	{
		return m_achVersionId;
	}
}TH323VendorInfo,*PTH323VendorInfo;


typedef struct tagH323RRQInfo
{
	TH323NetAddr		m_tCallAddr;			//呼叫信令地址
	TH323NetAddr		m_tLocalRASAddr;		//本地RAS地址
	cmEndpointType		m_eTerminalType;		//终端类型,GK用
	TH323AliasAddr		m_atAlias[LEN_128];		//向GK注册时的多个别名
	s32					m_nTimetoLive;			//timetolive，单位：秒	(optional)
	BOOL				m_bKeepAlive;			//轻量级注册标志位
	TH323AliasAddr		m_tEndpointID;			//终端ID,GK用			(optional)
	TH323NetAddr		m_tGkAddr;				//GK地址
	s8					m_ach_UserName[LEN_64];	//注册帐号(E164)
	s8					m_ach_Password[LEN_64];	//注册密钥
	TH323AliasAddr		m_tGKID;				//GK ID,GK用

	s32					m_nProductIdSize;
	u8					m_abyProductId[LEN_256];//产品号
	s32					m_nVerIdSize;
	u8					m_abyVersionId[LEN_256];//版本号

	tagH323RRQInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_tCallAddr.Clear();
		m_tLocalRASAddr.Clear();
		m_eTerminalType = cmEndpointTypeTerminal;
		memset( m_atAlias,0,sizeof(m_atAlias));
		m_nTimetoLive = 0;
		m_tGkAddr.Clear();
		memset(&m_tEndpointID,0,sizeof(TH323AliasAddr));
		memset(m_ach_UserName, 0, sizeof(m_ach_UserName));
		memset(m_ach_Password, 0, sizeof(m_ach_Password));
		m_bKeepAlive = FALSE;
		m_tCallAddr.SetNetAddress( 0, H323CALLPORT );
		m_tLocalRASAddr.SetNetAddress( 0, H323RASPORT );
		m_tGkAddr.SetNetAddress( 0, H323RASPORT );
		memset(&m_tGKID,0,sizeof(TH323AliasAddr));

		m_nProductIdSize = 0;
		m_nVerIdSize = 0;
		memset(m_abyProductId , 0 , LEN_256);
		memset(m_abyVersionId , 0 , LEN_256);
	}	

	void SetCallAddr( TH323NetAddr &tCallAddr )
	{
		m_tCallAddr = tCallAddr;
	}

	TH323NetAddr& GetCallAddr()
	{
		return m_tCallAddr;
	}

	void SetLocalRASAddr( TH323NetAddr &tLocalRASAddr )
	{
		m_tLocalRASAddr = tLocalRASAddr;
	}

	TH323NetAddr& GetLocalRASAddr()
	{
		return m_tLocalRASAddr;
	}


	void SetTerminalType( cmEndpointType emTerminalType )
	{
		m_eTerminalType = emTerminalType;
	}

	cmEndpointType GetTerminalType()
	{
		return m_eTerminalType;
	}


	void SetRRQAlias( TH323AliasAddr &tAlias, u8 byAliasNo )
	{
		if ( byAliasNo >= LEN_128 )
			return;

		m_atAlias[byAliasNo] = tAlias;
	}

	TH323AliasAddr& GetRRQAlias( u8 byAliasNo )
	{
		if ( byAliasNo < LEN_128 ) 
			return m_atAlias[byAliasNo];
		else
			return m_atAlias[0];        
	}


	void SetTimetoLive( s32 nTimetoLive )
	{
		m_nTimetoLive = nTimetoLive;
	}

	s32 GetTimetoLive()
	{
		return m_nTimetoLive;
	}

	void SetKeepAlive( BOOL bKeepAlive )
	{
		m_bKeepAlive = bKeepAlive;
	}

	BOOL IsKeepAlive()
	{
		return m_bKeepAlive;
	}

	void SetEndpointID( TH323AliasAddr &tEndpointID )
	{
		m_tEndpointID = tEndpointID;
	}

	TH323AliasAddr& GetEndpointID()
	{
		return m_tEndpointID;
	}

	void SetUserInfo(s8* pszUserName, s8* pszPassword)
	{
		if (pszUserName == NULL)
		{
			return;
		}
		memset(m_ach_UserName, 0, sizeof(m_ach_UserName));
		u16 nameLen = min( strlen(pszUserName), LEN_64);
		strncpy(m_ach_UserName, pszUserName, nameLen);
		m_ach_UserName[nameLen] = '\0';

		if (pszPassword != NULL)
		{
			memset(m_ach_Password, 0, sizeof(LEN_64));
			u16 pwdLen = min(strlen(pszPassword), LEN_64);
			strncpy(m_ach_Password, pszPassword, pwdLen);
			m_ach_Password[pwdLen] = '\0';
		}

	}

	s8* GetUserName()
	{
		return m_ach_UserName;
	}

	s8* GetPassword()
	{
		return m_ach_Password;
	}

	void SetGKAddr( TH323NetAddr &tGkAddr )
	{
		m_tGkAddr = tGkAddr;
	}

	TH323NetAddr& GetGKAddr()
	{
		return m_tGkAddr;
	}

	void SetGKID( TH323AliasAddr &tGKID )
	{
		m_tGKID = tGKID;
	}

	TH323AliasAddr& GetGKID()
	{
		return m_tGKID;
	}

	void SetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyProductId, 0, sizeof(m_abyProductId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nProductIdSize = nReal;
		if(nLen >= LEN_1)
		{
			memcpy(m_abyProductId, pbyId, nReal);
		}
	}

	s32 GetProductIdSize()
	{
		return m_nProductIdSize;
	}

	s32 GetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nProductIdSize); 
		memcpy(pbyId, m_abyProductId, nReal);
		return nReal;
	}
	
	void SetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyVersionId, 0, sizeof(m_abyVersionId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nVerIdSize = nReal;
		if(nLen >= LEN_1)
		{
			memcpy(m_abyVersionId, pbyId, nReal);
		}
	}

	s32 GetVersionIdSize()
	{
		return m_nVerIdSize;
	}

	s32 GetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nVerIdSize); 
		memcpy(pbyId, m_abyVersionId, nReal);
		return nReal;
	}

}TH323RRQInfo,*PTH323RRQInfo;

//RCF
typedef struct tagH323RCFInfo
{
	TH323NetAddr		m_tCallSignalAddr;		//GK返回的呼叫信令地址
	TH323AliasAddr		m_atAlias[LEN_128];		//GK返回的多个别名			(optional)
	TH323AliasAddr		m_tGKID;				//GK ID,GK用
	TH323AliasAddr		m_tEndpointID;			//终端ID,GK用
	s32					m_nTimetoLive;			//timetolive 单位：秒		(optional)
	BOOL				m_bWillRespondToIRR;	//是否响应IRR消息,GK用

	tagH323RCFInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_tCallSignalAddr.Clear();
		memset(m_atAlias,0,sizeof(m_atAlias));
		memset(&m_tGKID,0,sizeof(TH323AliasAddr));
		memset(&m_tEndpointID,0,sizeof(TH323AliasAddr));
		m_tCallSignalAddr.SetNetAddress( 0, H323CALLPORT );
		m_nTimetoLive = 0;
		m_bWillRespondToIRR = FALSE;
	}

	void SetCallSignalAddr( TH323NetAddr &tCallSignalAddr )
	{
		m_tCallSignalAddr = tCallSignalAddr;
	}

	TH323NetAddr& GetCallSignalAddr()
	{
		return m_tCallSignalAddr;
	}

	void SetRCFAlias( TH323AliasAddr &tAlias, u8 byAliasNo )
	{
		if ( byAliasNo >= LEN_128 )
			return;

		m_atAlias[byAliasNo] = tAlias;
	}

	TH323AliasAddr& GetRCFAlias( u8 byAliasNo )
	{
		if ( byAliasNo < LEN_128 ) 
			return m_atAlias[byAliasNo];
		else
			return m_atAlias[0];
	}

	void SetGKID( TH323AliasAddr &tGKID )
	{
		m_tGKID = tGKID;
	}

	TH323AliasAddr& GetGKID()
	{
		return m_tGKID;
	}

	void SetEndpointID( TH323AliasAddr &tEndpointID )
	{
		m_tEndpointID = tEndpointID;
	}

	TH323AliasAddr& GetEndpointID()
	{
		return m_tEndpointID;
	}

	void SetTimetoLive( s32 nTimetoLive )
	{
		m_nTimetoLive = nTimetoLive;
	}

	s32 GetTimetoLive()
	{
		return m_nTimetoLive;
	}

	void SetWillRespondToIRR( BOOL bKeepAlive )
	{
		m_bWillRespondToIRR = bKeepAlive;
	}

	BOOL IsWillRespondToIRR()
	{
		return m_bWillRespondToIRR;
	}

}TH323RCFInfo,*PTH323RCFInfo;

//ARQ
typedef struct tagH323ARQInfo
{
	cmCallType			m_eCallType;			    //呼叫类型,GK用
	cmCallModelType		m_eCallModel;			    //GK呼叫模式,默认值为直接呼叫	(optional)
	TH323AliasAddr		m_tEndpointID;			    //终端ID,GK用
	TH323AliasAddr		m_tDestAlias;			    //目的端别名
	TH323NetAddr		m_tCalledAddr;			    //目的端地址
	TH323AliasAddr		m_atSrcAlias[LEN_128];		//源端别名
	TH323NetAddr		m_tCallAddr;			    //源端地址,GK用					(optional)
	s32					m_nBandWidth;			    //单位：kbps
	s32					m_nCRV;					    //呼叫参考值,GK用
	s8  				m_achConferenceID[LEN_16];	//会议ID,GK用
	BOOL				m_bAnswerCall;			    //是否被叫,GK用
	s8  				m_achCallID[LEN_16];	    //呼叫ID,GK用
	TH323AliasAddr		m_tGKID;				    //GK ID,GK用					(optional)
	TH323NetAddr		m_tGkAddr;				    //GK地址

	tagH323ARQInfo()
	{
		Clear();
	}

	void Clear()
	{
		memset(&m_tEndpointID,0,sizeof(TH323AliasAddr));
		memset(&m_tGKID,0,sizeof(TH323AliasAddr));
		memset(m_achCallID,0,sizeof(m_achCallID));
		memset(m_achConferenceID,0,sizeof(m_achConferenceID));
		memset(&m_tDestAlias,0,sizeof(TH323AliasAddr));
		memset(m_atSrcAlias,0,sizeof(m_atSrcAlias));
		m_tCalledAddr.Clear();
		m_tCallAddr.Clear();		
		m_tGkAddr.Clear();
		m_eCallType = cmCallTypeP2P;
		m_eCallModel = cmCallModelTypeDirect;
		m_nBandWidth = 768;
		m_nCRV = 0;
		m_bAnswerCall = FALSE;
		m_tCalledAddr.SetNetAddress( 0, H323CALLPORT );
		m_tGkAddr.SetNetAddress( 0, H323RASPORT );
	}

	void SetCallType( cmCallType emCallType )
	{
		m_eCallType = emCallType;
	}

	cmCallType GetCallType()
	{ 
		return m_eCallType;
	}

	void SetCallModelType( cmCallModelType emCallModel )
	{
		m_eCallModel = emCallModel;
	}

	cmCallModelType GetCallModelType()
	{ 
		return m_eCallModel;
	}

	void SetEndpointID( TH323AliasAddr &tEndpointID )
	{
		m_tEndpointID = tEndpointID;
	}

	TH323AliasAddr& GetEndpointID()
	{
		return m_tEndpointID;
	}

	void SetDestAlias( TH323AliasAddr &tDestAlias )
	{
		m_tDestAlias = tDestAlias;
	}

	TH323AliasAddr& GetDestAlias()
	{
		return m_tDestAlias;
	}

	void SetCalledAddr( TH323NetAddr &tCalledAddr )
	{
		m_tCalledAddr = tCalledAddr;
	}

	TH323NetAddr& GetCalledAddr()
	{
		return m_tCalledAddr;
	}

	void SetARQSrcAlias( TH323AliasAddr &tAlias, u8 byAliasNo )
	{
		if ( byAliasNo >= LEN_128 )
			return;

		m_atSrcAlias[byAliasNo] = tAlias;
	}

	TH323AliasAddr& GetARQSrcAlias( u8 byAliasNo )
	{
		if ( byAliasNo < LEN_128 ) 
			return m_atSrcAlias[byAliasNo];
		else
			return m_atSrcAlias[0];
	}

	void SetCallAddr( TH323NetAddr &tCallAddr )
	{
		m_tCallAddr = tCallAddr;
	}

	TH323NetAddr& GetCallAddr()
	{
		return m_tCallAddr;
	}

	void SetBandWidth( s32 nBandWidth )
	{
		m_nBandWidth = nBandWidth;
	}

	s32 GetBandWidth()
	{
		return m_nBandWidth;
	}

	void SetCRV( s32 nCRV )
	{
		m_nCRV = nCRV;
	}

	s32 GetCRV()
	{
		return m_nCRV;
	}

	void SetConferenceID( s8 *pchConferenceID, u8 byLen )
	{
		if ( pchConferenceID == NULL )
			return;

		memset( m_achConferenceID, 0, sizeof(m_achConferenceID) );
		u8 byRealLen = min( byLen, LEN_16 );
		strncpy( m_achConferenceID, pchConferenceID, byRealLen );
		m_achConferenceID[byRealLen] = '\0';
	}

	s8* GetConferenceID()
	{
		return m_achConferenceID;
	}

	void SetAnswerCall( BOOL bAnswerCall )
	{
		m_bAnswerCall = bAnswerCall;
	}

	BOOL IsAnswerCall()
	{
		return m_bAnswerCall;
	}

	void SetCallID( s8 *pchCallID, u8 byLen )
	{
		if ( pchCallID == NULL )
			return;

		memset( m_achCallID, 0, sizeof(m_achCallID) );
		u8 byRealLen = min( byLen, LEN_16 );
		strncpy( m_achCallID, pchCallID, byRealLen );
		m_achCallID[byRealLen] = '\0';
	}

	s8* GetCallID()
	{
		return m_achCallID;
	}

	void SetGKID( TH323AliasAddr &tGKID )
	{
		m_tGKID = tGKID;
	}

	TH323AliasAddr& GetGKID()
	{
		return m_tGKID;
	}

	void SetGKAddr( TH323NetAddr &tGKAddr )
	{
		m_tGkAddr = tGKAddr;
	}

	TH323NetAddr& GetGKAddr()
	{
		return m_tGkAddr;
	}    
}TH323ARQInfo, *PTH323ARQInfo;

//ACF
typedef struct tagH323ACFInfo
{
	s32				m_nBandWidth;			    //单位：kbps
	cmCallModelType m_eCallModel;			    //GK呼叫模式,默认值为直接呼叫
	TH323NetAddr	m_tCalledAddr;			    //目的端地址
	s32				m_nIRRFrequency;		    //IRR的发送频率，单位：秒	(optional)
	BOOL			m_bWillResponseIRR;		    //响应IRR消息

	tagH323ACFInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_tCalledAddr.Clear();
		m_nBandWidth = 4000;
		m_eCallModel = cmCallModelTypeDirect;
		m_nIRRFrequency = 0;
		m_bWillResponseIRR = FALSE;
		m_tCalledAddr.SetNetAddress( 0, H323CALLPORT );
	}

	void SetBandWidth( s32 nBandWidth )
	{
		m_nBandWidth = nBandWidth;
	}

	s32 GetBandWidth()
	{
		return m_nBandWidth;
	}

	void SetCallModelType( cmCallModelType emCallModel )
	{
		m_eCallModel = emCallModel;
	}

	cmCallModelType GetCallModelType()
	{ 
		return m_eCallModel;
	}

	void SetCalledAddr( TH323NetAddr &tCalledAddr )
	{
		m_tCalledAddr = tCalledAddr;
	}

	TH323NetAddr& GetCalledAddr()
	{
		return m_tCalledAddr;
	}

	void SetIRRFrequency( s32 nIRRFrequency )
	{
		m_nIRRFrequency = nIRRFrequency;
	}

	s32 GetIRRFrequency()
	{
		return m_nIRRFrequency;
	}

	void SetWillResponseIRR( BOOL bWillResponseIRR )
	{
		m_bWillResponseIRR = bWillResponseIRR;
	}

	BOOL IsWillResponseIRR()
	{
		return m_bWillResponseIRR;
	}
}TH323ACFInfo, *PTH323ACFInfo;


typedef struct tagH323DRQInfo
{
	TH323AliasAddr			m_tEndpointID;			    //终端ID,GK用
	s8  					m_achConferenceID[LEN_16];//会议ID,GK用
	s32						m_nCRV;					    //呼叫参考值,GK用
	cmRASDisengageReason	m_eReason;					//DRQ的原因		
	s8  					m_achCallID[LEN_16];	    //呼叫ID,GK用
	TH323AliasAddr			m_tGKID;				    //GK ID,GK用				(optional)
	BOOL					m_bAnsweredCall;		    //是否为被叫端,GK用
	TH323NetAddr			m_tDestAddr;			    //DRQ的接受端地址

	tagH323DRQInfo()
	{
		Clear();
	}

	void Clear()
	{
		memset(&m_tEndpointID,0,sizeof(TH323AliasAddr));
		memset(m_achConferenceID,0,sizeof(m_achConferenceID));
		memset(m_achCallID,0,sizeof(m_achCallID));
		memset(&m_tGKID,0,sizeof(TH323AliasAddr));
		m_tDestAddr.Clear();
		m_bAnsweredCall = FALSE;
		m_eReason = (cmRASDisengageReason)0;
		m_nCRV = 0;
		m_tDestAddr.SetNetAddress( 0, H323RASPORT );
	}

	void SetEndpointID( TH323AliasAddr &tEndpointID )
	{
		m_tEndpointID = tEndpointID;
	}

	TH323AliasAddr& GetEndpointID()
	{
		return m_tEndpointID;
	}

	void SetConferenceID( s8 *pchConferenceID, u8 byLen )
	{
		if ( pchConferenceID == NULL )
			return;

		memset( m_achConferenceID, 0, sizeof(m_achConferenceID) );
		u8 byRealLen = min( byLen, LEN_16 );
		strncpy( m_achConferenceID, pchConferenceID, byRealLen );
		m_achConferenceID[byRealLen] = '\0';
	}

	s8* GetConferenceID()
	{
		return m_achConferenceID;
	}

	void SetCRV( s32 nCRV )
	{
		m_nCRV = nCRV;
	}

	s32 GetCRV()
	{
		return m_nCRV;
	}

	void SetDRQReason( cmRASDisengageReason emDRQReason )
	{
		m_eReason = emDRQReason;
	}

	cmRASDisengageReason GetDRQReason()
	{ 
		return m_eReason;
	}

	void SetCallID( s8 *pchCallID, u8 byLen )
	{
		if ( pchCallID == NULL )
			return;

		memset( m_achCallID, 0, sizeof(m_achCallID) );
		u8 byRealLen = min( byLen, LEN_16 );
		strncpy( m_achCallID, pchCallID, byRealLen );
		m_achCallID[byRealLen] = '\0';
	}

	s8* GetCallID()
	{
		return m_achCallID;
	}


	void SetGKID( TH323AliasAddr &tGKID )
	{
		m_tGKID = tGKID;
	}

	TH323AliasAddr& GetGKID()
	{
		return m_tGKID;
	}

	void SetAnsweredCall( BOOL bAnsweredCall )
	{
		m_bAnsweredCall = bAnsweredCall;
	}

	BOOL IsAnsweredCall()
	{
		return m_bAnsweredCall;
	}


	void SetDestAddr( TH323NetAddr &tDestAddr )
	{
		m_tDestAddr = tDestAddr;
	}

	TH323NetAddr& GetDestAddr()
	{
		return m_tDestAddr;
	}
}TH323DRQInfo, *PTH323DRQInfo;

//URQ
typedef struct tagH323URQInfo
{
	TH323NetAddr		m_tCallAddr;			//源端呼叫信令地址
	TH323AliasAddr		m_atAlias[LEN_128];	//向GK注册时的多个别名		(optional)
	TH323AliasAddr		m_tEndpointID;			//终端ID					(optional)
	TH323AliasAddr		m_tGKID;				//GK ID						(optional)
	cmRASUnregReason	m_eReason;				//URQ的原因					(optional)
	TH323NetAddr		m_tDestAddr;			//URQ的接受端地址

	tagH323URQInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_tCallAddr.Clear();		
		memset(m_atAlias,0,sizeof(m_atAlias));		
		memset(&m_tEndpointID,0,sizeof(TH323AliasAddr));	
		memset(&m_tGKID,0,sizeof(TH323AliasAddr));
		m_tDestAddr.Clear();
		m_eReason = cmRASUnregReasonUndefinedReason;
		m_tCallAddr.SetNetAddress( 0, H323CALLPORT );
		m_tDestAddr.SetNetAddress( 0, H323RASPORT );
	}

	void SetCallAddr( TH323NetAddr &tCallAddr )
	{
		m_tCallAddr = tCallAddr;
	}

	TH323NetAddr& GetCallAddr()
	{
		return m_tCallAddr;
	}

	void SetURQAlias( TH323AliasAddr &tAlias, u8 byAliasNo )
	{
		if ( byAliasNo >= LEN_128 )
			return;

		m_atAlias[byAliasNo] = tAlias;
	}

	TH323AliasAddr& GetURQAlias( u8 byAliasNo )
	{
		if ( byAliasNo < LEN_128 ) 
			return m_atAlias[byAliasNo];
		else
			return m_atAlias[0];
	}


	void SetEndpointID( TH323AliasAddr &tEndpointID )
	{
		m_tEndpointID = tEndpointID;
	}

	TH323AliasAddr& GetEndpointID()
	{
		return m_tEndpointID;
	}


	void SetGKID( TH323AliasAddr &tGKID )
	{
		m_tGKID = tGKID;
	}

	TH323AliasAddr& GetGKID()
	{
		return m_tGKID;
	}

	void SetURQReason( cmRASUnregReason emURQReason )
	{
		m_eReason = emURQReason;
	}

	cmRASUnregReason GetURQReason()
	{ 
		return m_eReason;
	}

	void SetDestAddr( TH323NetAddr &tDestAddr )
	{
		m_tDestAddr = tDestAddr;
	}

	TH323NetAddr& GetDestAddr()
	{
		return m_tDestAddr;
	}
}TH323URQInfo, *PTH323URQInfo;


//IRQ
typedef struct tagH323IRQInfo
{	
	s32				m_nCRV;					//呼叫参考值
	s8  			m_achCallID[LEN_16];	//呼叫ID

	HCALL			m_hsCall;				//呼叫句柄	
	TH323NetAddr	m_tPeerRasAddr;			//对端RAS地址

	tagH323IRQInfo()
	{
		Clear();
	}

	void Clear()
	{
		memset( m_achCallID, 0, sizeof(m_achCallID));
		m_tPeerRasAddr.Clear();
		m_nCRV = 0;
		m_hsCall = NULL;
		m_tPeerRasAddr.SetNetAddress( 0, H323RASPORT );
	}

	void SetCRV( s32 nCRV )
	{
		m_nCRV = nCRV;
	}

	s32 GetCRV()
	{
		return m_nCRV;
	}

	void SetCallID( s8 *pchCallID, u8 byLen )
	{
		if ( pchCallID == NULL )
			return;

		memset( m_achCallID, 0, sizeof(m_achCallID) );
		u8 byRealLen = min( byLen, LEN_16 );
		strncpy( m_achCallID, pchCallID, byRealLen );
		m_achCallID[byRealLen] = '\0';
	}

	s8* GetCallID()
	{
		return m_achCallID;
	}

	void SetHCALL( HCALL hsCall )
	{
		if ( hsCall != NULL )
			m_hsCall = hsCall;
	}

	HCALL GetHCALL()
	{
		return m_hsCall;
	}

	void SetPeerRasAddr( TH323NetAddr &tPeerRasAddr )
	{
		m_tPeerRasAddr = tPeerRasAddr;
	}

	TH323NetAddr& GetPeerRasAddr()
	{
		return m_tPeerRasAddr;
	}
}TH323IRQInfo, *PTH323IRQInfo;

//IRR
typedef struct tagH323IRRInfo
{
	cmEndpointType		m_eEndpointType;		    //终端类型,GK用
	TH323AliasAddr		m_tEndpointID;			    //终端ID,GK用
	TH323NetAddr		m_tRASAddr;				    //本地RAS地址
	TH323NetAddr		m_tCallAddr;			    //呼叫信令地址
	TH323AliasAddr		m_atAlias[LEN_128];		    //向GK注册的多个别名	(optional)	

	s32					m_nCRV;					    //呼叫参考值,GK用
	s8					m_achConferenceID[LEN_16];	//会议ID,GK用
	BOOL				m_bOriginator;			    //是否为主叫,GK用

	s8					m_achCallID[LEN_16];	    //呼叫ID,GK用
	BOOL				m_bNeedResponse;		    //是否响应IRR消息
	BOOL				m_bIsUnsolicited;		    //默认为TRUE：主动发IRR；FALSE：响应IRQ

	TH323NetAddr		m_tGKAddr;				    //GK地址


	tagH323IRRInfo()
	{
		Clear();
	}

	void Clear()
	{
		memset(&m_tEndpointID,0,sizeof(TH323AliasAddr));	
		m_tRASAddr.Clear();			
		m_tCallAddr.Clear();		
		memset(m_atAlias,0,sizeof(m_atAlias));
		m_tGKAddr.Clear();			

		m_bNeedResponse  = FALSE;
		m_bIsUnsolicited = TRUE;
		m_tRASAddr.SetNetAddress( 0, H323RASPORT );
		m_tCallAddr.SetNetAddress( 0, H323CALLPORT );
		m_tGKAddr.SetNetAddress( 0, H323RASPORT );
	}

	void SetEndpointType( cmEndpointType emEndpointType )
	{
		m_eEndpointType = emEndpointType;
	}

	cmEndpointType GetEndpointType()
	{
		return m_eEndpointType;
	}

	void SetEndpointID( TH323AliasAddr &tEndpointID )
	{
		m_tEndpointID = tEndpointID;
	}

	TH323AliasAddr& GetEndpointID()
	{
		return m_tEndpointID;
	}

	void SetRASAddr( TH323NetAddr &tRASAddr )
	{
		m_tRASAddr = tRASAddr;
	}

	TH323NetAddr& GetRASAddr()
	{
		return m_tRASAddr;
	}

	void SetCallAddr( TH323NetAddr &tCallAddr )
	{
		m_tCallAddr = tCallAddr;
	}

	TH323NetAddr& GetCallAddr()
	{
		return m_tCallAddr;
	}

	void SetIRRAlias( TH323AliasAddr &tAlias, u8 byAliasNo )
	{
		if ( byAliasNo >= LEN_128 )
			return;

		m_atAlias[byAliasNo] = tAlias;
	}

	TH323AliasAddr& GetIRRAlias( u8 byAliasNo )
	{
		if ( byAliasNo < LEN_128 ) 
			return m_atAlias[byAliasNo];
		else
			return m_atAlias[0];
	}

	void SetCRV( s32 nCRV )
	{
		m_nCRV = nCRV;
	}

	s32 GetCRV()
	{
		return m_nCRV;
	}

	void SetConferenceID( s8 *pchConferenceID, u8 byLen )
	{
		if ( pchConferenceID == NULL )
			return;

		memset( m_achConferenceID, 0, sizeof(m_achConferenceID) );
		u8 byRealLen = min( byLen, LEN_16 );
		strncpy( m_achConferenceID, pchConferenceID, byRealLen );
		m_achConferenceID[byRealLen] = '\0';
	}

	s8* GetConferenceID()
	{
		return m_achConferenceID;
	}

	void SetOriginator( BOOL bOriginator )
	{
		m_bOriginator = bOriginator;
	}

	BOOL IsOriginator()
	{
		return m_bOriginator;
	}

	void SetCallID( s8 *pchCallID, u8 byLen )
	{
		if ( pchCallID == NULL )
			return;

		memset( m_achCallID, 0, sizeof(m_achCallID) );
		u8 byRealLen = min( byLen, LEN_16 );
		strncpy( m_achCallID, pchCallID, byRealLen );
		m_achCallID[byRealLen] = '\0';
	}

	s8* GetCallID()
	{
		return m_achCallID;
	}

	void SetNeedResponse( BOOL bNeedResponse )
	{
		m_bNeedResponse = bNeedResponse;
	}

	BOOL IsNeedResponse()
	{
		return m_bNeedResponse;
	}

	void SetUnsolicited( BOOL bIsUnsolicited )
	{
		m_bIsUnsolicited = bIsUnsolicited;
	}

	BOOL IsUnsolicited()
	{
		return m_bIsUnsolicited;
	}

	void SetGKAddr( TH323NetAddr &tGkAddr )
	{
		m_tGKAddr = tGkAddr;
	}

	TH323NetAddr& GetGKAddr()
	{
		return m_tGKAddr;
	}

}TH323IRRInfo,*PTH323IRRInfo;


typedef struct tagH323H239Info
{
    emH239InfoType  m_emH239InfoType;
    TH323TerLabel   m_tTerLabel;
    s32             m_nChannelId;

    BOOL            m_bIsResponseAck;   
    u32             m_dwBitRate;          
    
    u8              m_bySymmetryBreaking; 
    
    tagH323H239Info()
    {
        Clear();
    }

    void Clear()
    {
        m_emH239InfoType     = emH239InfoTypeInvalid;
        m_bIsResponseAck     = FALSE;
        
        m_dwBitRate          = 0;
        m_bySymmetryBreaking = 0;
        m_nChannelId         = 0;
        
        m_tTerLabel.Clear();
    }

    void host2net(BOOL bhton)
    {
        s32 nH239InfoType = m_emH239InfoType;
        if (bhton)
        {
            m_emH239InfoType = (emH239InfoType)htonl(nH239InfoType);
            m_nChannelId     = htonl(m_nChannelId);
            m_bIsResponseAck = htonl(m_bIsResponseAck);
            m_dwBitRate      = htonl(m_dwBitRate);
        }
        else
        {
            m_emH239InfoType = (emH239InfoType)ntohl(nH239InfoType);
            m_nChannelId     = ntohl(m_nChannelId);
            m_bIsResponseAck = ntohl(m_bIsResponseAck);
            m_dwBitRate      = ntohl(m_dwBitRate);
        }
    }

    BOOL SetH239InfoType(emH239InfoType nH239InfoType)
    {
        if (nH239InfoType>emH239InfoTypeInvalid
            && nH239InfoType<emH239InfoTypeEnd)
        {
            m_emH239InfoType = nH239InfoType;
            return TRUE;
        }
        return FALSE;
    }
    emH239InfoType GetH239InfoType()
    {
        return m_emH239InfoType;
    }
    
    void SetTerLabel(TH323TerLabel &tTerlabel)
    {
        m_tTerLabel = tTerlabel;
    }

    TH323TerLabel &GetTerlabel()
    {
        return m_tTerLabel;
    }


    void SetSymmetryBreaking(u8 bySymmetryBreaking)
    {
        m_bySymmetryBreaking = bySymmetryBreaking;
    }
    u8 GetSymmetryBreaking()
    {
        return m_bySymmetryBreaking;
    }
    
    void SetResponseAck(BOOL bIsResponseAck)
    {
        m_bIsResponseAck = bIsResponseAck;
    }
    BOOL IsResponseAck()
    {
        return m_bIsResponseAck;
    }
    
    void SetChannelId(HCHAN hsChan)
    {
/*        m_nChannelId = cmGetLCNFromHchan(hsChan);*/
    }
    s32 GetChannelId()
    {
        return m_nChannelId;
    }
    void SetChannelId(s32 nChannelId)
    {
        m_nChannelId = nChannelId;
    }
    
}TH323H239Info,*PTH323H239Info;

/*====================================================================
函	 数   名: HpH323IsMasterCall
功	      能: 检查是否主叫
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall    - 呼叫句柄
输出参数说明：
返   回   值: FALSE - 被叫
              TRUE  - 主叫
====================================================================*/
BOOL HpH323IsMasterCall(IN HCALL hsCall);

/*====================================================================
函	 数   名: HpH323IsMasterTerminal
功	      能: 检查主从决定的结果是否master
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall    - 呼叫句柄
输出参数说明：
返   回   值: FALSE - slave
              TRUE  - master
====================================================================*/
BOOL HpH323IsMasterTerminal(HCALL hsCall);

/*====================================================================
函	 数   名: HpH323CallSetHandle
功	      能: 设置hsCall的应用层句柄
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall - 协议栈呼叫句柄
              haCall - 应用句柄
			  bSync  - 是否同步
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 HpH323CallSetHandle(IN HCALL hsCall, IN HAPPCALL haCall, IN bool bSync = true);

/*====================================================================
函	 数   名: HpH323CallGetHandle
功	      能: 获得应用句柄
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall - 协议栈呼叫句柄              
			  bSync  - 是否同步
输出参数说明: haCall - 应用句柄
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 HpH323CallGetHandle(IN HCALL hsCall, OUT HAPPCALL* haCall, IN bool bSync = true);

/*====================================================================
函	 数   名:HpH323RasUpdateRegAliasList
功	      能: 更新本端别名
算 法 实  现: 
引用全局变量: g_hApp
输入参数说明: alias  - 别名数组
			  size	 - 别名个数              
返   回   值: 成功   - act_ok
              失败   - act_err
====================================================================*/
s32 HpH323RasUpdateRegAliasList(cmAlias alias[], s32 nSize);

typedef s32 (CALLBACK* NotifyH323NewCallT)( HCALL hsCall, LPHAPPCALL lphaCall);
typedef s32 (CALLBACK* NotifyH323NewChanT)( HAPPCALL haCall, HCALL hsCall, HCHAN hsChan, LPHAPPCHAN lphaChan );
typedef s32 (CALLBACK* NotifyH323CallCtrlMsgT)( HAPPCALL haCall, HCALL hsCall, u16 msgType, const void* pBuf, u16 nBufLen );
typedef s32 (CALLBACK* NotifyH323ChanCtrlMsgT)( HAPPCALL haCall, HCALL hsCall, HAPPCHAN haChan, HCHAN hsChan, u16 msgType, const void* pBuf, u16 nBufLen );
typedef s32 (CALLBACK* NotifyH323ConfCtrlMsgT)( HAPPCALL haCall, HCALL hsCall, u16 msgType, const void* pBuf, u16 nBufLen );
typedef s32 (CALLBACK* NotifyH323RasCtrlMsgT)( HAPPCALL haCall, HCALL hsCall, HAPPRAS haRas, HRAS hsRas, u16 msgType, const void* pBuf, u16 nBufLen );

typedef s32 (CALLBACK *NotifyH323FeccMsgT)(HCALL hsCall, HCHAN hsChan,TFeccStruct feccStruct,TH323TerLabel terSrc, TH323TerLabel terDst);


//回调函数指针结构
typedef struct 
{
    NotifyH323NewCallT			fpNewCall;
    NotifyH323NewChanT			fpNewChan;
    NotifyH323CallCtrlMsgT		fpCallCtrlMsg;
    NotifyH323ChanCtrlMsgT		fpChanCtrlMsg;
    NotifyH323ConfCtrlMsgT		fpConfCtrlMsg;
    NotifyH323RasCtrlMsgT		fpRasCtrlMsg;

	NotifyH323FeccMsgT			fpFeccMsg;  
}H323EVENT,*LPH323EVENT;

void*  HpInitH323Adapter( s8* sConfigFile, TH323StackCfg *keyParams = NULL );
void HpCloseH323Adapter();
s32  HpSetAppCallBack( H323EVENT* ptEvent );
void HpH323CheckMessage();
s32 HpH323CreateNewCall( HAPPCALL haCall, HCALL* lphsCall, bool bSync = true );
s32 HpH323CreateNewChan( HCALL hsCall, HAPPCHAN haChan, LPHCHAN lphsChan, bool bSync = true);
s32 HpH323CreateRASStartTransaction( HAPPRAS haRas, HRAS* lphsRas, cmRASTransaction transaction,
							   TH323NetAddr* destAddress, HCALL hsCall, bool bSync = true );
s32 HpH323SendRasCtrlMsg( HCALL hsCall, HRAS hsRas, u16 msgType, const void* pBuf, u16 nBufLen, bool bSync = true );
s32 HpH323SendCallCtrlMsg( HCALL hsCall, u16 msgType, const void* pBuf, u16 nBufLen, bool bSync = true );
s32 HpH323SendChanCtrlMsg( HCHAN hsChan, u16 msgType, const void* pBuf, u16 nBufLen, bool bSync = true );
s32 HpH323SendConfCtrlMsg( HCALL hsCall, u16 msgType, const void* pBuf, u16 nBufLen, bool bSync = true );
s32 HpH323SetCallHandle( HCALL hsCall, HAPPCALL haCall, bool bSync = true );
s32 HpH323GetCallHandle( HCALL hsCall, HAPPCALL* haCall, bool bSync = true );
s32 HpH323RasUpdateRegAliasList( cmAlias alias[], s32 size );


#endif