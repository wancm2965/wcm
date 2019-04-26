//CUDAVideoEncoder.h
#ifndef __CUDA_VIDEO_ENCODER_H__
#define __CUDA_VIDEO_ENCODER_H__


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "unknwn.h"
#include "NVEncoderAPI.h"
#include "NVEncodeDataTypes.h"


class CUDAVideoEncoderCallback
{
public:
	virtual void OnCUDAVideoEncoderCallback(unsigned char*pData,int nLen,int nKeyFrame){};
};

// Wrapper class around the CUDA video encoder API.
//
class CUDAVideoEncoder
{
public:
	CUDAVideoEncoder(CUDAVideoEncoderCallback&rCallback);
	virtual~CUDAVideoEncoder(void);
public:
	int Open(int nWidth,int nHeight, int nFrameRate,int nBitrate, int nCBR, int nQMin,unsigned char nHigh);
	void Close(void);
	bool EncodeFrame(unsigned char*pFrameData,int nRequestKeyFrame);

	unsigned char* AcquireBitStream(int *pBufferSize);
	void ReleaseBitStream(int nBytesInBuffer, unsigned char *cb);
	void NextKeyFrame(int nNextKeyFrame){m_nNextKeyFrame=nNextKeyFrame;};
protected:
	HRESULT GetParamValue        ( DWORD dwParamType, void *pData );
	HRESULT SetParamValue        ( DWORD dwParamType, void *pData );
private:
	CUDAVideoEncoderCallback&	m_rCallback;

	void*					m_pEncoder;
	unsigned char*			m_pPacketBuffer;
	unsigned char*			m_pPacketData;
	int						m_nPacketBufferSize;


	int						m_nGPUCount;
	int						m_nGPUID;
	int						m_nWidth;
	int						m_nHeight;

	int						m_nNextKeyFrame;
};

#endif // NV_VIDEO_ENCODER