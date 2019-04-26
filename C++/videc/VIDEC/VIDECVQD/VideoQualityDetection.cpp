#include "stdafx.h"
#include "VideoQualityDetection.h"
#include "FunctionAlgorithm.h"
#include <VIDECVQD/VIDECVQDAPI.h>
#include <time.h>



VIDECVQD_API void*VIDECVQD_Open(unsigned long nDetectFlags)
{
	return new CVideoQualityDetection(nDetectFlags);
}

VIDECVQD_API void VIDECVQD_Close(void*pHandle)
{
	CVideoQualityDetection*pVQD=(CVideoQualityDetection*)pHandle;
	if (pVQD)
	{
		delete pVQD;
		pVQD=NULL;
	}
}

VIDECVQD_API unsigned long VIDECVQD_DetectFrame(void*pHandle, unsigned char *pFrameData, int nWidth, int nHeight)
{
	CVideoQualityDetection*pVQD=(CVideoQualityDetection*)pHandle;
	if (pVQD)
	{
		return pVQD->DetectFrame(pFrameData,nWidth,nHeight);
	}
	return VQDS_DONE;
}

static int g_nBrightThreshold=1;
static int g_nBlurThreshold=1;
static int g_nNoiseThreshold=1;
VIDECVQD_API void VIDECVQD_SetParams(unsigned long nDetectFlag,int nThreshold)
{
	if (nThreshold<1)
		nThreshold=1;
	else if (nThreshold>6)
		nThreshold=6;

	switch (nDetectFlag)
	{
	case VQDS_BRIGHT:
		{
			g_nBrightThreshold=nThreshold;
		}
		break;
	case VQDS_BLUR:
		{
			g_nBlurThreshold=nThreshold;
		}
		break;
	case VQDS_NOISE:
		{
			g_nNoiseThreshold=nThreshold;
		}
		break;
	}
}


CVideoQualityDetection::CVideoQualityDetection(unsigned long nDetectFlags)
:m_nDetectFlags(nDetectFlags)
{
	m_dblBrightThre = 1.0;			//默认值为1.0，最低值；
	m_dblBlurThre = 0.85;			//默认值为1.0，最低值；
	m_nNoiseThre = 2;			//默认值为2，	最低值；
	m_nFrameCount = 0;
	m_nDetectResult = 0;
	Flag_FD = 0;
	Frame_prev = NULL;

	for(int i=0;i<IMGAGENUM;i++)
	{
		m_grayImage[i] = NULL;
		rgbImage[i] = NULL;
	}
}

CVideoQualityDetection::~CVideoQualityDetection(void)
{
	for (int i=0;i<IMGAGENUM;i++)
	{
		if (m_grayImage[i])
		{
			cvReleaseImage(&m_grayImage[i]);
			m_grayImage[i]=NULL;
		}
		if (rgbImage[i])
		{
			cvReleaseImage(&rgbImage[i]);
			rgbImage[i]=NULL;
		}
	}
}


