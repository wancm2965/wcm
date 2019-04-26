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

////1 �������� 2�Զ����� 
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
	int                 m_nClockRate;         //ʱ����
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
	int                 m_nDirection;    //���䷽�� cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
	int                 m_nFormatValue;  //�ֱ��ʶ�Ӧʵ�ʴ�С
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
	int                 m_nDirection;    //���䷽�� cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit
	int                 m_nFormatValue;  //�ֱ��ʶ�Ӧʵ�ʴ�С
	int                 m_nLevelValue;   //H264 Level
	int                 m_nProfileValue; //H264 Profile
	int                 m_nMaxFS;        //H264 MaxFS
	int                 m_nMaxMBPS;      //H264 MaxMBPS
	int                 m_nPayload;      //SIP Payload
	int                 m_nClockRate;    //ʱ����
	bool                m_bNegEncryption;  //Э�̽�� Э�̳ɹ�Ϊtrue Э��ʧ��Ϊfalse
	bool                m_bNegMedia;     //Э�̽�� Э�̳ɹ�Ϊtrue Э��ʧ��Ϊfalse
}NegMediaCap;



/*��CallCapability�������õ���ͬʱ������*/
class CallCapability
{
public:
	CallCapability(void);
public:
	~CallCapability(void);
protected:
	virtual int SetSimultaneousCap();                     //����ͬʱ���������
	//����ֵΪ0ʧ�ܻ򲻴���
	virtual int IsExistSimul(int type ,int iEntryNumber); //�����Ƿ����
	virtual int IsFirstSimul(int type ,int iEntryNumber); //�����Ƿ�����ѡ	    
	int AddVideoCap(VIDEO_CAPABILITY_ITEM& cap) ;             //�����Ƶ����
	int AddAudioCap(AUDIO_CAPABILITY_ITEM& cap) ;             //�����Ƶ����
	int AddH239Cap(VIDEO_CAPABILITY_ITEM& cap) ;              //���H239Ƶ����
	int AddDataCap(DATA_CAPABILITY_ITEM& cap) ;              //�����������	
	int SetVideoCap(int type,VIDEO_CAPABILITY_ITEM& cap);     //��Ϊ��λ,�Ƴ�����
	int SetAudioCap(int type,AUDIO_CAPABILITY_ITEM& cap);     //��Ϊ��λ,�Ƴ�����
	int SetH239Cap(int type,VIDEO_CAPABILITY_ITEM& cap);      //��Ϊ��λ,�Ƴ�����
	int SetDataCap(int type,DATA_CAPABILITY_ITEM& cap);      //��Ϊ��λ,�Ƴ�����
	virtual int OperationVideoCap(eCapNum capIndex,int operation,BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);  //���������� 1��� 2��ѡ 3�Ƴ�,Ĭ��ֵѡ��ֻ����H261,H263
	virtual int OperationAudioCap(eCapNum capIndex,int operation);  //���������� 1��� 2��ѡ 3�Ƴ�   
	virtual int OperationH239Cap(eCapNum capIndex,int operation,BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);   //���������� 1��� 2��ѡ 3�Ƴ�  
	virtual int OperationDataCap(eCapNum capIndex,int operation);   //���������� 1��� 2��ѡ 3�Ƴ�   

public:
	int m_NodeId;                                 // ���������ID
	AudioCap		m_AudioCap;				      // ��Ƶ������
	VideoCap		m_VideoCap;				      // ��Ƶ������
	H239VideoCap	m_H239VideoCap;			      // H239˫��������
	DataCap			m_DataCap;	                  // ����������

	virtual int InitCapabilitis(int node);        //������������������ ʹ��ʱ���ȵ��ô˷���
	int BuildCapabilitis(int node);               //�ύ������������
	virtual int RemoveAllCapabilitis();           //�Ƴ����������� �ٰ���Ҫ���

	//��Ƶ:
	int AddH264QCIF();                         //���H264���� QCIF  
	int RemoveH264QCIF();                      //�Ƴ�H264���� QCIF
	int SetH264FirstQCIF();                    //����ͬʱ������Ϊ��ѡ
	int IsExistH264QCIF();                     //H264 QCIF�����Ƿ����
	int IsFirstH264QCIF();                     //H264 QCIF�Ƿ�Ϊͬʱ��������ѡ

