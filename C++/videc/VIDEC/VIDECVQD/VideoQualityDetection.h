#include "stdafx.h"
#include "opencv2/opencv.hpp"
/*#include <ctime>*/
#include <VIDECVQD/VIDECVQDAPI.h>
#define  IMGAGENUM   10
//===========================================
//����˵��:
//pScoureData��YUV֡���ݣ������˶�֡��֡����nImageNum��ͬ��
//nWidth: ͼ��֡�Ŀ�
//nHeight: ͼ��֡�ĸߣ�
//nImageNum�������ͼ��֡�������10֡���ң�
//===========================================
class  CVideoQualityDetection
{
public:
	CVideoQualityDetection(unsigned long nDetectFlags);
	~CVideoQualityDetection(void);
public:
	unsigned long DetectFrame(unsigned char*pFrameData,int nWidth,int nHeight);
	void		VQDS_StopDetect();
private:
	unsigned long			m_nDetectFlags;
	double						m_dblBlurThre;						//�������쳣����ֵ��ֵ�ķ�Χ��0.85~2.0��
	double						m_dblBrightThre;					//�����쳣����ֵ��ֵ�÷�Χ��1.0~2.0��
	int								m_nNoiseThre;					//�����쳣����ֵ��
	unsigned long			m_nDetectResult;
	int						m_nFrameCount;
	int Flag_FD;
	IplImage* Frame_prev;
	IplImage *				m_grayImage[IMGAGENUM]; 
	IplImage *              rgbImage[IMGAGENUM];
	unsigned char*      pFrameData[IMGAGENUM];
	
};
