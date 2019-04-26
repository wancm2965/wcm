// VideoQualityDetection.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FunctionAlgorithm.h"
#include "opencv2/opencv.hpp"
#include "../include/opencv2/legacy/compat.hpp"

 #include <fstream>
 #include <iomanip>
 using namespace std;

#define  Uint8 unsigned char

 IplImage* SobelConvert(IplImage* Img_Src, int xorder, int yorder, int aperture_size)
 {
	 IplImage* Img_Gray = cvCreateImage(cvGetSize(Img_Src), IPL_DEPTH_8U, 1);
	 IplImage* Img_Tmp = cvCreateImage(cvGetSize(Img_Src), IPL_DEPTH_16S, 1);
	 IplImage* Img_Sobel = cvCreateImage(cvGetSize(Img_Src), IPL_DEPTH_8U, 1);

	 cvCvtColor(Img_Src, Img_Gray, CV_BGR2GRAY);

	 cvSobel(Img_Gray, Img_Tmp, xorder, yorder, aperture_size);

	 cvConvertScaleAbs(Img_Tmp, Img_Sobel, 1, 0);

	 cvReleaseImage(&Img_Gray);
	 cvReleaseImage(&Img_Tmp);

	 return Img_Sobel;
 }

 IplImage* GradientConvert(IplImage* Img)
 {
	 IplImage* Img_Sob_Hor = cvCreateImage(cvGetSize(Img), IPL_DEPTH_8U, 1);
	 IplImage* Img_Sob_Vet = cvCreateImage(cvGetSize(Img), IPL_DEPTH_8U, 1);
	 IplImage* Img_Sum = cvCreateImage(cvGetSize(Img), IPL_DEPTH_32F, 1);
	 IplImage* Img_grad = cvCreateImage(cvGetSize(Img), IPL_DEPTH_8U, 1);

	 Img_Sob_Hor = SobelConvert(Img, 1, 0, 3);
	 Img_Sob_Vet = SobelConvert(Img, 0, 1, 3);

	 cvZero(Img_Sum);

	 cvAcc(Img_Sob_Hor, Img_Sum);
	 cvAcc(Img_Sob_Vet, Img_Sum);

	 cvConvertScaleAbs(Img_Sum, Img_grad, 1, 0);

	 cvReleaseImage(&Img_Sob_Hor);
	 cvReleaseImage(&Img_Sob_Vet);
	 cvReleaseImage(&Img_Sum);

	 return Img_grad;
 }

 void RGB2Lab(const IplImage* rgb,double* lab)
 {
	 int width = rgb->width;
	 int height = rgb->height;

	 double R,G,B;
	 double X,Y,Z,fx,fy,fz,L,a,b;

	 double T = 0.008856;
	 int idx;

	 uchar* data = (uchar*)rgb->imageData;
	 for (int i=0;i<height;i++)
	 {
		 for (int j=0;j<width;j++)
		 {
			 idx = i*rgb->widthStep+rgb->nChannels*j;
			 B = 1.0*data[idx];
			 G = 1.0*data[idx+1];
			 R = 1.0*data[idx+2];
			 // RGB -> sRGB -> XYZ  
			 R /= 255.0f;  
			 G /= 255.0f;  
			 B /= 255.0f; 

			 X = 0.412453f*R + 0.357580f*G + 0.180423f*B;  
			 Y = 0.212671f*R + 0.715160f*G + 0.072169f*B;  
			 Z = 0.019334f*R + 0.119193f*G + 0.950227f*B; 

			 //XYZ -> Lab
			 X /=  0.950456;
			 Z /=  1.088754;

			 if (X>T)
			 {
				 fx = pow(X,1.0/3.0);
			 }
			 else
			 {
				 fx = 7.787*X + 16.0/116.0;
			 }

			 if (Y>T)
			 {
				 fy = pow(Y,1.0/3.0);

				 L = 116 * pow(Y,1.0/3.0) - 16.0;
			 }
			 else
			 {
				 fy = 7.787*Y + 16.0/116.0;
				 L  = 903.3 * Y;
			 }

			 if (Z>T)
			 {
				 fz = pow(Z,1.0/3.0);
			 }
			 else
			 {
				 fz = 7.787*Z + 16.0/116.0;
			 }

			 a = 500 * (fx - fy);
			 b = 200 * (fy - fz);

			 lab[i*width*3+3*j] = L;
			 lab[i*width*3+3*j+1] = a;
			 lab[i*width*3+3*j+2] = b;

		 }
	 }

 }

 void YUV2Lab(const IplImage* pY,const IplImage* pU,const IplImage* pV,double* lab)
 {
	 int width = pY->width;
	 int height = pY->height;

	 double R,G,B;
	 double X,Y,Z,fx,fy,fz,L,a,b;

	 double T = 0.008856;
	 int idxY,idxU,idxV;
	 short temp;

	 uchar* pYData = (uchar*)pY->imageData;
	 uchar* pUData =  (uchar*)pU->imageData;
	 uchar* pVData =  (uchar*)pV->imageData;

	 for (int i=0;i<height;i++)
	 {
		 for (int j=0;j<width;j++)
		 {
			 idxY = i*pY->widthStep + j;
			 idxU = (int)((i/2)*pU->widthStep + j/2);
			 idxV = (int)((i/2)*pV->widthStep + j/2);
			 unsigned char y = pYData[idxY];
			 unsigned char u = pUData[idxU];
			 unsigned char v = pVData[idxV];
			 temp  = (short)(1.0 * y + 1.772 * (u - 128)) ;
			 B = (uchar)(temp < 0 ? 0:((temp > 255)? 255:temp)); //B

			 temp  = (short)(1.0 * y - 0.34413 * (u - 128) - 0.71414 * (v - 128)); 
			 G =(uchar)(temp < 0 ? 0:((temp > 255) ? 255:temp));// G

			 temp  = (short)(1.0 * y + 0 +1.402 * (v - 128));
			 R = (uchar)(temp < 0 ? 0:((temp > 255) ? 255:temp));// R

			 // 			B = 1.0*data[idx];
			 // 			G = 1.0*data[idx+1];
			 // 			R = 1.0*data[idx+2];
			 // RGB -> sRGB -> XYZ  
			 R /= 255.0f;  
			 G /= 255.0f;  
			 B /= 255.0f; 

			 X = 0.412453f*R + 0.357580f*G + 0.180423f*B;  
			 Y = 0.212671f*R + 0.715160f*G + 0.072169f*B;  
			 Z = 0.019334f*R + 0.119193f*G + 0.950227f*B; 

			 //XYZ -> Lab
			 X /=  0.950456;
			 Z /=  1.088754;

			 if (X>T)
			 {
				 fx = pow(X,1.0/3.0);
			 }
			 else
			 {
				 fx = 7.787*X + 16.0/116.0;
			 }

			 if (Y>T)
			 {
				 fy = pow(Y,1.0/3.0);

				 L = 116 * pow(Y,1.0/3.0) - 16.0;
			 }
			 else
			 {
				 fy = 7.787*Y + 16.0/116.0;
				 L  = 903.3 * Y;
			 }

			 if (Z>T)
			 {
				 fz = pow(Z,1.0/3.0);
			 }
			 else
			 {
				 fz = 7.787*Z + 16.0/116.0;
			 }

			 a = 500 * (fx - fy);
			 b = 200 * (fy - fz);

			 lab[i*width*3+3*j] = L;
			 lab[i*width*3+3*j+1] = a;
			 lab[i*width*3+3*j+2] = b;
		 }
	 }

 }

 bool VideoSignalLostDetect(const IplImage* pYimage,double& ratio, float& ra1)
 {
	 bool bSignalLost=false;

	 IplImage* Img_gray = cvCreateImage(cvGetSize(pYimage), IPL_DEPTH_8U, 1);

	 cvCvtColor(pYimage, Img_gray, CV_BGR2GRAY);
	 //cvSaveImage("gray.jpg", Img_gray);

	 //cvShowImage("ori", pYimage);
	 //cvShowImage("gray", Img_gray);
	 //cvWaitKey(0);

	 long Hist[256];
	 int idx=0;
	 for (int i=0;i<256;i++)
	 {
		 Hist[i]=0;
	 }

	 for (int i=0;i<Img_gray->height;i++)
	 {
		 for (int j=0;j<Img_gray->width;j++)
		 {
			 idx = i*Img_gray->widthStep+Img_gray->nChannels*j;
			 uchar Y = Img_gray->imageData[idx];
			 Hist[Y]++;
		 }
	 }

	 int darkSum=0;
	 for (int i=0;i<55;i++)
	 {
		 darkSum += Hist[i];
	 }

	 long maxH=0,sumH=0;
	 int idx_max=0;
	 for (int i=0;i<256;i++)
	 {
		 if(Hist[i]>maxH)
		 {
			 maxH = Hist[i];
			 idx_max = i;
		 }

		 //sumH += Hist[i];
	 }

	 sumH = Img_gray->height*Img_gray->width;

	 ra1 = darkSum*1.0/sumH;
	 ratio = maxH*1.0/(darkSum+0.00001);
	 if (ratio>0.7 && ra1>0.5)
	 {
		 //printf("信号丢失（黑屏或蓝屏） %.4f \n",ratio);
		 bSignalLost = true;
	 }
	 else
	 {
		 //printf("信号正常 %.4f \n",ratio);
		 bSignalLost = false;
	 }

	 cvReleaseImage(&Img_gray);

	 return bSignalLost;
 }

 bool ImageColorBarDiagnose(IplImage* Img_gray)
 {
	 int i, j;
	 int i_sta=0, i_end=0, j_sta=0, j_end=0;
	 int count=0, count_bar=0, count_tmp=0;;
	 int pixel_bar[256];
	 int val_pixel[256];
	 int bar_width[256];

	 j = Img_gray->height/5;

	 for (i=0; i<256; i++)
	 {
		 pixel_bar[i] = 0;
		 val_pixel[i] = 0;
		 bar_width[i] = 0;
	 }

	 for (i=0; i<Img_gray->width; i++)
	 {
		 if (i<Img_gray->width-1 && (CV_IMAGE_ELEM(Img_gray, uchar, j, i)==CV_IMAGE_ELEM(Img_gray, uchar, j, i+1)))
		 {
			 pixel_bar[CV_IMAGE_ELEM(Img_gray, uchar, j, i)]++;
		 }
		 else if (pixel_bar[CV_IMAGE_ELEM(Img_gray, uchar, j, i)]>=Img_gray->width/10 && 
			 pixel_bar[CV_IMAGE_ELEM(Img_gray, uchar, j , i)]<Img_gray->width/3)
		 {
			 val_pixel[count_tmp] = CV_IMAGE_ELEM(Img_gray, uchar, j, i);
			 bar_width[count_tmp] = pixel_bar[CV_IMAGE_ELEM(Img_gray, uchar, j, i)];

			 if (i<Img_gray->width-1 && count_tmp>0)
			 {
				 if (/*(CV_IMAGE_ELEM(Img_gray, uchar, j, i+1)==CV_IMAGE_ELEM(Img_gray, uchar, j, i+2)) && */
					 (abs(val_pixel[count_tmp]-val_pixel[count_tmp-1])>10) && (abs(bar_width[count_tmp]-bar_width[count_tmp-1])<10))
				 {
					 count++;

					 count_tmp++;

					 pixel_bar[CV_IMAGE_ELEM(Img_gray, uchar, j, i)] = 0;
				 }
				 else
				 {
					 count_tmp++;

					 pixel_bar[CV_IMAGE_ELEM(Img_gray, uchar, j, i)] = 0;
				 }
			 }
			 else
			 {
				 count_tmp++;

				 pixel_bar[CV_IMAGE_ELEM(Img_gray, uchar, j, i)] = 0;
			 }
		 }
	 }

	 if (count >= 3)
	 {
		 for (i=0; i<count_tmp-1; i++)
		 {
			 for (j=i+1; j<count_tmp; j++)
			 {
				 if (val_pixel[i] == val_pixel[j])
				 {
					 return false;
				 }
			 }
		 }

		 return true;
	 }
	 else
	 {
		 return false;
	 }
 }

 bool ImageSnowDiagnose(IplImage* Img)
 {
	 CvScalar scalar;
	 float Val_avg_grad;

	 IplImage* Img_Grad = cvCreateImage(cvGetSize(Img), IPL_DEPTH_8U, 1);

	 Img_Grad = GradientConvert(Img);
	 //cvShowImage("Gradient", Img_Grad);

	 scalar = cvAvg(Img_Grad);

	 Val_avg_grad = scalar.val[0];

	 cvReleaseImage(&Img_Grad);

	 if (Val_avg_grad >= 200.0)
	 {
		 return true;
	 }
	 else
	 {
		 return false;
	 }	
 }

 bool ImageLuminanceDiagnose(IplImage* Img)
{
	float global_thrd_B = 190.0;
	float global_thrd_D = 60.0;
	float local_thrd_B = 210.0;
	float local_thrd_D = 50.0;
	CvScalar Temp;
	CvRect rect;
	float Val_lum;
	float Arr_lum[9];	
	IplImage* Img_HSV = cvCreateImage(cvGetSize(Img), Img->depth, Img->nChannels);
	IplImage* Img_V = cvCreateImage(cvGetSize(Img), Img->depth, 1);
	int i, j, width, height;
	int a=0;
	int count_B=0, count_D=0/*, count_SL=0*/;

	IplImage* Img_cpy = cvCloneImage(Img);

	cvCvtColor(Img, Img_HSV, CV_BGR2HSV);

	cvSplit(Img_HSV, NULL, NULL, Img_V, NULL);

	Temp = cvAvg(Img_V, 0);
	
	Val_lum = Temp.val[0];

#ifdef DEBUG_ALGORITHEM_MODEL
	cout<<"Image Luminance:"<<Val_lum<<endl;
#endif
	width = Img->width/3;
	height= Img->height/3;

	for (j=0; j<2*Img->height/3; j+=Img->height/3-1)
	{
		for (i=0; i<2*Img->width/3; i+=Img->width/3-1)
		{
			rect = cvRect(i, j, width, height);

#ifdef DEBUG_ALGORITHEM_MODEL
			cvRectangle(Img_cpy, cvPoint(i, j), cvPoint(width+i, height+j), CV_RGB(255,0,0), 1, 8, 0);
#endif
			cvSetImageROI(Img_V, rect);

			IplImage* Img_tmp = cvCloneImage(Img_V);
			/*cvShowImage("Area", Img_tmp);
			cvWaitKey(0);*/

			cvResetImageROI(Img_V);

			Temp = cvAvg(Img_tmp, 0);

			Arr_lum[a] = Temp.val[0];

#ifdef DEBUG_ALGORITHEM_MODEL
			//cout<<"Area-"<<a<<" : "<<Arr_lum[a]<<endl;
#endif
			a++;

			cvReleaseImage(&Img_tmp);
		}
	}

	//for (i=0; i<9; i++)
	//{
	//	Arr_lum[i] = (int)floor(Arr_lum[i]);
	//}

	//for (i=0; i<9; i++)
	//{
	//	for (j=0; j<9; j++)
	//	{
	//		if (i == j)
	//		{
	//			continue;
	//		}
	//		if (Arr_lum[i] == Arr_lum[j])
	//		{
	//			count_SL++;
	//		}
	//	}
	//	if (count_SL > 5)
	//	{
	//		/*信号丢失*/
	//		return 2;
	//	}
	//	else
	//	{
	//		count_SL = 0;
	//	}
	//}

	cvReleaseImage(&Img_HSV);
	cvReleaseImage(&Img_V);
	//cvReleaseImage(&Img_cpy);

	if (Val_lum > global_thrd_B)
	{
		/*整个画面过亮*/
		return true;
	}
	else if (Val_lum < global_thrd_D)
	{
		/*整个画面过暗*/
		return true;
	}

	for (i=0; i<9; i++)
	{		
		if (Arr_lum[i] > local_thrd_B)
		{
			count_B++;
		}
		if (Arr_lum[i] < local_thrd_D)
		{
			count_D++;
		}		
	}

	if (count_B > 4)
	{
		/*局部画面过亮*/
		return true;
	}
	else if (count_D > 4)
	{
		/*局部画面过暗*/
		return true;
	}

	return false;
}

