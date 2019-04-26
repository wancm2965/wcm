#pragma once
#ifndef _HPDEVDLLMGR_H_
#define _HPDEVDLLMGR_H_

#ifdef  _HPDEVDLLMGR_H_
#define AVCON_API  extern "C" __declspec(dllexport) 
#else 
#define AVCON_API  extern "C" __declspec(dllimport) 
#endif
#include "KMONDefine.h"
#include "netDvr/netdvr.h"
#include "../include/dahua/win/dhnetsdk.h"
#include "../include/hhsdk/win/HHAVDefine.h"
#include "../include/hhsdk/win/HHNetInterface.h"
#include "../include/hhsdk/win/HH5PlayerSDK.h"
#include "../include/Hc/HCNetSDK.h"
#include "../include//Hc/HCstruct.h"
#include "../include/HikVisionSdk.h"
/*蓝星5k-6k*/
#include "BlueStar56/BSRClient.h"
#include "BlueStar56/BSRCommon.h"
#include "BlueStar56/BSRPlayer.h"
/*蓝星7k-8k*/
#include "BlueStar78/dvxSdkType.h"
#include "BlueStar78/dvxSdk.h"





typedef map<string,HINSTANCE> MAP_DEVDLL;
typedef MAP_DEVDLL::iterator IT_MAP_DEVDLL;

typedef map<MON_FACTORY,MAP_DEVDLL*> MAP_DEVINFO;
typedef MAP_DEVINFO::iterator IT_MAP_DEVINFO;
typedef map<MON_FACTORY,bool> MAP_DEVSUPPORT;
typedef MAP_DEVSUPPORT::iterator IT_MAP_DEVSUPPORT;

class AFX_EXT_CLASS HPDevDllMgr
{
public:
	HPDevDllMgr();
	virtual ~HPDevDllMgr(void);
public:
	virtual void InitDevDll();
public:
	static bool IsSupportTheDvr(MON_FACTORY eDevType);
	static bool IsDevOnMyCtrl(MON_FACTORY eDevType);
public:
	//-------通力处理函数----------//
	static int NETDVR_startup();
	static int NETDVR_cleanup();
	static int NETDVR_loginServer(int Handle, const struct NETDVR_loginInfo_t *p_para);
	static int NETDVR_createDVR(int *p_handle, unsigned int serverip, unsigned short serverport);
	static int NETDVR_destroyDVR(int Handle);
	static int NETDVR_logoutServer(int Handle);
	static int NETDVR_pausePlay(int nPlayBackHandle);
	static int NETDVR_playMute(int nPlayBackHandle, int b_mute);
	static int NETDVR_playProgress(int nPlayBackHandle, int b_send_progress);
	static int NETDVR_playSeek(int nPlayBackHandle, unsigned int new_time);
	static int NETDVR_recFilesSearch(int Handle, const struct NETDVR_fileSearchCondition_t *prfs, struct NETDVR_recFileSearchResult_t *pdesc);
    static int NETDVR_recFilesSearchClean(struct NETDVR_recFileSearchResult_t *pdesc);
	static int NETDVR_resumePlay(int nPlayBackHandle);
	static int NETDVR_setPlayRate(int nPlayBackHandle, int play_rate);
	static int NETDVR_singleFramePlay(int nPlayBackHandle);
	static int NETDVR_startPlayByFile(int Handle,  const struct NETDVR_recFileInfo_t *pFileInfo, const struct PlayBackClientInfo_t* pClientInfo, int* pPlayHandle);
	static int NETDVR_stopPlayBack(int nPlayBackHandle);
	static bool SnapJPGFile(char* strjpgfilename, unsigned char* YV12Buf, unsigned short width, unsigned short height);

