#ifndef _SIPADAPTER_
#define _SIPADAPTER_

#include "ipc.h"
#include "rvsipstack.h"
#include "rvsdpmsg.h"
#include "hptype.h"
#include "math.h"
#include "rvsocket.h"

#define MAX_NUM_CLIENT_REG 128
#define LEN_DOMAINNAME 64
#define LEN_USERNAME 64
#define LEN_PASSWORD 64
#define SIP_CHAN_NUM 10
#define MAX_LEN_SESSION_DESC 1024*10
#define MAX_LEN_VERSION 256
#define SESSION_NAME_LEN 64
#define MAX_HEADER_LEN 256
#define SESSION_VERSION_LEN 64
#define MAX_NUM_REGNAME_SIZE 32
#define MAX_CONTACKADDR_NUM 3
#define MAX_CALLID_NUM 128
#define MAX_XML_LEN 1024*10


#define   MEDIA_TYPE_NULL                 (u8)255
#define	  MEDIA_TYPE_MP3	              (u8)96  /*mp3 mode 0-4*/
#define   MEDIA_TYPE_G7221C	              (u8)98  /*G722.1.C Siren14*/
#define   MEDIA_TYPE_G719	              (u8)99  /*G719 non-standard of polycom serial 22*/
#define   MEDIA_TYPE_PCMA		          (u8)8   /*G.711 Alaw  mode 5*/
#define   MEDIA_TYPE_PCMU		          (u8)0   /*G.711 ulaw  mode 6*/
#define   MEDIA_TYPE_G721		          (u8)2   /*G.721*/
#define   MEDIA_TYPE_G722		          (u8)9   /*G.722*/
#define	  MEDIA_TYPE_G7231		          (u8)4   /*G.7231*/
#define   MEDIA_TYPE_ADPCM                (u8)5   /*DVI4 ADPCM*/
#define	  MEDIA_TYPE_G728		          (u8)15  /*G.728*/
#define	  MEDIA_TYPE_G729		          (u8)18  /*G.729*/
#define   MEDIA_TYPE_G7221                (u8)13  /*G.7221*/
#define   MEDIA_TYPE_AACLC                (u8)102 /*AAC LC*/
#define   MEDIA_TYPE_AACLD                (u8)103 /*AAC LD*/

/*视频*/
#define   MEDIA_TYPE_MP4	              (u8)97  /*MPEG-4*/
#define   MEDIA_TYPE_H261	              (u8)31  /*H.261*/
#define   MEDIA_TYPE_H262	              (u8)33  /*H.262 (MPEG-2)*/
#define   MEDIA_TYPE_H263	              (u8)34  /*H.263*/
#define   MEDIA_TYPE_H263PLUS             (u8)101 /*H.263+*/
#define   MEDIA_TYPE_H264	              (u8)109 /*H.264*/

#define IP2CHAR(ip)  ((u8 *)&(ip))[0], \
					 ((u8 *)&(ip))[1], \
					 ((u8 *)&(ip))[2], \
					 ((u8 *)&(ip))[3]


#define FPS_ADJUST(n)  ( n>0xFF ? 0xFF : ( (n==11 || n==12 || n==14 || n==24 || n==29 || n==59) ? n+1 : n )  ) 


#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

typedef struct tagSipStackCfg
{
	u32    m_dwMaxCalls;
	u32    m_dwLocalIP;
	u16    m_wLocalUdpPort;
	u16    m_wLocalTcpPort;
	
	BOOL32 m_bIsRegister;
	BOOL32 m_bManualAck;

	s8     m_achProduct[128];
	s8     m_achTerVer[128];
public:
	tagSipStackCfg()
	{
		Clear();
	}
	void Clear()
	{
		memset( this, 0, sizeof(tagSipStackCfg) );
	}
}TSipStackCfg;

//IP传输地址
typedef struct tagSipTransAddr
{
	u32 m_dwIP ;//网络序
	u16 m_wPort;//主机序
public:
	tagSipTransAddr()
	{
		Clear();
	}
	void Clear()
	{
		memset( this, 0, sizeof(tagSipTransAddr) );
	}
}TSipTransAddress;


enum emRegState
{
	emRegIdle,
	emRegRegistering,
	emRegRegistered,
	emRegFailed,
	emUnReg,
};

enum emRegResult
{
	emRegNone,
	emRegSuc,
	emRegFail,
	emRegNotFound,
	emRegNameDump,
	emRegTimeout,
};


typedef struct tagRegClient
{
	RvSipRegClientHandle   m_hRegClientHandle;
	emRegState             m_emRegState;
	s32					   m_nAppHandle;

	tagRegClient() { Clear(); }
	void Clear()
	{
		m_hRegClientHandle = NULL;
		m_emRegState = emRegIdle;
		m_nAppHandle = -1;
	}
}TRegClient;

enum emHostNameType
{
	emSipUrlIpv4,
	emSipUrlIpv6,
	emSipUrlDomain,
	emSipUrlAlias,
	emSipUrlE164Num,
	emSipUrlNone,
};


#define IsValidHostName( hostname ) \
	( ( hostname.m_emType == emSipUrlIpv4 && hostname.m_dwIpAddr ) \
	|| ( hostname.m_emType == emSipUrlDomain && strlen( hostname.m_achDomainName ) ) ) \
	?TRUE:FALSE

typedef struct tagSipHostName
{
	emHostNameType   m_emType;                   //主机名类型
	char             m_achDomainName[LEN_DOMAINNAME+1];//域名
	u32              m_dwIpAddr;                   //IP地址
	u16              m_wPort;
	void Clear()
	{
		m_emType = emSipUrlNone;
		memset( m_achDomainName, 0, sizeof(m_achDomainName) );
		m_dwIpAddr = 0;
		m_wPort = 5060;
	}
	tagSipHostName()
	{
		Clear();
	}
}TSipHostName;

typedef struct tagSipUrl
{
	s8             m_achDisplayName[LEN_USERNAME+1]; 
	s8			   m_achUserName[LEN_USERNAME+1];      //用户名   
	s8             m_achPassword[LEN_PASSWORD+1];      //密码
	TSipHostName   m_tHostName;                        //主机名
	u16            m_wPort;                            //端口    
public:
	tagSipUrl()
	{
		memset( this, 0, sizeof( tagSipUrl ) );
	}
}TSipUrl;

