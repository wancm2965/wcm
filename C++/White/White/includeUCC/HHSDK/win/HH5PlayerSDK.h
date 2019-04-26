 /******************************************************************************
* 系统名称：HH5PlayerSDK
* 文件名称：HH5PlayerSDK.h
* 版本    ：V5.5.0.1
* 说明	  ：该模块提供音视频的解码显示和播放

* 日    期: 2005年4月15日
  最近更新:	
* 其他说明: 无
******************************************************************************/
#ifndef HH5PLAYERSDK_H
#define HH5PLAYERSDK_H

#define DLLEXPORT_API  extern "C"__declspec(dllexport) 

#include <mmsystem.h>
#include "HHAVDefine.h"

/*****************************************************************************/
/*********************************  一、宏定义  ******************************/
/*****************************************************************************/
#define HH5PLAYER_MAX_PLAER		128											//最多支持128路解码<nPort = 0 ~ 127>

#define HH5PLAYER_MSG_PLAY_END	0xFFFFFFFF									//消息定义<文件播放结束>
																			//非0xFFFFFFFF: 播放进度: 0 ~ 文件时间长度（秒）
typedef	HHAV_INFO		HH5KAV_INFO;
typedef	PHHAV_INFO		PHH5KAV_INFO;

/*****************************************************************************/
/********************************  二、函数接口定义  *************************/
/*****************************************************************************/

//------------------------------  1、播放器初始化,释放  ----------------------------------//
//初始化SDK资源
DLLEXPORT_API int __stdcall HH5PLAYER_InitSDK(HWND hHwnd);		
//释放  SDK资源			
DLLEXPORT_API int __stdcall HH5PLAYER_ReleaseSDK();				
//初始化播放器(参见HH5PLAYER_InitPlayer2)			
DLLEXPORT_API int __stdcall HH5PLAYER_InitPlayer(USHORT nPort,HWND hWnd);
//释放播放器	
DLLEXPORT_API int __stdcall HH5PLAYER_ReleasePlayer(USHORT nPort);			

//------------------------------  2、文件播放  ----------------------------------//
//打开文件
DLLEXPORT_API int __stdcall HH5PLAYER_OpenStreamFileM(USHORT nPort,LPCTSTR filelist[], int filenum, DWORD &nTimeLength);
//得到文件信息
DLLEXPORT_API int __stdcall HH5PLAYER_GetStreamFileInfo(USHORT nPort, DWORD *dwTimeLength, DWORD *dwFileLength, DWORD *dwWidth,DWORD *dwHeight);  
//播放
DLLEXPORT_API int __stdcall HH5PLAYER_Play(USHORT nPort);                 
//快速播放  dwValue 1---1000之间毫秒     
DLLEXPORT_API int __stdcall HH5PLAYER_FastPlay(USHORT nPort,DWORD nValue);    
//块退 
DLLEXPORT_API int __stdcall HH5PLAYER_FastPlayBack(USHORT nPort,DWORD nValue); 
//单帧进
DLLEXPORT_API int __stdcall HH5PLAYER_FrameGO(USHORT nPort);          
//单帧退        
DLLEXPORT_API int __stdcall HH5PLAYER_FrameBack(USHORT nPort);             
//暂停      
DLLEXPORT_API int __stdcall HH5PLAYER_Pause(USHORT nPort);                     
//继续
DLLEXPORT_API int __stdcall HH5PLAYER_Resume(USHORT nPort);              
//停止      
DLLEXPORT_API int __stdcall HH5PLAYER_Stop(USHORT nPort);                      
//得到播放的时间
DLLEXPORT_API int __stdcall HH5PLAYER_GetPlayPosition(USHORT nPort,DWORD *dwPlayedTime);			
//设置播放位置
DLLEXPORT_API int __stdcall HH5PLAYER_SetPlayPosition(USHORT nPort,float fOffset);				
//移动播放指针到指定秒	
DLLEXPORT_API int __stdcall HH5PLAYER_SeekToSecond(USHORT nPort,int nSec);							
//循环播放 
DLLEXPORT_API int __stdcall HH5PLAYER_SetPlayLoop(USHORT nPort,bool  bIsLoop);						
//注册播放状态消息(1.H5PLAYER_MSG_PLAY_END:文件播放结束	2.播放进度:0 ~ 文件时间长度<秒)
DLLEXPORT_API int __stdcall HH5PLAYER_RegPlayStatusMsg(USHORT nPort, HWND hWnd, UINT MessageID);	


