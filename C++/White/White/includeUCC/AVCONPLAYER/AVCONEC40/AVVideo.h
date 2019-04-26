//AVVideo.h
#ifndef __AVVIDEO_H__
#define __AVVIDEO_H__

typedef enum VIDEO_SIZE
{
	VIDEO_SIZE_176X144=1,
	VIDEO_SIZE_240X176,
	VIDEO_SIZE_320X240,
	VIDEO_SIZE_352X288,
	VIDEO_SIZE_640X480,
	VIDEO_SIZE_704X576,
	VIDEO_SIZE_720X576,
	VIDEO_SIZE_480X352,
	VIDEO_SIZE_320X480
}VIDEO_SIZE;

typedef enum VIDEO_QUALITY
{	
	WORST=0,
	WORSE,
	BAD,
	GOOD,
	BETTER,
	BEST
}VIDEO_QUALITY;

typedef enum VIDEO_CODEC
{
	AVV1=0,		//divx502
	AVV2,		//divx505
	AVV3,		//xvid
	AVV4,		//H263
	AVV5,		//H263PLUS	
	AVV6,		//H264
	AVVIDEO_CODEC_H263PP,
	AVVIDEO_CODEC_WMV9,	
	AVVIDEO_CODEC_WMVA,
	AVVIDEO_CODEC_X264_VBR_HARDWARE,
	AVVIDEO_CODEC_X264_CBR_HARDWARE,
	AVVIDEO_CODEC_X264_VBR,
	AVVIDEO_CODEC_X264_CBR,
	AVVIDEO_CODEC_H263P_VBR,
	AVVIDEO_CODEC_H263P_CBR,
	AVVIDEO_CODEC_LSCC
}VIDEO_CODEC;

#define AVVIDEO_CODEC_X264_VBR_SOFTWARE AVVIDEO_CODEC_WMVA
#define AVVIDEO_CODEC_X264_CBR_SOFTWARE AVVIDEO_CODEC_WMV9

//{{xmc 20060630
//#ifndef HPMD_TYPE
//typedef enum
//{
//	HPMD_1X1=0,
//	HPMD_1X2,
//	HPMD_1P2,
//	HPMD_2X2,
//	HPMD_3X2,
//	HPMD_1P5,
//	HPMD_COUNT
//}HPMD_TYPE;
//#endif
//}}


#pragma pack(push,1)

/*
��Ƶ֡ͷ
*/
typedef struct tagAVVIDEO_FRAME_HEADER
{
	unsigned char	ucVideoCodec;
	unsigned char	ucFrameRate;//PAL1-25 NTSC1-30
	unsigned short	usVideoWidth;
	unsigned short	usVideoHeight;
}AVVIDEO_FRAME_HEADER,*PAVVIDEO_FRAME_HEADER;

/*
��Ƶѹ�����ͷ�������ǰ�������
*/
typedef struct tagAVVIDEO_PACKET_HEADER
{
	bool 					bKeyFrame;	//�Ƿ��ǹؼ�֡
	unsigned long			ulTimeStamp;//��������ʱ���
	AVVIDEO_FRAME_HEADER	AVVideoFrameHeader;//��Ƶ֡ͷ
}AVVIDEO_PACKET_HEADER,*PAVVIDEO_PACKET_HEADER;
#pragma pack(pop,1)
#pragma pack(push,4)


AFX_EXT_CLASS int GetVideoCaptureDeviceCount(void);
AFX_EXT_CLASS bool GetVideoCaptureDeviceName(int nVideoCaptureDeviceIndex,char*szVideoCaptureDeviceName);
AFX_EXT_CLASS bool IsVGADevice(int nDevID);

/***********************************************************************/
/*�ɼ�RGB24����Ƶ����ѹ��
/***********************************************************************/
class IAVVideoCaptureEx:public AVSource
{
public:
	IAVVideoCaptureEx(){};
	virtual~IAVVideoCaptureEx(){};
public:
	/***********************************************************************/
	/*���ܣ��ͷ���Ƶ�ɼ���ռ�е�������Դ
	/*������
	/*���أ�
	/*��ע������ɾ���ö���ǰ���ô˺�������ȷ���ͷ�������Դ
	/***********************************************************************/
	virtual void ReleaseConnections(void)=0;

	/***********************************************************************/
	/*���ܣ��Ƿ�ɼ���Ƶ
	/*��������bool bCapture���Ƿ�ɼ���Ƶ�����bCapture=true�ɼ�������ֹͣ�ɼ�
	/*���أ�����Ѿ�����Ƶ�豸�򷵻�true�����򷵻�false
	/*��ע��
	/***********************************************************************/
	virtual bool Capture(bool bCapture)=0;
	
