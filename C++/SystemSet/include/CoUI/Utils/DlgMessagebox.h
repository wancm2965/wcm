#ifndef __DlG_MESSAGEBOX_H__
#define __DlG_MESSAGEBOX_H__
#include <CoUI/Utils/CoAPI.h>
#include <CoUI/Utils/WinImplBase.h>
#include <CoUI/Control/UIText.h>
#include <CoUI/Control/UIButton.h>



namespace CoUI {
	/////////////////////////////////////////////////////////////////////////////////////

//��ʱ���ε���������չ
	class /*UILIB_API*/ CDlgMessageBox : public WindowImplBaseBase
	{
	public:
		CDlgMessageBox(CStdString strTitle, CStdString strContent, 
			UINT nWndStyle, const MsgboxAppendData &pAppendData, const int &nResID);

		//�������ر�
		void ForceClose();
		virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual void OnFinalMessage(HWND hWnd);
	private:
		//��д�麯��
		virtual LPCTSTR CDlgMessageBox::GetWindowClassName()const;
		virtual CStdString GetSkinFile();
		virtual CStdString GetSkinFolder();
		virtual unsigned int GetResourceID() const;
		virtual HINSTANCE GetResouceInstance();
		virtual EDialogResType GetResType()const;

	private:
		//���ڳ�ʼ������
		virtual void Init();
		void InitTitle();
		void InitContent();

	private:
		virtual void Notify(TNotifyUI& msg);

	private:
		CStdString			m_strTitle;			//����������
		CStdString			m_strContent;		//��ʾ�ı�����
		const UINT			m_nWndStye;			//������������

		bool				m_bTimeOutDestroy;	//��ʱ�Զ�����
		UINT				m_nSecTimeOut;
		bool				m_bAutoDestroy;		//ʧȥ�����Զ�����
		UINT				m_nMaxLineContent;	//�����������
		bool				m_bCustomPosition;
		UICPoint			m_ptWnd;
		bool				m_bContentCenter;
		CButtonUI*          m_pBtnTimeOutDestroy;//��ʱ�Զ�������ʾ��ť
		CStdString          m_strBtn;            //��ʱ�Զ�������ʾ��ť����

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
