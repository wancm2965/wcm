#include "StdAfx.h"
#include "UCCMessageBox.h"
#include <IPlugin/IU7T.Comm/IPlugin_U7_Common.h>
#include <ISystem/ISystem.h>
using namespace AVCONPlugin;

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
CUCCMessageBox::CUCCMessageBox(CString strTitle,CString strContent,UINT nStye, bool bAutoDestroy/* = false*/,CString strRename1Btn/* = _T("")*/,CString strRename2Btn/* = _T("")*/,CString strRename3Btn/* = _T("")*/)
: m_strTitle(strTitle)
, m_nStye(nStye)
, m_strContent(strContent)
, m_bAutoDestroy(bAutoDestroy)
, m_pTextTitle(NULL)
, m_pTextContent(NULL)
, m_pBtnOK(NULL)
, m_pBtnCancle(NULL)
, m_pBtnAbort(NULL)
, m_pBtnRetry(NULL)
, m_pBtnIgnore(NULL)
, m_pBtnYes(NULL)
, m_pBtnNo(NULL)
, m_pHLayoutFirst(NULL)
, m_pHLayoutSecond(NULL)
, m_pHLayoutThird(NULL)
, m_strRename1Btn(strRename1Btn)
, m_strRename2Btn(strRename2Btn)
, m_strRename3Btn(strRename3Btn)
, m_strLangOK(_T("ȷ��"))
, m_strLangCancel(_T("ȡ��"))
, m_strLangStop(_T("��ֹ"))
, m_strLangRetry(_T("����"))
, m_strLangIgnore(_T("����"))
, m_strLangYes(_T("��"))
, m_strLangNo(_T("��"))
{
}

CUCCMessageBox::~CUCCMessageBox()
{
}

