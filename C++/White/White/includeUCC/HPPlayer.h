#ifndef _HPPLAYER_H__
#define _HPPLAYER_H__
#define HPAPI __stdcall
//基本接口，暂时只使用这些接口
BOOL  HPAPI HPPlay_OpenFile(LONG nPort,LPSTR sFileName);
BOOL  HPAPI HPPlay_CloseFile(LONG nPort);
BOOL  HPAPI HPPlay_Play(LONG nPort, HWND hWnd);
BOOL  HPAPI HPPlay_Stop(LONG nPort);
BOOL  HPAPI HPPlay_Pause(LONG nPort,DWORD nPause);
BOOL  HPAPI HPPlay_Fast(LONG nPort);
BOOL  HPAPI HPPlay_Slow(LONG nPort);
BOOL  HPAPI HPPlay_OneByOne(LONG nPort);
BOOL  HPAPI HPPlay_OneByOneBack(LONG nPort);
BOOL  HPAPI HPPlay_SetCurrentFrameNum(LONG nPort,DWORD nFrameNum);
BOOL  HPAPI HPPlay_SetPlayedTimeEx(LONG nPort,DWORD nTime);

//enum{S1_16=-4,S1_8=-3,S1_4=-2,S1_2=-1,S1=0,S2=1,S4=2,S8=3,S16=4}
int	  HPAPI HPPlay_GetSpeed(LONG nPort);

#define HPPlay_OneByOneForward HPAPI HPPlay_OneByOne
#define HPPlay_SeekFrame	HPAPI HPPlay_SetCurrentFrameNum

BOOL  HPAPI HPPlay_PlaySound(LONG nPort);
BOOL  HPAPI HPPlay_StopSound(void);
BOOL  HPAPI HPPlay_StopSoundEx(LONG nPort);

BOOL  HPAPI HPPlay_SetVolume(LONG nPort,WORD nVolume);
WORD HPAPI HPPlay_GetVolume(LONG nPort);

BOOL  HPAPI HPPlay_SetFileEndMsg(LONG nPort,HWND hWnd,UINT nMsg);
DWORD HPAPI HPPlay_GetFileTime(LONG nPort);
DWORD HPAPI HPPlay_GetPlayedTime(LONG nPort);
DWORD HPAPI HPPlay_GetCurrentFrameNum(LONG nPort);
DWORD HPAPI HPPlay_GetFileTotalFrames(LONG nPort);
BOOL HPAPI HPPlay_GetPictureSize(LONG nPort,LONG *pWidth,LONG *pHeight);
DWORD HPAPI HPPlay_GetSdkVersion();
DWORD HPAPI HPPlay_GetLastError(LONG nPort);
BOOL HPAPI HPPlay_SetColor(LONG nPort, DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue);

DWORD HPAPI HPPlay_GetPlayedFrames(LONG nPort);
BOOL  HPAPI HPPlay_SetPlayPos(LONG nPort,float fRelativePos);
float HPAPI HPPlay_GetPlayPos(LONG nPort);
BOOL HPAPI HPPlay_SetDisplayCallBack(LONG nPort,void (CALLBACK* DisplayCBFun)(long nPort,char * pBuf,long nSize,long nWidth,long nHeight,long nStamp,long nType,long nReserved));
BOOL HPAPI HPPlay_SetAudioCallBack(LONG nPort, void (__stdcall * funAudio)(long nPort, char * pAudioBuf, long nSize, long nStamp, long nType, long nUser), long nUser);
BOOL HPAPI HPPlay_SetEncChangeMsg(LONG nPort,HWND hWnd,UINT nMsg);
BOOL HPAPI HPPlay_RefreshPlay(LONG nPort);
DWORD HPAPI HPPlay_GetBufferValue(LONG nPort,DWORD nBufType);
COLORREF HPAPI HPPlay_GetColorKey(LONG nPort);
BOOL HPAPI HPPlay_RefreshPlayEx(LONG nPort,DWORD nRegionNum);
BOOL HPAPI HPPlay_ResetBuffer(LONG nPort,DWORD nBufType);
BOOL HPAPI HPPlay_SetTimerType(LONG nPort,DWORD nTimerType,DWORD nReserved);
BOOL HPAPI HPPlay_GetJPEG(LONG nPort,PBYTE pJpeg,DWORD nBufSize,DWORD* pJpegSize);
BOOL HPAPI HPPlay_GetBMP(LONG nPort,PBYTE pBitmap,DWORD nBufSize,DWORD* pBmpSize);
BOOL HPAPI HPPlay_SetPicQuality(LONG nPort,BOOL bHighQuality);
BOOL HPAPI HPPlay_InputData(LONG nPort,PBYTE pBuf,DWORD nSize);
BOOL HPAPI HPPlay_SetDisplayType(LONG nPort,LONG nType);
BOOL HPAPI HPPlay_OpenStream(LONG nPort,PBYTE pFileHeadBuf,DWORD nSize,DWORD nBufPoolSize);
BOOL HPAPI HPPlay_GetNextKeyFramePos(LONG nPort,DWORD nValue, DWORD nType, PFRAME_POS pFramePos);
BOOL HPAPI HPPlay_ResetSourceBufFlag(LONG nPort);
BOOL HPAPI HPPlay_SetFileRefCallBack(LONG nPort, void (__stdcall *pFileRefDone)(DWORD nPort,DWORD nUser),DWORD nUser);
BOOL HPAPI HPPlay_SetSourceBufCallBack(LONG nPort,DWORD nThreShold,void (CALLBACK * SourceBufCallBack)(long nPort,DWORD nBufSize,DWORD dwUser,void*pResvered),DWORD dwUser,void *pReserved);
BOOL HPAPI HPPlay_SetStreamOpenMode(LONG nPort,DWORD nMode);
BOOL HPAPI HPPlay_CloseStream(LONG nPort);
BOOL HPAPI HPPlay_SetDisplayRegion(LONG nPort,DWORD nRegionNum, RECT *pSrcRect, HWND hDestWnd, BOOL bEnable);
BOOL HPAPI HPPlay_SetDecCallBackMend(LONG nPort,void (CALLBACK* DecCBFun)(long nPort,char * pBuf,long nSize,FRAME_INFO * pFrameInfo, long nReserved1,long nReserved2), long nUser);
BOOL HPAPI HPPlay_SetDeflash(LONG nPort,BOOL bDeflash);
BOOL HPAPI HPPlay_GetTimerType(LONG nPort,DWORD *pTimerType,DWORD *pReserved);
BOOL HPAPI HPPlay_GetKeyFramePos(LONG nPort,DWORD nValue, DWORD nType, PFRAME_POS pFramePos);
BOOL HPAPI HPPlay_GetPictureQuality(LONG nPort,BOOL *bHighQuality);
BOOL HPAPI HPPlay_ThrowBFrameNum(LONG nPort,DWORD nNum);
BOOL HPAPI HPPlay_ResetSourceBuffer(LONG nPort);
DWORD HPAPI HPPlay_GetDisplayBuf(LONG nPort);
BOOL HPAPI HPPlay_CatchPic(LONG nPort, LPSTR sFileName);
DWORD HPAPI HPPlay_GetFileHeadLength();
BOOL  HPAPI HPPlay_OpenFileEx(LONG nPort,LPSTR sFileName, HWND hWnd);
DWORD HPAPI HPPlay_GetPlayedTimeEx(LONG nPort);
void HPAPI HPPlay_SetVideoPos(LONG nPort, int x, int y, int cx, int cy);
#endif