bool VideoColorCastDetectRGB(const IplImage* rgb,double& Kfactors,double& D)
{
	int height=rgb->height;
	int width = rgb->width;
	double* data = new double[height*width*3];
	RGB2Lab(rgb,data);

	bool bColorCast=false;

	double da,db,ma,mb;
	da=db=ma=mb=0;
	int idx=0;
	long count=0;
	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			idx = i*width*3+3*j;
			//if (data[idx]>30 && data[idx]<95) //30<L<95
			{
				da += data[idx+1];//a
				db += data[idx+2];//b
				ma += data[idx+1]*data[idx+1]; //a^2
				mb += data[idx+2]*data[idx+2]; //b^2
				count++;
			}
		}
	}

	da /= (count+0.000001);//(height*width);
	db /= (count+0.000001);//(height*width);

	ma /= (count+0.000001);//(height*width);
	mb /= (count+0.000001);//(height*width);

	D = sqrt(da*da+db*db);

	ma = ma - da*da;
	mb = mb - db*db;

	double M = sqrt(ma+mb);

	Kfactors = D/(M+0.000001);
	//float mean_a = da;
	//float mean_b = db;

	//float maxab = std::max(abs(da),abs(db));

	//float LpALL = count*1.0/height/width;

	//if ((Kfactors>0.6 && D>20) || Kfactors>1.5)

	/*	if (Kfactors>1.5 && D>20)*/
	if (Kfactors>1.5 && D>15)
	{
		//printf("ColorHue Is ERROR: K=%.4f, da=%.4f,db=%.4f,D=%.4f, LpALL=%.4f  \n",Kfactors,da,db,D,LpALL);
		bColorCast = true;
	}
	else
	{
		//printf("ColorHue Is OK: K=%.4f, da=%.4f,db=%.4f,D=%.4f, LpALL=%.4f\n",Kfactors,da,db,D,LpALL);
		bColorCast = false;
	}

	delete []data;

	return bColorCast;
}

bool ImageSharpnessDiagnose(IplImage* Img)
{
#if 1

	int i, j, ii, jj;
	CvScalar scalar_ori;
	float Avg_ori=0, Avg_exp=0, Avg_ori_sqre=0, Avg_exp_sqre=0, Var_ori=0, Var_exp=0, Var_ori_exp=0, Sdv_ori=0, Sdv_exp=0, Sdv_ori_exp=0;
	float *Arr_Var_ori, *Arr_Var_exp, *Arr_Avg_ori, *Arr_Avg_exp, *Arr_Var_ori_exp;
	float constant_1, constant_2, SS_index, SS_index_numerator, SS_index_dinominator, Sum_SS_index=0, NSS_index;
	int count=0, count_arr=0;
	int constand_N = 64;	
	float Val_avg_ori;
	float Thrd_gradient = 9.0;
	float Thrd_NSS = 0.15;

	//IplImage* Img_reze = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);

	//cvResize(Img, Img_reze, CV_INTER_LINEAR);
	//
	//cvShowImage("Resize", Img_reze);
	//cvWaitKey(0);

	//IplImage* Img_reze = cvCloneImage(Img);

	IplImage* Img_exp = cvCreateImage(cvGetSize(Img), IPL_DEPTH_8U, 3);
	IplImage* Img_grad = cvCreateImage(cvGetSize(Img), IPL_DEPTH_8U, 1);
	IplImage* Img_exp_grad = cvCreateImage(cvGetSize(Img), IPL_DEPTH_8U, 1);

	cvSmooth(Img, Img_exp, CV_GAUSSIAN, 3, 7, 0, 0);
	//cvShowImage("gau", Img_exp);
	//cvSmooth(Img, Img_exp, CV_GAUSSIAN, 7, 3, 0, 0);
	//cvShowImage("7*3", Img_exp);
	//cvWaitKey(0);

	//time_star = clock();

	Img_grad = GradientConvert(Img);

	Img_exp_grad = GradientConvert(Img_exp);

	scalar_ori = cvAvg(Img_grad);
	//scalar_exp = cvAvg(Img_exp_grad);

	Val_avg_ori = scalar_ori.val[0];
	//Val_avg_exp = scalar_exp.val[0];

#ifdef DEBUG_ALGORITHEM_MODEL
	cout<<"Gradient Average: "<<Val_avg_ori<<endl;
	//cout<<"GauPro Average: "<<Val_avg_exp<<endl;
#endif

	if (Val_avg_ori < Thrd_gradient)
	{
		return 1;
	}

	//time_fin = clock();
	//printf("the time of GradientConver is: %.2fms\n", time_fin-time_star);

	//cvShowImage("ori", Img_grad);
	//cvShowImage("exp", Img_exp_grad);

	int size = (Img->width/4 - 1) * (Img->height/4 - 1);

	constant_1 = 0.01 * 255;
	constant_2 = 0.03 * 255;

	Arr_Avg_ori = new float[size];
	Arr_Avg_exp = new float[size];
	Arr_Var_ori = new float[size];
	Arr_Var_exp = new float[size];
	Arr_Var_ori_exp = new float[size];
	//Arr_Sdv_ori = new float[size];
	//Arr_Sdv_exp = new float[size];
	//Arr_Sdv_ori_exp = new float[size];

	//time_star = clock();
#if 1
	for (j=0; j<=Img->height-8; j=j+4)
	{
		for (i=0; i<=Img->width-8; i=i+4)
		{			
			for (jj=j; jj<j+8; jj++)
			{
				for (ii=i; ii<i+8; ii++)
				{
					Avg_ori+=CV_IMAGE_ELEM(Img_grad, uchar, jj, ii);

					Avg_exp+=CV_IMAGE_ELEM(Img_exp_grad, uchar, jj, ii);

					Avg_ori_sqre+=CV_IMAGE_ELEM(Img_grad, uchar, jj, ii) * CV_IMAGE_ELEM(Img_grad, uchar, jj, ii);

					Avg_exp_sqre+=CV_IMAGE_ELEM(Img_exp_grad, uchar, jj, ii) * CV_IMAGE_ELEM(Img_exp_grad, uchar, jj, ii);

					Sdv_ori_exp+=CV_IMAGE_ELEM(Img_grad, uchar, jj, ii) * CV_IMAGE_ELEM(Img_exp_grad, uchar, jj, ii);

					count++;
				}
			}
			Arr_Avg_ori[count_arr] = Avg_ori / count;

			Arr_Avg_exp[count_arr] = Avg_exp / count;

			Arr_Var_ori[count_arr] = Avg_ori_sqre/count - Arr_Avg_ori[count_arr]*Arr_Avg_ori[count_arr];

			Arr_Var_exp[count_arr] = Avg_exp_sqre/count - Arr_Avg_exp[count_arr]*Arr_Avg_exp[count_arr];

			//Arr_Var_ori_exp[count_arr] = sqrt(Arr_Var_ori[count_arr]*Arr_Var_exp[count_arr]);

			//Arr_Var_ori_exp[count_arr] = (sqrt(Avg_ori_sqre)/count-Arr_Avg_ori[count_arr])*(sqrt(Avg_exp_sqre)/count-Arr_Avg_exp[count_arr]);

			Arr_Var_ori_exp[count_arr] = Sdv_ori_exp/count - Arr_Avg_ori[count_arr]*Arr_Avg_exp[count_arr];

			//Arr_Var_ori_exp[count_arr] = abs(sqrt(Arr_Var_ori[count_arr]*Arr_Var_exp[count_arr]) - sqrt(Arr_Var_ori[count_arr]*Arr_Var_exp[count_arr]/count));

			Avg_ori = 0;
			Avg_exp = 0;
			Avg_ori_sqre = 0;
			Avg_exp_sqre = 0;
			Sdv_ori_exp = 0;
			count = 0;

			count_arr++;
		}
	}

#if 0
	count_arr = 0;

	for (j=0; j<=Img->height-8; j=j+4)
	{
		for (i=0; i<=Img->width-8; i=i+4)
		{
			for (jj=j; jj<j+8; jj++)
			{
				for (ii=i; ii<i+8; ii++)
				{
					//Var_ori+=pow((CV_IMAGE_ELEM(Img_grad, uchar, jj, ii)-Arr_Avg_ori[count_arr]), 2);

					//Var_exp+=pow((CV_IMAGE_ELEM(Img_exp_grad, uchar, jj, ii)-Arr_Avg_exp[count_arr]), 2);

					Var_ori_exp+=abs((CV_IMAGE_ELEM(Img_grad, uchar, jj, ii)-Arr_Avg_ori[count_arr]) * 
						(CV_IMAGE_ELEM(Img_exp_grad, uchar, jj, ii)-Arr_Avg_exp[count_arr]));

					count++;
				}
			}
			//Arr_Var_ori[count_arr] = Var_ori / count;

			//Arr_Var_exp[count_arr] = Var_exp / count;

			Arr_Var_ori_exp[count_arr] = Var_ori_exp / count;

			//Arr_Sdv_ori[count_arr] = pow(Arr_Var_ori[count_arr], float(0.5));

			//Arr_Sdv_exp[count_arr] = pow(Arr_Var_exp[count_arr], float(0.5));

			//Arr_Sdv_ori_exp[count_arr] = pow(Arr_Var_ori_exp[count_arr], float(0.5));

			count = 0;
			Var_ori = 0;
			Var_exp = 0;
			Var_ori_exp = 0;

			count_arr++;
		}
	}
#endif	
	//time_fin = clock();
	//printf("the time of Cala is: %.2fms\n", time_fin-time_star);

	for (j=0; j<constand_N-1; j++)
	{
		for (i=j+1; i<size; i++)
		{
			if (Arr_Var_ori[j] < Arr_Var_ori[i])
			{
				Var_ori = Arr_Var_ori[j];
				Arr_Var_ori[j] = Arr_Var_ori[i];
				Arr_Var_ori[i] = Var_ori;

				Var_exp = Arr_Var_exp[j];
				Arr_Var_exp[j] = Arr_Var_exp[i];
				Arr_Var_exp[i] = Var_exp;

				Var_ori_exp = Arr_Var_ori_exp[j];
				Arr_Var_ori_exp[j] = Arr_Var_ori_exp[i];
				Arr_Var_ori_exp[i] = Var_ori_exp;

				Avg_ori = Arr_Avg_ori[j];
				Arr_Avg_ori[j] = Arr_Avg_ori[i];
				Arr_Avg_ori[i] = Avg_ori;

				Avg_exp = Arr_Avg_exp[j];
				Arr_Avg_exp[j] = Arr_Avg_exp[i];
				Arr_Avg_exp[i] = Avg_exp;
			}
		}

		SS_index_numerator = (2*Arr_Avg_ori[j]*Arr_Avg_exp[j]+constant_1) * (2*Arr_Var_ori_exp[j]+constant_2);

		//SS_index_dinominator = (pow(Arr_Avg_ori[j],2)+pow(Arr_Avg_exp[j],2)+constant_1) * (Arr_Var_ori[j]+Arr_Var_exp[j]+constant_2);

		SS_index_dinominator = (Arr_Avg_ori[j]*Arr_Avg_ori[j]+Arr_Avg_exp[j]*Arr_Avg_exp[j]+constant_1) * (Arr_Var_ori[j]+Arr_Var_exp[j]+constant_2);

		SS_index = SS_index_numerator / SS_index_dinominator;

		Sum_SS_index+=SS_index;
	}	
#endif

#if 0
	for (j=0; j<=Img->height-8; j=j+4)
	{
		for (i=0; i<=Img->width-8; i=i+4)
		{			
			for (jj=j; jj<j+8; jj++)
			{
				for (ii=i; ii<i+8; ii++)
				{
					Avg_ori+=CV_IMAGE_ELEM(Img_grad, uchar, jj, ii);

					Avg_exp+=CV_IMAGE_ELEM(Img_exp_grad, uchar, jj, ii);

					Avg_ori_sqre+=pow((float)CV_IMAGE_ELEM(Img_grad, uchar, jj, ii), 2);

					Avg_exp_sqre+=pow((float)CV_IMAGE_ELEM(Img_exp_grad, uchar, jj, ii), 2);

					count++;
				}
			}
			Arr_Avg_ori[count_arr] = Avg_ori / count;

			Arr_Avg_exp[count_arr] = Avg_exp / count;

			Arr_Var_ori[count_arr] = Avg_ori_sqre/count - Arr_Avg_ori[count_arr]*Arr_Avg_ori[count_arr];

			Arr_Var_exp[count_arr] = Avg_exp_sqre/count - Arr_Avg_exp[count_arr]*Arr_Avg_exp[count_arr];

			Arr_Var_ori_exp[count_arr] = (Avg_ori*Avg_exp-Avg_ori*Arr_Avg_exp[count_arr]-Avg_exp*Arr_Avg_ori[count_arr]+Arr_Avg_ori[count_arr]*Arr_Avg_exp[count_arr])/(count-1);

			//Arr_Sdv_ori[count_arr] = sqrt(Arr_Var_ori[count_arr]);

			//Arr_Sdv_exp[count_arr] = sqrt(Arr_Var_exp[count_arr]);

			//Arr_Var_ori_exp[count_arr] = Arr_Sdv_ori[count_arr] * Arr_Sdv_exp[count_arr];

			Avg_ori = 0;
			Avg_exp = 0;
			Avg_ori_sqre = 0;
			Avg_exp_sqre = 0;
			count = 0;

			count_arr++;
		}
	}

	for (j=0; j<constand_N-1; j++)
	{
		for (i=j+1; i<size; i++)
		{
			if (Arr_Var_ori[j] < Arr_Var_ori[i])
			{
				Var_ori = Arr_Var_ori[j];
				Arr_Var_ori[j] = Arr_Var_ori[i];
				Arr_Var_ori[i] = Var_ori;

				Var_exp = Arr_Var_exp[j];
				Arr_Var_exp[j] = Arr_Var_exp[i];
				Arr_Var_exp[i] = Var_exp;

				Avg_ori = Arr_Avg_ori[j];
				Arr_Avg_ori[j] = Arr_Avg_ori[i];
				Arr_Avg_ori[i] = Avg_ori;

				Avg_exp = Arr_Avg_exp[j];
				Arr_Avg_exp[j] = Arr_Avg_exp[i];
				Arr_Avg_exp[i] = Avg_exp;

				Var_ori_exp = Arr_Var_ori_exp[j];
				Arr_Var_ori_exp[j] = Arr_Var_ori_exp[i];
				Arr_Var_ori_exp[i] = Var_ori_exp;
			}
		}

		SS_index_numerator = (2*Arr_Avg_ori[j]*Arr_Avg_exp[j]+constant_1) * (2*Arr_Var_ori_exp[j]+constant_2);

		SS_index_dinominator = (pow(Arr_Avg_ori[j],2)+pow(Arr_Avg_exp[j],2)+constant_1) * (Arr_Var_ori[j]+Arr_Var_exp[j]+constant_2);

		SS_index = SS_index_numerator / SS_index_dinominator;

		Sum_SS_index+=SS_index;
	}	
#endif

	//time_fin = clock();
	//printf("the time of Cala is: %.2fms\n", time_fin-time_star);

	NSS_index = 1 - Sum_SS_index / constand_N;
#ifdef DEBUG_ALGORITHEM_MODEL
	cout<<"NSS_index: "<<NSS_index<<endl;
#endif

	delete []Arr_Avg_ori;
	delete []Arr_Avg_exp;
	delete []Arr_Var_ori;
	delete []Arr_Var_exp;
	delete []Arr_Var_ori_exp;
	//delete []Arr_Sdv_ori;
	//delete []Arr_Sdv_exp;
	//delete []Arr_Sdv_ori_exp;

	//cvReleaseImage(&Img_reze);
	cvReleaseImage(&Img_exp);
	cvReleaseImage(&Img_grad);
	cvReleaseImage(&Img_exp_grad);

	if (NSS_index < Thrd_NSS)
	{
		return true;
	}
	else
	{
		return false;
	}

#endif

}