//////////////////////////////////////////////////////////////////////////
// �̳�WindowImplBase�ӿ�
void CUCCMessageBox::Init()
{
	InitLanguage();
	m_pHLayoutFirst = static_cast<CHorizontalLayoutUI*>(paint_manager_.FindControl(_T("HLayoutBottomFirst")));
	m_pHLayoutSecond = static_cast<CHorizontalLayoutUI*>(paint_manager_.FindControl(_T("HLayoutBottomSecond")));
	m_pHLayoutThird = static_cast<CHorizontalLayoutUI*>(paint_manager_.FindControl(_T("HLayoutBottomThird")));
	m_pHLayoutBlackFirst = static_cast<CHorizontalLayoutUI*>(paint_manager_.FindControl(_T("HLayoutBottom2")));
	m_pHLayoutBlackSecond = static_cast<CHorizontalLayoutUI*>(paint_manager_.FindControl(_T("HLayoutBottom3")));
	m_pHLayoutBlackThird = static_cast<CHorizontalLayoutUI*>(paint_manager_.FindControl(_T("HLayoutBottom4")));
	m_pTextTitle    = static_cast<CTextUI*>(paint_manager_.FindControl(_T("TextTitle")));
	m_pTextContent  = static_cast<CTextUI*>(paint_manager_.FindControl(_T("TextContent")));
	m_pTextContent->SetAttribute(_T("endellipsis"),_T("true"));
	m_pTextTitle->SetText(m_strTitle);
	// ʹ�����߿����ĸ��ʡ�ԺŽ�������ο����Ż�����ʹ�м�ʡ�ԣ�������ʾ	wangxin 2012/09/05 11
	m_pTextContent->SetTextStyle(m_pTextContent->GetTextStyle() | DT_WORD_ELLIPSIS);
	m_pTextContent->SetText(m_strContent);
	m_pTextContent->SetToolTip(m_strContent);
	switch(m_nStye)
	{
	case MB_OK:
		{
			m_pBtnCancle  = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Third")));	
			if (m_strRename1Btn == _T(""))
				m_pBtnCancle->SetText(m_strLangOK);
			else
				m_pBtnCancle->SetText(m_strRename1Btn);

			if (NULL != m_pHLayoutBlackThird)
			{
				m_pHLayoutBlackThird->SetVisible(true);
			}
			//�ʰ���ʹ��ť����
			if (NULL != m_pHLayoutThird)
			{
				m_pHLayoutThird->SetVisible(true);
			}
			m_pBtnCancle->SetVisible(TRUE);
		}
		break;
	case MB_OKCANCEL:
		{
			m_pBtnOK = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Second")));	
			m_pBtnCancle = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Third")));
			if (m_strRename1Btn == _T(""))
			{
				m_pBtnOK->SetText(m_strLangOK);
				m_pBtnCancle->SetText(m_strLangCancel);
			} 
			else
			{
				m_pBtnOK->SetText(m_strRename1Btn);
				m_pBtnCancle->SetText(m_strRename2Btn);
			}
			m_pBtnOK->SetVisible(TRUE);
			m_pBtnCancle->SetVisible(TRUE);
			//�ڰװ���ʹ��ť����
			if (NULL != m_pHLayoutBlackSecond)
			{
				m_pHLayoutBlackSecond->SetVisible(true);
			}
			if (NULL != m_pHLayoutBlackThird)
			{
				m_pHLayoutBlackThird->SetVisible(true);
			}
			//�ʰ���ʹ��ť����
			if (NULL != m_pHLayoutSecond)
			{
				m_pHLayoutSecond->SetVisible(true);
			}
			if (NULL != m_pHLayoutThird)
			{
				m_pHLayoutThird->SetVisible(true);
			}
		}
		break;
	case MB_ABORTRETRYIGNORE:
		{
			m_pBtnAbort = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_First")));
			m_pBtnRetry = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Second")));
			m_pBtnIgnore = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Third")));
			if (m_strRename1Btn == _T(""))
			{
				m_pBtnAbort->SetText(m_strLangCancel);
				m_pBtnRetry->SetText(m_strLangRetry);
				m_pBtnIgnore->SetText(m_strLangIgnore);
			} 
			else
			{
				m_pBtnAbort->SetText(m_strRename1Btn);
				m_pBtnRetry->SetText(m_strRename2Btn);
				m_pBtnIgnore->SetText(m_strRename3Btn);
			}
			m_pBtnAbort->SetVisible(TRUE);
			m_pBtnRetry->SetVisible(TRUE);
			m_pBtnIgnore->SetVisible(TRUE);
			//�ڰװ���ʹ��ť����
			if (NULL != m_pHLayoutBlackFirst)
			{
				m_pHLayoutBlackFirst->SetVisible(true);
			}
			if (NULL != m_pHLayoutBlackSecond)
			{
				m_pHLayoutBlackSecond->SetVisible(true);
			}
			if (NULL != m_pHLayoutBlackThird)
			{
				m_pHLayoutBlackThird->SetVisible(true);
			}
			//�ʰ���ʹ��ť����
			if (NULL != m_pHLayoutFirst)
			{
				m_pHLayoutFirst->SetVisible(true);
			}
			if (NULL != m_pHLayoutSecond)
			{
				m_pHLayoutSecond->SetVisible(true);
			}
			if (NULL != m_pHLayoutThird)
			{
				m_pHLayoutThird->SetVisible(true);
			}
		}
		break;
	case MB_YESNOCANCEL:
		{
			m_pBtnYes= static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_First")));
			m_pBtnNo = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Second")));
			m_pBtnCancle = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Third")));
			if (m_strRename1Btn == _T(""))
			{
				m_pBtnYes->SetText(m_strLangYes);
				m_pBtnNo->SetText(m_strLangNo);
				m_pBtnCancle->SetText(m_strLangCancel);
			} 
			else
			{
				m_pBtnYes->SetText(m_strRename1Btn);
				m_pBtnNo->SetText(m_strRename2Btn);
				m_pBtnCancle->SetText(m_strRename3Btn);
			}
			m_pBtnYes->SetVisible(TRUE);
			m_pBtnNo->SetVisible(TRUE);
			m_pBtnCancle->SetVisible(TRUE);
			//�ڰװ���ʹ��ť����
			if (NULL != m_pHLayoutBlackFirst)
			{
				m_pHLayoutBlackFirst->SetVisible(true);
			}
			if (NULL != m_pHLayoutBlackSecond)
			{
				m_pHLayoutBlackSecond->SetVisible(true);
			}
			if (NULL != m_pHLayoutBlackThird)
			{
				m_pHLayoutBlackThird->SetVisible(true);
			}
			//�ʰ���ʹ��ť����
			if (NULL != m_pHLayoutFirst)
			{
				m_pHLayoutFirst->SetVisible(true);
			}
			if (NULL != m_pHLayoutSecond)
			{
				m_pHLayoutSecond->SetVisible(true);
			}
			if (NULL != m_pHLayoutThird)
			{
				m_pHLayoutThird->SetVisible(true);
			}
		}
		break;
	case MB_YESNO:
		{
			m_pBtnYes = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Second")));
			m_pBtnNo = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Third")));
			if (m_strRename1Btn == _T(""))
			{
				m_pBtnYes->SetText(m_strLangYes);
				m_pBtnNo->SetText(m_strLangNo);
			} 
			else
			{
				m_pBtnYes->SetText(m_strRename1Btn);
				m_pBtnNo->SetText(m_strRename2Btn);
			}
			m_pBtnYes->SetVisible(true);
			m_pBtnNo->SetVisible(true);
			//�ڰװ���ʹ��ť����
			if (NULL != m_pHLayoutBlackSecond)
			{
				m_pHLayoutBlackSecond->SetVisible(true);
			}
			if (NULL != m_pHLayoutBlackThird)
			{
				m_pHLayoutBlackThird->SetVisible(true);
			}
			//�ʰ���ʹ��ť����
			if (NULL != m_pHLayoutSecond)
			{
				m_pHLayoutSecond->SetVisible(true);
			}
			if (NULL != m_pHLayoutThird)
			{
				m_pHLayoutThird->SetVisible(true);
			}
		}
		break;
	case MB_RETRYCANCEL:
		{
			m_pBtnRetry = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Second")));
			m_pBtnCancle = static_cast<CButtonUI*>(paint_manager_.FindControl(_T("Btn_Third")));
			if (m_strRename1Btn == _T(""))
			{
				m_pBtnRetry->SetText(m_strLangRetry);
				m_pBtnCancle->SetText(m_strLangCancel);
			} 
			else
			{
				m_pBtnRetry->SetText(m_strRename1Btn);
				m_pBtnCancle->SetText(m_strRename2Btn);
			}
			m_pBtnRetry->SetVisible(TRUE);
			m_pBtnCancle->SetVisible(TRUE);
			//�ڰװ���ʹ��ť����
			if (NULL != m_pHLayoutBlackSecond)
			{
				m_pHLayoutBlackSecond->SetVisible(true);
			}
			if (NULL != m_pHLayoutBlackThird)
			{
				m_pHLayoutBlackThird->SetVisible(true);
			}
			//�ʰ���ʹ��ť����
			if (NULL != m_pHLayoutSecond)
			{
				m_pHLayoutSecond->SetVisible(true);
			}
			if (NULL != m_pHLayoutThird)
			{
				m_pHLayoutThird->SetVisible(true);
			}
		}
		break;
	}
	IPlugin_U7_ComUI* pluginComUI = dynamic_cast<IPlugin_U7_ComUI*>(AVCONPlugin::g_System->GetPluginByName(PLUGIN_NAME_U7_COMUI));
	if(NULL != pluginComUI)
	{
		const TFontInfo* pFontinfo = pluginComUI->GetDefaultFont();
		paint_manager_.SetDefaultFont(pFontinfo->sFontName, pFontinfo->iSize, pFontinfo->bBold, pFontinfo->bUnderline, pFontinfo->bItalic);
		paint_manager_.GetRoot()->NeedUpdate();
		pluginComUI->AddObserver(this);
	}
}

