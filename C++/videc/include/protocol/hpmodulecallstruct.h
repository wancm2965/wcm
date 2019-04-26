#ifndef _HP_MODULECALLSTRUCT_H_
#define _HP_MODULECALLSTRUCT_H_


#define MAX_LEN_CALLID       32
#define MAX_LEN_CALL_ALIAS   64
#define MAX_LEN_PRODUCT_ID   64
#define MAX_LEN_VERSION_ID   64
#define MAX_AUDIO_NUM        5
#define MAX_VIDEO_NUM        5
#define MAX_ENCRYPT_KEY_LEN  128
#define MAX_LEN_EPID		 64
#define MAX_ALIAS_NUM        64
#define MAX_E164_NUM         64

#define CALLINGTIMEOUT			  20
#define CALLDISCONNECTTIMEOUT	  20
#define SENDROUNDTRIPTIMEOUT      5


#define MAKEDWORD(l,h)  ((u32)(((u16)(l)) | ((u32)((u16)(h))) << 16))

#ifndef HP_MODULE_DECLARE_HANDLE
#define HP_MODULE_DECLARE_HANDLE(name)    typedef struct { int unused; } name##__ ; \
	typedef const name##__ * name; \
	typedef name*  LP##name;
#endif

#ifndef IP2CHAR

#define IP2CHAR(ip)  ((u8 *)&(ip))[0], \
	((u8 *)&(ip))[1], \
	((u8 *)&(ip))[2], \
	((u8 *)&(ip))[3]
#endif

template < class T >
T SAFE_COPY( T &t, u8 *p )
{
	if (NULL == p) return t;
	memcpy( &t, p, sizeof( T ) );
	return t;
}


#ifdef WIN32
#define MODULECALLBACK __cdecl
#else
#define MODULECALLBACK
#endif


HP_MODULE_DECLARE_HANDLE(HMDLAPPCALL)
HP_MODULE_DECLARE_HANDLE(HMDLCALL)
HP_MODULE_DECLARE_HANDLE(HMDLAPPCHANNEL)
HP_MODULE_DECLARE_HANDLE(HMDLCHANNEL)
HP_MODULE_DECLARE_HANDLE(HMDLREG)
HP_MODULE_DECLARE_HANDLE(HMDLAPPREG)
HP_MODULE_DECLARE_HANDLE(HMDLAPPCONFID)



enum emHpModuleEndpointType
{
	emHpModuleEndpointTypeBegin = 0,
	emHpModuleEndpointMT ,
	emHpModuleEndpointMcu,
	emHpModuleEndpointGk , 
	emHpModuleEndpointGateway
};

enum emHpModuleIpType
{
	emHpModuleIPV4,
	emHpModuleIPV6,
};

enum emHpModuleCallType
{
	emHpModuleIPCall,
	emHpModuleAliasCall,
	emHpModuleE164Call,
};

enum emHpModuleDisconnectReason
{
	emHpModuleDisconnect_Busy = 1,
	emHpModuleDisconnect_Normal,
	emHpModuleDisconnect_Rejected,
	emHpModuleDisconnect_Unreachable,
	emHpModuleDisconnect_Local,
	emHpModuleDisconnect_unknown,
};


enum emHpModuleVendorID
{
	emHpModule_vendor_MICROSOFT_NETMEETING = 1,
	emHpModule_vendor_AVCON,
	emHpModule_vendor_KEDA,
	emHpModule_vendor_HUAWEI,
	emHpModule_vendor_POLYCOM,
	emHpModule_vendor_POLYCOM_MCU,
	emHpModule_vendor_TANDBERG,
	emHpModule_vendor_RADVISION,
	emHpModule_vendor_VCON,
	emHpModule_vendor_SONY,
	emHpModule_vendor_VTEL,
	emHpModule_vendor_ZTE,
	emHpModule_vendor_ZTE_MCU,
	emHpModule_vendor_CODIAN,
	emHpModule_vendor_AETHRA,
	emHpModule_vendor_CHAORAN,
	emHpModule_vendor_UNKNOWN,
	emHpModule_vendor_END
};


struct THpModuleIpAddr
{
	emHpModuleIpType m_emIpType;
	u32 m_dwIp;
	u8  m_abyIp[16];
public:
	THpModuleIpAddr()
	{
		memset( this , 0 , sizeof(THpModuleIpAddr) );
	}

