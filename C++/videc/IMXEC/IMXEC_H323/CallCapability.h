#pragma once

#ifdef HAS_RTPRTMP
#include "../stdafx.h"
#endif

#ifdef HAS_H323
#include <RVINC/cm.h>
#endif

#include <map>
#include "H323APIType.h"
#include "CapalityNewType.h"

//#ifdef HAS_H323
#ifndef HAS_RTPRTMP

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
#include <AVCONMP/IMXEC_Call.h>
#else
#include <IMXEC/IMXEC_Call.h>
#endif
#endif

struct tagCollapsing
{
	int standard;
	int value;
};

//add support frame bitrate
typedef struct tagMediaParam
{
	VIDEC_CODEC_TYPE  nCodecType;
	VIDEO_FORMAT_TYPE nFormat;
	int bitrate;
	int frame;
	unsigned long nIndex;
	tagMediaParam()
	{
		nCodecType=VIDEC_CODEC_H264;
		nFormat =VIDEO_FORMAT_QCIF;
		bitrate=384;
		frame=10;
		nIndex=0;
	}
}MediaParam;




#define VideoFrameBase  30.0 

////1 本端优先 2对端优先 
//#ifndef LocalCapPriority 
//#define LocalCapPriority   1  
//#endif

typedef struct tagRecvAudioCapability
{
	tagRecvAudioCapability()
	{
		m_AudioCodecType =X_AUDIO_CODEC_COUNT;
		m_nAudioBitrate =0;
		m_nEncryption =0;
		m_nAlgorithm =0;
		m_nDirection =0;
		m_nPayload =0;
		m_nClockRate =8000;
	}

	X_AUDIO_CODEC_ID	m_AudioCodecType;
	int					m_nAudioBitrate;
	int                 m_nEncryption;
	unsigned int        m_nAlgorithm;
	int                 m_nDirection;
	int                 m_nPayload;
	int                 m_nClockRate;         //时钟率
}RecvAudioCap;

typedef struct tagRecvVideoCapability
{
	tagRecvVideoCapability()
	{
		m_VideoCodecType =VIDEC_CODEC_COUNT;
		m_VideoFormatType =VIDEO_FORMAT_COUNT;
		m_nVideoBitrate =0;
		m_nVideoFrameRate =0;
		m_nEncryption =0;
		m_nAlgorithm =0;
		m_nDirection =0;
		m_nFormatValue =0;
		m_nLevelValue =0;
		m_nProfileValue =0;
		m_nMaxFS =0;
		m_nMaxMBPS =0;
		m_nPayload =0;
	}
	VIDEC_CODEC_TYPE	m_VideoCodecType;
	VIDEO_FORMAT_TYPE	m_VideoFormatType;
	int					m_nVideoBitrate;
	int                 m_nVideoFrameRate;
	int                 m_nEncryption;
	unsigned int        m_nAlgorithm;
	int                 m_nDirection;    //传输方向 cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
	int                 m_nFormatValue;  //分辨率对应实际大小
	int                 m_nLevelValue;   //H264 Level
	int                 m_nProfileValue; //H264 Profile
	int                 m_nMaxFS;        //H264 MaxFS
	int                 m_nMaxMBPS;      //H264 MaxMBPS
	int                 m_nPayload;
}RecvVideoCap;

typedef struct tagRecvDataCapability
{
	tagRecvDataCapability()
	{
		m_DataCodecID=DATA_TYPE_H224;
		m_nDataBitrate=0;
		m_nEncryption =0;
		m_nAlgorithm =0;
		m_nDirection =0;
		m_nPayload =0;
	}
	DATA_TYPE_ID	    m_DataCodecID;
	int					m_nDataBitrate;
	int                 m_nEncryption;
	unsigned int        m_nAlgorithm;
	int                 m_nDirection;
	int                 m_nPayload;
}RecvDataCap;

#define MAX_AUDIO_CAP2TS	32
#define MAX_VIDEO_CAP2TS	64
#define MAX_DATA_CAP2TS	 2