//清晰度异常检测，最小值0.85，最大值为2；
bool VideoBlurDetect(const IplImage* data /*grayimage*/, double& ratio,double blurThre)
{
	bool bBlur=false;

	int width=data->widthStep;
	int height=data->height;

	ushort* sobelTable = new ushort[width*height];
	memset(sobelTable, 0, width*height*sizeof(ushort));

	int i, j, mul;
	double m,stdv;
	cvMean_StdDev(data,&m,&stdv);  //计算平均值及标准差(方差开方)
	
	uchar sobelThre = (uchar) stdv;
	//float sobel0,sobel45,sobel90,sobel135;

	uchar* udata = (uchar*)data->imageData;	
	//计算sobel x方向一阶差分
	for(i = 1, mul = i*width; i < height - 1; i++, mul += width)
	{
		for(j = 1; j < width - 1; j++)
		{
			sobelTable[mul+j] = abs(-udata[mul+j-width-1] - 2*udata[mul+j-1] - udata[mul+j-1+width] +\
				udata[mul+j+1-width] + 2*udata[mul+j+1] + udata[mul+j+width+1]);
		}
	}

	for(i = 1, mul = i*width; i < height - 1; i++, mul += width)
	{
		for(j = 1; j < width - 1; j++)
		{
			if(sobelTable[mul+j] < sobelThre || sobelTable[mul+j] <= sobelTable[mul+j-1] ||\
				sobelTable[mul+j] <= sobelTable[mul+j+1]) 	
			{
				sobelTable[mul+j] = 0;
			}
		}
	}

	int totLen = 0;
	int totCount = 1;

	uchar suddenThre = sobelThre; //50
	uchar sameThre = (uchar)(0.05*sobelThre);//3

	for(i = 1, mul = i*width; i < height - 1; i++, mul += width)
	{
		for(j = 1; j < width - 1; j++)
		{
			if(sobelTable[mul+j])
			{
				int   count = 0;
				uchar tmpThre = 3;
				uchar max = udata[mul+j] > udata[mul+j-1] ? 0 : 1;

				for(int t = j; t > 0; t--)
				{

					//count++;
					if(abs(udata[mul+t] - udata[mul+t-1]) > suddenThre) 
						break;

					if(max && udata[mul+t] > udata[mul+t-1])
						break;

					if(!max && udata[mul+t] < udata[mul+t-1])
						break;

					int tmp = 0;
					for(int s = t; s > 0; s--)
					{ 
						if(abs(udata[mul+t] - udata[mul+s]) <= sameThre)
						{
							tmp++;
							if(tmp > tmpThre) 
							{
								count++; 
								break;
							}
						}
						else 
							break;
					}

					if(tmp > tmpThre) break;
				}

				max = udata[mul+j] > udata[mul+j+1] ? 0 : 1;

				for(int t = j; t < width; t++)
				{
					//count++;
					if(abs(udata[mul+t] - udata[mul+t+1]) > suddenThre)
						break;

					if(max && udata[mul+t] > udata[mul+t+1])
						break;

					if(!max && udata[mul+t] < udata[mul+t+1])
						break;

					int tmp = 0;
					for(int s = t; s < width; s++)
					{
						if(abs(udata[mul+t] - udata[mul+s]) <= sameThre)
						{
							tmp++;
							if(tmp > tmpThre) 
							{
								count++; 
								break;
							}
						}
						else 
							break;
					}

					if(tmp > tmpThre) break;
				}				
				//count--;

				totCount++;
				totLen += count;
			}
		}
	}

	ratio = (float)totLen/totCount;
	TRACE("BLUR ratio:%f",ratio);

	if (ratio>blurThre)
	{
		//printf("清晰度 模糊: re1=%.4f , re2=%.4f, avrr=%.4f \n",result1,result2,result);
		//return true;
		bBlur = true;
	}
	else
	{
		//printf("清晰度 正常: re1=%.4f , re2=%.4f, avrr=%.4f \n",result1,result2,result);
		//return false;
		bBlur = false;
	}

	delete[] sobelTable;
	//cvReleaseImage(&YUV);
	//cvReleaseImage(&data);

	return bBlur;
}

/*
bool VideoColorCastDetect(const IplImage* rgb,double& Kfactors,double& D)
{
	int height=rgb->height;
	int width = rgb->width;
	double* data = new double[height*width*3];
	RGB2Lab(rgb,data);

	bool bColorCast=false;

	double da,db,ma,mb;
	da=db=ma=mb=0;
	int idx=0;
	long count=0;
	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			idx = i*width*3+3*j;
			//if (data[idx]>30 && data[idx]<95) //30<L<95
			{
				da += data[idx+1];//a
				db += data[idx+2];//b
				ma += data[idx+1]*data[idx+1]; //a^2
				mb += data[idx+2]*data[idx+2]; //b^2
				count++;
			}
		}
	}

	da /= (count+0.000001);//(height*width);
	db /= (count+0.000001);//(height*width);

	ma /= (count+0.000001);//(height*width);
	mb /= (count+0.000001);//(height*width);

	D = sqrt(da*da+db*db);

	ma = ma - da*da;
	mb = mb - db*db;

	double M = sqrt(ma+mb);

	Kfactors = D/(M+0.000001);
	//float mean_a = da;
	//float mean_b = db;

	//float maxab = std::max(abs(da),abs(db));

	//float LpALL = count*1.0/height/width;

	//if ((Kfactors>0.6 && D>20) || Kfactors>1.5)

	if (Kfactors>1.5 && D>20)
	{
		//printf("ColorHue Is ERROR: K=%.4f, da=%.4f,db=%.4f,D=%.4f, LpALL=%.4f  \n",Kfactors,da,db,D,LpALL);
		bColorCast = true;
	}
	else
	{
		//printf("ColorHue Is OK: K=%.4f, da=%.4f,db=%.4f,D=%.4f, LpALL=%.4f\n",Kfactors,da,db,D,LpALL);
		bColorCast = false;
	}

	delete []data;

	return bColorCast;
}*/

bool FreezeDiagnose(IplImage* Img_prev, IplImage* Img_curt)
{
	int i, j, a=0;		
	int count_zero;	
	float thrd_dif = 0.01;

	//IplImage* Img_prev_gray = cvCreateImage(cvGetSize(Img_prev), IPL_DEPTH_8U, 1);
	//IplImage* Img_curt_gray  = cvCreateImage(cvGetSize(Img_curt), IPL_DEPTH_8U, 1);
	IplImage* Img_temp = cvCreateImage(cvGetSize(Img_curt), IPL_DEPTH_8U, 1);

	//cvZero(Img_temp);

	//cvCvtColor(Img_prev, Img_prev_gray, CV_BGR2GRAY);
	//
	//cvCvtColor(Img_curt, Img_curt_gray, CV_BGR2GRAY);

	cvAbsDiff(Img_prev, Img_curt, Img_temp);

	//long Hist[256];
	//int idx=0;
	//for (int i=0;i<256;i++)
	//{
	//	Hist[i]=0;
	//}

	for (i=0;i<Img_temp->height;i++)
	{
		for (j=0;j<Img_temp->width;j++)
		{
			//idx = CV_IMAGE_ELEM(Img_temp, uchar, i, j);	
			//Hist[idx]++;

			if (CV_IMAGE_ELEM(Img_temp, uchar, i, j)==1)
			{
				CV_IMAGE_ELEM(Img_temp, uchar, i, j) = 0;
			}
		}
	}

	//cvShowImage("Prev", Img_prev);
	//cvShowImage("Curt", Img_curt);
	//cvShowImage("Diff", Img_temp);
	//cvWaitKey(0);

	count_zero = cvCountNonZero(Img_temp);

	//index_dif = (float)(count_zero)/(float)(Img_temp->width*Img_temp->height);

#ifdef DEBUG_ALGORITHEM_MODEL
	cout<<"Different Pixels : "<<count_zero<<endl;
#endif

	if ((float)(count_zero)/(float)(Img_temp->width*Img_temp->height) <= thrd_dif)
	{
		cvReleaseImage(&Img_temp);		

		return true;
	}

	cvReleaseImage(&Img_temp);

	return false;
}

bool FreezeDiagnoseInterface(IplImage* grayImage[],int nImageNum)
{
	int count=0;
	for (int i=1;i<nImageNum;i++)
	{
		//cvShowImage("pre", grayImage[i-1]);
		//cvShowImage("curt", grayImage[i]);
		//cvWaitKey(0);

		if(FreezeDiagnose(grayImage[i-1], grayImage[i]))
		{
			count++;
		}
	}

	double r = count*1.0/nImageNum;
	if ( r>0.5)
	{
		return true;
	}
	else
		return false;
}

bool theSameImage(const IplImage* current,const IplImage* prev)
{
	bool is_same=false;
	int height = current->height;
	int width = current->width;
	int idx;
	int diff;
	int c=0;
	for (int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{
			idx = i*current->widthStep+j;
			diff = (uchar)(current->imageData[idx]) - (uchar)(prev->imageData[idx]);
			if (diff!=0)
				c++;
		}
	}

	if (c*1.0/(height*width)<0.00001)
		is_same = true;
	else
		is_same = false;

	return is_same;
}

float AdjacentRegionAveraging(IplImage* Img, int i, int j, int width, int height)
{	
	float Val_average;
	int Val_sum=0;
	int i_start, i_vector, j_start, j_vector;
	int count=0;

	i_vector = (width - 1) / 2;
	j_vector = (height - 1) / 2;	

	for (j_start=j-j_vector; j_start<=j+j_vector; j_start++)
	{
		for (i_start=i-i_vector; i_start<=i+i_vector; i_start++)
		{
			if (i_start==i && j_start==j)
			{
				continue;
			}

			//cvRectangle(Img, cvPoint(i-i_vector, j-j_vector), cvPoint(i+i_vector, j+j_vector), CV_RGB(255,255,255), 1, 8, 0);
			Val_sum+= CV_IMAGE_ELEM(Img, uchar, j_start, i_start);

			count++;
		}
	}

	Val_average = Val_sum / count;

	return Val_average;
}

IplImage* ConsecutivePointsFilter(IplImage* Img_Src)
{
	int i, j, i_tmp, j_tmp;	
	int flag_start=0, flag_do=0;

	IplImage* Img = cvCloneImage(Img_Src);

	for (j=1; j<Img->height-1; j++)
	{
		for (i=1; i<Img->width-1; i++)
		{
			if (CV_IMAGE_ELEM(Img, uchar, j, i) != 0)
			{
				if (CV_IMAGE_ELEM(Img, uchar, j, i+1) != 0)
				{
					CV_IMAGE_ELEM(Img, uchar, j, i) = 0;

					flag_do = 1;

					if (flag_start == 0)
					{
						flag_start = 1;

						i_tmp = i;
						j_tmp = j;
					}
				}
				else if (CV_IMAGE_ELEM(Img, uchar, j+1, i+1) != 0)
				{
					CV_IMAGE_ELEM(Img, uchar, j, i) = 0;

					flag_do = 1;

					if (flag_start == 0)
					{
						flag_start = 1;

						i_tmp = i;
						j_tmp = j;
					}

					j++;
				}
				else if (CV_IMAGE_ELEM(Img, uchar, j+1, i) != 0)
				{
					CV_IMAGE_ELEM(Img, uchar, j, i) = 0;

					flag_do = 1;

					if (flag_start == 0)
					{
						flag_start = 1;

						i_tmp = i;
						j_tmp = j;
					}

					j++;
					i--;

				}
				else if (CV_IMAGE_ELEM(Img, uchar, j+1, i-1) != 0)
				{
					CV_IMAGE_ELEM(Img, uchar, j, i) = 0;

					flag_do = 1;

					if (flag_start == 0)
					{
						flag_start = 1;

						i_tmp = i;
						j_tmp = j;
					}

					j++;
					i-=2;
				}
				else
				{
					if (flag_do == 1)
					{
						flag_start = 0;
						flag_do = 0;

						i = i_tmp;
						j = j_tmp;
					}					
				}
			}
		}
	}

	return Img;
}