	int AddH264CIF();                         //���H264���� CIF  
	int RemoveH264CIF();                      //�Ƴ�H264���� CIF
	int SetH264FirstCIF();                    //����ͬʱ������Ϊ��ѡ
	int IsExistH264CIF();                     //H264 CIF�����Ƿ����
	int IsFirstH264CIF();                     //H264 CIF�Ƿ�Ϊͬʱ��������ѡ
	int AddH2644CIF();
	int RemoveH2644CIF();
	int SetH264First4CIF();
	int IsExistH2644CIF();                     //H264 4CIF�����Ƿ����
	int IsFirstH2644CIF();                     //H264 4CIF�Ƿ�Ϊͬʱ��������ѡ

	int AddH26416CIF();
	int RemoveH26416CIF();
	int SetH264First16CIF();
	int IsExistH26416CIF();                     //H264 16CIF�����Ƿ����
	int IsFirstH26416CIF();                     //H264 16CIF�Ƿ�Ϊͬʱ��������ѡ

	int AddH264720P();                         //���H264���� 720P
	int RemoveH264720P();                      //�Ƴ�H264���� 720P
	int SetH264First720P();                    //����ͬʱ������Ϊ��ѡ
	int IsExistH264720P();                     //H264 720P�����Ƿ����
	int IsFirstH264720P();                     //H264 720P�Ƿ�Ϊͬʱ��������ѡ

	int AddH264720I();                         //���H264���� 720I
	int RemoveH264720I();                      //�Ƴ�H264���� 720I
	int SetH264First720I();                    //����ͬʱ������Ϊ��ѡ
	int IsExistH264720I();                     //H264 720I�����Ƿ����
	int IsFirstH264720I();                     //H264 720I�Ƿ�Ϊͬʱ��������ѡ

	int AddH2641080P();                         //���H264���� 1080P 
	int RemoveH2641080P();                      //�Ƴ�H264���� 1080P
	int SetH264First1080P();                    //����ͬʱ������Ϊ��ѡ
	int IsExistH2641080P();                     //H264 1080P�����Ƿ����
	int IsFirstH2641080P();                     //H264 1080P�Ƿ�Ϊͬʱ��������ѡ

	int AddH2641080I();                         //���H264���� 1080I 
	int RemoveH2641080I();                      //�Ƴ�H264���� 1080I
	int SetH264First1080I();                    //����ͬʱ������Ϊ��ѡ
	int IsExistH2641080I();                     //H264 1080I�����Ƿ����
	int IsFirstH2641080I();                     //H264 1080I�Ƿ�Ϊͬʱ��������ѡ

	int AddH264VGA();                           //���H264���� VGA 
	int RemoveH264VGA();                        //�Ƴ�H264���� VGA
	int SetH264FirstVGA();                      //����ͬʱ������Ϊ��ѡ
	int IsExistH264VGA();                       //H264 VGA�����Ƿ����
	int IsFirstH264VGA();                       //H264 VGA�Ƿ�Ϊͬʱ��������ѡ

	int AddH264SVGA();                           //���H264���� SVGA 
	int RemoveH264SVGA();                        //�Ƴ�H264���� SVGA
	int SetH264FirstSVGA();                      //����ͬʱ������Ϊ��ѡ
	int IsExistH264SVGA();                       //H264 SVGA�����Ƿ����
	int IsFirstH264SVGA();                       //H264 SVGA�Ƿ�Ϊͬʱ��������ѡ

	int AddH264XGA();                           //���H264���� XGA 
	int RemoveH264XGA();                        //�Ƴ�H264���� XGA
	int SetH264FirstXGA();                      //����ͬʱ������Ϊ��ѡ
	int IsExistH264XGA();                       //H264 XGA�����Ƿ����
	int IsFirstH264XGA();                       //H264 XGA�Ƿ�Ϊͬʱ��������ѡ

	int AddH264SXGA();                           //���H264���� SXGA 
	int RemoveH264SXGA();                        //�Ƴ�H264���� SXGA
	int SetH264FirstSXGA();                      //����ͬʱ������Ϊ��ѡ
	int IsExistH264SXGA();                       //H264 SXGA�����Ƿ����
	int IsFirstH264SXGA();                       //H264 SXGA�Ƿ�Ϊͬʱ��������ѡ