typedef struct tagRecvCallCapability
{
	tagRecvCallCapability()
	{
		m_nAudioCapCount=0;
		m_nMainVideoCapCount=0;
		m_nSecondVideoCapCount=0;
		m_nDataCapCount=0;
	}
	int					m_nAudioCapCount;
	RecvAudioCap		m_aAudioCap[MAX_AUDIO_CAP2TS];
	int					m_nMainVideoCapCount;
	RecvVideoCap		m_aMainVideoCap[MAX_VIDEO_CAP2TS];
	int					m_nSecondVideoCapCount;
	RecvVideoCap		m_aSecondVideoCap[MAX_VIDEO_CAP2TS];
	int					m_nDataCapCount;
	RecvDataCap  		m_DataCap[MAX_DATA_CAP];
}RecvCallCaps;

typedef struct tagNegMediaCapability
{
	tagNegMediaCapability()
	{
		m_CodecType = 0;
		m_nBitrate =0;
		m_VideoFormatType =VIDEO_FORMAT_COUNT;
		m_nVideoFrameRate =0;
		m_nAlgorithm =0;
		m_nDirection =0;
		m_nFormatValue =0;
		m_nLevelValue =0;
		m_nProfileValue =0;
		m_nMaxFS =0;
		m_nMaxMBPS =0;
		m_nPayload =0;
		m_nClockRate =8000;
		m_bNegEncryption =false;
		m_bNegMedia =false;
	}
	int                 m_CodecType;
	int					m_nBitrate;
	VIDEO_FORMAT_TYPE   m_VideoFormatType;
	int                 m_nVideoFrameRate;
	unsigned int        m_nAlgorithm;
	int                 m_nDirection;    //传输方向 cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
	int                 m_nFormatValue;  //分辨率对应实际大小
	int                 m_nLevelValue;   //H264 Level
	int                 m_nProfileValue; //H264 Profile
	int                 m_nMaxFS;        //H264 MaxFS
	int                 m_nMaxMBPS;      //H264 MaxMBPS
	int                 m_nPayload;      //SIP Payload
	int                 m_nClockRate;    //时钟率
	bool                m_bNegEncryption;  //协商结果 协商成功为true 协商失败为false
	bool                m_bNegMedia;     //协商结果 协商成功为true 协商失败为false
}NegMediaCap;



/*类CallCapability操作设置的是同时能力集*/
class CallCapability
{
public:
	CallCapability(void);
public:
	~CallCapability(void);
protected:
	virtual int SetSimultaneousCap();                     //设置同时能力项参数
	//返回值为0失败或不存在
	virtual int IsExistSimul(int type ,int iEntryNumber); //能力是否存在
	virtual int IsFirstSimul(int type ,int iEntryNumber); //能力是否是首选	    
	int AddVideoCap(VIDEO_CAPABILITY_ITEM& cap) ;             //添加视频能力
	int AddAudioCap(AUDIO_CAPABILITY_ITEM& cap) ;             //添加音频能力
	int AddH239Cap(VIDEO_CAPABILITY_ITEM& cap) ;              //添加H239频能力
	int AddDataCap(DATA_CAPABILITY_ITEM& cap) ;              //添加数据能力	
	int SetVideoCap(int type,VIDEO_CAPABILITY_ITEM& cap);     //设为首位,移除能力
	int SetAudioCap(int type,AUDIO_CAPABILITY_ITEM& cap);     //设为首位,移除能力
	int SetH239Cap(int type,VIDEO_CAPABILITY_ITEM& cap);      //设为首位,移除能力
	int SetDataCap(int type,DATA_CAPABILITY_ITEM& cap);      //设为首位,移除能力
	virtual int OperationVideoCap(eCapNum capIndex,int operation,BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);  //设置能力项 1添加 2首选 3移除,默认值选项只用于H261,H263
	virtual int OperationAudioCap(eCapNum capIndex,int operation);  //设置能力项 1添加 2首选 3移除   
	virtual int OperationH239Cap(eCapNum capIndex,int operation,BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);   //设置能力项 1添加 2首选 3移除  
	virtual int OperationDataCap(eCapNum capIndex,int operation);   //设置能力项 1添加 2首选 3移除   

public:
	int m_NodeId;                                 // 能力集结点ID
	AudioCap		m_AudioCap;				      // 音频能力集
	VideoCap		m_VideoCap;				      // 视频能力集
	H239VideoCap	m_H239VideoCap;			      // H239双流能力集
	DataCap			m_DataCap;	                  // 数据能力集

