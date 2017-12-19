#ifndef __UISEDIT_H__
#define __UISEDIT_H__

#include <oleidl.h>
#include "UILabel.h"

namespace CoUI
{
	class CSimpleTxtWinHost;
#define COUI_REST_CTRLC 0x01
#define COUI_REST_CTRLV 0x02
#define COUI_REST_CTRLX 0x04
#define COUI_REST_CTRLZ 0x08

	class UILIB_API CSimpleRichEditUI : public CLabelUI, public IMessageFilterUI
	{
	public:
		CSimpleRichEditUI();
		~CSimpleRichEditUI();
		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		bool IsRich();
		bool IsReadOnly();
		void SetWantTab(bool bWantTab = true);
		void SetWantReturn(bool bWantReturn = true);
		void SetWantCtrlReturn(bool bWantCtrlReturn = true);
		void SetRich(bool bRich = true);
		int GetFont();
		void SetFont(int index);
		void SetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
		DWORD GetTextColor();
		void SetTextColor(DWORD dwTextColor);
		int GetLimitText();
		long GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
		CStdString GetText() const;
		void SetText(LPCTSTR pstrText);
		bool GetModify() const;
		void SetModify(bool bModified = true) const;
		void GetSel(CHARRANGE &cr) const;
		void GetSel(long& nStartChar, long& nEndChar) const;
		int SetSel(CHARRANGE &cr);
		int SetSel(long nStartChar, long nEndChar);
		void ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo = false);
		void ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo = false);
		CStdString GetSelText() const;
		int SetSelAll();
		int SetSelNone();
		WORD GetSelectionType() const;
		bool GetZoom(int& nNum, int& nDen) const;
		bool SetZoom(int nNum, int nDen);
		bool SetZoomOff();
		bool GetAutoURLDetect() const;
		bool SetAutoURLDetect(bool bAutoDetect = true);
		DWORD GetEventMask() const;
		DWORD SetEventMask(DWORD dwEventMask);
		CStdString GetTextRange(long nStartChar, long nEndChar) const;
		void HideSelection(bool bHide = true, bool bChangeStyle = false);
		void ScrollCaret();
		int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo = false);
		int AppendText(LPCTSTR lpstrText, bool bCanUndo = false);
		DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
		DWORD GetDefaultCharFormat(CHARFORMAT &cf) const;
		bool SetDefaultCharFormat(CHARFORMAT2 &cf);
		bool SetDefaultCharFormat(CHARFORMAT &cf);
		DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
		bool SetSelectionCharFormat(CHARFORMAT2 &cf);
		bool SetSelectionCharFormat(CHARFORMAT &cf);
		bool SetWordCharFormat(CHARFORMAT2 &cf);
		DWORD GetParaFormat(PARAFORMAT2 &pf) const;
		bool SetParaFormat(PARAFORMAT2 &pf);
		bool Redo();
		bool Undo();
		void Clear();
		void Copy();
		void Cut();
		void Paste();
		int GetLineCount() const;
		CStdString GetLine(int nIndex, int nMaxLength) const;
		int LineIndex(int nLine = -1) const;
		int LineLength(int nLine = -1) const;
		bool LineScroll(int nLines, int nChars = 0);
		UICPoint GetCharPos(long lChar) const;
		long LineFromChar(long nIndex) const;
		UICPoint PosFromChar(UINT nChar) const;
		int CharFromPos(UICPoint pt) const;
		void EmptyUndoBuffer();
		UINT SetUndoLimit(UINT nLimit);
		long StreamIn(int nFormat, EDITSTREAM &es);
		long StreamOut(int nFormat, EDITSTREAM &es);

		void DoInit();
		// 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
		// 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
		virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const; 
		IDropTarget* GetTxDropTarget();
		void RegisterDrag();
		void UnRegisterDrag();
		virtual bool OnTxViewChanged();
		void SetPos(RECT rc);
		void SetInternVisible(bool bVisible );
		void DoEvent(TEventUI& event);
		void DoPaint(HDC hDC, const RECT& rcPaint);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
		void SetShowCaret(BOOL bShow = FALSE);
		BOOL IsShowCaret();
	protected:
		CSimpleTxtWinHost* m_pTwh;
		DWORD m_nDisabledShortCut;//被禁用的快捷键，参考相应的快捷键标识定义
		bool m_bVScrollBarFixing;
		bool m_bRich;
		bool m_bReadOnly;
		bool m_bWantReturn;
		bool m_bWantCtrlReturn;
		bool m_bWantTab;
		DWORD m_dwTextColor;
		int m_iFont;
		int m_iLimitText;
		LONG m_lTwhStyle;
		bool m_bInited;
		bool m_bSupportDrag;//默认为false，即不支持拖拽
	};
} // namespace CoUI

#endif // __UISEDIT_H__

