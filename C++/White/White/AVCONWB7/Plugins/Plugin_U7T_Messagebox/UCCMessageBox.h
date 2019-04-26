#pragma once
#include "win_impl_base.hpp"
#include "InnerInterface.h"
#include <IPlugin/IU7T.Comm/IPlugin_U7_ComUI.h>
class CUCCMessageBox
	: public WindowImplBase
	, public AVCONPlugin::IPlugin_U7_ComUI_Observer
{
public:
	CUCCMessageBox(CString strTitle,CString strContent,UINT nStye, bool bAutoDestroy = false, CString strRename1Btn = _T(""),CString strRename2Btn = _T(""),CString strRename3Btn = _T(""));
	~CUCCMessageBox(void);

private:
	void OnButtonClick(TNotifyUI& msg);

	//////////////////////////////////////////////////////////////////////////
	// 继承WindowImplBase接口
public:
	virtual void Init();
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LPCTSTR GetWindowClassName() const;
	virtual CString GetSkinFile();


	void SetMsgIcon(HICON hIcon);
	void SetCaptions(UINT nID,CString strCaption);
	//实现IDlgMessageBoxNotify接口
	//////////////////////////////////////////////////////////////////////////
	virtual void OnNotifyBtnOK();
	virtual void OnNotifyBtnCancle();
	virtual void OnNotifyBtnAbort();
	virtual void OnNotifyBtnRetry();
	virtual void OnNotifyBtnIgnore();
	virtual void OnNotifyBtnYes();
	virtual void OnNotifyBtnNo();
	//////////////////////////////////////////////////////////////////////////

	void InitLanguage();

	//////////////////////////////////////////////////////////////////////////
	// 继承IPlugin_U7_ComUI_Observer接口
public:
	virtual void SetTransparent(int nOpacity);
	virtual void SetUISize(float fScale);
	virtual void SetButtonSize(float fScale);
	virtual void SetFontSize(int nSize);
	virtual void SetFont( const TFontInfo* pFontInfo );
	//////////////////////////////////////////////////////////////////////////

	int MsgBoxResetPosition(int nScreenWidth, int nScreenHeight);
protected:
	virtual void Notify(TNotifyUI& msg);

private:
	CString			m_strTitle;
	UINT				m_nStye;
	CString			m_strContent;
	DWORD				m_dwStyle;
	bool				m_bAutoDestroy;

	CTextUI*			m_pTextTitle;//皮肤控件
	CTextUI*			m_pTextContent;
	CButtonUI*			m_pBtnOK;
	CButtonUI*			m_pBtnCancle;
	CButtonUI*			m_pBtnAbort;
	CButtonUI*			m_pBtnRetry;
	CButtonUI*			m_pBtnIgnore;
	CButtonUI*			m_pBtnYes;
	CButtonUI*			m_pBtnNo;
	CHorizontalLayoutUI* m_pHLayoutFirst;
	CHorizontalLayoutUI* m_pHLayoutSecond;
	CHorizontalLayoutUI* m_pHLayoutThird;
	CHorizontalLayoutUI* m_pHLayoutBlackFirst;
	CHorizontalLayoutUI* m_pHLayoutBlackSecond;
	CHorizontalLayoutUI* m_pHLayoutBlackThird;
	int                 m_nBtnValue;
	CString			m_strRename1Btn;
	CString			m_strRename2Btn;
	CString			m_strRename3Btn;

	CString				m_strLangOK;
	CString				m_strLangCancel;
	CString				m_strLangStop;
	CString				m_strLangRetry;
	CString				m_strLangIgnore;
	CString				m_strLangYes;
	CString				m_strLangNo;
};