	/***********************************************************************/
	/*���ܣ��ж��Ƿ������ڲɼ���Ƶ
	/*������
	/*���أ�������ڲɼ���Ƶ�򷵻�true�����򷵻�false
	/*��ע�����û�д���Ƶ�豸��Ҳ����false
	/***********************************************************************/
	virtual bool IsCapturing(void)=0;

	/***********************************************************************/
	/*���ܣ��Ƿ���ʾ��Ƶ����
	/*������(bool bShow) �Ƿ���ʾ��Ƶ���ڣ����bShow=true��ʾ����������
	/*���أ����û�д���Ƶ�豸�򷵻�false�����򷵻�true
	/*��ע��
	/***********************************************************************/
	virtual bool ShowVideoWindow(bool bShow)=0;
	
	/***********************************************************************/
	/*���ܣ��ж���Ƶ�����Ƿ�ɼ�
	/*������
	/*���أ������Ƶ���ڿɼ��򷵻�true�����򷵻�false
	/*��ע�����û�д���Ƶ�豸��Ҳ����false
	/***********************************************************************/
	virtual bool IsVideoWindowVisible(void)=0;
	
	/***********************************************************************/
	/*���ܣ�������Ƶ���ڵ�λ�úʹ�С
	/*������(int x, int y, int cx, int cy)x��yΪ���Ͻ����꣬cxΪ��Ƶ���ڿ�ȣ�
	/*		cyΪ��Ƶ���ڸ߶�
	/*���أ����û�д���Ƶ�豸�򷵻�false�����򷵻�true
	/*��ע��
	/***********************************************************************/
	virtual bool SetVideoWindowPosition(int x, int y, int cx, int cy)=0;
	
	/***********************************************************************/
	/*���ܣ�������Ƶ��һ֡����Ϊָ����λͼ�ļ�(RGB24)
	/*������(const char*cszBitmapFile)Ҫ�����λͼ�ļ���
	/*���أ�������񲢱���ɹ����򷵻�true�����򷵻�false
	/*��ע�������Ƶ�豸û�д�Ҳ����false
	/***********************************************************************/
	virtual bool CaptureToBitmapFile(const char*cszBitmapFile)=0;

	/***********************************************************************/
	/*���ܣ���ȡ��Ƶԭʼ���
	/*������
	/*���أ����û�д���Ƶ�豸�򷵻�0�����򷵻���Ƶԭʼ���
	/*��ע��
	/***********************************************************************/
	virtual int GetWidth(void)=0;

	/***********************************************************************/
	/*���ܣ���ȡ��Ƶԭʼ�߶�
	/*������
	/*���أ����û�д���Ƶ�豸�򷵻�0�����򷵻���Ƶԭʼ�߶�
	/*��ע��
	/***********************************************************************/
	virtual int GetHeight(void)=0;

	/***********************************************************************/
	/*���ܣ���ʾ��Ƶ��������
	/*������
	/*���أ�
	/*��ע�����û�д���Ƶ�豸������ʾ�κδ���
	/***********************************************************************/
	virtual void AdjustVideo(void)=0;
	
	/***********************************************************************/
	/*���ܣ���ʾ��Ƶ���봰��
	/*������
	/*���أ�
	/*��ע�����û�д���Ƶ�豸����Ƶ�豸û���ṩ��Ƶ����ӿڣ�����ʾ�κδ���
	/***********************************************************************/
	virtual void Crossbar(void)=0;

	/***********************************************************************/
	/*���ܣ��ж���Ƶ�豸�Ƿ��ṩ����Ƶ����ӿ�
	/*������
	/*���أ������Ƶ�豸�ṩ����Ƶ����ӿڣ��򷵻�true�����򷵻�false
	/*��ע�������Ƶ�豸û�д�Ҳ����false
	/***********************************************************************/
	virtual bool CanCrossBar(void)=0;

	/***********************************************************************/
	/*���ܣ��ߵ�ͼ��
	/*������(bool bInverted)�Ƿ�ߵ�ͼ�����bInverted=true��ʹͼ��ߵ���
	/*		����������ʾͼ��
	/*���أ�
	/*��ע�����ʹͼ��ߵ������Ժ�ɼ���ͼ����ǵ������ģ�
	/***********************************************************************/
	virtual void Invert(bool bInverted)=0;
	
