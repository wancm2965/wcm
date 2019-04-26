#include "stdafx.h"
#include "opencv2/opencv.hpp"
/*#include <ctime>*/
#include <VIDECVQD/VIDECVQDAPI.h>
#define  IMGAGENUM   10
//===========================================
//参数说明:
//pScoureData：YUV帧数据，包含了多帧，帧数和nImageNum相同；
//nWidth: 图像帧的宽；
//nHeight: 图像帧的高；
//nImageNum：传入的图像帧数，大概10帧左右；
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
	double						m_dblBlurThre;						//清晰度异常的阈值，值的范围是0.85~2.0；
	double						m_dblBrightThre;					//亮度异常的阈值，值得范围是1.0~2.0；
	int								m_nNoiseThre;					//噪声异常的阈值；
	unsigned long			m_nDetectResult;
	int						m_nFrameCount;
	int Flag_FD;
	IplImage* Frame_prev;
	IplImage *				m_grayImage[IMGAGENUM]; 
	IplImage *              rgbImage[IMGAGENUM];
	unsigned char*      pFrameData[IMGAGENUM];
	
};
