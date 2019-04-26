#include "stdafx.h"
#include "MenuBarDlg.h"

#include "IPlugin/IU7_AV/IPlugin_U7_AV_GlobalSetting.h"
#include "IPlugin/IU7_AV/IPlugin_U7_AV_Capture.h"
#include "IPlugin/IU7_AV/IPlugin_U7_AV_Player.h"
#include "IPlugin/IU7_TerminalMesg/IPlugin_U7C_TerminalMesg.h"


AVCONPlugin::CMenuBarDlg::CMenuBarDlg()
:m_nMicBaseline(100)
,m_nSpeakerBaseline(100)
,m_nCurMic(0)
,m_nCurVolume(0)
,m_bPBkChange(false)
,m_bQBkChange(false)
,m_pLblMicdB(NULL)
,m_pLblSpeakerdB(NULL)
,m_pslidCpuusage(NULL)
,m_bSaveMenuBar(false)
,m_bExpand(false)
,m_nMenuBarCount(0)
,m_nSelMenuBar(0)
,m_plbNetWork(NULL)
,m_plbPrompt(NULL)
,m_pOptMediaVol(NULL)
,m_pOptMediaMic(NULL)
,m_pOptPromptLog(NULL)
,m_pOptQuestion(NULL)
,m_pOptAutomaticMic(NULL)
,m_pAVGlobalSetting(NULL)
,m_pBtnSpeakerMinus(NULL)
,m_pBtnSpeakerAdd(NULL)
,m_pBtnMicMinus(NULL)
,m_pBtnMicAdd(NULL)
,m_pSliderSpeaker(NULL)
,m_pSliderMic(NULL)
,m_dwMicDBColor(0)
,m_dwSpkDBColor(0)
{

}

AVCONPlugin::CMenuBarDlg::~CMenuBarDlg()
{

}

void AVCONPlugin::CMenuBarDlg::Notify( TNotifyUI& msg )
{

	if (COUI_MSGTYPE_TIMER == msg.nType)
	{
		OnCoUITimer(msg);
	}
	else if (COUI_MSGTYPE_CLICK == msg.nType)
	{
		OnClick(msg);
	}
	else if (COUI_MSGTYPE_GESTURE == msg.nType)
	{
		WORD lWord = LOWORD(msg.wParam);
		if (TGT_DIRECTION == lWord)
		{
			if (MTOG_DOWN == msg.lParam)
			{
				if (true == m_bExpand)
				{
					m_bExpand = false;
					CoUI_KillTimer(TIMER_ENPAND_MENU);
					CoUI_KillTimer(TIMER_UNENPAND_MENU);
					CoUI_SetTimer(TIMER_UNENPAND_MENU, 10);

					paint_manager_.GetRoot()->NeedUpdate();
				}
			}
			if (MTOG_UP == msg.lParam)
			{
				if (false == m_bExpand)
				{
					m_bExpand = true;

					CVerticalLayoutUI* pVLayout = static_cast<CVerticalLayoutUI*>(paint_manager_.FindControl(_T("VLayoutMenuLeft")));
					if(pVLayout == NULL)
						return;

					m_nMenuBarCount = pVLayout->GetCount();
					for (int i = 0; i < m_nMenuBarCount; i++)
					{
						pVLayout->GetItemAt(i)->SetVisible(true);
					}

					CoUI_KillTimer(TIMER_UNENPAND_MENU);
					CoUI_KillTimer(TIMER_ENPAND_MENU);
					CoUI_SetTimer(TIMER_ENPAND_MENU, 10);

					paint_manager_.GetRoot()->NeedUpdate();
				}
			}
		}
	}
// 	if (_tcsicmp(msg.pSender->GetName(), _T("windowinit")) == 0)
// 	{
// 	}
// 	else if (_tcsicmp(msg.pSender->GetName(), _T("gesture")) == 0)
// 	{
// 		WORD lWord = LOWORD(msg.wParam);
//  		if (TGT_DIRECTION == lWord)
//  		{
//  			if (MTOG_DOWN == msg.lParam)
//  			{
//  				if (true == m_bExpand)
//  				{
//  					m_bExpand = false;
//  					CoUI_KillTimer(TIMER_ENPAND_MENU);
//  					CoUI_KillTimer(TIMER_UNENPAND_MENU);
//  					CoUI_SetTimer(TIMER_UNENPAND_MENU, 10);
//  
//  					paint_manager_.GetRoot()->NeedUpdate();
//  				}
//  			}
//  			if (MTOG_UP == msg.lParam)
//  			{
//  				if (false == m_bExpand)
//  				{
//  					m_bExpand = true;
//  
//  					CVerticalLayoutUI* pVLayout = static_cast<CVerticalLayoutUI*>(paint_manager_.FindControl(_T("VLayoutMenuLeft")));
//  					if(pVLayout == NULL)
//  						return;
//  
//  					m_nMenuBarCount = pVLayout->GetCount();
//  					for (int i = 0; i < m_nMenuBarCount; i++)
//  					{
//  						pVLayout->GetItemAt(i)->SetVisible(true);
//  					}
//  
//  					CoUI_KillTimer(TIMER_UNENPAND_MENU);
//  					CoUI_KillTimer(TIMER_ENPAND_MENU);
//  					CoUI_SetTimer(TIMER_ENPAND_MENU, 10);
//  
//  					paint_manager_.GetRoot()->NeedUpdate();
//  				}
//  			}
//  		}
// 	}
// 	else if (_tcsicmp(msg.pSender->GetName(), _T("click")) == 0)
// 	{
// 		OnClick(msg);
// 	}
// 	else if (_tcsicmp(msg.pSender->GetName(), _T("timer")) == 0)
// 	{
// 		OnCoUITimer(msg);
// 	}

}