	virtual int InitCapabilitis(int node);        //设置所有能力集参数 使用时首先调用此方法
	int BuildCapabilitis(int node);               //提交生成能力集表
	virtual int RemoveAllCapabilitis();           //移除所有能力集 再按需要添加

	//视频:
	int AddH264QCIF();                         //添加H264能力 QCIF  
	int RemoveH264QCIF();                      //移除H264能力 QCIF
	int SetH264FirstQCIF();                    //设置同时能力项为首选
	int IsExistH264QCIF();                     //H264 QCIF能力是否存在
	int IsFirstH264QCIF();                     //H264 QCIF是否为同时能力项首选

	int AddH264CIF();                         //添加H264能力 CIF  
	int RemoveH264CIF();                      //移除H264能力 CIF
	int SetH264FirstCIF();                    //设置同时能力项为首选
	int IsExistH264CIF();                     //H264 CIF能力是否存在
	int IsFirstH264CIF();                     //H264 CIF是否为同时能力项首选
	int AddH2644CIF();
	int RemoveH2644CIF();
	int SetH264First4CIF();
	int IsExistH2644CIF();                     //H264 4CIF能力是否存在
	int IsFirstH2644CIF();                     //H264 4CIF是否为同时能力项首选

	int AddH26416CIF();
	int RemoveH26416CIF();
	int SetH264First16CIF();
	int IsExistH26416CIF();                     //H264 16CIF能力是否存在
	int IsFirstH26416CIF();                     //H264 16CIF是否为同时能力项首选

	int AddH264720P();                         //添加H264能力 720P
	int RemoveH264720P();                      //移除H264能力 720P
	int SetH264First720P();                    //设置同时能力项为首选
	int IsExistH264720P();                     //H264 720P能力是否存在
	int IsFirstH264720P();                     //H264 720P是否为同时能力项首选

	int AddH264720I();                         //添加H264能力 720I
	int RemoveH264720I();                      //移除H264能力 720I
	int SetH264First720I();                    //设置同时能力项为首选
	int IsExistH264720I();                     //H264 720I能力是否存在
	int IsFirstH264720I();                     //H264 720I是否为同时能力项首选

	int AddH2641080P();                         //添加H264能力 1080P 
	int RemoveH2641080P();                      //移除H264能力 1080P
	int SetH264First1080P();                    //设置同时能力项为首选
	int IsExistH2641080P();                     //H264 1080P能力是否存在
	int IsFirstH2641080P();                     //H264 1080P是否为同时能力项首选

	int AddH2641080I();                         //添加H264能力 1080I 
	int RemoveH2641080I();                      //移除H264能力 1080I
	int SetH264First1080I();                    //设置同时能力项为首选
	int IsExistH2641080I();                     //H264 1080I能力是否存在
	int IsFirstH2641080I();                     //H264 1080I是否为同时能力项首选

	int AddH264VGA();                           //添加H264能力 VGA 
	int RemoveH264VGA();                        //移除H264能力 VGA
	int SetH264FirstVGA();                      //设置同时能力项为首选
	int IsExistH264VGA();                       //H264 VGA能力是否存在
	int IsFirstH264VGA();                       //H264 VGA是否为同时能力项首选

	int AddH264SVGA();                           //添加H264能力 SVGA 
	int RemoveH264SVGA();                        //移除H264能力 SVGA
	int SetH264FirstSVGA();                      //设置同时能力项为首选
	int IsExistH264SVGA();                       //H264 SVGA能力是否存在
	int IsFirstH264SVGA();                       //H264 SVGA是否为同时能力项首选

	int AddH264XGA();                           //添加H264能力 XGA 
	int RemoveH264XGA();                        //移除H264能力 XGA
	int SetH264FirstXGA();                      //设置同时能力项为首选
	int IsExistH264XGA();                       //H264 XGA能力是否存在
	int IsFirstH264XGA();                       //H264 XGA是否为同时能力项首选

	int AddH264SXGA();                           //添加H264能力 SXGA 
	int RemoveH264SXGA();                        //移除H264能力 SXGA
	int SetH264FirstSXGA();                      //设置同时能力项为首选
	int IsExistH264SXGA();                       //H264 SXGA能力是否存在
	int IsFirstH264SXGA();                       //H264 SXGA是否为同时能力项首选


	int m_nHighProfileLevel;

