#pragma once


//========================================================================
//������������ƵԤ������
//���ܣ�
//���ߣ�����
//ʱ�䣺2009-4-27
//˵����
//--------------------------------------------------------------------------

#include "videc/VIDEC_CodecDef.h"

//��ƵԤ�������Ͷ���
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

//��ƵԤ�������Ͷ���
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
	/* ����������Ԥ���ûص�����
	* �������ܣ��ص�����ֵ */
	virtual void HPDLG_OnVideoPreSetOK(const HPUI_VideoPreSetType& presetvalue) = 0;
};


class AFX_EXT_CLASS HPDLG_VideoPreSet
	: public HPDLG_Base
{	
public:
	HPDLG_VideoPreSet(void);
	virtual~ HPDLG_VideoPreSet(void);

	/* ��������������ʾ�Ի��� */
	bool Create(HPDLG_VideoPreSetNotify& rNotify, CWnd* pParent = NULL, unsigned long ulSessionID = 0);

	/* ǿ�ƹرնԻ���*/
	void Destroy();

public:
	/* ��ʼ������ */
	void InitVideoSetParam(const HPUI_VideoPreSetType& presetvalue);
};
