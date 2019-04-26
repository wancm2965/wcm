#ifndef _AVCONPLAYEX_H__
#define _AVCONPLAYEX_H__

//基本接口，暂时只使用这些接口
#include "AvconPlayer.h"

//其它接口
DWORD AVCON_Play_GetFileHeadLength();
int AVCON_Play_GetCapsEx(DWORD nDDrawDeviceNum);
typedef struct{
	long nFilePos;
	long nFrameNum;
	long nFrameTime;
	long nErrorFrameNum;
	SYSTEMTIME *pErrorTime;
	long nErrorLostFrameNum;
	long nErrorFrameSize;
}FRAME_POS,*PFRAME_POS;

//Frame Info
typedef struct{
	long nWidth;
	long nHeight;
	long nStamp;
	long nType;
	long nFrameRate;
	DWORD dwFrameNum;
}FRAME_INFO;

//Frame 
typedef struct{
	char *pDataBuf;
	long  nSize;
	long  nFrameNum;
	BOOL  bIsAudio;
	long  nReserved;
}FRAME_TYPE;

//Watermark Info	//add by gb 080119
typedef struct{
	char *pDataBuf;
	long  nSize;
	long  nFrameNum;
	BOOL  bRsaRight;
	long  nReserved;
}WATERMARK_INFO;

//Locate by
#define BY_FRAMENUM  1
#define BY_FRAMETIME 2
//Wave coef range;
#define MIN_WAVE_COEF -100
#define MAX_WAVE_COEF 100
//Timer type
#define TIMER_1 1 //Only 16 timers for every process.Default TIMER;
#define TIMER_2 2 //Not limit;But the precision less than TIMER_1; 
//BUFFER TYPE
#define BUF_VIDEO_SRC 1
#define BUF_AUDIO_SRC 2
#define BUF_VIDEO_RENDER 3
#define BUF_AUDIO_RENDER 4
///
DWORD AVCON_Play_GetPlayedFrames(LONG nPort);
BOOL  AVCON_Play_SetPlayPos(LONG nPort,float fRelativePos);
float AVCON_Play_GetPlayPos(LONG nPort);
BOOL AVCON_Play_SetDisplayCallBack(LONG nPort,void (CALLBACK* DisplayCBFun)(long nPort,char * pBuf,long nSize,long nWidth,long nHeight,long nStamp,long nType,long nReserved));
BOOL AVCON_Play_SetAudioCallBack(LONG nPort, void (__stdcall * funAudio)(long nPort, char * pAudioBuf, long nSize, long nStamp, long nType, long nUser), long nUser);
BOOL AVCON_Play_SetEncChangeMsg(LONG nPort,HWND hWnd,UINT nMsg);
DWORD AVCON_Play_GetBufferValue(LONG nPort,DWORD nBufType);
COLORREF AVCON_Play_GetColorKey(LONG nPort);
BOOL AVCON_Play_RefreshPlayEx(LONG nPort,DWORD nRegionNum);
BOOL AVCON_Play_ResetBuffer(LONG nPort,DWORD nBufType);
BOOL AVCON_Play_SetTimerType(LONG nPort,DWORD nTimerType,DWORD nReserved);
BOOL AVCON_Play_GetJPEG(LONG nPort,PBYTE pJpeg,DWORD nBufSize,DWORD* pJpegSize);
BOOL AVCON_Play_GetBMP(LONG nPort,PBYTE pBitmap,DWORD nBufSize,DWORD* pBmpSize);
BOOL AVCON_Play_SetPicQuality(LONG nPort,BOOL bHighQuality);
BOOL AVCON_Play_InputData(LONG nPort,PBYTE pBuf,DWORD nSize);
BOOL AVCON_Play_SetDisplayType(LONG nPort,LONG nType);
BOOL AVCON_Play_OpenStream(LONG nPort,PBYTE pFileHeadBuf,DWORD nSize,DWORD nBufPoolSize);
BOOL AVCON_Play_GetNextKeyFramePos(LONG nPort,DWORD nValue, DWORD nType, PFRAME_POS pFramePos);
BOOL AVCON_Play_ResetSourceBufFlag(LONG nPort);
BOOL AVCON_Play_SetFileRefCallBack(LONG nPort, void (__stdcall *pFileRefDone)(DWORD nPort,DWORD nUser),DWORD nUser);
BOOL AVCON_Play_SetSourceBufCallBack(LONG nPort,DWORD nThreShold,void (CALLBACK * SourceBufCallBack)(long nPort,DWORD nBufSize,DWORD dwUser,void*pResvered),DWORD dwUser,void *pReserved);
BOOL AVCON_Play_SetStreamOpenMode(LONG nPort,DWORD nMode);
BOOL AVCON_Play_CloseStream(LONG nPort);
BOOL AVCON_Play_SetDisplayRegion(LONG nPort,DWORD nRegionNum, RECT *pSrcRect, HWND hDestWnd, BOOL bEnable);
BOOL AVCON_Play_SetDecCallBackMend(LONG nPort,void (CALLBACK* DecCBFun)(long nPort,char * pBuf,long nSize,FRAME_INFO * pFrameInfo, long nReserved1,long nReserved2), long nUser);
BOOL AVCON_Play_SetDeflash(LONG nPort,BOOL bDeflash);
BOOL AVCON_Play_GetTimerType(LONG nPort,DWORD *pTimerType,DWORD *pReserved);
BOOL AVCON_Play_GetKeyFramePos(LONG nPort,DWORD nValue, DWORD nType, PFRAME_POS pFramePos);
BOOL AVCON_Play_GetPictureQuality(LONG nPort,BOOL *bHighQuality);
BOOL AVCON_Play_ThrowBFrameNum(LONG nPort,DWORD nNum);
BOOL AVCON_Play_ResetSourceBuffer(LONG nPort);
DWORD AVCON_Play_GetDisplayBuf(LONG nPort);
#endif