//ImageColorEnhance.h
#ifndef __IMAGECOLORENHANCE_H__
#define __IMAGECOLORENHANCE_H__

class ImageColorEnhance
{
public:
	ImageColorEnhance(void);
	virtual~ImageColorEnhance(void);
public:
	virtual int Open(void);
	virtual void Close(void);
	virtual void Process(unsigned char*pY,unsigned char*pU,unsigned char*pV,int nWidth,int nHeight);
protected:
	void*	m_pHandleICE;
	int		m_nWidth;
	int		m_nHeight;
};
#endif
