/********************************************************************
*	Copyright (C) 2008 - BlueStar. All Rights Reserved
*
*	Date Created:2008-01-01
*	Filename	:BSRPlayer.h
*	Author		:BlueStar luoyanhua
*	Description	:BlueStar MP4Player SDK interface declare
*********************************************************************/
#ifndef __BLUESTAR_BSRPLAYER_INTERFACE_H__
#define __BLUESTAR_BSRPLAYER_INTERFACE_H__

#include <windows.h>

#ifdef __cplusplus
#define BSRPLAYER_API extern "C"
#else
#define BSRPLAYER_API
#endif

#ifndef BSRPLAYER_STDCALL
#define BSRPLAYER_STDCALL __stdcall
#endif


//��Ч�������˿�
#define INVALID_PLAYERPORT			(-1)

//��Ч����ֵ
#define INVALID_SOUNDVOL			(-1)

//��Ч����ֵ
#define INVALID_PLAYPOS				(-1)

//��������֧�ֵ����·��
#define BSRPLAYER_MAXPORT			(256)		//���������֧��·��

//�������˿��Զ�������ʼ��
#define	BSRPLAYER_ALLOCBEGIN		(128)		//����ŷ�Χ(BSRPLAYER_ALLOCBEGIN��BSRPLAYER_MAXPORT)

//��ʾ����֡��
#define PLAYER_MINDISBUF			(2)			//��С��ʾ����֡
#define PLAYER_DEFDISBUF			(5)			//Ĭ����ʾ����֡
#define PLAYER_MAXDISBUF			(50)		//�����ʾ����֡

//��������������С
#define PLAYER_MINSOURCEBU		(1024*512)		//��Сֵ
#define PLAYER_DEFSOURCEBUF		(1024*1024*1)	//Ĭ��ֵ
#define PLAYER_MAXSOURCEBUF		(1024*1024*10)	//���ֵ

//Ĭ��֡��
#define PLAYER_DEFPALRATE			(25)		//PALĬ��֡��
#define PALYER_DEFNTSCRATE			(30)		//NTSCĬ��֡��

//�������Ƶ���ݻ����С
#define AUDIO_BUFFER_SIZE			(288)		//��ǰSDK����Ƶ֡�����Ĺ̶���С

//���ſ���
typedef enum
{
	PLAYER_SPEED_NORMAL			= 0x00,			//��������
	PLAYER_SPEED_PAUSE,							//��ͣ����
	PLAYER_SPEED_ONEBYONE,						//��֡����
	PLAYER_SPEED_1OF2TIMES,						//1/2��(����)
	PLAYER_SPEED_1OF4TIMES,						//1/4��(����)
	PLAYER_SPEED_1OF8TIMES,						//1/8��(����)
	PLAYER_SPEED_1OF16TIMES,					//1/16��(����)
	PLAYER_SPEED_2TIMES,						//2����(���)
	PLAYER_SPEED_4TIMES,						//4����(���)
	PLAYER_SPEED_8TIMES,						//8����(���)
	PLAYER_SPEED_16TIMES,						//16����(���)
	PLAYER_SPEED_MAXSPEED,						//������
	PLAYER_SPEED_IFRAME,						//I֡����
	PLAYER_SPEED_INVALID		= 0xFF,			//��Ч״̬
}BSRPLAYER_PLAYCTRL;



//���ʵ�����Χ
#define PLAYER_WAVECOEF_MIN			(0)			//������Сֵ
#define PLAYER_WAVECOEF_MAX			(100)		//�������ֵ