bool ImageNoiseDiagnose(IplImage* Img)
{
	IplImage* Img_gray = cvCreateImage(cvGetSize(Img), 8, 1);
	IplImage* Img_noise = cvCreateImage(cvGetSize(Img), 8, 1);
	IplImage* Img_filter = cvCreateImage(cvGetSize(Img), 8, 1);

	int i, j, width, height, result;
	int count_N=0;
	int Thrd_noise = 20;
	float Thrd_area = 10;
	float Val_AaCurt, Val_AaNt;
	float noise_ratio;
	float Arr_lum[9];
	CvRect rect;	
	int a=0;
	float Val_min, Val_max;
	
	cvZero(Img_noise);

	cvCvtColor(Img, Img_gray, CV_BGR2GRAY);

	for (j=1; j<Img_gray->height-1; j++)
	{
		for (i=1; i<Img_gray->width-1; i++)
		{
			if (abs(CV_IMAGE_ELEM(Img_gray, uchar, j, i)-CV_IMAGE_ELEM(Img_gray, uchar, j, i+1)) > Thrd_noise)
			{
				Val_AaCurt = AdjacentRegionAveraging(Img_gray, i, j, 3, 3);
				Val_AaNt = AdjacentRegionAveraging(Img_gray, i+1, j, 3, 3);

				if (abs(Val_AaCurt-Val_AaNt) < Thrd_area)
				{
					CV_IMAGE_ELEM(Img_noise, uchar, j, i) = 255;
				}
				else
				{
					i = i + 2;
				}
			}
		}
	}

	Img_filter = ConsecutivePointsFilter(Img_noise);

	for (j=0; j<Img_filter->height; j++)
	{
		for (i=0; i<Img_filter->width; i++)
		{
			if (CV_IMAGE_ELEM(Img_filter, uchar, j, i) != 0)
			{
				count_N++;
			}
		}
	}

	noise_ratio = float(count_N) / float((Img_filter->width * Img_filter->height)) * 100.0;
#ifdef DEBUG_ALGORITHEM_MODEL
	cout<<"Noise Ratio: "<<noise_ratio<<endl;
#endif

	Val_max = noise_ratio;
	Val_min = noise_ratio;
	
	width = Img->width / 3;
	height= Img->height / 3;

	for (j=0; j<2*Img->height/3; j+=Img->height/3-1)
	{
		for (i=0; i<2*Img->width/3; i+=Img->width/3-1)
		{
			rect = cvRect(i, j, width, height);

			//cvRectangle(Img_gray, cvPoint(i, j), cvPoint(width+i, height+j), CV_RGB(255,255,255), 1, 8, 0);
			//cvShowImage("Gray_area", Img_gray);

			cvSetImageROI(Img_filter, rect);

			IplImage* Img_tmp = cvCloneImage(Img_filter);
			//cvShowImage("Area", Img_tmp);
			//cvWaitKey(0);
			
			cvResetImageROI(Img_filter);
			
			count_N = cvCountNonZero(Img_tmp);

			Arr_lum[a] = (float)count_N / (float)(width*height) * 100.0;
#ifdef DEBUG_ALGORITHEM_MODEL
			cout<<"Area-"<<a<<" : "<<Arr_lum[a]<<endl;
#endif

			Val_max = Arr_lum[a]>Val_max ? Arr_lum[a]:Val_max;
			Val_min = Arr_lum[a]<Val_min ? Arr_lum[a]:Val_min;

			a++;
			
			cvReleaseImage(&Img_tmp);
		}
	}	

	if (noise_ratio > 1.0)
	{
		result = Val_min >= 1.0*Val_max/4.0 ? 1:0;
	}

	//cvShowImage("Gray", Img_gray);
	//cvShowImage("Noise", Img_noise);
	//cvShowImage("Filetr", Img_filter);
	//cvWaitKey(0);

	cvReleaseImage(&Img_gray);
	cvReleaseImage(&Img_noise);
	cvReleaseImage(&Img_filter);

	if (result == 1)
	{
		return true;
	}
	else
		return false;
}

/*
//灰度图像上的噪声检测,NoiseRatioThre默认值0.1,PSNR_Thre默认值40
bool VideoNoiseDection(IplImage* grayImage[],int nImageNum,double NoiseRatioThre,double PSNR_Thre)
{
	double ratio;
	int count=0;
	for (int i=1;i<nImageNum;i++)
	{
		if (DiffDetect(grayImage[i],grayImage[i-1],ratio,NoiseRatioThre))
		{
			count++;
		}
	}

	double r = count*1.0/nImageNum;
	if ( r>0.7)
	{
		return true;
	}
	else
		return false;
}


//用于灰度图像噪声检测算法中的差分；
bool DiffDetect(IplImage* gray,IplImage* PrevImage,double& ratio,double DiffRatioThre)
{
	bool bLargeDiff=false;
	int i,j,width,height,mul;
	int diff = 0;
	int count=0;
//  	int sameThre=10;
	int sameThre=2;

	int sumCount=0;

	int Xgrids=3;
	int Ygrids=3;

	int stepx,stepy;

	width = gray->widthStep;
	height = gray->height;

	stepx = (int)floor(float(width)/Xgrids);
	stepy = (int)floor(float(height)/Ygrids);

	sumCount=0;
	for (int m=0;m<Ygrids;m++)
	{
		for (int n=0;n<Xgrids;n++)
		{	
			count=0;
			for(i = 0+stepy*m, mul = i*width; i < (m+1)*stepy; i++, mul += width)
			{
				for(j = 0+n*stepx; j < (n+1)*stepx; j++)
				{
					uchar nCurVal = gray->imageData[mul+j];
					uchar nPreVal = PrevImage->imageData[mul+j];
					diff = abs(nCurVal - nPreVal);

					//diffImage->imageData[mul+j] = diff;
					if(diff>sameThre)
						count++;
				}
			}
			
			double diffratio = count*1.0/(stepx*stepy);
			if (diffratio>DiffRatioThre) //large difference
			{
				sumCount++;
			}
		}
	}

	ratio = sumCount*1.0/(Xgrids*Ygrids);
	if (ratio>0.7)
	{
		bLargeDiff=true;
	}
	else
	{
		bLargeDiff=false;
	}

	return bLargeDiff;
}
*/
/*噪声检测（时序差分），效果不好
bool VideoNoiseTimeDection(IplImage* grayImage[],int nImageNum,double NoiseRatioThre, double DiffThre)
{
	double ratio;
	int i,j,k,width,height,mul;
	int diff, Maxdiff;
	int count=0;

	int sumCount=0;

	int Xgrids=3;
	int Ygrids=3;

	int stepx,stepy;

	width = grayImage[0]->widthStep;
	height = grayImage[0]->height;

	stepx = (int)floor(float(width)/Xgrids);
	stepy = (int)floor(float(height)/Ygrids);

	sumCount=0;
	for (int m=0;m<Ygrids;m++)
	{
		for (int n=0;n<Xgrids;n++)
		{	
			count=0;
			for(i = 0+stepy*m, mul = i*width; i < (m+1)*stepy; i++, mul += width)
			{
				for(j = 0+n*stepx; j < (n+1)*stepx; j++)
				{
					Maxdiff = 0;
					for (k=1; k<nImageNum;k++)
					{
						diff = abs((uchar)grayImage[k]->imageData[mul+j] - (uchar)grayImage[k-1]->imageData[mul+j]);
						if (Maxdiff<diff)
						{
							Maxdiff = diff;
						}
					}

					//diffImage->imageData[mul+j] = diff;

					if(Maxdiff>DiffThre)
						count++;
				}
			}

			double diffratio = count*1.0/(stepx*stepy);
			if (diffratio>NoiseRatioThre) //large difference
			{
				sumCount++;
			}
		}
	}

	ratio = sumCount*1.0/(Xgrids*Ygrids);
	if (ratio>0.7)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VideoNoiseBreakupDection(IplImage* grayImage[],int nImageNum,double NoiseRatioThre, double DiffThre)
{
	//In paper of Efficient video breakup detection and verification , no finish and some errors exists in the used opencv function;
	int idx;	
	int height = grayImage[0]->height;
	int width = grayImage[0]->width;
// 	Mat mt(nImageNum-1, grayImage[0]->height,CV_8U);
	unsigned char *pDiff = NULL;
	int nDiffSize ;
	float nEdgeRatio, nDiffDistance; 
	nDiffSize = (nImageNum-1) * height;
	pDiff = new unsigned char[nDiffSize];
	memset( pDiff, 0, nDiffSize);  
	IplImage *pImgDiff = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
	IplImage *pImgDiff2 = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);

	IplImage * pImgXSobel = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
	IplImage * pImgYSobel = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
	IplImage * pImgXSobel_8u = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
	IplImage * pImgYSobel_8u = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
	IplImage * pImgDiv = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
	IplImage * pImgDivAbs = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
	IplImage * pImgDiffSumPre = cvCreateImage(cvSize(1,height),IPL_DEPTH_32S,1);
	IplImage * pImgDiffSum = cvCreateImage(cvSize(1,height),IPL_DEPTH_32S,1);
	//cvNamedWindow("Noise",CV_WINDOW_AUTOSIZE);

	for (int k=1; k<nImageNum;k++)
	{
		cvAbsDiff(grayImage[k],grayImage[k-1],pImgDiff);
		cvShowImage("Noise", pImgDiff);
		cvWaitKey(0);
		cvSobel(pImgDiff, pImgXSobel, 1, 0, 3);
		cvSobel(pImgDiff, pImgYSobel, 0, 1, 3);
		cvConvertScaleAbs(pImgXSobel,pImgXSobel_8u,1,0);
		cvConvertScaleAbs(pImgYSobel,pImgYSobel_8u,1,0);
		cvShowImage("Noise",pImgXSobel_8u);
		cvWaitKey(0);
		cvShowImage("Noise",pImgYSobel_8u);
		cvWaitKey(0);
		cvDiv(pImgXSobel,pImgYSobel,pImgDiv,1);
//		cvMul(pImgXSobel_8u,pImgYSobel_8u,pImgDiv,1);
//  		cvDiv(pImgXSobel_8u,pImgYSobel_8u,pImgDiv,1);

		cvShowImage("Noise",pImgDiv);
		cvWaitKey(0);
		cvAbs(pImgDiv, pImgDivAbs);
		CvScalar scale = cvSum(pImgDivAbs);
		nEdgeRatio = scale.val[0]/(width * height);
 		cvReduce(pImgDiff, pImgDiffSum, 1, CV_REDUCE_SUM);
		if (k>1)
		{
			nDiffDistance = 0;
		}
		cvCopy(pImgDiffSum, pImgDiffSumPre,0);
		
// 		int nWidthStep = grayImage[k]->widthStep;
// 		for (int i=0;i<height;i++)
// 		{
// 			float nDiffSum = 0;
// 			for(int j=0;j<width;j++)
// 			{
// 				idx = i*nWidthStep+j;
// 				nDiffSum += abs(pImgDiff->imageData[idx]);
// 			}
// 			int count =(k-1)*height +i;
// 			pDiff[count] = nDiffSum/width;
// // 			mt.at<int>(i,k-1) = nDiffSum/width;
// 		}

	}

	if (pDiff!=NULL)
	{
		delete[] pDiff;
	}
	cvReleaseImage(&pImgDiff);
	cvReleaseImage(&pImgXSobel);
	cvReleaseImage(&pImgYSobel);
	cvReleaseImage(&pImgXSobel_8u);
	cvReleaseImage(&pImgYSobel_8u);
	cvReleaseImage(&pImgDiv);
	cvReleaseImage(&pImgDivAbs);
	cvReleaseImage(&pImgDiffSumPre);
	cvReleaseImage(&pImgDiffSum);
// 	Mat mt(pImgDiff);
// 	cvShowImage("Noise",pImgDiff);
// 	imshow("Noise",mt);
	cvDestroyWindow("Noise");
return false;

}
/*

bool VideoNoisePointDection(const IplImage* pImage,double NoiseRatioThre=0.5)
{
	int i,j,width,height,idx=0;
	int spaceSize = 1;   //3*3的模版
	float diffThre,L1Thre,L2Thre;
	diffThre = 10;
	L2Thre =L1Thre = ;
	width = pImage->width;
	height = pImage->height;
	for (i=spaceSize; i<(height-spaceSize); i++)
	{
		for (j=spaceSize; j<(width-2-spaceSize); j=j+3)
		{
			idx = i*pImage->widthStep+j;
			float diffAB = abs(pImage->imageData[idx] - pImage->imageData[idx+2]);
			if (diffAB>)
			{

			}
		}
	}
}*/

/*
bool VideoEqualBrightDetect(IplImage* grayImage, float& LowRatio,float& HighRatio, float RatioThre)
{
	bool bBrightUnNormal=false;

	int hist_size=256;  
	float range[] = {0,255};  
	float* ranges[]={range}; 
	CvHistogram* gray_hist = cvCreateHist(1,&hist_size,CV_HIST_ARRAY,ranges,1);
	CvHistogram* equal_hist = cvCreateHist(1,&hist_size,CV_HIST_ARRAY,ranges,1);
	IplImage * pImgEqualHist = cvCreateImage(cvSize(grayImage->width, grayImage->height), grayImage->depth, grayImage->nChannels);
	cvCalcHist(&grayImage,gray_hist,0,0);  
	cvNormalizeHist(gray_hist,1.0);

	float* tmp;
	LowRatio=0;
	HighRatio=0;
	int BrightThre=20, BrightThre2=80;

	cvEqualizeHist(grayImage, pImgEqualHist);
	cvCalcHist(&pImgEqualHist, equal_hist, 0, 0 );


	for (int i=0;i<=BrightThre2;i++)
	{
		tmp = cvGetHistValue_1D(gray_hist,i);
		LowRatio += *tmp;
	}
	for (int i=0;i<=BrightThre;i++)
	{
		tmp = cvGetHistValue_1D(gray_hist,255-i);
		HighRatio += *tmp;
	}

	double tmpr=0;
	//if ((LowRatio>RatioThre || HighRatio>RatioThre) && !VideoSignalLostDetect(grayImage,tmpr))
	if (LowRatio>RatioThre || HighRatio>RatioThre) 
		bBrightUnNormal = true;
	else
		bBrightUnNormal = false;

	cvReleaseHist(&gray_hist);  

	return bBrightUnNormal;
}
*/


/*
bool VideoBrightDetect(IplImage* grayImage, float& LowRatio,float& HighRatio,bool bBlur, float RatioThre)
{
	bool bBrightUnNormal=false;

	int hist_size=256;  
	float range[] = {0,255};  
	float* ranges[]={range}; 
	CvHistogram* gray_hist = cvCreateHist(1,&hist_size,CV_HIST_ARRAY,ranges,1);

	cvCalcHist(&grayImage,gray_hist,0,0);  
	cvNormalizeHist(gray_hist,1.0);

	float* tmp;
	LowRatio=0;
	HighRatio=0;

	int BrightThre, BrightThre2;
// 	if (bBlur) //图像不清晰
// 	{
// 		BrightThre = 205;
// 		BrightThre2 = 80;
// 	}
// 	else
// 	{
		BrightThre = 235;
		BrightThre2=65;
//	}

	for (int i=0;i<=BrightThre2;i++)
	{
		tmp = cvGetHistValue_1D(gray_hist,i);
		LowRatio += *tmp;
	}
	for (int i=BrightThre;i<=255;i++)
	{
		tmp = cvGetHistValue_1D(gray_hist,i);
		HighRatio += *tmp;
	}

	double tmpr=0;
	//if ((LowRatio>RatioThre || HighRatio>RatioThre) && !VideoSignalLostDetect(grayImage,tmpr))
	if (LowRatio>RatioThre || HighRatio>RatioThre) 
		bBrightUnNormal = true;
	else
		bBrightUnNormal = false;

	cvReleaseHist(&gray_hist);  

	return bBrightUnNormal;
}*/