LRESULT CUCCMessageBox::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	IPlugin_U7_ComUI* pluginComUI = dynamic_cast<IPlugin_U7_ComUI*>(AVCONPlugin::g_System->GetPluginByName(PLUGIN_NAME_U7_COMUI));
	if(NULL != pluginComUI)
	{
		pluginComUI->DelObserver(this);
	}
	return WindowImplBase::OnDestroy(uMsg, wParam, lParam, bHandled);
}

LRESULT CUCCMessageBox::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (m_bAutoDestroy)
	{
		Close(IDCLOSE);
		return TRUE;
	}
	return WindowImplBase::OnKillFocus(uMsg, wParam, lParam, bHandled);
}

LPCTSTR CUCCMessageBox::GetWindowClassName() const
{
	return _T("CUCCMessageBox");
}

CString CUCCMessageBox::GetSkinFile()
{
	return _T("UCCMessageBox.xml");
}

void CUCCMessageBox::Notify(TNotifyUI& msg)
{
	if (COUI_MSGTYPE_CLICK == msg.nType)
	{
		// ��ΪMessageBox��ֻ�е����ť����Ӧ����Ϣ,
		// �����ť֮��ͻ����ٶԻ��������ڴ�ʱ��MessageBOX,�Ƴ�UI����
//		GetIMainFrameFun().RemoveWindow(this);
		if ( NULL == msg.pSender )
		{
			return;
		}
		else if ( msg.pSender  == m_pBtnOK )
		{
			OnNotifyBtnOK();
//			GetConference().MessageBoxDlgDelCount(this);
		}
		
		else if ( msg.pSender  == m_pBtnCancle)
		{
			OnNotifyBtnCancle();
//			GetConference().MessageBoxDlgDelCount(this);
		}

		else if ( msg.pSender  == m_pBtnAbort)
		{
			OnNotifyBtnAbort();
//			GetConference().MessageBoxDlgDelCount(this);
		}

		else if ( msg.pSender  == m_pBtnRetry)
		{
			OnNotifyBtnRetry();
//			GetConference().MessageBoxDlgDelCount(this);
		}

		else if ( msg.pSender  == m_pBtnIgnore)
		{
			OnNotifyBtnIgnore();
//			GetConference().MessageBoxDlgDelCount(this);
		}

		else if ( msg.pSender  == m_pBtnYes)
		{
			OnNotifyBtnYes();
//			GetConference().MessageBoxDlgDelCount(this);
		}

		else if ( msg.pSender  == m_pBtnNo)
		{
			OnNotifyBtnNo();
//			GetConference().MessageBoxDlgDelCount(this);
		}
		return;
	}
	else if (COUI_MSGTYPE_ITEMSELECT == msg.nType)
	{
// 		if (_tcsicmp(msg.pSender->GetName(), kComSearchRangeControlName) == 0)
// 		{
// 			//CComboUI* SearchRange = static_cast<CComboUI*>(msg.pSender);
// 
// // 			if (m_pComSearchType != NULL)
// // 			{
// // 				SearchRange_Type = m_pComSearchType->GetText();
// // 				//FontStyleChanged();
// // 			}
// 		}
		// 		else if (_tcsicmp(msg.pSender->GetName(), kFontSizeControlName) == 0)
		// 		{
		// 			CComboUI* font_size = static_cast<CComboUI*>(msg.pSender);
		// 			if (font_size != NULL)
		// 			{
		// 				font_size_ = _ttoi(font_size->GetText());
		// 				FontStyleChanged();
		// 			}
		// 		}
	}
}
//////////////////////////////////////////////////////////////////////////