	int AddH264720PHighProfile();								//添加H264HighProfile能力 
	int RemoveH264720PHighProfile();							//移除H264HighProfile能力 
	int SetFirstH264720PHighProfile();
	int IsExistMainVideoH264720PHighProfile();                //H264HighProfile能力是否存在
	int IsFirstMainVideoH264720PHighProfile();                //H264HighProfile是否为同时能力项首选


	int AddH2641080PHighProfile();								//添加H264HighProfile能力 
	int RemoveH2641080PHighProfile();							//移除H264HighProfile能力 
	int SetFirstH2641080PHighProfile();
	int IsExistMainVideoH2641080PHighProfile();                //H264HighProfile能力是否存在
	int IsFirstMainVideoH2641080PHighProfile();                //H264HighProfile是否为同时能力项首选



	int AddH239VideoH264720PHighProfile();                    //添加H264HighProfile能力 
	int RemoveH239VideoH264720PHighProfile();                 //移除H264HighProfile能力 
	int SetFirstH239VideoH264720PHighProfile();
	int IsExistH239VideoH264720PHighProfile();                //H264HighProfile能力是否存在
	int IsFirstH239VideoH264720PHighProfile();                //H264HighProfile是否为同时能力项首选

	int AddH239VideoH2641080PHighProfile();                    //添加H264HighProfile能力 
	int RemoveH239VideoH2641080PHighProfile();                 //移除H264HighProfile能力 
	int SetFirstH239VideoH2641080PHighProfile();
	int IsExistH239VideoH2641080PHighProfile();                //H264HighProfile能力是否存在
	int IsFirstH239VideoH2641080PHighProfile();                //H264HighProfile是否为同时能力项首选

	int IsExistMainVideoH264HighProfile();                //H264HighProfile能力是否存在
	int IsFirstMainVideoH264HighProfile();                //H264HighProfile是否为同时能力项首选
	int IsExistMainVideoH264BaseLineProfile();                       //H264 BaseLineProfile能力是否存在
	int IsFirstMainVideoH264BaseLineProfile();                       //H264 BaseLineProfile是否为同时能力项首选

	int IsExistH239VideoH264HighProfile();                //H264HighProfile能力是否存在
	int IsFirstH239VideoH264HighProfile();                //H264HighProfile是否为同时能力项首选
	int IsExistH239VideoH264BaseLineProfile();				//H264 BaseLineProfile能力是否存在
	int IsFirstH239VideoH264BaseLineProfile();				//H264 BaseLineProfile是否为同时能力项首选


	int IsExistH264();                           //任一H264能力是否存在
	int IsFirstH264(void);



	int AddH261(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE);                         //添加H261能力 
	int RemoveH261();                      //移除H261能力 
	int SetH261First();                    //设置同时能力项为首选
	int IsExistH261();                     //H261能力是否存在
	int IsFirstH261();     

	int AddH263(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);                         //添加H263能力 
	int RemoveH263();                      //移除H263能力 
	int SetH263First();                    //设置同时能力项为首选
	int IsExistH263();                     //H263能力是否存在
	int IsFirstH263();     

	int AddH263v2(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);                         //添加H263 v2能力 
	int RemoveH263v2();                      //移除H263 v2能力 
	int SetH263v2First();                    //设置同时能力项为首选
	int IsExistH263v2();                     //H263 v2能力是否存在
	int IsFirstH263v2();     

	int AddH263v3(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);                         //添加H263 v3能力 
	int RemoveH263v3();                      //移除H263 v3能力 
	int SetH263v3First();                    //设置同时能力项为首选
	int IsExistH263v3();                     //H263 v3能力是否存在
	int IsFirstH263v3();     

	//音频:
	int AddG7221_32K();                          //添加G7221_32K能力
	int RemoveG7221_32K();
	int SetFirstG7221_32K();
	int IsExistG7221_32K();                      //G7221_32K能力是否存在
	int IsFirstG7221_32K();                      //G7221_32K是否为同时能力项首选

	int AddG7221_24K();
	int RemoveG7221_24K();
	int SetFirstG7221_24K();
	int IsExistG7221_24K();                      //G7221_24K能力是否存在
	int IsFirstG7221_24K();                      //G7221_24K是否为同时能力项首选

	// G7221 siren annex c