void AVCONPlugin::CMenuBarDlg::Init()
{
	CPaintManagerUI::SetResourcePath(GetSkinFolder());
	m_pslidCpuusage = static_cast<CProgressUI*>(paint_manager_.FindControl(_T("CpuProgress")));
	if (m_pslidCpuusage == NULL)
		return;
	
	m_plbNetWork = static_cast<CLabelUI*>(paint_manager_.FindControl(_T("netWork")));
	if (NULL == m_plbNetWork)
		return ;
	
	m_plbPrompt = static_cast<CLabelUI*>(paint_manager_.FindControl(_T("lbRemote")));	
	if (NULL == m_plbPrompt)
		return ;

	m_pOptPromptLog = static_cast<COptionUI*>(paint_manager_.FindControl(_T("OptPrompt")));	
	if (NULL == m_pOptPromptLog)
		return ;
	m_pOptPromptLog->OnNotify += MakeDelegate(this, &CMenuBarDlg::OnControlNotify);	//提示

	m_pOptQuestion = static_cast<COptionUI*>(paint_manager_.FindControl(_T("OptQuestion")));
	if (NULL == m_pOptQuestion)
		return ;
	m_pOptQuestion->OnNotify += MakeDelegate(this, &CMenuBarDlg::OnControlNotify);
	CoUI_SetTimer(TIMER_USAGE_INFO, 1000);

	//音量和麦克
	m_pOptMediaVol = static_cast<COptionUI*>(paint_manager_.FindControl(_T("OptMediaVol")));		//音量
	if (NULL == m_pOptMediaVol)
		return ;
	m_pOptMediaVol->OnNotify += MakeDelegate(this, &CMenuBarDlg::OnControlNotify);

	m_pOptMediaMic = static_cast<COptionUI*>(paint_manager_.FindControl(_T("OptMediaMic")));		//麦克
	if (NULL == m_pOptMediaMic)
		return ;
	m_pOptMediaMic->OnNotify += MakeDelegate(this, &CMenuBarDlg::OnControlNotify);




	m_pOptAutomaticMic = static_cast<COptionUI*>(paint_manager_.FindControl(_T("OptAutomaticMic")));
	if (NULL == m_pOptAutomaticMic)
		return ;

	CString strImage = _T("");
	strImage = GetFolderPath() + _T("");
	strImage = strImage + _T("AutoMicOn.png");

	CStdString strNormalImage;
	strNormalImage.Format(_T("file='%s' source='0,0,24,24'"), strImage);
	m_pOptAutomaticMic->SetSelectedImage(strNormalImage);

	CStdString strHotImage;
	strHotImage.Format(_T("file='%s' source='24,0,48,24'"), strImage);
	m_pOptAutomaticMic->SetSelectedHotImage(strHotImage);

	CStdString strPushedImage;
	strPushedImage.Format(_T("file='%s' source='48,0,72,24'"), strImage);
	m_pOptAutomaticMic->SetPushedImage(strPushedImage);

	CStdString strDisabledImage;
	strDisabledImage.Format(_T("file='%s' source='72,0,96,24'"), strDisabledImage);
	m_pOptAutomaticMic->SetSelectedDisabledImage(strDisabledImage);
	
	IPlugin* pPlugin = g_System->GetPluginByName("Plugin_U7_AV_GlobalSetting");
	if (pPlugin)
	{
		m_pAVGlobalSetting = dynamic_cast<IPlugin_U7_AV_GlobalSetting*>(pPlugin);
	}
	
	ASSERT(m_pAVGlobalSetting);

	if (0 == m_pAVGlobalSetting->GetSpeakerVolume())
	{
		m_pOptMediaVol->Selected(true);
	}
	
	m_nCurVolume = m_pAVGlobalSetting->GetSpeakerVolume();

	m_pOptAutomaticMic->Selected(m_pAVGlobalSetting->GetPreAGC());
	m_pOptAutomaticMic->OnNotify += MakeDelegate(this, &CMenuBarDlg::OnControlNotify);

#pragma  region 新音频界面
	m_pSliderSpeaker = static_cast<CSliderUI*>(paint_manager_.FindControl(_T("SliderSpeaker")));
	ASSERT(m_pSliderSpeaker);
	m_pSliderSpeaker->SetMinValue(1);
	m_pSliderSpeaker->SetMaxValue(10);
	m_pSliderMic = static_cast<CSliderUI*>(paint_manager_.FindControl(_T("SliderMic")));
	ASSERT(m_pSliderMic);
	m_pSliderMic->SetMinValue(-3);
	m_pSliderMic->SetMaxValue(3);

	m_pLblMicdB = static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LblMicdB")));
	ASSERT(m_pLblMicdB);
	m_pLblSpeakerdB = static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LblSpeakerdB")));
	ASSERT(m_pLblSpeakerdB);

	m_pBtnSpeakerMinus = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("BtnSpeakerMinus")));
	ASSERT(m_pBtnSpeakerMinus);
	m_pBtnSpeakerAdd = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("BtnSpeakerAdd")));
	ASSERT(m_pBtnSpeakerAdd);

	m_pBtnMicMinus = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("BtnMicMinus")));
	ASSERT(m_pBtnMicMinus);
	m_pBtnMicAdd = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("BtnMicAdd")));
	ASSERT(m_pBtnMicAdd);

	IPlugin_U7_Cfg_Comm* pIPlugin_U7_Cfg_Comm = GetPluginCfgComm();

	if ( NULL != pIPlugin_U7_Cfg_Comm)
	{
		m_nSpeakerBaseline = pIPlugin_U7_Cfg_Comm->GetCfgInt(_T("AVWizard"), _T("SpeakerVolume"), 100);
		m_nMicBaseline = pIPlugin_U7_Cfg_Comm->GetCfgInt(_T("AVWizard"), _T("MicVolume"), 100);
	}

	m_nSpeakerBaseline = m_nSpeakerBaseline < 10 ? 10 : m_nSpeakerBaseline; 
	int nStep = m_nSpeakerBaseline / 10;
	nStep = nStep == 0 ? 1 : nStep;
	int nValue = m_pAVGlobalSetting->GetSpeakerVolume();
	nValue = nValue > m_nSpeakerBaseline ? m_nSpeakerBaseline : nValue;
	nValue = nValue < nStep ? nStep : nValue;
	m_pSliderSpeaker->SetChangeStep(nStep);
	if (0 != nStep)
	{
		m_pSliderSpeaker->SetValue(nValue / nStep);
	}
	//m_pBtnSpeakerMinus->SetEnabled(m_pSliderSpeaker->GetMinValue() != m_pSliderSpeaker->GetValue());
	//m_pBtnSpeakerAdd->SetEnabled(m_pSliderSpeaker->GetMaxValue() != m_pSliderSpeaker->GetValue());
	m_pOptMediaVol->Selected(m_pAVGlobalSetting->GetSpeakerMute() != 0);
	m_pOptMediaMic->Selected(m_pAVGlobalSetting->GetMicphoneMute() != 0);


	nValue = m_pAVGlobalSetting->GetMicVolume();
	int nMax = m_nMicBaseline + 30;
	nMax = nMax >= 100 ? 100 : nMax;

	int nMin = m_nMicBaseline - 30;
	nMin = nMin <= 0 ? 0 : nMin; 

	if (nValue >= nMax)
	{
		m_pSliderMic->SetValue((nMax - m_nMicBaseline) / 10);
// 		m_pBtnMicMinus->SetEnabled(true);
// 		m_pBtnMicAdd->SetEnabled(false);
	}
	else if (nValue <= nMin)
	{
		m_pSliderMic->SetValue((nMin - m_nMicBaseline) / 10);
// 		m_pBtnMicMinus->SetEnabled(false);
// 		m_pBtnMicAdd->SetEnabled(true);
	}
	else
	{
		m_pSliderMic->SetValue((nValue - m_nMicBaseline) / 10);
// 		m_pBtnMicMinus->SetEnabled(nValue >= 10);
// 		m_pBtnMicAdd->SetEnabled(true);
	}

	CoUI_SetTimer(TIMER_GET_DB, 50);

