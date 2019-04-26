//========================================================================
//����������Ƶ����
//���ܣ����Ÿ�������Ƶ
//���ߣ�����
//ʱ�䣺2008-12-17
//˵����
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


//¼���ʽ���Ͷ���
typedef enum AVCON_RECORD_TYPE
{
	RECORD_TYPE_AVMS = 0,
	RECORD_TYPE_MKV,
	RECORD_TYPE_STANDARD_AVI,
}AVCON_RECORD_TYPE;

HPAV_API void HPAV_SetLangPathName(const std::string& pathname);
