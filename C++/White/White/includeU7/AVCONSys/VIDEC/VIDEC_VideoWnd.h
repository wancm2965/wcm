//VIDEC_VideoWnd.h
#ifndef __VIDEC_VIDEOWND_H__
#define __VIDEC_VIDEOWND_H__

#include <winsock2.h>
#include <windows.h>

#include <VIDEC/VIDEC_Export.h>

class VIDEC_API VIDEC_VideoWnd
{
public:
	VIDEC_VideoWnd(void){};
	virtual~VIDEC_VideoWnd(void){};
public:
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

	//���ֵ�ǰͼ��BMP�ļ�
	virtual bool CaptureToBMPFile(const char*cszFile)=0;

	virtual double GetAverageFPS(void)=0;

	//������Ƶ����
	virtual int SetVideoParam(int nBrightness=128/*����ֵ0-255*/, int nContrast=64/*�Աȶ�0-127*/, int nSaturation=64/*���Ͷ�0-127*/, int nHue=0/*ɫ��0-255*/,int nGamma=100/*٤��1-400*/,int nSharpen=0/*��0-127*/)=0;
	//int nGammaR[1-400 100]
	//int nGammaG[1-400 100]
	//int nGammaB[1-400 100]
	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)=0;
	//����ͼ����ʾģʽ

	/******************************************************************************
	* SetScreenWnd
	*����������������Ļ����λ��
	*���룺	nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;

	virtual void EnablePreview(int nPreview)=0;
};

#endif