#pragma  endregion

	m_pAVGlobalSetting->AddObserver(this);

}

bool AVCONPlugin::CMenuBarDlg::OnControlNotify( void* param )
{
	TNotifyUI* pMsg = (TNotifyUI*)param;
	
	if (_tcsicmp(pMsg->pSender->GetName(), _T("valuechanged")) == 0)
	{
		OnValueChanged(pMsg->pSender->GetName());
	}
	else if(_tcsicmp(pMsg->pSender->GetName(), _T("selectchanged") )==0)
	{
		if (pMsg->pSender == m_pOptMediaVol)
		{
			if (NULL == m_pAVGlobalSetting)
				return true ;
			m_pAVGlobalSetting->SetSpeakerMute(m_pOptMediaVol->IsSelected()?1:0);
		}
		else if(pMsg->pSender == m_pOptMediaMic)
		{
			if (NULL == m_pAVGlobalSetting)
				return true;
			m_pAVGlobalSetting->SetMicphoneMute(m_pOptMediaMic->IsSelected()?1:0);

		}
		else if(pMsg->pSender == m_pOptAutomaticMic)
		{
			//自动调整麦克风音量
		}
		else
		{
			;
		}
	}
	return true;

}

void AVCONPlugin::CMenuBarDlg::OnSpeakerMuteChanged( int nMute )
{
	m_pOptMediaVol->Selected(0 != nMute );
}