//------------------------------  3、流播放 ----------------------------------//
//打开播放流
DLLEXPORT_API int __stdcall HH5PLAYER_OpenStream(USHORT nPort);
//输入播放视频
DLLEXPORT_API int __stdcall HH5PLAYER_PutDecStreamData(USHORT nPort,PBYTE pBuf, DWORD nSize, int nDataType);
//输入播放视频扩展
DLLEXPORT_API int __stdcall HH5PLAYER_PutDecStreamDataEx(USHORT nPort,PBYTE pBuf, DWORD nSize, UINT nDataType, HH5KAV_INFO *pAVInfo);	
//停止播放视频
DLLEXPORT_API int __stdcall HH5PLAYER_StopStream(USHORT nPort);
//关闭播放视频
DLLEXPORT_API int __stdcall HH5PLAYER_CloseStream(USHORT nPort);
//取得视频帧大小
DLLEXPORT_API int __stdcall HH5PLAYER_GetStreamFrameSize(USHORT nPort);
//取得音频帧大小
DLLEXPORT_API int __stdcall HH5PLAYER_GetAudioFrameSize(USHORT nPort);
//设置播放速度
DLLEXPORT_API int __stdcall HH5PLAYER_SetStreamPlaySpeed(USHORT nPort, USHORT nSpeed);
//取得播放速度
DLLEXPORT_API int __stdcall HH5PLAYER_GetStreamPlaySpeed(USHORT nPort);
//设置播放速度2, fValue:每帧播放时间, 如: 40ms, 33.3ms, bDelayAdjust:是否在有丢帧时, 校准时间(注:若是对关键帧播放,一定要置为false)
DLLEXPORT_API int __stdcall HH5PLAYER_SetStreamPlaySpeed2(USHORT nPort,float fValue,bool bDelayAdjust = true);

//------------------------------  4、其它播放操作 ----------------------------------//
//设置音频
DLLEXPORT_API int __stdcall HH5PLAYER_SetAudio(USHORT nPort,bool bEnabled);       
//刷新图象  
DLLEXPORT_API int __stdcall HH5PLAYER_UpDateImage(USHORT nPort);					
//刷新显示窗口边界
DLLEXPORT_API int __stdcall HH5PLAYER_UpdateBounds(USHORT nPort);					
//设置显示窗口  
DLLEXPORT_API int __stdcall HH5PLAYER_SetDisplayhWnd(USHORT nPort,HWND hWnd);		
//局部放大显示
DLLEXPORT_API int __stdcall HH5PLAYER_SetPartDisplay(USHORT nPort,HWND hPartWnd,RECT *pPartRect);	
//捕捉当前的图像
DLLEXPORT_API int __stdcall HH5PLAYER_CaptureOnePicture(USHORT nPort,void **bmpbuf,int &bmpsize);	
//获取音量
DLLEXPORT_API int __stdcall HH5PLAYER_GetVolume(USHORT nPort, long *lpVolume);		
//设置音量
DLLEXPORT_API int __stdcall HH5PLAYER_SetVolume(USHORT nPort, long lVolume);		
//显示质量设置 0: 高质量  1: 低质量
DLLEXPORT_API int __stdcall HH5PLAYER_SetDecoderQulity(bool bQulity);				
//显示控制	bZoomIn:大画面显示,bDeInterlace:消隔行锯齿,bPlayMode  TRUE  流畅优先	FALSE 实时优先
DLLEXPORT_API int __stdcall HH5PLAYER_SetDecoderParam(bool bZoomIn, bool bDeInterlace, bool bPlayMode = FALSE);		
//当流畅性优先时，缓存的帧数(1 ~ 60)
DLLEXPORT_API int __stdcall HH5PLAYER_SetVideoBufferSize(int nFrameNum = 15);		
//清除所有缓冲
DLLEXPORT_API int __stdcall HH5PLAYER_ClearAllBuffer(USHORT nPort);										
//清除视频缓冲
DLLEXPORT_API int __stdcall HH5PLAYER_ClearVideoBuffer(USHORT nPort);	
//清除音频缓冲	
DLLEXPORT_API int __stdcall HH5PLAYER_ClearAudioBuffer(USHORT nPort);		
//清除DirctX表面
DLLEXPORT_API int __stdcall HH5PLAYER_ClearImage(USHORT nPort, DWORD dwColor = 0);	


