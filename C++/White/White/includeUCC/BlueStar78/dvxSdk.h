/** $Id$ $DateTime$
 *  @file  bsrDvrSdk.h
 *  @brief DvrSdk�ӿڵ������ļ�
 *  @version 0.0.1
 *  @since 0.0.1
 *  @author SongZhanjun<songzhj@bstar.com.cn>
 *  @date 2009-06-15    Created it
 */
/******************************************************************************
*@note
    Copyright 2007, BeiJing Bluestar Corporation, Limited
                        ALL RIGHTS RESERVED
Permission is hereby granted to licensees of BeiJing Bluestar, Inc. products
to use or abstract this computer program for the sole purpose of implementing
a product based on BeiJing Bluestar, Inc. products. No other rights to
reproduce, use, or disseminate this computer program,whether in part or  in
whole, are granted. BeiJing Bluestar, Inc. makes no representation or
warranties with respect to the performance of this computer program, and
specifically disclaims any responsibility for any damages, special or
consequential, connected with the use of this program.
For details, see http://www.bstar.com.cn/
******************************************************************************/

#ifndef USE_DVXSDK_HEADER
#define USE_DVXSDK_HEADER

#include <windows.h>
#include <winsock2.h>
#include "IATypeDef.h"
#include "dvxPlayer.h"
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC 
#endif

#ifdef DVXSDK_EXPORTS
#define DVXSDK_API EXTERNC __declspec(dllexport)	
#else	// else BSRDVRSDK_EXPORTS
#define DVXSDK_API EXTERNC __declspec(dllimport)
#endif	// end BSRDVRSDK_EXPORTS


/**********************************************************************************/
// ϵͳ����
DVXSDK_API int dvxSdkInit();
DVXSDK_API int dvxSdkDeInit();
DVXSDK_API int dvxSdkGetVersion();
DVXSDK_API int dvxSdkSetRegCallback( unsigned short port, int (*pfRegRoutine)( void*, SOCKET ), void* pRegPara );
DVXSDK_API int dvxCreate( const char* ip, unsigned short nCmdPort, unsigned short nDataPort, 
                         const char* szUserName, const char* szPasswd, DvxHandle* pDvr );
DVXSDK_API int dvxCreateBySocket( SOCKET sDvr, DvxHandle* pDvx, unsigned short nDataPort );
DVXSDK_API int dvxDestory( DvxHandle hDvx );
DVXSDK_API int dvxSetReconnect( DvxHandle hDvx, bool bIsReconn );
DVXSDK_API int dvxSetNotifyWnd( DvxHandle hDvx, HWND hNotifyWnd, int nMsgId );
DVXSDK_API int dvxLogin( DvxHandle hDvx, const char* szUserName, const char* szPasswd, int msTimeout );
DVXSDK_API int dvxLogout( DvxHandle hDvx );
DVXSDK_API bool dvxIsLogin( DvxHandle hDvx );

/**********************************************************************************/
// �ֳ�����
DVXSDK_API int dvxRealOpen( DvxHandle hDvx, RealOpenPara* pPara, HWND hPlayWnd,
																	 HWND hNotifyWnd, unsigned int msgId, RealHandle* phReal ); 
DVXSDK_API int dvxRealOpenEx(DvxHandle hDvx, RealOpenPara* pPara, HWND hPlayWnd,
						HWND hNotifyWnd, unsigned int msgId, int* pRealId, int* pSessionId, unsigned int* pPort, RealHandle* phReal, bool bLocalPlay = false );
DVXSDK_API int dvxRealStart( RealHandle hReal );
DVXSDK_API int dvxRealStop( RealHandle hReal );
DVXSDK_API int dvxRealClose( RealHandle hReal );
DVXSDK_API int dvxRealReopen(RealHandle hReal);
DVXSDK_API int dvxRealState( RealHandle hReal, StreamState* pState );
DVXSDK_API int dvxRealSetPara( RealHandle hReal, RealParameter* pPara );
DVXSDK_API int dvxRealGetPara( RealHandle hReal, RealParameter* pPara );
DVXSDK_API int dvxRealList( DvxHandle hDvx, StreamInfoList* pList );
DVXSDK_API int dvxRealPurge( DvxHandle hDvx, int sessionId );
DVXSDK_API int dvxPtzCtrl( RealHandle hReal,unsigned short cmd, short arg);
DVXSDK_API int dvxRealStartSaveAs( RealHandle hReal, const char* szFName, int fileType, int openMode );
DVXSDK_API int dvxRealStopSaveAs( RealHandle hReal );
DVXSDK_API bool dvxRealIsSaving( RealHandle hReal );
DVXSDK_API int dvxRealSetDataCallback( RealHandle hReal, int (*pfDataRoutine)( void*, void*, int ), void* pPara );

/**********************************************************************************/
// ͸��ͨ������

DVXSDK_API int dvxTunnelOpen( DvxHandle hDvx, TunnelOpenPara* pPara, TunnelHandle* phTunnel);
DVXSDK_API int dvxTunnelStart(TunnelHandle hTunnel);
DVXSDK_API int dvxTunnelStop(TunnelHandle hTunnel);
DVXSDK_API int dvxTunnelClose(TunnelHandle hTunnel);
DVXSDK_API int dvxTunnelState(TunnelHandle hTunnel, StreamState* pState);
DVXSDK_API int dvxTunnelReceiveDataCallback(TunnelHandle hTunnel, int(*pfDataRoutine)(void*, void*, int), void* pPara );
DVXSDK_API int dvxTunnelSendData(TunnelHandle hTunnel, void* nSendBuff, int buffsize);

/**********************************************************************************/
// �Խ�����
DVXSDK_API int dvxSpeechOpen( DvxHandle hDvx, SpeechOpenPara* pPara,
																	 HWND hNotifyWnd, unsigned int msgId, SpeechHandle* phSpeech ); 

DVXSDK_API int dvxSpeechOpenEx( DvxHandle hDvx, SpeechOpenPara* pPara,  HWND hNotifyWnd, unsigned int msgId,
                             int( *pfDataRoutine )( void* pUserData, char* pData, int readLen, int &writelen ), void *pUserData, SpeechHandle* phSpeech ); 
DVXSDK_API int dvxSpeechStart( SpeechHandle hSpeech );
DVXSDK_API int dvxSpeechStop( SpeechHandle hSpeech );
DVXSDK_API int dvxSpeechClose( SpeechHandle hSpeech );
DVXSDK_API int dvxSpeechState( SpeechHandle hSpeech, StreamState* pState );
//DVXSDK_API int dvxSpeechSound( SpeechHandle hSpeech );
DVXSDK_API int dvxSpeechSetPara( SpeechHandle hSpeech, SpeechParameter* pPara );
DVXSDK_API int dvxSpeechGetPara( SpeechHandle hSpeech, SpeechParameter* pPara );
DVXSDK_API int dvxSpeechList( DvxHandle hDvx, StreamInfoList* pList );
DVXSDK_API int dvxSpeechPurge( DvxHandle hDvx, int sessionId );

DVXSDK_API int dvxSpeechSetDataCallback( SpeechHandle phSpeech, int( *pfDataRoutine )( void* pUserData, char* pData, int readLen, int &writelen ), 
                                                                void *pUserData );