void AVCONPlugin::CMenuBarDlg::OnSpeakerVolumeChanged( int nVolume )
{
	if (nVolume > m_nSpeakerBaseline)
	{
		nVolume = m_nSpeakerBaseline;
	}
	else if (nVolume < m_pSliderSpeaker->GetChangeStep())
	{
		nVolume = m_pSliderSpeaker->GetChangeStep();
	}
	m_pSliderSpeaker->SetValue(nVolume/m_pSliderSpeaker->GetChangeStep());
}

void AVCONPlugin::CMenuBarDlg::OnMicBoostChanged( int nBoost )
{

}

void AVCONPlugin::CMenuBarDlg::OnMicInputChanged( TMIC_INPUT nType )
{

}

void AVCONPlugin::CMenuBarDlg::OnMicVolumeChanged( int nVolume )
{
	int nMax = m_nMicBaseline + 30;
	nMax = nMax > 100 ? 100 : nMax;

	int nMin = m_nMicBaseline - 30;
	nMin = nMin <= 0 ? 0 : nMin; 

	if (nVolume >= nMax)
	{
		m_pSliderMic->SetValue((nMax - m_nMicBaseline) / 10);
// 		m_pBtnMicMinus->SetEnabled(true);
// 		m_pBtnMicAdd->SetEnabled(false);
		m_pBtnMicAdd->SetToolTip(_T("已到最大,不能再加"));
	}
	else if (nVolume <= nMin)
	{
		m_pSliderMic->SetValue((nMin - m_nMicBaseline) / 10);
// 		m_pBtnMicMinus->SetEnabled(false);
// 		m_pBtnMicAdd->SetEnabled(true);
	}
	else
	{
		m_pSliderMic->SetValue((nVolume - m_nMicBaseline) / 10);
// 		m_pBtnMicMinus->SetEnabled(nVolume >= 10);
// 		m_pBtnMicAdd->SetEnabled(true);
		m_pBtnMicAdd->SetToolTip(_T(""));
	}

}

