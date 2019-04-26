// WBRNameEdit.cpp : 实现文件
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



// CWBRNameEdit 消息处理程序




void CWBRNameEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
