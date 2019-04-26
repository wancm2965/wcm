#ifndef __UICOMINTERFACE_H__
#define __UICOMINTERFACE_H__

using namespace CoUI;

class CCommBtnMgr
{
public:
	CCommBtnMgr(CPaintManagerUI* pPaintManager)
		: m_pPaintManager(pPaintManager)
		, m_Wnd(NULL)
		, m_bottomMaxHeight(58)
	{
		m_initSize = pPaintManager->GetInitSize();
	}

	virtual ~CCommBtnMgr() 
	{
	}
public:
	void AdjustBtnPos(float fScale) 
	{
		RECT clientRect;
		RECT posRect;
		int right = 20;
		SIZE currentSize;

		GetClientRect(m_Wnd, &clientRect);
		currentSize.cx = clientRect.right - clientRect.left;
		currentSize.cy = 0;
		RECT rcDefault ;

		int nHeight = 0;
		int nWidth = 0;
		int nVCenter = 0;
		for (int i = m_autoAdjustBtn.GetSize() - 1; i >= 0; i--)
		{
			CButtonUI* pBtn = static_cast<CButtonUI*>(m_autoAdjustBtn.GetAt(i));
			RECT rc = pBtn->GetPos();

			nVCenter = (rc.top + rc.bottom) / 2 ;

			rcDefault = pBtn->GetDefaultPos();

			nHeight = (int)((rcDefault.bottom - rcDefault.top) * fScale);
			nWidth = (int)((rcDefault.right - rcDefault.left) * fScale);

			posRect.right = currentSize.cx - right;
			right +=nWidth + 20;

			posRect.left = posRect.right - nWidth;

			if (nHeight >= m_bottomMaxHeight)
			{
				if (rc.top <= 0)
				{
					continue;
				}
				if (rc.bottom - rc.top < nHeight)
				{
					int height = (m_bottomMaxHeight - (rc.bottom - rc.top))/2;
					posRect.top = rc.top - height;
					posRect.bottom = posRect.top + m_bottomMaxHeight;
				}
				else
				{
					posRect.top = rc.top;
					posRect.bottom = rc.bottom;
				}
				pBtn->SetPos(posRect);
				continue;
			}
			posRect.top = nVCenter - (nHeight/2);

			posRect.bottom = posRect.top + nHeight;

			pBtn->SetPos(posRect);

		}
	}
	void InitSetButtonSize(HWND hWnd, float fScale)
	{
		if (NULL == m_Wnd)
		{
			m_Wnd = hWnd;
		}
		SIZE currentSize;
		currentSize.cx = (LONG)(m_initSize.cx * fScale);
		currentSize.cy = (LONG)(m_initSize.cy * fScale);

		int nHCenter = 0;
		int nVCenter = 0;
		int nHeight = 0;
		int nWidth = 0;
		/*CStdPtrArray aPlugins = *(m_pPaintManager->FindSubControlsByClass(m_pPaintManager->GetRoot(), _T("ButtonUI")));*/
		int nCount = m_autoAdjustBtn.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			CButtonUI* pBtn = static_cast<CButtonUI*>(m_autoAdjustBtn[i]);
			if (!pBtn->IsFloat())
				continue;

			if (_tcsicmp(pBtn->GetName(), _T("btnClose")) == 0)
			{
				continue;
			}
			SIZE rc = pBtn->GetFixedXY();
			RECT rcDefault = pBtn->GetDefaultPos();

			nHCenter = rc.cx + (rcDefault.right - rcDefault.left) / 2;
			nVCenter = rc.cy + (rcDefault.bottom - rcDefault.top) / 2;


			RECT rct;
			if(_tcsicmp(pBtn->GetName(), _T("BtnClose")) == 0)
			{
				continue;
			}

			nHeight = (int)((rcDefault.bottom - rcDefault.top) * fScale);
			nWidth = (int)((rcDefault.right - rcDefault.left) * fScale);

			if (nHeight > m_bottomMaxHeight)
			{
				nHeight = m_bottomMaxHeight;
				rct.top = 0;
			}
			else
			{
				rct.top = (m_bottomMaxHeight - nHeight)/2;
			}
			rct.bottom = rct.top + nHeight;
			rct.left = nHCenter - (nWidth / 2);
			if (rct.left < 0)
			{
				rct.left = 0;
			}
			rct.right = rct.left + nWidth;

			pBtn->SetPos(rct);
		}
	}

	//去掉了index参数，直接把pBtn加到列表的最后
	bool AddBtn(CButtonUI* pBtn)
	{
		return m_autoAdjustBtn.Add(pBtn);
	}
private:
	CPaintManagerUI* m_pPaintManager;

	CStdPtrArray m_autoAdjustBtn;  //自动调整位置的按钮，如确定，取消，应用按钮，这类按钮排列在一起需要间隙

	SIZE m_initSize;
	HWND m_Wnd;
	int m_bottomMaxHeight;
};
#endif
