// VideoDrawPin.h: interface for the VideoDrawPin class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VIDEODRAWPIN_H__
#define __VIDEODRAWPIN_H__


class VideoDrawFilter;
class VideoDrawPin : public CBaseOutputPin  
{
	DECLARE_IUNKNOWN;
public:
	VideoDrawPin(int nWidth,int nHeight,LPCSTR pObjName,VideoDrawFilter *pFilter,HRESULT *phr,LPCWSTR pPinName,bool bNV12);
	virtual~VideoDrawPin();

public:
    // Check that we can support an output type
    HRESULT CheckMediaType(const CMediaType *pMediaType);
    // set the connection media type
    HRESULT SetMediaType(const CMediaType *pMediaType);
	HRESULT GetMediaType(int iPosition, CMediaType *pMediaType); 

    // Negotiation to use our input pins allocator
    HRESULT DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties);


	int Draw(unsigned char*pYUV,int nStride);
protected:
    VideoDrawFilter*		m_pVideoDrawFilter;                  // Main filter object pointer
    friend class VideoDrawFilter;

private :
	HRESULT ChangeColorspace(GUID subtype, GUID formattype, void * format);
protected:
	int					m_nWidth;
	int					m_nHeight;
	int					m_nStride;
	bool				m_bNV12;
	bool				m_bSwapUV;
};

#endif 