/**********************************************************************************/
// ��������
DVXSDK_API int dvxEventOpen(DvxHandle hDvx, ArchEventOpts* opts, unsigned int* total, int msTimeout);
DVXSDK_API int dvxEventOpenATM(DvxHandle hDvx, ArchEventATMOpts* opts, unsigned int* total, int msTimeout);
DVXSDK_API int dvxEventOpenIpcSD(DvxHandle hDvx, ArchEventATMOpts* opts, unsigned int* total, int msTimeout);
DVXSDK_API int dvxEventClose(DvxHandle hDvx/*, ArchEventOpts* opts, unsigned int* total, int msTimeout*/);
DVXSDK_API int dvxEventCloseIpcSD(DvxHandle hDvx/*, ArchEventOpts* opts, unsigned int* total, int msTimeout*/);
DVXSDK_API int dvxEventFetch(DvxHandle hDvx, int rowId, int rowCount, FetchResult* pFetchList);
DVXSDK_API int dvxEventFetchIpcSD(DvxHandle hDvx, int rowId, int rowCount, FetchResultIpcSD* pFetchList);
DVXSDK_API int dvxEventPlot(DvxHandle hDvx, EventPlotOpts* opts,EventPlotResult *pResult);
DVXSDK_API int dvxEventPlotATM(DvxHandle hDvx, EventPlotATMOpts* opts,EventPlotResult *pResult);
DVXSDK_API int dvxEventCalender(DvxHandle hDvx, EventCalendar* opts, EventCalendarList *pResult);
DVXSDK_API int dvxEventDescribe(DvxHandle hDvx, int channel, long long id, EventDescribe *pResult);
DVXSDK_API int dvxEventXlate(DvxHandle hDvx, EventXlate *pXlate);

DVXSDK_API int dvxBackupGet(DvxHandle hDvx, BackupPara* pPara);
DVXSDK_API int dvxBackupSet(DvxHandle hDvx, BackupPara* pPara);
DVXSDK_API int dvxBackupEXGet(DvxHandle hDvx, BackupParaEX* pPara);
DVXSDK_API int dvxBackupEXSet(DvxHandle hDvx, BackupParaEX* pPara);
DVXSDK_API int dvxBackupImgeGet(DvxHandle hDvx, int* task, BackupParaEX* pPara);

//ͼƬ����
DVXSDK_API int dvxEventPicOpen(DvxHandle hDvx, ArchEventATMOpts* opts, unsigned int* total, int msTimeout);
DVXSDK_API int dvxEventPicClose(DvxHandle hDvx);
DVXSDK_API int dvxEventPicFetch(DvxHandle hDvx, int rowId, int rowCount, PicFetchResult* pFetchList);
DVXSDK_API int dvxEventPicPlot(DvxHandle hDvx, ArchEventOpts* opts, EventPlotResult *pResult);
DVXSDK_API int dvxEventPicCalender(DvxHandle hDvx, EventCalendar* opts, EventCalendarList *pResult);
DVXSDK_API int dvxEventPicXlate(DvxHandle hDvx, EventPicXlateIn* InpXlate, EventPicXlateOut* OutpXlate);
/**********************************************************************************/
// ���ع���
DVXSDK_API int dvxRecordDumpOpen( DvxHandle hDvx, RecordDumpOpenPara* pPara, const char* szFName, 
                                 HWND hNotifyWnd, unsigned int msgId, DumpHandle* phDump , bool bContinue);
DVXSDK_API int dvxRecordDumpOpenIpcSD( DvxHandle hDvx, RecordDumpOpenPara* pPara, const char* szFName, 
								 HWND hNotifyWnd, unsigned int msgId, DumpHandle* phDump , bool bContinue);
DVXSDK_API int dvxRecordDumpStart( DumpHandle hDump );
DVXSDK_API int dvxRecordDumpStop( DumpHandle hDump );
DVXSDK_API int dvxRecordDumpClose( DumpHandle hDump );
DVXSDK_API int dvxRecordDumpState( DumpHandle hDump, StreamState* pState );
DVXSDK_API int dvxRecordDumpSetPara( DumpHandle hDump, RecordDumpParameter* pPara );
DVXSDK_API int dvxRecordDumpGetPara( DumpHandle hDump, RecordDumpParameter* pPara );
DVXSDK_API int dvxRecordDumpCount( DvxHandle hDvx, int* pCount );
DVXSDK_API int dvxRecoredDumpList( DvxHandle hDvx, StreamInfoList* pList );
DVXSDK_API int dvxRecordDumpPurge( DvxHandle hDvx, int sessionId );
DVXSDK_API int dvxRecordDumpGetFileSize( DumpHandle hDump, __int64 *nSize );
DVXSDK_API int dvxRecordDumpPos( DumpHandle hDump, __int64 *nSize );
DVXSDK_API bool dvxRecordDumpIsEnd( DumpHandle hDump );
DVXSDK_API int dvxRecordDumpSetDataCallback( DumpHandle hDump, int (*pfDataRoutine)( void*, void*, int ), void* pPara );
/**********************************************************************************/
//ͼƬ��������
DVXSDK_API int dvxImageDumpState( DumpHandle hDump, StreamState* pState );
DVXSDK_API int dvxImageDumpOpen( DvxHandle hDvx, ImageDumpOpenPara* pPara, const char* szFName, 
																 HWND hNotifyWnd, unsigned int msgId, DumpHandle* phDump , bool bContinue);
DVXSDK_API int dvxImageDumpStart( DumpHandle hDump );
DVXSDK_API int dvxImageDumpStop( DumpHandle hDump );
DVXSDK_API int dvxImageDumpClose( DumpHandle hDump );
DVXSDK_API int dvxImageDumpSet( DumpHandle hDump, ImageDumpParameter* pPara );
DVXSDK_API int dvxImageDumpGet( DumpHandle hDump, ImageDumpParameter* pPara );
DVXSDK_API int dvxImageDumpList( DvxHandle hDvx, StreamInfoList* pList );
DVXSDK_API int dvxImageDumpPurge( DvxHandle hDvx, int sessionId );
DVXSDK_API int dvxImageDumpSetDataCallback( DumpHandle hPlay, int (*pfDataRoutine)( void*, void*, int ), void* pPara );
/**********************************************************************************/
// ͼƬԶ�̻ط�
DVXSDK_API int dvxImgPlayState( PlayHandle hPlay , StreamState* pState );
DVXSDK_API int dvxImgPlayOpen( DvxHandle hDvx, ImagePlayOpenPara* pPara, HWND hPlayWnd,
														 HWND hNotifyWnd, unsigned int msgId, PlayHandle* hPlay ); 
DVXSDK_API int dvxImgPlayStart( PlayHandle hPlay );
DVXSDK_API int dvxImgPlayStop( PlayHandle hPlay );
DVXSDK_API int dvxImgPlayClose( PlayHandle hPlay );
DVXSDK_API int dvxImgPlaySet( PlayHandle hPlay, ImagePlayParam* pPara );
DVXSDK_API int dvxImgPlayGet( PlayHandle hPlay, ImagePlayParam* pPara );
DVXSDK_API int dvxImgPlayList( DvxHandle hDvx, StreamInfoList* pList );
DVXSDK_API int dvxImgPlayPurge( DvxHandle hDvx, int sessionId );
DVXSDK_API int dvxImgPlaySeek( PlayHandle hPlay, __int64 ms );
DVXSDK_API int dvxImgPlay( PlayHandle hPlay );
DVXSDK_API int dvxImgSetRate(PlayHandle hPlay, int nRate, int nScale);
DVXSDK_API int dvxImgPlaySetDataCallback( PlayHandle hPlay, int (*pfDataRoutine)( void*, void*, int ), void* pPara );
/**********************************************************************************/
// Զ��¼��طŹ���
DVXSDK_API int dvxRecordOpen( DvxHandle hDvx, RecordPlayOpenPara* pPara, HWND hPlayWnd,
                                 HWND hNotifyWnd, unsigned int msgId, PlayHandle* hPlay ); 
