
#pragma once

#ifdef  HAS_SIP
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>
#include "pjsip.h"

#endif

#include <map>
#include "SIPAPIType.h"
#include "../imxec_h323/CallCapability.h"

#include "../SIPBFCP/SIPBFCP.h"


#ifndef  HAS_SIP

typedef  void   pj_pool_t ;
typedef  void  pjmedia_sdp_session ;
typedef  int     pj_status_t;

/** Status is OK. */
#define PJ_SUCCESS  0 
#define PJ_TRUE	    1 
#define PJ_FALSE    0 

typedef struct pj_time_val
{
    /** The seconds part of the time. */
    long    sec;
    /** The miliseconds fraction of the time. */
    long    msec;
} pj_time_val;

#endif


//H264 Capability 最大宏块处理速度 MaxMBPS   Macroblock processing speed
#define H264_MacroblockPS_QCIF  3000 //1.1
#define H264_MacroblockPS_CIF   11880
#define H264_MacroblockPS_4CIF  40500
#define H264_MacroblockPS_16CIF 245760
#define H264_MacroblockPS_720I  108000
#define H264_MacroblockPS_720P  108000
#define H264_MacroblockPS_1080I 216000
#define H264_MacroblockPS_1080P 245760
#define H264_MacroblockPS_VGA   40500
#define H264_MacroblockPS_SVGA  108000
#define H264_MacroblockPS_XGA   108000
#define H264_MacroblockPS_SXGA  216000

//0x14 2.0 43
//0x15 2.1 50
//0x16 2.2 57
//0x29 4.1 92
//0x2A 4.2 99
//0x32 5.0 106
//0x33 5.1 113
////1-15   1b-19   1.1-22   1.2-29  1.3-36   2-43  2.1-50  2.2-57   3-64   3.1-71   3.2-78  4-85   4.1-92   4.2-99    5-106  5.1-113 

//H264 Level Value use to SDP
#define H264_SDP_LEVEL_QCIF  0x0B //11      //1.1 30fps   3000 396 
#define H264_SDP_LEVEL_CIF   0x0D //13      //1.3 30fps   11880 396
#define H264_SDP_LEVEL_4CIF  0x16 //22      //2.2   25fps   40500 1620 30-->22
#define H264_SDP_LEVEL_16CIF 0x28 //40      //4   39fps   245760 8192
#define H264_SDP_LEVEL_720I  0x1F //31      //3.1 60fps ? 108000 3600
#define H264_SDP_LEVEL_720P  0x1F //31      //3.1 30fps   108000 3600
#define H264_SDP_LEVEL_1080I 0x20 //32      //3.2 60fps ? 216000 5120
#define H264_SDP_LEVEL_1080P 0x28 //40      //4   30fps   245760 8192
#define H264_SDP_LEVEL_VGA   0x1E //30      //3   34fps   40500 1620
#define H264_SDP_LEVEL_SVGA  0x1F //31      //3.1 57fps   108000 3600
#define H264_SDP_LEVEL_XGA   0x1F //31      //3.1 35fps   108000 3600
#define H264_SDP_LEVEL_SXGA  0x20 //32      //3.2 42fps   216000 5120


//H264 Max frame size (macroblocks)
#define H264_SDP_MaxFS_QCIF  396 //11      //1.1 30fps   3000 396 
#define H264_SDP_MaxFS_CIF   396 //13      //1.3 30fps   11880 396
#define H264_SDP_MaxFS_4CIF  1620 //30      //3   25fps   40500 1620
#define H264_SDP_MaxFS_16CIF 8192 //40      //4   39fps   245760 8192
#define H264_SDP_MaxFS_720I  3600 //31      //3.1 60fps ? 108000 3600
#define H264_SDP_MaxFS_720P  3600 //31      //3.1 30fps   108000 3600
#define H264_SDP_MaxFS_1080I 5120 //32      //3.2 60fps ? 216000 5120
#define H264_SDP_MaxFS_1080P 8192 //40      //4   30fps   245760 8192
#define H264_SDP_MaxFS_VGA   1620 //30      //3   34fps   40500 1620
#define H264_SDP_MaxFS_SVGA  3600 //31      //3.1 57fps   108000 3600
#define H264_SDP_MaxFS_XGA   3600 //31      //3.1 35fps   108000 3600
#define H264_SDP_MaxFS_SXGA  5120 //32      //3.2 42fps   216000 5120





