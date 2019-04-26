// DSOutputPin.h: interface for the DSOutputPin class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DSOUTPUTPIN_H__
#define __DSOUTPUTPIN_H__

class DSOutputFilter;
class DSOutputPin : public CBaseOutputPin  
{
	DECLARE_IUNKNOWN;
public:
	DSOutputPin(char *pObjName,DSOutputFilter *pFilter,HRESULT *phr,LPCWSTR pPinName);
	virtual~DSOutputPin();

public:
    // Check that we can support an output type
    HRESULT CheckMediaType(const CMediaType *pMediaType);
    // set the connection media type
    HRESULT SetMediaType(const CMediaType *pMediaType);
	HRESULT GetMediaType(int iPosition, CMediaType *pMediaType); 

    // Negotiation to use our input pins allocator
    HRESULT DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties);
	STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

	int SetTitle(const char*cszTitle,unsigned long nColor);

	HRESULT GetImageFormat(int&nWidth,int&nHeight);
	HRESULT SetImageFormat(int nWidth, int nHeight);

	bool DisplayFrame(unsigned char*pYUV420Data[3],int nStride[3]);
private :
	HRESULT ChangeColorspace(GUID subtype, GUID formattype, void * format);
protected:
    DSOutputFilter*		m_pSDRDrawFilter;                  // Main filter object pointer
    friend class DSOutputFilter;
protected:
	int					m_nWidth;
	int					m_nHeight;
	int					m_nStride;
	int					m_nBitCount;

	int					m_nFlip;
	REFERENCE_TIME		m_nFrameLength;
	int					m_nFrameRate;
	int					m_nFrameNumber;

	BITMAPINFO			m_bmpInfo;
	unsigned char*		m_pPaintBuffer;
	HDC					m_hPaintDC;
	HBITMAP				m_hDibSection;
	HGDIOBJ				m_hObject;
	HFONT				m_hFont;
	HFONT				m_hOldFont;

	int					m_nOverlayHeight;
	int					m_nOverlayImageSize;
	int					m_nImageSize;
	int					m_nOverlayImageOffset;

	char				m_szTitle[1024];
	int					m_nCorlor;
};

#endif 