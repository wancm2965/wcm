#ifndef __DlG_MESSAGEBOX_H__
#define __DlG_MESSAGEBOX_H__
#include <CoUI/Utils/CoAPI.h>
#include <CoUI/Utils/WinImplBase.h>
#include <CoUI/Control/UIText.h>
#include <CoUI/Control/UIButton.h>



namespace CoUI {
	/////////////////////////////////////////////////////////////////////////////////////

//暂时屏蔽导出进行扩展
	class /*UILIB_API*/ CDlgMessageBox : public WindowImplBaseBase
	{
	public:
		CDlgMessageBox(CStdString strTitle, CStdString strContent, 
			UINT nWndStyle, const MsgboxAppendData &pAppendData, const int &nResID);

		//非主动关闭
		void ForceClose();
		virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual void OnFinalMessage(HWND hWnd);
	private:
		//重写虚函数
		virtual LPCTSTR CDlgMessageBox::GetWindowClassName()const;
		virtual CStdString GetSkinFile();
		virtual CStdString GetSkinFolder();
		virtual unsigned int GetResourceID() const;
		virtual HINSTANCE GetResouceInstance();
		virtual EDialogResType GetResType()const;

	private:
		//窗口初始化过程
		virtual void Init();
		void InitTitle();
		void InitContent();

	private:
		virtual void Notify(TNotifyUI& msg);

	private:
		CStdString			m_strTitle;			//弹出窗标题
		CStdString			m_strContent;		//提示文本内容
		const UINT			m_nWndStye;			//窗口类型配置

		bool				m_bTimeOutDestroy;	//超时自动消亡
		UINT				m_nSecTimeOut;
		bool				m_bAutoDestroy;		//失去焦点自动消亡
		UINT				m_nMaxLineContent;	//内容最大行数
		bool				m_bCustomPosition;
		UICPoint			m_ptWnd;
		bool				m_bContentCenter;
		CButtonUI*          m_pBtnTimeOutDestroy;//超时自动消亡提示按钮
		CStdString          m_strBtn;            //超时自动消亡提示按钮的字

		CLabelUI*			m_pTextTitle;       
		CTextUI*			m_pTextContent;

		CLabelUI*			m_pLabelIcon; 
		CButtonUI*			m_pBtnOK;
		CButtonUI*			m_pBtnCancle;
		CButtonUI*			m_pBtnAbort;
		CButtonUI*			m_pBtnRetry;
		CButtonUI*			m_pBtnIgnore;
		CButtonUI*			m_pBtnYes;
		CButtonUI*			m_pBtnNo;
		CButtonUI*			m_pBtnClose;

	private:
		const int m_nResID;
	};
} // namespace CoUI
#endif // __DlG_MESSAGEBOX_H__