	int AddG7221C_24K();
	int RemoveG7221C_24K();
	int SetFirstG7221C_24K();
	int IsExistG7221C_24K();                      //G7221_24K能力是否存在
	int IsFirstG7221C_24K();                      //G7221_24K是否为同时能力项首选
	// G7221 siren annex c
	int AddG7221C_32K();
	int RemoveG7221C_32K();
	int SetFirstG7221C_32K();
	int IsExistG7221C_32K();                      //G7221_24K能力是否存在
	int IsFirstG7221C_32K();                      //G7221_24K是否为同时能力项首选
	// G7221 siren annex c
	int AddG7221C_48K();
	int RemoveG7221C_48K();
	int SetFirstG7221C_48K();
	int IsExistG7221C_48K();                      //G7221_24K能力是否存在
	int IsFirstG7221C_48K();                      //G7221_24K是否为同时能力项首选



	int AddG722_64K();                          //添加G7221_32K能力
	int RemoveG722_64K();   
	int SetFirstG722_64K();   
	int IsExistG722_64K();                        //G722_64K能力是否存在
	int IsFirstG722_64K();                        //G722_64K是否为同时能力项首选

	int AddG7231();
	int RemoveG7231();
	int SetFirstG7231();
	int IsExistG7231();                          //G7231能力是否存在
	int IsFirstG7231();                          //G7231是否为同时能力项首选

	int AddG729A();
	int RemoveG729A();
	int SetFirstG729A();

	int IsExistG729A(int type=2);                   /*1=G729 2=G729A 3=G729B*/                         //G729A能力是否存在
	int IsFirstG729A(int type=2);                   /*1=G729 2=G729A 3=G729B*/                         //G729A是否为同时能力项首选


	int AddG711u_64K();
	int RemoveG711u_64K();
	int SetFirstG711u_64K();
	int IsExistG711u_64K();                      //G711u_64K能力是否存在
	int IsFirstG711u_64K();                      //G711u_64K是否为同时能力项首选

	int AddG711a_64K();
	int RemoveG711a_64K();
	int SetFirstG711a_64K();
	int IsExistG711a_64K();                       //G711a_64K能力是否存在
	int IsFirstG711a_64K();                       //G711a_64K是否为同时能力项首选

	//notice add 20120202
	//GSM 注意 暂用于SIP
	int AddGSM();
	int RemoveGSM();
	int SetFirstGSM();
	int IsExistGSM();                      
	int IsFirstGSM();

	//notice 20111227 update
	// G719
	int AddG719_64K();
	int RemoveG719_64K();
	int SetFirstG719_64K();
	int IsExistG719_64K();                      //G719能力是否存在
	int IsFirstG719_64K();                      //G719是否为同时能力项首选
	// G719
	int AddG719_32K();
	int RemoveG719_32K();
	int SetFirstG719_32K();
	int IsExistG719_32K();                      //G719能力是否存在
	int IsFirstG719_32K();                      //G719是否为同时能力项首选
	// G719
	int AddG719_48K();
	int RemoveG719_48K();
	int SetFirstG719_48K();
	int IsExistG719_48K();                      //G719能力是否存在
	int IsFirstG719_48K();                      //G719是否为同时能力项首选





	//H239
	int IsExistH239VideoH264Extend();                           //任一H264能力是否存在
	int IsFirstH239VideoH264Extend();                           //任一H264能力是否存在

	int AddH239VideoH264QCIFExtend();
	int AddH239VideoH264CIFExtend();
	int AddH239VideoH2644CIFExtend();
	int AddH239VideoH26416CIFExtend();
	int AddH239VideoH264720PExtend();
	int AddH239VideoH264720IExtend();
	int AddH239VideoH2641080PExtend();
	int AddH239VideoH2641080IExtend();
	int AddH239VideoH264VGAExtend();
	int AddH239VideoH264SVGAExtend();
	int AddH239VideoH264XGAExtend();
	int AddH239VideoH264SXGAExtend();


	int RemoveH239VideoH264QCIFExtend();
	int RemoveH239VideoH264CIFExtend();
	int RemoveH239VideoH2644CIFExtend();
	int RemoveH239VideoH26416CIFExtend();
	int RemoveH239VideoH264720PExtend();
	int RemoveH239VideoH264720IExtend();
	int RemoveH239VideoH2641080PExtend();
	int RemoveH239VideoH2641080IExtend();
	int RemoveH239VideoH264VGAExtend();
	int RemoveH239VideoH264SVGAExtend();
	int RemoveH239VideoH264XGAExtend();
	int RemoveH239VideoH264SXGAExtend();

