//VideoCallback.h
#ifndef __VIDEO_CALLBACK_H__
#define __VIDEO_CALLBACK_H__


DEFINE_GUID(IID_IVideoDecoder, 
0x1CC2385F, 0x36FA, 0x91B1, 0x99, 0x92, 0x50, 0x29, 0xCE, 0x02, 0x35, 0xDC);

[uuid("1CC2385F-36FA-91B1-9992-5029CE0235DC")]
interface IVideoDecoder : public IUnknown
{
	STDMETHOD_(int,GetWidth)()=0;
	STDMETHOD_(int,GetHeight)()=0;
};

class VideoDecoder
	: public IVideoDecoder
{
public:
	VideoDecoder(void)
		:m_nWidth(0)
		,m_nHeight(0)
	{
	}
	virtual~VideoDecoder(void)
	{
	}
public:
	STDMETHODIMP_(int) GetWidth(void){return m_nWidth;}
	STDMETHODIMP_(int) GetHeight(void){return m_nHeight;}
protected:
	int						m_nWidth;
	int						m_nHeight;
};

#endif