DVXSDK_API int dvxRecordOpenEx(DvxHandle hDvx, RecordPlayOpenParaEx* pPara, HWND hPlayWnd,
									  HWND hNotifyWnd, unsigned int msgId, PlayHandle* hPlay);
DVXSDK_API int dvxRecordState( PlayHandle hPlay, StreamState* pState );
DVXSDK_API int dvxRecordGetTime( PlayHandle hPlay, __int64* msBegin, __int64* msEnd );
DVXSDK_API int dvxRecordSetPlayForward( PlayHandle hPlay, bool bIsForward );
DVXSDK_API int dvxRecordGetPlayForward( PlayHandle hPlay, bool* pIsForward );
DVXSDK_API int dvxRecordSetPlayMode( PlayHandle hPlay, int nPlayMode );
DVXSDK_API int dvxRecordPlay( PlayHandle hPlay );
DVXSDK_API int dvxRecordPause( PlayHandle hPlay );
DVXSDK_API int dvxRecordStop( PlayHandle hPlay );
DVXSDK_API int dvxRecordClose( PlayHandle hPlay );
DVXSDK_API int dvxRecordSetRate( PlayHandle hPlay, int nRate, int nScale);
DVXSDK_API int dvxRecordSeek( PlayHandle hPlay, __int64 ms );
DVXSDK_API int dvxRecordPlaySet( PlayHandle hPlay, RecordPlayParam* pPara );
DVXSDK_API int dvxRecordPlayGet( PlayHandle hPlay, RecordPlayParam* pPara );
DVXSDK_API int dvxRecordPlayList( DvxHandle hDvx, StreamInfoList* pList );
DVXSDK_API int dvxRecordPlayPurge( DvxHandle hDvx, int sessionId );
DVXSDK_API int dvxRecordPlaySetDataCallback( PlayHandle hPlay, int (*pfDataRoutine)( void*, void*, int ), void* pPara );
DVXSDK_API int dvxRecordGetCurrentTime(PlayHandle hPlay, __int64* ms);

DVXSDK_API int dvxRecordSeekEx( PlayHandle hPlay, __int64 ms, int timeout );

/**********************************************************************************/
// �����ļ�����
DVXSDK_API int dvxLocalPlayOpen( const char* szFName, HWND hPlayWnd,
                                HWND hNotifyWnd, unsigned int msgId, LocalPlayHandle* phPlay ); 
DVXSDK_API int dvxLocalPlayGetTime( LocalPlayHandle hPlay, __int64* msBegin, __int64* msEnd );
DVXSDK_API int dvxLocalPlaySetForward( LocalPlayHandle hPlay, bool bIsForward );
DVXSDK_API int dvxLocalPlaySetPlayMode( LocalPlayHandle hPlay, int nPlayMode );
DVXSDK_API int dvxLocalPlayPlay( LocalPlayHandle hPlay );
DVXSDK_API int dvxLocalPlayPause( LocalPlayHandle hPlay);
DVXSDK_API int dvxLocalPlayStop( LocalPlayHandle hPlay );
DVXSDK_API int dvxLocalPlayClose( LocalPlayHandle hPlay );
DVXSDK_API int dvxLocalPlaySetRate( LocalPlayHandle hPlay, int nRate, int nScale);
DVXSDK_API int dvxLocalPlaySeek( LocalPlayHandle hPlay, __int64 ms );
DVXSDK_API bool dvxLocalPlayIsEnd( LocalPlayHandle hPlay );
DVXSDK_API int	dvxLocalPlayGetCurrentTime(LocalPlayHandle hPlay, __int64* ms);
DVXSDK_API int dvxLocalPlaySeekEx( LocalPlayHandle hPlay, __int64 ms, int timeout );

// ��Render��صĹ���
DVXSDK_API int dvxPlaySound( PlayHandle hPlay );
DVXSDK_API int dvxStopSound( PlayHandle hPlay );
DVXSDK_API int dvxSetVolume( PlayHandle hPlay, int nVolume );
DVXSDK_API int dvxGetVolume( PlayHandle hPlay, int *pnVolume);
DVXSDK_API int dvxPlayRefresh( PlayHandle hPlay );
DVXSDK_API int dvxPlayPicture( PlayHandle hPlay, const char* szFName );
DVXSDK_API int dvxPlayPictureEx( PlayHandle hPlay, const char* szFName, int nImgType = 0/*0 bitmap,1 jpg*/);
DVXSDK_API int dvxPlayGetPos( PlayHandle hPlay, __int64* ms );
DVXSDK_API int dvxPlaySetDrawCallback( PlayHandle hPlay, void (*pfDrawRoutine)( void*, HDC, unsigned int, unsigned int), void* pDrawPara );
DVXSDK_API int dvxPlayShowDateTime( PlayHandle hPlay, bool bShow );
DVXSDK_API int dvxPlaySetDateTimeColor( PlayHandle hPlay, unsigned int rgbColor );
DVXSDK_API int dvxPlaySetDateTimeCoordinate( PlayHandle hPlay, unsigned int nX, unsigned int nY );
DVXSDK_API int dvxPlayAddExtraInfo( PlayHandle hPlay, InfoHandle* phInfo, const char* szText,unsigned int rgbColor, unsigned int nX, unsigned int nY );
DVXSDK_API int dvxPlayDelExtraInfo( PlayHandle hPlay, InfoHandle hInfo );
DVXSDK_API int dvxPlaySetExtraInfoColor( PlayHandle hPlay, InfoHandle hInfo, unsigned int rgbColor );
DVXSDK_API int dvxPlaySetExtraInfoCoordinate( PlayHandle hPlay, InfoHandle hInfo, unsigned int nX, unsigned int nY );
DVXSDK_API void dvxPlayShowIAInfo( PlayHandle hPlay, bool bShow ); //�����Ƿ���ʾ������Ϣ
DVXSDK_API bool dvxPlayIAInfoShow( PlayHandle hPlay );             //�Ƿ���ʾ������Ϣ
DVXSDK_API void dvxPlayShowAiLang(PlayHandle hPlay, int nLanguage);//������ʾ���ܵ�����Ϣ������
DVXSDK_API bool dvxPlayHasIAInfo(PlayHandle hPlay);				//�Ƿ��е���������Ϣ
DVXSDK_API void dvxPlaySetDecCallback( PlayHandle hPlay, void (CALLBACK *pDecDataRoutine)( dvxDecFrameInfo *pInfo, void* pUser ), void *pUser );
/**********************************************************************************/
// �����ļ� ����
DVXSDK_API int dvxFileCutStart( const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, FileCutHandle* phFileCut );
DVXSDK_API int dvxFileCutClose( FileCutHandle hFileCut );
DVXSDK_API int dvxFileCutProcess( FileCutHandle hFileCut );

/**********************************************************************************/
// ��������
//������ձ�����Ϣ
DVXSDK_API int dvxAlarmRcvEnable(unsigned short port, HWND hWnd, unsigned int msgId);
//������ձ�����Ϣ�ص�
DVXSDK_API int dvxAlarmRcvEnableCallback(unsigned short port,  HWND hWnd,void (*pfRcvRoutine)( void* ), void* pPara);
//��ֹ���ձ�����Ϣ
DVXSDK_API int dvxAlarmRcvDisable( );
//��ȡ������Ϣ
DVXSDK_API int dvxAlarmRcvInfoGet(DvxAlarmInfo* pInfo);