	void SetIpAddr( u32 dwIp ) 
	{
		m_emIpType = emHpModuleIPV4;
		m_dwIp = dwIp;
	}
	u32 GetIpAddr()
	{
		if ( emHpModuleIPV4 == m_emIpType )
		{
			return m_dwIp;
		}
		else
			return 0;
	}
	void SetIpv6Addr( u8 abyIpv6[] )
	{
		if ( NULL == abyIpv6 )
		{
			return;
		}
		m_emIpType = emHpModuleIPV6;
		memcpy( m_abyIp , abyIpv6 , 16 );
	}

	void GetIpv6Addr( u8* pbyIpv6 )
	{
		if ( NULL == pbyIpv6 )
		{
			return;
		}
		if ( emHpModuleIPV6 == m_emIpType )
		{
			memcpy( pbyIpv6 , m_abyIp , 16 );
		}
		else
			return;
	}


};

struct THpModuleAliasAddr
{
	u8 m_abyCallAliasAddr[MAX_LEN_CALL_ALIAS+1];
public:
	THpModuleAliasAddr()
	{
		Clear();
	}

	void Clear()
	{
		memset( m_abyCallAliasAddr , 0 , sizeof(m_abyCallAliasAddr) );
	}

	void SetCallAliasAddr( u8* pbyAlias , u16 wLen )
	{
		if ( NULL == pbyAlias || 0 == wLen )
		{
			return;
		}
		strncpy( (s8*)m_abyCallAliasAddr , (s8*)pbyAlias ,  MAX_LEN_CALL_ALIAS );
	}

	void GetAliasAddr( u8* pbyAlias , u16 wLen )
	{
		if ( NULL == pbyAlias || 0 == wLen )
		{
			return;
		}
		strncpy( (s8*)pbyAlias , (s8*)m_abyCallAliasAddr, wLen ); 
	}
};

struct THpModuleTransportAddr
{
	emHpModuleIpType m_emIpType;
	u32 m_dwIp;
	u8  m_abyIp[16];
	u16 m_wPort;

public:
	THpModuleTransportAddr()
	{
		Clear();
	}

	void Clear()
	{
		m_emIpType = emHpModuleIPV4;
		m_dwIp = 0;
		memset( m_abyIp , 0 , 16 );
		m_wPort = 0;
	}

	void SetIpAddr( u32 dwIp , u16 wPort ) 
	{
		m_emIpType = emHpModuleIPV4;
		m_dwIp = dwIp;
		m_wPort = wPort;
	}
	void GetIpAddr( u32* dwIp , u16* wpPort )
	{
		if ( NULL == dwIp && NULL == wpPort )
		{
			return;
		}
		if ( emHpModuleIPV4 == m_emIpType )
		{
			*dwIp = m_dwIp;
			*wpPort = m_wPort;
		}
		else
			return;
	}
	void SetIpv6Addr( u8 abyIpv6[] , u16 wPort )
	{
		if ( NULL == abyIpv6 )
		{
			return;
		}
		m_emIpType = emHpModuleIPV6;
		memcpy( m_abyIp , abyIpv6 , 16 );
	}

	void GetIpv6Addr( u8* pbyIpv6 , u16* wpPort )
	{
		if ( NULL != pbyIpv6 && NULL != wpPort )
		{
			return;
		}
		if ( emHpModuleIPV6 == m_emIpType )
		{
			memcpy( pbyIpv6 , m_abyIp , 16 );
		}
		return;
	}
};

struct THpModuleCallInfo
{
	u8 m_abyCallId[MAX_LEN_CALLID+1];

	u16 m_wBitrate;
	emHpModuleEndpointType m_emEndpointType;

	emHpModuleCallType m_emCallType;

	THpModuleTransportAddr m_tCallerIp;
	THpModuleTransportAddr m_tCalleeIp;

	THpModuleAliasAddr m_tCallerAlias;
	THpModuleAliasAddr m_tCalleeAlias;

	THpModuleAliasAddr m_tCallerE164;
	THpModuleAliasAddr m_tCalleeE164;

	u8 m_abyLocalProductId[MAX_LEN_PRODUCT_ID+1];
	u8 m_abyLocalVersionId[MAX_LEN_VERSION_ID+1];

