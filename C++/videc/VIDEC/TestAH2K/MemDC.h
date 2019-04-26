#ifndef _MEMDC_H
#define _MEMDC_H

class CMemDC : public CDC {
private:
	CBitmap*	m_bitmap;
	CBitmap*	m_oldBitmap;
	CDC*		m_pDC;
	CRect		m_rcBounds;
public:
	CMemDC(CDC* pDC, const CRect& rcBounds) : CDC()
	{
		CreateCompatibleDC(pDC);
		m_bitmap = new CBitmap;
		m_bitmap->CreateCompatibleBitmap(pDC, rcBounds.Width(), rcBounds.Height());
		m_oldBitmap = SelectObject(m_bitmap);
		m_pDC = pDC;
		m_rcBounds = rcBounds;
        //For some reason the background color is not correct,
        //so we use the button face color.
        DWORD	color = ::GetSysColor( COLOR_BTNFACE );
        CBrush bkg(color);
        FillRect(rcBounds, &bkg);
		m_bDraw = true;

	}
	~CMemDC() 
	{
		if (m_bDraw)
		{
			m_pDC->BitBlt(m_rcBounds.left, m_rcBounds.top, m_rcBounds.Width(), m_rcBounds.Height(), 
					this, m_rcBounds.left, m_rcBounds.top, SRCCOPY);
		}
		SelectObject(m_oldBitmap);
		if (m_bitmap != NULL) delete m_bitmap;

		DeleteDC();
	}
	CMemDC* operator->() {
		return this;
	}
	CBitmap* GetMemBitmap()
	{
		return m_bitmap;
	}
	bool	m_bDraw;
};

class AFX_EXT_CLASS CMemDC2 : public CDC
{
public:

    CMemDC2(CDC* pDC) : CDC()
    {
        ASSERT(pDC != NULL);

        m_pDC = pDC;
        m_pOldBitmap = NULL;
        m_bMemDC = !pDC->IsPrinting();
              
        if (m_bMemDC) 
        {
            pDC->GetClipBox(&m_rect);
            CreateCompatibleDC(pDC);
			if(this->m_hDC == NULL)
				return;

            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			if(m_bitmap.m_hObject == NULL)
				return;

            m_pOldBitmap = SelectObject(&m_bitmap);
            SetWindowOrg(m_rect.left, m_rect.top);
        }
        else 
        {
            m_bPrinting = pDC->m_bPrinting;
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
        }
    }
    
    ~CMemDC2()
    {
		if(this->m_hDC == NULL || m_bitmap.m_hObject == NULL)
			return;

        if (m_bMemDC) 
        {    
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                          this, m_rect.left, m_rect.top, SRCCOPY);

            SelectObject(m_pOldBitmap);
        } else {
            m_hDC = m_hAttribDC = NULL;
        }

		m_bitmap.DeleteObject();
		DeleteDC();
    }

    CMemDC2* operator->() {return this;}
        
    operator CMemDC2*() {return this;}

private:    
    CBitmap* m_pOldBitmap;  // bitmap originally found in CMemDC2
    CDC*     m_pDC;         // Saves CDC passed in constructor
    CRect    m_rect;        // Rectangle of drawing area.
    BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.
	CBitmap  m_bitmap;      // Offscreen bitmap
};

#endif