int StripeSeek(IplImage* Img, int Thrd_0, int Thrd_1, int Thrd_2)
{
	int i, j, width, height, ii, jj;
	int count_stripe = 0;
	int Val_0_0=0, Val_0_1=0, Val_1_0=0, Val_1_1=0, Val_2_0=0, Val_2_1=0, Val_3_0=0, Val_3_1=0, Val_4_0=0, Val_4_1=0, Val_5_0=0, Val_5_1=0, Val_6_0=0, 
		Val_6_1=0, Val_7_0=0, Val_7_1=0;

	//cvNamedWindow("Sample", 1);

	width = Img->width-6;
	height = Img->height-6;

	for (j=6; j<height; j+=10)
	{
		for (i=6; i<width; i+=3)
		{
			if (abs(CV_IMAGE_ELEM(Img, uchar, j, i)-CV_IMAGE_ELEM(Img, uchar, j+3, i+3)) >= Thrd_0)
			{
				jj = 6;

#ifdef IMG_STRIPESEEK
				IplImage* Img_0 = cvCloneImage(Img);
				IplImage* Img_1 = cvCloneImage(Img);
				IplImage* Img_2 = cvCloneImage(Img);
				IplImage* Img_3 = cvCloneImage(Img);
				IplImage* Img_4 = cvCloneImage(Img);
				IplImage* Img_5 = cvCloneImage(Img);
				IplImage* Img_6 = cvCloneImage(Img);
				IplImage* Img_7 = cvCloneImage(Img);
#endif

				for (ii=-6; ii<=6; ii++)
				{
					Val_0_0 += abs(CV_IMAGE_ELEM(Img, uchar, j+ii, i+ii) - CV_IMAGE_ELEM(Img, uchar, j, i));

					Val_0_1 += abs(CV_IMAGE_ELEM(Img, uchar, j+ii, i+ii) - CV_IMAGE_ELEM(Img, uchar, j+ii+4, i+ii));	
#ifdef IMG_STRIPESEEK
					CV_IMAGE_ELEM(Img_0, uchar, j+ii, i+ii) = 255;
					CV_IMAGE_ELEM(Img_0, uchar, j+ii+4, i+ii) = 255;
#endif

					Val_1_0 += abs(CV_IMAGE_ELEM(Img, uchar, j, i+ii) - CV_IMAGE_ELEM(Img, uchar, j, i));

					Val_1_1 += abs(CV_IMAGE_ELEM(Img, uchar, j, i+ii) - CV_IMAGE_ELEM(Img, uchar, j+3, i+ii));
#ifdef IMG_STRIPESEEK
					CV_IMAGE_ELEM(Img_1, uchar, j, i+ii) = 255;
					CV_IMAGE_ELEM(Img_1, uchar, j+3, i+ii) = 255;
#endif

					Val_2_0 += abs(CV_IMAGE_ELEM(Img, uchar, j+jj, i+ii) - CV_IMAGE_ELEM(Img, uchar, j, i));

					Val_2_1 += abs(CV_IMAGE_ELEM(Img, uchar, j+jj, i+ii) - CV_IMAGE_ELEM(Img, uchar, j+jj+3, i+ii+3));
#ifdef IMG_STRIPESEEK
					CV_IMAGE_ELEM(Img_2, uchar, j+jj, i+ii) = 255;
					CV_IMAGE_ELEM(Img_2, uchar, j+jj+3, i+ii+3) = 255;
#endif

					Val_3_0 += abs(CV_IMAGE_ELEM(Img, uchar, j+ii, i) - CV_IMAGE_ELEM(Img, uchar, j, i));

					Val_3_1 += abs(CV_IMAGE_ELEM(Img, uchar, j+ii, i) - CV_IMAGE_ELEM(Img, uchar, j+ii, i+3));
#ifdef IMG_STRIPESEEK
					CV_IMAGE_ELEM(Img_3, uchar, j+ii, i) = 255;
					CV_IMAGE_ELEM(Img_3, uchar, j+ii, i+3) = 255;
#endif

					jj--;
				}

				jj = 6;

				for (ii=-6; ii<=6; ii+=2)
				{
					Val_4_0 += abs(CV_IMAGE_ELEM(Img, uchar, j+jj, i+ii) - CV_IMAGE_ELEM(Img, uchar, j+3, i));

					Val_4_1 += abs(CV_IMAGE_ELEM(Img, uchar, j+jj, i+ii) - CV_IMAGE_ELEM(Img, uchar, j+jj+3, i+ii+3));
#ifdef IMG_STRIPESEEK
					CV_IMAGE_ELEM(Img_4, uchar, j+jj, i+ii) = 255;
					CV_IMAGE_ELEM(Img_4, uchar, j+jj+3, i+ii+3) = 255;
#endif

					Val_5_0 += abs(CV_IMAGE_ELEM(Img, uchar, j-jj, i+ii) - CV_IMAGE_ELEM(Img, uchar, j-3, i));

					Val_5_1 += abs(CV_IMAGE_ELEM(Img, uchar, j-jj, i+ii) - CV_IMAGE_ELEM(Img, uchar, j-jj+4, i+ii));
#ifdef IMG_STRIPESEEK
					CV_IMAGE_ELEM(Img_5, uchar, j-jj, i+ii) = 255;
					CV_IMAGE_ELEM(Img_5, uchar, j-jj+4, i+ii) = 255;
#endif

					Val_6_0 += abs(CV_IMAGE_ELEM(Img, uchar, j-ii, i-jj) - CV_IMAGE_ELEM(Img, uchar, j, i-3));

					Val_6_1 += abs(CV_IMAGE_ELEM(Img, uchar, j-ii, i-jj) - CV_IMAGE_ELEM(Img, uchar, j-ii+3, i-jj+3));
#ifdef IMG_STRIPESEEK
					CV_IMAGE_ELEM(Img_6, uchar, j-ii, i-jj) = 255;
					CV_IMAGE_ELEM(Img_6, uchar, j-ii+3, i-jj+3) = 255;
#endif

					Val_7_0 += abs(CV_IMAGE_ELEM(Img, uchar, j+ii, i-jj) - CV_IMAGE_ELEM(Img, uchar, j, i-3));

					Val_7_1 += abs(CV_IMAGE_ELEM(Img, uchar, j+ii, i-jj) - CV_IMAGE_ELEM(Img, uchar, j+ii+4, i-jj));
#ifdef IMG_STRIPESEEK
					CV_IMAGE_ELEM(Img_7, uchar, j+ii, i-jj) = 255;
					CV_IMAGE_ELEM(Img_7, uchar, j+ii+4, i-jj) = 255;
#endif

					jj--;
				}

				if (Val_0_0<Thrd_1 && Val_0_1>=Thrd_2 && Val_0_1>=Val_0_0*2)
				{
#ifdef IMG_STRIPESEEK
					cvShowImage("Sample", Img_0);
					cvWaitKey(0);
					cvReleaseImage(&Img_0);
#endif
					count_stripe++;
				}
				if (Val_1_0<Thrd_1 && Val_1_1>=Thrd_2 && Val_1_1>=Val_1_0*2)
				{
#ifdef IMG_STRIPESEEK
					cvShowImage("Sample", Img_1);
					cvWaitKey(0);
					cvReleaseImage(&Img_1);
#endif
					count_stripe++;
				}
				if (Val_2_0<Thrd_1 && Val_2_1>=Thrd_2 && Val_2_1>=Val_2_0*2)
				{
#ifdef IMG_STRIPESEEK
					cvShowImage("Sample", Img_2);
					cvWaitKey(0);
					cvReleaseImage(&Img_2);
#endif
					count_stripe++;
				}
				if (Val_3_0<Thrd_1 && Val_3_1>=Thrd_2 && Val_3_1>=Val_3_0*2)
				{
#ifdef IMG_STRIPESEEK
					cvShowImage("Sample", Img_3);
					cvWaitKey(0);
					cvReleaseImage(&Img_3);
#endif
					count_stripe++;
				}
				if (Val_4_0<Thrd_1 && Val_4_1>=Thrd_2 && Val_4_1>=Val_4_0*2)
				{
#ifdef IMG_STRIPESEEK
					cvShowImage("Sample", Img_4);
					cvWaitKey(0);
					cvReleaseImage(&Img_4);
#endif
					count_stripe++;
				}
				if (Val_5_0<Thrd_1 && Val_5_1>=Thrd_2 && Val_5_1>=Val_5_0*2)
				{
#ifdef IMG_STRIPESEEK
					cvShowImage("Sample", Img_5);
					cvWaitKey(0);
					cvReleaseImage(&Img_5);
#endif
					count_stripe++;
				}
				if (Val_6_0<Thrd_1 && Val_6_1>=Thrd_2 && Val_6_1>=Val_6_0*2)
				{
#ifdef IMG_STRIPESEEK
					cvShowImage("Sample", Img_6);
					cvWaitKey(0);
					cvReleaseImage(&Img_6);
#endif
					count_stripe++;
				}
				if (Val_7_0<Thrd_1 && Val_7_1>=Thrd_2 && Val_7_1>=Val_7_0*2)
				{
#ifdef IMG_STRIPESEEK
					cvShowImage("Sample", Img_7);
					cvWaitKey(0);
					cvReleaseImage(&Img_7);
#endif
					count_stripe++;
				}

				Val_0_0 = Val_0_1 = Val_1_0 = Val_1_1 = Val_2_0 = Val_2_1 = Val_3_0 = Val_3_1 = Val_4_0 = Val_4_1 = Val_5_0 = Val_5_1 = Val_6_0 = Val_6_1 = 
					Val_7_0 = Val_7_1 = 0;
				//Val_1_0 = abs(CV_IMAGE_ELEM(Img, uchar, j-6, i-6)-CV_IMAGE_ELEM(Img, uchar, j, i)) + abs(CV_IMAGE_ELEM(Img, uchar, j-5, i-5)-CV_IMAGE_ELEM(Img, uchar, j, i)) +
				//	abs(CV_IMAGE_ELEM(Img, uchar, j-4, i-4)-CV_IMAGE_ELEM(Img, uchar, j, i)) + abs(CV_IMAGE_ELEM(Img, uchar, j-3, i-3)-CV_IMAGE_ELEM(Img, uchar, j, i)) +
				//	abs(CV_IMAGE_ELEM(Img, uchar, j-2, i-2)-CV_IMAGE_ELEM(Img, uchar, j, i)) + abs(CV_IMAGE_ELEM(Img, uchar, j-1, i-1)-CV_IMAGE_ELEM(Img, uchar, j, i)) + 
				//	abs(CV_IMAGE_ELEM(Img, uchar, j+1, i+1)-CV_IMAGE_ELEM(Img, uchar, j, i)) + abs(CV_IMAGE_ELEM(Img, uchar, j+2, i+2)-CV_IMAGE_ELEM(Img, uchar, j, i)) + 
				//	abs(CV_IMAGE_ELEM(Img, uchar, j+3, i+3)-CV_IMAGE_ELEM(Img, uchar, j, i)) + abs(CV_IMAGE_ELEM(Img, uchar, i+4, j+4)-CV_IMAGE_ELEM(Img, uchar, j, i)) + 
				//	abs(CV_IMAGE_ELEM(Img, uchar, j+5, i+5)-CV_IMAGE_ELEM(Img, uchar, j, i)) + abs(CV_IMAGE_ELEM(Img, uchar, j+6, i+6)-CV_IMAGE_ELEM(Img, uchar, j, i));

				//Val_1_1 = abs(CV_IMAGE_ELEM(Img, uchar, j-6, i-6)-CV_IMAGE_ELEM(Img, uchar, j-3, i-3)) + abs(CV_IMAGE_ELEM(Img, uchar, j-5, i-5)-CV_IMAGE_ELEM(Img, uchar, j-2, i-2)) + 
				//	abs(CV_IMAGE_ELEM(Img, uchar, j-4, i-4)-CV_IMAGE_ELEM(Img, uchar, j-1, i-1)) + abs(CV_IMAGE_ELEM(Img, uchar, j-3, i-3)-CV_IMAGE_ELEM(Img, uchar, j, i)) + 
				//	abs(CV_IMAGE_ELEM(Img, uchar, j-2, i-2)-CV_IMAGE_ELEM(Img, uchar, j+1,i+1)) + abs(CV_IMAGE_ELEM(Img, uchar, j-1, i-1)-CV_IMAGE_ELEM(Img, uchar, j+2, i+2)) + 
				//	abs(CV_IMAGE_ELEM(Img, uchar, j+1,i+1)-CV_IMAGE_ELEM(Img, uchar, j+4, i+4)) + abs(CV_IMAGE_ELEM(Img, uchar, j+2, i+2)-CV_IMAGE_ELEM(Img, uchar, j+5, i+5)) + 
				//	abs(CV_IMAGE_ELEM(Img, uchar, j+3,i+3)-CV_IMAGE_ELEM(Img, uchar, j+6, i+6)) + abs(CV_IMAGE_ELEM(Img, uchar, j+4, i+4)-CV_IMAGE_ELEM(Img, uchar, j+7, i+7)) + 
				//	abs(CV_IMAGE_ELEM(Img, uchar, j+5, i+5)-CV_IMAGE_ELEM(Img, uchar, j+8, i+8)) + abs(CV_IMAGE_ELEM(Img, uchar, j+6, i+6)-CV_IMAGE_ELEM(Img, uchar, j+9, i+9)) + 
				//	abs(CV_IMAGE_ELEM(Img, uchar, j, i)-CV_IMAGE_ELEM(Img, uchar, j+3, i+3));
			}			
		}
	}

	//cvDestroyWindow("Sample");

	return count_stripe;

}

bool StripeNoiseDiagnose(IplImage* Img_RGB, IplImage* Img_gray)
{	
	int Thrd_0=30, Thrd_1=100, Thrd_2=120, Thrd_3=2000;
	int Val_stripe_gray, Val_stripe_R, Val_stripe_G, Val_stripe_B;

	IplImage* Img_R = cvCreateImage(cvGetSize(Img_RGB), IPL_DEPTH_8U, 1);
	IplImage* Img_G = cvCreateImage(cvGetSize(Img_RGB), IPL_DEPTH_8U, 1);
	IplImage* Img_B = cvCreateImage(cvGetSize(Img_RGB), IPL_DEPTH_8U, 1);

	cvSplit(Img_RGB, Img_B, Img_G, Img_R, NULL);	
	//cvShowImage("Src", Img_RGB);
	//cvShowImage("R", Img_R);
	//cvShowImage("G", Img_G);
	//cvShowImage("B", Img_B);
	//cvWaitKey(0);

	Val_stripe_gray = StripeSeek(Img_gray, Thrd_0, Thrd_1, Thrd_2);
#ifdef  DEBUG_ALGORITHEM_MODEL
	cout<<"gray stripe count : "<<Val_stripe_gray<<endl;
#endif

	Val_stripe_B = StripeSeek(Img_B, Thrd_0, Thrd_1, Thrd_2);
#ifdef DEBUG_ALGORITHEM_MODEL
	cout<<"B stripe count : "<<Val_stripe_B<<endl;
#endif

	Val_stripe_G = StripeSeek(Img_G, Thrd_0, Thrd_1, Thrd_2);
#ifdef DEBUG_ALGORITHEM_MODEL
	cout<<"G stripe count : "<<Val_stripe_G<<endl;
#endif

	Val_stripe_R = StripeSeek(Img_R, Thrd_0, Thrd_1, Thrd_2);
#ifdef DEBUG_ALGORITHEM_MODEL
	cout<<"R stripe count : "<<Val_stripe_R<<endl;
#endif

	cvReleaseImage(&Img_R);
	cvReleaseImage(&Img_G);
	cvReleaseImage(&Img_B);

	if (Val_stripe_gray>Thrd_3 || Val_stripe_R>Thrd_3 || Val_stripe_G>Thrd_3 || Val_stripe_B>Thrd_3)
	{
		return true;
	}
	else if (Val_stripe_B>2*Val_stripe_G || Val_stripe_B>2*Val_stripe_R || Val_stripe_R>2*Val_stripe_B || Val_stripe_R>2*Val_stripe_G || Val_stripe_G>2*Val_stripe_B 
		|| Val_stripe_G>2*Val_stripe_R)
	{
		return true;
	}

	return false;
}