//������ձ�������ͼƬ�ϴ���Ϣ
DVXSDK_API int dvxAlarmImgRcvEnable(DvxHandle hDvx, unsigned short port, HWND hWnd, unsigned int msgId);
//��ֹ���ձ�������ͼƬ�ϴ���Ϣ
DVXSDK_API int dvxAlarmImgRcvDisable( DvxHandle hDvx );
//��ȡ��������ͼƬ�ϴ���Ϣ
DVXSDK_API int dvxAlarmImgRcvInfoGet(DvxHandle hDvx, void* pInfo, int nSize);
//���ñ�������ͼƬ�ϴ���Ϣ���ڲ��Ͽ����ӵ�������������
DVXSDK_API int dvxAlarmImgRcvInfoReset(DvxHandle hDvx);

/**********************************************************************************/
// ��־����
DVXSDK_API int dvxSyslogOpen( DvxHandle hDvx,SyslogOpenOpts* opt, unsigned int* total);
DVXSDK_API int dvxSyslogClose(DvxHandle hDvx);
DVXSDK_API int dvxSyslogFetch(DvxHandle hDvx, int rowId, int rowCount, SyslogListInfo* pInfo);
DVXSDK_API int dvxSyslogFetchEx(DvxHandle hDvx, int rowId, int rowCount,SyslogListInfoEx* pInfo);
DVXSDK_API int dvxSyslogExport( DvxHandle hDvx,SyslogExportPara* pPara, SyslogExportInfo* pResult );
DVXSDK_API int dvxSyslogPurge( DvxHandle hDvx, SyslogPurgeOpts* opt);
DVXSDK_API int dvxSyslogHDSmartFetch(DvxHandle hDvx, SlFetchHDSMARTInPara* InPara, SlFetchHDSMARTOutPara* OutPara);

/**********************************************************************************/
// ϵͳ����
DVXSDK_API int dvxSysHello( DvxHandle hDvx);
DVXSDK_API int dvxSysState( DvxHandle hDvx, SystemStateInfo* pInfo);
DVXSDK_API int dvxSysVersion( DvxHandle hDvx, SystemVersionInfo* pVersion);
DVXSDK_API int dvxSysDescribe( DvxHandle hDvx, SystemDescribleInfo* pDes);
DVXSDK_API int dvxSysSerialGet( DvxHandle hDvx, SystemSerialGet* pSerial);
DVXSDK_API int dvxSysModulesGet( DvxHandle hDvx, SysModulePara* pModules);
DVXSDK_API int dvxSysStateUploadcfgGet( DvxHandle hDvx, StatusUploadCfg *pUpload);
DVXSDK_API int dvxSysStateUploadcfgSet( DvxHandle hDvx, StatusUploadCfg *pUpload);
DVXSDK_API int dvxSysIpcDvsStatusGet( DvxHandle hDvx, SystemIpcDvsStatus *pPara);
DVXSDK_API int dvxSysIndentityGet( DvxHandle hDvx, SystemIndentityPara* pPara);
DVXSDK_API int dvxSysIndentitySet( DvxHandle hDvx, SystemIndentityPara* pPara);
DVXSDK_API int dvxSysTimeSet( DvxHandle hDvx, unsigned int time);
DVXSDK_API int dvxSysTimeGet( DvxHandle hDvx, unsigned int* time);
DVXSDK_API int dvxSysTimeStyleSet( DvxHandle hDvx, SystemTimeStyle* time);
DVXSDK_API int dvxSysTimeStyleGet( DvxHandle hDvx, SystemTimeStyle* time);
DVXSDK_API int dvxSysGetSdCodecCap( DvxHandle hDvx, DevAvCap* pCap );
DVXSDK_API int dvxSysGetSdCodecCapEx(DvxHandle hDvx, DevAvCapEx* pCap);
DVXSDK_API int dvxSysLocaleGet( DvxHandle hDvx, int* locale);
DVXSDK_API int dvxSysLocaleSet( DvxHandle hDvx, int locale);
DVXSDK_API int dvxSysShell( DvxHandle hDvx, char* cmd, int wait, int* retValue, char* text);
DVXSDK_API int dvxSysTop( DvxHandle hDvx, char* text);
DVXSDK_API int dvxSysReboot( DvxHandle hDvx, RebootStatus* pConflictStatus );
DVXSDK_API int dvxSysHalt( DvxHandle hDvx);
DVXSDK_API int dvxSysRestore( DvxHandle hDvx,int mode, unsigned int exclusions);
DVXSDK_API int dvxSysUpgrade( DvxHandle hDvx, char* path);
DVXSDK_API int dvxSysBurnState( DvxHandle hDvx, int *stage, int* percent);


DVXSDK_API int dvxSysExport( DvxHandle hDvx, int type, char* path, int *retValue);
DVXSDK_API int dvxSysImport( DvxHandle hDvx, int type, char* path, int *retValue);

/**********************************************************************************/
// �������
DVXSDK_API  int dvxNetLinkState( DvxHandle hDvx, int iface, NetLinkState* pState);
DVXSDK_API  int dvxNetLinkGet( DvxHandle hDvx,int iface, NetLinkPara* pPara);
DVXSDK_API  int dvxNetLinkSet( DvxHandle hDvx,NetLinkPara* pPara);
DVXSDK_API  int dvxNetLinkStartSet( DvxHandle hDvx,NetLinkStartup* pPara);
DVXSDK_API  int dvxNetLinkShutSet( DvxHandle hDvx,NetLinkShutDown* pPara);
DVXSDK_API  int dvxNetLinkMultiEthModeGet(DvxHandle hDvx, NetlinkMultiEthMode* pPara);
DVXSDK_API	int dvxNetLinkMultiEthModeSet(DvxHandle hDvx, NetlinkMultiEthMode* pPara);
DVXSDK_API  int dvxNetLinkTest( DvxHandle hDvx,NetLinkTest* pPara, char* text);
DVXSDK_API  int dvxNetServerAddrGet( DvxHandle hDvx,NetServerAddrPara* pPara);
DVXSDK_API  int dvxNetServerAddrSet( DvxHandle hDvx,NetServerAddrPara* pPara);
DVXSDK_API  int dvxNetServerAlertGet( DvxHandle hDvx,NetAlertServerPara* pPara);
DVXSDK_API  int dvxNetServerAlertSet( DvxHandle hDvx,NetAlertServerPara* pPara);
DVXSDK_API  int dvxNetServerSmtpGet( DvxHandle hDvx,NetSmtpPara* pPara);
DVXSDK_API  int dvxNetServerSmtpSet( DvxHandle hDvx,NetSmtpPara* pPara);
DVXSDK_API  int dvxNetServerSmnpGet( DvxHandle hDvx,NetSmnpPara* pPara);
DVXSDK_API  int dvxNetServerSmnpSet( DvxHandle hDvx,NetSmnpPara* pPara);
DVXSDK_API  int dvxNetRegState( DvxHandle hDvx, NetRegistyState* pState);
DVXSDK_API  int dvxNetRegGet( DvxHandle hDvx,NetRegistryPara* pPara);
DVXSDK_API  int dvxNetRegSet( DvxHandle hDvx,NetRegistryPara* pPara);
DVXSDK_API  int dvxNetRegLogon( DvxHandle hDvx);
DVXSDK_API  int dvxNetRegLogoff( DvxHandle hDvx);
DVXSDK_API  int dvxNetReportCfgGet( DvxHandle hDvx,StatusUpload* pPara );
DVXSDK_API  int dvxNetReportCfgSet( DvxHandle hDvx,StatusUpload* pPara );

