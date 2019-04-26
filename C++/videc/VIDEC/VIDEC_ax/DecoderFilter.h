#ifndef __DECODERFILTER_H__
#define __DECODERFILTER_H__

#include "../VIDEC/VIDEC_StreamDecoder.h"

#define VIDEC_NAME_L		L"VIDEC Video Decoder"
#define FOURCC_VIDEC		mmioFOURCC('V','D','E','C')

DEFINE_GUID(CLSID_VIDEC,mmioFOURCC('V','D','E','C'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

#ifdef __cplusplus
extern "C" {
#endif


DEFINE_GUID(IID_IDecoderFilter,
	0x00000000, 0x4fef, 0x4dd3, 0xb3, 0xfa, 0xee, 0x53, 0x1b, 0x89, 0x7f, 0x9c);

DECLARE_INTERFACE_(IDecoderFilter, IUnknown)
{
};



#ifdef __cplusplus
}
#endif

class DecoderFilter 
	: public CVideoTransformFilter
	, public IDecoderFilter
{
public :
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	DECLARE_IUNKNOWN;

	DecoderFilter(LPUNKNOWN punk, HRESULT *phr);
	~DecoderFilter();

	HRESULT CheckInputType(const CMediaType * mtIn);
	HRESULT GetMediaType(int iPos, CMediaType * pmt);
	HRESULT SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt);
	
	HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
	HRESULT DecideBufferSize(IMemAllocator * pima, ALLOCATOR_PROPERTIES * pProperties);

	HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);
private :
	HRESULT ChangeColorspace(GUID subtype, GUID formattype, void * format);
protected:
	VIDEC_StreamDecoder*	m_pDecoder;
	int						m_nWidth;
	int						m_nHeight;
	int						m_nStride;
	int						m_nImageSize;
};



#endif

