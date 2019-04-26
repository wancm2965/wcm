// SDRDrawPin.h: interface for the SDRDrawPin class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SDRDRAWPIN_H__
#define __SDRDRAWPIN_H__


class SDRDrawFilter;
class SDRDrawPin : public CBaseOutputPin  
{
	DECLARE_IUNKNOWN;
public:
	SDRDrawPin(char *pObjName,SDRDrawFilter *pFilter,HRESULT *phr,LPCWSTR pPinName);
	virtual~SDRDrawPin();

public:
    // Check that we can support an output type
    HRESULT CheckMediaType(const CMediaType *pMediaType);
    // set the connection media type
    HRESULT SetMediaType(const CMediaType *pMediaType);
	HRESULT GetMediaType(int iPosition, CMediaType *pMediaType); 

    // Negotiation to use our input pins allocator
    HRESULT DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties);

	void SetImageFormat(int nWidth,int nHeight);

	bool DisplayFrame(unsigned char*szData,int nLen);
protected:
    SDRDrawFilter*		m_pSDRDrawFilter;                  // Main filter object pointer
    friend class SDRDrawFilter;

private :
	HRESULT ChangeColorspace(GUID subtype, GUID formattype, void * format);
protected:
	int					m_nWidth;
	int					m_nHeight;
	int					m_nStride;
	int					m_nBitCount;

	int					m_nFlip;
};

#endif 