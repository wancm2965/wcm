//VIDEC_Player.h
#ifndef __VIDEC_PLAYER_H__
#define __VIDEC_PLAYER_H__


#include <winsock2.h>
#include <windows.h>
#include <VIDEC/VIDEC_Export.h>
#include <VIDECVQD/VIDECVQDAPI.h>

class VIDEC_PlayerCallback
{
public:
	VIDEC_PlayerCallback(void){}
	virtual~VIDEC_PlayerCallback(void){};
public:
	virtual void OnVIDEC_PlayerCallbackPlayData(void*pPacketData,int nPacketSize){};
	virtual void OnVIDEC_PlayerCallbackVQDResult(unsigned long nResult){};
};

class VIDEC_API VIDEC_Player
{
public:
	VIDEC_Player(void){};
	virtual~VIDEC_Player(void){};
public:
	//����ʾ����
	//hWndParent����ʾ���ڵĸ����ھ��
	//�ɹ�����0��ʧ�ܷ���-1
	virtual int Open(HWND hWndParent,unsigned long*lpnVideoTimestamp=NULL,unsigned long*lpnAudioTimestamp=NULL)=0;
	//�ر���ʾ����
	virtual void Close(void)=0;

	//ȡ��Ƶ���
	virtual int GetVideoWidth(void)=0;
	//ȡ��Ƶ�߶�
	virtual int GetVideoHeight(void)=0;

	//ȡ��Ƶ����
	virtual HWND GetWnd(void)=0;
	//same as CWnd::IsWindowVisible
	virtual bool IsWindowVisible(void)=0;
	//same as CWnd::SetWindowPos
	virtual bool SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags)=0; 

	virtual double GetAverageFPS(void)=0;

	virtual void SetSynFlag(int nSynFlag)=0;

	virtual bool HasVideo(void)=0;

	virtual int PlayData(void*pPacketData,int nPacketSize)=0;

	//���ֵ�ǰͼ��BMP�ļ�
	virtual bool CaptureToBMPFile(const char*cszFile)=0;
	
	//������Ƶ����
	virtual int SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)=0;

	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)=0;
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;
	virtual VIDEC_Image*GetImage(void)=0;

	static VIDEC_Player*Create(VIDEC_PlayerCallback&rCallback);
	static void SetMaxDXVAPlayers(unsigned int nCount);

	virtual int GetSupportPreview(void)=0;
	virtual void SetEnableVideo(int nEnable)=0;
	virtual int GetEnableVideo(void)=0;

	virtual int StartVQD(unsigned long nDetectFlags=VQDS_SIGNALLOST|VQDS_COLORCAST|VQDS_BLUR|VQDS_BRIGHT|VQDS_FREEZE|VQDS_NOISE|VQDS_JITTER)=0;
	virtual void StopVQD(void)=0;

	virtual void ResetImage(VIDEC_Image*pImage)=0;
};

#endif