	u8 m_abyPeerProductId[MAX_LEN_PRODUCT_ID+1];
	u8 m_abyPeerVersionId[MAX_LEN_VERSION_ID+1];

	emHpModuleVendorID m_emVendorId;

public:
	THpModuleCallInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_emVendorId = emHpModule_vendor_AVCON;
		m_emEndpointType = emHpModuleEndpointMT;
		m_emCallType = emHpModuleIPCall;
		memset( m_abyLocalProductId , 0 , sizeof(m_abyLocalProductId) );
		memset( m_abyLocalVersionId , 0 , sizeof(m_abyLocalVersionId) );
		memset( m_abyPeerProductId , 0 , sizeof(m_abyPeerProductId) );
		memset( m_abyPeerVersionId , 0 , sizeof(m_abyPeerVersionId) );
		memset( m_abyCallId , 0 , sizeof(m_abyCallId) );
		m_wBitrate = 768;
	}

};


enum emHpModuleVideoFormat
{
	emHpModuleVH261     = 0,
	emHpModuleVH262     = 1,
	emHpModuleVH263     = 2,
	emHpModuleVH263plus = 3,
	emHpModuleVH264     = 4,
	emHpModuleVMPEG4    = 5,
	emHpModuleVEnd		
};

enum emHpModuleAudioFormat
{
	emHpModuleAG711a = 0, 
	emHpModuleAG711u,
	emHpModuleAG722,  
	emHpModuleAG7231,
	emHpModuleAG728,
	emHpModuleAG729,
	emHpModuleAG721,
	emHpModuleAG719,
	emHpModuleAMpegAACLC,
	emHpModuleAMpegAACLD,
	emHpModuleAEnd,	 
};

enum emHpModuleAudioSampleFreq
{
	emHpModuleFs96000 = 0,
	emHpModuleFs88200 = 1,
	emHpModuleFs64000 = 2,
	emHpModuleFs48000 = 3,
	emHpModuleFs44100 = 4,
	emHpModuleFs32000 = 5,
	emHpModuleFs24000 = 6,
	emHpModuleFs22050 = 7,
	emHpModuleFs16000 = 8,
	emHpModuleFs12000 = 9,
	emHpModuleFs11025 = 10,
	emHpModuleFs8000  = 11
};

enum emHpModuleAudioChnlCfg
{
	emHpModuleChnlCust   = 0,
	emHpModuleChnl1      = 1,                   
	emHpModuleChnl2      = 2,                   
	emHpModuleChnl3      = 3,
	emHpModuleChnl4      = 4,
	emHpModuleChnl5      = 5,
	emHpModuleChnl5dot1  = 6,                   
	emHpModuleChnl7dot1  = 7                  
};

enum emHpModuleVideoRes
{
	emHpModuleVResAuto     = 0,   //自动
	emHpModuleVSQCIF              = 1,   //SQCIF88x72
	emHpModuleVQCIF               = 2,   //QCIF176x144
	emHpModuleVCIF                = 3,   //CIF352x288
	emHpModuleV2CIF               = 4,   //2CIF352x576
	emHpModuleV4CIF               = 5,   //4CIF704x576
	emHpModuleV16CIF              = 6,   //16CIF1408x1152

	emHpModuleVHD480i720x480      = 7,   //480i720x480
	emHpModuleVHD480p720x480      = 8,   //480p720x480
	emHpModuleVHD576i720x576      = 9,   //576i720x576
	emHpModuleVHD576p720x576      = 10,  //576p720x576

	emHpModuleVHD720p1280x720     = 11,  //720p1280x720
	emHpModuleVHD1080i1920x1080   = 12,  //1080i1920x1080
	emHpModuleVHD1080p1920x1080   = 13,  //1080p1920x1080


