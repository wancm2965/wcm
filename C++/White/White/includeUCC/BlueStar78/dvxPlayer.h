/** $Id$ $DateTime$
 *  @file  bsrPlayer.h
 *  @brief bsrPlayerģ��ĵ���ͷ�ļ�
 *  @version 0.0.1
 *  @since 0.0.1
 *  @author SongZhanjun<songzhj@bstar.com.cn>
 *  @date 2009-07-02    Created it
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

#ifndef __BSR_PLAYER_H__
#define __BSR_PLAYER_H__


#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC 
#endif

#ifdef BSRPLAYER_EXPORTS
#define DVXPLAYER_API EXTERNC __declspec(dllexport)	
#else	// else BSRPLAYER_EXPORTS
#define DVXPLAYER_API EXTERNC __declspec(dllimport)
#endif	// end of BSRPLAYER_EXPORTS

/// ���������
typedef void* PlayerHandle;

/// ������Ϣ���
typedef void* InfoHandle;

/// ���Ŷ���
enum
{
	PLAYER_SOURCE_REAL		= 0,
	PLAYER_SOURCE_RECORD	= 1,
	PLAYER_SOURCE_FILE		= 2,
  PLAYER_SOURCE_PICTURE_EVENT		= 3,
  PLAYER_SOURCE_PICTURE_CLIP		= 4
};

/// ����ģʽ, ��λʹ��
enum 
{
	PLAYER_MODE_NORMAL		= 0x00,
	PLAYER_MODE_PERFRAME	= 0x01,
	PLAYER_MODE_IFRAME		= 0x02,
};

/// ����ֵ����
enum
{
	PLAYER_ERR_NOIMPL		= -1,
	PLAYER_OK						= 0,

	PLAYER_ERR_BASE			= 0x00030000,
	PLAYER_ERR_PARA			= (PLAYER_ERR_BASE + 1),
	PLAYER_ERR_SYSTEM		= (PLAYER_ERR_BASE + 2),
	PLAYER_ERR_INVALID_HANDLE	=	(PLAYER_ERR_BASE + 3),
	PLAYER_ERR_FAILED		= (PLAYER_ERR_BASE + 4),
	PLAYER_ERR_FRAMEHDR	= (PLAYER_ERR_BASE + 5),
	PLAYER_ERR_FRAMEDATA =(PLAYER_ERR_BASE + 5),
	PLAYER_ERR_NO_SPACE =	(PLAYER_ERR_BASE + 6),
	PLAYER_ERR_NOTPLAY	=	(PLAYER_ERR_BASE + 7),
};

enum
{
  DECDATA_VIDEO = 1, 
  DECDATA_AUDIO,
};

struct dvxDecFrameInfo
{
  unsigned int sequence;				//֡���,��ͨ������,����Ƶ����
  unsigned int type;					  //��������:1��Ƶ,2��Ƶ
  unsigned int frameRate;			//����ʱ������ͼ��֡��(��Ϊ��Ƶ�˲���������)
  unsigned int timestamp;				//ʱ����Ϣ,��λ��;
  unsigned int tick;          //ʱ����Ϣ����λΪus


  unsigned char audioLen;   //��Ƶ����
  unsigned char *pAudio;   ////��Ƶָ��
  unsigned int  frameType;  //��������

  //��Ƶ֡��Ϣ
  unsigned int videoType;		//�Ƿ�I֡(�Խ�������������壬��Ϊ�㣬�ݲ���)
  unsigned int width;				//�����,��λ����(��Ϊ��Ƶ�˲���������)
  unsigned int height;			//�����,��λ����(��Ϊ��Ƶ�˲���������)

  unsigned int strideY;      //��Ƶ���ȿ��
  unsigned int strideU;      //��Ƶɫ�ȿ��
  unsigned int strideV;      //

  unsigned char* pY;      //��ƵY����
  unsigned char* pU;      //��ƵU����
  unsigned char* pV;      //��ƵV����

};


//
typedef void (CALLBACK *DecDataRoutine)( dvxDecFrameInfo *pInfo, void* pUser );

DVXPLAYER_API int dvxPlayerCreate( PlayerHandle* phPlayer, int nPlaySource, int nBufferFrameScale );
DVXPLAYER_API int dvxPlayerDestory( PlayerHandle hPlayer );
DVXPLAYER_API int dvxPlayerInit( PlayerHandle hPlayer, HWND hPlayeWnd, int sizeVideoFifo, 
																unsigned int nChannels, unsigned int nSamplesPerSec, unsigned int nBitsPerSample, 
																void (*pfNotifyRoutine)( void*, int ), void* pRoutinePara );
DVXPLAYER_API int dvxPlayerReset( PlayerHandle hPlayer, bool bIsForward );
DVXPLAYER_API int dvxPlayerGetPosition( PlayerHandle hPlayer, __int64* ms );
DVXPLAYER_API int dvxPlayerInputFrame( PlayerHandle hPlayer, void* pBuff, int nSize );
DVXPLAYER_API int dvxPlayerPlay( PlayerHandle hPlayer, int nPlayMode );
DVXPLAYER_API int dvxPlayerPause( PlayerHandle hPlayer );
DVXPLAYER_API int dvxPlayerStop( PlayerHandle hPlayer );
DVXPLAYER_API int dvxPlayerSetRate( PlayerHandle hPlayer, int nRate, int nScale );
DVXPLAYER_API int dvxPlayerGetRate( PlayerHandle hPlayer, int* pnRate, int* pnScale );
DVXPLAYER_API int dvxPlayerSound( PlayerHandle hPlayer );
DVXPLAYER_API int dvxPlayerSetVolume( PlayerHandle hPlayer, int nVolume );
DVXPLAYER_API int dvxPlayerGetVolume( PlayerHandle hPlayer, int *pnVolume );

// ��Render��صĹ���
DVXPLAYER_API void dvxPlayerSetDrawCallback( PlayerHandle hPlayer, 
void (*pfDrawRoutine)( void*, HDC hDC, unsigned int nWidth, unsigned int nHeight ), void* pDrawPara );
DVXPLAYER_API int dvxPlayerSavePicture( PlayerHandle hPlayer, const char* szFileName);
DVXPLAYER_API int dvxPlayerSavePictureEx( PlayerHandle hPlayer, const char* szFileName, int nImgType=0/*0 bitmap,1 jpg*/ );
DVXPLAYER_API int dvxPlayerRefresh( PlayerHandle hPlayer );
DVXPLAYER_API void dvxPlayerShowDateTime( PlayerHandle hPlayer, bool bShow );
DVXPLAYER_API void dvxPlayerSetDateTimeColor( PlayerHandle hPlayer, unsigned int rgbColor );
DVXPLAYER_API void dvxPlayerSetDateTimeCoordinate( PlayerHandle hPlayer, unsigned int nX, unsigned int nY );
DVXPLAYER_API int dvxPlayerAddExtraInfo( PlayerHandle hPlayer, InfoHandle* phInfo, const char* szText,
																				unsigned int rgbColor, unsigned int nX, unsigned int nY );