//������
typedef enum
{
	PLAYER_ERR_NOERROR				= 0,		//û�д���
	PLAYER_ERR_PORTNOTUSE,						//�˿�δʹ��
	PLAYER_ERR_PORTFULL,						//�˿���ռ��
	PLAYER_ERR_PARA_OVER,						//��������Ƿ�
	PLAYER_ERR_ORDER_ERROR,						//����˳�򲻶�
	PLAYER_ERR_TIMER_ERROR,						//��ʱ������ʧ��
	PLAYER_ERR_DEC_VIDEO_ERROR,					//��Ƶ����ʧ��
	PLAYER_ERR_DEC_AUDIO_ERROR,					//��Ƶ����ʧ��
	PLAYER_ERR_ALLOC_MEMORY_ERROR,				//�����ڴ�ʧ��
	PLAYER_ERR_OPEN_FILE_ERROR,					//�ļ�����ʧ��
	PLAYER_ERR_CREATE_OBJ_ERROR,				//�����߳��¼���ʧ��
	PLAYER_ERR_CREATE_DDRAW_ERROR,				//����DirectDrawʧ��
	PLAYER_ERR_CREATE_OFFSCREEN_ERROR,			//������˻���ʧ��
	PLAYER_ERR_BUF_OVER,						//��������,������ʧ��
	PLAYER_ERR_CREATE_SOUND_ERROR,				//������Ƶ�豸ʧ��	
	PLAYER_ERR_SET_VOLUME_ERROR,				//��������ʧ��
	PLAYER_ERR_SUPPORT_FILE_ONLY,				//ֻ���ڲ����ļ�ʱ����ʹ�ô˽ӿ�
	PLAYER_ERR_SUPPORT_STREAM_ONLY,				//ֻ���ڲ�����ʱ����ʹ�ô˽ӿ�
	PLAYER_ERR_SYS_NOT_SUPPORT,					//ϵͳ��֧��,������ֻ�ܹ�����Pentium 3����
	PLAYER_ERR_FILEHEADER_UNKNOWN,				//û���ļ�ͷ
	PLAYER_ERR_FILEHEADER_CRCERR,				//�ļ�ͷУ�����
	PLAYER_ERR_VERSION_INCORRECT,				//�������ͱ������汾����Ӧ 
	PLAYER_ERR_INITDECODER_ERROR,				//��ʼ��������ʧ��
	PLAYER_ERR_CHECK_FILE_ERROR,				//�ļ�̫�̻������޷�ʶ��
	PLAYER_ERR_INIT_TIMER_ERROR,				//��ý�嶨ʱ����ʼ��ʧ��
	PLAYER_ERR_BLT_ERROR,						//λ����ʧ��
	PLAYER_ERR_UPDATE_ERROR,					//��ʾOverlayʧ��
	PLAYER_ERR_OPEN_FILE_ERROR_MULTI,			//�ļ�����ʧ��,�����Ϊ������
	PLAYER_ERR_OPEN_FILE_ERROR_VIDEO,			//�ļ�����ʧ��,�����Ϊ��Ƶ��
	PLAYER_ERR_JPEG_COMPRESS_ERROR,				//JPEGѹ������
	PLAYER_ERR_EXTRACT_NOT_SUPPORT,				//��֧�ִ��ļ��İ汾
	PLAYER_ERR_EXTRACT_DATA_ERROR,				//�ͷ���Ƶ����ʧ��
	PLAYER_ERR_VALIDFLAG,						//��Ч�������ʶ
}BSRPLAYER_ERRORCODE;

//����ص���������
typedef enum
{
	PLAYER_DATATYPE_VIDEORGB	= 0,	//�����ΪRGB����
	PLAYER_DATATYPE_VIDEOYV12,			//�����ΪYUV����
	PLAYER_DATATYPE_AUDIOPCM			//�����ΪPCM����
}BSRPLAYER_DECDATATYPE;

//����֡��Ϣ
typedef struct tagBSRPLAYER_DECFRAMEINFO
{
	LONG nFrameSN;				//֡���,��ͨ������,����Ƶ����
	LONG nType;					//��������,BSRPLAYER_DECDATATYPE
	BOOL bIFlag;				//�Ƿ�I֡(��Ϊ��Ƶ�˲���������)
	LONG nWidth;				//�����,��λ����(��Ϊ��Ƶ�˲���������)
	LONG nHeight;				//�����,��λ����(��Ϊ��Ƶ�˲���������)
	LONG nFrameRate;			//����ʱ������ͼ��֡��(��Ϊ��Ƶ�˲���������)
	LONG nStamp;				//ʱ����Ϣ,��λ��,�����޷�������ȷ����ʱ��
}BSRPLAYER_DECFRAMEINFO, *PBSRPLAYER_DECFRAMEINFO;