	emHpModuleVGA352x240          = 14,  //352x240
	emHpModuleVGA704x480          = 15,  //704x480
	emHpModuleVGA640x480          = 16,  //VGA640x480
	emHpModuleVGA800x600          = 17,  //SVGA800x600
	emHpModuleVGA1024x768         = 18,  //XGA1024x768
	emHpModuleVGA1280x768         = 19,  //WXGA1280x768
	emHpModuleVGA1280x800         = 20,  //WXGA1280x800
	emHpModuleVGA1366x768         = 21,  //WXGA1366x768
	emHpModuleVGA1280x854         = 22,  //WSXGA1280x854
	emHpModuleVGA1440x900         = 23,  //WSXGA1440x900
	emHpModuleVGA1280x1024        = 24,  //SXGA1280x1024
	emHpModuleVGA1680x1050        = 25,  //SXGA+1680x1050
	emHpModuleVGA1600x1200        = 26,  //UXGA1600x1200
	emHpModuleVGA1920x1200        = 27,  //WUXGA1920x1200

	emHpModuleVResEnd ,
};

enum emHpModuleProfileMask
{
	emHpModuleProfileBaseline = 64,
	emHpModuleProfileMain = 32,
	emHpModuleProfileExtend = 16,
	emHpModuleProfileHigh = 8
};

struct THpModuleVideoDescript
{
	emHpModuleVideoRes m_emRes;
	u8                 m_byMPI;
	u16                m_wMaxBitrate;
public:
	THpModuleVideoDescript()
	{
		Clear();
	}

	void Clear()
	{
		m_emRes = emHpModuleVResEnd;
		m_byMPI = 1;
		m_wMaxBitrate = 768;
	}
};

struct THpModuleVideoFormatCap
{
	u8                    byNum;
	u8                    m_wDynamicPayload;
	emHpModuleVideoFormat m_emFormat;
	emHpModuleProfileMask m_emProfile;
	THpModuleVideoDescript        m_atItem[emHpModuleVResEnd];

	THpModuleVideoFormatCap()
	{
		Clear();
	}

	void Clear()
	{
		byNum = 0;
		m_emFormat = emHpModuleVEnd;
		m_wDynamicPayload = 0;

		for ( u8 i=0; i<emHpModuleVResEnd; i++ )
		{
			m_atItem[i].Clear();
		}
	}
};

struct THpModuleVideoCapList
{	
	u8 m_byNum;
	THpModuleVideoFormatCap m_atList[emHpModuleVEnd];

	HMDLAPPCHANNEL m_hAppChan;
	THpModuleTransportAddr m_tRtpAddr;
	THpModuleTransportAddr m_tRtcpAddr;
	BOOL32 m_bActive;
public:
	THpModuleVideoCapList()
	{
		Clear();
	}

	void Clear()
	{
		m_byNum = 0;
		m_hAppChan = NULL;

		for ( u8 i=0; i<emHpModuleVEnd; i++ )
		{
			m_atList[i].Clear();
		}
		m_tRtpAddr.Clear();
		m_tRtcpAddr.Clear();
		m_bActive = TRUE;
	}
};

struct THpModuleAudioDescript
{
	emHpModuleAudioFormat     m_emFormat;
	u8                        m_byPackTime;
	u8                        m_wDynamicPayload;
	emHpModuleAudioSampleFreq m_emSampleFreq;
	emHpModuleAudioChnlCfg    m_emChnlCfg;

	//g7231用
	u8				          m_byMaxFrameNum;		//单包最大的帧数
	BOOL			          m_bSilenceSuppression;	//是否静音抑制
public: 
	THpModuleAudioDescript()
	{
		Clear();
	}

	void Clear()
	{
		m_emFormat            = emHpModuleAEnd;
		m_byPackTime          = 20;
		m_wDynamicPayload     = 0;
		m_emChnlCfg           = emHpModuleChnl1;
		m_emSampleFreq        = emHpModuleFs32000;
		m_byMaxFrameNum       = 1;
		m_bSilenceSuppression = FALSE;
	}
};

struct THpModuleAudioCapList
{
	u8                     m_byNum;
	THpModuleAudioDescript m_atItem[emHpModuleAEnd];

	HMDLAPPCHANNEL         m_hAppChan;
	THpModuleTransportAddr m_tRtpAddr;
	THpModuleTransportAddr m_tRtcpAddr;
	BOOL32                 m_bActive;

public:
	THpModuleAudioCapList()
	{
		Clear();
	}

	void Clear()
	{
		m_hAppChan = NULL;
		m_byNum = 0;

		for ( u8 i=0; i<emHpModuleAEnd; i++ )
		{
			m_atItem[i].Clear();
		}

		m_tRtpAddr.Clear();
		m_tRtcpAddr.Clear();
		m_bActive = TRUE;
	}
};

