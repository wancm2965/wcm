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
	TIMER_DYLATEINVILATE= 100,	// 延迟一段时间再刷新
	TIMER_ENPAND_MENU	,		// 展开菜单
	TIMER_UNENPAND_MENU,			// 收起菜单
	TIMER_USAGE_INFO,				// 信息监控定时器
	TIMER_PROMPT,
	TIMER_PFLICKER,		// 进会提示图片闪烁
	TIMER_QFLICKER,		// 提问图片闪烁
	TIMER_GETLEVEL,					// 获取音感
	TIMER_GET_DB,					// 获得音频分贝
	TIMER_MOVEBARSCROLLTOLEFT,		// 向左移动bar条滚动条
	TIMER_QUICKMOVBARSCROLLTOLEFT,	// 向左加速移动bar条滚动条
	TIMER_MOVEBARSCROLLTORIGHT,		// 向右移动bar条滚动条
	TIMER_QUICKMOVBARSCROLLTORIGHT	// 向右加速移动bar条滚动条

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
		//继承WindowImplBase接口
		virtual CString GetSkinFile(){ return "MainMenuBar.xml";};
		virtual LPCTSTR GetWindowClassName() const{return _T("CMenuBarDlg");};
		virtual void Notify(TNotifyUI& msg);
		virtual void Init();
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	protected:
		//控件托管消息
		bool OnControlNotify(void* param);
		void OnValueChanged(CStdString strOptName);

		//////////////////////////////////////////////////////////////////////////
		// 继承IPlugin_U7_AV_GlobalSetting_Observer接口
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

		//新的音频界面
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
		int				m_nSpeakerBaseline;	// 扬声器音量基准值
		int				m_nMicBaseline;		// 麦克风音量基准值
		CControlUI*	m_pCurBtnFun;			//当前选中的功能按钮
		

		//////////////////////////////////////////////////////////////////////////

		void SetLeve();
		void SetBitrate();

	protected:
		DWORD	GetDBColor(int nDB); 
	};
}
#endif