//�⸴��֡��Ϣ
typedef struct tagBSRPLAYER_DEMUXFRAMEINFO
{
	LONG nFrameSN;				//֡���,��ͨ������,����Ƶ����
	BOOL bVideo;				//�Ƿ���Ƶ����
	BOOL bIFlag;				//�Ƿ�I֡(��Ϊ��Ƶ�˲���������)
	LONG nWidth;				//�����,��λ����(��Ϊ��Ƶ�˲���������)
	LONG nHeight;				//�����,��λ����(��Ϊ��Ƶ�˲���������)
	LONG nFrameRate;			//����ʱ������ͼ��֡��(��Ϊ��Ƶ�˲���������)
	LONG nStamp;				//ʱ����Ϣ,��λ��,�����޷�������ȷ����ʱ��
}BSRPLAYER_DEMUXFRAMEINFO, *PBSRPLAYER_DEMUXFRAMEINFO;

//��ʾ���ھ��HOOK�ص�
typedef void (CALLBACK *FUNCPLAYERHWNDHOOK)(LONG nPort, HWND *hWnd, LPVOID lpVoid);

//��Ļ���ӻص�timeΪ��׼��ʱ����������ʱ��(time_t����)
typedef void (CALLBACK *FUNCPLAYERTITLEDRAW)(LONG nPort, HDC hDC, UINT time, DWORD dwWidth, DWORD dwHeight, LPVOID lpVoid);

//�ļ���ȡ�ص�
typedef void (CALLBACK *FUNFILEREADPROC)(LONG nPort, BYTE *lpBuffer, DWORD dwSize, LPVOID lpVoid);

//����ǰ���ݻص�(Ŀǰ��ƵΪYV12����,��ƵΪMP2����,�Ժ���ܻ���������չ����)
typedef void (CALLBACK *FUNDECDATAPROC)(LONG nPort, BYTE *lpDataBuf, DWORD dwSize, BSRPLAYER_DECFRAMEINFO *lpFrameInfo, LPVOID lpVoid);

//�⸴�����ݻص�(����Ƶ֡����),��ƵΪMPEG4��ʽ,���Ե���XVID����;��ƵΪMP2��ʽ,���Ե�����Ƶ����ӿڽ��н���
typedef void (CALLBACK *FUNDEMUXDATAPROC)(LONG nPort, BYTE *lpDataBuf, DWORD dwSize, BSRPLAYER_DEMUXFRAMEINFO *lpFrameInfo, LPVOID lpVoid);

////////////////////////////API//////////////////////////////////////////
//�������
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_GetSDKVersion();
BSRPLAYER_API LPCSTR BSRPLAYER_STDCALL BSR_PLAYER_GetSDKUpdateInfos();
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_IsSupport();
BSRPLAYER_API VOID BSRPLAYER_STDCALL BSR_PLAYER_SetDebugPara(LONG nLevel, BOOL bScopeMode);

//�����봦��
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_GetLastError();
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_SetLastError(LONG nError);

//�˿ڴ���
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_AllocSysPort();
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_ReleaseSysPort(LONG nPort);

//��ƵԴ��
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_OpenFile(LONG nPort, LPCSTR lpszFileName);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_OpenStream(LONG nPort, BYTE *lpFileHeadBuf, DWORD dwSize, DWORD dwBufFrame);

//���ӹ���
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_SetFileEndMsg(LONG nPort, HWND hWnd, UINT nMsg);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_RigisterDrawFun(LONG nPort, FUNCPLAYERTITLEDRAW lpfnDrawfun, LPVOID lpVoid);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_RigisterWndHookFun(LONG nPort, FUNCPLAYERHWNDHOOK lpfnWndHookfun, LPVOID lpVoid);

//���ݷ���
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_RigisterDemuxCallBackFun(LONG nPort, FUNDEMUXDATAPROC lpfnDemuxFun, LPVOID lpVoid);

//����ص�
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_RigisterDecCallBackFun(LONG nPort, FUNDECDATAPROC lpfnDecFun, LPVOID lpVoid);

//�ļ�֡����
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_StartFileRead(LONG nPort, FUNFILEREADPROC lpfnReadfun, LPVOID lpVoid);

