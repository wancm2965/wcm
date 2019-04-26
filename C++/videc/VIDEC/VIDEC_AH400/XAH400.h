#ifndef __XAH400_H__
#define __XAH400_H__

#include "VIDEC_AH400.h"
#include "VIDEC_Config.h"

#ifdef SUPPORT_AH400

#include "HikVisionSdk.h"
#include "XAutoLock.h"



class XAH400
	: public VIDEC_AH400
{
public:
	XAH400(VIDEC_AH400Callback&rCallback);
	virtual ~XAH400(void);
public:
	int Open(int nDevID,int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate);
	void Close(void);
	
	int StartVideoStream(int nBitrate,int nQuant,int qmin,int qmax,int nKeyFrameInterval,bool bEnableSub=false,int nSubScaleType=1,int nSubBitrate=0,int nSubQuant=0,int nSubQMin=0,int nSubQMax=0);
	int StopVideoStream(void);
	int RequestKeyFrame(void);
	int RequestSubKeyFrame(void);

	int GetWidth(void){return m_nWidth;};
	HANDLE GetChannelHandle(void);
	bool IsNTSC(void);
	void OnImageStreamCallback(void);

	void OnVideoFrameMain(unsigned char*pData,unsigned long ulLen,bool bKeyFrame);
	void OnVideoFrameSub(unsigned char*pData,unsigned long ulLen,bool bKeyFrame);
	void OnAudioFramePacket(unsigned char*pData,unsigned long ulLen);

	//设置遮掩并启动遮掩
	int SetupMask(RECT *RectList, int iRectCount);
	//停止遮掩
	int StopMask(void);
	 
	int SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/);

	int StartMotionDetect(void);
	int StopMotionDetect(void);
	int SetupMotionDetect(int iGrade/*灵敏度等级0-6*/,RECT *RectList, int iRectCount);
	
	void OnMotionDetected(void*pData);

	void EnableOSD(bool bEnable);
	void SetOSDText(void);
protected:
	VIDEC_AH400Callback&	m_rCallback;
	bool					m_bNTSC;

	int						m_nImageSize;
	HANDLE					m_pChannelHandle;
	unsigned char*			m_pImageBuffer;
	int						m_nDevID;


	int						m_nRectCount;
	int*					m_pRectResult;


	int						m_nFrameRate;
	int						m_nWidth;
	int						m_nHeight;
	bool					m_bVideoStreamStarted;

	int						m_nSubWidth;
	int						m_nSubHeight;
	bool					m_bSubVideoStreamStarted;

	unsigned char*			m_pStreamBuf;
	unsigned short			m_usSeqNo;
	unsigned short			m_usSubSeqNo;

	unsigned long			m_nSkipCount;
	unsigned long			m_nSkipIndex;

	unsigned short			m_usAudioSeqNo;
	unsigned long			m_ulLastFrameTimestamp;
	unsigned long			m_ulLastPacketTimestamp;

	bool					m_bEnableOSD;
};

#endif
#endif