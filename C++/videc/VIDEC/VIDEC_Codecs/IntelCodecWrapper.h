#ifndef __INTELCODECWRAPPER_H__
#define __INTELCODECWRAPPER_H__

#include "XDllLoadLibrary.h"
#include "../VIDECINL/VIDECINLAPI.h"

void intel_codec_init(void);
void intel_codec_uninit(void);

void IntelEncoderSetMaxCount(unsigned int nMaxCount);
int IntelEncoderTry(int nWidth,int nHeight);
void*IntelEncoderOpen(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nQMin,int nQMax,int nKeyIntMax,VIDECINL_ENCODER_CB encoder_cb,void*pUserData);
void IntelEncoderClose(void*pHandle);
int IntelEncoderEncoding(void*pHandle,unsigned char*pData,int nLen,int nRequestKeyFrame);

int IntelDecoderTry(void);
void* IntelDecoderOpen(int nWidth,int nHeight,VIDECINL_DECODER_CB decoder_cb,void*pUserData);
void IntelDecoderClose(void*pHandle);
int IntelDecoderDecode(void*pHandle,const unsigned char*pData,int nLen);

class IntelCodecWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	IntelCodecWrapper(HINSTANCE hInstance);
	~IntelCodecWrapper(void);
public:
	static IntelCodecWrapper&Instance(void);
	bool IsOK(void);

	void (*pVIDECINL_EncoderSetMaxCount)(unsigned int nMaxCount);
	int (*pVIDECINL_EncoderTry)(int nWidth,int nHeight);
	void* (*pVIDECINL_EncoderOpen)(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nQMin,int nQMax,int nKeyIntMax,VIDECINL_ENCODER_CB encoder_cb,void*pUserData,int nKey);
	void (*pVIDECINL_EncoderClose)(void*pHandle);
	int (*pVIDECINL_EncoderEncoding)(void*pHandle,unsigned char*pData,int nLen,int nRequestKeyFrame);

	int (*pVIDECINL_DecoderTry)(VIDECINL_ALIGNED_MALLOC aligned_malloc,VIDECINL_ALIGNED_FREE aligned_free);
	void* (*pVIDECINL_DecoderOpen)(int nWidth,int nHeight,VIDECINL_DECODER_CB decoder_cb,void*pUserData,int nKey);
	void (*pVIDECINL_DecoderClose)(void*pHandle);
	int (*pVIDECINL_DecoderDecode)(void*pHandle,const unsigned char*pData,int nLen);
};

#endif
