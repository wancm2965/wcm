// WBEdit.cpp : ʵ���ļ�
//
#include "StdAfx.h"
#include "WBEdit.h"
#include "whiteBoard.h"


// CWBEdit

IMPLEMENT_DYNAMIC(CWBEdit, CEdit)
CWBEdit::CWBEdit()
{
	m_clrBackground	= RGB(255, 255, 255);	//������ɫ
	m_clrFrame		= RGB(61, 61, 61);	//�߿���ɫ
// 	m_clrText		= RGB(110, 90, 95);		//������ɫ

// 	m_clrDisableTxt	= RGB(185, 190, 170);	//��������ɫ
// 	m_clrDisableBkg	= RGB(230, 235, 210);	//���ñ���ɫ

	m_brushBkg.CreateSolidBrush(m_clrBackground);//����������ˢ
	m_bIsWritePageCount = false;
	m_bIsHave = false;
}

CWBEdit::~CWBEdit()
{
	m_bIsWritePageCount = false;
}


BEGIN_MESSAGE_MAP(CWBEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCPAINT()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CWBEdit ��Ϣ�������
HBRUSH CWBEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	return (HBRUSH)m_brushBkg.GetSafeHandle();
}

void CWBEdit::OnNcPaint()
{
	CRect	rectEdit, rectInser;
	CBrush	brushInser, brushOuter;
	CDC*	pDC= GetWindowDC();

	//���ñ༭�������
	GetWindowRect(rectEdit);
	ScreenToClient(rectEdit);
	rectEdit.OffsetRect(CSize(2, 2));
	rectInser.SetRect(rectEdit.left+1, rectEdit.top+1, rectEdit.right-1, rectEdit.bottom-1);

	//��������
	brushInser.CreateSolidBrush(m_clrBackground);
	brushOuter.CreateSolidBrush(m_clrFrame);

	//���߿�
	pDC->FrameRect(rectEdit, &brushOuter);
	pDC->FrameRect(rectInser, &brushInser);

	ReleaseDC(pDC);
}

void CWBEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (g_bAvconLock)
	{
		return;
	}
	if (nChar == 8)
	{
		g_pWhiteBoardView->SetIsKeyDel(TRUE);
	} 
	else
	{
		g_pWhiteBoardView->SetIsKeyDel(FALSE);
	}
	if (m_bIsWritePageCount)
	{
		if ((nChar >= 48 && nChar <= 57) || nChar == 45 || nChar == 44 || nChar == 8)
		{
			;
		}
		else
		{
			CString strPrompt;
			strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
			if (strPrompt.IsEmpty())
			{
				strPrompt = _T("��ʾ");
			}		
			CString strFolderCreateFail;
			strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SavePageToPicture"));
			if (strFolderCreateFail.IsEmpty())
			{
				strFolderCreateFail = _T("ֻ���������֣���-���������ַ�");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strFolderCreateFail, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strFolderCreateFail);
			}
			else
			{
				AfxMessageBox(strFolderCreateFail);
			}
			return;
		}
	}
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CWBEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��
	if (g_bAvconLock)
	{
		return;
	}
	/************************************************************************/
	//�ж���-���ַ�ֻ����һ��
	if ( nChar != 45)
	{
		m_bIsHave = false;
	}
	if (m_bIsHave)
	{
		return;
	}
	if (nChar == 45)
	{
		m_bIsHave = true;
	}                                                                
	/************************************************************************/
	if (IsCTRLPressed()) //����CTRL��ʱ����ռ��а�
	{
		if(OpenClipboard())//�򿪼��а�
		{
			EmptyClipboard();//��ռ��а�
			CloseClipboard();//�ر�
		}
		return;
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