	int SetFirstH239VideoH264QCIFExtend();
	int SetFirstH239VideoH264CIFExtend();
	int SetFirstH239VideoH2644CIFExtend();
	int SetFirstH239VideoH26416CIFExtend();
	int SetFirstH239VideoH264720PExtend();
	int SetFirstH239VideoH264720IExtend();
	int SetFirstH239VideoH2641080PExtend();
	int SetFirstH239VideoH2641080IExtend();
	int SetFirstH239VideoH264VGAExtend();
	int SetFirstH239VideoH264SVGAExtend();
	int SetFirstH239VideoH264XGAExtend();
	int SetFirstH239VideoH264SXGAExtend();

	int IsExistH239VideoH264QCIFExtend();            //H264Extend能力是否存在
	int IsExistH239VideoH264CIFExtend();            //H264Extend能力是否存在
	int IsExistH239VideoH2644CIFExtend();            //H264Extend能力是否存在
	int IsExistH239VideoH26416CIFExtend();            //H264Extend能力是否存在
	int IsExistH239VideoH264720PExtend();            //H264Extend能力是否存在
	int IsExistH239VideoH264720IExtend();            //H264Extend能力是否存在
	int IsExistH239VideoH2641080PExtend();            //H264Extend能力是否存在
	int IsExistH239VideoH2641080IExtend();            //H264Extend能力是否存在
	int IsExistH239VideoH264VGAExtend();            //H264Extend能力是否存在
	int IsExistH239VideoH264SVGAExtend();            //H264Extend能力是否存在
	int IsExistH239VideoH264XGAExtend();            //H264Extend能力是否存在
	int IsExistH239VideoH264SXGAExtend();            //H264Extend能力是否存在

	int IsFirstH239VideoH264QCIFExtend();            //H264Extend是否为同时能力项首选
	int IsFirstH239VideoH264CIFExtend();            //H264Extend是否为同时能力项首选
	int IsFirstH239VideoH2644CIFExtend();            //H264Extend是否为同时能力项首选
	int IsFirstH239VideoH26416CIFExtend();            //H264Extend是否为同时能力项首选
	int IsFirstH239VideoH264720PExtend();            //H264Extend是否为同时能力项首选
	int IsFirstH239VideoH264720IExtend();            //H264Extend是否为同时能力项首选
	int IsFirstH239VideoH2641080PExtend();            //H264Extend是否为同时能力项首选
	int IsFirstH239VideoH2641080IExtend();            //H264Extend是否为同时能力项首选
	int IsFirstH239VideoH264VGAExtend();            //H264Extend是否为同时能力项首选
	int IsFirstH239VideoH264SVGAExtend();            //H264Extend是否为同时能力项首选
	int IsFirstH239VideoH264XGAExtend();            //H264Extend是否为同时能力项首选
	int IsFirstH239VideoH264SXGAExtend();            //H264Extend是否为同时能力项首选


	int AddH239VideoH261Extend(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE);
	int RemoveH239VideoH261Extend();
	int SetFirstH239VideoH261Extend();
	int IsExistH239VideoH261Extend();            //H263Extend能力是否存在
	int IsFirstH239VideoH261Extend();            //H263Extend是否为同时能力项首选

	int AddH239VideoH263Extend(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);
	int RemoveH239VideoH263Extend();
	int SetFirstH239VideoH263Extend();
	int IsExistH239VideoH263Extend();            //H263Extend能力是否存在
	int IsFirstH239VideoH263Extend();            //H263Extend是否为同时能力项首选

	int AddH239VideoH263v2Extend(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);
	int RemoveH239VideoH263v2Extend();
	int SetFirstH239VideoH263v2Extend();
	int IsExistH239VideoH263v2Extend();            //H263Extend能力是否存在
	int IsFirstH239VideoH263v2Extend();            //H263Extend是否为同时能力项首选

	int AddH239VideoH263v3Extend(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);
	int RemoveH239VideoH263v3Extend();
	int SetFirstH239VideoH263v3Extend();
	int IsExistH239VideoH263v3Extend();            //H263Extend能力是否存在
	int IsFirstH239VideoH263v3Extend();            //H263Extend是否为同时能力项首选