/**********************************************************************************/
// ��ȫ����
DVXSDK_API  int dvxSecuritySessionLogin( DvxHandle hDvx);
DVXSDK_API  int dvxSecuritySessionLogout( DvxHandle hDvx);
DVXSDK_API  int dvxSecuritySessionLoglist( DvxHandle hDvx, SecuritySessionList* pList);
DVXSDK_API  int dvxSecurityUserAdd( DvxHandle hDvx, SecurityUser* user);
DVXSDK_API  int dvxSecurityUserEdit( DvxHandle hDvx,SecurityUser* user);
DVXSDK_API  int dvxSecurityUserDel( DvxHandle hDvx,char* user);
DVXSDK_API  int dvxSecurityUserList( DvxHandle hDvx,SecurityUserList* pList);
DVXSDK_API  int dvxSecurityUserPasswd( DvxHandle hDvx,char* user, char* oldPassword, char* newPassword);
DVXSDK_API  int dvxSecurityUserPrivGet( DvxHandle hDvx, char* user, SecurityUserPrivilege* pPriv);
DVXSDK_API  int dvxSecurityUserPrivSet(DvxHandle hDvx, SecurityUserPrivilege* pPriv);
DVXSDK_API  int dvxSecurityRoleList( DvxHandle hDvx,SecurityRoleList* pList);
DVXSDK_API  int dvxSecurityRolePrivGet( DvxHandle hDvx, char* role, SecurityRolePrivilege* pPriv);
DVXSDK_API  int dvxSecurityRolePrivSet( DvxHandle hDvx,SecurityRolePrivilege* pPriv);

/**********************************************************************************/
// �洢����
DVXSDK_API  int dvxStorageDiskList( DvxHandle hDvx, DiskStatusList* pList);
DVXSDK_API  int dvxStorageDiskListEx(DvxHandle hDvx, DiskStatusListEx* pList);
DVXSDK_API  int dvxStorageDiskDescribe( DvxHandle hDvx, char disk, DiskPartInfo* pPart);
DVXSDK_API  int dvxStorageDiskState( DvxHandle hDvx, char disk, DiskStateInfo* pInfo);
DVXSDK_API  int dvxStorageDiskStateEx(DvxHandle hDvx, char disk, DiskStateInfoEx* pInfo);
DVXSDK_API  int dvxStorageDiskWaken( DvxHandle hDvx, char disk, char awake);
DVXSDK_API  int dvxStorageDiskCfgGet( DvxHandle hDvx, StorageCfg* pCfg );
DVXSDK_API  int dvxStorageDiskCfgSet( DvxHandle hDvx, const StorageCfg* pCfg );
DVXSDK_API  int dvxStorageDiskFormat( DvxHandle hDvx, unsigned int disk, char partition, char fsType);
DVXSDK_API  int dvxStorageDiskFormatEx(DvxHandle hDvx, DiskFormatEx* pPara);
DVXSDK_API  int dvxStorageDiskAlbumGet( DvxHandle hDvx, DiskAlbumPara* pPara);
DVXSDK_API  int dvxStorageDiskAlbumGetEx(DvxHandle hDvx,DiskAlbumParaEx* pPara);
DVXSDK_API  int dvxStorageDiskAlbumSet( DvxHandle hDvx, DiskAlbumPara* pPara);
DVXSDK_API  int dvxStorageDiskAlbumSetEx(DvxHandle hDvx, DiskAlbumParaEx* pPara);
DVXSDK_API  int dvxStorageDiskSchemGet( DvxHandle hDvx, DiskSchemePara* pPara);
DVXSDK_API  int dvxStorageDiskSchemGetEx(DvxHandle hDvx,DiskSchemeParaEx* pPara);
DVXSDK_API  int dvxStorageDiskSchemSet( DvxHandle hDvx, DiskSchemePara* pPara);
DVXSDK_API  int dvxStorageDiskSchemSetEx(DvxHandle hDvx, DiskSchemeParaEx* pPara);

//��ȡӲ�̸�ʽ��״̬
DVXSDK_API  int dvxStorageDiskFormatState( DvxHandle hDvx, unsigned int disk,  DiskFormatStatus *pStatus );

/**********************************************************************************/
// ���ڹ���
DVXSDK_API int dvxUartPortCfgGet(  DvxHandle hDvx, char link, char dock, UartPortCfg* pCfg);
DVXSDK_API int dvxUartPortCfgGetEx(DvxHandle hDvx, UartPortCfgEx_In* pInCfg, UartPortCfgEx* pCfg);
DVXSDK_API int dvxUartPortCfgSet(  DvxHandle hDvx, UartPortCfg* pCfg);
DVXSDK_API int dvxUartPortCfgSetEx(DvxHandle hDvx, UartPortCfgEx* pCfg);
DVXSDK_API int dvxUartCipherList(  DvxHandle hDvx, char link, char type, UartCipherInfoList* pList);
DVXSDK_API int dvxUartSnoopCfgGet(  DvxHandle hDvx, UartSnoopCfg* pCfg);
DVXSDK_API int dvxUartSnoopCfgSet(  DvxHandle hDvx, UartSnoopCfg* pCfg);
DVXSDK_API int dvxUartAgentCfgGet(  DvxHandle hDvx, UartAgentCfg* pCfg);
DVXSDK_API int dvxUartAgentCfgSet(  DvxHandle hDvx, UartAgentCfg* pCfg);

