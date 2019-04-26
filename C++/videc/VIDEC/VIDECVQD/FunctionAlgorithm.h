#include "stdafx.h"
#include "opencv2/opencv.hpp"



bool VideoBlurDetect(const IplImage* rgb, double& ratio,double blurThre);
bool VideoColorCastDetectRGB(const IplImage* rgb,double& Kfactors,double& D);
// bool VideoColorCastDetectBYTE(BYTE* pFrameData,int nWidth,int nHeight,double& Kfactors,double& D);
// bool VideoColorCastDetectYUV(const IplImage* pY,const IplImage* pU,const IplImage* pV,int nWidth,int nHeight,double& Kfactors,double& D);

bool VideoSignalLostDetect(const IplImage* pYimage,double& ratio, float& ra1);
// bool VideoFreezeDetect(IplImage* grayImage[],int nImageNum);
// bool VideoNoiseDection(IplImage* grayImage[],int nImageNum,double NoiseRatioThre=0.1,double PSNR_Thre=40);
// bool VideoNoiseTimeDection(IplImage* grayImage[],int nImageNum,double NoiseRatioThre=0.15, double DiffThre = 10);
// bool VideoNoiseBreakupDection(IplImage* grayImage[],int nImageNum,double NoiseRatioThre=0.15, double DiffThre = 10);
// bool VideoNoisePointDection(const IplImage* pImage,double NoiseRatioThre=0.5);

// bool VideoBrightConstancyDection(IplImage* grayImage,float& LowRatio,float& HighRatio,bool bBlur, float RatioThre=0.8);
// bool VideoBrightDetect(IplImage* grayImage,float& LowRatio,float& HighRatio,bool bBlur, float RatioThre=0.8);
// bool VideoEqualBrightDetect(IplImage* grayImage,float& LowRatio,float& HighRatio, float RatioThre=0.8);

bool VideoJitterDection(IplImage* grayImage[],int nImageNum);

void RGB2Lab(const IplImage* rgb,float* lab);
void YUV2Lab(const IplImage* pY,const IplImage* pU,const IplImage* pV,double* lab);
bool theSameImage(const IplImage* current,const IplImage* prev);
// bool DiffDetect(IplImage* gray,IplImage* PrevImage,double& ratio,double DiffRatioThre);
void MotionEstimate(IplImage* current_gray, IplImage* prev_gray,double& shiftDist, int& oriIDX);
void bbPoints(CvPoint2D32f points[],int nNum,CvRect bb);
float median(std::vector<float> v);

void Byte2YUVImage(BYTE* pBuf/*YUV420 bytes*/,int nWidth,int nHeight,int nOutChannels,IplImage* pY,IplImage* pU=NULL,IplImage* pV=NULL);
// void Byte2ScaleYUVImage(BYTE* pBuf/*YUV420 bytes*/,int srcWidth,int srcHeight,int dstWidth, int dstHeight, int nOutChannels,IplImage* pY,IplImage* pU,IplImage* pV);
void Byte2RGBImage(BYTE* pFrameData/*YUV420 bytes*/,int nWidth,int nHeight,IplImage* Img_rgb);

int ImageStretchByHistogram(IplImage *src1,IplImage *dst1);
bool VideoBrightDetect_EquGamma(IplImage* Oridata, double& darkRatio,double& distRatio, double distThre,double darkThre=0.95);

bool NoiseDetection_RGB(IplImage* grayImage[],int nImageNum, int nNoiseThre);

bool DiffRGBDetect(IplImage* pCurrent,IplImage* PrevImage, int nNoiseThre);

void YUV2RGB(BYTE* pFrameData,int nWidth,int nHeight, IplImage*Img_gray, IplImage* Img_rgb);
IplImage* SobelConvert(IplImage* Img_Src, int xorder, int yorder, int aperture_size);
IplImage* GradientConvert(IplImage* Img);
bool ImageColorBarDiagnose(IplImage* Img_gray);
bool ImageSnowDiagnose(IplImage* Img);
bool ImageLuminanceDiagnose(IplImage* Img);
bool ImageSharpnessDiagnose(IplImage* Img);
bool FreezeDiagnose(IplImage* Img_prev, IplImage* Img_curt);
bool FreezeDiagnoseInterface(IplImage* grayImage[],int nImageNum);
float AdjacentRegionAveraging(IplImage* Img, int i, int j, int width, int height);
IplImage* ConsecutivePointsFilter(IplImage* Img_Src);
bool ImageNoiseDiagnose(IplImage* Img);
int StripeSeek(IplImage* Img, int Thrd_0, int Thrd_1, int Thrd_2);
bool StripeNoiseDiagnose(IplImage* Img_RGB, IplImage* Img_gray);
bool ScrollDiagnose(IplImage* Img);
bool CoverDiagnose(IplImage* Img);