typedef struct tagSipRegInfo
{
	TSipHostName m_tRegistrar;
	TSipUrl m_tFrom;
	TSipUrl m_tTo;
	u32 m_dwExpires;
	BOOL32 m_bIsInterReg;
	tagSipRegInfo() { memset( this, 0, sizeof(tagSipRegInfo) ); }
}TSipRegInfo;

//呼叫消息
enum emSipCallMsgType 
{
	h_sip_call_msg_begin = 0,
	h_sip_call_make,		            //发起呼叫	      [outgoing]: TCALLPARAM + CSipCapSet(optional)
	h_sip_call_incoming,		        //呼叫到来		  [incoming]: TCALLPARAM + VendorProductId		  	  	
	h_sip_call_answer,		            //接受(拒绝)呼叫  [outgoing]: TNODEINFO + CSipCapSet
	h_sip_call_bye,                    //结束呼叫		  [outgoing]: 1 byte,reason
	h_sip_call_connected,               //呼叫建立		  [incoming]: TNODEINFO	
	h_sip_call_disconnected,            //呼叫断开		  [incoming]: 1 byte,reason	
	h_sip_call_capsetnotify,            //能力级交互      [incoming]: 
	h_sip_dual_open,                    //打开双流
	h_sip_dual_open_accept,
	h_sip_dual_close,                   //关闭双流
	h_sip_dual_close_accept,                   
	h_sip_dual_ack,
	h_sip_dual_reject,
	h_sip_dual_connected,
	h_sip_dual_disconnected,
	h_sip_dual_failed,
	h_sip_call_roundtripdelay,          //roundtrip implemented by options method
	h_sip_call_ack,
	h_sip_call_msg_end,
};


enum  emSipPayloadType 
{
	emSipPayloadTypeBegin = 0,
	//音频格式
	emAudioTypeBegin,
	emG711a,
	emG711u,
	emG722,
	emG7231,
	emG728,
	emG729,  
	emG723,
	emG7221,
	emG719,
	emAudioTypeEnd = 49,

	//视频格式
	emVideoTypeBegin = 50,	
	emH263,
	emH263plus,
	emH264,
	emVideoTypeEnd = 99,

	emSipPayloadTypeEnd = 255
};


//分辨率
enum emSipRes
{
	emSipResBegin = 0,
	emSipSQCIF,
	emSipQCIF,
	emSipCIF,
	emSipCIF2,
	emSipCIF4,
	emSipCIF16,

	emSipVGA,
	emSipSVGA,
	emSipXGA,

	emSipWCIF4,  //1024*576
	emSipHD720,  //1280*720
	emSipSXGA,   //1280*1024
	emSipUXGA,   //1600*1200
	emSipHD1080, //1920*1080
	emSipResEnd
};

enum emSipMediaType
{
	emSipMediaNone,
	emSipMediaAudio,
	emSipMediaVideo,
};


//设备厂商标识信息
enum emSipVendorId
{
	emVendorUnknow = 0,
	emMicrosoftNetmeeting = 1,
	emKeda,
	emHuaWei,
	emPolycom,
	emTandberg,
	emRadvision,
	emVcon,
	emSony,
	emVtel,
	emZTE,
	emCodian,
};

//信道控制消息
enum emSipChanCtrlMsgType 
{
	h_sip_chan_invalidInterface = 0,
	h_sip_chan_videofastupdate,
	h_sip_chan_flowControlCmd,
	h_sip_chan_flowControlInd,
	h_sip_chan_fastupdatewithoutid,
};

enum emSipRegCtrlMsgType
{
	h_sip_reg = 0,
	h_sip_unreg,
	h_sip_registered,
	h_sip_regfailed,
	h_sip_regterminated,
	h_sip_regsendregfailed,
};

enum emSipServerMsgType
{
	h_sipserver_invalidInterface = 0,
	h_sipserver_registrationcmd,
	h_sipserver_registrationind,
	h_sipserver_unregistrationcmd,
	h_sipserver_unregistrationind,
	h_sipserver_query,     
	h_sipserver_answer,
	h_sipserver_option,
};

typedef struct tagSipNodeInfo
{
	emSipVendorId m_emVendorId;
	s8            m_achProductId[LEN_USERNAME+1];
	s8            m_achVersionId[LEN_USERNAME+1];
	tagSipNodeInfo()
	{
		m_emVendorId = emVendorUnknow;
		memset( m_achProductId, 0, sizeof(m_achProductId) );
		memset( m_achVersionId, 0, sizeof(m_achVersionId) );
	}
}TSipNodeInfo;

typedef struct tagSessionInfo
{
	s8     m_achSessionName[SESSION_NAME_LEN + 1];
    s8     m_achOriginSID[SESSION_NAME_LEN + 1];
	s32    m_nOriginVer;
	u32    m_dwStartTime;
	u32    m_dwEndTime;
public:
	tagSessionInfo()
	{
		memset( this, 0, sizeof(tagSessionInfo) );
	}
}TSessionInfo;


enum emSipCallType
{
	emSipDirect = 0,         //直接呼叫
	emSipByRedirect,         //通过重定向服务器呼叫
	emSipByPxy,              //通过代理服务器呼叫
};

enum emSipConnType
{
	emSipTcpConn,
	emSipUdpConn,
	emSipSCTPConn,
	emSipTLSConn,
};

typedef struct tagSipRegName
{
	emHostNameType m_emSipAddrType;
	s8 m_achUserName[LEN_USERNAME];
	s8 m_achPassword[LEN_PASSWORD];

	tagSipRegName()
	{
		memset( this, 0, sizeof(tagSipRegName) );
	}
}TSipRegName;

typedef struct tagSipServerRegisterReq
{
	TSipRegName m_atSipRegName[MAX_NUM_REGNAME_SIZE];
	s32 m_nSipRegNameCount;

	TSipTransAddress m_atContactAddr[MAX_CONTACKADDR_NUM];

	u32 m_dwExpires;
	s8 m_aCallId[MAX_CALLID_NUM];
	s32 m_nCSqence;
	emSipConnType m_emSipConnType;
	BOOL32 m_bIsInterReg;
public:
	tagSipServerRegisterReq()
	{
		memset(this , 0 , sizeof(tagSipServerRegisterReq));
		m_emSipConnType = emSipTcpConn;
	}
}TSipServerRegisterReq;

