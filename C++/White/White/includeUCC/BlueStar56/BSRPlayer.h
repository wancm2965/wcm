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


//无效播放器端口
#define INVALID_PLAYERPORT			(-1)

//无效音量值
#define INVALID_SOUNDVOL			(-1)

//无效进度值
#define INVALID_PLAYPOS				(-1)

//播放器所支持的最大路数
#define BSRPLAYER_MAXPORT			(256)		//播放器最大支持路数

//播放器端口自动分配起始号
#define	BSRPLAYER_ALLOCBEGIN		(128)		//分配号范围(BSRPLAYER_ALLOCBEGIN到BSRPLAYER_MAXPORT)

//显示缓冲帧数
#define PLAYER_MINDISBUF			(2)			//最小显示缓冲帧
#define PLAYER_DEFDISBUF			(5)			//默认显示缓冲帧
#define PLAYER_MAXDISBUF			(50)		//最大显示缓冲帧

//播放器缓冲区大小
#define PLAYER_MINSOURCEBU		(1024*512)		//最小值
#define PLAYER_DEFSOURCEBUF		(1024*1024*1)	//默认值
#define PLAYER_MAXSOURCEBUF		(1024*1024*10)	//最大值

//默认帧率
#define PLAYER_DEFPALRATE			(25)		//PAL默认帧率
#define PALYER_DEFNTSCRATE			(30)		//NTSC默认帧率

//编码后音频数据缓冲大小
#define AUDIO_BUFFER_SIZE			(288)		//当前SDK中音频帧编码后的固定大小

//播放控制
typedef enum
{
	PLAYER_SPEED_NORMAL			= 0x00,			//正常播放
	PLAYER_SPEED_PAUSE,							//暂停播放
	PLAYER_SPEED_ONEBYONE,						//单帧播放
	PLAYER_SPEED_1OF2TIMES,						//1/2速(慢进)
	PLAYER_SPEED_1OF4TIMES,						//1/4速(慢进)
	PLAYER_SPEED_1OF8TIMES,						//1/8速(慢进)
	PLAYER_SPEED_1OF16TIMES,					//1/16速(慢进)
	PLAYER_SPEED_2TIMES,						//2倍速(快进)
	PLAYER_SPEED_4TIMES,						//4倍速(快进)
	PLAYER_SPEED_8TIMES,						//8倍速(快进)
	PLAYER_SPEED_16TIMES,						//16倍速(快进)
	PLAYER_SPEED_MAXSPEED,						//不限速
	PLAYER_SPEED_IFRAME,						//I帧播放
	PLAYER_SPEED_INVALID		= 0xFF,			//无效状态
}BSRPLAYER_PLAYCTRL;



//音率调整范围
#define PLAYER_WAVECOEF_MIN			(0)			//音量最小值
#define PLAYER_WAVECOEF_MAX			(100)		//音量最大值

