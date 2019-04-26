//VIDEC_CapDev.h
#ifndef __VIDEC_CAPDEV_H__
#define __VIDEC_CAPDEV_H__

#include <winsock2.h>
#include <windows.h>

#include <VIDEC_WCE/VIDEC.h>
#include <VIDEC_WCE/VIDEC_CodecDef.h>

#define VIDEC_BITRATE_CONTROL_VBR		0			//可变码流
#define VIDEC_BITRATE_CONTROL_CBR		1			//恒定码流
#define VIDEC_BITRATE_CONTROL_CQP		2			//固定量化
#define VIDEC_BITRATE_CONTROL_QUALITY	0x80000000	//是否质量优先

class VIDEC_CapDevCallback
{
public:
	VIDEC_CapDevCallback(void){};
	virtual~VIDEC_CapDevCallback(void){};
public:
	virtual void OnVIDEC_CapDevCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)=0;
	virtual void OnVIDEC_CapDevCallbackRTPPacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)=0;
};

class VIDEC_API VIDEC_CapDev
{
public:
	VIDEC_CapDev(void){};
	virtual~VIDEC_CapDev(void){};
public:
	//RTP模式
	//视频大小支持CIF[352X288],QCIF[176X144]
	//视频编码支持H263
	virtual int Open(HWND hWnd,int nWidth,int nHeight,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nCodecType)=0;
	//私有模式
	virtual int Open(HWND hWnd,int nWidth,int nHeight,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nCodecType,unsigned long nBitrateControlType)=0;
	virtual void Close(void)=0;
	virtual void SetWndPos(int x,int y,int cx,int cy)=0;
	virtual int GetWidth(void)=0;
	virtual int GetHeight(void)=0;
	virtual HWND GetWnd(void)=0;

	static VIDEC_CapDev*Create(VIDEC_CapDevCallback&rCallback);
};

#endif