	//------------黄河处理函数-------//
	static HHERR_CODE HHNET_CloseFile(IN  HANDLE hFile);
	static HHERR_CODE HHNET_OpenFile(IN  char *pServerIP,IN  WORD wServerPort,IN  char *pDeviceName,IN  char *pUserName,IN  char *pUserPassword,HHOPEN_FILE_INFO *pOpenInfo,OUT  HANDLE &hFile);
	static HHERR_CODE HHNET_ReadFile(IN  HANDLE hFile ,IN UINT size,IN UINT count,OUT void *buffer,OUT UINT &nRealReadLen);
	static HHERR_CODE HHNET_ReadFirstFrame(IN  HANDLE hFile, void *outBuf, OUT UINT &nOutLen,OUT long &dwRemain);
	static HHERR_CODE HHNET_ReadNextFrame(IN  HANDLE hFile,  void *outBuf, OUT UINT &nOutLen,OUT long &dwRemain);
	static HHERR_CODE HHNET_Cleanup();
	static HHERR_CODE HHNET_CloseSearchRecInfo(IN  HANDLE hSearch);
	static HHERR_CODE HHNET_OpenSearchRecInfo(IN  char *pServerIP,IN  WORD wServerPort,IN  char *pUserName,IN  char *pUserPassword,OUT  HANDLE &hSearch);
	static HHERR_CODE HHNET_SearchRecFile(IN  HANDLE hSearch,HH_SEARCH_REC_FILE *pSearchInfo);
	static HHERR_CODE HHNET_Startup(IN  HWND hNotifyWnd,IN  UINT nCommandID,IN DWORD dwFrameBufNum = 0,IN BOOL bReadyRelay = FALSE,IN BOOL bReadyCenter=FALSE,IN char *pLocalAddr=NULL);
	static int HH5PLAYER_ClearAllBuffer(USHORT nPort);	
	static int HH5PLAYER_CloseStream(USHORT nPort);
	static int HH5PLAYER_InitPlayer(USHORT nPort,HWND hWnd);
	static int HH5PLAYER_OpenStream(USHORT nPort);
	static int HH5PLAYER_PutDecStreamDataEx(USHORT nPort,PBYTE pBuf, DWORD nSize, UINT nDataType, HH5KAV_INFO *pAVInfo);	
	static int HH5PLAYER_ReleasePlayer(USHORT nPort);	
	static int HH5PLAYER_InitPlayer2(USHORT nPort,HWND hWnd,bool bSupportDraw = false);


	//------------大华处理函数-------//
	static BOOL  PLAY_PlaySoundShare(LONG nPort);
	static BOOL  PLAY_RefreshPlay(LONG nPort);
	static BOOL  PLAY_StopSoundShare(LONG nPort);

	static BOOL CLIENT_CapturePicture(LONG hPlayHandle, const char *pchPicFileName);
	static BOOL CLIENT_FastPlayBack(LONG lPlayHandle);
	static LONG CLIENT_Login(char *pchDVRIP, WORD wDVRPort, char *pchUserName, char *pchPassword, LPNET_DEVICEINFO lpDeviceInfo, int *error = 0);
	static BOOL CLIENT_Logout(LONG lLoginID);
	static BOOL CLIENT_NormalPlayBack(LONG lPlayHandle);
	static BOOL CLIENT_PausePlayBack(LONG lPlayHandle, BOOL bPause);
	static LONG CLIENT_PlayBackByRecordFileEx(LONG lLoginID, LPNET_RECORDFILE_INFO lpRecordFile, HWND hWnd, fDownLoadPosCallBack cbDownLoadPos, DWORD dwPosUser, fDataCallBack fDownLoadDataCallBack, DWORD dwDataUser);
	static LONG CLIENT_PlayBackByTime(LONG lLoginID, int nChannelID, LPNET_TIME lpStartTime, LPNET_TIME lpStopTime, HWND hWnd, fDownLoadPosCallBack cbDownLoadPos, DWORD dwPosUser);
	static BOOL CLIENT_SeekPlayBack(LONG lPlayHandle, unsigned int offsettime, unsigned int offsetbyte);
	static BOOL CLIENT_SlowPlayBack(LONG lPlayHandle);
	static BOOL CLIENT_StepPlayBack(LONG lPlayHandle, BOOL bStop);
	static BOOL CLIENT_StopPlayBack(LONG lPlayHandle);
	static void CLIENT_Cleanup();
	static BOOL CLIENT_FindClose(LONG lFindHandle);
	static LONG	CLIENT_FindFile(LONG lLoginID, int nChannelId, int nRecordFileType, char* cardid, LPNET_TIME time_start, LPNET_TIME time_end, BOOL bTime, int waittime);
	static int	CLIENT_FindNextFile(LONG lFindHandle,LPNET_RECORDFILE_INFO lpFindData);
	static BOOL CLIENT_Init(fDisConnect cbDisConnect, DWORD dwUser);

