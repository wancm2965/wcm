#ifndef __NULLRENDER_H__
#define __NULLRENDER_H__

class NullRendererCallback
{
public:
	virtual void OnNullRendererYV12Buffer(unsigned char**ppBuffer,int nBufferLen)=0;
	virtual void OnNullRendererCallback(unsigned char*pData,int nLen,int nWidth,int nHeight,int nFrameRate,double dblSampleTime)=0;
};
class NullRenderer 
	: public CBaseRenderer
{
public:
	DECLARE_IUNKNOWN;
	NullRenderer(LPUNKNOWN punk, HRESULT *phr, NullRendererCallback&rCallback);
	virtual~NullRenderer(void);
	    
public:
	HRESULT CheckMediaType( const CMediaType *pmtIn );
	HRESULT DoRenderSample( IMediaSample *pMediaSample );
	HRESULT SetMediaType( const CMediaType *pmt );
	HRESULT Active();
	HRESULT Inactive();
protected:
	NullRendererCallback&	m_rCallback;
	int						m_nWidth;
	int						m_nHeight;
	int						m_nFrameRate;
	int						m_nYLen;
	int						m_nUVLen;
};

#endif