bool ScrollDiagnose(IplImage* Img)
{
	int i, j, width, height, j_prev=0;
	int count_row=0, count_col=0;
	int Thrd_row = Img->width*3/4, Thrd_col = 10;
	int hist[256];

	for (i=0; i<256; i++)
	{
		hist[i] = 0;
	}

	width = Img->width;
	height = Img->height;

	//cvShowImage("Image", Img);
	//cvSaveImage("temp.jpg", Img);

	for (j=0; j<height; j++)
	{
		for (i=0; i<width-1; i++)
		{
			hist[CV_IMAGE_ELEM(Img, uchar, j, i)]++;

			if (CV_IMAGE_ELEM(Img, uchar, j, i)==CV_IMAGE_ELEM(Img, uchar, j, i+1) && CV_IMAGE_ELEM(Img, uchar, j, i)<20)
			{
				count_row++;
			}
		}
		if (count_row > Thrd_row)
		{
			if (j == j_prev+1)
			{
				count_col++;

				if (count_col >= Thrd_col)
				{					
					return true;
				}

				j_prev = j;

			}
			else
			{
				j_prev = j;

				count_col = 0;
			}
		}

		count_row = 0;
	}
#ifdef DEBUG_ALGORITHEM_MODEL
	cout<<"column count :  "<<count_col<<endl;
#endif

	return false;
}

bool CoverDiagnose(IplImage* Img)
{
	int i, j, width, height, a=0;
	CvScalar scalar;
	float Arr_area[16];
	CvRect rect;
	int Sum_area=0;
	float Thrd = 5.0;

	//IplImage* Img_gray = cvCloneImage(Img);
	//IplImage* Img_view = cvCloneImage(Img);
	IplImage* Img_grad = cvCreateImage(cvGetSize(Img), IPL_DEPTH_8U, 1);

	Img_grad = GradientConvert(Img);	
	//cvShowImage("Gradient", Img_grad);
	//cvWaitKey(0);

	width = Img_grad->width / 4;
	height = Img_grad->height / 4;

	for (j=0; j<3*Img_grad->height/4; j+=Img_grad->height/4-1)
	{
		for (i=0; i<3*Img_grad->width/4; i+=Img_grad->width/4-1)
		{
			rect = cvRect(i, j, width, height);

			//cvRectangle(Img_view, cvPoint(i, j), cvPoint(width+i, height+j), CV_RGB(255,255,255), 1, 8, 0);

			cvSetImageROI(Img_grad, rect);

			IplImage* Img_tmp = cvCloneImage(Img_grad);			
			/*cvShowImage("Area", Img_tmp);
			cvWaitKey(0);*/

			cvResetImageROI(Img_grad);

			scalar = cvAvg(Img_tmp, 0);

			Arr_area[a] = scalar.val[0];

			//cout<<"Area-"<<a<<" diff : "<<Arr_area[a]<<endl;

			a++;

			cvReleaseImage(&Img_tmp);
		}
	}

	cvReleaseImage(&Img_grad);	

	if (Arr_area[0]<Thrd && Arr_area[1]<Thrd && Arr_area[4]<Thrd)
	{
		return true;
	}
	if (Arr_area[0]<Thrd && Arr_area[1]<Thrd && Arr_area[2]<Thrd && Arr_area[3]<Thrd)
	{
		return true;
	}
	if (Arr_area[2]<Thrd && Arr_area[3]<Thrd && Arr_area[7]<Thrd)
	{
		return true;
	}
	if (Arr_area[3]<Thrd && Arr_area[7]<Thrd && Arr_area[11]<Thrd && Arr_area[15]<Thrd)
	{
		return true;
	}
	if (Arr_area[11]<Thrd && Arr_area[14]<Thrd && Arr_area[15]<Thrd)
	{
		return true;
	}
	if (Arr_area[12]<Thrd && Arr_area[13]<Thrd && Arr_area[14]<Thrd && Arr_area[15]<Thrd)
	{		
		return true;
	}
	if (Arr_area[8]<Thrd && Arr_area[12]<Thrd && Arr_area[13]<Thrd)
	{		
		return true;
	}
	if (Arr_area[0]<Thrd && Arr_area[4]<Thrd && Arr_area[8]<Thrd && Arr_area[12]<Thrd)
	{
		return true;
	}

	return false;	
}

int ImageStretchByHistogram(IplImage *src1,IplImage *dst1)
/*************************************************
Function:      通过直方图变换进行图像增强，将图像灰度的域值拉伸到0-255
src1:               单通道灰度图像                  
dst1:              同样大小的单通道灰度图像 
*************************************************/
{
	assert(src1->width==dst1->width);
	double p[256],p1[256],num[256];

	int minVal=80;
	int maxVal=200;

	memset(p,0,sizeof(p));
	memset(p1,0,sizeof(p1));
	memset(num,0,sizeof(num));
	int height=src1->height;
	int width=src1->width;
	long wMulh = height * width;

	//statistics
	for(int x=0;x<src1->width;x++)
	{
		for(int y=0;y<src1->height;y++)
		{
			uchar v=((uchar*)(src1->imageData + src1->widthStep*y))[x];
			num[v]++;
		}
	}
	//calculate probability
	for(int i=0;i<256;i++)
	{
		p[i]=num[i]/wMulh;
	}

	//p1[i]=sum(p[j]);	j<=i;
	for(int i=0;i<256;i++)
	{
		for(int k=0;k<=i;k++)
			p1[i]+=p[k];
	}

	// histogram transformation
	for(int x=0;x<src1->width;x++)
	{
		for(int y=0;y<src1-> height;y++)
		{
			uchar v=((uchar*)(src1->imageData + src1->widthStep*y))[x];

			if (v>=maxVal)
			{
				((uchar*)(dst1->imageData + dst1->widthStep*y))[x]= MIN((p1[v]-p1[maxVal])*(255-maxVal)/(1-p1[maxVal])+maxVal+0.5,255);
			}
			else if (v<=minVal)
			{
				((uchar*)(dst1->imageData + dst1->widthStep*y))[x]= p1[v]*minVal/(p1[minVal]+0.0001)+0.5;
			}
			else
				((uchar*)(dst1->imageData + dst1->widthStep*y))[x]= v;
		}
	}
	return 0;
}

//distThre is [1.0,2.0]
bool VideoBrightDetect_EquGamma(IplImage* grayImage, double& darkRatio,double& distRatio,double distThre,double darkThre)
{
	uchar* udata;

	int i, j, mul;
	double m,stdv;

	double mu1;
	double mu2;

	double std1;
	double std2;

	bool bBrightUnNormal=false;

	IplImage* EquGrayImg = cvCreateImage(cvGetSize(grayImage),IPL_DEPTH_8U,1);
	IplImage* EquGrayImg2 = cvCreateImage(cvGetSize(grayImage),IPL_DEPTH_8U,1);

	int widthstep = grayImage->widthStep;
	int height = grayImage->height;
	int width = grayImage->width;

	ImageStretchByHistogram(grayImage,EquGrayImg);

	int dark=0;
	udata = (uchar*)grayImage->imageData;
	for (i=0;i<height;i++)
	{
		for (j=0;j<width;j++)
		{
			uchar d = udata[i*widthstep+j];
			if(d<=10)
			{
				//udata[i*widthstep+j]=0;
				dark++;
			}

		}
	}

	double darkratio = dark*1.0/(width*height);


	double gamma=0.45;
	int gr,gr2;
	udata = (uchar*)grayImage->imageData;
	for (i=0;i<height;i++)
	{
		for (j=0;j<width;j++)
		{

			uchar d=udata[i*width+j];
			double delta = (uchar)EquGrayImg->imageData[i*width+j] - udata[i*width+j];

			if (delta>0)
			{
				gr = (int)floor(255*pow(abs(delta)*1.0/255,1/gamma));
				EquGrayImg2->imageData[i*width+j] = (uchar)(udata[i*width+j] + gr);
			}
			else
			{
				gr = (int)floor(255*pow(abs(delta)*1.0/255,1/gamma));
				EquGrayImg2->imageData[i*width+j] = (uchar)(EquGrayImg->imageData[i*width+j]+gr);//abs(udata[i*width+j] - gr);
			}

		}
	}

	int hist_size=256;  
	float range[] = {0,255};  
	float* ranges[]={range}; 
	CvHistogram* gray_hist = cvCreateHist(1,&hist_size,CV_HIST_ARRAY,ranges,1);
	cvCalcHist(&grayImage,gray_hist,0,0); 
	cvNormalizeHist(gray_hist,1.0); 

	CvHistogram* gray_hist2 = cvCreateHist(1,&hist_size,CV_HIST_ARRAY,ranges,1);
	cvCalcHist(&EquGrayImg2,gray_hist2,0,0);  
	cvNormalizeHist(gray_hist2,1.0); 
	double dist;
	double x1,y2;
	double chidist=0;
	mu1=0;
	mu2=0;
	double qmu1=0,qmu2=0;
	double prb1=0;
	double prb2=0;
	for (i=0;i<hist_size;i++)
	{
		x1 = cvGetReal1D(gray_hist->bins,i);
		y2 = cvGetReal1D(gray_hist2->bins,i);

		chidist += (x1-y2)*(x1-y2)/(x1+y2+0.00001);

		prb1 += x1;
		prb2 += y2;

		mu1 += i*x1;
		mu2 += i*y2;

		qmu1 +=i*i*x1;
		qmu2 +=i*i*y2;
	}

	mu1 /=prb1;
	mu2 /=prb2;
	qmu1 /=prb1;
	qmu2 /=prb2;

	std1 = sqrt(qmu1-mu1*mu1);
	std2 = sqrt(qmu2-mu2*mu2);

	//dist = dist*MAX(std1,std2)/(MIN(std1,std2)+0.0001);
	dist = chidist + abs(mu1-mu2)/(MIN(std1,std2)+0.0001) ;


	// 	float max1=0;
	// 	float max2=0;
	// 	cvGetMinMaxHistValue(gray_hist,NULL,&max1,NULL,NULL); 
	// 	cvGetMinMaxHistValue(gray_hist2,NULL,&max2,NULL,NULL); 
	// 
	// 	IplImage* dst=cvCreateImage(cvSize(400,300),8,3);    
	// 	cvSet(dst,cvScalarAll(255),0);    
	// 	double bin_width=(double)dst->width/hist_size;    
	// 	double bin_unith1=(double)dst->height/max1;    
	// 	double bin_unith2=(double)dst->height/max2;   
	// 	for(i=5;i<hist_size-5;i++)    
	// 	{    
	// 		CvPoint p0=cvPoint(i*bin_width,dst->height);    
	// 		CvPoint p1=cvPoint((i+1)*bin_width,dst->height-cvGetReal1D(gray_hist->bins,i)*bin_unith1);    
	// 		cvRectangle(dst,p0,p1,cvScalar(0,0,255),-1,8,0);  //用红色显示直方图  
	// 		CvPoint p2=cvPoint((i+1)*bin_width,dst->height-cvGetReal1D(gray_hist2->bins,i)*bin_unith2);   
	// 		cvRectangle(dst,p0,p2,cvScalar(255,0,0),-1,8,0);    //用绿色显示直方图  
	// 	}  
	// 
	//   	cvNamedWindow("gray image histogram",CV_WINDOW_AUTOSIZE);
	// 	cvShowImage("gray image histogram",dst); 
	// 	cvWaitKey(0);
	// 
	// 	cvReleaseImage(&dst);
	// 
//  	cvNamedWindow("orgi",CV_WINDOW_AUTOSIZE);
//  	cvShowImage("orgi",grayImage);
// 	cvWaitKey(0);
// 
// 	cvNamedWindow("light",CV_WINDOW_AUTOSIZE);
// 	cvShowImage("light",EquGrayImg2);
// 	cvWaitKey(0);

	cvReleaseHist(&gray_hist);  
	cvReleaseHist(&gray_hist2); 
	cvReleaseImage(&EquGrayImg);
	cvReleaseImage(&EquGrayImg2);
// 	cvReleaseImage(&grayImage);

	if (darkRatio>darkThre ||dist>distThre)
	{
		bBrightUnNormal = true;
	}
	else
	{
		bBrightUnNormal=false;
	}

	return bBrightUnNormal;
}

/*
//根据亮度一致性检测亮度异常。
bool VideoBrightConstancyDection(IplImage* grayImage, float& LowRatio,float& HighRatio,bool bBlur, float RatioThre)
{
	double ratio;
	int i,j,k,width,height,mul;
	long sumPixels;
	int count=0;
	int blockCount = 0;
	int sumCount=0;

	int Xgrids=3;
	int Ygrids=3;
    int  blocksPixelVal[9];
	int stepx,stepy;

	blockCount = Xgrids * Ygrids;
	width = grayImage->widthStep;
	height = grayImage->height;

	stepx = (int)floor(float(width)/Xgrids);
	stepy = (int)floor(float(height)/Ygrids);

	for (int m=0;m<Ygrids;m++)
	{
		for (int n=0;n<Xgrids;n++)
		{	
			sumPixels = 0; count = 0;
			for(i = 0+stepy*m, mul = i*width; i < (m+1)*stepy; i++, mul += width)
			{
				for(j = 0+n*stepx; j < (n+1)*stepx; j++)
				{
					sumPixels +=(uchar)(grayImage->imageData[mul+j]) ;
					count++;
				}
			}
			blocksPixelVal[m*Ygrids+n] = sumPixels/count;
		}
	}

	for (int m=0;m<blockCount;m++)
	{
		count = 0;
		for (int n=0;n<blockCount;n++)
		{	
			if (m!=n)
			{
				if ((blocksPixelVal[m]-blocksPixelVal[n])<10)
				{
					count++;
				}				
			}
		}
		if (count>=6)
		{
			sumCount++;
		}
	}
	ratio = sumCount*1.0/(Xgrids*Ygrids);
	if (ratio>0.7)
	{
		return true;
	}
	else
	{
		return false;
	}
}
*/
//
bool VideoJitterDection(IplImage* grayImage[],int nImageNum)
{
	bool bJitter = false;
	double* shiftDists = new double[nImageNum];
	int* shiftOriIdx = new int[nImageNum];
	for (int i=1;i<nImageNum;i++)
	{
		MotionEstimate(grayImage[i],grayImage[i-1],shiftDists[i],shiftOriIdx[i]);
	}

	int count=0;
	int count2=0;
	for (int i=1;i<nImageNum;i++)
	{
		if(shiftDists[i]<3) //位移小于3个像素的，视为非抖动
		{	
			shiftDists[i]=0;
			count++;
		}
		
		if (shiftDists[i]>30) //位移超过30，视为大范围移动
		{
			count2++;
			shiftDists[i]=30;
		}
	}

	if (count2>nImageNum*0.6)
	{
		bJitter = true;
	}
	else if (count>nImageNum*0.6)
	{
		bJitter = false;
	}else{

		double HistProb[5];
		for (int i=0;i<5;i++)
		{
			HistProb[i]=0;
		}

		double sumall=0;
		for (int i=1;i<nImageNum;i++)
		{
			HistProb[shiftOriIdx[i]] += shiftDists[i];

			sumall += shiftDists[i];

			//printf("i=%d    shiftDists=%f    ori=%d\n",i,shiftDists[i],shiftOriIdx[i]);
		}

		double Entropy=0;
		for (int i=0;i<5;i++)
		{
			HistProb[i] /= (sumall+0.000001);

			Entropy += (-HistProb[i]*log10(HistProb[i]+0.000001));
		}

/*
		for (int i=0;i<5;i++)
		{
			printf("Hisprob=%f    i=%d\n",HistProb[i],i);
		}
		printf("Entroy=%f \n",Entropy);*/

		
		if (Entropy>0.2)   //熵0.2大致 概率分布 0 0.2 0.8 0，范围【0.1,0.6】
		{
			bJitter = true;
		}
		else{
			bJitter = false;
		}
	}

	delete []shiftDists;
	delete []shiftOriIdx;
	return bJitter;
}