typedef struct tagSipServerRegRsp
{
	TSipTransAddress m_tContactAddr;
	emSipConnType m_emSipConnType;
	BOOL32 m_bSerchPrefix;
	s8 m_achAlias[LEN_USERNAME];
	s32 m_nExpire;

	tagSipServerRegRsp()
	{
		memset( this, 0, sizeof(tagSipServerRegRsp) );
		m_bSerchPrefix = FALSE;
		m_emSipConnType = emSipTcpConn;
	}
}TSipServerRegRsp;


typedef struct tagSipCallParameter
{
	u16            m_dwCallBitrate;
	TSipUrl        m_tCalledUrl;
	TSipUrl        m_tCallingUrl;
	emSipCallType  m_emCallType;
	TSipHostName   m_tServerAddr;
	u16            m_wServerPort;
	emSipConnType  m_emConnType;
	s32            m_nProductIdSize;
	u8             m_abyProductId[LEN_USERNAME];
	s32            m_nVersionIdSize;
	u8             m_abyVersionId[LEN_USERNAME];
public:
	tagSipCallParameter()
	{
		memset( this, 0, sizeof(tagSipCallParameter) );
		m_emConnType = emSipTcpConn;
	}
}TSipCallParameter;

enum emSipCapDirection
{
	emSipCapDirectionBegin,
	emSipCapReceive = 1,
	emSipCapTransmit,
	emSipCapRecvAndTrans,
};

typedef struct tagSipAudioCap
{
	tagSipAudioCap()
	{
		Clear();
	}
	void Clear() 
	{
		m_emCapDirection = emSipCapRecvAndTrans;
		m_byPackTime = 60;
		m_byMaxFrameNum = 1;
		m_bSilenceSuppression = TRUE;
		m_dwSampleRate = 8000;
		m_dwBitrate = 64;
		m_wDynamicPayload = 0;
		m_emPayloadType = emSipPayloadTypeBegin;
	}
	void				SetSampleRate( u32 dwSampleRate ) { m_dwSampleRate = dwSampleRate; }
	u32&				GetSampleRate() { return m_dwSampleRate; }

	void				SetBitrate( u32 dwBitrate ) { m_dwBitrate = dwBitrate; }
	u32&			    GetBitrate() { return m_dwBitrate; }

	void			    SetDynamicPayload( u16 wDynamicPayload ) { m_wDynamicPayload = wDynamicPayload; }
	u16&				GetDynamicPayload() { return m_wDynamicPayload; }

	void				SetPackTime( u8 packTime ) { m_byPackTime = packTime; }
	u8&				    GetPackTime() { return m_byPackTime; }

	void				SetIsSilenceSuppression( BOOL32 bTrue ){ m_bSilenceSuppression = bTrue; }
	BOOL32&				GetIsSilenceSuppression() { return m_bSilenceSuppression; }

	void			    SetMaxFrameNum( u8 frameNum ) { m_byMaxFrameNum = frameNum; }
	u8&				    GetMaxFrameNum() { return m_byMaxFrameNum; }

	void				SetPayloadType( emSipPayloadType emPT ) { m_emPayloadType = emPT; }
	emSipPayloadType&	GetPayloadType() { return m_emPayloadType; }

	emSipCapDirection   m_emCapDirection;
	u8				    m_byPackTime;
	u8					m_byMaxFrameNum;
	BOOL32				m_bSilenceSuppression;
	u32					m_dwSampleRate;
	u32					m_dwBitrate;
	u16					m_wDynamicPayload;
	emSipPayloadType	m_emPayloadType;
}TSipAudioCap;

typedef struct tagSipVideoCap
{
	emSipCapDirection m_emCapDirection;
	u16               m_wMaxBitrate;
	emSipPayloadType  m_emPayloadType;
	u8                m_abyResolution[emSipResEnd];
	u16               m_wDynamicPayload;
	u32               m_dwSampleRate;
public:
	tagSipVideoCap()
	{
		Clear();
	}
	void Clear()
	{
		m_emCapDirection = emSipCapRecvAndTrans;
		m_wMaxBitrate = 0;
		m_emPayloadType = emSipPayloadTypeBegin;
		memset( m_abyResolution, 0, sizeof(m_abyResolution) );
		m_wDynamicPayload = 0;
		m_dwSampleRate = 90000;
	}
	
	BOOL SetRes( emSipRes emRes, u8 byMpi )
	{
		if( emRes <= emSipResBegin || emRes >= emSipResEnd )
		{
			return FALSE;
		}
		m_abyResolution[emRes] = byMpi;

		return TRUE;
	}

	u8 GetRes( emSipRes emRes )
	{
		if( emRes <= emSipResBegin || emRes >= emSipResEnd )
		{
			return 0;
		}
		return m_abyResolution[emRes];
	}

	void SetBitrate( u16 wBiterate )
	{
		m_wMaxBitrate = wBiterate;
	}
	u16& GetBitrate()
	{
		return m_wMaxBitrate;
	}


	void SetSampleRate( u32 dwSampleRate )
	{
		m_dwSampleRate = dwSampleRate;
	}
	u32& GetSampleRate()
	{
		return m_dwSampleRate;
	}
	
	void SetDynamicPayload( u16 wDynamicPayload )
	{
		m_wDynamicPayload = wDynamicPayload;
	}
	u16& GetDynamicPayload()
	{
		return m_wDynamicPayload;
	}

}TSipVideoCap;

