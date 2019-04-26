#include "StdAfx.h"
#include "GlobalMessageBoxManager.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

void CGlobalMessageBoxManager::AddMessageBox(CUCCMessageBox *pMess)
{
	if (m_listMessageBoxes.size() == 0)
	{
		RECT rc1;
		RECT rc2;
		::GetWindowRect(pMess->GetHWND(),&rc1);
		::GetWindowRect(GetDesktopWindow(),&rc2);
		::MoveWindow(pMess->GetHWND(),(rc2.right-475)/2,(rc2.bottom-170)/2,475,170, TRUE);
	}
	else
	{
		/*CUCCMessageBox *dlgMsgintemp = m_listMessageBoxes.back();
		RECT rc;
		::GetWindowRect(dlgMsgintemp->GetHWND(),&rc);
		::MoveWindow(pMess->GetHWND(),rc.left-20,rc.top-20,430, 260,TRUE);*/
		//张志明修改  解决弹出N个对话框问题
		CUCCMessageBox* dlgMsgintemp = m_listMessageBoxes.back();
		dlgMsgintemp->Close(IDCANCEL);

		RECT rc1;
		RECT rc2;
		::GetWindowRect(pMess->GetHWND(),&rc1);
		::GetWindowRect(GetDesktopWindow(),&rc2);
		::MoveWindow(pMess->GetHWND(),(rc2.right-475)/2,(rc2.bottom-170)/2,475,170, TRUE);
	}

	m_listMessageBoxes.push_back(pMess);
}

void CGlobalMessageBoxManager::DelMessageBox(CUCCMessageBox *pMess)
{
	std::list<CUCCMessageBox*>::iterator it = m_listMessageBoxes.begin();
	for (; it != m_listMessageBoxes.end(); ++it)
	{
		if (*it == pMess)
		{
			//if (::IsWindow(pMess->GetHWND()))//避免非正常消亡
			//{
			//	pMess->Close(IDCLOSE);
			//}
			m_listMessageBoxes.erase(it);
			break;
		}
	}
}

void CGlobalMessageBoxManager::DeleteAllMessageBox()
{
	std::list<CUCCMessageBox*>::const_iterator it = m_listMessageBoxes.begin();
	for (; it != m_listMessageBoxes.end(); ++it)
	{
		if (::IsWindow((*it)->GetHWND()))
		{
			(*it)->Close(IDCLOSE);
		}
	}
	m_listMessageBoxes.clear();
}