//错误码
typedef enum
{
	PLAYER_ERR_NOERROR				= 0,		//没有错误
	PLAYER_ERR_PORTNOTUSE,						//端口未使用
	PLAYER_ERR_PORTFULL,						//端口已占满
	PLAYER_ERR_PARA_OVER,						//输入参数非法
	PLAYER_ERR_ORDER_ERROR,						//调用顺序不对
	PLAYER_ERR_TIMER_ERROR,						//定时器设置失败
	PLAYER_ERR_DEC_VIDEO_ERROR,					//视频解码失败
	PLAYER_ERR_DEC_AUDIO_ERROR,					//音频解码失败
	PLAYER_ERR_ALLOC_MEMORY_ERROR,				//分配内存失败
	PLAYER_ERR_OPEN_FILE_ERROR,					//文件操作失败
	PLAYER_ERR_CREATE_OBJ_ERROR,				//创建线程事件等失败
	PLAYER_ERR_CREATE_DDRAW_ERROR,				//创建DirectDraw失败
	PLAYER_ERR_CREATE_OFFSCREEN_ERROR,			//创建后端缓存失败
	PLAYER_ERR_BUF_OVER,						//缓冲区满,输入流失败
	PLAYER_ERR_CREATE_SOUND_ERROR,				//创建音频设备失败	
	PLAYER_ERR_SET_VOLUME_ERROR,				//设置音量失败
	PLAYER_ERR_SUPPORT_FILE_ONLY,				//只能在播放文件时才能使用此接口
	PLAYER_ERR_SUPPORT_STREAM_ONLY,				//只能在播放流时才能使用此接口
	PLAYER_ERR_SYS_NOT_SUPPORT,					//系统不支持,解码器只能工作在Pentium 3以上
	PLAYER_ERR_FILEHEADER_UNKNOWN,				//没有文件头
	PLAYER_ERR_FILEHEADER_CRCERR,				//文件头校验错误
	PLAYER_ERR_VERSION_INCORRECT,				//解码器和编码器版本不对应 
	PLAYER_ERR_INITDECODER_ERROR,				//初始化解码器失败
	PLAYER_ERR_CHECK_FILE_ERROR,				//文件太短或码流无法识别
	PLAYER_ERR_INIT_TIMER_ERROR,				//多媒体定时器初始化失败
	PLAYER_ERR_BLT_ERROR,						//位拷贝失败
	PLAYER_ERR_UPDATE_ERROR,					//显示Overlay失败
	PLAYER_ERR_OPEN_FILE_ERROR_MULTI,			//文件操作失败,流类别为复合流
	PLAYER_ERR_OPEN_FILE_ERROR_VIDEO,			//文件操作失败,流类别为视频流
	PLAYER_ERR_JPEG_COMPRESS_ERROR,				//JPEG压缩错误
	PLAYER_ERR_EXTRACT_NOT_SUPPORT,				//不支持此文件的版本
	PLAYER_ERR_EXTRACT_DATA_ERROR,				//释放视频数据失败
	PLAYER_ERR_VALIDFLAG,						//无效错误码标识
}BSRPLAYER_ERRORCODE;

//解码回调数据类型
typedef enum
{
	PLAYER_DATATYPE_VIDEORGB	= 0,	//解码后为RGB数据
	PLAYER_DATATYPE_VIDEOYV12,			//解码后为YUV数据
	PLAYER_DATATYPE_AUDIOPCM			//解码后为PCM数据
}BSRPLAYER_DECDATATYPE;

//解码帧信息
typedef struct tagBSRPLAYER_DECFRAMEINFO
{
	LONG nFrameSN;				//帧序号,各通道独立,音视频独立
	LONG nType;					//数据类型,BSRPLAYER_DECDATATYPE
	BOOL bIFlag;				//是否I帧(若为音频此参数无意义)
	LONG nWidth;				//画面宽,单位像素(若为音频此参数无意义)
	LONG nHeight;				//画面高,单位像素(若为音频此参数无意义)
	LONG nFrameRate;			//编码时产生的图像帧率(若为音频此参数无意义)
	LONG nStamp;				//时标信息,单位秒,这里无法给出精确毫秒时钟
}BSRPLAYER_DECFRAMEINFO, *PBSRPLAYER_DECFRAMEINFO;

//解复用帧信息
typedef struct tagBSRPLAYER_DEMUXFRAMEINFO
{
	LONG nFrameSN;				//帧序号,各通道独立,音视频独立
	BOOL bVideo;				//是否视频数据
	BOOL bIFlag;				//是否I帧(若为音频此参数无意义)
	LONG nWidth;				//画面宽,单位像素(若为音频此参数无意义)
	LONG nHeight;				//画面高,单位像素(若为音频此参数无意义)
	LONG nFrameRate;			//编码时产生的图像帧率(若为音频此参数无意义)
	LONG nStamp;				//时标信息,单位秒,这里无法给出精确毫秒时钟
}BSRPLAYER_DEMUXFRAMEINFO, *PBSRPLAYER_DEMUXFRAMEINFO;

