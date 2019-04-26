//VIDECVQDAPI.h
#ifndef __VIDECVQDAPI_H__
#define __VIDECVQDAPI_H__

#ifdef VIDECVQD_EXPORT
#define VIDECVQD_API _declspec(dllexport)
#elif VIDECVQD_DLL
#define VIDECVQD_API _declspec(dllimport)
#else
#define VIDECVQD_API
#endif

#define VQDS_DONE			0x00000000
#define VQDS_SIGNALLOST		0x00000001		//�ź�ȱʧ��
#define VQDS_COLORCAST		0x00000002			//ƫɫ��
#define VQDS_BLUR			0x00000004					//�������쳣��
#define VQDS_BRIGHT			0x00000008				//�����쳣��
#define VQDS_FREEZE			0x00000010				//���涳�᣻
#define VQDS_NOISE			0x00000020					//���������ţ�
#define VQDS_JITTER			0x00000040				//������������
#define VQDS_DETECTING		0xFFFFFFFF

VIDECVQD_API void*VIDECVQD_Open(unsigned long nDetectFlags);	//��һ��������϶���
VIDECVQD_API void VIDECVQD_Close(void*pHandle);		//�رմ򿪵��Ǹ�������϶���
VIDECVQD_API unsigned long VIDECVQD_DetectFrame(void*pHandle, unsigned char *pFrameData, int nWidth, int nHeight);	//֡��⺯����

/******************************************************************************************/
/*Ŀǰʵ����nDetectFlag=��VQDS_BRIGHT��VQDS_BLUR��VQDS_NOISE���ķ�Χ����nThreshold=��1-6��*/
/******************************************************************************************/
VIDECVQD_API void VIDECVQD_SetParams(unsigned long nDetectFlag,int nThreshold);	//��ֵ����������,��Χ1-6���ں���VIDECVQD_DetectFrame֮ǰ���ã�
#endif