void CUCCMessageBox::OnNotifyBtnOK()
{
	Close(IDOK);
	m_nBtnValue = UCCMB_BTN_OK;
}

void CUCCMessageBox::OnNotifyBtnCancle()
{
	Close(IDCANCEL);
	m_nBtnValue = UCCMB_BTN_CANCLE;
}

void CUCCMessageBox::OnNotifyBtnAbort()
{
	Close(IDABORT);
	m_nBtnValue = UCCMB_BTN_ABORT;
}
void CUCCMessageBox::OnNotifyBtnRetry()
{
	Close(IDRETRY);
	m_nBtnValue = UCCMB_BTN_RETRY;
}
void CUCCMessageBox::OnNotifyBtnIgnore()
{
	Close(IDIGNORE);
	m_nBtnValue = UCCMB_BTN_IGNORE;
}
void CUCCMessageBox::OnNotifyBtnYes()
{
	Close(IDYES);
	m_nBtnValue = UCCMB_BTN_YES;
}
void CUCCMessageBox::OnNotifyBtnNo()
{
	Close(IDNO);
	m_nBtnValue = UCCMB_BTN_NO;
}


void CUCCMessageBox::InitLanguage()
{
	if (GetPluginU7Lang())
	{
		m_strLangCancel	= GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("CANCEL"), _T("ȡ��"));
		m_strLangOK		= GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("OK"), _T("ȷ��"));
		m_strLangStop	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_MessageBox"), _T("STOP"), _T("��ֹ"));
		m_strLangRetry	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_MessageBox"), _T("RETRY"), _T("����"));
		m_strLangIgnore	= GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_MessageBox"), _T("IGNORE"), _T("����"));
		m_strLangYes	= GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("YES"), _T("��"));
		m_strLangNo		= GetPluginU7Lang()->GetCommLangString(_T("LANGUAGE"), _T("NO"), _T("��"));
	}
}