void MotionEstimate(IplImage* current_gray, IplImage* prev_gray,double& shiftDist, int& oriIDX)
{
	int nPointsNum = 100;
	CvPoint2D32f* prev_points = new CvPoint2D32f[nPointsNum];
	CvPoint2D32f* current_points = new CvPoint2D32f[nPointsNum];
	CvPoint2D32f* FB_points = new CvPoint2D32f[nPointsNum];
	float* error = new float[nPointsNum];
	std::vector<float> FBerror;

	CvTermCriteria criteria;
	criteria.type = CV_TERMCRIT_EPS | CV_TERMCRIT_ITER;
	criteria.epsilon = 0.03;
	criteria.max_iter = 20;
	char* status = new char[nPointsNum];
	char* FBstatus = new char[nPointsNum];
	int level=3;
	CvSize win_size = cvSize(10,10);

	bbPoints(prev_points,nPointsNum,cvRect(0,0,prev_gray->width,prev_gray->height));

    /*
	for (int i=20;i<30;i++)
	{
		printf("points x=%f  y=%f\n",prev_points[i].x,prev_points[i].y);
	}*/

	CvSize pyr_sz=cvSize(prev_gray->width/3,current_gray->height /3);
	IplImage* pyrA=cvCreateImage(pyr_sz,IPL_DEPTH_32F,1);
	IplImage* pyrB=cvCreateImage(pyr_sz,IPL_DEPTH_32F,1);

	cvCalcOpticalFlowPyrLK(prev_gray,current_gray,pyrA,pyrB,
		prev_points,current_points,nPointsNum,win_size,level,status,
		error,criteria,0);

/*
	for (int i=20;i<30;i++)
	{
		printf("points x=%f  y=%f\n",current_points[i].x,current_points[i].y);
	}*/


	//cvCalcOpticalFlowPyrLK(current_gray,prev_gray,pyrA,pyrB,
	//	current_points,FB_points,nPointsNum,win_size,level,FBstatus,
	//	error,criteria,0);

/*
	for (int i=20;i<30;i++)
	{
		printf("points x=%f  y=%f\n",FB_points[i].x,FB_points[i].y);
	}*/



	for( int i= 0; i<nPointsNum; ++i )
	{
		error[i] = sqrt((prev_points[i].x-FB_points[i].x)*(prev_points[i].x-FB_points[i].x)+
			(prev_points[i].y-FB_points[i].y)*(prev_points[i].y-FB_points[i].y));

		FBerror.push_back(error[i]);
	}

	float fbmed = median(FBerror);

	float dx,dy;
	int OriHist[5];
	float shiftHist[5];
	for (int i=0;i<5;i++)
	{
		OriHist[i]=0;
		shiftHist[i]=0;
	}
	for (int i=0;i<nPointsNum;i++)
	{
		if (error[i]<=fbmed)
		{
			dx = current_points[i].x - prev_points[i].x;
			dy = current_points[i].y - prev_points[i].y;

			if (abs(dx)>=abs(dy)) //横向
			{
				if(dx>10)          // 位移10个像素视为移动 left //0
				{
					OriHist[0]++;
					shiftHist[0] += sqrt(dx*dx+dy*dy);
				}
				else if(dx<-10){            //right //1
					OriHist[1]++;
					shiftHist[1] += sqrt(dx*dx+dy*dy);
				}
				else                       //静止状态
				{
					OriHist[4]++;
					shiftHist[4] += sqrt(dx*dx+dy*dy);
				}
			}else{                 //纵向
				if (dy>5)          // up //2
				{
					OriHist[2]++;
					shiftHist[2] += sqrt(dx*dx+dy*dy);
				}else if(dy<-5){          //bottom /3/
					OriHist[3]++;
					shiftHist[3] += sqrt(dx*dx+dy*dy);
				}
				else
				{
					OriHist[4]++;
					shiftHist[4] += 0;//sqrt(dx*dx+dy*dy);
				}
			}

		}	
	}

	int maxOri=0;
	oriIDX=0;
	for(int i=0;i<5;i++)
	{
		if (OriHist[i]>maxOri)
		{
			maxOri = OriHist[i];
			oriIDX = i;
		}
	}

/*
	switch (idx)
	{
	case 0:
		ori=Left;
		break;
	case 1:
		ori = Right;
		break;
	case 2:
		ori = Up;
		break;
	case 3:
		ori = Bottom;
		break;		
	}*/


	shiftDist = shiftHist[oriIDX]/OriHist[oriIDX];

	delete []prev_points;
	delete []current_points;
	delete []FB_points;
	delete []error;
	delete []status;
	delete []FBstatus;
	cvReleaseImage(&pyrA);
	cvReleaseImage(&pyrB);

	FBerror.clear();
}

float median(std::vector<float> v)
{
	int n = (int)floor((float)v.size() / 2);
	nth_element(v.begin(), v.begin()+n, v.end());
	return v[n];
}

void bbPoints(CvPoint2D32f points[],int nNum,CvRect bb)
{
	int max_pts=10;
	int margin_h=0;
	int margin_v=0;

	int stepx = (int)ceil(float(bb.width-2*margin_h)/max_pts);
	int stepy = (int)ceil(float(bb.height-2*margin_v)/max_pts);

	int idx=0;
	for (int y=bb.y+margin_v;y<bb.y+bb.height-margin_v;y+=stepy)
	{
		for (int x=bb.x+margin_h;x<bb.x+bb.width-margin_h;x+=stepx)
		{
			points[idx].x = (float)x;
			points[idx].y = (float)y;
			idx++;
			if (idx>=nNum)
				break;
		}
	}
}



#define yf(x,y) (*((Uint8*)pYdata + (y)*srcWidth+ (x) ))
#define vf(x,y) (*((Uint8*)pUdata+ ((y)/2 )*srcWidth + ((x)/2 ) ))
#define uf(x,y) (*((Uint8*)pVdata + ((y)/2)*srcWidth + ((x)/2) ))

/*pBuf is YUV420 bytes,将YUV尺度化大小
void Byte2ScaleYUVImage(BYTE* pBuf,int srcWidth,int srcHeight,int dstWidth, int dstHeight, int nOutChannels,IplImage* pY,IplImage* pU,IplImage* pV)
{	
	IplImage *pUtemp=NULL,*pVtemp=NULL;
	long yDataLen = dstWidth*dstHeight;
	long uDataLen = yDataLen>>2;
	long vDataLen = uDataLen;

	unsigned int x,y, i,j, tu,tv,tu1,tv1,tf;
	unsigned int y1,v1,u1;

	if(srcWidth <= 0 || srcHeight <= 0 || dstWidth <= 0 || dstHeight <= 0)
		return ;

	BYTE* buffY=(BYTE *)pY->imageData;
	BYTE* buffU=(BYTE *)pU->imageData;
	BYTE* buffV=(BYTE *)pV->imageData;

	BYTE* pYdata = (BYTE*)pBuf;
	BYTE* pUdata = pYdata + yDataLen;
	BYTE* pVdata = pUdata + uDataLen;

	for(int j = 0; j < dstHeight; ++j)
	{
		int mul=j*pY->widthStep;
		int mu2=(int)(j/2)*pU->widthStep;
		int mu3=(int)(j/2)*pV->widthStep;
		for(int i = 0; i < dstWidth; ++i)
		{
			x = srcWidth*i / dstWidth;
			y = srcHeight*j / dstHeight;
			tu = srcWidth*i % dstWidth;
			tv = srcHeight*j % dstHeight;
			tu1 = dstWidth - tu;
			tv1 = dstHeight - tv;
// 			yf0 = *(pY + (y)*srcWidth+ (x) )
			tf = 	tu1 * tv1 * yf(x,y) + tu1 * tv * yf(x,y+1) + tu * tv1 * yf(x+1,y) + tu * tv * yf(x+1,y+1);
			y1 = tf/(dstWidth*dstHeight);
			buffY[mul+i] = y1;
// 			*(dst->pixels[0] + (dstrect->y + y) * dst->pitches[0] + (dstrect->x + x)) = y1;
			if((i%2 == 0 ) && (j%2 == 0 ))
			{
				tf = 	tu1 * tv1 * vf(x,y) + tu1 * tv * vf(x,y+1) + tu * tv1 * vf(x+1,y) + tu * tv * vf(x+1,y+1);
				v1 = tf/(dstWidth*dstHeight);
				tf = 	tu1 * tv1 * uf(x,y) + tu1 * tv * uf(x,y+1) + tu * tv1 * uf(x+1,y) + tu * tv * uf(x+1,y+1);
				u1 = tf/(dstWidth*dstHeight);
				buffU[mu2+i/2] = u1;
				buffV[mu3+i/2] = v1;
// 				*(dst->pixels[1] + (y/2 + dstrect->y /2) * dst->pitches[1] + (dstrect->x/2 + x/2)) = v1;
// 				*(dst->pixels[2] + (y/2 + dstrect->y /2) * dst->pitches[2] + (dstrect->x/2 + x/2)) = u1;
			}
		}
	}
}
*/

void Byte2YUVImage(BYTE* pBuf/*YUV420 bytes*/,int nWidth,int nHeight,int nOutChannels,IplImage* pY,IplImage* pU,IplImage* pV)
{

	//YUV420 bytes data flow transfered to IplImage
	int imgWidth,imgHeight, nInterWidth, nInterHeight;
	nInterWidth = nWidth>>3;
	nInterHeight = nHeight>>3;
	imgWidth = nWidth - 2*nInterWidth;
	imgHeight = nHeight - 2*nInterHeight;

	IplImage *pUtemp=NULL,*pVtemp=NULL;
	long yDataLen = nWidth*nHeight;
	long uDataLen = yDataLen>>2;
	long vDataLen = uDataLen;

	BYTE* buff=(BYTE *)pY->imageData;
	BYTE* pYdata = (BYTE*)pBuf;
	for(int j = 0; j < imgHeight; ++j)
	{
		int mul=j*pY->widthStep;
		for(int i = 0; i < imgWidth; ++i)
		{

			buff[mul+i] = *(pYdata + (nInterHeight + j)*nWidth + nInterWidth + i );
			//pYdata++;
		}
	}

	if (nOutChannels>1)
	{
// 		pUtemp = cvCreateImageHeader(cvSize(nWidth>>1, nHeight>>1),IPL_DEPTH_8U,1);
// 		pVtemp = cvCreateImageHeader(cvSize(nWidth>>1, nHeight>>1),IPL_DEPTH_8U,1);
// 
// 		cvSetData(pUtemp,pBuf+yDataLen,nWidth>>1);
// 		cvSetData(pVtemp,pBuf+yDataLen+uDataLen,nWidth>>1);
// 
// 		cvResize(pUtemp,pU,CV_INTER_LINEAR);
// 		cvResize(pVtemp,pV,CV_INTER_LINEAR);
// 
// 		cvReleaseImage(&pUtemp);
// 		cvReleaseImage(&pVtemp);

	}

	return;

	/*
	IplImage *pUtemp=NULL,*pVtemp=NULL;
	long yDataLen = nWidth*nHeight;
	long uDataLen = yDataLen>>2;
	long vDataLen = uDataLen;

	BYTE* buff=(BYTE *)pY->imageData;
	BYTE* pYdata = (BYTE*)pBuf;
	for(int j = 0; j < nHeight; ++j)
	{
		int mul=j*pY->widthStep;
		for(int i = 0; i < nWidth; ++i)
		{

			buff[mul+i] = *pYdata;
			pYdata++;
		}
	}

	if (nOutChannels>1)
	{
		pUtemp = cvCreateImageHeader(cvSize(nWidth>>1, nHeight>>1),IPL_DEPTH_8U,1);
		pVtemp = cvCreateImageHeader(cvSize(nWidth>>1, nHeight>>1),IPL_DEPTH_8U,1);

		cvSetData(pUtemp,pBuf+yDataLen,nWidth>>1);
		cvSetData(pVtemp,pBuf+yDataLen+uDataLen,nWidth>>1);

		cvResize(pUtemp,pU,CV_INTER_LINEAR);
		cvResize(pVtemp,pV,CV_INTER_LINEAR);

		cvReleaseImage(&pUtemp);
		cvReleaseImage(&pVtemp);

	}

	return;
	*/
}

void Byte2RGBImage(BYTE* pFrameData/*YUV420 bytes*/,int nWidth,int nHeight,IplImage* rgbImage)
{
	LONG nYDataLen = nWidth * nHeight;
	int nUDataLen = (nYDataLen >> 2);
	int nVDataLen = nUDataLen;

	BYTE* pY = (BYTE*)pFrameData;
	BYTE* pU = pY + nYDataLen;
	BYTE* pV = pU + nVDataLen;

	BYTE* buff=(BYTE *)rgbImage->imageData;
	short temp;
	for(int j = 0; j < nHeight; ++j)
	{
		for(int i = 0; i < nWidth; ++i)
		{
			unsigned char y = *pY;
			++pY;

			int offset = (int)((j / 2) * (nWidth / 2) + i / 2);
			unsigned char u = pU[offset];
			unsigned char v = pV[offset];
			temp  = (short)(1.0 * y + 1.772 * (u - 128)) ;
			*buff = (char)(temp < 0 ? 0:((temp > 255)? 255:temp)); //B
			++buff;

			temp  = (short)(1.0 * y - 0.34413 * (u - 128) - 0.71414 * (v - 128)); 
			*buff =(char)(temp < 0 ? 0:((temp > 255) ? 255:temp));// G
			++buff;

			temp  = (short)(1.0 * y + 0 +1.402 * (v - 128));
			*buff = (char)(temp < 0 ? 0:((temp > 255) ? 255:temp));// R
			++buff;
		}
	}
}