	//Data FECC
	int AddFECCData();
	int RemoveFECCData();
	int SetFirstFECCData();
	int IsExistFECCData();                      //H224能力是否存在
	int IsFirstFECCData();                      //H224是否为同时能力项首选


public:
	int						m_nMainVideoMaxBitrate;
	int						m_nSecondVideoMaxBitrate;
	int						m_nUseCapOLC;									//0仅用于能力集 1仅用于逻辑通道能力集 默认为0
	int						m_nG729Type;									//用于OLC  1=G729 2=G729A 3=G729B

	int RemoveAudioAllCapabilitis();										//移除音频所有能力集
	int RemoveVideoAllCapabilitis();										//移除视频所有能力集
	int RemoveDualVideoAllCapabilitis();									//移除辅流所有能力集
	int RemoveFECCDataAllCapabilitis();										//移除FECC所有能力集

	void SetMainVideoMaxBitrate(int nMaxBitrate);
	void SetSecondVideoMaxBitrate(int nMaxBitrate);
	int GetMainVideoMaxBitrate(int nVideoRate);
	int GetSecondVideoMaxBitrate(int nVideoRate);
	void SetCapabilitisUseType(int nType=0);								//用于OLC 0仅用于能力集 1仅用于逻辑通道能力集 默认为0
	void SetG729Type(int nType);											//用于OLC  1=G729 2=G729A 3=G729B
	int AddH264MainVideoCap(VIDEC_CODEC_TYPE codecType,int nLevel);			//用于OLC 添加H264能力 level  
	int AddH264SecondVideoCap(VIDEC_CODEC_TYPE codecType,int nLevel);		//用于OLC 添加H264能力 level
	void SetH264OptionParamMaxMBPS(int nValue);								//用于OLC能力集可选参数值
	void SetH264OptionParamMaxFS(int nValue);								//用于OLC能力集可选参数值
	void SetH264OptionParamMaxDPB(int nValue);								//用于OLC能力集可选参数值
	void SetH264OptionParamMaxBR(int nValue);								//用于OLC能力集可选参数值
	void SetMainVideoFrameRate(int nFrame);
	void SetSecondVideoFrameRate(int nFrame);

protected:
	int						m_nOLCMaxMBPS;									//用于OLC能力集可选参数值 最大宏块处理速率
	int						m_nOLCMaxFS;									//用于OLC能力集可选参数值 最大帧长
	int						m_nOLCMaxDPB;									//用于OLC能力集可选参数值 最大解码图像缓存大小
	int						m_nOLCMaxBR;									//用于OLC能力集可选参数值 最大视频码率
	int						m_nMainVideoFrameRate;							//default >0
	int						m_nSecondVideoFrameRate;						//default >0

private:
	//设置能力集描述语中同时能力等
	void DescriptorBuild(HPVT valH,int nodeId,char * tempbuf,int iEntryNumber,int &nSimulNum,int &nSimultaneous,int &iAlternative);
	//设置能力集表EntryNumber及构建树形路径子串
	void VideoEntryBuild(HPVT valH,int nodeId,char * subBuf,char * tempbuf,int iEntryNumber,int iCapIndex,int nDir=1);
	void DualVideoEntryBuild(HPVT valH,int nodeId,char * subBuf,char * tempbuf,int iEntryNumber,int iCapIndex,int nDir=1);
	void AudioEntryBuild(HPVT valH,int nodeId,char * subBuf,char * tempbuf,int iEntryNumber,int iCapIndex,int nDir=1);
	void DataEntryBuild(HPVT valH,int nodeId,char * subBuf,char * tempbuf,int iEntryNumber,int iCapIndex,int nDir=1);
	int SetH264CapOptionParam(int nValue,tagCollapsing Collapsing[],int & nCount,int type,VIDEC_CODEC_TYPE codecType,int format);//1 audio 2 video 3 H239 4 Data

public:
	//use to mcu
	virtual void CopyMainVideoRateArray(MediaParam videoParam[]);
	virtual void CopySecondVideoRateArray(MediaParam dualParam[]);
	VIDEO_FORMAT_TYPE GetVideoFormatTypeFromEntry(int type,int format);
	void GetVideoSetOtherParam(int type,VIDEC_CODEC_TYPE codecType,VIDEO_FORMAT_TYPE nFormatType,int & frame,int & bitrate);
	int  GetH263CapOptionParam(int type,VIDEC_CODEC_TYPE codecType,VIDEO_FORMAT_TYPE nFormatType); //1 audio 2 video 3 H239 4 Data
	void SetMainVideoH263OLCOptionParam(VIDEC_CODEC_TYPE codecType,int nFormat);
	void SetDualVideoH263OLCOptionParam(VIDEC_CODEC_TYPE codecType,int nFormat);
	void SetH263OptionParam(int type,VIDEC_CODEC_TYPE codecType,char * szPath,HPVT hPvt,int nNode);//1 audio 2 video 3 H239 4 Data
	void GetVideoMaxFSMBPSFromEntry(int & nMaxMBPS,int & nMaxFS,int type,int format);