typedef struct tagSipH264Cap
{
	enum emSipH264Profile
	{
		emSipProfileBaseline = 64,
		emSipProfileMain     = 32,
		emSipProfileExtend   = 16,
		emSipProfileHigh     = 8,
	};

	tagSipH264Cap()
	{
		Clear();
	}

	void Clear()
	{
		memset( this, 0, sizeof(tagSipH264Cap) );
		m_emCapDirection = emSipCapRecvAndTrans;
		m_wMaxBitrate = 0;
		m_byProfile = emSipProfileBaseline;
		m_byLevel = 0;
		m_nFlag = 0;
		m_nMaxBRandCPB = 0;
		m_nMaxDPB = 0;
		m_nMaxStaticMBPS = 0;
		m_dwSampleRate = 90000;
		m_wDynamicPayload = 0;
	}

	void SetCapDirection( emSipCapDirection emDirect )
	{
		m_emCapDirection = emDirect;
	}

	emSipCapDirection GetCapDirection() const
	{
		return m_emCapDirection;
	}

	void SetBitrate( u16 wBitrate )
	{
		m_wMaxBitrate = wBitrate;
	}

	u16 GetBitrate( ) const
	{
		return m_wMaxBitrate;
	}

	void SetProfile( u8 byProfile )
	{
		m_byProfile = byProfile;
	}

	u8 GetProfile() const
	{
		return m_byProfile;
	}

	BOOL32 IsSupportProfile( u8 byProfile )
	{
		return  (m_byProfile&byProfile) == byProfile;
	}

	void Res2WxH( u8 byRes, u16& wWidth, u16 &wHeight )
	{
		switch (byRes)
		{
		case emSipCIF:
			wWidth = 352;   wHeight = 288;   break;
		case emSipCIF2:
			wWidth = 576;   wHeight = 352;   break;
		case emSipCIF4:
			wWidth = 704;   wHeight = 576;   break;
		case emSipHD720:
			wWidth = 1280;  wHeight = 720;   break;
		case emSipSXGA:
			wWidth = 1280;  wHeight = 1024;  break;
		case emSipUXGA:
			wWidth = 1600;  wHeight = 1200;  break;
		case emSipHD1080:
			wWidth = 1920;  wHeight = 1080;  break;
		case emSipCIF16:
			wWidth = 1408;  wHeight = 1152;  break;
		case emSipSQCIF:
			wWidth = 128;   wHeight = 96;    break;
		case emSipQCIF:
			wWidth = 256;   wHeight = 192;   break;
		case emSipVGA:
			wWidth = 640;   wHeight = 480;   break;
		case emSipSVGA:
			wWidth = 800;   wHeight = 600;   break;
		case emSipWCIF4:
			wWidth = 1024;  wHeight = 576;   break;
		case emSipXGA:
			wWidth = 1024;  wHeight = 768;   break;
		default:
			wWidth = 352;   wHeight = 288;   break;     // default as cif
		}
		return;
	}

	u8 GetResolution( u16 wWidth, u16 wHeight )
	{
		if( wWidth*wHeight > (m_nMaxFS)<<8 )
		{
			return 0;
		}

		int nFps = 0;
		s32 nMaxFs = (wWidth*wHeight)>>8;
		if ( ((nMaxFs>>8)<<8) < nMaxFs )
		{
			nMaxFs = (nMaxFs>>8) + 1;
			nMaxFs = nMaxFs << 8;
		}
		double dabs = nMaxFs - m_nMaxFS;

		if( m_nMaxStaticMBPS != 0 && fabs( dabs ) <= 2.0 )
		{
			double dMaxMBPS = m_nMaxMBPS;
			double dMaxFs   = m_nMaxFS;
			double dMaxStaticMBPS = m_nMaxStaticMBPS;

			s32 nNewMaxMBPS = (s32)(1.0/(((4.0/dMaxFs)/dMaxMBPS)+(((dMaxFs-4.0)/dMaxFs)/dMaxStaticMBPS)));
			nFps = (nNewMaxMBPS<<8) / (wWidth*wHeight);
		}
		else
		{
			nFps = (m_nMaxMBPS<<8) / (wWidth*wHeight);
		}

		return FPS_ADJUST(nFps);
	}

	u8 GetResolution( u8 byRes )
	{
		u16 wWidth = 0;
		u16 wHeight = 0;

		Res2WxH( byRes, wWidth, wHeight );

		return GetResolution( wWidth, wHeight );
	}


	void SetResolution( u8 byRes, u8 byFps )
	{
		u16 wWidth = 0;
		u16 wHeight = 0;

		Res2WxH( byRes, wWidth, wHeight );
		
		SetResolution( wWidth, wHeight, byFps );
	}

	void SetResolution( u16 wWidth, u16 wHeight, u8 byFps )
	{
		//const u8 abyLevel[13] = {15, 22, 29, 36, 50, 57, 64, 71, 78, 85, 99, 106, 113};
		const u8 abyLevel[13] = {10, 11, 12, 13, 21, 22, 30, 31, 32, 40, 42, 50, 51};

		const s32 MaxMBPSAndMaxFrame[13][2] = {{1485,99},{3000,396},{6000,396},{11880,396},{19800,792},{20250,1620},{40500,1620},
		{108000,3600},{216000,5120},{245760,8192},{522240,8704},{589824,22080},{983040,36864}};

		s32 nNewMaxFs = (wWidth*wHeight)>>8;
// 		if (nNewMaxFs > m_nMaxFS)
// 		{
			m_nMaxFS = nNewMaxFs;
			if ( ((m_nMaxFS>>8)<<8) < m_nMaxFS )
			{
				m_nMaxFS = (m_nMaxFS>>8) + 1;
				m_nMaxFS = m_nMaxFS << 8;
			}
/*		}*/
		s32 nNewMaxMBPS = nNewMaxFs*byFps;
// 		if (nNewMaxMBPS > m_nMaxMBPS)
// 		{
			m_nMaxMBPS = nNewMaxMBPS;
			if ( (m_nMaxMBPS/500*500) < m_nMaxMBPS )
			{
				m_nMaxMBPS = m_nMaxMBPS/500 + 1;
				m_nMaxMBPS = m_nMaxMBPS * 500;
			}
// 		}

		for(s32 i=0; i<13; i++)
		{
			if(m_nMaxMBPS >= MaxMBPSAndMaxFrame[i][0] && m_nMaxFS >= MaxMBPSAndMaxFrame[i][1])
			{
				m_byLevel = abyLevel[i];
				m_nFlag = 0;
				if(m_nMaxMBPS > MaxMBPSAndMaxFrame[i][0])
					m_nFlag = 1;
				if(m_nMaxFS > MaxMBPSAndMaxFrame[i][1])
					m_nFlag = m_nFlag | 2;
			}
			else
			{
				break;
			}
		}
	}

	void SetMaxMBPS( s32 nMax )
	{
		m_nMaxMBPS = nMax;
	}
	s32 GetMaxMBPS() const
	{
		return m_nMaxMBPS;
	}

	void SetMaxFs( s32 nMax )
	{
		m_nMaxFS = nMax;
	}
	s32 GetMaxFs() const
	{
		return m_nMaxFS;
	}

	void SetMaxBRandCPB( s32 nMax )
	{
		m_nMaxBRandCPB = nMax;
	}
	s32 GetMaxBRandCPB() const
	{
		return m_nMaxBRandCPB;
	}

	void SetLevel( u8 byLevel )
	{
		m_byLevel = byLevel;
	}
	u8 GetLevel() const 
	{
		return m_byLevel;
	}

	void SetFlag( s32 flag )
	{
		m_nFlag = flag;
	}
	s32 GetFlag() const
	{
		return m_nFlag;
	}

	void SetMaxStaticMBPS( s32 nMax )
	{
		m_nMaxStaticMBPS = nMax;
	}
	s32 GetMaxStaticMBPS() const
	{
		return m_nMaxStaticMBPS;
	}

	BOOL32 operator <= ( const tagSipH264Cap& tH264Cap )
	{
		if( m_nMaxMBPS <= tH264Cap.GetMaxMBPS() && m_nMaxFS <= tH264Cap.GetMaxFs() &&
			( m_byProfile | tH264Cap.GetProfile() ) == tH264Cap.GetProfile() )
		{
			return TRUE;
		}
		return FALSE;
	}

	void operator = ( const tagSipH264Cap& tH264Cap )
	{
		memcpy( this, &tH264Cap, sizeof(tH264Cap) );
	}

	BOOL32 operator == ( const tagSipH264Cap& tH264Cap )
	{
		if( !memcmp( this, &tH264Cap, sizeof(tH264Cap) ) )
			return TRUE;
		return FALSE;
	}

	void SetSampleRate( u32 dwSampleRate ) { m_dwSampleRate = dwSampleRate; }
	u32  GetSampleRate() { return m_dwSampleRate; }
	void SetDynamicPayload( u16 wDynamicPayload ) { m_wDynamicPayload = wDynamicPayload; }
	u16& GetDynamicPayload() { return m_wDynamicPayload; }

	emSipCapDirection m_emCapDirection;
	u16               m_wMaxBitrate;
	u8                m_byProfile;
	u8                m_byLevel;
	s32               m_nMaxMBPS;
	s32               m_nMaxFS;

	s32               m_nMaxDPB;
	s32               m_nMaxBRandCPB;
	s32               m_nMaxStaticMBPS;
	u32               m_dwSampleRate;
	u16               m_wDynamicPayload;

	s32               m_nFlag;

}TSipH264Cap;