struct THpModuleDualCapList
{
	THpModuleVideoCapList m_tSndVidCap[MAX_VIDEO_NUM];
	u16			    m_wSndVidNum;
	THpModuleVideoCapList m_tRcvVidCap[MAX_VIDEO_NUM];
	u16				m_wRcvVidNum;

	THpModuleAudioCapList m_tSndAudCap[MAX_AUDIO_NUM];
	u16             m_wSndAudNum;
	THpModuleAudioCapList m_tRcvAudCap[MAX_AUDIO_NUM];
	u16			    m_wRcvAudNum; 

	void Clear()
	{
		u8 index = 0;
		m_wSndAudNum = 0;
		m_wRcvAudNum = 0;
		m_wSndVidNum = 0;
		m_wRcvVidNum = 0;
		for ( index=0; index<MAX_VIDEO_NUM; index++)
		{
			m_tSndVidCap[index].Clear();
			m_tRcvVidCap[index].Clear();
		}
		for ( index=0; index< MAX_AUDIO_NUM; index++)
		{
			m_tSndAudCap[index].Clear();
			m_tRcvAudCap[index].Clear();
		}
	}
	THpModuleDualCapList()
	{
		Clear();
	}

};

struct THpModuleCallCap
{
	u16 m_wAudioNum;
	THpModuleAudioCapList m_tAudioCapList[MAX_AUDIO_NUM];     //每一个通道
	u16 m_wVideoNum;
	THpModuleVideoCapList m_tVideoCapList[MAX_VIDEO_NUM];

	THpModuleVideoCapList m_tDualCapList;					  //视频双流能力，默认为1路

	THpModuleCallCap()
	{
		Clear();
	}

	void Clear()
	{
		m_wAudioNum = 0;
		m_wVideoNum = 0;

		u8 i=0;
		for ( i=0; i<MAX_AUDIO_NUM; i++)
		{
			m_tAudioCapList[i].Clear();
		}

		for ( i=0; i<MAX_VIDEO_NUM; i++)
		{
			m_tVideoCapList[i].Clear();
		}
		m_tDualCapList.Clear();
	}

};




struct THpModuleEncryptKey
{
	u8 byLen;   //密钥长度
	u8 byKey[MAX_ENCRYPT_KEY_LEN]; //密钥长度
public:
	THpModuleEncryptKey(){ memset( this ,0 ,sizeof( THpModuleEncryptKey ) );}	
};

struct THpModuleVideoChanParam
{
	emHpModuleVideoFormat     m_emVideoFormat;
	emHpModuleVideoRes        m_emVideoResolution;
	u16                       m_wChanMaxBitrate; 
	u8						  m_byPayload;
	THpModuleEncryptKey       m_tKey;
	u8                        m_byFrameRate;
public:
	THpModuleVideoChanParam()
	{ 
		Clear();
	}	

	void Clear()
	{
		m_emVideoFormat = emHpModuleVEnd;
		m_emVideoResolution = emHpModuleVResEnd;
		m_wChanMaxBitrate   = 0;
		m_byPayload         = 0;
		memset( &m_tKey ,0 ,sizeof(m_tKey) );
		m_byFrameRate = 30;
	}
};

struct TModuleAudioChanParam
{
	emHpModuleAudioFormat   m_emAudioFormat;
	u8                      m_byPayload;
	THpModuleEncryptKey     m_tKey;
public:
	TModuleAudioChanParam()
	{
		Clear();
	}

	void Clear()
	{
		m_emAudioFormat = emHpModuleAEnd;
		m_byPayload     = 0;
		memset( &m_tKey ,0 ,sizeof(m_tKey));
	}
};

enum emHpModuleChanType
{
	emHpModuleChanTypeBegin,
	emHpModuleSndAud,
	emHpModuleSndVid,
	emHpModuleRcvAud,  
	emHpModuleRcvVid,
};

enum emHpModuleAVType
{
	emHpModuleTypeBegin,
	emHpModuleAudioType,
	emHpModuleVideoType,

	emHpModuleAudioSecondType,
	emHpModuleVideoSecondType,
};