//------------------------------  5、音频编解码 ----------------------------------//
//音频编码
DLLEXPORT_API int   __stdcall HH5PLAYER_AudioEncodeData(unsigned short *BuffIn,   int InNum, BYTE *BuffOut);
//音频解码
DLLEXPORT_API int   __stdcall HH5PLAYER_AudioDecodeData(BYTE *BuffIn,  int InNum, unsigned short  *BuffOut);

//G.726音频编码 默认G.726 16kbps
DLLEXPORT_API int   __stdcall HH5PLAYER_G726AEncodeData(unsigned short *BuffIn,   int InNum, BYTE *BuffOut);
//G.726音频解码
DLLEXPORT_API int   __stdcall HH5PLAYER_G726ADecodeData(BYTE *BuffIn,  int InNum, unsigned short  *BuffOut);

#define		HH5PLAYER_G722AEncodeData	HH5PLAYER_AudioEncodeData
#define		HH5PLAYER_G722ADecodeData	HH5PLAYER_AudioDecodeData

/*****************************************************************************/
/********************************  三、新增函数接口  *************************/
/*****************************************************************************/

//------------------------------  1、新增函数1: 音频编解码 ----------------------------------//
//针HH98系列和HH58系列设备提供的接口，可以编解码G.711 G.726 ADPCM格式

//音频编码扩展
DLLEXPORT_API int   __stdcall HH5PLAYER_AudioEncodeDataEx(unsigned short *BuffIn,   int InNum, BYTE *BuffOut, UINT nEncodeType, UINT nBitrate);
//音频解码扩展
DLLEXPORT_API int   __stdcall HH5PLAYER_AudioDecodeDataEx(BYTE *BuffIn,  int InNum, unsigned short  *BuffOut, UINT nEncodeType, UINT nBitrate);

//------------------------------  2、新增函数2: 视频解码及视频操作 ----------------------------------//
//选择新的H.264解码 nVersion 1:old(123),2:new(125)
DLLEXPORT_API int   __stdcall HH5PLAYER_SelectH264(USHORT nPort,UINT nVersion);		
//抗裂痕功能开关 false: 关  true: 开		
DLLEXPORT_API int   __stdcall HH5PLAYER_WaitForVerticalBlank(bool bWait = false);			

//注: 当要在在视频图像上叠加图片、画线等时，请用以下函数初始化播放器，并将bSupportDraw设为true(在播放窗口上支持外部或内部HDC)
//初始化播放2
DLLEXPORT_API int   __stdcall HH5PLAYER_InitPlayer2(USHORT nPort,HWND hWnd,bool bSupportDraw = false);
//播放画图
DLLEXPORT_API int   __stdcall HH5PLAYER_DrawImage( USHORT nPort,int x,int y,unsigned char *pBmpImage,int nSize);
//播放文本OSD
DLLEXPORT_API int   __stdcall HH5PLAYER_DrawText(  USHORT nPort,int x,int y,char *pszText,LOGFONT ft,COLORREF crText,COLORREF crBack = -1/*crBack -1: 透明*/);
//设置画笔
DLLEXPORT_API int   __stdcall HH5PLAYER_SetDrawPen(USHORT nPort,int nPenStyle,int nWidth, COLORREF crColor);
//画线
DLLEXPORT_API int   __stdcall HH5PLAYER_DrawLine(  USHORT nPort,int x1,int y1,int x2,int y2);
//画框
DLLEXPORT_API int   __stdcall HH5PLAYER_DrawRect(  USHORT nPort,int x1,int y1,int x2,int y2);
//清除显示
DLLEXPORT_API int   __stdcall HH5PLAYER_ClearDraw( USHORT nPort,int nType);					//nType: 0 all, 1 image, 2 text, 3 line, rect

