//CapChan.h
#ifndef __CAPCHAN_H__
#define __CAPCHAN_H__

#include <IMXEC/IMXEC_MediaChannel.h>
#include <NETEC/NETEC_MediaSender.h>
#include "XBitrate.h"
#include "string"

class CapChan
	: public NETEC_MediaSenderCallback
	, public VIDEC_WndCallback
{
public:
	CapChan(IMXEC_CapChanCallback&rCallback);
	virtual~CapChan(void);
public:
	//��ͨ��
	virtual int Open(HWND hwndParent,int nOnlyPreview);
	//�ر�ͨ��
	virtual void Close(void);

	virtual void EnablePreview(int nPreview);

	//ȡ��Ƶ����
	virtual HWND GetVideoWnd(void);
	//ȡ��Ƶ���
	virtual int GetVideoWidth(void);
	//ȡ��Ƶ�߶�
	virtual int GetVideoHeight(void);

	//same as CWnd::IsWindowVisible
	virtual bool IsWindowVisible(void);
	//same as CWnd::SetWindowPos
	virtual bool SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags); 
	//���ֵ�ǰͼ��BMP�ļ�
	virtual bool CaptureToBMPFile(const char*cszFile);

	virtual int SetVolume(int nVolume);
	virtual int GetLevel(int&nLevel);

	virtual unsigned long GetAudioID(void);
	virtual unsigned long GetVideoID(void);

	virtual void OnNETEC_MediaSenderCallbackRecvdPTZ(void*pData,int nLen){};
	virtual void OnNETEC_MediaSenderCallbackRecvdCtrl(void*pData,int nLen);

	virtual void OnVIDEC_WndCallbackDrawD3D9DC(void* hDC/*HDC*/);

	virtual int SendAudio(unsigned char*pData,int nLen);
	virtual int SendMainVideo(unsigned char*pData,int nLen);
	virtual int SendSubVideo(unsigned char*pData,int nLen);
	virtual int SendQSubVideo(unsigned char*pData,int nLen);

	/******************************************************************************
	* GetVideoPacketStatistics
	*��������ȡ��Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetAudioPacketStatistics
	*��������ȡ��Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);
	virtual unsigned long DoGetTotalBytes(void);
	virtual int GetVideoShowType(void);
	virtual int GetVideoShowFrameRate(void);
	virtual int GetVideoCaptureFrameRate(void);
	virtual int GetVideoEncodeFrameRate(void);
	virtual int GetVideoSendFrameRate(void);

	void ReSetFrameRate(void);
protected:
	IMXEC_CapChanCallback&	m_rCallback;
	VIDEC_Wnd*				m_pWnd;
	NETEC_MediaSender*		m_pMediaSender;
	HWND					m_hWndParent;
	int						m_nOnlyPreview;
	std::string             m_strVideoWndBkImage;
	int                     m_nVideoWnBkR;
	int                     m_nVideoWnBkG;
	int                     m_nVideoWnBkB;

	XBitrate				m_XBitrateAudio;
	XBitrate				m_XBitrateVideo;

	unsigned long			m_nTotalBytes;

	int						m_nSendFrameRate;
	unsigned long			m_ulLastCountFrameTimestamp;
	unsigned long			m_nSendFrameRateCount;
};


#endif