typedef struct tagSipMediaStream
{
public:
	tagSipMediaStream() { Clear(); }
	void				Clear();
	void				ClearCap();
	void			    SetMediaType( emSipMediaType emType ) { m_emMediaType = emType; }
	emSipMediaType		GetMediaType() const { return m_emMediaType; }
	void				SetMediaCount( u8 count ) { m_byMediaCount = count; }
	u8					GetMediaCount() const { return m_byMediaCount; }
	void				SetBitRate( u32 bitrate ) { m_dwBitRate = bitrate; }
	u32					GetBitRate() const { return m_dwBitRate; }
	TSipTransAddress&	GetLocalRtp() { return m_tLocalRtp; }
	TSipTransAddress&	GetPeerRtp() { return m_tPeerRtp; }
	TSipTransAddress&	GetLocalRtcp() { return m_tLocalRtcp; }
	TSipTransAddress&	GetPeerRtcp() { return m_tPeerRtcp; }
	void				SetLocalIP( u32 dwIP ) { m_dwLocalIp = dwIP; }
	u32					GetLocalIP() const { return m_dwLocalIp; }
	void				SetLocalRtp( u16 wPort ) { m_tLocalRtp.m_dwIP = m_dwLocalIp; m_tLocalRtp.m_wPort = wPort; }
	void				SetLocalRtcp( u16 wPort ){ m_tLocalRtcp.m_dwIP = m_dwLocalIp; m_tLocalRtcp.m_wPort = wPort; }
	void                SetPeerRtp( u32 dwIp, u16 wPort ){ m_tPeerRtp.m_dwIP = dwIp; m_tPeerRtp.m_wPort = wPort; }
	void                SetPeerRtcp( u32 dwIp, u16 wPort ) { m_tPeerRtcp.m_dwIP = dwIp; m_tPeerRtcp.m_wPort = wPort; }
	emSipPayloadType	GetMediaPayload( u8 byIdx );
	void				SetMediaPayloadArray( s32 nPos, emSipPayloadType emSipPT );
	TSipAudioCap&		GetAudioCap( emSipPayloadType emPayload );
	TSipVideoCap&		GetVideoCap( emSipPayloadType emPayload );
	TSipH264Cap&		GetH264VideoCap( emSipPayloadType emPayload ){ return m_tH264Cap; }
	void				AddAudioMedia( emSipPayloadType emType, TSipAudioCap& tAudio );
	void				AddVideoMedia( emSipPayloadType emType, TSipVideoCap& tVideo );
	void				AddH264Media( emSipPayloadType emType, TSipH264Cap& tH264 );

	BOOL32&             GetIsMatched() { return m_bMatched; }
	void                SetIsMatched( BOOL32 bIsMatched ) { m_bMatched = bIsMatched; }
	void				SetActivePayload( emSipPayloadType emPayload ) { m_emActivePayloadType = emPayload;}
	emSipPayloadType&   GetActivePayload() { return m_emActivePayloadType; }

	BOOL32              IsSupport( emSipPayloadType payload, TSipAudioCap* tAudio = NULL, TSipVideoCap* tVideo = NULL, TSipH264Cap* tH264 = NULL );
	void				SetIsActive( BOOL32 bActive ){ m_bActive = bActive; }
	BOOL32&				GetIsActive() { return m_bActive; }
private:
	BOOL32           m_bActive;
	emSipMediaType	 m_emMediaType;
	emSipPayloadType m_emActivePayloadType;
	u32              m_dwBitRate;
	u32              m_dwLocalIp;

	emSipPayloadType m_aemMediaPT[10];
	u8               m_byMediaCount;


	TSipTransAddress m_tLocalRtp;
	TSipTransAddress m_tLocalRtcp;
	TSipTransAddress m_tPeerRtp;
	TSipTransAddress m_tPeerRtcp;


	TSipVideoCap m_tH263Cap;
	TSipVideoCap m_tH263PlusCap;
	TSipH264Cap  m_tH264Cap;

	TSipAudioCap m_tG711a;
	TSipAudioCap m_tG711u;
	TSipAudioCap m_tG722;
	TSipAudioCap m_tG728;
	TSipAudioCap m_tG729;
	TSipAudioCap m_tG723;
	TSipAudioCap m_tG719;
	TSipAudioCap m_tG7231;
	TSipAudioCap m_tG7221;
	BOOL32       m_bMatched;
}TSipMediaStream;