/**********************************************************************************/
// ϵͳ���ñ�������
DVXSDK_API int dvxAlertAlarmState( DvxHandle hDvx, int* status, char* zones);
DVXSDK_API int dvxAlartAlarmStateEx(DvxHandle hDvx, int* status, int* count, int* zones);
DVXSDK_API int dvxAlertAlarmArm( DvxHandle hDvx, const char* password, int mode );
DVXSDK_API int dvxAlertAlarmDisarm( DvxHandle hDvx, char* password ) ;
DVXSDK_API int dvxAlertAlarmBypass( DvxHandle hDvx, const char* password, unsigned int zones);
DVXSDK_API int dvxAlertAlarmBypassEx(DvxHandle hDvx, char* password, int count, int* zones);
DVXSDK_API int dvxAlertAlarmReset( DvxHandle hDvx, char* password) ;
DVXSDK_API int dvxAlertAlarmPasswd(DvxHandle hDvx, const char* password, char* newPassword);
DVXSDK_API int dvxAlertAlarmGet(DvxHandle hDvx, AlertAlarmCfg* pInfo);
DVXSDK_API int dvxAlertAlarmGetipc(DvxHandle hDvx, AlarmZoneCfg* pInfo);
DVXSDK_API int dvxAlertAlarmGetEx(DvxHandle hDvx, AlertAlarmCfgEx* pInfo);
DVXSDK_API int dvxAlertAlarmSet(DvxHandle hDvx, AlertAlarmCfg* pInfo);
DVXSDK_API int dvxAlertAlarmSetipc(DvxHandle hDvx, AlarmZoneCfg* pInfo);
DVXSDK_API int dvxAlertAlarmSetEx(DvxHandle hDvx,  AlertAlarmCfgEx* pInfo);
DVXSDK_API int dvxAlertAlarmInputState(DvxHandle hDvx, char* status);
DVXSDK_API int dvxAlertAlarmInputStateEx(DvxHandle hDvx, int* count, int* status);
DVXSDK_API int dvxAlertAlarmInputGet(DvxHandle hDvx, int inputs, AlarmInputList* pList);
DVXSDK_API int dvxAlertAlarmInputGetEx(DvxHandle hDvx, int inputcount, int* inputsIn, AlarmInputListEx* pList);
DVXSDK_API int dvxAlertAlarmInputSet(DvxHandle hDvx, AlarmInputList* pList);
DVXSDK_API int dvxAlertAlarmInputSetEx(DvxHandle hDvx,AlarmInputListEx* pList);
DVXSDK_API int dvxAlertAlarmOutputState(DvxHandle hDvx, char* status);
DVXSDK_API int dvxAlertAlarmOutputStateEx(DvxHandle hDvx, int* count, int* status);
DVXSDK_API int dvxAlertAlarmOutputGet(DvxHandle hDvx, int outputs, AlarmOutputList* pList);
DVXSDK_API int dvxAlertAlarmOutputGetEx(DvxHandle hDvx,int outcount, int* outsIn, AlarmOutputListEx* pList);
DVXSDK_API int dvxAlertAlarmOutputSet( DvxHandle hDvx, AlarmOutputList* pList);
DVXSDK_API int dvxAlertAlarmOutputSetEx(DvxHandle hDvx, AlarmOutputListEx* pList);
DVXSDK_API int dvxAlertAlarmOutputStart(DvxHandle hDvx, int out, int timeout, int* outputs) ;
DVXSDK_API int dvxAlertAlarmOutputStop(DvxHandle hDvx, int out, int* outputs);
DVXSDK_API int dvxAlertActionGet(DvxHandle hDvx, int alertType, unsigned int alertIds, ActionCfg* pCfg);
DVXSDK_API int dvxAlertActionCfgGetExtra(DvxHandle hDvx, int alertType, unsigned int alerts, ActionCfgExtra* pCfg );
DVXSDK_API int dvxAlertActionGetEx(DvxHandle hDvx, int alertType, unsigned int alertCount,unsigned int* alertIds,ActionCfgEx* pCfg);
DVXSDK_API int dvxAlertActionSet(DvxHandle hDvx, ActionCfg* pCfg);
DVXSDK_API int dvxAlertActionCfgSetExtra(DvxHandle hDvx,ActionCfgExtra* pCfg );
DVXSDK_API int dvxAlertActionSetEx(DvxHandle hDvx, ActionCfgEx* pCfg);
DVXSDK_API int dvxAlertActionAck(DvxHandle hDvx, int alertType, int alertIds );
DVXSDK_API int dvxEventActionGet(DvxHandle hDvx, int alertType, unsigned int alertIds, ActionCfg* pCfg);
DVXSDK_API int dvxIpcEventActionGet(DvxHandle hDvx, int alertType, unsigned int alertIds, ActionCfgIpc* pCfg);
DVXSDK_API int dvxEventActionGetExtra(DvxHandle hDvx, int alertType, unsigned int alertIds, ActionCfgExtra* pCfg);
DVXSDK_API int dvxEventActionSet(DvxHandle hDvx, const ActionCfg* pCfg);
DVXSDK_API int dvxEventActionSetipc(DvxHandle hDvx, const ActionCfgIpc* pCfg);
DVXSDK_API int dvxEventActionSetExtra(DvxHandle hDvx, const ActionCfgExtra* pCfg);
DVXSDK_API int dvxManualRaiseAlertIn(DvxHandle hDvx, const int alertInId);
DVXSDK_API int dvxManualRaiseAlertOut(DvxHandle hDvx, const ManualRaiseAlertOut* pManualOut);
DVXSDK_API int dvxEventFilterCfgGet(DvxHandle hDvx, int alertType, unsigned int alertCount,unsigned int* alertIds, FilterCfg* pCfg);
DVXSDK_API int dvxEventFilterCfgSet(DvxHandle hDvx, FilterCfg* pCfg);
/**********************************************************************************/
// ͨ������ // Hid (Human Interface Device, �˻������豸)
DVXSDK_API int dvxSceneHidCfgGet( DvxHandle hDvx, SceneHidCfg* pCfg );
DVXSDK_API int dvxSceneHidCfgSet( DvxHandle hDvx, const SceneHidCfg* pCfg );

/**********************************************************************************/

/**********************************************************************************/
DVXSDK_API int dvxSceneDevNameOverlayGet( DvxHandle hDvx, SceneDevNameOverlayCfg *pCfg );
DVXSDK_API int dvxSceneDevNameOverlaySet( DvxHandle hDvx, const SceneDevNameOverlayCfg *pCfg );
DVXSDK_API int dvxSceneDevChnnlOverlayGet( DvxHandle hDvx, int chnnl, SenceDevChnnlOverlayCfg *pCfg, unsigned int ChannelsRound=0);
DVXSDK_API int dvxSceneDevChnnlOverlayGetEx(DvxHandle hDvx, unsigned long long chnl, SenceDevChnnlOverlayCfgEx *pCfg);
DVXSDK_API int dvxSceneDevChnnlOverlaySet( DvxHandle hDvx, const SenceDevChnnlOverlayCfg *pCfg, unsigned int ChannelsRound=0);
DVXSDK_API int dvxSceneDevChnnlOverlaySetEx(DvxHandle hDvx, const SenceDevChnnlOverlayCfgEx *pCfg);
/**********************************************************************************/