	MediaParam m_videoParam[MAX_VIDEOCAP_NUM];
	MediaParam m_dualParam[MAX_VIDEOCAP_NUM];

	//mcu use
	virtual void GetH264SVCVideoParam(int type,VIDEC_CODEC_TYPE codecType,int & frame,int & nLevel,int & nBitrate);

	void AddMPEG4(VIDEO_FORMAT_TYPE format,int nBitrate,int nFrame);                        //添加MPEG4能力 ,use to call kedacom
    int IsExistMPEG4(VIDEO_FORMAT_TYPE format =VIDEO_FORMAT_COUNT,int nBitrate =384,int nFrame =30); 
    int IsFirstMPEG4(VIDEO_FORMAT_TYPE format =VIDEO_FORMAT_COUNT,int nBitrate =384,int nFrame =30); 
	void SetMPEG4First(VIDEO_FORMAT_TYPE format,int nBitrate,int nFrame); 

	//{{发送能力集接收能力集及协商能力集重定义处理 20141030
public:
	int BuildConfCapabilitis(int node,ConfInfo & confCap);
	void GetVideoH264MaxFSMBPS(int & nMaxMBPS,int & nMaxFS,int & nLevel,int nFrame,VIDEO_FORMAT_TYPE format);
	int GetVideoH264Level(VIDEO_FORMAT_TYPE nFormat);
	void SetH263OptionParam(char * tempbuf,HPVT valH,int nodeId,VIDEO_FORMAT_TYPE nH263Format);
	int BuildMediaCapabilitis(int node,DapCallCapability & DapCap);
	int GetAudioCapEntry(X_AUDIO_CODEC_ID CodecType,int nType);
	int GetVideoCapEntry(VIDEC_CODEC_TYPE CodecType,VIDEO_FORMAT_TYPE format,int nDual);  //nDual 0 is main video ,1 is dual video
	void CopyToOriginalCaps(DapCallCapability & DapCap);

	int                 GetVideoFormatValue(VIDEO_FORMAT_TYPE formatType);
	float               GetFrameRateFromH264Param(int nMaxFS,int nMBPS);
	VIDEO_FORMAT_TYPE   GetH264FormatTypeFromBitrate(float nFrame,int nProfile,int nBitrate);
	VIDEO_FORMAT_TYPE   GetFormatTypeFromH264Param(int nProfile,int nLevel,int nMaxFS,int nMBPS,int nBitrate,int nType=0); //nType 0 get min ,1 get max
	VIDEO_FORMAT_TYPE   GetVideoFormatTypeFromMaxFS(int nMaxFS,int nAllMaxFS=0);
	VIDEO_FORMAT_TYPE   GetFormatTypeFromLevel(int nLevel);
	VIDEO_FORMAT_TYPE   GetFormatTypeFromH264OtherParam(int nProfile,int nLevel,int nBitrate,int nType,VIDEO_FORMAT_TYPE nMaxFSType,float nFrame); //nType 0 get min ,1 get max
	void                GetVideoH264MaxFS(int & nMaxFS,VIDEO_FORMAT_TYPE format);

};

//分治法递归求取数组中的最大和最小值
void GetMaxMinBitrateValue(int i,int j,int &fmax,int &fmin,MediaParam A[]);
void GetMaxFormatValue(int &fmax,MediaParam videoParam[]);

char * GetAudioCodecName(X_AUDIO_CODEC_ID type,char * pNameBuf);
char * GetVideoCodecName(VIDEC_CODEC_TYPE type,char * pNameBuf);
char * GetVideoFormatName(VIDEO_FORMAT_TYPE type,char * pNameBuf);
