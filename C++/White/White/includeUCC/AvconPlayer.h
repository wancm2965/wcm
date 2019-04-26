#ifndef _AVCONPLAY_H__
#define _AVCONPLAY_H__
#include "HPDEV_Define.h"


//�����ӿڣ���ʱֻʹ����Щ�ӿ�
BOOL  AVCON_Play_OpenFile(LONG nPort,LPSTR sFileName);
BOOL  AVCON_Play_CloseFile(LONG nPort);
BOOL  AVCON_Play_Play(LONG nPort, HWND hWnd);
BOOL  AVCON_Play_Stop(LONG nPort);
BOOL  AVCON_Play_Pause(LONG nPort,DWORD nPause);
BOOL  AVCON_Play_Fast(LONG nPort);
BOOL  AVCON_Play_Slow(LONG nPort);
BOOL  AVCON_Play_OneByOne(LONG nPort);
BOOL  AVCON_Play_OneByOneBack(LONG nPort);
BOOL  AVCON_Play_SetCurrentFrameNum(LONG nPort,DWORD nFrameNum);
BOOL  AVCON_Play_SetPlayedTimeEx(LONG nPort,DWORD nTime/*����*/);

//enum{S1_16=-4,S1_8=-3,S1_4=-2,S1_2=-1,S1=0,S2=1,S4=2,S8=3,S16=4}
int	  AVCON_Play_GetSpeed(LONG nPort);

#define AVCON_Play_OneByOneForward AVCON_Play_OneByOne
#define AVCON_Play_SeekFrame	AVCON_Play_SetCurrentFrameNum

BOOL  AVCON_Play_PlaySound(LONG nPort);
BOOL  AVCON_Play_StopSound(LONG nPort);

BOOL  AVCON_Play_SetVolume(LONG nPort,WORD nVolume);
WORD  AVCON_Play_GetVolume(LONG nPort);

BOOL  AVCON_Play_SetFileEndMsg(LONG nPort,HWND hWnd,UINT nMsg);
DWORD AVCON_Play_GetFileTime(LONG nPort);
DWORD AVCON_Play_GetPlayedTime(LONG nPort);
DWORD AVCON_Play_GetCurrentFrameNum(LONG nPort);
DWORD AVCON_Play_GetFileTotalFrames(LONG nPort);
BOOL  AVCON_Play_GetPictureSize(LONG nPort,LONG *pWidth,LONG *pHeight);

DWORD AVCON_Play_GetSdkVersion();
DWORD AVCON_Play_GetLastError(LONG nPort);

//Ver6.2.1
/*nRegionNumĬ��Ϊ0, nBrightness nContrast nSaturation ����ǰĬ��Ϊ64��nHue����ǰĬ��Ϊ0��ȡֵ��Χ0-127*/
BOOL  AVCON_Play_SetColor(LONG nPort, DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue);
BOOL  AVCON_Play_CaptureFile(LONG nPort, LPSTR sFileName);
BOOL  AVCON_Play_RefreshPlay(LONG nPort);
DWORD AVCON_Play_GetCurrentFrameRate(LONG nPort);/*��ʱ��Ч*/

//Ver6.2.2
/* 
   ֻ�Ա����ļ���Ч
   bByTime=TRUEʱ
	0��<=dwFirst<dwLast<=��ʱ�䳤��(��)
   bByTime=FALSEʱ
    0֡<=dwFirst<=dwLast<=��֡����dwFirst���Զ���λ����ǰ֡���ڵĹؼ�֡

	nDstFileType = 0 ��ʽͬԴ�ļ���ʽ����ֻ֧�ֺ�����avms�ļ���dav�ļ�δ���ԣ�
				 = 1 avms���ݲ�֧�֣�
				 = 2 mpms���ݲ�֧�֣�
				 = 3 mp4ֻ֧�ֺ���¼���ݲ�֧�֣�
				 = 4 davֻ֧�ִ�¼���ݲ�֧�֣�
				 = 5 avi���ݲ�֧�֣�
*/
BOOL AVCON_Play_SaveStream(LONG nPort, LPSTR sDstFile, DWORD dwFirst, DWORD dwLast, BOOL bByTime, int nDstFileType);
char*  AVCON_Play_GetStreamType(LONG nPort);
BOOL AVCON_Play_GetColor(LONG nPort, DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue);
BOOL AVCON_Play_IsPlaySound(LONG nPort);
BOOL AVCON_Play_IsStop(LONG nPort);
BOOL AVCON_Play_IsPasue(LONG nPort);
BOOL AVCON_Play_IsSupportSeekFrame(LONG nPort);

//Ver6.2.3
BOOL AVCON_Play_OpenFileEx(LONG nPort,LPSTR sFileName, HWND hWnd);

//Ver6.2.4
void AVCON_Play_Initialize();
void AVCON_Play_Uninitialize();

//Ver6.2.6
BOOL AVCON_Play_IsSupportSaveStream(LONG nPort);

//Ver6.3
/* 
	������ͨ��ý���ļ��ĵ�֧��
*/
BOOL AVCON_Play_IsCommPlayer(LONG nPort);
void AVCON_Play_SetVideoPos(LONG nPort, int x, int y, int cx, int cy);

#endif