	/***********************************************************************/
	/*���ܣ��ж�ͼ���Ƿ��ǵߵ���
	/*������
	/*���أ�����ǵߵ����򷵻�true�����򷵻�false
	/*��ע��
	/***********************************************************************/
	virtual bool IsInverted(void)=0;
	virtual bool IsDeinterlaceEnable(void)=0;
	virtual void EnableDeinterlace(bool bDeinterlace)=0;

	virtual bool SetH263PacketsSink(AVSink*pH263PacketsSink)=0;
	virtual void StartManualKeyFrame(void)=0;
	virtual void EnableSkipDisplay(bool bEnable)=0;

	virtual void EnableDenoise(bool bDenoise)=0;
	virtual bool IsDenoiseEnable(void)=0;

	//{{xmc add 20060630
	virtual bool EnableVideoOutput(unsigned long ulDisplayChannelIndex,unsigned long ulRectIndex=0)=0;
	virtual bool DisableVideoOutput(unsigned long ulDisplayChannelIndex,unsigned long ulRectIndex=0)=0;
	//}}

	//{{xmc add 20070505
	virtual void EnableTitle(bool bEnable)=0;
	virtual bool IsTitleEnable(void)=0;
	virtual void SetTitleText(const TCHAR*cszText,int nAlign=0)=0;//Align==0 TOP_LEFT 1:BOTOM_LEFT
	//}}
};

/***********************************************************************/
/*���ܣ�������Ƶ�ɼ�ѹ������
/*������HWND hWnd[��ʾ��Ƶ�Ĵ��ھ��],int nDevice[��Ƶ�豸����ţ���0��ʼ],
/*		unsigned short usFrameRate[��Ƶ֡�� PAL1-25,NTSC1-30],
/*		int nWidth[��Ƶ�Ŀ��],int nHeight[��Ƶ�ĸ߶�],
/*		VIDEO_QUALITY VideoQuality[��Ƶ����],bool bDeinterlace[��������ɨ��],
/*		VIDEO_CODEC VideoCodec[��Ƶ��������]
/*���أ���������ɹ��򷵻���Ƶ�ɼ�ѹ�����󣬷��򷵻�NULL
/*��ע������޷�����Ƶ�豸����Ƶ��֧�ֵķֱ��ʻ�֧�ֵ���Ƶ���붼������NULL
/***********************************************************************/
AFX_EXT_CLASS IAVVideoCaptureEx*CreateVideoCaptureEx(HWND hWnd,int nDevice,unsigned short usFrameRate,int nWidth,int nHeight,VIDEO_CODEC VideoCodec,VIDEO_QUALITY VideoQuality,bool bDeinterlace,int nMaxBitrate,bool bAutoFrameRate);

/***********************************************************************/
/*��Ƶ��������ѹ����Ƶ��ֻ��ѹ��RGB24��λͼ
/***********************************************************************/
class IAVVideoEncoder
{
public:
	IAVVideoEncoder(){};
	virtual ~IAVVideoEncoder(void){};
public:
	/***********************************************************************/
	/*���ܣ��ͷű�������ռ����Դ
	/*������
	/*���أ�
	/*��ע��ɾ������������ǰӦ�ȵ��ô˷�������ȷ���ͷ�������Դ
	/***********************************************************************/
	virtual void ReleaseConnections(void)=0;
	
	/***********************************************************************/
	/*���ܣ���ȡ����������
	/*������
	/*���أ�����������
	/*��ע��
	/***********************************************************************/
	virtual VIDEO_CODEC GetCodec(void)=0;
	
	/***********************************************************************/
	/*���ܣ�ѹ��һ֡ͼ��
	/*������void *input[in,δѹ����ͼ������], int inputSizeBytes[in,δѹ����ͼ�����ݴ�С],
	/*		void *output[out,ѹ�����ͼ������], int *outputSizeBytes[out,ѹ�����ͼ�����ݴ�С],
	/*		bool*pbIsKeyFrame[out,�Ƿ��ǹؼ�֡]
	/*���أ�����ɹ����򷵻�ѹ�����ͼ�����ݴ�С�����򷵻�0
	/*��ע��ֻ��ѹ��RGB24��λͼ
	/***********************************************************************/
	virtual int Encode(void *input, int inputSizeBytes, void *output, int *outputSizeBytes,bool*pbIsKeyFrame)=0;

	virtual bool SetH263PacketsSink(AVSink*pH263PacketsSink){return false;};

	virtual void StartManualKeyFrame(void){};

	virtual void GetExtraData(const void**ppExtraData,unsigned int*pnExtraDataLen){};
};