//===========================================================================
//��������: ��MessageBox�ƶ������½�
//ʱ��    : 2011/12/29
//����    : ��
//����ֵ  : ��
//===========================================================================
int CUCCMessageBox::MsgBoxResetPosition(int nScreenWidth, int nScreenHeight)
{
// 	UICRect rc;
// 	CWnd* pcWnd = CWnd::FromHandle(this->GetHWND());
// 	pcWnd->GetClientRect(rc);
// 	pcWnd->ClientToScreen(rc);
// 	SetWindowPos(m_hWnd,NULL,nScreenWidth-rc.GetWidth()-20,nScreenHeight-rc.GetHeight()),rc.GetHeight(),rc.GetHeight(),SWP_SHOWWINDOW|SWP_NOZORDER);	
// 	return ShowModal();

	return 0;
}

void CUCCMessageBox::SetUISize( float fScale )
{
//	GetIMainFrameFun().SetUISize(m_hWnd, &paint_manager_, fScale);
}

void CUCCMessageBox::SetTransparent( int nOpacity )
{
	paint_manager_.SetTransparent(nOpacity);
}

void CUCCMessageBox::SetButtonSize( float fScale )
{
// 	CStdPtrArray aPlugins = *paint_manager_.FindSubControlsByClass(GetRoot(), _T("ButtonUI"));
// 	int nCount = aPlugins.GetSize();
// 	for (int i = 0; i < nCount; i++)
// 	{
// 		CButtonUI* pBtn = static_cast<CButtonUI*>(aPlugins[i]);
// 		if (!pBtn->IsFloat())
// 			continue;
// 
// 		RECT rc = pBtn->GetPos();
// 		RECT rcDefault = pBtn->GetDefaultPos();
// 
// 		int nHCenter = (rc.left + rc.right) / 2;
// 		int nVCenter = (rc.top + rc.bottom) / 2;
// 
// 		int nHeight = (int)((rcDefault.bottom - rcDefault.top) * fScale);
// 		int nWidth = (int)((rcDefault.right - rcDefault.left) * fScale);
// 
// 		rc.top = nVCenter - nHeight / 2;
// 		if (rc.top < 0)
// 			continue;
// 		rc.bottom = rc.top + nHeight;
// 		rc.left = nHCenter - nWidth / 2;
// 		if(rc.left < 0)
// 			continue;
// 		rc.right = rc.left + nWidth;
// 		pBtn->SetPos(rc);
// 	}
}

void CUCCMessageBox::SetFontSize( int nSize )
{
	TFontInfo* pFontInfo = paint_manager_.GetDefaultFontInfo();
	if( pFontInfo == NULL)
		return;

	pFontInfo->iSize = nSize;

	paint_manager_.SetDefaultFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, 
		pFontInfo->bUnderline, pFontInfo->bItalic);

	GetRoot()->NeedUpdate();
}

void CUCCMessageBox::SetFont( const TFontInfo* pFontInfo )
{
	paint_manager_.SetDefaultFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, 
		pFontInfo->bUnderline, pFontInfo->bItalic);
}