	static BOOL CLIENT_GetDevConfig(LONG lLoginID, DWORD dwCommand, LONG lChannel, LPVOID lpOutBuffer, DWORD dwOutBufferSize, LPDWORD lpBytesReturned,int waittime=500);
	static BOOL CLIENT_SetDevConfig(LONG lLoginID, DWORD dwCommand, LONG lChannel, LPVOID lpInBuffer, DWORD dwInBufferSize, int waittime=500);
	static BOOL CLIENT_QueryDevState(LONG lLoginID, int nType, char *pBuf, int nBufLen, int *pRetLen, int waittime=1000);

	//------------柯达处理函数-------//



	//------------海康处理函数-------//
	static BOOL  NET_DVR_FindClose(LONG lFindHandle);
	static LONG  NET_DVR_FindNextFile(LONG lFindHandle,LPNET_DVR_FIND_DATA lpFindData);
	static DWORD NET_DVR_GetLastError();
	static LONG  NET_DVR_Login(char *sDVRIP,WORD wDVRPort,char *sUserName,char *sPassword,LPNET_DVR_DEVICEINFO lpDeviceInfo);
	static BOOL  NET_DVR_Logout(LONG lUserID);
	static BOOL  NET_DVR_Cleanup();
	static LONG  NET_DVR_GetFileByName(LONG lUserID,char *sDVRFileName,char *sSavedFileName);
	static BOOL  NET_DVR_Init();
	static LONG  NET_DVR_FindFile(LONG lUserID,LONG lChannel,DWORD dwFileType, LPNET_DVR_TIME lpStartTime, LPNET_DVR_TIME lpStopTime);
	static int   NET_DVR_GetDownloadPos(LONG lFileHandle);
	static LONG  NET_DVR_PlayBackByName(LONG lUserID,char *sPlayBackFileName, HWND hWnd);
	static LONG  NET_DVR_PlayBackByTime(LONG lUserID,LONG lChannel, LPNET_DVR_TIME lpStartTime, LPNET_DVR_TIME lpStopTime, HWND hWnd);
	static BOOL  NET_DVR_PlayBackControl(LONG lPlayHandle,DWORD dwControlCode,DWORD dwInValue,DWORD *lpOutValue);
	static BOOL  NET_DVR_StopPlayBack(LONG lPlayHandle);
	static BOOL  NET_DVR_PlayBackCaptureFile(LONG lPlayHandle,char *sFileName);
	static BOOL  NET_DVR_SetPlayDataCallBack(LONG lPlayHandle,void(CALLBACK *fPlayDataCallBack) (LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,DWORD dwUser),DWORD dwUser);
	static BOOL  NET_DVR_StopGetFile(LONG lFileHandle);