//注CAP_H264_QCIF=396 该值重复可去之

/*类SIPCallCapability操作设置的能力集  只用于SIP SetPayloadType*/
enum EMediaCoderType{
	e_Video_H264=0,
	e_Video_H263P,		//
	e_Video_H263PP,		//
	e_Video_H264SVC,

	e_DualVideo_H264SVC,
	e_DualVideo_H264,
	e_DualVideo_H263P,
	e_DualVideo_H263PP,

	e_H224,		        //H.224
	e_Audio_G722_48k,	//G.722 48k(暂不支持)
	e_Audio_G722_56k,	//G.722 56k(暂不支持)
	e_Audio_G722_64k,	//G.722 64k(暂不支持)
	e_Audio_G7221_24k,		//G.722.1 24k
	e_Audio_G7221_32k,		//G.722.1 32k(暂不支持)
	e_Audio_G7231,			//G.723.1
	e_Audio_G729A,			//G.729AnnexA
	e_Audio_G7221C_24k,	    //G.722.1 AnnexC 24k(暂不支持)
	e_Audio_G7221C_32k,		//G.722.1 AnnexC 32k(暂不支持)
	e_Audio_G7221C_48k,		//G.722.1 AnnexC 48k(暂不支持)
};

enum EMediaType{
	e_Media_Video=0,
	e_Media_Audio,		
	e_Media_H224,
	e_Media_DualVideo,
};
enum EH263ResolutionOption{
	e_H263_Resolution_None=0,
	e_H263_Resolution_All,
	e_H263_1280_1024,
	e_H263_1280_720,
	e_H263_1024_768,
	e_H263_1024_576,
	e_H263_800_600,
	//e_H263_720_480,
	//e_H263_640_480,
};


typedef struct tagSIPDATACAPBILITY
{
	DATA_CODEC_ID	    DataCodecID;
	int					nMaxBitrate;	
    int                 nClockRate;         //时钟率
	int                 nPayLoad;           //payLoad type
	int                 nDirection;
	tagSIPDATACAPBILITY()
	{
		DataCodecID=DATA_CODEC_H224;
		nMaxBitrate=480;
		nPayLoad=0;
		nClockRate=4800;
		nDirection =0;
	}
} SIPDATACAPBILITY;


struct SIPDataCap
{
	unsigned int	        m_nCapNum;	//有效数据（远遥）能力集个数
	SIPDATACAPBILITY		m_Caps[MAX_DATACAP_NUM];

	SIPDataCap()
	{
		m_nCapNum = 0;
	}
};



class SIPCallCapability : public CallCapability
{
public:
	SIPCallCapability(void);
public:
	~SIPCallCapability(void);

public:
	//返回值为0失败或不存在
	virtual int SetSimultaneousCap();
	virtual int OperationVideoCap(eCapNum capIndex,int operation,BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);  //设置能力项 1添加 2首选 3移除,默认值选项只用于H261,H263
	virtual int OperationH239Cap(eCapNum capIndex,int operation,BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);   //设置能力项 1添加 2首选 3移除  	
	virtual int IsExistSimul(int type ,int iEntryNumber); //能力是否存在
	virtual int IsFirstSimul(int type ,int iEntryNumber); //能力是否是首选	
	virtual int InitCapabilitis(int node);                //设置所有能力集参数 使用时首先调用此方法

private:
	//从媒体能力集数组中查找
	//opetionType first is 0 exist is 1
	int FindEntryFromCapsArray(int type ,int iEntryNumber,int opetionType);
	//对H263P及H263PP设置更高分辨率能力集参数
	void SetH263CustomStr(EH263ResolutionOption option,char * buffer,int &buf_len,int maxLen);

public:
	//H263P H263PP 高分辨率设置选项
	void SetH263POption(EH263ResolutionOption option);
	void SetH263PPOption(EH263ResolutionOption option);
	void SetDualH263POption(EH263ResolutionOption option);
	void SetDualH263PPOption(EH263ResolutionOption option);

	void SetH263HighResolution(EMediaCoderType coderType,int width,int height);