void AVCONPlugin::CMenuBarDlg::OnMicMuteChanged( int nMute )
{

}

void AVCONPlugin::CMenuBarDlg::OnValueChanged( CStdString strOptName )
{
	
}

LRESULT AVCONPlugin::CMenuBarDlg::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	LRESULT lptr = WindowImplBase::OnCreate(uMsg,wParam,lParam,bHandled);
	//ShowWindow(SW_HIDE);
	return lptr;
}

void AVCONPlugin::CMenuBarDlg::OnClick( TNotifyUI& msg )
{
	CString strName = msg.pSender->GetName();

	if (_tcsicmp(strName, _T("OptAutomaticMic")) == 0)
	{
		IPlugin* pPlugin = g_System->GetPluginByName("Plugin_U7_AV_GlobalSetting");
		IPlugin_U7_AV_GlobalSetting* pAVSet = dynamic_cast<IPlugin_U7_AV_GlobalSetting*>(pPlugin);
		if (pAVSet != NULL)
		{
			if (m_pOptAutomaticMic->IsSelected())
			{
				pAVSet->SetPreAGC(0);
			}
			else
			{
				pAVSet->SetPreAGC(1);
			}
		}
	}
	
	else if (msg.pSender == m_pBtnSpeakerAdd)
	{
		int nValue = m_pAVGlobalSetting->GetSpeakerVolume();
		nValue = nValue < m_pSliderSpeaker->GetChangeStep() ? m_pSliderSpeaker->GetChangeStep() : nValue;
		nValue += m_pSliderSpeaker->GetChangeStep();
		m_pAVGlobalSetting->SetSpeakerVolume(nValue >= m_nSpeakerBaseline ? m_nSpeakerBaseline : nValue);

		m_pSliderSpeaker->SetValue(nValue / m_pSliderSpeaker->GetChangeStep());
// 		m_pBtnSpeakerAdd->SetEnabled(m_pSliderSpeaker->GetMaxValue() != m_pSliderSpeaker->GetValue());
// 		m_pBtnSpeakerMinus->SetEnabled(true);
	}
	else if (msg.pSender == m_pBtnSpeakerMinus)
	{
		int nValue = m_pAVGlobalSetting->GetSpeakerVolume();
		nValue = nValue > m_nSpeakerBaseline ? m_nSpeakerBaseline : nValue;
		nValue -= m_pSliderSpeaker->GetChangeStep();
		m_pAVGlobalSetting->SetSpeakerVolume(nValue < m_pSliderSpeaker->GetChangeStep() ? m_pSliderSpeaker->GetChangeStep() : nValue);

		m_pSliderSpeaker->SetValue(nValue / m_pSliderSpeaker->GetChangeStep());
// 		m_pBtnSpeakerMinus->SetEnabled(m_pSliderSpeaker->GetMinValue() != m_pSliderSpeaker->GetValue());
// 		m_pBtnSpeakerAdd->SetEnabled(true);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("BtnSpeakerReset")) == 0)
	{
		IPlugin_U7_Cfg_Comm* pIPlugin_U7_Cfg_Comm = GetPluginCfgComm();
		if (NULL == pIPlugin_U7_Cfg_Comm)
			return;

		m_nSpeakerBaseline = pIPlugin_U7_Cfg_Comm->GetCfgInt(_T("AVWizard"), _T("SpeakerVolume"), 100);
		int nStep = m_nSpeakerBaseline / 10;
		m_pSliderSpeaker->SetChangeStep(nStep);

		m_pAVGlobalSetting->SetSpeakerVolume(nStep * 6);
		m_pSliderSpeaker->SetValue(6);
// 		m_pBtnSpeakerAdd->SetEnabled(true);
// 		m_pBtnSpeakerMinus->SetEnabled(true);
	}
	else if (msg.pSender == m_pBtnMicAdd)
	{
		m_pSliderMic->SetValue(m_pSliderMic->GetValue() + 1);

		int nValue = 10 * m_pSliderMic->GetValue() + m_nMicBaseline;
// 		m_pBtnMicAdd->SetEnabled(m_pSliderMic->GetMaxValue() != m_pSliderMic->GetValue() ||nValue>=100);
// 		m_pBtnMicMinus->SetEnabled(true);

		m_pAVGlobalSetting->SetMicVolume(nValue);
	}
	else if (msg.pSender == m_pBtnMicMinus)
	{
		m_pSliderMic->SetValue(m_pSliderMic->GetValue() - 1);

		int nValue = 10 * m_pSliderMic->GetValue() + m_nMicBaseline;
// 		m_pBtnMicMinus->SetEnabled(m_pSliderMic->GetMinValue() != m_pSliderMic->GetValue() && nValue >= 10);
// 		m_pBtnMicAdd->SetEnabled(true);

		m_pAVGlobalSetting->SetMicVolume(nValue);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("BtnMicReset")) == 0)
	{
		IPlugin_U7_Cfg_Comm* pIPlugin_U7_Cfg_Comm = GetPluginCfgComm();
		if (NULL == pIPlugin_U7_Cfg_Comm)
			return;

		int nMicVolume = pIPlugin_U7_Cfg_Comm->GetCfgInt(_T("AVWizard"), _T("MicVolume"), 100);
		m_pAVGlobalSetting->SetMicVolume(nMicVolume);
		m_pSliderMic->SetValue(0);
// 		m_pBtnMicMinus->SetEnabled(true);
// 		m_pBtnMicAdd->SetEnabled(false);
	}


}