//¼�����
DVXSDK_API int dvxAVRecordState( DvxHandle hDvx, unsigned int channels,AVRecordStatusList* pList, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVRecordStateipc( DvxHandle hDvx, unsigned int channels,DvxMsgRecord_Status* pList, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVRecordProfileGet( DvxHandle hDvx, EncoderProfile* pProfile);
DVXSDK_API int dvxAVRecordProfileSet( DvxHandle hDvx, EncoderProfile* pProfile);
DVXSDK_API int dvxAVRecordCfgGet( DvxHandle hDvx, unsigned int channels, AVRecordCfgList* pList, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVRecordCfgSet( DvxHandle hDvx, AVRecordCfgList* pList, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVRecordScheduleGet( DvxHandle hDvx, unsigned int channels, AVRecordPlanList* pList, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVRecordScheduleGetEx(DvxHandle hDvx, unsigned int channels, AVRecordPlanListEx* pList, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVRecordScheduleSet( DvxHandle hDvx, AVRecordPlanList* pList, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVRecordScheduleSetEx(DvxHandle hDvx, AVRecordPlanListEx* pList, unsigned int ChannelsRound=0 );
DVXSDK_API int dvxAVRecListBitrate( DvxHandle hDvx, unsigned int nStreamIndex, BitrateList* pBitrate ); 
//DVXSDK_API int dvxAVRecordStart( DvxHandle hDvx, unsigned int channels);
DVXSDK_API int dvxAVRecordStart( DvxHandle hDvx, unsigned int channels, char* typeList,int *retValue, unsigned int ChannelsRound=0);
//DVXSDK_API int dvxAVRecordStop( DvxHandle hDvx, unsigned int channels);
DVXSDK_API int dvxAVRecordStop( DvxHandle hDvx, unsigned int channels, char* typeList,int *retValue, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVRecordPicState(DvxHandle hDvx, unsigned int channels, PicRecordstate* pState, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVRecordPicstateSimple(DvxHandle hDvx, unsigned int channels, PicstateSimple* pState, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVRecordPicCfgGet(DvxHandle hDvx, unsigned int channels, AVPicRecordCfg* pCfg);
DVXSDK_API int dvxAVRecordPicCfgSet(DvxHandle hDvx, AVPicRecordCfg* pCfg);
DVXSDK_API int dvxAVRecordPicStart(DvxHandle hDvx, AVPicRecordStartPara* pPara, unsigned int* channel, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVRecordPicStop(DvxHandle hDvx, AVPicRecordStartPara* pPara, unsigned int* channel, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVRecordPicTimeStart(DvxHandle hDvx, AVPicRecordTimeStarPara* pPara, unsigned int* channel);
DVXSDK_API int dvxAVRecordPicTimeStop(DvxHandle hDvx, AVPicRecordTimeStarPara* pPara, unsigned int* channel);

/**********************************************************************************/
//��̨����
DVXSDK_API int dvxAVDomeCfgGet( DvxHandle hDvx, unsigned int channels, AVDomeCfg* pCfg);
DVXSDK_API int dvxAVDomeCfgSet( DvxHandle hDvx, AVDomeCfg* pCfg);
DVXSDK_API int dvxAVDomeControl( DvxHandle hDvx, unsigned int channels, short cmd, short arg);
DVXSDK_API int dvxAVDomeControlEx(DvxHandle hDvx, unsigned long long channels, short cmd, short arg, unsigned long long* OutChs);
DVXSDK_API int dvxAVDomeGetPresets( DvxHandle hDvx, unsigned int channels, AVDomePresets* pPresets);
DVXSDK_API int dvxAVDomeGetPresetsEx(DvxHandle hDvx, unsigned long long channels, AVDomePresetsEx* pPresets);
DVXSDK_API int dvxAVDomeSetPresets( DvxHandle hDvx, AVDomePresets* pPresets);
DVXSDK_API int dvxAVDomeSetPresetsEx(DvxHandle hDvx, AVDomePresetsEx* pPresets);
DVXSDK_API int dvxAVDomePatrolGet( DvxHandle hDvx, unsigned int channels, AVDomePatrol* pPatrol);
DVXSDK_API int dvxAVDomePatrolGetEx(DvxHandle hDvx, unsigned long long channels, AVDomePatrolEx* pPatrol);
DVXSDK_API int dvxAVDomePatrolSet( DvxHandle hDvx, AVDomePatrol* pPatrol);
DVXSDK_API int dvxAVDomePatrolSetEx(DvxHandle hDvx, AVDomePatrolEx* pPatrol);
DVXSDK_API int dvxAVDomeGetTrailStatus( DvxHandle hDvx, unsigned int channels, AVDomeTrailStatus* pTrail );
DVXSDK_API int dvxAVDomeGetTrailStatusEx(DvxHandle hDvx, unsigned long long channels, AVDomeTrailStatusEx* pTrail);
DVXSDK_API int dvxAVDomeSetTrailStatus( DvxHandle hDvx, AVDomeTrailStatus* pTrail);
DVXSDK_API int dvxAVdomeSetTrailStatusEx(DvxHandle hDvx, AVDomeTrailStatusEx* pTrail);
DVXSDK_API int dvxAVDomeGetScanStatus( DvxHandle hDvx, unsigned int channels, AVDomeScanStatus* pScan );
DVXSDK_API int dvxAVDomeGetScanStatusEx(DvxHandle hDvx, unsigned long long channels, AVDomeScanStatusEx* pScan);
DVXSDK_API int dvxAVDomeSetScanStatus( DvxHandle hDvx, AVDomeScanStatus* pScan );
DVXSDK_API int dvxAVDomeSetScanStatusEx(DvxHandle hDvx, AVDomeScanStatusEx* pScan);
DVXSDK_API int dvxAVDomeStatusGet( DvxHandle hDvx, unsigned int chnnl, unsigned int *pStatus );
DVXSDK_API int dvxAVDomeStatusGetEx(DvxHandle hDvx, unsigned long long chnnl, unsigned long long *pStatus);

/**********************************************************************************/
//ͨ������
DVXSDK_API int dvxAVDescrible( DvxHandle hDvx, AVDescribe* pInfo);
DVXSDK_API int dvxAVCfgGet( DvxHandle hDvx, char* system);
DVXSDK_API int dvxAVCfgSet( DvxHandle hDvx,char* system);
DVXSDK_API int dvxAVXEncodeCfgGet( DvxHandle hDvx, AVXEncoderCfg* translateEncodeEnable);
DVXSDK_API int dvxAVXEncodeCfgSet( DvxHandle hDvx, AVXEncoderCfg* translateEncodeEnable);
DVXSDK_API int dvxAVInputCfgGet( DvxHandle hDvx, unsigned int channels, AVInputPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVInputCfgSet( DvxHandle hDvx, AVInputPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVInputPictureGetExmore( DvxHandle hDvx, unsigned int channels, AVPictureParaEx* pPara);
DVXSDK_API int dvxAVInputPictureSetExmore( DvxHandle hDvx, AVPictureParaEx* pPara);
DVXSDK_API int dvxAVInputPictureGet( DvxHandle hDvx, unsigned int channels, AVPicturePara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVInputPictureGetEx(DvxHandle hDvx, unsigned int channels, AVPictureParaEx* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVInputPictureSet( DvxHandle hDvx, AVPicturePara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVInputPictureSetEx( DvxHandle hDvx, AVPictureParaEx* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVInputImprintGet( DvxHandle hDvx, unsigned int channels, AVInputImprintPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVInputImprintSet( DvxHandle hDvx, AVInputImprintPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVInputMaskGet( DvxHandle hDvx, unsigned int channels, AVInputMaskPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVInputMaskSet( DvxHandle hDvx, AVInputMaskPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVInputColorCfgGet(DvxHandle hDvx, unsigned int channels, AVInputColorCfg* pPara);
DVXSDK_API int dvxAVInputColorCfgSet(DvxHandle hDvx, AVInputColorCfg* pPara);
DVXSDK_API int dvxAVOutputImprintGet( DvxHandle hDvx, unsigned int channels, AVOutImprintPara* pPara);
DVXSDK_API int dvxAVOutputImprintSet( DvxHandle hDvx, AVOutImprintPara* pPara);
DVXSDK_API int dvxAVEncoderCfgGet( DvxHandle hDvx, unsigned int channels, char bitstreams, EncoderPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVEncoderCfgGetEx( DvxHandle hDvx, unsigned int channels, char bitstreams, char ver, EncoderParaEx* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVEncoderCfgSet( DvxHandle hDvx, EncoderPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVEncoderCfgSetEx(DvxHandle hDvx, EncoderParaEx* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVMotionGet( DvxHandle hDvx, unsigned int channels,AVDetectMotionPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVMotionGetipc( DvxHandle hDvx, unsigned int channels,AVBlockDetectCfg* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVMotionSet( DvxHandle hDvx, AVDetectMotionPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVMotionSetipc( DvxHandle hDvx, AVBlockDetectCfg * pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVBlockGet( DvxHandle hDvx, unsigned int channels,AVDetectBlockPara* pPara, unsigned int ChannelsRound=0 );
DVXSDK_API int dvxAVBlockSet( DvxHandle hDvx, AVDetectBlockPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVLossGet( DvxHandle hDvx, unsigned int channels,AVDectectLossPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVLossSet( DvxHandle hDvx, AVDectectLossPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVEncoderRealTimeCfgGet(DvxHandle hDvx, unsigned int channels, char bitstreams, char* res, EncoderPara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVEncoderROICfgGet(DvxHandle hDvx, unsigned int channels, ROIEncoderpara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVEncoderROICfgSet(DvxHandle hDvx, ROIEncoderpara* pPara, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAVEncoderZeroCfgGet(DvxHandle hDvx, ZeroCfg* pPara);
DVXSDK_API int dvxAVEncoderZeroCfgSet(DvxHandle hDvx, ZeroCfg* pPara);
DVXSDK_API int dvxAVIPCWdrparamGet(DvxHandle hDvx,WideDynamicRangeParam *ipcWdrParam);
DVXSDK_API int dvxAVIPCWdrparamSet(DvxHandle hDvx,WideDynamicRangeParam *ipcWdrParam);
DVXSDK_API int dvxAVIPCBasicparaGet(DvxHandle hDvx, PictureMakeParam* pIn, PictureMakeParam *pOut);
DVXSDK_API int dvxAVIPCBasicparaSet(DvxHandle hDvx, PictureMakeParam* pIn);
DVXSDK_API int dvxAVBncFormatGet(DvxHandle hDvx, int nChnnl, DevBncPara* pPara);
DVXSDK_API int dvxAVBncFormatSet(DvxHandle hDvx, DevBncPara* pPara);
//�ֳ���Ч
DVXSDK_API int dvxSceneSoundGet(DvxHandle hDvx, SceneSoundCfg* pPara);
DVXSDK_API int dvxSceneSoundSet(DvxHandle hDvx, SceneSoundCfg* pPara);

//���ܼ������
DVXSDK_API int dvxALGDetectCfgGet(DvxHandle hDvx, unsigned int channels, VCA_ALGDECTVIDEOPARA_78* pAlgParam);
DVXSDK_API int dvxALGDetectCfgSet(DvxHandle hDvx, VCA_ALGDECTVIDEOPARA_78* pAlgParam);

//��ȡͨ����������
DVXSDK_API int dvxAVEncCapGet( DvxHandle hDvx, ChnnlAVCap *pCap, unsigned int ChannelsRound=0);
DVXSDK_API int dvxALGParamGet(DvxHandle hDvx, unsigned int channels, void* pAlgParam, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAlgParamSet(DvxHandle hDvx, unsigned int channels, void* pAlgParam, unsigned int ChannelsRound=0);
DVXSDK_API int dvxAlgCalibrationCheck(DvxHandle hDvx, unsigned int channels, void* Calibration, void* result);
//��·����
DVXSDK_API int dvxALGParamGetEx(DvxHandle hDvx, unsigned int channels, void* pAlgParam, unsigned int nSize, unsigned int ChannelsRound=0);
DVXSDK_API int dvxALGParamSetEx(DvxHandle hDvx, void* pAlgParam, unsigned int nSize, unsigned int ChannelsRound=0);
DVXSDK_API int dvxALGDefGetEx(DvxHandle hDvx,unsigned int channels, void* pAlgParam, unsigned int nSize, unsigned int ChannelsRound=0);

//����AVD����(��Ƶ���)
DVXSDK_API int dvxDetectAvdConfGet(DvxHandle hDvx, unsigned int channels, AvdConfPara* pPara);
DVXSDK_API int dvxDetectAvdConfSet(DvxHandle hDvx, AvdConfPara* pPara);
DVXSDK_API int dvxDetectTriggerScnMode(DvxHandle hDvx, unsigned int channels);


DVXSDK_API HANDLE dvxAudioEncInit();
DVXSDK_API HANDLE dvxAudioEncInitEx(unsigned int nAudioType, unsigned int nSubType);
DVXSDK_API int dvxAudioEncode(HANDLE hEnc, char *pIn, char *pOut );
DVXSDK_API void dvxAudioEncodeEx(HANDLE hEnc, unsigned int nAudioType, char *pIn, char *pOut, int *nOutlen);
DVXSDK_API void dvxAudioEncClose( HANDLE hEnc );

DVXSDK_API HANDLE dvxAudioDecInit( );
DVXSDK_API HANDLE dvxAudioDecInitEx(unsigned int nAudioType, unsigned int nSubType);
DVXSDK_API int dvxAudioDecode(HANDLE hDec, char *pIn, char *pOut );
DVXSDK_API int dvxAudioDecodeEx(HANDLE hDec, unsigned int nAudioType, char *pIn, char *pOut, int nInlen);
DVXSDK_API void dvxAudioDecClose( HANDLE hDec );


// ����
DVXSDK_API int dvxConfigParaSet(DvxHandle hDvx, EdvxConfigCmd cmd, LPVOID lpSend, int nSend);
DVXSDK_API int dvxConfigParaGet(DvxHandle hDvx, EdvxConfigCmd cmd, LPVOID lpSend, int nSend, LPVOID lpRecv, int nRecv);

//IPͨ������
DVXSDK_API int dvxAVDigitalChannelSearch(DvxHandle hDvx, NvrSearchInfo* pPara);
DVXSDK_API int dvxAVSearchInfoFetch(DvxHandle hDvx,  unsigned int rowId, unsigned int rowCount, NvrFetchSearchInfo* pInfo);
DVXSDK_API int dvxAVDevNetSet(DvxHandle hDvx, NvrSetDeviceNet* pPara);
DVXSDK_API int dvxAVDigitalChannelAdd(DvxHandle hDvx, NvrAddChannelParam* pPara);
DVXSDK_API int dvxAVDigitalPropGet(DvxHandle hDvx, NvrGetChannelProp* pPara, unsigned int* Channel);
DVXSDK_API int dvxAVDigitalPropModify(DvxHandle hDvx, NvrModChannelProp* pPara);
DVXSDK_API int dvxAVDigitalDevVersionGet(DvxHandle hDvx, NvrGetDeviceVersion* pPara, NvrGetDeviceVersion* pOutPara);
DVXSDK_API int dvxAVDigitalDevicedump(DvxHandle hDvx,unsigned int* channel);
DVXSDK_API int dvxAVDigitalChannelDel(DvxHandle hDvx,unsigned int* channel);
DVXSDK_API int dvxAVDigitalDevListGet(DvxHandle hDvx, NvrGetDeviceInfos* pPara);
DVXSDK_API int dvxAVDigitalChannelStateGet(DvxHandle hDvx, NvrChannelState* pPara, NvrChannelState* pOutPara);
DVXSDK_API int dvxAVDigitalChannelListGet(DvxHandle hDvx, NvrGetChannelInfos* pPara);
DVXSDK_API int dvxAVExistChannelsGet(DvxHandle hDvx, NvrGetExistChannels* pOutPara);
DVXSDK_API int dvxAVAnalogChannelsGet(DvxHandle hDvx, NvrGetAnalogChannels* pOutPara);
DVXSDK_API int dvxAVDigitalChannelModeSet(DvxHandle hDvx, NvrSetChannelMode* pPara);
DVXSDK_API int dvxAVDigitalChannelMOdeCapGet(DvxHandle hDvx, unsigned int* nModifyFlag, NvrgetChannelModeCap* pPara);
DVXSDK_API int dvxAVDigitalChannelModeCapSet(DvxHandle hDvx, NvrgetChannelModeCap* pPara);
DVXSDK_API int dvxAVDigitalChannelSysCapGet(DvxHandle hDvx, int* pInPara, sysChanCapList* pOutPara);

//��㶨�ư汾�ӿڹ���
DVXSDK_API int dvxSGConfGet(DvxHandle hDvx, SGServerConf* pPara);
DVXSDK_API int dvxSGConfSet(DvxHandle hDvx, SGServerConf* pPara);
DVXSDK_API int dvxSGAgentIDGet(DvxHandle hDvx, SGAgentId* pPara);
DVXSDK_API int dvxSGAgentIDSet(DvxHandle hDvx, SGAgentId* pPara);
DVXSDK_API int dvxSGLogUploadGet(DvxHandle hDvx, LogUpLoad* pPara);
DVXSDK_API int dvxSGLogUploadSet(DvxHandle hDvx, LogUpLoad* pPara);

//�Ƿ������԰�
DVXSDK_API int dvxIsNeutral(DvxHandle hDvx, bool* pIsNeutral, bool* pIsSG);
//�Ƿ�֧������
DVXSDK_API bool dvxIsSupportSmart( DvxHandle hDvx );


#endif	// end of USE_DVXSDK_HEADER
