#pragma once


//========================================================================
//类名：设置视频预设置类
//功能：
//作者：戴磊
//时间：2009-4-27
//说明：
//--------------------------------------------------------------------------

#include "videc/VIDEC_CodecDef.h"

//视频预设置类型定义
typedef enum
{
	HPUI_PRETYPE_UN_DEFINE		= 0,
	HPUI_PRETYPE_NORMAL_MID,
	HPUI_PRETYPE_NORMAL_SMALL,
	HPUI_PRETYPE_NORMAL_BIG,

	HPUI_PRETYPE_HIGH_MID,
	HPUI_PRETYPE_HIGH_SMALL,
	HPUI_PRETYPE_HIGH_BIG,

	HPUI_PRETYPE_SCREEN_MID,
	HPUI_PRETYPE_SCREEN_SMALL,
	HPUI_PRETYPE_SCREEN_BIG,

	HPUI_PRETYPE_HPTMWND_CHILD,
}HPUI_PreSettingType;

//视频预设置类型定义
typedef struct
{
	HPUI_PreSettingType		nPreSettingType;
	VIDEC_CODEC_TYPE		nVideoCodec;
	int 					nVideoSize;
	int 					nVideoQuality;
    int 					nFrameRate;
	BOOL					bNoise;
	int						nBitrateControlType;
}HPUI_VideoPreSetType;


class AFX_EXT_CLASS HPDLG_VideoPreSetNotify
{
public:
	HPDLG_VideoPreSetNotify(){};
	~HPDLG_VideoPreSetNotify(){};

public:
	/* 函数：设置预设置回调函数
	* 函数功能：回调设置值 */
	virtual void HPDLG_OnVideoPreSetOK(const HPUI_VideoPreSetType& presetvalue) = 0;
};


class AFX_EXT_CLASS HPDLG_VideoPreSet
	: public HPDLG_Base
{	
public:
	HPDLG_VideoPreSet(void);
	virtual~ HPDLG_VideoPreSet(void);

	/* 函数：创建及显示对话框 */
	bool Create(HPDLG_VideoPreSetNotify& rNotify, CWnd* pParent = NULL, unsigned long ulSessionID = 0);

	/* 强制关闭对话框*/
	void Destroy();

public:
	/* 初始化参数 */
	void InitVideoSetParam(const HPUI_VideoPreSetType& presetvalue);
};