	int m_nHighProfileLevel;

	int AddH264720PHighProfile();								//���H264HighProfile���� 
	int RemoveH264720PHighProfile();							//�Ƴ�H264HighProfile���� 
	int SetFirstH264720PHighProfile();
	int IsExistMainVideoH264720PHighProfile();                //H264HighProfile�����Ƿ����
	int IsFirstMainVideoH264720PHighProfile();                //H264HighProfile�Ƿ�Ϊͬʱ��������ѡ


	int AddH2641080PHighProfile();								//���H264HighProfile���� 
	int RemoveH2641080PHighProfile();							//�Ƴ�H264HighProfile���� 
	int SetFirstH2641080PHighProfile();
	int IsExistMainVideoH2641080PHighProfile();                //H264HighProfile�����Ƿ����
	int IsFirstMainVideoH2641080PHighProfile();                //H264HighProfile�Ƿ�Ϊͬʱ��������ѡ



	int AddH239VideoH264720PHighProfile();                    //���H264HighProfile���� 
	int RemoveH239VideoH264720PHighProfile();                 //�Ƴ�H264HighProfile���� 
	int SetFirstH239VideoH264720PHighProfile();
	int IsExistH239VideoH264720PHighProfile();                //H264HighProfile�����Ƿ����
	int IsFirstH239VideoH264720PHighProfile();                //H264HighProfile�Ƿ�Ϊͬʱ��������ѡ

	int AddH239VideoH2641080PHighProfile();                    //���H264HighProfile���� 
	int RemoveH239VideoH2641080PHighProfile();                 //�Ƴ�H264HighProfile���� 
	int SetFirstH239VideoH2641080PHighProfile();
	int IsExistH239VideoH2641080PHighProfile();                //H264HighProfile�����Ƿ����
	int IsFirstH239VideoH2641080PHighProfile();                //H264HighProfile�Ƿ�Ϊͬʱ��������ѡ

	int IsExistMainVideoH264HighProfile();                //H264HighProfile�����Ƿ����
	int IsFirstMainVideoH264HighProfile();                //H264HighProfile�Ƿ�Ϊͬʱ��������ѡ
	int IsExistMainVideoH264BaseLineProfile();                       //H264 BaseLineProfile�����Ƿ����
	int IsFirstMainVideoH264BaseLineProfile();                       //H264 BaseLineProfile�Ƿ�Ϊͬʱ��������ѡ

	int IsExistH239VideoH264HighProfile();                //H264HighProfile�����Ƿ����
	int IsFirstH239VideoH264HighProfile();                //H264HighProfile�Ƿ�Ϊͬʱ��������ѡ
	int IsExistH239VideoH264BaseLineProfile();				//H264 BaseLineProfile�����Ƿ����
	int IsFirstH239VideoH264BaseLineProfile();				//H264 BaseLineProfile�Ƿ�Ϊͬʱ��������ѡ


	int IsExistH264();                           //��һH264�����Ƿ����
	int IsFirstH264(void);



	int AddH261(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE);                         //���H261���� 
	int RemoveH261();                      //�Ƴ�H261���� 
	int SetH261First();                    //����ͬʱ������Ϊ��ѡ
	int IsExistH261();                     //H261�����Ƿ����
	int IsFirstH261();     

	int AddH263(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);                         //���H263���� 
	int RemoveH263();                      //�Ƴ�H263���� 
	int SetH263First();                    //����ͬʱ������Ϊ��ѡ
	int IsExistH263();                     //H263�����Ƿ����
	int IsFirstH263();     

	int AddH263v2(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);                         //���H263 v2���� 
	int RemoveH263v2();                      //�Ƴ�H263 v2���� 
	int SetH263v2First();                    //����ͬʱ������Ϊ��ѡ
	int IsExistH263v2();                     //H263 v2�����Ƿ����
	int IsFirstH263v2();     

	int AddH263v3(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);                         //���H263 v3���� 
	int RemoveH263v3();                      //�Ƴ�H263 v3���� 
	int SetH263v3First();                    //����ͬʱ������Ϊ��ѡ
	int IsExistH263v3();                     //H263 v3�����Ƿ����
	int IsFirstH263v3();     