void AVCONPlugin::CMenuBarDlg::OnCoUITimer( TNotifyUI& msg )
{
	switch (msg.wParam)
	{
		case TIMER_USAGE_INFO:
			{
				if (m_pslidCpuusage)
				{
					static IPlugin_U7C_TerminalMesg* pTerminalMsg = dynamic_cast<IPlugin_U7C_TerminalMesg*>(g_System->GetPluginByName("Plugin_U7C_TerminalMesg"));
					if (pTerminalMsg)
					{
						int nUsage = pTerminalMsg->GetCpuUsage();
						m_pslidCpuusage->SetValue(nUsage);
					}
					else
					{
						m_pslidCpuusage->SetValue(m_cpuUsage.GetUsage());
					}
				}
				SetBitrate();
			}
			break;
		case TIMER_PROMPT:
			{
				CoUI_KillTimer(TIMER_PROMPT);
				if (m_plbPrompt)
				{
					m_plbPrompt->SetText(_T(""));
				}
			}
			break;
		case TIMER_PFLICKER:		//进会信息闪烁
			{
				if (m_bPBkChange)
				{
					m_pOptPromptLog->SetNormalImage(GetFolderPath() + _T("plogsel.png"));
				}
				else
				{
					m_pOptPromptLog->SetNormalImage(GetFolderPath() + _T("plog.png"));
				}
				m_bPBkChange = !m_bPBkChange;
			}
			break;

		case TIMER_QFLICKER:
			{
				if (m_bQBkChange)
				{
						m_pOptQuestion->SetNormalImage(GetFolderPath()+_T("questionsel.png"));
				}
				else
				{
					m_pOptQuestion->SetNormalImage(GetFolderPath()+_T("question.png"));
				}
			}
			break;
		case TIMER_GET_DB:
			{
					if ( NULL == m_pAVGlobalSetting)
					{
						return ;
					}
					int nMicphoneDB = (int)m_pAVGlobalSetting->GetMicphoneDB();
					CString strText;
					DWORD dwDBColor = GetDBColor(nMicphoneDB);
					if (dwDBColor != m_dwMicDBColor)
					{
						m_dwMicDBColor = dwDBColor;
						m_pLblMicdB->SetTextColor(m_dwMicDBColor);
					}
					strText.Format(_T("%dDB"), nMicphoneDB);
					m_pLblMicdB->SetText(strText);

					int nSpeakerDB = (int)m_pAVGlobalSetting->GetSpeakerDB();
					dwDBColor = GetDBColor(nSpeakerDB);
					if (dwDBColor != m_dwSpkDBColor)
					{
						m_dwSpkDBColor = dwDBColor;
						m_pLblSpeakerdB->SetTextColor(m_dwSpkDBColor);
					}
					strText.Format(_T("%dDB"), nSpeakerDB);
					m_pLblSpeakerdB->SetText(strText);
			}
			break;
	}
}

