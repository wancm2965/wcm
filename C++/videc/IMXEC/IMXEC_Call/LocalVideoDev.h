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
	*���������ñ���������Ƶ����
	*���룺	nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetVideoEnableOSD
	*���������ñ���������Ƶ�Ƿ�����OSD
	*���룺	cszCallID		-���б�ʶ
			nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetVideoEnableOSD(int nEnable);

	/******************************************************************************
	* SetVideoOSDText
	*���������ñ���������ƵOSD����
	*���룺	cszCallID		-���б�ʶ
			cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetVideoOSDText(const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetVideoOSDFont
	*���������ñ���������ƵOSD����,�ֵ���ɫ�ͱ���ɫ
	*���룺	logfont			-OSD����
			crText			-�ֵ���ɫ
			crBk			-����ɫ
	*�������
	*����ֵ����
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