/***********************************************************************/
/*���ܣ�������Ƶ������
/*������VIDEO_CODEC VideoCodec[��Ƶ��������],int nWidth[��Ƶ�Ŀ��],
/*		int nHeight[��Ƶ�ĸ߶�],unsigned short usFrameRate[��Ƶ֡�� PAL1-25,NTSC1-30],
/*		VIDEO_QUALITY VideoQuality[��Ƶ����],bool bDeinterlace[��������ɨ��],
/*���أ������֧�ֵı������򷵻���Ƶ���������󣬷��򷵻�NULL
/*��ע��
/***********************************************************************/
AFX_EXT_CLASS IAVVideoEncoder*CreateVideoEncoder(VIDEO_CODEC VideoCodec,
												 int cx,int cy,
												 unsigned short usFrameRate,	
												 VIDEO_QUALITY VideoQuality,
												 bool bI420=false);

/***********************************************************************/
/*��Ƶ����������ѹ����Ƶ֡��ֻ�ܽ�ѹ����RGB24��λͼ
/***********************************************************************/
class IAVVideoDecoder
{
public:
	IAVVideoDecoder(void){};
	virtual~IAVVideoDecoder(void){};
public:
	/***********************************************************************/
	/*���ܣ��ͷŽ�������ռ����Դ
	/*������
	/*���أ�
	/*��ע��ɾ������������ǰӦ�ȵ��ô˷�������ȷ���ͷ�������Դ
	/***********************************************************************/
	virtual void ReleaseConnections(void)=0;
	
	virtual void SetPostProcessQuality(int nQuality)=0;

	virtual bool Decompress(const unsigned char *pSrc,unsigned int nSrcLen,unsigned char* dst[],int dstStride[])=0;
};

/***********************************************************************/
/*���ܣ�������Ƶ������
/*������VIDEO_CODEC VideoCodec[��Ƶ��������],int cx[��Ƶ�Ŀ��],int cy[��Ƶ�ĸ߶�]
/*���أ������֧�ֵĽ������򷵻���Ƶ���������󣬷��򷵻�NULL
/*��ע��
/***********************************************************************/
AFX_EXT_CLASS IAVVideoDecoder*CreateVideoDecoder(VIDEO_CODEC VideoCodec,int cx,int cy,unsigned short usFrameRate,const void *pExtraData,unsigned int nExtraDataLen);

/***********************************************************************/
/*���ܣ���VIDEO_SIZEת���ɿ�Ⱥ͸߶�
/*������VIDEO_SIZE VideoSize[in,��Ƶ�ֱ���],int&nWidth[out,���],int&nHeight[out,�߶�]
/*���أ������֧�ֵķֱ����򷵻�true,�����ؿ�Ⱥ͸߶ȣ����򷵻�false
/*��ע��
/***********************************************************************/
AFX_EXT_CLASS bool VideoSizeConvert(VIDEO_SIZE VideoSize,int&nWidth,int&nHeight);

class XRGBCallback
{
public:
	XRGBCallback(void){};
	virtual~XRGBCallback(void){};
public:
	virtual void OnXRGBCallbackData(unsigned char*pRGBData,int nWidth,int nHeight)=0;
};


#define AV_VIDEO_PLAYER_RESIZE	(WM_USER+7086)
/***********************************************************************/
/*��Ƶ�����������н����ͬ�����Ź���
/***********************************************************************/
class IAVVideoPlayerEx : public AVSink
{
public:
	IAVVideoPlayerEx(){};
	virtual~IAVVideoPlayerEx(){};
public:
	/***********************************************************************/
	/*���ܣ��ͷŲ�������ռ����Դ
	/*������
	/*���أ�
	/*��ע��ɾ������������ǰӦ�ȵ��ô˷�������ȷ���ͷ�������Դ
	/***********************************************************************/
	virtual void ReleaseConnections()=0;
	/***********************************************************************/
	/*���ܣ���ȡ��Ƶԭʼ���
	/*������
	/*���أ����û�д���Ƶ�豸�򷵻�0�����򷵻���Ƶԭʼ���
	/*��ע��
	/***********************************************************************/
	virtual int GetWidth(void)=0;

	/***********************************************************************/
	/*���ܣ���ȡ��Ƶԭʼ�߶�
	/*������
	/*���أ����û�д���Ƶ�豸�򷵻�0�����򷵻���Ƶԭʼ�߶�
	/*��ע��
	/***********************************************************************/
	virtual int GetHeight(void)=0;

