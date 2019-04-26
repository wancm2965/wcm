//========================================================================
//����������Ƶ������
//���ܣ���������Ƶ����
//���ߣ�����
//ʱ�䣺2009-9-15
//˵����
//--------------------------------------------------------------------------
#include "../HPLoadData/HPLoadiniData.h"

#pragma once


typedef enum{
	AV_SET_LOCAL = 0,	//����
	AV_SET_REMOTE,		//Զ��
	AV_SET_DESKTOP,		//���湲��
	AV_SET_DEVICE,		//�豸
	AV_SET_TALK,		//�Խ�ͨ��
	AV_SET_MIXVIDEO,	//�����Ļ
	AV_SET_IPCAM,		//IP Camer
}HPUI_AV_SET_TYPE;

typedef enum{
	SHOW_AVSETTING = 0,		//��׼�汾����Ƶ����
	SHOW_AUDIOSETTING,		//ֻ����Ƶ����
	SHOW_VIDEOSETTING,		//ֻ����Ƶ����
}DLG_AVSETING_SHOWTYPE;

//��Ƶ�豸�б�
typedef struct tagHPUI_AUDIO_DEVICE
{
	std::string	strDeviceName;
	int			iDeviceNo;
}HPUI_AUDIO_DEVICE, *PHPUI_AUDIO_DEVICE;
typedef std::vector<HPUI_AUDIO_DEVICE>			HPUI_AUDIO_DEVICE_LIST;
typedef HPUI_AUDIO_DEVICE_LIST::iterator	HPUI_AUDIO_DEVICE_LIST_IT;

//��Ƶ�豸�б�
typedef struct tagHPUI_VIDEO_DEVICE
{
	std::string	strDeviceName;		//�豸����
	int			iDeviceNo;			//�豸���
	bool		bVideo_HDDevice;	//�Ƿ�Ϊ���忨
	bool		bHaveSoundCard;
}HPUI_VIDEO_DEVICE, *PHPUI_VIDEO_DEVICE;
typedef std::vector<HPUI_VIDEO_DEVICE>			HPUI_VIDEO_DEVICE_LIST;
typedef HPUI_VIDEO_DEVICE_LIST::iterator	HPUI_VIDEO_DEVICE_LIST_IT;
/*
typedef struct tagHPUI_AVSET
{
	int	nVideoDev;				//��Ƶ�豸
	int	nVideoCodec;			//��Ƶ������
	int	nResolution;			//�ֱ���
	int	nFrameRate;				//ͼ��֡��
	int	nAverageStream;			//ƽ������
	int	nBitrateControlType;	//�������Ʒ�ʽ

	int	nAudioDev;				//��Ƶ�豸
	int	nAudioCodec;			//��Ƶ������
	int	nAudioInType;			//��Ƶ��������
}HPUI_AVSET, *PHPUI_AVSET;
*/
class HPDLG_API HPDLG_AVSetNotify
{
public:
	HPDLG_AVSetNotify(){};
	virtual~ HPDLG_AVSetNotify(){};

	virtual void OnApplyAVSet(HPUI_AVSET videoset, unsigned long ulSessionID) = 0;
	virtual void OnAbandonAVSet(unsigned long ulSessionID) = 0;
	virtual void SetRemoteAudioAdvanceNotify(string strAudioSet) = 0;
	virtual void SetVideoAdvanceSetNotify(string strVideoSet, HPUI_AV_SET_TYPE type) = 0;
	virtual void OnAVSetHide() = 0;
};

class AFX_EXT_CLASS HPDLG_AVSetting
{
public:
	HPDLG_AVSetting(void){};
	virtual~ HPDLG_AVSetting(void){};

	// ��ʾ/���ضԻ���
	virtual void ShowWindow(int nCmdShow) = 0;

	//��ǰ����Ƿ��Ѵ�
	virtual bool IsWindowShow() = 0;

	// ��������Ƶ��ʼ��ֵ
	virtual void SetIniOption(HPUI_AUDIO_DEVICE_LIST listMic, HPUI_VIDEO_DEVICE_LIST listCamer, HPUI_AVSET initValue, HPUI_AV_SET_TYPE SetType = AV_SET_LOCAL, bool bWindows7 = false, std::string strSupport = "") = 0;

	//�޸ĸ�����
	virtual CWnd* SetAVSetDlgParent(CWnd* pParent) = 0;

	//�������͸ı���ʾ
	virtual void UpdateDlgShowType() = 0;

	//��������
	virtual void SetShowType(DLG_AVSETING_SHOWTYPE type) = 0;

	//��������
	virtual void SetSessionID(unsigned long ulSessionID) = 0;

	//����Զ����Ƶ�Ĳ���
	virtual void SetRemoteAdvanceSetString(string strAudioSet, string strVideoSet) = 0;

	//���ý��ջص���Ϣ�Ĵ���ָ��
	virtual void SetMessageWnd(CWnd* pWnd) = 0;
};
// �����Ի���
HPDLG_API HPDLG_AVSetting* CreateAVSettingDlg(HPDLG_AVSetNotify& rNotify, CWnd* pParent = NULL, DLG_AVSETING_SHOWTYPE type = SHOW_AVSETTING, unsigned long ulSessionID = 0, CWnd* pMessageWnd = NULL);
// ���ٶԻ���
HPDLG_API void DestroyAVSettingDlg();
// ���ָ��
HPDLG_API HPDLG_AVSetting* GetHPDLG_AVSetting(void);