typedef struct tagMediaDesc
{
	emSipCapDirection m_emDirect;
	u8                m_bySendPos;
	u8                m_byRecvPos;
	tagMediaDesc()
	{
		m_emDirect = emSipCapDirectionBegin;
		m_byRecvPos = 0;
		m_bySendPos = 0;
	}
}TMediaDesc;

typedef struct tagSipCapSet
{
public:
	enum {  
		MAX_AUDIO_STREAM_NUM = 1,
		MAX_VIDEO_STREAM_NUM = 2,
	};

	tagSipCapSet();

	void Clear();

	void SetLocalIP( u32 dwIP ) { m_dwLocalIp = dwIP; }
	u32 GetLocalIP() const { return m_dwLocalIp; }

	void SetBandWidth( s32 nBandwidth ) { m_nBandWidth = nBandwidth; }
	s32 GetBandWidth() const { return m_nBandWidth; }

	void SetAudioNum( u32 count ) { m_dwAudioCount = count; }
	u32 GetAudioNum() const { return m_dwAudioCount; }

	void SetVideoNum( u32 count ) { m_dwVideoCount = count; }
	u32 GetVideoNum() const { return m_dwVideoCount; }

	TMediaDesc* GetMediaDesc( u32 dwCount, BOOL32 bVideo = FALSE );
	BOOL32 SetMediaDesc( u32 dwCount, emSipCapDirection emDirect, u8 bySndPos, u8 byRcvPos, BOOL32 bVideo = FALSE );

	void SetMediaStream( TSipMediaStream* ptMediaStream, emSipMediaType emMediaType, u8 byPos, emSipCapDirection emDirect );
	TSipMediaStream* GetMediaStream( emSipMediaType emMediaType, u8 byPos, emSipCapDirection emDirect );

	s32 GetVideoSendStreamCount() { return m_nVideoSendStreamCount; }
	s32 GetVideoRecvStreamCount() { return m_nVideoRecvStreamCount; }
	s32 GetAudioSendStreamCount() { return m_nAudioSendStreamCount; }
	s32 GetAudioRecvStreamCount() { return m_nAudioRecvStreamCount; }

	void SetVideoSendStreamCount( s32 count ) { m_nVideoSendStreamCount = count; }
	void SetVideoRecvStreamCount( s32 count ) { m_nVideoRecvStreamCount = count; }
	void SetAudioSendStreamCount( s32 count ) { m_nAudioSendStreamCount = count; }
	void SetAudioRecvStreamCount( s32 count ) { m_nAudioRecvStreamCount = count; }

	void AddSendVideoStreamCount();
	void AddRecvVideoStreamCount();
	void AddSendAudioStreamCount();
	void AddRecvAudioStreamCount();

	void ResetIsMatched();
	void AddCapToMedia( emSipPayloadType emPayloadType, BOOL32 bRecv, int nSequence, TSipAudioCap* ptAudioCap = NULL, TSipVideoCap* ptVideoCap = NULL, TSipH264Cap* ptH264 = NULL );
	s8 GetMediaDescPosBySendPos( u8 bySendPos, BOOL32 bVideo );
	s8 GetMediaDescPosByRecvPos( u8 byRecvPos, BOOL32 bVideo );

private:
	TSipMediaStream m_atAudioSendStream[MAX_AUDIO_STREAM_NUM];
	s32 m_nAudioSendStreamCount;
	TSipMediaStream m_atAudioRecvStream[MAX_AUDIO_STREAM_NUM];
	s32 m_nAudioRecvStreamCount;
	TMediaDesc m_atAudioDescription[MAX_AUDIO_STREAM_NUM*2];
	u32 m_dwAudioCount;
	
	TSipMediaStream m_atVideoSendStream[MAX_VIDEO_STREAM_NUM];
	s32 m_nVideoSendStreamCount;
	TSipMediaStream m_atVideoRecvStream[MAX_VIDEO_STREAM_NUM];
	s32 m_nVideoRecvStreamCount;
	TMediaDesc m_atVideoDescription[MAX_VIDEO_STREAM_NUM*2];
	u32 m_dwVideoCount;

	s32 m_nBandWidth;
	u32 m_dwLocalIp;
}TSipCapSet;



RV_DECLARE_HANDLE(RvSipAppChanHandle);
RV_DECLARE_HANDLE(RvSipChanHandle);
RV_DECLARE_HANDLE(RvSipAppRASHandle);
RV_DECLARE_HANDLE(RvSipRASHandle);