	/***********************************************************************/
	/*���ܣ��Ƿ���ʾ��Ƶ����
	/*������(bool bShow) �Ƿ���ʾ��Ƶ���ڣ����bShow=true��ʾ����������
	/*���أ����û�д���Ƶ�豸�򷵻�false�����򷵻�true
	/*��ע��
	/***********************************************************************/
	virtual bool ShowVideoWindow(bool bShow)=0;
	
	/***********************************************************************/
	/*���ܣ��ж���Ƶ�����Ƿ�ɼ�
	/*������
	/*���أ������Ƶ���ڿɼ��򷵻�true�����򷵻�false
	/*��ע�����û�д���Ƶ�豸��Ҳ����false
	/***********************************************************************/
	virtual bool IsVideoWindowVisible(void)=0;
	
	/***********************************************************************/
	/*���ܣ�������Ƶ���ڵ�λ�úʹ�С
	/*������(int x, int y, int cx, int cy)x��yΪ���Ͻ����꣬cxΪ��Ƶ���ڿ�ȣ�cyΪ��
	/*		Ƶ���ڸ߶�
	/*���أ����û�д���Ƶ�豸�򷵻�false�����򷵻�true
	/*��ע��
	/***********************************************************************/
	virtual bool SetVideoWindowPosition(int x, int y, int cx, int cy)=0;

	virtual void SetVideoQuality(int nVideoQuality)=0;
	virtual int GetVideoQuality(void)=0;

	virtual bool HasVideo(void)=0;
	virtual bool CaptureToBitmapFile(const char*cszBitmapFile)=0;
	virtual void EnableSkipDisplay(bool bEnable)=0;

	virtual void EnableVideoJitterBuffer(bool bEnable)=0;
	virtual bool IsVideoJitterBufferEnable(void)=0;

	//{{xmc add 20060629
	virtual bool UseHardwareDecoder(bool bUse)=0;
	virtual bool IsUsingHardwareDecoder(void)=0;
	virtual bool EnableVideoOutput(unsigned long ulDisplayChannelIndex,unsigned long ulRectIndex=0)=0;
	virtual bool DisableVideoOutput(unsigned long ulDisplayChannelIndex,unsigned long ulRectIndex=0)=0;
	//}}

	//{{xmc add 20070505
	virtual void EnableTitle(bool bEnable)=0;
	virtual bool IsTitleEnable(void)=0;
	virtual void SetTitleText(const TCHAR*cszText,int nAlign=0)=0;//Align==0 TOP_LEFT 1:BOTOM_LEFT
	//}}

	virtual void SetXRGBCallback(XRGBCallback*pXRGBCallback)=0;
};

/***********************************************************************/
/*���ܣ�������Ƶͬ�����Ŷ���
/*������HWND hWnd[in,��Ƶ��ʾ���],int cx[in,��Ƶ���],int cy[in,��Ƶ�߶�],
/*		unsigned short usFrameRate[in,��Ƶ����֡��],
/*		VIDEO_CODEC VideoCodec[in,��Ƶ����������],AVSyn*pAVSyn[in,ͬ����]
/*���أ���������ɹ��򷵻���Ƶͬ�����Ŷ��󣬷��򷵻�NULL
/*��ע�������Ƶ���������Ͳ�֧�֣��ͷ���NULL
/***********************************************************************/
AFX_EXT_CLASS IAVVideoPlayerEx*CreateVideoPlayerEx(HWND hWnd,int cx,int cy,unsigned short usFrameRate,VIDEO_CODEC VideoCodec,AVSyn*pAVSyn);

AFX_EXT_CLASS bool IsVideoCaptureOpen(void);

//{{xmc add 20060629
AFX_EXT_CLASS long GetHiKHWDisplayChannelCount(void);
AFX_EXT_CLASS long GetHiKHWDecoderChannelCount(void);
AFX_EXT_CLASS bool SetHiKHWDisplayChannelTM(unsigned long ulDisplayChannelIndex,HPMD_TYPE HPType);
AFX_EXT_CLASS bool GetHiKHWDisplayChannelTM(unsigned long ulDisplayChannelIndex,HPMD_TYPE&HPType);
AFX_EXT_CLASS unsigned long GetHiKHWDisplayChannelRectCount(unsigned long ulDisplayChannelIndex);
//}}

//{{xmc add 20060814
AFX_EXT_CLASS void LoadHiKHWCapture(void);
AFX_EXT_CLASS void UnLoadHiKHWCapture(void);
AFX_EXT_CLASS void LoadHiKHWDisplay(void);
AFX_EXT_CLASS void UnLoadHiKHWDisplay(void);
//}}


#endif

