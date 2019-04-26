// XOSDFilter.h: interface for the XOSDFilter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XOSDFILTER_H__
#define __XOSDFILTER_H__


class XOSDFilter  
{
public:
	XOSDFilter(void);
	virtual~XOSDFilter(void);
public:
	void SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk);
	bool Process(PBYTE pData,int nWidth,int nHeight,int nLeftOffet,int nTopOffset,float bXRatio,float bYRatio,LPCTSTR lpOSD,int nAlign,int nDoubleField=0);
protected:
	void AddTopOSD(PBYTE pOsdY,PBYTE pOsdU,PBYTE pOsdV,PBYTE pImgY,PBYTE pImgU,PBYTE pImgV,int nTopOffset);
	void AddBottomOSD(PBYTE pOsdY,PBYTE pOsdU,PBYTE pOsdV,PBYTE pImgY,PBYTE pImgU,PBYTE pImgV,int nTopOffset);
protected:
	int					m_nWidth;
	int					m_nHeight;
	int					m_nImageSizeY;
	float				m_fXRatio;
	float				m_fYRatio;
	CString				m_strOSD;

	int					m_nAlign;
	COLORREF			m_crText;
	COLORREF			m_crBk;
	LOGFONTA			m_fontOSD;
	bool				m_bFontChanged;

	PBYTE				m_pOSDYUV;
	int					m_nOSDWidth;
	int					m_nOSDHeight;
	int					m_nOSDClipWidth;
	int					m_nOSDWidthOffset;
	int					m_nOSDImageSizeY;
};

#endif 