//显示窗口句柄HOOK回调
typedef void (CALLBACK *FUNCPLAYERHWNDHOOK)(LONG nPort, HWND *hWnd, LPVOID lpVoid);

//字幕叠加回调time为标准无时区格林威治时间(time_t类型)
typedef void (CALLBACK *FUNCPLAYERTITLEDRAW)(LONG nPort, HDC hDC, UINT time, DWORD dwWidth, DWORD dwHeight, LPVOID lpVoid);

//文件读取回调
typedef void (CALLBACK *FUNFILEREADPROC)(LONG nPort, BYTE *lpBuffer, DWORD dwSize, LPVOID lpVoid);

//解码前数据回调(目前视频为YV12数据,音频为MP2数据,以后可能还有其它扩展数据)
typedef void (CALLBACK *FUNDECDATAPROC)(LONG nPort, BYTE *lpDataBuf, DWORD dwSize, BSRPLAYER_DECFRAMEINFO *lpFrameInfo, LPVOID lpVoid);

//解复用数据回调(音视频帧分离),视频为MPEG4格式,可以调用XVID解码;音频为MP2格式,可以调用音频解码接口进行解码
typedef void (CALLBACK *FUNDEMUXDATAPROC)(LONG nPort, BYTE *lpDataBuf, DWORD dwSize, BSRPLAYER_DEMUXFRAMEINFO *lpFrameInfo, LPVOID lpVoid);

////////////////////////////API//////////////////////////////////////////
//常规操作
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_GetSDKVersion();
BSRPLAYER_API LPCSTR BSRPLAYER_STDCALL BSR_PLAYER_GetSDKUpdateInfos();
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_IsSupport();
BSRPLAYER_API VOID BSRPLAYER_STDCALL BSR_PLAYER_SetDebugPara(LONG nLevel, BOOL bScopeMode);

//错误码处理
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_GetLastError();
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_SetLastError(LONG nError);

//端口处理
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_AllocSysPort();
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_ReleaseSysPort(LONG nPort);

//视频源打开
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_OpenFile(LONG nPort, LPCSTR lpszFileName);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_OpenStream(LONG nPort, BYTE *lpFileHeadBuf, DWORD dwSize, DWORD dwBufFrame);

//附加功能
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_SetFileEndMsg(LONG nPort, HWND hWnd, UINT nMsg);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_RigisterDrawFun(LONG nPort, FUNCPLAYERTITLEDRAW lpfnDrawfun, LPVOID lpVoid);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_RigisterWndHookFun(LONG nPort, FUNCPLAYERHWNDHOOK lpfnWndHookfun, LPVOID lpVoid);

//数据分离
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_RigisterDemuxCallBackFun(LONG nPort, FUNDEMUXDATAPROC lpfnDemuxFun, LPVOID lpVoid);

//解码回调
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_RigisterDecCallBackFun(LONG nPort, FUNDECDATAPROC lpfnDecFun, LPVOID lpVoid);

//文件帧解析
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_StartFileRead(LONG nPort, FUNFILEREADPROC lpfnReadfun, LPVOID lpVoid);

//播放操作
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_Play(LONG nPort, HWND hWnd);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_Stop(LONG nPort);