	//��Ƶ:
	int AddG7221_32K();                          //���G7221_32K����
	int RemoveG7221_32K();
	int SetFirstG7221_32K();
	int IsExistG7221_32K();                      //G7221_32K�����Ƿ����
	int IsFirstG7221_32K();                      //G7221_32K�Ƿ�Ϊͬʱ��������ѡ

	int AddG7221_24K();
	int RemoveG7221_24K();
	int SetFirstG7221_24K();
	int IsExistG7221_24K();                      //G7221_24K�����Ƿ����
	int IsFirstG7221_24K();                      //G7221_24K�Ƿ�Ϊͬʱ��������ѡ

	// G7221 siren annex c

	int AddG7221C_24K();
	int RemoveG7221C_24K();
	int SetFirstG7221C_24K();
	int IsExistG7221C_24K();                      //G7221_24K�����Ƿ����
	int IsFirstG7221C_24K();                      //G7221_24K�Ƿ�Ϊͬʱ��������ѡ
	// G7221 siren annex c
	int AddG7221C_32K();
	int RemoveG7221C_32K();
	int SetFirstG7221C_32K();
	int IsExistG7221C_32K();                      //G7221_24K�����Ƿ����
	int IsFirstG7221C_32K();                      //G7221_24K�Ƿ�Ϊͬʱ��������ѡ
	// G7221 siren annex c
	int AddG7221C_48K();
	int RemoveG7221C_48K();
	int SetFirstG7221C_48K();
	int IsExistG7221C_48K();                      //G7221_24K�����Ƿ����
	int IsFirstG7221C_48K();                      //G7221_24K�Ƿ�Ϊͬʱ��������ѡ



	int AddG722_64K();                          //���G7221_32K����
	int RemoveG722_64K();   
	int SetFirstG722_64K();   
	int IsExistG722_64K();                        //G722_64K�����Ƿ����
	int IsFirstG722_64K();                        //G722_64K�Ƿ�Ϊͬʱ��������ѡ

	int AddG7231();
	int RemoveG7231();
	int SetFirstG7231();
	int IsExistG7231();                          //G7231�����Ƿ����
	int IsFirstG7231();                          //G7231�Ƿ�Ϊͬʱ��������ѡ

	int AddG729A();
	int RemoveG729A();
	int SetFirstG729A();

	int IsExistG729A(int type=2);                   /*1=G729 2=G729A 3=G729B*/                         //G729A�����Ƿ����
	int IsFirstG729A(int type=2);                   /*1=G729 2=G729A 3=G729B*/                         //G729A�Ƿ�Ϊͬʱ��������ѡ


	int AddG711u_64K();
	int RemoveG711u_64K();
	int SetFirstG711u_64K();
	int IsExistG711u_64K();                      //G711u_64K�����Ƿ����
	int IsFirstG711u_64K();                      //G711u_64K�Ƿ�Ϊͬʱ��������ѡ

	int AddG711a_64K();
	int RemoveG711a_64K();
	int SetFirstG711a_64K();
	int IsExistG711a_64K();                       //G711a_64K�����Ƿ����
	int IsFirstG711a_64K();                       //G711a_64K�Ƿ�Ϊͬʱ��������ѡ

	//notice add 20120202
	//GSM ע�� ������SIP
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
	int IsExistG719_64K();                      //G719�����Ƿ����
	int IsFirstG719_64K();                      //G719�Ƿ�Ϊͬʱ��������ѡ
	// G719
	int AddG719_32K();
	int RemoveG719_32K();
	int SetFirstG719_32K();
	int IsExistG719_32K();                      //G719�����Ƿ����
	int IsFirstG719_32K();                      //G719�Ƿ�Ϊͬʱ��������ѡ
	// G719
	int AddG719_48K();
	int RemoveG719_48K();
	int SetFirstG719_48K();
	int IsExistG719_48K();                      //G719�����Ƿ����
	int IsFirstG719_48K();                      //G719�Ƿ�Ϊͬʱ��������ѡ





	//H239
	int IsExistH239VideoH264Extend();                           //��һH264�����Ƿ����
	int IsFirstH239VideoH264Extend();                           //��һH264�����Ƿ����

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

