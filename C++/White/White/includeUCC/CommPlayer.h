#ifndef _COMMPLAYER_H__
#define _COMMPLAYER_H__
#define COMMAPI __stdcall
//基本接口，暂时只使用这些接口
BOOL  COMMAPI CommPlay_OpenFile(LONG nPort,LPSTR sFileName);
BOOL  COMMAPI CommPlay_CloseFile(LONG nPort);
BOOL  COMMAPI CommPlay_Play(LONG nPort, HWND hWnd);
BOOL  COMMAPI CommPlay_Stop(LONG nPort);
BOOL  COMMAPI CommPlay_Pause(LONG nPort,DWORD nPause);
BOOL  COMMAPI CommPlay_Fast(LONG nPort);
BOOL  COMMAPI CommPlay_Slow(LONG nPort);
BOOL  COMMAPI CommPlay_OneByOne(LONG nPort);
BOOL  COMMAPI CommPlay_OneByOneBack(LONG nPort);
BOOL  COMMAPI CommPlay_SetCurrentFrameNum(LONG nPort,DWORD nFrameNum);
BOOL  COMMAPI CommPlay_SetPlayedTimeEx(LONG nPort,DWORD nTime);

//enum{S1_16=-4,S1_8=-3,S1_4=-2,S1_2=-1,S1=0,S2=1,S4=2,S8=3,S16=4}
int	  COMMAPI CommPlay_GetSpeed(LONG nPort);

#define CommPlay_OneByOneForward COMMAPI CommPlay_OneByOne
#define CommPlay_SeekFrame	COMMAPI CommPlay_SetCurrentFrameNum

BOOL  COMMAPI CommPlay_PlaySound(LONG nPort);
BOOL  COMMAPI CommPlay_StopSound(void);
BOOL  COMMAPI CommPlay_StopSoundEx(LONG nPort);

BOOL  COMMAPI CommPlay_SetVolume(LONG nPort,WORD nVolume);
WORD COMMAPI CommPlay_GetVolume(LONG nPort);

BOOL  COMMAPI CommPlay_SetFileEndMsg(LONG nPort,HWND hWnd,UINT nMsg);
DWORD COMMAPI CommPlay_GetFileTime(LONG nPort);
DWORD COMMAPI CommPlay_GetPlayedTime(LONG nPort);
DWORD COMMAPI CommPlay_GetCurrentFrameNum(LONG nPort);
DWORD COMMAPI CommPlay_GetFileTotalFrames(LONG nPort);
BOOL COMMAPI CommPlay_GetPictureSize(LONG nPort,LONG *pWidth,LONG *pHeight);
DWORD COMMAPI CommPlay_GetSdkVersion();
DWORD COMMAPI CommPlay_GetLastError(LONG nPort);
BOOL COMMAPI CommPlay_SetColor(LONG nPort, DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue);