/*====================================================================
函数名      : SipNotifyNewCall
功能        : 交换呼叫句柄
算法实现    : 无
引用全局变量: 无
输入参数说明: hsCall   - 协议栈呼叫句柄
    		  lphaCall - 应用程序呼叫句柄
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (RVCALLCONV* SipNotifyNewCall)( RvSipCallLegHandle hsCall, LPRvSipAppCallLegHandle lphaCall, const void* pBuf, u16 nBufLen);

/*====================================================================
函数名      : SipNotifyCallCtrl
功能        : 呼叫控制通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall  - 应用程序呼叫句柄
			  hsCall  - 协议栈呼叫句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (RVCALLCONV* SipNotifyCallCtrl)(RvSipAppCallLegHandle haCall,RvSipCallLegHandle hsCall, u16 msgType,
											  const void* pBuf,u16 nBufLen);


/*====================================================================
函数名      : SipNotifyChanCtrl
功能        : 信道控制通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall  - 应用程序呼叫句柄
              hsCall  - 协议栈呼叫句柄
              haChan  - 应用程序信道句柄
			  hsChan  - 协议栈信道句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (RVCALLCONV* SipNotifyChanCtrl)(RvSipAppCallLegHandle haCall,RvSipCallLegHandle hsCall,
											 RvSipAppChanHandle haChan,RvSipChanHandle hsChan,
											 u16 msgType,const void* pBuf,u16 nBufLen);

/*====================================================================
函数名      : SipNotifyConfCtrl
功能        : 会议控制通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall  - 应用程序呼叫句柄
              hsCall  - 协议栈呼叫句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (RVCALLCONV* SipNotifyConfCtrl)(RvSipAppCallLegHandle haCall,RvSipCallLegHandle hsCall,
											 RvSipTranscHandle hTransc,u16 msgType,const void* pBuf,u16 nBufLen);


/*====================================================================
函数名      : SipNotifyRasCtrl
功能        : RAS信息通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall  - 应用程序呼叫句柄
              hsCall  - 协议栈呼叫句柄
              haRas   - 应用程序RAS句柄
			  hsRas   - 协议栈RAS句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (RVCALLCONV* SipNotifyRegCtrl)(IN  RvSipAppRegClientHandle         hAppRegClient,
											IN  RvSipRegClientHandle            hRegClient,
                                            u16 msgType, const void* pBuf,u16 nBufLen);

typedef s32 (RVCALLCONV* SipNotifySipServerMsg)(IN RvSipTranscHandle hTransc, u16 msgType, 
												const void* pBuf, u16 nBufLen);


typedef struct 
{
	SipNotifyNewCall   fpNewCall;
	SipNotifyCallCtrl  fpCallCtrlMsg;
	SipNotifyChanCtrl  fpChanCtrlMsg;
	SipNotifyConfCtrl  fpConfCtrlMsg;
	SipNotifyRegCtrl   fpRegCtrlMsg;
	SipNotifySipServerMsg fpSipServerMsg;
}SIPCALLEVENT,*LPSIPCALLEVENT;


class CSipMediaStreamCtrl;

class CHPSipAdapter
{
public:
	static RvStatus SipStackInit( TSipStackCfg tSipCfg );
	static RvStatus SipStackQuit();
	static RvStatus SipSetAdpCallBack( LPSIPCALLEVENT lpCallBack );
	static void     SipCheckMessage( u32 dwMiliSecond );
	static RvStatus SetAppHandleToCall( IN RvSipCallLegHandle hCallLeg, IN RvSipAppCallLegHandle hAppCallLeg );
	static RvStatus SipCreateNewCall( RvSipAppCallLegHandle haCall, LPRvSipCallLegHandle lphsCall );
	static BOOL32   SipCraateNewChan( RvSipCallLegHandle hsCall, RvSipAppChanHandle haChan, emSipMediaType byMediaType, BOOL bSend, LPRvSipChanHandle lphsChan );
	static RvStatus SipReleaseChan( RvSipCallLegHandle hsCall, RvSipChanHandle hsChan );
	static s32      SipSendCallCtrl( RvSipCallLegHandle hsCall, u16 msgType, u8* pBuf, u16 nBufLen );
	static s32      SipSendChanCtrl( RvSipCallLegHandle hsCall, RvSipChanHandle hsChan, u8 byMsgType, u8* pBuf, u16 nBufLen );
	static s32      SipSendRegCtrl( emSipRegCtrlMsgType emMsgType, u8* pBuf, u16 nBufLen );
	static s32      SipSendSipServerCtrl( RvSipTranscHandle hTransc, emSipServerMsgType emMsgType, const u8* pBuf, u16 nBufLen );
	static s32      SipSetSdpSession( RvSipCallLegHandle hsCall, char* pchSessionName, u32 dwStartTime, u32 dwStopTime );
	static s32      OnCreateRegClient( RvSipAppRegClientHandle hAppReg, LPRvSipRegClientHandle lphReg );


private:
	static s32 OnSipMakeCall( RvSipCallLegHandle hsCall, u8* pBuf,u16 nBufLen );
	static s32 OnSipAcceptCall( RvSipCallLegHandle hsCall, u8* pBuf,u16 nBufLen );
	static s32 OnSipSendAck( RvSipCallLegHandle hsCall, u8* pBuf,u16 nBufLen );
	static s32 OnSipCallDisconnect( RvSipCallLegHandle hsCall, u8* pBuf,u16 nBufLen );
	static s32 OnSipReinvite( RvSipCallLegHandle hsCall, u8* pBuf, u16 nBufLen );


	static void		ConstructSipUrl( TSipUrl tUrl, s8* achOut, emSipConnType emSipConnectType = emSipTcpConn, BOOL32 bPassword = FALSE, BOOL32 bTo = FALSE );
	static RvStatus ConstructSipAllowHeader( RvSipMsgHandle hMsg );
	static RvStatus ConstructSipSupportedHeader( RvSipMsgHandle hMsg );
	static RvStatus AddSipSdpToMsg( RvSipCallLegHandle hsCall, TSipCapSet* ptCapSet, BOOL32 bSave = TRUE );
	static RvStatus GetSipSdpFromMsg( RvSipCallLegHandle hCallLeg, TSipCapSet& tCapSet );
	static BOOL32	ConstructSdpFromCapSet( RvSdpMsg* pSdpMsg, RvSdpConnectionMode rvMode, TSipMediaStream* pcMediaSendStream = NULL, u16 wRecvRtcpPort = 0 );
	static RvStatus ConstructCapSetFromSdp( TSipMediaStream *pcStream, RvSdpMediaDescr *pMediaDesc , u32 dwConnectionAddr, s32 nCallBandWidth );
	static s32      GetFmtpRelatedRtpmapPos( s32 nFormat, RvSdpMediaDescr *pMediaDesc );
	static void     ConstructH264Capset( RvSdpRtpMap* pSdpRtpMap, TSipH264Cap &tH264Cap, char *achAttr );
	static void     ConstructVideoCapset( RvSdpRtpMap* pSdpRtpMap, TSipVideoCap &tVideoCap, char *achAttr );
	static RvStatus GetSipCallParamFromMsg( RvSipCallLegHandle hCallLeg, TSipCallParameter& tCallParam, BOOL bCaller );
	static s8*		GetFmtpAttr( RvSdpMediaDescr* pMediaDesc, s32 nPayload );


	static BOOL32 IsAudioType( emSipPayloadType emType );
	static BOOL32 IsVideoType( emSipPayloadType emType );
	static u16 GetStaticPayload( emSipPayloadType emType );
	static emSipPayloadType GetFormat( s32 wPayload );
	static s8* GetMediaTypeName( emSipPayloadType emType );
	static emSipPayloadType GetPayloadByName( s8 *pchName );
	static s8* GetResName( emSipRes emResType );
	static BOOL32 GetValueByFieldIds( char *pchString, char *pchField, int &nValue );
	static BOOL32 GetValueByFieldIds( char *pchString, char *pchField, char *pValue );

	static void PrintStackLog( RvSipMsgHandle hMsg );

	static void OnStateChangeRedirect( RvSipCallLegHandle hsCall, RvSipAppCallLegHandle hAppCall );
	static void OnStateChangeOffer( RvSipCallLegHandle hCallLeg, RvSipAppCallLegHandle hAppCallLeg, TSipCapSet& tCapSet, TSipCallParameter& tCallParam );
	static void OnStateChangeConnected( RvSipCallLegHandle hCallLeg, RvSipAppCallLegHandle hAppCallLeg, RvSipCallLegDirection eDirect, TSipCapSet& tCapSet, TSipCallParameter& tCallParam );
	
	//reg
	static void InitRegClient();
	static void OnRegister( u8* pBuf, u16 nBufLen );
	static void OnUnregister( u8* pBuf, u16 nBufLen );
	
public:
	static void SipSetStackCallBack();

	static void RVCALLCONV SipCallLegCreatedEvHandler( 
		IN  RvSipCallLegHandle     hCallLeg,
		OUT RvSipAppCallLegHandle  *phAppCallLeg );

	static void RVCALLCONV SipCallLegStateChangedEvHandler( 
		IN  RvSipCallLegHandle hCallLeg,
		IN  RvSipAppCallLegHandle  hAppCallLeg,
		IN  RvSipCallLegState      eState,
		IN  RvSipCallLegStateChangeReason eReason );

	static void RVCALLCONV SipCallLegModifyStateChangedEvHandler( 
		IN  RvSipCallLegHandle hCallLeg,
		IN  RvSipAppCallLegHandle   hAppCallLeg,
		IN  RvSipCallLegModifyState eState,
		IN  RvSipCallLegStateChangeReason eReason );

	static RvStatus RVCALLCONV SipCallLegMsgToSendEvHandler( 
		IN  RvSipCallLegHandle          hCallLeg,
		IN  RvSipAppCallLegHandle       hAppCallLeg,
		IN  RvSipMsgHandle              hMsg );

	static RvStatus RVCALLCONV SipCallLegMsgReceivedEvHandler( 
		IN  RvSipCallLegHandle       hCallLeg,
		IN  RvSipAppCallLegHandle    hAppCallLeg,
		IN  RvSipMsgHandle           hMsg );

	static void  RVCALLCONV SipCallLegTranscCreatedEvHandler( 
		IN  RvSipCallLegHandle            hCallLeg,
		IN  RvSipAppCallLegHandle         hAppCallLeg,
		IN  RvSipTranscHandle             hTransc,
		OUT RvSipAppTranscHandle          *hAppTransc,
		OUT RvBool                        *bAppHandleTransc );

	static void RVCALLCONV SipCallLegTranscStateChangedEvHandler(
		IN  RvSipCallLegHandle                hCallLeg,
		IN  RvSipAppCallLegHandle             hAppCallLeg,
		IN  RvSipTranscHandle                 hTransc,
		IN  RvSipAppTranscHandle              hAppTransc,
		IN  RvSipCallLegTranscState           eTranscState,
		IN  RvSipTransactionStateChangeReason eReason );

	static void RVCALLCONV SipRegClientStateChangedEvHandler(
		IN  RvSipRegClientHandle            hRegClient,
		IN  RvSipAppRegClientHandle         hAppRegClient,
		IN  RvSipRegClientState             eState,
		IN  RvSipRegClientStateChangeReason eReason);

	static RvStatus RVCALLCONV SipRegClientMsgReceivedEvHandler(
		IN  RvSipRegClientHandle          hRegClient,
		IN  RvSipAppRegClientHandle       hAppRegClient,
		IN  RvSipMsgHandle                hMsg );

	static void RVCALLCONV SipStackLog(
		IN void*                          context , 
		IN RvSipLogFilters                filter , 
		IN const RvChar*                  formattedText);

	static void RVCALLCONV   SipTransactionStateChangedEv(	
		IN RvSipTranscHandle                 hTransc,
		IN RvSipTranscOwnerHandle            hTranscOwner,
		IN RvSipTransactionState             eState,
		IN RvSipTransactionStateChangeReason eReason );

	static void RVCALLCONV  SipTransactionCreatedEv(	
		IN    RvSipTranscHandle        hTransc,
		IN    void                     *pAppContext,
		OUT   RvSipTranscOwnerHandle   *hAppTransc,
		OUT   RvBool                   *b_handleTransc );


private:
	static s8 m_achVendorId[LEN_USERNAME + 1];
	static s8 m_achProductId[LEN_USERNAME + 1];
	static s8 m_achVersionId[LEN_USERNAME + 1];

	static RvSipStackHandle m_hStackMgr;
	static RvSipCallLegMgrHandle m_hCallLegMgr;
	static RV_LOG_Handle m_hLog;
	static HRPOOL m_hRPool;
	static RvSipAuthenticatorHandle m_hAuthMgr;

	static SIPCALLEVENT m_tSipCallBack;

	static TRegClient m_atRegClient[MAX_NUM_CLIENT_REG];

	static void SipVendorInfo();
	static void SipSetAdpCallBack();

private:
	static CSipMediaStreamCtrl* m_pcSipMediaStreamCtrl;
};


template < class T >
T SAFE_CAST( T &t, u8 *p )
{
	if (NULL == p) return t;
	memcpy( &t, p, sizeof( T ) );
	return t;
};

#endif