	int IsExistH239VideoH264QCIFExtend();            //H264Extend�����Ƿ����
	int IsExistH239VideoH264CIFExtend();            //H264Extend�����Ƿ����
	int IsExistH239VideoH2644CIFExtend();            //H264Extend�����Ƿ����
	int IsExistH239VideoH26416CIFExtend();            //H264Extend�����Ƿ����
	int IsExistH239VideoH264720PExtend();            //H264Extend�����Ƿ����
	int IsExistH239VideoH264720IExtend();            //H264Extend�����Ƿ����
	int IsExistH239VideoH2641080PExtend();            //H264Extend�����Ƿ����
	int IsExistH239VideoH2641080IExtend();            //H264Extend�����Ƿ����
	int IsExistH239VideoH264VGAExtend();            //H264Extend�����Ƿ����
	int IsExistH239VideoH264SVGAExtend();            //H264Extend�����Ƿ����
	int IsExistH239VideoH264XGAExtend();            //H264Extend�����Ƿ����
	int IsExistH239VideoH264SXGAExtend();            //H264Extend�����Ƿ����

	int IsFirstH239VideoH264QCIFExtend();            //H264Extend�Ƿ�Ϊͬʱ��������ѡ
	int IsFirstH239VideoH264CIFExtend();            //H264Extend�Ƿ�Ϊͬʱ��������ѡ
	int IsFirstH239VideoH2644CIFExtend();            //H264Extend�Ƿ�Ϊͬʱ��������ѡ
	int IsFirstH239VideoH26416CIFExtend();            //H264Extend�Ƿ�Ϊͬʱ��������ѡ
	int IsFirstH239VideoH264720PExtend();            //H264Extend�Ƿ�Ϊͬʱ��������ѡ
	int IsFirstH239VideoH264720IExtend();            //H264Extend�Ƿ�Ϊͬʱ��������ѡ
	int IsFirstH239VideoH2641080PExtend();            //H264Extend�Ƿ�Ϊͬʱ��������ѡ
	int IsFirstH239VideoH2641080IExtend();            //H264Extend�Ƿ�Ϊͬʱ��������ѡ
	int IsFirstH239VideoH264VGAExtend();            //H264Extend�Ƿ�Ϊͬʱ��������ѡ
	int IsFirstH239VideoH264SVGAExtend();            //H264Extend�Ƿ�Ϊͬʱ��������ѡ
	int IsFirstH239VideoH264XGAExtend();            //H264Extend�Ƿ�Ϊͬʱ��������ѡ
	int IsFirstH239VideoH264SXGAExtend();            //H264Extend�Ƿ�Ϊͬʱ��������ѡ


	int AddH239VideoH261Extend(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE);
	int RemoveH239VideoH261Extend();
	int SetFirstH239VideoH261Extend();
	int IsExistH239VideoH261Extend();            //H263Extend�����Ƿ����
	int IsFirstH239VideoH261Extend();            //H263Extend�Ƿ�Ϊͬʱ��������ѡ

	int AddH239VideoH263Extend(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);
	int RemoveH239VideoH263Extend();
	int SetFirstH239VideoH263Extend();
	int IsExistH239VideoH263Extend();            //H263Extend�����Ƿ����
	int IsFirstH239VideoH263Extend();            //H263Extend�Ƿ�Ϊͬʱ��������ѡ

	int AddH239VideoH263v2Extend(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);
	int RemoveH239VideoH263v2Extend();
	int SetFirstH239VideoH263v2Extend();
	int IsExistH239VideoH263v2Extend();            //H263Extend�����Ƿ����
	int IsFirstH239VideoH263v2Extend();            //H263Extend�Ƿ�Ϊͬʱ��������ѡ

	int AddH239VideoH263v3Extend(BOOL hasQCIF=FALSE,BOOL hasCIF=FALSE,BOOL has4CIF=FALSE);
	int RemoveH239VideoH263v3Extend();
	int SetFirstH239VideoH263v3Extend();
	int IsExistH239VideoH263v3Extend();            //H263Extend�����Ƿ����
	int IsFirstH239VideoH263v3Extend();            //H263Extend�Ƿ�Ϊͬʱ��������ѡ