DWORD COMMAPI CommPlay_GetPlayedFrames(LONG nPort);
BOOL  COMMAPI CommPlay_SetPlayPos(LONG nPort,float fRelativePos);
float COMMAPI CommPlay_GetPlayPos(LONG nPort);
BOOL COMMAPI CommPlay_SetDisplayCallBack(LONG nPort,void (CALLBACK* DisplayCBFun)(long nPort,char * pBuf,long nSize,long nWidth,long nHeight,long nStamp,long nType,long nReserved));
BOOL COMMAPI CommPlay_SetAudioCallBack(LONG nPort, void (__stdcall * funAudio)(long nPort, char * pAudioBuf, long nSize, long nStamp, long nType, long nUser), long nUser);
BOOL COMMAPI CommPlay_SetEncChangeMsg(LONG nPort,HWND hWnd,UINT nMsg);
BOOL COMMAPI CommPlay_RefreshPlay(LONG nPort);
DWORD COMMAPI CommPlay_GetBufferValue(LONG nPort,DWORD nBufType);
COLORREF COMMAPI CommPlay_GetColorKey(LONG nPort);
BOOL COMMAPI CommPlay_RefreshPlayEx(LONG nPort,DWORD nRegionNum);
BOOL COMMAPI CommPlay_ResetBuffer(LONG nPort,DWORD nBufType);
BOOL COMMAPI CommPlay_SetTimerType(LONG nPort,DWORD nTimerType,DWORD nReserved);
BOOL COMMAPI CommPlay_GetJPEG(LONG nPort,PBYTE pJpeg,DWORD nBufSize,DWORD* pJpegSize);
BOOL COMMAPI CommPlay_GetBMP(LONG nPort,PBYTE pBitmap,DWORD nBufSize,DWORD* pBmpSize);
BOOL COMMAPI CommPlay_SetPicQuality(LONG nPort,BOOL bHighQuality);
BOOL COMMAPI CommPlay_InputData(LONG nPort,PBYTE pBuf,DWORD nSize);
BOOL COMMAPI CommPlay_SetDisplayType(LONG nPort,LONG nType);
BOOL COMMAPI CommPlay_OpenStream(LONG nPort,PBYTE pFileHeadBuf,DWORD nSize,DWORD nBufPoolSize);
BOOL COMMAPI CommPlay_GetNextKeyFramePos(LONG nPort,DWORD nValue, DWORD nType, PFRAME_POS pFramePos);
BOOL COMMAPI CommPlay_ResetSourceBufFlag(LONG nPort);
BOOL COMMAPI CommPlay_SetFileRefCallBack(LONG nPort, void (__stdcall *pFileRefDone)(DWORD nPort,DWORD nUser),DWORD nUser);
BOOL COMMAPI CommPlay_SetSourceBufCallBack(LONG nPort,DWORD nThreShold,void (CALLBACK * SourceBufCallBack)(long nPort,DWORD nBufSize,DWORD dwUser,void*pResvered),DWORD dwUser,void *pReserved);
BOOL COMMAPI CommPlay_SetStreamOpenMode(LONG nPort,DWORD nMode);
BOOL COMMAPI CommPlay_CloseStream(LONG nPort);
BOOL COMMAPI CommPlay_SetDisplayRegion(LONG nPort,DWORD nRegionNum, RECT *pSrcRect, HWND hDestWnd, BOOL bEnable);
BOOL COMMAPI CommPlay_SetDecCallBackMend(LONG nPort,void (CALLBACK* DecCBFun)(long nPort,char * pBuf,long nSize,FRAME_INFO * pFrameInfo, long nReserved1,long nReserved2), long nUser);
BOOL COMMAPI CommPlay_SetDeflash(LONG nPort,BOOL bDeflash);
BOOL COMMAPI CommPlay_GetTimerType(LONG nPort,DWORD *pTimerType,DWORD *pReserved);
BOOL COMMAPI CommPlay_GetKeyFramePos(LONG nPort,DWORD nValue, DWORD nType, PFRAME_POS pFramePos);
BOOL COMMAPI CommPlay_GetPictureQuality(LONG nPort,BOOL *bHighQuality);
BOOL COMMAPI CommPlay_ThrowBFrameNum(LONG nPort,DWORD nNum);
BOOL COMMAPI CommPlay_ResetSourceBuffer(LONG nPort);
DWORD COMMAPI CommPlay_GetDisplayBuf(LONG nPort);
BOOL COMMAPI CommPlay_CatchPic(LONG nPort, LPSTR sFileName);
DWORD COMMAPI CommPlay_GetFileHeadLength();
BOOL  COMMAPI CommPlay_OpenFileEx(LONG nPort,LPSTR sFileName, HWND hWnd);
DWORD COMMAPI CommPlay_GetPlayedTimeEx(LONG nPort);
void COMMAPI CommPlay_SetVideoPos(LONG nPort, int x, int y, int cx, int cy);
#endif