void AVCONPlugin::CMenuBarDlg::SetLeve()
{
	
}

void AVCONPlugin::CMenuBarDlg::SetBitrate()
{
	IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_AV_CaptureMgr");
	IPlugin_U7_AV_CaptureMgr* pU7CaptureMgr = dynamic_cast<IPlugin_U7_AV_CaptureMgr*>(pIPlugin);
	if (NULL == pU7CaptureMgr)
		return ;
	pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_AV_PlayerMgr");
	IPlugin_U7_AV_PlayerMgr* pU7PlayerMgr = dynamic_cast<IPlugin_U7_AV_PlayerMgr*>(pIPlugin);
	if (NULL == pU7PlayerMgr)
		return ;
	unsigned long ulUploadBitrate = pU7CaptureMgr->GetUploadTotalBitrate();
	unsigned long ulDownloadBitrate = pU7PlayerMgr->GetDownloadTotalBitrate();

	if (NULL != m_plbNetWork)
	{
		CString str;
		str.Format(_T("%dK/%dK"), ulUploadBitrate, ulDownloadBitrate);
		m_plbNetWork->SetText(str);
	}
}

DWORD AVCONPlugin::CMenuBarDlg::GetDBColor( int nDB )
{
	DWORD dwColor = 0xFFFFFF00;
	if (nDB<-20)
	{
		dwColor = 0xFF00FF00;
	}
	else if (nDB>-5)
	{
		dwColor = 0xFFFF0000;
	}
	return dwColor;
}

CControlUI* AVCONPlugin::CMenuBarDlg::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(pstrClass, _T("MySlider")) == 0)
	{
	//	return new CSliderUIEx();
	}
	if (_tcsicmp(pstrClass, _T("HorizontalLayoutEx")) == 0)
	{
		return new CHorizontalLayoutUIEx();
	}
	return NULL;
}

void AVCONPlugin::CMenuBarDlg::OnPreAGCChanged( int bAuto )
{

}
