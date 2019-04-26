//HWVideoDecoder.h
#ifndef __HWVIDEO_DECODER_H__
#define __HWVIDEO_DECODER_H__

class HWVideoDecoderCallback
{
public:
	virtual void OnHWVideoDecoderCallback(unsigned char*pData,int nLen,int nWidth,int nHeight,int nStride)=0;
};

class HWVideoDecoder
{
public:
	HWVideoDecoder(void);
	virtual~HWVideoDecoder(void);
public:
	virtual int Open(int nWidth,int nHeight)=0;
	virtual void Close(void) =0;
	virtual int Decode(const unsigned char *buffer, int buflen)=0;
	virtual int GetWidth(void)=0;
	virtual int GetHeight(void)=0;

	static HWVideoDecoder*Create(HWVideoDecoderCallback&rCallback);

	static void Load(void);
	static void Unload(void);
	static int TryDecoder(unsigned long nWidht,unsigned long nHeight);
	static void SetMaxCount(unsigned int nMaxCount);
	static int GetSupport(void);
};

#endif