	static BOOL  NET_DVR_GetDVRConfig(LONG lUserID, DWORD dwCommand,LONG lChannel, LPVOID lpOutBuffer,DWORD dwOutBufferSize,LPDWORD lpBytesReturned);
	static BOOL  NET_DVR_SetDVRConfig(LONG lUserID, DWORD dwCommand,LONG lChannel, LPVOID lpInBuffer,DWORD dwInBufferSize);
	static BOOL  NET_DVR_StartDVRRecord(LONG lUserID,LONG lChannel,LONG lRecordType);
	static BOOL  NET_DVR_StopDVRRecord(LONG lUserID,LONG lChannel);
	static LONG  NET_DVR_RealPlay(LONG lUserID,LPNET_DVR_CLIENTINFO lpClientInfo);
	static BOOL  NET_DVR_RigisterDrawFun(LONG lRealHandle,void (CALLBACK* fDrawFun)(LONG lRealHandle,HDC hDc,DWORD dwUser),DWORD dwUser);
	static BOOL  NET_DVR_StopRealPlay(LONG lRealHandle);
	static BOOL  NET_DVR_RebootDVR(LONG lUserID);
	static BOOL  NET_DVR_SetScaleCFG(LONG lUserID, DWORD dwScale);
	static BOOL  NET_DVR_GetScaleCFG(LONG lUserID, DWORD *lpOutScale);
	static int   SetOsdDisplayModeEx(HANDLE hChannelHandle,int color,BOOL Translucent,int param,int nLineCount,USHORT **Format);
	static int   SetOsd(HANDLE hChannelHandle, BOOL Enable);
	//---------------蓝星5K-6K-------------------//
	//SDK初始化
	static BOOL  BSR_DVR_InitSDK();
	static BOOL  BSR_DVR_DeInitSDK();
	//登录相关
	static HANDLE BSR_DVR_Login(LPCSTR lpDVRIP, WORD wDVRPort, LPCSTR lpUserName, LPCSTR lpPasswd, BSRCLIENT_DEVICEINFO *lpDeviceInfo);
	static BOOL	  BSR_DVR_Logout(HANDLE hUserHandle);
	//常规操作
	static LONG	  BSR_DVR_IsSupport();
	//错误信息
	static LONG  BSR_DVR_GetLastError();
	//文件检索扩展接口
	static HANDLE BSR_DVR_FindFileEx(HANDLE hUserHandle, BSRCLIENT_REMOTEFINDEX *lpFindCond);
	static BOOL   BSR_DVR_FindNextFileEx(HANDLE hFindHandleEx, LONG nPageIndex, BSRCLIENT_FILELIST *lpFileData, DWORD *lpdwCount);
	static BOOL   BSR_DVR_FindFileCloseEx(HANDLE hFindHandleEx);
	//远程回放
	static BOOL   BSR_DVR_ChangeCurTransRate(HANDLE hUserHandle, LONG nBitrate);
	static HANDLE BSR_DVR_PlayBackByName(HANDLE hUserHandle, WORD wDVRPort, WORD RealPort, UINT64 nFileHandle, UINT64 nOffset, UINT64 nFileSize, HWND hWnd, DWORD dwDeviceNum, BOOL bFrameMode = FALSE, BOOL bShowTime = TRUE );
	static BOOL   BSR_DVR_PlayBackSetPlayList(HANDLE hPlayHandle, PBSCP_NETTRANS_FILEINFO pzfileInfo, DWORD nFileCount);
	static BOOL   BSR_DVR_PlayBackPlayListSetTimePos(HANDLE hPlayHandle, DWORD time);
	static BOOL   BSR_DVR_PlayBackStop(HANDLE hPlayHandle);
	static BOOL   BSR_DVR_PlayBackCaptureBMPPicture(HANDLE hPlayHandle, LPCSTR lpszFileName);
	static BOOL   BSR_DVR_PlayBackRefresh(HANDLE hPlayHandle);
	static BOOL   BSR_DVR_PlayBackGetOsdTime(HANDLE hPlayHandle, SYSTEMTIME *lpOsdTime);
	static BOOL   BSR_DVR_PlayBackSetOSDTimePara(HANDLE hPlayHandle, BOOL bShow, POINT *lpPoint, COLORREF *lpColor);
	static BOOL   BSR_DVR_PlayBackSetOSDTitlePara(HANDLE hPlayHandle, BOOL bShow, POINT *lpPoint, COLORREF *lpColor);
	static BOOL   BSR_DVR_PlayBackSetDataCallBack(HANDLE hPlayHandle, CLIENTDATACALLBACKFUN lpfnCallBackFun, LPVOID lpVoid);
	static BOOL   BSR_DVR_PlayBackWndHookFunc(HANDLE hPlayHandle, CLIENTHWNDHOOKFUN lpfnWndFunProc, LPVOID lpVoid);
	static BOOL   BSR_DVR_PlayBackDrawFunc(HANDLE hPlayHandle, CLIENTTITLEDRAWFUN lffnDrawFunProc, LPVOID lpVoid);
	static BOOL   BSR_DVR_PlayBackSaveData(HANDLE hPlayHandle, LPCSTR lpszFileName);
	static BOOL   BSR_DVR_PlayBackStopSave(HANDLE hPlayHandle);
	static BOOL   BSR_DVR_PlayBackControl(HANDLE hPlayHandle, DWORD dwControlCode, DWORD dwInValue, DWORD *lpOutValue);
	//抓图
	static BOOL   BSR_DVR_RecordBMPCapture( HANDLE hUserHandle, int chnnl, int port, SYSTEMTIME time, LPCSTR pathname, HWND hWnd );
	//时间转换
	static BOOL BSR_DVR_TimetToSystemTime(time_t t, SYSTEMTIME *lpTime);