//播放控制
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_InputData(LONG nPort, BYTE *lpBuffer, DWORD dwSize, LONG nReserved);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_StopInput(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_PlayControl(LONG nPort, UINT nControl);
BSRPLAYER_API UINT BSRPLAYER_STDCALL BSR_PLAYER_GetPlayStat(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_RefreshPlay(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_SetDisplayBufNumber(LONG nPort, DWORD dwBufFrame);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_ResetDisplayBuf(LONG nPort);

//截图处理
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_CaptureBMPPicture(LONG nPort, LPCSTR lpszFileName);

//丢帧处理
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_GetAllowLostFrame(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_SetAllowLostFrame(LONG nPort, DWORD dwFrames);

//音频操作
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_PlaySound(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_StopSound();
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_GetVolume(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_SetVolume(LONG nPort, LONG nVolume);
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_GetSoundPort();

//文件播放操作
BSRPLAYER_API LONG BSRPLAYER_STDCALL BSR_PLAYER_GetPlayPos(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_SetPlayPos(LONG nPort, LONG nRelativePos);

//信息获取
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_GetCurFrameRate(LONG nPort);
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_GetPlayedTime(LONG nPort);
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_GetOSDTime(LONG nPort);
BSRPLAYER_API LPCSTR BSRPLAYER_STDCALL BSR_PLAYER_GetChannelName(LONG nPort);
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_GetPlayedFrames(LONG nPort);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_GetPictureSize(LONG nPort, LONG *lpWidth, LONG *lpHeight);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_GetFileFrameAndTime(LONG nPort, DWORD *lpdwCount, DWORD *lpdwTime);

//音频编解码
BSRPLAYER_API HANDLE BSRPLAYER_STDCALL BSR_PLAYER_InitAudioHandle();
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_EncodeAudioFrame(HANDLE lpAudioHandle, BYTE *lpInBuffer, BYTE *lpOutBuffer);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DecodeAudioFrame(HANDLE lpAudioHandle, BYTE *lpInBuffer, BYTE *lpOutBuffer);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_ReleaseAudioHandle(HANDLE lpAudioHandle);

/////////////////////////待实现API/////////////////////////////////////////////////
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_DDrawDeviceGetCount();
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DDrawDeviceSet(LONG nPort, DWORD nDeviceNum);
/////////////////////////待实现API/////////////////////////////////////////////////



/////////////////////////////文件系统[仅支持只读操作]/////////////////////////////////////////////
#define PLAYER_MAXDISK_NUM	(32)	//PC上最大物理磁盘个数
#define PLAYER_ONEPAGE_NUM	(50)	//默认每一页文件列表数

//磁盘文件类别
typedef enum 
{
	PLAYERFILETYPE_ALL	= 0,	//全部录像
	PLAYERFILETYPE_MAMUAL,		//手动录像
	PLAYERFILETYPE_TIMER,		//定时录像
	PLAYERFILETYPE_MOTODETEC,	//移动侦测
	PLAYERFILETYPE_ALARM,		//报警录像
	PLAYERFILETYPE_CARDNO,		//卡号录像
	PLAYERFILETYPE_MAX			//最大类型
}PLAYER_RECORDFILETYPE;

//磁盘子信息
typedef struct tagPLAYER_DISKINFOITEM
{
	BOOL bDVRDisk;				//是否有效的DVR磁盘(包括录像盘,备份[U]盘等)
	BOOL bRemovable;			//是否为可移动存储设备
	BOOL bSata;					//磁盘物理类别,FALSE-IDE,TRUE-SATA(仅指DVR所识别的类别方式)
	BOOL bWithAlgInf;			//带智能机型(智能机型)
	BOOL bWithCardNo;			//带卡号信息(ATM机型)
	BOOL bBackup;				//磁盘使用类别,FALSE-录像盘,TRUE-备份盘
	UINT nChanType;				//路数类别,标识为几路机所用盘(1,4,8,16)
	UINT nFileCount;			//磁盘内总文件数
	DWORD64 dwTotalSize;		//磁盘总容量大小(BYTE)
	DWORD64 dwFreeSize;			//磁盘剩余容量大小(BYTE)
}PLAYER_DISKINFOITEM, *PPLAYER_DISKINFOITEM;

//磁盘信息组
typedef struct tagPLAYER_DISKINFOGROUP
{
	PLAYER_DISKINFOITEM diskinfo[PLAYER_MAXDISK_NUM];
}PLAYER_DISKINFOGROUP, *PPLAYER_DISKINFOGROUP;

//时间日期
typedef struct tagPLAYER_DATETIME
{
	WORD year;					//年
	WORD month;					//月
	WORD day;					//日
	WORD hour;					//时
	WORD minute;				//分
	WORD second;				//秒
}PLAYER_DATETIME, *PPLAYER_DATETIME;

//文件详细信息
typedef struct tagPLAYER_FILEINFO
{
	DWORD fileID;				//文件ID号(在同一磁盘内唯一)
	DWORD type;					//录像类型(PLAYER_RECORDFILETYPE)
	DWORD channel;				//所属通道(1-16)
	DWORD Quality;				//录像画质(1-6)
	PLAYER_DATETIME tBegin;		//录像起始时间
	PLAYER_DATETIME tEnd;		//录像结束时间
	DWORD period;				//录像时长(秒)
	DWORD res;					//保留
	UINT64 size;				//文件大小(Bytes)
}PLAYER_FILEINFO, *PPLAYER_FILEINFO;

//磁盘操作
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_DiskEnum(PPLAYER_DISKINFOGROUP lpDiskInfos);
BSRPLAYER_API HANDLE BSRPLAYER_STDCALL BSR_PLAYER_DiskInit(UINT nDiskIndex);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DiskDeinit(HANDLE hDiskHandle);

//文件检索
BSRPLAYER_API HANDLE BSRPLAYER_STDCALL BSR_PLAYER_FindFile(HANDLE hDiskHandle, UINT nChannel, DWORD dwFileType, DWORD dwNumOfOnePage, PPLAYER_DATETIME lpBeginTime, PPLAYER_DATETIME lpEndTime, LPCSTR lpszCardNumber, PLONG lpPageCount, PLONG lpFileCount);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_FindNextPage(HANDLE hFindHandle, UINT nPageIndex, PPLAYER_FILEINFO lpFileData, PDWORD lpdwCount);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_FindFileClose(HANDLE hFindHandle);

//文件读取
BSRPLAYER_API HANDLE BSRPLAYER_STDCALL BSR_PLAYER_DiskFileOpen(HANDLE hDiskHandle, DWORD dwFileID);
BSRPLAYER_API DWORD64 BSRPLAYER_STDCALL BSR_PLAYER_DiskFileRead(HANDLE hFileHandle, LPVOID lpBuffer, DWORD64 dwReadSize);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DiskFileSeekByByte(HANDLE hFileHandle, DWORD64 dwBytesToMove, DWORD dwMoveMethod);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DiskFileSeekByTime(HANDLE hFileHandle, DWORD64 dwSecondToMove, DWORD dwMoveMethod);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DiskFileClose(HANDLE hFileHandle);

//文件下载
BSRPLAYER_API HANDLE BSRPLAYER_STDCALL BSR_PLAYER_DiskFileDown(HANDLE hDiskHandle, DWORD dwFileID, LPCSTR lpszFileName);
BSRPLAYER_API DWORD BSRPLAYER_STDCALL BSR_PLAYER_DiskFileGetDownPos(HANDLE hFileHandle);

/////////////////////////////////待实现,请不要调用/////////////////////////////////////////
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_DiskMakeFileHeader(LPVOID lpBuffer);
BSRPLAYER_API HANDLE BSRPLAYER_STDCALL BSR_PLAYER_OpenDiskPlay(HANDLE hDiskHandle, DWORD dwFileID);
BSRPLAYER_API BOOL BSRPLAYER_STDCALL BSR_PLAYER_CloseDiskPlay(HANDLE hPlayHandle);
/////////////////////////////////待实现/////////////////////////////////////////
/////////////////////////////文件系统[仅支持只读操作]//////////////////////////////////////

#endif //__BLUESTAR_BSRPLAYER_INTERFACE_H__