	//if copyOutBuf !=NULL ,not call pjmedia_sdp_parse,slap copy to copyOutBuf
	int SDP_Capabilitis_Add(pj_pool_t *pool,pjmedia_sdp_session **p_sdp,char * copyOutBuf=NULL );
	void SetPayloadType(EMediaCoderType type,int pt=97);
	void SetSDPMediaIP(char * ip=NULL);
	void SetSDPMediaPort(EMediaType type,int port);
	void SetRtpProxyAttribute(bool proxy=false);
	void SetRtcpAttribute(bool rtcp=true);
	void SetSDPMediaBandwidth(int audioBand,int videoBand,int dualVideoBand=384);
	void SetSDPOwnerUsername(char * name=NULL);

	void SetBFCPDualVideoLabelValue(int label);
	void SetBFCPUserId(int userId);
	void SetBFCPSetupType(EBFCPSetupType type);
	void SetBFCPFloorControlType(EBFCPFloorControlType type);
	void SetBFCPConfId(int confId);
	void SetBFCPFloorId(int floorId);
	void SetBFCPTransportType(EBFCPTransportType type);
	void SetBFCPTransportPort(int port);
	VIDEO_FORMAT_TYPE GetVideoFormatTypeFromCapIndex(eCapNum capIndex);
	void GetVideoFormatTypeFromCapIndex(int type,eCapNum capIndex,VIDEO_CAPABILITY_ITEM  & cap); //mainvideo is type 2 ,dualvideo is type 3
	void SetRtpFECEnableAttribute(bool bUseFECEnable=true);
	//mcu use
	virtual void GetH264SVCVideoParam(int type,VIDEC_CODEC_TYPE codecType,int & frame,int & nLevel,int & nBitrate);

	int  BuildMediaCapabilitisTest(pj_pool_t *pool,DapCallCapability & DapCap,pjmedia_sdp_session **p_sdp);
	void SetH263CustomStr2(VIDEO_FORMAT_TYPE nH263Format,char * buffer,int &buf_len,int maxLen);
	void GetVideoSIPH264MaxFSMBPS(int & nMaxMBPS,int & nMaxFS,int & nLevel,int nFrame,VIDEO_FORMAT_TYPE format);
	int  GetH264SIPLevel(int nH323Level);
	int  GetH264H323Level(int nSIPLevel);
	VIDEO_FORMAT_TYPE GetSIPFormatTypeFromH264Param(int nProfile,int nLevel,int nMaxFS,int nBitrate,float fFrame,int nType=0); //nType 0 get min ,1 get max

public:
	int m_H264PT, m_H264SVCPT, m_H263PPT, m_H263PPPT, m_H224PT;
	int m_VideoPort, m_AudioPort, m_DualVideoPort,m_H224Port;
	char m_strLocalIP[128];
	int m_G7221_24k_PT;
	int m_G7221_32k_PT;
	int m_G7221C_24k_PT;
	int m_G7221C_32k_PT;
	int m_G7221C_48k_PT;
	bool m_bProxy;
	bool m_bUseFECEnable;

	EBFCPTransportType m_nBFCPTransportType;
	int m_nBFCPConfId;
	int m_nBFCPUserId;
	int m_nBFCPFloorId;
	int m_nBFCPDualVideoLabel;
	char m_strBFCPFloorControl[16];
	char m_strBFCPSetupType[16];
	int m_nBFCPTransportPort;

	int m_DualH264PT;
	int m_DualH264SVCPT;
	int	m_DualH263PPT;
	int	m_DualH263PPPT;

	bool m_bRtcp; 
	char m_strOwnerUsername[128];
	int m_nVideoBandwidth; //kbps
	int m_nAudioBandwidth;
	int m_nDualVideoBandwidth;

	EH263ResolutionOption m_H263PResolutionOption;
	EH263ResolutionOption m_H263PPResolutionOption;
	EH263ResolutionOption m_DualH263PResolutionOption;
	EH263ResolutionOption m_DualH263PPResolutionOption;


	//only use to media live ,neg sdp payloadtype set
	void SetLiveSDPMediaPayloadType(EMediaType type,int pt);
	//only use to media live ,neg sdp payloadtype set
	int m_VideoPayloadType, m_AudioPayloadType, m_DualVideoPayloadType,m_H224PayloadType;

};