//���Ų���
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_Play(LONG nPort, HWND hWnd);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_Stop(LONG nPort);

//���ſ���
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_InputData(LONG nPort, BYTE *lpBuffer, DWORD dwSize, LONG nReserved);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_StopInput(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_PlayControl(LONG nPort, UINT nControl);
BSRPLAYER_API UINT BSRPLAYER_STDCALL BSR_PLAYER_GetPlayStat(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_RefreshPlay(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_SetDisplayBufNumber(LONG nPort, DWORD dwBufFrame);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_ResetDisplayBuf(LONG nPort);

//��ͼ����
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_CaptureBMPPicture(LONG nPort, LPCSTR lpszFileName);

//��֡����
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_GetAllowLostFrame(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_SetAllowLostFrame(LONG nPort, DWORD dwFrames);

//��Ƶ����
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_PlaySound(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_StopSound();
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_GetVolume(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_SetVolume(LONG nPort, LONG nVolume);
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_GetSoundPort();

//�ļ����Ų���
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_GetPlayPos(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_SetPlayPos(LONG nPort, LONG nRelativePos);

//��Ϣ��ȡ
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_GetCurFrameRate(LONG nPort);
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_GetPlayedTime(LONG nPort);
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_GetOSDTime(LONG nPort);
BSRPLAYER_API LPCSTR BSRPLAYER_STDCALL BSR_PLAYER_GetChannelName(LONG nPort);
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_GetPlayedFrames(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_GetPictureSize(LONG nPort, LONG *lpWidth, LONG *lpHeight);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_GetFileFrameAndTime(LONG nPort, DWORD *lpdwCount, DWORD *lpdwTime);

//��Ƶ�����
BSRPLAYER_API HANDLE BSRPLAYER_STDCALL BSR_PLAYER_InitAudioHandle();
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_EncodeAudioFrame(HANDLE lpAudioHandle, BYTE *lpInBuffer, BYTE *lpOutBuffer);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DecodeAudioFrame(HANDLE lpAudioHandle, BYTE *lpInBuffer, BYTE *lpOutBuffer);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_ReleaseAudioHandle(HANDLE lpAudioHandle);

/////////////////////////��ʵ��API/////////////////////////////////////////////////
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_DDrawDeviceGetCount();
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DDrawDeviceSet(LONG nPort, DWORD nDeviceNum);
/////////////////////////��ʵ��API/////////////////////////////////////////////////



/////////////////////////////�ļ�ϵͳ[��֧��ֻ������]/////////////////////////////////////////////
#define PLAYER_MAXDISK_NUM	(32)	//PC�����������̸���
#define PLAYER_ONEPAGE_NUM	(50)	//Ĭ��ÿһҳ�ļ��б���

//�����ļ����
typedef enum 
{
	PLAYERFILETYPE_ALL	= 0,	//ȫ��¼��
	PLAYERFILETYPE_MAMUAL,		//�ֶ�¼��
	PLAYERFILETYPE_TIMER,		//��ʱ¼��
	PLAYERFILETYPE_MOTODETEC,	//�ƶ����
	PLAYERFILETYPE_ALARM,		//����¼��
	PLAYERFILETYPE_CARDNO,		//����¼��
	PLAYERFILETYPE_MAX			//�������
}PLAYER_RECORDFILETYPE;

//��������Ϣ
typedef struct tagPLAYER_DISKINFOITEM
{
	BOOL bDVRDisk;				//�Ƿ���Ч��DVR����(����¼����,����[U]�̵�)
	BOOL bRemovable;			//�Ƿ�Ϊ���ƶ��洢�豸
	BOOL bSata;					//�����������,FALSE-IDE,TRUE-SATA(��ָDVR��ʶ������ʽ)
	BOOL bWithAlgInf;			//�����ܻ���(���ܻ���)
	BOOL bWithCardNo;			//��������Ϣ(ATM����)
	BOOL bBackup;				//����ʹ�����,FALSE-¼����,TRUE-������
	UINT nChanType;				//·�����,��ʶΪ��·��������(1,4,8,16)
	UINT nFileCount;			//���������ļ���
	DWORD64 dwTotalSize;		//������������С(BYTE)
	DWORD64 dwFreeSize;			//����ʣ��������С(BYTE)
}PLAYER_DISKINFOITEM, *PPLAYER_DISKINFOITEM;

//������Ϣ��
typedef struct tagPLAYER_DISKINFOGROUP
{
	PLAYER_DISKINFOITEM diskinfo[PLAYER_MAXDISK_NUM];
}PLAYER_DISKINFOGROUP, *PPLAYER_DISKINFOGROUP;

//ʱ������
typedef struct tagPLAYER_DATETIME
{
	WORD year;					//��
	WORD month;					//��
	WORD day;					//��
	WORD hour;					//ʱ
	WORD minute;				//��
	WORD second;				//��
}PLAYER_DATETIME, *PPLAYER_DATETIME;

//�ļ���ϸ��Ϣ
typedef struct tagPLAYER_FILEINFO
{
	DWORD fileID;				//�ļ�ID��(��ͬһ������Ψһ)
	DWORD type;					//¼������(PLAYER_RECORDFILETYPE)
	DWORD channel;				//����ͨ��(1-16)
	DWORD Quality;				//¼����(1-6)
	PLAYER_DATETIME tBegin;		//¼����ʼʱ��
	PLAYER_DATETIME tEnd;		//¼�����ʱ��
	DWORD period;				//¼��ʱ��(��)
	DWORD res;					//����
	UINT64 size;				//�ļ���С(Bytes)
}PLAYER_FILEINFO, *PPLAYER_FILEINFO;

//���̲���
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_DiskEnum(PPLAYER_DISKINFOGROUP lpDiskInfos);
BSRPLAYER_API HANDLE BSRPLAYER_STDCALL BSR_PLAYER_DiskInit(UINT nDiskIndex);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DiskDeinit(HANDLE hDiskHandle);

//�ļ�����
BSRPLAYER_API HANDLE BSRPLAYER_STDCALL BSR_PLAYER_FindFile(HANDLE hDiskHandle, UINT nChannel, DWORD dwFileType, DWORD dwNumOfOnePage, PPLAYER_DATETIME lpBeginTime, PPLAYER_DATETIME lpEndTime, LPCSTR lpszCardNumber, PLONG lpPageCount, PLONG lpFileCount);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_FindNextPage(HANDLE hFindHandle, UINT nPageIndex, PPLAYER_FILEINFO lpFileData, PDWORD lpdwCount);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_FindFileClose(HANDLE hFindHandle);

//�ļ���ȡ
BSRPLAYER_API HANDLE BSRPLAYER_STDCALL BSR_PLAYER_DiskFileOpen(HANDLE hDiskHandle, DWORD dwFileID);
BSRPLAYER_API DWORD64 BSRPLAYER_STDCALL BSR_PLAYER_DiskFileRead(HANDLE hFileHandle, LPVOID lpBuffer, DWORD64 dwReadSize);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DiskFileSeekByByte(HANDLE hFileHandle, DWORD64 dwBytesToMove, DWORD dwMoveMethod);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DiskFileSeekByTime(HANDLE hFileHandle, DWORD64 dwSecondToMove, DWORD dwMoveMethod);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DiskFileClose(HANDLE hFileHandle);

//�ļ�����
BSRPLAYER_API HANDLE BSRPLAYER_STDCALL BSR_PLAYER_DiskFileDown(HANDLE hDiskHandle, DWORD dwFileID, LPCSTR lpszFileName);
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_DiskFileGetDownPos(HANDLE hFileHandle);

/////////////////////////////////��ʵ��,�벻Ҫ����/////////////////////////////////////////
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DiskMakeFileHeader(LPVOID lpBuffer);
BSRPLAYER_API HANDLE BSRPLAYER_STDCALL BSR_PLAYER_OpenDiskPlay(HANDLE hDiskHandle, DWORD dwFileID);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_CloseDiskPlay(HANDLE hPlayHandle);
/////////////////////////////////��ʵ��/////////////////////////////////////////
/////////////////////////////�ļ�ϵͳ[��֧��ֻ������]//////////////////////////////////////

#endif //__BLUESTAR_BSRPLAYER_INTERFACE_H__