	//---------------蓝星7K-8K-------------------//
	static int   dvxSdkInit(); 
	static int   dvxSdkDeInit();

	static int   dvxCreate(const char* ip, unsigned short nCmdPort, unsigned short nDataPort, const char* szUserName, const char* szPasswd, DvxHandle* pDvr);
	static int   dvxDestory(DvxHandle hDvx);

	static int   dvxSysVersion( DvxHandle hDvx, SystemVersionInfo* pVersion);
	static int   dvxEventOpenATM(DvxHandle hDvx, ArchEventATMOpts* opts, unsigned int* total, int msTimeout);
	static int   dvxEventOpenIpcSD(DvxHandle hDvx, ArchEventATMOpts* opts, unsigned int* total, int msTimeout);
	static int   dvxEventFetch(DvxHandle hDvx, int rowId, int rowCount, FetchResult* pFetchList);
	static int   dvxEventFetchIpcSD(DvxHandle hDvx, int rowId, int rowCount, FetchResultIpcSD* pFetchList);
	static int   dvxEventClose(DvxHandle hDvx);
	static int   dvxEventCloseIpcSD(DvxHandle hDvx);

	static int   dvxRecordOpen( DvxHandle hDvx, RecordPlayOpenPara* pPara, HWND hPlayWnd,HWND hNotifyWnd, unsigned int msgId, PlayHandle* hPlay ); 
	static int   dvxRecordClose( PlayHandle hPlay );
	static int   dvxPlaySound( PlayHandle hPlay );
	static int   dvxStopSound( PlayHandle hPlay );
	static int   dvxRecordSetPlayMode( PlayHandle hPlay, int nPlayMode );
	static int   dvxRecordPlay( PlayHandle hPlay );
	static int   dvxRecordPause( PlayHandle hPlay );
	static int   dvxRecordStop( PlayHandle hPlay );
	static int   dvxRecordSeek( PlayHandle hPlay, __int64 ms );
	static int   dvxRecordGetCurrentTime(PlayHandle hPlay, __int64* ms);
	static int   dvxRecordSetRate( PlayHandle hPlay, int nRate, int nScale);
	static int   dvxPlayPicture( PlayHandle hPlay, const char* szFName );
	static int   dvxRecordPlaySetDataCallback(PlayHandle hPlay, int (*pfDataRoutine)( void*, void*, int ), void* pPara);
	static int   dvxPlayGetPos( PlayHandle hPlay, __int64* ms );
	static int   dvxRecordPlayGet( PlayHandle hPlay, RecordPlayParam* pPara );


	void Release();
private:
	void InitSupportDevData();
	bool LoadDevLib(MON_FACTORY eDevType,MAP_DEVDLL* pDevDll);
	bool FreeDevLib(MON_FACTORY eDevType,MAP_DEVDLL* pDevDll);
	HINSTANCE FindLibInstace(MON_FACTORY eDevType,string strDllName);
};


AVCON_API HPDevDllMgr* HPDevDllMgr_GetDllMgr();
AVCON_API void HPDevDllMgr_ReleaseDllMgr();

#endif