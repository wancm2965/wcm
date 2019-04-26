//libXPreProc.h
#ifndef __LIBXPREPRO_H__
#define __LIBXPREPRO_H__

#include <winsock2.h>
#include <windows.h>
#include "VIDEC.h"

class IXPreProcessNotify
{
public:
	IXPreProcessNotify(void){};
	virtual~IXPreProcessNotify(void){};
public:
	virtual void OnIXPreProcessNotifyOutputData(VIDEC_Image*pImage)=0;
};

class IXPreProcess
{
public:
	IXPreProcess(void){};
	virtual~IXPreProcess(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual void PreProcess(VIDEC_Image*pImage)=0;
	virtual void EnableI420(bool bEnable)=0;
	virtual bool IsI420Enable(void)=0;
	virtual void EnableInvert(bool bEnable)=0;
	virtual bool IsInvertEnable(void)=0;
	virtual void EnableDenoise(bool bEnable)=0;
	virtual bool IsDenoiseEnable(void)=0;

	virtual void EnableInterlaced(bool bEnable)=0;
	virtual bool IsInterlacedEnable(void)=0;
	virtual void EnableDeinterlaced(bool bEnable,bool bDoubleFrameRate=false)=0;
	virtual bool IsDeinterlacedEnable(void)=0;

	//�������ڲ���������
	virtual bool SetupMask(RECT *RectList, int iRectCount)=0;
	//ֹͣ����
	virtual bool StopMask(void)=0;
	
	virtual void SetPictProp(int nBrightness/*����ֵ0-255*/, int nContrast/*�Աȶ�0-127*/, int nSaturation/*���Ͷ�0-127*/, int nHue/*ɫ��0-255*/,int nGamma/*٤��1-400*/,int nSharpen=0/*��0-127*/)=0;
	virtual void SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)=0;

	//��ʾOSD
	virtual void EnableOSD(bool bEnable)=0;
	//�Ƿ���ʾOSD  
	virtual bool IsOSDEnable(void)=0;
	//����OSD 
	virtual void SetOSDText(const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT,int nTop=-1,int nLeft=-1)=0;//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT
	//����OSD������,�ֵ���ɫ�ͱ���ɫ
	virtual void SetOSDFont(LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255))=0;
};

IXPreProcess*CreateIXPreProcess(IXPreProcessNotify&rIXPreProcessNotify);


#endif