DVXPLAYER_API void dvxPlayerDelExtraInfo( PlayerHandle hPlayer, InfoHandle hInfo );
DVXPLAYER_API void dvxPlayerSetExtraInfoColor( PlayerHandle hPlayer, InfoHandle hInfo, unsigned int rgbColor );
DVXPLAYER_API void dvxPlayerSetExtraInfoCoordinate( PlayerHandle hPlayer, InfoHandle hInfo, unsigned int nX, unsigned int nY );
DVXPLAYER_API void dvxPlayerShowIAInfo( PlayerHandle hPlayer, BOOL bShow ); //�����Ƿ���ʾ������Ϣ
DVXPLAYER_API BOOL dvxPlayerIAInfoShow( PlayerHandle hPlayer );             //�Ƿ���ʾ������Ϣ
DVXPLAYER_API bool dvxPlayerHasAiInfo(PlayerHandle hPlayer);
DVXPLAYER_API void dvxSetPlayerShowAiLang(PlayerHandle hPlayer, int nLanguage);//�������ܵ�����Ϣ����ʾ����

DVXPLAYER_API void dvxPlayerSetDecCallback( PlayerHandle hPlayer, DecDataRoutine pCallback, void* pUser );

//��ָ����ʱ�俪ʼ���ţ�ֻ��Զ�̻طźͱ����ļ�������Ч
DVXPLAYER_API bool dvxPlayerWaitForTime( PlayerHandle hPlayer, __int64 ms, int timeout );
DVXPLAYER_API int dvxPlayerSetFluence( PlayerHandle hPlayer,int frameChacNum );

//�������
DVXPLAYER_API BOOL dvxSetAirScapeRect( PlayerHandle hPlayer, RECT miniRect, RECT selRect, bool bOn );
#endif	// end of __BSR_PLAYER_H__

