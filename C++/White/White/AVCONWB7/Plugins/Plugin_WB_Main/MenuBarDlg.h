#ifndef _MENUBAR_DLG_H__
#define _MENUBAR_DLG_H__
#include "win_impl_base.hpp"

#include "CPUUsage.h"

#include "IPlugin/IU7_AV/IPlugin_U7_AV_GlobalSetting.h"
#include "HorizontalLayoutUIEx.h"
#include "MTInterface/MTGesture.h"
//#include "../UIlib.h"
#include "CoUI/UIlib.h"


using namespace AVCON_SDK_MTTouch;
enum{
	TIMER_DYLATEINVILATE= 100,	// �ӳ�һ��ʱ����ˢ��
	TIMER_ENPAND_MENU	,		// չ���˵�
	TIMER_UNENPAND_MENU,			// ����˵�
	TIMER_USAGE_INFO,				// ��Ϣ��ض�ʱ��
	TIMER_PROMPT,
	TIMER_PFLICKER,		// ������ʾͼƬ��˸
	TIMER_QFLICKER,		// ����ͼƬ��˸
	TIMER_GETLEVEL,					// ��ȡ����
	TIMER_GET_DB,					// �����Ƶ�ֱ�
	TIMER_MOVEBARSCROLLTOLEFT,		// �����ƶ�bar��������
	TIMER_QUICKMOVBARSCROLLTOLEFT,	// ��������ƶ�bar��������
	TIMER_MOVEBARSCROLLTORIGHT,		// �����ƶ�bar��������
	TIMER_QUICKMOVBARSCROLLTORIGHT	// ���Ҽ����ƶ�bar��������

};

namespace AVCONPlugin
{
	class CMenuBarDlg 
		: public  WindowImplBase
		 ,IPlugin_U7_AV_GlobalSetting_Observer
	{
	public:
		CMenuBarDlg();
		~CMenuBarDlg();
	public:
		//�̳�WindowImplBase�ӿ�
		virtual CString GetSkinFile(){ return "MainMenuBar.xml";};
		virtual LPCTSTR GetWindowClassName() const{return _T("CMenuBarDlg");};
		virtual void Notify(TNotifyUI& msg);
		virtual void Init();
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	protected:
		//�ؼ��й���Ϣ
		bool OnControlNotify(void* param);
		void OnValueChanged(CStdString strOptName);

		//////////////////////////////////////////////////////////////////////////
		// �̳�IPlugin_U7_AV_GlobalSetting_Observer�ӿ�
		virtual void OnSpeakerMuteChanged(int nMute);		// nMute: 0|1
		virtual void OnSpeakerVolumeChanged(int nVolume);	// nVolume:0-100
		virtual void OnMicBoostChanged(int nBoost);		//nBoost: 0|1
		virtual void OnMicInputChanged(TMIC_INPUT nType);
		virtual void OnMicVolumeChanged(int nVolume);	//nVolume:0-100
		virtual void OnMicMuteChanged(int nMute);	//nMute: 0|1
		virtual void OnPreAGCChanged(int bAuto);
		//////////////////////////////////////////////////////////////////////////

		virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	private:
		void OnClick(TNotifyUI& msg);
		void OnCoUITimer(TNotifyUI& msg);


	private:

		CPUUsage m_cpuUsage;
		bool				m_bSaveMenuBar;
		bool				m_bExpand;
		bool				m_bPBkChange;
		bool				m_bQBkChange;
		int					m_nMenuBarCount;
		int					m_nSelMenuBar;
		int					m_nCurVolume;
		int					m_nCurMic;

		CProgressUI*		m_pslidCpuusage;
		CLabelUI*			m_plbNetWork;
		CLabelUI*			m_plbPrompt;
		COptionUI*			m_pOptMediaVol;
		COptionUI*			m_pOptMediaMic;
		COptionUI*			m_pOptPromptLog;
		COptionUI*			m_pOptQuestion;
		COptionUI*			m_pOptAutomaticMic;

		//�µ���Ƶ����
		IPlugin_U7_AV_GlobalSetting* m_pAVGlobalSetting;	
		CLabelUI*		m_pLblSpeakerdB;
		CLabelUI*		m_pLblMicdB;
		CButtonUI*		m_pBtnSpeakerMinus;
		CButtonUI*		m_pBtnSpeakerAdd;
		CButtonUI*		m_pBtnMicMinus;
		CButtonUI*		m_pBtnMicAdd;
		CSliderUI*		m_pSliderSpeaker;
		CSliderUI*		m_pSliderMic;

		DWORD			m_dwMicDBColor;
		DWORD			m_dwSpkDBColor;
		int				m_nSpeakerBaseline;	// ������������׼ֵ
		int				m_nMicBaseline;		// ��˷�������׼ֵ
		CControlUI*	m_pCurBtnFun;			//��ǰѡ�еĹ��ܰ�ť
		

		//////////////////////////////////////////////////////////////////////////

		void SetLeve();
		void SetBitrate();

	protected:
		DWORD	GetDBColor(int nDB); 
	};
}
#endif