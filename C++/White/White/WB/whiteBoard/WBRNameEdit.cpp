// WBRNameEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBRNameEdit.h"


// CWBRNameEdit

IMPLEMENT_DYNAMIC(CWBRNameEdit, CEdit)

CWBRNameEdit::CWBRNameEdit()
{

}

CWBRNameEdit::~CWBRNameEdit()
{
}


BEGIN_MESSAGE_MAP(CWBRNameEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()



// CWBRNameEdit ��Ϣ�������




void CWBRNameEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	/*switch(nChar)
	{*/
	
	//case '#':
	//case '$':
	//case '&':
	//case '^':
	//case '!':
	//case '%':
	//case '@':
	//case '~':
	//case '/':
	////case '*':
	//case '?':
	//case '|':
	//case ':':
	/*case '\\':*/
	
	/* return;
	}*/
	if(nChar == '\\')
	{
		return;
	}
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