void YUV2RGB(BYTE* pFrameData, int nWidth, int nHeight, IplImage*Img_gray, IplImage* Img_rgb)
{
	IplImage *image, *yimg, *uimg,*vimg,*uuimg,*vvimg;

	image = cvCreateImage(cvSize(nWidth, nHeight),IPL_DEPTH_8U,3);
	yimg = cvCreateImageHeader(cvSize(nWidth, nHeight),IPL_DEPTH_8U,1);
	uimg = cvCreateImageHeader(cvSize(nWidth/2, nHeight/2),IPL_DEPTH_8U,1);
	vimg = cvCreateImageHeader(cvSize(nWidth/2, nHeight/2),IPL_DEPTH_8U,1);
	uuimg = cvCreateImage(cvSize(nWidth, nHeight),IPL_DEPTH_8U,1);
	vvimg = cvCreateImage(cvSize(nWidth, nHeight),IPL_DEPTH_8U,1);

	cvSetData(yimg, pFrameData, nWidth);

	cvSetData(vimg, pFrameData+nWidth*nHeight, nWidth/2);

	cvSetData(uimg, pFrameData+long(nWidth*nHeight*1.25), nWidth/2);

	cvResize(uimg, uuimg, CV_INTER_LINEAR);

	cvResize(vimg, vvimg, CV_INTER_LINEAR);

	cvMerge(yimg, uuimg, vvimg, NULL, image);
	
	cvCvtColor(image, Img_rgb, CV_YCrCb2BGR);
	
	cvCvtColor(Img_rgb, Img_gray, CV_BGR2GRAY);

	cvReleaseImage(&image);
	cvReleaseImageHeader(&yimg);
	cvReleaseImageHeader(&uimg);
	cvReleaseImageHeader(&vimg);
	cvReleaseImage(&uuimg);
	cvReleaseImage(&vvimg);
}

bool VideoColorCastDetectBYTE(BYTE* pFrameData,int nWidth,int nHeight,double& Kfactors,double& D)
 {   LONG nYDataLen = nWidth * nHeight;
	int nUDataLen = (nYDataLen >> 2);
	int nVDataLen = nUDataLen;

	BYTE* pY = (BYTE*)pFrameData;
	BYTE* pU = pY + nYDataLen;
	BYTE* pV = pU + nVDataLen;
	
	double R,G,B;
	double X,Y,Z,fx,fy,fz,L,a,b;
	
	double da,db,ma,mb;
	da=db=ma=mb=0;
	
	double T = 0.008856;
	short temp;
	long  imgSize = 0;
	imgSize = nWidth * nHeight;
	for(int j = 0; j < nHeight; ++j)
	{
		for(int i = 0; i < nWidth; ++i)
		{
			unsigned char y = *pY;
			++pY;

			int offset = (int)((j / 2) * (nWidth / 2) + i / 2);
			unsigned char u = pU[offset];
			unsigned char v = pV[offset];
			temp  = (short)(1.0 * y + 1.772 * (u - 128)) ;
			B = (uchar)(temp < 0 ? 0:((temp > 255)? 255:temp)); //B
			
			temp  = (short)(1.0 * y - 0.34413 * (u - 128) - 0.71414 * (v - 128)); 
			G = (uchar)(temp < 0 ? 0:((temp > 255) ? 255:temp));// G
			
			temp  = (short)(1.0 * y + 0 +1.402 * (v - 128));		
			R =  (uchar)(temp < 0 ? 0:((temp > 255) ? 255:temp));// R
// 			TRACE("B=%u,G=%u,R=%u\n", B ,G, R);
			
// 			// RGB -> sRGB -> XYZ  
// 			R /= 255.0f;  
// 			G /= 255.0f;  
// 			B /= 255.0f; 
// 
// 			X = 0.412453f*R + 0.357580f*G + 0.180423f*B;  
// 			Y = 0.212671f*R + 0.715160f*G + 0.072169f*B;  
// 			Z = 0.019334f*R + 0.119193f*G + 0.950227f*B; 
// 			
// 			//XYZ -> Lab
// 			X /=  0.950456;
// 			Z /=  1.088754;
			
// 			下面R,G,B的系数直接根据上面注释部分算出
			X = 0.001702*R + 0.001475*G + 0.000744*B;
			Y = 0.000834*R + 0.002805*G + 0.000283*B;
			Z = 0.000070*R + 0.000429*G + 0.003423*B;
			if (X>T)
			{
				fx = pow(X,1.0/3.0);
			}
			else
			{
				fx = 7.787*X + 16.0/116.0;
			}

			if (Y>T)
			{
				fy = pow(Y,1.0/3.0);
				//L = 116 * pow(Y,1.0/3.0) - 16.0; //后面计算不需L分量
			}
			else
			{
				fy = 7.787*Y + 16.0/116.0;
				//L  = 903.3 * Y;
			}

			if (Z>T)
			{
				fz = pow(Z,1.0/3.0);
			}
			else
			{
				fz = 7.787*Z + 16.0/116.0;
			}
			a = 500 * (fx - fy);
			b = 200 * (fy - fz);
/*			TRACE("a=%f,b=%f\n",a,b);*/
			/*
			lab[j*width*3+3*i] = L;
			lab[j*width*3+3*i+1] = a;
			lab[j*width*3+3*i+2] = b;	
			da += data[idx+1];//a
			db += data[idx+2];//b
			ma += data[idx+1]*data[idx+1]; //a^2
			mb += data[idx+2]*data[idx+2]; //b^2
			*/
			da +=a;
			db +=b;
			ma +=a*a;
			mb +=b*b;
		}
	}
	
	da /= (imgSize+0.000001);//(height*width);
	db /= (imgSize+0.000001);//(height*width);

	ma /= (imgSize+0.000001);//(height*width);
	mb /= (imgSize+0.000001);//(height*width);

	D = sqrt(da*da+db*db);

	ma = ma - da*da;
	mb = mb - db*db;

	double M = sqrt(ma+mb);


	Kfactors = D/(M+0.000001);
	
	if (Kfactors>1.5 && D>15)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

/*
//直接在YUV图像上进行颜色检测
bool VideoColorCastDetectYUV(const IplImage* pY,const IplImage* pU,const IplImage* pV,int nWidth,int nHeight,double& Kfactors,double& D)
{
	int height=pY->height;
	int width = pY->width;
	double* data = new double[height*width*3];
	YUV2Lab(pY,pU,pV,data);

	bool bColorCast=false;

	double da,db,ma,mb;
	da=db=ma=mb=0;
	int idx=0;
	long count=0;
	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			idx = i*width*3+3*j;
			//if (data[idx]>30 && data[idx]<95) //30<L<95
			{
				da += data[idx+1];//a
				db += data[idx+2];//b
				ma += data[idx+1]*data[idx+1]; //a^2
				mb += data[idx+2]*data[idx+2]; //b^2
				count++;
			}
		}
	}

	da /= (count+0.000001);//(height*width);
	db /= (count+0.000001);//(height*width);

	ma /= (count+0.000001);//(height*width);
	mb /= (count+0.000001);//(height*width);

	D = sqrt(da*da+db*db);

	ma = ma - da*da;
	mb = mb - db*db;

	double M = sqrt(ma+mb);


	Kfactors = D/(M+0.000001);
	//float mean_a = da;
	//float mean_b = db;

	//float maxab = std::max(abs(da),abs(db));

	//float LpALL = count*1.0/height/width;

	//if ((Kfactors>0.6 && D>20) || Kfactors>1.5)

	if (Kfactors>1.5 && D>20)
	{
		//printf("ColorHue Is ERROR: K=%.4f, da=%.4f,db=%.4f,D=%.4f, LpALL=%.4f  \n",Kfactors,da,db,D,LpALL);
		bColorCast = true;
	}
	else
	{
		//printf("ColorHue Is OK: K=%.4f, da=%.4f,db=%.4f,D=%.4f, LpALL=%.4f\n",Kfactors,da,db,D,LpALL);
		bColorCast = false;
	}

	delete []data;

	return bColorCast;
}

*/

/*

bool VideoNoiseDect(const IplImage *GrayImg)
{
	bool bNoise = false;
	int nHeight = GrayImg->height;
	int nWidth = GrayImg->width;
	int d_threshold ;
	int L_threshold ;
	double d,L_ratio,d_ratio;
	
	LONG counts[10]={0};

	int step = GrayImg->widthStep;
	char *data = GrayImg->imageData;
	int i,j;
	ofstream file(_T("2.txt"));
	
	for (d_threshold = 1;d_threshold<11;d_threshold++)
	{
		for (L_threshold = 1;L_threshold<16;L_threshold++)
		{
			LONG sum = 0;
			LONG count = 0;
			LONG countd = 0;
			for(i=1;i<nHeight-1;i++)
			{
				for(j = 1;j<nWidth-2;)
				{
					sum ++;
					d = abs(data[i*step+j]-data[i*step+j+2]);

// 					file<<_T("d:  ")<<setiosflags(ios::left)<<setw(8)<<d;
// 					if (d<10)
// 					{
// 						int inx = (int)d;
// 						counts[inx]++;
// 					}
					if (d>d_threshold)
					{
						countd++;
						double L1,L2;
						for (int k = i-1;k<i+2;k++)
						{
							for(int s = j-1;s<j+2;s++)
							{
								L1 = abs(data[i*step+j]-data[k*step+s]);
							}
						}

						for (int k = i-1;k<i+2;k++)
						{
							for(int s = j+1;s<j+4;s++)
							{
								L2 = abs(data[i*step+j+2]-data[k*step+s]);
							}
						}
					//	file<<_T("L1:  ")<<setiosflags(ios::left)<<setw(8)<<L1<<_T("L2:  ")<<setiosflags(ios::left)<<setw(8)<<L2<<endl;
						if (L1>L_threshold||L2>L_threshold)
						{
							count++;
							j=j+3;
						}
						else
						{
							j++;
						}

					}
					else
					{
						j++;
					}
				}
			}
			
			L_ratio = count/(sum+0.000001);
			d_ratio = countd/(sum+0.000001);
			file<<_T("d_threshold=")<<d_threshold;
			file<<_T(",L_threshold=")<<L_threshold<<endl;
			file<<_T("d_ratio: ")<<d_ratio;
			file<<_T(",L_ratio: ")<<L_ratio<<endl;
			
		}
		file<<endl;
	}
	
	file.close();
	
// 	L_ratio = count/(sum+0.000001);
// 	d_ratio = countd/(sum+0.000001);
// 	double temp = 0;
// 	for (int h=0;h<10;h++)
// 	{
// 		temp += counts[h]/(sum+0.000001);
// 		file<<h<<_T(":")<<temp<<endl;
// 	}
// 	file<<_T("L_ratio:  ")<<L_ratio<<endl;
// 	file<<_T("d_ratio:  ")<<d_ratio<<endl;
// 	file.close();

	if (L_ratio>0.1)
	{
		bNoise = true;
	}

	return bNoise;

}

bool VideoNoiseDection(IplImage* grayImage[],int nImageNum,double NoiseRatioThre,double PSNR_Thre)
{

//	double ratio;
// 	int width = grayImage[0]->widthStep;
// 	int height = grayImage[0]->height;

	int count=0;
	for (int i=1;i<nImageNum;i++)
	{
		if (VideoNoiseDect(grayImage[i-1]))
		{
			count++;
		}
	}

	double r = count*1.0/nImageNum;
	if ( r>0.5)
	{
		return true;
	}
	else
		return false;
}
*/
/*
bool NoiseDetection_diff(IplImage* grayImage[],int nImageNum)
{
	unsigned long sum = 0;
	IplImage *current = grayImage[2];
	IplImage *prev   = grayImage[1];
	IplImage *diffImg = cvCreateImage(cvSize(current->width,current->height),current->depth,3);
	IplImage *graydiffImg = cvCreateImage(cvSize(current->width,current->height),current->depth,1);
	cvAbsDiff(current,prev,diffImg);
	int height = current->height;
	int width = current->width;
	int idx;
	int diff;
	unsigned char * buffer = (unsigned char *)diffImg->imageData;
	for (int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{

			//idx = i*current->widthStep+j;
			//diff = abs((uchar)(current->imageData[idx]) - (uchar)(prev->imageData[idx]));
			sum += *buffer;
			buffer++;
			sum += *buffer;
			buffer++;
			sum += *buffer;
			buffer++;

		}
	}
// 	cvConvertImage(diffImg,graydiffImg,CV_BGR2GRAY);
// 	cvNamedWindow("diffImage",0);
// 	cvNamedWindow("graydiffImage",0);
// 	cvShowImage("diffImage",diffImg);
// 	cvShowImage("graydiffImage",graydiffImg);
// 	cvWaitKey(0);
	float res = sum/(height*width*3+0.000001);
	ofstream file(_T("2.txt"));
	file<<_T("res:  ")<<setiosflags(ios::left)<<setw(8)<<res<<endl;
	file.close();
	if (res>1.65)
	{
		return true;
	}

	return false;
	
}
*/


bool NoiseDetection_RGB(IplImage* grayImage[],int nImageNum, int nNoiseThre)
{
	int count=0;
	for (int i=1;i<nImageNum;i++)
	{
		cvShowImage("pre", grayImage[i-1]);
		cvShowImage("cur", grayImage[i]);
		cvWaitKey(0);

		if (DiffRGBDetect(grayImage[i],grayImage[i-1], nNoiseThre))
		{
			count++;
		}
	}

	double r = count*1.0/nImageNum;
	if ( r>0.7)
	{
		return true;
	}
	else
		return false;
}


bool DiffRGBDetect(IplImage* pCurrent,IplImage* PrevImage, int nNoiseThre)
{
	bool bLargeDiff=false;
	int i,j,width,height,mul;
	int diff = 0;
	int count=0;
	/* int sameThre=10;*/
	double ratio = 0;
	double DiffRatioThre = 0.13;
	int sameThre=nNoiseThre;

	int sumCount=0;

	int Xgrids=3;
	int Ygrids=3;

	int stepx,stepy;

	width = pCurrent->widthStep;
	height = pCurrent->height;

	stepx = (int)floor(float(width)/Xgrids);
	stepy = (int)floor(float(height)/Ygrids);

	IplImage * pDiffImg = cvCreateImage(cvGetSize(pCurrent),pCurrent->depth,3);
	cvAbsDiff(pCurrent,PrevImage,pDiffImg);
	uchar * buffer = (uchar *)pDiffImg->imageData;
	
	sumCount=0;
	for (int m=0;m<Ygrids;m++)
	{
		for (int n=0;n<Xgrids;n++)
		{	
			count=0;
			for(i = 0+stepy*m, mul = i*width; i < (m+1)*stepy; i++, mul += width)
			{
				for(j = 0+n*stepx; j < (n+1)*stepx; j++)
				{
					diff = buffer[mul+j] + buffer[mul+j+1] + buffer[mul+j+2];
					diff = diff/3.0;
					if(diff>sameThre)
						count++;
				}
			}

			double diffratio = count*1.0/(stepx*stepy);
			if (diffratio>DiffRatioThre) //large difference
			{
				sumCount++;
			}
		}
	}

	ratio = sumCount*1.0/(Xgrids*Ygrids);
	sumCount = 0;
	if (ratio>0.7)
	{
		bLargeDiff=true;
	}
	else
	{
		bLargeDiff=false;
	}
	cvReleaseImage(&pDiffImg);
	return bLargeDiff;
}