#pragma once

#if !defined(_StyleButton5_H)
#define _StyleButton5_H


// CStyleButton5

#include <gdiplus.h>
using namespace Gdiplus;

class CStyleButton5 : public CButton
{
	DECLARE_DYNAMIC(CStyleButton5)

public:
	CStyleButton5();
	virtual ~CStyleButton5();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	virtual void PreSubclassWindow();

public:
	BOOL LoadButtonImage(CString sFile, int iWidth, int iHeight);
	CRgn* GetRgn();
	void SetSelect();
	int GetSelect();

protected:
	virtual void Clear();
	virtual void DrawFrame(int iFrame, HDC hDC);
	virtual void UpdateRgn(int iFrame = 0);

protected:
	Bitmap* m_pImageBg;

	int m_iWidth;
	int m_iHeight;
	int m_iFrameCount;

	CRgn m_rgn;

	ImageAttributes m_imAtt;

	int m_iSelect;

public:
	static int m_iWidthOri;
	static int m_iHeightOri;

public:
	afx_msg void OnMove(int x, int y);
};

#endif