enum emHpModuleChannelDirection
{
	emHpModuleChannelDirectionBegin,
	emHpModuleChannelSend,
	emHpModuleChannelRecv,
	emHpModuleChannelBidirectional,
};


struct THpModuleChannelType
{
	emHpModuleAVType m_emAVType;
	emHpModuleChannelDirection m_emDirection;
public:
	THpModuleChannelType()
	{
		m_emAVType = emHpModuleTypeBegin;
		m_emDirection = emHpModuleChannelDirectionBegin;
	}
};

struct THpModuleChanInfo
{
	THpModuleTransportAddr		m_tLocalRtcpAddr;
	THpModuleTransportAddr      m_tLocalRtpAddr;
	THpModuleTransportAddr		m_tPeerRtcpAddr;
	THpModuleTransportAddr      m_tPeerRtpAddr;
	u8                          m_wDynamicPayload;

	emHpModuleAudioFormat		m_emAudioFormat;      
	u8						    m_byPackTime;           
	emHpModuleAudioChnlCfg		m_emAudioChnlCfg;      
	emHpModuleAudioSampleFreq   m_emAudioSampleFreq;
	//g7231用
	u8							m_byMaxFrameNum;		    
	BOOL						m_bSilenceSuppression;	    

	emHpModuleVideoFormat		m_emVideoFormat;       
	THpModuleVideoDescript	    m_atItem;               

public:
	THpModuleChanInfo()
	{
		memset(this, 0, sizeof(THpModuleChanInfo));
		m_emAudioFormat = emHpModuleAEnd;
		m_emVideoFormat = emHpModuleVEnd;
	}
};

struct THpModuleChanConnectedInfo
{
	HMDLCHANNEL           m_hChan;
	HMDLAPPCHANNEL        m_hAppChan;
	THpModuleChannelType  m_emChanType;

	THpModuleChanInfo     m_tCapsetInfo;

	THpModuleChanConnectedInfo()
	{
		memset( this, 0, sizeof(THpModuleChanConnectedInfo) );
	}
};

struct THpModuleChanDisconnectedInfo
{
	HMDLCHANNEL         m_hChan;
	HMDLAPPCHANNEL      m_hAppChan; 

	THpModuleChanDisconnectedInfo()
	{
		memset(this, 0, sizeof(THpModuleChanDisconnectedInfo));
	}
};

struct THpModuleRegistReq
{
	THpModuleAliasAddr	   m_atH323Alias[MAX_ALIAS_NUM];
	THpModuleAliasAddr	   m_atE164[MAX_E164_NUM];

	s8					   m_achEPID[MAX_LEN_EPID + 1];

	THpModuleTransportAddr m_tLocalAddress;
	THpModuleTransportAddr m_tGKAddress;
	THpModuleTransportAddr m_tRegAddress;

	s32					   m_nTimeToLive;

	u8                     m_abyProductId[MAX_LEN_PRODUCT_ID+1];
	u8                     m_abyVersionId[MAX_LEN_VERSION_ID+1];

	BOOL32				   m_bInterReg;

public:
	THpModuleRegistReq()
	{
		Clear();
	}
	void Clear()
	{
		memset( m_atE164 , 0 , sizeof(m_atE164) );
		memset( m_atH323Alias , 0 , sizeof(m_atH323Alias) );
		memset( m_achEPID, 0, sizeof( m_achEPID) );
		memset( m_abyProductId, 0, sizeof( m_abyProductId) );
		memset( m_abyVersionId, 0, sizeof( m_abyVersionId) );
		m_tLocalAddress.Clear();
		m_tGKAddress.Clear();
		m_tRegAddress.Clear();
		m_nTimeToLive = 60;
		m_bInterReg = FALSE;
	}
};



enum emHpModuleRegResult
{
	emHpModuleRegResultBegin = 0,
	emHpModuleRegResultReg,
	emHpModuleRegResultUnReg,
};


enum emHpModuleRasReason
{
	emHpModuleRasGKUnReachable,
	emHpModuleRasInvalidAlias,
	emHpModuleRasInvalidCallSignalAddress,
	emHpModuleRasInvalidTerminalType,
	emHpModuleRasFullRegistrationRequired,
	emHpModuleRasInvalidTerminalAliases,
	emHpModuleRasDuplicateAlias,
	emHpModuleRasUnknownReason
};

#endif