unsigned long CVideoQualityDetection::DetectFrame(unsigned char*pFrameData,int nWidth,int nHeight)
{
	char szDebug[64] = {0};
	sprintf(szDebug,"DetectFrame width=%d,height=%d\n",nWidth,nHeight);
	OutputDebugStringA(szDebug);

	int imgWidth,imgHeight;

	imgWidth = nWidth - 2*(nWidth>>3);
	imgHeight = nHeight - 2*(nHeight>>3);

	m_grayImage[m_nFrameCount] = cvCreateImage(cvSize(nWidth, nHeight),IPL_DEPTH_8U,1);
	rgbImage[m_nFrameCount] = cvCreateImage(cvSize(nWidth,nHeight), IPL_DEPTH_8U,3);

	m_dblBrightThre =  1.0+0.2*(g_nBrightThreshold - 1);		//亮度异常的阈值，值得范围是1.0~2.0；设6个等级，等级间距为0.2;
	m_dblBlurThre =	0.85 + 0.2*(g_nBlurThreshold - 1);			//清晰度异常的阈值，值的范围是0.85~2.0,设6个等级，实际值的范围0.85~1.85；
	m_nNoiseThre = 2 + 2*(g_nNoiseThreshold - 1);				//噪声异常的阈值，值的范围是2~12，设6个等级，等级间距为2;
	//判断越界；
	if (m_dblBrightThre<1.0)
		m_dblBrightThre = 1.0;
	else if (m_dblBrightThre>2.0)
		m_dblBrightThre = 2.0;

	if(m_dblBlurThre<0.85)
		m_dblBlurThre = 0.85;
	else if (m_dblBlurThre>2.0)
		m_dblBlurThre = 2.0;

	if(m_nNoiseThre<2)
		m_nNoiseThre =2;
	else if (m_nNoiseThre>12)
		m_nNoiseThre = 12;

	//BOOL bBlur = false;	
	
	//Byte2YUVImage(pFrameData,nWidth,nHeight,1,m_grayImage[m_nFrameCount],NULL,NULL);//格式转错
	//Byte2RGBImage(pFrameData, nWidth, nHeight, m_grayImage[m_nFrameCount], rgbImage[m_nFrameCount]);

	YUV2RGB(pFrameData, nWidth, nHeight, m_grayImage[m_nFrameCount], rgbImage[m_nFrameCount]);
	//cvShowImage("1", m_grayImage[m_nFrameCount]);
	//cvShowImage("2", rgbImage[m_nFrameCount]);
	//cvWaitKey(0);
	//cvSaveImage("temp.jpg", rgbImage[m_nFrameCount]);

	if(m_nFrameCount>=0)//检测第一帧时
	{
		if (m_nDetectFlags&VQDS_SIGNALLOST) //第一帧就检测信号是否缺失
		{
 			double signalLostRatio;
			float signallostra1;

   			if (VideoSignalLostDetect(rgbImage[m_nFrameCount],signalLostRatio, signallostra1))
   			{
   				m_nDetectResult = VQDS_SIGNALLOST;   //信号缺失，立即返回，不进行其他检测

				char szDebug1[64] = {0};
				sprintf(szDebug1,"DetectFrame nResult=%d\n",m_nDetectResult);
				OutputDebugStringA(szDebug1);

   				return m_nDetectResult;
   			}
 
 			if (ImageColorBarDiagnose(m_grayImage[m_nFrameCount]))
 			{
				m_nDetectResult = VQDS_SIGNALLOST;
				char szDebug1[64] = {0};
				sprintf(szDebug1,"DetectFrame nResult=%d\n",m_nDetectResult);
				OutputDebugStringA(szDebug1);
 				return m_nDetectResult;
 			}
 
 			if (ImageSnowDiagnose(rgbImage[m_nFrameCount]))
 			{
				m_nDetectResult = VQDS_SIGNALLOST;
				char szDebug1[64] = {0};
				sprintf(szDebug1,"DetectFrame nResult=%d\n",m_nDetectResult);
				OutputDebugStringA(szDebug1);
 				return m_nDetectResult;
 			}
		}
	}

	if ((m_nDetectFlags&VQDS_FREEZE) || (m_nDetectFlags&VQDS_BRIGHT) || (m_nDetectFlags&VQDS_JITTER) || (m_nDetectFlags&VQDS_COLORCAST) || 
		(m_nDetectFlags&VQDS_BLUR) || (m_nDetectFlags&VQDS_NOISE))//多帧&隔帧检测
	{
		if (m_nFrameCount % 3==0 && m_nFrameCount!=0)
		{
   			if (m_nDetectFlags&VQDS_BRIGHT)
   			{
   				if (ImageLuminanceDiagnose(rgbImage[m_nFrameCount]))
   				{
   					m_nDetectResult |= VQDS_BRIGHT;  					
   				}
   			}
 
 			if (m_nDetectFlags&VQDS_COLORCAST)
 			{
  				double clrCast_K,clrCast_D;
  				if (VideoColorCastDetectRGB(rgbImage[m_nFrameCount],clrCast_K,clrCast_D))
  				{
  						m_nDetectResult |= VQDS_COLORCAST;   //有问题，置该标志位为1
   				}
 			}
 
 			if (m_nDetectFlags&VQDS_BLUR)
 			{ 
 				if (ImageSharpnessDiagnose(rgbImage[m_nFrameCount]))
 				{
 					m_nDetectResult |= VQDS_BLUR;
  				}

				if (CoverDiagnose(rgbImage[m_nFrameCount]))
				{
					m_nDetectResult |= VQDS_BLUR;
				}
 			}

			if (m_nDetectFlags & VQDS_JITTER)
			{
				if (ScrollDiagnose(m_grayImage[m_nFrameCount]))
				{
					m_nDetectResult |= VQDS_JITTER;
				}
			}

			if (m_nDetectFlags & VQDS_NOISE)
			{
				if (ImageNoiseDiagnose(rgbImage[m_nFrameCount]))
				{
					m_nDetectResult |=VQDS_NOISE;   //有问题，置该标志位为1
				}

				if (StripeNoiseDiagnose(rgbImage[m_nFrameCount], m_grayImage[m_nFrameCount]))
				{
					m_nDetectResult |= VQDS_NOISE;
				}
			}
		}

		if(m_nFrameCount==IMGAGENUM-1)    //接满10帧，进行画面冻结、噪声、PTZ检测
		{
			if (m_nDetectFlags & VQDS_FREEZE)
			{
				if (FreezeDiagnoseInterface(m_grayImage, IMGAGENUM-1))
				{
					m_nDetectResult |= VQDS_FREEZE;
				}
			}

			char szDebug1[64] = {0};
			sprintf(szDebug1,"DetectFrame nResult=%d\n",m_nDetectResult);
			OutputDebugStringA(szDebug1);
			return m_nDetectResult;
		}
	m_nFrameCount++;

	return VQDS_DETECTING;    //不够10帧，返回-1，继续接收
	}
	else       //没有包含需要多帧的检测,只需第一帧即可完成检测任务
	{
		if (m_nDetectFlags&VQDS_BRIGHT)
		{
			if (ImageLuminanceDiagnose(rgbImage[m_nFrameCount]))
			{
				m_nDetectResult |= VQDS_BRIGHT;  					
			}
		}

		if (m_nDetectFlags&VQDS_COLORCAST)
		{
			double clrCast_K,clrCast_D;
			if (VideoColorCastDetectRGB(rgbImage[m_nFrameCount],clrCast_K,clrCast_D))
			{
				m_nDetectResult |= VQDS_COLORCAST;   //有问题，置该标志位为1
			}
		}

		if (m_nDetectFlags&VQDS_BLUR)
		{ 
			if (ImageSharpnessDiagnose(rgbImage[m_nFrameCount]))
			{
				m_nDetectResult |= VQDS_BLUR;
			}

			if (CoverDiagnose(rgbImage[m_nFrameCount]))
			{
				m_nDetectResult |= VQDS_BLUR;
			}
		}

		if (m_nDetectFlags&VQDS_NOISE)
		{
			if (ImageNoiseDiagnose(rgbImage[m_nFrameCount]))
			{
				m_nDetectResult |=VQDS_NOISE;   //有问题，置该标志位为1
			}

			if (StripeNoiseDiagnose(rgbImage[m_nFrameCount], m_grayImage[m_nFrameCount]))
			{
				m_nDetectResult |= VQDS_NOISE;
			}
		}

		if (m_nDetectFlags & VQDS_JITTER)
		{
			if (ScrollDiagnose(m_grayImage[m_nFrameCount]))
			{
				m_nDetectResult |= VQDS_JITTER;
			}
		}

		char szDebug1[64] = {0};
		sprintf(szDebug1,"DetectFrame nResult=%d\n",m_nDetectResult);
		OutputDebugStringA(szDebug1);

		return m_nDetectResult;
	}
}
