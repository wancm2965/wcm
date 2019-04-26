//LocalVideoDev.h
#ifndef __LOCALVIDECDEV_H__
#define __LOCALVIDECDEV_H__

#include <string>

#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_Header.h>
#include <VIDEC/VIDEC_CameraDev.h>
#include <VIDEC/VIDEC_AH2KDev.h>
#include <VIDEC/VIDEC_AH400Dev.h>

#include "XBitrate.h"


class LocalVideoDev
	: public VIDEC_CapChanRTPCallback
	, public VIDEC_StreamSink
{
public:
	LocalVideoDev(void);
	virtual~LocalVideoDev(void);
public:
	virtual int Open(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate);
	virtual void RequestKeyFrame(void);
	virtual void Close(void);


	/******************************************************************************
	* SetVideoParam
	*描述：设置本地主流视频参数
	*输入：	nBrightness		-亮度值0-255
			nContrast		-对比度0-127
			nSaturation		-饱和度0-127
			nHue			-色调0-255
			nGamma			-伽马1-400
			nSharpen		-锐化0-127
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetVideoEnableOSD
	*描述：设置本地主流视频是否启用OSD
	*输入：	cszCallID		-呼叫标识
			nEnable			-是否启用OSD
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetVideoEnableOSD(int nEnable);

	/******************************************************************************
	* SetVideoOSDText
	*描述：设置本地主流视频OSD文字
	*输入：	cszCallID		-呼叫标识
			cszText			-OSD文字
			nAlign			-对齐方式
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetVideoOSDText(const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetVideoOSDFont
	*描述：设置本地主流视频OSD字体,字的颜色和背景色
	*输入：	logfont			-OSD字体
			crText			-字的颜色
			crBk			-背景色
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetVideoOSDFont(LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));
protected:
	virtual void OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource){};
	virtual void OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource){};
	virtual void OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame=false,int nWidth=0,int nHeight=0,int nHeaderLen=0);
	virtual void OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen);
protected:
	VIDEC_CapDev*			m_pVideoCapDev;
	VIDEC_CapChanRTP*		m_pVideoCapChan;
	VIDEC_CODEC_TYPE		m_nVideoCodecType;
	int						m_nVideoBrightness;
	int						m_nVideoContrast;
	int						m_nVideoSaturation;
	int						m_nVideoHue;
	int						m_nVideoGamma;
	int						m_nVideoSharpen;
	bool					m_bVideoSharpenLocalOnly;

	int						m_nVideoEnableOSD;
	std::string				m_strVideoOSD;
	int						m_nVideoAlign;
	COLORREF				m_crVideoText;
	COLORREF				m_crVideoBk;
	LOGFONTA				m_fontVideoOSD;
	XBitrate				m_XBitrateVideo;

	int						m_nScreenIndex;
	VIDEC_MPScreen*			m_pMPScreen;
};

#endif