	//Data FECC
	int AddFECCData();
	int RemoveFECCData();
	int SetFirstFECCData();
	int IsExistFECCData();                      //H224�����Ƿ����
	int IsFirstFECCData();                      //H224�Ƿ�Ϊͬʱ��������ѡ


public:
	int						m_nMainVideoMaxBitrate;
	int						m_nSecondVideoMaxBitrate;
	int						m_nUseCapOLC;									//0������������ 1�������߼�ͨ�������� Ĭ��Ϊ0
	int						m_nG729Type;									//����OLC  1=G729 2=G729A 3=G729B

	int RemoveAudioAllCapabilitis();										//�Ƴ���Ƶ����������
	int RemoveVideoAllCapabilitis();										//�Ƴ���Ƶ����������
	int RemoveDualVideoAllCapabilitis();									//�Ƴ���������������
	int RemoveFECCDataAllCapabilitis();										//�Ƴ�FECC����������

	void SetMainVideoMaxBitrate(int nMaxBitrate);
	void SetSecondVideoMaxBitrate(int nMaxBitrate);
	int GetMainVideoMaxBitrate(int nVideoRate);
	int GetSecondVideoMaxBitrate(int nVideoRate);
	void SetCapabilitisUseType(int nType=0);								//����OLC 0������������ 1�������߼�ͨ�������� Ĭ��Ϊ0
	void SetG729Type(int nType);											//����OLC  1=G729 2=G729A 3=G729B
	int AddH264MainVideoCap(VIDEC_CODEC_TYPE codecType,int nLevel);			//����OLC ���H264���� level  
	int AddH264SecondVideoCap(VIDEC_CODEC_TYPE codecType,int nLevel);		//����OLC ���H264���� level
	void SetH264OptionParamMaxMBPS(int nValue);								//����OLC��������ѡ����ֵ
	void SetH264OptionParamMaxFS(int nValue);								//����OLC��������ѡ����ֵ
	void SetH264OptionParamMaxDPB(int nValue);								//����OLC��������ѡ����ֵ
	void SetH264OptionParamMaxBR(int nValue);								//����OLC��������ѡ����ֵ
	void SetMainVideoFrameRate(int nFrame);
	void SetSecondVideoFrameRate(int nFrame);

protected:
	int						m_nOLCMaxMBPS;									//����OLC��������ѡ����ֵ ����鴦������
	int						m_nOLCMaxFS;									//����OLC��������ѡ����ֵ ���֡��
	int						m_nOLCMaxDPB;									//����OLC��������ѡ����ֵ ������ͼ�񻺴��С
	int						m_nOLCMaxBR;									//����OLC��������ѡ����ֵ �����Ƶ����
	int						m_nMainVideoFrameRate;							//default >0
	int						m_nSecondVideoFrameRate;						//default >0

private:
	//������������������ͬʱ������
	void DescriptorBuild(HPVT valH,int nodeId,char * tempbuf,int iEntryNumber,int &nSimulNum,int &nSimultaneous,int &iAlternative);
	//������������EntryNumber����������·���Ӵ�
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

	void AddMPEG4(VIDEO_FORMAT_TYPE format,int nBitrate,int nFrame);                        //���MPEG4���� ,use to call kedacom
    int IsExistMPEG4(VIDEO_FORMAT_TYPE format =VIDEO_FORMAT_COUNT,int nBitrate =384,int nFrame =30); 
    int IsFirstMPEG4(VIDEO_FORMAT_TYPE format =VIDEO_FORMAT_COUNT,int nBitrate =384,int nFrame =30); 
	void SetMPEG4First(VIDEO_FORMAT_TYPE format,int nBitrate,int nFrame); 

	//{{����������������������Э���������ض��崦�� 20141030
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

//���η��ݹ���ȡ�����е�������Сֵ
void GetMaxMinBitrateValue(int i,int j,int &fmax,int &fmin,MediaParam A[]);
void GetMaxFormatValue(int &fmax,MediaParam videoParam[]);

char * GetAudioCodecName(X_AUDIO_CODEC_ID type,char * pNameBuf);
char * GetVideoCodecName(VIDEC_CODEC_TYPE type,char * pNameBuf);
char * GetVideoFormatName(VIDEO_FORMAT_TYPE type,char * pNameBuf);
