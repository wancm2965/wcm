//VIDEC_DVCDev.h
#ifndef __VIDEC_DVCDEV_H__
#define __VIDEC_DVCDEV_H__

#include "VIDEC.h"

//DVC�ɼ�Դ
class VIDEC_API VIDEC_DVCDev
	: public VIDEC_CapDev
	, public VIDEC_StreamSource
{
public:
	VIDEC_DVCDev(void){}
	virtual~VIDEC_DVCDev(void){};
public:
	//���豸
	//nDevID�豸�ţ���0��ʼ
	//nWidth,ͼ����
	//nHeight,ͼ��߶�
	//nFrameRate,֡�ʣ�1-30
	//bInterlaced,�Ƿ����ɨ��
	//�ɹ�����0��ʧ�ܷ���-1
	virtual int Open(int nDevID,int nFrameRate,int nBitrate,int nBitrateControlType,int nResolusion)=0;
	//�ر�����
	virtual void Close(void)=0;
	//�����豸
	//nDevID�豸�ţ���0��ʼ
	//nWidth,ͼ����
	//nHeight,ͼ��߶�
	//nFrameRate,֡�ʣ�1-30
	//bInterlaced,�Ƿ����ɨ��
	//�ɹ�����0��ʧ�ܷ���-1
	virtual int Reset(int nDevID,int nFrameRate,int nBitrate,int nBitrateControlType,int nResolusion)=0;

	virtual int SetVideoParam(int nBrightness=128/*����ֵ0-255*/, int nContrast=64/*�Աȶ�0-127*/, int nSaturation=64/*���Ͷ�0-127*/, int nHue=0/*ɫ��0-255*/,int nGamma=100/*٤��1-400*/,int nSharpen=0/*��0-127*/)=0;
 	
	virtual int RequestKeyFrame(void)=0;

	//int nGammaR[1-400 100]
	//int nGammaG[1-400 100]
	//int nGammaB[1-400 100]
	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)=0;

	virtual void SetLevelFixer(int nLevelFixer)=0;
	virtual int GetLevelFixer(void)=0;
 
	//�����ӿ�
	static VIDEC_DVCDev*Create(void);

	//ȡ�豸����
	static int GetDevCount(void);
	//ȡ�豸����
	static int GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount);
	static int GetDevType(unsigned int nDevIndex,VIDEC_DEV_TYPE&nDevType);
	static bool HasAudioInput(unsigned int nDevIndex);
	static bool SupportHardwareEncoding(unsigned int nDevIndex);


	//�г��ɲü��ĸ�ʽ�ĸ���
	static int GetFormatCount(void);
	//ȡ��ʽ����
	static int GetFormatName(int nIndex,char*szName,int nMaxCount);
	//ȡ��ʽ��С
	static int GetFormatSize(int nIndex,int&nWidth,int&nHeight);
	//example
	//int nCount=VIDEC_DVCDev::GetFormatCount();
	//for (int i=0;i<nCount;i++)
	//{
	//	char szName[128];
	//	VIDEC_DVCDev::GetFormatName(i,szName,127);
	//	int nWidth=0;
	//	int nHeight=0;
	//	VIDEC_DVCDev::GetFormatSize(i,nWidth,nHeight);
	//}
	static void SetIP(const char*cszIP,unsigned short nPort,const char*cszPassword);

	virtual int SendData(const char*pData,int nLen)=0;
};

#endif