//========================================================================
//类名：音视频播放
//功能：播放各种音视频
//作者：戴磊
//时间：2008-12-17
//说明：
//--------------------------------------------------------------------------

#pragma once


#define HPAV_EXPORT
#ifdef HPAV_EXPORT
#define HPAV_API _declspec(dllexport)
#elif HPAV_DLL
#define HPAV_API _declspec(dllimport)
#else
#define HPAV_API
#endif

#include "hpav/HPAV_Player.h"
#include "hpav/HPAV_Manager.h"
#include "hpav/HPAV_ChannelLocal.h"


//录像格式类型定义
typedef enum AVCON_RECORD_TYPE
{
	RECORD_TYPE_AVMS = 0,
	RECORD_TYPE_MKV,
	RECORD_TYPE_STANDARD_AVI,
}AVCON_RECORD_TYPE;

HPAV_API void HPAV_SetLangPathName(const std::string& pathname);