typedef int (WINAPI *HHCBOnDraw)(USHORT nPort, HDC hdc, int nWidth, int nHeight, void *pContext);	//支持外部调用HDC
DLLEXPORT_API int   __stdcall HH5PLAYER_RegCBOnDraw(USHORT nPort, HHCBOnDraw pCBOnDraw, void *pContext = NULL);

//------------------------------  3、新增函数2: 对讲 ----------------------------------//
//typedef int (WINAPI *HHTalkCaptureData)(BYTE *pBuffer, int nBufLen, DWORD dwContext = 0, void *pContext = NULL);
typedef int (WINAPI *HHTalkCaptureData)(BYTE *pBuffer, int nBufLen, DWORD dwContext, void *pContext);
//初始化对讲
DLLEXPORT_API int __stdcall HH5PLAYER_TKInit(  HWND hWnd, HANDLE &hTalk);
DLLEXPORT_API int __stdcall HH5PLAYER_TKRegCaptureDataCB(HANDLE hTalk,HHTalkCaptureData pCBTalk,DWORD dwContext = 0,void *pContext = NULL);

//开始对讲
DLLEXPORT_API int __stdcall HH5PLAYER_TKStart( HANDLE hTalk,WAVEFORMATEX *pInFormat, WAVEFORMATEX *pOutFormat);  
//结束对讲
DLLEXPORT_API int __stdcall HH5PLAYER_TKStop(  HANDLE hTalk); 
//发送本地对讲数据
DLLEXPORT_API int __stdcall HH5PLAYER_TKSendToPCData(HANDLE hTalk, BYTE *pBuffer,int nBufLen);
//设置对讲音频
DLLEXPORT_API int __stdcall HH5PLAYER_TKSetVolume(   HANDLE hTalk, long lVolume);	
//取得对讲音频
DLLEXPORT_API int __stdcall HH5PLAYER_TKGetVolume(   HANDLE hTalk, long *lpVolume);
//对讲释放
DLLEXPORT_API int __stdcall HH5PLAYER_TKRelease(HANDLE hTalk);

/*****************************************************************************/
/*******************************  四、不再支持的函数  ************************/
/*****************************************************************************/
//================================================================
//在新版中，不再支持的函数 
//           1. 多显卡的支持由内部处理
//           2. 在视频图像上叠加图片、画线等，可以通过新增函数实现
//================================================================
DLLEXPORT_API int __stdcall HH5PLAYER_SetDisplayGUID(char *pGuid);				
DLLEXPORT_API int __stdcall HH5PLAYER_SetDisplayGUIDEx(USHORT nPort, char *pGuid);
DLLEXPORT_API int __stdcall HH5PLAYER_SetDisplayWndEx(USHORT nPort,  unsigned int nFirstScreenWidth);

typedef int (WINAPI *YUVDataCallBack)(USHORT nPort,	
							     unsigned char *YBuf,
							     unsigned char *UBuf,
							     unsigned char *VBuf,
								 unsigned int  nYStride,
								 unsigned int  nUVStride,
								 unsigned int  nWidth,
								 unsigned int  nHeight,
							     unsigned int  nViFormat 	
								 );

DLLEXPORT_API int __stdcall HH5PLAYER_RegYUVDataCallBack(YUVDataCallBack pYUVDataCallBack, bool nDispImage);

typedef int (WINAPI *DrawImageCallBack)(USHORT nPort,	
							     HDC hdc,
							     unsigned char *pDispBuf,
								 unsigned int  nStride,
								 unsigned int  nDDrawMode
								 );
DLLEXPORT_API int __stdcall HH5PLAYER_RegDrawImageCallBack(DrawImageCallBack pDrawImageCallBack);


#endif  
